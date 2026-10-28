# Shield Sheldon (stage 10, main object type 51): behaviour sheet

Shield Sheldon is the Maverick of the museum stage. He is a clam-like Reploid with a large shell
shield (a 32 × 32 px hurt box). He never walks: he moves only by **warps** (he goes out and comes
in at a fixed point of the room) and by flight. Most of the time he holds his **guard**: then
every shot is deflected. He opens for some time in each attack: when he throws his shields, while
he chases with his four **shield plates**, while his **corner turrets** are out, and in his lunge.
The plates and the turrets answer each shot that touches them with a shot of their own. Three
weapon ids stun him and knock him against a wall. The attacks change below half HP.

Stage overlay `10` (`rock_bin/10.bin`, loaded at `0x800E9860`, to `0x800FD008`; the boss code is at
`0x800F44A0`–`0x800FA8FC`; no boss overlay: the RAM at `0x800FA000` is overlay 10 itself), save state
`sheldon-17_boss.state.gz` (stage 7, area 0, checkpoint 3; X, form 1 in the state; the boss is in
his first shield throw). Written with the RE kit (`python3 -m mmxdemake.rekit --state
sheldon-17_boss.state.gz --type 51 --name shield-sheldon`) and `docs/re-guide.md`. Units,
confidence tags and the shared engine rules: `docs/engine.md`. The stage: `docs/stages/sheldon.md`.
His plates, his mirror image and his corner turrets are main type 51 with other `var` values; his
shots are visual type 16; his warp flashes are effect type 5 (in overlay 10).

"The room" below is the view at the fight: its origin is the camera (`camera_layer0` x, y; fixed at
x 5152, y 256 in the save state). All his points are camera + an offset. The room is 320 × 240 px;
the middle is x 160. He stands at y 144 (his origin).

## Spawn and set-up (`0x800F44A0`, `var` 0) [certain]

- **HP: 32 + a level bonus.** `boss_hp_max` (`game+0x8C`) = 32 + {0, 4, 8, 16}[boss level] (table
  `0x800E9E28`; level from `BossLevel`, `game+0x8F`): **32, 36, 40, 48**. The save state has level 0
  (`game+0x8D` = 5) and 32 HP. The HP starts at 0 and fills in the intro.
- **Contact damage 10.**
- Boxes:

  | box | value | size | use |
  |---|---|---|---|
  | attack | `0x800FC734` (−14, −14, 28, 28) | 28 × 28 px | most of the time |
  | hurt | `0x800FC738` (−16, −16, 32, 32) | 32 × 32 px | most of the time |
  | attack, lunge | `0x800FC740` (−32, −24, 48, 40) | 48 × 40 px | mirror attack, low lunge (animation 29) |
  | hurt, lunge | `0x800FC744` (−20, −20, 40, 40) | 40 × 40 px | the same |
  | terrain | `0x800FC73C` (0, 0, 8, 8) | 16 × 16 px | only in the knock-back of a stun |

  No terrain box at other times: he does not collide with the room. During a warp (animations 12–17,
  while he is out) he has no boxes.
- Damage table `dmg_tbl_deflect` at the set-up (his guard). Draw priority 5. Animation table
  `0x800FCBBC` (41 animations). Texture page `0x140`; his sprite data from his bank (two sprite sets,
  see "Animations"). In stage 12 he uses other graphics (`stage_object_table+0x8`, palette `0x7948`
  / `0x794C`).
- Position: the record position (the record of main 51 in `docs/stages/sheldon.md`); the intro moves
  him. `display` = 0: he is not drawn until his warp-in.
- Fields cleared: the fight timer `0x800FD000`, the plate freeze counter `0x800FD004`, the pattern
  fields (`+0x8A` = −1, `+0x8B`, `+0x8C`), the guard flag `+0x8E`.
- Then state 1, step 8 (INTRO).

## The `var` parameter

`var` makes five kinds of object [certain]:

| `var` | object | made by |
|---|---|---|
| 0 | Shield Sheldon | his record |
| 16–19 | a shield plate (up, left, down, right) | step 5 |
| 32 | his mirror image | step 6 |
| 48–51 | a corner turret (top right, top left, bottom left, bottom right) | step 7 |
| other | not used | |

The entry (`0x800F78AC`) runs the state from `0x800FCC60` (8 states: 0 set-up, 1 boss main, 2 boss
death, 3 plate, 4 mirror image, 5 turret, 6 fall, 7 delete). For `var` ≠ 0 it first tests the
parent (`+0x80`): when the boss is in step 9 (stunned), the object goes to state 6 (it falls with
gravity 0.258 px/frame², `0x4200`, flickers, and is deleted 64 px off screen).

## State machine

States: above. Step table of the boss `0x800FCC80`: 0 engine hit reaction, 1 nothing, 2 WARP GUARD
(not used), 3 BOUNCE, 4 SHIELD THROW, 5 PLATES, 6 MIRROR, 7 TURRETS, 8 INTRO, 9 STUNNED.

### Main (state 1, `0x800F48B4`) [certain]

1. `AnimeTick`, then the step.
2. Outside the intro (step ≠ 8): the fight timer `0x800FD000` counts 1 per update (for the soul).
3. `ContactWeapon`:
   - a kill: `mega+0x7A` = 1, state 2 (death);
   - a hit (result > 0, not 127; so also 126): `dmg_tbl_deflect` and **30 updates of
     invulnerability** (`+0x88`; 2 updates after weapon ids 24 and 25, the first sabre hits);
   - a hit by **weapon id 4, 13 or 35**: `dmg_tbl_deflect`, `+0x8E` = 1, **80 updates** of
     invulnerability, step 9 (STUNNED). The damage of the hit is applied first;
   - a deflected shot (127): `+0x8D` = 1 for this update (read only by step 2).
4. While the invulnerability counts down, the palette flash toggles every 2 updates. At the end:
   `dmg_tbl_deflect` again when the guard flag `+0x8E` is 1 (then `+0x8E` = 0), else his own table
   `dmg_tbl_sheldon`.
5. `ContactMega`.

The boss state does not call `DisplayCheck`; only the intro sets `display` = 1.

### The guard [certain]

He takes damage only while his table is `dmg_tbl_sheldon` (`0x80074E7C`). Every step starts with
`dmg_tbl_deflect`, because each step ends with it (and the intro starts with it). The steps write his
own table only at these points ("open" below):

| step | open from | open until |
|---|---|---|
| 3 BOUNCE | never | |
| 4 SHIELD THROW | the second throw | the catch |
| 5 PLATES | the plates come out | the end of the 600-update chase |
| 6 MIRROR | the throw (high) or the lunge (low) | the end of the throw animation or of the lunge |
| 7 TURRETS | the turrets come out | the fourth dash ends |

`dmg_tbl_deflect` deflects every weapon id (the standard "tink": the shot bounces off with
`WeaponHitReaction(shot, −1)`), except ids 7, 23 and 79 (no effect). His code does not send a shot
back at the player. **The "reflect" is his plates and turrets**: each shot that touches one makes it
fire at the player (below).

After a hit, the 30 updates of invulnerability use `dmg_tbl_deflect`. When they end, the guard
flag `+0x8E` decides: when the step has taken the guard again in the meantime, the table stays
`dmg_tbl_deflect`.

### INTRO (step 8, sub-steps `0x800FCDA8`) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800F948C` | `PlayerEvent(20, 64)`: the player is held. |
| 1 | `0x800F94C4` | Waits until `game.exit_req` = 0 and the player is in step 20. Then `PlayerEvent(29, 64)` (not in stage 12), engine object 24 (stage-logic class; probably "WARNING" [guess]), a 30-update timer. |
| 2 | `0x800F5880` | At the end of the timer: `PlayerEvent(20, 64)`. When object 24 is gone: `game+0x24` = 1, `game+0x25` = 0, `game+0x20` = this object, `BossLevel`, `boss_hp_max` (above); he moves to room (240, 144) and makes a warp flash (effect 5, `var` 0). |
| 3 | `0x800F955C` | When the flash sets his `act4` = 1 (its event flag 2; sound 79): second sprite set, sound 80, animation 0, `display` = 1. |
| 4 | `0x800F95CC` | At the end of animation 0: animation 1. |
| 5 | `0x800F9614` | At the end of animation 1: first sprite set, animation 5 (the shell opens to a stand). |
| 6 | `0x800F9668` | At the end of animation 5: his text (`TextOpen`, `game+0x2E` = 1; not in stage 12). |
| 7 | `0x800F96DC` | When the text is closed (`text_open` = 0): `BossMusicRequest`. |
| 8 | `0x800F9720` | Waits while `BossMusicWait` (the boss music wait). |
| 9 | `0x800F975C` | **+1 HP per update**, a tick (bank 0, id 14) every 3 updates, up to `boss_hp_max`. |
| 10 | `0x800F97E0` | `PlayerEventEnd`, THINK. |

### THINK (`0x800F5D58`) [certain]

THINK has no wait. It runs at the end of every step and picks the next step from a pattern (8
bytes each; `0xFF` ends a pattern). "High HP" means HP > `boss_hp_max` / 2 (16 at level 0).

| table | entries (step lists) |
|---|---|
| high HP (`0x800FCE24`, 16) | 0–8: 4, 3; 9–15: 4, 6 |
| low HP (`0x800FCEA4`, 16) | 0–4: 5, 7; 5–10: 5, 4; 11–15: 6, 6, 7 |

1. The first time (`+0x8A` = −1): a pattern `GetRNG() & 15` from the high table.
2. In the high mode (`+0x8A` = 0), each THINK first tests the HP: at HP ≤ half it changes **at once**
   (also in the middle of a pattern) to the low mode with a pattern `GetRNG() & 7` (so the first low
   pattern is never 6, 6, 7).
3. Else the next entry of the pattern. At `0xFF`, a new pattern `GetRNG() & 15` from the same table.

So at high HP each cycle is a shield throw, then a bounce (9/16) or a mirror attack (7/16). At low HP
the plates or the mirror start the cycle, and the turrets come only at low HP. The difficulty and
the player are not tested.

### Warps (shared by steps 2–7) [certain]

Each step starts with **animation 13** (he goes out; boxes off). At its event flag 3: sound 81; at
flag 2: sound 89; at its end (flag 1): sound 89, **animation 12** (he comes in) at the step's point.
Animations 15 / 14 (step 6) and 17 / 16 (steps 4 and 7) are the same warp with other sprites. The
warp out is 47 updates (animation 13; 16 for animations 15 and 17), the warp in 46 updates
(animation 12; 26 for animations 14 and 16). He has no boxes while he is out.

### BOUNCE (step 3, sub-steps `0x800FCCC8`; high HP only) [certain]

1. Sub 0: face the player, warp out. Sub 1: warp in at room (160, 32), facing the player.
2. Sub 2: at the end of the warp: animation 19, boxes on.
3. Sub 3: at its event flag 2: sound 81. At the end of animation 19 (31 updates): second sprite
   set, animation 1 (the shell). The route row: 0 when the player x > room x 160, else 1. Two
   random bits: each makes the first or the second floor point follow the player (below).
4. Subs 4 and 5: he flies in straight lines through 8 points (route `0x800FCCE4`, points
   `0x800FCCF4`), at **6, 6, 7, 7, 8, 8, 9, 9 px/frame** (point i: 6 + i / 2). At each point (within
   5 px on both axes): sound 85 and `Rumble(6)`. After the third point: animation 0.

   | row | points (room x, y) |
   |---|---|
   | 0 (player right) | (80, 176), (16, 80), (96, 0), (240, 176), (304, 80), (224, 0), (160, 176), (160, 32) |
   | 1 (player left) | (240, 176), (304, 80), (224, 0), (80, 176), (16, 80), (96, 0), (160, 176), (160, 32) |

   Point (80, 176), when its random bit is set: x = the player's x, rounded down to 16 px, limited to
   48–144. Point (240, 176): the same, limited to 176–272. So he bounces off the floor, a wall and the
   ceiling, twice, then drops to the middle of the floor and goes back to the top middle.
5. Then: first sprite set, he faces the player, animation 20; at its end (46 updates): THINK.

His table stays `dmg_tbl_deflect` for the whole step: he cannot be hurt. Contact 10 with the 28 ×
28 px box. The path is about 1,200 px: about 160 updates of flight.

### SHIELD THROW (step 4, sub-steps `0x800FCD14`) [certain]

1. Sub 0, 1: warp out; warp in at room (48, 144) facing right when the player x ≥ room x 160, else
   at (272, 144) facing left. So he comes in on the side away from the player and faces the player.
2. Sub 2: animation 21, boxes on.
3. Sub 3, at the event flag 2 of animation 21 (update 12): sound 34 or 35 (random), sound 83, the
   **first shield** (visual 16, `var` 0). At its end: animation 22.
4. Sub 4, at the event flag 2 of animation 22 (update 16): sounds 34/35 and 83, the **second
   shield** (`var` 1); **his own table: he is open**. At the end: animation 10 (stand).
5. Sub 5: he waits for the first shield. It flies 2 px/frame to the far wall (room x 288 or 32),
   comes back to x 208 or x 112, and picks at random (`GetRNG() & 1`):
   - **catch high** (`act4` = 1): he warps out (animation 17), and in at room (64, 64) facing right
     when he faced left, else at (256, 64) facing left (sub 6). At the end of the warp in: sound 82,
     animation 8 (the catch), the guard (sub 7). The first shield flies to (64, 64) or (272, 64) at
     4 px/frame and ends there; the second shield waits at its high point and ends when he reaches
     sub 8.
   - **catch low** (`act4` = 2): he stays (sub 8). The first shield flies back to his side (x 48 or
     272) at 2 px/frame: he catches it (`act4` = 3; sound 82, animation 8, the guard). The second
     shield then flies to (48, 144) or (272, 144) at 4 px/frame.
6. Sub 8: when the last shield arrives (`act4` = 4): sound 82, animation 6 (guard), the guard.
   Sub 9: **10 updates**, then THINK.

The shields are described under "Shots and effects". The step waits for the shields with no time
limit [certain for the code].

### PLATES (step 5, sub-steps `0x800FCD3C`; low HP only) [certain]

1. Sub 0, 1: warp out; warp in at room (160, 64), facing right.
2. Sub 2: at the end of the warp: sound 86, animation 23 (the plates come off), boxes on.
3. Sub 3: at its event flag 2: sound 87. At its end (71 updates): sound 36 or 37, animation 10,
   **his own table (open)**, the freeze counter `0x800FD004` = 0, the four **plates** (main 51,
   `var` 16–19), a 600-update timer, `act4` = 1.
4. Sub 4: **600 updates**. Each update he aims at the player and moves at **1 px/frame** (no move
   when the player is within 6 px on both axes). He faces his move. While the freeze counter is not
   0, he does not move and the counter goes down by 1.
5. At the end: animation 6, the guard, `act4` = 0 (the plates end). Sub 5: THINK.

### MIRROR (step 6, sub-steps `0x800FCD54`) [certain]

1. Sub 0, 1: warp out; warp in at room (160, 64), facing right.
2. Sub 2: at the end of the warp: animation 6 (guard), face the player, boxes on, 20 updates.
3. Sub 3: animation 8, his **mirror image** (main 51, `var` 32; sound 101), `act4` = 1, 20 updates.
4. Sub 4: animation 15 (a warp out), boxes off, `act4` = 2.
5. Sub 5: at the end of the warp out: he comes in (animation 14) at the **mirror point of the
   player ± 48 px** (mirror about room x 160), at room y 48 or y 160 (random). The image goes to the
   other point (`act4` = 3). In numbers, with the player at room x p:
   - p ≤ 160: the image at x max(p − 48, 40), facing right; he is at 320 − that x, facing left;
   - p > 160: the image at x min(p + 48, 264), facing left; he is at 320 − that x, facing right.

   So one of the two appears 48 px behind the player, and the other on the far side of the room.
6. Sub 6, at the end of the warp in:
   - **high (y 48)**: animation 22, sub 7. At its event flag 2: sounds 34/35 and 83, **his own table
     (open)**, a **mirror shield pair** (visual 16, `var` 32 from him and `var` 33 from the image
     side). At the end of animation 22: animation 10, sub 8: he waits until the shield ends
     (`act4` = 4): sound 82, animation 8, the guard, 30 updates (sub 9).
   - **low (y 160)**: sounds 34/35 and 84, animation 29 (a lunge, 23 updates), **his own table
     (open)**, the lunge boxes (48 × 40 px attack, 40 × 40 px hurt). At its end (sub 11): the guard,
     the normal boxes, 30 updates (sub 9).
7. Sub 9: sound 82, animation 6, `act4` = 0 (the image ends), 20 updates (sub 10), THINK.

The image copies his sprite, his boxes and his `display` every update, and it flickers (drawn every
other update). Its table is not set (see "The plates, the image and the turrets").

### TURRETS (step 7, sub-steps `0x800FCD84`; low HP only) [certain]

1. Sub 0, 1: warp out; warp in at room (160, 80), facing right.
2. Sub 2: at the end of the warp: draw priority 6, animation 30, face the player, boxes on.
3. Sub 3: at the event flag 2 of animation 30: a ring flash (effect 5, `var` 16). At flag 3 (update 48):
   sound 38 or 39, `act4` = 1, the four **corner turrets** (main 51, `var` 48–51). At its end (59
   updates):
   **his own table (open)**, animation 17 (warp out), boxes off, dash count 0.
4. Sub 4: at flag 2 of the warp: sound 89 for the first warp, else 90. At its end: priority 4; he
   comes to a random corner (`GetRNG() & 3`): 0 (288, 32) facing left, 1 (32, 32) facing right,
   2 (32, 144) facing right, 3 (288, 144) facing left. The target is another corner:
   (corner + 1 + `GetRNG()` % 3) & 3. Timer 50.
5. Sub 5: after **50 updates**: animation 16 (warp in). Sub 6: at its end: boxes on (at flag 2:
   sound 90).
6. Sub 7: he **dashes to the target corner at 4 px/frame** (Hard and above: 5 px/frame); the
   animation by direction: 37 horizontal or diagonal up, 38 diagonal down, 39 straight up, 40
   straight down. He faces his x move; on a vertical dash he faces the room middle. At the corner
   (within 3 px): dash count + 1. Fewer than 4 dashes: animation 17, sound 90, boxes off, sub 4
   (warp to a new corner). After **4 dashes**: animation 6, the guard, `act4` = 0 (the turrets end),
   priority 5, THINK.

### STUNNED (step 9, sub-steps `0x800FCDD4`) [certain]

- Entered on a hit by **weapon id 4, 13 or 35**. All his shots fall away (visual state 2), and the
  plates, the image and the turrets fall (state 6); the warp flashes end.
- Sub 0: sound 33, animation 18 (stunned), the normal boxes, contact 10. **Knock-back:** away from
  the player at **12 px/frame**, slowed by 0.3 px/frame² (`0x4CCC`) until 4 px/frame; then 4 px/frame.
  He gets the terrain box.
- Sub 1: when he touches the wall behind him: sound 85, `Rumble(6)`, `Quake(10, 4, 2)`, no terrain
  box.
- Sub 2: **40 updates**. Sub 3: THINK.

The stun hit sets 80 updates of invulnerability with `dmg_tbl_deflect`; the guard flag keeps that
table after them. Contact damage stays on during the stun.

### WARP GUARD (step 2, `0x800FCCB4`; not used) [certain for the code]

No pattern holds step 2 and no code writes it. It is a guard that warps away when a shot is
deflected: warp in at room (64, 80) or (256, 80) on the side away from the player, the guard
(animation 6) with `dmg_tbl_sheldon_guard` for 40 updates; a deflected shot (`+0x8D`) makes him warp
to the other side, up to 2 times; then THINK with `dmg_tbl_deflect`.

### Death (state 2, steps `0x800FCCA8`) [certain]

1. Sound 32, `mega+0xCA` = 0, `PlayerEvent(20, <the player's facing>)`, animation 18, palette flash
   off. His plates, image, turrets, shots and flashes end (their parent is
   not in state 1).
2. **127 updates.** The flash toggles at shorter intervals (26, 21, 16, 11, then 6 updates). Then
   engine object 26 (stage-logic class; probably the boss explosion [guess]) at his position.
3. While object 26 runs he flashes every 6 updates; while its `+0x07` is not 0 he is not drawn. When
   it is gone: `game+0x24` = 0, `game+0x20` = 0, and:
   - in stage 12: `game.exit_req` = `0x80`, `game[0x2C + checkpoint]` = 1, checkpoint + 8;
   - else a **Nightmare Virus** (main type 15) with `var` **`0x10` (200 souls) when the fight took
     less than 5,401 updates (90 s)**, else `var` `0x11` (100 souls)
     (`docs/enemies/nightmare-virus.md`, BOSS SOUL).

   Then he is deleted (`DeleteObject3`).

## Animations (table `0x800FCBBC`, 41 animations) [certain for the frames]

**Two sprite sets.** `0x800F5F04(obj, set)` picks the sprite data: set 0 = his bank (texture
`0x140`), set 1 = the next bank (bank + 1; texture `0x540`, the palette of the next bank) [certain].
The boss uses set 1 for animations 0 and 1 (the intro and the bounce). The turrets (main 51 `var`
48–51) and the shields (visual 16 `var` 0, 1, 32, 33) always use set 1. All other objects use set 0.
The export (`shield-sheldon`, `enemy-0d386ba53d`) takes set 1 from the `sets` rule of
`catalog/enemies.json` (stage 7, type 51: animations 0, 1, 2, 3, 35, 36; texture `0x540`, palette
of bank + 1). `sheldon-17_boss` draws these strips from entry 15 correctly; the other strips did
not change.

| # | what | used by | set |
|---|---|---|---|
| 0 | shell (8 frames of 2, then flag 1) | intro sub 3; bounce after the third point | 1 |
| 1 | shell, slower (8 frames of 4) | intro sub 4; bounce | 1 |
| 2 | turret | turrets, flying out | 1 |
| 3 | shield, spinning (loops) | shields `var` 0, 1, 32, 33 | 1 |
| 4 | warp-in flash (sparkle, 35 updates; flag 2 at update 13) | effect 5 `var` 0; boss set-up (not drawn) | 0 |
| 5 | shell opens to a stand (59 updates) | intro sub 5 | 0 |
| 6 | guard (loops, 4 frames of 14) | guard pose | 0 |
| 7, 9, 11, 32 | one frame each | not started by the code | 0 |
| 8 | catch / guard (loops) | catch, image start, stun end | 0 |
| 10 | stand (loops) | between attacks | 0 |
| 12 | warp in (46 updates) | all steps | 0 |
| 13 | warp out (47 updates; flags 3, 2, 1 at 12, 33, 47) | all steps | 0 |
| 14, 15 | warp in / out (other sprites) | mirror | 0 |
| 16, 17 | warp in / out (other sprites) | shield throw, turrets | 0 |
| 18 | stunned (one frame) | stun, death | 0 |
| 19 | pose before the bounce (31 updates) | bounce | 0 |
| 20 | end of the bounce (46 updates) | bounce | 0 |
| 21, 22 | first and second throw (flag 2 = release) | shield throw | 0 |
| 23 | the plates come off (71 updates) | plates | 0 |
| 24, 25 | plate, closed (vertical, horizontal) | plates | 0 |
| 26 | plate, open | plates | 0 |
| 27 | plate fires (flag 1 at the end) | plates | 0 |
| 28 | plate shot (loops) | visual 16 `var` 16–19 | 0 |
| 29 | lunge (23 updates) | mirror, low | 0 |
| 30 | turret call (59 updates; flags 2, 3 at 6, 48) | turrets | 0 |
| 31 | ring flash | effect 5 `var` 16 (played twice) | 0 |
| 33 | hit spark | effect 5 `var` 32 | 0 |
| 34 | turret shot (loops) | visual 16 `var` 48–51 | 0 |
| 35, 36 | turret at its corner (top, bottom) | turrets | 1 |
| 37, 38, 39, 40 | dash: horizontal or diagonal up, diagonal down, up, down | turrets step | 0 |

## The plates, the image and the turrets (main type 51)

Each copies the boss's animation table and layer, keeps the boss in `+0x80`, and ends (state 7) when
the boss is not in state 1 or the boss's `act4` is 0. **They have no damage table** (`+0x58` = 0) and
contact damage 0 [certain]. So `ApplyWeaponHit` reads the "table" at RAM address 0. In the save state
that area is zero for most ids (class 0, damage 0): a touching shot is a hit with 0 damage, the HP
(0) stays at 0, the result is a kill, and the shot stops [likely: it depends on the low RAM, which
the BIOS writes]. They never die from it, and the kill count (`game+0x94`) goes up by 1 for each such
hit [certain for the engine code]. A contact with the player applies contact damage 0, which
`PlayerDamageScale` raises to 1 [likely].

### Shield plates (`var` 16–19, state 3, steps `0x800FCDE4`) [certain]

- Positions: 38 px from his origin: `var` 16 above, 17 left, 18 below, 19 right. They follow him
  every update. Priority 5.
- Closed (step 2): animation 24 (16, 18) or 25 (17, 19); boxes (−16, −8, 32, 16) (32 × 16 px) or
  (−8, −16, 16, 32) (16 × 32 px), attack = hurt.
- **Any shot that touches a closed plate** (any non-zero `ContactWeapon` result): animation 26
  (open), boxes (−8, −8, 16, 16), 30 updates (step 3). Then: **he stops for 8 updates** (the freeze
  counter), animation 27 (step 4). At its end: a **plate shot** outward (visual 16, `var` = the
  plate's `var`), 30 updates (step 5), then closed again.

### Mirror image (`var` 32, state 4, steps `0x800FCDFC`) [certain]

- Set-up: sound 101, animation 8, his boxes, 32 px in front of him (its own facing is opposite to
  his).
- Each update: his sprite, `display` and boxes; facing opposite to his; then it flickers (drawn on
  every other update).
- Step 2 waits for `act4` = 2; step 3, at `act4` = 3: it moves to the mirror point of his position
  (room x 320 − his x, his y); step 4 keeps it there.

### Corner turrets (`var` 48–51, state 5, steps `0x800FCE10`) [certain]

- Set-up: animation 2 (set 1), his normal boxes (28 × 28 attack, 32 × 32 hurt), priority 5.
- Step 2: each flies from his position to its corner at **6 px/frame**: `var` 48 (288, 32), 49
  (32, 32), 50 (32, 144), 51 (288, 144). There (within 6 px): animation 35 (top) or 36 (bottom),
  facing the room.
- Step 3: **a shot that touches it**: a hit spark (effect 5, `var` 32), it flashes for 7 updates
  (step 4), then fires a **turret shot** at the player (visual 16, `var` = its `var`) and waits again.

## Shots and effects

### Visual type 16 (`0x800F9FE4`, states `0x800FCF40`)

Each copies the boss's graphics (`docs/engine.md`, "copies the parent"; the parent in `+0x7C` is the
boss). HP 1. They end (state 3) when the boss is not in state 1. When the boss is stunned (step 9)
they fall (state 2: gravity 0.258 px/frame², flicker, deleted 64 px off screen). Off screen by
32 px: deleted. Visible within 16 px.

| `var` | object | boxes, damage | movement and end |
|---|---|---|---|
| 0 | first shield (`0x800FA0EC`) | attack = hurt (−16, −12, 32, 28): 32 × 28 px; `dmg_tbl_deflect`; contact **8**; priority 4 | Starts 16 px in front of him. **2 px/frame** to room x 288 (moving right) or 32, back to x 208 or 112; then a random choice (`act4` 1 or 2, step 4). Choice 1: it turns and flies at **4 px/frame** to room (272, 64) or (64, 64), sets `act4` = 4 and ends. Choice 2: on at 2 px/frame to room x 48 or 272 (his side); sets `act4` = 3 and ends. |
| 1 | second shield (`0x800FA340`) | as `var` 0 | **4 px/frame** to room (272, 64) when it faces right, else (64, 64). At `act4` 1: it ends when he reaches sub 8. At `act4` 2: it turns and flies 4 px/frame to (48, 144) or (272, 144), sets `act4` = 4, ends. |
| 16–19 | plate shot (`0x800FA61C`) | attack = hurt (−4, −4, 8, 8); `dmg_tbl_one_hit` (one hit from a weapon with `+0x9C` ≠ 0 destroys it; plain buster shots pass through, `docs/damage.md`); contact **4** | Sound 88. **2 px/frame** straight out: up (16), left (17), down (18), right (19). Hard and above: 3 px/frame. No terrain test. Ends on the player (hit effect) or off screen. |
| 32 | mirror shield (`0x800FA42C`) | as `var` 0 | Starts 16 px in front of him. 7 straight legs at **6, 6, 7, 7, 8, 8, 9 px/frame** through the points (route `0x800FCFA4`, points `0x800FCFB4`): row 0 (he faces left): (80, 176), (16, 64), (64, 0), (240, 176), (304, 64), (256, 0), back to the mirror point of its start; row 1: the halves swapped. Points (80, 176) and (240, 176) can follow the player's x (random bits, limits 32–144 and 176–272). Sound 85 and `Rumble(6)` at each point but the last. At the end: `act4` = 4, it ends. |
| 33 | mirror of `var` 32 (`0x800FA538`) | as `var` 0 | Every update at the mirror point (room x 320 − x) of `var` 32, with its sprite and `display`, facing the other way. Ends with `var` 32. |
| 48–51 | turret shot (`0x800FA5BC`) | attack (−4, −4, 8, 8), no hurt box (it cannot be shot); terrain box (0, 0, 2, 2); contact **8** | Sound 88. **Aimed at the player at 2 px/frame** (Hard and above: 4 px/frame). Ends on terrain (`CornerCheck`, hit effect) or on the player (hit effect). |

The shields have `dmg_tbl_deflect`: they deflect shots and cannot be destroyed. A full visual pool
makes a shield step wait for ever [certain for the code; not seen].

### Effect type 5 (overlay 10, `0x800FA8C4`, states `0x800FCFD4`) [certain]

Priority 4, his graphics (set 0). They end when the boss is not in state 1 or is stunned.

| `var` | animation | what |
|---|---|---|
| 0 | 4 | intro warp flash at his position; at its flag 2: sound 79 and his `act4` = 1 |
| 16 | 31, two times | the ring of the turret call; sound 79 at flag 2 |
| 32 | 33 | the hit spark on a turret |

## Damage

`0x80074E7C` (named `dmg_tbl_sheldon` here), his own table when he is open:

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 5 2 2 – 2 1 2 2 2 8 2 2 |
| 16–31 | D 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 8 2 2 2 2 2 2 2 2 D 1 4 8 |
| 48–63 | 2 1 2 2 1 D 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

The ids that matter:

| ids | damage |
|---|---|
| 0 (lemon), 9 (half charge) | 1 |
| 18, 19, 20 (full charge parts) | 4 each |
| 24, 25 (sabre) | 2 each (+1 sabre bonus: 3) |
| **4 / 13 / 35 (the stun ids)** | **5 / 8 / 8** |
| 17, 22, 47, 57 | 8 |
| 60, 62 | 15 |
| 55, 79 | 127 |
| 16, 21, 23, 44, 53, 64–78 | deflected |
| 7 | no effect |

`0x80074F1C` (named `dmg_tbl_sheldon_guard`; only the unused step 2): D for every id, except id 1
(2), id 54 (1), id 55 (127), ids 7 and 23 (no effect).

With 32 HP (level 0, before the part bonuses and the Easy × 1.25): 32 lemons, 8 full charges, 16
sabre hits, 4 hits by id 13 or 35, 7 by id 4. The stun ids are probably his weakness: ids 4 and 13
fit "special weapon slot 4, uncharged and charged" (`docs/engine.md`: 10–17 = slot + 9) [guess]; the
same pattern is in Rainy Turtloid (2, 11, 33) and Commander Yammark (8, 17, 44).

Dealt (before the difficulty scaling): body 10 (also in the lunge and while stunned); shields 8;
plate shot 4; turret shot 8; plates, image and turrets 0 (1 after the scaling [likely]).

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 79 | warp flash (effect 5), intro and turret call |
| 80 | intro: he appears |
| 81 | warp out (flag 3 of animation 13); the bounce pose (flag 2 of animation 19) |
| 89 | warp in and out (all steps); first turret warp |
| 90 | turret warps after the first, and dash ends |
| 34 or 35 (random) | a throw or a lunge |
| 83 | a shield throw |
| 84 | the lunge |
| 82 | catch / guard |
| 85 | a bounce point (with `Rumble(6)`); the stun hits the wall |
| 86, 87 | the plates come off |
| 36 or 37 (random) | the plates are out |
| 38 or 39 (random) | the turrets come out |
| 88 | a plate shot or a turret shot |
| 101 | the mirror image appears |
| 33 | stunned |
| 32 | death |
| bank 0, 14 | HP bar tick |

The random pairs (34/35, 36/37, 38/39) are probably two voice clips each [guess].

## Summary for a reimplementation

- A boss with no walk: 32 × 32 px hurt box, contact 10, HP 32 (36 / 40 / 48 at boss levels 1–3). He
  moves by warps (out 47 updates, in 46) and by flight. After a hit: 30 updates of invulnerability
  (2 for the first sabre hits).
- His guard deflects every shot. He is open only: after the second shield throw until the catch;
  for the 600 updates of the plate chase; in the mirror throw or lunge; from the turret call to the
  fourth dash. The bounce attack is never open.
- High HP: a shield throw (two shields, 2 and 4 px/frame, caught high or low), then a bounce (8
  legs, 6–9 px/frame, off floor, walls and ceiling) or the mirror attack.
- Low HP (≤ half): plates (four plates around him, he chases at 1 px/frame for 600 updates; a shot on
  a plate opens it, stops him 8 updates and makes it fire outward at 2 px/frame), the mirror attack
  (he and a flickering image appear on both sides of the player; a mirror shield pair flies 7 legs,
  or a lunge), and turrets (four corner turrets that fire an aimed shot at each shot that touches
  them; he dashes corner to corner 4 times at 4 px/frame).
- Weapon ids 4, 13 and 35 stun him: knock-back at 12 px/frame into the wall, 40 updates, all his
  helpers and shots fall away.
- Death: he holds the player, flashes 127 updates, explodes, and leaves a boss soul: 200 souls under
  90 s, else 100.

## Open questions

1. Which weapons are ids 4, 13 and 35 (the stun; probably his weakness [guess]).
2. The plates, the image and the turrets have no damage table: what the low RAM gives in other
   emulators and on hardware, and whether every weapon id makes them react (a play check: shoot a
   plate with each weapon).
3. Whether the plates, the image and the turrets hurt the player on contact (contact 0; 1 after the
   scaling [likely]).
4. Re-export animations 0–3, 35 and 36 from bank 15 (texture `0x540`); a save state in the bounce or
   with the turrets out has that texture in use.
5. Engine objects 24 and 26 and player event step 29, as for the other bosses.
6. Step 2 (the warp guard with `dmg_tbl_sheldon_guard`) is not used: check that no other overlay
   writes it.
7. The random pairs of sounds (34/35, 36/37, 38/39).
