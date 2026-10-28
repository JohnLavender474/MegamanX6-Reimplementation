import sys
from img import *
ovl=int(sys.argv[2]) if len(sys.argv)>2 else None
r=image(stage_ovl=ovl)
pat=sys.argv[1]
hi=0x800f0000 if ovl is None else 0x80100000
for pc in range(0x80010000,0x80100000,4):
    d=dis(w(r,pc),pc)
    if pat in d: print(line(r,pc))
