import sys,struct
sys.path.insert(0,sys.argv[0].rsplit('/',1)[0])
from mips import load_image
ram,_=load_image()
a=int(sys.argv[1],16); n=int(sys.argv[2])
for i in range(n):
    v=struct.unpack_from('<I',ram,(a+4*i)&0x1fffff)[0]
    b=ram[(a+4*i)&0x1fffff:(a+4*i+4)&0x1fffff]
    print(f'{a+4*i:08x}: {v:08x}  {b.hex(" ")}')
