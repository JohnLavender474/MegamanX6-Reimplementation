import sys
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import banks, scratchpad, stage_area, type_list

dumps = Path.home() / "mmx-data/x6/dumps"
exe = (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()
for p in sorted(dumps.glob("*.state.gz")):
    data = read_state(p)
    ram = ram_from(data, exe, p)
    try:
        bl = banks(ram, scratchpad(data))
    except Exception as e:
        print(p.name, "error", e)
        continue
    tl = type_list(ram)
    twice = {t for t in tl if tl.count(t) > 1}
    if twice:
        print(p.name, stage_area(ram), [(b.index, b.type, b.count, b.texture) for b in bl if b.type in twice],
              "listlen", len(tl), "entries", len(bl))
