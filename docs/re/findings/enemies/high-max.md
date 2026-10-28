# High Max (boss overlay 44, main type 55, `var` 1): behaviour sheet

High Max is the boss of the hidden room behind gate 45 of the alternate Turtloid section when
`boss_choice` is 1. He is a large floating Reploid in a dark palette. He has no gravity: he hovers at
one height and bobs 4 px up and down. A **guard** stops almost all weapons. Only a few weapon ids
break the guard. Then he is stunned for a short time, and one damaging hit knocks him to the floor.
He attacks with volleys of aimed energy balls, a big energy ball that rolls on the floor, a barrier
rush and a shield ram. Below one third of his HP he loses the guard, stays in the room centre and
makes orbs that burst into homing pieces, without end.

Boss overlay `44` (`rock_bin/44.bin`, loaded at `0x800FA000` by `BossSpawner`; `docs/engine.md`,
"Boss overlay"), save state `turtloid-8_high-max.state.gz` (stage 21, area 1; Zero is the player; the
boss is in his first volley). Written with the RE kit (`python3 -m mmxdemake.rekit --name high-max
--state turtloid-8_high-max.state.gz --type 55`) and `docs/re-guide.md`. Also checked with
`sheldon-12_high-max` (stage 22, area 0; X; INTRO sub 3) and `scaravich-9_high-max` (stage 21, area
0; Zero; INTRO sub 11); see "The three hidden areas". The kit does not walk his
shots (visual type 21, entry `0x80100C60`); they were read from a kit of that type. Units, confidence
tags and the shared engine rules: `docs/engine.md`. The room: `docs/stages/turtloid.md`, "The same
room with High Max". "The room" below has its origin at the camera's minimum x and y (x 1264, y 704
in the save state), as in `docs/enemies/zero-nightmare.md`. The room is 352 px wide (the camera can
move 32 px), and the floor is at room y 192.

## The `var` parameter: three fights in one code [certain]

`BossSpawner` makes type 55 with `var` 1. The intro stage overlay holds the same code: after
relocation, every instruction of overlay 44's boss and shot code is also in the intro's type 55, in
the same order. The code tests `var` in many places:

| | `var` 0 | `var` 1 | `var` 2 |
|---|---|---|---|
| where | the intro stage (made by intro overlay code at `0x800F9A54`, at x 6162, y 346) | the hidden areas (this sheet) | not seen; probably stage 12 [guess: Gate's lab], see "Death" |
| HP maximum | 32 | 48 / 52 / 56 / 64 by boss level | as `var` 1 |
| damage table | `dmg_tbl_deflect` always: he **cannot be hurt** | guard rules (below) | guard rules |
| attacks (THINK) | patterns 0, 2, 3: dive sweep, volley, big ball | patterns 4, 5, 6 and the low-HP orbs | patterns 7–12 (steps 13–17, not read in full) |
| energy ball speed | 2 px/frame | 5 px/frame | 5 px/frame |
| end of the fight | after 900 updates, or when the player's HP is below 2: he flies away (step 9), `exit_req` = `0x40` | death | death |
| the player | cannot die: the boss and his shots keep the player's HP at 1 or more | normal | normal |
| text at the intro | message 4 | message 1 | message 0 |

`var` 0 also sets `game+0x26` = 1, uses texture `0x1E0`, and fixes the camera limits to x 6064, then
6064–6144. `var` 2 has two more objects (`var` 9 and 10, below), a counter for each at `+0x8D` and
`+0x8E` = {2, 3, 4, 5}[boss level], and a different intro. The rest of this sheet is about `var` 1
unless marked.

## The intro version (`var` 0) [certain unless marked]

The intro overlay has its own copy of the code (entry `IntroHighMax` `0x800F5170`, set-up
`0x800F1724`, main `0x800F1980`, animation table `0x800FB7B4` with the same 23 animations as
`0x80101EF0`). Zero (intro misc type 16) makes him at (6162, 346) after text 3
(`IntroHighMaxSpawn`, `0x800F9A54`; `docs/stages/intro.md`, "Events").

- **Set-up:** HP 0, contact 8, `dmg_tbl_deflect`, `game+0x26` = 1, texture `0x1E0`, fight timer
  `+0x90` = 0, priority 5, facing left, no terrain box, `PlayerEvent(20)`, animation 2, not drawn,
  step 2.
- **INTRO** (sub-steps `0x800FB8A4`; room origin at camera x 6064, y 224):
  1. Sub 1 (`0x800F205C`), when the player is in step 20: makes stage logic 24 (`BossWarning`,
     which loads the High Max pack, member 124; `docs/engine.md`, "Boss entry and exit"); camera
     `+0x48` = 1 (old value saved at `0x800FBC7C`); both target x limits 6064; he is put at room
     (240, −64); when the player is outside room x 32–64, it walks there (step 29).
  2. Sub 2 (`0x800F21B0`): the walk stops at room x ≤ 72 (walking left) or ≥ 24 (walking right)
     with `PlayerEvent(20, 0x40)`. When object 24 is gone: `game+0x24` = 1, `game+0x20` = him,
     `boss_hp_max` = 32, `BossLevel`, y speed 1 px/frame down; he is drawn.
  3. Sub 3 (`0x800F22E8`): he comes down at 1 px/frame. Below room y 96 (about 161 updates): the
     floor effect (visual 21 `var` 3) and `TextOpen(4)` (`0x800F23DC`).
  4. Sub 4: when the text is closed, `BossMusicRequest`.
  5. Sub 5: when `BossMusicWait` is 0 (XA track 23): camera x limits 6064–6144; camera `+0x48` gets
     its old value back.
  6. Sub 6: HP + 1 per update to 32, a tick every 2 updates. Then `PlayerEventEnd`, x speed 2
     px/frame right, y speed 1 px/frame down, gravity 1/16 up.
  7. Sub 11: he rises; at room y 64, THINK.
- **The fight timer and the HP floor** (main, `0x800F1DC8`): `+0x90` counts every update except in
  INTRO. Player HP `0x80` (dead) → 1. Player HP below 2 → `PlayerEvent(20)`, HP = 1, step 9. `+0x90`
  ≥ 901 → `PlayerEvent(20)`, step 9. His shots apply the same rule (`0x800F505C`): a shot that
  touches the player when the HP is below 2 holds the player, sets HP 1, plays sound 20 and deletes
  itself. So the player cannot lose this fight, and cannot win it.
- **THINK** (`0x800F2584`): r = `GetRNG() & 15`. r < 2: pattern 0, DIVE SWEEP (2/16); r < 7:
  pattern 2, DIVE SWEEP then VOLLEY (5/16); else pattern 3, BIG BALL (9/16). Pattern table
  `0x800FB810`. In 900 updates he does about 2–3 patterns [likely].
- Other differences from `var` 1: always `dmg_tbl_deflect` (no guard, stun, knockdown or orbs),
  energy balls 2 px/frame, the big ball copies palettes 84 → 88 (`0x7944` → `0x7948`), DIVE SWEEP
  (below), INTRO END (below; `TextOpen(5)` at `0x800F301C`, `TextOpen(6)` at `0x800F657C`).
- **Sprites:** the intro's sprite data (banks 13 and 14: 49 body and 67 shot sprites) and its
  animations are the same bytes as in the hidden areas. The texture comes from DAT member 124,
  loaded at the scene: its texture, palette and sound entries are the same bytes as member 139 (the
  hidden-area pack) and member 136. Only the VRAM place differs: entry type `0x10412` at (576, 16),
  texture `0x1E0`, against `0x1040a` at (512, 16), texture `0x140`. Palette `0x7944` has the same
  colours as `0x7948`. Drawn with member 124 put into the `intro-3_boss` VRAM, all 116 sprites are
  pixel-identical to `turtloid-8` [certain]. The live save state `intro-6_high-max-text` (text 4
  open, member 124 in VRAM) gives the same result: all 116 sprites drawn from its own VRAM are
  pixel-identical [certain]. So the hidden-area export also serves the intro.

## Spawn and set-up (`0x800FA04C`) [certain]

- **HP: {48, 52, 56, 64}[boss level].** The intro writes `boss_hp_max` (`game+0x8C`) from the boss
  level (`game+0x8F`) and then calls `BossLevel`. The save state has level 0 and 48 HP. The HP
  starts at 0 and fills in the intro.
- **Contact damage 8** in all poses.
- Body boxes: attack = hurt = (−20, −28, 40, 72): 40 × 72 px, from 28 px above his origin to 44 px
  below it. No terrain box, except when he falls in KNOCKDOWN: (0, 16, 32, 40), a 64 × 80 px body,
  so his origin is 56 px above the floor when he lies there.
- Damage table: the guard table `0x8007523C` (below). Draw priority 5. Animation table `0x80101EF0`.
  Own sprite source (texture `0x140`, palette `0x7948`, sprites from the first entry of his bank;
  `docs/engine.md`). His shots take the **second** entry of the bank (another sprite set).
- Speeds and gravity 0. Facing left. `PlayerEvent(20, <the player's facing>)`: the player is held.
- The bob index `+0x8C` = 8 (`0x800FA244`).
- Then state 1, step 2 (INTRO).

## The three hidden areas [certain]

The stage overlays do not hold the code of type 55. Their main tables point into overlay 44: type
53 `0x800FC128`, type 55 `0x800FDA98`, type 63 `0x800FB214`.

| | stage 21, area 0 (Scaravich) | stage 21, area 1 (Turtloid) | stage 22, area 0 (Sheldon) |
|---|---|---|---|
| stage overlay, member | 29, 119 | 30, 120 | 31, 121 |
| main table | `0x800EBE3C` | `0x800EF20C` | `0x800F2828` |
| save state | `scaravich-9_high-max` | `turtloid-8_high-max` | `sheldon-12_high-max` |
| room (camera minimum, limits) | (912, 224), x 912–944 | (1264, 704), x 1264–1296 | (2576, 224), x 2576–2608 |
| `BossSpawner` record | (992, 368): minimum + (80, 144) | (1360, 864): minimum + (96, 160) | (2640, 384): minimum + (64, 160) |
| boss in the save state | INTRO sub 11, room (288, 97), HP 48 | VOLLEY (pattern 6), room (288, 62), HP 48 | INTRO sub 3, room (240, 41), HP 0 |

- The 33304 bytes of overlay 44 in RAM are the same in the three states. One byte differs from the
  disc: `0x80102214` (the saved camera `+0x48`) = 8.
- The room size and the room points are the same, so the fight is the same [likely].
- The 116 sprites are pixel-identical in the three states with palettes `0x7948` and `0x794C`.
- `scaravich-9`: boss level 0 (Zero reads `game+0x8E` = 5), `+0x84` = 0 (THINK has not run yet).

## Sprites and palettes [certain]

- **Pack.** `BossPackLoad` (`0x80052998`) writes 138 / 139 / 140 for `boss_choice` 0 / 1 / 2 into
  `boss_pack_tbl[stage]` for stages ≥ 19. The value stays in RAM. High Max is member **139**. Member
  136 (`boss_pack_tbl[17]`) has the same bytes (the `var` 2 fight of stage 17,
  `docs/stages/gate-2.md`). Member 124 (the
  intro) has the same entries with texture entry type `0x10412`.
- **Texture.** Entry `0x1040A` (131072 bytes) fills 6.4 columns of 64 words × 160 rows from VRAM
  (512, 16): x 512–895, rows 16–175, and x 896–959, rows 16–79. Texture `0x140` is cell `0x810` of
  the mode `0x40` remap. No byte differs in the three states.
- **Palettes.** `BossSpawner` copies 8 palettes to slots 88–95 (`0x7948`–`0x794F`) from
  `*(0x1F800030)` + 0 / 256 / 384 by `boss_choice`. `0x1F800030` points to entry `0xD` of the stage
  member. High Max takes bytes 256–511, which are the same in the 8 hidden-area members (115–122).
  Member 117's entry `0xD` is 512 bytes, so the Dynamo copy (+384) reads past its end.
- **Sprite data.** Entry `0xA` of the stage member holds the body set (49 sprites) and the shot set
  (67 sprites). They are the same bytes in the three areas.

## State machine

States (table `0x80101F80`): 0 set-up, 1 main (`0x800FA2A8`), 2 death (`0x800FDAF4`). The entry runs
the state and then `CollideCheck`.

### Main (state 1) [certain]

1. `ContactWeapon`. A kill: state 2 (death).
2. The hit rules (next section) can change the step and the damage table.
3. The step (table `0x80101F8C`): 0 engine hit reaction, 1 nothing, 2 INTRO, 3 DIVE SWEEP, 4 HOVER
   (no pattern uses it), 5 VOLLEY, 6 BIG BALL, 7 THINK, 8 KNOCKDOWN, 9 INTRO END (`var` 0),
   10 BARRIER RUSH, 11 SHIELD RAM, 12 LOW-HP ORBS, 13–17 (`var` 2 only).
4. `ContactMega`.
5. `var` 0 only: the fight timer and the player's HP floor (table above).
6. While the timer `+0x89` is not 0: `dmg_tbl_deflect`, and the palette flash toggles every 4
   updates. At its end: the guard table again.
7. `DisplayCheck(128, 128)`, except in INTRO.

"Bob" below means the hover bob: on every even frame, y changes by the next value of the table
`0x8010216C` (0, 1, 0, 1, 1, 0, 1, 0, 0, −1, 0, −1, −1, 0, −1, 0), at the index `+0x8C` (4 bits;
`0x80100BE8`). The set-up writes 8, so he first moves 4 px up and then back down, in 32 updates.
The hover height is the low point of the bob. Most waiting steps bob.

### The guard, the stun and the damage [certain for the code; the weapon owners: below]

`ContactWeapon` gives 0 (no hit), 127 (blocked, which includes "deflected") or the weapon id + 1.

| when | which hits count | what happens |
|---|---|---|
| guard (all steps except 8 and 12) | ids 18, 19, 20, 33–36, 42, 43, 44, 46, 56 (the guard table gives them 0 damage; all other ids are deflected) | **Guard break:** sounds 61 and 20; he keeps the step and sub-step in `+0x86`/`+0x87`; step 8, sub 0 (STUN); damage table = the open table |
| STUN (step 8, sub 0) | Zero: any damaging hit. X: ids 1, 3–8, 10–17, 45, 52, 53; with `mega+0xE7` = 2, ids 46, 50, 51 | The hit does its damage. Then sound 61, animation 4 (hurt), `dmg_tbl_deflect`, sub 1 (he is knocked down) |
| KNOCKDOWN subs 1–6, BARRIER RUSH (from the barrier to the return), SHIELD RAM subs 0–5, the 60 updates after a knockdown | none | all deflected |
| LOW-HP ORBS (step 12) | any damaging hit (open table) | 60 updates of `dmg_tbl_deflect` with a flash every 4 updates (2 updates after ids 24 and 25, the first sabre hits). Then the open table again |

The open table is `0x800752DC`, or `0x8007537C` when `mega+0xE7` = 2. `mega+0xE7` is the player
form (`docs/engine.md`, "The player"): 2 is X in the Shadow Armor; Zero is 5 [certain]. The owners
of the ids are in "Weapon ids of the guard break and the stun" (below). Because the guard table has class 0 with
damage 0, a guard break does not hurt him. So above one third of his HP, **each guard break gives
one damaging hit**.

STUN (sub 0): he does not move and his animation stops. The palette flash toggles every 8 updates. A
counter runs for **121 updates**. If no damaging hit comes, the guard table comes back and he goes on
with the step and sub-step that the guard break stopped. (Table `0x800FA004` holds the highest
sub-step for each step; a higher saved sub-step goes to THINK. With the values in the table this
does not occur [likely].)

### KNOCKDOWN (step 8, subs 1–6, `0x80102058`) [certain]

| sub | what |
|---|---|
| 1 | Animation 4 (hurt). x speed 1.5 px/frame **backwards** (opposite to his facing), y speed 1 px/frame down, gravity 1/8 px/frame² down (maximum fall speed 6.5 px/frame). |
| 2 | He falls with a terrain box. On the floor: `Rumble(3)`, sounds 27 and 33, `Quake(90, 2, 2)`. |
| 3 | He lies on the floor for **90 updates**. Then animation 5 (he gets up). |
| 4 | At the animation's flag 1 (update 43) he rises at **5 px/frame**, without a terrain box. Above room y −64 (out of view): the **re-place** (below), then 50 updates. |
| 5 | Then he comes down at **4 px/frame**. |
| 6 | At room y 64: 60 updates of `+0x89` (all hits deflected, flash), and the next step of his pattern. |

He flashes for all of subs 1–6. The flash interval in sub 1 is 30 updates for Zero and 100 for X;
this changes only the flash.

**Re-place** (used by KNOCKDOWN, DIVE SWEEP and BARRIER RUSH): when he faces left, he comes back at
room x 64 and faces right; when he faces right, at room x 288 and faces left. So he comes back on the
side that he faced, and he faces into the room.

### INTRO (step 2, sub-steps `0x80101FE0`) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800FDCE4` | Nothing; next sub. |
| 1 | `0x800FA984` | Waits until the player is in step 20 (held). Makes `BossWarning` (stage logic 24, `docs/engine.md`, "Boss entry and exit"); its pointer goes to `+0x80`. Camera field `+0x48` is set to 1 (the old value is kept at `0x80102214`). His position: x = `cam.target_min_x` + 240, y = `cam.target_min_y` − 64 (room (240, −64)), above the view. When the player is not between room x 32 and 64, `PlayerEvent(29, …)` walks the player (facing left above room x 48, else right). |
| 2 | `0x800FAAD8` | The walk stops at room x ≤ 72 (walking left) or ≥ 24 (walking right): `PlayerEvent(20, 0x40)`. When byte 0 of the `+0x80` object is 0 (the warning is gone): `game+0x24` = 1, `game+0x25` = 0, `game+0x20` = this object, `boss_hp_max`, `BossLevel`. y speed 1 px/frame down. He is drawn from now on. |
| 3 | `0x800FAC10` | He comes down at **1 px/frame**. At y > `cam.target_min_y` + 96 (room y 97, after 161 updates): a floor effect (`var` 3) and `TextOpen(1, 255, game+0x2E)`, `game+0x2E` = 1. He stops. When `game+0x2E` is already 1 (a second intro in the same visit), `TextOpen` makes a 60-update wait and shows no text [certain for the code]. |
| 4 | `0x800FDCF8` | Bob. When the text is closed: `BossMusicRequest`. |
| 5 | `0x800FDD48` | Waits while `BossMusicWait` (the boss music wait). The camera field `+0x48` gets its old value. |
| 6 | `0x800FDDC4` | **+1 HP per update**, a tick (bank 0, id 14) every 2 updates, up to `boss_hp_max`. Then `PlayerEventEnd`, x speed **2 px/frame** to the right, y speed 1 px/frame down, gravity 1/16 px/frame² **up** (`var` 2: subs 7–10 come first). |
| 11 | `0x800FE0F0` | He dips and rises (no bob). x ≤ `cam.max_x` + 256 (room x 288). At y < `cam.min_y` + 64: THINK. |

Subs 7–10 are for `var` 2 only: a charge, two satellite objects (`var` 9 and 10), a 60-update wait.

The save states agree. `scaravich-9`: sub 11, room (288, 97), y speed 1.0625 px/frame up (−1 + 33 ×
1/16: 33 updates, which sum to 0 px); THINK comes at update 53 (room y 63.875). `turtloid-8`: room
(288, 62), y speed 2.25 px/frame (52 updates). 52 updates from room y 97 end at 66.125, so there sub
11 started at room y 93, 4 px higher [likely; the cause is not known]. `sheldon-12`: sub 3, room
(240, 41), HP 0.

### THINK (step 7, `0x800FAEAC`) [certain]

THINK has no wait. `r` = `GetRNG() & 15`. "One third" = `boss_hp_max` / 3 (16 at level 0).

| HP | pattern |
|---|---|
| ≥ two thirds (≥ 32 at level 0) | r < 8: 4; else 6 |
| ≥ one third (16–31) | r < 5: 4; r < 11: 5; else 6 |
| < one third (< 16) | no pattern: `dmg_tbl_deflect`, a 60-update timer, step 12 (LOW-HP ORBS) |

A pattern is a list of steps (`0x80101F4C`, 4 bytes each). Each step ends with the next entry
(`+0x85`); the last entry is THINK.

| pattern | steps |
|---|---|
| 4 | VOLLEY, BARRIER RUSH |
| 5 | BIG BALL |
| 6 | VOLLEY, SHIELD RAM |
| 0, 2, 3 (`var` 0) | DIVE SWEEP; DIVE SWEEP, VOLLEY; BIG BALL |

`var` 0 THINK: r < 2: pattern 0; r < 7: 2; else 3. The difficulty and the player's position are not
tested. Patterns 1 and 7–12 are not used by `var` 1.

The save state agrees: HP 48 (full), `+0x84` = 6 (pattern 6), `+0x85` = 0, step 5 (VOLLEY).

### VOLLEY (step 5, `0x80102038`) [certain]

1. Animation 6, sound 34. He stays where he is and bobs only at the end.
2. At each flag 8 of the animation: an **energy ball** (`var` 0, below) and sound 28. 12 balls in 3
   groups of 4, 15 updates apart: at about updates 20, 35, 50, 65 (one hand: 56 px in front, 8 px up),
   98, 113, 128, 143 (the other hand: 44 px in front, 8 px up) and 176, 191, 206, 221 (first hand
   again).
3. At flag 1 (update 243): the animation goes on with a charge loop. 41 updates of bob. The next step.

So one VOLLEY is about 285 updates. The save state agrees: 4 balls 74 px apart (15 updates at
5 px/frame), and the animation is 2 updates after the fourth shot.

### BIG BALL (step 6, `0x80102044`) [certain]

1. Sound 35, animation 7 (charge). At update 10: `Rumble(7)`, sound 29.
2. At update 41: animation 8 (release). At its flag 1 (4 updates): sound 30 and a **big ball**
   (`var` 1, below) at his origin.
3. Bob. At update 121 of this sub: sound 36. He waits **until the big ball is gone** (it sets his
   `+0x07` = 1 when it ends).
4. Animation 9. 40 updates of bob. The next step.

### BARRIER RUSH (step 10, `0x8010208C`) [certain]

The rush runs {1, 2, 3, 3}[boss level] times (1 at level 0).

| sub | what |
|---|---|
| 0–1 | Animation 7 (charge). At update 10: `Rumble(7)`, sound 29. 61 updates. Animation 8. |
| 2 | At its flag 1: sound 123, the **barrier aura** (`var` 4 and 5), `dmg_tbl_deflect`. |
| 3 | 91 updates of bob. Animation 9. |
| 4 | At its flag 1 (7 updates): y speed 4 px/frame down. |
| 5 | At room y 112 he stops. |
| 6 | **50 updates** of bob, low in the room. |
| 7 | He rushes: **6 px/frame** in his facing direction, + 1/256 px/frame² (about 6.2 px/frame at the end). Past room x 48 (to the left) or 304 (to the right): y speed 2 px/frame up, + 0.375 px/frame² up; x speed 1 px/frame in the same direction. |
| 8 | Above room y −64: the re-place. |
| 9 | 50 updates out of view. Then the guard table again, and y speed **6 px/frame** down. |
| 10 | At room y 64: if more rushes remain, sub 1 (from the other side); else the next step. |

During the rush his body (contact 8) is the only damaging part; the aura has no boxes. At room y 112
his body box covers room y 84–156. A standing X has a hurt box from 38 px above the floor (room y
154; `docs/enemies/zero-nightmare.md`), so the rush touches a standing X by only 2 px and hits a
jumping player [likely; check in play].

### SHIELD RAM (step 11, `0x801020B8`) [certain]

`+0x89` is kept at 120 in subs 0–5, so he cannot be hit and he flashes.

| sub | what |
|---|---|
| 0 | When he and the player are on the same half of the room (centre room x 176), he flies at **6 px/frame** to the edge of the other half: room x 64 when they are on the right, room x 288 when they are on the left (sub 1). Else he does not move: sound 35, animation 18, sub 2. |
| 1 | He turns to face the room centre at room x 112 / 240. At room x 64 / 288 he stops. Sound 35, animation 18. |
| 2 | At the flag 1 of animation 18 (update 11): the **energy shield** (`var` 6, below). |
| 3 | 60 updates of bob. Then y speed 4 px/frame down. |
| 4 | At room y 116 he stops. x speed **6 px/frame** in his facing direction, + 1/16 px/frame². |
| 5 | The **ram**. After 6 updates: animation 21. At room x 64 / 288 (the other edge): `Quake2(40, 2, 2)`, sound 27. The ram takes about 28 updates and ends at about 7.8 px/frame. At room y 116 his body box reaches room y 160, 6 px into a standing X's hurt box. |
| 6 | 41 updates of bob. Then he can be hit again (`+0x89` = 0, the guard table). |
| 7 | 31 updates of bob. Animation 19. |
| 8 | At its flag 1 (8 updates): y speed 2 px/frame up; he faces the room centre. |
| 9 | At room y 64: the next step. |

The shield ends at sub 6. So the ram is a 6–8 px/frame charge with an 80 × 80 px shield in front of
him (contact 12) and his body behind it (contact 8).

### LOW-HP ORBS (step 12, `0x801020E0`) [certain for the code]

1. Animation 2. x speed **4 px/frame** toward the room centre (room x 176).
2. At the centre: sound 35, animation 7 (charge). He stays there, and he bobs.
3. 10 updates: `Rumble(7)`, sound 29.
4. Every **127 updates** (61 + 66): sounds 61 and 35 and an **orb** (`var` 7, below); 66 updates
   later sound 29.

This step has no end. He stays in it until he dies. He has no guard in this step (see the hit rules).
A KNOCKDOWN cannot start here.

### DIVE SWEEP (step 3, `var` 0 only, `0x80102010`) [certain]

y speed 4 px/frame down to room y 112. 50 updates of bob. Then a sweep at 4 px/frame (+ 1/256
px/frame²) in his facing direction to room x 48 / 304 (no aura), a rise at 2 px/frame (+ 0.375
px/frame² up), the same re-place as BARRIER RUSH, 50 updates out of view, and a descent at **1
px/frame** to room y 64. About 330 updates in all [likely]. Intro copy: `0x800F5834`.

### INTRO END (step 9, `var` 0 only, `0x80102074`) [certain]

He holds the player (step 20), turns off his boxes and flies at 1 px/frame to room (256, 96), where
he makes the floor effect (`var` 3). The player is walked (`PlayerEvent(29, …)`) to room x 80–112.
`mega+0x7A` = 1 while this runs. The HP bar goes off (`game+0x24` = 0, `game+0x20` = 0). Text 5. He
rises at 2 px/frame. Above room y −96 (about 97 updates): text 6, he is not drawn. When the text is
closed: `exit_req` = `0x40` and he is deleted. In the intro this ends the stage: `PlayerExit` sets
`mega.state` = 3 (no beam-out), the fade-out runs, and `StageExit` (stage 0 branch, `0x8001E38C`)
sets `game+0x1E` = 0, `game+0x66` = 1 and game mode 11 (the report screen, stage 15, overlay 20).
Intro copy: `0x800F64C0` (sub-steps `0x800FB938`).

### Death (state 2, steps in `0x80101FD4`) [certain]

1. At the kill: `StopSound(255, 0)` (`0x8001750C`: stop all sounds [likely]), sounds 20, 61, 33 (bank 2),
   `mega+0x7A` = 1. All his shots end (they test his state).
2. Animation 4 (hurt), `PlayerEvent(20, <the player's facing>)`, palette flash off.
3. **127 updates.** The flash toggles after 25, 20, 15, 10, then every 5 updates. Then
   `BossExplosion` (stage logic 26) at his position.
4. While `BossExplosion` runs, he flashes every 6 updates. When it is gone: in stage 12, `exit_req` =
   `0x80`, `game[0x2C + checkpoint]` = 1 and `checkpoint` += 9; else `exit_req` = **`0x10`**. Then
   `game+0x24` = 0, `game+0x20` = 0, the camera's target minimum y = the camera y, and he is deleted.

He writes **no** `boss_choice`, `game+0x5F` or other progress byte. The change of `boss_choice` from
1 to 2 is in overlay 20 (the report screen), after the clear (below). He leaves no Nightmare soul.

### Where the clear leads (all three hidden-area bosses) [certain for the code]

Zero Nightmare, High Max `var` 1 and Dynamo all end with `game.exit_req` = `0x10`. Then the
stage-play mode waits until `fade_dir` (`0x80097424`) is 0 (`0x8001EB48`) and calls `StageExit`
(`0x8001E32C`). For a stage of 19 or more, `StageExit` tests only that the exit request is above 0,
and then `boss_choice`:

| `boss_choice` | `StageExit` | next |
|---|---|---|
| 0 (Zero Nightmare) | `game+0x5F` \|= `0x10` (Zero can be played); game mode 19, scene 2 | scene 2: stage 11 with overlay 12, and scene overlay 48 at `0x800FA000` (scene n loads overlay 46 + n, table `0x8006DC50`). At its end: scene 5 when `game+0x66` < 3 and a character has 3000 souls or more; else game mode 11, the report screen |
| 1 (High Max) | game mode 19, scene 4 | scene 4 (overlay 50). At its end: game mode 11, the report screen |
| 2 or more (Dynamo) | game mode 11 | the report screen |

- The player does **not** go back to the stage or to the area that the hidden area came from.
- Game mode 11 (`0x8001E81C`) clears `game+0x26`…`+0x35`, sets stage 15 area 0 (overlay 20, the
  report screen; `report-1` and `report-2_shark` have overlay 20) and game mode 12. `StageExit`
  keeps the stage number at `0x800E3141` (and 1 at `0x800E3140` when the exit request was 0 or
  less); the report set-up (`0x800EEA1C`) copies them to `game+0x26` and `+0x27`.
- The report builds a scene list at `game+0x158` (`0x800EE6F4`). When the cleared stage is 19 or
  more (`+0x27` = 0) and `boss_choice` is 1, it writes **`boss_choice` = 2** and adds scene 6 (X)
  or 7 (Zero) when `game+0x66` < 3. The same scene is added when a stage 1–8 clear makes
  `game+0x60` = `0xFF` (all 8 Mavericks; `mega+0xC9` not yet `0xFF`) or when a character has 3000
  souls or more (`docs/engine.md` calls this scene "Gate's lab opens"). The list ends with `0xFF`.
- At the end of the report (`0x800EFC1C`, `0x800F08FC`): the first scene of the list (game mode
  19), or game mode 3 when the list is empty. After each scene, game mode 21 (`0x8001EDDC`) goes on
  to the next scene of the list; after the last one, game mode 3. Game mode 3 (`0x8001E8CC`) sets
  stage 13 area 0 (overlay 16, the stage select) and game mode 4.
- The game modes are the table `0x8007112C` (indexed by game byte 0): 0 new game (`0x8001E088`),
  3, 8 stage load (`0x8001E914`), 10 stage play (`0x8001E9EC`; its sub-modes `0x80071184`: play,
  then the exit), 11, 19 scene start (`0x8001ED88`: stage 11, `game+0x1D` = the scene), 21.

So the order after a hidden-area boss is: (scene 2 or 4) → report screen → (scene 6 or 7, when
Gate's lab opens now) → stage select. When the player dies in a hidden area (exit request 0),
`StageExit` uses the normal lives path.

### Weapon ids of the guard break and the stun [certain for the owners; names likely]

The weapon type table `0x80071220` (indexed by weapon id) points into the main program or into the
player overlay at `0x801EA000`. X's overlay (`rock_bin/00.bin`) and Zero's overlay (`01.bin`) use
the same addresses. Each overlay entry is a function start in only one of the two overlays, so
each such id belongs to one character:

| owner | weapon ids |
|---|---|
| X (overlay 00) | 0, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17–23, 32, 43, 45, 46, 47, 49, 50, 51, 52, 56, 57, 59, 60, 63–78 |
| Zero (overlay 01) | 24–31, 33, 34, 35–42, 44, 48, 58 |
| both (main program) | 1, 4, 7, 10, 13, 16, 53, 54, 55, 61, 62, 79–82 |

Ids 24–31, 35–42 and 48 run one routine (`0x801ED2A8`): Zero's sabre hit objects. Id 44 is made by
Zero's special attack (step 59, button `0x40`, skill bit `0x80`): entry 4 (`0x801ED198`) of its
sub-step table `0x801EE39C` makes a row of id-44 objects every 10 updates (`0x801EAA24`). Id 58 is
made by Zero's step 62 (the special) and gets the Buster Plus bonus: the Z-Buster shot [likely].
Ids 1, 10, 54 (`0x800434E0`) and 7, 16, 53 (`0x80045D68`, `0x80045E58`) share routines.

The guard-break ids, by owner:

| ids | owner | weapon |
|---|---|---|
| 18, 19, 20 | X | full charge: forms 0 and 2, Falcon, Ultimate (`docs/players/attacks.md`) |
| 43 | X | Blade Armor full charge |
| 46 | X | Shadow Armor charged buster; also made by the Shadow pose step 38 |
| 56 | X | the weapon object of pose step 76 (shot type 56, `docs/players/steps-extra.md`) |
| 33 | Zero | Hyoroga (step 31 / 63) |
| 34 | Zero | Shoenzan, the fire wave (step 55) |
| 35 | Zero | Rakukojin (step 56) |
| 36 | Zero | Sentsuizan (step 57) |
| 42 | Zero | Ensuizan (step 58) |
| 44 | Zero | Rekkoha, the special attack (step 59) |

The Zero technique names and steps are from `docs/players/zero.md` (Zero agent) [likely for the
names]. So X breaks the guard only with full charges, the Shadow charged buster and id 56. Zero
breaks it only with six techniques, not with the plain sabre (24–31).

The X ids of the STUN hit: 1, 3–8, 10–17 (the special weapons of slots 1 and 3–8, uncharged and
charged: Yammar Option, Magma Blade, Metal Anchor, Ground Dash, Meteor Rain, Guard Shell, Ray
Arrow; slot names from `docs/players/weapons.md` [likely]; ids 1/10, 4/13, 7/16 are main-program
routines, the others X's), 45 (Shadow Armor buster, plain), 52 (the Ice Burst shards, the same
routine as id 2) and 53 (the Guard Shell shards). So slot 2 (Ice Burst, ids 2 and 11) cannot
knock him down. In the Shadow Armor (form 2) also 46, 50 and 51 (X's saber slash: 50 on the floor,
51 in the air or on a wall; `docs/enemies/zero-nightmare.md`).

## Animations (table `0x80101EF0`, 23 animations) [certain for the frames]

"Body" animations use his own sprite set (49 sprites); "shot" animations use the second set, which
his shots carry.

| # | set | what | used by |
|---|---|---|---|
| 0 | body | stand, slow breathing (loops, 120 updates) | not set by this code |
| 1 | body | stand → float (the legs fold into one pod) | not set by this code |
| 2 | body | float pose (one frame) | set-up, LOW-HP ORBS, INTRO END |
| 3 | body | float → stand | not set by this code |
| 4 | body | hurt (one pose) | STUN hit, death |
| 5 | body | gets up from the floor and folds into the float pose; flag 1 at update 43 | KNOCKDOWN |
| 6 | body | the volley: the hands fire in turn (12 × flag 8), then flag 1 and a charge loop | VOLLEY |
| 7 | body | charge: a blue ball forms between the hands (loops) | BIG BALL, BARRIER RUSH, LOW-HP ORBS |
| 8 | body | release, a flash from the hand; flag 1 at update 4 | BIG BALL, BARRIER RUSH |
| 9 | body | back to the float pose; flag 1 at update 7 | BIG BALL, BARRIER RUSH |
| 10 | shot | energy ball (loops) | `var` 0 |
| 11 | shot | – | not set by this code |
| 12 | shot | big ball / orb (loops) | `var` 1, 7, 11 |
| 13 | shot | piece | `var` 2, 8 |
| 14 | shot | floor effect (loops) | `var` 3 |
| 15 | shot | barrier aura, front (sprites 36–47 with sprite 60 between them) | `var` 4 |
| 16 | shot | barrier aura, back (sprites 48–59 with sprite 60) | `var` 5 |
| 17 | shot | satellite | `var` 9, 10 (`var` 2) |
| 18 | body | one arm goes forward; flag 1 at update 11 | SHIELD RAM |
| 19 | body | the arm comes back; flag 1 at update 8 | SHIELD RAM, `var` 2 |
| 20 | shot | energy shield: 54 updates of forming, then the active loop (flag 1) | `var` 6 |
| 21 | body | ram pose (flags 1, 2) | SHIELD RAM |
| 22 | body | a pose for `var` 2 (step 17) | `var` 2 |

The export `~/mmx-data/x6/sprites/enemies/high-max/` (ID `enemy-1b0293f642`) has all 23
animations (cell 136 × 137, origin (65, 63)), from `turtloid-8`, `sheldon-12`, `scaravich-9` and
`intro-6_high-max-text`; each state gives the same PNG files [certain]. It uses the `sets` rule of
`catalog/enemies.json` (stages 0, 21 and 22, type 55, set 1, animations 10–17 and 20, `"texture":
null`, `"palette": null`). The shots copy the boss's own palette when they are made (`PaletteCopy`),
so their palette is the main set's (`0x7948` in stage 21), not a fixed bank + n. Animation 11 has no
user; it is in set 1 because it uses aura sprites [likely].

The old export (ID `enemy-b15382edbf`, animations 0–14, with 10–14 drawn from the wrong set) is kept
in `~/mmx-data/x6/sprites/enemies/enemy-b15382edbf/`; it is not used any more. The exporter files
stage 21 area 0 under the stage name `turtloid-alt`, because the `stages` map of
`catalog/enemies.json` has no area.

## Shots and effects (visual object type 21)

Each copies his texture and animation table and takes the **second** sprite source of his bank
(`docs/engine.md`, "copies the parent"). The palette is `0x7948`, or `0x794C` for `var` 1, 6, 7 and 11:
when such a shot is made, it copies `0x7948`–`0x794B` to `0x794C`–`0x794F` (`PaletteCopy(92,
palette_buffer + 88 × 32, 4)`, `0x800FC61C`, `0x800FC7DC`). So it starts with his colours, and the
31-step `PaletteBlend` changes only the shot. Set-up `0x800FC21C` sets the step from `var`; the main routine
(`0x800FD86C`) runs the step table `0x801021E0`. All of them end when he dies (sound 20). `var` 6
(and `var` 9, 10) also end when he is in KNOCKDOWN sub 1 or later. All except `var` 3, 4 and 5 run `ContactMega`, and all end more
than 96 × 64 px out of view. Positions are from his origin; "in front" = in his facing direction.

| `var` | object | boxes, HP, damage table, contact | movement and end |
|---|---|---|---|
| 0 | energy ball (VOLLEY) | attack = hurt (−8, −8, 16, 16); terrain 16 × 16; 1 HP, `dmg_tbl_one_hit`; contact **6** | Aimed at the player when it is made, then straight at **5 px/frame** (`var` 0 boss: 2 px/frame). Ends on a wall or floor (hit effect) or when a weapon hits it. It does not end when it touches the player [likely]. |
| 1 | big ball (BIG BALL) | attack = hurt (−40, −40, 80, 80); terrain box half sizes 48; **8 HP**, table `0x800754BC`; contact **12**; sound 31 | Falls at **3 px/frame**. On the floor it rolls at **3 px/frame** in his facing direction. At the wall it rises at 3 px/frame and leaves the view. A hit stops it for 50 updates. **Killed:** sounds 20 and 124, 4 pieces (`var` 8), then a 31-update palette blend and the end. When it ends, his `+0x07` = 1. |
| 3 | floor effect (INTRO, INTRO END) | none | 8 px right of him, on the floor (room y 192); sound 26. Ends when he leaves (INTRO sub 11, INTRO END sub 4). |
| 4, 5 | barrier aura, front and back (BARRIER RUSH) | none | Stays on his origin. Ends when he is back in view after the last rise (step 10, sub 9). |
| 6 | energy shield (SHIELD RAM) | attack (−8, −8, 16, 16), then (−40, −40, 80, 80) when its animation reaches flag 1 (54 updates); no hurt box; contact **12**; sound 31 | 24 px above him while his animation flag is 1, else 48 px in front and 8 px up. Ends with a 31-update palette blend when the ram hits the wall (sub 6). |
| 7 | orb (LOW-HP ORBS) | hurt (−40, −40, 80, 80), `dmg_tbl_capsule`; no attack box; sound 31 | At his origin. After **61 updates**: sounds 20 and 124, **4 pieces** (`var` 8; **6** when the difficulty is 2 or more), then a 31-update palette blend and the end. |
| 8 | piece (from `var` 1 and 7) | attack = hurt (−8, −8, 16, 16); `dmg_tbl_deflect` (it cannot be destroyed); contact **12** | Piece k (k = 0, 1, …) starts at angle 90° × k + a random offset of 0–45° (`GetRNG() & 0x2D`, the same for all pieces of one burst). It flies at **3 px/frame** for 24 updates, then stops. After 40 × (k + 1) updates it aims at the player and flies at **5 px/frame** (sound 125). So the pieces leave one by one, 40 updates apart. With 6 pieces, pieces 4 and 5 use the directions of pieces 0 and 1. |
| 2 | piece with a 20-update flight | as `var` 8, with no burst | Not made by any code read here. |
| 9, 10, 11 | satellites and an orb of `var` 2 | 9, 10: (−24, −56, 48, 112), 32 HP, table `0x8007541C`, contact 12 | `var` 2 only; not read in full. |

The palette blend (`0x8002B54C`, 31 steps) also sets `bg_refresh`, so it probably changes the
background palette too: a screen flash [guess].

## Damage

`D` = deflected, `–` = no effect, a number = damage (`docs/engine.md`, "The damage tables").

Guard table `0x8007523C` (named `dmg_tbl_high_max_guard` here); a `0` is a hit with no damage that
breaks the guard:

| ids | damage |
|---|---|
| 0–15 | D D D D D D D – D D D D D D D D |
| 16–31 | D D 0 0 0 D D D D D D D D D D D |
| 32–47 | D 0 0 0 0 D D D D D 0 0 0 D 0 D |
| 48–63 | D D D D D D D D 0 D D D D D D D |
| 64–79 | D × 16 |

Open table `0x800752DC` (`dmg_tbl_high_max_open`), in the STUN and in LOW-HP ORBS:

| ids | damage |
|---|---|
| 0–15 | D 3 D 3 3 3 3 – 3 D 5 5 5 5 5 5 |
| 16–31 | 5 5 D D D D 8 D 3 3 3 3 3 3 2 3 |
| 32–47 | 2 3 D 4 3 3 3 3 3 3 3 D D D D D |
| 48–63 | 4 D D D 3 4 D – D D 8 D D D D 0 |
| 64–79 | D × 16 |

Open table for `mega+0xE7` = 2, `0x8007537C`: all D (id 7 –) except **46: 6, 50: 3, 51: 4**.

Big ball `0x800754BC`: all D (ids 7, 23, 79 –) except 24, 25, 26: 2; 27, 28, 29, 31, 37: 4; 48, 50,
51: 2; 56, 57: 8. So the X-Buster cannot destroy it.

The ids that matter for Zero (the player in the save state) [likely for the names]:

| ids | guard | open |
|---|---|---|
| 24–29, 31 (sabre and combo) | D | 3 |
| 30 | D | 2 |
| 18, 19, 20 (X full charge) | guard break | D |
| 0, 9 (X lemon, half charge) | D | D |
| 33, 35, 36, 42 | guard break | 3, 4, 3, 3 |
| 34, 43, 44, 46, 56 | guard break | D |

With 48 HP and sabre hits of 3 (+1 with the sabre part), he needs about 9–11 guard breaks to reach
one third. Then the LOW-HP hits come every 60 updates, or faster with ids 24 and 25.

Dealt (before the difficulty scaling): body 8; energy ball 6; big ball 12; shield 12; piece 12.

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 20 | the kill; a shot ends; a guard break |
| 61 | a guard break; a STUN hit; the kill; an orb |
| 33 | the kill; he lands in KNOCKDOWN |
| 27 | he lands in KNOCKDOWN; the ram hits the wall |
| 34 | VOLLEY start |
| 28 | each energy ball |
| 35 | charge start (BIG BALL, SHIELD RAM, LOW-HP ORBS) |
| 29 | 10 updates into a charge (with `Rumble(7)`); LOW-HP ORBS cycle |
| 30 | big ball release |
| 31 | big ball, shield or orb made |
| 36 | 121 updates into the big-ball wait |
| 123 | barrier up |
| 124 | a ball or orb bursts |
| 125 | a piece starts to home |
| 26 | floor effect |
| bank 0, 14 | HP bar tick |

## Summary for a reimplementation

- A floating boss, 40 × 72 px, contact 8, HP 48 (52 / 56 / 64 at higher boss levels). He hovers at
  room y 64 with a 4 px bob, and dives to y 112–116 for his rushes. He enters and leaves the room
  from the top, at room x 64 or 288.
- **Guard:** only weapon ids 18–20, 33–36, 42–44, 46, 56 have an effect, and they do no damage. They
  stun him for up to 121 updates. One damaging hit in that time knocks him to the floor (90 updates),
  and then he flies out and comes back. Then 60 updates of full invulnerability.
- THINK at full HP: volley + barrier rush, or volley + shield ram. Under two thirds: also the big
  ball. Under one third: he goes to the room centre, has no guard (60 updates of invulnerability after
  each hit), and makes an orb every 127 updates for the rest of the fight.
- Volley: 12 aimed energy balls, 15 updates apart, 5 px/frame, contact 6. One hit destroys a
  ball, but only from a weapon with `+0x9C` ≠ 0 (`dmg_tbl_one_hit`); plain buster shots pass
  through it (`docs/damage.md`, "Shots that can hit enemy shots") [certain for the code].
- Big ball: drops, rolls at 3 px/frame, climbs the wall; 8 HP; killed, it bursts into 4 homing
  pieces (3 px/frame out, then 5 px/frame at the player, 40 updates apart).
- Barrier rush: invulnerable; charge, dive, 50-update pause, 6 px/frame across the room, out at the
  top, back from the other side; 1–3 times by boss level.
- Shield ram: invulnerable; an 80 × 80 shield in front (contact 12); 6–8 px/frame from wall to wall.
- Orbs (low HP): after 61 updates each orb bursts into 4 pieces (6 on Hard).
- Death: he holds the player, flashes 127 updates, explodes; exit request `0x10`; no soul, no
  progress byte.

## Open questions

1. Who sets X's shot type 56 (the guard-break id 56). Check the guard breaks in play.
2. Player event step 29 (the walk).
3. `var` 2: record 112 of stage 17 (main 55 `id` 2 at (3312, 336); overlay 23 holds the same 158
   functions; `docs/stages/gate-2.md`) [certain]. Open: steps 13–17, objects `var` 9–11, and the
   maker of `var` 2 shots (a save state of the stage 17 fight is needed).
4. What `dmg_tbl_capsule` (class 7) does on the orb's hurt box.
5. Who uses animations 0, 1, 3 and 11 (not this code).
6. In play: the volley timing, the rush height against a standing and a dashing player, the stun
   length (121 updates), and whether the low-HP step really never ends.
7. `0x794C`–`0x794F` before the first shot copy (bytes 384–511 of entry `0xD`): no user found.
8. Why `turtloid-8` ends the intro rise 4 px higher than `scaravich-9`.
