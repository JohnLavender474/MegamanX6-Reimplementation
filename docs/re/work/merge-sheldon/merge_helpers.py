import json, sys
path, *props = sys.argv[1:]
doc = json.load(open(path))
apply = "--apply" in props
props = [p for p in props if p != "--apply"]
seen = {}
for p in props:
    d = json.load(open(p))
    for sec in ("functions", "globals", "fields"):
        for k, v in d.get(sec, {}).items():
            k = "0x" + k[2:].upper()
            old = doc.setdefault(sec, {}).get(k)
            if k in seen.get(sec, {}) and seen[sec][k][1]["name"] != v["name"]:
                print("agent clash", sec, k, seen[sec][k][0], seen[sec][k][1]["name"], "|", p, v["name"])
            seen.setdefault(sec, {})[k] = (p, v)
            if old and old["name"] != v["name"]:
                print("rename", sec, k, old["name"], "->", v["name"], "(", p, ")")
            elif old and old.get("note") != v.get("note"):
                print("note", sec, k, old["name"])
for sec, items in seen.items():
    for k, (p, v) in items.items():
        entry = dict(doc[sec].get(k, {}))
        entry.update({kk: vv for kk, vv in v.items() if kk in ("name", "note", "overlay", "alt")})
        entry.setdefault("note", "")
        doc[sec][k] = entry
        if int(k, 16) >= 0x800E9860 and "overlay" not in entry:
            print("untagged overlay-range entry", sec, k, entry["name"], "(", p, ")")
    doc[sec] = dict(sorted(doc[sec].items(), key=lambda kv: int(kv[0], 16)))
names = [(s, v["name"]) for s in doc for v in doc[s].values()]
dup = sorted({n for s, n in names if names.count((s, n)) > 1})
print("duplicate names:", dup)
if apply:
    out = ["{"]
    secs = list(doc)
    for i, sec in enumerate(secs):
        out.append(f'  "{sec}": {{')
        items = list(doc[sec].items())
        for j, (k, v) in enumerate(items):
            out.append(f'    "{k}": {json.dumps(v, ensure_ascii=False)}' + ("," if j < len(items) - 1 else ""))
        out.append("  }" + ("," if i < len(secs) - 1 else ""))
    out.append("}")
    open(path, "w").write("\n".join(out) + "\n")
