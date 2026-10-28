import struct

from mmxdemake.mips import branch_target, jump_target, s16, written
from mmxdemake.savestate import ram_at

JR_RA = 0x03E00008
STACK_ADJUST = 0x27BD
OP_JAL = 3
OP_ADDIU = 9
OP_LUI = 15
FUNCT_ADDU = 0x21
FUNCT_OR = 0x25
FUNCT_JALR = 0x09
LOADS = set(range(0x20, 0x27))
STORES = set(range(0x28, 0x2F))
A0 = 4
SELF_SCAN = 16
PAIR_GAP = 12
MIN_TABLE = 2
TABLE_MAX = 64
END_LIMIT = 4000
WALK_LIMIT = 400
MARGIN = 0x400
RAM_LO, RAM_HI = 0x80000000, 0x80200000
SCRATCH_LO, SCRATCH_HI = 0x1F800000, 0x1F800400
CLASS_TABLES = (("visual", 0x84, 58), ("misc", 0x90, 64), ("stage", 0x94, 32))
OWNER_TABLES = ((0x80, 77), (0x94, 32))
SCRATCH_HI16 = 0x1F80
CURRENT_OBJECT = (0x4C, 0x60, 0x64)
OP_LW = 0x23
STUB_BYTES = 8


class WalkError(Exception):
    pass


class Graph:
    def __init__(self, roots):
        self.roots = list(roots)
        self.functions = {}
        self.calls = {}
        self.callers = {}
        self.tables = {}
        self.leaves = set()
        self.truncated = False


def _u32(ram, addr):
    return struct.unpack("<I", ram_at(ram, addr, 4))[0]


def _inside(addr, regions):
    return any(lo <= addr < hi for lo, hi in regions)


def is_function_start(ram, addr):
    x = _u32(ram, addr)
    return bool(x >> 16 == STACK_ADJUST and x & 0x8000) or _u32(ram, addr - 8) == JR_RA


def function_end(ram, start, limit=END_LIMIT):
    far = start
    for k in range(limit):
        pc = start + 4 * k
        word = _u32(ram, pc)
        target = branch_target(word, pc)
        if target is not None:
            far = max(far, target)
        if word == JR_RA and pc >= far:
            return pc + 8
    raise WalkError(f"no end found for the function at {start:#x} within {limit} words")


def _keeps_base(word, reg):
    return (word >> 26 == 0 and word & 63 == FUNCT_ADDU and (word >> 11) & 31 == reg
            and reg in ((word >> 21) & 31, (word >> 16) & 31))


def constants(ram, start, end):
    out = []
    for pc in range(start, end, 4):
        x = _u32(ram, pc)
        if x >> 26 != OP_LUI:
            continue
        reg = (x >> 16) & 31
        for k in range(1, PAIR_GAP + 1):
            y = _u32(ram, pc + 4 * k)
            op = y >> 26
            if (y >> 21) & 31 == reg and (op == OP_ADDIU or op in LOADS or op in STORES):
                out.append((pc + 4 * k, (((x & 0xFFFF) << 16) + s16(y)) & 0xFFFFFFFF))
                break
            if written(y) == reg and not _keeps_base(y, reg):
                break
    return out


def code_tables(ram, start, end, regions):
    out = []
    for _, value in constants(ram, start, end):
        if not _inside(value, regions):
            continue
        targets = []
        while len(targets) < TABLE_MAX:
            t = _u32(ram, value + 4 * len(targets))
            if not (_inside(t, regions) and t % 4 == 0 and is_function_start(ram, t)):
                break
            targets.append(t)
        if len(targets) >= MIN_TABLE:
            out.append((value, targets))
    return out


def walk(ram, roots, expand, regions, helpers, limit=WALK_LIMIT, accept=None):
    graph = Graph(roots)
    todo = list(roots)
    direct = {}
    while todo:
        f = todo.pop(0)
        if f in graph.functions:
            continue
        if len(graph.functions) == limit:
            graph.truncated = True
            break
        end = function_end(ram, f)
        graph.functions[f] = end
        callees = []
        for pc in range(f, end, 4):
            t = jump_target(_u32(ram, pc), pc)
            if t is not None and not f <= t < end and t not in callees:
                callees.append(t)
        direct[f] = list(callees)
        for addr, targets in code_tables(ram, f, end, regions):
            graph.tables[addr] = (f, targets)
            callees.extend(t for t in targets if t not in callees)
        for t in callees:
            if t not in helpers and _inside(t, expand) and (accept is None or accept(f, t)):
                todo.append(t)
    starts = sorted(graph.tables)
    for addr, following in zip(starts, starts[1:]):
        user, targets = graph.tables[addr]
        graph.tables[addr] = (user, targets[:(following - addr) // 4])
    for f, callees in direct.items():
        for _, (user, targets) in sorted(graph.tables.items()):
            if user == f:
                callees.extend(t for t in targets if t not in callees)
        graph.calls[f] = callees
        for t in callees:
            graph.callers.setdefault(t, set()).add(f)
            if t not in graph.functions:
                graph.leaves.add(t)
    return graph


def is_stub(ram, func):
    return function_end(ram, func) - func <= STUB_BYTES


def neighbours(ram, scratch, graph, expand, margin=MARGIN):
    lo, hi = expand
    first = min(graph.functions)
    stop = max(graph.functions.values()) + margin
    for offset, count in OWNER_TABLES:
        table = struct.unpack_from("<I", scratch, offset)[0]
        for t in range(count):
            f = _u32(ram, table + 4 * t)
            if first < f < stop and f not in graph.functions:
                stop = f
    out = []
    for name, offset, count in CLASS_TABLES:
        table = struct.unpack_from("<I", scratch, offset)[0]
        for t in range(count):
            f = _u32(ram, table + 4 * t)
            inside = any(start < f < end for start, end in graph.functions.items())
            if (lo <= f < hi and first <= f < stop and f not in graph.roots and not inside
                    and is_function_start(ram, f) and not is_stub(ram, f)):
                out.append((name, t, f))
    return out


def references(ram, graph, regions):
    known = set(graph.functions) | set(graph.tables) | graph.leaves
    out = {}
    for f, end in graph.functions.items():
        for _, value in constants(ram, f, end):
            if value in known:
                continue
            if RAM_LO <= value < RAM_HI or SCRATCH_LO <= value < SCRATCH_HI:
                out.setdefault(value, set()).add(f)
    return {v: sorted(users) for v, users in sorted(out.items())}


def self_fields(ram, start, end):
    regs, fields, notes, scratch = {A0}, {}, {}, set()
    a0_live = True
    for k, pc in enumerate(range(start, end, 4)):
        x = _u32(ram, pc)
        op, rs, rt = x >> 26, (x >> 21) & 31, (x >> 16) & 31
        if k < SELF_SCAN and op == 0 and x & 63 in (FUNCT_ADDU, FUNCT_OR) and rs == A0 and rt == 0:
            regs.add((x >> 11) & 31)
            continue
        if op == OP_LUI and x & 0xFFFF == SCRATCH_HI16:
            scratch.add(rt)
            regs.discard(rt)
            continue
        if op == OP_LW and rs in scratch and s16(x) in CURRENT_OBJECT:
            regs.add(rt)
            scratch.discard(rt)
            if rt == A0:
                a0_live = True
            continue
        if written(x) in scratch:
            scratch.discard(written(x))
        if (op in LOADS or op in STORES) and rs in regs and (rs != A0 or a0_live):
            entry = fields.setdefault(s16(x), {"read": 0, "write": 0})
            entry["write" if op in STORES else "read"] += 1
            notes[pc] = s16(x)
        if op == OP_JAL or (op == 0 and x & 63 == FUNCT_JALR):
            a0_live = False
        reg = written(x)
        if reg == A0:
            a0_live = False
        elif reg in regs and k >= SELF_SCAN:
            regs.discard(reg)
    return fields, notes
