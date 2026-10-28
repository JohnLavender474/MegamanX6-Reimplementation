import sys
from img import *
st=sys.argv[1]; size=int(sys.argv[2])
r=image(state=st)
lo,hi=0x801EA000,0x801EA000+size
def fstart(pc):
    a=pc
    while a>lo:
        x=w(r,a)
        if x>>16==0x27bd and x&0x8000: return a
        a-=4
    return lo
steps={w(r,0x80073acc+4*i):i for i in range(78)}
wt={}
for i in range(83): wt.setdefault(w(r,0x80071220+4*i),[]).append(i)
calls={}
for pc in range(lo,hi,4):
    x=w(r,pc)
    if x>>26==3: calls.setdefault(((x&0x3ffffff)<<2)|0x80000000,[]).append(pc)
res=[]
for pc in range(lo,hi,4):
    d=dis(w(r,pc),pc)
    if d.startswith('sb ') and d.endswith(')') and ',1(' in d:
        reg=d.split()[1].split(',')[0]
        for k in range(1,10):
            e=dis(w(r,pc-4*k),pc-4*k)
            if e.startswith(f'addiu {reg},zero,'):
                val=int(e.split(',')[-1])
                # need a GetWeaponObject call nearby before
                ctx=[dis(w(r,pc-4*j),pc-4*j) for j in range(1,16)]
                if any('jal 0x8002c32c' in c or 'jal 0x8003d3f8' in c for c in ctx):
                    f=fstart(pc)
                    res.append((val,pc,f))
                break
            p=e.split(); 
            if len(p)>1 and p[1].split(',')[0]==reg: break
for val,pc,f in sorted(res):
    tag=[]
    if f in steps: tag.append(f'step {steps[f]}')
    if f in wt: tag.append(f'weapon type {wt[f]}')
    cs=calls.get(f,[])
    print(f'id {val:2d} at {pc:08x} func {f:08x} {tag} callers {[hex(c) for c in cs][:4]}')
