import sys, os, struct, json
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
names = {}
try:
    h = json.load(open(os.path.expanduser('~/IdeaProjects/mmx-demake-tools/catalog/helpers.json')))
    for sec in h.values():
        if isinstance(sec, dict):
            for k, v in sec.items():
                try: names[int(k, 16)] = v.get('name') if isinstance(v, dict) else v
                except Exception: pass
except Exception as e:
    print('helpers', e)
lo, hi = int(sys.argv[2], 16), int(sys.argv[3], 16)
for pc in range(lo, hi, 4):
    w = struct.unpack_from('<I', r, pc & 0x1fffff)[0]
    t = disassemble(w, pc)
    extra = ''
    if w >> 26 in (2, 3):
        tgt = ((pc + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
        extra = f'  -> {tgt:#x} {names.get(tgt, "")}'
    print(f'{pc:#x}: {w:08x}  {t}{extra}')
