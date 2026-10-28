import json, re
from pathlib import Path
H = json.load(open(Path.home() / "IdeaProjects/mmx-demake-tools/catalog/helpers.json"))
ev = {}
for line in open(Path.home() / "mmx-data/x6/re/kit-fixes/place.txt"):
    parts = line.split(" | ")
    k, key, name = parts[0].split(" ", 2)
    ev[key] = dict(x.split(":") for x in parts[1].split()) if parts[1].strip() else {}
STAGE7 = ("Sheldon", "Guardian", "Mettaur", "NightmareBug", "HiddenFloor", "LaserGate", "GetVisualObjectAfter",
          "bug_", "sheldon_", "guardian_")
OV07 = ("Shark", "Pressure", "NPThink", "MoveToward16", "Belt", "LowerPathWarp", "PressBlock7", "JunkBlockRun7",
        "NightmareBlock2", "FloorValueLogic", "AutoScroll", "belt_", "shark_", "NightmarePressure")
OV06 = ("PressBlock", "JunkBlock", "FallingCrate", "Ride", "press_block", "ride_")
tags, why = {}, {}
for kind in ("functions", "globals"):
    for key, v in H[kind].items():
        a = int(key, 16)
        if not 0x800E9860 <= a < 0x80200000:
            continue
        name, note = v["name"], v.get("note", "")
        if name == "boss_overlay":
            n = [43, 44]
        elif a >= 0x801EA000:
            n = 0
        elif name.startswith(OV07) or name == "SheldonFloorMove" and False:
            n = 7
        elif name.startswith(STAGE7) or "Sheldon main" in note or "Sheldon visual" in note:
            n = 10
        elif name.startswith(OV06):
            n = 6
        else:
            n = None
        m = re.search(r"overlay (\d+)", note)
        if m and n is not None and n != int(m.group(1)) and name != "PressBlock7":
            print("CONFLICT", key, name, n, m.group(1))
        if m and n is None:
            n = int(m.group(1))
        e = ev[key]
        if isinstance(n, int):
            got = e.get(f"{n:02d}", "")
            need = "start" if kind == "functions" else "ref"
            if need not in got and "ptr" not in got:
                print("WEAK", key, name, n, e)
        tags[key] = n
        if n is None:
            print("UNPLACED", key, name)
json.dump(tags, open(Path.home() / "mmx-data/x6/re/kit-fixes/tags.json", "w"), indent=0)
from collections import Counter
print(len(tags), Counter(str(t) for t in tags.values()))
