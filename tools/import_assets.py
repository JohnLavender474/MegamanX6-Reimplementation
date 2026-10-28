import argparse
import re
import shutil
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
TILESET = re.compile(r'<tileset (?P<attrs>[^>]*?)>\s*<image (?P<image>[^>]*?)/>\s*</tileset>')
ATTR = re.compile(r'(\w+)="([^"]*)"')

FOLDERS = [
    ("sprites/players", "sprites/players"),
    ("sprites/enemies", "sprites/enemies"),
    ("sprites/banks", "sprites/banks"),
    ("sprites/stages", "sprites/stages"),
    ("re/front-end/images", "sprites/frontend"),
    ("sounds", "sounds"),
    ("music/bgm", "music"),
    ("movies", "movies"),
]


def copy_file(src, dst, update, counts):
    if dst.exists() and not update:
        counts["kept"] += 1
        return
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, dst)
    counts["copied"] += 1


def copy_folder(src, dst, update, counts):
    for f in sorted(src.rglob("*")):
        if f.is_file():
            copy_file(f, dst / f.relative_to(src), update, counts)


def tsx(attrs, image_attrs, png):
    a = dict(ATTR.findall(attrs))
    a.pop("firstgid")
    img = dict(ATTR.findall(image_attrs))
    head = " ".join(f'{k}="{v}"' for k, v in a.items())
    return ('<?xml version="1.0" encoding="UTF-8"?>\n'
            f'<tileset version="1.10" tiledversion="1.10.2" {head}>\n'
            f' <image source="../png/{png}" width="{img["width"]}" height="{img["height"]}"/>\n'
            '</tileset>\n')


def import_map(map_dir, out, update, counts):
    tmx_files = list(map_dir.glob("*.tmx"))
    if len(tmx_files) != 1:
        return
    src = tmx_files[0]
    name = src.stem
    tiled = out / "tiled"
    dst_tmx = tiled / "tmx" / src.name
    if dst_tmx.exists():
        counts["maps kept"] += 1
        return
    text = src.read_text(encoding="utf-8")

    def external(m):
        a = dict(ATTR.findall(m.group("attrs")))
        base = "collision" if a["name"] == "collision" else f"{name}-{a['name']}"
        png = f"{base}.png"
        image = dict(ATTR.findall(m.group("image")))["source"]
        copy_file(map_dir / image, tiled / "png" / png, update or base != "collision", counts)
        tsx_path = tiled / "tsx" / f"{base}.tsx"
        tsx_path.parent.mkdir(parents=True, exist_ok=True)
        tsx_path.write_text(tsx(m.group("attrs"), m.group("image"), png), encoding="utf-8")
        return f'<tileset firstgid="{a["firstgid"]}" source="../tsx/{base}.tsx"/>'

    text, n = TILESET.subn(external, text)
    if n == 0:
        raise SystemExit(f"{src}: no embedded tileset to move out")
    dst_tmx.parent.mkdir(parents=True, exist_ok=True)
    dst_tmx.write_text(text, encoding="utf-8")
    counts["maps"] += 1


def main():
    p = argparse.ArgumentParser(description="Copy the exported X6 data into assets/. Files already in assets/ are kept unless --update is given; maps in assets/tiled/tmx/ are never replaced.")
    p.add_argument("--from", dest="source", type=Path, default=Path.home() / "mmx-data" / "x6",
                   help="the exported data (default: ~/mmx-data/x6)")
    p.add_argument("--maps", type=Path, help="take the map folders (mNNN-name/) from here, not from <from>/tmx")
    p.add_argument("--out", type=Path, default=REPO / "assets")
    p.add_argument("--update", action="store_true", help="replace the files that are already in assets/ (not maps)")
    args = p.parse_args()

    counts = {"copied": 0, "kept": 0, "maps": 0, "maps kept": 0}
    for src, dst in FOLDERS:
        folder = args.source / src
        if not folder.is_dir():
            raise SystemExit(f"missing {folder}")
        copy_folder(folder, args.out / dst, args.update, counts)
    maps = args.maps or args.source / "tmx"
    for map_dir in sorted(d for d in maps.iterdir() if d.is_dir()):
        import_map(map_dir, args.out, args.update, counts)
    print(", ".join(f"{v} {k}" for k, v in counts.items()))


if __name__ == "__main__":
    main()
