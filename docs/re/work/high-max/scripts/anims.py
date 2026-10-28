import sys
sys.path.insert(0, sys.argv[0].rsplit('/',1)[0])
from peek import *
r = load(sys.argv[1]); T = int(sys.argv[2],16); n = int(sys.argv[3])
for i in range(n):
    p = u32(r, T+4*i); out=[]; a=p; seen=set()
    while a not in seen and len(out)<120:
        seen.add(a)
        d,f,nx,sp = u8(r,a),u8(r,a+1),s8(r,a+2),u8(r,a+3)
        out.append(f'{sp}:{d or 256}' + (f'f{f}' if f else ''))
        if nx==0: out.append('hold'); break
        a += nx*4
        if a in seen: out.append(f'->loop to frame {(a-p)//4}'); break
    tot=sum(int(x.split(':')[1].split('f')[0]) for x in out if ':' in x)
    print(f'{i:2d} @{p:08x} sum={tot}: '+' '.join(out))
