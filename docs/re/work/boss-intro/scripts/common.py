import sys, struct
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.vram import read_vram
EXT = Path.home() / 'mmx-data/x6/extracted/retail'
EXE_FILE = (EXT / 'iso/SLUS_013.95').read_bytes()
STATE_PATH = Path.home() / 'mmx-data/x6/dumps/wolfang-0_boss-intro.state.gz'

def load(path=STATE_PATH):
    st = read_state(path)
    return ram_from(st, EXE_FILE, path), read_vram(st)

def w32(ram, a): return struct.unpack_from('<I', ram, a & 0x1FFFFF)[0]
def h16(ram, a): return struct.unpack_from('<H', ram, a & 0x1FFFFF)[0]
def b8(ram, a): return ram[a & 0x1FFFFF]
