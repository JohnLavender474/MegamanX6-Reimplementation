import struct
import zlib
from pathlib import Path

SIGNATURE = b"\x89PNG\r\n\x1a\n"


def _chunk(kind, data):
    return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data))


def write_png(path, width, height, rows):
    Path(path).write_bytes(encode_png(width, height, rows))


def encode_png(width, height, rows):
    if len(rows) != height:
        raise ValueError(f"{len(rows)} rows, expected {height}")
    for n, row in enumerate(rows):
        if len(row) != width * 4:
            raise ValueError(f"row {n} has {len(row)} bytes, expected {width * 4}")
    raw = b"".join(b"\0" + row for row in rows)
    header = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    return SIGNATURE + _chunk(b"IHDR", header) + _chunk(b"IDAT", zlib.compress(raw, 9)) + _chunk(b"IEND", b"")
