import sys
from probe_ram import *
st = sys.argv[1]; lo = int(sys.argv[2],16); hi = int(sys.argv[3],16)
ranges = [(0x80010000, 0x80090000), (0x800E9860, 0x80100000)]
if len(sys.argv) > 4:
    ranges = [(int(sys.argv[4],16), int(sys.argv[5],16))]
ram, scr = load(st)
for a0, a1 in ranges:
    lui = {}
    for pc in range(a0, a1, 4):
        x = u32(ram, pc); op = x >> 26; rs = (x >> 21) & 31; rt = (x >> 16) & 31
        imm = x & 0xFFFF; simm = imm - 0x10000 if imm & 0x8000 else imm
        if op == 0x0F:
            lui[rt] = (imm << 16, pc); continue
        if op in (0x20,0x21,0x23,0x24,0x25,0x28,0x29,0x2B) and rs in lui and pc - lui[rs][1] < 64:
            addr = (lui[rs][0] + simm) & 0xFFFFFFFF
            if lo <= addr < hi:
                kind = {0x20:'lb',0x21:'lh',0x23:'lw',0x24:'lbu',0x25:'lhu',0x28:'sb',0x29:'sh',0x2B:'sw'}[op]
                print(f"{pc:08x} {kind} {addr:08x}")
        if op == 0x09 and rs in lui and pc - lui[rs][1] < 64:
            addr = (lui[rs][0] + simm) & 0xFFFFFFFF
            if lo <= addr < hi: print(f"{pc:08x} addiu {addr:08x}")
        if op in (0x09, 0x0F) or (op == 0 and (x & 0x3F) in (0x21,0x25)):
            pass
        # crude invalidation on writes to the tracked reg
        if op in (0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x20,0x21,0x23,0x24,0x25) and rt in lui and op != 0x0F:
            if not (op == 0x09 and rs == rt):
                lui.pop(rt, None)
