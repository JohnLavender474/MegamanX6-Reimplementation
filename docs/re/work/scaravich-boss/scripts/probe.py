import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
D = Path.home()/"mmx-data/x6/dumps"
def load(name):
    if not name.endswith(".gz"): name += ".state.gz"
    p = D/name
    return ram_from(read_state(p), exe, p)
def u8(r,a): return r[a & 0x1FFFFF]
def s8(r,a): return struct.unpack_from("<b", r, a & 0x1FFFFF)[0]
def u16(r,a): return struct.unpack_from("<H", r, a & 0x1FFFFF)[0]
def s16(r,a): return struct.unpack_from("<h", r, a & 0x1FFFFF)[0]
def u32(r,a): return struct.unpack_from("<I", r, a & 0x1FFFFF)[0]
def s32(r,a): return struct.unpack_from("<i", r, a & 0x1FFFFF)[0]
def fx(r,a): return round(s32(r,a)/65536, 4)
G=0x800CCED0; M=0x800970A0; CAM=0x800971F8
POOLS = [("main",0x8008EF48,48,0x9C),("stage",0x800C1220,80,0xA4),("visual",0x80093C98,32,0xA0),
         ("effect",0x80091298,96,0x70),("weapon",0x800950A0,32,0xA0),("misc",0x800CD410,96,0x60),
         ("misc2",0x800964A0,32,0x60),("logic",0x8009B7B0,96,0x30),("ray",0x8009C9B0,64,0x60)]
def obj(r,a,full):
    s = (f"@{a:08x} fl {u8(r,a):02x} type {u8(r,a+1)} var {s8(r,a+2)} disp {u8(r,a+3)} st {u8(r,a+4)}/{u8(r,a+5)}/{u8(r,a+6)}/{s8(r,a+7)}"
         f" x {s16(r,a+0xA)} y {s16(r,a+0xE)}")
    if full:
        s += (f" flip {u8(r,a+0x15):02x} anim {u8(r,a+0x17)} af {u8(r,a+0x45)} dur {u8(r,a+0x44)} spr {u8(r,a+0x47)}"
              f" sx {fx(r,a+0x20)} sy {fx(r,a+0x24)} hp {s8(r,a+0x5C)} cd {u8(r,a+0x60)} if {u8(r,a+0x61)} nohit {u8(r,a+0x7A)}"
              f" tex {u16(r,a+0x40):04x} clut {u16(r,a+0x42):04x} tbl {u32(r,a+0x30):08x} spd {u32(r,a+0x3C):08x}"
              f" atk {u32(r,a+0x50):08x} hurt {u32(r,a+0x54):08x} 7C {u32(r,a+0x7C):08x} 80 {u32(r,a+0x80):08x} 84 {u32(r,a+0x84):08x} 88 {u32(r,a+0x88):08x} 8C {u32(r,a+0x8C):08x}")
    return s
def report(name, full=True):
    r = load(name)
    print("=====", name)
    print(f"stage {u8(r,G+0xC)} area {u8(r,G+0xD)} exit {u8(r,G+0xF):02x} stop {list(r[(G+0x10)&0x1FFFFF:(G+0x1B)&0x1FFFFF])} lock {u8(r,G+0x1C)} cp {u8(r,G+0x1D)}"
          f" boss {u32(r,G+0x20):08x} g24 {u8(r,G+0x24)} 2F-31 {u8(r,G+0x2F)},{u8(r,G+0x30)},{u8(r,G+0x31)} lives {u8(r,G+0x39)}"
          f" bossmax {u8(r,G+0x8C)} lvl {u8(r,G+0x8F)} 8D {u8(r,G+0x8D)},{u8(r,G+0x8E)} time {u32(r,G+0x90)} dmg {u32(r,G+0x98)} maxhit {u16(r,G+0x9C)} g0E {u8(r,G+0xE)} diff {u8(r,G+0x68)}")
    print("mega", obj(r,M,True))
    print(f"  D0-D2 {u8(r,M+0xD0)},{u8(r,M+0xD1)},{u8(r,M+0xD2)} 67 {s8(r,M+0x67)} 86 {s8(r,M+0x86)} 8E {s8(r,M+0x8E)} CB {s8(r,M+0xCB)} 89 {u8(r,M+0x89):02x} E3 {u8(r,M+0xE3)} keys {u16(r,M+0x7C):04x} {u16(r,M+0x80):04x} 68 {u32(r,M+0x68):08x} 70 {u8(r,M+0x70):02x}")
    print(f"cam0 x {s16(r,CAM+0xA)} y {s16(r,CAM+0xE)} lim maxx {s16(r,CAM+0x1C)} minx {s16(r,CAM+0x1E)} maxy {s16(r,CAM+0x20)} miny {s16(r,CAM+0x22)} tgt {[s16(r,CAM+o) for o in (0x24,0x26,0x28,0x2A)]} 44 {u8(r,CAM+0x44)} 47 {u8(r,CAM+0x47)} 48 {u8(r,CAM+0x48)}")
    for nm, base, n, sz in POOLS:
        for i in range(n):
            a = base + i*sz
            if u8(r,a):
                print(f"  {nm:6s}{i:3d}", obj(r,a, nm in ("main","stage","visual","weapon","effect")))
    return r
if __name__ == "__main__":
    for s in sys.argv[1:]: report(s)
