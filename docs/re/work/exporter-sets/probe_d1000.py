import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad, banks, entries, clut_word
from mmxdemake.animfind import code_regions, live_sources, live_tables, MAIN_OBJECTS
from mmxdemake.spriteexport import EXTRA_CLASSES
X = Path.home() / "mmx-data/x6/extracted/retail"
D = Path.home() / "mmx-data/x6/dumps"
exe = (X / "iso/SLUS_013.95").read_bytes()
overlays = {int(p.stem): p.read_bytes() for p in sorted((X / "rock_bin").glob("*.bin"))}
path = D / sys.argv[1]
data = read_state(path)
ram = ram_from(data, exe, path)
scratch = scratchpad(data)
regions = code_regions(ram, overlays, exe)
print("regions", [(hex(a), hex(b)) for a, b in regions])
bl = banks(ram, scratch)
ent = entries(ram, scratch)
for i, e in enumerate(ent):
    b = bl[i] if i < len(bl) else None
    print(i, hex(e), struct.unpack_from("<H", ram, (e & 0x1FFFFF) + 2)[0], b and (b.type, b.texture, hex(b.clut)))
u32 = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
for cls in (MAIN_OBJECTS,) + EXTRA_CLASSES:
    for s in range(cls.slots):
        a = cls.base + s * cls.size
        f, t = ram[a & 0x1FFFFF], ram[(a & 0x1FFFFF) + 1]
        if not f: continue
        dat = u32(a + 0x3C); tex, clut = struct.unpack_from("<HH", ram, (a & 0x1FFFFF) + 0x40)
        if int(sys.argv[2]) in (t,) or dat in ent[15:]:
            print(cls.name, s, hex(a), f"flags {f:#x} type {t} var {ram[(a&0x1FFFFF)+2]} st {ram[(a&0x1FFFFF)+4:(a&0x1FFFFF)+8].hex()} table {u32(a+0x30):#x} data {dat:#x} entry {ent.index(dat) if dat in ent else None} tex {tex:#x} clut {clut:#x} parent {u32(a+0x24):#x} {u32(a+0x28):#x}")
