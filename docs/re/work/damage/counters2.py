import struct
from pathlib import Path
root = Path.home() / "mmx-data/x6/extracted/retail"
files = [root / "iso/SLUS_013.95"] + sorted((root / "rock_bin").glob("*.bin"))
MEM = {0x20: "lb", 0x24: "lbu", 0x21: "lh", 0x25: "lhu", 0x23: "lw", 0x28: "sb", 0x29: "sh", 0x2B: "sw"}
for f in files:
    data = f.read_bytes()
    exe = f.name.startswith("SLUS")
    conv = (lambda o: o + 0x80010000 - 0x800) if exe else (lambda o: o)
    words = struct.unpack_from(f"<{len(data)//4}I", data)
    hi, base = {}, {}
    for i, w in enumerate(words):
        op, rs, rt, imm = w >> 26, (w >> 21) & 31, (w >> 16) & 31, w & 0xFFFF
        if op in MEM and imm in (0x98, 0x9C) and rs in base and i - base[rs] < 400:
            print(f.name, hex(conv(4 * i)), MEM[op], f"game+{imm:#x}")
        if op == 15:
            hi[rt] = i if imm == 0x800D else -999
            base.pop(rt, None)
        elif op == 9:
            if imm == 0xCED0 and rs in hi and i - hi[rs] < 400:
                base[rt] = i
            else:
                base.pop(rt, None)
        elif op in (0x20, 0x24, 0x21, 0x25, 0x23):
            base.pop(rt, None)
