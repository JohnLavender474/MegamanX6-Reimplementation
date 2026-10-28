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
        x = w(pc); op = x>>26; rs=(x>>21)&31; rt=(x>>16)&31; imm=x&0xffff; s=imm-0x10000 if imm&0x8000 else imm
        if op == 0x0f: regs[rt] = (imm<<16, pc); continue
        if rs in regs and pc - regs[rs][1] < 64 and op in (0x09,0x20,0x21,0x23,0x24,0x25,0x28,0x29,0x2b):
            t = (regs[rs][0] + s) & 0xffffffff
            if lo <= t < hi: print(f'{pc:08x}: {disassemble(x,pc)}  -> {t:#x}')
