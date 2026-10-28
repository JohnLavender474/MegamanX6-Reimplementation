import sys, struct
from mmxdemake import rekit
from mmxdemake.codewalk import class_table, OWNER_TABLES, CLASS_TABLES, _u32
path, ram, scratch, regions = rekit._load(rekit.DEFAULT_DUMPS, rekit.DEFAULT_EXTRACTED, sys.argv[1])
lo, hi = int(sys.argv[2], 16), int(sys.argv[3], 16)
for off, count in OWNER_TABLES + tuple((o, c) for _, o, c in CLASS_TABLES):
    t = class_table(ram, scratch, off)
    for k in range(count):
        f = _u32(ram, t + 4 * k)
        if lo <= f < hi:
            print(hex(off), k, hex(f))
