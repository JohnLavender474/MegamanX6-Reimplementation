import re,sys
from zprobe import *
A={}
for line in open('/home/johnlavender/mmx-data/x6/re/zero/kit/summary.md'):
    m=re.match(r'\| (\d+) \| ([\d ]+) \| ([\d ]+) \| ([\d ]+) \| (\w+) \|',line)
    if m and int(m.group(1)) not in A:
        A[int(m.group(1))]=([int(x) for x in m.group(2).split()],[int(x) for x in m.group(3).split()],[int(x) for x in m.group(4).split()],m.group(5))
r=load(STATES[0])
bn=lambda i: struct.unpack_from("<bbBB", r, (0x801ee414+4*i)&0x1fffff)
for n in [int(x) for x in sys.argv[1:]]:
    s,t,f,l=A[n]; u=0; out=[]
    for i,(ss,tt,ff) in enumerate(zip(s,t,f)):
        out.append(f"f{i}@{u}:t{tt}:fl{ff}" + (f"(0x{ff:x})" if ff>=16 else ""))
        u+=tt
    print(f"anim {n} loop={l} end@{u-t[-1]}:", " ".join(out))
