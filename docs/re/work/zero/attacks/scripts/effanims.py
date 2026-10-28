from zprobe import *
from mmxdemake.rekit import animation_list
r=load(STATES[0])
an,err=animation_list(r,0x8007798c,[(0x80010000,0x80090000)],None,None,40)
print(err)
for k in [int(x) for x in sys.argv[1:]]:
    if k>=len(an): print(k,"n/a"); continue
    fr,loop=an[k]; u=0; s=[]
    for f in fr:
        s.append(f"@{u}:t{f.get('ticks', f.get('duration'))}:fl{f.get('flag', f.get('flags'))}"); u+=f.get('ticks', f.get('duration',0)) or 0
    print(k, "loop", loop, " ".join(s))
print(an[0][0][0].keys() if an else None)
