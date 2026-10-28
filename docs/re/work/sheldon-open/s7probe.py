import sys, struct, os
sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.mips import disassemble
EXT = Path.home() / 'mmx-data/x6/extracted/retail'
DUMPS = Path.home() / 'mmx-data/x6/dumps'
_exe = (EXT / 'iso/SLUS_013.95').read_bytes()
_cache = {}
def ram(state):
    if state not in _cache:
        p = DUMPS / (state if state.endswith('.gz') else state + '.state.gz')
        d = read_state(p)
        _cache[state] = (ram_from(d, _exe, p), d)
    return _cache[state][0]
def w(r, a): return struct.unpack_from('<I', r, a & 0x1FFFFF)[0]
def h(r, a): return struct.unpack_from('<H', r, a & 0x1FFFFF)[0]
def sh(r, a): return struct.unpack_from('<h', r, a & 0x1FFFFF)[0]
def b(r, a): return r[a & 0x1FFFFF]
def sb(r, a): v = r[a & 0x1FFFFF]; return v - 256 if v > 127 else v
def hexd(r, a, n): return ' '.join(f'{x:02x}' for x in r[a & 0x1FFFFF:(a & 0x1FFFFF) + n])
def dis(r, a, n):
    for i in range(n):
        pc = a + 4 * i
        print(f'{pc:08x}: {w(r, pc):08x}  {disassemble(w(r, pc), pc)}')
