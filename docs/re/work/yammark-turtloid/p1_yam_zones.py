from probe_ram import *
ram, scr = load("yammark-1_start")
print("stage,area", u8(ram,0x800CCEDC), u8(ram,0x800CCEDD))
tbl = u32(ram, 0x80079D70 + (1*2+0)*4)
print("zone table", hex(tbl))
for i in range(26):
    e = tbl + i*16
    v = [u16(ram, e+2*k) for k in range(8)]
    print(i, v[:4], [x if x != 0xFFFF else '-' for x in v[4:]])
print("stage logic class table +0x8C", hex(sp32(scr,0x8C)), "+0x94", hex(sp32(scr,0x94)), "+0x90", hex(sp32(scr,0x90)), "+0x80", hex(sp32(scr,0x80)))
t = sp32(scr,0x8C)
for i in range(45):
    print("SL", i, hex(u32(ram, t+4*i)))
fl = u32(ram, 0x80073554 + 8)
print("fixed list", hex(fl))
a = fl
while True:
    r = ram[a&0x1FFFFF:(a&0x1FFFFF)+8]
    fa,b,id_,kind = r[0],r[1],r[2],r[3]; x,y = struct.unpack_from("<hh", r, 4)
    print(hex(a), "flags", hex(fa), "b", b, "id", id_, "kind", hex(kind), x, y)
    if kind & 0xF == 15: break
    a += 8
