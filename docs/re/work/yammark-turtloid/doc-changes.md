# Proposed changes to files this agent does not own (Yammark / Turtloid code items, 2026-10-22)

Probes: `~/mmx-data/x6/re/yammark-turtloid/p*.py` (run from that folder; `probe_ram.py` reads a
save state). All facts below are from the disassembly of the named states.

## `docs/engine.md`

### Game state table

- `+0x8D`[2] (`0x800CCF5D`): replace the row text with:
  "**soul rank** per character (X, Zero): the first i (0–7) with `game.souls[char]` (`+0xD2`) ≥
  `soul_rank_tbl[i]` (`0x8006D624` = {9999, 5000, 1200, 800, 500, 300, 200, 0}). Only the report
  screen writes it (overlay 20, `SoulRank` `0x800F22E8`: `game[0x8D + game.character]` = i; for i < 4
  it also makes an object at its x − 4 with `a1` = i − 124). Overlay 18 has the same function at
  offset `0x5CE8`. The new game (`0x8001E0C0`) writes 7 for both. Boss level =
  `boss_level_tbl[it]` = {3, 3, 2, 1, 0, 0, 0, 0}: 5000 souls → level 3, 1200 → 2, 800 → 1.
  [certain for the code; checked in all 44 states: X 28 souls → 7, 228 → 6, 312 → 5, 664 → 4; Zero
  0 → 7, 200 → 6, 400 → 5, each after a report screen]."
  The rank is computed only on the report screen, so the value can lag the souls (X has 312 souls
  but rank 6 in `turtloid-5_zero`; it is 5 after the next report).
- `+0x98`: drop "[likely]": "HP lost by the player in this stage: every player damage path adds the
  damage (`0x80031000`, `0x800311C4`, `0x800311D8`, `0x80039270`, `0x800392A4`, `0x80039350`,
  `0x800393DC`, `0x80039434`); an instant death adds the HP left (`0x80028304`). The report screen
  reads it (`0x800F1D2C`, overlay 20)."
- New row `+0x9C` (`0x800CCF6C`), 16-bit: "largest damage of one player hit on an enemy in this stage
  (`ApplyWeaponHit`, `0x800318DC`, after the cap of 127)".
- New row `+0x9E` (`0x800CCF6E`), 16-bit words: "bit sets per stage group (group =
  `0x8007A124`[stage × 2 + area]); `EventSeen(n)` (`0x800533F4`) returns bit n; read by stage logic
  14 [likely: the events already seen]". Note: `+0x9C` read as a 32-bit word gives `0x1000F` and
  similar values; the two halves are separate fields.
- `+0xAA`: open; 0 in the intro, Yammark and `turtloid-1`, 4 in all later states. No game-base
  access found in the main program or the 59 overlays.

### Object classes table

- Ray objects row: update table "scratchpad `+0x98`, 16 entries; loop `RayLoop` (`0x800207D4`;
  while `game+0x18` is set only flag bit `0x08`)"; made by `GetRayObject` and **record kind 6**.
  Draw: `RayDrawQuad` (`0x80023ED8`, a flat quad, semi-transparent with flag `0x10`, blend mode =
  flags bits 5–7) or `0x800241D4` (flag `0x02`). Types: 4 boss explosion rays, 5 darkness effect
  (overlay), 13 Nightmare Virus soul rays.
- Stage logic row: types 3 (`PaletteCycle`) and 11 (Turtloid rain) in the second lists.

### Spawning: second list

- Add: "Yammark (`0x800F66D0`, 21 records): 17 palette cycles (stage logic 3), 3 darkness-effect
  rays (kind 6, type 5) and the rain generator (main 37). So the Nightmare Effect objects of a stage
  are always in its second list; each deletes itself when `nightmare_effect` does not match."

### Checkpoints

- Add: "**Respawn points.** `CheckpointStart` (`0x80029B8C`) reads `checkpoint_start_tbl`
  (`0x80073300`, a word per stage × 2 + area, 0 = none) → [checkpoint] → a 36-byte record: player x,
  y; the cameras of layers 0–2 (x, y each); the camera limits min x, max x, min y, max y (current and
  target); the facing (`+0x18`); the layer 1 and 2 offsets `+0x40`/`+0x42` (`+0x1A`–`+0x20`); byte
  `+0x22` is the return value (1 in all records read, 8 in one stage-2 record) [certain for the
  code]. The lists: stage 1 area 0 `0x80073140` (4), stage 6 area 0 `0x800731B0` (6), stage 21
  area 1 `0x800732E8`."

### Camera: BG layer modes

- Add: "Layer 1 mode 2 and layer 2 mode 1 in Yammark; layer 1 mode 5 (`0x800297B4`, nothing) and
  layer 2 mode 4 (`0x8002998C`: x = layer 0 x / 2 + `+0x40`, y = layer 0 y + `+0x42`) in Turtloid.
  Turtloid layer 2 is the sky and sea (the black areas of `stage.png`); layer 1 is the rain curtain,
  which the rain controller moves."

### Nightmare Effect

- Value 8: replace "the overlay 03 object at `0x800EE154`" with "the darkness effect (ray type 5,
  overlays 03 and 09; `docs/enemies/darkness-effect.md`); `0x800EE154` is its state 0 in overlay
  03". Add: "value 6 (rain): stages 1 and 5 have rain zones; value 8 (darkness): stages 1 and 6
  have zones" (both match the pick table rows of stages 6 and 8).

### Palettes

- Add: "Scratchpad `+0x28` is the working palette buffer (32 bytes per palette; palette p is VRAM
  CLUT x = (p mod 16) × 16, y = 480 + p / 16), `+0x30` the source palettes. `PaletteSet(p, src, n)`
  (`0x8002B3D4`) copies n palettes and sets `bg_refresh` bit 0 (the upload [likely]).
  `PaletteDarken(dst, src, n, k, max)` (`0x8002B410`) writes each channel × (1 − k / max)."

## `docs/TODO.md`

- Object legend: tick "Name the Yammark section 1 objects (108) the same way" (2026-10-22,
  `docs/stages/yammark.md`).
- Object legend: in the `fixed_set_tbl` item, "Still open: the kind-6 records and the Yammark list"
  → done (kind 6 = ray class, the darkness effect; Yammark list read).
- Enemy RE: tick "What draws the Turtloid sea where the art layer is empty" (BG layer 2, x / 2
  parallax; `docs/stages/turtloid.md`, "The background layers").
- Enemy RE: "Not documented yet": the Rescue Reploid shot was already in the sheet (checked and
  completed, `0x800500E0` dispatcher); the ray class (`docs/enemies/nightmare-virus.md`, "The ray
  class"); the rain control (`docs/enemies/rain-effect.md`, "The other rain"); `0x800EE154` =
  `docs/enemies/darkness-effect.md`. Still open: the camera records (now listed for Yammark).
- Enemy RE: tick the Turtloid item "stage logic type 3, the kind-6 records, the counters, the
  respawn points" except `game+0xAA` (keep it open).
- Enemy RE: tick "What `game+0x8D` counts": the soul rank (above).
- Kit: per-overlay helper names: `0x800EE1D4` in overlay 03 is shown as `RideInit` (an overlay-06
  name) in `rekit --at 0x800EE154`. Add to the existing per-overlay item.
- New open item: `game+0xAA` writer; overlay 18 (has `SoulRank` at `+0x5CE8`): which screen.
- New open item: a save state with Nightmare Effect 8 (Yammark or Turtloid after a stage-8 pick) to
  check the darkness effect.

## `docs/stages/sheldon.md`, `docs/stages/shark.md`

- Stage logic 3 rows ("not read"): it is `PaletteCycle` (`docs/stages/turtloid.md`).
