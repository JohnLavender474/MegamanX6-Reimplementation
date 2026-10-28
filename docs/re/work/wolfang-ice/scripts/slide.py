import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
st = read_state(sys.argv[1]); ram = st[RAM_OFFSET:RAM_OFFSET+RAM_SIZE]
b=lambda a: ram[a&0x1FFFFF]; h=lambda a: struct.unpack_from('<h',ram,a&0x1FFFFF)[0]
i32=lambda a: struct.unpack_from('<i',ram,a&0x1FFFFF)[0]; w=lambda a: struct.unpack_from('<I',ram,a&0x1FFFFF)[0]
m=0x800970A0
tb=w(m+0x68)
print('mega x', i32(m+8)/65536, 'y', i32(m+0xC)/65536, 'x13C', i32(m+0x13C)/65536, 'state', b(m+4), 'step', b(m+5), 'sub', b(m+6), 'flip', b(m+0x15), 'speed_x', i32(m+0x20)/65536, 'speed_y', i32(m+0x24)/65536, 'cf70', hex(b(m+0x70)), '89', hex(b(m+0x89)), '104', i32(m+0x104)/65536, 'tbox', ram[tb&0x1FFFFF:(tb&0x1FFFFF)+4].hex() if tb else None)
for i in range(96):
    a=0x8009B7B0+i*0x30
    if b(a) and b(a+1)==7:
        print('logic7', 'state', b(a+4), 'x', h(a+0xA), 'v', i32(a+0x14)/65536, 'a', i32(a+0x18)/65536, 'step1C', b(a+0x1C), 'mode1D', b(a+0x1D), 'dir1E', hex(b(a+0x1E)), 'timer1F', b(a+0x1F), 'val20', b(a+0x20), 'prev21', b(a+0x21))
