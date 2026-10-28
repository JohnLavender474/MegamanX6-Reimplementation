# Palettes from save states (task H) — design

Date: 2026-10-19. Status: draft for review.

## Purpose

Draw the stage art in its real colours. The palettes come from the copy of
video memory (VRAM) in the stage's save state. `learn` finds and checks the
rule that turns a tile's palette number into a VRAM position, and `stages`
draws `stage.png` and the art layers of `stage.ora` in colour. Colour `0x0000`
is transparent.

## Decisions already made

- `stages` reads VRAM from the save state each time (option A). It does not
  keep a second copy. The catalog already names the save state and its SHA-1.
- `learn` learns the palette rule from the game's draw commands. It does not
  assume a fixed rule. It refuses when the evidence is weak.
- An entry without `palettes` is still drawn in greyscale.
- Out of scope:
  - palettes that change during play (the colours are those at the save point);
  - semi-transparency (bit 15 of a colour; drawn opaque);
  - sprites for X, enemies and the HUD;
  - the Yammark stage.

## Facts this design depends on

| Fact | Source | Status |
|---|---|---|
| VRAM (1024 × 512 × 16 bit = `0x100000` bytes, 2048 bytes per row) starts at decompressed save state offset `0x29b749` | intro state; the only page-aligned offset with a valid header | observed on 1 state |
| The u32 at VRAM offset − `0x408` is 1 (the graphics chip's state version), followed by the status word and 256 control words | intro state | observed on 1 state |
| The stage's texture block is in VRAM unchanged: 4 bpp page p, row r (128 bytes) is at VRAM (x0 + 64p, y0 + r); intro: (x0, y0) = (320, 256); all 8 × 256 rows match, at no other page-aligned position | intro state | verified on 1 state |
| RAM holds 16 × 16 sprite commands: word 0 = `cmd << 24 \| colour` with cmd `0x7C`–`0x7F`; word 1 = `y << 16 \| x` (signed 16 bit, screen pixels); word 2 = `clut << 16 \| v << 8 \| u` | intro state: 1,264 with u, v multiples of 16 and a sensible screen position | observed on 1 state |
| `clut` field: VRAM x = (`clut` & 63) × 16, y = `clut` >> 6 | PS1 hardware | fact |
| Camera vote (stage tile position − screen position, over tiles with the same texture cell): layer 0 (0, 1200) 294 votes, next 48; layer 1 (0, 1288) 192, next 108; layer 2 (0, 1132) 588, next 60 | intro state | observed on 1 state |
| Palette n is at VRAM x = `clut_x` + (n mod 16) × 16, y = `clut_y` + n ÷ 16; intro: (`clut_x`, `clut_y`) = (0, 486); 1,074 matched sprites over 20 palette numbers, no exceptions | intro state | observed on 1 state |
| 8 bpp tiles use the same position and read 256 colours | intro: 11 of the 20 palettes are 8 bpp; colour pictures look right | observed on 1 state |
| Colour conversion: 5 → 8 bit as `(v << 3) \| (v >> 2)`; `0x0000` is transparent (`psxcolor.to_rgba`) | task C | verified |

## Scope

In scope:

1. `vram` (new): read and check VRAM in a save state; find the texture block in
   VRAM.
2. `palettes` (new):
   - read the sprite commands;
   - vote for the camera of each layer;
   - learn (`clut_x`, `clut_y`);
   - read one palette's colours.
3. `learn`: add the `palettes` field.
4. `stages`: colour drawing when `palettes` is present; read the save state.
5. Run `learn` again on the intro and redraw.

## Repository layout (additions)

```
mmxdemake/
  vram.py           # VRAM from a save state; texture block position
  palettes.py       # sprite commands, camera vote, palette rule, palette colours
tests/
  test_vram.py
  test_palettes.py
```

Changed:
- code: `savestate.py` (one new function), `learn.py`, `stages.py`;
- tests: `tests/synth.py` (a save state with VRAM), `tests/stagefix.py`
  (sprites, palettes, distinct texture cells), `tests/test_learn.py`,
  `tests/test_stages.py`, `tests/test_retail_stages.py`;
- docs: `README.md`, `docs/TODO.md`.

Imports:
- `vram` imports no project module except `savestate`.
- `palettes` uses `stagemap`, `texture` and `psxcolor`.
- `learn` adds `vram` and `palettes`.
- `stages` adds `vram`, `palettes` and `savestate`.

## Units

### `savestate` (addition)

- `read_state(path) -> bytes`: the decompressed save state. It has the same
  checks as `read_ram` (gzip and header). `read_ram` uses it.

### `vram`

- Constants: `VRAM_OFFSET = 0x29B749`, `VRAM_SIZE = 0x100000`,
  `VRAM_W = 1024`, `VRAM_H = 512`, `ROW = 2048`, `GPU_HEADER = 0x408`,
  `GPU_VERSION = 1`.
- `VramError(Exception)`.
- `read_vram(state: bytes) -> bytes`: refuse when the data is too short, or
  when the u32 at `VRAM_OFFSET − GPU_HEADER` is not 1.
- `find_textures(vram, block) -> (x0, y0)`: try every page-aligned position
  (x0 a multiple of 64 with x0 + 512 ≤ 1024; y0 in {0, 256}). A position
  matches when all 8 × 256 rows of the 8 4 bpp pages are equal. Exactly one
  position must match; else refuse with the number of matches and the best
  candidate's count of differing rows.

### `palettes`

- Constants: `SPRITE_CMDS = range(0x7C, 0x80)`, `MIN_VOTES = 50`,
  `VOTE_MARGIN = 1.5`, `MIN_MATCHES = 50`, `MIN_AGREE = 0.95`.
- `PaletteError(Exception)`.
- `sprites(ram) -> list[(sx, sy, cu, cv, clut_x, clut_y)]`: every aligned word
  with a command in `SPRITE_CMDS` and these conditions:
  - u and v are multiples of 16 (`cu = u // 16`, `cv = v // 16`);
  - −16 < sx < 512 and −16 < sy < 512;
  - the `clut` position is inside VRAM.
- `cameras(sprites, layers, w, h, screens, info) -> list[(x, y) | None]`: one
  entry per layout layer.
  - Each sprite and each tile of that layer with the same texture cell
    (`tile_ref.cx`, `cy`) gives one vote for (tile x − sx, tile y − sy).
  - The winner needs at least `MIN_VOTES` votes and more than `VOTE_MARGIN` ×
    the runner-up's votes; else that layer's entry is `None`.
  - Layer 0 must have a camera; else refuse.
- `learn_rule(sprites, cameras, layers, w, h, screens, info) -> dict`:
  - Each sprite is matched to the first layer (0, 1, 2) with a camera whose
    tile at (camera + screen position) has the same texture cell.
  - Each match with palette number n gives a candidate (`clut_x` − (n mod 16)
    × 16, `clut_y` − n ÷ 16).
  - Refuse when there are fewer than `MIN_MATCHES` matches, or when the most
    common candidate has a share below `MIN_AGREE`.
  - Refuse when the rule puts any palette number used by the stage outside VRAM.
    The checks are: `clut_y` + max(n) ÷ 16 < 512, and `clut_x` + 15 × 16 + width
    ≤ 1024, where width is 16 for 4 bpp and 256 for 8 bpp.
  - Return `{"clut_x", "clut_y", "matches", "agree", "cameras"}`. `agree` is
    rounded to 3 decimals. `cameras` is a list with `[x, y]` or `null` for each
    layer.
- `palette(vram, rule, n, count) -> list[(r, g, b, a)]`: `count` colours (16 or
  256) from the position of palette n, through `psxcolor.to_rgba`.

### `learn` (changes)

- Read the state once (`read_state`), then RAM and VRAM from it.
- After the layout and the objects:
  - `find_textures(vram, texture block)` (the result is only a check);
  - `sprites`, `cameras`, `learn_rule`.
- The entry gets `"palettes": {...}` as above.
- A `VramError` or `PaletteError` gives exit 1 with one `error:` line.
  Nothing is written.
- The printed line ends with
  `, palettes @(<clut_x>, <clut_y>) from <matches> tiles`.

### `stages` (changes)

- New option `--dumps` (default `~/mmx-data/dumps`).
- An entry with `palettes`:
  - `_validate` checks that `palettes` has integer `clut_x` and `clut_y`, and
    that the entry has `save_state` and `save_state_sha1` strings; else exit 2.
  - `stages` reads the save state, checks its SHA-1 against the catalog
    (refuse, exit 1, when it differs or the file is missing), and reads VRAM.
  - Tiles are drawn in colour. 4 bpp tiles use 16 colours and 8 bpp tiles use
    256, from `palette(vram, rule, tile palette, count)`.
  - A colour with alpha 0 is not drawn, so the layer below shows through in
    `stage.png` and the pixel is transparent in the art layer.
- An entry without `palettes` is drawn in greyscale as today. The top
  summary line ends with `, palettes not learned`; with palettes it ends with
  `, colour`.
- `stage.ora`, the art layer names and the merged image are unchanged in
  structure.

## Error handling

The exit codes are the same as in tasks F and G:

- **0**: done.
- **1**: a failed check, which includes:
  - `VramError` or `PaletteError`;
  - a missing save state, or one whose SHA-1 differs, during `stages`.
- **2**: bad arguments, or a malformed catalog.

## Testing

- `test_vram`:
  - A synthetic state (header, RAM, padding, GPU header, VRAM) reads back.
  - A wrong version word refuses, and short data refuses.
  - `find_textures` finds a block placed at (320, 256), refuses when the block
    is in VRAM twice, and refuses when one row differs (the message gives the
    count).
- `test_palettes`:
  - `sprites` parses a command and skips commands that are not aligned or not
    on screen.
  - `cameras` finds a known camera on a synthetic stage with distinct texture
    cells; a layer with too few votes gives `None`; no camera for layer 0
    refuses.
  - `learn_rule` finds (0, 486) from synthetic sprites. A 10 % share of
    sprites with a wrong palette position refuses (agreement 0.90), and too few
    matches refuse.
  - `palette` reads 16 colours, with `0x0000` → alpha 0.
- `test_learn`:
  - The fake state gets VRAM with the texture block, palettes and sprite
    commands, and the entry gets the expected `palettes` field.
  - A state whose VRAM lacks the textures gives exit 1 and writes no catalog.
- `test_stages`:
  - An entry with `palettes` draws a known colour at a known pixel, and its
    art layer has a transparent pixel where the palette colour is `0x0000`.
  - An entry without `palettes` is still greyscale.
  - A save state with a different SHA-1 gives exit 1.
  - A malformed `palettes` field gives exit 2.
- `test_retail_stages`:
  - `learn` on the intro gives `clut_x` 0 and `clut_y` 486, with at least
    1,000 matches, agreement 1.0, and a layer 0 camera of (0, 1200).
  - `stages` exits 0.

## Done when

1. The unit suite passes, and the real-data test passes.
2. `learn` runs again on the intro, with the note kept. `catalog/stages.json`
   gets `palettes` (positions and counts only).
3. `stages` draws the intro in colour, and the user confirms that it looks like
   the game.
4. The README describes `--dumps`, the colour drawing, and what is not
   covered (palettes that change during play, semi-transparency).
5. `git status` shows no game data.

## Legal

- The repo keeps no VRAM, palette or texture bytes. The catalog holds only
  positions, counts and cameras.
