import sys, struct
sys.path.insert(0, "/home/johnlavender/mmx-data/x6/re/shark-alt/scripts")
from probe_live import load, u8, s8, u16, s16, u32
r = load(sys.argv[1] if len(sys.argv) > 1 else "shark-10_alt")
G = 0x800CCED0
st, ar = u8(r, G + 0xC), u8(r, G + 0xD)
idx = st * 2 + ar
print("stage", st, "area", ar, "index", idx)
print("boss_choice", u8(r, 0x800CD30B), "nightmare_effect", u8(r, 0x800CD30A), "player(game+0x38)", u8(r, G + 0x38),
      "difficulty", u8(r, G + 0x68), "game+0x60", hex(u8(r, G + 0x60)), "armour game+0x69", hex(u8(r, G + 0x69)),
      "game+0x66", u8(r, G + 0x66), "game+0x26", u8(r, G + 0x26), "lives", u8(r, G + 0x39), "frame_count", u32(r, 0x80097420),
      "gate_time", u16(r, 0x800CD30C), "boss_pack_loaded", u8(r, 0x800CC868))
print("stage_effect", [u8(r, G + 0x3C0 + i) for i in range(0)])
cp = u32(r, 0x80073300 + 4 * idx)
print("checkpoint list", hex(cp))
for k in range(4):
    p = u32(r, cp + 4 * k)
    if not (0x80000000 <= p < 0x80200000) or (k > 0 and p == 0):
        break
    f = [s16(r, p + 2 * i) for i in range(17)]
    print(f" cp {k} @{p:08x} player {f[0]},{f[1]} cam0 {f[2]},{f[3]} cam1 {f[4]},{f[5]} cam2 {f[6]},{f[7]} lim x {f[8]}-{f[9]} y {f[10]}-{f[11]} facing {f[12]:#x} l1 {f[13]},{f[14]} l2 {f[15]},{f[16]} beam {u8(r, p + 0x22)}")
print("camera_zone_tbl word", hex(u32(r, 0x80079D70 + 4 * idx)))
k = u8(r, 0x8007A124 + idx)
print("talk k", k, "talk_seen", hex(u16(r, 0x800CCF6E + 2 * k)))
for i in range(16):
    b = 0x8007A154 + k * 128 + i * 8
    v = [s16(r, b + 2 * j) for j in range(4)]
    if any(v):
        print("  box", i, "x", v[0], "-", v[1], "y", v[2], "-", v[3])
for c in range(2):
    print("text member char", c, u16(r, 0x8006DC30 + 2 * (((st - 19) * 2 + ar) * 2 + c)))
print("bg_mode_tbl word", hex(u32(r, 0x800720AC + 4 * st)))
cam1, cam2 = 0x800971F8 + 0x50, 0x800971F8 + 0xA0
for name, c in (("layer0", 0x800971F8), ("layer1", 0x800971F8 + 0x50), ("layer2", 0x800971F8 + 0xA0)):
    print(name, "mode", u8(r, c + 4), "x", s16(r, c + 0xA), "y", s16(r, c + 0xE), "+40", s16(r, c + 0x40), "+42", s16(r, c + 0x42))
print("actor2 hp", s8(r, 0x800CD340 + 0x5C), "anim", u8(r, 0x800CD340 + 0x17), "flip", hex(u8(r, 0x800CD340 + 0x15)), "tex", hex(u16(r, 0x800CD340 + 0x40)), "clut", hex(u16(r, 0x800CD340 + 0x42)), "+0x68", hex(u32(r, 0x800CD340 + 0x68)))
print("mega +0xD5", s8(r, 0x800970A0 + 0xD5), "+0xF4", hex(u32(r, 0x800970A0 + 0xF4)), "form", u8(r, G + 0x3A) if True else 0)
k7 = 0x800C4778
for i in range(4):
    a = k7 + i * 0x30
    if u8(r, a):
        print("kind7", i, "type", u8(r, a + 1), "st", u8(r, a + 4), u8(r, a + 5), "+14", s16(r, a + 0x14), "+16", s16(r, a + 0x16), "+18", hex(u32(r, a + 0x18)), "+1C", hex(u32(r, a + 0x1C)), "+20", hex(u32(r, a + 0x20)))
