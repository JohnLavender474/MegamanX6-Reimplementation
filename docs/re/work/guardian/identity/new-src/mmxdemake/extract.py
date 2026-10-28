import argparse
import hashlib
import sys
from math import ceil
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.cdimage import FORM1_DATA, DiscError, DiscImage, check_identity, parse_cue
from mmxdemake.index import build_index, write_index
from mmxdemake.iso9660 import iter_file, walk
from mmxdemake.problem import Problem
from mmxdemake.rockarc import (
    SECTOR, check_toc, observe_header, parse_toc, read_member, tail_padding_is_zero,
)
from mmxdemake.verify import compare, load_reference

MEDIUM = {"serial": "SLUS-01395", "revision": "v1.1", "bin_sha1": "d4f7e08371027a87a3bf13311db5a4c56733f4ea"}
BIN_MEMBERS = 59
DAT_MEMBERS = 243
HEADER_SECTORS = 2
SUBMODE_FORM2 = 0x20
REPO_ROOT = Path(__file__).resolve().parents[1]


class StageFailed(Exception):
    def __init__(self, problems):
        super().__init__(f"{len(problems)} problems")
        self.problems = problems


OUTPUT_ENTRIES = {"iso", "rock_bin", "rock_dat", "index.json"}


def _extract_iso(disc, tmp, log):
    records = []
    files = walk(disc)
    for f in files:
        dest = tmp / "iso" / f.path
        dest.parent.mkdir(parents=True, exist_ok=True)
        h = hashlib.sha1()
        written = 0
        with open(dest, "wb") as out:
            for chunk in iter_file(disc, f):
                out.write(chunk)
                h.update(chunk)
                written += len(chunk)
        records.append({
            "kind": "iso", "path": f"iso/{f.path}", "lba": f.lba, "sectors": ceil(f.size / FORM1_DATA),
            "form": f.form, "size": written, "sha1": h.hexdigest(),
            "observed": {"first_submode": f.first_submode},
        })
        if bool(f.first_submode & SUBMODE_FORM2) != (f.form == 2):
            log(f"note: {f.path}: directory says Form {f.form}, first sector submode {f.first_submode:#04x}")
    log(f"ISO: {len(files)} files")
    return records, {f.path for f in files}


def _split(tmp, name, kind, width, expected, hard_tail, log):
    data = (tmp / "iso" / name).read_bytes()
    header = data[:HEADER_SECTORS * SECTOR]
    members = parse_toc(header)
    problems = check_toc(name, members, header, ceil(len(data) / SECTOR), HEADER_SECTORS, expected)
    if problems:
        raise StageFailed(problems)
    (tmp / kind).mkdir()
    records, tail_bad = [], []
    for m in members:
        body = read_member(data, m)
        rel = f"{kind}/{m.index:0{width}d}.bin"
        (tmp / rel).write_bytes(body)
        tail_zero = tail_padding_is_zero(data, m)
        if not tail_zero:
            tail_bad.append(Problem(f"{name} member {m.index}", "bytes after size in last sector are not zero"))
        record = {"kind": kind, "path": rel, "index": m.index, "sector": m.sector, "size": m.size,
                  "sha1": hashlib.sha1(body).hexdigest()}
        if not hard_tail:
            record["observed"] = observe_header(body, tail_zero)
        records.append(record)
    if hard_tail and tail_bad:
        raise StageFailed(tail_bad)
    zero = len(members) - len(tail_bad)
    log(f"{name}: {len(members)} members, TOC ok, {zero}/{len(members)} tail-zero{'' if hard_tail else ' (observation)'}")
    return records


def run(cue, out, reference=None, expected_sha1=MEDIUM["bin_sha1"], bin_members=BIN_MEMBERS,
        dat_members=DAT_MEMBERS, log=print):
    out = Path(out)
    refusal = outdir.check(out, REPO_ROOT, OUTPUT_ENTRIES.__contains__)
    if refusal:
        log(f"error: {refusal}")
        return 2
    ref = None
    if reference is not None:
        try:
            ref = load_reference(reference)
        except (OSError, ValueError) as e:
            log(f"error: {e}")
            return 2
    try:
        bin_path = parse_cue(cue)
        if not bin_path.exists():
            raise DiscError(f"{cue}: FILE names {bin_path}, which does not exist")
        check_identity(bin_path, expected_sha1)
    except (DiscError, OSError) as e:
        log(f"error: {e}")
        return 2
    log("identity: ok")
    try:
        _produce(bin_path, out, ref, expected_sha1, bin_members, dat_members, log)
    except StageFailed as e:
        for p in e.problems:
            log(f"error: {p}")
        return 1
    except (DiscError, OSError) as e:
        log(f"error: {e}")
        return 1
    log(f"done: {out}")
    return 0


def _produce(bin_path, out, ref, expected_sha1, bin_members, dat_members, log):
    tmp = outdir.fresh_tmp(out)
    with DiscImage(bin_path) as disc:
        records, names = _extract_iso(disc, tmp, log)
    missing = [n for n in ("ROCK_X6.BIN", "ROCK_X6.DAT") if n not in names]
    if missing:
        raise StageFailed([Problem("ISO", f"{n} not found on disc") for n in missing])
    records += _split(tmp, "ROCK_X6.BIN", "rock_bin", 2, bin_members, True, log)
    records += _split(tmp, "ROCK_X6.DAT", "rock_dat", 3, dat_members, False, log)
    index = build_index(dict(MEDIUM, bin_sha1=expected_sha1), records)
    write_index(tmp / "index.json", index)
    if ref is None:
        log("verify: no reference given; only structural checks ran")
    else:
        problems, unreferenced = compare(index, ref)
        if problems:
            raise StageFailed(problems)
        log(f"verify: {len(ref)} reference hashes match; {len(unreferenced)} outputs have no reference")
    outdir.publish(tmp, out)


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.extract")
    parser.add_argument("--cue", required=True)
    parser.add_argument("--out", required=True)
    parser.add_argument("--reference")
    args = parser.parse_args(argv)
    reference = Path(args.reference).expanduser() if args.reference else None
    return run(Path(args.cue).expanduser(), Path(args.out).expanduser(), reference,
               MEDIUM["bin_sha1"], BIN_MEMBERS, DAT_MEMBERS)


if __name__ == "__main__":
    sys.exit(main())
