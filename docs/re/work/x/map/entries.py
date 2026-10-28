import json, sys
F = json.load(open(sys.argv[1]))
steps = {}
for f, v in F.items():
    for r in v["role"].split(", "):
        if r.startswith("step "): steps.setdefault(f, []).append(int(r[5:]))
def reach(f, seen, depth=4):
    if f in seen or f not in F or depth < 0: return
    seen.add(f)
    for c in F[f]["calls"]: reach(c, seen, depth - 1)
R = {}
for f, ks in steps.items():
    s = set(); reach(f, s)
    for k in ks: R[k] = s
into = {}
for g, v in F.items():
    for t in v["step"]:
        src = sorted(k for k, s in R.items() if g in s)
        into.setdefault(t, []).append((g, v["area"], v["anim"], v["sub"], src))
for t in sorted(into):
    if t > 31: continue
    print(f"== into step {t}")
    for g, a, an, sub, src in into[t]:
        print(f"   by {g} [{a}] anims {an} sub {sub} reached from steps {src[:16]}{'...' if len(src) > 16 else ''}")
