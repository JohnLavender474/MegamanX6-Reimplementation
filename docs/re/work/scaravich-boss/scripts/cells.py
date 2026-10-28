import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from probe import load, u8, u16, u32, D
from mmxdemake.savestate import read_state
from mmxdemake.sprites import scratchpad

def ctx(name):
    if not name.endswith(".gz"): name += ".state.gz"
    r = load(name); sp = scratchpad(read_state(D/name))
    return r, sp

def cell(r, sp, x, y, layer=0):
    w = u8(r, 0x800CD338); size = u16(r, 0x8008EC10)
    lay, scr, info = (struct.unpack_from('<I', sp, o)[0] for o in (4, 8, 12))
    s = u8(r, lay + layer * size + (y >> 8) * w + (x >> 8))
    v = u16(r, scr + s * 512 + ((y >> 4) & 15) * 32 + ((x >> 4) & 15) * 2)
    return v, u32(r, info + 4 * (v & 0x3FFF)) & 0x3F

X0, X1, Y0, Y1 = 1536, 2048, 1104, 1440
def grid(name):
    r, sp = ctx(name)
    return {(x, y): cell(r, sp, x, y) for y in range(Y0, Y1, 16) for x in range(X0, X1, 16)}

if __name__ == "__main__":
    names = sys.argv[1:]
    gs = [grid(n) for n in names]
    for n, g in zip(names, gs):
        print("==", n, "collision values, x", X0, "...", X1 - 16, "step 16")
        for y in range(Y0, Y1, 16):
            print(f"{y:5d} " + " ".join(f"{g[(x, y)][1]:2d}" for x in range(X0, X1, 16)))
    for i in range(1, len(gs)):
        diff = [(k, gs[i-1][k], gs[i][k]) for k in gs[0] if gs[i-1][k] != gs[i][k]]
        print(f"== {names[i-1]} -> {names[i]}: {len(diff)} cells differ (word, collision)")
        for k, a, b in diff: print("  ", k, f"{a[0]:04x}/{a[1]} -> {b[0]:04x}/{b[1]}")
