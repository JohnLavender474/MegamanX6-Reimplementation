import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, RAM_OFFSET, RAM_SIZE
st = read_state(sys.argv[1]); ram = st[RAM_OFFSET:RAM_OFFSET+RAM_SIZE]
def b(a): return ram[a & 0x1FFFFF]
def h(a): return struct.unpack_from('<h', ram, a & 0x1FFFFF)[0]
def H(a): return struct.unpack_from('<H', ram, a & 0x1FFFFF)[0]
def w(a): return struct.unpack_from('<I', ram, a & 0x1FFFFF)[0]
classes = [("main",0x8008EF48,48,0x9C),("stage",0x800C1220,80,0xA4),("visual",0x80093C98,32,0xA0),
 ("misc",0x800CD410,96,0x60),("misc2",0x800964A0,32,0x60),("effect",0x80091298,96,0x70),
 ("weapon",0x800950A0,32,0xA0),("logic",0x8009B7B0,96,0x30),("kind7",0x800C4778,4,0x30),("actor2",0x800CD340,1,0xB0)]
g=0x800CCED0
print("stage",b(g+0x1A) if False else '', "cam?", "mega x,y", h(0x800970A0+0x0A), h(0x800970A0+0x0E))
only = sys.argv[2:] 
for name,base,n,sz in classes:
    if only and name not in only: continue
    for i in range(n):
        a = base + i*sz
        if b(a) == 0: continue
        rec = w(a+0x10)
        extra = ''
        if sz >= 0x48:
            extra = f" tbl={w(a+0x30):#x} spr={w(a+0x3C):#x} tex={H(a+0x40):#x} pal={H(a+0x42):#x} anim={b(a+0x44)} s47={b(a+0x47)}"
        print(f"{name}[{i}] @{a:#x} f={b(a):#x} type={b(a+1)} var={b(a+2)} disp={b(a+3)} state={b(a+4)} step={b(a+5)} step2={b(a+6)} x={h(a+0x0A)} y={h(a+0x0E)} rec={rec:#x}{extra}")
