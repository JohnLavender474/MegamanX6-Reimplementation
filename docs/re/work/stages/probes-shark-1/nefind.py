import struct, sys
from pathlib import Path
from mmxdemake.mips import disassemble as disasm
ov = int(sys.argv[1])
data = Path.home().joinpath(f'mmx-data/x6/extracted/retail/rock_bin/{ov:02d}.bin').read_bytes()
base = 0x800E9860
for i in range(0, len(data) - 3, 4):
    w = struct.unpack_from('<I', data, i)[0]
    op = w >> 26
    if op in (0x20, 0x24, 0x28) and (w & 0xFFFF) == 0xD30A:
        a = base + i
        for j in range(max(0, i - 8), i + 40, 4):
            print(hex(base + j), disasm(struct.unpack_from('<I', data, j)[0], base + j))
        print('--')
