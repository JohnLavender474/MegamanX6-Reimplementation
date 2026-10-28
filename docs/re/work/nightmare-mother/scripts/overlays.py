import sys; sys.path.insert(0, sys.argv[0].rsplit('/',1)[0])
from peek import *
from pathlib import Path
r = load(sys.argv[1] if len(sys.argv)>1 else STATE)
d = Path('/home/johnlavender/mmx-data/x6/extracted/retail/rock_bin')
for base in (0x800E9860, 0x800FA000):
    best=[]
    for f in sorted(d.glob('*.bin')):
        b=f.read_bytes(); m=ram_at(r,base,len(b))
        diff=sum(1 for x,y in zip(b,m) if x!=y)
        best.append((diff/len(b),diff,len(b),f.name))
    best.sort(); print(hex(base), best[:3])
