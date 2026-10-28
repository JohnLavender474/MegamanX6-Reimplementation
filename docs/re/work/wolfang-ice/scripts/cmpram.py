import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
def ram(p):
    s=read_state(p); return s[RAM_OFFSET:RAM_OFFSET+RAM_SIZE]
ra, rb = ram(sys.argv[1]), ram(sys.argv[3])
a0, b0, n = int(sys.argv[2],16), int(sys.argv[4],16), int(sys.argv[5])
bad=0
for i in range(n):
    wa=struct.unpack_from('<I',ra,(a0+4*i)&0x1FFFFF)[0]; wb=struct.unpack_from('<I',rb,(b0+4*i)&0x1FFFFF)[0]
    if wa!=wb:
        op_a, op_b = wa>>26, wb>>26
        # allowed: same opcode and (j/jal) or (lui / addiu / load-store imm differ) or branch offset
        if op_a==op_b and (op_a in (2,3) or (wa>>16)==(wb>>16)):
            continue
        bad+=1; print(hex(a0+4*i), hex(wa), hex(wb))
print('words', n, 'unexplained', bad)
