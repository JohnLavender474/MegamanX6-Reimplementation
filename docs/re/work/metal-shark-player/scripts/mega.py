import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))
from peek import load, R
for s in sys.argv[1:]:
    r = R(load(s))
    M = 0x800970A0
    print(s, "form", r.b(M+0xE7), "93..98", [r.sb(M+o) for o in range(0x93, 0x99)], "9B 9D", r.b(M+0x9B), r.b(M+0x9D), "C9", hex(r.b(M+0xC9)), "energy?", r.bytes(M+0xA8, 0x10).hex(" "))
