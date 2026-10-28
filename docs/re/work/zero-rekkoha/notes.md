# Notes: Zero's Rekkoha (step 59) in `heatnix-3_rekkoha`

Date: 2026-10-23. State: `~/mmx-data/x6/dumps/heatnix-3_rekkoha.state.gz` (SHA-1
`f26dadb1e7dfcadf820275a583375f4aeb845a8b`). Earlier work: [`~/mmx-data/x6/re/zero/attacks/`](../zero/attacks/)
(code read from the Turtloid kit; the pseudo code of step 59 is in its `pseudocode.md`).
The Zero overlay code in this state is the same as in the kit state (`turtloid-6_zero-player`);
only the data at `0x801EF97A`–`0x801EF9B3` differs (`rekkoha_mask_save`, below) [certain].

## Scripts (run from this folder: `python3 <script>`)

| script | what |
|---|---|
| `scripts/zprobe.py` | loaders (copy of the Zero attacks one) |
| `scripts/live.py` | player fields and the live weapon objects |
| `scripts/effects.py` | the live effect objects |
| `scripts/objs.py` | the live main, stage and visual objects (what the beams hit) |
| `scripts/probe2.py` | camera, beam box, effect type table of stage 3, palette channel 1, mask |
| `scripts/effanims.py` | the animations of the shared table `0x8007798C` |
| `scripts/dmg44.py` | id 44 in every damage table named in `docs/engine.md` |
| `scripts/backdrop_probe.py` | the sprite directories `scratch+0x1C` / `+0x3C`; draws sprites 80–87 of `0x8013D86C` |
| `scripts/rekkoha_export.py` | writes `sprites-root/enemies/rekkoha-beam/` and `rekkoha-backdrop/` |
| `kits/at-0x8002AF6C.txt`, `kits/at-0x8004AE1C.txt` | `PaletteFxChannel` and effect type 17 (`rekit --at`) |

## What is live [certain]

- Stage 3 area 0, character 1 (Zero), `game+0x5F` = `0x11`, `game+0x60` = `mega+0xC9` = `0xFB`
  (all Mavericks except stage 3: Blaze Heatnix is not beaten).
- Zero at (656, 650), facing left, step 59.4, `act4` (`+0x07`) = 3, animation 104 (sprite 94,
  frame 13 of the loop), `+0x8E` = 1, `+0x93` = 5, `+0xE0` = 2, `+0xE1` = 20, `+0xE4` = 1,
  `no_hit` = 1, `game.player_lock` = 1, HP 26. Energy slot 5 = 216 = 288 − 72: one use paid.
- Camera layer 0 at (496, 480). Zero is at view x 160 (the middle).
- Weapon slots 0–2: id 44, flag `0x21`, animation 11 of `0x8007798C`, sprite data
  `0x801474A4` (= `scratch+0x1C` directory entry 15), CLUT `0x7802`, priority 17, box
  `0x801EE3DC`. Beam 0 at (656, 504), beams 1 and 2 at (624, 424) and (688, 424).
- Effect slots 12, 17, 18: type 26 (`0x801EE254` in this stage's effect table `0x800F3914`),
  animation 12 of the same table and data, at the beam x and 160 px above each beam.
- Effect slots 2–11: type 17 (`0x8004AE1C`), `var` 0, `+5` = 59, sprites 83 (x 128) and 84 (x 384),
  y 32, 96, 160, 224, 288 (screen coordinates), data `0x8013D86C` (`scratch+0x3C` entry 0),
  texture `0x1E00`, CLUT `0x7E50`.
- Effect type 3 (`shot_anime_tbl` `0x8007789C`, data `0x801423CC`): var 0 at (656, 564) and two
  var 7 sparks near (692, 519); effect type 4 explosions.
- Main objects: Nightmare Snake (main type 25): body (var 5, `dmg_tbl_deflect`, `+0x65` = 3,
  `+0x63` = `0xFF`) and 4 gems (`dmg_tbl_nightmare_snake`); gem var 1 at (684, 530) has HP 16 of
  32 and `+0x65` = 2.
- Palette channel 1: `fade_mode[1]` = 1, `fade_r[1]` = 31, `fade_g[1]` = `0x3E0`, `fade_b[1]` =
  `0x7C00`, `fade_mask[1]` = the stage BG mask (= the copy at `0x801EF974`), `bg_refresh` = 8. BG
  CLUT 100 is all 0 in `palette_fx_buf` and in VRAM (the raw palette has colours) [certain].

## Checks of the code against the live values [certain]

- **Wave count.** Waves at `+0xE1` = 1 (k = 0: 1 beam) and 11 (k = 1: 2 beams) → `act4` = 3. The
  next wave is at `+0xE1` = 21. Zero in the middle of the view: 6 waves (k = 0…5, ±160 px still on
  view) give 11 beams.
- **Positions.** Beam y = cam y − 128 + 8 × moves: beam 0: 480 − 128 + 8 × 19 = 504; beams 1, 2:
  + 8 × 9 = 424. So the set-up (state 0) of a beam runs in the same update as the spawn, and the
  first move is in the next update. The top part is 160 px higher (cam y − 288 vs − 128).
- **Hit tag.** `+0x64` starts at the flag of frame 0 of animation 11 (1) and + 1 when
  `frame_count & 7` = 0: beam 0 (19 updates) = 3, beams 1, 2 (9 updates) = 2, with `frame_count`
  = 3885291.
- **Hit count.** `+0x8C`: beam 0 = 3 (three deflects on the snake body: tags 1, 2, 3), beam 2 = 2
  (two hits of 8 on gem var 1: 32 → 16), beam 1 = 0. **A deflect uses one of the 4 hits too**:
  `shot_hit_tbl` row 44 is {spark 7, sound kind 1, end on hit 1, go on 0, sound 12}, so a deflect
  sets `+0x98` = −1 and a hit `+0x98` = 1; the beam code counts any `+0x98` ≠ 0 and does not end.

## Timeline (Zero in the middle of the view) [certain for the code; update numbers ±1, likely]

Update 0 = the giga press (`TryRekkoha` runs step 59 once in the same update).

| update | what |
|---|---|
| 0 | animation 104, voice 7, pay 72, lock, `no_hit` = 1, no terrain box; `PaletteFxReset`, scale mode, `+0xE4` \|= 1 |
| 1–15 | `fade_r[1]` + 2 per update (2…30): the BG goes dark |
| 16 | `fade_r[1]` = 31, `fade_mask[1]` saved and set to all 512 CLUTs, add mode (+31 on each component): **the whole screen white, sprites too** |
| 18 | mask written back, scale mode with 31: **BG CLUTs transparent black**; 10 backdrop objects (effect type 17) |
| ≈23 | animation 104 frame 5 (flag `0x80`): sound (1, 27) |
| ≈27 | frame 9 (flag `0x40`): sound (1, 28); waves start |
| ≈27, 37, 47, 57, 67, 77 | waves k = 0…5 (1 + 2 + 2 + 2 + 2 + 2 = 11 beams) |
| ≈78 | `act4` ≥ 11: hold 71 updates |
| ≈149 | animation 111; `fade_r[1]` − 8 per update (31, 23, 15, 7 → 0); the backdrop objects delete themselves (`fade_r[1]` ≠ 31); at 0: `+0xE4` &= ~1, `bg_refresh` \|= 1 |
| ≈180 | animation 111 at its hold frame (31 updates) and fade 0: `+0x93` = 0, `+0x8E` = 0, `+0xE0` = 0; when `+0xD3` = 0: `player_lock` = 0, `no_hit` = 0; terrain box |
| ≈181 | `StandOrFall` |

With Zero near a view edge, the beams on one side are off the view and not counted, so more
waves come (up to 10 waves at the edge) [certain for the code].

## The objects [certain for the code]

**Beam, weapon id 44 (`RekkohaBeam` `0x801EDFC8`).**
- Spawn (`RekkohaWave` `0x801EAA24`): flag `0x21`, x = Zero x (k = 0) or Zero x ∓ 32k, Zero's layer.
- State 0: x outside [cam x − 16, cam x + 336] → delete (not counted). `var` 0: `mega.act4` + 1;
  y = cam y − 128; animation 11; box (−16, −128, 32 × 255). (`var` ≠ 0: y = cam y − 288,
  animation 12, no box: the same as the top part; no spawner sets `var` ≠ 0 [likely: a leftover].)
  `speed_y` = −8.0.
- State 1: tick, move 8 px down per update; hit tag + 1 every 8 frames (the global frame counter,
  so all beams change tag in the same frame); `+0x98` ≠ 0 → `+0x8C` + 1; at 4: no box; delete at
  y ≥ cam y + 368 (62 updates after the spawn).
- One target in contact: at most one hit per 8 updates per beam, at most 4 per beam.
- Damage (id 44): `dmg_tbl_normal` 8, `nightmare_insect` 8, `nightmare_snake` 8, `yammark` 8,
  `sabre_only` 8, `reflector` 8, `minor` 4, `d1000` 2, `one_hit` 127; deflected by `deflect`,
  `illumina`, `nightmare_bug`, `sheldon`, `sheldon_guard`, `shark`, `wolfang`, `mijinion`,
  `ice_block`, `grey_block`; no effect on `reflector_wait`, `nightmare_shadow`, `capsule`.
- Hit sound: (1, 9), kill (1, 10); deflect (0, 6); spark variant 7 (`docs/damage.md`).

**Top part, effect type 26 (`RekkohaBeamTop` `0x801EE254`).** Made with each beam, at the same x;
the same view test; animation 12; y = cam y − 288; 8 px down per update; delete at cam y + 400.
No box. It is not a "light": it is the pointed top of the beam, 160 px above the beam origin.

**Backdrop, effect type 17 (`GigaBackdrop` `0x8004AE1C`, main program).** `RekkohaLights`
(`0x801EA888`) makes 5 rows × 2: x 128 (sprite 83, 256 × 64) and x 384 (sprite 84, 64 × 64; its
origin is at its right side + 64, so it fills view x 256–319), y = 64 × row (1…5). Sprites 85/86 when
`mega+0x02` = 0 (X; dead code in Zero's overlay). The object: screen layer, priority 55, `var` 0 →
y − 8 per update, wraps from y ≤ 0 to 320, hidden at y ≥ 304. It deletes itself when Zero's step
is not its `+5` (59) or `fade_r[1]` ≠ 31. So **a red and yellow streak curtain fills the view and
scrolls up at 8 px per update** while the beams fall at 8 px per update. The live y (32…288) is the
start (64…320) after the scroll and wrap. CLUT 416 (`0x7E50`) is in the BG mask, but the raw
palette list holds 0 there and VRAM holds the colours: the palette effect does not change it
[certain for the state; who uploads it is open].

## The screen effect, details [certain for the code]

- `PaletteFxChannel` (`0x8002AF6C`) reads, for channel 1, only `fade_mode[1]` and `fade_r[1]` in
  scale mode, and `fade_r/g/b[1]` in add mode. The Rekkoha also writes 1 to `fade_sub_g[1]`
  (`0x8009742D`) and `fade_sub_b[1]` (`0x80090D57`) and adds 64 / 2048 per update to `fade_g[1]` /
  `fade_b[1]`; only the add mode (the flash) uses `g` and `b`.
- Scale mode with k = 31 writes 0 to all 16 colours of each masked CLUT (transparent). Add mode
  with 31 on all three components writes `0x7FFF` (white).
- The white flash takes all 512 CLUTs (mask all ones) for 2 updates; the dark phase takes only the
  stage BG mask (CLUTs 96–479 by default, `PaletteFxReset`).

## Sprites and exports

- `playerexport --root sprites-root`: `players/zero/` is **byte-identical** to the shared
  `~/mmx-data/x6/sprites/players/zero/` (all PNGs and `animations.json` without the sources).
  Animations 104 (47 updates, loop from frame 13) and 111 (127 updates, hold at 31) are there and
  complete. Nothing new for Zero.
- `spriteexport --root sprites-root`: 9 exports. 7 have the IDs of the shared exports. Two new IDs,
  both **not to copy**:
  - `enemy-d2c1ab74d7`: effect type 3 with `shot_anime_tbl` (`0x8007789C`): all 40 animations, the
    lemon problem of `docs/TODO.md` (only animations 0 and 13 are the live effects).
  - `enemy-2fbbe8998b`: main type 36 (search, bank 3, table `0x800F3CBC`): every sprite is blank
    (cell 1 × 1). Not the Rekkoha.
- **Skipped by the tool:** weapon type 44 and effect type 26: "table `0x8007798c`: animation 0
  uses sprite 8, bank -1 has 6". The tool keeps the leading animations that fit and stops at the
  first that does not; animation 0 of this table already needs sprite 8, so it keeps none. Of the
  first 60 animations of `0x8007798C`, **only 11 (beam) and 12 (top part) are the Rekkoha's**: the
  beam code sets 11 (`var` 0) or 12, and the top part sets 12 [certain for the code]. Other short
  ones (2, 13, 21, 22, 29, 31, 36–39) would fit 6 sprites but belong to other objects.
- **Not reached by the tool:** the backdrop (effect type 17, sprite data in the `scratch+0x3C`
  directory, no animation table).
- **New kit exports** (`scripts/rekkoha_export.py`, the tool's draw and ID functions; checked by eye):
  - `sprites-root/enemies/rekkoha-beam/` (`enemy-56bb20f5e7`): `anim-0.png` = table animation 11
    (sprites 0, 1, 2: orange, blue, cyan beam, 1 update each, loop), `anim-1.png` = table animation
    12 (sprites 3, 4, 5: the top part). Cell 32 × 255 = the hit box. Mode `0x20`, CLUT `0x7802`.
  - `sprites-root/enemies/rekkoha-backdrop/` (`enemy-c61736d6e0`): `sprites.png` (sprites 83, 84)
    and `screen.png` (the 2 × 5 grid as it is made, 320 × 320).
  - The `animations.json` files keep the game's table indexes in the source
    (`table_animations`: [11, 12]); `index` is the position, as the tool writes it.
  - The bank sheets `banks/heatnix/heatnix-3_rekkoha/weapon-44-live-0.png` and
    `effect-26-live-0.png` (6 sprites each) are the same sprites.

## Answers to the open points of `zero/attacks/notes.md`

- "A state in step 48, 55 or 59 would test them": step 59 is tested; the code values agree with
  the state (waves, positions, tags, hit count, energy).
- New: a weapon object made in the player step runs its set-up in the same update (beam y).

## Still open

- Who uploads CLUT 416 (the backdrop palette) and why the dark mode does not clear it.
- `mega+0xE0` = 2 during step 59 (set by `TryRekkoha`, cleared at the end): no reader read.
- `mega+0xD3` (keeps the lock at the end when not 0): not read.
- Whether effect types 17 and 26 are the same in every stage's effect table (only stage 3 area 0
  is in this state).
