import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
st = read_state(sys.argv[1]); ram = st[RAM_OFFSET:RAM_OFFSET+RAM_SIZE]
a = int(sys.argv[2], 16) & 0x1FFFFF
out=[]
for i in range(80):
    c, d = ram[a+2*i], ram[a+2*i+1]
    out.append('D' if c == 0xFF else '-' if c == 2 else (str(d) if c == 0 else f'c{c}:{d}'))
for r in range(5): print(f"{r*16}-{r*16+15}:", ' '.join(out[r*16:r*16+16]))
