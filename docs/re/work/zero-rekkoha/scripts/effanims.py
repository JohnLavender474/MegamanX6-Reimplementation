import sys
from zprobe import *
from mmxdemake.rekit import animation_list
r=load("heatnix-3_rekkoha")
an,err=animation_list(r,0x8007798c,[(0x80010000,0x80090000)],None,None,60)
print("err",err,"count",len(an))
for k,(fr,loop) in enumerate(an):
    u=0; s=[]
    for f in fr:
        t=f.get('ticks', f.get('duration')); s.append(f"s{f.get('sprite')}:t{t}:fl{f.get('flag', f.get('flags'))}"); u+=t or 0
    mx=max(f.get('sprite') for f in fr) if fr else None
    print(k,"loop",loop,"max",mx,"len",u," ".join(s))
