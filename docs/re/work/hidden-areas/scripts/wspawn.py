import sys
from img import *
r=image(state=(sys.argv[1] if len(sys.argv)>1 else 'sheldon-1_lemons.state.gz'))
WT=0x80071220
types={}
for i in range(83):
    types.setdefault(w(r,WT+4*i),[]).append(i)
regions=[(0x80010000,0x8008F000),(0x801EA000,0x801EA000+int(sys.argv[2]) if len(sys.argv)>2 else 0x801EA000+44588)]
# function starts: prologue addiu sp,sp,-N
def fstart(pc):
    a=pc
    while a>pc-0x2000:
        x=w(r,a)
        if x>>16==0x27bd and x&0x8000: return a
        a-=4
    return None
want=set(range(0,80))
for lo,hi in regions:
    for pc in range(lo,hi,4):
        x=w(r,pc)
        if x>>26==3 and (((x&0x3ffffff)<<2)|0x80000000) in (0x8003d3f8,0x8002c32c):
            tgt=((x&0x3ffffff)<<2)|0x80000000
            found=None
            for k in range(-10,2):
                d=dis(w(r,pc+4*k),pc+4*k)
                if tgt==0x8003d3f8 and d.startswith('addiu a1,zero,'):
                    found=int(d.split(',')[-1])
            if tgt==0x8002c32c:
                for k in range(1,14):
                    d=dis(w(r,pc+4*k),pc+4*k)
                    if d.startswith('addiu ') and ',zero,' in d:
                        reg=d.split()[1].split(',')[0]; val=int(d.split(',')[-1])
                        for j in range(k,k+6):
                            e=dis(w(r,pc+4*j),pc+4*j)
                            if e.startswith('sb '+reg+',1('): found=val
            if found in want:
                f=fstart(pc)
                print(f'id {found:2d} spawned at {pc:08x} in func {f:08x}' + (f'  (weapon type(s) {types.get(f)})' if f in types else ''))
print('type table entries for 33-44,46,56:',{i:hex(w(r,WT+4*i)) for i in list(range(33,45))+[46,50,51,56]})
