"""Tile info bits and CLUT semi-transparency (bit 15) of the cells that one BG layer shows in a
save state view. Usage (from the repo root): python3 layer_view.py <state.gz> <layer> <x> <y>
[w h]. Needs catalog entry "Blaze Heatnix, second texture" (member 97, texture 0x16)."""
import json, struct, sys
from collections import Counter
from pathlib import Path
from mmxdemake.arcentries import entry_data, only, split
from mmxdemake.index import load_index
from mmxdemake.learn import texture_block
from mmxdemake.savestate import read_state
from mmxdemake.stagemap import decode_screens, decode_tile_info, tile, SIDE
from mmxdemake.texture import tile_ref, raw_tile, bits
from mmxdemake.vram import read_vram, ROW
from mmxdemake.stages import _layers

EX = Path.home() / "mmx-data/x6/extracted/retail"
state, layer, x0, y0 = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4])
w, h = (int(sys.argv[5]), int(sys.argv[6])) if len(sys.argv) > 6 else (320, 240)
entry = next(e for e in json.loads(Path("catalog/stages.json").read_text()) if e["name"] == "Blaze Heatnix, second texture")
rec = next(r for r in load_index(EX / "index.json")["files"] if r["kind"] == "rock_dat" and r["index"] == entry["member"])
body = (EX / rec["path"]).read_bytes(); ents = split(body)
screens = decode_screens(entry_data(body, only(ents, 0))); info = decode_tile_info(entry_data(body, only(ents, 1)))
block = texture_block(body, ents, entry.get("texture", 0x10000))
lay = _layers(entry, (EX / f"rock_bin/{entry['overlay']:02d}.bin").read_bytes(), len(screens))[layer]
vram = read_vram(read_state(state)); rule = entry["palettes"]
hi, cellflags, stp, opaque = Counter(), Counter(), Counter(), 0
for cy in range(y0 // 16, (y0 + h + 15) // 16):
    for cx in range(x0 // 16, (x0 + w + 15) // 16):
        s = lay[cy // SIDE][cx // SIDE] if cy // SIDE < len(lay) and cx // SIDE < len(lay[0]) else 0
        if not s: continue
        v = screens[s][(cy % SIDE) * SIDE + cx % SIDE]
        t = tile(v)
        if not t: continue
        cellflags[v >> 14] += 1
        wd = info[t]; hi[(wd >> 28, (wd >> 6) & 3)] += 1
        ref = tile_ref(wd); vals = raw_tile(block, ref)
        if vals is None: continue
        n = 16 if bits(ref.page) == 4 else 256
        o = (rule["clut_y"] + ref.palette // 16) * ROW + (rule["clut_x"] + (ref.palette % 16) * 16) * 2
        clut = struct.unpack_from(f"<{n}H", vram, o)
        for val in vals:
            c = clut[val]
            if c == 0: continue
            stp[bool(c & 0x8000)] += 1
print("cell flag bits (v >> 14):", dict(cellflags))
print("tile info (bits 28-31, bits 6-7):", dict(hi))
print("non-zero pixels by CLUT bit 15:", dict(stp))
