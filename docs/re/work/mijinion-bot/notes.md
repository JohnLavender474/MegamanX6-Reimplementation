# Illumina (stage 8, overlay 11, main type 24): notes

## Method

- Kits (all from `mijinion-2_illumina-helpers.state.gz`):
  - `kit/` main 24 (134 functions); `kit-main8/`, `kit-main11/`, `kit-main19/`, `kit-main28/`;
    `kit-logic5/` (stage logic 5), `kit-logic27/` (stage logic 27 = `ReadyControl`, not related).
  - `--at` reads: `0x800232D4` (DrawObject4, layer < 0), `0x8002D654` (BoxOverlap),
    `0x8002D868` (WeaponBoxTest), `0x800F21C0` + `0x800F4BB8`, `0x800F4CBC`, `0x800F4D28`,
    `0x800F116C`, `0x800F4588` (ray type 0).
- `anim-table-full.txt`: the full animation table (84 animations; the kit and the exporter stop at
  64, `MAX_ANIMS` in `mmxdemake/anims.py`).
- Scripts (`scripts/`): `live.py` (live objects and cameras of a state), `ram.py` (hex dump of RAM),
  `findseq.py` (find a word sequence in RAM).
- Tables found by searching RAM: main table `0x800FB830` (stage 8: types 8, 11, 15, 19, 24, 28, 47,
  58 have code), visual table entry 28 at `0x800FB974`, effect table entry 11 at `0x800FBA18`, ray
  table `0x800FBCC0` (entry 0 = `0x800F21C0`, overlay code).
- Name: `~/mmx-data/x6/re/damage/step3/text/m219/text.txt`, messages 0–1 (Illumina, the cables). A web
  search (Fandom summary) agrees: "two large cables with lens ... its only weak spots", "after the
  first cable is destroyed Illumina will deploy drones to fire lasers and use its head to pinpoint
  their target". The drones, the cannons and the orbs have no name in the game text read.

## Key facts and evidence

- Screen space: the body writes x 160, y 136 each update (`0x800F2214`); all pieces and parts get
  `layer` −1. `DrawObject4` uses x, y directly when `layer` < 0 (`0x8002331C`). The box tests add
  `cam0` x and y (`0x8002D6CC`, `0x8002D7E0`, `0x8002D8DC`, `0x8002DBD4`) [certain].
- BG layer 2 is not the robot: `stage.ora` layer `art 2 background` shows a green tunnel in three
  frames (y 0, 256, 512); stage logic 5 (`0x800FB1B4`) cycles layer 2 y every 16 updates. State 1
  layer 2 y 256, state 2 y 512 [certain].
- The cables: `stage.ora` layer `art 1 decoration` shows a grey cable with a red lens at world
  (2240–2384, 368–640) and (4416–4560, 752–1024) [certain for the drawing]. The lens box
  `0x800FC2B0` = (104, −46, 24, 26) from screen (160, 136) with the locked cameras gives world
  (2312–2336, 474–500) and (4488–4512, 858–884); the lens pictures are at about (2318, 481) and
  (4494, 865) [likely].
- Phases: `0x800F1BE8`; camera writes `cam+0x26` (target min x), `+0x28`/`+0x2A` (target max/min y);
  set-up writes target max x 2048 (`0x800ED918`); step 8 sub 4 writes 4224 (`0x800F327C`); death
  writes 5168 (`0x800F34E0`).
- HP: `boss_hp_max` = 64 (`0x800F23BC`); state check: `0x800CCF5C` = `0x40` in both states.
- Weakness: `0x800F2280`: ContactWeapon returns 7, 16 or 43 (ids 6, 15, 42) → step 10.
- `dmg_tbl_illumina` read from the executable (file offset `0x8007483C − 0x80010000 + 0x800`);
  RAM and file agree.
- Zoom shot: `0x800F013C` (start (160, 56), target = player), `0x800F0330` (division by 39 with
  `0xD20D20D3`, sprite frame t/3, at t = 39 box `0x800FCB9C` (−20, −20, 40, 40) and layer 0),
  `0x800F47B8` (16 updates, then delete). Animation 32 is 14 one-tick hold frames (sprites 0–13);
  `SetAnimeFrame(32, n)` indexes them directly.
- Drone attack signals (`illumina_laser`, `0x800FD51B`): head sets 0 at the probe start; probe
  sets 1 on player touch (`0x800F3888`, BoxOverlap with `mega`), 2 after 60 updates
  (`0x800F3948`); drones fire on 2 and set −1 after 50 updates (`0x800F36DC`); the head goes home
  on `illumina_drones_done` and sets −3 at home; the body closes on −3.
- Head in phase 3: `+0x8A` = 360 at the start; the test `illumina_time < 11` ends the attack at
  the start of fight 2 (`0x800EF278`).
- Wall controller (`var` 64): `0x800F3B54`…`0x800F3C10`, `0x800EF7FC`, `0x800EF98C`, `0x800EFAE4`.
  The walls are `CopyTiles(0, …)` on layer 0 with the source `0x800FC9B0` (x 2048 / 2336, rows grow);
  restore from `0x800FC9C0`. That the tiles are solid is likely (layer 0 is the collision layer).
- Reward: `0x800F1FBC`: `illumina_time` < 5401 → main 28 `var` 1 at (4384, 864). Main 28 of
  overlay 11 is the teleporter crystal (export `teleport-crystal`, the same fingerprint as the
  Yammark one) [certain for the export; the destination is open].

## Check against the save states

| state | expected from the code | found |
|---|---|---|
| 1 | body at (160, 136), step 4 (phase 0 uses only ZOOM SHOTS), sub 1 | slot 0, 1.4.1, (160, 136); `illumina_phase` 0 |
| 1 | pieces `var` 32–36 of step 4 sub 0 | effect 11 `var` 32–36 at (160, 136) ± 32 |
| 1 | camera max x 2048 | `cam0` max x 2048 |
| 1 | no jets yet; jet timer counting | no main 8; body `+0x84` = 35 |
| 1, 2 | `boss_hp_max` 64, `game+0x20` = body, bar off | 64, `0x8008EF48`, `game+0x24` 0 |
| 2 | phase 2, step 5 sub 3 (waiting for the head) | `illumina_phase` 2; body 1.5.3; body HP 0 |
| 2 | camera min x 2048, max x 4224 | 2048 / 4224 |
| 2 | head `var` 16 at (160, 32 ± 6), step 2 sub 1 | (160, 30), 2.2.1 |
| 2 | drones at (288, 96), (32, 32), (80, 176) ± 8 in y, step 3 sub 1 | (288, 96), (32, 37), (80, 175), 2.3.1 |
| 2 | drone count 3 | `illumina_drones` 3 |
| 2 | probe locked: step 5 sub 2, target = player − camera | `var` 32 2.5.2 at (160, 170); `illumina_target` (160, 170); Zero (2439, 554) − camera (2279, 384) = (160, 170); `illumina_laser` 1 |
| 2 | markers (piece `var` 4) at the target; hatch pieces 48–53 | effect 11 `var` 4 ×3 at (160, 170); `var` 48–53 |
| 2 | scan beam ray `var` 0 and its copies `var` 1 | ray 0 `var` 0, `var` 1 ×3 |
| 2 | a jet group blocks new jets | main 8 `var` 18 (a follower) at (2338, 730); body `+0x84` 0 |
| 1, 2 | main 19 `var` 1 (Nightmare Effect 3 spawner) inactive (player x < 2513) | 1.2.0.0, no fire balls |

All agree.

## Main 19 here (Nightmare Effect 3) [certain]

The same code as in Blizzard Wolfang's stage (`docs/engine.md`: "3 main type 19 of stage 2"), now
read: set-up `0x800ECF30` deletes the object unless `nightmare_effect` = 3. `var` 1 (fixed-list
record) is the spawner: its active zone depends on the stage; for **stage 8 it is player x
2513–4223** (`0x800ED2E0`), i.e. the part of section 1 between the two Illumina fights. While
active: `Rumble(8)` once, every 20/30/40 updates (random) 0–2 fire balls (main 19 `var` 0) at
camera x + (0–3) × 80 + (0–2) × 32, camera y − (0–4) × 16, and a camera y shake of ±2 (±6 at
random 1/16) every second update. The fire ball (`var` 0): HP 8, contact 4, `dmg_tbl_reflector_wait
+0xA0` (`0x8007465C`), 3 px/update down, sound 2/80; it explodes and drops (table 18) when killed.
The export `enemy-78c8f556f5` is the same object as in Wolfang's stage (same fingerprint).
For stage 2 the zones are (x 4641–5039, y 225–1263, checkpoint 2) and (x 4993–5679, y 1281–1695,
checkpoint 3) (`0x800ED238`…`0x800ED2C4`) [certain for the code].

## Main 8 (jets) and main 11 (orbs) [partly read]

- Main 8: `var` 0 leader HP 4, contact 4, `dmg_tbl_normal`; it makes 4 followers `var` 16–19 (second
  palette) and fires visual 6 (green ring shots). Steps 2 (Easy, Normal) or 6 (Hard). Export
  `enemy-304e0899c6`: a red jet fighter, 7 animations [good sheet].
- Main 11: an orb that opens into a turret, visual 10 shots; export `enemy-c3301f6267`, 12
  animations [good sheet]. Only `var` 16 from Illumina is known here.

## Hazards

- `SetAnimeFrame(obj, 32, t / 3)` (`0x800F0424`): animation 32 has 14 hold frames; t ≤ 39 gives
  frame ≤ 13. A larger t would read the next animation. Safe because t stops at 39.
- `illumina_clear` is one global for all parts: the body clears it one update after setting it (steps
  8, 9, 10 sub 0). Objects that update before the body in that update see it set; all parts are in
  later slots or other classes, so all see it once [likely; order of the loops].
- `illumina_target` is written by the probe in screen coordinates and read by the drones and the
  hit point; the hit point adds the camera at its update, so a camera move between lock and fire
  moves the hit point with the screen [certain for the code]. In the fights the camera is locked; in
  phase 2 the camera can scroll, and the hit point follows the screen, not the world place.

## Sprites

`spriteexport --root ~/mmx-data/x6/re/mijinion-bot/sprites-root` on both states; then once more with
a test catalog (`catalog-test/enemies.json` = `catalog/enemies.json` + one `sets` rule).

| export | result |
|---|---|
| `enemy-ef4d46bae1` (main 24, state 1, with the test rule) | **good** for 0 and 2–63: robot pieces 0–31, drone bank 32–63 (drones, zoom shot frame 0, marker, cannons, beams). Wrong: animation 1 (empty sprites 100–104). Missing: 64–83 (exporter cap of 64) |
| `enemy-b797c30985` (main 24 without the rule / effect 11 live, both states) | good 0, 2–31; **wrong 1 and 32–48** (drone animations drawn with robot sprites) |
| `enemy-cf8f009f32` (effect 11 live-1, state 2: piece `var` 4, drone bank) | good 32–63; **wrong 0–31** (robot animations drawn with drone sprites) |
| `enemy-304e0899c6` (main 8) | good (jets) |
| `enemy-c3301f6267` (main 11) | good (orbs) |
| `enemy-78c8f556f5` (main 19) | good (fire ball; same as Wolfang's) |
| `teleport-crystal`, `nightmare-virus`, `nightmare-virus-orb` | good; already in `sprites/` |
| bank sheets | bank 7 (robot, 105 sprites) and `effect-11-live-1` (drone bank, 170 sprites) are clean in both states. The state 2 skip "type 24: cells overwritten by type 24" is a false alarm: banks 7 and 8 share a texture page area, and both draw correctly |

## Open

See the sheet. Also: the effect 11 pieces export as separate IDs (their sprite source is live), so
the exporter makes duplicates of the type 24 table with half of it wrong.
