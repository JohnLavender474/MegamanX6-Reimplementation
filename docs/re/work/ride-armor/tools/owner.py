import sys, json
sys.path.insert(0, __import__("os").path.dirname(__file__))
from ramtool import ram, w32
H = json.load(open("/home/johnlavender/IdeaProjects/mmx-demake-tools/catalog/helpers.json"))["functions"]
r = ram(sys.argv[1])
for s in sys.argv[2:]:
    a = int(s, 16); p = a
    while p > a - 0x4000:
        w = w32(r, p)
        if (w >> 16) == 0x27bd and (w & 0x8000): break
        p -= 4
    k = f"0x{p:08X}"
    print(s, "in", k, H.get(k, {}).get("name"), H.get(k, {}).get("note", "")[:90])
