import hashlib
from pathlib import Path

SECTOR_RAW = 2352
FORM1_DATA = 2048
FORM2_RAW = 2336
USER_OFFSET = 24
SUBHEADER_OFFSET = 16
SUBMODE_OFFSET = 18


class DiscError(Exception):
    pass


def parse_cue(cue_path):
    cue_path = Path(cue_path)
    text = cue_path.read_bytes().decode("utf-8-sig")
    lines = [line.strip() for line in text.splitlines() if line.strip()]
    files = [line for line in lines if line.upper().startswith("FILE ")]
    tracks = [line for line in lines if line.upper().startswith("TRACK ")]
    if len(files) != 1 or len(tracks) != 1:
        raise DiscError(f"{cue_path}: need exactly 1 FILE and 1 TRACK, found {len(files)} and {len(tracks)}")
    mode = tracks[0].split()[-1].upper()
    if mode != "MODE2/2352":
        raise DiscError(f"{cue_path}: track mode is {mode}, need MODE2/2352")
    line = files[0]
    first, last = line.find('"'), line.rfind('"')
    if first < 0 or last <= first:
        raise DiscError(f"{cue_path}: FILE line has no quoted name: {line}")
    return cue_path.parent / line[first + 1:last]


class DiscImage:
    def __init__(self, bin_path):
        self.path = Path(bin_path)
        size = self.path.stat().st_size
        if size % SECTOR_RAW:
            raise DiscError(f"{self.path}: size {size} is not a multiple of {SECTOR_RAW}")
        self.sector_count = size // SECTOR_RAW
        self._file = open(self.path, "rb")

    def close(self):
        self._file.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def raw(self, lba):
        if not 0 <= lba < self.sector_count:
            raise DiscError(f"LBA {lba} outside image (0..{self.sector_count - 1})")
        self._file.seek(lba * SECTOR_RAW)
        return self._file.read(SECTOR_RAW)

    def form1(self, lba):
        return self.raw(lba)[USER_OFFSET:USER_OFFSET + FORM1_DATA]

    def form2_raw(self, lba):
        return self.raw(lba)[SUBHEADER_OFFSET:SUBHEADER_OFFSET + FORM2_RAW]

    def submode(self, lba):
        return self.raw(lba)[SUBMODE_OFFSET]


def sha1_file(path):
    h = hashlib.sha1()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def check_identity(bin_path, expected_sha1):
    actual = sha1_file(bin_path)
    if actual != expected_sha1:
        raise DiscError(f"{bin_path}: sha1 {actual} != expected {expected_sha1}")
