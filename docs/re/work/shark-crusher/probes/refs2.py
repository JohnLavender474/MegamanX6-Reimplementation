import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home()/'IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
X = Path.home()/'mmx-data/x6'
exe = (X/'extracted/retail/iso/SLUS_013.95').read_bytes()
ram = read_ram(X/'dumps'/sys.argv[1], exe)
lo, hi = int(sys.argv[2],16), int(sys.argv[3],16)
ranges = [(0x80010000, 0x80010000+len(exe)-0x800), (0x800E9860, 0x800E9860+0xA7C0)]
def w(a): return struct.unpack_from('<I', ram, a&0x1fffff)[0]
for a0,a1 in ranges:
    regs = {}
    for pc in range(a0, a1, 4):
        x = w(pc); op = x>>26; rs=(x>>21)&31; rt=(x>>16)&31; rd=(x>>11)&31; imm=x&0xffff; s=imm-0x10000 if imm&0x8000 else imm
        if (x & 0xffff8000) == 0x27bd8000: regs = {}
        if op in (0x20,0x21,0x23,0x24,0x25,0x28,0x29,0x2b) and rs in regs:
            t = (regs[rs] + s) & 0xffffffff
            if lo <= t < hi: print(f'{pc:08x}: {disassemble(x,pc)}  -> {t:#x}')
        if op == 0x0f: regs[rt] = imm<<16; continue
        if op == 0x09 and rs in regs: regs[rt] = (regs[rs]+s)&0xffffffff; continue
        if op == 0 and (x&63)==0x21 and rt==0 and rs in regs: regs[rd]=regs[rs]; continue
        if op == 0 and (x&63)==0x21 and rs==0 and rt in regs: regs[rd]=regs[rt]; continue
        # writes kill
        if op == 0: regs.pop(rd, None)
        elif op in (0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x20,0x21,0x23,0x24,0x25): regs.pop(rt, None)
        elif op == 3: regs.pop(31, None)
