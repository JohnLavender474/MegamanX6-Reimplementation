import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))
from peek import load, R, dmg_table, EXE
r = R(load())
exe = EXE.read_bytes()
for name, a in (("sheldon", 0x80074E7C), ("sheldon_guard", 0x80074F1C), ("deflect", 0x8007433C)):
    off = a - 0x80010000 + 0x800
    same = exe[off:off+160] == bytes(r.bytes(a, 160))
    t = dmg_table(r, a)
    print(name, hex(a), "exe match", same)
    for i in range(0, 80, 16):
        print(f"  {i}-{i+15}: " + " ".join(t[i:i+16]))
