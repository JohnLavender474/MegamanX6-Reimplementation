import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.mips import disassemble
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
D = Path.home()/"mmx-data/x6/dumps"
_cache = {}
def load(name="sheldon-1_lemons"):
    if not name.endswith(".gz"): name += ".state.gz"
    if name not in _cache:
        p = D/name
        _cache[name] = ram_from(read_state(p), exe, p)
    return _cache[name]
def u8(r,a): return r[a & 0x1FFFFF]
def s8(r,a): return struct.unpack_from("<b", r, a & 0x1FFFFF)[0]
def u16(r,a): return struct.unpack_from("<H", r, a & 0x1FFFFF)[0]
def s16(r,a): return struct.unpack_from("<h", r, a & 0x1FFFFF)[0]
def u32(r,a): return struct.unpack_from("<I", r, a & 0x1FFFFF)[0]
def s32(r,a): return struct.unpack_from("<i", r, a & 0x1FFFFF)[0]
def raw(r,a,n): return r[(a & 0x1FFFFF):(a & 0x1FFFFF)+n]
def dis(r, a, n):
    out=[]
    for i in range(n):
        pc=a+4*i; w=u32(r,pc)
        out.append(f"{pc:08x}: {w:08x}  {disassemble(w, pc)}")
    return out
def dis_fn(r, a, maxn=2000):
    """disassemble until jr ra + delay slot, following no branches; stops at 2nd jr ra past furthest branch target"""
    out=[]; far=a; i=0
    while i<maxn:
        pc=a+4*i; w=u32(r,pc); t=disassemble(w,pc)
        out.append(f"{pc:08x}: {w:08x}  {t}")
        op=w>>26
        if op in (4,5,6,7,1) or (op==0x14 or op==0x15):
            off=struct.unpack("<h",struct.pack("<H",w&0xffff))[0]
            far=max(far,pc+4+off*4)
        if w==0x03e00008 and pc>=far:
            out.append(f"{pc+4:08x}: {u32(r,pc+4):08x}  {disassemble(u32(r,pc+4),pc+4)}"); break
        i+=1
    return out
if __name__=="__main__":
    st=sys.argv[1]; a=int(sys.argv[2],16)
    r=load(st)
    if len(sys.argv)>3: print("\n".join(dis(r,a,int(sys.argv[3]))))
    else: print("\n".join(dis_fn(r,a)))
