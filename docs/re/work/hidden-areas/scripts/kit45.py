import sys
from pathlib import Path

sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
import mmxdemake.rekit as rk

OVL = Path('/home/johnlavender/mmx-data/x6/extracted/retail/rock_bin/45.bin').read_bytes()
BASE = 0x800FA000
orig = rk.ram_from


def ram_with_45(state, exe, path):
    ram = bytearray(orig(state, exe, path))
    o = BASE & 0x1FFFFF
    ram[o:o + len(OVL)] = OVL
    return bytes(ram)


rk.ram_from = ram_with_45
state = sys.argv[1] if len(sys.argv) > 1 else 'turtloid-5_zero.state.gz'
out = sys.argv[2] if len(sys.argv) > 2 else '/home/johnlavender/mmx-data/x6/re/hidden-areas/kit-dynamo'
sys.exit(rk.main(['--state', state, '--type', '63', '--out', out] + sys.argv[3:]))
