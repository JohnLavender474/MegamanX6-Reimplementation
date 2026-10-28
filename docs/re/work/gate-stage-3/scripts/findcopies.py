"""For each kit given, find other kits under ~/mmx-data/x6/re/ with the same functions after masking
(immediates and jump targets). Usage: python3 findcopies.py kitdir [kitdir ...]"""
import glob
import os
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
    return [tuple(f) for f in out if len(f) > 4]


root = os.path.expanduser("~/mmx-data/x6/re")
others = glob.glob(f"{root}/*/kit/code.txt") + glob.glob(f"{root}/*/kits/*/code.txt")
cache = {}
for kit in sys.argv[1:]:
    mine = funcs(os.path.join(kit, "code.txt"))
    ms = set(mine)
    res = []
    for o in others:
        if os.path.abspath(o).startswith(os.path.abspath(kit)) or "/gate-stage-3/" in o:
            continue
        if o not in cache:
            cache[o] = set(funcs(o))
        n = len(ms & cache[o])
        if n:
            res.append((n, o.replace(root + "/", "")))
    res.sort(reverse=True)
    print(f"{kit}: {len(ms)} distinct functions")
    for n, o in res[:6]:
        print(f"   {n:3d} equal  {o}")
