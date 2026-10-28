import json, re, struct
from pathlib import Path
from mmxdemake.codewalk import constants
from mmxdemake.mips import jump_target

X = Path.home() / "mmx-data/x6/extracted/retail/rock_bin"
H = json.load(open(Path.home() / "IdeaProjects/mmx-demake-tools/catalog/helpers.json"))
BASES = {0x800E9860: [n for n in range(2, 41)], 0x800FA000: [43, 44], 0x801EA000: [0, 1]}
ram = bytearray(0x200000)
info = {}
for base, ns in BASES.items():
    for n in ns:
        body = (X / f"{n:02d}.bin").read_bytes()
        if len(body) < 16:
            continue
        o = base & 0x1FFFFF
        ram[o:o + len(body)] = body
        lo, hi = base, base + len(body) // 4 * 4
        u = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
        starts = set()
        jals = set()
        for a in range(lo, hi, 4):
            w = u(a)
            if (w >> 16 == 0x27BD and w & 0x8000) or (a - 8 >= lo and u(a - 8) == 0x03E00008):
                starts.add(a)
            t = jump_target(w, a)
            if t is not None:
                jals.add(t)
        refs = {v for _, v in constants(ram, lo, hi)}
        words = {u(a) for a in range(lo, hi, 4)}
        info[n] = (lo, hi, starts, jals, refs, words)
        ram[o:o + len(body)] = bytes(len(body))
out = {}
for kind in ("functions", "globals"):
    for key, v in H[kind].items():
        a = int(key, 16)
        if not 0x800E9860 <= a < 0x80200000:
            continue
        ev = []
        for n, (lo, hi, starts, jals, refs, words) in info.items():
            if not lo <= a < hi:
                continue
            tags = []
            if a in starts: tags.append("start")
            if a in jals: tags.append("jal")
            if a in refs: tags.append("ref")
            if a in words: tags.append("ptr")
            if tags:
                ev.append(f"{n:02d}:{'+'.join(tags)}")
        hint = re.findall(r"overlay (\d+)|stage (\d+)", v.get("note", ""))
        print(kind[0], key, v["name"], "|", " ".join(ev), "| hint", hint, "|", v.get("note", "")[:70])
