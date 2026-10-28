import sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake import learn as L
from mmxdemake.arcentries import entry_data, only, split
from mmxdemake.collision import map_records
from mmxdemake.index import load_index
from mmxdemake.ramlayout import read_layout, read_size, find_member, edge_score, solid_masks
from mmxdemake.savestate import ram_from, read_state
from mmxdemake.stagemap import decode_screens, decode_tile_info
from mmxdemake.vram import read_vram, find_textures, VramError
from mmxdemake.texture import BLOCK_SIZE

ex = Path.home() / "mmx-data/x6/extracted/retail"
sp = Path.home() / "mmx-data/x6/dumps" / sys.argv[1]
records = [r for r in load_index(ex / "index.json")["files"] if r["kind"] == "rock_dat"]
sd = read_state(sp)
ram = ram_from(sd, (ex / "iso/SLUS_013.95").read_bytes(), sp)
vram = read_vram(sd)
w, h = read_size(ram)
print("size", w, h)
cands = {r["index"]: r for r in map_records(records)}
bodies = {i: L._member_body(ex, r) for i, r in cands.items()}
infos = {i: entry_data(b, only(split(b), 1)) for i, b in bodies.items()}
layout = read_layout(ram, w, h)
from mmxdemake.ramlayout import info_in_ram
found = sorted(k for k, i in infos.items() if i and info_in_ram(ram, i))
print("tile info in RAM:", found)
m = find_member(ram, infos, lambda k: L._fits(layout, w, h, bodies[k], infos[k]))
print("member", m)
if m:
    ents = split(bodies[m])
    print("entries of", m, [(hex(e[0]) if isinstance(e, tuple) else e) for e in ents][:40])
    for kind in L.TEXTURE_TYPES:
        bl = L._blocks(ents, kind)
        print("kind", hex(kind), "blocks", len(bl) if bl else 0)
        if bl:
            try:
                print(find_textures(vram, L.texture_block(bodies[m], ents, kind)))
            except VramError as e:
                print("  ", e)
    screens = decode_screens(entry_data(bodies[m], only(ents, 0)))
    masks = solid_masks(screens, decode_tile_info(infos[m]))
    print("edges", edge_score(layout, w, h, masks))
