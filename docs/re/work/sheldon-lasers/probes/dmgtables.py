import os
EXE=open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'),'rb').read()
def tbl(addr):
    off=addr-0x80010000+0x800
    b=EXE[off:off+160]
    out=[]
    for i in range(80):
        c,d=b[2*i],b[2*i+1]
        out.append('D' if c==0xff else '-' if c==2 else str(d) if c==0 else f'c{c}:{d}')
    return out
for a in (0x8007451c,0x800745bc):
    t=tbl(a); print(hex(a))
    for r in range(5): print(f'  {16*r}-{16*r+15}:',' '.join(t[16*r:16*r+16]))
