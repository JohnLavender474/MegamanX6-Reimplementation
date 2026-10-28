"""List the game state, the type list (banks) and every live object of all classes in a save state.
Usage (repo root, PYTHONPATH=.): python3 live.py <state.gz> [class ...]"""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_state, read_ram
from mmxdemake.sprites import banks, scratchpad, type_list, entries

DUMPS = Path.home() / "mmx-data/x6/dumps"
EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
CLASSES = [("main", 0x8008EF48, 48, 0x9C), ("stage", 0x800C1220, 80, 0xA4), ("visual", 0x80093C98, 32, 0xA0),
           ("misc", 0x800CD410, 96, 0x60), ("misc2", 0x800964A0, 32, 0x60), ("effect", 0x80091298, 96, 0x70),
           ("weapon", 0x800950A0, 32, 0xA0), ("logic", 0x8009B7B0, 96, 0x30), ("ray", 0x8009C9B0, 64, 0x60),
           ("kind7", 0x800C4778, 4, 0x30), ("actor2", 0x800CD340, 1, 0xB0)]
G, MEGA = 0x800CCED0, 0x800970A0

name = sys.argv[1]
st = read_state(DUMPS / name)
ram = read_ram(DUMPS / name, EXE.read_bytes())
b = lambda a: ram[a & 0x1FFFFF]
h = lambda a: struct.unpack_from("<h", ram, a & 0x1FFFFF)[0]
H = lambda a: struct.unpack_from("<H", ram, a & 0x1FFFFF)[0]
w = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
only = sys.argv[2:]
if not only:
    print(f"stage {b(G+0x0C)} area {b(G+0x0D)} checkpoint {b(G+0x1D)} exit {b(G+0x0F):#x} chara {b(G+0x38)} form {b(G+0x5E)} "
          f"difficulty {b(G+0x68)} lives {b(G+0x39)} time {w(G+0x90)} kills {w(G+0x94)} armor_parts {b(G+0x69):#x}")
    print(f"nightmare_effect {b(0x800CD30A)} stage_effect {[b(G+0x168+i) for i in range(16)]} effect_new {[b(G+0x465+i) for i in range(3)]}")
    print(f"flags +26 {b(G+0x26)} +27 {b(G+0x27)} +28 {b(G+0x28)} reploid words {[hex(w(G+0xD8+4*i)) for i in range(8)]}")
    print(f"player x {h(MEGA+0xA)} y {h(MEGA+0xE)} chara {b(MEGA+2)} state {b(MEGA+4)} step {b(MEGA+5)} hp {b(MEGA+0x5C)}")
    cam = 0x800971F8
    print(f"camera x {h(cam+0x20)} y {h(cam+0x22)} limits {[h(cam+o) for o in (0x24, 0x26, 0x28, 0x2A)]}")
    sp = scratchpad(st)
    print("type list", type_list(ram))
    for bk in banks(ram, sp):
        print(f"  bank {bk.index}: type {bk.type} data {bk.data:#x} sprites {bk.count} texture {bk.texture} clut {bk.clut:#x}")
    print("sprite_bank_base entries", len(entries(ram, sp)))
for cname, base, n, sz in CLASSES:
    if only and cname not in only:
        continue
    for i in range(n):
        a = base + i * sz
        if b(a) == 0:
            continue
        extra = ""
        if sz >= 0x48:
            extra = f" tbl {w(a+0x30):#x} spr {w(a+0x3C):#x} tex {H(a+0x40):#x} pal {H(a+0x42):#x} anim {b(a+0x44)} s47 {b(a+0x47)}"
        if sz >= 0x9C:
            extra += f" hp {b(a+0x5C)} cd {b(a+0x60)}"
        print(f"{cname}[{i}] {a:#x} f {b(a):#x} type {b(a+1)} var {b(a+2)} disp {b(a+3)} st {b(a+4)}.{b(a+5)}.{b(a+6)} "
              f"x {h(a+0x0A)} y {h(a+0x0E)} rec {w(a+0x10):#x}{extra}")
