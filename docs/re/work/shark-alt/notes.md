# Notes: `shark-10_alt` (stage 20 area 1, Metal Shark Player's hidden area)

User's words: "See in slot 10 that a new section of Metal Shark's stage is present".

## Result in short

- First save state of stage 20 area 1 (overlay 28, stage member 118). Until now the area was known
  only from the code (`docs/stages/shark.md`, `docs/enemies/nightmare-press.md`,
  `docs/players/ride-armor.md`) [certain].
- The "large red enemy" is **the Ride Armor** (`actor2` type 1, empty, at (32, 352) on the start
  ledge). Same sprites as the stage 4 ride (export `ride-armor`, `enemy-e32b3dd423`) [certain].
- The "small item at the bottom right" of the preview is **the talk "!" icon** of the HUD: the
  talk zone (record 1, box 4: x 0–224, y 240–416) holds X, `mega.hint` = 4. The RAM primitive list
  of the HUD has a 16 × 16 sprite at screen (276, 196), CLUT `0x7805`, `uv` (128, 208); drawn from
  VRAM page 9 with that CLUT it is the "!" box [likely: the texture page of the sprite was found by
  trial, not from the draw-mode command].
- No new sprite ID. `spriteexport` gives the same 5 IDs as the stage 4 states; all bank sheets are
  byte-equal to `shark-1_ride`'s, and `actor2-1-live-0.png` has the same pixels as
  `actor2-0-live-0.png` of `shark-1_ride` [certain].
- `learn` fails on the shared code: the fix is a one-line change in `mmxdemake/ramlayout.py`
  (below). With it, `learn` gives member 118 and all 18 other non-texture entries of the catalog
  come out the same [certain].
- Two new object codes: kind-7 type 4 (forced scroll, `0x800E9864`) and stage logic 15 (the belt,
  `0x800F0984`), both variants of the stage 4 area 1 code; documented in `area-page.md` and as a
  new section of `docs/enemies/conveyor-belt.md` (`doc-changes.md`).

## Method and files

- Live objects: `scripts/probe_live.py shark-10_alt` (copy of `re/dynamo/scripts/probe.py` with
  the kind-7 and `actor2` pools added).
- Tables: `scripts/tables.py` (checkpoints, camera zones, talk box, text members, kind-7 fields).
- Learn: `scripts/probe_learn.py` (why it fails), `scripts/where_member.py` (where each entry of
  members 98 and 118 is in RAM), `scripts/info_diff.py` (tile info of 118 against RAM; edge scores),
  `scripts/scan_textures.py` (which member's texture block is in VRAM),
  `scripts/learn_patched.py` (learn with the widened mask; run with `--catalog` on a copy).
- Map: `stages/m118-metal-shark-player-hidden-area/` and `tmx/m118-metal-shark-player-hidden-area/`
  (drawn with `catalog-one/`: a catalog copy that has only the new entry);
  `scripts/ascii_map.py`, `scripts/marked_map.py`.
- Code: kits in `kit-kind7-4/`, `kit-logic-15/`, `kit-kind7-2/` and `kit/` (overlay 28 against
  overlays 06 and 07); `scripts/cmpkit.py` compares two `code.txt` files after masking jumps,
  branches and `lui`; `scripts/own_types.py` lists the type-table entries in the overlay;
  `scripts/scan_ov.py` finds loads of a global in an overlay.
- Text: `mmxdemake.text --member 233` / `234` into the scratch folder.
- Sprites: `sprites-root/` (copy of `~/mmx-data/x6/sprites/` + the run of `shark-10_alt` with
  `catalog-copy/enemies.json`, which has `"20.1": "shark-alt"`).

## Why `learn` fails, and the fix

`python3 -m mmxdemake.learn --state shark-10_alt.state.gz --name "Metal Shark Player, hidden area"`
(on a catalog copy):

```
Metal Shark Player, hidden area: the screens join less than the limit (edges 0.900/0.000), but the layout is the disc layout of overlay 28 @0x8b28; kept
error: texture block found at 0 VRAM positions; need exactly 1 (closest: (0, 0) with 2016 differing rows)
```

1. `find_member` takes the members whose tile info (entry type 1) is in RAM. Only member 98
   (stage 4 area 0) is found: a stale copy of its 8 000-byte tile info is still at `0x801AFDF4`
   (124 of 125 chunks) [certain].
2. The real member is 118: its tile info is at `0x80169CE0`, its screens (entry type 0) at about
   `0x801624E0`, and its texture block `0x10000` is in VRAM at (320, 256) with 0 differing rows
   [certain]. The layout fits member 118 (edges 0.986 over 27 joins / 1.000 over 3 joins; the
   highest screen index in the layout is 59, and member 118 has 60 screens) and not member 98
   (0.900 / no vertical join) [certain].
3. `info_in_ram` misses member 118 because 40 bytes of its tile info differ in RAM, all in bytes 2
   and 3 of a 4-byte word: the cell number of animated tiles. Example: offset `0x338` disc
   `000a8d00`, RAM `000a1301` (cell `0x008D` → `0x0113`). `_without_cells` clears only byte 2, so a
   cell number above 255 is not masked [certain].
4. Fix: clear bytes 2 and 3 (the whole cell halfword). `scripts/learn_patched.py` does this by a
   monkey patch. With it: `member 118, 15x6, overlay 28 @0x8b28, edges 0.986/1.000, 0 cells differ,
   31 objects @0x8c38, palettes @(0, 486) from 211 tiles`. Member 98's stale copy still matches,
   but the fit test then keeps only 118 [certain].
5. Regression check: `learn_patched.py` on the save state and name of each of the 18 catalog
   entries without `texture`, into a catalog copy: `stages.json` is the same as the repo's (the
   `diff` prints nothing) [certain]. The 3 entries with `texture` were not run.
6. Tests: on a scratch copy of `mmxdemake/` and `tests/` with the code change and the test change
   of `doc-changes.md`, `python3 -m unittest discover -s tests` gives 614 tests OK (21 skipped), as
   the repo does now. The new test line fails on the old code and passes on the new [certain].

## Facts read from the state [certain unless marked]

- `game`: stage 20, area 1, checkpoint 0, X (`game+0x38` = 0), difficulty 1, lives 4, stage time
  24 286, `frame_count` 4 105 421, `boss_choice` 2, `nightmare_effect` 2, `game+0x60` = `0xFF`
  (all 8 Mavericks defeated), `game+0x69` = `0xFF`, `game+0x66` = 3, `game+0x26` = 0,
  `gate_time` 0, `boss_pack_loaded` 0, `boss_pack_tbl`[20] = 140.
- X: step 2 (stand), (79, 363), facing right, HP 50, `mega+0xD5` = 0 (not riding), walk speed
  `mega+0xF4` = 2.0, `mega.hint` = 4.
- Camera layer 0 (0, 224), limits x 0–2576, y 224–224; layer 1 y 224 (= 224 + 32 − 32).
- `actor2`: flags `0x41`, type 1, `var` 0, state 1/0/1, HP 32, (32, 352), texture `0x0140`, CLUT
  `0x788C`, terrain box `0x800F2F28`. The stage 4 ride (`shark-1_ride`) has the same texture and
  CLUT.
- Kind-7 slot 0: type 2 (press), state 1, sub 1 (wait), timer 143, speed +1.0, offset 32. Slot 1:
  type 4, state 1 (waits for X at x ≥ 128), `+0x14` = 2576.
- Stage logic: 0 talk zone (`var` −124 = `0x84`), 2 type 15 (belt, state 1), 3 type 27 (stage
  start banner). Stage objects 0–23: the 24 press bodies of the second list. No main, visual,
  effect, misc or weapon object.
- Rescue Reploid status: ids 48–62 are 2 (saved), id 63 is 0 [likely for the nibble order].
- Overlay 28 in RAM: 63 bytes differ from the disc: two record flag bytes and the belt variables
  `0x800F35AC`–`0x800F35EC`.
- Text: member 233 (X), 234 (Zero) (`text_members_alt` index ((20 − 19) × 2 + 1) × 2 + character).
  Message 4 (X): "...What happened? / Where am I now?"; messages 0 (Zero Nightmare), 1 (High Max),
  2 and 3 (Dynamo).

## Code read [certain for the code]

- Kind-7 type 4, `0x800E9864` (states `0x800F2678`): returns while `mega+0xCC` ≠ 0 or `mega+0xE0`
  = 2. State 0 (`0x800E98C4`): checkpoint 1 → delete; else `+0x14` = camera max x, state 1. State 1
  (`0x800E9918`): when X's x ≥ 128: `mega+0xF4` += 0.5, state 2. State 2 (`0x800E995C`): camera x
  < `+0x14`: camera x += 0.5 (16.16), current and target min x = max x = camera x; else
  `mega+0xF4` −= 0.5 and delete.
- Stage logic 15, `0x800F0984`: state 0: `0x800F35EC` = `belt_turns`, `0x800F04C0` (save the
  frames), `0x800F089C` (draw the roller), state 1. Then: `mega+0xE0` = 2 → return. Player step 20
  → stop the sound (5, 3) if on, return. Camera x ≥ 2752 → `PlaySound(5, 3)`, delete. Sound (5, 6)
  when off or `mega+0x14C` ≠ 0. `0x800F0754` (turn the tiles every 5 updates); every 5 updates
  frame + 1 and redraw the roller. Then, when step ≠ 20, state 1 and HP ≠ 0: X with a terrain box
  on a cell 58 → x −= 1; `actor2` with a terrain box, no left-wall flag (`+0x70` & 2 = 0), on a
  cell 58 → x −= 1. There is no test of step 0 and no camera lock.
- `PressUpdate` set-up `0x800472D0`: offset 0, speed +2.0; stage ≥ 19: count 16; stage 4: count 8;
  else (stage 18): sub 1 at once with layer 1 mode 9. So the intro drop is 32 px in stage 20 area
  1, and the top rest is offset 32.
- Same code after relocation (instruction count, and every differing line is an address half):
  stage 9 (= overlay 07), stage 15 (= overlay 07), main 27 (= 06 and 07), main 1 (= 06), stage 11
  (= 06, 512 instructions; overlay 07's copy is shorter), stage logic 30 (= 06, 0 differences),
  `actor2` type 1 `0x800F01A4` (= overlay 06 type 0 `0x800F2414`, 4 439 instructions).
- Overlay 28 reads `nightmare_effect` once: `0x800F0D38` in the Nightmare Cube set-up (needs 5).
  No Nightmare Freeze code (overlay 07 `0x800F01CC`). So with effect 2 no Nightmare Effect object is
  live here.

## Checks against the state

- Press: layer 1 y 224 = 224 + 32 − 32 [certain].
- Teeth positions: live y = record y (offset 32 = `var`) [certain].
- Talk box holds X: x 79 in 0–224, y 363 in 240–416 → `mega.hint` 4 [certain].
- Ride Armor on the ledge: y 352 + box centre 3 + half 29 = 384 = ledge top [certain].

## Open questions

- A state with X on the belt (the scroll in state 2), one at the gates, and one in the boss room
  (Dynamo with X).
- Does the camera clamp at the left edge of the view crush X against the 96 px block at x 2752 when
  X is slow? (The camera stops at 2576, so the left edge stops at 2584; the block is at 2752: no
  [likely].)
- The tooth contact rule (the `SolidObject` squeeze) on the belt floor, in play.
