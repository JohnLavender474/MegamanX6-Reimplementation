import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.arcentries import ArcError, entry_data, only, split
from mmxdemake.collision import COLOURS, EMPTY_RGB
from mmxdemake.digits import text_mask
from mmxdemake.index import load_index
from mmxdemake.learn import DEFAULT_CATALOG, DEFAULT_DUMPS, DEFAULT_EXTRACTED, TEXTURE, TEXTURE_TYPES, slug, texture_block
from mmxdemake.objects import ObjectError, decode, list_at
from mmxdemake.ora import write_ora
from mmxdemake.palettes import palette
from mmxdemake.png import write_png
from mmxdemake.savestate import SaveStateError, read_state
from mmxdemake.shade import grey
from mmxdemake.stagemap import SIDE, collision, decode_screens, decode_tile_info, tile
from mmxdemake.texture import BLOCK_SIZE, TILE, bits, raw_tile, tile_ref
from mmxdemake.vram import VramError, read_vram

REPO_ROOT = Path(__file__).resolve().parents[1]
SCREEN_PX = SIDE * TILE
COLLISION_PX = TILE
COLLISION_OPACITY = 0.5
ART_LABELS = ("main", "decoration", "background")
LAYERS = 3
REQUIRED = ("name", "member", "width", "height", "overlay", "offset", "differing_cells")
OBJECT_KEYS = ("overlay", "offset", "count")
PALETTE_KEYS = ("clut_x", "clut_y")
OPAQUE = b"\xff" * (TILE * TILE)
MARKER = 4
GAP = 2
SCALE = 2
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
HALF = bytes(v >> 1 for v in range(256))
KIND_COLOURS = [(255, 64, 64), (64, 255, 64), (64, 128, 255), (255, 255, 64), (255, 64, 255), (64, 255, 255),
                (255, 160, 32), (160, 255, 32), (32, 160, 255), (255, 32, 160), (160, 32, 255), (32, 255, 160),
                (200, 200, 200), (255, 128, 128), (128, 255, 128), (128, 128, 255)]


class StagesFailed(Exception):
    pass


def _validate(entries):
    if not isinstance(entries, list) or not all(isinstance(e, dict) for e in entries):
        raise ValueError("catalog stages.json must be a list of entries")
    for e in entries:
        name = e.get("name", "?")
        for key in REQUIRED:
            if key not in e:
                raise ValueError(f"catalog entry {name!r} is missing {key}")
        w, h = e["width"], e["height"]
        for cell in e["differing_cells"]:
            ok = (isinstance(cell, list) and len(cell) == 3 and all(isinstance(v, int) for v in cell)
                  and 0 <= cell[0] < LAYERS and 0 <= cell[1] < w and 0 <= cell[2] < h)
            if not ok:
                raise ValueError(f"catalog entry {name!r} has differing cell {cell} outside the layout")
        if "texture" in e and e["texture"] not in TEXTURE_TYPES:
            raise ValueError(f"catalog entry {name!r} has texture {e['texture']!r}; known types are "
                             f"{', '.join(f'{t:#x}' for t in TEXTURE_TYPES)}")
        if "objects" in e:
            o = e["objects"]
            if not isinstance(o, dict) or not all(isinstance(o.get(k), int) for k in OBJECT_KEYS):
                raise ValueError(f"catalog entry {name!r} has a malformed objects field")
        if "palettes" in e:
            p, s = e["palettes"], e.get("save_state")
            ok = (isinstance(p, dict) and all(isinstance(p.get(k), int) for k in PALETTE_KEYS)
                  and isinstance(s, str) and Path(s).name == s and isinstance(e.get("save_state_sha1"), str))
            if not ok:
                raise ValueError(f"catalog entry {name!r} has a malformed palettes field")


def _allowed(name):
    return name == "summary.txt" or re.fullmatch(r"m\d{3}-[a-z0-9-]+", name) is not None


def _layers(entry, overlay, screen_count):
    w, h = entry["width"], entry["height"]
    raw = bytearray(overlay[entry["offset"]:entry["offset"] + LAYERS * w * h])
    if len(raw) != LAYERS * w * h:
        raise StagesFailed(f"{entry['name']}: overlay {entry['overlay']} ends before the layout")
    for layer, x, y in entry["differing_cells"]:
        raw[layer * w * h + y * w + x] = 0
    bad = sum(1 for v in raw if v >= screen_count)
    if bad:
        raise StagesFailed(f"{entry['name']}: {bad} layout cells are not screen numbers")
    return [[[raw[l * w * h + y * w + x] for x in range(w)] for y in range(h)] for l in range(LAYERS)]


def _grey(ref, values):
    return grey(values, bits(ref.page), ref.palette)


def _shader(vram, rule):
    def shade(ref, values):
        pal = palette(vram, rule, ref.palette, 16 if bits(ref.page) == 4 else 256)
        return b"".join(bytes(pal[v]) for v in values)
    return shade


def _vram_of(dumps, entry):
    path = dumps / entry["save_state"]
    if not path.is_file():
        raise StagesFailed(f"{entry['name']}: save state {path} does not exist")
    sha1 = hashlib.sha1(path.read_bytes()).hexdigest()
    if sha1 != entry["save_state_sha1"]:
        raise StagesFailed(f"{entry['name']}: save state {entry['save_state']} has sha1 {sha1}, "
                           f"the catalog says {entry['save_state_sha1']}")
    try:
        return read_vram(read_state(path))
    except (SaveStateError, VramError) as e:
        raise StagesFailed(f"{entry['name']}: {e}") from e


def _stage_png(layers, w, h, screens, info, block, cache, shade):
    width, height = w * SCREEN_PX, h * SCREEN_PX
    rows = [bytearray(b"\0\0\0\xff" * width) for _ in range(height)]
    for layer in (2, 1, 0):
        _draw_art(rows, layers[layer], w, h, screens, info, block, cache, shade)
    return width, height, [bytes(r) for r in rows]


def _art_layer(layout, w, h, screens, info, block, cache, shade):
    rows = [bytearray(w * SCREEN_PX * 4) for _ in range(h * SCREEN_PX)]
    _draw_art(rows, layout, w, h, screens, info, block, cache, shade)
    return [bytes(r) for r in rows]


def _draw_art(rows, layout, w, h, screens, info, block, cache, shade):
    stride = TILE * 4
    for y in range(h):
        for x in range(w):
            s = layout[y][x]
            if not s:
                continue
            for k, v in enumerate(screens[s]):
                t = tile(v)
                if not t:
                    continue
                if t not in cache:
                    ref = tile_ref(info[t])
                    values = raw_tile(block, ref)
                    if values is None:
                        cache[t] = None
                    else:
                        rgba = shade(ref, values)
                        cache[t] = (rgba, rgba[3::4] == OPAQUE)
                if cache[t] is None:
                    continue
                rgba, opaque = cache[t]
                px = x * SCREEN_PX + (k % SIDE) * TILE
                py = y * SCREEN_PX + (k // SIDE) * TILE
                for r in range(TILE):
                    line, src = rows[py + r], rgba[r * stride:(r + 1) * stride]
                    if opaque:
                        line[px * 4:px * 4 + stride] = src
                        continue
                    for i in range(TILE):
                        if src[i * 4 + 3]:
                            line[(px + i) * 4:(px + i) * 4 + 4] = src[i * 4:i * 4 + 4]


def _collision_png(layer0, w, h, screens, info):
    width, height = w * SIDE * COLLISION_PX, h * SIDE * COLLISION_PX
    rows = [bytearray(bytes(EMPTY_RGB + (255,)) * width) for _ in range(height)]
    for y in range(h):
        for x in range(w):
            s = layer0[y][x]
            if not s:
                continue
            for k, v in enumerate(screens[s]):
                t = tile(v)
                rgb = EMPTY_RGB if t == 0 else COLOURS[collision(info[t])]
                block = bytes(rgb + (255,)) * COLLISION_PX
                px = (x * SIDE + k % SIDE) * COLLISION_PX
                py = (y * SIDE + k // SIDE) * COLLISION_PX
                for r in range(COLLISION_PX):
                    rows[py + r][px * 4:(px + COLLISION_PX) * 4] = block
    return width, height, [bytes(r) for r in rows]


def _objects(extracted, entry, w, h):
    o = entry["objects"]
    body = (extracted / f"rock_bin/{o['overlay']:02d}.bin").read_bytes()
    try:
        records = decode(list_at(body, o["offset"]), w, h)
    except ObjectError as e:
        raise StagesFailed(f"{entry['name']}: {e}") from e
    if len(records) != o["count"]:
        raise StagesFailed(f"{entry['name']}: {len(records)} objects on disc, the catalog says {o['count']}")
    return records


def _draw_markers(rows, width, height, records):
    def put(x, y, rgb):
        if 0 <= x < width and 0 <= y < height:
            rows[y][x * 4:x * 4 + 4] = bytes(rgb + (255,))

    for rec in records:
        cx, cy = rec["x"], rec["y"]
        for dy in range(-MARKER, MARKER + 1):
            for dx in range(-MARKER, MARKER + 1):
                edge = abs(dx) == MARKER or abs(dy) == MARKER
                put(cx + dx, cy + dy, BLACK if edge else KIND_COLOURS[rec["kind"] % len(KIND_COLOURS)])
        mask = text_mask(str(rec["index"]))
        lw, lh = len(mask[0]) * SCALE, len(mask) * SCALE
        left = cx + MARKER + GAP + 1
        if left + lw > width:
            left = cx - MARKER - GAP - lw
        top = cy - lh // 2
        on = [(left + c * SCALE + i, top + r * SCALE + j) for r, line in enumerate(mask)
              for c, bit in enumerate(line) if bit for i in range(SCALE) for j in range(SCALE)]
        for x, y in on:
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    put(x + dx, y + dy, BLACK)
        for x, y in on:
            put(x, y, WHITE)
    return rows


def _marker_layer(width, height, records):
    rows = [bytearray(width * 4) for _ in range(height)]
    return [bytes(r) for r in _draw_markers(rows, width, height, records)]


def _merged(width, height, stage_rows, collision_rows, records):
    rows = []
    for s, c in zip(stage_rows, collision_rows):
        total = int.from_bytes(s.translate(HALF), "little") + int.from_bytes(c.translate(HALF), "little")
        row = bytearray(total.to_bytes(width * 4, "little"))
        row[3::4] = b"\xff" * width
        rows.append(row)
    return [bytes(r) for r in _draw_markers(rows, width, height, records)]


def _object_rows(records):
    head = f"{'index':>5} {'kind':>4} {'id':>3} {'b':>3} {'a':>3} {'x':>5} {'y':>5} screen"
    return [head] + [f"{r['index']:5d} {r['kind']:4d} {r['id']:3d} {r['b']:3d} {r['a']:3d} {r['x']:5d} {r['y']:5d} "
                     f"{r['x'] // SCREEN_PX},{r['y'] // SCREEN_PX}" for r in records]


def _draw(extracted, dumps, tmp, entry, record):
    body = (extracted / record["path"]).read_bytes()
    if hashlib.sha1(body).hexdigest() != record["sha1"]:
        raise StagesFailed(f"{entry['name']}: member {entry['member']} sha1 does not match the index")
    entries = split(body)
    screens = decode_screens(entry_data(body, only(entries, 0)))
    info = decode_tile_info(entry_data(body, only(entries, 1)))
    try:
        block = texture_block(body, entries, entry.get("texture", TEXTURE))
    except VramError as e:
        raise StagesFailed(f"{entry['name']}: {e}") from e
    if len(block) != BLOCK_SIZE:
        raise StagesFailed(f"{entry['name']}: texture block is {len(block)} bytes, need {BLOCK_SIZE}")
    overlay = (extracted / f"rock_bin/{entry['overlay']:02d}.bin").read_bytes()
    layers = _layers(entry, overlay, len(screens))
    w, h = entry["width"], entry["height"]
    folder = tmp / f"m{entry['member']:03d}-{slug(entry['name'])}"
    folder.mkdir()
    doc = {"schema_version": 1, "name": entry["name"], "member": entry["member"], "width": w, "height": h,
           "source": {"overlay": entry["overlay"], "offset": entry["offset"]}, "layers": layers}
    (folder / "layout.json").write_text(json.dumps(doc, sort_keys=True) + "\n")
    cache = {}
    shade = _shader(_vram_of(dumps, entry), entry["palettes"]) if "palettes" in entry else _grey
    width, height, stage_rows = _stage_png(layers, w, h, screens, info, block, cache, shade)
    write_png(folder / "stage.png", width, height, stage_rows)
    _, _, collision_rows = _collision_png(layers[0], w, h, screens, info)
    write_png(folder / "collision.png", width, height, collision_rows)
    line = f"{entry['name']}: member {entry['member']}, {w}x{h}, overlay {entry['overlay']} @{entry['offset']:#x}"
    table = []
    ora_layers = [("collision", collision_rows, COLLISION_OPACITY)] + [
        (f"art {n} {label}", _art_layer(layers[n], w, h, screens, info, block, cache, shade), 1.0)
        for n, label in enumerate(ART_LABELS)]
    records = []
    if "objects" in entry:
        records = _objects(extracted, entry, w, h)
        o = entry["objects"]
        obj_doc = {"schema_version": 1, "name": entry["name"], "member": entry["member"],
                   "source": {"overlay": o["overlay"], "offset": o["offset"]}, "objects": records}
        (folder / "objects.json").write_text(json.dumps(obj_doc, sort_keys=True) + "\n")
        ora_layers.insert(0, ("objects", _marker_layer(width, height, records), 1.0))
        line += f", {len(records)} objects"
        table = [f"objects: {len(records)}"] + _object_rows(records)
    else:
        line += ", objects not learned"
    line += ", colour" if "palettes" in entry else ", palettes not learned"
    if "texture" in entry:
        line += f", texture {entry['texture']:#x}"
    write_ora(folder / "stage.ora", width, height, ora_layers,
              _merged(width, height, stage_rows, collision_rows, records))
    (folder / "summary.txt").write_text("\n".join([line] + table) + "\n")
    return line


def run(extracted, out, catalog=DEFAULT_CATALOG, log=print, dumps=DEFAULT_DUMPS):
    extracted, out, catalog = Path(extracted), Path(out), Path(catalog)
    dumps = Path(dumps)
    refusal = outdir.check(out, REPO_ROOT, _allowed)
    if refusal:
        log(f"error: {refusal}")
        return 2
    try:
        records = {r["index"]: r for r in load_index(extracted / "index.json")["files"] if r["kind"] == "rock_dat"}
        entries = json.loads((catalog / "stages.json").read_text())
        _validate(entries)
    except (OSError, ValueError, KeyError, TypeError, AttributeError) as e:
        log(f"error: {e}")
        return 2
    try:
        if not entries:
            raise StagesFailed("catalog has no stage entries")
        tmp = outdir.fresh_tmp(out)
        lines = []
        for entry in entries:
            record = records.get(entry["member"])
            if record is None:
                raise StagesFailed(f"{entry['name']}: member {entry['member']} is not in the index")
            line = _draw(extracted, dumps, tmp, entry, record)
            log(line)
            lines.append(line)
        (tmp / "summary.txt").write_text("\n".join(lines + ["done"]) + "\n")
        outdir.publish(tmp, out)
    except (StagesFailed, ArcError, ValueError, KeyError, OSError) as e:
        log(f"error: {e}")
        return 1
    log(f"done: {out}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.stages")
    parser.add_argument("--extracted", default=str(DEFAULT_EXTRACTED))
    parser.add_argument("--out", required=True)
    parser.add_argument("--catalog", default=str(DEFAULT_CATALOG))
    parser.add_argument("--dumps", default=str(DEFAULT_DUMPS))
    args = parser.parse_args(argv)
    return run(Path(args.extracted).expanduser(), Path(args.out).expanduser(), Path(args.catalog).expanduser(),
               log=print, dumps=Path(args.dumps).expanduser())


if __name__ == "__main__":
    sys.exit(main())
