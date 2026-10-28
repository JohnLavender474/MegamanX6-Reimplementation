import sys
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake import rekit
from mmxdemake.animfind import ObjectClass
VIS = ObjectClass("visual", 0x80093C98, 32, 0xA0, 58)
rekit.CLASSES["visual"] = (VIS, 0x84)
rekit.found_tables = lambda *a, **k: []
rekit.sprite_count = lambda *a, **k: None
state, type_, out = sys.argv[1], int(sys.argv[2]), sys.argv[3]
sys.exit(rekit.run(None, state, type_, "visual", rekit.DEFAULT_DUMPS, rekit.DEFAULT_EXTRACTED,
                   rekit.DEFAULT_CATALOG, rekit.DEFAULT_ROOT, rekit.Path(out)))
