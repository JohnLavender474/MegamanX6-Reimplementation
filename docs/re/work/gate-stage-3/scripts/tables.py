import os
import struct
import sys

sys.path.insert(0, os.path.expanduser("~/IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_ram

EXE = open(os.path.expanduser("~/mmx-data/x6/extracted/retail/iso/SLUS_013.95"), "rb").read()
r = read_ram(os.path.expanduser("~/mmx-data/x6/dumps/" + (sys.argv[1] if len(sys.argv) > 1 else "gate-4_before-gate.state.gz") + ""), EXE)
u8 = lambda a: r[a & 0x1FFFFF]
u16 = lambda a: struct.unpack_from("<H", r, a & 0x1FFFFF)[0]
s16 = lambda a: struct.unpack_from("<h", r, a & 0x1FFFFF)[0]
u32 = lambda a: struct.unpack_from("<I", r, a & 0x1FFFFF)[0]
ST = int(sys.argv[2]) if len(sys.argv) > 2 else 18
print("stage_members", [u16(0x8006D9FC + 2 * (ST * 2 + a)) for a in (0, 1)],
      "bytes?", [u8(0x8006D9FC + ST * 2 + a) for a in (0, 1)])
print("stage_overlays", [u16(0x8006DB78 + 2 * (ST * 2 + a)) for a in (0, 1)])
print("text_members", [u16(0x8006DBD4 + 2 * (ST * 2 + k)) for k in (0, 1)], [u8(0x8006DBD4 + ST * 2 + k) for k in (0, 1)])
print("stage_music", [(u8(0x8007093C + ST * 8 + a * 4 + c * 2), u8(0x8007093C + ST * 8 + a * 4 + c * 2 + 1)) for a in (0, 1) for c in (0, 1)],
      "boss_music", (u8(0x800709F4 + ST * 2), u8(0x800709F4 + ST * 2 + 1)))
print("boss_pack_tbl", u16(0x8007A0E0 + 2 * ST), "loaded", u8(0x800CC868))
k = u8(0x8007A124 + ST * 2)
print("talk_stage_idx", k, [u8(0x8007A124 + ST * 2 + a) for a in (0, 1)])
kb = 0x8007A154 + k * 128
print("talk_boxes k", k, [[s16(kb + 8 * i + 2 * j) for j in range(4)] for i in range(4)], "talk_seen?")
ptr = u32(0x80073300 + 4 * (ST * 2))
print("checkpoint list", hex(ptr))
for c in range(8):
    rec = u32(ptr + 4 * c)
    if not (0x80000000 <= rec < 0x80200000):
        print(" end", hex(rec)); break
    f = [s16(rec + 2 * i) for i in range(17)]
    print(f" cp {c} {rec:#x} player {f[0:2]} cam0 {f[2:4]} cam1 {f[4:6]} cam2 {f[6:8]} limits {f[8:12]} facing {u16(rec+0x18):#x} l1off {f[13:15]} l2off {f[15:17]} beam {u8(rec+0x22)}")
cz = u32(0x80079D70 + 4 * (ST * 2))
print("camera_zone_tbl", hex(cz))
G = 0x800CCED0
print("game: checkpoint", u8(G + 0x1D), "boss_choice?", "time", u32(G + 0x90), "+0x2F", u8(G + 0x2F), "+0x60", hex(u8(G + 0x60)),
      "+0x61", u8(G + 0x61), "+0x62", u8(G + 0x62), "hp", u8(0x800970A0 + 0x5C), "maxgauge", u16(0x800970A0 + 0x14A),
      "gauges", [u16(0x800970A0 + 0xA8 + 2 * i) for i in range(6)])
print("stage_texture_swap", u8(0x8008EAB0), "bg_mode", [u8(0x800720AC + ST * 4 + i) for i in range(4)])
