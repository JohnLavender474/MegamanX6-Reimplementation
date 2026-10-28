import sys, struct, json
from pathlib import Path
sys.path.insert(0, str(Path.home()/'IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
X = Path.home()/'mmx-data/x6'
exe = (X/'extracted/retail/iso/SLUS_013.95').read_bytes()
ram = read_ram(X/'dumps'/sys.argv[1], exe)
H = json.load(open(Path.home()/'IdeaProjects/mmx-demake-tools/catalog/helpers.json'))['functions']
N = {int(k,16): v['name'] for k, v in H.items()}
a, b = int(sys.argv[2],16), int(sys.argv[3],16)
for pc in range(a, b, 4):
    x = struct.unpack_from('<I', ram, pc&0x1fffff)[0]
    d = disassemble(x, pc)
    if (x & 0xffff0000) == 0x27bd0000 and (x & 0x8000): print(f'; ==== {pc:08x}')
    t = None
    if x>>26 == 3: t = ((pc+4)&0xf0000000)|((x&0x3ffffff)<<2)
    print(f'{pc:08x}: {x:08x}  {d}' + (f'  ; {N[t]}' if t in N else ''))
