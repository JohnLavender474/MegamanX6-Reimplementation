"""Dump the Nightmare Snake objects and related globals from a save state.
Usage: python3 dump_state.py <state.gz> (run with PYTHONPATH=~/IdeaProjects/mmx-demake-tools)"""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE

st = read_state(sys.argv[1]); ram = st[RAM_OFFSET:RAM_OFFSET + RAM_SIZE]
def b(a): return ram[a & 0x1FFFFF]
def sb(a): v = b(a); return v - 256 if v > 127 else v
def h(a): return struct.unpack_from('<h', ram, a & 0x1FFFFF)[0]
def w(a): return struct.unpack_from('<i', ram, a & 0x1FFFFF)[0]
def uw(a): return struct.unpack_from('<I', ram, a & 0x1FFFFF)[0]

def obj(base, i, size, cls):
    o = base + i * size
    return dict(cls=cls, slot=i, addr=hex(o), flag=hex(b(o)), type=b(o+1), var=sb(o+2), disp=b(o+3),
                state=b(o+4), step=b(o+5), sub=b(o+6), act4=sb(o+7), x=h(o+0xA), y=h(o+0xE),
                vx=w(o+0x20)/65536, vy=w(o+0x24)/65536, ax=w(o+0x28), grav=w(o+0x2C),
                hp=sb(o+0x5C), contact=b(o+0x60), atk=hex(uw(o+0x50)), hurt=hex(uw(o+0x54)),
                dmg=hex(uw(o+0x58)), terr=hex(uw(o+0x68)), cf=b(o+0x70), t7c=h(o+0x7C), t7e=h(o+0x7E),
                p80=hex(uw(o+0x80)), p84=hex(uw(o+0x84)), p88=hex(uw(o+0x88)), b8d=b(o+0x8D), b8e=b(o+0x8E),
                h90=h(o+0x90), b92=b(o+0x92), flip=hex(b(o+0x15)), anim_tbl=hex(uw(o+0x30)), pal=hex(h(o+0x42)&0xffff))

for i in range(48):
    o = 0x8008EF48 + i * 0x9C
    if b(o) and b(o+1) == 25: print(obj(0x8008EF48, i, 0x9C, 'main'))
for i in range(32):
    o = 0x80093C98 + i * 0xA0
    if b(o) and b(o+1) == 29: print(obj(0x80093C98, i, 0xA0, 'visual'))
for i in range(80):
    o = 0x800C1220 + i * 0xA4
    if b(o): print('stage', i, 'type', b(o+1), 'var', b(o+2), 'state', b(o+4), b(o+5), b(o+6), 'xy', h(o+0xA), h(o+0xE))
g = 0x800CCED0
print('game: stage', b(g+0x1A) if False else '', 'checkpoint', sb(0x800CCEED), 'difficulty', b(g+0x68), 'bar', b(g+0x24), 'boss', hex(uw(g+0x20)),
      'hpmax(+0x8C)', b(g+0x8C), '+0x1C', b(g+0x1C), '+0x25', b(g+0x25), 'stage_flag', b(g+0x26), 'level(+0x8F)', b(g+0x8F))
m = 0x800970A0
print('mega: var', b(m+2), 'state', b(m+4), 'step', b(m+5), 'xy', h(m+0xA), h(m+0xE), 'hp', sb(m+0x5C), 'D0', b(m+0xD0), 'D1', b(m+0xD1), 'layer', b(m+0x14))
for L, base in enumerate((0x800971F8, 0x8009724C, 0x800972A0)):
    print(f'layer{L}: mode', b(base+4), 'vis(+3)', b(base+3), 'xy', h(base+0xA), h(base+0xE), 'lim', [h(base+o) for o in (0x1C,0x1E,0x20,0x22)],
          'tgt', [h(base+o) for o in (0x24,0x26,0x28,0x2A)], 'ofs', h(base+0x40), h(base+0x42), '+0x4C', b(base+0x4C), '+0x52', b(base+0x52))
print('frame_count', uw(0x80097420))
