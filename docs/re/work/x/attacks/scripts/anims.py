from probe import *
r=load(STATES[2])
def dump(tbl, n):
    p=u32(r,tbl+4*n); out=[]
    i=0
    while i<40:
        d,f,nx,sp=struct.unpack_from("<BBbB", r, (p+4*i)&0x1FFFFF)
        out.append((d,hex(f),nx,sp))
        if nx==0: break
        if nx<0: break
        i+=nx
    return hex(p), out
for n in [21,22,23,24,25,26,27,28,39]:
    print("789c", n, dump(0x8007789c, n))
for n in [0,1,2]:
    print("7a80", n, dump(0x80077a80, n))
