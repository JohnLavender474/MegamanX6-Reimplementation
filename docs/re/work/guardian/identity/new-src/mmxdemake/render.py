import argparse
import hashlib
import sys
from math import ceil
from pathlib import Path

from mmxdemake import outdir
from mmxdemake.index import load_index
from mmxdemake.indexed import dimensions, to_rgba_rows, unpack
from mmxdemake.png import write_png
from mmxdemake.psxcolor import decode_clut, stats, to_rgba

REPO_ROOT = Path(__file__).resolve().parents[1]
PALETTE_SIZE = 512
PIXELS_SIZE = 8192
PAIR_BPP = 8
PAIR_WIDTH = 128
EXPECT_PAIRS = 19
SWATCH = 16
SWATCH_COLUMNS = 16


class RenderFailed(Exception):
    pass


def _allowed(name):
    return name.endswith(".png") or name == "summary.txt"


def find_pairs(records):
    by_index = {r["index"]: r for r in records}
    return [(i, i + 1) for i in sorted(by_index)
            if by_index[i]["size"] == PALETTE_SIZE and i + 1 in by_index and by_index[i + 1]["size"] == PIXELS_SIZE]


def _member(extracted, by_index, i):
    r = by_index.get(i)
    if r is None:
        raise RenderFailed(f"rock_dat member {i} is not in the index")
    data = (extracted / r["path"]).read_bytes()
    sha1 = hashlib.sha1(data).hexdigest()
    if len(data) != r["size"] or sha1 != r["sha1"]:
        raise RenderFailed(f"{r['path']}: size {len(data)}, sha1 {sha1} do not match the index ({r['size']}, {r['sha1']})")
    return data


def _swatch_rows(clut):
    rows = []
    for y in range(ceil(len(clut) / SWATCH_COLUMNS) * SWATCH):
        row = bytearray()
        for x in range(SWATCH_COLUMNS):
            k = (y // SWATCH) * SWATCH_COLUMNS + x
            row += bytes(to_rgba(clut[k]) if k < len(clut) else (0, 0, 0, 0)) * SWATCH
        rows.append(bytes(row))
    return rows


def _render_one(tmp, extracted, by_index, pal_i, pix_i, bpp, width, clut_row, image_name, note):
    palette_data = _member(extracted, by_index, pal_i)
    pixel_data = _member(extracted, by_index, pix_i)
    try:
        clut = decode_clut(palette_data)
        w, h = dimensions(len(pixel_data), bpp, width)
        rows = to_rgba_rows(unpack(pixel_data, bpp), clut, w, 16 * clut_row if bpp == 4 else 0)
    except ValueError as e:
        raise RenderFailed(f"pair {pal_i}/{pix_i}: {e}") from e
    write_png(tmp / image_name, w, h, rows)
    swatch = _swatch_rows(clut)
    write_png(tmp / f"clut{pal_i:03d}.png", SWATCH_COLUMNS * SWATCH, len(swatch), swatch)
    s = stats(clut)
    first = "transparent" if s["first_transparent"] else "opaque"
    note(f"pair {pal_i}/{pix_i}: {w}x{h} {bpp}bpp, clut[0] {first}, stp {s['stp']}/{s['size']}, black {s['black']}")


def run(extracted, out, palette=None, pixels=None, bpp=None, width=None, clut_row=0,
        expect_pairs=EXPECT_PAIRS, log=print):
    extracted, out = Path(extracted), Path(out)
    manual = [palette, pixels, bpp, width]
    if any(v is not None for v in manual) and not all(v is not None for v in manual):
        log("error: manual mode needs all of --palette, --pixels, --bpp and --width")
        return 2
    refusal = outdir.check(out, REPO_ROOT, _allowed)
    if refusal:
        log(f"error: {refusal}")
        return 2
    try:
        records = [r for r in load_index(extracted / "index.json")["files"] if r["kind"] == "rock_dat"]
    except (OSError, ValueError) as e:
        log(f"error: {e}")
        return 2
    by_index = {r["index"]: r for r in records}
    lines = []

    def note(line):
        lines.append(line)
        log(line)

    try:
        if palette is None:
            pairs = find_pairs(records)
            if len(pairs) != expect_pairs:
                raise RenderFailed(f"found {len(pairs)} palette/image pairs, expected {expect_pairs}")
            jobs = [(p, q, PAIR_BPP, PAIR_WIDTH, 0, f"pair{p:03d}_{q:03d}.png") for p, q in pairs]
        else:
            jobs = [(palette, pixels, bpp, width, clut_row, f"pal{palette:03d}_pix{pixels:03d}_{bpp}bpp_w{width}.png")]
        tmp = outdir.fresh_tmp(out)
        for job in jobs:
            _render_one(tmp, extracted, by_index, *job, note)
        (tmp / "summary.txt").write_text("\n".join(lines) + "\n")
        outdir.publish(tmp, out)
    except (RenderFailed, OSError) as e:
        log(f"error: {e}")
        return 1
    log(f"done: {out}")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog="python3 -m mmxdemake.render")
    parser.add_argument("--extracted", required=True)
    parser.add_argument("--out", required=True)
    parser.add_argument("--palette", type=int)
    parser.add_argument("--pixels", type=int)
    parser.add_argument("--bpp", type=int, choices=(4, 8))
    parser.add_argument("--width", type=int)
    parser.add_argument("--clut-row", type=int, default=0, choices=range(16))
    parser.add_argument("--expect-pairs", type=int, default=EXPECT_PAIRS)
    args = parser.parse_args(argv)
    return run(Path(args.extracted).expanduser(), Path(args.out).expanduser(), args.palette, args.pixels,
               args.bpp, args.width, args.clut_row, args.expect_pairs)


if __name__ == "__main__":
    sys.exit(main())
