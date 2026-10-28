import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from mmxdemake import rekit
path, ram, scratch, regions = rekit._load(rekit.DEFAULT_DUMPS, rekit.DEFAULT_EXTRACTED, sys.argv[1])
for name, off, n in (("main",0x80,77),("visual",0x84,58),("effect",0x88,64),("logic",0x8C,45),("misc",0x90,64),("stage",0x94,32)):
    base = struct.unpack_from("<I", scratch, off)[0]
    print(name, hex(base), hex(base+4*n))
    for i in range(n):
        v = struct.unpack_from("<I", ram, base-0x80000000+4*i)[0]
        if len(sys.argv) > 2 and v == int(sys.argv[2],16): print("  type", i, hex(v))
