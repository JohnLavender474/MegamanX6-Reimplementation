# Nightmare crate (stage object type 15): behaviour sheet

The Nightmare crate is a solid 48 × 48 px block of the Nightmare Effect value 5. Value 5 comes from
the pick of stage 5 (Ground Scaravich's stage). The crates fall when nothing holds them. The player
can stand on them. Two kinds slide when X or Zero pushes them with a special move; two kinds break,
and one of them explodes. "Nightmare crate" is a descriptive name; it is not from the game.

The same code is in three overlays [certain: the instruction lists are the same, only the data
addresses differ, and the data bytes are the same]:

| stage | overlay | entry | set-up | state table | records |
|---|---|---|---|---|---|
| 4 (Metal Shark Player), area 0 | 06 | `0x800EE050` | `0x800ECBEC` | – | 23: `id` 0 ×4, 1 ×2, 2 ×17 (`docs/stages/shark.md`) |
| 7 (Shield Sheldon) | 10 | `0x800F3B98` | `0x800F2734` | `0x800FC6B8` | 7: all `id` 3 (`docs/stages/sheldon.md`) |
| 22 (Sheldon hidden area) | 31 | `0x800F1DC4` | – | – | 3: `id` 3, 0, 3 (`docs/stages/sheldon-alt.md`) |

No state has Nightmare Effect 5 in these stages, so no crate is live in any save state. All
addresses below are those of overlay 10. Kit: `python3 -m mmxdemake.rekit --state
sheldon-7_lasers.state.gz --type 15 --class stage`. Units and tags: `docs/engine.md`.

## When it exists [certain]

The set-up reads `nightmare_effect` (`0x800CD30A`). When it is not 5, `DeleteObject3` at once. A
hidden area keeps the value of the stage before it (`docs/stages/sheldon-alt.md`), so the stage 22
crates come only after a stage 7 visit with value 5.

## The four kinds (record `id` = `var`) [certain for the code; the colours are from bank 8 of stage 4]

The sprite type is 147 (bank 8 in `~/mmx-data/x6/sprites/banks/stage-4/shark-1_ride/bank-8.png`).
The object sets its sprite (`+0x47`) directly; the animation table `0x800FC68C` is used only for the
debris.

| `var` | sprite | colour | damage table | pushed by X (step 44) or Zero (step 57) | broken |
|---|---|---|---|---|---|
| 0 | 6 | grey | `dmg_tbl_capsule` (`0x80075B9C`) | slides exactly 48 px, slowing down | no |
| 1 | 5 (the value of `nightmare_effect`) | purple | `dmg_tbl_capsule` | slides and speeds up to 4.125 px per update until it hits something | no |
| 2 | 4 | brown | `0x80075D7C` | – | by weapon hits, or by X in step 44 |
| 3 | 0 | red | `0x80075D7C` | – | the same, and it **explodes** |

`var` 17: the set-up copies the object into the last stage-object slot (79) and deletes the first
copy [certain for the code; the reason is not known, maybe the draw order].

## Spawn and set-up [certain]

- HP 8, contact damage 16, `iframes` 0, draw priority 32, layer = `mega.layer`, `flip` 0, `+0xA2` = 1
  (other crates and objects test it as "solid stage object").
- Terrain box (`+0x80`, from `0x800FC6AC`) = (0, 0, 24, 24): 48 × 48 px around the origin. Hurt box
  `0x800FC6B0` = (−24, −24, 48, 48). No attack box (no contact damage while it is whole).
- Start step: 2 (rest).
- When the point of the crate is in the terrain box of another crate (`0x800F2600`: the 80 stage
  objects with flags, `+0xA2` and a terrain box), it goes to state 2 at once (it breaks).

## State machine

States (table `0x800FC6B8`): 0 set-up, 1 main `0x800F29BC`, 2 end `0x800F3BF8`. Steps (`+0x06`,
table `0x800FC6C4`): 0 slide `0x800F2BE4`, 1 fall `0x800F35F8`, 2 rest `0x800F37E4`, 3 nothing.
The entry skips the update while `mega+0xE0` is 2.

### Main (state 1) [certain]

1. Only in stages 19 and up: when nothing is on its left (no crate, collision 0) and the cell under
   its bottom is collision 58, x −= 1 per update [certain for the code; what value 58 is in the
   hidden areas was not read].
2. The step.
3. `SolidWeaponHit`: a kill (HP ≤ 0) makes 3 debris objects (animations 0–2 for `var` 2, 3–5 for
   `var` 3) and an explosion, then state 2. `var` 3 also gets the blast: attack box `0x800FC6B4` =
   (−64, −64, 128, 128) and a timer of 30.
4. Off screen by 128 px: `DeleteObject2` (the record can spawn it again at the record point),
   **except in stage 7 when the checkpoint is 1**: then it stays.
5. The solid-object helpers (`0x80031F4C`, `0x80032BE8`, `SolidPushShots`).

### Rest (step 2)

- No floor (collision bit `0x08` clear and no crate under either bottom corner): gravity 0.0508
  (`0x0D00`), step 1.
- `SolidObject` (the player stands on it or is pushed out) when the player is within 48 px in x and
  `mega+0xE5` is 0. Before and after the call it compares `mega+0x71` (the player's contact bits):
  the bits that changed show the side where the player touches it.
- **The push.** The player must be X in step 44, sub-step 2 (`mega+0x02` = 0; the pose of shot
  type 5, the slot 5 weapon, `docs/players/steps-extra.md`) or Zero in step 57 (`mega+0x02` = 1;
  skill bit `0x10`, the stage 5 technique). Contact bit `0x02` while the player faces left pushes it
  left; bit `0x01` while the player faces right pushes it right [likely for the bit meanings].
  - `var` 0 and 1: `speed_x` = ±4.0, the start x goes to `+0x7C`, step 0.
  - `var` 2 and 3, X only: 3 debris objects, then state 2 (`var` 3: the blast, as for a kill; `var`
    2: an explosion as well).

### Slide (step 0)

- `MoveObject2` with `accel_x` = ±0.1406 (`0x2400`). With `flip` 0 the engine does `speed_x −=
  accel_x`: for `var` 0 the sign is chosen so that the crate slows down; for `var` 1 so that it
  speeds up. `speed_x` is clamped to ±4.125.
- `var` 0 stops after exactly 48 px: when it is 48 px or more from the start x, x = start ± 48,
  all speeds 0, step 2. (It would need 56 px to stop by friction, so the 48 px rule always ends it.)
- It stops (step 2) when a wall (collision bits `0x01` / `0x02`) or another crate is at its front
  edge (three test points: y − 16, y, y + 16); it then moves back 1 px at a time, up to 5 px, until it
  is free.
- When it has no floor under it while it slides, it drops: step 1 with the fall gravity.

### Fall (step 1)

- Fall speed at most 2.0 px per update. It lands on the floor (collision bit `0x08`) or on a crate.
- Landing: y = (y & ~15) + 8, `PlaySound(2, 82)`, and a screen shake (`Quake2(16, 2, 1)`) when it
  fell more than 16 updates and is shown; then step 2.
- `SolidObject` as in step 2 (the player can ride a falling crate).

### End (state 2)

`display` = 0. `var` 3: for 30 updates `ContactMega` with the blast box (16 damage before the
player's damage scaling, 128 × 128 px), an explosion every 4 updates and 3 debris objects every 8;
then `DeleteObject`. Other kinds: `DeleteObject` at once. `DeleteObject` clears the record bits, so
the record can spawn the crate again [likely].

## Damage taken

`dmg_tbl_capsule` (`var` 0, 1): no damage from any weapon; class 7 for most ids (the shot stops)
[certain for the table].

`0x80075D7C` (`var` 2, 3; HP 8) [certain for the table]:

| ids | effect |
|---|---|
| 5, 14, 36 | 8 damage (one hit breaks it) |
| 0, 1, 6, 8, 9, 11–13, 15–22, 42, 43, 45–54, 58, 63, 79 | class 7 (the shot stops, no damage) |
| 2–4, 7, 10, 23–35, 37–41, 44, 55–57, 59–62 | no effect (the shot passes) |
| 64–78 | deflected |

Ids 5 and 14 are slot 5 uncharged and charged (the stage 5 weapon, `docs/engine.md`, "Weapon ids").
Id 36 is in the range of the third ids (33–44) [guess: the slot 5 one, Zero's]. (Id 4 has the
damage byte 3, but class 2, so it has no effect.)

## Summary for a reimplementation

- Exists only with Nightmare Effect 5. 48 × 48 px solid block, with gravity, lands on the 16 px grid,
  stacks on other crates.
- Grey: a push with the stage 5 move slides it 48 px. Purple: a push sends it sliding until it hits
  something. Brown: breaks with one hit of ids 5, 14 or 36 (the stage 5 weapon) or with the X push
  move. Red: as brown, then a 128 × 128 px blast for 30 updates that deals 16 contact damage.
- Landing after a long fall shakes the screen.

## Open questions

1. What the slot 5 weapon (X step 44) and Zero's step 57 look like in play; which weapon is id 36.
2. The collision value 58 drift in stages 19 and up.
3. Why `var` 17 moves itself to the last slot.
4. A save state with Nightmare Effect 5 in stage 4 or 7.
