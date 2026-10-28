from zprobe import *
r = load(sys.argv[1] if len(sys.argv)>1 else STATES[0])
for t in range(80):
    print(t, hex(u32(r, 0x80071220+4*t)))
