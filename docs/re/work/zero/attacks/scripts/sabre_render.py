import sys, struct
from pathlib import Path
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
import mmxdemake.playerexport as pe
from mmxdemake.savestate import read_state
from mmxdemake.sprites import scratchpad
from mmxdemake.vram import read_vram

state, out = sys.argv[1], Path(sys.argv[2])
dumps = Path('/home/johnlavender/mmx-data/x6/dumps')
data = read_state(dumps / state)
base = struct.unpack_from('<I', scratchpad(data), 0x24)[0]
ram = pe.ram_from(data, (Path(pe.DEFAULT_EXTRACTED) / pe.EXE).read_bytes(), dumps / state)
for n in range(3, 9):
    words = bytes(pe.ram_at(ram, base + 32 * n, 32))
    def patched(d, words=words):
        v = bytearray(read_vram(d))
        x, y = (0x7800 & 63) * 16, 0x7800 >> 6
        v[y * pe.ROW + x * 2:y * pe.ROW + x * 2 + 32] = words
        return bytes(v)
    pe.read_vram = patched
    pe.run(state, dumps, Path(pe.DEFAULT_EXTRACTED), Path(pe.DEFAULT_CATALOG), out / f'pal{n}', log=print)
