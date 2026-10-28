REGS = ("zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
        "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra")
IMMEDIATE = {0x08: "addi", 0x09: "addiu", 0x0A: "slti", 0x0B: "sltiu", 0x0C: "andi", 0x0D: "ori", 0x0E: "xori",
             0x0F: "lui", 0x20: "lb", 0x21: "lh", 0x22: "lwl", 0x23: "lw", 0x24: "lbu", 0x25: "lhu", 0x26: "lwr",
             0x28: "sb", 0x29: "sh", 0x2A: "swl", 0x2B: "sw", 0x2E: "swr", 0x32: "lwc2", 0x3A: "swc2"}
SPECIAL = {0x00: "sll", 0x02: "srl", 0x03: "sra", 0x04: "sllv", 0x06: "srlv", 0x07: "srav", 0x08: "jr",
           0x09: "jalr", 0x0C: "syscall", 0x0D: "break", 0x10: "mfhi", 0x11: "mthi", 0x12: "mflo", 0x13: "mtlo",
           0x18: "mult", 0x19: "multu", 0x1A: "div", 0x1B: "divu", 0x20: "add", 0x21: "addu", 0x22: "sub",
           0x23: "subu", 0x24: "and", 0x25: "or", 0x26: "xor", 0x27: "nor", 0x2A: "slt", 0x2B: "sltu"}
REGIMM = {0: "bltz", 1: "bgez", 16: "bltzal", 17: "bgezal"}
BRANCHES = {4: "beq", 5: "bne", 6: "blez", 7: "bgtz"}
SHIFTS = ("sll", "srl", "sra")
VAR_SHIFTS = ("sllv", "srlv", "srav")
FROM_HILO = ("mfhi", "mflo")
TO_HILO = ("mthi", "mtlo")
MULDIV = ("mult", "multu", "div", "divu")
LOGICAL = ("andi", "ori", "xori")
NO_WRITE = {0x08, 0x0C, 0x0D, 0x11, 0x13, 0x18, 0x19, 0x1A, 0x1B}
OP_JAL = 3
RA = 31


def s16(x):
    return (x & 0xFFFF) - ((x & 0x8000) << 1)


def branch_target(word, pc):
    op = word >> 26
    if op in BRANCHES or (op == 1 and (word >> 16) & 31 in REGIMM):
        return (pc + 4 + s16(word) * 4) & 0xFFFFFFFF
    return None


def jump_target(word, pc):
    if word >> 26 in (2, 3):
        return ((pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
    return None


def written(word):
    op = word >> 26
    rs, rt = (word >> 21) & 31, (word >> 16) & 31
    if op == 0:
        return None if word & 63 in NO_WRITE else (word >> 11) & 31
    if op == OP_JAL:
        return RA
    if 8 <= op <= 15 or 0x20 <= op <= 0x26:
        return rt
    if 0x10 <= op <= 0x13 and rs in (0, 2):
        return rt
    return None


def _raw(word):
    return f".word {word:#010x}"


def disassemble(word, pc):
    if word == 0:
        return "nop"
    op, rs, rt, rd = word >> 26, (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31
    if op == 0:
        name = SPECIAL.get(word & 63)
        if name is None:
            return _raw(word)
        if name in SHIFTS:
            return f"{name} {REGS[rd]},{REGS[rt]},{(word >> 6) & 31}"
        if name == "jr":
            return f"jr {REGS[rs]}"
        if name == "jalr":
            return f"jalr {REGS[rd]},{REGS[rs]}"
        if name in FROM_HILO:
            return f"{name} {REGS[rd]}"
        if name in TO_HILO:
            return f"{name} {REGS[rs]}"
        if name in MULDIV:
            return f"{name} {REGS[rs]},{REGS[rt]}"
        if name in VAR_SHIFTS:
            return f"{name} {REGS[rd]},{REGS[rt]},{REGS[rs]}"
        if name in ("syscall", "break"):
            return name
        return f"{name} {REGS[rd]},{REGS[rs]},{REGS[rt]}"
    target = branch_target(word, pc)
    if op == 1:
        return f"{REGIMM[rt]} {REGS[rs]},{target:#x}" if target is not None else _raw(word)
    if op in (2, 3):
        return f"{'j' if op == 2 else 'jal'} {jump_target(word, pc):#x}"
    if op in (4, 5):
        return f"{BRANCHES[op]} {REGS[rs]},{REGS[rt]},{target:#x}"
    if op in (6, 7):
        return f"{BRANCHES[op]} {REGS[rs]},{target:#x}"
    if op in (0x10, 0x12):
        return f"cop{op & 3} {word:#x}"
    name = IMMEDIATE.get(op)
    if name is None:
        return _raw(word)
    imm = word & 0xFFFF
    if name == "lui":
        return f"lui {REGS[rt]},{imm:#x}"
    if name in LOGICAL:
        return f"{name} {REGS[rt]},{REGS[rs]},{imm:#x}"
    if op >= 0x20:
        return f"{name} {REGS[rt]},{s16(word)}({REGS[rs]})"
    return f"{name} {REGS[rt]},{REGS[rs]},{s16(word)}"
