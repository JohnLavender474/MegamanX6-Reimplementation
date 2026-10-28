import sys, struct, hashlib
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad, banks, entries, clut_word, cells, with_source, Bank, cell_bytes
from mmxdemake.vram import read_vram
from mmxdemake.animfind import code_regions, live_sources
X = Path.home() / "mmx-data/x6/extracted/retail"
D = Path.home() / "mmx-data/x6/dumps"
RULES = {57: [(0, None, 0), (1, 0x140, 0), (2, 0x540, 1)], 51: [(0, None, 0), (1, 0x540, 1)], 56: [(0, None, 0), (1, 0x540, 1)]}
exe = (X / "iso/SLUS_013.95").read_bytes()
overlays = {int(p.stem): p.read_bytes() for p in sorted((X / "rock_bin").glob("*.bin"))}
for name in sys.argv[1:]:
    path = D / name
    data = read_state(path)
    ram = ram_from(data, exe, path)
    scratch, vram = scratchpad(data), read_vram(data)
    regions = code_regions(ram, overlays, exe)
    bl = banks(ram, scratch)
    ent = entries(ram, scratch)
    live = live_sources(ram, regions)
    for t, rules in RULES.items():
        k = next((b.index for b in bl if b.type == t), None)
        if k is None:
            continue
        print(name, "type", t, "bank", k, "textures", [bl[i].texture if i < len(bl) else "-" for i in range(k, k + 3)],
              "types", [bl[i].type if i < len(bl) else "-" for i in range(k, k + 3)])
        for d0, tex, p in rules:
            if k + d0 >= len(ent):
                print("  set", d0, "no entry"); continue
            dat = ent[k + d0]
            texv = tex if tex is not None else 0x140
            b = with_source(ram, Bank(k + d0, t, dat, 0, None, 0), dat, texv, clut_word(k + p))
            cs = sorted(cells(ram, b))
            h = hashlib.sha1(b"".join(cell_bytes(vram, *c) for c in cs)).hexdigest()[:10]
            pal = hashlib.sha1(bytes(vram[(clut_word(k+p) >> 6) * 2048 + (clut_word(k+p) & 63) * 32:][:128])).hexdigest()[:8]
            print(f"  set data {dat:#x} count {b.count} tex {texv:#x} clut {clut_word(k+p):#x} cells {len(cs)} vram {h} pal {pal}")
        for s in sorted(live.get(t, ())):
            print(f"  live data {s[0]:#x} tex {s[1]:#x} clut {s[2]:#x} mode {s[3]:#x}")
