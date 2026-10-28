import struct
from pathlib import Path
from mmxdemake.mips import disassemble
data = Path.home().joinpath('mmx-data/x6/extracted/retail/iso/SLUS_013.95').read_bytes()[0x800:]
base = 0x80010000
for i in range(0, len(data) - 3, 4):
    w = struct.unpack_from('<I', data, i)[0]
    if (w >> 26) in (0x20, 0x24, 0x28) and (w & 0xFFFF) == 0xD30A:
        print(hex(base + i), disassemble(w, base + i), '|', disassemble(struct.unpack_from('<I', data, i + 4)[0], base + i + 4), '|', disassemble(struct.unpack_from('<I', data, i + 8)[0], base + i + 8))
