import json, sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from, ram_at
from mmxdemake.sprites import stage_area
ex = Path.home() / "mmx-data/x6/extracted/retail"
dumps = Path.home() / "mmx-data/x6/dumps"
cat = json.loads((Path.home() / "IdeaProjects/mmx-demake-tools/catalog/stages.json").read_text())
exe = (ex / "iso/SLUS_013.95").read_bytes()
for e in cat:
    st = dumps / e["save_state"]
    ram = ram_from(read_state(st), exe, st)
    stage, area = stage_area(ram)
    p = struct.unpack("<I", ram_at(ram, 0x800733C4 + 4 * (stage * 2 + area), 4))[0]
    base = 0x800E9860 + e["offset"] - 26
    tl = ram_at(ram, p, 24) if p else b""
    print(e["name"][:36], "stage", stage, area, f"ptr {p:#x}", tl.hex(" "))
    print(f"   layout-26 @ {base:#x}, type list at +{p-base}")
