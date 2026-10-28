from zprobe import *
r=load(sys.argv[1])
for tgt in [int(x,16) for x in sys.argv[2:]]:
    jal = (3<<26)|((tgt>>2)&0x3ffffff)
    hits=[]
    for a in range(0x80010000,0x80200000,4):
        w=u32(r,a)
        if w==jal or w==tgt: hits.append(hex(a)+(" jal" if w==jal else " word"))
    print(hex(tgt),hits)
