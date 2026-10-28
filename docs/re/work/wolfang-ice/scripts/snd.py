import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
st = read_state(sys.argv[1]); ram = st[RAM_OFFSET:RAM_OFFSET+RAM_SIZE]
w=lambda a: struct.unpack_from('<I', ram, a & 0x1FFFFF)[0]
h=lambda a: struct.unpack_from('<h', ram, a & 0x1FFFFF)[0]
print('vab', [h(0x80090D50+2*i) for i in range(6)], [struct.unpack_from('<b',ram,(0x80090D50+i)&0x1FFFFF)[0] for i in range(6)])
for bank in range(6):
    s, e = w(0x80097778+4*bank), w(0x80097738+4*bank)
    print(bank, hex(s), hex(e))
for bank, ids in ((2,[17]),(5,[3,8,9])):
    s=w(0x80097778+4*bank)
    for i in ids:
        if s: print(bank, i, ram[(s+4*i)&0x1FFFFF:(s+4*i+4)&0x1FFFFF].hex())
