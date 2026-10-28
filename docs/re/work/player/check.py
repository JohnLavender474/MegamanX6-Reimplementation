import sys, struct, hashlib
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
sys.path.insert(0, "/home/johnlavender/mmx-data/x6/probe/sprites")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.vram import read_vram
from wordlz import decode
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
M = 0x970A0
disc = {p: p.read_bytes() for d in ("rock_bin", "rock_dat") for p in sorted((X/d).glob("*")) if p.is_file()}
disc.update({p: p.read_bytes() for p in sorted((X/"iso").rglob("*")) if p.is_file() and p.stat().st_size < 64<<20})
seen = {}
for state in sys.argv[1:]:
    p = Path.home()/"mmx-data/x6/dumps"/state
    data = read_state(p); r = ram_from(data, exe, p); v = read_vram(data)
    tb = struct.unpack_from("<I", r, M+0x38)[0] - 0x80000000
    n = (struct.unpack_from("<I", r, tb)[0] & 0xFFFFF) // 4
    end = max(decode(r, tb + (struct.unpack_from("<I", r, tb+4*i)[0] & 0xFFFFF))[1] for i in range(n))
    blob = bytes(r[tb:end]); h = hashlib.sha1(blob).hexdigest()[:12]
    key = r[M+2]
    if key not in seen:
        seen[key] = h
        hits = [(str(q.relative_to(X)), b.find(blob[:4096])) for q, b in disc.items() if blob[:4096] in b]
        full = [(q, o) for q, o in hits if disc[X/q][o:o+len(blob)] == blob]
        print(f"var {key}: blob sha1 {h}, disc hits (first 4 KB) {hits}, full match {full}")
    else:
        print(f"var {key}: blob sha1 {h} {'same' if h == seen[key] else 'DIFFERENT'} in {state}")
    f = r[M+0x48]; e = struct.unpack_from("<I", r, tb+4*f)[0]
    out, _ = decode(r, tb + (e & 0xFFFFF)); cnt = e >> 20
    rows = []
    for k in range(0, cnt, 16):
        chunk = out[k*128:(k+16)*128]; w = 64 if cnt - k >= 16 else (cnt-k)*4
        rows.append((chunk, w))
    y = 0; match = True
    for chunk, w in rows:
        hgt = len(chunk) // (w*2)
        for yy in range(hgt):
            o = ((y+yy)*1024 + 320)*2
            if v[o:o+w*2] != chunk[yy*w*2:(yy+1)*w*2]: match = False
        y += hgt
    print(f"   {state}: frame {f} ({cnt} tiles) matches VRAM (320, 0): {match}")
