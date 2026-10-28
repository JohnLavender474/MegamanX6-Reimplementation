from probe import *
M=0x800970A0
for s in STATES:
    r=load(s)
    print(s, "mega x", u16(r,M+0xA), "y", u16(r,M+0xE), "flip", hex(u8(r,M+0x15)), "anim", u8(r,M+0x17), "sprite", u8(r,M+0x47), "frame_count", u32(r,0x80097420))
    objs=[("wpn",0x800950A0+k*0xA0) for k in range(32) if u8(r,0x800950A0+k*0xA0)]+[("eff",0x80091298+k*0x70) for k in range(96) if u8(r,0x80091298+k*0x70)]
    for kind,o in objs:
        print(f"  {kind} {o:#x} flags {u8(r,o):#x} type {u8(r,o+1)} var {u8(r,o+2)} disp {u8(r,o+3)} state {u8(r,o+4)} step {u8(r,o+5)} x {u16(r,o+0xA)}+{u16(r,o+8)/65536:.3f} y {u16(r,o+0xE)} flip {u8(r,o+0x15):#x} anim {u8(r,o+0x17)} spd_x {s32(r,o+0x20)/65536} spd_y {s32(r,o+0x24)/65536} frame {bytes_at(r,o+0x44,4).hex(' ')} atk {u32(r,o+0x50):#x} ter {u32(r,o+0x68) if kind=='wpn' else 0:#x} +5C {u8(r,o+0x5c)} +64 {u8(r,o+0x64)} +70 {u8(r,o+0x70)} " + (f"+7C {u32(r,o+0x7C):#x} +84 {u32(r,o+0x84)} +98 {s8(r,o+0x98)}" if kind=='wpn' else f"+50 {u32(r,o+0x50):#x}"))
