import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "mmx-data/x6/re/tools"))
from mips import dis
ex = Path.home() / "mmx-data/x6/extracted/retail"
READERS = {0x80030234: "TileCollisionAt", 0x8003015C: "TileAt", 0x80030068: "TileBelow", 0x8002FD3C: "TileAbove",
           0x8002FE30: "Tile_fe30", 0x8002FF70: "Tile_ff70", 0x80030304: "Tile_0304"}
images = [("exe", (ex / "iso/SLUS_013.95").read_bytes()[0x800:], 0x80010000)]
for p in sorted((ex / "rock_bin").glob("*.bin")):
    images.append((f"ov{p.stem}", p.read_bytes(), 0x800E9860))
want = {int(a) for a in sys.argv[1:]} if len(sys.argv) > 1 else None
for name, data, base in images:
    n = len(data) // 4
    for i in range(n):
        w = struct.unpack_from("<I", data, 4 * i)[0]
        if w >> 26 != 3:
            continue
        t = ((base + 4 * i + 4) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
        if t not in READERS:
            continue
        out = []
        for j in range(i + 1, min(n, i + 16)):
            x = struct.unpack_from("<I", data, 4 * j)[0]
            d = dis(x, base + 4 * j)
            out.append(d)
        imms = []
        for d in out:
            for pre in ("addiu v1,zero,", "addiu v0,zero,", "addiu a0,zero,", "addiu a1,zero,", "addiu at,zero,", "sltiu ", "slti ", "addiu v1,v0,-", "addiu a0,v0,-", "addiu v0,v0,-"):
                if d.startswith(pre.split(",")[0]) and pre in d:
                    imms.append(d)
        if imms:
            print(f"{name} {base + 4 * i:#x} {READERS[t]}: " + " | ".join(imms[:8]))
