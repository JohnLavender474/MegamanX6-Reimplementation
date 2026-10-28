import shutil
from pathlib import Path


def tmp_of(out):
    out = Path(out)
    return out.with_name(out.name + ".tmp")


def _inside(path, folder):
    try:
        Path(path).resolve().relative_to(Path(folder).resolve())
        return True
    except ValueError:
        return False


def _usable(path, allowed):
    if path.is_symlink():
        return False
    if not path.exists():
        return True
    return path.is_dir() and all(allowed(p.name) for p in path.iterdir())


def check(out, repo_root, allowed):
    out = Path(out)
    if _inside(out, repo_root):
        return f"--out {out} is inside the repo {repo_root}; game data must stay outside it"
    for path in (out, tmp_of(out)):
        if not _usable(path, allowed):
            return f"{path} exists and is not an earlier output of this tool; choose another --out"
    return None


def fresh_tmp(out):
    tmp = tmp_of(out)
    if tmp.exists():
        shutil.rmtree(tmp)
    tmp.mkdir(parents=True)
    return tmp


def publish(tmp, out):
    out = Path(out)
    if out.exists():
        shutil.rmtree(out)
    Path(tmp).rename(out)
