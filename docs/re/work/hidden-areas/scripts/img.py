import os
import struct
import sys

sys.path.insert(0, '/home/johnlavender/mmx-data/x6/re/tools')
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mips import dis
from names import NAMES

X6 = os.path.expanduser('~/mmx-data/x6')


def image(stage_ovl=None, stage_base=0x800E9860, boss_ovl=None, state=None):
    if state:
        from pathlib import Path
        from mmxdemake.savestate import read_ram
        exe = Path(X6 + '/extracted/retail/iso/SLUS_013.95').read_bytes()
        ram = bytearray(read_ram(Path(X6 + '/dumps') / state, exe))
    else:
        ram = bytearray(0x200000)
        exe = open(X6 + '/extracted/retail/iso/SLUS_013.95', 'rb').read()
        body = exe[0x800:]
        ram[0x10000:0x10000 + len(body)] = body
    if stage_ovl is not None:
        ov = open(X6 + f'/extracted/retail/rock_bin/{stage_ovl:02d}.bin', 'rb').read()
        o = stage_base & 0x1FFFFF
        ram[o:o + len(ov)] = ov
    if boss_ovl is not None:
        ov = open(X6 + f'/extracted/retail/rock_bin/{boss_ovl:02d}.bin', 'rb').read()
        o = 0xFA000
        ram[o:o + len(ov)] = ov
    return ram


def w(ram, a):
    return struct.unpack_from('<I', ram, a & 0x1FFFFF)[0]


def h(ram, a):
    return struct.unpack_from('<h', ram, a & 0x1FFFFF)[0]


def b(ram, a):
    return ram[a & 0x1FFFFF]


def line(ram, pc):
    x = w(ram, pc)
    d = dis(x, pc)
    t = int(d.split()[-1], 16) if d.startswith(('jal ', 'j ')) else None
    return f'{pc:08x}: {x:08x}  {d}' + (f'  ; {NAMES[t]}' if t in NAMES else '')


def lin(ram, a, n):
    for pc in range(a, a + 4 * n, 4):
        print(line(ram, pc))


def func(ram, a, maxn=1500):
    pc = a
    far = a
    for _ in range(maxn):
        x = w(ram, pc)
        d = dis(x, pc)
        print(line(ram, pc))
        op = x >> 26
        if op in (1, 4, 5, 6, 7):
            imm = x & 0xFFFF
            simm = imm - 0x10000 if imm & 0x8000 else imm
            far = max(far, pc + 4 + simm * 4)
        if d == 'jr ra' and pc >= far:
            print(line(ram, pc + 4))
            return
        pc += 4
