import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.mips import disassemble
EXE = Path.home()/'mmx-data/x6/extracted/retail/iso/SLUS_013.95'
DUMPS = Path.home()/'mmx-data/x6/dumps'

def load(name):
    exe_path = EXE
    if not exe_path.exists():
        cands = list((Path.home()/'mmx-data/x6/extracted/retail/iso').glob('SLUS*'))
        exe_path = cands[0]
    data = read_state(DUMPS/(name + '.state.gz'))
    return ram_from(data, exe_path.read_bytes(), name)

def w(ram, a): return struct.unpack_from('<I', ram, a & 0x1FFFFF)[0]
def h(ram, a): return struct.unpack_from('<h', ram, a & 0x1FFFFF)[0]
def b(ram, a): return ram[a & 0x1FFFFF]

def dis(ram, a, n):
    out = []
    for i in range(n):
        pc = a + 4*i
        out.append(f'{pc:08x}: {w(ram,pc):08x}  {disassemble(w(ram,pc), pc)}')
    return '\n'.join(out)

def calls_to(ram, target, lo=0x80010000, hi=0x80200000):
    word = 0x0C000000 | ((target >> 2) & 0x3FFFFFF)
    res = []
    for a in range(lo, hi, 4):
        if w(ram, a) == word: res.append(a)
    return res

def words_eq(ram, val, lo=0x80010000, hi=0x80200000):
    return [a for a in range(lo, hi, 4) if w(ram, a) == val]
