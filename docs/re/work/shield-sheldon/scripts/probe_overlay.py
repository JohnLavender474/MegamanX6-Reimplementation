import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))
from peek import load, R
r = R(load())
ov = (Path.home()/"mmx-data/x6/extracted/retail/rock_bin/10.bin").read_bytes()
ram = r.bytes(0x800E9860, len(ov))
diff = [i for i in range(len(ov)) if ov[i] != ram[i]]
print("overlay 10 bytes", len(ov), "differ", len(diff))
runs=[]
for i in diff:
    if runs and i-runs[-1][1] <= 4: runs[-1][1]=i
    else: runs.append([i,i])
for a,b in runs[:80]: print(hex(0x800E9860+a), b-a+1)
code_lo, code_hi = 0x800F44A0-0x800E9860, 0x800FA644-0x800E9860
print("diffs in boss code", sum(1 for i in diff if code_lo<=i<code_hi))
for n in range(25, 59):
    p = Path.home()/f"mmx-data/x6/extracted/retail/rock_bin/{n:02d}.bin"
    d = p.read_bytes()
    m = r.bytes(0x800FA000, min(len(d), 0x2000))
    same = sum(1 for i in range(len(m)) if m[i]==d[i])
    if same > 0.9*len(m): print("boss overlay candidate", n, same, len(m))
