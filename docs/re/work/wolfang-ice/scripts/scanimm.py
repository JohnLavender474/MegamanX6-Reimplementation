import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
st = read_state(sys.argv[1]); ram = st[RAM_OFFSET:RAM_OFFSET+RAM_SIZE]
imm = int(sys.argv[2],16); lo=int(sys.argv[3],16); hi=int(sys.argv[4],16)
for a in range(lo, hi, 4):
    w=struct.unpack_from('<I',ram,a&0x1FFFFF)[0]
    op=w>>26
    if op in (0x20,0x21,0x23,0x24,0x25,0x28,0x29,0x2b) and (w&0xffff)==imm:
        print(hex(a), f"{w:08x}", 'store' if op>=0x28 else 'load')
