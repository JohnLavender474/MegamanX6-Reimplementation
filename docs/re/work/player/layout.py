import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
M = 0x970A0
for state, mem in (("intro-1_start.state.gz", 87), ("turtloid-6_zero-player.state.gz", 85)):
    p = Path.home()/"mmx-data/x6/dumps"/state
    r = ram_from(read_state(p), exe, p)
    b = (X/f"rock_dat/{mem:03d}.bin").read_bytes()
    print(f"{state} / member {mem}: header words {[hex(w) for w in struct.unpack_from('<12I', b, 0)]}")
    print(f"  mega+0x38 {struct.unpack_from('<I', r, M+0x38)[0]:#x} +0x3C {struct.unpack_from('<I', r, M+0x3C)[0]:#x} +0x40 {struct.unpack_from('<H', r, M+0x40)[0]:#x} +0x42 {struct.unpack_from('<H', r, M+0x42)[0]:#x} +0x34 {struct.unpack_from('<I', r, M+0x34)[0]:#x}")
    for o in range(0, len(b), 0x4000):
        chunk = b[o:o+256]
        if chunk.count(0) > 200: continue
        at = r.find(chunk)
        print(f"  member +{o:#07x}: {'RAM ' + hex(at + 0x80000000) if at >= 0 else 'not in RAM'}")
