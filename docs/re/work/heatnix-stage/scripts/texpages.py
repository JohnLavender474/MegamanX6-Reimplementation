"""For each save state, compare every 4 bpp texture page of the two texture blocks of
DAT member 97 (entry 0x10000 and entry 0x16) with VRAM, at the position where the
0x10000 block was found. Usage (repo root): python3 texpages.py <state.gz> ..."""
import sys
from pathlib import Path
from mmxdemake.arcentries import split
from mmxdemake.learn import texture_block
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram, find_textures, PAGES, PAGE_W, PAGE_ROWS, ROW, PAGE_BYTES, TEX_ROW, VRAM_H, VRAM_W

DUMPS = Path.home() / "mmx-data/x6/dumps"
body = (Path.home() / "mmx-data/x6/extracted/retail/rock_dat/097.bin").read_bytes()
entries = split(body)
blocks = {k: texture_block(body, entries, k) for k in (0x10000, 0x16)}


def page_rows(vram, block, p, x0, y0):
    same = 0
    for r in range(PAGE_ROWS):
        o = (y0 + r) * ROW + (x0 + p * PAGE_W) * 2
        s = p * PAGE_BYTES + r * TEX_ROW
        same += vram[o:o + TEX_ROW] == block[s:s + TEX_ROW]
    return same


for name in sys.argv[1:]:
    vram = read_vram(read_state(DUMPS / name))
    print(name)
    for y0 in (0, VRAM_H - PAGE_ROWS):
        for x0 in range(0, VRAM_W - PAGES * PAGE_W + 1, PAGE_W):
            res = {k: [page_rows(vram, b, p, x0, y0) for p in range(PAGES)] for k, b in blocks.items()}
            if max(max(v) for v in res.values()) >= 200:
                print(f"  VRAM ({x0}, {y0}):", {f"{k:#x}": v for k, v in res.items()})
