"""Print a kit code.txt with the boilerplate removed: nops, register saves, and the
tex/clut/sprite_data block before SetAnime (shown as one 'SETANIME bank-of-type, n' line).
Usage: python3 condense.py code.txt [start_addr end_addr]"""
import re
import sys

lines = open(sys.argv[1]).read().splitlines()
lo = int(sys.argv[2], 16) if len(sys.argv) > 2 else 0
hi = int(sys.argv[3], 16) if len(sys.argv) > 3 else 1 << 32
SAVE = re.compile(r"\b(sw|lw) (s[0-8]|ra|fp),-?\d+\(sp\)")
out = []
skip = False
buf_anim = None
for ln in lines:
    m = re.match(r"([0-9a-f]{8}): ([0-9a-f]{8})  (.*)", ln)
    if not m:
        if ln.startswith("; ===="):
            a = int(ln.split()[2], 16)
            skip = not (lo <= a < hi)
        if not skip:
            out.append(ln)
        continue
    if skip:
        continue
    a, w, ins = m.groups()
    if ins.strip() == "nop" or SAVE.search(ins) or re.search(r"addiu sp,sp,", ins):
        continue
    out.append(f"{a[4:]}: {ins}")

text = "\n".join(out)
pat = re.compile(r"(\n[0-9a-f]{4}: addiu v0,zero,320\n)(?:.*\n)*?(([0-9a-f]{4}): jal 0x800179a4  ; SetAnime)", re.M)


def rep(mo):
    block = mo.group(0)
    if block.count("\n") > 45:
        return block
    am = re.findall(r"addiu a1,zero,(-?\d+)", block)
    am2 = re.findall(r"addu a1,zero,zero", block)
    n = am[-1] if am else ("0" if am2 else "?")
    return f"\n      [tex 320, clut/sprites of own bank] SETANIME {n}"


text = pat.sub(rep, text)
print(text)
