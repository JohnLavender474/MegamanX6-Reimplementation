"""playerexport with X's armour layer: a test of the proposed tool change (notes.md).

usage (from the mmx-demake-tools repo root):
  python3 layered_export.py --state NAME.state.gz --dumps DIR --root ROOT [--name x-blade]

It runs mmxdemake.playerexport.run unchanged, but replaces its render() so that each sprite is
the body layer (sprite entry 0 / 18, CLUT mega+0x42 + piece bits) with the armour layer on top
(sprite entry mega+0xDC = 5 / 19, CLUT from the table 0x80071E68 by piece bits), as the game
draws it in 0x80022AE0 -> 0x8002619C -> 0x80025984 for forms 1-4. The repo is not changed.
"""
import argparse
import gzip
import json
import os
import struct
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, os.getcwd())
import mmxdemake.playerexport as pe
import mmxdemake.sprites as sp
from mmxdemake.savestate import read_ram, ram_at

MEGA = 0x800970A0
LAYER_CLUTS = 0x80071E68
ARMOR_ENTRY = {0: 5, 18: 19}
FAKE = 0x7F00
BEAM_SPRITE = 126

clut_map = {}
real_palette_words = sp.palette_words


def palette_words(vram, clut, n=16):
    return real_palette_words(vram, clut_map.get(clut, clut), n)


sp.palette_words = palette_words


def merge(top, under):
    canvas = {}
    for im in (under, top):
        for y in range(im.height):
            for x in range(im.width):
                c = im.pixels[y][x]
                if c[3]:
                    canvas[(x - im.ox, y - im.oy)] = c
    return sp._image(canvas)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--state', required=True)
    ap.add_argument('--dumps', required=True)
    ap.add_argument('--root', required=True)
    ap.add_argument('--name', default=None)
    a = ap.parse_args()
    path = Path(a.dumps) / a.state
    data = gzip.open(path).read()
    exe = (Path(pe.DEFAULT_EXTRACTED).expanduser() / pe.EXE).read_bytes()
    ram = read_ram(path, exe)
    scratch = sp.scratchpad(data)
    sdir = struct.unpack_from('<I', scratch, 0x1C)[0]
    form = ram_at(ram, MEGA + 0xE7, 1)[0]
    character = ram_at(ram, MEGA + 2, 1)[0]
    layered = character == 0 and 1 <= form <= 4
    for k in range(4):
        clut_map[FAKE + k] = struct.unpack('<H', ram_at(ram, LAYER_CLUTS + 2 * k, 2))[0]
    real_render = pe.render

    def render(ram_, vram, s, mode):
        images, count, end = real_render(ram_, vram, s, mode)
        if not layered or s.sprite_entry not in ARMOR_ENTRY:
            return images, count, end
        e = ARMOR_ENTRY[s.sprite_entry]
        adata = (sdir + struct.unpack('<I', ram_at(ram_, sdir + 4 * e, 4))[0]) & 0xFFFFFFFF
        armor = sp.Bank(-1, None, adata, struct.unpack('<H', ram_at(ram_, adata + 2, 2))[0],
                        s.bank.texture, FAKE)
        out = []
        for n, body in enumerate(images):
            if n >= armor.count or (s.sprite_entry == 0 and n == BEAM_SPRITE and form != 4):
                out.append(body)
                continue
            tiles, packed, _ = pe.unpack(ram_, s.frames, n)
            top = sp.draw(ram_, pe.place(vram, tiles, packed, s.bank.texture), armor, n, mode)
            out.append(merge(top, body))
        return out, count, end

    pe.render = render
    with tempfile.TemporaryDirectory() as cat:
        names = pe.load_names(Path(pe.DEFAULT_CATALOG).expanduser())
        if a.name:
            names[89] = a.name
        Path(cat, 'players.json').write_text(json.dumps({'members': {str(k): v for k, v in names.items()}}))
        rc = pe.run(a.state, Path(a.dumps), Path(pe.DEFAULT_EXTRACTED).expanduser(), Path(cat),
                    Path(a.root), log=print)
    print('form', form, 'armour layer', 'on' if layered else 'off')
    return rc


sys.exit(main())
