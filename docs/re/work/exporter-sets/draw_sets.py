import sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad, banks, entries, clut_word, with_source, Bank
from mmxdemake.vram import read_vram
from mmxdemake.spriteexport import sheet
from mmxdemake.png import write_png
X = Path.home() / "mmx-data/x6/extracted/retail"
D = Path.home() / "mmx-data/x6/dumps"
name, t, d, tex, p = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4], 16), int(sys.argv[5])
path = D / name
data = read_state(path)
ram = ram_from(data, (X / "iso/SLUS_013.95").read_bytes(), path)
scratch, vram = scratchpad(data), read_vram(data)
k = next(b.index for b in banks(ram, scratch) if b.type == t)
dat = entries(ram, scratch)[k + d]
b = with_source(ram, Bank(k + d, t, dat, 0, None, 0), dat, tex, clut_word(k + p))
out = Path(f"out/{name.split('.')[0]}-set{d}.png"); out.parent.mkdir(exist_ok=True)
write_png(out, *sheet(ram, vram, b, print))
print(out, b.count)
