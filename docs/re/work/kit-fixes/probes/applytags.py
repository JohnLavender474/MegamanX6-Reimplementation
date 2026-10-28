import json, re
from pathlib import Path
P = Path.home() / "IdeaProjects/mmx-demake-tools/catalog/helpers.json"
tags = json.load(open(Path.home() / "mmx-data/x6/re/kit-fixes/tags.json"))
lines = P.read_text().split("\n")
done = 0
section = None
for i, line in enumerate(lines):
    m = re.match(r'  "(functions|globals|fields)": \{', line)
    if m:
        section = m.group(1)
    m = re.match(r'    "(0x[0-9A-Fa-f]+)": (\{.*\})(,?)$', line)
    if not m or section == "fields" or m.group(1) not in tags:
        continue
    entry = json.loads(m.group(2))
    assert "overlay" not in entry
    entry["overlay"] = tags[m.group(1)]
    lines[i] = f'    "{m.group(1)}": {json.dumps(entry, ensure_ascii=False)}{m.group(3)}'
    done += 1
P.write_text("\n".join(lines))
print(done)
