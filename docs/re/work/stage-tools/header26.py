import json, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from, ram_at
ex = Path.home() / "mmx-data/x6/extracted/retail"
dumps = Path.home() / "mmx-data/x6/dumps"
cat = json.loads((Path.home() / "IdeaProjects/mmx-demake-tools/catalog/stages.json").read_text())
exe = (ex / "iso/SLUS_013.95").read_bytes()
seen = set()
for e in cat:
    key = (e["overlay"], e["offset"])
    ov = (ex / f"rock_bin/{e['overlay']:02d}.bin").read_bytes()
    st = dumps / e["save_state"]
    ram = ram_from(read_state(st), exe, st)
    disc = ov[e["offset"] - 26:e["offset"]]
    r = ram_at(ram, 0x8009741E, 26)
    sp = ram_at(ram, 0x1F800000, 0x10)
    print(f"{e['name'][:40]:40s} ov{e['overlay']:02d}@{e['offset']:#x} {e['width']}x{e['height']}")
    print("   disc", disc.hex(" "))
    print("   ram ", r.hex(" "), "same" if r == disc else "DIFF")
    print("   scratch", sp.hex(" "))
