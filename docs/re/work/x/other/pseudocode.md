# Player ladder, hurt, beams, ride and events — pseudo code (part 2c)

Source: `~/mmx-data/x6/re/x/kit/code.txt` (X kit, `intro-1_start`), `../../zero/kit/code.txt`
(Zero), and `main-x.dis` (a full disassembly of `0x80010000`–`0x8008F000`, made with
`scripts/dumpmain.py`) for the code outside the kit. `m` is `mega` (`0x800970A0`). `held` =
`m+0x7C`, `pressed` = `m+0x80`, `blocked` = `m+0x89`. Speeds are 16.16 px/update.

## Tables

| address | what |
|---|---|
| `0x80073ABC` | player states: 0 `PlayerInit` `0x8003CBE4`, 1 `PlayerUpdate`, 2 `PlayerDeath` `0x8003CC78`, 3 `jr ra` `0x8003CCB4` |
| `0x80073ACC` | `player_steps` (32 shared, then overlay steps: 32 = X `0x801EB730`) |
| `0x80073C1C` | `ladder_steps`: `0x80037488`, `…74D8`, `…7548`, `…75D0`, `…77FC` |
| `0x80073C30` | hurt handlers by class 0–9: `…7880`, `…7880`, `0x80035BDC`, `…79AC`, `…7880`, `…7920`, then `0x80035BDC` × 4 |
| `0x80073C58` | another table (`0x8003649C`, `0x80038248`, `0x800382C8`, `0x800365D4`, `0x80038310`, `0x80038390`): part 2b |
| `0x80073C70` | hurt animation by class: 0, 33, 34, 36, 35, 34 |
| `0x80073C78` | hurt motion by class, 4 words: (0,0,0,0), (2.5, 0.1875, 0, 0), (0.75, 0, 2.75, 0.2578125), (0,0,0,0), (2.5, 0.1875, 0, 0), (14.0, 0, 0, 0) |
| `0x80073CD8` | hurt `+0x61` by class: 0, 75, 100, 0, 75, 63 |
| `0x80073CE4` | voices (part 2a) |
| `0x80073CF4` | init: `0x8003D850` (beam), `0x8003C1AC` (no beam) |
| `0x80073CFC` | y by checkpoint for the no-beam start (halfwords 427, 939, …) |
| `0x80073D20` | death steps: `0x8003D8A8`, `…D954`, `…D9F8`, `…DACC` |
| `0x80073D30` | death orb directions, 4 × 8 bytes |
| `0x80073D50`, `…D54` | terrain box X {0, 2, 11, 19}, Zero {0, 3, 11, 19} |
| `0x800113C4` | `PlayerStartEvent` switch, step − 20, 54 cases |
| `0x80010E78`, `0x80010E98` | tile switches (values 56–63, 33–63): 62, 63 → spike handler `0x80030B5C` |

## Ladder

```
TileAt (0x8003015C)(o): tile value at (x, y); 59 reads as 56
TileAbove (0x8002FD3C): at y + box.oy - box.h;  TileBelow (0x80030068): at y + box.oy + box.h

LadderFromGround (0x8003B63C): anim 27; latch 1; y.lo 0; x = (x & ~15) + 8; speed_x/accel/grav 0
    speed_y = m.118 (1.0); ClearDashFlags; ClearShotFlags (0x8003CDBC); step 14.0
LadderClimbTop (0x8003B6B4): anim 28; box 0; y = y & ~15 (lo 0); ClearShotFlags; step 14.1
LadderDown (0x8003B708): anim 29; y.lo 0; box 0; latch 1; x centre; ClearDash/Shot; step 14.2
LadderOffBottom (0x8003B774): anim 30; ClearShotFlags; step 14.3
ToLadderUp (0x8003B7B4): x centre; speeds 0; speed_y = m.11C (1.5); ClearDashFlags; step 15.0
ToLadderDown (0x8003B810): same, speed_y = -m.11C; step 16.0

TryLadderAir (0x8003A488):
  if lock or not held&4 or TileAt != 32: return 0
  if m.8E: X overlay 0x801EB6E8 (ladder shot, step 32)   else: anim 31; ToLadderUp
  return 1

LadderExit (0x8003A514):
  t = TileAt(m)
  if blocked & 8: LadderOffBottom; return 1
  if held & 4 and t == 33: LadderClimbTop; return 1
  off = (u8)(t - 32) >= 2;  if pressed & 0x80: off = 1
  if off: anim 30; step 14.4; return 1
  return 0

step 14 (0x8003744C): ladder_steps[sub](m)
  0 (0x80037488): AnimeTick; PlayerMove; if next < 0: anim 31; ToLadderUp
  1 (0x800374D8): AnimeTick; if flag & 0x80: flag = 0; y -= 16; old_y.px = y - 16; terrain box
                  if next < 0: latch 0; Stop
  2 (0x80037548): AnimeTick; if flag & 0x80: flag = 0; y = (y & ~15) + 32
                  if next < 0: y += 16; terrain box; anim 32 at 3; ToLadderDown
  3 (0x800375D0): AnimeTick; if next < 0: latch 0; Stop
  4 (0x800377FC): AnimeTick; if next < 0: StartFall (0x8003AFF4)

step 15 (0x80037610) / step 16 (0x80037704):
  if LadderExit: return
  X: if 0x8003E260 (ladder shoot): return
  Zero: if 0x801EA560 or 0x801EC0F0: return
  pressed & 2: flip 0;  pressed & 1: flip 0x40
  15: held & 4: AnimeTick; PlayerMove
      elif held & 8: keep = duration; anim 32 at (frame flag); duration = keep; ToLadderDown
  16: held & 4: keep; anim 31 at (frame flag); ToLadderUp
      elif held & 8: AnimeTick; PlayerMove

X step 32 (0x801EB730, overlay 0):
  if LadderShotExit (0x8003A5D8: TileAt != 32 or jump pressed -> StartFall): return
  if 0x8003E260: return
  AnimeTick; flip from pressed; if m.90: anim 32 at 3; ToLadderDown
```

## Hit

```
ContactMega (0x80030ECC)(o):                     ; returns 1 on a hit
  if byte 0x800C4568: return 0
  if m.7A == 1: return 0
  if m.7A == 2 and o not in 0x80093C98..+0x1400: return 0
  if m.144 == 1 and o not in that range: return 0
  if m.D5 < 0: (riding) the same on actor2 (0x800CD340): no scaling, class 1/2 by d < 5, +0x84 side
  if m.hp & 0x80 or m.A4: return 0
  if not BoxOverlap(o, m): return 0
  if m.7A == 2: m.7A = 0
  d = o.60
  if d >= 0:
     m.CA = 0; m.63 = o.62
     switch o.62:
       3: m.CA = 1
       5: (keep)
       0: if (u8)(form - 1) < 4: 4
          elif form == 5 and parts & 0x200: 4
          elif var and game.5F & 0x20: 4
          else: d < 5 ? 1 : 2
  s = PlayerDamageScale(m, d)
  game.98 += (s == 127 ? m.hp : s)
  m.hp -= s
  if m.hp > 0: m.hp |= 0x80  else: game.1C = 1; m.hp = 0x80
  if o.62 == 3: return 1
  if o.62 == 0 and (parts & 0x200 or (var and game.5F & 0x20)): m.A5 = m.flip
  else: m.A5 = (m.x < o.x) ? 0x40 : 0
  return 1

PlayerHitCheck (0x80039228):                     ; PlayerUpdate, before the step
  if step in (0, 1): return
  if (collide & 0xC) == 0xC: game.98 += hp & 0x7F; hp = 0x80
  if (collide & 0x3) == 0x3: same
  if hp != 0x80 and ((blocked & 0xC) == 0xC or (blocked & 3) == 3):     ; squeeze
      m.7A = 0
      if var == 0 and m.E0 == 1: m.E0 = 0; game.1C = 0; 0x8002ABA8()
      m.E5 = 75; hp &= 0x7F; if E6 >= hp: game.98 += hp
      hp -= E6; clear +0x76, +0x72 of the 80 stage objects (0x800C1220, 0xA4 each)
      if hp > 0: hp |= 0x80; CA = 0; 63 = 1; A5 = flip; if (u8)(form-1) < 4: 63 = 4; game.98 += E6
      else: hp = 0x80
  if m.79 and m.61 == 0 and m.7A != 1: game.98 += mega.hp & 0x7F; hp = 0x80
  if hp == 0x80:
      hp = 0; game.1C = 1; 0x8003A980 (CF = 1, 84 = 0, ClearShot, 8C = 0, 0x8003CE30)
      0x8003F3DC; terrain box 0; hurt box 0; state 2; step 0; sub 0; return
  if m.61:
      if m.61 >= 60: m.61 -= 1
      elif not (parts & 0x400) or frame_count & 1:
          m.61 -= 1; if m.61 == 0: m.A4 = 0; 0x8003C4EC
  if hp > 0: return
  PlayerHurt(m)
  X:    A8 += 14A / 8 (rounded to 0); if A8 > 14A or 14A - A8 < 14A / 8: A8 = 14A
  Zero: B2 += 0x8003EB0C(m, 5) / 2; if B2 >= 14A: B2 = 14A
  if parts & 0x800: i = argmin(A8..B8, first lowest); e[i] = min(e[i] + 12, 14A)

PlayerHurt (0x8003963C):
  terrain box; hp &= 0x7F; Rumble(1); PlaySound(3, (rand & 1) ^ 1); PlaySound(3, 17)
  if step == 17: return
  A4 = 1; CF = 1; 84 = 0; ClearShotFlags; 8C = 0; 0x8003CE30
  c = m.63; flip = A5; anim hurt_anime[c]
  speed_x, accel_x, speed_y, gravity = hurt_speed[c]
  if (parts & 0x200 or (var and game.5F & 0x20)) and c in (1, 2, 4): all four = 0
  if flip: speed_x = -speed_x
  m.61 = hurt_iframes[c]
  if c in (2, 3, 5): latch = 1
  step 17.0

step 17 (0x80037838): AnimeTick; hurt_steps[m.63](m)

done(m):                                         ; inline at the end of the handlers
  if lock: A4 = 0; 61 = 0   else: 0x8003D014 (HP < max/3: 8D = 120); A4 = -1

HurtLight (0x80037880):
  HurtMove; X: sprite == 208 ? 0x8003CF54(m, 82) : 0x8003C4EC
  if 61 == 60: HurtEnd; done
HurtHeavy (0x80035BDC):
  HurtMove; 0x8003D394; if sub == 0: sub = 1; return
  if latch and floor: latch 0; unless shock: speed_x = flip ? -0.5 : 0.5; speed_y = grav = 0
  if 61 == 60: floor: latch 0, ToStand; elif gravity: anim 11, latch -1, step 7.0; else StartFall
               done
HurtPush (0x80037920):
  HurtMove2; 0x8003D394; if sub == 0: sub = 1; return
  if 61 == 60: StartFall; done
HurtGrab (0x800379AC):
  held & 2: flip 0; held & 1: flip 0x40
  CA > 0: return; CA == 0: A4 = 0; HurtEnd
  CA == -1: anim 37; CA = -2;   CA <= -2: if next < 0: anim 36; CA = 1
HurtMove (0x80037A60):
  if m.D4: return
  if speed_x and blocked & (speed_x > 0 ? 1 : 2): speed_x = accel_x = 0
  PlayerMoveAccel
  if flip ? speed_x > 0 : speed_x < 0: speed_x = accel_x = 0
HurtMove2 (0x80037B10): the same, also speed_y = 0 at the wall
HurtEnd (0x8003B114):
  floor: latch 0; 86 = 0; ClearDashFlags; if PlayerRequests: return
         step 2.0; X shooting: anim 94 at (t<5 ? 2 : t<9), duration t-8 when t >= 9; else StandAnime
  air:   SetAnimeShoot 11; speeds 0; latch -1; gravity = m.100; if 84: 84 = 0, 8C = -1; step 7.0
PlayerGrabDamage (0x8003D07C)(d):
  CA = -1; if hp & 0x7F == 0: return
  if d: hp -= PlayerDamageScale(m, d)
  hp > 0: hp |= 0x80   else: CA = 0; game.1C = 1; hp = 0x80
```

## Death (state 2)

```
PlayerDeath (0x8003CC78): death_steps[step](m)
0 (0x8003D8A8): StopSound(1, 5); StopSound(1, 7); Rumble(0)
   if D5: disp 0; D7 = 1   else: game.12..1A = 1; anim 33; D7 = 8
   step++
1 (0x8003D954): if --D7: return
   game.12..1A = 0; disp 0; PlaySound(3, 16); PlaySound(3, 14); D7 = D6 = 0; step++; step 2 now
2 (0x8003D9F8): if D7 & 3 == 0: 4 × 0x8003DAF4((rand & 31) | 0x80)
   if D6 == 3: D7 = 20; step++
   elif D7: D7 -= 1
   else: 0x8003DB44(D6); if D6 == 2: 0x8003DB44(3); D7 = 18; D6++
3 (0x8003DACC): if D7: D7 -= 1  else: state 3; step 0
0x8003DAF4(v): o = GetMiscObject; o.flag 0x21, type 17, var v, state/step/sub 0
0x8003DB44(n): for i in 0..7: the same with var = 0x80073D30[n*8 + i]
```

Pit death (`0x8002820C`, the camera clamp of each layer record `a0`): x clamp of the camera and
the player (8 px margins), then `if mega.y < 0 and not exit_req: mega.y = 0`; `if cam.max_y <
cam.y: cam.y = max_y; if mega.y - 8 >= max_y + 256: game.98 += hp & 0x7F; hp = 0x80`; else
`cam.y = max(cam.y, min_y)`.

## Set-up and beams

```
PlayerInit (0x8003CBE4, state 0):
  if game.1E == 0: return
  m.3 = 1; game.1F = 1
  n = stage == 12 and area == 0 and (u8)(game.1D - 2) < 16
  state++; player_init[n](m)
0x8003D850: anim 1; PlaySound(1, 13); speed_y = -8.0; others 0; latch -1; step 0
0x8003C1AC: y = (stage 12: area ? 459 : table 0x80073CFC[game.1D]); form/parts/C9 as PlayerLoadForm;
  PlayerPhysicsDefaults + the part bonuses inline; Zero: y -= 1; y.lo = 0; terrain box; game.1C = 0
  ToStand; m.92 = 4; m.71 |= 8; cameras +0x44 = 1 (3 layers)

step 0 (0x80035144):
  AnimeTick
  sub 0: if CE: CE -= 1; if CE == 0: terrain box;  PlayerMove; return
         if blocked & 8: anim 2; latch 0; cam0/1/2.follow = 1; sub 1; return
         PlayerMove; 0x8003D394 (splash at tile 36)
  sub 1: if flag: flag = 0; PlayerLoadForm; PlayerPhysicsParts
         if next == 0: game.1C = 0; Zero: PlayerLoadForm; PlayerPhysicsParts; ToStand

PlayerLoadForm (0x8003D718):
  E7 = game.5E; EC = game.74[var]
  if E7 == 2: C9 = 0; 0x8003D510(1, 27, 5, 0); 0x8003CF24(2, 25, 1); 0x8003CF24(2, 24, 1)
  else: C9 = game.60

PlayerExit (0x8003B870):
  0x8003CDF0 (clear 84, 86, 8C, 8E..92, A2; palette); 0x8003F3DC; game.1C = 1
  req & 0x10: StandAnime; 0x8003F000; StreamPlay(X 4 / Zero 5, 127) (Zero: 93 = 0); step 25.0
  req & 0x40: state 3; step 0
  else: anim 3; 0x8003F000; Zero: 93 = 0; BeamOut
BeamOut (0x8003B954): PlaySound(1, 11); speed_y = 8.0; others 0; box 0; latch 1; step 1.0
step 1 (0x8003674C):
  AnimeTick
  sub 0: if next == 0: cam0 +0x28, +0x2A, +0x20, +0x22 = cam0.y; anim 4; box 0; sub 1
  sub 1: if OffScreenCheck2(m, 32, 64): disp 0; state 3   else: PlayerMove; 0x8003D394
step 25 (0x80038144):
  AnimeTick
  sub 0: if stream_state == 0: anim 39; sub 1
  sub 1: if flag & 0x80: flag 0; PlaySound(0, 33, 0)
         if next == 0: anim 40; BeamOut
```

## Ride

```
PlayerRideCheck (0x8003A268, PlayerUpdate):
  if D5 <= 0: return
  0x8003CDF0; 0x8003F3DC; box 0; latch 1; step 18.1; D5 = -D5
step 18 (0x80037BC4):
  if exit_req: PlayerExit; return
  if D5 == 0: terrain box; StartJump; 86 = 1; game.1C = 0; return
  sub 0: anim D8; sub 1
  AnimeTick
```

## Events

```
PlayerStartEvent (0x8003AAD0):
  sub 0; step = D1; 0x8003CDF0
  switch step - 20 (0x800113C4):
    20: if (u8)(anime - 5) >= 2: StandAnime          -> D0 = -1; flip = D2
    21: anim 38; speed_y/accel/grav 0; D2 == 0: EventWalkTo  else: flip 0x40   (D0 stays)
    22: anim 3                                         -> D0 = -1; flip = D2
    23: anim 10; box; speed_x/accel 0; latch 1; speed_y = F8; grav = 100; flip = D2
    24: anim (var ? 5 : 107); 0x8003F000; flip = D2
    29: anim 38; D0 = -1; speed_x = 1.0; others 0; flip = D2; D2 == 0: speed_x = -1.0
    30: anim 38; latch 1; speed_x 2.0; accel 0; speed_y 2.90625; grav 0.0322; sub 0
        PlayVoice(m, 0x80073CE4[var*4 + rand&3]); anim 10; flip = D2
    73: if anime != 6: anim 6                          -> D0 = -1; flip = D2
EventWalkTo (0x8003A678):
  d = x - 64 - cam[layer].x
  d == 0: stop;  d > 0: flip 0, stop = d < 3;  d < 0: flip 0x40, stop = d >= -2
  stop: StandAnime; D0 = -1; flip 0x40; step 20; x = cam.x + 64
step 20 (0x80037DC4): terrain box; if PlayerRequests: return; D0 == 0: step 2.0  else AnimeTick
step 21 (0x80037E88): exit_req: PlayerExit. speed_x = flag ? F4 : F0, negated facing left
                     PlayerMove; AnimeTick; D2 == 0: EventWalkTo
step 22 (0x80037F70): PlayerRequests; sub 0: AnimeTick; next == 0: disp 0; sub 1
step 23 (0x80037FD4): sub 0: AnimeTick; MoveAccel; speed_y <= 0: anim 11; latch -1; speed_y 0; sub 1
                     sub 1: floor: 0x8003CDF0; StandAnime; latch 0; D0 = -1; step 20.0
                            else AnimeTick; MoveAccel
step 24 (0x8003809C): AnimeTick; sub 0: E2 == 4: anim 39; sub 1
                     sub 1: flag & 0x80: PlaySound(0, 33, 0); next == 0: PlayerEventEnd; ToStand
step 29 (0x80037F34): PlayerRequests; PlayerMove; AnimeTick
step 30 (0x80036364): AnimeTick; MoveAccel
   sub 2: next == 0: PlayerRequests
   sub 1: speed_x <= 0: anim 82; speed_x = accel = 0; sub 2
   sub 0: flag & 0x80: flag &= 0x3F; PlaySound(1, 1)
          speed_y <= 0: anim 11; latch -1
          latch == -1 and floor: anim 81; speed_x 2.0; accel -0.0625; speed_y = grav = 0; latch 0; sub 1
```

## Zero

```
step 5 (0x80036800): PlayerRequests, TryLadder, Zero A (0x801EC86C), B (0x801EA560), C (0x801EBE04),
  TryDashJump, Zero D (0x801EBE54), F (0x801EBEDC), TryDirection -> ToWalkStart;
  else AnimeTick; next < 0: ToStand
ZeroIdle (0x801EC9C4): anime == 6 and hp >= max/3: anim 5  else AnimeTick
0x801EA560 (Zero B): lock, 86 == 8, not C9 & 0x20, not held & 8, not pressed & 0x20 -> 0;
  8E: 0. Else DD = 8E = 1, CB = 0, ClearDashFlags, 8C = 0, anim 103, 0x801ECD90(42), voice 5,
  PlaySound(1, 24), 61 = 0, A4 = 0, palette, 7A = 1, step 58, 0x801EC504, 86 = 8
0x801EC0F0 (Zero ladder sabre): 0x801ECCD4 (attack input) -> anim 93, PlaySound(1, 7),
  1 in 2 voice 8, 84 = 86 = 0, step 51, 0x801EB934
```

## Engine helpers called

| address | name | note |
|---|---|---|
| `0x80017A04` | `AnimeTick` | |
| `0x8002D5D0` | `PlayerMove` | part 2a |
| `0x8002CFB0` | `PlayerMoveAccel` | part 2a |
| `0x8003BA04` / `0x8003BB94` | `PlayerSetAnime` / `PlayerSetAnimeAt` | |
| `0x8003CCBC` | `PlayerSetTerrainBox` | X `0x80073D50`, Zero `…D54`; none when `+0xD5` < 0 |
| `0x80030DC0` | `PlayerDamageScale` | |
| `0x8002D654` | `BoxOverlap` | |
| `0x8002C530` | `GetMiscObject` | death particles and orbs |
| `0x8002C398` | `GetEffectObject` | water splash (`0x8003CA70`) |
| `0x8002CBFC` | `OffScreenCheck2` | beam-out |
| `0x80018060` | `StreamPlay` | victory music |
| `0x80012D9C` | `Rumble` | |
| `0x80016C48`, `0x8001750C` | `PlaySound`, `StopSound` | |

## Object struct (the fields used here)

See `docs/players/ladder-hurt-events.md`, "Fields used here", and `reconstructed/include/player.h`.
