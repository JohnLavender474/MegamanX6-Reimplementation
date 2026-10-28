import struct

VRAM_OFFSET = 0x29B749
VRAM_SIZE = 0x100000
VRAM_W = 1024
VRAM_H = 512
ROW = 2048
GPU_HEADER = 0x408
GPU_VERSION = 1
PAGES = 8
PAGE_W = 64
PAGE_ROWS = 256
PAGE_BYTES = 0x8000
TEX_ROW = 128


class VramError(Exception):
    pass


def read_vram(state):
    if len(state) < VRAM_OFFSET + VRAM_SIZE:
        raise VramError(f"save state is {len(state)} bytes, too short for VRAM at {VRAM_OFFSET:#x}")
    (version,) = struct.unpack_from("<I", state, VRAM_OFFSET - GPU_HEADER)
    if version != GPU_VERSION:
        raise VramError(f"GPU state version {version} at {VRAM_OFFSET - GPU_HEADER:#x}, expected {GPU_VERSION}")
    return bytes(state[VRAM_OFFSET:VRAM_OFFSET + VRAM_SIZE])


def _differing_rows(vram, block, x0, y0):
    bad = 0
    for p in range(PAGES):
        for r in range(PAGE_ROWS):
            o = (y0 + r) * ROW + (x0 + p * PAGE_W) * 2
            s = p * PAGE_BYTES + r * TEX_ROW
            if vram[o:o + TEX_ROW] != block[s:s + TEX_ROW]:
                bad += 1
    return bad


def pages_from_rows(block):
    if len(block) != PAGES * PAGE_BYTES:
        raise VramError(f"row texture block is {len(block)} bytes, need {PAGES * PAGE_BYTES}")
    line = PAGES * TEX_ROW
    return b"".join(block[r * line + p * TEX_ROW:r * line + (p + 1) * TEX_ROW]
                    for p in range(PAGES) for r in range(PAGE_ROWS))


def find_textures(vram, block):
    scores = {(x0, y0): _differing_rows(vram, block, x0, y0)
              for y0 in (0, VRAM_H - PAGE_ROWS) for x0 in range(0, VRAM_W - PAGES * PAGE_W + 1, PAGE_W)}
    hits = [pos for pos, bad in scores.items() if bad == 0]
    if len(hits) != 1:
        (x, y), bad = min(scores.items(), key=lambda kv: (kv[1], kv[0][1], kv[0][0]))
        raise VramError(f"texture block found at {len(hits)} VRAM positions; need exactly 1 "
                        f"(closest: ({x}, {y}) with {bad} differing rows)")
    return hits[0]
