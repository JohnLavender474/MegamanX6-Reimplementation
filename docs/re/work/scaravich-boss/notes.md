# Ground Scaravich boss fight: save state `scaravich-12_boss-fight` (notes)

Date: 2026-10-23. Tags as in `docs/engine.md`: [certain], [likely], [guess].

## Method

- State: `~/mmx-data/x6/dumps/scaravich-12_boss-fight.state.gz` (SHA-1 `d52e0ef3bcf2a584f2906deb39216b5b3abb7f5f`),
  compared with `scaravich-11_boss` and `scaravich-10_section-4-sabre`.
- Probes (in `scripts/`, run from that folder):
  - `probe.py STATE...`: game fields, `mega`, layer-0 camera, every live object of the main, stage,
    visual, effect, weapon, misc, logic and ray pools. Output kept in `probe-11.txt`, `probe-12.txt`.
  - `bossfields.py STATE...`: the boss work fields, the overlay globals `0x800F6AF0`…`0x800F6AFA`
    and the live ball (visual 14).
  - `cells.py STATE...`: layer-0 words and collision values of the boss room (x 1536–2047,
    y 1104–1439) and the cells that differ between the states.
  - `ball_export.py STATE OUT`: draws the ball (visual 14) from the live object (see "Extractions").
- Kit: `python3 -m mmxdemake.rekit --name scaravich-boss --state scaravich-12_boss-fight.state.gz
  --type 50` → `kit/` (78 functions, 12 state tables). All boss and visual 14 functions were read.
- Sub-step tables were dumped by hand (`0x800F5F24`…`0x800F601C`): the kit merges the sub-step
  tables of steps 3, 4 and 5 into one table `0x800F5F8C` (11 entries), but the steps index
  different starts of it (see "State machine").
- Exports into `sprites-root/` only (`spriteexport --root`), plus the ball script.

## Live values

| what | `scaravich-11_boss` | `scaravich-12_boss-fight` |
|---|---|---|
| stage time `game+0x90` | 33961 | 36533 |
| fight time `0x800F6AF4` (updates outside step 9) | 26 | 2598 (2598 − 26 = 2572 = the stage-time difference) |
| Zero step / sub / animation | 2 / 0 / 5 (stand) | **28 / 1 / 163** (bar hang, flat bar, frame 3: sprite 40, hold 96) |
| Zero x, y, facing, `speed_x` | 1724, 1354, right, 1.0 | 1890, 1256, right, 2.0 |
| Zero HP, `game+0x98` (damage taken), lives | 32, 272, 7 | 28, 276, 7 (one hit of 4) |
| weapon objects | none | none |
| camera layer 0 | (1600, 1183); min y still easing (75) | (1664, 1155); limits x 1600–1664, y 1136–1184 |
| boss (main slot 0) | state 1 step 2 sub 1, x 1796, `speed_x` −3.0, animation 1, HP 40 | **state 1 step 5 sub 1**, x 1730, y 1360, facing right, animation 0, HP 17, timer `+0x7C` 23 |
| boss `+0x87` (cycle count), `+0x88` (ball level), `+0x8A`, `+0x9A` (live ball var) | 0, 1, 0, 1 | 0, 1, **1**, **3** |
| visual 14 (slot 0) | var 1, step 1, (1768, 1356), animation 0, HP 8, contact 4 | **var 3**, step 1, (1778, 1324), animation 2, HP 48, contact 8 |
| `game+0x8C` (bar max), `+0x8F` (boss level), difficulty | 40, 0, 1 | 40, 0, 1 |
| shots, effects, debris | none | none |

- The live boss HP is the object `+0x5C` (`game+0x8C` is only the bar maximum) [certain].
  `game+0x20` = `0x8008EF48` (main slot 0) in both states [certain].
- Also live in both states and not part of the fight: the two boss gates (stage objects 8,
  `id` 19, 20; state 2), two grey blocks of section 4 (stage object 12 `var` 2 at (1344, 3352) and
  (1440, 3352), state 1) although the checkpoint is 2, the 5 palette cycles, the 19 shaft tile
  animators (logic 29), 11 talk zones (logic 14) and one stage logic 27 (state 1/3, not read).
- No new boss animation is live (animation 0 is in the export). The boss export from this state
  gets the same ID `enemy-dd72564fdd` as the export from `scaravich-11` [certain].

## Findings

### 1. The boss room: the "platforms" are bars, and what the set-up `CopyTiles` does [certain]

- The four small platforms of `docs/stages/scaravich.md` are **flat bars, collision 45** (tiles
  37–45 are bars, `docs/engine.md` "Tile collision"): (1616–1663, 1280), (1664–1711, 1216),
  (1872–1919, 1216), (1920–1967, 1280). Zero hangs on the right upper bar in this state (step 28,
  y 1256 = bar row 1216 + 40).
- `scaravich-10` (before the boss set-up) → `scaravich-11` (after): 60 layer-0 cells change, and
  `scaravich-11` → `scaravich-12`: none. The two `CopyTiles` calls replace the 2 × 15 cells at
  x 1568–1599 and 1984–2015, y 1136–1375 with the source strip (1600, 1136):
  - left: the wall (collision 56, words `0x4B`/`0x4C`/`0x51`/`0x52`/`0x56`/`0x57`) and the boss door
    cells of gate 20 (collision 59, words `0x881`–`0x88D`, y 1312–1375) become empty (collision 0);
  - right: the wall (56) becomes empty;
  - row y 1280 gets one bar cell (45) at x 1584 and x 2000 (the source column x 1616 is a bar end);
  - row y 1360 gets the words `0x1F`, `0x20` (collision 0) on both sides.
  So the room is open on both sides behind the camera limits. With the limits x 1600–1664 the view is
  x 1600–1983, so the changed cells are not on screen, and `CameraClamp` keeps the player in x
  1608–1976. The boss has no terrain box, but its walk turns at x 2176 and 1424 and the ball is
  deleted only past x 2016 + 16 × var and 1568 − 16 × var. Purpose: the boss and the ball go out
  through the walls [guess]; the only tile test of the fight is the pellet (`CornerCheck`), which
  now does not stop at the side walls [likely].

### 2. The boss state machine (main 50, overlay 08) [certain for the code]

State table `0x800F5F34`: 0 `ScaravichInit`, 1 `ScaravichMain`, 2 `ScaravichDeath`.
Step table `0x800F5F40` (12 entries). Sub-step tables: step 2 `0x800F5F7C` (2 used, then 2
empty), step 3 `0x800F5F8C`, step 4 `0x800F5F94`, step 5 `0x800F5FA0`, step 6 `0x800F5FB0`, step 7
`0x800F5FB8`, step 8 `0x800F5FC0`, step 9 `0x800F5FD8`, step 10 `0x800F5FF8`, step 11 `0x800F600C`.
Death steps `0x800F5F70`.

**`ScaravichMain` each update** (`0x800F0200`):

1. `+0x82` = HP (to see a hit in the step).
2. Ball level `+0x88` from the cycle count `+0x87`: Easy/Normal (difficulty < 2) 0–3 → 1, 4–6 → 2,
   7–8 → 3, 9+ → 4; Hard and up 0–2 → 1, 3–4 → 2, 5 → 3, 6+ → 4.
3. In steps 2, 4 and step 11 sub-steps 0–1 (the ball rolls): a sound timer `0x800F6AF0`. At 0: with
   `+0x8B` = 1 sound (2, 35) and 50–53 updates; else by the ball var `+0x9A`: 1 → (2, 34), 40–43;
   2 → (2, 35), 50–53; 3 → (2, 36), 90–93; 4 → (2, 37), 100–103. Also `0x800F6AF2` counts up and
   plays (2, 52) every 60 updates. Outside these steps `0x800F6AF2` = 0.
4. When the hit timer `+0x80` = 0: `no_hit` = 0 and `ContactMega` (contact 4). So no contact damage
   while the hit timer runs.
5. `ContactWeapon`: a kill → sound (2, 32), `mega+0x7A` = 1, state 2. A hit (not 0, not 127)
   with `+0x80` = 0: `dmg_tbl_deflect`, hit flash (`clut` bit 15), `+0x80` = 2 for weapon ids 24
   and 25 (sabre hits 1 and 2), else 30. **Stun**: weapon ids 1, 10, 54 (`ContactWeapon` 2, 11,
   55) in steps 2, 4 or 6: sound (2, 33), `+0x80` = 180, step 10. While `+0x80` > 0 it counts down
   and the palette blinks; at 0 the damage table is `0x80074C9C` again.
6. The step from the step table; the fight timer `0x800F6AF4` += 1 except in step 9;
   `DisplayCheck(96, 96)`.

| step | function | what [certain for the code; names likely] |
|---|---|---|
| 0 | `HitReactionDispatch` | engine hit reaction |
| 1 | `0x800F32D8` | step 3 |
| 2 | `0x800F1E48` | **walk out with the ball**. `act4` = 1 (ball destroyed) → step 7. HP changed and `+0x84` = 0 → `+0x84` = 1, step 8. Sub 0: animation 1, `speed_x` by the ball var: 1 → 3.0, 2 → 2.5, 3 → 2.0, 4 → 1.5 px/update, sign by facing. Sub 1: walk; past x 2176 (going right) → face left, step 3; below x 1424 (going left) → face right, step 3. Both points are off screen |
| 3 | `0x800F1FFC` | **new ball**. Sub 0: damage table `0x80074C9C`, `+0x87` += 1 (max 10), `+0x8B` = 0, `+0x84` = 0, timer = ball level × 10. Sub 1 (`0x800F0794`): at the end of the timer, visual 14 `var` = ball level (`+0x9A` = it), at the boss; on Hard and up, a chance of 1, 2, 3, 4 in 16 (by level) that the ball is the hard ball (visual `act4` = 1, boss `+0x8B` = 1). Then: HP < bar max / 2 → RNG % 16 < 9 → step 4, else step 11; else step 4. The first time the HP is below half (`+0x8A` = 0 → 1): level 3 → `+0x87` = 0; level 4 → `+0x87` = 4 (Easy/Normal) or 3 (Hard): the ball gets small again |
| 4 | `0x800F208C` | **walk in**. Same `act4` and hit tests as step 2. Sub 0 = step 2 sub 0. Sub 1: walk until x > 1728 (going right) or x < 1856 (going left), not on sprites 4, 5. Sub 2: RNG % 16: 0–4 → animation 0, step 5; 5–9 → step 6 with a timer of 90; 10–15 → step 2 sub 1 (walk on) |
| 5 | `0x800F22D8` | **pellet throw** (live in `scaravich-12`, sub 1, 23 updates left). `act4` = 1 → step 7. Sub 0: animation 0, timer 40. Sub 1: wait; repeats `+0x89` = 0 (boss level 0–1), 1 (level 2), 2 (level 3); animation 2. Sub 2: at frame flag 5 (sprite 11): sounds (2, 38), (2, 55), visual 14 `var` 0 (the pellet) with the boss graphics; timer 50. Sub 3: wait; then again from sub 2 while `+0x89` > 0 (−1 each); else step 2 |
| 6 | `0x800F2574` | **wait**. `act4` = 1 → step 7. Sub 0: animation 0. Sub 1: timer (90 from step 4, 40 from step 8), then damage table `0x80074C9C`, step 2 |
| 7 | `0x800F2660` | **run off** (ball destroyed). Sub 0: animation 4, 5.0 px/update: left when x < 1792, else right. Sub 1: past x 2176 or 1424: `+0x87` += 1 (max 10), step 3 |
| 8 | `0x800F27B8` | **hit run**. `act4` = 1 → step 7. Sub 0: animation 4 at the same speed for 32 / |v| + 6 (level 1), 64 / |v| + 6 (2), 96 / |v| (3), 128 / |v| − 6 (4) updates, or 32 / |v| + 8 with `+0x8B` = 1. Sub 2: face against the move, step 6 with 40. Only once per ball (`+0x84`) |
| 9 | `ScaravichIntro` | entry (below) |
| 10 | `0x800F2D7C` | **stun**. Sub 0: `dmg_tbl_deflect`, animation 5, `speed_y` 4.0 up, gravity 0.125. Sub 1: at y 1360 again: `speed_x` by the ball var, `+0x80` = 60, step 8 (step 7 when `act4` = 1). Sub-steps 2–4 (`0x800F2F04`, `0x800F2F24`: floor → `Rumble(5)`, `Quake(20, 2, 2)`; `0x800F2FA0`: 41 updates, face the player, step 3) have no writer in the walked code [likely not used] |
| 11 | `0x800F3048` | **kick the ball** (only when HP < half). `act4` = 1 → step 7. Sub 0: walk (as step 2 sub 0). Sub 1 (`0x800F0AB0`): going right past x 1664, or going left below x 1920: `+0x89` = boss level + 1, sound (2, 39) or (2, 40), animation 3. At its frame flag 2 (sprite 16) the ball is launched (below). Sub 2: wait for `act4` = 2 (the ball is gone); `+0x89` −= 1; 0 → step 7. Sub 3: visual 14 `var` 5 (a ball falls from y 1024 in front of the boss, random size 1–4). Sub 4: wait for `act4` = 3 (it landed and is now carried), animation 3 again, sound (2, 39)/(2, 40), sub 2. So boss level + 1 kicks |

**Entry, step 9** (`0x800F5FD8`), additions to the doc:

- Sub 1 calls `PlayerEvent(29, right)` at once and sets the timer (100; stage 12: 10). It saves
  `cam+0x48` in `+0x86` and sets `cam+0x48` = 1, `cam+0x47` = 2, target min x = max x = 1632, and
  makes `BossWarning` (`+0x8C`).
- Sub 2: at the end of the timer `PlayerEvent(20, right)`. When `BossWarning` is gone:
  `game+0x24` = 1, `game+0x25` = 0, `game+0x20` = boss, `game+0x8C` = 40, `BossLevel`, `+0x85` = 40,
  `speed_x` = −3.0, `cam+0x47` = 2.
- **Sub 3 (`0x800F09B4`, walk in and text)**: `AnimeTick`, `MoveObject` (−3.0 px/update); sound
  timer `0x800F6AF0`: sounds (2, 52) and (2, 34), then 40–43 updates. When x < 1888 and the sprite is
  not 4 or 5: stage ≠ 12 → `TextOpen(0, 255, game+0x2E)` and `game+0x2E` = 1. So the first time
  the text box opens (mode 0); after a retry `game+0x2E` is 1 and `TextOpen` only waits 60 updates
  with no text (mode ≠ 0). Stage 12: no text. Then animation 0, sub 4.
- Sub 6 (HP fill) also restores `cam+0x48` from `+0x86`.

**Death** (`0x800F5F70`): sub 0 `PlayerEvent(20, player facing)`, `mega+0xCA` = 0, animation 5,
127 updates, blink; sub 1 at 0: `BossExplosion` (stage logic 26, `+0x8C`); sub 2 (`0x800F063C`):
blink until the explosion sets its `+0x07`; when the explosion object is gone: `game+0x24` = 0,
`game+0x20` = 0; stage 12: exit request `0x80`, `game[0x2C + checkpoint]` = 1, checkpoint += **8**;
else a Nightmare Virus soul (main 15) `var` `0x10` when the fight time `0x800F6AF4` < 5401, else
`0x11`; `DeleteObject3`.

### 3. Visual 14 is the ball (and the pellet) [certain for the code]

Entry `0x800F32E8`, states `0x800F62AC`: 0 init `0x800F0BD4`, 1 main `0x800F1AEC`, 2 delete.
Steps by `+0x05` (table `0x800F62D0`). Data per `var` at `0x800F6288` {dx, HP, contact, dy}; hurt
box `0x800F6024` + 4 × var, attack box `0x800F6038` + 4 × var (centred squares):

| var | what | dx | HP | contact | dy | hurt box | attack box |
|---|---|---|---|---|---|---|---|
| 0 | pellet (boss graphics, boss animation 6) | 10 | 0 | 4 | 0 (y + 6) | 16 × 16 | 12 × 12 |
| 1 | ball, size 1 | 28 | 8 | 4 | 4 | 32 × 32 | 30 × 30 |
| 2 | ball, size 2 | 42 | 16 | 6 | 16 | 48 × 48 | 46 × 46 |
| 3 | ball, size 3 | 48 | 48 | 8 | 36 | 80 × 80 | 72 × 72 |
| 4 | ball, size 4 | 46 | 127 | 16 | 50 | 112 × 112 | 100 × 100 |

The ball is in front of the boss (x ± dx by facing) with its bottom on the floor (y = boss y −
dy). `scaravich-12`: ball var 3 at (1778, 1324) = (1730 + 48, 1360 − 36) [certain].

- **Graphics**: texture = boss texture + `0x100` (`0x240`), palette and sprite data of type-list
  bank + 1 (bank 15, the second type-50 bank, 120 sprites; stage 12: `ray_table+4` entry 1 and
  palette `0x794C`), animation table `0x800F6234`. Animation var − 1 (0–3) for a normal ball; on
  Hard and up a coin toss gives animation var + 10 (11–14) with damage table `0x80074DDC` and
  `+0x96` = 1; the hard ball (`act4` set by boss step 3) is animation 10, `var` 1, `dmg_tbl_deflect`.
- **Pellet** (`var` 0, step 0, `0x800F3354`): boss level 0–1: ±3.0 px/update straight; level 2–3:
  3.0 px/update aimed at the player (`ratan2`); `dmg_tbl_capsule`; terrain box `0x800F6020` (4 × 4);
  ends at the player (spark) or at a wall (`CornerCheck`, spark).
- **Carried** (step 1, `0x800F12D4`): follows the boss (not in x during boss step 8; not in y during
  step 10 or the death). It rolls (`AnimeTick`) only on boss frames with flag 1 (the walk). A boss
  frame with flag 2 (animation 3, the kick) launches it: step 3, 4 or 5 (RNG % 3). Boss step 3 →
  the ball is deleted. Destroyed → step 2, boss `act4` = 1 (boss runs off, step 7).
- **Launched** (boss step 11): step 3 rolls at 3.0 px/update toward (player x, y 1323 / 1307 / 1291
  / 1275 by var); step 4 a lob: `speed_x` = (player x − x − 16) / 40, `speed_y` 2.5, gravity 0.078,
  on the floor `Rumble(5)`, sound (2, 54) and 3.0 px/update rolling; step 5 bounces at 1.33 px/update,
  `speed_y` 3.5, gravity 0.109, sound and `Rumble(5)` at each bounce. All: sound (2, 53) at the
  launch; out of x 1568 − 16 var … 2016 + 16 var or y 1104 − 16 var … 1408 + 16 var → deleted, boss
  `act4` = 2; destroyed → break (step 2), boss `act4` = 2.
- **Falling balls**: `var` 5 (boss step 11 sub 3, step 6): falls at 3 px/update from y 1024; on
  the floor it becomes the carried ball (step 1, boss `act4` = 3); destroyed → boss `act4` = 1.
  `var` 6 (step 7): made when the hard ball (`act4` ≠ 0) takes a hit, at most once per 121 updates;
  it falls from y 1088 at the player's x and breaks on the floor (`Rumble(5)`).
- **Break** (step 2, `0x800F1470`): 120 updates of blinking and explosions, then 6 × var debris
  (`SpawnJunk`, animations `0x800F629C` or `0x800F62A4` for `+0x96`), 4 × var explosions,
  delete. At the boss death every ball breaks.

### 4. Damage [certain for the tables; the weapon names are open]

- Boss `0x80074C9C` (the kit names it `dmg_tbl_yammark+0xa0`): HP 40. id 0 (lemon) 1, **id 1 5,
  id 10 8, id 54 5** (the stun ids), ids 17, 22, 47, 57 8, ids 18–20 4, sabre ids 24 and 25 1, 26–43 2,
  id 60 and 62 15; ids 5, 14, 21, 23, 36, 44, 64–78 deflected.
  Zero's ground combo does 1 + 1 + 2 per hit: the boss is very strong against the sabre.
- Ball `0x80074D3C`: sabre ids 24, 25 2, **id 26 5**, id 44 8, ids 1, 10 2, id 54 1.
- Ball with `+0x96` `0x80074DDC`: everything deflected except id 56 (127).
- Ids 1, 10, 54 are also the ids of `dmg_tbl_nightmare_bug` (`docs/engine.md`). Slot 1 is Commander
  Yammark's weapon (Yammar Option; Zero's Yammar Option is weapon type 1 too) and 10 = 1 + 9 (the
  charged slot), so the pattern "slot n, slot n + 9, one more id" of the other bosses fits [likely].
  What id 54 is: open.

### 5. Zero: step 28 (bar hang) live for the first time [certain]

`docs/players/map.md`: step 28 = bar hang (`0x800361D4`), `docs/players/air.md` "Hang". The state
agrees: sub-step 1 (`BarMove`), animation 163 = 163 + n with n = 0 (flat bar, tile 45), frame 3 of 4
(sprite 40, 96 ticks, the hold), `speed_x` 2.0 (the bar speed `m.124`), `+0x84` = 0 (no bar dash),
`+0x86` = 0 (dash flags cleared), `+0x67` = −1 (in the air), `+0xE3` = 0. The held buttons
`+0x7C` are 0 in the state, but `speed_x` keeps the 2.0 of the last move [observed]. The Zero export
(`players/zero/animations.json`) has animation 163 with sprites 38, 39, 40, 40 and the hold 96:
the live frame matches [certain]. No weapon object is live, so this state does not help the sabre
or technique open points.

## Extractions

- `sprites-root/` (`spriteexport --root`): `ground-scaravich` is the same export as in
  `sprites/` (same ID). `enemy-c3b1ceb655` (type 31), `enemy-74a8cdd23c` (type 37) and
  `enemy-f9d273655b` (stage type 12) and `bank-7.png`, `bank-9.png`, `stage-12-live-0.png` are
  **wrong**: by eye they show pieces of the ball (grey rock lumps, ball rims), not their objects.
  The cause [likely]: the ball texture `0x240` (boss `0x140` + `0x100`, 256 texture cells) covers
  the texture cells of banks 7 (`0x260`), 8 (`0x260`) and 9 (`0x2A0`). `spriteexport`'s overwrite
  check knows the boss (main type 50, texture `0x140`; it skips types 12 and 28) but not the ball,
  which is a visual object whose sprite data is a main-type bank (bank 15). This is the cause of the
  `docs/TODO.md` item about `scaravich-11_boss` (types 31, 37 and stage 12).
- **New: the ball** (`scripts/ball_export.py`) → `sprites-root/enemies/scaravich-ball/`:
  `bank-15-ball-live.png` (all 120 sprites of bank 15: grey balls of 4 sizes, the gold hard ball,
  purple balls, small debris), `anim-0.png`…`anim-20.png` (table `0x800F6234`, 21 animations, all
  8 or 4 frames, looped) and `animations.json`. Checked by eye: the 4 grey sizes (animations 0–3)
  roll as one ball each, the gold one (10) is the small hard ball, 11–14 are purple versions of 0–3,
  4–9 and 15–20 are small pieces (debris) [certain for the drawing; the purple colour is the palette
  +1 of the sprite pieces as drawn by `draw`, likely right]. `spriteexport` does not make this
  export: bank 15 has no texture-table entry, and a main-type bank is not an extra source.
- No `playerexport` run: all player frames are in RAM in every state, so this state adds no
  frames.

## Open questions

1. Weapon id 54 (a Scaravich stun id and a Nightmare Bug id).
2. Boss step 10 sub-steps 2–4: a writer outside the walked code, or dead code.
3. Why the set-up opens the side walls (a state with the ball or the boss off screen, or a state
   where a pellet passes x 1600).
4. Stage logic 27 (live in states 11 and 12, state 1, sub-step 3): not read.
5. The grey blocks of section 4 stay live at checkpoint 2 (state 1); the doc says they delete
   themselves when `game.checkpoint` ≥ `var`. Likely only the set-up tests it.
6. A Hard state, to see the hard ball, animations 11–14 and the falling ball `var` 6 live.
