import sys, struct, os, json
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
H = json.load(open(os.path.expanduser('~/IdeaProjects/mmx-demake-tools/catalog/helpers.json')))
names = {}
for k, d in H.items():
    if isinstance(d, dict):
        for a, v in d.items():
            try:
                names[int(a, 16)] = v['name'] if isinstance(v, dict) else str(v)
            except (ValueError, TypeError):
                pass
r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + sys.argv[1]), EXE)
for spec in sys.argv[2:]:
    if spec.startswith('w'):
        a, n = spec[1:].split(':')
        a = int(a, 16)
        print(' '.join(f'{struct.unpack_from("<I", r, (a + 4*i) & 0x1fffff)[0]:08x}' for i in range(int(n))))
        continue
    a, n = spec.split(':') if ':' in spec else (spec, '400')
    a = int(a, 16)
    rets = 0
    print('---', hex(a), names.get(a, ''))
    for i in range(int(n)):
        pc = a + 4 * i
        w = struct.unpack_from('<I', r, pc & 0x1fffff)[0]
        t = disassemble(w, pc)
        extra = ''
        if (w >> 26) == 3:
            tgt = (pc & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
            extra = '  ; ' + names.get(tgt, '')
        print(f'{pc:08x}: {w:08x}  {t}{extra}')
        if rets:
            break
        if w == 0x03e00008 and ':' not in spec:
            rets = 1
