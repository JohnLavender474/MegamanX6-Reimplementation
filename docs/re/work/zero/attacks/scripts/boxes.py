import re
from zprobe import *
exec(open('/home/johnlavender/mmx-data/x6/re/zero/attacks/scripts/anims.py').read().split("r=load")[0])
r=load(STATES[0])
def box(t,i): return struct.unpack_from("<bbBB", r, (t+4*i)&0x1fffff)
for typ,an in [(24,112),(25,113),(26,114),(27,115),(28,116),(29,119),(30,117),(31,118),(35,120),(36,123),(36,124),(37,126),(38,127),(42,121),(48,132)]:
    s,t,f,l=A[an]; u=0; act=[]; fr=bk=0; top=999; bot=-999; segs=[]
    for tt,ff in zip(s and t, f):
        if ff and ff<=90:
            b=box(0x801ee414,ff); fr=max(fr,-b[0]); bk=max(bk,b[0]+b[2]); top=min(top,b[1]); bot=max(bot,b[1]+b[3]); act.append(u)
            segs.append(f"{u}:{ff}{b}")
        u+=tt
    ext=""
    print(f"type {typ} anim {an}: box on updates {act[0] if act else '-'}..{(act[-1]+t[f.index(next(x for x in reversed(f) if x))] -1) if act else '-'}, obj life {u-t[-1]}; front {fr} back {bk-0 if bk>0 else 0} y {top}..{bot}")
    print("   ", " ".join(segs))
print("shoenzan wave boxes 0x801ee6f4:", [box(0x801ee6f4,i) for i in range(12)])
print("buster tbox 0x801ee3e8:", [struct.unpack_from('<bbBB',r,(0x801ee3e8+4*i)&0x1fffff) for i in range(5)])
print("buster abox 0x801ee3fc:", [box(0x801ee3fc,i) for i in range(6)])
print("junk 0x801ee6f0:", list(bytes_at(r,0x801ee6f0,4)))
