import struct, sys
from pathlib import Path
root = Path(sys.argv[1]).expanduser()
targets = {0x80030ECC: "ContactMega", 0x80031474: "ContactWeapon", 0x800315CC: "ApplyWeaponHit",
           0x80032B34: "SolidWeaponHit", 0x80030DC0: "PlayerDamageScale", 0x8003D07C: "PlayerGrabDamage",
           0x800319EC: "WeaponHitReaction"}
files = [root / "iso/SLUS_013.95"] + sorted((root / "rock_bin").glob("*.bin"))
for f in files:
    data = f.read_bytes()
    exe = f.name.startswith("SLUS")
    counts = {}
    stores = []
    for off in range(0, len(data) - 3, 4):
        w = struct.unpack_from("<I", data, off)[0]
        if w >> 26 == 3:
            t = 0x80000000 | ((w & 0x3FFFFFF) << 2)
            if t in targets:
                counts.setdefault(targets[t], []).append(off)
        if w >> 26 == 0x28 and (w & 0xFFFF) == 0x70FC:
            stores.append(off)
    if counts or stores:
        conv = (lambda o: hex(o + 0x80010000 - 0x800)) if exe else hex
        print(f.name, {k: len(v) for k, v in counts.items()}, "mega.hp stores:", [conv(o) for o in stores])
        if exe:
            for k, v in counts.items():
                print("   ", k, [conv(o) for o in v])
