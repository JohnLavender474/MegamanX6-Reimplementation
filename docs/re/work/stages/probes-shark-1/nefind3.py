import struct
from pathlib import Path
from mmxdemake.mips import disassemble
srcs = [('ov06', Path.home().joinpath('mmx-data/x6/extracted/retail/rock_bin/06.bin').read_bytes(), 0x800E9860),
        ('ov07', Path.home().joinpath('mmx-data/x6/extracted/retail/rock_bin/07.bin').read_bytes(), 0x800E9860),
        ('main', Path.home().joinpath('mmx-data/x6/extracted/retail/iso/SLUS_013.95').read_bytes()[0x800:], 0x80010000)]
for name, data, base in srcs:
    for i in range(0, len(data) - 3, 4):
        w = struct.unpack_from('<I', data, i)[0]
        if (w >> 26) in (0x20, 0x24) and (w & 0xFFFF) in (0xD30A, 0x043A):
            print(name, hex(base + i), disassemble(w, base + i))
