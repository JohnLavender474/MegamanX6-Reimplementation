import sys
sys.path.insert(0, sys.argv[0].rsplit('/',1)[0])
from peek import *
r = load('turtloid-8_high-max.state.gz')
for t in sys.argv[1:]:
    a=int(t,16); print(f'table {a:08x}')
    for row in range(5):
        cells=[]
        for i in range(16):
            c,d=u8(r,a+(row*16+i)*2),u8(r,a+(row*16+i)*2+1)
            cells.append('D' if c==0xff else '–' if c==2 else str(d) if c==0 else f'c{c}:{d}')
        print(f'| {row*16}–{row*16+15} | '+' '.join(cells)+' |')
