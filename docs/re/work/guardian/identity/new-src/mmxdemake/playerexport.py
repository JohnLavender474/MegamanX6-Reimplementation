import argparse
import hashlib
import json
import re
import shutil
import struct
import sys
from collections import namedtuple
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.animfind import MODE_BITS, FindError, player_overlay
from mmxdemake.anims import FPS, AnimError, read_table, walk
from mmxdemake.learn import DEFAULT_CATALOG, DEFAULT_DUMPS, DEFAULT_EXTRACTED
from mmxdemake.png import write_png
from mmxdemake.savestate import SaveStateError, ram_at, ram_from, read_state
from mmxdemake.spriteexport import (DEFAULT_ROOT, EXE, NAME, REPO_ROOT, SUFFIX, TOP, ExportFailed, _replace,
                                    geometry, grid, strip)
from mmxdemake.sprites import Bank, SpriteError, draw, scratchpad
from mmxdemake.vram import VramError, read_vram
from mmxdemake.wordlz import LzError, decode

MEGA = 0x800970A0
MAX_ANIMS = 256
MAX_FRAMES = 1024
TILE_BYTES = 128
TILES_PER_ROW = 16
ROW = 2048
PROBE = 4096
FOLDER = "players"
TRASH = "_old"
FRAME_DIR = 0x14
SPRITE_DIR = 0x1C
SET_RULES = {0: (0x801F49F4, {0: (0, 0), 1: (10, 18)}),
             1: (0x801EF620, {0: (0, 0), 1: (1, 5), 2: (2, 7)})}

Player = namedtuple("Player", "character mode animations texture clut")
FrameSet = namedtuple("FrameSet", "flag frame_entry sprite_entry frames bank")


def _u16(ram, addr):
    return struct.unpack("<H", ram_at(ram, addr, 2))[0]


def _u32(ram, addr):
    return struct.unpack("<I", ram_at(ram, addr, 4))[0]


def load_names(catalog):
    path = Path(catalog) / "players.json"
    if not path.exists():
        return {}
    doc = json.loads(path.read_text())
    if not isinstance(doc, dict) or set(doc) != {"members"} or not isinstance(doc["members"], dict):
        raise ValueError("players.json must be an object with only members")
    for k, v in doc["members"].items():
        if not re.fullmatch(r"\d+", k) or not isinstance(v, str) or not NAME.fullmatch(v):
            raise ValueError(f"players.json: bad member entry {k!r}: {v!r}")
    return {int(k): v for k, v in doc["members"].items()}


def player(ram):
    if not ram_at(ram, MEGA, 1)[0]:
        raise SpriteError(f"no live player object at {MEGA:#x} in this state")
    return Player(ram_at(ram, MEGA + 2, 1)[0], ram_at(ram, MEGA, 1)[0] & MODE_BITS, _u32(ram, MEGA + 0x30),
                  _u16(ram, MEGA + 0x40), _u16(ram, MEGA + 0x42))


def frame_sets(ram, scratch, overlay, p, flags):
    if overlay not in SET_RULES:
        raise SpriteError(f"player overlay {overlay} has no frame-set rule")
    rules = SET_RULES[overlay][1]
    frame_dir, sprite_dir = (struct.unpack_from("<I", scratch, o)[0] for o in (FRAME_DIR, SPRITE_DIR))
    out = {}
    for flag in sorted(set(flags) & set(rules)):
        fi, si = rules[flag]
        data = (sprite_dir + _u32(ram, sprite_dir + 4 * si)) & 0xFFFFFFFF
        out[flag] = FrameSet(flag, fi, si, (frame_dir + _u32(ram, frame_dir + 4 * fi)) & 0xFFFFFFFF,
                             Bank(-1, None, data, _u16(ram, data + 2), p.texture, p.clut))
    return out


def frame_count(ram, table, sprites):
    n = (_u32(ram, table) & 0xFFFFF) // 4
    if not 1 <= n <= MAX_FRAMES:
        raise SpriteError(f"frame table {table:#x}: first offset gives {n} frames")
    if n < sprites:
        raise SpriteError(f"frame table {table:#x} has {n} frames, fewer than the {sprites} sprites")
    return n


def unpack(ram, table, n):
    entry = _u32(ram, table + 4 * n)
    tiles, offset = entry >> 20, entry & 0xFFFFF
    start = (table & 0x1FFFFF) + offset
    try:
        out, end = decode(ram, start, tiles * TILE_BYTES // 2)
    except LzError as e:
        raise SpriteError(f"frame {n}: {e}") from e
    if len(out) != tiles * TILE_BYTES:
        raise SpriteError(f"frame {n}: unpacks to {len(out)} bytes, not {tiles} tiles x {TILE_BYTES}")
    return tiles, out, (end & 0x1FFFFF) | 0x80000000


def place(vram, tiles, data, texture):
    page = texture >> 8
    x0 = (page & 15) * 64 + (texture & 15) * 4
    y0 = (page >> 4) * 256 + ((texture >> 4) & 15) * 16
    out = bytearray(vram)
    pos = 0
    for k in range(0, tiles, TILES_PER_ROW):
        width = 4 * min(TILES_PER_ROW, tiles - k) * 2
        for r in range(16):
            o = (y0 + k // TILES_PER_ROW * 16 + r) * ROW + x0 * 2
            out[o:o + width] = data[pos:pos + width]
            pos += width
    return bytes(out)


def render(ram, vram, s, mode):
    count = frame_count(ram, s.frames, s.bank.count)
    images, end = [], s.frames
    for n in range(count):
        tiles, data, stop = unpack(ram, s.frames, n)
        end = max(end, stop)
        if n < s.bank.count:
            images.append(draw(ram, place(vram, tiles, data, s.bank.texture), s.bank, n, mode))
    return images, count, end


def animations(ram, p, pointers, flags, sets, log):
    kept, skipped = [], []
    for k, ptr in enumerate(pointers):
        try:
            if flags[k] not in sets:
                raise AnimError(f"frame-set byte {flags[k]} has no rule")
            frames, loop = walk(ram, ptr)
            top = max(f["sprite"] for f in frames)
            count = sets[flags[k]].bank.count
            if top >= count:
                raise AnimError(f"uses sprite {top}, its frame set has {count}")
        except AnimError as e:
            log(f"animation {k}: skipped: {e}")
            skipped.append({"index": k, "reason": str(e)})
            continue
        kept.append((k, flags[k], frames, loop))
    if not kept:
        raise AnimError(f"animation table {p.animations:#x}: no animation fits")
    return kept, skipped


def find_member(blob, members):
    if not blob:
        raise ExportFailed("the main frame set is empty")
    for n, path in members:
        body = path.read_bytes()
        at = body.find(blob[:PROBE])
        while at >= 0:
            if body[at:at + len(blob)] == blob:
                return n, at
            at = body.find(blob[:PROBE], at + 1)
    raise ExportFailed(f"the frame set ({len(blob)} bytes) is in no ROCK_X6.DAT member")


def publish(root, name, doc, images, kept, log):
    target = root / FOLDER / name
    if target.exists() and not (target / "animations.json").is_file():
        raise ExportFailed(f"{target} is not an earlier player export; choose another --root")
    tmp = outdir.fresh_tmp(root)
    folder = tmp / FOLDER / name
    folder.mkdir(parents=True)
    for flag, set_images in images.items():
        write_png(folder / f"sprites-{flag}.png", *grid(set_images))
    used = {(flag, f["sprite"]) for _, flag, seq, _ in kept for f in seq}
    w, h, ox, oy = geometry({key: images[key[0]][key[1]] for key in used})
    for k, flag, seq, _ in kept:
        write_png(folder / f"anim-{k}.png", *strip(dict(enumerate(images[flag])), seq, w, h, ox, oy))
    doc = dict(doc, cell=[w, h], origin=[ox, oy])
    (folder / "animations.json").write_text(json.dumps(doc, indent=2, sort_keys=True) + "\n")
    try:
        _replace(folder, target, tmp / TRASH)
        shutil.rmtree(tmp)
    except OSError as e:
        raise ExportFailed(f"moving into {root} failed ({e}); run the same state again to repair it") from e


def run(state, dumps, extracted, catalog, root, log=print):
    dumps, extracted, root = Path(dumps), Path(extracted), Path(root)
    if Path(state).name != state or not state.endswith(SUFFIX):
        log(f"error: --state {state!r} must be a file name ending in {SUFFIX}")
        return 2
    try:
        names = load_names(catalog)
        refusal = outdir.check(root, REPO_ROOT, lambda n: n in TOP)
        if refusal:
            raise ValueError(refusal)
    except (OSError, ValueError) as e:
        log(f"error: {e}")
        return 2
    try:
        path = dumps / state
        data = read_state(path)
        exe = (extracted / EXE).read_bytes()
        ram = ram_from(data, exe, path)
        vram = read_vram(data)
        overlays = {int(q.stem): q.read_bytes() for q in sorted((extracted / "rock_bin").glob("*.bin"))}
        overlay, lo, hi = player_overlay(ram, overlays)
        p = player(ram)
        pointers = read_table(ram, p.animations, lo, hi, MAX_ANIMS)
        if overlay not in SET_RULES:
            raise SpriteError(f"player overlay {overlay} has no frame-set rule")
        flags = list(ram_at(ram, SET_RULES[overlay][0], len(pointers)))
        sets = frame_sets(ram, scratchpad(data), overlay, p, flags)
        images, info = {}, {}
        for flag, s in sets.items():
            images[flag], count, end = render(ram, vram, s, p.mode)
            info[flag] = (count, end)
        kept, skipped = animations(ram, p, pointers, flags, sets, log)
        main = sets[0]
        members = sorted((int(q.stem), q) for q in (extracted / "rock_dat").glob("*.bin"))
        member, offset = find_member(ram_at(ram, main.frames, info[0][1] - main.frames), members)
        name = names.get(member, f"player-m{member}")
        doc = {"name": name, "member": member, "member_offset": offset, "fps": FPS,
               "sets": [{"flag": f, "frame_entry": s.frame_entry, "sprite_entry": s.sprite_entry,
                         "frame_table": f"{s.frames:#010x}", "sprite_data": f"{s.bank.data:#010x}",
                         "sprites": s.bank.count, "frames": info[f][0]} for f, s in sets.items()],
               "animations": [{"index": k, "set": flag, "frames": seq, "loop_to": loop}
                              for k, flag, seq, loop in kept],
               "skipped": skipped,
               "source": {"state": state, "state_sha1": hashlib.sha1(path.read_bytes()).hexdigest(),
                          "character": p.character, "overlay": overlay,
                          "animation_table": f"{p.animations:#010x}",
                          "set_bytes": f"{SET_RULES[overlay][0]:#010x}",
                          "texture": f"{p.texture:#06x}", "palette": f"{p.clut:#06x}"}}
        publish(root, name, doc, images, kept, log)
    except (SaveStateError, SpriteError, AnimError, FindError, VramError, ExportFailed, OSError, ValueError) as e:
        log(f"error: {e}")
        return 1
    log(f"{state}: {name} (member {member}, overlay {overlay}), sets "
        + ", ".join(f"{f}: {s.bank.count} sprites" for f, s in sets.items())
        + f"; {len(kept)} animations, {len(skipped)} skipped")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.playerexport")
    parser.add_argument("--state", required=True)
    parser.add_argument("--dumps", default=str(DEFAULT_DUMPS))
    parser.add_argument("--extracted", default=str(DEFAULT_EXTRACTED))
    parser.add_argument("--catalog", default=str(DEFAULT_CATALOG))
    parser.add_argument("--root", default=str(DEFAULT_ROOT))
    args = parser.parse_args(argv)
    return run(args.state, Path(args.dumps).expanduser(), Path(args.extracted).expanduser(),
               Path(args.catalog).expanduser(), Path(args.root).expanduser(), log=print)


if __name__ == "__main__":
    sys.exit(main())
