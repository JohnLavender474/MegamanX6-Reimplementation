from zprobe import *
r = load(sys.argv[1] if len(sys.argv)>1 else STATES[0])
print("sabre table 0x801ee68c (type-24)*4:")
for t in range(24, 24+30):
    b = bytes_at(r, 0x801ee68c + 4*(t-24), 4)
    print(t, list(b))
print("combo sounds 0x801ee3bc:", list(bytes_at(r,0x801ee3bc,8)))
for name,a,n in (("box_n",0x801ee414,95),("box_x",0x801ee580,95)):
    print(name)
    for i in range(n):
        b = struct.unpack_from("<bbBB", r, (a+4*i)&0x1fffff)
        print(i, b, end="; ")
    print()
print("terrain 0x801ee2d4 boxes (idx 75..79):")
for i in range(75,80):
    print(i, struct.unpack_from("<bbBB", r, (0x801ee2d4+4*i)&0x1fffff))
