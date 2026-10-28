import os
import struct
import sys

sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.png import write_png
from mmxdemake.psxcolor import to_rgba
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram

st, out = sys.argv[1], sys.argv[2]
vram = read_vram(read_state(os.path.expanduser('~/mmx-data/x6/dumps/' + st)))
rows = []
for y in range(512):
    line = bytearray()
    for x in range(1024):
        c = struct.unpack_from('<H', vram, y * 2048 + x * 2)[0]
        p = to_rgba(c)
        line += bytes((p[0], p[1], p[2], 255))
    rows.append(bytes(line))
write_png(out, 1024, 512, rows)
