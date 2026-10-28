import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.codewalk import walk
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad

DUMPS = Path.home() / "mmx-data/x6/dumps"
EXE = (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()
TARGETS = {0x80031474: "ContactWeapon", 0x80032B34: "SolidWeaponHit", 0x800315CC: "ApplyWeaponHit",
           0x80030ECC: "ContactMega"}
REGIONS = [(0x80010000, 0x80080000), (0x800E0000, 0x80200000)]

JOBS = [
    ("Shield Sheldon visual 16", "sheldon-17_boss", ("visual", 16)),
    ("Zero Nightmare visual 19", "turtloid-5_zero", ("visual", 19)),
    ("Worm Spider ball", "yammark-1_start", 0x800EA2A0),
    ("Sea Attacker bubble", "turtloid-1", 0x800EAB90),
    ("Shield Sheldon plate shot", "sheldon-17_boss", 0x800FA61C),
    ("Zero Nightmare shot", "turtloid-5_zero", 0x800FB328),
    ("Zero Nightmare pillar", "turtloid-5_zero", 0x800FB964),
    ("Zero Nightmare crescent", "turtloid-5_zero", 0x800FBD38),
    ("High Max shots (visual 21)", "turtloid-8_high-max", 0x80100C60),
    ("Mettaur D2 shot (visual 32)", "sheldon-6_tile-shown", ("visual", 32)),
    ("Guardian shot (visual 35)", "sheldon-16_death-guardian", ("visual", 35)),
]

for name, state_name, root in JOBS:
    state = read_state(DUMPS / f"{state_name}.state.gz")
    ram = ram_from(state, EXE, state_name)
    if isinstance(root, tuple):
        table = struct.unpack_from("<I", scratchpad(state), 0x84)[0]
        root = struct.unpack_from("<I", ram, (table & 0x1FFFFF) + 4 * root[1])[0]
    graph = walk(ram, [root], REGIONS, REGIONS, set(TARGETS))
    hits = {}
    for f, callees in graph.calls.items():
        for t in callees:
            if t in TARGETS:
                hits.setdefault(TARGETS[t], []).append(hex(f))
    print(f"{name}: root {root:#x}, {len(graph.functions)} functions{' (truncated)' if graph.truncated else ''}")
    for k, v in sorted(hits.items()):
        print(f"    {k}: {v}")
