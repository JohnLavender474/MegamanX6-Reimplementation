"""Write the Markdown record table (kinds 0-2 and 4, sorted by x) of the Heatnix section 1
objects.json, with the proposed names. Usage: python3 record_table.py <objects.json>"""
import json, sys

NAMES = {
    (0, 15): "Nightmare Virus", (0, 22): "Nightmare Bug (needs effect 1)",
    (0, 25): "main 25: mini boss (open)", (0, 28): "teleporter crystal (area change)",
    (0, 36): "flame jet", (0, 47): "item object (`id` = item kind) [likely]", (0, 52): "Blaze Heatnix [likely]",
    (4, 4): "Rescue Reploid (`id` = Reploid number)", (4, 8): "boss gate", (4, 12): "grey block (effect 4)",
    (4, 26): "Light capsule",
}
DIFF = {0: "all", 1: "Normal and up", 2: "Hard and up"}
objs = json.load(open(sys.argv[1]))["objects"]
print("| record | kind | difficulty | type | object | id | x | y |")
print("|---|---|---|---|---|---|---|---|")
for o in sorted(objs, key=lambda o: (o["x"], o["y"], o["index"])):
    k = o["kind"] & 0x0F
    if k == 3:
        continue
    cls = 0 if k <= 2 else k
    print(f"| {o['index']} | {k} | {DIFF.get(k, '–') if k <= 2 else '–'} | {o['b']} | "
          f"{NAMES.get((cls, o['b']), '?')} | {o['id']} | {o['x']} | {o['y']} |")
