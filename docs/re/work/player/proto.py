import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
sys.path.insert(0, "/home/johnlavender/mmx-data/x6/probe/sprites")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.vram import read_vram
from mmxdemake.sprites import Bank, draw
from mmxdemake.anims import walk, AnimError
from mmxdemake.spriteexport import sheet
from mmxdemake.png import write_png
from wordlz import decode
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
M = 0x800970A0
s = sys.argv[1]
p = Path.home()/"mmx-data/x6/dumps"/s
data = read_state(p); r = ram_from(data, exe, p); v = bytearray(read_vram(data))
u32 = lambda a: struct.unpack_from("<I", r, a - 0x80000000)[0]
u16 = lambda a: struct.unpack_from("<H", r, a - 0x80000000)[0]
ft, sd, at = u32(M+0x38), u32(M+0x3C), u32(M+0x30)
tex, clut = u16(M+0x40), u16(M+0x42)
bank = Bank(-1, -1, sd, u16(sd+2), tex >> 0, clut)
print("texture field", hex(tex), "clut", hex(clut), "count", bank.count)
class V(bytearray): pass
def with_frame(n):
    e = u32(ft + 4*n); cnt, off = e >> 20, e & 0xFFFFF
    out, _ = decode(r, ft - 0x80000000 + off)
    w = bytearray(v)
    for k in range(0, cnt, 16):
        part = out[k*128:(k+16)*128]; width = 64 if cnt - k >= 16 else (cnt - k)*4
        for row in range(16):
            o = ((k//16)*16 + row)*2048 + 320*2
            w[o:o+width*2] = part[row*width*2:(row+1)*width*2]
    return bytes(w)
import mmxdemake.sprites as S
imgs = []
for n in range(bank.count):
    imgs.append(draw(r, with_frame(n), bank, n, 0))
from mmxdemake.sprites import Image
class Fake: pass
# simple contact sheet
cols = 16; cw = max(i.width for i in imgs)+4; ch = max(i.height for i in imgs)+4
rows = -(-len(imgs)//cols)
canvas = [bytearray(bytes((40,40,60,255))*cw*cols) for _ in range(ch*rows)]
for n, im in enumerate(imgs):
    x0, y0 = (n%cols)*cw+2, (n//cols)*ch+2
    for y, line in enumerate(im.pixels):
        for x, px in enumerate(line):
            if px[3]: canvas[y0+y][(x0+x)*4:(x0+x)*4+4] = bytes(px)
write_png(Path(sys.argv[2]), cw*cols, ch*rows, [bytes(c) for c in canvas])
ptrs = []
while True:
    w_ = u32(at + 4*len(ptrs))
    if not (0x801EA000 <= w_ < at): break
    ptrs.append(w_)
bad = []
for k, q in enumerate(ptrs):
    try:
        fr, loop = walk(r, q)
        if max(f["sprite"] for f in fr) >= bank.count: bad.append((k, "sprite range"))
    except AnimError as e: bad.append((k, str(e)))
print(len(ptrs), "animations, bad:", bad[:10])
