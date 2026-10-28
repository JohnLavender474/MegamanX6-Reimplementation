import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from
EXE = (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()

def u32(r, a): return struct.unpack_from("<I", r, a & 0x1FFFFF)[0]

for p in sys.argv[1:]:
    r = ram_from(read_state(p), EXE, p)
    print("==", Path(p).name)
    for slot in range(4):
        t = 0x801F8100 + slot * 0x80
        tcb = u32(r, t + 0x0C)
        st = struct.unpack_from("<H", r, t & 0x1FFFFF)[0]
        if not st:
            continue
        regs = [u32(r, tcb + 8 + 4 * i) for i in range(32)]
        epc = u32(r, tcb + 0x88)
        print(f" slot {slot} state {st} tcb {tcb:08x} epc {epc:08x} ra {regs[31]:08x} sp {regs[29]:08x} a0 {regs[4]:08x} s0 {regs[16]:08x}")
        sp = regs[29]
        top = 0x801FEC00 + slot * 0x400
        words = [u32(r, a) for a in range(sp, min(sp + 0x200, top + 0x400), 4)]
        print("   code words on stack:", [f"{w:08x}" for w in words if 0x80010000 <= w < 0x80070000 or 0x800E9000 <= w < 0x80110000])
