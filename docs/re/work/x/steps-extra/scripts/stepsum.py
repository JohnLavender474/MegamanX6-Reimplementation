import re, sys
kit = sys.argv[1]
L = open(kit).read().splitlines()
funcs, cur = {}, None
for l in L:
    if l.startswith("; ===="):
        cur = int(l.split()[2], 16); funcs[cur] = []
    elif cur is not None and re.match(r"^[0-9a-f]{8}:", l):
        funcs[cur].append(l)
tables = {}
text = open(kit.replace("code.txt", "summary.md")).read().split("## State tables")[1].split("## Neighbour")[0]
for l in text.splitlines():
    m = re.match(r"\| (0x[0-9a-f]+) \| (0x[0-9a-f]+) \| (0x.*) \|$", l)
    if m:
        tables.setdefault(int(m.group(2), 16), []).extend(int(t, 16) for t in m.group(3).split(", "))
def info(f):
    anims, steps, calls, consts = [], [], [], []
    body = funcs.get(f, [])
    for i, l in enumerate(body):
        m = re.search(r"; (PlayerSetAnime|PlayerSetAnimeAt|PlayerSetAnimeShoot|PlayerSyncShootAnime|SetAnime)$", l)
        if m:
            for x in body[max(0, i-3):i+2]:
                a = re.search(r"addiu a1,zero,(\d+)", x)
                if a: anims.append(int(a.group(1))); break
        m = re.search(r"sb (\w+),5\(", l)
        if m:
            for x in body[max(0, i-10):i+1][::-1]:
                a = re.search(rf"addiu {m.group(1)},zero,(\d+)", x)
                if a: steps.append(int(a.group(1))); break
        m = re.search(r"jal 0x\w+  ; ([A-Z]\w+)", l)
        if m and m.group(1) not in ("AnimeTick",): calls.append(m.group(1))
        m = re.search(r"jal (0x\w+)  ; f_", l)
        if m: calls.append(m.group(1))
    return anims, steps, list(dict.fromkeys(calls))
for a in sys.argv[2:]:
    f = int(a, 16)
    todo = [f] + tables.get(f, [])
    print(f"== {a}")
    for g in todo:
        an, st, ca = info(g)
        print(f"   {g:08x}: anims {an} steps {st} calls {ca}")
