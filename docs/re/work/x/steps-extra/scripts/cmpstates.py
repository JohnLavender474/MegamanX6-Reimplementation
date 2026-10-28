import sys, struct, hashlib
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
D = Path.home()/"mmx-data/x6/dumps"
M, G = 0x970A0, 0xCCED0
u32 = lambda r, a: struct.unpack_from("<I", r, a)[0]
for n in sys.argv[1:]:
    p = D/n; r = ram_from(read_state(p), exe, p)
    m = r[M:M+0x160]
    print(n, "var", r[M+2], "form", r[M+0xE7], "g5E", r[G+0x5E], "g5F", hex(r[G+0x5F]), "g60", hex(r[G+0x60]), "g69", hex(r[G+0x69]),
          "ov sha", hashlib.sha1(r[0x1EA000:0x1EA000+44588]).hexdigest()[:8])
    print("  +30", hex(u32(r, M+0x30)), "+38", hex(u32(r, M+0x38)), "+3C", hex(u32(r, M+0x3C)), "+EC", hex(u32(r, M+0xEC)), "+47", r[M+0x47])
    print("  g74..", r[G+0x74:G+0x7C].hex(), "g5B", r[G+0x5B:G+0x5D].hex())
    print("  +F0..+120", " ".join(f"{struct.unpack_from('<i', r, M+o)[0]/65536:.4f}" for o in range(0xF0, 0x124, 4)))
    steps = [u32(r, 0x73ACC + 4*k) for k in range(78)]
    print("  steps32+", " ".join(f"{s:x}" for s in steps[32:]))
