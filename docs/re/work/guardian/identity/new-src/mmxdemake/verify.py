import json
from pathlib import Path

from mmxdemake.problem import Problem


def load_reference(path):
    out = {}
    for number, line in enumerate(Path(path).read_text().splitlines(), 1):
        if not line.strip():
            continue
        try:
            r = json.loads(line)
            if r["kind"] == "iso":
                out[r["path"]] = (r["sha1"], r["size"])
            elif r["kind"] == "rock":
                out["rock_bin/" + r["path"].split("/", 1)[1]] = (r["sha1"], r["size"])
        except (ValueError, KeyError, IndexError, TypeError) as e:
            raise ValueError(f"{path}:{number}: bad reference record ({type(e).__name__}: {e})") from e
    if not out:
        raise ValueError(f"{path}: no iso or rock records")
    return out


def compare(index, reference):
    by_path = {r["path"]: r for r in index["files"]}
    problems = []
    for path, (sha1, size) in sorted(reference.items()):
        r = by_path.get(path)
        if r is None:
            problems.append(Problem(path, "in reference, missing from output"))
            continue
        if r["size"] != size:
            problems.append(Problem(path, f"size {r['size']} != reference {size}"))
        if r["sha1"] != sha1:
            problems.append(Problem(path, f"sha1 {r['sha1']} != reference {sha1}"))
    unreferenced = sorted(set(by_path) - set(reference))
    return problems, unreferenced
