"""Run learn with the tile-info mask widened to bytes 2 and 3 (the whole cell field)."""
import sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake import ramlayout, learn


def without_cells(data):
    out = bytearray(data)
    out[2::4] = bytes(len(out[2::4]))
    out[3::4] = bytes(len(out[3::4]))
    return bytes(out)


ramlayout._without_cells = without_cells
sys.exit(learn.main(sys.argv[1:]))
