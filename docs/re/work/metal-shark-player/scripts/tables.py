import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))
from peek import load, R
r = R(load(sys.argv[1] if len(sys.argv) > 1 else "shark-7_boss.state.gz"))
AT = 0x800F952C
ptrs = [r.w(AT + 4 * i) for i in range(40)]
order = sorted(set(ptrs))
for i, p in enumerate(ptrs):
    nxt = min([q for q in order if q > p] + [0x800F95CC])
    n = (nxt - p) // 4
    fr = [r.bytes(p + 4 * k, 4) for k in range(n)]
    print(f"anim {i:2d} {p:#x} {n} frames: " + " ".join(f"{b[3]}/{b[0]}/{b[1]}/{b[2]}" for b in fr))
print("box by sprite (sprite: attack box addr)")
row = []
for k in range(95):
    t = r.w(0x800E9B1C + 4 * k)
    row.append(f"{k+10}:{t & 0xffff:04x}")
print(" ".join(row))
