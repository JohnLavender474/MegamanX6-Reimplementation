# Player air movement — pseudo code (part 2b)

Source: `~/mmx-data/x6/re/x/kit/code.txt` (X kit, `intro-1_start`), `movement/fn.sh <addr>`, and
`scripts/rangedis.py` for tables and code outside the kit. `m` is `mega` (`0x800970A0`).
`held` = `m+0x7C`, `pressed` = `m+0x80`, `blocked` = `m+0x89` (1 right, 2 left, 4 ceiling,
8 floor). Speeds are 16.16 px/update. Part 2a names (`PlayerMove`, `PlayerMoveAccel`, `DashStep`,
`TryDashInput`, `ToLand`, `ClearDashFlags`, `PlayerSetAnimeShoot`, `PlayerSyncShootAnime`) are in
`../movement/pseudocode.md`.

## Dispatch

- `player_steps` (`0x80073ACC`) has more than 32 entries: 32–63 point into the player overlay
  (for example 33 `0x801EB7C8`, 35 `0x801EB8B8`), 64–71 to stand, 72 `0x800E9B40`,
  73 `0x80037E1C`, 74–77 overlay (74 `0x801EBBEC`). `PlayerUpdate` has no bound test. The table
  is the same in X and Zero states (main program). The sub-step tables of steps 12 and 13 follow
  it (`0x80073C04`, `0x80073C10`).
- `air_dash_steps` (`0x80073C10`): `AirDashStart` `0x80037220`, `AirDashRun` `0x800372DC`,
  `AirDashFall` `0x8003737C`.
- `super_jump_steps` (`0x80073C58`): 0 `0x8003649C`, 1 `0x80038248`, 2 `0x800382C8`,
  3 `0x800365D4`, 4 `0x80038310`, 5 `0x80038390`.
- Falcon (step 35) sub-steps `0x801F3730`: 0 `0x801EC878`, 1 `0x801EC908`.

## Every update (before the step)

```
CheckWalkOff (0x80039dfc), from PlayerUpdate:
  if step in (0, 1): return
  if m.latch(67) or m.A4 > 0 or m.D5 < 0 or m.gate(D4): return
  if blocked & 8: return
  if m.var: ClearShotPose(m)                 ; 0x8003cdbc
  ToFall(m); PlayerSetTerrainBox(m)
```

`latch` (`+0x67`): 1 rising (`StartJump`, `StartWallKick`), −1 in the air (every fall entry),
0 on the ground (`ToLand`, `LandOrFall`). So there is no grace time after a ledge.

`+0x4A` (`wall_ok`): `PlayerUpdate` clears it after the step; the side-wall tile test
(`0x80030C58`, jump table `0x80010F18`, tile types 56–63) sets it again:

| tile | result |
|---|---|
| 56, 58 | solid, `wall_ok` = 1 |
| 57, 60 | solid, `wall_ok` = 0 |
| 59, 61 | not solid |
| 62, 63 | form 2: solid, `wall_ok` = 1; else solid, `wall_ok` = 0, `+0x79` = 1 (spike: `0x80039228` sets HP to the death mark when `iframes` = 0 and `no_hit` ≠ 1) |

## Entries

```
StartJump (0x8003af1c):
  latch = 1; speed_x = accel_x = 0; speed_y = m.F8 (5.8125); gravity = m.134
  if m.84 or held & 0x100:  m.84 = 1; m.86 = 2; m.8C = 1      ; dash jump
  m.8B = 10
  PlayerSetAnimeShoot(m, 10)
  if GetRNG() & 4: PlayVoice(m, voice_tbl[var*4 + (GetRNG() & 3)])   ; 0x80073ce4
  AirControl(m)                                 ; the first move happens here
  step = 6; step2 = 0

ToFall (0x8003aff4):
  PlayerSetAnimeShoot(m, 11)
  speed_x = accel_x = speed_y = 0; latch = -1; gravity = m.100
  if m.84: m.84 = 0; m.8C = -1
  step = 7; step2 = 0

PlayVoice (0x8003cfe0)(m, id): if m.8D <= 0: PlaySound(3, id, m)

AirControl (0x8003d18c):
  if lock: speed_x = 0; PlayerMoveAccel(m); return
  spd = m.84 ? m.108 (4.125) : m.F4 (2.0)
  if held & 1:   flip = 0x40; speed_x = blocked & 1 ? 0 : +spd
  elif held & 2: flip = 0;    speed_x = blocked & 2 ? 0 : -spd
  else:          speed_x = 0
  PlayerMoveAccel(m)                            ; x, y, gravity, fall clamp

SplashCheck (0x8003d394):
  if TileAtFeet(m) (0x8002ff70) == 36: WaterSplash(m, feet y & ~15)   ; 0x8003ca70
WaterSplash: once (m.DF latch): effect type 65 var 9 (sprite bank 139), PlaySound(5, 5)
```

## Step 6 — jump (`0x80035620`)

```
if lock: speed_y = 0
if blocked & 4: speed_y = 0                    ; ceiling
if not (held & 0x80): speed_y = 0              ; jump released: the rise stops
X: if AirWeaponPose(m): return                 ; 0x8003e174
Zero: if 801ea560 or 801ea454 or 801ebf94: return
AnimeTick
if anime.flag & 0x80: clear; PlaySound(1, 1)   ; animation 10 frame 1
if TryLadderAir(m): return                     ; 0x8003a488
if TryHang(m): return                          ; 0x80039984
if m.8B == 0: if TryWall(m): return            ; 0x8003a2cc
else: m.8B -= 1
if TryAirDash(m): return                       ; 0x8003906c
AirControl(m); SplashCheck(m)
if speed_y <= 0: PlayerSetAnimeShoot(m, 11); latch = -1; step = 7; step2 = 0
else: PlayerSyncShootAnime(m, 10)
```

The change to step 7 keeps `gravity` (the jump gravity `+0x134`) and the dash flag `+0x84`.

## Step 7 — fall (`0x800368fc`)

```
if blocked & 8: if not TrySuperJump(m): ToLand(m); return
X: AirWeaponPose; Zero: 801ea560, 801ea454, 801ec028
TryLadderAir, TryHang, TryWall, TryAirDash     ; the first that returns 1 ends the update
AnimeTick; AirControl; SplashCheck; PlayerSyncShootAnime(m, 11)
```

## Wall

```
TryWall (0x8003a2cc):
  if lock or not (blocked & 3): return 0
  if TryWallKick(m): return 1
  if WallHeld(m): StartWallGrab(m); return 1
  return 0

TryWallKick (0x8003a33c): lock → 0; pressed & 0x80 and m.4A > 0 → StartWallKick; 1
WallHeld (0x8003a394):    lock → 0; m.4A != 0 and (held & 3 & blocked) → 1

StartWallGrab (0x8003b3b8):
  PlayerSetAnimeShoot(m, 13); PlaySound(1, 4)
  m.8C = -1; speed_x = accel_x = speed_y = gravity = 0; m.84 = m.86 = 0
  m.8A = 8; flip = blocked & 1 ? 0x40 : 0      ; face the wall
  step = 9

Step 9 (0x80036b10):
  blocked & 8 → ToLand
  lock → ToFall
  AirWeaponPose → return
  not (blocked & 3) → ToFall
  TryWallKick → return
  AnimeTick
  anime.flag & 0x80 (animation 13 frame 1): flip = blocked & 1 ? 0 : 0x40   ; face away
  m.8A -= 1; at 0: StartWallSlide(m)
  else PlayerSyncShootAnime(m, 13)

StartWallSlide (0x8003b548):
  PlayerSetAnimeShoot(m, 15)
  if form != 2: WallSlideDust(m)               ; 0x8003d2bc: effect type 33 var 0
  latch = -1; speed_x = accel_x = gravity = 0; speed_y = m.110 (-1.4375; 0 in form 2)
  ClearDashFlags; step = 11; step2 = 0

Step 11 sub 0 (0x80036eb0):
  blocked & 8 → ToLand; lock → ToFall
  X: AirWeaponPose; Zero: 801ea560, 801ec178
  not (blocked & 3) → ToFall
  TryWallKick → return
  if not WallHeld: WallRelease(m); return
  AnimeTick; PlayerMove (no gravity); SplashCheck; PlayerSyncShootAnime(m, 15)

WallRelease (0x8003b5b8):
  PlayerSetAnimeShoot(m, 11)
  speed_x = blocked & 1 ? -m.F4 : +m.F4        ; 2.0 away from the wall
  latch = -1; m.8A = 4; accel_x = speed_y = 0; gravity = m.100
  step = 11; step2 = 1

Step 11 sub 1 (0x80036fbc):
  blocked & 8 → ToLand; lock → ToFall
  X: AirWeaponPose; Zero: 801ea560, 801ec028
  AnimeTick; PlayerMoveAccel; SplashCheck
  m.8A -= 1; at 0: speed_x = 0; step = 7 (no new animation, no new gravity)
  else PlayerSyncShootAnime(m, 11)

StartWallKick (0x8003b440):
  PlayerSetAnimeShoot(m, 14); PlaySound(1, 3); PlayVoice(m, 2)
  latch = 1
  if held & 0x100: m.84 = m.86 = m.8C = 1      ; dash wall kick
  else: ClearDashFlags
  spd = m.84 ? m.108 (4.125) : m.F4 (2.0)
  if blocked & 1: flip = 0x40; speed_x = -spd  ; face the wall, move away
  else:           flip = 0;    speed_x = +spd
  accel_x = 0; m.8A = 10; speed_y = m.114 (5.625); gravity = m.134
  WallKickSpark(m)                              ; 0x8003c964: effect 33 var 3, offset 0x80073d6c
  step = 10; step2 = 0

Step 10 (0x80036c28): step2 == 0 ? KickPush : KickRise
KickPush (0x80036c68):
  lock → ToFall; AirWeaponPose → return
  if blocked & 4 or blocked & (flip ? 2 : 1): ToFall    ; ceiling, or a wall in the move direction
  AnimeTick; PlayerMoveAccel (no control, no release cut); SplashCheck
  m.8A -= 1; at 0: speed_x = 0; step2 = 1
  else PlayerSyncShootAnime(m, 14)
KickRise (0x80036d4c): as step 6 without the jump sound and without the m.8B delay
  (release cut, ceiling, AirWeaponPose / Zero 801ebf94, TryLadderAir, TryHang, TryWall,
  TryAirDash, AirControl, SplashCheck; speed_y <= 0 → animation 11, latch -1, step 7;
  else PlayerSyncShootAnime(m, 14))
```

## Air dash

```
TryAirDash (0x8003906c):
  if lock or m.86: return 0
  X:
    form 1: return FalconTry(m)                 ; 0x8003f28c
    form 3: if BladeTry(m): return 1            ; 0x8003f32c
    else:   if UltimateTry(m): return 1         ; 0x8003f200 (does something only in form 4)
    if m.92 or form != 4: return 0
  Zero: if 801ec298(m): return 1
  if lock: return 0
  if m.87 == 0 and not (pressed & 0x100): return 0
  flip from held; if blocked on the facing side: return 0
  speed_x = flip ? +m.104 : -m.104              ; 6.5
  StartAirDash(m); return 1

StartAirDash (0x8003b298):
  PlayerSetAnime(m, 18)                         ; no shooting version
  speed_y = gravity = 0; m.84 = -1; accel_x = m.10C (-0.1875)
  m.85 = parts & 0x10 ? 10 : 18; m.86 = 3
  ClearShotPose(m)                              ; 0x8003cdbc: m.8E..m.92 = 0, m.A2 = 0, 0x8003c4ec
  step = 13; step2 = 0

Step 13 (0x80037130):
  blocked & 8 → ToLand; lock → ToFall
  TryHang → return; TryLadder (ground ladder test) → return
  X: AirWeaponPose → return; if m.8E (shot pose): AirDashShot(m); return
  AnimeTick; air_dash_steps[step2](m)

AirDashStart (0x80037220):
  Zero: if 801ec028: speed_x = accel_x = speed_y = 0; latch = -1; m.84 = 0; gravity = m.100; return
  flag 0x40: AirDashJet(m)                      ; 0x8003d26c: effect type 33 var 1
  flag 0x80: PlaySound(1, 5); m.8C = 1; step2 = 1
  (no move: X hangs in the air)

AirDashRun (0x800372dc):
  Zero: as above
  if DashStep(m): AirDashStop(m)               ; wall in front, timer 0, no dash/direction, opposite
  flag 0x40: AirDashJet(m)

AirDashStop (0x8003b30c):
  PlayerSetAnime(m, 19); latch = -1; m.8C = -1
  speed_x = accel_x = speed_y = 0; m.84 = 0; step2 = 2; gravity = m.100

AirDashFall (0x8003737c):
  blocked & 8 → ToLand; Zero as above
  PlayerMoveAccel; SplashCheck
  if anime.next < 0 or held & 3: PlayerSetAnime(m, 20); step = 7

AirDashShot (0x8003b360):
  PlayerSetAnime(m, 132); latch = -1; speed_x = accel_x = speed_y = 0
  m.84 = m.8C = 0; gravity = m.100; step = 7
```

## Armour air actions (X)

```
FalconTry (0x8003f28c), form 1:
  if not (pressed & 0x80) and not TryDashInput(m): return 0
  StartAirDash(m); speed_y = 0; PlaySound(1, 5)
  step = 35; PlayerSetAnime(m, 169)
  MakeObject(1, 23, 0, m) (0x8003d3f8); MakeEffect(1, 18, 0, 0) (0x8003d510)
  return 1
  ; with the jump button, speed_x keeps its air value; FalconDashStep raises it to 4.125

Step 35 (overlay 0x801eb8b8): ToLand on the floor; ToFall when locked; TryHang; TryLadder;
  AnimeTick; sub 0 (0x801ec878): when the animation ends, animation 171 (speed_y = 0),
  172 (up) or 173 (down); AirDashJet; m.8C = 1; sub 1
  sub 1 (0x801ec908): if FalconDashStep(m): ToFall(m)

FalconDashStep (0x8003a0e8):                     ; as DashStep, but release does not end it
  if blocked on the facing side: return 1
  PlayerMoveAccel; clamp speed_x to at least m.108 (4.125) in the facing direction
  m.85 -= 1; at 0: return 1
  return the opposite direction is held

BladeTry (0x8003f32c), form 3:
  if not (pressed & 0x80) and not TryDashInput(m): return 0
  PlayerSetAnime(m, 189); m.84 = 1; speed_x = accel_x = speed_y = gravity = 0
  m.85 = 15; m.86 = 7; m.D9 = 0; ClearShotPose(m)
  step = 74; step2 = 0; MakeObject(33, 32, 0, m); return 1

UltimateTry (0x8003f200):
  if form != 4 or not (pressed & 0x80): return 0
  speed_x = accel_x = speed_y = gravity = 0; m.84 = 0; m.86 = 4; m.8C = 0
  m.D9 = 180; m.DA = 0; PlayerSetAnimeShoot(m, 21); step = 33; return 1
```

## Hang on a rope or a bar

```
TryHang (0x80039984):
  if lock: return 0
  if m.E3: m.E3 -= 1; if m.E3: return 0
  if not (held & 4): return 0
  t = TileHang(m)                               ; 0x8003040c
  if t == 46:                                   ; rope
      X with shot pose: animation 144; else PlaySound(0, 38); m.8E = 0; animation 160
      x, y fractions = 0; x = (x & ~15) + 8; speed_x = 0
      step = 27; ClearDashFlags; return 1
  if t not in 37..45 or speed_y > 0: return 0    ; bars: only when not rising
  n = 1 or 2 (37..40 and 41..44, by flip), 0 for 45
  X with shot pose: animation 148 | n; else PlaySound(0, 38); animation 163 + n; m.8E = 0
  step = 28; speed_y = 0; BarSnapY(m, t) (0x800397f8); ClearDashFlags; return 1

HangExit (0x8003a764), first in steps 27 and 28 (after HangWeapon):
  if m.D0 > 0 (event): ToFall; return 1
  if TileAtHands (0x8002fe30) not in 37..47: m.E3 = 4; ToFall; return 1
  if pressed & 0x80: (held & 8 ? ToFall : StartJump); m.E3 = 4; return 1
  return 0

Step 27 (0x8003605c): flip from held; HangWeapon (0x8003e06c); HangExit; Zero 801ea560, 801ea004
  sub 0: AnimeTick (X weapon pose: 144 at the same frame); when next == 0: sub 1
  sub 1: RopeTryDash (0x80038ba8); RopeClimb (0x800389b4)
         when y changed: PlaySound(0, 39) every 21 updates (m.DE)
RopeClimb: m.84 → RopeDashStep (0x80038788)
           else up: speed_y = +m.128 (1.5); down: -m.128; probe PlayerMove, test the tile
           (46, and not the floor when going down), restore y, then PlayerMove for real
RopeTryDash: dash pressed (or m.87), m.84 == 0: speed_y = ±m.130 (4.125, up when up held,
           else down); probe; m.84 = m.86 = 1; m.85 = 30 (15); m.8C = 1; PlaySound(1, 5)
RopeDashStep: m.85 -= 1; the dash goes on while m.85 > 0 and the direction of the dash is held;
           else StopSound(1, 5), ClearDashFlags, climb

Step 28 (0x800361d4): as step 27 with Zero 801ea124
  sub 1: BarTryDash (0x80038f64): speed_x = ±m.12C (4.125) by flip; probe tiles 37..47;
         m.84 = m.86 = 1; m.85 = 30 (15); m.8C = 1; PlaySound(1, 5)
         BarMove (0x80038cd0): m.84 → 0x80038594 [not read]; else left/right: speed_x = ±m.124
         (2.0); probe; then BarSnapY on the new tile; animations 151 (weapon), 154 / 163+n /
         148+n
         when x changed: PlaySound(0, 39) every 21 updates
```

## Step 31 — super jump (`0x800381f0`)

```
TrySuperJump (part 2a): speed_y = 2 × m.F8 (11.625), speed_x = 0, animation 10, m.8B = 10,
  Zero: m.93 = 1, m.86 = 6; PlaySound(1, 1); step 31, step2 0, act4 0
Step 31: lock → ToFall; else super_jump_steps[step2]
0 (0x8003649c):
  if blocked & 4:
      if collide_flags & 4 and the tile above the head (0x80030304) == 57: ToFall; return
      PlaySound(1, 4); PlayerSetAnime(m, 83); step2 = 2; act4 = Zero ? 120 : 180; return
  act4 += 1; PlayerMove (no gravity, no control); AnimeTick; PlayerSyncShootAnime(m, 10)
  if (speed_y × act4) >> 16 >= 144: act4 = 5; step2 = 1; PlayerSetAnime(m, 11)
1 (0x80038248): AnimeTick on odd frame_count; act4 -= 1 until 0; then speed 0, gravity m.100,
  latch -1, step 7
2 (0x800382c8): AnimeTick; when next == 0: step2 = 3; PlayerSetAnime(m, 84)
3 (0x800365d4): ceiling hold. flip from held; TryDashInput → StartAirDash, animation 85, step 31,
  step2 4. AnimeTick. Down held, jump pressed or no ceiling: act4 = 0. While act4: act4 -= 1;
  X: 0x8003f700 [not read]; Zero: 801eca50. X: weapon pose → 140, pose end → 84.
  act4 == 0 and next == 0: LandOrFall (0x8003b114)
4 (0x80038310): as AirDashStart (flag 0x40 jet, flag 0x80 sound 5, m.8C = 1, step2 5)
5 (0x80038390): AnimeTick; DashStep → AirDashStop, step = 13 (sub 2); flag 0x40 jet
```

## Engine helpers called

| address | name | note |
|---|---|---|
| `0x8002FE30` | `TileAtHands` | tile type at the hang point |
| `0x8002FF70` | `TileAtFeet` | tile type under the terrain box |
| `0x8003015C` | `TileAtBody` | tile type at the body (ladder 32) |
| `0x8003040C` | `TileHang` | tile type for the hang test, with a y window by type |
| `0x80030304` | | tile at (x, y) |
| `0x80030C58` | | side-wall tile test; sets `+0x4A`, `+0x79` |
| `0x800397F8` | `BarSnapY` | y onto the bar by tile type |
| `0x8003A814` | `BarShape` | 0, 1, 2 by tile and flip |
| `0x8003CA70` | `WaterSplash` | |
| `0x8003CDBC` | `ClearShotPose` | |
| `0x8003D26C` | `AirDashJet` | effect 33 var 1 |
| `0x8003D2BC` | `WallSlideDust` | effect 33 var 0 |
| `0x8003C964` | `WallKickSpark` | effect 33 var 3 |
| `0x8003D3F8`, `0x8003D510` | `MakeObject`, `MakeEffect` | (a, type, var, parent) |
| `0x8001750C` | `StopSound` | |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x18`, `+0x1C` | `old_x`, `old_y` | position before this update; the hang probes restore to it |
| `+0x4A` | `wall_ok` | the side tile allows wall slide and kick |
| `+0x67` | `latch` | 1 rising, −1 in the air, 0 on the ground |
| `+0x79` | `spike` | spike wall touched |
| `+0x84` | `dash_a` | air speed 4.125 while not 0 (1 dash jump, −1 air dash) |
| `+0x85` | dash timer | air dash 18 (10), rope and bar dash 30 (15) |
| `+0x86` | `dash_b` | not 0 blocks the air dash: 1 ground/rope/bar dash, 2 dash jump, 3 air dash, 4 Ultimate, 6 Zero super jump, 7 Mach Dash |
| `+0x8A` | `wall_timer` | 8 grab, 10 kick, 4 push-off |
| `+0x8B` | `jump_work` | 10 updates without a wall grab after a jump |
| `+0x8C` | dash flag | |
| `+0x8D` | voice timer | voices only at ≤ 0 |
| `+0x8E` | shot pose | |
| `+0x92` | | X air dash only at 0 |
| `+0xD9`, `+0xDA` | | Ultimate (180) and Blade |
| `+0xDE` | hang sound timer | 21 |
| `+0xDF` | splash latch | |
| `+0xE3` | hang wait | 4 updates without a new hang |
| `+0xE7` | form | 1 Falcon, 2 Shadow, 3 Blade, 4 Ultimate, 5 Zero [likely names] |
