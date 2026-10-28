import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.animfind import (MAIN_OBJECTS, STAGE_OBJECTS, FindError, choose, code_regions, live_objects,
                                live_sources, live_tables, player_overlay, program_bounds, searched_tables)
from mmxdemake.anims import MAX_ANIMS, AnimError, read_table, walk as walk_animation
from mmxdemake.codewalk import (STACK_ADJUST, WalkError, constants, function_end, is_function_start, is_stub,
                                neighbours, references, self_fields, walk)
from mmxdemake.learn import DEFAULT_CATALOG, DEFAULT_DUMPS, DEFAULT_EXTRACTED
from mmxdemake.mips import disassemble, jump_target
from mmxdemake.savestate import SaveStateError, ram_at, ram_from, read_state
from mmxdemake.spriteexport import DEFAULT_ROOT, EXE
from mmxdemake.sprites import SpriteError, bank_of_type, banks, scratchpad, stage_area

REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_RE = Path.home() / "mmx-data/x6/re"
FILES = ("summary.md", "code.txt", "calls.txt", "data.txt")
CLASSES = {MAIN_OBJECTS.name: (MAIN_OBJECTS, 0x80), STAGE_OBJECTS.name: (STAGE_OBJECTS, 0x94)}
KINDS = ("functions", "globals", "fields")
DUMP_MAX = 256
MAX_NEIGHBOURS = 8
GLOBAL_SPAN = 0x100
LINE = 16
MEGA = 0x800970A0
PLAYER_UPDATE = 0x80034FBC
PLAYER_STEPS = 0x80073ACC
PLAYER_MAIN_END = 0x80040000
STEP_COUNT = 78
FOREIGN_STEP = "not a function start in this overlay (the other character's entry)"
OUTSIDE_STEP = "outside the player code of the main program and the player overlay"
PLAYER_LIMIT = 2000
PLAYER_ANIMS = 256
CHARACTERS = ("x", "zero")


class KitFailed(Exception):
    pass


def _u32(ram, addr):
    return struct.unpack("<I", ram_at(ram, addr, 4))[0]


def load_helpers(catalog):
    doc = json.loads((Path(catalog) / "helpers.json").read_text())
    out = {}
    for kind in KINDS:
        part = doc.get(kind, {})
        if not isinstance(part, dict):
            raise ValueError(f"helpers.json: {kind} must be an object")
        table = {}
        for key, value in part.items():
            if not isinstance(value, dict) or not isinstance(value.get("name"), str):
                raise ValueError(f"helpers.json: bad {kind} entry {key!r}")
            table[int(key, 16)] = (value["name"], value.get("note", ""))
        out[kind] = table
    return out


def resolve(name, sprites):
    doc = json.loads((Path(sprites) / "enemies" / name / "animations.json").read_text())
    first = doc["sources"][0]
    cls = first.get("class", MAIN_OBJECTS.name)
    tables = sorted({int(s["table"], 16) for s in doc["sources"]
                     if s["state"] == first["state"] and s["type"] == first["type"]
                     and s.get("class", MAIN_OBJECTS.name) == cls})
    return first["state"], first["type"], cls, tables, len(doc.get("animations", [])) or None


def sprite_count(ram, scratch, regions, cls, t):
    if cls == MAIN_OBJECTS.name:
        bank = bank_of_type(banks(ram, scratch), t)
        if bank is not None and bank.texture is not None:
            return bank.count
        found = sorted(live_sources(ram, regions).get(t, ()))
    else:
        found = [source[2:] for source in live_objects(ram, regions, STAGE_OBJECTS) if source[0] == t]
    return struct.unpack("<H", ram_at(ram, found[0][0] + 2, 2))[0] if found else None


def found_tables(ram, scratch, regions, cls, t):
    if cls == STAGE_OBJECTS.name:
        return sorted(live_tables(ram, regions, STAGE_OBJECTS).get(t, ()))
    searched = {}
    for lo, hi in regions:
        searched.update(searched_tables(ram, scratch, lo, hi))
    choices, _ = choose(None, live_tables(ram, regions).get(t), searched.get(t))
    return [table for table, _ in choices]


def animation_list(ram, table, regions, limit=None, sprites=None, count=MAX_ANIMS):
    region = next(((lo, hi) for lo, hi in regions if lo <= table < hi), None)
    if region is None:
        return [], f"table {table:#x} is outside the overlay and the main program"
    anims = []
    try:
        pointers = read_table(ram, table, *region, count)
    except AnimError as e:
        return [], str(e)
    for k, p in enumerate(pointers):
        if limit is not None and k == limit:
            return anims, f"cut to the export's {limit} animations; {len(pointers) - k} more pointer-like words follow (often the next table)"
        try:
            frames, loop = walk_animation(ram, p)
        except AnimError as e:
            return anims, f"animation {k}: {e}"
        top = max(f["sprite"] for f in frames)
        if sprites is not None and top >= sprites:
            return anims, f"cut: animation {k} uses sprite {top}; the sprite set has {sprites}"
        anims.append((frames, loop))
    return anims, None


def _name(helpers, kind, addr):
    entry = helpers[kind].get(addr)
    return entry[0] if entry else None


def _global(helpers, addr):
    name = _name(helpers, "globals", addr)
    if name:
        return name
    bases = [b for b in helpers["globals"] if b < addr <= b + GLOBAL_SPAN]
    if not bases:
        return None
    base = max(bases)
    return f"{helpers['globals'][base][0]}+{addr - base:#x}"


def _field(helpers, offset, raw=False):
    return (None if raw else _name(helpers, "fields", offset)) or f"self+{offset:#x}"


def main_functions(graph, entry):
    seen, todo = set(), [entry]
    while todo:
        f = todo.pop()
        if f in seen or f not in graph.functions:
            continue
        seen.add(f)
        todo.extend(graph.calls.get(f, ()))
    return seen


def listing(ram, start, end, helpers):
    lines = []
    for pc in range(start, end, 4):
        word = _u32(ram, pc)
        target = jump_target(word, pc)
        name = _name(helpers, "functions", target) if target is not None else None
        lines.append(f"{pc:08x}: {word:08x}  {disassemble(word, pc)}" + (f"  ; {name}" if name else ""))
    return lines


def _hexes(values):
    return ", ".join(f"{v:#x}" for v in sorted(values))


def summary_text(ctx):
    h, g = ctx["helpers"], ctx["graph"]
    lines = [f"# RE kit: {ctx['label']}", "",
             f"- Object: {ctx['label']}",
             f"- State: `{ctx['state']}` (SHA-1 `{ctx['sha1']}`), stage {ctx['stage']} area {ctx['area']}",
             f"- Class: {ctx['cls']}, type {ctx['type']}; entry function {ctx['entry']:#010x}",
             f"- Functions walked: {len(g.functions)}; state tables: {len(g.tables)}"
             + ("; the walk stopped at its limit" if g.truncated else ""),
             "", "## Engine helpers called", "", "| address | name | note | called from |", "|---|---|---|---|"]
    if "roles" in ctx:
        head = lines[:-5]
        for area, (lo, hi) in ctx["spans"].items():
            head.append(f"- Functions in the {area} ({lo:#010x}..{hi:#010x}): "
                        f"{sum(1 for f in g.functions if lo <= f < hi)}")
        head += ["", "## Roots", "", "| function | end | role |", "|---|---|---|"]
        head += [f"| {f:#010x} | {g.functions[f]:#010x} | {role} |"
                 for f, role in sorted(ctx["roles"].items()) if f in g.functions]
        head += [f"| {f:#010x} | not walked | {role} |" for f, role in sorted(ctx["unwalked_steps"].items())]
        lines = head + lines[-5:]
    named = sorted(t for t in g.leaves if t in h["functions"])
    for t in named:
        name, note = h["functions"][t]
        lines.append(f"| {t:#010x} | {name} | {note} | {_hexes(g.callers.get(t, ()))} |")
    lines += ["", "## Calls to unnamed code outside the walk", "", "| address | called from |", "|---|---|"]
    for t in sorted(g.leaves - set(named)):
        mark = " (not a function start in this overlay: the other character's entry)" if t in ctx.get("foreign", ()) else ""
        lines.append(f"| {t:#010x}{mark} | {_hexes(g.callers.get(t, ()))} |")
    lines += ["", "## State tables", "", "| table | used by | targets |", "|---|---|---|"]
    for addr, (user, targets) in sorted(g.tables.items()):
        lines.append(f"| {addr:#010x} | {user:#010x} | " + ", ".join(f"{t:#x}" for t in targets) + " |")
    lines += ["", "## Neighbour entries (likely shots and effects of this object)", "",
              "| class | type | function |", "|---|---|---|"]
    for cls, t, f in ctx["neighbours"]:
        lines.append(f"| {cls} | {t} | {f:#010x} |")
    for cls, t, f in ctx["unwalked"]:
        lines.append(f"| {cls} | {t} | {f:#010x} (not walked: more than {MAX_NEIGHBOURS} neighbours) |")
    if ctx["unwalked"]:
        lines.append(f"\nWarning: {len(ctx['neighbours']) + len(ctx['unwalked'])} neighbour entries; on shared "
                     "code many of them belong to other objects.")
    lines += ["", "## Animation tables", ""]
    for table, (anims, note) in ctx["animations"].items():
        lines += [f"### Table {table:#010x} ({len(anims)} animations)", "",
                  "| # | sprites | ticks | flags | loop to |", "|---|---|---|---|---|"]
        for k, (frames, loop) in enumerate(anims):
            lines.append(f"| {k} | " + " ".join(str(f["sprite"]) for f in frames) + " | "
                         + " ".join(str(f["ticks"]) for f in frames) + " | "
                         + " ".join(str(f["flag"]) for f in frames) + f" | {'hold' if loop is None else loop} |")
        if note:
            lines.append(f"\nNote: {note}")
        lines.append("")
    lines += ["## Object fields used", "", "| offset | name | reads | writes | functions |", "|---|---|---|---|---|"]
    totals = {}
    for f, (fields, _) in ((f, v) for f, v in ctx["fields"].items() if f in ctx["main"]):
        for offset, counts in fields.items():
            entry = totals.setdefault(offset, {"read": 0, "write": 0, "functions": set()})
            entry["read"] += counts["read"]
            entry["write"] += counts["write"]
            entry["functions"].add(f)
    for offset, entry in sorted(totals.items()):
        lines.append(f"| {offset:#x} | {_field(h, offset)} | {entry['read']} | {entry['write']} | "
                     f"{_hexes(entry['functions'])} |")
    lines += ["", "## Globals and data", "", "| address | name | used by |", "|---|---|---|"]
    for addr, users in ctx["references"].items():
        lines.append(f"| {addr:#010x} | {_global(h, addr) or ''} | {_hexes(users)} |")
    return "\n".join(lines) + "\n"


def _note(ctx, pc, word, values, notes, raw):
    h, g = ctx["helpers"], ctx["graph"]
    target = jump_target(word, pc)
    if target is not None:
        name = _name(h, "functions", target)
        if name:
            return name
        return f"f_{target:08x}" if target in g.functions else None
    if pc in notes:
        return _field(h, notes[pc], raw)
    if pc in values:
        value = values[pc]
        name = _global(h, value)
        if name:
            return name
        if value in g.tables:
            return f"state table {value:#x}"
        if value in g.functions:
            return f"f_{value:08x}"
        return f"{value:#x}"
    return None


def code_text(ctx):
    ram, g = ctx["ram"], ctx["graph"]
    tables_of = {}
    for addr, (_, targets) in g.tables.items():
        for i, t in enumerate(targets):
            tables_of.setdefault(t, []).append(f"{addr:#x}[{i}]")
    roles = {ctx["entry"]: "entry"}
    roles.update({f: f"neighbour: {cls} type {t}" for cls, t, f in ctx["neighbours"]})
    roles.update(ctx.get("roles", {}))
    lines = []
    for f in sorted(g.functions):
        end = g.functions[f]
        role = roles.get(f)
        lines.append(f"; ==== {f:#010x} .. {end:#010x}" + (f"  ({role})" if role else ""))
        if g.callers.get(f):
            lines.append("; called from: " + _hexes(g.callers[f]))
        if f in tables_of:
            lines.append("; in state tables: " + ", ".join(tables_of[f]))
        if f not in ctx["main"]:
            lines.append("; neighbour code: its object may use other fields, so offsets are not named")
        _, notes = ctx["fields"][f]
        values = dict(constants(ram, f, end))
        for pc in range(f, end, 4):
            word = _u32(ram, pc)
            note = _note(ctx, pc, word, values, notes, f not in ctx["main"])
            lines.append(f"{pc:08x}: {word:08x}  {disassemble(word, pc)}" + (f"  ; {note}" if note else ""))
        lines.append("")
    return "\n".join(lines)


def call_text(ctx):
    h, g = ctx["helpers"], ctx["graph"]
    lines, seen = [], set()

    def visit(f, depth):
        name = _name(h, "functions", f)
        label = "  " * depth + f"{f:#010x}" + (f" {name}" if name else "")
        if f in seen:
            lines.append(label + " (see above)")
            return
        if f not in g.functions:
            lines.append(label + ("" if name else " (not walked)"))
            return
        seen.add(f)
        lines.append(label)
        for callee in g.calls[f]:
            visit(callee, depth + 1)

    for root in g.roots:
        visit(root, 0)
        lines.append("")
    return "\n".join(lines)


def data_text(ctx):
    ram, regions = ctx["ram"], ctx["regions"]
    addrs = list(ctx["references"])
    lines = []
    for i, addr in enumerate(addrs):
        name = _global(ctx["helpers"], addr)
        lines.append(f"; ==== {addr:#010x}" + (f" {name}" if name else "")
                     + "  used by " + _hexes(ctx["references"][addr]))
        if any(lo <= addr < hi for lo, hi in regions):
            stop = min(addrs[i + 1] if i + 1 < len(addrs) else addr + DUMP_MAX, addr + DUMP_MAX)
            for a in range(addr, stop, LINE):
                lines.append(f"{a:08x}: " + ram_at(ram, a, min(LINE, stop - a)).hex(" "))
        lines.append("")
    return "\n".join(lines)


def _load(dumps, extracted, state, player=False):
    path = Path(dumps) / state
    data = read_state(path)
    exe = (Path(extracted) / EXE).read_bytes()
    ram = ram_from(data, exe, path)
    overlays = {int(p.stem): p.read_bytes() for p in sorted((Path(extracted) / "rock_bin").glob("*.bin"))}
    regions = code_regions(ram, overlays, exe)
    if player:
        n, lo, hi = player_overlay(ram, overlays)
        return path, ram, scratchpad(data), regions + [(lo, hi)], (n, lo, hi, program_bounds(exe))
    return path, ram, scratchpad(data), regions


def _prologue(ram, addr):
    word = _u32(ram, addr)
    return word >> 16 == STACK_ADJUST and bool(word & 0x8000)


def overlay_starts(ram, lo, hi, spans):
    pointed = set()
    for a_lo, a_hi in spans:
        for a in range(a_lo, a_hi, 4):
            w = _u32(ram, a)
            if lo <= w < hi and w % 4 == 0:
                pointed.add(w)
    out, last_end = [], lo
    for a in range(lo, hi, 4):
        if a < last_end or not (_prologue(ram, a) or (a in pointed and is_function_start(ram, a))):
            continue
        try:
            end = function_end(ram, a)
        except WalkError:
            continue
        if end <= hi:
            out.append(a)
            last_end = end
    return out


def player_graph(ram, regions, info, helpers):
    n, lo, hi, program = info
    starts = overlay_starts(ram, lo, hi, [program, (lo, hi)])
    spans = [(s, function_end(ram, s)) for s in starts]
    steps, unwalked = [], {}
    for k in range(STEP_COUNT):
        f = _u32(ram, PLAYER_STEPS + 4 * k)
        if PLAYER_UPDATE <= f < PLAYER_MAIN_END or (lo <= f < hi and is_function_start(ram, f)
                                                    and not any(s < f < e for s, e in spans)):
            steps.append((k, f))
        elif lo <= f < hi:
            unwalked.setdefault(f, []).append(f"step {k}: {FOREIGN_STEP}")
        else:
            unwalked.setdefault(f, []).append(f"step {k}: {OUTSIDE_STEP}")
    roles = {PLAYER_UPDATE: ["entry: PlayerUpdate"]}
    for k, f in steps:
        roles.setdefault(f, []).append(f"step {k}")
    for f in starts:
        roles.setdefault(f, []).append(f"overlay {n} function")
    stop = {a: v for a, v in helpers.items() if not PLAYER_UPDATE <= a < PLAYER_MAIN_END}
    graph = walk(ram, list(roles), [program, (lo, hi)], regions, stop, PLAYER_LIMIT,
                 accept=lambda f, t: not lo <= t < hi or lo <= f < hi or is_function_start(ram, t))
    foreign = {t for t in graph.leaves if lo <= t < hi}
    return (graph, {f: ", ".join(r) for f, r in roles.items()}, {f for _, f in steps}, foreign,
            {f: ", ".join(r) for f, r in unwalked.items()})


def run_player(state, dumps, extracted, helpers, out, log):
    path, ram, scratch, regions, info = _load(dumps, extracted, state, player=True)
    character = ram_at(ram, MEGA + 2, 1)[0]
    name = CHARACTERS[1 if character else 0]
    out = Path(out) if out else DEFAULT_RE / name / "kit"
    refusal = outdir.check(out, REPO_ROOT, lambda n: n in FILES)
    if refusal:
        raise ValueError(refusal)
    graph, roles, steps, foreign, unwalked = player_graph(ram, regions, info, helpers["functions"])
    table = _u32(ram, MEGA + 0x30)
    sprites = struct.unpack("<H", ram_at(ram, _u32(ram, MEGA + 0x3C) + 2, 2))[0]
    stage, area = stage_area(ram)
    ctx = {"ram": ram, "helpers": helpers, "graph": graph, "label": f"player {name}", "state": state,
           "cls": "player", "type": character, "entry": PLAYER_UPDATE, "stage": stage, "area": area,
           "regions": regions, "sha1": hashlib.sha1(path.read_bytes()).hexdigest(), "neighbours": [],
           "unwalked": [], "main": set(graph.functions), "roles": roles, "foreign": foreign,
           "unwalked_steps": unwalked,
           "spans": {"main program": info[3], f"overlay {info[0]}": (info[1], info[2])},
           "animations": {table: animation_list(ram, table, regions, None, sprites, PLAYER_ANIMS)},
           "references": references(ram, graph, regions),
           "fields": {f: self_fields(ram, f, end) for f, end in graph.functions.items()}}
    texts = {"summary.md": summary_text(ctx), "code.txt": code_text(ctx), "calls.txt": call_text(ctx),
             "data.txt": data_text(ctx)}
    tmp = outdir.fresh_tmp(out)
    for file_name, text in texts.items():
        (tmp / file_name).write_text(text)
    outdir.publish(tmp, out)
    in_overlay = sum(1 for f in graph.functions if info[1] <= f < info[2])
    log(f"player {name}: {len(graph.functions)} functions ({in_overlay} in overlay {info[0]}), "
        f"{len(graph.tables)} state tables, {len(steps)} step routines"
        + ("; the walk stopped at its limit" if graph.truncated else "") + f" -> {out}")


def run(name, state, type_, cls, dumps, extracted, catalog, sprites, out, log=print, at=None, player=False):
    if player:
        if state is None or name is not None or type_ is not None or at is not None:
            log("error: --player takes --state only")
            return 2
        try:
            run_player(state, dumps, extracted, load_helpers(catalog), out, log)
        except (SaveStateError, FindError, AnimError, WalkError, OSError, ValueError) as e:
            log(f"error: {e}")
            return 1
        return 0
    if name is None and (state is None or (type_ is None and at is None)):
        log("error: give --name, or --state and --type (or --state and --at)")
        return 2
    try:
        helpers = load_helpers(catalog)
        tables, limit = None, None
        if name is not None and state is None:
            state, type_, cls, tables, limit = resolve(name, sprites)
        if cls not in CLASSES:
            raise ValueError(f"class {cls!r} is not one of {', '.join(CLASSES)}")
    except (OSError, ValueError, KeyError, IndexError, TypeError) as e:
        log(f"error: {e}")
        return 2
    if at is not None:
        try:
            _, ram, _, _ = _load(dumps, extracted, state)
            for line in listing(ram, at, function_end(ram, at), helpers):
                log(line)
        except (SaveStateError, FindError, WalkError, OSError, ValueError) as e:
            log(f"error: {e}")
            return 1
        return 0
    label = name or f"{cls} type {type_}"
    out = Path(out) if out else DEFAULT_RE / (name or f"{cls}-{type_}") / "kit"
    refusal = outdir.check(out, REPO_ROOT, lambda n: n in FILES)
    if refusal:
        log(f"error: {refusal}")
        return 2
    try:
        path, ram, scratch, regions = _load(dumps, extracted, state)
        obj_class, offset = CLASSES[cls]
        if not 0 <= type_ < obj_class.types:
            raise KitFailed(f"{cls} type {type_} is outside 0..{obj_class.types - 1}")
        entry = _u32(ram, struct.unpack_from("<I", scratch, offset)[0] + 4 * type_)
        expand = next(((a, b) for a, b in regions if a <= entry < b), None)
        if expand is None:
            raise KitFailed(f"{cls} type {type_} has no entry function ({entry:#x})")
        if is_stub(ram, entry):
            raise KitFailed(f"{cls} type {type_} is an empty slot (stub at {entry:#x})")
        graph = walk(ram, [entry], [expand], regions, helpers["functions"])
        found = neighbours(ram, scratch, graph, expand)
        extra, unwalked = found[:MAX_NEIGHBOURS], found[MAX_NEIGHBOURS:]
        if extra:
            graph = walk(ram, [entry] + [f for _, _, f in extra], [expand], regions, helpers["functions"])
        sprites_in_set = None
        if tables is None:
            tables = found_tables(ram, scratch, regions, cls, type_)
            sprites_in_set = sprite_count(ram, scratch, regions, cls, type_)
        stage, area = stage_area(ram)
        ctx = {"ram": ram, "helpers": helpers, "graph": graph, "label": label, "state": state, "cls": cls,
               "type": type_, "entry": entry, "stage": stage, "area": area, "regions": regions,
               "sha1": hashlib.sha1(path.read_bytes()).hexdigest(), "neighbours": extra, "unwalked": unwalked,
               "main": main_functions(graph, entry),
               "animations": {t: animation_list(ram, t, regions, limit, sprites_in_set) for t in tables},
               "references": references(ram, graph, regions),
               "fields": {f: self_fields(ram, f, end) for f, end in graph.functions.items()}}
        texts = {"summary.md": summary_text(ctx), "code.txt": code_text(ctx), "calls.txt": call_text(ctx),
                 "data.txt": data_text(ctx)}
        tmp = outdir.fresh_tmp(out)
        for file_name, text in texts.items():
            (tmp / file_name).write_text(text)
        outdir.publish(tmp, out)
    except (SaveStateError, SpriteError, FindError, AnimError, WalkError, KitFailed, OSError, ValueError) as e:
        log(f"error: {e}")
        return 1
    log(f"{label}: {len(graph.functions)} functions, {len(graph.tables)} state tables, {len(extra)} neighbours"
        + (f" ({len(unwalked)} more listed, not walked)" if unwalked else "")
        + f", {len(tables)} animation tables -> {out}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.rekit")
    parser.add_argument("--name")
    parser.add_argument("--state")
    parser.add_argument("--type", type=int)
    parser.add_argument("--class", dest="cls", default=MAIN_OBJECTS.name, choices=sorted(CLASSES))
    parser.add_argument("--dumps", default=str(DEFAULT_DUMPS))
    parser.add_argument("--extracted", default=str(DEFAULT_EXTRACTED))
    parser.add_argument("--catalog", default=str(DEFAULT_CATALOG))
    parser.add_argument("--sprites", default=str(DEFAULT_ROOT))
    parser.add_argument("--out")
    parser.add_argument("--at", type=lambda v: int(v, 16))
    parser.add_argument("--player", action="store_true")
    args = parser.parse_args(argv)
    return run(args.name, args.state, args.type, args.cls, Path(args.dumps).expanduser(),
               Path(args.extracted).expanduser(), Path(args.catalog).expanduser(), Path(args.sprites).expanduser(),
               Path(args.out).expanduser() if args.out else None, log=print, at=args.at, player=args.player)


if __name__ == "__main__":
    sys.exit(main())
