import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

from mmxdemake.arcentries import ArcError, entry_data, only, split
from mmxdemake.collision import map_records
from mmxdemake.index import load_index
from mmxdemake.objects import ObjectError, decode, find_stage_list
from mmxdemake.palettes import PaletteError, cameras, layers_from, learn_rule, sprites
from mmxdemake.ramlayout import (
    LayoutError, check_layout, differing_cells, find_member, find_on_disc, read_layout, read_size, solid_masks,
)
from mmxdemake.savestate import SaveStateError, ram_from, read_state
from mmxdemake.stagemap import decode_screens, decode_tile_info
from mmxdemake.vram import VramError, find_textures, pages_from_rows, read_vram

REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CATALOG = REPO_ROOT / "catalog"
DEFAULT_DUMPS = Path.home() / "mmx-data/x6/dumps"
DEFAULT_EXTRACTED = Path.home() / "mmx-data/x6/extracted/retail"
TEXTURE = 0x10000
ROW_TEXTURE = 0x16
TEXTURE_TYPES = (TEXTURE, ROW_TEXTURE)


class LearnFailed(Exception):
    pass


def slug(name):
    return re.sub(r"[^a-z0-9]+", "-", name.lower()).strip("-")


def _validate(manifest, entries):
    if not isinstance(manifest, list) or not all(
            isinstance(m, dict) and isinstance(m.get("name"), str) and isinstance(m.get("sha1"), str) for m in manifest):
        raise ValueError("catalog savestates.json must be a list of entries with name and sha1")
    if not isinstance(entries, list) or not all(
            isinstance(e, dict) and isinstance(e.get("name"), str) and isinstance(e.get("member"), int) for e in entries):
        raise ValueError("catalog stages.json must be a list of entries with name and member")


def _load_list(path):
    return json.loads(path.read_text()) if path.exists() else []


def _write_list(path, items, key):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(sorted(items, key=key), sort_keys=True, indent=2) + "\n")


def _member_body(extracted, r):
    body = (extracted / r["path"]).read_bytes()
    if hashlib.sha1(body).hexdigest() != r["sha1"]:
        raise LearnFailed(f"member {r['index']}: sha1 does not match the index")
    return body


def texture_block(body, entries, kind=TEXTURE):
    data = entry_data(body, only(entries, kind))
    return data if kind == TEXTURE else pages_from_rows(data)


def _find_texture(vram, body, entries):
    kinds = [TEXTURE] + [k for k in TEXTURE_TYPES[1:] if any(e.type == k for e in entries)]
    first = None
    for kind in kinds:
        try:
            find_textures(vram, texture_block(body, entries, kind))
            return kind
        except VramError as e:
            first = first or e
    raise first


def _fits(layout, w, h, body, info):
    screens = decode_screens(entry_data(body, only(split(body), 0)))
    try:
        check_layout(layout, w, h, solid_masks(screens, decode_tile_info(info)), len(screens))
    except LayoutError:
        return False
    return True


def run(state, name, note="", dumps=DEFAULT_DUMPS, extracted=DEFAULT_EXTRACTED, catalog=DEFAULT_CATALOG, log=print):
    if Path(state).name != state:
        log("error: --state must be a file name in --dumps, not a path")
        return 2
    if not slug(name):
        log(f"error: name {name!r} has no letters or digits")
        return 2
    state_path = Path(dumps) / state
    extracted, catalog = Path(extracted), Path(catalog)
    if not state_path.is_file():
        log(f"error: save state {state_path} does not exist")
        return 2
    try:
        records = [r for r in load_index(extracted / "index.json")["files"] if r["kind"] == "rock_dat"]
        manifest = _load_list(catalog / "savestates.json")
        entries = _load_list(catalog / "stages.json")
        _validate(manifest, entries)
    except (OSError, ValueError, KeyError, TypeError, AttributeError) as e:
        log(f"error: {e}")
        return 2
    try:
        sha1 = hashlib.sha1(state_path.read_bytes()).hexdigest()
        old = next((m for m in manifest if m["name"] == state), None)
        if old and old["sha1"] != sha1:
            raise LearnFailed(f"manifest has {state} with sha1 {old['sha1']}, the file has {sha1}")
        state_data = read_state(state_path)
        ram = ram_from(state_data, (extracted / "iso/SLUS_013.95").read_bytes(), state_path)
        vram = read_vram(state_data)
        w, h = read_size(ram)
        candidates = {r["index"]: r for r in map_records(records)}
        bodies = {i: _member_body(extracted, r) for i, r in candidates.items()}
        infos = {i: entry_data(b, only(split(b), 1)) for i, b in bodies.items()}
        layout = read_layout(ram, w, h)
        member = find_member(ram, infos, lambda m: _fits(layout, w, h, bodies[m], infos[m]))
        entries_of = split(bodies[member])
        screens = decode_screens(entry_data(bodies[member], only(entries_of, 0)))
        info = decode_tile_info(infos[member])
        hsc, hj, vsc, vj = check_layout(layout, w, h, solid_masks(screens, info), len(screens))
        overlays = {int(p.stem): p.read_bytes() for p in sorted((extracted / "rock_bin").glob("*.bin"))}
        overlay, offset = find_on_disc(layout, w, overlays)
        diff = differing_cells(overlays[overlay][offset:offset + len(layout)], layout, w, h)
        table_entry, table_area, obj_overlay, obj_offset, data = find_stage_list(ram, w, h, overlays, overlay)
        count = len(decode(data, w, h))
        texture = _find_texture(vram, bodies[member], entries_of)
        layers = layers_from(layout, w, h)
        sp = sprites(ram)
        rule = learn_rule(sp, cameras(sp, layers, w, h, screens, info), layers, w, h, screens, info)
    except (SaveStateError, LayoutError, ObjectError, VramError, PaletteError, ArcError, LearnFailed, ValueError,
            OSError) as e:
        log(f"error: {e}")
        return 1
    clash = next((e for e in entries if e["name"] != name and e["member"] == member and slug(e["name"]) == slug(name)), None)
    if clash:
        log(f"error: name {name!r} gives the same folder as entry {clash['name']!r}")
        return 2
    entry = {"name": name, "member": member, "width": w, "height": h, "overlay": overlay, "offset": offset,
             "save_state": state, "save_state_sha1": sha1,
             "edge": {"h": round(hsc, 3), "h_joins": hj, "v": round(vsc, 3), "v_joins": vj},
             "differing_cells": [list(c) for c in diff],
             "objects": {"overlay": obj_overlay, "offset": obj_offset, "count": count, "table_entry": table_entry,
                         "table_area": table_area},
             "palettes": rule}
    if texture != TEXTURE:
        entry["texture"] = texture
    manifest = [m for m in manifest if m["name"] != state] + [{"name": state, "sha1": sha1, "note": note}]
    entries = [e for e in entries if e["name"] != name] + [entry]
    try:
        _write_list(catalog / "savestates.json", manifest, key=lambda m: m["name"])
        _write_list(catalog / "stages.json", entries, key=lambda e: (e["member"], e["name"]))
    except OSError as e:
        log(f"error: {e}")
        return 1
    where = " (layer 0 row 0)" if diff and all(l == 0 and y == 0 for l, _, y in diff) else ""
    log(f"{name}: member {member}, {w}x{h}, overlay {overlay} @{offset:#x}, edges {hsc:.3f}/{vsc:.3f}, "
        f"{len(diff)} cells differ{where}, {count} objects @{obj_offset:#x}, "
        f"palettes @({rule['clut_x']}, {rule['clut_y']}) from {rule['matches']} tiles"
        + (f", texture {texture:#x}" if texture != TEXTURE else ""))
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.learn")
    parser.add_argument("--state", required=True)
    parser.add_argument("--name", required=True)
    parser.add_argument("--note", default="")
    parser.add_argument("--dumps", default=str(DEFAULT_DUMPS))
    parser.add_argument("--extracted", default=str(DEFAULT_EXTRACTED))
    parser.add_argument("--catalog", default=str(DEFAULT_CATALOG))
    args = parser.parse_args(argv)
    return run(args.state, args.name, args.note, Path(args.dumps).expanduser(), Path(args.extracted).expanduser(),
               Path(args.catalog).expanduser(), log=print)


if __name__ == "__main__":
    sys.exit(main())
