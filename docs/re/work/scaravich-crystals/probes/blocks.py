import os, struct, sys
from ramdis import D, X, u32, u16, u8
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad
from rngsim import step
exe = open(os.path.join(X, 'iso/SLUS_013.95'), 'rb').read()

def load(name):
    st = read_state(os.path.join(D, name))
    return ram_from(st, exe, name), scratchpad(st)

def cell(r, sp, x, y, layer=0):
    w, h = u8(r, 0x800CD338), u8(r, 0x800CD339)
    size = u16(r, 0x8008EC10)
    lay = struct.unpack_from('<I', sp, 4)[0]
    scr = struct.unpack_from('<I', sp, 8)[0]
    s = u8(r, lay + layer * size + (y >> 8) * w + (x >> 8))
    return u16(r, scr + s * 512 + ((y >> 4) & 15) * 32 + ((x >> 4) & 15) * 2)

def rect(r, sp, x, y, cw, ch):
    return tuple(cell(r, sp, x + 16 * i, y + 16 * j) for j in range(ch) for i in range(cw))

SEC2 = [(1760, 48), (1888, 240), (1664, 272), (1696, 464), (1888, 496), (1664, 592)]
SRC = (1984, 80, 2, 3)
r1, s1 = load('scaravich-1_start.state.gz')
r2, s2 = load('scaravich-2_totem-section.state.gz')
src = rect(r2, s2, SRC[0], SRC[1], SRC[2], SRC[3])
print('source block', [hex(c) for c in src])
bits = []
for x, y in SEC2:
    a, b = rect(r1, s1, x, y, 2, 3), rect(r2, s2, x, y, 2, 3)
    placed = b == src
    bits.append(0 if placed else 1)
    print(f'({x},{y}) state1 {"block" if a == src else "free "} state2 {"block" if placed else "free "}')
print('coin bits (bit0 of GetRNG; 0 = block placed):', bits)
s = u16(r1, 0x80090E70)
target = u16(r2, 0x80090E70)
seq = []
for k in range(1300):
    s, v = step(s); seq.append(v)
    if s == target: n = k + 1; break
print('calls between dumps', n)
hits = [k for k in range(n - 7) if seq[k] % 12 == 6 and [v & 1 for v in seq[k + 1:k + 7]] == bits]
print('call indexes with roll%12==6 followed by these 6 coin bits:', hits, 'of', n)
for k in hits: print('  roll value', seq[k], 'calls after the roll until dump 2:', n - k - 1)
