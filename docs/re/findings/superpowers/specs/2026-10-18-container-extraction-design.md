# Container extraction (sample task A) — design

Date: 2026-10-18. Status: draft for review.

## Purpose

Prove that we can extract data from the Mega Man X6 disc correctly before we
decode any game format. This task opens the three containers on the disc and
writes every member to `~/mmx-data/`. Each output has a check that can fail, and
a failure names the exact file, member or byte range.

This task is the base for sample task C (decode a palette candidate to PNG) and
for all later map, graphics and object work.

## Scope

In scope:

1. Disc image: read a raw `MODE2/2352` BIN through its CUE.
2. ISO9660: list and extract the 9 files on the data track.
3. `ROCK_X6.BIN`: split into its 59 code-overlay members.
4. `ROCK_X6.DAT`: split into its 243 members.
5. An index file (`index.json`) that records every output with its source
   position, size and SHA-1.
6. Verification against the `mmx6` reference manifest (hashes only), and
   structural checks where no reference exists.

Out of scope:

- Decoding the sub-entries inside `ROCK_X6.DAT` members. This task only records
  the header words it sees (see "Observations").
- XA audio and STR video demuxing. These files are extracted and hashed only.
- Any runtime work (emulator, RAM dumps).
- Any Maverick-side output.

## Facts this design depends on

| Fact | Source | Status |
|---|---|---|
| Disc is SLUS-01395 v1.1, BIN SHA-1 `d4f7e08371027a87a3bf13311db5a4c56733f4ea` | Redump 32516, `mmx6/config/medium.sha1` | verified on our copy |
| 9 files; 7 hash-match `mmx6/manifest/retail.jsonl` with our probe | our probe, 2026-10-18 | verified |
| Form 2 detection: directory record XA attribute, big-endian, bit `0x1000` | `mmx6/docs/formats.md` | documented, not yet tested by us |
| First-sector submode does not always show Form 2: `STR/CAPLOGO.STR` LBA 25 has submode `0x48` (bit `0x20` clear). `mmx6` says the two sources agree on all 9 files | our `xxd` check, 2026-10-18 | observed; conflicts with `mmx6`. Our probe trusted the submode and missed the 2 `.STR` hashes |
| Form 2 output: 2336 B per sector (subheader + data + EDC), dumpsxiso convention | `mmx6/docs/formats.md` | documented |
| `ROCK_X6.BIN`: 2-sector header, 59 `{u32 sector, u32 size}` pairs, zero pair, contiguous, no compression | `mmx6/docs/formats.md` | documented, member hashes in manifest |
| `ROCK_X6.DAT`: same TOC form, 243 pairs, zero pair, header tail zero, contiguous from sector 2 to 24,860 (file end) | our probe, 2026-10-18 | observed, no reference |

## Repository and layout

All code is in `~/IdeaProjects/mmx-demake-tools/`. Python 3.10, standard library
only.

```
mmxdemake/
  __init__.py
  problem.py      # Problem(where, message): one failed check
  cdimage.py      # CUE parse, raw sector read, Form 1 / Form 2 user data
  iso9660.py      # PVD, directory walk, file records with form flag
  rockarc.py      # {sector,size} TOC parse and structural checks (BIN and DAT)
  index.py        # index.json model, write and load
  verify.py       # compare outputs to a reference manifest
  extract.py      # CLI entry point
tests/
  synth.py        # builds small synthetic disc images and archives in memory
  test_cdimage.py
  test_iso9660.py
  test_rockarc.py
  test_index_verify.py
  test_extract.py
  test_retail.py  # runs only when MMX6_CUE is set
```

Each module has one job. Imports: `iso9660` uses `cdimage`; `rockarc` and
`verify` use `problem`; `problem`, `cdimage` and `index` import no project
module; `extract` uses all of them.

Commands run from the repo root without `-I`. Isolated mode removes the current
folder from `sys.path`, so `python3 -m mmxdemake…` cannot find the package.
The `-I` rule protects against code planted in a folder of downloaded files.
Here the code is this repo, and game data stays outside it.

## Units

### `cdimage`

- `parse_cue(path) -> Path`: return the BIN path from the single `FILE` line.
  Refuse a CUE with more than one track or a mode other than `MODE2/2352`.
- `DiscImage(bin_path)`: `raw(lba) -> bytes` (2352 B), `form1(lba) -> bytes`
  (2048 B at offset 24), `form2_raw(lba) -> bytes` (2336 B at offset 16),
  `submode(lba) -> int`.
- `check_identity(bin_path, expected_sha1)`: hash the whole BIN. Refuse on a
  mismatch with the two hashes in the message.

### `iso9660`

- `walk(disc) -> list[IsoFile]`, where `IsoFile` has `path`, `lba`, `size`,
  `form` (1 or 2).
- The form comes only from the XA system-use attribute in the directory record.
  The first sector's submode is recorded in `index.json` as an observation
  (`observed.first_submode`). A disagreement is printed as a note, not an
  error, because `STR/CAPLOGO.STR` already disagrees.
- `read_file(disc, f) -> bytes`: Form 1 is `ceil(size/2048)` sectors truncated
  to `size`. Form 2 is `ceil(size/2048)` sectors at 2336 B each.

### `rockarc`

- `parse_toc(header: bytes) -> list[Member]`, where `Member` has `index`,
  `sector`, `size`. Read pairs until the first zero pair.
- `check_toc(members, header, file_sectors, header_sectors=2) -> list[Problem]`:
  - a zero pair exists and every header byte after it is zero;
  - `members[0].sector == header_sectors`;
  - `sector[i+1] == sector[i] + ceil(size[i]/2048)` for every `i`;
  - the last member ends exactly at `file_sectors`;
  - `size[i] >= 4`.
- `read_member(data, m) -> bytes`: `data[sector*2048 : sector*2048 + size]`.
- `tail_padding_is_zero(data, m) -> bool`: the bytes after `size` in the last
  sector. This is a hard check for `ROCK_X6.BIN` (documented) and an
  observation for `ROCK_X6.DAT` (not known).

`rockarc` does not know which file it reads. `extract.py` passes the expected
member count (59 or 243) and decides which checks are hard.

### `index`

`index.json`, `schema_version: 1`:

```json
{
  "schema_version": 1,
  "medium": {"serial": "SLUS-01395", "revision": "v1.1", "bin_sha1": "d4f7…"},
  "files": [
    {"kind": "iso", "path": "iso/ROCK_X6.DAT", "lba": 211998, "sectors": 24860,
     "form": 1, "size": 50913280, "sha1": "…"},
    {"kind": "rock_bin", "path": "rock_bin/00.bin", "index": 0, "sector": 2,
     "size": 44588, "sha1": "…"},
    {"kind": "rock_dat", "path": "rock_dat/000.bin", "index": 0, "sector": 2,
     "size": 20480, "sha1": "…",
     "observed": {"tail_zero": true, "word0": 2, "word1": 20480,
                  "subentries": [[65555, 16384], "…"]}}
  ]
}
```

- `files` is sorted by `path`. Keys are sorted. The file ends with `\n`. Two
  runs on the same disc give byte-equal `index.json`.
- `observed` exists on `iso` records (`first_submode`) and on `rock_dat`
  records. `subentries` exists only when
  `0 < word0 < 64` and `word1 == size` (197 of 243 members in the probe). These
  values are raw observations for task C. Nothing in this task interprets them.
- `index.json` holds hashes and positions, never game bytes. It lives in
  `~/mmx-data/` with the outputs, not in the repo.

### `verify`

- `load_reference(path) -> dict[path, sha1]` reads `mmx6/manifest/retail.jsonl`
  records of kind `iso` and `rock`, and maps `rock/NN.bin` to `rock_bin/NN.bin`.
- `compare(index, reference) -> list[Problem]`: each reference path must exist
  in the index with the same SHA-1 and size. Extra index paths (all of
  `rock_dat/`) are listed as "no reference", not as errors.
- The reference is optional (`--reference`). Without it, the run prints that
  only structural checks ran.

### `extract` (CLI)

```
python3 -m mmxdemake.extract \
  --cue "~/mmx-data/disc/Mega Man X6 (USA) (v1.1).cue" \
  --out ~/mmx-data/extracted/retail \
  --reference ~/IdeaProjects/mmx-reference/mmx6/manifest/retail.jsonl
```

Order: identity check → ISO walk and extract → `ROCK_X6.BIN` split and checks
→ `ROCK_X6.DAT` split and checks → write `index.json` → verify. It prints one
summary line per stage, for example
`ROCK_X6.DAT: 243 members, TOC ok, 243/243 tail-zero (observation)`.

## Error handling

- A failed hard check stops the run with exit code 1 after the stage that
  found it. All problems of that stage print first, so one run shows the full
  list.
- A message names the item and the numbers: for example
  `rock_bin/17.bin: sha1 3a… != reference 9c…` or
  `ROCK_X6.DAT TOC: member 120 starts at sector 13661, expected 13660`.
- Outputs are written to `<out>.tmp/` and renamed to `<out>/` only when every
  hard check passes. A failed run never leaves a half-written output that looks
  complete. A rerun replaces the old output.
- Exit code 2 means bad arguments or a disc identity mismatch. This includes a
  CUE whose BIN does not exist, and an `--out` inside this repo (the tool
  refuses to write game data where `git add` could pick it up).
- Paths may start with `~`. The tool expands it, because a quoted `"~/…"` is not
  expanded by the shell.

## Testing

- Unit tests use only synthetic data from `tests/synth.py`: a small ISO with one
  Form 1 file, one Form 2 file and a subdirectory; archives with a correct TOC,
  a gap, an overlap, a missing terminator, a non-zero header tail and a short
  last member. Each bad case must give the expected `Problem`.
- `tests/test_retail.py` runs the whole CLI on the real disc only when the
  `MMX6_CUE` environment variable is set. It asserts: 9 ISO files, 59 + 243
  members, every reference hash matches (this includes the 2 `.STR` files that
  the probe missed), and a second run gives a byte-equal `index.json`.
- Command: `python3 -m unittest discover -s tests`.

## Done when

1. The unit tests pass.
2. `MMX6_CUE=… python3 -m unittest tests.test_retail` passes on the real disc.
3. `git status` in the repo shows no game data, and `index.json` is only under
   `~/mmx-data/`.

## Legal

- All code is written for this repo. `mmx6` is read for facts (its
  `formats.md`) and its manifest is used as a list of hashes. No code is copied.
- The repo `.gitignore` already blocks `*.BIN`, `*.DAT`, `data/`, `out/`.
  Tests do not write outside a temporary directory.
