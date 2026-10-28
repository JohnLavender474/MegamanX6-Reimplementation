import sys
from probe_ram import *
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram
from mmxdemake.palettes import sprites
st = sys.argv[1]; pals = [int(x) for x in sys.argv[2].split(",")]
state = read_state(DUMPS / (st + ".state.gz"))
ram, scr = load(st)
vram = read_vram(state)
buf = sp32(scr, 0x28)
for p in pals:
    pal = ram_at(ram, buf + p*32, 32) if False else ram[(buf + p*32) & 0x1FFFFF:(buf + p*32 + 32) & 0x1FFFFF]
    hits = []
    for y in range(512):
        row = vram[y*2048:(y+1)*2048]
        i = row.find(pal)
        while i >= 0:
            if i % 32 == 0: hits.append((i//2, y))
            i = row.find(pal, i+1)
    print("palette", p, pal.hex()[:32], "vram", hits[:5])
    for (cx, cy) in hits[:1]:
        sp = [s for s in sprites(ram) if s[4] == cx and s[5] == cy]
        print("  sprites with this clut:", len(sp), sorted(set((s[0], s[1]) for s in sp))[:40])
