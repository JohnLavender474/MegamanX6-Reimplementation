import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
st = read_state(sys.argv[1]); ram = st[RAM_OFFSET:RAM_OFFSET+RAM_SIZE]
b=lambda a: ram[a&0x1FFFFF]; h=lambda a: struct.unpack_from('<h',ram,a&0x1FFFFF)[0]
i32=lambda a: struct.unpack_from('<i',ram,a&0x1FFFFF)[0]; w=lambda a: struct.unpack_from('<I',ram,a&0x1FFFFF)[0]
m=0x800970A0
print('mega x,y', h(m+0x0A), h(m+0x0E), 'step', b(m+5), 'state', b(m+4), 'cam0 x,y', h(0x800971F8+0x0A), h(0x800971F8+0x0E), 'checkpoint', b(0x800CCED0+0x1D), 'nightmare', b(0x800CD30A))
for i in range(48):
    a=0x8008EF48+i*0x9C
    if b(a) and b(a+1)==17:
        print(f"slot {i} var={b(a+2)} state={b(a+4)} step={b(a+5)} act4={b(a+7)} x={h(a+0xA)} y={h(a+0xE)} vx={i32(a+0x20)/65536} vy={i32(a+0x24)/65536} g={i32(a+0x2C)/65536} 7C={h(a+0x7C)} 7E={h(a+0x7E)} size={b(a+0x80)} 81={b(a+0x81)} hp={b(a+0x5C)} atk={w(a+0x50):#x} hurt={w(a+0x54):#x} terr={w(a+0x68):#x} anim_spr={b(a+0x47)} disp={b(a+3)} prio={b(a+0x16)}")
