import struct

LIMIT = 1 << 22


class LzError(Exception):
    pass


def _word(data, pos):
    if pos + 2 > len(data):
        raise LzError(f"packed data ends at {len(data):#x}, before the word at {pos:#x}")
    return struct.unpack_from("<H", data, pos)[0]


def decode(data, pos=0, limit=LIMIT):
    out = []
    while True:
        flags = _word(data, pos)
        pos += 2
        for bit in range(15, -1, -1):
            if not flags >> bit & 1:
                out.append(_word(data, pos))
                pos += 2
                continue
            v = _word(data, pos)
            pos += 2
            if v & 0xF800:
                n, dist = v >> 11, v & 0x7FF
            else:
                n, dist = _word(data, pos), v
                pos += 2
            if n == 0 and dist == 0:
                return struct.pack(f"<{len(out)}H", *out), pos
            if dist == 0:
                out.extend([0] * n)
            elif dist > len(out):
                raise LzError(f"copy at {pos:#x} reaches {dist} words back, before the start")
            else:
                start = len(out) - dist
                for k in range(n):
                    out.append(out[start + k])
            if len(out) > limit:
                raise LzError(f"packed data at {pos:#x} unpacks to more than {limit} words")
