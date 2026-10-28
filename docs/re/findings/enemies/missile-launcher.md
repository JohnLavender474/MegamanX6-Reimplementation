# Missile launcher (stage 22 and 23, main object type 34): behaviour sheet

A heavy grey and orange robot on the floor of Gate's lab. It does not walk. It fires volleys of
small shells straight ahead from three launch points, then two homing missiles from its top, then
reloads. Its shots are faster against Zero (one more shell per volley). It has 30 HP and 4 contact
damage, and it breaks at once when the terrain crushes it. "Missile launcher" is a descriptive
name; no official name was found.

Code in overlay `23` (stage 17, entry `0x800EAF7C`, state table `0x800F8D18`) and overlay `22`
(stage 16, entry `0x800F4BA4`, set-up `0x800F43A0`, state table `0x800FC87C`): the 42 functions are
identical after masking [certain]. Save state `gate-3_stage-2.state.gz` (stage 17, checkpoint 1; no
launcher is live, the animation table was found by the code search). Kit: `python3 -m
mmxdemake.rekit --state gate-3_stage-2.state.gz --type 34`. Export: `enemy-eb6483d4fb` (proposed
name `missile-launcher`). Units, tags and the shared rules: `docs/engine.md`. Records:
`docs/stages/gate-1.md` (3), `docs/stages/gate-2.md` (4). The addresses below are those of overlay
23.

## Spawn and set-up (state 0, `0x800EA778`) [certain]

- Flags `0x41`, **HP 30**, **contact damage 4**, `iframes` 0, `dmg_tbl_normal`, draw priority 5,
  layer = the player's layer, animation table `0x800F8CD8`, animation 0 (idle).
- Hurt box `0x800F8BE4` = (−27, −35, 62, 66); attack box `0x800F8BE8` = (−27, −32, 58, 66);
  terrain box `0x800F8BEC` = (3, 0, 27, 32): a **54 × 64 px** body.
- All speeds and gravity 0; `latch` 0; step 2, `step2` 0; timer `work.7C` = **40** (10 for `var`
  2); shot counter `work.84` = 0.
- Facing by `var >> 4`: 0 = toward the player at the spawn, 1 = `flip` 0, 2 = `flip` `0x40`.

## The `id` parameter

All 7 records have `id` 0: the launcher faces the player at its spawn and uses the `var` 0 rules
below [certain for the records]. The code also has `var` 2 (a shorter first wait, a range trigger,
4 shells, no missiles) and the facing nibble; no record uses them.

## State machine [certain]

States (`0x800F8D18`): 0 set-up, 1 main `0x800EA8EC`, 2 death `0x800EAFC8`, 3 and 4 delete
`0x800EB054`.

Main (state 1), each update:

1. The step function (table `0x800F8D2C`, by `step`).
2. `0x800EB634`: when there is no floor under it (`collide_flags` bit 3 clear) and `latch` is 0:
   step 6 (fall), gravity `0x4200`.
3. `ContactWeapon`, `ContactMega`.
4. Death when the weapon kill result is < 0, or when the terrain touches both sides
   (`collide_flags & 0x0C` = `0x0C` or `& 0x03` = `0x03`: crushed): `SpawnJunk(5, {8, 9, 10, 11,
   12})`, display 0, timer 30, state 2.
5. Off screen (96, 64): state 4 (delete, the record can come back); else `DisplayCheck(48, 48)`.

Then `CollideCheck` (the entry, every state).

| step | routine | what |
|---|---|---|
| 0 | `HitReactionDispatch` | the engine hit reaction |
| 1 | `0x800EB094` | `step` = `+0x94` (back to the saved step) |
| 2 WAIT | `0x800EB0A0` | animation 0. Sub 0 (`0x800EB0DC`): wait `work.7C` updates; then step 3 (`var` 2: sub 1). Sub 1 (`0x800EB148`, `var` 2 only): wait until the player is less than 144 px away in x; then step 3 |
| 3 SHELLS | `0x800EB1B0` | sub 0 (`0x800EB1EC`): animation 1 or 2 (by shot count and character), sound 2/86, one shell (`0x800EB684`) or a pair (`0x800EB854`); timer 75 (50 for `var` 2). Sub 1 (`0x800EB2F4`): wait; shot count + 1; again until the count is 3 + `game.character` (X 3 shells, Zero 4; `var` ≠ 0: 4). Then step 4 with timer 20 (`var` 2: animation 0, step 2, timer 120) |
| 4 MISSILES | `0x800EB3C8` | sub 0: wait 20; animation 5 (the launcher opens), timer 41. Sub 1: wait 41; two homing missiles (`0x800EB780`, visual 38 `var` 0 and 1); timer 60. Sub 2: wait 60; step 5 |
| 5 RELOAD | `0x800EB504` | animation 6, timer 26; then step 2 with timer 40 |
| 6 FALL | `0x800EB5D4` | `MoveObject2` until it lands; then step 2, speeds and gravity 0 |

So one cycle against X is: 40 updates, shell, 75, shell, 75, shell (a pair), 75, 20, 41, two
missiles, 60, 26: about 410 updates (7 s) [certain for the timers; the animations add no wait].

### The shell volley

- `0x800EB684` makes one visual 37. Its `var` (the launch point) = `game.character` XOR (shot
  count & 1); at shot count 3 with X it is + 1 (only a `var` 1 launcher reaches that count
  against X). `var` 2 launchers take `var` from the byte table `0x800F8BF0`[shot count].
- The missiles start at the back of the launcher (x + 26 when `flip` is 0, the shells at x − 32)
  and drift backward at 0.3125 px/frame until the fuse ends; then `MoveObject2` accelerates them
  forward [certain for the code; check in play].
- `0x800EB854` makes two visual 37 at once (`var` 0 and 2). The launcher uses it for `var` 0, shot
  count 2, when the player is X.
- The launch animation is 1 when (shot count & 1) = `game.character`, else 2.

## Animations

Table `0x800F8CD8` (14 animations; found by the code search, the export has the same table):

| # | sprites | use |
|---|---|---|
| 0 | 0 1 0 2 (21, 22, 21, 20 frames) | idle |
| 1, 2 | 0 3 4 5 / 0 6 7 5 | shell launch (two recoil poses) |
| 3 | 8–13 | shell: launch smoke (visual 37 start) |
| 4 | 14–17 | shell in flight |
| 5 | 0 18 19 18 0 20 21 20 … 22 | the launcher opens, missile launch |
| 6 | 22–27, 0 | reload (closes) |
| 7 | 28–31 | homing missile |
| 8–12 | 32–36 | debris (junk) |
| 13 | 37–41 | muzzle flash (effect 21) |

## Shots and effects

### Shell (visual 37, `0x800EB91C`; states `0x800F8D8C`) [certain]

- Set-up (`0x800EA9F4`): contact 6, HP 1, `dmg_tbl_deflect` (shots bounce off), attack and hurt box
  `0x800F8D6C` = (−13, −6, 23, 10), terrain box `0x800F8D74` = (−1, −1, 10, 4). Position by `var`
  (mirrored by `flip`): `var` 0 (x ∓ 32, y − 8), 1 (x ∓ 29, y + 14), 2 (x ∓ 12, y + 18).
  Animation 3 (launch smoke); one effect 21 (muzzle flash, animation 13, follows the parent while
  it lives).
- After 16 updates: x ± 16, speed x ±2.125 px/frame (table `0x800F8D7C`), `accel_x` `0x600`
  (0.023 px/frame²), animation 4. The acceleration stops above 3 px/frame.
- It explodes on a weapon kill or on any terrain contact (also `CornerCheck`). Off screen (32, 32)
  it is deleted.

### Homing missile (visual 38, `0x800EBB44`; states `0x800F8DC8`, steps `0x800F8DD8`) [certain for
the code]

- Set-up (`0x800EACB8`): contact 5, HP 1, `dmg_tbl_minor`, attack and hurt box `0x800F8DA8` =
  (−11, −3, 27, 10), terrain box `0x800F8DAC` = (2, 2, 13, 4); x ± 26, y − 35 (`var` 0) or − 16
  (`var` 1); animation 7. Timers: start 32, fuse `+0x8C` = 30 + 20 × `var`.
- Step 0: 32 updates of blinking (the CLUT bit 15 every 2 updates). Then speed x ±0.3125 px/frame
  and speed y +1 (`var` 0, up) or −1 (`var` 1, down).
- Step 1: count the fuse down (`var` 1 stops its vertical move below 21). At 0: `accel_x` `0x3000`
  (0.1875 px/frame²), sound 2/88, homing timer 40, step 2.
- Step 2 (`0x800EAE14`): for 40 updates, speed y = ±2 px/frame toward the player's y (no move
  within 3 px); a smoke puff (misc 25) every second update. From the 8th update of the homing it
  explodes on terrain. The acceleration stops above 4 px/frame.
- It explodes on a weapon kill. Off screen (64, 32) it is deleted.

## Damage

`dmg_tbl_normal` with 30 HP (`docs/damage.md`). The shells cannot be destroyed
(`dmg_tbl_deflect`); the missiles have 1 HP (`dmg_tbl_minor`).

## Death

State 2 (`0x800EAFC8`): 30 updates, a random explosion (type 1, ±32 px) every 5 updates; then
`DropItem(obj, 20)` (table 20) and delete. The junk of the kill: 5 pieces, animations 8–12.

## Sounds

| bank, id | when |
|---|---|
| 2, 86 | each shell launch |
| 2, 88 | a homing missile starts to accelerate |

## Summary for a reimplementation

1. 54 × 64 px, 30 HP, contact 4; it falls when there is no floor; it breaks when crushed.
2. Face the player at the spawn; wait 40 frames.
3. Fire 3 shells (4 against Zero), 75 frames apart, from three heights; against X the third volley
   is a pair. Shells: 6 damage, 2.125 → 3 px/frame straight, indestructible, explode on walls.
4. Open, fire two missiles up and down; after 30/50 frames they accelerate forward to 4 px/frame
   and follow the player's height at 2 px/frame for 40 frames. 5 damage, 1 HP.
5. Reload 26 frames, wait 40, repeat.
6. Death: 30 frames of explosions, drop table 20.

## Open questions

1. The official name.
2. The exact height and angle of each launch point against the sprites (seen only in the code).
3. Whether the shell speed table index (by `flip`) gives the shell the launcher's facing in all
   cases (checked only in the code).
4. A save state with a live launcher, to check the cycle timing.
