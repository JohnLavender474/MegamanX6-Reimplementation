# Metal Shark Player: pseudo code (overlay 07, main type 56, visual type 22)

Units: positions px, speeds px/update (16.16 in the code), times updates. `R(x, y)` = room point =
(camera target min x + x, camera target min y + y).

## Dispatch tables

| table | entries |
|---|---|
| `0x800F95CC` states | 0 Setup, 1 BossMain, 2 BossDeath, 3 KickMain, 4 Delete |
| `0x800F95E0` boss steps | 0 HitReactionDispatch, 1 Nop, 2 Intro, 3 Stun, 4 Fin, 5 Anchor, 6 Slam, 7 Burrow, 8 Resurrect, 9 PopUp |
| `0x800F9608` death steps | Death0, Death1, Death2 |
| `0x800F96A8` intro | 44CC, 4504, 45A0, 2D98, 4674, 46CC, 472C, 47C8, 480C, 4848, 48CC |
| `0x800F96D4` stun | 4934, 49D8, 4A18, 4A9C |
| `0x800F9614` fin | 4D00, 4DB0, 4DFC, 4E4C, 2468, 4EE0 |
| `0x800F962C` anchor | 4F58, 4FA8, 5094, 50EC, 51F0, 5254 |
| `0x800F9644` slam | 52C0, 52FC, 25F0, 53C8, 5430, 5530, 5580 |
| `0x800F9660` burrow | 55F8, 5648, 2738, 56F0, 5764, 2884, 2A04, 5814, 5884, 58E8, 5934 |
| `0x800F968C` resurrect | 59AC, 5A1C, 2C38, 5AA8, 5B9C, 5C00, 5C64 |
| `0x800F96E4` pop-up | 4B14, 4BE4, 4C28, 4C88 |
| `0x800F96F4` kick steps | 0 HitReactionDispatch, 1 Nop, 2 KickBeam, 3 KickSpark, 4 KickBee |
| `0x800F9708` kick `var` 1 | 5CD8, 3014, 5D28, 5D3C, 5DA4, 5DF8, 5E48, 5EBC, 5EEC |
| `0x800F972C` kick `var` 2 | 5FB8, 6008, 6118, 6180, 61D4, 3180, 6220, 62AC |
| `0x800F974C` kick `var` 3 | 6354, 63AC, 64BC, 6524, 6578, 3290, 65C4, 668C |
| `0x800F97A4` visual states | 0 VisSetup `0x800F35A8`, 1 VisMain `0x800F37DC`, 2 VisDelete `0x800F7024` |
| `0x800F97B0` visual steps | 0 Debris, 1 Anchor, 2 Piece, 3 Beam, 4 Segment, 5 Spark, 6 Bee |

## Boss

### Entry — 0x800F4230
```
prev = pos; state_table[state](obj); CollideCheck(obj)
```

### Setup — 0x800F1F80 (state 0)
```
if stage == 12: clut = 0x7948; sprite_data = stage_object_table[0]
table = dmg_tbl_shark; hp = 0; contact = 4; iframes = 0; display = 0; layer = mega.layer
speeds, accel, gravity = 0; latch = 0; anime_table = 0x800F952C; priority = 5
attack = hurt = terrain = null; SetAnime(2)
if var == 0: tex = 0x140; +8A = -1; flip = left; +8B..+8E = 0; fight_time = 0
             attack = 0x800F9088; hurt = 0x800F908C; pieces = 0; bees = 0; state 1, step 2
var 1/2/3: hp 24, priority 6, display 0, state 3, step 2/3/4
step2 = act4 = 0
```

### BossMain — 0x800F213C (state 1)
```
AnimeTick; step_table[step](obj); BoxBySprite(obj)
if step != 2 and var == 0: fight_time++
r = ContactWeapon(obj)
if r < 0: mega+0x7A = 1; state 2; step 0; return
if r > 0 and r != 127:
    table = deflect; inv = (r == 25 or r == 26) ? 2 : 30
    stun = r in (7, 16, 43)
    if step == 8:   if stun: inv = 120
    elif step == 7: if stun: inv = 120
                    elif +8D == 1 and r == 36: inv = 10000; step 9; step2 0
    elif stun: inv = 10000; step 3; step2 0
if inv: inv--; clut flash = inv & 2; if inv == 0: table = dmg_tbl_shark; flash off
ContactMega(obj)
```

### BoxBySprite — 0x800F66FC
```
if !display: attack = hurt = null; return
s = sprite; box = table 0x800E9B1C[s - 10] for s in 10..104, else 0x800F9088
```

### Think — 0x800F33F8
```
step2 = 0; contact = 4
if +8A == 0 and hp <= boss_hp_max / 2:
    p = rng & 15; +8B = p; +8A = 1; step = low[p][0]; +8C = 1; return
tbl = (+8A == 1) ? low : high
if +8A == -1: p = rng & 15; +8B = p; step = high[p][0]; +8C = 1; +8A = 0; return
s = tbl[+8B][+8C]
if s == 0xFF: p = rng & 15; +8B = p; +8C = 0; s = tbl[p][0]
step = s; +8C++
```

### Intro (step 2)
```
44CC: PlayerEvent(20, 64); next
4504: if exit_req == 0 and mega.step == 20:
          if stage != 12: PlayerEvent(29, 64)
          o = GetSysObject(); o.type = 24; +80 = o; t = 20; next
45A0: if --t == 0: PlayerEvent(20, 64)
      if +80 dead: game+24 = 1; game+25 = 0; game+20 = obj; BossLevel()
                   boss_hp_max = 48 + {0,0,4,8}[level]; t = 120; act4 = 0; next
2D98: t--; if t & 3 == 0: Debris(16, R(240, 224))
      if t & 15 == 0: sound 94; Rumble(5)
      if t < 61 and t & 3 == 0: Quake2(2, 4, 2)
      if t == 0: SetAnime(0); display 1; pos = R(240, 272); vy = 4.40; g = 0.055; next
4674: if vy < 0: SetAnime(1); terrain = 0x800F907C; next
      MoveObject2
46CC: if flag == 1: SetAnime(6); next  elif flag == 2: sound 93
472C: if floor: sound 92; SetAnime(7); if stage != 12: TextOpen(0, 255, game+2E); game+2E = 1; next
      else MoveObject2
47C8: if !text_open: HpBarShow(0); next
480C: if !HpBarBusy(): +7E = 3; next
4848: if --+7E == 0: sound(0, 14); +7E = 3
      hp++; if hp == boss_hp_max: next
48CC: PlayerEventEnd(); Think
```

### Stun (step 3)
```
4934: StopSound(2, 20); sound 33; sound 119; SpriteSet1; SetAnime(37); contact 4
      if +8E == 1: +8E = 0; terrain = 0x800F907C; y -= 28
      t = 90; next
49D8: if --t == 0: vx = ax = vy = 0; g = 0.25; next
4A18: if floor: sound 20; sound 92; SpriteSet0; SetAnime(2); t = 50; next  else MoveObject2
4A9C: if --t == 0: inv = 40; Think
```

### Fin (step 4)
```
4D00: SetAnime(13); FacePlayer; t = rng % 15 + 10; g = 70 / (t*t/2); vy = g * t; MoveObject2; next
4DB0: MoveObject2; if vy <= 0: SetAnime(14); next
4DFC: MoveObject2; if floor: SetAnime(38); next
4E4C: if flag: sound 95; vx = ±4; terrain = 0x800F9080; +8E = 1; t = 50; y += 28; vy = g = 0; next
2468: MoveObject
      if passed the player's x or wall in front:
          SetAnime(39); t = 90; terrain = 0x800F907C; +8E = 0; vx = 0; y -= 28
          vy = *0x800F986C; g = *0x800F9870 (both 0); next; return
      if frame_count & 7 == 0: Debris(facing left ? 21 : 20, x ∓ 30, y + 12)
      if --t == 0: t = 50; sound 95
4EE0: if --t == 0: Think
```

### Anchor (step 5)
```
4F58: sound 91; SetAnime(3); FacePlayer; next
4FA8: if flag == 1: SetAnime(4); H = rng % 20 + 60; t = rng % 15 + 10; g = H/(t*t/2); vy = g*t; vx = 0; next
5094: MoveObject2; if vy <= 0: FacePlayer; SetAnime(8); next
50EC: if flag == 1: sound 34 + (rng % 3); sound 93; Shot(0)      // the anchor
      elif flag == 2: SetAnime(6); next
51F0: if floor: sound 92; SetAnime(7); next
      MoveObject2
5254: if flag == 1: Think
```

### Slam (step 6)
```
52C0: SetAnime(10); FacePlayer; next
52FC: if flag == 1: tx = clamp(mega.x, R(40), R(312)); ty = max(mega.y - 80, R(48)); sound 91; next
25F0: FlyTo(tx, ty, 8); if near (≤ 8 px) or ceiling or wall ahead: SetAnime(11); t = 30; next
53C8: if --t == 0: sound 37; contact 8; vx = 0; vy = -5; next
5430: if floor: sound 94; Rumble(5); Quake2(50, 3, 2); SetAnime(12); Debris 16, 16, 17, 18 at y + 32; next
      else MoveObject
5530: if flag == 1: contact 4; SetAnime(2); t = 50; next
5580: if --t == 0: Think
```

### Burrow (step 7)
```
55F8: sound 91; SetAnime(15); FacePlayer; next
5648: if flag == 1: tx = clamp(mega.x); ty = max(mega.y - 112, R(48)); next
2738: FlyTo(tx, ty, 8); if near or ceiling or wall: SetAnime(16); vx = 0; vy = -5; next
56F0: if floor: sound 94; Rumble(5); SetAnime(32); vx = vy = 0; next  else MoveObject
5764: if frame_count & 3 == 0: Debris(16, x, y + 48)
      if flag == 1: SetAnime(33); contact 4; +7E = 180; terrain = 0x800F9084; t = 90
                    display 0; +8D = 1; y += 32; next
2884: if --+7E == 0: Emerge(+2, debris at y + 48)
      elif --t == 0: display 1; FacePlayer; vx = ±5; vy = 0; next
      else MoveObject
2A04: if --+7E == 0: Emerge(+1, debris at y + 32)
      else: if +7E in (27, 87): sound 95
            MoveObject; if passed the player's x or wall: t = 4; step2-- (back to 2884)
            elif frame_count & 3 == 0: Debris(facing left ? 21 : 20, x ∓ 8, y + 8)
Emerge: sound 94; Rumble(5); SetAnimeFrame(33, 4); contact 4; vx = 0; vy = 4; +8D = 0
        y -= 32; ty = y - 72; Debris 16, 16, 17, 18
5814: MoveObject; if y < ty: terrain = 0x800F907C; SetAnime(4); vx = ax = vy = 0; g = 4189; next
5884: if floor: sound 92; SetAnime(7); next;  MoveObject2
58E8: if flag == 1: SetAnime(2); t = 20; next
5934: if --t == 0: Think
```

### Resurrect (step 8)
```
59AC: sound 91; SetAnime(3); face the room middle (x < R(176) → right); next
5A1C: if flag == 1: SetAnime(4); tx = facing left ? R(48) : R(304); ty = R(80); next
2C38: FlyTo(tx, ty, 6); if within 6 px: sound 38; SetAnime(17); face the room middle; next
      elif frame_count & 3 == 0: Debris(facing left ? 21 : 20, x ∓ 8, y + 8)
5AA8: if flag == 1:
          if difficulty >= 2 and rng & 1: sound 41; Kick(3)
          elif mega.var == 0:           sound 40; Kick(2)
          else:                         sound 39; Kick(1)
          act4 = 1; next
      elif flag == 2: sound 96
5B9C: if act4 == 0: StopSound(2, 20); SetAnime(4); vx = ax = vy = 0; g = 4189; next
5C00: if floor: sound 92; SetAnime(7); next;  MoveObject2
5C64: if flag == 1: Think
```

### PopUp (step 9)
```
4B14: SetAnime(28); contact 4; y -= 32; vy = 5.333; g = 0x500000 / 450; ax = 0
      if x < mega.x: vx = -32/30; flip left  else: vx = +32/30; flip right
      MoveObject2; next
4BE4: if vy <= 0: terrain = 0x800F907C; next;  MoveObject2
4C28: if floor: sound 92; t = 50; next  else MoveObject2
4C88: if --t == 0: inv = 40; Think
```

### Death (state 2)
```
42C8: StopSound(2, 20); sound 32; mega+CA = 0; PlayerEvent(20, mega.flip); SetAnime(28)
      step 1; t = 127; +7E = 25; iframes = 25; flash off; DisplayCheck(96, 96)
4360: if --t == 0: step 2; o = GetSysObject(); o.type = 26; o.pos = pos; +80 = o
      DisplayCheck(96, 96)
      if --+7E == -1: flash ^= 1; iframes -= 5; if iframes >= 26: iframes = 0
                      +7E = max(iframes, 5)
2340: display = 0
      if +80 alive: if +80.act4 != 0: return
                    if --+7E == -1: +7E = 5; flash ^= 1;  DisplayCheck(96, 96); return
      clear +80..+97; game+24 = 0; game+20 = 0
      if stage == 12: exit_req = 0x80; game[0x2C + cp] = 1; cp += 8
      else Soul(fight_time < 5401 ? 0x10 : 0x11)
      DeleteObject3(obj)
```

### Delete — 0x800F4454 (state 4)
```
if var != 0: +80.act4 = 0     // the boss may go on
DeleteObject3(obj)
```

## Side kicks (state 3)

### KickMain — 0x800F2EE4
```
if boss.state != 1: state 4; display 0; return
AnimeTick; kick_steps[step](obj)
r = ContactWeapon(obj)
if r < 0: SpawnExplosion; state 4; step 0; return
if r > 0 and r != 127: table = deflect; +84 = (r in 25, 26) ? 2 : 30
if +84: +84--; flash = +84 & 2; at 0: table = dmg_tbl_shark; flash off
ContactMega(obj)
```

### KickBeam (`var` 1)
```
5CD8: +7E = {2,3,4,4}[level]; next
3014: x = mega.x; y = R(128)
      if mega.x > R(176): x -= 112; flip right  else: x += 112; flip left
      x = clamp(x, R(40), R(312)); y = max(y, R(32))
      Piece(32, a) for a in 2560, 0, 1536, 2760, 200, 1736, 2960, 400, 1936; next
5D28: next
5D3C: if pieces == 0: sound 97; display 1; SpriteSet1; SetAnime(35); next
5DA4: if flag == 1: SpriteSet0; SetAnime(21); next
5DF8: attack = hurt = (-16, -24, 32, 60); t = 50; FacePlayer; next
5E48: if --t == 0: sound 98; SetAnimeFrame(22, 1); BeamShot(48); next     // BeamShot sets act4 = 1
5EBC: if act4 == 0: attack = hurt = null; t = 30; next
5EEC: blink (display = t & 1, AnimeTick on odd t); if --t == 0: if --+7E == 0: state 4  else step2 = 1
```

### KickSpark (`var` 2)
```
5FB8: +7E = {2,3,4,4}[level]; next
6008: if x > R(176): x = R(80); flip right  else: x = R(272); flip left
      y = R(16); 9 pieces; next
6118: if pieces == 0: sound 97; display 1; SpriteSet1; SetAnime(34); next
6180: if flag == 1: SpriteSet0; SetAnime(18); next
61D4: SetAnime(19); attack = hurt = (-24, -16, 48, 48); next
3180: if flag == 1: t = 60; p = (x ± 16, y + 52); a = ratan2(p.y - mega.y, mega.x - p.x)
                    sound 99; Spark(a - 455, p); Spark(a, p); Spark(a + 455, p); next
6220: if --t == 0: if --+7E == 0: SetAnime(18); boxes null; t = 30; next  else step2 -= 2
62AC: blink; if --t == 0: state 4
```

### KickBee (`var` 3)
```
6354: +7E = {120,160,200,240}[level]; next
63AC: if x > R(176): x = R(64); flip right  else: x = R(288); flip left
      y = R(64); 9 pieces; next
64BC: if pieces == 0: sound 97; display 1; SpriteSet1; SetAnime(36); next
6524: if flag == 1: SpriteSet0; SetAnime(25); next
6578: SetAnime(26); attack = hurt = (-16, -24, 32, 72); next
3290: if +7E: +7E--
      if flag == 1: act4 = 0; bees = 0; n = {3,5,6,10}[level]
                    facing left: a = 3042, d = -964/(n-1)  else a = 3102, d = +964/(n-1)
                    sound 100; for i in 0..n-1: Bee(a + i*d)   // bees++ each
                    +86 = bees; next
65C4: if +7E == 0 or --+7E == 0: t = 30; boxes null; act4 = 1; next; return
      if n - bees >= ceil(n / 3): act4 = 1; step2 -= 2
668C: blink; if --t == 0: act4 = 0; state 4
```

## Visual type 22

### VisEntry — 0x800F6FA0
```
+95 = col_f | +71; prev = pos; vis_states[state](obj)
if var & 0xF0 == 0: CollideCheck(obj); +71 = 0
```

### VisSetup — 0x800F35A8
```
if boss.state != 1: state 2; display 0; return
contact 0; hp 1; priority 4; +67 = 1; table = deflect; boxes null; display 0; state 1
step2 = act4 = 0; speeds 0; +94 = +96 = 0
switch var & 0xF0:
  0x00: SetAnime(9); contact 4; x += facing left ? -32 : 32; step 1            // anchor
  0x10: flip 0; step 0                                                        // debris
  0x20: flip 0; step 2                                                        // piece
  0x30: contact 4
        if var == 48: SetAnime(23); beam = obj; x += facing left ? -144 : 144; y += 45; step 3
        else: attack = (-16, -8, 32, 16); step 4
  0x40: SetAnime(20); contact 4; step 5                                       // spark
  0x50: SetAnime(27); +96 = 1; table = dmg_tbl_minor; contact 4; step 6; var += rng & 3   // bee
```

### VisMain — 0x800F37DC
```
if boss.state != 1: state 2; display 0; return
prev = pos; if +94 == 0: AnimeTick
vis_steps[step](obj)
if state != 1: display 0; return
k = var & 0xF0
if k != 0x30: if ContactWeapon(obj) < 0: SpawnExplosion; display 0; state 2; return
if ContactMega(obj) and k in (0x00, 0x40): SpawnExplosion; display 0; state 2; return
k 0x00, 0x10: if OffScreenCheck2(48, 48): state 2  else DisplayCheck(16, 16)
k 0x20:       DisplayCheck(16, 16)
k 0x40, 0x50: if OffScreenCheck2(48, 48): state 2
```

### VisDelete — 0x800F7024
```
if var & 0xF0 == 0x20: pieces--
elif var & 0xF0 == 0x50 and +96 == 1: bees--
DeleteObject3(obj)
```

### Debris (step 0): 3988, 70D4
```
SetAnime(var >= 19 ? 29 + (rng & 1) : 29 + rng % 3); flip = random; terrain null
dx = rng % 40 + 20; H = rng % 60 + 50; t = rng % 10 + 15
g = H / (t*t/2); vy = g * t; ax = 0
vx = (var - 16) in (0, 3): ±dx / t random; (1, 4): -dx / t; (2, 5): +dx / t
then MoveObject2 each update
```

### Anchor (step 1): 7130, 71D8, 7224, 72BC
```
7130: hurt = attack = (-12, -12, 24, 24); terrain = (0, 0, 8, 8)
      a = ratan2(y - mega.y, mega.x - x); v = 4 px at a; next
71D8: MoveObject2; if col_f: t = 50; +94 = 1; next
7224: if --t == 0: FacePlayer; vx = ±1.28; vy = 6.4; g = 0.256; ax = 0; t = 9; +94 = 0; next
72BC: MoveObject2
      if col_f: if --t == -1: SpawnExplosion; state 2; return
                if +95 & 4: vy = -3.2
                elif +95 & 8: vy = 6.4; g = 0.256
                elif +95 & 2: vx = +1.28; flip right
                elif +95 & 1: vx = -1.28; flip left
                pos = prev; MoveObject2
```

### Piece (step 2): 73F0, 3B9C
```
73F0: SetAnime(29 + rng % 3); flip = random; centre = pos; a = (+84 + 2048) & 4095
      pos = centre + (rcos(a), rsin(a)) / 8; next          // 512 px out
3B9C: v = 8 px toward centre; MoveObject; if |dx| <= 8 and |dy| <= 8: state 2
```

### Beam (step 3): 3C80, 7500, 7554
```
3C80: if kick.state != 3: state 2; display 0; return
      display = kick.display; flash = kick.flash
      f = flag; if f in 1..7: Segment(48 + f, offset[f]) (mirrored when facing right)
      if f == 16: t = 60; next
7500: if --t == 0: SetAnime(24); next
7554: (as 3C80 for display and kick); if flag == 16: state 2; display 0; kick.act4 = 0
offset (facing left): 49 (+112,-56), 50 (+80,-40), 51 (+48,-24), 52 (+16,-8),
                      53 (-16,+8), 54 (-48,+24), 55 (-80,+40)
```

### Segment (step 4): 75D8
```
if kick.state != 3 or beam.flag < (var & 15): state 2
```

### Spark (step 5): 765C, 76E0, 7748, 777C
```
765C: display 1; attack = (-6, -6, 12, 12); terrain = (0, 0, 6, 6); v = 6 px at +84; next
76E0: MoveObject; if col_f or CornerCheck: +94 = 1; t = 60; next
7748: if --t == 0: t = 30; next
777C: display = t & 1; if --t == 0: display 0; state 2
```

### Bee (step 6): 77C8, 7820, 78C8, 793C, 3F94, 7970
```
77C8: if kick.act4 == 1: +96 = 0         // stop counting
7820: display 1; attack = hurt = (-4, -4, 8, 8); terrain = (0, 0, 4, 4); v = 3 px at +84
      flip by vx; t = 32; next
78C8: MoveObject; if --t == 0: SetAnimeFrame(27, 2 + 2*(rng & 3)); t = 40; next
793C: if --t == 0: t = 180; next
3F94: if --t == 0: attack = (-16, -16, 32, 32); display 0; hurt = terrain = null; t = 34; next; return
      a = ratan2(y - mega.y, mega.x - x); v = {0.5, 1, 1.5, 2}[var & 15] px at a
      ax = g = 0; flip by vx; MoveObject
7970: if t & 7 == 0: SpawnExplosionRand(obj, 1, 16, 16); if --t == 0: state 2
```

## Spawn helpers

| address | what |
|---|---|
| `0x800F67D0` | Kick(boss, var): main 56, copies flip, x, y, palette, anime table, sprite data, texture, layer; `+80` = boss |
| `0x800F6880` | Debris(obj, var, x, y): visual 22 at (x, y) |
| `0x800F6948` | Piece(obj, var, angle): visual 22 at obj; `+84` = angle; `+7C` = obj `+80` (the boss); pieces++ |
| `0x800F6A1C` | Shot(obj, var): visual 22 at obj (the anchor) |
| `0x800F6AD0` | BeamShot(obj, var): visual 22 at obj; `+7C` = obj `+80`; `+8C` = obj; obj act4 = 1 |
| `0x800F6B94` | Segment(beam, var, x, y): visual 22; copies the beam's `+7C` and `+8C` |
| `0x800F6C70` | Spark(obj, angle, x, y): visual 22 `var` 64 |
| `0x800F6D4C` | Bee(obj, angle): visual 22 `var` 80 at (x ∓ 36, y − 12); bees++ |
| `0x800F6E48` | FacePlayer(obj): flip right when x < mega.x |
| `0x800F4144` | SpriteSet1(obj): texture `0x540`, palette and sprite data of bank + 1 |
| `0x800F6E78` | SpriteSet0(obj): texture `0x140`, palette and sprite data of the bank |
| `0x800F6F60` | Soul(var): main 15 with `var` |
| `0x800F25F0`, `0x800F2738`, `0x800F2C38` | FlyTo for SLAM, BURROW (8 px) and RESURRECTION (6 px) |

## Engine helpers called

| address | name | use |
|---|---|---|
| `0x80012D9C` | Rumble | impacts |
| `0x80016C48` | PlaySound | sounds |
| `0x8001750C` | StopSound | (2, 20) |
| `0x800179A4` | SetAnime | |
| `0x800179D0` | SetAnimeFrame (new name) | (obj, anim, frame): start an animation at a frame |
| `0x80017A04` | AnimeTick | |
| `0x80021760` | TextOpen | intro text |
| `0x80025FD8` | BossLevel | |
| `0x8002943C` | Quake2 | intro, slam |
| `0x8002A47C` | GetSpriteBank | sprite sets |
| `0x8002BBF4` | GetMainObject | side kick, soul |
| `0x8002C3F8` | GetSysObject | engine objects 24, 26 |
| `0x8002C458` | GetVisualObject | visual 22 |
| `0x8002C9B0` | DeleteObject3 | |
| `0x8002CBFC` | OffScreenCheck2 | visual 22 |
| `0x8002CCB0` | DisplayCheck | |
| `0x8002D230` | MoveObject2 | |
| `0x8002D2B0` | MoveObject | |
| `0x8002D2D4` | GetRNG | |
| `0x8002EE10` | CornerCheck | spark |
| `0x8002F288` | CollideCheck | |
| `0x80030ECC` | ContactMega | |
| `0x80031474` | ContactWeapon | |
| `0x8003D308` | PlayerEvent | |
| `0x8003D330` | PlayerEventEnd | |
| `0x80040290` | HitReactionDispatch | step 0 |
| `0x80040488`, `0x80040498` | HpBarShow, HpBarBusy | |
| `0x80049E50`, `0x80049EBC` | SpawnExplosion, SpawnExplosionRand | |
| `0x80065944`, `0x80065A14`, `0x80065B74` | rsin, rcos, ratan2 | |

## Object struct (the fields used here)

| offset | boss / side kick | visual 22 |
|---|---|---|
| `+0x03` | display | display |
| `+0x04`…`+0x07` | state, step, step2, act4 (side kick done) | state, step, step2 |
| `+0x2C` | gravity | gravity |
| `+0x50`, `+0x54`, `+0x68` | attack, hurt, terrain box | same |
| `+0x58`, `+0x5C`, `+0x60`, `+0x61` | damage table, HP, contact, death flash step | same |
| `+0x7C` | timer | the boss |
| `+0x7E` | second timer, repeat count | — |
| `+0x80` | engine object 24/26; the boss (side kick) | — |
| `+0x84`, `+0x86` | target x, y; kick: hit invulnerability (`+0x84`), shot count (`+0x86`) | angle (`+0x84`) |
| `+0x88` | invulnerability | timer / bounce count |
| `+0x8A`, `+0x8B`, `+0x8C` | pattern mode, number, position | `+0x8C`: the side kick |
| `+0x8D` | buried (BURROW) | — |
| `+0x8E` | fin (FIN) | — |
| `+0x90`, `+0x92` | — | piece centre |
| `+0x94` | — | animation stop |
| `+0x95` | — | contact flags of the update |
| `+0x96` | — | bee counts toward the side kick |

| global | name | use |
|---|---|---|
| `0x800F9868` | shark_pieces | live forming pieces |
| `0x800F986A` | shark_bees | live bees of the last fan |
| `0x800F986C`, `0x800F9870` | (0) | y speed and gravity after the fin; not written in the boss code (0 in all states) |
| `0x800F9874` | shark_fight_time | fight timer |
| `0x800F9878` | shark_beam | the live beam object |
