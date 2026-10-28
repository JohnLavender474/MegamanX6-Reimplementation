import gzip
import zlib
from pathlib import Path

RAM_OFFSET = 0x9025
RAM_SIZE = 0x200000
HEADER = b"STv4 PCSXR"
TEXT_RAM = 0x11000
TEXT_EXE = 0x1800
TEXT_LEN = 0x400


class SaveStateError(Exception):
    pass


def read_state(path):
    path = Path(path)
    raw = path.read_bytes()
    try:
        data = gzip.decompress(raw)
    except (OSError, EOFError, zlib.error) as e:
        raise SaveStateError(f"{path}: not a gzip save state ({e})") from e
    if not data.startswith(HEADER):
        raise SaveStateError(f"{path}: header {data[:16]!r} is not a PCSXR save state")
    return data


def ram_from(state, exe, path):
    if len(state) < RAM_OFFSET + RAM_SIZE:
        raise SaveStateError(f"{path}: {len(state)} bytes, too short for 2 MB of RAM at {RAM_OFFSET:#x}")
    ram = state[RAM_OFFSET:RAM_OFFSET + RAM_SIZE]
    if ram[TEXT_RAM:TEXT_RAM + TEXT_LEN] != exe[TEXT_EXE:TEXT_EXE + TEXT_LEN]:
        raise SaveStateError(f"{path}: RAM does not hold the X6 program text at {TEXT_RAM:#x}")
    return ram


def read_ram(path, exe):
    return ram_from(read_state(path), exe, path)


def ram_at(ram, addr, n):
    offset = addr & 0x1FFFFF
    return ram[offset:offset + n]
