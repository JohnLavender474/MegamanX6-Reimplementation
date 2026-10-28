# Player ground movement — pseudo code (part 2a)

Source: `~/mmx-data/x6/re/x/kit/code.txt` (X kit, `intro-1_start`), plus `rekit --at` for the
engine helpers outside the kit (`PlayerMove`, `PlayerMoveAccel`). `m` is `mega` (`0x800970A0`).
`held` = `m+0x7C`, `pressed` = `m+0x80`. Speeds are 16.16 px/update.

## Dispatch

```
PlayerUpdate (0x80034FBC):                      ; state 1 of mega
  m.8F = m.90 = m.CF = m.DD = 0
  m.lock(D3) = m.D0 | game.exit_req | m.D4
  m.blocked(89) = m.collide_flags | m.71
  if game.stop[player] (game+0x10) == 0:
      if m.E5: m.E5 -= 1                         ; post-hit timer
      f_80039228(m)
      if m.hp == 0: return
      count down m.92, m.91 (shot timer; at 0: m.90 = 1, m.8E = 0), m.8D
      PlayerGateEnter(m); f_8003A268(m); f_80039F78(m); f_80039DFC(m); f_8003F048(m)
      player_steps[m.step](m)                    ; 0x80073ACC
      m.71 = m.72 = m.4A = 0
  if game.stop[player] != 2:
      f_8003EC78(m); f_8003CE38(m); f_8003C3C8(m)
```

## Physics set-up

```
PlayerPhysicsDefaults (0x8003D598):
  F0 = 0.5; F4 = 2.0; F8 = 5.8125; FC = -6.5; 100 = 0.2578125
  104 = 6.5; 108 = 4.125; 10C = -0.1875; 110 = -1.4375; 114 = 5.625
  118 = 1.0; 11C = 1.5; 120 = 2.0; 124 = 2.0; 128 = 1.5; 12C = 4.125; 130 = 4.125
  134 = 0.2578125

PlayerPhysicsParts (0x8003D630):                 ; called from step 0
  PlayerPhysicsDefaults(m)
  if m.parts(EC) & 0x8:  m.134 -= 0x1080
  if m.form(E7) == 2:    m.110 = 0
  if m.parts & 0x10:     m.104 += 0x1A000; m.10C += 0xC00
                         m.108 += 0x10800; m.12C += 0x10800; m.130 += 0x10800
  if m.parts & 0x4:      m.F0 += 0x4000;   m.F4 += 0x10000

PlayerLoadForm (0x8003D718):                     ; called from step 0 (start only read)
  m.form(E7) = game+0x5E
  m.parts(EC) = game+0x74 [m.var]                ; 4 bytes per character
  ...
```

## Animation helpers

```
PlayerSetAnimeShoot (0x8003F4C4)(m, n):
  if m.var == 0 and m.8E: n += 112               ; shooting version
  PlayerSetAnime(m, n)

PlayerSyncShootAnime (0x8003F508)(m, n):         ; every update of a step
  if m.var != 0: return
  if m.90:             target = n                ; shot pose just ended
  elif m.8E:           target = n + 112
  else: return
  if m.anime == target: return (when m.8E path)
  keep = m.anime.duration
  PlayerSetAnimeAt(m, target, m.anime.flag & 0x3F)
  m.anime.duration = keep

StandAnime (0x8003D10C)(m):
  limit = (s8) game[0x5B + m.var] / 3
  PlayerSetAnime(m, m.hp < limit ? 6 : 5)
```

## Movement helpers

```
PlayerMove (0x8002D5D0)(o):
  dx = o.speed_x
  if mega.wind and o.flip: dx = dx * 3 / 4
  o.x += dx; o.y -= o.speed_y

PlayerMoveAccel (0x8002CFB0)(o):
  if mega.wind and o.flip:
      o.y -= o.speed_y; o.speed_x += o.accel_x; o.speed_y -= o.gravity
      o.x += old_speed_x * 86 / 100
  else:
      o.x += o.speed_x; o.y -= o.speed_y
      o.speed_x = o.flip ? o.speed_x + o.accel_x : o.speed_x - o.accel_x
      o.speed_y -= o.gravity
  if o.speed_y < mega.fall_max: o.speed_y = mega.fall_max
```

## Tests (return 1 when they start something)

```
TryDirection (0x80039CFC)(m) / TryDirectionWalk (0x80039D7C)(m):   ; speed F0 / F4
  if m.lock: return 0
  if not (held & 3): return 0
  if held & 1: m.flip = 0x40; if m.blocked & 1: return 0; m.speed_x = +speed
  else:        m.flip = 0;    if m.blocked & 2: return 0; m.speed_x = -speed
  return 1

TryDashInput (0x80039EB0)(m):
  if m.lock: return 0
  if m.87 == 0 and not (pressed & 0x100): return 0
  if held & 1: m.flip = 0x40
  if held & 2: m.flip = 0
  side = m.flip ? 1 : 2
  if m.blocked & side: return 0
  m.speed_x = m.flip ? m.104 : -m.104
  return 1

TryDashJump (0x80039C40)(m):
  if TryDashInput(m): StartDash(m); return 1
  if pressed & 0x80:  StartJump(m) (0x8003AF1C); return 1
  return 0

TryLand (0x80039BC4)(m):                         ; land, dash end
  if TryDashInput(m): StartDash(m); return 1
  if pressed & 0x80:  StartJump(m); return 1
  if TryDirectionWalk(m): ToWalk(m); return 1
  return 0

TryCrouch (0x80039C9C)(m):
  if m.lock or not (game+0xC9 & 8) or not (held & 8): return 0
  StartCrouch(m); return 1

TryLadder (0x8003A3DC)(m):
  if m.lock or m.92: return 0
  if held & 4 and TileAbove(m) (0x8002FD3C) == 32: LadderFromGround(m) (0x8003B63C); return 1
  if held & 8 and TileBelow(m) (0x80030068) == 33: LadderDown(m) (0x8003B708); return 1
  return 0

TrySuperJump (0x8003A870)(m):
  if m.lock: return 0
  if m.var: if not (m.C9 & 2): return 0
  else:     if m.form != 2: return 0
  if not (held & 4) or not (pressed & 0x80): return 0
  m.latch(67) = 1; m.speed_x = 0; m.speed_y = 2 * m.F8
  PlayerSetAnimeShoot(m, 10); m.8B = 10; f_8003F3DC(m)
  if m.var: m.93 = 1; m.86 = 6
  PlaySound(1, 1, m)
  m.step = 31; m.step2 = 0; m.act4 = 0
  return 1

WeaponPose (0x8003F13C)(m):                      ; X only
  if m.lock or m.var: return 0
  if f_8003E344(m): return 1
  f_8003E614(m)
  if m.8F == 0: return 0
  if weapon_pose_tbl[m.weapon(96)] (0x80073E00) == 0: return 0
  m.84 = m.86 = m.8C = 0
  if m.weapon in (5, 14): PlayerSetAnime(m, 88)
  f_8003F864(m, 0)
  return 1
```

## Step entries

```
ToWalkStart (0x8003AE8C)(m): PlayerSetAnimeShoot(m, 7); m.speed_y = m.accel_x = m.gravity = 0
                             PlayerMove(m); m.step = 3; m.step2 = 0
ToWalk      (0x8003AED4)(m): as above with animation 8, step 4

ToStand (0x8003AD18)(m):
  ClearDashFlags(m)
  if PlayerRequests(m): return
  m.step = 2; m.step2 = 0
  if m.var == 0 and m.8E:                        ; X, shooting pose
      a = m.91 < 5 ? 2 : (m.91 < 9 ? 1 : 0)
      PlayerSetAnimeAt(m, 94, a)
      if m.91 >= 9: m.anime.duration = m.91 - 8
  else: StandAnime(m)

Stop (0x8003ADB4)(m):
  if PlayerRequests(m): return
  if m.var != 0: PlayerSetAnime(m, 9); m.step = 5; m.step2 = 0; return     ; Zero
  ClearDashFlags(m)
  if PlayerRequests(m): return
  (then as ToStand from "m.step = 2")

ClearDashFlags (0x8003CD9C)(m): m.84 = 0; m.86 = 0; if m.8C > 0: m.8C = -1

ToLand (0x8003B054)(m):
  PlaySound(1, 2, m); m.latch = 0; ClearDashFlags(m)
  if PlayerRequests(m) or WeaponPose(m): return
  if m.var and f_801EBE04(m): return
  if TryLand(m): return
  if m.var and f_801EBE54(m): return
  PlayerSetAnimeShoot(m, 12); m.step = 8; m.step2 = 0

StartDash (0x8003A9C4)(m):
  m.84 = 1; m.86 = 1
  if pressed & 0x80:                             ; dash jump
      m.latch = 1; m.8C = 1; m.86 = 2; m.speed_x = 0; m.accel_x = 0; m.8B = 10
      m.speed_y = m.F8; m.gravity = m.134
      PlayerSetAnimeShoot(m, 10)
      if GetRNG() & 4: f_8003CFE0(m, voice_tbl[m.var * 4 + (GetRNG() & 3)])   ; 0x80073CE4
      f_8003D18C(m); m.step = 6
  else:
      PlayerSetAnimeShoot(m, 16)
      m.speed_y = 0; m.gravity = 0; m.accel_x = m.10C
      m.dash_timer(85) = (m.parts & 0x10) ? 15 : 30
      m.step = 12
  m.step2 = 0

StartCrouch (0x8003B9AC)(m):
  if PlayerRequests(m): return
  PlayerSetAnime(m, (m.8C & 0xFFFF0000) ? 109 : 81)   ; high half of the 4 bytes at +0x8C
  m.step = 26; m.step2 = 0
```

## Steps

```
Stand (step 2, 0x80035274):
  PlayerSetTerrainBox(m)
  if PlayerRequests or TryLadder or WeaponPose: return
  if m.var and (f_801EC86C or f_801EA560 or f_801EBE04): return
  if TrySuperJump or TryDashJump: return
  if m.var and (f_801EBE54 or (not f_801EA254 and f_801EBEDC)): return
  if TryCrouch: return
  if TryDirection: ToWalkStart(m); return
  if m.var == 0: XIdle(m) (0x801EB61C) else ZeroIdle(m) (0x801EC9C4)

XIdle (0x801EB61C)(m):
  if m.8F: f_801EBF28(m)                          ; weapon pose
  elif m.90: StandAnime(m)                        ; shot pose ended
  elif m.anime == 6 and m.hp >= limit: PlayerSetAnime(m, 5)
  else: AnimeTick(m)

Walk start (step 3, 0x800353C0):
  if PlayerRequests or TryLadder or WeaponPose: return
  if m.var and (three Zero tries): return
  if TryDashJump: return
  if m.var and (two Zero tries): return
  if not TryDirection(m): Stop(m); return
  AnimeTick(m)
  if m.anime.next < 0: ToWalk(m); return
  PlayerMove(m); PlayerSyncShootAnime(m, 7)

Walk (step 4, 0x800354E8):
  as walk start, with TrySuperJump before TryDashJump, TryCrouch after the Zero tries,
  TryDirectionWalk instead of TryDirection, and no "next < 0" test:
  AnimeTick(m); PlayerMove(m); PlayerSyncShootAnime(m, 8)

Land (step 8, 0x80036A04):
  if PlayerRequests or TryLadder or TrySuperJump or WeaponPose: return
  if m.var and (f_801EC86C or f_801EBE04): return
  if TryLand: return
  if m.var and (f_801EBE54 or f_801EBEDC): return
  if TryCrouch: return
  AnimeTick(m)
  if m.anime.next < 0: Stop(m) else PlayerSyncShootAnime(m, 12)

Dash (step 12, 0x800370B0):
  if PlayerRequests(m) == 0:
      AnimeTick(m)
      dash_steps[m.step2](m)                      ; 0x80073C04
  if m.step != 12 and m.terrain_box: PlayerSetTerrainBox(m)

DashStart (0x800357A8):
  if WeaponPose: return
  if m.var and (four Zero tries): return
  if pressed & 0x80: StartJump(m); return
  if m.var and f_801EBE54: return
  if m.anime.flag & 0x20: m.anime.flag &= 0x0F; DashDust(m) (0x8003C6A4)
  if m.anime.flag & 0x80:
      m.anime.flag &= 0x0F; PlaySound(1, 5, m); m.8C = 1; m.step2 += 1; return
  PlayerSyncShootAnime(m, 16)

DashRun (0x800358D4):
  PlayerSetTerrainBox(m)
  if WeaponPose: return
  if m.var and (Zero tries): return
  if TrySuperJump: return
  if pressed & 0x80: StartJump(m); return
  if m.var and f_801EBE54: return
  if DashStep(m) (0x80039FFC): DashEnd(m) (0x8003B218); return
  if m.anime.flag & 0x40: m.anime.flag &= 0x0F; f_8003D26C(m)
  if (m.dash_timer & 3) == 0: f_8003C7D8(m)       ; afterimage or dust
  PlayerSyncShootAnime(m, 16)
  DashTerrainBox(m) (0x8003CD00)

DashStep (0x80039FFC)(m):                         ; 1 = end the dash
  if m.blocked & (m.flip ? 1 : 2): return 1
  PlayerMoveAccel(m)
  clamp: m.speed_x >= m.108 (right) / <= -m.108 (left)
  m.dash_timer -= 1
  if m.dash_timer == 0: return 1
  h = held & 0x103
  if h == 0: return 1
  return m.flip ? (h & 2 != 0) : (h & 1 != 0)

DashEnd (0x8003B218)(m):
  PlayerSetAnimeShoot(m, 17); StopSound(1, 5)
  m.speed_x = m.flip ? m.108 : -m.108; m.accel_x = m.10C
  m.8C = -1; m.84 = 0; m.86 = 0; m.step2 = 2

DashSlide (0x80035A2C):
  if TryLadder or WeaponPose: return
  if m.var and (Zero tries): return
  if TryLand: return
  if m.var and (Zero tries): return
  if m.anime.flag & 0x80: m.anime.flag &= 0x7F; PlaySound(1, 6, m)
  if m.anime.flag & 0x40 and m.dash_timer == 0:
      if m.blocked & (m.flip ? 1 : 2): m.speed_x = 0
      if m.speed_x != 0:
          PlayerMoveAccel(m)
          if the sign of speed_x crossed 0: m.speed_x = 0
  if m.anime.next < 0: ToStand(m) else PlayerSyncShootAnime(m, 17)

DashTerrainBox (0x8003CD00)(m):
  if m.D5 < 0: m.terrain_box = 0
  else: m.terrain_box = m.var ? 0x80073D5C : 0x80073D58

Crouch (step 26, 0x80035D38):
  PlayerSetTerrainBox(m)
  if pressed & 2: m.flip = 0; if pressed & 1: m.flip = 0x40
  if PlayerRequests or f_8003F644: return
  if TryDashJump:
      if m.step != 12: return                    ; it was a jump
      DashTerrainBox(m)
      PlayerSetAnimeAt(m, m.8E ? 128 : 16, 5); DashDust(m); PlaySound(1, 5, m)
      m.8C = 1; m.step2 += 1; return             ; straight into DashRun
  if m.var and (four Zero tries): return
  if not (held & 8) and TryDirection: ToWalkStart(m); return
  AnimeTick(m)
  sub 0: X weapon pose: keep 109 at the frame; when next == 0: sub 1
  sub 1: DashTerrainBox; shooting poses 139 (weapons 18, 19, 20, 43, 46) or 108, or 81 at frame 2
         if down released: animation (dash flag and X ? 110 : 82); sub 2
  sub 2: when next == 0: ToStand(m)
```

## Engine helpers called

| address | name | note |
|---|---|---|
| `0x8002D5D0` | `PlayerMove` | x += speed_x (3/4 in a head wind), y −= speed_y |
| `0x8002CFB0` | `PlayerMoveAccel` | move, then accel_x in the facing direction and gravity; fall clamp |
| `0x80017A04` | `AnimeTick` | |
| `0x8003BA04` | `PlayerSetAnime` | frame set, then `SetAnime` |
| `0x8003BB94` | `PlayerSetAnimeAt` | with a start frame |
| `0x8003CCBC` | `PlayerSetTerrainBox` | |
| `0x8003A628` | `PlayerRequests` | |
| `0x80016C48` | `PlaySound` | |
| `0x8001750C` | `StopSound` | (bank, id); (255, 0) stops all |
| `0x8002D2D4` | `GetRNG` | |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x02` | `var` | 0 X, else Zero |
| `+0x05`, `+0x06` | step, sub-step | |
| `+0x15` | `flip` | `0x40` right |
| `+0x20`, `+0x24`, `+0x28`, `+0x2C` | speed x, speed y, accel x, gravity | 16.16 |
| `+0x44`…`+0x47` | animation frame word | duration, flag (frame number in bits 0–5), next, sprite |
| `+0x5C` | HP | |
| `+0x67` | jump latch | |
| `+0x7C`, `+0x80` | held, pressed | |
| `+0x84`, `+0x86`, `+0x8B` | dash and jump work | |
| `+0x85` | dash timer | 30 (15 with part `0x10`) |
| `+0x87` | dash without the button | [open] |
| `+0x89` | blocked sides | |
| `+0x8C` | dash flag | 1 dashing, −1 after |
| `+0x8E`, `+0x8F`, `+0x90`, `+0x91` | shot pose, weapon pose, pose ended, shot timer | |
| `+0x92` | ladder block timer | [likely] |
| `+0x96` | weapon | |
| `+0xC9` | Zero's skill bits | bit 1: super jump |
| `+0xD3` | lock | |
| `+0xD5` | dash box off when < 0 | |
| `+0xE7` | form | X 1, Zero 5 in the states |
| `+0xEC` | part bits | |
| `+0xF0`…`+0x134` | physics constants | |
