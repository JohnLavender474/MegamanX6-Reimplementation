import json, sys
F = json.load(open(sys.argv[1]))
steps = {}
for f, v in F.items():
    for r in v["role"].split(", "):
        if r.startswith("step "): steps[int(r[5:])] = f
users = {}
for f, v in F.items():
    for c in v["calls"]: users.setdefault(c, set()).add(f)
def tree(f, depth, seen):
    if f in seen or f not in F or depth < 0: return
    seen.add(f)
    for c in F[f]["calls"]: tree(c, depth - 1, seen)
reach = {k: set() for k in steps}
for k, f in steps.items(): tree(f, 2, reach[k])
shared = {}
for k, s in reach.items():
    for f in s: shared[f] = shared.get(f, 0) + 1
for k in sorted(steps):
    f = steps[k]
    own = [g for g in reach[k] if shared[g] <= 2 or g == f]
    anims = sorted({a for g in own for a in F[g]["anim"]})
    to = sorted({s for g in own for s in F[g]["step"]} - {k})
    sub = sorted({s for g in own for s in F[g]["sub"]})
    snd = sorted({s for g in own for s in F[g]["sound"]})
    ov = sum(1 for g in own if F[g]["area"] == "ov")
    print(f"step {k:2} {f}: own {len(own):2} (ov {ov:2}) anims {anims} -> {to} sub {sub} snd {snd}")
