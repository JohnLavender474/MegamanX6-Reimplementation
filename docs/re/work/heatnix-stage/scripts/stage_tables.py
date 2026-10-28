"""Print the checkpoint respawn records (checkpoint_start_tbl 0x80073300) and the camera zone
table (camera_zone_tbl 0x80079D70) of the stage and area of a save state.
Usage (repo root, PYTHONPATH=.): python3 stage_tables.py <state.gz> [zones to list]"""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram

EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
DUMPS = Path.home() / "mmx-data/x6/dumps"
ram = read_ram(DUMPS / sys.argv[1], EXE.read_bytes())
nz = int(sys.argv[2]) if len(sys.argv) > 2 else 16
W = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
stage, area = ram[0x800CCEDC & 0x1FFFFF], ram[0x800CCEDD & 0x1FFFFF]
word = stage * 2 + area
lst = W(0x80073300 + 4 * word)
nxt = W(0x80073300 + 4 * (word + 1))
print(f"stage {stage} area {area}: checkpoint list {lst:#x} (next list {nxt:#x})")
for c in range(12):
    p = W(lst + 4 * c)
    if not (0x80000000 <= p < 0x80200000) or (nxt and lst + 4 * c >= nxt):
        break
    v = struct.unpack_from("<12h", ram, p & 0x1FFFFF)
    face = ram[(p + 0x18) & 0x1FFFFF]
    beam = ram[(p + 0x22) & 0x1FFFFF]
    print(f"  cp {c} @{p:#x}: player {v[0]},{v[1]} cam0 {v[2]},{v[3]} cam1 {v[4]},{v[5]} cam2 {v[6]},{v[7]} "
          f"limits x {v[8]}-{v[9]} y {v[10]}-{v[11]} facing {face:#x} offsets "
          f"{struct.unpack_from('<4h', ram, (p + 0x1A) & 0x1FFFFF)} beam {beam}")
zt = W(0x80079D70 + 4 * word)
print(f"camera zones {zt:#x}")
for i in range(nz):
    x0, x1, y0, y1, *lim = struct.unpack_from("<8H", ram, (zt + 16 * i) & 0x1FFFFF)
    lim = ["keep" if v == 0xFFFF else v for v in lim]
    print(f"  zone {i}: x {x0}-{x1} y {y0}-{y1} limits min x {lim[0]} max x {lim[1]} min y {lim[2]} max y {lim[3]}")
