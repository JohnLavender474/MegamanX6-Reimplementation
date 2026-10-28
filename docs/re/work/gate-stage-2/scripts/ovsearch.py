import re
import struct
import sys
from pathlib import Path

MASK_IMM = {1, 4, 5, 6, 7, 9, 10, 11, 12, 13, 15, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B}
OV = Path.home() / "mmx-data/x6/extracted/retail/rock_bin"


def mask(w):
    op = w >> 26
    if op in (2, 3):
        return w & 0xFC000000
    if op in MASK_IMM:
        return w & 0xFFFF0000
    return w


def kit_funcs(path):
    out, cur = [], None
    for line in open(path):
        m = re.match(r"; ==== (0x[0-9a-f]+) \.\. ", line)
        if m:
            cur = [int(m.group(1), 16), []]
            out.append(cur)
            continue
        m = re.match(r"([0-9a-f]{8}): ([0-9a-f]{8})", line)
        if m and cur is not None:
            cur[1].append(mask(int(m.group(2), 16)))
    return out


def overlay_words(n, base):
    data = (OV / f"{n:02d}.bin").read_bytes()
    words = [mask(w) for w in struct.unpack_from(f"<{len(data) // 4}I", data)]
    return words, base


kit = sys.argv[1]
targets = [(int(a), int(b, 16)) for a, b in (t.split(":") for t in sys.argv[2:])]
funcs = kit_funcs(kit)
for n, base in targets:
    words, _ = overlay_words(n, base)
    joined = ",".join(map(str, words))
    found = 0
    lines = []
    for addr, ws in funcs:
        hits = []
        for i in range(len(words) - len(ws) + 1):
            if words[i] == ws[0] and words[i:i + len(ws)] == ws:
                hits.append(base + 4 * i)
        if hits:
            found += 1
        lines.append(f"  {addr:#x} ({len(ws)}): {', '.join(hex(h) for h in hits[:3]) or '-'}")
    print(f"overlay {n:02d}: {found} of {len(funcs)} functions found")
    if "-v" not in sys.argv[0:1]:
        print("\n".join(lines))
