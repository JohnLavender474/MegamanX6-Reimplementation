import glob
import os
import struct
import sys

sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake import objects as O
from mmxdemake.savestate import RAM_OFFSET, RAM_SIZE, ram_at, read_state

FIXED_SET_TBL = 0x80073554
TYPE = int(sys.argv[1]) if len(sys.argv) > 1 else 26
seen = {}
for f in sorted(glob.glob(os.path.expanduser('~/mmx-data/x6/dumps/*.state.gz'))):
    try:
        st = read_state(f)
    except Exception:
        continue
    ram = st[RAM_OFFSET:RAM_OFFSET + RAM_SIZE]
    stage, area = ram[0xCCEDC], ram[0xCCEDD]
    if (stage, area) in seen:
        continue
    p = struct.unpack('<I', ram_at(ram, FIXED_SET_TBL + stage * 8 + area * 4, 4))[0]
    try:
        recs = O.decode(O.list_at(ram, p - 0x80000000), 0, 0, bounded=False)
    except Exception as e:
        seen[(stage, area)] = (os.path.basename(f), 'error', str(e))
        continue
    hits = [(r['index'], r['id']) for r in recs if r['b'] == TYPE and r['kind'] == 0]
    seen[(stage, area)] = (os.path.basename(f), hex(p), hits, 'nightmare_effect', ram[0xCD30A])
for k, v in sorted(seen.items()):
    print(k, v)
