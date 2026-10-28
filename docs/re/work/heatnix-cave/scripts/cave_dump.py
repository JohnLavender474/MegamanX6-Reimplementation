"""Dump the cave part of Blaze Heatnix's stage from a save state: camera, flame jets (main 36),
grey blocks (stage 12), main 25, the boss gate, the record flags of the first tower and the virus bits.
Usage (repo root, PYTHONPATH=.): python3 cave_dump.py <state name in ~/mmx-data/x6/dumps>"""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_state, read_ram

DUMPS = Path.home() / "mmx-data/x6/dumps"
EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
G, MEGA = 0x800CCED0, 0x800970A0

name = sys.argv[1]
ram = read_ram(DUMPS / name, EXE.read_bytes())
b = lambda a: ram[a & 0x1FFFFF]
sb = lambda a: struct.unpack_from("<b", ram, a & 0x1FFFFF)[0]
h = lambda a: struct.unpack_from("<h", ram, a & 0x1FFFFF)[0]
w = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
sw = lambda a: struct.unpack_from("<i", ram, a & 0x1FFFFF)[0]

print(f"== {name}")
print(f"stage {b(G+0x0C)} area {b(G+0x0D)} checkpoint {b(G+0x1D)} time {w(G+0x90)} kills {w(G+0x94)} "
      f"souls {h(G+0xD2)} {h(G+0xD4)} +26 {b(G+0x26)} frame_count {w(0x80097420)}")
print(f"player x {h(MEGA+0xA)} y {h(MEGA+0xE)} state {b(MEGA+4)}.{b(MEGA+5)}.{b(MEGA+6)} hp {b(MEGA+0x5C)}")
for L, base in enumerate((0x800971F8, 0x8009724C, 0x800972A0)):
    print(f"layer{L} mode {b(base+4)} show {b(base+3)} xy {h(base+0xA)} {h(base+0xE)} "
          f"lim maxx {h(base+0x1C)} minx {h(base+0x1E)} maxy {h(base+0x20)} miny {h(base+0x22)} "
          f"tgt {[h(base+o) for o in (0x24, 0x26, 0x28, 0x2A)]} follow {b(base+0x44)} ease {b(base+0x47)}")
print(f"stage_texture_swap {b(0x8008EAB0)}")

def anim(a):
    return f"anim tick {b(a+0x44)} flag {b(a+0x45)} next {sb(a+0x46)} spr {b(a+0x47)}"

print("-- main 36 (flame jet) and main 25")
for i in range(48):
    a = 0x8008EF48 + i * 0x9C
    if b(a) == 0:
        continue
    t = b(a + 1)
    box = struct.unpack_from("<bbBB", ram, (a + 0x80) & 0x1FFFFF)
    print(f"main[{i}] type {t} var {b(a+2)} disp {b(a+3)} st {b(a+4)}.{b(a+5)}.{b(a+6)} xy {h(a+0xA)} {h(a+0xE)} "
          f"flip {b(a+0x15):#x} layer {b(a+0x14)} {anim(a)} atk {w(a+0x50):#x} hurt {w(a+0x54):#x} "
          f"hp {b(a+0x5C)} cd {b(a+0x60)} 7C {h(a+0x7C)} 7E {h(a+0x7E)} box80 {box} 84 {b(a+0x84)} 85 {b(a+0x85)} "
          f"rec {w(a+0x10):#x}")

print("-- stage objects")
for i in range(80):
    a = 0x800C1220 + i * 0xA4
    if b(a) == 0:
        continue
    print(f"stage[{i}] type {b(a+1)} var {b(a+2)} disp {b(a+3)} st {b(a+4)}.{b(a+5)}.{b(a+6)} xy {h(a+0xA)} {h(a+0xE)} "
          f"{anim(a)} vy {sw(a+0x24)/65536:.3f} terr {w(a+0x68):#x} cf {b(a+0x70):#x} latch {sb(a+0x71)} "
          f"7C {h(a+0x7C)} 7E {h(a+0x7E)} hp {b(a+0x5C)} rec {w(a+0x10):#x}")

print("-- records 0x800F3280 (flags type id kind x y)")
REC = 0x800F3280
for n in range(140):
    a = REC + n * 8
    fl, ty, idv, kind = b(a), b(a+1), b(a+2), b(a+3)
    x, y = h(a+4), h(a+6)
    if kind & 0x0F == 15:
        print("end at", n); break
    if (ty in (36, 15, 25) and kind & 0x0F in (0, 1, 2)) or (ty == 12 and kind & 0x0F == 4) or fl:
        print(f"rec {n:3d} {a:#x} flags {fl:#04x} type {ty} id {idv} kind {kind:#04x} xy {x} {y}")

print("-- fixed list 0x800F36D8")
for n in range(4):
    a = 0x800F36D8 + n * 8
    print(f"fixed {n} flags {b(a):#04x} type {b(a+1)} id {b(a+2)} kind {b(a+3):#04x} xy {h(a+4)} {h(a+6)}")

row = b(0x80075FE8 + 3 * 2 + 0)
print(f"virus soul row {row}: bits {[hex(b(G+0x178+row*32+k)) for k in range(8)]}")
