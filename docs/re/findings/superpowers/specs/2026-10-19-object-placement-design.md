# Object placements (task G) — design

Date: 2026-10-19. Status: draft for review.

## Purpose

Read the list of objects (enemies, items, gates, triggers) that the game
places in a stage, record where that list is on the disc, and draw each object
on the stage picture with a number. The user then names what each number is,
in the same way as the collision check.

## Decisions already made

- The list is read from the disc, not from RAM. RAM is used only to find where
  the list starts (the game can change its RAM copy during play).
- Naming each `kind` and `id` is not part of this task. It goes into
  `docs/TODO.md` as an object legend.
- The other pointers next to the list pointer (`0x800734a4`, `0x800734ac`, …)
  are not part of this task.

## Facts this design depends on

| Fact | Source | Status |
|---|---|---|
| RAM `0x8007349c` (u32) holds a pointer to the object list of the running stage | intro state: `0x800fa214` | observed on 1 state; assumed fixed |
| Intro: the list is in overlay 02 at offset `0x109b4` (overlay 02 loads at `0x800E9860`); the RAM bytes are equal to the disc bytes | intro state | observed on 1 state |
| A record is 8 bytes: `a` (u8), `b` (u8), `id` (u8), `kind` (u8), `x` (u16 LE), `y` (u16 LE) | intro list | observed on 1 stage |
| The list ends with a record whose `kind` is `0x0f` (intro: `00 00 00 0f 00 00 00 00`) | intro list | observed on 1 stage |
| Intro: 65 records before the end record; `kind` 0–5; `x` 0–6162 (stage width 6656 px); `y` 0–1112 (stage height 1536 px); 7 records at (0, 0) | intro list | observed on 1 stage |
| `x` and `y` are stage pixels from the top-left corner | values are inside the stage size | assumed; the user checks it on `objects.png` |

## Scope

In scope:

1. `objects` (new unit): read the list from RAM, find it on the disc, decode
   records.
2. `learn`: add the list location to each catalog entry.
3. `stages`: write `objects.json` and `objects.png`, and add an object table
   to the stage's `summary.txt`.
4. Run `learn` again on `intro-1_start.state.gz` to update the catalog entry.

Out of scope: names for kinds and ids, the other pointer lists, the meaning of
`a` and `b`, the Yammark stage.

## Repository layout (additions)

```
mmxdemake/
  objects.py        # object list: RAM pointer, disc search, decode
  digits.py         # 3 x 5 pixel font for the digits 0-9
tests/
  test_objects.py
  test_digits.py
```

Changed: `learn.py`, `stages.py`, `tests/test_learn.py`, `tests/test_stages.py`,
`tests/test_retail_stages.py`, `tests/stagefix.py` (fixtures), `README.md`.

Imports: `objects` uses `savestate` (`ram_at`). `digits` imports no project
module. `learn` adds `objects`. `stages` adds `objects` and `digits`.

## Units

### `objects`

- Constants: `LIST_POINTER = 0x8007349C`, `RECORD = 8`, `END_KIND = 0x0F`,
  `MAX_RECORDS = 1024`, `SCREEN_PX = 256`.
- `ObjectError(Exception)`.
- `read_list(ram) -> bytes`: read the u32 at `LIST_POINTER`. Refuse when it
  is not a RAM address (`0x80000000 <= p < 0x80200000`) or is not a multiple
  of 2. Read 8-byte records from `p` until a record with `kind == END_KIND`.
  The result includes the end record. Refuse when there is no end record in
  the first `MAX_RECORDS` records or before the end of RAM.
- `find_list(data, overlays: dict[int, bytes]) -> (overlay, offset)`: search
  every overlay for `data`. Exactly one hit must exist; else refuse with the
  number of hits.
- `decode(data, w, h) -> list[dict]`: one dict per record before the end
  record: `{"index", "a", "b", "id", "kind", "x", "y"}`, `index` from 0.
  Refuse when there is no end record, when the length is not a multiple of 8,
  or when a record has `x >= w × 256` or `y >= h × 256`. The message names the
  record index.

### `digits`

- `GLYPHS`: for each digit 0–9, 5 rows of 3 bits.
- `text_mask(s: str) -> list[list[int]]`: a 5-row mask of the digits in `s`,
  3 columns per digit and 1 empty column between digits. Refuse a character
  that is not a digit (`ValueError`).

### `learn` (changes)

- After the layout is found: `data = objects.read_list(ram)`,
  `(ov, off) = objects.find_list(data, overlays)`,
  `records = objects.decode(data, w, h)`.
- The entry gets `"objects": {"overlay": ov, "offset": off, "count": n}`.
- An `ObjectError` gives exit 1 with one `error:` line. Nothing is written.
- The printed line ends with `, <n> objects @<off:#x>`.

### `stages` (changes)

- `objects` is optional in a catalog entry. When it is present, `_validate`
  checks that it has the integer keys `overlay`, `offset` and `count`; else
  exit 2. An entry without `objects` gets no object files and the summary line
  says `objects not learned`.
- When it is present: read the overlay from `offset` up to and including the
  end record, decode it, and refuse (exit 1) when the record count is not
  `count`.
- New files in each stage folder:
  - `objects.json`: `{"schema_version": 1, "name", "member",
    "source": {"overlay", "offset"}, "objects": [records]}`, sorted keys.
  - `objects.png`: the same size as `stage.png`. It is `stage.png` with each
    channel halved. For each record there is a marker: a 9 × 9 px square
    centred on (`x`, `y`), filled with `KIND_COLOURS[kind]`, with a 1 px black
    border. The record's `index` is printed in white digits at scale 2
    (6 × 10 px for each digit), with a 1 px black outline, to the right of the
    marker and 2 px away from it. The number moves to the left side when it
    would go past the right edge. Pixels outside the image are cut.
    `KIND_COLOURS` is a fixed table of 16 bright colours, one for each kind.
  - `summary.txt`: the existing line, then `objects: <n>`, then one row for
    each record with these columns: `index kind id b a x y screen`.
    `screen` is `x // 256,y // 256`.
- The top `summary.txt` line for each entry ends with `, <n> objects`.

## Error handling

The rules are the same as in task F:

- Exit 0: done.
- Exit 1: a check failed (`ObjectError`, or a count that does not agree).
- Exit 2: bad arguments or a malformed catalog (including a malformed
  `objects` field).

## Testing

- `test_objects`:
  - `read_list`: a fake RAM with a pointer and three records plus an end
    record gives 32 bytes. Each of these refuses:
    - a pointer of 0;
    - a pointer above RAM;
    - an odd pointer;
    - no end record within `MAX_RECORDS`;
    - a list that runs to the end of RAM.
  - `find_list`: one hit gives (overlay, offset); zero hits and two hits
    refuse.
  - `decode`: the field order and little-endian `x` and `y`; the end record
    stops decoding; a record at (0, 0) is accepted; `x == w × 256` refuses;
    a missing end record refuses; a length that is not a multiple of 8
    refuses.
- `test_digits`: the mask of `"10"` is 5 × 7 and has the expected bits;
  `"a"` refuses.
- `test_learn`: the fake RAM gets a list pointer and a list that is also in
  the fake overlay. The entry has
  `objects == {"overlay", "offset", "count"}` with the expected values. A
  list that is not on the disc gives exit 1 and writes no catalog.
- `test_stages`:
  - An entry with `objects` writes `objects.json` with the records.
  - In `objects.png`, the centre pixel of a marker has the kind colour and
    a pixel away from all markers is half of the `stage.png` pixel.
  - `summary.txt` has the object rows.
  - A catalog `count` that does not agree gives exit 1.
  - An entry without `objects` writes no object files.
  - A malformed `objects` field gives exit 2.
- `test_retail_stages`: `learn` on the intro gives
  `objects == {"overlay": 2, "offset": 0x109b4, "count": 65}`. Then
  `stages` writes `objects.json`. Its first record is
  `{"a": 0, "b": 6, "id": 1, "kind": 3, "x": 0x840, "y": 0x440}`, and
  `objects.png` exists.

## Done when

1. The unit suite passes, and the real-data test passes.
2. `learn` runs again on `intro-1_start.state.gz` under the name `Intro`, and
   `catalog/stages.json` gets the `objects` field (committed: positions only).
3. `stages` writes `~/mmx-data/stages/m094-intro/objects.png`.
4. The user looks at it and confirms that the markers are on objects in the
   stage. This also checks the assumption about `x` and `y`.
5. `docs/TODO.md` has an object legend section for the user's names.
6. `git status` shows no game data.

## Legal

- The repo keeps no object bytes. The catalog holds only the overlay, the
  offset and the count.
