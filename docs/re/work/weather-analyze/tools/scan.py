import struct,sys
sys.path.insert(0,'/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.mips import s16, written, disassemble
ram=open('ram.bin','rb').read()
targets={int(a,16) for a in sys.argv[1].split(',')}
ranges=[(0x80010000,0x80074000),(0x800E9860,0x80100000)]
for lo,hi in ranges:
    known={}
    for pc in range(lo,hi,4):
        w=struct.unpack('<I',ram[pc-0x80000000:pc-0x80000000+4])[0]
        op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31
        if op>=0x20 and op<=0x2e and rs in known:
            a=(known[rs]+s16(w))&0xffffffff
            if a in targets: print(hex(pc),disassemble(w,pc),hex(a))
        if op==0xf: known[rt]=(w&0xffff)<<16
        elif op==9 and rs in known: known[rt]=(known[rs]+s16(w))&0xffffffff
        else:
            r=written(w)
            if r is not None: known.pop(r,None)
        if w==0x03e00008: known={}
