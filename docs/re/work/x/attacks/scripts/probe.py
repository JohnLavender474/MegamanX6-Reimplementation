import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
D = Path.home()/"mmx-data/x6/dumps"
def load(name):
    p = D/name
    return ram_from(read_state(p), exe, p)
def u8(r,a): return r[a & 0x1FFFFF]
def s8(r,a): return struct.unpack_from("<b", r, a & 0x1FFFFF)[0]
def u16(r,a): return struct.unpack_from("<H", r, a & 0x1FFFFF)[0]
def s16(r,a): return struct.unpack_from("<h", r, a & 0x1FFFFF)[0]
def u32(r,a): return struct.unpack_from("<I", r, a & 0x1FFFFF)[0]
def s32(r,a): return struct.unpack_from("<i", r, a & 0x1FFFFF)[0]
def bytes_at(r,a,n): return r[(a & 0x1FFFFF):(a & 0x1FFFFF)+n]
STATES = ["sheldon-1_lemons.state.gz","sheldon-2_half-charge.state.gz","sheldon-3_full-charge.state.gz"]
