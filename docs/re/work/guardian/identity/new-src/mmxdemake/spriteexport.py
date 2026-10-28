import argparse
import hashlib
import json
import re
import shutil
import struct
import sys
from functools import partial
from itertools import product
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.animfind import (MAIN_OBJECTS, MODE_BITS, SPRITE_FIELD, STAGE_OBJECTS, TEXTURE_FIELD, FindError,
                                ObjectClass, choose, code_regions, function_start, live_objects, live_sources,
                                live_tables, searched_tables, stage_types)
from mmxdemake.anims import FPS, AnimError, read_table, walk
from mmxdemake.digits import text_mask
from mmxdemake.fingerprint import PREFIX, content_id
from mmxdemake.learn import DEFAULT_CATALOG, DEFAULT_DUMPS, DEFAULT_EXTRACTED
from mmxdemake.png import write_png
from mmxdemake.savestate import SaveStateError, ram_at, ram_from, read_state
from mmxdemake.sprites import (CLEAR, ENEMY_MODE, Bank, Image, SpriteError, bank_of_type, banks, cells, draw, entries,
                               scratchpad, stage_area, with_source)
from mmxdemake.vram import VramError, read_vram

REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_ROOT = Path.home() / "mmx-data/x6/sprites"
EXE = "iso/SLUS_013.95"
SUFFIX = ".state.gz"
TOP = ("enemies", "banks", "stages", "players")
NAME = re.compile(r"[a-z0-9-]+")
CONTENT_ID = re.compile(r"enemy-[0-9a-f]{10}")
ADDRESS = re.compile(r"0x[0-9A-Fa-f]{8}")
FIRST_MISC = 0x80
COLUMNS = 8
PAD = 2
LABEL = 7
BACK = bytes((40, 40, 60, 255))
INK = bytes((255, 255, 255, 255))
NO_TEXTURE = "no texture loaded"
NO_CODE = "update function is in neither the stage overlay nor the main program"
TRASH = "_old"
OVERWRITTEN = "cells overwritten by type"
SHEET = "sprites.png"
CLUT_MASK = 0x7FFF
VISUAL_OBJECTS = ObjectClass("visual", 0x80093C98, 32, 0xA0, 58)
EXTRA_CLASSES = (VISUAL_OBJECTS, ObjectClass("misc", 0x800CD410, 96, 0x60, 64),
                 ObjectClass("misc", 0x800964A0, 32, 0x60, 64))


class ExportFailed(Exception):
    pass


def load_catalog(catalog):
    doc = json.loads((Path(catalog) / "enemies.json").read_text())
    if not isinstance(doc, dict) or set(doc) - {"stages", "names", "tables"}:
        raise ValueError("enemies.json must be an object with only stages, names and tables")
    stages, names, tables = doc.get("stages", {}), doc.get("names", {}), doc.get("tables", [])
    if not isinstance(stages, dict) or not isinstance(names, dict) or not isinstance(tables, list):
        raise ValueError("enemies.json: stages and names must be objects, tables a list")
    for k, v in stages.items():
        if not re.fullmatch(r"\d+", k) or not isinstance(v, str) or not NAME.fullmatch(v):
            raise ValueError(f"enemies.json: bad stage entry {k!r}: {v!r}")
    for k, v in names.items():
        if not CONTENT_ID.fullmatch(k):
            raise ValueError(f"enemies.json: {k!r} is not an ID")
        if not isinstance(v, str) or not NAME.fullmatch(v) or v.startswith(PREFIX):
            raise ValueError(f"enemies.json: bad name {v!r} for {k}")
    values = list(names.values())
    twice = sorted({v for v in values if values.count(v) > 1})
    if twice:
        raise ValueError(f"enemies.json: name {twice[0]!r} is used twice")
    by_type = {}
    for e in tables:
        if not (isinstance(e, dict) and set(e) == {"stage", "type", "animations"}
                and all(type(e[k]) is int for k in ("stage", "type"))
                and isinstance(e["animations"], str) and ADDRESS.fullmatch(e["animations"])):
            raise ValueError(f"enemies.json: bad tables entry {e!r}")
        by_type[(e["stage"], e["type"])] = int(e["animations"], 16)
    return {int(k): v for k, v in stages.items()}, names, by_type


def _read_id(folder):
    try:
        cid = json.loads((folder / "animations.json").read_text())["id"]
    except (OSError, ValueError, KeyError, TypeError):
        return None
    return cid if isinstance(cid, str) and CONTENT_ID.fullmatch(cid) else None


def existing(root):
    found = {}
    enemies = Path(root) / "enemies"
    if enemies.is_dir():
        for p in sorted(enemies.iterdir()):
            cid = _read_id(p) if p.is_dir() and NAME.fullmatch(p.name) else None
            if cid is None or cid in found:
                raise ValueError(f"{p} is not an earlier export; choose another --root")
            found[cid] = p.name
    return found


def fitting(ram, label, table, pointers, bank, found, log):
    anims = []
    for k, p in enumerate(pointers):
        try:
            frames, loop = walk(ram, p)
            top = max(f["sprite"] for f in frames)
            problem = f"uses sprite {top}, bank {bank.index} has {bank.count}" if top >= bank.count else None
        except AnimError as e:
            problem = str(e)
        if problem:
            if found == "catalog" or not anims:
                raise AnimError(f"{label}: table {table:#x}: animation {k} {problem}")
            log(f"{label}: table {table:#x}: kept {k} of {len(pointers)} animations; animation {k} {problem}")
            break
        anims.append((frames, loop))
    return anims


def live_banks(ram, bank_list, sources):
    out = {}
    for bank in bank_list:
        if bank.texture is None and bank.type not in out:
            out[bank.type] = [(with_source(ram, bank, data, texture, clut), mode)
                              for data, texture, clut, mode in sorted(sources.get(bank.type, ()))]
    return out


def overwritten(ram, bank_list, takers):
    taken = []
    for label, owner, data, texture, clut, mode in takers:
        try:
            taken.append((label, texture, cells(ram, with_source(ram, owner, data, texture, clut), mode)))
        except SpriteError:
            continue
    out = {}
    for bank in bank_list:
        if bank.texture is None:
            continue
        mine = cells(ram, bank)
        hit = next((t for t, texture, theirs in taken if texture != bank.texture and mine & theirs), None)
        if hit is not None:
            out[bank.index] = hit
    return out


def extra_objects(ram, scratch, bank_list):
    banked = {b.data for b in bank_list}
    index = {}
    for i, data in enumerate(entries(ram, scratch)):
        if i >= len(bank_list) and data not in banked:
            index.setdefault(data, i)
    found = set()
    for cls in EXTRA_CLASSES:
        for s in range(cls.slots):
            a = cls.base + s * cls.size
            flags, t = ram_at(ram, a, 2)
            data = struct.unpack("<I", ram_at(ram, a + SPRITE_FIELD, 4))[0]
            if flags and t < cls.types and data in index:
                texture, clut = struct.unpack("<HH", ram_at(ram, a + TEXTURE_FIELD, 4))
                found.add((cls.name, t, index[data], data, texture, clut & CLUT_MASK, flags & MODE_BITS))
    return sorted(found)


def second_bank(bank_list, bank):
    after = bank.index + 1
    if after < len(bank_list) and bank_list[after].type == bank.type and bank_list[after].texture is not None:
        return bank_list[after]
    return None


def state_table(ram, table, lo, hi):
    try:
        pointers = read_table(ram, table, lo, hi)
    except AnimError:
        return False
    return all(function_start(ram, p, lo) == p for p in pointers)


def collect(ram, scratch, vram, regions, stage, area, tables, state, sha1, log):
    bank_list = banks(ram, scratch)
    types, searched = {}, {}
    for lo, hi in regions:
        types.update(stage_types(ram, scratch, lo, hi))
        searched.update(searched_tables(ram, scratch, lo, hi))
    live = live_tables(ram, regions)
    sources = live_sources(ram, regions)
    stage_objects = live_objects(ram, regions, STAGE_OBJECTS)
    extra = live_banks(ram, bank_list, sources)
    takers = [(str(t), bank_of_type(bank_list, t) or Bank(-1, t, data, 0, None, clut), data, texture, clut, mode)
              for t, found in sorted(sources.items()) for data, texture, clut, mode in sorted(found)]
    takers += [(f"{t} ({STAGE_OBJECTS.name} object)", Bank(-1, t, data, 0, None, clut), data, texture, clut, mode)
               for t, _, data, texture, clut, mode in stage_objects]
    stale = overwritten(ram, bank_list, takers)
    exports, skipped, seen = {}, [], set()
    add = partial(add_export, ram, vram, regions, exports, skipped, log)
    for bank in bank_list:
        t = bank.type
        if t >= FIRST_MISC or t in seen:
            continue
        seen.add(t)
        variants = [(bank, ENEMY_MODE, "bank")] if bank.texture is not None else [
            (b, mode, "live") for b, mode in extra.get(t, [])]
        second = second_bank(bank_list, bank)
        if second is not None and second.index not in stale:
            variants.append((second, ENEMY_MODE, "bank"))
        reason = (NO_CODE if t not in types else f"{OVERWRITTEN} {stale[bank.index]}" if bank.index in stale
                  else NO_TEXTURE if not variants else None)
        if reason:
            skipped.append({"type": t, "reason": reason})
            continue
        choices, note = choose(tables.get((stage, t)), live.get(t), searched.get(t))
        if note:
            log(f"type {t}: {note}")
        if not choices:
            skipped.append({"type": t, "reason": note})
            continue
        for (table, found), (source_bank, mode, sprites) in product(choices, variants):
            add(table, found, source_bank, mode,
                {"class": MAIN_OBJECTS.name, "stage": stage, "area": area, "type": t, "bank": source_bank.index,
                 "sprites": sprites, "state": state, "state_sha1": sha1})
    stage_sheets, counts = [], {}
    for t, table, data, texture, clut, mode in stage_objects:
        source_bank = with_source(ram, Bank(-1, t, data, 0, None, clut), data, texture, clut)
        k = counts[t] = counts.get(t, -1) + 1
        stage_sheets.append((f"{STAGE_OBJECTS.name}-{t}-live-{k}", source_bank, mode))
        add(table, "live", source_bank, mode,
            {"class": STAGE_OBJECTS.name, "stage": stage, "area": area, "type": t, "bank": None,
             "sprites": "live", "state": state, "state_sha1": sha1})
    names = {}
    for cls, t, index, data, texture, clut, mode in extra_objects(ram, scratch, bank_list):
        source_bank = with_source(ram, Bank(index, t, data, 0, None, clut), data, texture, clut)
        key = (index, data, texture, clut, mode)
        if key not in names:
            names[key] = f"entry-{index}-live-{sum(k[0] == index for k in names)}"
            stage_sheets.append((names[key], source_bank, mode))
        add_sheet(ram, vram, exports, skipped, log, source_bank, mode,
                  {"class": cls, "stage": stage, "area": area, "type": t, "bank": None, "entry": index,
                   "sprites": "live", "state": state, "state_sha1": sha1}, "live")
    return bank_list, exports, skipped, extra, stale, stage_sheets


def add_sheet(ram, vram, exports, skipped, log, source_bank, mode, base, found):
    label = _label(base)
    try:
        if not source_bank.count:
            raise SpriteError("the sprite source has no sprites")
        frames = {n: draw(ram, vram, source_bank, n, mode) for n in range(source_bank.count)}
        every = [([{"sprite": n, "ticks": 1, "flag": 0} for n in sorted(frames)], None)]
        cid = content_id(ram, vram, source_bank, every, mode)
    except SpriteError as e:
        if found == "catalog":
            raise
        log(f"{label}: skipped: {e}")
        entry = {"type": base["type"], "reason": str(e)}
        entry.update({k: base[k] for k in ("table", "entry") if k in base})
        if base["class"] != MAIN_OBJECTS.name:
            entry["class"] = base["class"]
        skipped.append(entry)
        return
    export = exports.setdefault(cid, {"id": cid, "animations": [], "frames": frames, "sources": [], "sheet": True})
    export["sources"].append(dict(base, found=found))


def add_export(ram, vram, regions, exports, skipped, log, table, found, source_bank, mode, base):
    t, label = base["type"], _label(base)
    region = next(((lo, hi) for lo, hi in regions if lo <= table < hi), None)
    if region is not None and state_table(ram, table, *region):
        add_sheet(ram, vram, exports, skipped, log, source_bank, mode, dict(base, table=f"{table:#010x}"), found)
        return
    try:
        region = next(((lo, hi) for lo, hi in regions if lo <= table < hi), None)
        if region is None:
            raise AnimError(f"{label}: table {table:#x} is outside the overlay and the main program")
        anims = fitting(ram, label, table, read_table(ram, table, *region), source_bank, found, log)
        used = sorted({f["sprite"] for frames, _ in anims for f in frames})
        cid = content_id(ram, vram, source_bank, anims, mode)
        frames = {n: draw(ram, vram, source_bank, n, mode) for n in used}
    except (AnimError, SpriteError) as e:
        if found == "catalog":
            raise
        log(f"{label}: skipped: {e}")
        entry = {"type": t, "table": f"{table:#010x}", "reason": str(e)}
        if base["class"] != MAIN_OBJECTS.name:
            entry["class"] = base["class"]
        skipped.append(entry)
        return
    export = exports.setdefault(cid, {"id": cid, "animations": anims, "frames": frames, "sources": []})
    export["sources"].append(dict(base, table=f"{table:#010x}", found=found))


def _label(source):
    if source.get("class", MAIN_OBJECTS.name) == MAIN_OBJECTS.name:
        return f"type {source['type']}"
    return f"{source['class']} type {source['type']}"


def geometry(frames):
    left = max(im.ox for im in frames.values())
    top = max(im.oy for im in frames.values())
    right = max(im.width - im.ox for im in frames.values())
    bottom = max(im.height - im.oy for im in frames.values())
    return left + right, top + bottom, left, top


def _blit(canvas, im, x0, y0):
    for y, line in enumerate(im.pixels):
        for x, p in enumerate(line):
            if p[3]:
                o = (x0 + x) * 4
                canvas[y0 + y][o:o + 4] = bytes(p)


def strip(frames, sequence, w, h, ox, oy):
    canvas = [bytearray(w * len(sequence) * 4) for _ in range(h)]
    for k, f in enumerate(sequence):
        im = frames[f["sprite"]]
        _blit(canvas, im, k * w + ox - im.ox, oy - im.oy)
    return w * len(sequence), h, [bytes(r) for r in canvas]


def sprite_grid(frames, columns, w, h, ox, oy):
    rows = -(-len(frames) // columns)
    canvas = [bytearray(w * columns * 4) for _ in range(h * rows)]
    for k, n in enumerate(sorted(frames)):
        im = frames[n]
        _blit(canvas, im, (k % columns) * w + ox - im.ox, (k // columns) * h + oy - im.oy)
    return w * columns, h * rows, [bytes(r) for r in canvas]


def sheet(ram, vram, bank, log, mode=ENEMY_MODE):
    images = []
    for n in range(bank.count):
        try:
            images.append(draw(ram, vram, bank, n, mode))
        except SpriteError as e:
            log(f"bank {bank.index}: sprite {n}: {e}")
            images.append(Image(1, 1, 0, 0, [[CLEAR]]))
    return grid(images)


def grid(images):
    cw = max(max(im.width for im in images), len(str(len(images) - 1)) * 4) + 2 * PAD
    ch = max(im.height for im in images) + LABEL + 2 * PAD
    cols = min(COLUMNS, len(images))
    rows = -(-len(images) // cols)
    canvas = [bytearray(BACK * (cw * cols)) for _ in range(ch * rows)]
    for n, im in enumerate(images):
        x0, y0 = (n % cols) * cw + PAD, (n // cols) * ch + PAD
        for y, line in enumerate(text_mask(str(n))):
            for x, bit in enumerate(line):
                if bit:
                    canvas[y0 + y][(x0 + x) * 4:(x0 + x) * 4 + 4] = INK
        _blit(canvas, im, x0, y0 + LABEL)
    return cw * cols, ch * rows, [bytes(r) for r in canvas]


def _key(source):
    bank = source.get("bank")
    return (source.get("class", MAIN_OBJECTS.name), source["stage"], source["area"], source["type"], source["state"],
            -1 if bank is None else bank)


def merged(old, new, state):
    out = {_key(s): s for s in old if s["state"] != state}
    out.update({_key(s): s for s in new})
    return [out[k] for k in sorted(out)]


def _retire(path, trash):
    trash.mkdir(parents=True, exist_ok=True)
    path.rename(trash / str(len(list(trash.iterdir()))))


def _replace(src, dst, trash):
    dst.parent.mkdir(parents=True, exist_ok=True)
    if dst.exists():
        _retire(dst, trash)
    src.rename(dst)


def publish(root, stage_name, stem, area, sha1, ram, vram, bank_list, exports, skipped, extra, stale, stage_sheets,
            names, before, log):
    extra = dict(extra)
    targets = {cid: names.get(cid, cid) for cid in exports}
    renames = {cid: names[cid] for cid, folder in before.items()
               if cid not in exports and cid in names and names[cid] != folder}
    owner = {folder: cid for cid, folder in before.items()}
    for cid, folder in list(targets.items()) + list(renames.items()):
        other = owner.get(folder)
        if other is not None and other != cid:
            raise ExportFailed(f"enemies/{folder} holds {other}, not {cid}")
    tmp = outdir.fresh_tmp(root)
    bank_dir = tmp / "banks" / stage_name / stem
    bank_dir.mkdir(parents=True)
    drawn = without = 0
    for bank in bank_list:
        if bank.texture is None:
            without += 1
            log(f"bank {bank.index}: type {bank.type}, no texture")
            if bank.type in extra:
                for k, (live_bank, mode) in enumerate(extra.pop(bank.type)):
                    write_png(bank_dir / f"bank-{bank.index}-live-{k}.png", *sheet(ram, vram, live_bank, log, mode))
                    log(f"bank {bank.index}: type {bank.type}, live object {k}, {live_bank.count} sprites")
        elif bank.index in stale:
            log(f"bank {bank.index}: type {bank.type}, {OVERWRITTEN} {stale[bank.index]}")
        elif bank.count:
            write_png(bank_dir / f"bank-{bank.index}.png", *sheet(ram, vram, bank, log))
            drawn += 1
            log(f"bank {bank.index}: type {bank.type}, {bank.count} sprites")
    for name, live_bank, mode in stage_sheets:
        write_png(bank_dir / f"{name}.png", *sheet(ram, vram, live_bank, log, mode))
        log(f"{name}: {live_bank.count} sprites")
    for cid, export in exports.items():
        folder = tmp / "enemies" / targets[cid]
        folder.mkdir(parents=True)
        w, h, ox, oy = geometry(export["frames"])
        if export.get("sheet"):
            columns = min(COLUMNS, len(export["frames"]))
            write_png(folder / SHEET, *sprite_grid(export["frames"], columns, w, h, ox, oy))
            body = {"sheet": {"file": SHEET, "columns": columns, "sprites": len(export["frames"])}}
            what = f"sheet of {len(export['frames'])} sprites"
        else:
            for k, (frames, _) in enumerate(export["animations"]):
                write_png(folder / f"anim-{k}.png", *strip(export["frames"], frames, w, h, ox, oy))
            body = {"fps": FPS}
            what = f"{len(export['animations'])} animations"
        old = []
        if cid in before:
            old = json.loads((root / "enemies" / before[cid] / "animations.json").read_text())["sources"]
        doc = dict(body, id=cid, cell=[w, h], origin=[ox, oy],
                   animations=[{"index": k, "frames": frames, "loop_to": loop}
                               for k, (frames, loop) in enumerate(export["animations"])],
                   sources=merged(old, export["sources"], stem + SUFFIX))
        (folder / "animations.json").write_text(json.dumps(doc, indent=2, sort_keys=True) + "\n")
        log(f"{targets[cid]}: {cid}, {what}, " + ", ".join(f"{_label(s)} ({s['found']})" for s in export["sources"]))
    index_path = root / "stages" / f"{stage_name}.json"
    index = json.loads(index_path.read_text()) if index_path.exists() else {"states": {}}
    index["states"][stem] = {"area": area, "state_sha1": sha1, "enemies": sorted(exports), "skipped": skipped}
    (tmp / "stages").mkdir()
    (tmp / "stages" / index_path.name).write_text(json.dumps(index, indent=2, sort_keys=True) + "\n")
    trash = tmp / "enemies" / TRASH
    try:
        for cid in exports:
            if cid in before and before[cid] != targets[cid]:
                _retire(root / "enemies" / before[cid], trash)
            _replace(tmp / "enemies" / targets[cid], root / "enemies" / targets[cid], trash)
        _replace(bank_dir, root / "banks" / stage_name / stem, trash)
        _replace(tmp / "stages" / index_path.name, index_path, trash)
        for cid, name in renames.items():
            (root / "enemies" / before[cid]).rename(root / "enemies" / name)
            log(f"renamed enemies/{before[cid]} -> enemies/{name}")
        shutil.rmtree(tmp)
    except OSError as e:
        raise ExportFailed(f"moving into {root} failed ({e}); run the same state again to repair it") from e
    return drawn, without, len(renames)


def run(state, dumps, extracted, catalog, root, log=print):
    dumps, extracted, root = Path(dumps), Path(extracted), Path(root)
    if Path(state).name != state or not state.endswith(SUFFIX):
        log(f"error: --state {state!r} must be a file name ending in {SUFFIX}")
        return 2
    try:
        stage_names, names, tables = load_catalog(catalog)
        refusal = outdir.check(root, REPO_ROOT, lambda n: n in TOP)
        if refusal:
            raise ValueError(refusal)
        before = existing(root)
    except (OSError, ValueError) as e:
        log(f"error: {e}")
        return 2
    try:
        path = dumps / state
        data = read_state(path)
        exe = (extracted / EXE).read_bytes()
        ram = ram_from(data, exe, path)
        scratch, vram = scratchpad(data), read_vram(data)
        overlays = {int(p.stem): p.read_bytes() for p in sorted((extracted / "rock_bin").glob("*.bin"))}
        regions = code_regions(ram, overlays, exe)
        stage, area = stage_area(ram)
        sha1 = hashlib.sha1(path.read_bytes()).hexdigest()
        bank_list, exports, skipped, extra, stale, stage_sheets = collect(ram, scratch, vram, regions, stage, area,
                                                                          tables, state, sha1, log)
        drawn, without, renamed = publish(root, stage_names.get(stage, f"stage-{stage}"), state[:-len(SUFFIX)],
                                          area, sha1, ram, vram, bank_list, exports, skipped, extra, stale,
                                          stage_sheets, names, before, log)
    except (SaveStateError, SpriteError, AnimError, FindError, VramError, ExportFailed, OSError, ValueError) as e:
        log(f"error: {e}")
        return 1
    found = [s["found"] for e in exports.values() for s in e["sources"]]
    log(f"{state}: stage {stage} area {area}, {drawn} banks drawn, {without} without texture, "
        f"{len(exports)} exports ({found.count('live')} live, {found.count('search')} search, "
        f"{found.count('catalog')} catalog), {len(skipped)} skipped, {renamed} renamed")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.spriteexport")
    parser.add_argument("--state", required=True)
    parser.add_argument("--dumps", default=str(DEFAULT_DUMPS))
    parser.add_argument("--extracted", default=str(DEFAULT_EXTRACTED))
    parser.add_argument("--catalog", default=str(DEFAULT_CATALOG))
    parser.add_argument("--root", default=str(DEFAULT_ROOT))
    args = parser.parse_args(argv)
    return run(args.state, Path(args.dumps).expanduser(), Path(args.extracted).expanduser(),
               Path(args.catalog).expanduser(), Path(args.root).expanduser(), log=print)


if __name__ == "__main__":
    sys.exit(main())
