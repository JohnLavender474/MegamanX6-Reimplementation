import argparse
import os
import re
import shutil
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
DATA_FOLDERS = ("re", "notes", "stages")
DATA_TYPES = {".md", ".py", ".sh", ".c", ".h"}
SNAPSHOTS = ("re/merge-sheldon/index-check", "re/exporter-sets/check/old-code", "re/guardian/identity/old-src")
FENCE = re.compile(r"^\s*(`{3,}|~{3,})")
SPAN = re.compile(r"(?<!\[)`(?P<path>(?:~|/home/[^/`]+)/(?P<root>mmx-data/x6|IdeaProjects/mmx-demake-tools/docs)/(?P<rel>[^`\s]+))`")
LINK = re.compile(r"\]\((?P<path>(?:~|/home/[^/)]+)/(?P<root>mmx-data/x6|IdeaProjects/mmx-demake-tools/docs)/(?P<rel>[^)\s]+))\)")


def copy_all(files, dst_of):
    for f in files:
        out = dst_of(f)
        out.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(f, out)
    return len(files)


def tool_docs(src):
    return sorted(f for f in src.rglob("*.md") if f.is_file())


def data_docs(root):
    skip = [root / s for s in SNAPSHOTS]
    files = []
    for folder in DATA_FOLDERS:
        for f in sorted((root / folder).rglob("*")):
            if not f.is_file() or f.suffix not in DATA_TYPES or "__pycache__" in f.parts:
                continue
            if any(f.is_relative_to(s) for s in skip):
                continue
            files.append(f)
    return files


def work_path(rel):
    parts = Path(rel).parts
    return Path(*parts[1:]) if parts and parts[0] == "re" else Path(rel)


def target(out, root, rel):
    if root == "mmx-data/x6":
        if Path(rel).parts[0] not in DATA_FOLDERS:
            return None
        path = out / "work" / work_path(rel)
    else:
        path = out / "findings" / rel
    return path if path.exists() else None


def link_paths(md, out, counts):
    def href(m):
        t = target(out, m.group("root"), m.group("rel").rstrip("/"))
        if t is None:
            return None
        return os.path.relpath(t, md.parent) + ("/" if t.is_dir() else "")

    def span(m):
        h = href(m)
        if h is None:
            return m.group(0)
        counts["links"] += 1
        return f"[{m.group(0)}]({h})"

    def link(m):
        h = href(m)
        if h is None:
            return m.group(0)
        counts["links"] += 1
        return f"]({h})"

    lines = md.read_text(encoding="utf-8").split("\n")
    fence = None
    for i, line in enumerate(lines):
        m = FENCE.match(line)
        if m:
            mark = m.group(1)
            if fence is None:
                fence = mark
            elif mark[0] == fence[0] and len(mark) >= len(fence) and not line.strip()[len(mark):]:
                fence = None
            continue
        if fence is None:
            lines[i] = LINK.sub(link, SPAN.sub(span, line))
    md.write_text("\n".join(lines), encoding="utf-8")


def main():
    p = argparse.ArgumentParser(description="Copy the RE docs of mmx-demake-tools to docs/re/findings/ and the RE notes and scripts of the data root to docs/re/work/. Both folders are replaced as a whole, and paths into the copies become relative links.")
    p.add_argument("--tools", type=Path, default=Path.home() / "IdeaProjects" / "mmx-demake-tools",
                   help="the tools repo (default: ~/IdeaProjects/mmx-demake-tools)")
    p.add_argument("--from", dest="source", type=Path, default=Path.home() / "mmx-data" / "x6",
                   help="the data root (default: ~/mmx-data/x6)")
    p.add_argument("--out", type=Path, default=REPO / "docs" / "re")
    args = p.parse_args()

    docs = args.tools / "docs"
    for folder in [docs] + [args.source / f for f in DATA_FOLDERS]:
        if not folder.is_dir():
            raise SystemExit(f"missing {folder}")
    for name in ("findings", "work"):
        if (args.out / name).exists():
            shutil.rmtree(args.out / name)
    counts = {"findings": 0, "work": 0, "links": 0}
    counts["findings"] = copy_all(tool_docs(docs), lambda f: args.out / "findings" / f.relative_to(docs))
    counts["work"] = copy_all(data_docs(args.source),
                              lambda f: args.out / "work" / work_path(f.relative_to(args.source)))
    for md in sorted((args.out / "findings").rglob("*.md")) + sorted((args.out / "work").rglob("*.md")):
        link_paths(md, args.out, counts)
    print(", ".join(f"{v} {k}" for k, v in counts.items()))


if __name__ == "__main__":
    main()
