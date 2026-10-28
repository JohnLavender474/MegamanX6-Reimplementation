import argparse
import colorsys
import hashlib
import json
import re
import sys
from collections import Counter
from math import ceil
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.arcentries import ArcError, entry_data, only, split
from mmxdemake.index import load_index
from mmxdemake.png import write_png
from mmxdemake.stagemap import SIDE, check, collision, decode_screens, decode_tile_info, flags, tile

REPO_ROOT = Path(__file__).resolve().parents[1]
SCHEMA_VERSION = 1
EXPECT_STAGES = 23
SCREENS_PER_ROW = 16
CELL = 4
GAP = 2
LEGEND_SQUARE = 16
GAP_RGB = (60, 60, 60)
EMPTY_RGB = (16, 16, 16)
BACKGROUND_RGB = (40, 40, 64)


def _colour(value):
    r, g, b = colorsys.hsv_to_rgb((value * 0.618034) % 1, 0.75, 0.95)
    return round(r * 255), round(g * 255), round(b * 255)


COLOURS = [BACKGROUND_RGB] + [_colour(v) for v in range(1, 64)]


class StageFailed(Exception):
    pass


def _allowed(name):
    return name in ("summary.txt", "legend.png") or re.fullmatch(r"m\d{3}", name) is not None


MAP_TYPES = {0, 1}
STAGE_TYPES = {0, 1, 0x10102}


def _types(r):
    return {t for t, _ in r.get("observed", {}).get("subentries", [])}


def map_records(records):
    return [r for r in records if MAP_TYPES <= _types(r)]


def stage_records(records):
    return [r for r in records if STAGE_TYPES <= _types(r)]


def _cell_rgb(v, info):
    if tile(v) == 0:
        return EMPTY_RGB
    return COLOURS[collision(info[tile(v)])]


def _sheet(screens, info):
    step = SIDE * CELL + GAP
    width = min(SCREENS_PER_ROW, len(screens)) * step + GAP
    height = ceil(len(screens) / SCREENS_PER_ROW) * step + GAP
    rows = [bytearray(bytes(GAP_RGB + (255,)) * width) for _ in range(height)]
    for n, screen in enumerate(screens):
        ox = (n % SCREENS_PER_ROW) * step + GAP
        oy = (n // SCREENS_PER_ROW) * step + GAP
        for k, v in enumerate(screen):
            block = bytes(_cell_rgb(v, info) + (255,)) * CELL
            x0 = (ox + (k % SIDE) * CELL) * 4
            y0 = oy + (k // SIDE) * CELL
            for dy in range(CELL):
                rows[y0 + dy][x0:x0 + CELL * 4] = block
    return width, height, [bytes(r) for r in rows]


def _legend():
    rows = []
    for r in range(8):
        row = b"".join(bytes(COLOURS[8 * r + c] + (255,)) * LEGEND_SQUARE for c in range(8))
        rows += [row] * LEGEND_SQUARE
    return 8 * LEGEND_SQUARE, 8 * LEGEND_SQUARE, rows


def _grid(screen, fn):
    return [[fn(screen[y * SIDE + x]) for x in range(SIDE)] for y in range(SIDE)]


def _process(extracted, tmp, r, log):
    i = r["index"]
    try:
        body = (extracted / r["path"]).read_bytes()
        sha1 = hashlib.sha1(body).hexdigest()
        if len(body) != r["size"] or sha1 != r["sha1"]:
            raise StageFailed(f"member {i}: size {len(body)}, sha1 {sha1} do not match the index "
                              f"({r['size']}, {r['sha1']})")
        entries = split(body)
        screens_entry, info_entry = only(entries, 0), only(entries, 1)
        screens = decode_screens(entry_data(body, screens_entry))
        info = decode_tile_info(entry_data(body, info_entry))
    except (OSError, ArcError, ValueError) as e:
        raise StageFailed(f"member {i}: {e}") from e
    if not screens:
        raise StageFailed(f"member {i}: screens entry is empty")
    problems = check(screens, info)
    if problems:
        raise StageFailed(f"member {i}: " + "; ".join(str(p) for p in problems))
    folder = tmp / f"m{i:03d}"
    folder.mkdir()
    doc = {
        "schema_version": SCHEMA_VERSION,
        "source": {"member": i, "sha1": r["sha1"],
                   "screens_entry": {"offset": screens_entry.offset, "size": screens_entry.size},
                   "tile_info_entry": {"offset": info_entry.offset, "size": info_entry.size}},
        "tile_count": len(info),
        "tile_info": info,
        "screens": [{"index": n, "tile": _grid(s, tile), "flags": _grid(s, flags),
                     "collision": _grid(s, lambda v: collision(info[tile(v)]))}
                    for n, s in enumerate(screens)],
    }
    (folder / "screens.json").write_text(json.dumps(doc, sort_keys=True, indent=2) + "\n")
    write_png(folder / "sheet.png", *_sheet(screens, info))
    cells = [v for s in screens for v in s]
    empty = sum(1 for v in cells if tile(v) == 0)
    header = (f"member {i}: {len(screens)} screens, {len(info)} tiles, "
              f"max tile {max(tile(v) for v in cells)}, empty cells {empty / len(cells):.0%}")
    counts = Counter(collision(info[tile(v)]) for v in cells)
    lines = [header] + [f"collision {c}: {n} cells #%02x%02x%02x" % COLOURS[c]
                        for c, n in sorted(counts.items(), key=lambda kv: (-kv[1], kv[0]))]
    (folder / "summary.txt").write_text("\n".join(lines) + "\n")
    log(header)
    return header


def run(extracted, out, member=None, expect_stages=EXPECT_STAGES, log=print):
    extracted, out = Path(extracted), Path(out)
    refusal = outdir.check(out, REPO_ROOT, _allowed)
    if refusal:
        log(f"error: {refusal}")
        return 2
    try:
        records = [r for r in load_index(extracted / "index.json")["files"] if r["kind"] == "rock_dat"]
    except (OSError, ValueError, KeyError, TypeError, AttributeError) as e:
        log(f"error: {e}")
        return 2
    try:
        if member is None:
            stages = stage_records(records)
            if len(stages) != expect_stages:
                raise StageFailed(f"found {len(stages)} stage members, expected {expect_stages}")
            chosen = stages
        else:
            chosen = [r for r in map_records(records) if r["index"] == member]
            if not chosen:
                raise StageFailed(f"member {member} is not a stage member (needs entry types 0 and 1)")
        tmp = outdir.fresh_tmp(out)
        headers = [_process(extracted, tmp, r, log) for r in chosen]
        write_png(tmp / "legend.png", *_legend())
        (tmp / "summary.txt").write_text("\n".join(headers + ["done"]) + "\n")
        outdir.publish(tmp, out)
    except (StageFailed, OSError) as e:
        log(f"error: {e}")
        return 1
    log(f"done: {out}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.collision")
    parser.add_argument("--extracted", required=True)
    parser.add_argument("--out", required=True)
    parser.add_argument("--member", type=int)
    parser.add_argument("--expect-stages", type=int, default=EXPECT_STAGES)
    args = parser.parse_args(argv)
    return run(Path(args.extracted).expanduser(), Path(args.out).expanduser(), args.member,
               args.expect_stages, log=print)


if __name__ == "__main__":
    sys.exit(main())
