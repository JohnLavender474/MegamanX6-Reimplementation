import sys
from probe import *
from mmxdemake.sprites import Bank, draw, SpriteError
from mmxdemake.vram import read_vram
from mmxdemake.savestate import read_state
from mmxdemake.spriteexport import grid
from mmxdemake.png import write_png
out = Path(sys.argv[1]); out.mkdir(parents=True, exist_ok=True)
seen = {}
for s in STATES:
    p = D/s; data = read_state(p); r = ram_from(data, exe, p); vram = read_vram(data)
    objs=[0x800950A0+k*0xA0 for k in range(32) if u8(r,0x800950A0+k*0xA0)]+[0x80091298+k*0x70 for k in range(96) if u8(r,0x80091298+k*0x70)]
    for o in objs:
        src, tex, clut, mode = u32(r,o+0x3C), u16(r,o+0x40), u16(r,o+0x42), u8(r,o) & 0x60
        key=(src,tex,clut,mode)
        if key in seen: continue
        b = Bank(0, u8(r,o+1), src, u16(r,src+2), tex, clut)
        imgs=[]; errs=0
        for n in range(b.count):
            try: imgs.append(draw(r, vram, b, n, mode))
            except SpriteError as e: errs+=1; imgs.append(None)
        good=[i for i in imgs if i is not None and i.width>1]
        name=f"{s.split('_')[0]}_obj{o:08x}_type{u8(r,o+1)}_var{u8(r,o+2)}"
        seen[key]=name
        print(name, "src", hex(src), "tex", hex(tex), "clut", hex(clut), "mode", hex(mode), "sprites", b.count, "drawn", len(good), "errors", errs)
        from mmxdemake.sprites import Image, CLEAR
        imgs=[i if i is not None else Image(1,1,0,0,[[CLEAR]]) for i in imgs]
        w,h,rows=grid(imgs)
        write_png(out/f"{name}.png", w, h, rows)
