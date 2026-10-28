import sys, json, hashlib
from pathlib import Path
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from mmxdemake import spriteexport as se
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad, banks, bank_of_type, draw, ENEMY_MODE
from mmxdemake.vram import read_vram
from mmxdemake.animfind import code_regions
from mmxdemake.anims import read_table
from mmxdemake.png import write_png
X = Path.home()/"mmx-data/x6/extracted/retail"; D = Path.home()/"mmx-data/x6/dumps"
state = sys.argv[1]; out = Path(sys.argv[2]); out.mkdir(parents=True, exist_ok=True)
path = D/state; data = read_state(path); exe = (X/"iso/SLUS_013.95").read_bytes()
ram = ram_from(data, exe, path); scratch, vram = scratchpad(data), read_vram(data)
overlays = {int(p.stem): p.read_bytes() for p in sorted((X/"rock_bin").glob("*.bin"))}
regions = code_regions(ram, overlays, exe)
bank = bank_of_type(banks(ram, scratch), 225)
table = int.from_bytes(ram[0xCD340+0x30:0xCD340+0x34], "little")
region = next((lo, hi) for lo, hi in regions if lo <= table < hi)
anims = se.fitting(ram, "ride", table, read_table(ram, table, *region, 31), bank, "live", print)
used = sorted({f["sprite"] for frames, _ in anims for f in frames})
frames = {n: draw(ram, vram, bank, n, ENEMY_MODE) for n in used}
w, h, ox, oy = se.geometry(frames)
meta = {"source": {"state": state, "sha1": hashlib.sha1(path.read_bytes()).hexdigest(), "object": "actor2 0x800CD340",
        "table": f"{table:#010x}", "bank": bank.index, "bank_type": 225}, "cell": [w, h], "origin": [ox, oy],
        "fps": 60, "animations": []}
for k, (seq, loop) in enumerate(anims):
    W, H, rows = se.strip(frames, seq, w, h, ox, oy)
    write_png(out/f"anim-{k}.png", W, H, rows)
    meta["animations"].append({"index": k, "frames": seq, "loop_to": loop})
(out/"animations.json").write_text(json.dumps(meta, indent=1))
print(len(anims), "animations,", len(used), "sprites, cell", w, h, "origin", ox, oy, "->", out)
