import sys, struct, json
from common import *
from mmxdemake.mips import disassemble
H = json.load(open('/home/johnlavender/IdeaProjects/mmx-demake-tools/catalog/helpers.json'))
names = {}
for k, v in H.items():
    if isinstance(v, dict):
        for a, e in v.items():
            if a.startswith('0x') and isinstance(e, dict) and 'name' in e:
                names[int(a, 16)] = e['name']
def dis(data, base, start, end, out):
    for pc in range(start, end, 4):
        w = struct.unpack_from('<I', data, pc - base)[0]
        s = disassemble(w, pc)
        import re
        for m in re.findall(r'0x[0-9a-fA-F]{8}', s):
            if int(m, 16) in names: s += f'   ; {names[int(m,16)]}'
        out.write(f'{pc:08X}: {w:08X}  {s}\n')
if __name__ == '__main__':
    which = sys.argv[1]
    if which == 'ov17':
        d = open(EXT/'rock_bin/17.bin','rb').read(); base = 0x800E9860
        dis(d, base, base, base + len(d)//4*4, sys.stdout)
    else:
        ram, _ = load()
        a, b = int(sys.argv[1], 16), int(sys.argv[2], 16)
        dis(ram, 0x80000000, a, b, sys.stdout)
