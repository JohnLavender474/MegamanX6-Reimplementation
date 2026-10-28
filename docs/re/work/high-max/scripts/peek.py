import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from pathlib import Path
from mmxdemake.savestate import read_ram, ram_at
EXE = Path('/home/johnlavender/mmx-data/x6/extracted/retail/iso/SLUS_013.95').read_bytes()
def load(name):
    return read_ram(Path('/home/johnlavender/mmx-data/x6/dumps') / name, EXE)
def u8(r,a): return r[a&0x1fffff]
def s8(r,a): v=u8(r,a); return v-256 if v>127 else v
def u16(r,a): return struct.unpack_from('<H',r,a&0x1fffff)[0]
def s16(r,a): return struct.unpack_from('<h',r,a&0x1fffff)[0]
def u32(r,a): return struct.unpack_from('<I',r,a&0x1fffff)[0]
def s32(r,a): return struct.unpack_from('<i',r,a&0x1fffff)[0]
def hexd(r,a,n):
    b=ram_at(r,a,n)
    for i in range(0,n,16):
        print(f'{a+i:08x}: '+' '.join(f'{x:02x}' for x in b[i:i+16]))
from mmxdemake.savestate import read_state
from mmxdemake.sprites import scratchpad
def load2(name):
    d = read_state(Path('/home/johnlavender/mmx-data/x6/dumps') / name)
    return read_ram(Path('/home/johnlavender/mmx-data/x6/dumps') / name, EXE), scratchpad(d)
