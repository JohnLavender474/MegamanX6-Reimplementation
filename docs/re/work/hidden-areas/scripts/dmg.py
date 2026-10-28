import sys
from img import *
r=image()
for a in sys.argv[1:]:
    a=int(a,16); print(hex(a))
    for row in range(5):
        cells=[]
        for i in range(16):
            c=b(r,a+2*(row*16+i)); d=b(r,a+2*(row*16+i)+1)
            cells.append('D' if c==0xff else '–' if c==2 else str(d) if c==0 else f'c{c}:{d}')
        print(f'| {row*16}–{row*16+15} | '+' '.join(cells)+' |')
