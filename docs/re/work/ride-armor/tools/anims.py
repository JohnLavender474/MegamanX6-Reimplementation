import sys
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from mmxdemake import rekit
path, ram, scratch, regions = rekit._load(rekit.DEFAULT_DUMPS, rekit.DEFAULT_EXTRACTED, sys.argv[1])
table = int(sys.argv[2], 16)
res = rekit.animation_list(ram, table, regions, None, 77, 40)
anims = res[0] if isinstance(res, tuple) else res
print(type(res))
for i, a in enumerate(anims): print(i, a)
