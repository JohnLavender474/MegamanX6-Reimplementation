import struct, sys
from pathlib import Path
root = Path.home() / "mmx-data/x6/extracted/retail"
def code(ch):
    if "A" <= ch <= "Z":
        k = ord(ch) - 65; return (k // 7) * 16 + k % 7
    if "a" <= ch <= "z":
        k = ord(ch) - 97; return 0x70 + (k // 7) * 16 + k % 7
    return {" ": 0x35, "+": 0x54, "-": 0x55, ".": 0x60}[ch]
words = sys.argv[1:]
files = sorted((root / "rock_dat").glob("*")) + sorted((root / "rock_bin").glob("*.bin")) + [root / "iso/SLUS_013.95"]
for w in words:
    cs = [code(c) for c in w]
    b8 = bytes(cs) if max(cs) < 256 else None
    for f in files:
        d = f.read_bytes()
        if b8:
            i = d.find(b8)
            while i >= 0:
                print(w, "u8", f.name, hex(i)); i = d.find(b8, i + 1)
        n = len(d) // 2
        arr = struct.unpack_from(f"<{n}H", d)
        for par in (0,):
            for i in range(0, n - len(cs)):
                if (arr[i] & 0x1FF) == cs[0] and all((arr[i + j] & 0x1FF) == cs[j] for j in range(len(cs))):
                    print(w, "u16", f.name, hex(2 * i))
