import sys, struct, json
from pathlib import Path
sys.path.insert(0, str(Path.home()/'IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
X = Path.home()/'mmx-data/x6'
exe = (X/'extracted/retail/iso/SLUS_013.95').read_bytes()
def load(name): return read_ram(X/'dumps'/name, exe)
class R:
    def __init__(s, name): s.ram = load(name)
    def r(s, a, n): return bytes(s.ram[a&0x1fffff:(a&0x1fffff)+n])
    def b(s, a): return s.ram[a&0x1fffff]
    def h(s, a): return struct.unpack('<h', s.r(a,2))[0]
    def H(s, a): return struct.unpack('<H', s.r(a,2))[0]
    def w(s, a): return struct.unpack('<I', s.r(a,4))[0]
    def i(s, a): return struct.unpack('<i', s.r(a,4))[0]
