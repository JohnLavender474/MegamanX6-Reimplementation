import sys
from probe_ram import *
st = sys.argv[1]; ids = [int(x) for x in sys.argv[2].split(",")]
ram, scr = load(st)
stage, area = u8(ram,0x800CCEDC), u8(ram,0x800CCEDD)
idx = stage*2 + area
scripts = u32(ram, 0x8007A01C + idx*4); dsts = u32(ram, 0x80079F64 + idx*4)
print(st, "stage", stage, "area", area, "scripts", hex(scripts), "dsts", hex(dsts))
for i in ids:
    sp = u32(ram, scripts + 4*i)
    dst, cnt = u8(ram, dsts + 2*i), u8(ram, dsts + 2*i + 1)
    ent = []
    p = sp
    for k in range(40):
        if sp == 0: break
        s, d = u8(ram, p), s8(ram, p+1)
        ent.append((s, d))
        if d <= 0: break
        p += 2
    tot = sum(d for s, d in ent if d > 0)
    print(f" id {i}: script {sp:#x} dst palette {dst} count {cnt}  entries {ent}  loop length {tot}")
# live stage-logic objects of type 3
for s in range(96):
    b = 0x8009B7B0 + s*0x30
    if u8(ram,b) and u8(ram,b+1) == 3:
        print("  live slot", s, "flags", hex(u8(ram,b)), "id", u8(ram,b+2), "state", u8(ram,b+4), "timer", u8(ram,b+0x21), "count", u8(ram,b+0x20), "script", hex(u32(ram,b+0x1C)))
