import sys
sys.path.insert(0, "/home/johnlavender/mmx-data/x6/re/dynamo/scripts")
from probe import *
name = sys.argv[1] if len(sys.argv) > 1 else "mijinion-4_dynamo"
r = load(name)
stage, area = u8(r, G+0xC), u8(r, G+0xD)
idx = stage*2 + area
lst = u32(r, 0x8007349C + 4*idx*2 // 2 * 1) if False else u32(r, 0x8007349C + stage*8 + area*4)
print(f"stage {stage} area {area} idx {idx} enemy list {lst:08x}")
def records(a):
    i = 0
    while True:
        fl, ty, id_, kind = u8(r,a), u8(r,a+1), u8(r,a+2), u8(r,a+3)
        x, y = s16(r,a+4), s16(r,a+6)
        if kind & 0xF == 15:
            print(f"  end at {a:08x}")
            break
        print(f"  {i:2d} @{a:08x} flags {fl:02x} kind {kind:02x} b {ty:3d} id {id_:3d} x {x:5d} y {y:5d}")
        a += 8; i += 1
records(lst)
fl = u32(r, 0x80073554 + stage*8 + area*4)
print(f"second list {fl:08x}")
records(fl)
zw = u32(r, 0x80079D70 + 4*idx)
print(f"camera zone word {zw:08x}")
if zw:
    for k in range(4):
        a = zw + 16*k
        print("  zone", k, [u16(r, a+2*j) for j in range(8)])
cw = u32(r, 0x80073300 + 4*idx)
print(f"checkpoint list {cw:08x}")
for k in range(3):
    p = u32(r, cw + 4*k)
    if not (0x80000000 <= p < 0x80200000): break
    f = [s16(r, p+2*j) for j in range(17)]
    print(f"  cp {k} @{p:08x} player {f[0]},{f[1]} cam {f[2]},{f[3]} l1 {f[4]},{f[5]} l2 {f[6]},{f[7]} lim x {f[8]}-{f[9]} y {f[10]}-{f[11]} facing {f[12]:#x} l1o {f[13]},{f[14]} l2o {f[15]},{f[16]} beam {u8(r,p+0x22)}")
k = u8(r, 0x8007A124 + idx)
print("talk k", k, "box id 4", [s16(r, 0x8007A154 + k*128 + 4*8 + 2*j) for j in range(4)], "talk_seen", hex(u16(r, G+0x9E+2*k)))
print("gate_time", u32(r, 0x800CD30C), "boss_pack_loaded", u8(r, 0x800CC868), "frame_count", u32(r, 0x80097420))
print("bg_mode", [u8(r, 0x800720AC + idx*0 + stage*4 + j) for j in range(4)])
