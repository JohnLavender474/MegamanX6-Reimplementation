import struct

from mmxdemake.savestate import ram_at

MAX_ANIMS = 64
MAX_STEPS = 256
FPS = 60
LONGEST = 256


class AnimError(Exception):
    pass


def read_table(ram, addr, lo, hi, limit=MAX_ANIMS):
    if not lo <= addr < hi:
        raise AnimError(f"animation table {addr:#x} is outside the overlay {lo:#x}..{hi:#x}")
    out = []
    while len(out) < limit:
        (p,) = struct.unpack("<I", ram_at(ram, addr + 4 * len(out), 4))
        if not lo <= p < addr:
            break
        out.append(p)
    if not out:
        raise AnimError(f"animation table {addr:#x}: first word {p:#x} is not a frame list "
                        f"between {lo:#x} and {addr:#x}")
    return out


def walk(ram, ptr):
    frames, seen, i = [], {}, 0
    while i not in seen:
        if len(frames) == MAX_STEPS:
            raise AnimError(f"animation {ptr:#x}: more than {MAX_STEPS} frames without a hold or a loop")
        ticks, flag, step, sprite = struct.unpack("<BBbB", ram_at(ram, ptr + 4 * i, 4))
        seen[i] = len(frames)
        frames.append({"sprite": sprite, "ticks": ticks or LONGEST, "flag": flag})
        if step == 0:
            return frames, None
        i += step
    return frames, seen[i]
