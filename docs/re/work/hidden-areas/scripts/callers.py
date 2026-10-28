import sys
from img import *
ram=image(*( [int(sys.argv[2])] if len(sys.argv)>2 else []))
t=int(sys.argv[1],16)
lo,hi=0x80010000,0x80200000
for pc in range(lo,hi,4):
    x=w(ram,pc)
    if x>>26==3 and ((x&0x3ffffff)<<2)|0x80000000==t: print('jal',hex(pc))
    if x==t: print('word',hex(pc))
