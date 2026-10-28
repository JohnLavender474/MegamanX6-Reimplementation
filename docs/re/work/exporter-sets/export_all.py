import sys
from pathlib import Path
code, root, catalog, names = sys.argv[1], Path(sys.argv[2]), Path(sys.argv[3]), Path(sys.argv[4])
sys.path.insert(0, code)
from mmxdemake.spriteexport import run
D = Path.home() / "mmx-data/x6/dumps"
X = Path.home() / "mmx-data/x6/extracted/retail"
for name in names.read_text().split():
    lines = []
    rc = run(name, D, X, catalog, root, log=lines.append)
    print(name, rc, lines[-1] if lines else "")
