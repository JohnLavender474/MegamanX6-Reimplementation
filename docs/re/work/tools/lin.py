import sys,struct
sys.path.insert(0,sys.argv[0].rsplit('/',1)[0])
from mips import dis, load_image
from names import NAMES
ram,_=load_image()
a=int(sys.argv[1],16); b=int(sys.argv[2],16)
for pc in range(a,b,4):
    x=struct.unpack_from('<I',ram,pc&0x1fffff)[0]; d=dis(x,pc)
    t=int(d.split()[-1],16) if d.startswith(('jal ','j ')) else None
    if d=='addiu sp,sp,'+d.split(',')[-1] and d.split(',')[-1].startswith('-'): print(f'; ==== {pc:08x}')
    print(f'{pc:08x}: {x:08x}  {d}'+(f'  ; {NAMES[t]}' if t in NAMES else ''))
