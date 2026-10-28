import sys; sys.path.insert(0, sys.argv[0].rsplit('/',1)[0])
from peek import *
from pathlib import Path
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram
from mmxdemake import sprites as S
from mmxdemake.spriteexport import grid
from mmxdemake.png import write_png
p = Path('/home/johnlavender/mmx-data/x6/dumps') / STATE
st = read_state(p); r = load(); vram = read_vram(st); scr = S.scratchpad(st)
bl = S.banks(r, scr)
b14 = bl[14]; b15 = bl[15]
print(b14, b15)
live = 0x8008efe4
print('live tex', hex(u16(r, live+0x40)), 'b14 texture', b14.texture)
# kind 5/6: texture 0x4A0, clut built from bank 14
clut = (((14-1)*4+0x20) % 16) | ((((14+7)//4) + 0x1E0) << 6)
tex = 0x4A0
ratio = u16(r, live+0x40) / b14.texture if b14.texture else None
print('ratio', ratio)
src = S.with_source(r, b15, b15.data, tex, clut)
ims = []
for n in range(src.count):
    try: ims.append(S.draw(r, vram, src, n))
    except S.SpriteError as e: print(n, e); ims.append(S.Image(1,1,0,0,[[S.CLEAR]]))
w, h, rows = grid(ims)
out = sys.argv[1]
write_png(out, w, h, rows)
print(out, w, h, src.count)
