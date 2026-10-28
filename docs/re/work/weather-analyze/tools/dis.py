import struct,sys
sys.path.insert(0,'/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.mips import disassemble,s16
ram=open('ram.bin','rb').read()
lo=int(sys.argv[1],16); hi=int(sys.argv[2],16)
for pc in range(lo,hi,4):
    w=struct.unpack('<I',ram[pc-0x80000000:pc-0x80000000+4])[0]
    print(f"{pc:08x}: {w:08x}  {disassemble(w,pc)}")
