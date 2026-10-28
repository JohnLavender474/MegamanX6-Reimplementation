import sys
from probe import *
from mmxdemake.sprites import Bank, draw, SpriteError, Image, CLEAR
from mmxdemake.vram import read_vram
from mmxdemake.savestate import read_state
from mmxdemake.spriteexport import strip, geometry, grid
from mmxdemake.png import write_png
out = Path(sys.argv[1]); out.mkdir(parents=True, exist_ok=True)
jobs = [(STATES[0], 0x800950A0, [(21, "lemon"), (39, "lemon-flash")]),
        (STATES[1], 0x800950A0, [(26, "half-charge"), (22, "half-charge-flash")]),
        (STATES[2], 0x80091298, [(25, "full-charge-flash-falcon"), (23, "full-charge-flash-18"), (27, "full-charge-18")])]
def walk(r, tbl, n):
    p=u32(r,tbl+4*n); seq=[]; i=0
    while i<64:
        d,f,nx,sp=struct.unpack_from("<BBbB", r, (p+4*i)&0x1FFFFF)
        seq.append({"sprite":sp,"ticks":d})
        if nx<=0: break
        i+=nx
    return seq
for s,o,anims in jobs:
    p=D/s; data=read_state(p); r=ram_from(data,exe,p); vram=read_vram(data)
    src=u32(r,o+0x3C); b=Bank(0,u8(r,o+1),src,u16(r,src+2),u16(r,o+0x40),u16(r,o+0x42)); mode=u8(r,o)&0x60
    for n,name in anims:
        seq=walk(r,0x8007789c,n)
        frames={}
        for f in seq:
            if f["sprite"] not in frames:
                frames[f["sprite"]]=draw(r,vram,b,f["sprite"],mode)
        w,h,ox,oy=geometry(frames)
        W,H,rows=strip(frames,seq,w,h,ox,oy)
        write_png(out/f"anim-{n}-{name}.png",W,H,rows)
        print(name, n, [(f["sprite"],f["ticks"]) for f in seq], "size", w, h)
    imgs=[draw(r,vram,b,k,mode) for k in range(b.count)]
    W,H,rows=grid(imgs); write_png(out/f"bank-{src:08x}.png",W,H,rows)
