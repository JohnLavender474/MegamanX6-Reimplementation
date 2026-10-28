import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad, Bank, with_source, draw, _u32
from mmxdemake.vram import read_vram
from mmxdemake.png import write_png
from mmxdemake.rekit import animation_list
from mmxdemake.fingerprint import content_id
from mmxdemake.spriteexport import geometry, strip, sprite_grid, _blit, FPS

STATE = "heatnix-3_rekkoha.state.gz"
X = Path.home() / "mmx-data/x6/extracted/retail"
P = Path.home() / "mmx-data/x6/dumps" / STATE
ROOT = Path.home() / "mmx-data/x6/re/zero-rekkoha/sprites-root/enemies"

TABLE = 0x8007798C
BEAM_ANIMS = [11, 12]
BEAM_MODE = 0x20
BACKDROP_SPRITES = [83, 84]

st = read_state(P)
ram = ram_from(st, (X / "iso/SLUS_013.95").read_bytes(), P)
sc = scratchpad(st)
vram = read_vram(st)
import hashlib
sha1 = hashlib.sha1(P.read_bytes()).hexdigest()

shots = struct.unpack_from("<I", sc, 0x1C)[0]
beam_data = shots + _u32(ram, shots + 4 * 15)
assert beam_data == 0x801474A4, hex(beam_data)
beam = with_source(ram, Bank(-1, 44, beam_data, 0, None, 0), beam_data, 0, 0x7802)

an, err = animation_list(ram, TABLE, [(0x80010000, 0x80090000)], None, None, 60)
anims = []
for k in BEAM_ANIMS:
    frames, loop = an[k]
    anims.append(([{"sprite": f["sprite"], "ticks": f["ticks"], "flag": f["flag"]} for f in frames], loop))
used = sorted({f["sprite"] for fr, _ in anims for f in fr})
frames = {n: draw(ram, vram, beam, n, BEAM_MODE) for n in used}
cid = content_id(ram, vram, beam, anims, BEAM_MODE)
w, h, ox, oy = geometry(frames)
out = ROOT / "rekkoha-beam"
out.mkdir(parents=True, exist_ok=True)
for k, (fr, _) in enumerate(anims):
    write_png(out / f"anim-{k}.png", *strip(frames, fr, w, h, ox, oy))
src = {"area": 0, "bank": None, "class": "weapon", "found": "live", "sprites": "live", "stage": 3,
       "state": STATE, "state_sha1": sha1, "table": f"{TABLE:#010x}", "type": 44, "entry": 15,
       "table_animations": BEAM_ANIMS, "also": "effect type 26 (0x801EE254) uses table animation 12"}
doc = {"fps": FPS, "id": cid, "cell": [w, h], "origin": [ox, oy],
       "animations": [{"index": k, "frames": fr, "loop_to": loop} for k, (fr, loop) in enumerate(anims)],
       "sources": [src]}
(out / "animations.json").write_text(json.dumps(doc, indent=2, sort_keys=True) + "\n")
print(out, cid, w, h)

fx = struct.unpack_from("<I", sc, 0x3C)[0]
bd_data = fx + _u32(ram, fx)
assert bd_data == 0x8013D86C, hex(bd_data)
bd = with_source(ram, Bank(-1, 17, bd_data, 0, None, 0), bd_data, 0x1E00, 0x7E50)
bframes = {n: draw(ram, vram, bd, n, 0) for n in BACKDROP_SPRITES}
bid = content_id(ram, vram, bd, [([{"sprite": n, "ticks": 1, "flag": 0} for n in BACKDROP_SPRITES], None)], 0)
out = ROOT / "rekkoha-backdrop"
out.mkdir(parents=True, exist_ok=True)
w, h, ox, oy = geometry(bframes)
write_png(out / "sprites.png", *sprite_grid(bframes, 2, w, h, ox, oy))
screen = [bytearray(320 * 4) for _ in range(320)]
for row in range(5):
    for n, x in ((83, 128), (84, 384)):
        im = bframes[n]
        sub = [bytearray(320 * 4) for _ in range(64)]
        y = 64 * row + 64
        for yy, line in enumerate(im.pixels):
            for xx, p in enumerate(line):
                sx, sy = x - im.ox + xx, y - im.oy + yy
                if 0 <= sx < 320 and 0 <= sy < 320 and p[3]:
                    screen[sy][sx * 4:sx * 4 + 4] = bytes(p)
write_png(out / "screen.png", 320, 320, [bytes(r) for r in screen])
doc = {"id": bid, "sheet": {"file": "sprites.png", "columns": 2, "sprites": 2},
       "cell": [w, h], "origin": [ox, oy], "animations": [],
       "sources": [{"area": 0, "class": "effect", "type": 17, "found": "live", "sprites": "live", "stage": 3,
                    "state": STATE, "state_sha1": sha1, "data": f"{bd_data:#010x}", "scratch_dir": "0x3c entry 0",
                    "texture": "0x1e00", "palette": "0x7e50", "sprites_used": BACKDROP_SPRITES,
                    "note": "sprite-only; screen.png = the 2 x 5 grid at y 64..320 (effect var 0 scrolls it up 8 px per update)"}]}
(out / "animations.json").write_text(json.dumps(doc, indent=2, sort_keys=True) + "\n")
print(out, bid)
