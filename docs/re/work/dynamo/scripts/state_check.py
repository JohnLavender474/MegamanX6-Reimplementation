import sys
sys.path.insert(0, "/home/johnlavender/mmx-data/x6/re/dynamo/scripts")
from probe import *
name = sys.argv[1] if len(sys.argv) > 1 else "mijinion-4_dynamo"
r = load(name)
report(name)
ov = (X/"rock_bin/45.bin").read_bytes()
base = 0x800FA000 & 0x1FFFFF
diff = [i for i in range(len(ov)) if r[base+i] != ov[i]]
print("overlay45 len", len(ov), "differ", len(diff), [hex(0x800FA000+i) for i in diff[:40]])
for n in (43, 44):
    o = (X/f"rock_bin/{n}.bin").read_bytes()
    d = sum(1 for i in range(min(len(o), 0x3000)) if r[base+i] != o[i])
    print("overlay", n, "first 0x3000 differ", d)
print("boss_choice", u8(r, 0x800CD30B), "nightmare_effect", u8(r, 0x800CD30A), "game+0x33", u8(r, G+0x33), "game+0x26..35", list(r[(G+0x26)&0x1FFFFF:(G+0x36)&0x1FFFFF]))
print("game+0x2E", u8(r,G+0x2E), "game+0x0E", u8(r,G+0x0E), "text_open", u8(r,0x8008EAFC), "char", u8(r,G+0x38), "form", u8(r,G+0x5E), "5F", hex(u8(r,G+0x5F)), "60", hex(u8(r,G+0x60)))
print("dyn globals ce58", u32(r,0x800FCE58), "ce5c", u32(r,0x800FCE5C), "ce60", hex(u32(r,0x800FCE60)), "ce64", s32(r,0x800FCE64), "pts", [s16(r,0x800FCE48+2*i) for i in range(7)])
print("tables: main63", hex(u32(r, 0)), )
for pname, base0, n, sz in POOLS:
    for i in range(n):
        a = base0 + sz*i
        if u8(r, a):
            print(pname, i, obj(r, a, pname in ("main", "visual")))
