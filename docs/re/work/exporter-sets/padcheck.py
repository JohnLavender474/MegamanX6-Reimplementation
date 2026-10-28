import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools/tests"))
from pngread import read_png

old_dir, new_dir = Path(sys.argv[1]), Path(sys.argv[2])
skip = {int(x) for x in sys.argv[3].split(",")} if len(sys.argv) > 3 else set()
a = json.loads((old_dir / "animations.json").read_text())
b = json.loads((new_dir / "animations.json").read_text())
(ow, oh), (nw, nh) = a["cell"], b["cell"]
dx, dy = b["origin"][0] - a["origin"][0], b["origin"][1] - a["origin"][1]
print("cell", a["cell"], b["cell"], "shift", dx, dy)
bad = []
for k in range(len(a["animations"])):
    if k in skip:
        continue
    o = read_png(old_dir / f"anim-{k}.png")
    n = read_png(new_dir / f"anim-{k}.png")
    ow_, oh_, op = o[0], o[1], o[2]
    nw_, nh_, np_ = n[0], n[1], n[2]
    frames = ow_ // ow
    if nw_ // nw != frames or oh_ != oh or nh_ != nh:
        bad.append((k, "size", o[:2], n[:2]))
        continue
    for f in range(frames):
        for y in range(nh):
            for x in range(nw):
                px = np_[y][(f * nw + x) * 4:(f * nw + x) * 4 + 4]
                ox, oy = x - dx, y - dy
                if 0 <= ox < ow and 0 <= oy < oh:
                    want = op[oy][(f * ow + ox) * 4:(f * ow + ox) * 4 + 4]
                else:
                    want = None
                if want is None:
                    if px[3] != 0:
                        bad.append((k, f, x, y, "outside", px))
                        break
                elif px != want:
                    bad.append((k, f, x, y, px, want))
                    break
            else:
                continue
            break
print("checked", len(a["animations"]) - len(skip), "strips; problems", bad[:5], len(bad))
