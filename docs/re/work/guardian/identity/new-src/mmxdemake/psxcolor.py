import struct


def _expand(v):
    return (v << 3) | (v >> 2)


def to_rgba(c):
    return (_expand(c & 31), _expand((c >> 5) & 31), _expand((c >> 10) & 31), 0 if c == 0 else 255)


def decode_clut(data):
    if len(data) % 2:
        raise ValueError(f"palette length {len(data)} is odd")
    return list(struct.unpack(f"<{len(data) // 2}H", data))


def stats(clut):
    return {
        "first_transparent": bool(clut) and clut[0] == 0,
        "stp": sum(1 for c in clut if c & 0x8000),
        "black": sum(1 for c in clut if (c & 0x7FFF) == 0),
        "size": len(clut),
    }
