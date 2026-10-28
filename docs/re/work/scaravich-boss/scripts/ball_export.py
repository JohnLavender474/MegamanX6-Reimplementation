"""Draw the Ground Scaravich ball (visual 14, overlay 08) from a save state.

The ball takes the sprite data of type list bank + 1 (bank 15), the texture of the
boss + 0x100 and its own palette (ScaravichBallInit 0x800F0BD4); its animation table
is 0x800F6234. spriteexport does not export it (bank 15 has no texture-table entry and
a main-type bank is not an extra sprite source). This script draws it from the live
visual 14 object of the state.
Usage: python3 ball_export.py STATE OUT_DIR
"""
import json, struct, sys
from pathlib import Path
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.vram import read_vram
from mmxdemake.sprites import Bank, banks, scratchpad, with_source, draw, SpriteError
from mmxdemake.anims import read_table, walk
from mmxdemake.spriteexport import geometry, strip, sheet
from mmxdemake.png import write_png

D = Path.home() / "mmx-data/x6/dumps"
EXE = (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()
VIS, TABLE, LO, HI = 0x80093C98, 0x800F6234, 0x800E9860, 0x800FA000


def u8(r, a): return r[a & 0x1FFFFF]
def u16(r, a): return struct.unpack_from("<H", r, a & 0x1FFFFF)[0]
def u32(r, a): return struct.unpack_from("<I", r, a & 0x1FFFFF)[0]


def main(state, out):
    out = Path(out); out.mkdir(parents=True, exist_ok=True)
    raw = read_state(D / state)
    ram, vram, sp = ram_from(raw, EXE, D / state), read_vram(raw), scratchpad(raw)
    bl = banks(ram, sp)
    ball = next(VIS + 0xA0 * i for i in range(32) if u8(ram, VIS + 0xA0 * i) and u8(ram, VIS + 0xA0 * i + 1) == 14
                and (u8(ram, VIS + 0xA0 * i + 2) or 0) >= 1)
    data, tex, clut, mode = u32(ram, ball + 0x3C), u16(ram, ball + 0x40), u16(ram, ball + 0x42) & 0x7FFF, u8(ram, ball) & 0x60
    entry = next((b.index for b in bl if b.data == data), None)
    src = with_source(ram, Bank(entry if entry is not None else -1, 50, data, 0, None, clut), data, tex, clut)
    print(f"ball object {ball:#x}: sprite data {data:#x} (bank {entry}), texture {tex:#x}, palette {clut:#x}, mode {mode:#x}, {src.count} sprites")
    write_png(out / f"bank-{entry}-ball-live.png", *sheet(ram, vram, src, print, mode))
    pointers = read_table(ram, TABLE, LO, HI)
    anims = []
    for p in pointers:
        try:
            frames, loop = walk(ram, p)
        except Exception as e:
            print("stop at", hex(p), e); break
        if any(f["sprite"] >= src.count for f in frames):
            print(f"table entry {len(anims)} uses a sprite past {src.count}: end"); break
        anims.append((frames, loop))
    used = sorted({f["sprite"] for fr, _ in anims for f in fr})
    images = {n: draw(ram, vram, src, n, mode) for n in used}
    w, h, ox, oy = geometry(images)
    for k, (frames, loop) in enumerate(anims):
        write_png(out / f"anim-{k}.png", *strip(images, frames, w, h, ox, oy))
    doc = {"name": "scaravich-ball (visual 14, not official)", "table": f"{TABLE:#010x}", "cell": [w, h], "origin": [ox, oy],
           "source": {"state": state, "object": f"{ball:#x}", "sprite_data": f"{data:#x}", "bank": entry,
                      "texture": f"{tex:#06x}", "palette": f"{clut:#06x}", "mode": mode},
           "animations": [{"index": k, "frames": fr, "loop_to": lp} for k, (fr, lp) in enumerate(anims)]}
    (out / "animations.json").write_text(json.dumps(doc, indent=1) + "\n")
    print(f"{len(anims)} animations, {len(used)} sprites, cell {w}x{h} -> {out}")


if __name__ == "__main__":
    main(*sys.argv[1:3])
