from wp import *
r=load(sys.argv[1] if len(sys.argv)>1 else "sheldon-1_lemons")
for i in range(83):
    print(i, hex(u32(r,0x80071220+4*i)))
