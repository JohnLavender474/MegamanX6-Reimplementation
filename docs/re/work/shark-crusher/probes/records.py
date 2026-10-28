import sys
from pathlib import Path
from collections import Counter
sys.path.insert(0, str(Path.home()/'IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
from mmxdemake.objects import read_list, decode
X = Path.home()/'mmx-data/x6'
ram = read_ram(X/'dumps'/sys.argv[1], (X/'extracted/retail/iso/SLUS_013.95').read_bytes())
for area in (0, 1):
    try: recs = decode(read_list(ram, int(sys.argv[2]), area), 99, 99)
    except Exception as e: print(area, e); continue
    print('area', area, len(recs), Counter((r['kind'] & 15, r['b']) for r in recs))
    for r in recs:
        if (r["kind"]&15) in tuple(int(k) for k in sys.argv[3].split(",")): print(' ', r)
