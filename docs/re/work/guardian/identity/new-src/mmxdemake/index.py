import json
from pathlib import Path

SCHEMA_VERSION = 1


def build_index(medium, files):
    return {"schema_version": SCHEMA_VERSION, "medium": medium, "files": sorted(files, key=lambda r: r["path"])}


def dumps(index):
    return json.dumps(index, sort_keys=True, indent=2) + "\n"


def write_index(path, index):
    Path(path).write_text(dumps(index))


def load_index(path):
    index = json.loads(Path(path).read_text())
    version = index.get("schema_version")
    if version != SCHEMA_VERSION:
        raise ValueError(f"{path}: schema_version {version}, this tool reads {SCHEMA_VERSION}")
    return index
