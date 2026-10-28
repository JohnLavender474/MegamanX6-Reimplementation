import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
D = Path.home()/"mmx-data/x6/dumps"
STATES = ["turtloid-6_zero-player","turtloid-7_boss","turtloid-8_high-max","scaravich-1_start","scaravich-2_totem-section"]
def load(name):
    if not name.endswith(".gz"): name += ".state.gz"
    p = D/name
    return ram_from(read_state(p), exe, p)
def u8(r,a): return r[a & 0x1FFFFF]
def s8(r,a): return struct.unpack_from("<b", r, a & 0x1FFFFF)[0]
def u16(r,a): return struct.unpack_from("<H", r, a & 0x1FFFFF)[0]
def s16(r,a): return struct.unpack_from("<h", r, a & 0x1FFFFF)[0]
def u32(r,a): return struct.unpack_from("<I", r, a & 0x1FFFFF)[0]
def s32(r,a): return struct.unpack_from("<i", r, a & 0x1FFFFF)[0]
def fx(r,a): return s32(r,a)/65536
def bytes_at(r,a,n): return bytes(r[(a & 0x1FFFFF):(a & 0x1FFFFF)+n])
