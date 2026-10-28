import sys, time
from mmxdemake import rekit
from mmxdemake.codewalk import constants
path, ram, scratch, regions = rekit._load(rekit.DEFAULT_DUMPS, rekit.DEFAULT_EXTRACTED, sys.argv[1])
for lo, hi in regions:
    t = time.time()
    c = constants(ram, lo, hi)
    print(hex(lo), hex(hi), len(c), round(time.time() - t, 2))
    vals = {}
    for pc, v in c:
        vals.setdefault(v, []).append(pc)
    for a in sys.argv[2:]:
        a = int(a, 16)
        for k in range(0, 0x20, 4):
            if a + k in vals:
                print("  ", hex(a + k), [hex(x) for x in vals[a + k]])
