import argparse
import hashlib
import re
import sys
from collections import Counter
from math import ceil
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.arcentries import ArcError, entry_data, only, split
from mmxdemake.collision import map_records, stage_records
from mmxdemake.index import load_index
from mmxdemake.png import write_png
from mmxdemake.shade import grey
from mmxdemake.stagemap import SIDE, check, decode_screens, decode_tile_info, tile
from mmxdemake.texture import BLOCK_SIZE, TILE, bits, covered_4bpp_cells, raw_tile, tile_ref

REPO_ROOT = Path(__file__).resolve().parents[1]
EXPECT_STAGES = 23
TEXTURE_TYPE = 0x10000
SCREEN_PX = SIDE * TILE
SCREENS_PER_ROW = 8
SCREEN_GAP = 4
PAGE_GAP = 2
PAGE_COLUMNS = 4
PAGES = 12
GAP_RGB = (60, 60, 60)
UNUSED_RGB = (32, 0, 32)
BLACK_RGB = (0, 0, 0)


class TilesFailed(Exception):
    pass


def _allowed(name):
    return name == "summary.txt" or re.fullmatch(r"m\d{3}", name) is not None


def _canvas(width, height, rgb):
    return [bytearray(bytes(rgb + (255,)) * width) for _ in range(height)]


def _fill(rgb):
    return bytes(rgb + (255,)) * (TILE * TILE)


def _blit(rows, x, y, rgba):
    stride = TILE * 4
    for r in range(TILE):
        rows[y + r][x * 4:x * 4 + stride] = rgba[r * stride:(r + 1) * stride]


def _shaded(block, ref, shade):
    values = raw_tile(block, ref)
    return None if values is None else shade(values, bits(ref.page), ref.palette)


def _screens_png(screens, info, block, shade):
    step = SCREEN_PX + SCREEN_GAP
    width = min(SCREENS_PER_ROW, len(screens)) * step + SCREEN_GAP
    height = ceil(len(screens) / SCREENS_PER_ROW) * step + SCREEN_GAP
    rows = _canvas(width, height, GAP_RGB)
    black = _fill(BLACK_RGB)
    cache = {}
    for n, screen in enumerate(screens):
        ox = (n % SCREENS_PER_ROW) * step + SCREEN_GAP
        oy = (n // SCREENS_PER_ROW) * step + SCREEN_GAP
        for k, v in enumerate(screen):
            t = tile(v)
            if t and t not in cache:
                cache[t] = _shaded(block, tile_ref(info[t]), shade)
            rgba = cache.get(t) if t else None
            _blit(rows, ox + (k % SIDE) * TILE, oy + (k // SIDE) * TILE, rgba or black)
    return width, height, [bytes(r) for r in rows]


def _pages_png(refs, block, shade):
    step = SCREEN_PX + PAGE_GAP
    width = PAGE_COLUMNS * step + PAGE_GAP
    height = ceil(PAGES / PAGE_COLUMNS) * step + PAGE_GAP
    rows = _canvas(width, height, GAP_RGB)
    unused = _fill(UNUSED_RGB)
    origin = {p: ((p % PAGE_COLUMNS) * step + PAGE_GAP, (p // PAGE_COLUMNS) * step + PAGE_GAP) for p in range(PAGES)}
    for p, (ox, oy) in origin.items():
        for cy in range(SIDE):
            for cx in range(SIDE):
                _blit(rows, ox + cx * TILE, oy + cy * TILE, unused)
    cells = {}
    for ref in refs:
        if bits(ref.page) is not None:
            cells.setdefault((ref.page, ref.cx, ref.cy), ref)
    for (p, cx, cy), ref in sorted(cells.items()):
        ox, oy = origin[p]
        _blit(rows, ox + cx * TILE, oy + cy * TILE, _shaded(block, ref, shade))
    return width, height, [bytes(r) for r in rows]


def _summary(i, screens, refs_by_tile):
    refs = list(refs_by_tile.values())
    lines = [f"member {i}: {len(screens)} screens, {len(refs)} tiles used"]
    per_page = Counter(r.page for r in refs)
    for p in sorted(per_page):
        b = bits(p)
        if b is None:
            lines.append(f"page {p} (not drawn): {per_page[p]} tiles")
            continue
        label = "4bpp" if b == 4 else "8bpp, unverified"
        cell_count = len({(r.cx, r.cy) for r in refs if r.page == p})
        lines.append(f"page {p} ({label}): {per_page[p]} tiles, {cell_count} cells")
    cells4 = {(r.page, r.cx, r.cy) for r in refs if bits(r.page) == 4}
    covered = set()
    for r in refs:
        covered |= covered_4bpp_cells(r)
    lines.append(f"4bpp cells also read by 8bpp tiles: {len(cells4 & covered)}")
    return lines


def _process(extracted, tmp, r, shade, log):
    i = r["index"]
    try:
        body = (extracted / r["path"]).read_bytes()
        sha1 = hashlib.sha1(body).hexdigest()
        if len(body) != r["size"] or sha1 != r["sha1"]:
            raise TilesFailed(f"member {i}: size {len(body)}, sha1 {sha1} do not match the index "
                              f"({r['size']}, {r['sha1']})")
        entries = split(body)
        screens_entry, info_entry, texture_entry = only(entries, 0), only(entries, 1), only(entries, TEXTURE_TYPE)
        screens = decode_screens(entry_data(body, screens_entry))
        info = decode_tile_info(entry_data(body, info_entry))
        block = entry_data(body, texture_entry)
        if len(block) != BLOCK_SIZE:
            raise ValueError(f"texture block is {len(block)} bytes, need {BLOCK_SIZE}")
    except (OSError, ArcError, ValueError) as e:
        raise TilesFailed(f"member {i}: {e}") from e
    if not screens:
        raise TilesFailed(f"member {i}: screens entry is empty")
    problems = check(screens, info)
    if problems:
        raise TilesFailed(f"member {i}: " + "; ".join(str(p) for p in problems))
    used = sorted({tile(v) for s in screens for v in s} - {0})
    refs_by_tile = {t: tile_ref(info[t]) for t in used}
    folder = tmp / f"m{i:03d}"
    folder.mkdir()
    write_png(folder / "screens.png", *_screens_png(screens, info, block, shade))
    write_png(folder / "pages.png", *_pages_png(refs_by_tile.values(), block, shade))
    lines = _summary(i, screens, refs_by_tile)
    (folder / "summary.txt").write_text("\n".join(lines) + "\n")
    log(lines[0])
    return lines[0]


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
            chosen = stage_records(records)
            if len(chosen) != expect_stages:
                raise TilesFailed(f"found {len(chosen)} stage members, expected {expect_stages}")
        else:
            chosen = [r for r in map_records(records) if r["index"] == member]
            if not chosen:
                raise TilesFailed(f"member {member} is not a screen set (needs entry types 0 and 1)")
        tmp = outdir.fresh_tmp(out)
        headers = [_process(extracted, tmp, r, grey, log) for r in chosen]
        (tmp / "summary.txt").write_text("\n".join(headers + ["done"]) + "\n")
        outdir.publish(tmp, out)
    except (TilesFailed, OSError) as e:
        log(f"error: {e}")
        return 1
    log(f"done: {out}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.tiles")
    parser.add_argument("--extracted", required=True)
    parser.add_argument("--out", required=True)
    parser.add_argument("--member", type=int)
    parser.add_argument("--expect-stages", type=int, default=EXPECT_STAGES)
    args = parser.parse_args(argv)
    return run(Path(args.extracted).expanduser(), Path(args.out).expanduser(), args.member,
               args.expect_stages, log=print)


if __name__ == "__main__":
    sys.exit(main())
