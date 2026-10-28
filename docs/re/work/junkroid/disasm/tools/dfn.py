import sys,struct
sys.path.insert(0,sys.argv[0].rsplit('/',1)[0])
from mips import dis, load_image
ram,_=load_image()
def w(a): return struct.unpack_from('<I',ram,a&0x1fffff)[0]
def func(a,maxn=600):
    # dump until a jr ra whose address is beyond all seen forward branch targets
    pc=a; far=a; out=[]
    for i in range(maxn):
        x=w(pc); d=dis(x,pc); out.append(f'{pc:08x}: {x:08x}  {d}')
        op=x>>26
        if op in(1,4,5,6,7):
            imm=x&0xffff; simm=imm-0x10000 if imm&0x8000 else imm
            t=pc+4+simm*4; far=max(far,t)
        if d=='jr ra' and pc>=far:
            pc+=4; x=w(pc); out.append(f'{pc:08x}: {x:08x}  {dis(x,pc)}'); break
        pc+=4
    return out
for arg in sys.argv[1:]:
    a=int(arg,16); print(f'; ---- func {a:08x}'); print('\n'.join(func(a))); print()
