import re
import sys
from pathlib import Path

REPO = Path.home() / "IdeaProjects/mmx-demake-tools"
text = Path(sys.argv[1]).read_text()
current = None
bad = 0
for part in re.split(r"^(## `[^`]+`.*|Old:|New:)$", text, flags=re.M):
    m = re.match(r"## `([^`]+)`", part)
    if m:
        current = m.group(1)
        continue
    if part == "Old:":
        mode = "old"
        continue
    if part == "New:":
        mode = "new"
        continue
    if current and part.strip().startswith("```") and "mode" in dir() and mode == "old":
        block = re.search(r"```\n(.*?)\n```", part, re.S).group(1) + "\n"
        n = (REPO / current).read_text().count(block)
        ok = n == 1 and not re.search(r"^(Old|New):$", block, re.M)
        bad += not ok
        print(("ok " if ok else "BAD") + f" {current}: {n} x {block.splitlines()[0][:70]}")
        mode = None
print("bad", bad)
