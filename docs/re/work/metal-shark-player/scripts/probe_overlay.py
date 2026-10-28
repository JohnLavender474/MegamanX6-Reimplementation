import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))
from peek import load, R
r = R(load(sys.argv[1] if len(sys.argv) > 1 else "shark-7_boss.state.gz"))
RB = Path.home()/"mmx-data/x6/extracted/retail/rock_bin"
for n in range(0, 59):
    d = (RB/f"{n:02d}.bin").read_bytes()
    for base in (0x800E9860, 0x800FA000):
        m = r.bytes(base, len(d))
        k = min(len(d), len(m), 0x4000)
        same = sum(1 for i in range(k) if m[i] == d[i])
        if same > 0.9 * k:
            full = sum(1 for i in range(len(d)) if m[i] != d[i])
            print(f"overlay {n:02d} at {base:#x}: len {len(d)} end {base+len(d):#x} differ {full}")
