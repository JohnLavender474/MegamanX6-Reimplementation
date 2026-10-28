# Player steps 32–77 and the armour forms: notes

Date: 2026-10-21. Kits: [`~/mmx-data/x6/re/x/kit/`](../kit/) (X, `intro-1_start`), [`~/mmx-data/x6/re/zero/kit/`](../../zero/kit/)
(Zero, `turtloid-6_zero-player`), `kit-no-armor/` here (X, `sheldon-4_no-armor`). Scripts in
`scripts/`. Overlay disassemblies here: `ovl16.dis` (stage select menu), `ovl19.dis` (title / new
game). `kit-before/` holds the two summaries from before the tool change.

## Tool change (rekit --player)

- `STEP_COUNT` 32 → 78 (table `0x80073ACC`; the next table, `dash_steps`, is at `0x80073C04` =
  `0x80073ACC + 78 × 4`).
- A step entry is a root when it is in the player code of the main program
  (`0x80034FBC..0x80040000`), or when it is in the player overlay, passes `is_function_start`,
  and is not strictly inside a function that `overlay_starts` found. Else it is listed as
  "not walked" in the Roots table.
- The last rule is needed: X's RAM has the prologue `addiu sp,-56` at `0x801EB324` (steps 49 and
  53, Zero's) inside the X function `0x801EB31C` (that function loads a byte before its
  prologue). Without the rule the X kit got a second copy of that function as "step 49, step 53".
- Kit counts: X 398 → 399 functions (main 218 → 219, overlay 180 → 180); Zero 276 → 277 (main
  212 → 213, overlay 64 → 64). The new function in both is step 73 (`0x80037E1C`). The X
  overlay steps were already walked as "overlay 0 function" (the step table is in the main
  program span, so `overlay_starts` saw the pointers); now they also carry "step N".
- Step routines walked: X 53, Zero 47 (distinct addresses).

## Ownership of the entries (from `is_function_start` in both overlays)

| entries | target | owner |
|---|---|---|
| 32–47, 74–77 | X overlay functions | X (in Zero's RAM no entry starts a function) |
| 48–63 | Zero overlay functions | Zero (49/53 and 50/54 share routines) |
| 64–71 | `0x80035274` (stand) | both |
| 72 | `0x800E9B40` | stage overlay area (data in intro, code in Turtloid); no writer |
| 73 | `0x80037E1C` | main program; no writer |

No code writes 72 or 73: no `addiu r,zero,72/73` before `sb r,5(mega)` in the player code, no
`PlayerEvent(72|73, …)` call in the main program or any `rock_bin` overlay (all event calls use
1, 4, 7, 10, 20, 24, 29), and no absolute `sb r,0x70A5(…)` anywhere. Step 73 runs `AnimeTick`
until `game.exit_req` ≠ 0, then sub-step 1 sets state 3.

## Entries of the X steps

| step | entered by | condition |
|---|---|---|
| 32 | `0x801EB6E8` (from `TryLadderAir` `0x8003A488`, `XLadderShoot` `0x8003E260`) | X on a ladder in the shooting pose: animation from `0x801F36C8[+0x97]` (93: duration = `+0x91` − 8), x and y fractions 0, x.px = (x.px & ~15) + 8 |
| 33 | `UltimateAirTry` `0x8003F200` (from `TryAirDash` `0x8003906C`) | form 4, jump pressed: speeds 0, `+0x84` 0, `+0x86` 4, `+0x8C` 0, `+0xD9` 180, `+0xDA` 0, animation 21 (shoot version) |
| 34, 36, 37, 77 | `0x8003E344` (called by `0x8003F644`, `0x8003E06C`, `0x8003E174`, `0x8003E260`, `WeaponPose` `0x8003F13C`) | X, `+0x92` 0, `+0xA8` = `+0x14A`, pressed bit `0x40`, form 1–4; form 4 also `+0x86` 0 and the facing side not blocked |
| 35 | `FalconAirTry` `0x8003F28C` | form 1 (air.md) |
| 38–47, 75, 76 | `0x8003F864` (jump table `0x8001149C`, index `+0x96` − 3, 54 entries) | the shot type `+0x96` |
| 74 | `BladeAirTry` `0x8003F32C`; also `0x801EFC70` (a weapon object) | form 3, jump pressed or `TryDashInput` |

`0x8003F864` table (shot type → function → step):

| `+0x96` | function | step | animation | other |
|---|---|---|---|---|
| 3, 50 | `0x8003FD6C` | 45 ground, 46 air (a1 ≠ 0), 47 (state `0x0B00` or step 47 = wall) | 183/184 (form 2: 185/186), 187 wall | 46: gravity = fall gravity when 0; calls the step at once |
| 5 | `0x8003FBF8` | 44 | 96 at frame 2 when the facing side is not blocked, sub-step 2; else sub-step 0 | `+0x8E`, `+0x8F` 0 |
| 11 | `0x8003FB48` | 39 | 95 | latch 1, weapon object (1, 11, 0), effect (1, 27, 9), `+0xA6` 1 |
| 12 | `0x8003FBBC` | 43 | 95 | latch 1 |
| 13 | `0x8003FAFC` | 40 | 97 | sound (1, 26), latch 1 |
| 14 | `0x8003FC88` | 75 | — | `game+0x12..0x19` = 1 (all classes frozen), `game+0x1C` 1, latch 1, `+0xE1` 0 |
| 16 | `0x8003FCD4` | 42 | 95 | latch 1, `+0xA6` 2 |
| 17 | `0x8003FD18` | 41 | 95 | `+0xE0` 2, terrain box 0, `no_hit` 1, player lock 1, latch 1 |
| 46 | `0x8003FF1C` | 38 | 185 ground / 186 air (act4 0/1) | `+0x86` 10, effect (1, 27, 6) |
| 56 | `0x8003FE74` | 76 | 187 (state `0x0B00` or step 47) / 183 / 184 (act4 2/0/1) | latch 1 |

## X step routines

- 32 `0x801EB730`: `LadderShotExit`, `XLadderShoot`, `AnimeTick`; pressed left/right sets the
  facing; when `+0x90` ≠ 0: `PlayerSetAnimeAt(32, 3)` and `ToLadderDown`.
- 33 `0x801EB7C8`: lock → fall; `AirWeaponPose`; `AnimeTick`; sub-steps `0x801F36F8` (7).
  Each sub-step 1..6 first runs `0x801EC35C`: `+0xD9` − 1; at 0, or jump pressed again
  (`+0x80` bit 7): `ToFall`. Then `TryWall`. Then `0x801EC3AC`: y.px += `0x801F3714[+0xDA]`
  (16 bytes, `+0xDA` cycles 0..15), `speed_x` 0; left or right held and that side free:
  `speed_x` = ± `+0xF4` (2.0), returns +1 for the facing side, −1 for the other. Sub 0: at
  frame flag `0x80`, effect (33, 33, 0), sub 1. Sub 1 (first hover): `TryDashInput` and
  `+0x92` = 0 → `StartAirDash`; a direction → `+0xD9` = 40 and sub 3 (forward, 23) or 5
  (back, 25). Sub 2: hover (22). Sub 3/5: `PlayerMove`; direction gone → 24/26 (sub 4/6); other
  direction → the other move. Sub 4/6: at the animation end, 22 and sub 2.
- 34 `0x801EB844`: lock → palette restore (`0x8003C4EC`), `+0xE0` 0, `StandOrFall`. Sub-steps
  `0x801F3724`: helper `0x801EC678` (ceiling: `speed_y`, gravity 0; facing side blocked:
  `speed_x`, `accel_x` 0, returns 1). Sub 0: `PlayerMoveAccel` until frame flag `0x80`. Sub 1:
  blocked and flag `0x40` → `no_hit` 0 (when not locked), `+0xE0` 0, lock off, `StandOrFall`;
  flag `0x20` → weapon object (1, 22, 0), palette 73 (`0x8003CF54`), sound (1, 18), `speed_x`
  ± 8.0, `accel_x`, `speed_y`, gravity 0, sub 2. Sub 2: `PlayerMove` for `+0xE1` updates (24
  from the entry), or until blocked; then `no_hit` 0, `+0xE0` 0, lock off, palette restore,
  `StandOrFall`.
- 35 `0x801EB8B8` (air.md): floor → `ToLand`; lock → fall; `TryHang`; `TryLadder`; sub 0: at the
  animation end, 172 (`speed_y` > 0), 173 (< 0), 171 (0); `+0x8C` 1; effect `0x8003D26C`; sub 1:
  `FalconDashStep`, at its end `ToFall`.
- 36 `0x801EB968`: sub-steps `0x801F3738`. Sub 0 `0x801EA0C4`: at `+0xE1` = 0 a screen effect
  starts (`0x8002ABA8`, fade fields `0x800CF850..`, `0x80097420..`, `0x80090D56..`, `+0xE4` bit 0,
  15 words saved to `0x801F4DE8`); counts `+0xE1` to 51 while the fade rises by 2 a step to 31;
  then animation 178 (when the animation is ≥ 177, the air version) or 175, sound (1, 18),
  `+0xE1` = 3 + RNG % 3, sub 1. Sub 1 `0x801EA2B8`: every 3–5 updates a weapon object
  (1, 49, var 0/1 alternating); `act4` counts to 121, then sub 2 with 179 / 176. At `0x801F4DD9`
  = 2: effect objects (1, 17, sprite 85/86 for X, 83/84 for Zero) in 5 pairs; after `act4` 115
  the fade goes down by 8. Sub 2 `0x801EC93C`: at the animation end, `no_hit` 0 and lock off
  (when `+0xD0` = 0), `PlayerSetTerrainBox`, `+0xE0` 0, sub 3. Sub 3 `0x801EC9A0`: palette restore,
  `StandOrFall`.
- 37 `0x801EB9B0`: sub-steps `0x801F3748`. Sub 0 `0x801EA5A0`: the same fade, 51 updates, then
  `0x801F4DD9` = 2, sub 1. Sub 1 `0x801EA724`: when `0x801F4DD9` reaches 0: 5 + 1 effect objects
  (1, 17, sprite 85, var 1/2 by the facing), two weapon objects (1, 47, var 0) and (1, 47, var 1);
  then each update `AnimeTick` and `0x801EC6D0(50|51)` (weapon object 50 ground / 51 air at frame
  flag `0x80`); `+0xE1` to 149 → sub 2; from 143 the fade goes down. Subs 2, 3 as 36.
- 38 `0x801EB9EC`: lock or `+0x14C` = 2 → `StandOrFall`; `AnimeTick`; `0x801EC6D0(50|51)` by
  act4; frame flag `0x01`: weapon object (1, 46) with the animation in `+0x84`; animation end →
  `StandOrFall`.
- 39 `0x801EBC58`: `AnimeTick`; sub 0 at flag `0x80`: effect (9, 23, 0, sprite 3), sub 1;
  animation end → `StandOrFall`.
- 40 `0x801ECA84`: lock → `StandOrFall`; `AnimeTick`; holds while `+0x99` ≠ 0 and `+0x93` = 4;
  else `StandOrFall`.
- 41 `0x801EBEA4`: sub-steps `0x801F3758`. Sub 0 `0x801ECAF4`: at the animation end, sound (1, 37)
  and 10 weapon objects (33, 17, var 0/1) in a row from x − 64, 32 px apart, at camera y + 368 and
  + 528; sub 1. Sub 1 `0x801ECC38`: when `+0x99` = 0: `+0xE0` 0, lock and `no_hit` off (when not
  locked), `PlayerSetTerrainBox`, `StandOrFall`.
- 42 `0x801EBE28`: holds while `+0x99` ≠ 0 and `+0x93` = 7, and until the animation ends in
  sub-step 1; then `StandOrFall`.
- 43 `0x801EBCF4`: the same with `+0x93` = 3.
- 44 `0x801EBD70`: sub-steps `0x801F3760`, and `StandOrFall` when `+0x93` ≠ 5. Sub 0: when
  act4 ≠ 0 (set by the weapon object [likely]), animation 96, sub 1. Sub 1: at flag `0x80`,
  sub 2. Sub 2: at the animation end, `StandOrFall`.
- 45 `0x801EBAE0`: lock or `+0x14C` = 2 → `StandOrFall`; at flag `0x80` weapon object (1, 50)
  with animation and facing; at the end `StandOrFall`; slot `+0x93` = 0 and flag `0x40`:
  `TryDashJump`, `TryCrouch` (the slash can be cancelled into a jump or crouch).
- 46 `0x801EAA38`: the air slash: weapon object (1, 51) at flag `0x80`; floor → `StandOrFall`;
  ceiling: `speed_y` ≤ 0; flag `0x40`: `TryWall`, `TryAirDash`; `AirControl`,
  `WaterSplashCheck`; latch −1 when falling; at the animation end, 10/step 6 (rising) or 11/step 7.
- 47 `0x801EACA0`: the wall slash: weapon object (1, 51) at flag `0x80`; floor → `StandOrFall`;
  no wall or locked → fall; `TryWallKick`; `WallHeld` (else `WallRelease`); flag `0x40`:
  `AirWeaponPose`; `PlayerMove`; at the animation end, 15 and step 11.
- 74 `0x801EBBEC` (Mach Dash): lock → `StandOrFall`; sub-steps `0x801F3778`. Sub 0
  `0x801EAECC`: facing from the held direction; at the animation end (189), while jump or dash
  is held (`+0x7C` & `0x180`) wait up to 120 updates (`+0xD9`); then sound (1, 5); up held:
  190, `speed_y` = 2 × `+0x108` up; down held: 191, `speed_y` = 2 × `+0x108` down; else 192,
  `speed_x` = 2 × `+0x108` forward. `+0x8C` 1, `no_hit` 1, effect (33, 25, 0), sub 1. Sub 1
  `0x801EC754`: `PlayerMove`; `+0x85` − 1; below 8, `no_hit` 0; at 0, or blocked in the move
  direction: sound (1, 6), `no_hit` 0; a down dash that hits the floor with down held: latch 0,
  `+0x86` 0, `ClearDashFlags`, `StartCrouch`; else `StandOrFall`. Entry `+0x85` = 15, so 15
  moves of 8.25 px = 123.75 px [certain for the arithmetic].
- 75 `0x801EBDCC`: sub-steps `0x801F376C`; `StandOrFall` when `+0x93` ≠ 5. Sub 0: `+0xE1` + 1;
  when act4 ≠ 0, sub 1. Sub 1: `+0xE1` to 60, then `game+0x12..0x19` = 0, `game+0x1C` = 0,
  sub 2. Sub 2: `StandOrFall`.
- 76 `0x801EB02C`: `+0x14C` = 2 → `StandOrFall`; at flag `0x80` weapon object (1, 56); then the
  slash object 50 (animation 183) or 51 at flag `0x80`; at the end, act4 2 → 15 and step 11 (wall),
  else `StandOrFall`.
- 77 `0x801EBEEC`: sub-steps `0x801F3780`. Sub 0 `0x801EB188`: the fade (51 updates), sub 1.
  Sub 1 `0x801EB31C`: effects as 37; weapon object 50 (act4 0) or 51 at flag `0x80`; at
  `+0xE1` = 2 and the animation end the fade goes down, sub 2. Subs 2, 3 as 36.

## Entries of the Zero steps

| step | entered by | condition |
|---|---|---|
| 48 | `0x801EBE54` (ground tries of stand, walk, dash, land …) and `0x801EBC60` | attack (`0x801ECCD4`): animation 88, sound (1, 7), voice 3 (RNG bit 2) |
| 49 / 53 | `0x801EBF94` (from step 6 jump and `0x80036D4C` wall kick) | attack in the air; 53 when `+0x86` = 6 (after the super jump): animation 96 / 91 |
| 50 / 54 | `0x801EC028` (fall, wall slide `0x80036FBC`, air dash subs) | the same in a fall |
| 51 | `0x801EC0F0` (ladder 15, 16) | attack on a ladder: 93 |
| 52 | `0x801EC178` (step 11 `0x80036EB0`) | attack in the wall slide: 15 |
| 55 | `0x801EC86C` (ground tries) | skill `0x04`, up held, attack: 90, effects 26, 34 |
| 56 | `0x801EA34C` (from 50 and `0x801EC028`) | skill `0x08`, down held, attack in the air: 98, voice 5, sound (1, 23), slot 2 |
| 57 | `0x801EA454` (jump 6, fall 7) | skill `0x10`, up held, attack in the air: 101, `no_hit` 1, `speed_x` ± 3.0, `speed_y` −5.0 |
| 58 | `0x801EA560` (ground and air tries) | skill `0x20`, down held, pressed `0x20`, `+0x86` ≠ 8: 103, `no_hit` 1, `+0x86` 8 |
| 59 | `0x801EA680` (from 48, `0x801EBC60`, `0x801EBE04`) | skill `0x80`, pressed `0x40`, energy slot 5 ≥ cost: 104, terrain box 0, lock, `no_hit`, `+0xE0` 2 |
| 60 | `0x801EC1F8` (crouch 26) | attack while crouched: 81 |
| 61 | `0x801EA004` (rope 27), `0x801EA124` (bar 28) | attack in a hang |
| 62 | `0x801EBEDC` (ground tries) | pressed `0x20`: 110, speeds 0 |
| 63 | `0x801ECA50` (ceiling hold `0x800365D4`) | attack on the ceiling: 84 |

Skill bits without a step: `0x01` (`0x801EA254`, up held + pressed `0x20`: weapon object (1, 1),
slot 3 energy), `0x02` (super jump, air.md), `0x40` (`0x801ECBE8`: weapon object (1, 7), slot 4
energy; no caller in the walk).

Zero step bodies (mechanical summary, `scripts/stepsum.py`): 48 → 55 or 48 again (combo),
`TryDashJump`, `TryCrouch`, `0x801EA680`; 49/53 → 50 or step 7, animation 11 at the end; 50/54 →
`ToLand` → 48 or step 7; 51 → `ToLadderUp`; 52 → step 11; 56 → sub 2 (99, energy, weapon object,
`PlayerMoveAccel`) → 48 on landing; 57 → `PlayerMove` down, on the floor sound (1, 26), `Quake`,
`Quake2`, 102, then stand; 58 → 103, `StandOrFall`; 59 → fade, effects, 111, terrain box,
`StandOrFall`; 60 → 81, step 26; 61 → step 27 (`StartJump` on exit); 62 → weapon object, `TryLand`;
63 → 84, step 31.

## Forms

### Where the value comes from

- `PlayerLoadForm` `0x8003D718` and the no-beam set-up `0x8003C1AC` (code at `0x8003C210`):
  `mega+0xE7` = `game+0x5E`. Only these two read `game+0x5E` in the player code.
- New game, overlay 19 (`0x800E9AEC`): `game+0x38` = 0 (X): `game+0x5F` = 1, `game+0x5E` = 1.
  Else `game+0x5F` = `0x10`, `game+0x5E` = 5. Cheat (input counter `0x800EA760[character]` = 11
  at confirm): X → `game+0x5F` = 8, `game+0x5E` = 4; Zero → `game+0x5F` = `0x31`.
- Armour select, overlay 16 `0x800E9898`: list `0x800F1CB0`, count `0x800F1CEB`. Always
  `[0]` = 0; bit `0x01` → 1; bit `0x10` → 5; bit `0x02` → 2; bit `0x04` → 3; bit `0x08` → 4.
  Confirm (`0x800EB174`): `game+0x5E` = list[`game+0x30`]; 5 → `game+0x38` = 1, else 0.
- `0x800213FC` copies `game+0x38`, `+0x5E`, `+0x0C`, `+0x0D` from `0x801F600C` (a saved copy).
- The Light capsule (light-capsule.md) gives the Ultimate Armor only when `mega+0xE7` = 0 or
  `game.armors` bit 3: X must wear no armour.

### State comparison

| field | `sheldon-4_no-armor` | `sheldon-1_lemons` (Falcon) |
|---|---|---|
| `mega+0xE7`, `game+0x5E` | 0, 0 | 1, 1 |
| `game+0x5F`, `+0x60`, `+0x69` | `0x11`, `0x21`, `0x08` | the same |
| `mega+0x30` | `0x801F4AD4` | `0x801F4AD4` |
| `mega+0x38` | `0x80103ACC` = member 86 + `0x2504C` | `0x80103ACC` = member 87 + `0x2504C` |
| `mega+0x3C` (frame 119 / 85) | `0x80143A24` = member 86 + `0x97880` | `0x80147C68` = member 87 + `0x9C080` |
| frame directory `scratch+0x14` | `0x80103A80` = member 86 + `0x25000` | member 87 + `0x25000` |
| sprite directory `scratch+0x1C` | `0x801439A4` = member 86 + `0x97800` | `0x80147BE8` = member 87 + `0x9C000` |
| `+0xF0..+0x120` | 0.5, 2.0, 5.8125, −6.5, 0.2578, 6.5, 4.125, −0.1875, −1.4375, 5.625, 1.0, 1.5, 2.0 | the same |
| `+0xEC` | 0 | 0 |
| overlay 0 | SHA-1 `69773d2a…` | the same |

Both members load at base `0x800DEA80`. The member is the whole difference in the player data.

### Member load `0x80013BF0`

```
if game+0x38 != 0: load(85)
else: load({1: 87, 2: 88, 3: 89}.get(game+0x5E, 86))   ; form 0 and 4 -> 86
```

No other call of the loader `0x80014F70` takes 85–89. `PlayerSetAnime` `0x8003BA04` and the
palette code (`0x8003CF54`, `0x8003C4EC`) do not test the form; they read the directories of the
loaded member. So forms 0 and 4 have the same frames and palettes [certain for these paths].

Frame sets in the members (`scripts/memberscan.py`): each X member has one 228-frame set at about
`+0x2504C` and a second large set: 86: 66, 87: 93, 88: 94, 89: 76 frames. The no-armour export
skips the 22 animations that use set-1 sprites ≥ 66 (83–85, 140, 169–182, 189–192).

### Form readers

| reader | test | effect |
|---|---|---|
| `TryAirDash` `0x8003906C` | 1, 3, 4 | `FalconAirTry`, `BladeAirTry`, `UltimateAirTry` / air dash |
| `PlayerHitCheck` `0x80039228` (`0x800393A8`) | 1–4 | hit class 4 |
| `TrySuperJump` `0x8003A870` | 2 | super jump |
| `StartWallSlide` `0x8003B548` | 2 | no dust effect |
| `0x8003C3C8` (per update) | 1, 2, other | hurt box table `0x801F3D4C`, `0x801F3E4C`, `0x801F3C4C` (set 0); set ≠ 0: `0x801F3B4C`; entry 0 → no hurt box; entries index `0x801F3F4C` |
| `PlayerPhysicsParts` `0x8003D630` | 2 | `+0x110` = 0 |
| `PlayerLoadForm` `0x8003D718`, `0x8003C210` | 2 | `+0xC9` = 0, effect (1, 27, 5), `0x8003CF24(2, 25, 1)`, `0x8003CF24(2, 24, 1)` (palettes 25 and 24 from `scratch+0x24` [likely]) |
| `0x8003DF58` | 2 | with `+0x144` = 6 (shot code; attacks agent) |
| `0x8003E344` | 1–4 | special attack, steps 36, 37, 77, 34 |
| `0x8003EB0C(slot)` (weapon cost) | 1, 3, 4 | X: 6 × `0x80073D74[slot]`; part bit `0x20`: half; else forms 1, 3, 4: × 2/3 (`0x55555556` multiply of 2 × cost); minimum 1 (0 stays 0). Zero: 6 × `0x80073DF4[slot]`, 3 × with part bit `0x20`, 4 × with `game.armors` bit `0x20` |
| `0x8003EC78` | 4 | with `+0xE0` ≠ 0: the shot update does not run |
| `0x8003EEE4` (charge of a special weapon) | 1, 3, 4 | `+0x9B` = 1 at 25 updates (12 with part bit `0x100000`), 2 at 90 (45); forms 0, 2 never |
| `0x8003F200` | 4 | step 33 |
| `0x8003F7BC` | 4, 1, 3 | `+0x94[slot]` = 20, 19, 43; else 18 (shot type at full charge [likely]) |
| `0x8003FD6C` | 2 | animations 185/186 instead of 183/184 |
| `0x801ED36C` (weapon type 21) | 4 | up to 3 objects of type 21 at a time, else 1 |
| `0x801EDCB0` (weapon object) | 2 | other `SetAnime` |
| `0x80030B5C`, `0x80030CA4` | 2 | spike tiles do not set `+0x79` |
| `0x80021B40`, `0x800228AC` | 0, 4, 1, 2, 3 | index 0, 2, 3, 4, 5; Zero 1, or 6 with `game+0x5F` bit `0x20` (icon or portrait [guess]) |
