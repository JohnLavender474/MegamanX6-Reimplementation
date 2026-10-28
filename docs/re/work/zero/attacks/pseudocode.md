# Pseudo code: Zero's attacks and techniques (overlay 1)

Names are guesses; addresses, constants and control flow are from the code. `m` = `mega`.
Buttons: `held` = `m+0x7C`, `pressed` = `m+0x80`; `0x1` right, `0x2` left, `0x4` up, `0x8` down,
`0x10` attack (sabre), `0x20` special, `0x40` giga, `0x80` jump, `0x100` dash. `busy` = `m+0x8E`,
`combo` = `m+0xCB`, `skill` = `m+0xC9`, `lock` = `m+0xD3`. "Face held" = left held → flip 0,
right held → flip `0x40`.

## Helpers

```
SabreStart(m) 0x801ECD3C:  m+0xDD = 1; busy = 1; combo = 0; m+0x8C = 0
TrySabre(m) 0x801ECCD4:    if Zero && !lock && pressed&0x10 && !busy: SabreStart; return 1
SpawnSabre(m, t) 0x801ECD90:
    o = GetWeaponObject(); if o: o.flag = 1; o.type = t; o.var = combo; o.layer = m.layer
                                 o+0x84 = m.anime; o.flip = m.flip
SpawnSabreOnFlag(m, t) 0x801ECD54: if m.anime.flag & 0x80: m.anime.flag &= 0x7F; SpawnSabre(m, t)
GroundEnd(m) 0x801ECE60:   if anime at its hold frame: busy = 0
                           exit_req → PlayerExit; event (m+0xD0) → PlayerStartEvent; else ToStand
GroundMoves(m) 0x801ECE08: TryLadder || TryCrouch || (TryDirection && ToWalkStart)
```

## Ground sabre: try `0x801EBE54`, step 48 `0x801EAE7C`, combo `0x801EB12C`

```
TryGroundSabre: if TrySabre: anim 88; sound(1,7); 1 in 2: voice 3
                m+0x84 = m+0x86 = 0; step 48.0; run step 48 now

Step48:
  if sub == 0: sub = 1; goto tail                      # no tick in the first update
  AnimeTick
  SpawnSabreOnFlag(anim == 92 ? 28 : 24)
  f = anime.flag
  if !(f & 1) && combo < 2 && !lock && skill&0x04 && held&up && pressed&0x10:
      Shoenzan start (below); return
  if ComboTry(): return
tail:
  if lock: goto end
  if f & 1: busy = 1; goto end                         # first 2 frames: no cancel
  busy = 0
  if TryDashJump: return
  if combo >= 2: busy = 1; goto end                    # third hit: only dash or jump
  if TryCrouch: return
  if f & 0x40:
      m+0xDD = 1; if TryRekkoha: return; m+0xDD = 0
      if TrySabre: anim 88; sound(1,7); 1 in 2 voice 3; step 48.0 again; return
      if GroundMoves: return
  busy = 1
end: GroundEnd

ComboTry:
  if lock or !(f & 0x30): return 0
  if pressed&0x10 and f & 0x20:
      combo += 1; anim 88 + combo; sound(1, combo_snd[combo].s)
      voice combo_snd[combo].v (always for combo 2, else 1 in 2)
      SpawnSabre(24 + combo); return 1
  if f & 0x10 and sub == 1 and the held side is behind Zero:
      sub = 2; flip ^= 0x40; combo += 1; if combo >= 2: combo = 1
      anim 88 + combo; sound; 1 in 2 voice; SpawnSabre(24 + combo); return 1
  return 0
```

`combo_snd` (`0x801EE3BC`): {7, 3}, {8, 4}, {7, 5}.

## Air sabre: tries `0x801EBF94` (jump, wall kick) and `0x801EC028` (fall, push-off, air dash)

```
TryJumpSabre: if TrySabre: if m+0x86 == 6: anim 96, sound(1,7), step 53 else anim 91, sound(1,8), step 49
TryFallSabre: if Zero && !lock: if TryRakukojin: return 1
              if TrySabre: as above with steps 54 / 50

Step49/53 0x801EB324:
  if TryHang: return
  sub 0 → 1 (no tick) else AnimeTick; SpawnSabreOnFlag(step 53 ? 29 : 27)
  if lock or ceiling (m+0x89 & 4) or !(held & jump): speed_y = 0
  if busy and at hold frame: busy = 0
  if TrySabre: restart as TryJumpSabre; return
  b = busy; busy = 0
  if f & 0x40 and TrySabre: restart; return
  if m+0x8B == 0: TryLadderAir || TryWall || TryAirDash → return
  else m+0x8B -= 1
  AirControl (busy = b); WaterSplashCheck
  if speed_y <= 0:
      if busy: step 50 (or 54), sub 1  else anim 11 at frame 5, m+0x8A = 0, step 7
      latch = -1

Step50/54 0x801EB5F8:
  if TryHang: return
  if on the floor (m+0x89 & 8):
      latch = 0; m+0x84 = m+0x86 = m+0x8C = 0
      if busy and step 50 and !(f & 0x40):             # landing in the swing
          PlayerSetAnimeAt(92, f & 0x1F), keep the frame time
          if f & 0x1F: SpawnSabre(28)
          step 48.1; return
      busy = 0; ToLand; return
  sub 0 → 1 else AnimeTick; SpawnSabreOnFlag(step 54 ? 29 : 27)
  if busy and at hold frame: busy = 0
  if Zero && !lock: TryRakukojin || TrySabre(restart) → return
  b = busy; busy = 0
  if f & 0x40: the same two tries
  TryLadderAir || TryWall || TryAirDash → return
  AirControl (busy = b); WaterSplashCheck
  if !busy: anim 11 at frame 5; step 7
```

## Other sabre steps

```
Ladder 0x801EC0F0 (steps 15, 16): TrySabre → anim 93, sound(1,7), 1 in 2 voice 8, step 51
Step51 0x801EB934: if LadderShotExit: busy = 0; return
  tick; SpawnSabreOnFlag(30)
  if f & 0x40: busy = 0; if TrySabre: restart step 51; return; busy = 1
  face pressed side; at hold frame: busy = 0, anim 31 at frame 2, ToLadderUp

Wall 0x801EC178 (step 11 sub 0): TrySabre → anim 94, sound(1,7), 1 in 2 voice 8, step 52
Step52 0x801EC334:
  floor → busy = 0, ToLand.  no wall (m+0x89 & 3 = 0) or lock → busy = 0, ToFall
  TryWallKick → busy = 0
  if WallHeld: tick; SpawnSabreOnFlag(31); PlayerMove; splash
               at hold frame: busy = 0, anim 15, step 11.0
  else busy = 0, WallRelease

Crouch 0x801EC1F8 (step 26): down held and TrySabre → anim 105, sound(1,7), 1 in 2 voice 3, step 60
Step60 0x801EC68C: terrain box; tick; SpawnSabreOnFlag(37); face pressed side
  if !lock: busy = 0; if TryDashJump: return; busy = 1
  at hold frame: busy = 0, anim 81 at frame 2, step 26.1; DashTerrainBox

Rope 0x801EA004 / bar 0x801EA124 (steps 27, 28): TrySabre → anim 106 (rope) or 107 + n (bar),
  sound(1,7 or 8), 1 in 2 voice 8, ClearDashFlags, step 61.1, face held side
  if !lock: pressed jump → busy = 0, StartJump; else busy = 1
  at hold frame: busy = 0; step 27 (tile 46) or 28, sub 1
Step61 0x801EC76C: tick; SpawnSabreOnFlag(38 + (flag & 0x7F)); face pressed side;
  jump pressed → StartJump; at hold frame back to the hang (as above)
```

## Techniques

```
TryYammar 0x801EA254 (steps 2, 4, dash): skill&0x01 && held&up && pressed&0x20 && m+0xA6 != 1
   && energy[3] >= Cost(3) && !busy && GetWeaponObject:
   m+0xA6 = 1; weapon type 1 var 0; m+0x93 = 3; SpendEnergy(3); m+0x98 += 1; m+0x99 += 1

TryShoenzan 0x801EC86C (ground tries, crouch): !lock && skill&0x04 && held&up && pressed&0x10:
   busy = 1; anim 90; SpawnSabre(26); SpawnSabre(34); sound(1,7); sound(1,22); voice 5
   ClearDashFlags; step 55.0; SabreStart; run step 55
Step55 0x801EC95C: lock → busy = 0, StandOrFall; sub 0 → 1 else tick; GroundEnd

TryRakukojin 0x801EA34C (falls only): !lock && skill&0x08 && held&down && pressed&0x10 && !busy:
   m+0xDD = busy = 1; combo = 0; m+0x8C = 0; anim 98; voice 5; sound(1,23); face held side
   m+0x93 = 2; step 56.0; run
Step56 0x801EC440: lock → busy = 0, StandOrFall; tick; sub table 0x801EE3C4:
  0 0x801ECED4: SpawnSabreOnFlag(35); face held side
                frame flag 0x10: speed_y = -3.0, speed_x = accel = 0, gravity = m+0x100, latch -1, sub 1
  1 0x801EBA7C: floor: anim 99; 4 × SpawnExplosionRand(m, 0, 15, 15) at (x ± 11 forward, y + 19)
                       latch = 0; m+0x84 = m+0x86 = m+0x8C = 0
                       if !lock && energy[2] >= Cost(2): m+0x93 = 2; SpendEnergy(2); weapon type 4 var 0
                       sub 2
                air:   busy = 0; TryLadderAir || TryWall || TryAirDash → return; busy = 1
                       speed_x = 0, -1.5 with left held, +1.5 with right held
                       PlayerMoveAccel; WaterSplashCheck
  2 0x801EBC60: as the step-48 tail: TryDashJump; in flag 0x40 frames TryRekkoha, TrySabre (→ step 48),
                ladder, crouch, walk; at hold frame → GroundEnd

TrySentsuizan 0x801EA454 (steps 6, 7): !lock && skill&0x10 && held&up && pressed&0x10 && !busy:
   m+0xDD = busy = 1; combo = 0; anim 101; sound(1,25); voice 9; iframes = 0
   PlayerPaletteRestore; m+0xA4 = 0; step 57.0; no_hit = 1
   speed_x = +3.0 (facing right) or -3.0; speed_y = -5.0; run
Step57 0x801EC4AC: lock → busy = 0, StandOrFall; sub table 0x801EE3D0:
  0: sub 1 (1 update still)
  1 0x801ECF90: tick; SpawnSabreOnFlag(36)
     if m+0x89 & (facing right ? 9 : 10):          # floor or the wall in front
        sound(1,26); if !lock: no_hit = 0
        floor: Quake(30,2,1) in stage 0x104, else Quake2(30,2,1); anim 102; combo = 1
               SpawnSabreOnFlag(36) (var 1); latch = 0; sub 2
        wall:  latch = 0; busy = 0; StandOrFall
     else PlayerMove                               # 3 px forward, 5 px down, no gravity
  2 0x801ED0A0: tick; at hold frame busy = 0, StandOrFall

TryEnsuizan 0x801EA560 (ground, air, wall, ladder, hang): !lock && m+0x86 != 8 && skill&0x20
   && held&down && pressed&0x20 && !busy:
   m+0xDD = busy = 1; combo = 0; m+0x8C = 0; ClearDashFlags; anim 103; SpawnSabre(42); voice 5
   sound(1,24); iframes = 0; PlayerPaletteRestore; m+0xA4 = 0; no_hit = 1; step 58.0; m+0x86 = 8
Step58 0x801EC504: lock → busy = 0, StandOrFall; sub 0 → 1 else:
   tick
   if !(m+0x71 & 8) && f & 0x40:
      no_hit = 0
      if held&down && pressed&0x20 && combo < 2: combo += 1; anim 103; SpawnSabre(42)
   at hold frame: busy = 0; no_hit = 0; StandOrFall
   (no move call: Zero stays in place)

TryRekkoha 0x801EA680 (ground tries, step 48, step 56.2): !lock && skill&0x80 && pressed&0x40
   && energy[5] >= Cost(5) && !busy:
   m+0x93 = 5; m+0xDD = busy = 1; combo = 0; m+0x8C = 0; ClearDashFlags; anim 104; voice 7
   iframes = 0; PlayerPaletteRestore; m+0xA4 = 0; m+0xE0 = 2; m+0xE1 = 0; terrain box 0; latch 1
   no_hit = 1; game.player_lock = 1; SpendEnergy(5); step 59.0; act4 = 0; run
Step59 0x801EC614: tick; flag 0x80 → sound(1,27), clear it; sub table 0x801EE39C:
  0 0x801ED0E0: 0x8002ABA8(); clear the fade values; m+0xE4 |= 1; sub 1
  1 0x801EA7A4: fade level (0x800CF856) += 2 each update (colours + 64, + 2048); at 31: save
                16 words at 0x80090CD0 to 0x801EF974, fill them with -1; sub 2
  2 0x801EA888: at the 2nd update: restore the words; 5 × 2 effect objects type 17 var 0
                (animations 83/84 for Zero) at screen x 128 and 384, y 64…320; sub 3
  3 0x801ED150: at frame flag 0x40: m+0xE1 = 0; sound(1,28); sub 4
  4 0x801ED198: until act4 >= 11: m+0xE1 += 1; when m+0xE1 % 10 == 1: Beams(m, m+0xE1 / 10)
  5 0x801ED230: m+0xE1 += 1 to 71; then anim 111; sub 6
  6 0x801EABB4: fade level -= 8 to 0 (then m+0xE4 &= ~1, bg refresh); at the hold frame of
                anim 111 and fade 0: m+0x93 = busy = m+0xE0 = 0; if !lock: player_lock = 0,
                no_hit = 0; PlayerSetTerrainBox; sub 7
  7 0x801ED288: StandOrFall
Beams(m, k) 0x801EAA24: k == 0 (act4 == 0): beam (weapon 44, flag 0x21) and light (effect 26) at x
                        else: the same at x - 32k and at x + 32k

TryHyorogaAttack 0x801ECA50 (step 31 sub 3, ceiling hold): energy[1] >= Cost(1) && TrySabre:
   anim 100; sound(1,20); m+0x84 = m+0x86 = 0; m+0x93 = 1; SpendEnergy(1); step 63.0; run
Step63 0x801ECAF0: sub 0 → 1 else: act4 -= 1 if not 0; tick; SpawnSabreOnFlag(48)
   flag 0x40: clear it; 3 × weapon type 33 (flag 0x21) var 0, 1, 2
   at hold frame: anim 84; busy = 0; step 31.3 (back to the ceiling hold)

TryGuardShell 0x801ECBE8 (pause-menu close with slot 4): !lock && skill&0x40 && m+0xA6 != 2
   && energy[4] >= Cost(4) && !busy && GetWeaponObject:
   m+0xA6 = 2; weapon type 7 var 0; m+0x93 = 4; SpendEnergy(4); m+0x98 += 1; m+0x99 += 1

TryZBuster 0x801EBEDC (ground tries, crouch): !lock && pressed&0x20 && !busy:
   busy = 1; combo = 0; ClearDashFlags; anim 110; 1 in 2 voice 9; all speeds 0; step 62.0; run
Step62 0x801EACAC:
  0: sub 1
  1: tick; flag 0x80: weapon type 58 (flag 0x21, flip); sound(1,14)
     m+0x144 == 6 → act4 = 0, sub 2 ; else sub 3
  2: act4 += 1; act4 % 3 == 0: one more type 58, sound(1,14); act4 >= 12 → sub 3
  3: if flag == 1 && !lock: busy = 0; if TryLand: return; busy = 1
     tick; GroundEnd

TryDoubleJump 0x801EC298 (in TryAirDash, only while m+0x86 == 0): pressed&jump:
   anim 95; sound(1,1); voice 7; speed_x = accel = 0; m+0x84 = m+0x8C = 0; m+0x86 = 6
   speed_y = m+0xF8 (5.8125); gravity = m+0x134; AirControl; step 6.0
```

## Weapon objects

```
Sabre 0x801ED2A8 (types 24–31, 35–42, 48):
  delete when: HP <= 0, lock, m+0xCF (death), m.anime != o+0x84, (types 24–26) flip != m.flip,
               (type 42) var != combo
  state 0 SabreInit 0x801ED3DC:
     display 1; e = tbl[type-24] (0x801EE68C)
     part 0x800000 && e.ext && m.step != 55: effect type 3 var 16 (a copy of Zero's pose)
     sprite source and animation table = Zero's; priority 2; clut 0x7801
     type 42: var = combo
     pal = e.pal; if pal == 3: Black Zero → 4; part 0x1000000 → 5; part 0x400000 → 4
     Zero && (part 0x1000000 || Black Zero): o+0x9C = tbl[type-23].pal
     type 28: anim 116 at Zero's frame (flag & 0x1F), the same frame time; else anim e.anim
     type 26 in step 55: pal 8
     PalLoad(pal, 1, 1)
     type 36 with var: anim 124
     types 35, 36: o+0x8C = o+0x90 = tag = 0; else tag = anime.flag
     state 1
  each update: AnimeTick (state 1); delete at the hold frame (not type 36 var 0)
  SabreFollow 0x801ED6F0:
     x, y, flip = Zero's
     box = flag == 0 ? none : (part 0x800000 && e.ext ? 0x801EE580 : 0x801EE414)[flag]
     type 36: terrain box 0x801EE2D4[flag] for flags 75–79, else none
     tag: types 24, 25, 37–41: var + 1; type 26: flag; others: flag every 4th frame
     types 35, 36: after each hit (o+0x98): box off 2 updates, count + 1; no box from 4 hits
     type 26 in step 55: no box
     DisplayCheck(o, 128, 128)

Icicle 0x801ED928 (type 33): init: box (-12,-20,24,36), terrain box (0,-2,12,18 half);
   x = Zero x, y = Zero y + 24; SetSpeedDir(22 + 2 var) × 5 (5 px per update); anim 3
   run: off screen (32) or hit → delete; tick; MoveObject; tag = var + 1
        terrain (collide flags or CornerCheck) → sound(1,21), SpawnJunk(6, {4,5,6,7,…}), end

Fire wave 0x801EDAF4 (type 34): delete when HP <= 0, lock, death, or Zero not in step 55
   x = Zero x ± 8 (front), y = Zero y - 8; anim 10; box 0x801EE6F4[flag]
   after each hit: box off 4 updates; no box from 8 hits; delete at the hold frame

Z-Buster shot 0x801EDCC8 (type 58): init x = Zero x ± 48, y - 6; speed ±8.75 (12.75 with part 0x100)
   terrain box 0x801EE3E8, box (-24,-8,48,16); anim 0; start x kept in o+0x84
   state 1: stay at Zero's x ± 48, y - 6 until frame flag 0x80, then state 2
   state 2: off screen 16 or hit → delete; terrain → ShotSpark, delete; MoveObject
            player in step 17 → delete; 160 px or more from the start → anim 1, no boxes, state 3
   state 3: tick; delete at the end or off screen

Rekkoha beam 0x801EDFC8 (type 44): x outside [cam x - 16, cam x + 337) → delete
   var 0: m.act4 += 1; y = cam y - 128; anim 11; box (-16,-128,32,255)
   var ≠ 0: y = cam y - 288; anim 12; no box
   speed_y = -8.0 (8 px down per update); tag = flag, + 1 every 8 frames; no box from 4 hits
   delete at cam y + 368 (var 0) or + 400
```
