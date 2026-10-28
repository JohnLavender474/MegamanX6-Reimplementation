"""Scan an overlay for loads of a global (lui hi + load/addiu lo) and list type-table entries."""
import struct, sys
from pathlib import Path
ov = int(sys.argv[1]); target = int(sys.argv[2], 16)
data = (Path.home() / f"mmx-data/x6/extracted/retail/rock_bin/{ov:02d}.bin").read_bytes()
base = 0x800E9860
hi = (target + 0x8000) >> 16; lo = target & 0xFFFF
words = struct.unpack(f"<{len(data)//4}I", data[:len(data)//4*4])
for i, w in enumerate(words):
    if (w >> 16) & 0xFFFF and (w & 0xFFFF) == lo and (w >> 26) in (0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B, 0x09):
        rs = (w >> 21) & 31
        for j in range(max(0, i - 12), i):
            u = words[j]
            if u >> 26 == 0x0F and (u & 0xFFFF) == hi and ((u >> 16) & 31) == rs:
                print(hex(base + 4 * i), hex(w)); break
