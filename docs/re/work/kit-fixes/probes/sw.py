import sys
from mmxdemake import rekit
from mmxdemake.codewalk import switches, function_end
path, ram, scratch, regions = rekit._load(rekit.DEFAULT_DUMPS, rekit.DEFAULT_EXTRACTED, sys.argv[1])
for a in sys.argv[2:]:
    f = int(a, 16)
    end = function_end(ram, f)
    print(hex(f), hex(end), [(hex(pc), hex(t), len(ts), hex(min(ts)), hex(max(ts))) for pc, t, ts in switches(ram, f, end)])
