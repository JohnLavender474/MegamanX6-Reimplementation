# Nightmare Snake (Heatnix stage, first mini boss): working notes

## Method

- Read `CLAUDE.md`, `README.md`, `docs/re-guide.md`, `docs/enemies/nightmare-pressure.md` (model),
  `docs/enemies/boss-gate.md`, the camera, damage and game-state parts of `docs/engine.md`.
- Nothing on stage 3 existed in `docs/` or [`~/mmx-data/x6/re/`](../) before (only the gate doc names
  "main type 25 (id 7)" as an open question).
- Kit: `python3 -m mmxdemake.rekit --state heatnix-2_mini-boss.state.gz --type 25 --name
  heatnix-mini-boss` → `kit/` (60 functions, 13 state tables, neighbour visual 29). Visual 29:
  `--class visual --type 29 --out .../kit/visual29` (the same 7 functions as the neighbour part of
  `kit/`). `--at 0x800504D0` for the boss gate code that makes type 25.
- `scripts/dump_state.py <state>`: the type 25 and visual 29 objects, stage objects, game, mega and
  the three layer cameras. `scripts/find_type25_makers.py <state>`: code that writes 25 into byte +1
  of an object (one hit: the gate, `0x800504F8`).
- Overlay check: `rock_bin/05.bin` against RAM at `0x800E9860`: 4 of 44084 bytes differ → stage 3
  area 0 = overlay 05 [certain].
- Name: web search ("Nightmare Snake", ナイトメア・スネーク, five red Mechaniloid mid-bosses of Blaze
  Heatnix's stage, Ouroboros). The Fandom API returned HTTP 402, so the page was not read.

## Key facts and where they come from

| fact | evidence |
|---|---|
| body = `var` 5; gems = `var` 0–3; flames = visual 29 `var` 0–3 | `0x800EB0E4` loop (4 × `GetMainObjectAfter` + `GetVisualObject`), state: slots 2–5, visual slots 0–3 |
| body made by the fixed record list (index 0, (448, 512)), not by a record | TMX `fixed records`; `heatnix-1_start` has `var` 5 and `var` 4 in state 0 in slots 0, 1 |
| ring = BG layer 1 at (512 − x, 512 − y), drawn relative to layer 0 | `0x800EB0BC`, `0x800EB434`; state layer 1 (−105, −72) for body (617, 584); stage tool cameras for this entry (231, 408) = layer 0 + layer 1; ring art in the layer 1 map at (433–591, 432–576) |
| trigger x ≥ 363 or y ≥ 464, checkpoint 0 | `0x800EAB74`, `0x800EACC8` |
| HP = sum of gems, cap 127 | `0x800EB3BC` (body HP 0), `0x800EB478` (gem add, −128 → 127); state HP 127 with four gems at 32 |
| death = four gems dead | `0x800EB3C0` (`act4` ≥ 4), `0x800EB4BC` (`act4` + 1) |
| gate opens through `game+0x26` | `0x800EC5C0` (= 1 at the end), `0x800EB28C` (= 0 at the set-up); gate table in `boss-gate.md` |
| roll 1 → 2 px/frame, 60-update stops | `0x800ECC24`–`0x800ECDC0`; state `speed_x` 2.0625 |
| player y ≥ 432 | `0x800EC67C`, `0x800EC5EC` |
| shots every 120 updates by difficulty | `0x800EB538` |
| `dmg_tbl_nightmare_snake` | `data.txt` at `0x800748DC` |

## Checked against `heatnix-2_mini-boss` [certain]

- Body slot 0: `var` 5, state 1, step 3, sub 1 (roll right), (617, 584), `speed_x` 2.0625,
  `accel_x` −4096, HP 127, contact 8, boxes `0x800F3DDC` / `0x800F3DE0` / `0x800F3DD8`,
  `dmg_tbl_deflect`, `+0x8D` = 1, `+0x7C` = 0, `+0x7E` = 3 (left by the intro), `display` 0.
- Gems slots 2–5: HP 32 each, step 1, no attack box, hurt boxes `0x800F3DE4` + 4 × `var`, table
  `0x800748DC`, places body + offset exactly: (573, 527), (675, 530), (589, 634), (683, 620).
- Flames: visual 0 drawn (body sub 1 = right), 1–3 not drawn; places body + offset exactly:
  (533, 573), (699, 566), (643, 654), (613, 501); `var` 0 flip `0x40`.
- `game`: checkpoint 0, difficulty 1, bar 1, boss = slot 0, `boss_hp_max` 127, boss level 1.
- Zero `var` 1 at (366, 618), `mega+0xD1` = 20 (from the intro event), `+0xD0` = 0 (event ended).
- Layer 1 mode 5, shown, (−105, −72); layer 0 camera (336, 480), limits x 336–624, y 480.
- Body `var` 4 waits in slot 1 (state 0, (1248, 2624)); gate stage 8 `var` 7 at (944, 608) state 1.
- From x 576 to 617 at full speed: about 25 updates since the intro ended [likely].

`heatnix-1_start`: `var` 5 in state 0 at the fixed-record point (448, 512), with HP 32, contact 8
and the animation table already stored. The checkpoint test (`0x800EAB88`) comes before the common
stores, the position test (`0x800EACE8`) after them; the player is at (80, 394), so the code returns
at the position test each update [certain].

## Recursion and re-entry

None. No function calls itself; step functions are called once per update through the tables.

## Hazards

- **Byte HP sum** (`0x800EB478`): the body HP is a byte; 4 × 32 = 128 wraps to −128 and the code
  changes exactly −128 to 127. A gem with more HP, or a fifth gem, would wrap to a negative value
  and the bar would show a wrong value; it would not kill the body (death is by `act4`). In the
  game the gem HP is fixed at 32, so it is safe. A reimplementation: `min(sum, 127)`.
- **Kill update**: a gem that dies adds its negative HP (after the hit) to the body for one update.
  The bar can show a lower value for one update. Harmless.
- **Order**: the HP sum needs the body (slot 0) to update before the gems (later slots, made by
  `GetMainObjectAfter`). If a gem were in a lower slot than the body, the body would clear the
  HP after the gem added it, and the bar would show only part of the sum. A reimplementation:
  compute the sum in the body from its gem list.
- **Missing null test**: `SnakeGemShoot` and the flames read the parent pointer without a test;
  the parent always lives longer (the jets end at body state 3, the gems at their own death).
- **`var` 5 never starts at checkpoint ≠ 0**: the object stays in state 0 forever and the gate stays
  locked unless `game+0x26` is set by something else. A restart behind the gate must not need it.

## Sprites

- `spriteexport --state heatnix-2_mini-boss.state.gz --root sprites-root`: 8 exports.
- `enemy-6e15315f49` (type 25, live, table `0x800F3ED8`, bank 6, 13 animations, cell 49 × 64):
  checked by eye: all 13 strips correct and complete (gems, ball, flames). Bank 6 is clean (no
  boss texture over it).
- The ring is not a sprite: `sprites-root/ring-layer1.png` is the crop of the layer 1 art
  (158 × 144, layer px (433, 432)–(591, 576)) from `stages/m097-blaze-heatnix-mini-boss/stage.ora`
  (`art 1 decoration`), colours of the save state.
- `enemy-7f9bbc9a47` (main type 36, found by the code search, 5 animations): fire jets / flame
  pillars; strips look complete. Not this topic; a stage hazard of stage 3 (name open).
- `enemy-c1aea0bc4c` (stage type 4 live, the Rescue Reploid `id` 32 in this room; table
  `0x80078BEC`, 36 animations): the same table as `rescue-reploid`, other sprite data (green and
  brown Reploid). Animations 26, 27 and 33 mix Reploid poses with shot sprites: they are the set-B
  animations, which `rescue-reploid` draws from other sprites (`docs/enemies/rescue-reploid.md`,
  set B). In this live source those sprite numbers are other images → **26, 27, 33 are wrong**.
  The other 33 strips look right.
- `boss-gate`, `nightmare-bug`, `nightmare-virus`, `nightmare-virus-orb`, `teleport-crystal`: IDs
  already in `~/mmx-data/x6/sprites/` (same fingerprint); the run only adds stage 3 sources.

## Open questions

See the sheet. Also: which of records 22–26 (`var` 6–10) belong to which of the "five" snakes of
the wiki (the code has seven bodies: `var` 4–10; 8 and 9 exclude each other by checkpoint).
