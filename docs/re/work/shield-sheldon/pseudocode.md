# Shield Sheldon (main 51, overlay 10): pseudo code

Room coordinates: `R(x, y)` = (cam.x + x, cam.y + y). `anim_flag` = `self+0x45`.
`face_player(o)`: `o.flip = (mega.x > o.x) ? RIGHT : LEFT` (`0x800F9E04`; an equal x faces left).

## Dispatch tables

| table | entries |
|---|---|
| `0x800FCC60` states (all `var`) | 0 `Setup` 0x800F44A0, 1 `BossMain` 0x800F48B4, 2 `BossDeath` 0x800F792C, 3 `PlateMain` 0x800F5990, 4 `ImageMain` 0x800F7AE8, 5 `TurretMain` 0x800F7BF0, 6 `ChildFall` 0x800F7CD4, 7 `ChildDelete` 0x800F7D70 |
| `0x800FCC80` boss steps | 0 HitReactionDispatch, 1 nop 0x800F9E34, 2 0x800F7D90, 3 0x800F80D4, 4 0x800F838C, 5 0x800F8884, 6 0x800F8B3C, 7 0x800F90CC, 8 0x800F9450, 9 0x800F980C |
| `0x800FCCA8` death steps | 0x800F7968, 0x800F79F4, 0x800F4A74 |
| `0x800FCCB4` step 2 subs | 0x800F7DCC, 0x800F7E24, 0x800F7F00, 0x800F7F70, 0x800F7FFC |
| `0x800FCCC8` step 3 subs | 0x800F8110, 0x800F815C, 0x800F821C, 0x800F8278, 0x800F4B9C, 0x800F4D7C, 0x800F835C |
| `0x800FCD14` step 4 subs | 0x800F83C8, 0x800F4F9C, 0x800F8404, 0x800F8464, 0x800F850C, 0x800F85C0, 0x800F862C, 0x800F86F8, 0x800F8794, 0x800F8848 |
| `0x800FCD3C` step 5 subs | 0x800F88C0, 0x800F88FC, 0x800F89BC, 0x800F8A30, 0x800F50A0, 0x800F8B1C |
| `0x800FCD54` step 6 subs | 0x800F8B78, 0x800F8BB4, 0x800F8C74, 0x800F8CE0, 0x800F8D4C, 0x800F5208, 0x800F8DAC, 0x800F8EB0, 0x800F8F60, 0x800F8FD0, 0x800F903C, 0x800F9078 |
| `0x800FCD84` step 7 subs | 0x800F9108, 0x800F9144, 0x800F9204, 0x800F9274, 0x800F5378, 0x800F9370, 0x800F93C4, 0x800F55A0, 0x800F942C |
| `0x800FCDA8` intro subs | 0x800F948C, 0x800F94C4, 0x800F5880, 0x800F955C, 0x800F95CC, 0x800F9614, 0x800F9668, 0x800F96DC, 0x800F9720, 0x800F975C, 0x800F97E0 |
| `0x800FCDD4` stun subs | 0x800F9848, 0x800F990C, 0x800F99C8, 0x800F99FC |
| `0x800FCDE4` plate steps | 0, 1 (nop), 0x800F9A1C, 0x800F9A80, 0x800F9ADC, 0x800F9BA4 |
| `0x800FCDFC` image steps | 0, 1, 0x800F9C90, 0x800F9CC4, 0x800F9D18 |
| `0x800FCE10` turret steps | 0, 1, 0x800F5AF0, 0x800F9D48, 0x800F9DA8 |
| `0x800FCF40` visual 16 states | 0x800F60AC set-up, 0x800F6484 main, 0x800FA030 fall, 0x800FA0CC delete |
| `0x800FCF50` visual 16 steps | 0 0x800FA63C (nop), 1 shield A 0x800FA0EC, 2 shield B 0x800FA340, 3 straight 0x800FA61C, 4 mirror shield 0x800FA42C, 5 mirror copy 0x800FA538, 6 aimed 0x800FA5BC |
| `0x800FCF6C` shield A subs | 0x800FA128, 0x800FA1B4, 0x800FA230, 0x800F65DC, 0x800FA2B0 |
| `0x800FCF80` shield B subs | 0x800F6794, 0x800FA37C, 0x800FA3D4, 0x800F6978, 0x800FA408 |
| `0x800FCF94` mirror shield subs | 0x800FA468, 0x800F6B5C, 0x800F6D58, 0x800FA51C |
| `0x800FCFD4` effect 5 states | 0x800FA644 set-up, 0x800FA710 main, 0x800FA798 delete; subs `0x800FCFE0`: 0x800FA7B8, 0x800FA81C, 0x800FA89C |

### Entry — 0x800F78AC

```
if var != 0 and state != 6 and parent(+0x80).step == 9: state = 6; step = 0
STATES[state](self)
```

### Setup — 0x800F44A0 (state 0)

```
if stage == 12 and var == 0: clut = 0x7948; sprite_data = stage_sprt_bank[0]
contact = 10; dmg = dmg_tbl_deflect; hp = 0; iframes = 0; display = 0; layer = mega.layer
speeds, accel, gravity, latch = 0; anime_tbl = 0x800FCBBC
switch var & 0xF0:
 0:  tex = 0x140; prio = 5; atk = BOX_BODY_A; hurt = BOX_BODY_D; terrain = 0
     SetAnime(4); flip = LEFT; fight_timer = 0; freeze = 0
     +0x8A = -1; +0x8B = +0x8C = +0x8E = 0; state = 1; step = 8
 16: if parent.state != 1: state = 7; display = 0; return
     prio = 5; display = 1
     16: SetAnime(24); boxes PLATE_V; pos = parent + (0, -38)
     17: SetAnime(25); boxes PLATE_H; pos = parent + (-38, 0)
     18: SetAnime(24); boxes PLATE_V; pos = parent + (0, +38)
     19: SetAnime(25); boxes PLATE_H; pos = parent + (+38, 0)
     terrain = 0; flip = LEFT; state = 3; step = 2
 32: PlaySound(2, 101); if parent.state != 1: delete
     prio = 5; display = 1; SetAnime(8); body boxes; terrain = 0
     x += (flip == LEFT) ? +32 : -32; state = 4; step = 2
 48: if parent.state != 1: delete
     flip = RIGHT for 48, 51; LEFT for 49, 50
     prio = 5; display = 1; SetAnime(2); body boxes; terrain = 0; +0x89 = 0; state = 5; step = 2
step2 = 0; act4 = 0
```

### BossMain — 0x800F48B4 (state 1)

```
save prev; AnimeTick; BOSS_STEPS[step]()
if step != 8 and var == 0: fight_timer++
+0x8D = 0
r = ContactWeapon(self)
if r < 0: mega+0x7A = 1; state = 2; step = 0; return
if r > 0:
    if r == 127: +0x8D = 1
    else:
        dmg = dmg_tbl_deflect; inv(+0x88) = (r == 25 or r == 26) ? 2 : 30
        if r in (5, 14, 36): dmg = dmg_tbl_deflect; +0x8E = 1; inv = 80; step = 9; step2 = 0
if inv: inv--; clut flash = inv & 2
    if inv == 0: flash off; if +0x8E == 1: dmg = deflect; +0x8E = 0  else dmg = dmg_tbl_sheldon
ContactMega(self)
```

### Think — 0x800F5D58

```
step2 = 0
if +0x8A == 0 and hp <= boss_hp_max / 2:
    +0x8B = GetRNG() & 7; +0x8A = 1; step = LOW[+0x8B][0]; +0x8C = 1; return
switch +0x8A:
 0, 1: tbl = (+0x8A == 0) ? HIGH : LOW
       s = tbl[+0x8B][+0x8C]
       if s == 0xFF: +0x8B = GetRNG() & 15; +0x8C = 0; s = tbl[+0x8B][0]
       step = s; +0x8C++
 -1:   +0x8B = GetRNG() & 15; step = HIGH[+0x8B][0]; +0x8C = 1; +0x8A = 0
```

### Warp helpers (inline in each step)

```
WarpOut(n = 13|15|17): SetAnime(n); atk = hurt = 0; step2++
WarpIn(sub): if flag == 1: PlaySound(2, 89); SetAnime(12|14|16); place; step2++
             elif flag == 2: PlaySound(2, 89) elif flag == 3: PlaySound(2, 81)
```

### Step 2 (unused) — 0x800F7D90

```
0: SetAnime(13); timer = 40; boxes 0; +0x7E = 0; dmg = dmg_tbl_sheldon_guard; +0x8E = 0; step2++
1: WarpIn: x = R(mega.x < R(160).x ? 256 : 64), y = R(80); face_player
2: on flag 1: SetAnime(6); body boxes; step2++; if +0x7E >= 2: Think
3: if +0x8D == 1: SetAnime(13); boxes 0; +0x7E++; step2++
   elif --timer == 0: dmg = deflect; +0x8E = 1; Think
4: WarpIn: x = R(self.x < R(160).x ? 256 : 64), y = R(80); face_player; step2 = 2
```

### Step 3 BOUNCE — 0x800F80D4

```
0: face_player; SetAnime(13); boxes 0; +0x7C = +0x7E = 0; step2++
1: WarpIn at R(160, 32); face_player
2: on flag 1: SetAnime(19); body boxes; step2++
3: flag 1: SpriteSet(1); SetAnime(1); +0x7C (row) = (mega.x > R(160).x) ? 0 : 1
           +0x7E = (rng&1 ? 0 : 0x0F) | (rng&1 ? 0 : 0xF0); act4 = 0; step2++
   flag 2: PlaySound(2, 81)
4: face_player; i = ROUTE3[row][act4]; (tx, ty) = PTS3[i]
   if i == 1 and (+0x7E & 0x0F): tx = clamp(mega.x - cam.x, 48, 144) & 0xFF0
   if i == 4 and (+0x7E & 0xF0): tx = clamp(mega.x - cam.x, 176, 272) & 0xFF0
   +0x84, +0x86 = tx, ty; step2++
5: a = ratan2(R(ty) - y, R(tx) - x); v = act4 / 2 + 6 px/frame
   spd = (rcos(a), rsin(a)) * 16 * v; flip by spd_x; MoveObject
   if |dx| <= 5 and |dy| <= 5:
       PlaySound(2, 85); Rumble(6); snap; act4++
       if act4 >= 8: SpriteSet(0); face_player; SetAnime(20); act4 = 0; step2++
       else: if act4 == 3: SetAnime(0); step2--
6: on flag 1: Think
```

### Step 4 SHIELD THROW — 0x800F838C

```
0: SetAnime(13); boxes 0; step2++
1: WarpIn: if mega.x < R(160).x: flip LEFT, R(272, 144) else flip RIGHT, R(48, 144)
2: on flag 1: SetAnime(21); body boxes; act4 = 0; step2++
3: flag 2: PlaySound(2, rng&1 ? 34 : 35); PlaySound(2, 83); Shot(self, 0)
   flag 1: SetAnime(22); step2++
4: flag 2: sounds 34/35, 83; Shot(self, 1); dmg = dmg_tbl_sheldon; +0x8E = 0
   flag 1: SetAnime(10); step2++
5: if act4 == 1: PlaySound(2, 89); SetAnime(17); boxes 0; step2 = 6; act4 = 0
   if act4 == 2: step2 = 8; act4 = 0
6: on flag 1: SetAnime(16); if flip == LEFT: flip RIGHT, R(64, 64) else flip LEFT, R(256, 64); step2++
   flag 2: PlaySound(2, 89)
7: flag 1: PlaySound(2, 82); SetAnime(8); dmg = deflect; body boxes; +0x8E = 1; step2 = 8
   flag 2: PlaySound(2, 89)
8: if act4 == 3: PlaySound(2, 82); SetAnime(8); dmg = deflect; +0x8E = 1; act4 = 0
   if act4 == 4: PlaySound(2, 82); SetAnime(6); dmg = deflect; +0x8E = 1; timer = 10; act4 = 0; step2++
9: if --timer == 0: Think
```

### Step 5 PLATES — 0x800F8884

```
0: SetAnime(13); boxes 0; step2++
1: WarpIn: SetAnime(12); face_player; flip = RIGHT; R(160, 64)
2: on flag 1: PlaySound(2, 86); SetAnime(23); face_player; body boxes; step2++
3: flag 1: PlaySound(2, rng&1 ? 36 : 37); SetAnime(10); dmg = dmg_tbl_sheldon; +0x8E = 0
           freeze = 0; MakePlate(16..19); timer = 600; act4 = 1; step2++
   flag 2: PlaySound(2, 87)
4: a = ratan2(mega - self); spd = (rcos, rsin) * 16          (1 px/frame)
   if not (|mega.x - x| <= 6 and |mega.y - y| <= 6):
       flip by spd_x; if freeze: freeze-- else MoveObject
   if --timer == 0: SetAnime(6); dmg = deflect; +0x8E = 1; act4 = 0; step2++
5: Think
```

### Step 6 MIRROR — 0x800F8B3C

```
0: SetAnime(13); boxes 0; step2++
1: WarpIn: SetAnime(12); face_player; flip = RIGHT; R(160, 64)
2: on flag 1: SetAnime(6); face_player; body boxes; timer = 20; step2++
3: if --timer == 0: SetAnime(8); MakeImage(32); timer = 20; act4 = 1; step2++
4: if --timer == 0: SetAnime(15); boxes 0; act4 = 2; step2++
5: flag 1: if mega.x <= R(160).x: flip LEFT; p = max(mega.x - 48, R(40).x)
           else: flip RIGHT; p = min(mega.x + 48, R(264).x)
           x = 2 cam.x + 320 - p; y = R(48) if rng&1 else R(160)
           SetAnime(14); body boxes; act4 = 3; step2++
   flag 2: PlaySound(2, 89)
6: flag 1: if y == R(48).y: SetAnime(22); step2 = 7
           else: sounds 34/35, 84; SetAnime(29); dmg = dmg_tbl_sheldon; contact = 10
                 atk = BOX_LUNGE_A; hurt = BOX_LUNGE_D; +0x8E = 0; step2 = 11
   flag 2: PlaySound(2, 89)
7: flag 2: sounds 34/35, 83; ShotPair(self); dmg = dmg_tbl_sheldon; +0x8E = 0
   flag 1: SetAnime(10); step2++
8: if act4 == 4: PlaySound(2, 82); SetAnime(8); dmg = deflect; +0x8E = 1; timer = 30; step2++
9: if --timer == 0: PlaySound(2, 82); SetAnime(6); act4 = 0; timer = 20; step2++
10: if --timer == 0: Think
11: on flag 1: +0x8E = 1; dmg = deflect; contact = 10; body boxes; timer = 30; step2 = 9
```

### Step 7 TURRETS — 0x800F90CC

```
0: SetAnime(13); boxes 0; step2++
1: WarpIn: SetAnime(12); face_player; flip = RIGHT; R(160, 80)
2: on flag 1: prio = 6; SetAnime(30); face_player; body boxes; timer = 20; step2++
3: flag 2: Effect5(self, x, y, 16)
   flag 3: PlaySound(2, rng&1 ? 38 : 39); act4 = 1; MakeTurret(48..51)
   flag 1: dmg = dmg_tbl_sheldon; +0x8E = 0; SetAnime(17); boxes 0; +0x7E = 0; step2++
4: flag 2: PlaySound(2, +0x7E == 0 ? 89 : 90)
   flag 1: prio = 4; c = GetRNG() & 3; place at CORNER[c] (flip toward the room)
           t = (c + 1 + GetRNG() % 3) & 3; (+0x84, +0x86) = CORNER[t]; timer = 50; step2++
5: if --timer == 0: SetAnime(16); step2++
6: flag 1: body boxes; timer = 0; step2++    flag 2: PlaySound(2, 90)
7: a = ratan2(target - self); spd = (rcos, rsin) << 6          (4 px/frame)
   if difficulty >= 2: spd = spd * 5 / 4
   SetAnime(spd_y > 0 ? (spd_x ? 37 : 39) : spd_y < 0 ? (spd_x ? 38 : 40) : 37)
   if anime in (39, 40): flip toward the room middle else flip by spd_x
   MoveObject
   if |dx| <= 3 and |dy| <= 3: snap; +0x7E++
       if +0x7E >= 4: SetAnime(6); dmg = deflect; +0x8E = 1; act4 = 0; step2++
       else: SetAnime(17); PlaySound(2, 90); boxes 0; step2 = 4
8: prio = 5; Think
CORNER = {(288, 32) LEFT, (32, 32) RIGHT, (32, 144) RIGHT, (288, 144) LEFT}
```

### Step 8 INTRO — 0x800F9450

```
0: PlayerEvent(20, 64); step2++
1: if exit_req == 0 and mega.step == 20:
       if stage != 12: PlayerEvent(29, 64)
       +0x80 = GetSysObject() as type 24; timer = 30; step2++
2: if --timer == 0: PlayerEvent(20, 64)
   if +0x80.flag == 0:
       game+0x24 = 1; game+0x25 = 0; game+0x20 = self; BossLevel()
       boss_hp_max = 32 + HPTBL[game+0x8F]; pos = R(240, 144)
       Effect5(self, x, y, 0); act4 = 0; step2++
3: if act4 == 1: SpriteSet(1); PlaySound(2, 80); SetAnime(0); display = 1; act4 = 0; step2++
4: on flag 1: SetAnime(1); step2++
5: on flag 1: SpriteSet(0); SetAnime(5); step2++
6: on flag 1: if stage != 12: TextOpen(0, 255, game+0x2E); game+0x2E = 1;  step2++
7: if text_open == 0: HpBarShow(0); step2++
8: if !HpBarBusy(): tick = 3; step2++
9: if --tick == 0: PlaySound(0, 14); tick = 3
   hp++; if hp == boss_hp_max: step2++
10: PlayerEventEnd(); Think
```

### Step 9 STUNNED — 0x800F980C

```
0: PlaySound(2, 33); SetAnime(18); contact = 10; body boxes
   if mega.x < x: flip LEFT; spd_x = +12; acc = 0x4CCC   else: flip RIGHT; spd_x = -12; acc = 0x4CCC
   terrain = BOX_TERRAIN; spd_y = grav = 0; step2++
1: MoveObject2; if |spd_x| <= 4: acc = 0
   CollideCheck; if collide & (flip == LEFT ? 1 : 2):
       PlaySound(2, 85); Rumble(6); Quake(10, 4, 2); timer = 40; terrain = 0; step2++
2: if --timer == 0: step2++
3: Think
```

### Death — 0x800F792C (state 2)

```
0: PlaySound(2, 32); mega+0xCA = 0; PlayerEvent(20, mega.flip); SetAnime(18); step = 1
   timer = 127; blink = 25; +0x61 = 25; flash off; DisplayCheck(96, 96)
1: if --timer == 0: step = 2; +0x80 = GetSysObject() as type 26 at (x, y)
   DisplayCheck(96, 96)
   if --blink == -1: flash ^= 1; +0x61 -= 5 (0 if >= 26); blink = max(+0x61, 5)
2: display = 0
   if +0x80.flag: if +0x80[7] == 0: blink every 6; DisplayCheck(96, 96)
   else: clear work; game+0x24 = 0; game+0x20 = 0
         if stage == 12: exit_req = 0x80; game[0x2C + cp] = 1; cp += 8
         else: Virus(fight_timer < 5401 ? 0x10 : 0x11)
         DeleteObject3(self)
```

### PlateMain — 0x800F5990 (state 3)

```
if parent.state != 1 or parent.act4 == 0: state = 7; display = 0; return
save prev; follow parent at the var offset; AnimeTick; PLATE[step]()
+0x88 = (ContactWeapon(self) != 0); ContactMega(self)
2: if +0x88: SetAnime(26); timer = 30; boxes PLATE_OPEN; step++
3: if --timer == 0: freeze = 8; SetAnime(27); step++
4: on flag 1: Shot(self, var); timer = 30; step++
5: if --timer == 0: SetAnime(24|25); closed boxes; step = 2
```

### ImageMain — 0x800F7AE8 (state 4)

```
if parent.state != 1 or parent.act4 == 0: state = 7; display = 0; return
save prev; flip = !parent.flip; sprite = parent.sprite; display = parent.display
atk, hurt = parent's; IMAGE[step]()
2: if parent.act4 == 2: step++
3: if parent.act4 == 3: x = 2 cam.x + 320 - parent.x; y = parent.y; step++
4: x = 2 cam.x + 320 - parent.x; y = parent.y
display = (++timer & 1) ? 0 : 1; ContactWeapon; ContactMega
```

### TurretMain — 0x800F7BF0 (state 5)

```
if parent.state != 1 or parent.act4 == 0: state = 7; display = 0; return
save prev; AnimeTick; TURRET[step]()
+0x88 = ContactWeapon(self) ? 1 : 0; ContactMega
clut flash = (+0x89 == 1)
2: fly to CORNER(var) at 6 px/frame; on arrival (<= 6 px): snap; SetAnime(35|36); flip; step++
3: if +0x88: Effect5(self, x, y, 32); +0x89 = 1; timer = 7; step++
4: +0x89 ^= 1; if --timer == 0: Shot(self, var); +0x89 = 0; step--
```

### Visual 16 — 0x800F9FE4

```
Setup 0x800F60AC:
  if parent.state != 1: state = 3; return
  contact 0; hp 1; prio 4; latch 1; dmg = deflect; boxes 0; speeds 0
  0x00: SetAnime(3); hurt = atk = SHIELD; contact 8
        var 0: spd_x = (x < R(160).x) ? +2 : -2; flip by it; step 1
        var 1: flip = (x < R(160).x) ? RIGHT : LEFT; step 2
  0x10: PlaySound(2, 88); SetAnime(28); dmg = one_hit; boxes SMALL; contact 4; flip LEFT
        16: spd_y = +2; 17: spd_x = -2; 18: spd_y = -2; 19: spd_x = +2
        if difficulty >= 2: spd *= 1.5; step 3
  0x20: SetAnime(3); SHIELD boxes; contact 8; step = (var == 32) ? 4 : 5
  0x30: PlaySound(2, 88); SetAnime(34); atk = SMALL; hurt = 0; terrain = TINY; contact 8; display 1
        a = ratan2(mega - self); spd = (rcos, rsin) << 5 (2 px/frame); Hard: × 2; step 6
Main 0x800F6484:
  if parent.state != 1: state = 3; return
  if parent.step == 9: state = 2; step = 0; return
  save prev; AnimeTick; VSTEP[step]()
  r = ContactWeapon; if r < 0: SpawnExplosion; state = 3
  elif ContactMega and group in (0x10, 0x30): SpawnHitEffect; state = 3
  elif OffScreenCheck2(32, 32): state = 3 else DisplayCheck(16, 16)
Shield A: 0 move until x <= R(32) / >= R(288): turn (2 px/frame)
          1 move until x <= R(112) / >= R(208)
          2 if rng&1: parent.act4 = 1; flip ^= 1; sub 3   else: parent.act4 = 2; Move; sub 4
          3 fly to R(64, 64) (flip LEFT) / R(272, 64) at 4 px/frame; <= 4 px: parent.act4 = 4; end
          4 move until x <= R(48) / >= R(272): parent.act4 = 3; end
Shield B: 0 fly to R(64, 64) / R(272, 64) at 4 px/frame; snap; sub 1
          1 parent.act4 == 1: sub 2; == 2: flip ^= 1; sub 3
          2 parent.step2 == 8: end
          3 fly to R(48, 144) / R(272, 144) at 4 px/frame; snap; sub 4
          4 parent.act4 = 4; end
Mirror shield: 0 row = flip ? 1 : 0; home = mirror(x), y; nibbles = rng; act4 = 0
               1 i = ROUTE6[row][act4]; target = PTS6[i] (0 = home; 1, 4 may follow mega.x)
               2 fly at (act4 / 2 + 6) px/frame; <= 5 px: snap; act4++;
                 act4 >= 7: sub 3 else PlaySound(2, 85); Rumble(6); sub 1
               3 parent.act4 = 4; end
Mirror copy: x = mirror(pair.x); y = pair.y; sprite, display = pair's; flip = !pair.flip
             if pair.state != 1: state = 3
Straight: MoveObject
Aimed: MoveObject; if collide or CornerCheck: SpawnHitEffect; state = 3
Fall 0x800FA030: once: speeds 0, gravity 0x4200; MoveObject2; off screen 64: state 3; flicker
```

### Effect 5 — 0x800FA8C4

```
Setup: var 0: SetAnime(4) sub 0; var 16: SetAnime(31), +0x54 = 0, sub 1; var 32: SetAnime(33) sub 2
       prio 4; display 1; clut flash off; state 1
Main: if parent.state != 1 or parent.step == 9: display 0; state 2 (delete)
      AnimeTick; sub
  0: flag 2: PlaySound(2, 79); parent.act4 = 1    flag 1: end
  1: flag 1: SetAnime(31); if ++(+0x54) >= 2: end    flag 2: PlaySound(2, 79)
  2: flag 1: end
```

## Engine helpers called

| address | name | use |
|---|---|---|
| `0x80012D9C` | Rumble | bounce points, stun wall |
| `0x80016C48` | PlaySound | all sounds |
| `0x800179A4` | SetAnime | |
| `0x80017A04` | AnimeTick | |
| `0x80021760` | TextOpen | intro |
| `0x80025FD8` | BossLevel | intro |
| `0x80029478` | Quake | stun wall |
| `0x8002A47C` | GetSpriteBank | sprite sets |
| `0x8002BBF4` | GetMainObject | plates, image, turrets, soul |
| `0x8002C398` | GetEffectObject | effect 5 |
| `0x8002C3F8` | GetSysObject | engine objects 24, 26 |
| `0x8002C458` | GetVisualObject | shots |
| `0x8002C9B0` | DeleteObject3 | |
| `0x8002CBFC` | OffScreenCheck2 | shots, falls |
| `0x8002CCB0` | DisplayCheck | death, shots |
| `0x8002D230` | MoveObject2 | stun, falls |
| `0x8002D2B0` | MoveObject | flights |
| `0x8002D2D4` | GetRNG | |
| `0x8002EE10` | CornerCheck | turret shot |
| `0x8002F288` | CollideCheck | stun |
| `0x80030ECC` | ContactMega | |
| `0x80031474` | ContactWeapon | |
| `0x8003D308` / `0x8003D330` | PlayerEvent / PlayerEventEnd | intro, death |
| `0x80040488` / `0x80040498` | HpBarShow / HpBarBusy | intro |
| `0x80049D90` | SpawnHitEffect | shots |
| `0x80049E50` | SpawnExplosion | shot kill |
| `0x80065944` / `0x80065A14` / `0x80065B74` | rsin / rcos / ratan2 | flights |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x02` | var | kind of object |
| `+0x03` | display | |
| `+0x04`…`+0x07` | state, step, step2, act4 | `act4` of the boss is the message byte with his shields, plates, image and turrets |
| `+0x15` | flip | |
| `+0x16` | priority | 5; 6 / 4 in step 7 |
| `+0x30`, `+0x3C`, `+0x40`, `+0x42` | animation table, sprite data, texture, palette | sprite set 0 / 1 |
| `+0x50`, `+0x54`, `+0x58`, `+0x68` | attack, hurt, damage table, terrain box | |
| `+0x5C` | hp | |
| `+0x60` | contact | 10 |
| `+0x61` | iframes | death blink interval only |
| `+0x7C` | timer | |
| `+0x7E` | counter | warp count (step 2), route bits (step 3), dash count (step 7), HP tick, death blink |
| `+0x80` | parent / engine object | children: the boss; boss: object 24 / 26 |
| `+0x84`, `+0x86` | target x, y (room) | steps 3, 7 |
| `+0x88` | invulnerability (boss); hit flag (children) | |
| `+0x89` | turret flash | |
| `+0x8A`, `+0x8B`, `+0x8C` | pattern mode (−1, 0 high, 1 low), pattern, entry | THINK |
| `+0x8D` | "a shot was deflected" this update | step 2 only |
| `+0x8E` | guard flag | |

Overlay globals: `0x800FD000` fight timer (32-bit); `0x800FD004` plate freeze counter (32-bit).
