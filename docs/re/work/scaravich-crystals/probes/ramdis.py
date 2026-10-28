import sys, os, struct, json
sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
X = os.path.expanduser('~/mmx-data/x6/extracted/retail')
D = os.path.expanduser('~/mmx-data/x6/dumps')
H = json.load(open(os.path.expanduser('~/IdeaProjects/mmx-demake-tools/catalog/helpers.json')))
NAMES = {int(k, 16): v['name'] for k, v in H['functions'].items()}
GL = {int(k, 16): v['name'] for k, v in H['globals'].items()}

def ram(state):
    exe = open(os.path.join(X, 'iso/SLUS_013.95'), 'rb').read()
    return read_ram(os.path.join(D, state), exe)

def u32(r, a): return struct.unpack_from('<I', r, a & 0x1FFFFF)[0]
def u16(r, a): return struct.unpack_from('<H', r, a & 0x1FFFFF)[0]
def u8(r, a): return r[a & 0x1FFFFF]

def dis(r, lo, hi):
    out = []
    for a in range(lo, hi, 4):
        w = u32(r, a)
        t = disassemble(w, a)
        if (w >> 26) == 3:
            tgt = ((a & 0xF0000000) | ((w & 0x3FFFFFF) << 2))
            if tgt in NAMES: t += '  ; ' + NAMES[tgt]
        out.append(f'{a:08x}: {w:08x}  {t}')
    return '\n'.join(out)

if __name__ == '__main__':
    r = ram(sys.argv[1])
    lo = int(sys.argv[2], 16); n = int(sys.argv[3])
    print(dis(r, lo, lo + 4 * n))
