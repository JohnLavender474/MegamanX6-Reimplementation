import hashlib
import json

from mmxdemake.sprites import cell, cell_bytes, loaded, palette_words, pieces

VERSION = 1
ID_HEX = 10
PREFIX = "enemy-"
KEPT_FLAGS = 0xF0


def content_id(ram, vram, bank, animations, mode):
    texture = loaded(bank)
    used = sorted({f["sprite"] for frames, _ in animations for f in frames})
    sprites = []
    for n in used:
        parts = []
        for flags, tile, dx, dy in pieces(ram, bank, n):
            page, u, v = cell(flags, tile, texture, mode)
            colours = palette_words(vram, bank.clut + ((flags >> 2) & 3))
            parts.append([flags & KEPT_FLAGS, dx, dy, cell_bytes(vram, page, u, v).hex(), colours])
        sprites.append([n, parts])
    doc = {
        "version": VERSION,
        "animations": [[[[f["sprite"], f["ticks"], f["flag"]] for f in frames], loop] for frames, loop in animations],
        "sprites": sprites,
    }
    text = json.dumps(doc, sort_keys=True, separators=(",", ":"))
    return PREFIX + hashlib.sha1(text.encode()).hexdigest()[:ID_HEX]
