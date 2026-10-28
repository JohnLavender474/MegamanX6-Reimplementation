import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE

state = read_state(sys.argv[1])
ram = state[RAM_OFFSET:RAM_OFFSET + RAM_SIZE]
def b(a, n=1): return ram[(a & 0x1FFFFF):(a & 0x1FFFFF) + n]
def u8(a): return b(a)[0]
def u16(a): return struct.unpack('<H', b(a, 2))[0]
def u32(a): return struct.unpack('<I', b(a, 4))[0]
MEGA = 0x800970A0
print('mega step', u8(MEGA + 2), u8(MEGA + 3), 'x', u16(MEGA + 0x0A) if False else '', 'anim word +0x44', b(MEGA + 0x44, 4).hex())
for off in (0x30, 0x38, 0x3C):
    print(f'mega+{off:#x} = {u32(MEGA + off):#010x}')
print('mega+0x40 tex', hex(u16(MEGA + 0x40)), '+0x42 clut', hex(u16(MEGA + 0x42)))
for off in (0x8E, 0x90, 0x91, 0x92, 0x93, 0x95, 0x97, 0x98, 0xA2, 0xA6, 0xE7):
    print(f'mega+{off:#x} = {u8(MEGA + off)}')
print('mega 0x00..0xFF'); 
for r in range(0, 0x100, 16): print(f'{r:02x}', b(MEGA + r, 16).hex(' '))
W = 0x800950A0
for s in range(32):
    a = W + s * 0xA0
    if u8(a):
        print(f'weapon slot {s}: ', b(a, 0x50).hex(' '))
E = 0x80091298
for s in range(96):
    a = E + s * 0x70
    if u8(a):
        print(f'effect slot {s}: type {u8(a+1)} var {u8(a+2)}', b(a, 0x50).hex(' '))
