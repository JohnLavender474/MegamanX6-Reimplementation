import struct, sys
from pathlib import Path
root = Path.home() / "mmx-data/x6/extracted/retail"
files = [root / "iso/SLUS_013.95"] + sorted((root / "rock_bin").glob("*.bin"))
OPS = {0x20: "lb", 0x24: "lbu", 0x21: "lh", 0x25: "lhu", 0x23: "lw", 0x28: "sb", 0x29: "sh", 0x2B: "sw"}
for f in files:
    data = f.read_bytes()
    exe = f.name.startswith("SLUS")
    conv = (lambda o: o + 0x80010000 - 0x800) if exe else (lambda o: o)
    for off in range(0, len(data) - 3, 4):
        w = struct.unpack_from("<I", data, off)[0]
        op, imm = w >> 26, w & 0xFFFF
        if op in OPS and imm in (0xCF68, 0xCF6C, 0xCF69, 0xCF6A, 0xCF6D):
            print(f.name, hex(conv(off)), OPS[op], hex(imm))
        if op == 9 and imm in (0xCF68, 0xCF6C):
            print(f.name, hex(conv(off)), "addiu", hex(imm))
