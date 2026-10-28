import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))
from peek import load, R
r = R(load(sys.argv[1] if len(sys.argv) > 1 else "shark-7_boss.state.gz"))
G = 0x800CCED0
print("game stage/area", r.b(G+0xC), r.b(G+0xD), "cp", r.b(G+0x1D), "diff", r.b(G+0x68), "boss_hp_max", r.b(G+0x8C), "8D", r.b(G+0x8D), r.b(G+0x8E), "level", r.b(G+0x8F), "char", r.b(G+0x38), "form", r.b(G+0x5E), "+20", hex(r.w(G+0x20)), "+24", r.b(G+0x24), "+2E", r.b(G+0x2E), "exit", r.b(G+0xF))
print("ovl vars 9868..987c", r.bytes(0x800F9868, 0x14).hex(" "))
cam = 0x800971F8
print("cam x,y", r.sh(cam+0xA), r.sh(cam+0xE), "limits", [r.sh(cam+o) for o in (0x1C,0x1E,0x20,0x22,0x24,0x26,0x28,0x2A)])
M = 0x800970A0
print("mega x,y", r.sh(M+0x1A), r.sh(M+0x1E), "step", r.b(M+5), "hp", r.b(M+0x5C), "D0", r.b(M+0xD0), r.b(M+0xD1))
def dump(base, n, size, label):
    for i in range(n):
        a = base + i*size
        if r.b(a) == 0: continue
        yield a
for a in dump(0x8008EF48, 48, 0x9C, "main"):
    t = r.b(a+1)
    print(f"main {hex(a)} type {t} var {r.b(a+2)} flags {r.b(a):#x} disp {r.b(a+3)} state {r.b(a+4)} step {r.b(a+5)} step2 {r.b(a+6)} act4 {r.sb(a+7)}")
    print(f"  pos {r.sh(a+0xA)},{r.sh(a+0xE)} rel {r.sh(a+0xA)-r.sh(cam+0xA)},{r.sh(a+0xE)-r.sh(cam+0xE)} flip {r.b(a+0x15):#x} prio {r.b(a+0x16)} anim {r.b(a+0x17)} frame {r.bytes(a+0x44,4).hex()} hp {r.sb(a+0x5C)} contact {r.b(a+0x60)} iframes {r.b(a+0x61)}")
    print(f"  spd {r.sw(a+0x20)/65536:.3f},{r.sw(a+0x24)/65536:.3f} acc {r.sw(a+0x28)/65536:.3f} grav {r.sw(a+0x2C)/65536:.4f} tbl {r.w(a+0x58):#x} atk {r.w(a+0x50):#x} hurt {r.w(a+0x54):#x} terr {r.w(a+0x68):#x} anitbl {r.w(a+0x30):#x} spr {r.w(a+0x3C):#x} tex {r.h(a+0x40):#x} clut {r.h(a+0x42):#x}")
    print(f"  7C {r.sh(a+0x7C)} 7E {r.sh(a+0x7E)} 80 {r.w(a+0x80):#x} 84 {r.sh(a+0x84)} 86 {r.sh(a+0x86)} 88 {r.sh(a+0x88)} 8A {r.sb(a+0x8A)} 8B {r.sb(a+0x8B)} 8C {r.sb(a+0x8C)} 8D {r.b(a+0x8D)} 8E {r.b(a+0x8E)} rec {r.w(a+0x10):#x}")
for a in dump(0x80093C98, 32, 0xA0, "vis"):
    print(f"visual {hex(a)} type {r.b(a+1)} var {r.b(a+2)} state {r.b(a+4)} step {r.b(a+5)} sub {r.b(a+6)} pos {r.sh(a+0xA)},{r.sh(a+0xE)} parent {r.w(a+0x7C):#x}")
for a in dump(0x8009B7B0, 96, 0x30, "sys"):
    print(f"sys {hex(a)} type {r.b(a+1)} var {r.b(a+2)} state {r.b(a+4)} step {r.b(a+5)}")
for a in dump(0x80091298, 96, 0x70, "eff"):
    print(f"effect {hex(a)} type {r.b(a+1)} var {r.b(a+2)} state {r.b(a+4)} pos {r.sh(a+0xA)},{r.sh(a+0xE)}")
