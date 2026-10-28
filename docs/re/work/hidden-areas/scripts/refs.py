import sys
from img import *
ram=image()
lo=int(sys.argv[1],16);hi=int(sys.argv[2],16)
for pc in range(0x80010000,0x800E0000,4):
    x=w(ram,pc)
    if lo<=x<hi: print('word',hex(pc),hex(x))
# lui/addiu pairs
for pc in range(0x80010000,0x80080000,4):
    x=w(ram,pc)
    if x>>26==0xf:
        hi16=x&0xffff; rt=(x>>16)&31
        for k in range(1,8):
            y=w(ram,pc+4*k)
            if (y>>26) in (0x9,0x23,0x24,0x20,0x21,0x25) and (y>>21)&31==rt:
                imm=y&0xffff; s=imm-0x10000 if imm&0x8000 else imm
                a=((hi16<<16)+s)&0xffffffff
                if lo<=a<hi: print('ref',hex(pc+4*k),hex(a),dis(y,pc+4*k))
                break
