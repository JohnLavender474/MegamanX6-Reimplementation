from zprobe import *
r=load(STATES[0])
print("Zero cost tbl 0x80073DF4:", [s16(r,0x80073DF4+2*i) for i in range(10)])
print("X cost tbl 0x80073D74:", [s16(r,0x80073D74+2*i) for i in range(10)])
