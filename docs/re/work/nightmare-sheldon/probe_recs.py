import sys, struct, os
sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'),'rb').read()
st=sys.argv[1]
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/'+st), EXE)
def u32(a): return struct.unpack_from('<I',r,a&0x1fffff)[0]
stage, area = r[0xCCEDC], r[0xCCEDD]
for tbl,name in ((0x8007349C,'enemy_set'),(0x80073554,'fixed_set')):
    p = u32(tbl + stage*8 + area*4)
    print(name, hex(p))
    a=p; i=0
    while True:
        f,t,idv,k = r[a&0x1fffff:(a&0x1fffff)+4]
        x,y = struct.unpack_from('<hh',r,(a&0x1fffff)+4)
        print(f"  [{i}] {a:#x}: flags{f:02x} type{t} id{idv} kind{k:#x} x{x} y{y}")
        if k&0xf==15 or i>200: break
        a+=8; i+=1
