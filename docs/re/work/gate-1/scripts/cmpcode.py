import re
import sys

MASK_IMM = {1, 4, 5, 6, 7, 9, 10, 11, 12, 13, 15, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B}


def funcs(path):
    out, cur = [], None
    for line in open(path):
        if re.match(r"; ==== 0x[0-9a-f]+ \.\. ", line):
            cur = []
            out.append(cur)
            continue
        m = re.match(r"([0-9a-f]{8}): ([0-9a-f]{8})", line)
        if m and cur is not None:
            w = int(m.group(2), 16)
            op = w >> 26
            if op in (2, 3):
                w &= 0xFC000000
            elif op in MASK_IMM:
                w &= 0xFFFF0000
            cur.append(w)
    return out


a, b = funcs(sys.argv[1]), funcs(sys.argv[2])
print("functions", len(a), len(b))
print("sizes a", [len(x) for x in a])
print("sizes b", [len(x) for x in b])
sa, sb = {tuple(x) for x in a}, {tuple(x) for x in b}
print("identical after masking", len(sa & sb), "of", len(sa), len(sb))
print("instructions", sum(map(len, a)), sum(map(len, b)))
