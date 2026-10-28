import sys
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad, Bank, with_source, draw, _u32, _u16
from mmxdemake.vram import read_vram
from mmxdemake.png import write_png
from mmxdemake.spriteexport import grid
import struct
X = Path.home()/"mmx-data/x6/extracted/retail"
p = Path.home()/"mmx-data/x6/dumps/heatnix-3_rekkoha.state.gz"
st = read_state(p); ram = ram_from(st, (X/"iso/SLUS_013.95").read_bytes(), p)
sc = scratchpad(st); vram = read_vram(st)
for off in (0x1C, 0x3C):
    d = struct.unpack_from("<I", sc, off)[0]
    n = _u32(ram, d) // 4
    print(f"scratch+{off:#x} = {d:#x}, {n} entries:", [hex(d + _u32(ram, d + 4*i)) for i in range(min(n, 20))])
data = 0x8013D86C
print("count", _u16(ram, data + 2))
b = with_source(ram, Bank(-1, 17, data, 0, None, 0), data, 0x1E00, 0x7E50)
ims = [draw(ram, vram, b, n, 0) for n in range(80, 88)]
for n, im in zip(range(80, 88), ims): print(n, im.width, im.height, im.ox, im.oy)
write_png(sys.argv[1], *grid(ims))
