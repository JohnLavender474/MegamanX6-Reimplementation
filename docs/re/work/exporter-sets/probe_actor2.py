import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad, banks, entries
from mmxdemake.animfind import code_regions
from mmxdemake.anims import read_table
X = Path.home() / "mmx-data/x6/extracted/retail"
D = Path.home() / "mmx-data/x6/dumps"
exe = (X / "iso/SLUS_013.95").read_bytes()
overlays = {int(p.stem): p.read_bytes() for p in sorted((X / "rock_bin").glob("*.bin"))}
for path in sorted(D.glob("*.state.gz")):
    data = read_state(path)
    ram = ram_from(data, exe, path)
    scratch = scratchpad(data)
    regions = code_regions(ram, overlays, exe)
    a = 0x800CD340 & 0x1FFFFF
    head = ram[a:a + 8]
    table, = struct.unpack_from("<I", ram, a + 0x30)
    dat, = struct.unpack_from("<I", ram, a + 0x3C)
    tex, clut = struct.unpack_from("<HH", ram, a + 0x40)
    ent = entries(ram, scratch)
    try: bl = banks(ram, scratch)
    except Exception: bl = []
    idx = ent.index(dat) if dat in ent else None
    n = None
    reg = next(((lo, hi) for lo, hi in regions if lo <= table < hi), None)
    if reg:
        try: n = len(read_table(ram, table, *reg))
        except Exception as e: n = str(e)[:40]
    print(path.name, head.hex(), f"table {table:#x} data {dat:#x} tex {tex:#x} clut {clut:#x} entry {idx}",
          (bl[idx].type, bl[idx].texture) if idx is not None and idx < len(bl) else "", "anims", n)
