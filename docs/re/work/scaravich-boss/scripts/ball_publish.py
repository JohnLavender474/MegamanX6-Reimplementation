"""Copy the probe export of the Scaravich ball into a spriteexport root, in the tool's format.

Adds the content ID (mmxdemake.fingerprint.content_id, the same function and inputs that
spriteexport uses), fps and a sources list, so that spriteexport accepts the folder.
Usage: python3 ball_publish.py STATE PROBE_DIR ROOT
"""
import hashlib, json, shutil, struct, sys
from pathlib import Path
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.vram import read_vram
from mmxdemake.sprites import Bank, banks, scratchpad, with_source
from mmxdemake.anims import FPS, walk, read_table
from mmxdemake.fingerprint import content_id

D = Path.home() / "mmx-data/x6/dumps"
EXE = (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()
VIS, TABLE, LO, HI = 0x80093C98, 0x800F6234, 0x800E9860, 0x800FA000


def main(state, probe, root):
    probe, target = Path(probe), Path(root) / "enemies" / "scaravich-ball"
    raw = read_state(D / state)
    ram, vram, sp = ram_from(raw, EXE, D / state), read_vram(raw), scratchpad(raw)
    u8 = lambda a: ram[a & 0x1FFFFF]
    u16 = lambda a: struct.unpack_from("<H", ram, a & 0x1FFFFF)[0]
    u32 = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
    ball = next(VIS + 0xA0 * i for i in range(32) if u8(VIS + 0xA0 * i) and u8(VIS + 0xA0 * i + 1) == 14
                and u8(VIS + 0xA0 * i + 2) >= 1)
    data, tex, clut, mode = u32(ball + 0x3C), u16(ball + 0x40), u16(ball + 0x42) & 0x7FFF, u8(ball) & 0x60
    entry = next((b.index for b in banks(ram, sp) if b.data == data), None)
    src = with_source(ram, Bank(entry if entry is not None else -1, 50, data, 0, None, clut), data, tex, clut)
    doc = json.loads((probe / "animations.json").read_text())
    anims = [(a["frames"], a["loop_to"]) for a in doc["animations"]]
    pointers = read_table(ram, TABLE, LO, HI)
    if [walk(ram, p) for p in pointers[:len(anims)]] != anims:
        sys.exit("the probe animations do not match the table in this state")
    cid = content_id(ram, vram, src, anims, mode)
    out = {"id": cid, "fps": FPS, "cell": doc["cell"], "origin": doc["origin"],
           "animations": doc["animations"],
           "sources": [{"class": "visual", "stage": 5, "area": 0, "type": 14, "bank": entry,
                        "sprites": "live", "found": "live", "table": f"{TABLE:#010x}",
                        "texture": f"{tex:#06x}", "palette": f"{clut:#06x}", "state": state,
                        "state_sha1": hashlib.sha1((D / state).read_bytes()).hexdigest(),
                        "made_by": "re/scaravich-boss/scripts/ball_export.py (spriteexport does not export it)"}]}
    if target.exists():
        sys.exit(f"{target} exists")
    shutil.copytree(probe, target)
    (target / "animations.json").write_text(json.dumps(out, indent=2, sort_keys=True) + "\n")
    print(cid, target)


if __name__ == "__main__":
    main(*sys.argv[1:])
