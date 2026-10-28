# Nightmare Pressure (main 16 + stage 10 + visual 13, overlay 07): pseudo code

Overlay 07 is at `0x800E9860` (stage 4 area 1). All addresses below are in overlay 07.

Names:

- `cam0`, `cam1`, `cam2`: the camera records of BG layers 0, 1, 2 (`0x800971F8`, `0x8009724C`,
  `0x800972A0`). `cam1.y32` is the 16.16 word at `cam1+0x0C` (so `cam1.y` is its high half).
- **The machine** is BG layer 1. Its screen place is moved by `cam1.y`. `M(ox, oy)` is the world
  point of a machine offset: `x = cam0.x + cam1.x + ox`, `y = cam0.y + 952 - cam1.y + oy`.
  At rest `cam1.y` = 872, so `M(ox, oy)` = room `(ox, 80 + oy)`.
- `body` = main 16 `var` 0. `ofs_x` = `+0x82` (with `+0x80` as its fraction: a 16.16 at `+0x80`),
  `ofs_y` = `+0x86`.
- `MoveToward(p, target, step)` = `0x800ECD7C`: moves the halfword `*p` toward `target` by
  `step`; returns 1 (and sets `*p = target`) when the rest is ≤ `step`.
- `anim_flag` = `+0x45`.

## Dispatch tables

| table | entries |
|---|---|
| `0x800F868C` main 16 states | 0 `Setup` 0x800E9E08, 1 `BodyMain` 0x800EA0B4, 2 `BodyDeath` 0x800EB540, 3 `PartMain` 0x800EA4F4, 4 `PartDead` 0x800EB6B8, 5 `Delete` 0x800EB770 |
| `0x800F86A4` body steps | 0 HitReactionDispatch, 1 nop 0x800ECC3C, 2 INTRO 0x800EB790, 3 WAIT 0x800EBA84, 4 PRESS 0x800EBB14, 5 BOMBS 0x800EBDC4, 6 TURRET SHOTS 0x800EBF1C, 7 DRILL 0x800EC138, 8 BEAM 0x800EC380, 9 END CRUSH 0x800EC550 |
| `0x800F86CC` death subs | 0x800EB5C0, 0x800EA1CC, 0x800EA2E0, 0x800EB62C |
| `0x800F86DC` intro subs | 0x800EB7CC, 0x800EB80C, 0x800EB87C, 0x800EB910, 0x800EB9B8, 0x800EBA04 |
| `0x800F86F4` wait subs | 0x800EBAC0, 0x800EBAD8 |
| `0x800F86FC` press subs | 0x800EBB78, 0x800EBBC4, 0x800EBBF8, 0x800EBC80, 0x800EBD0C, 0x800EBD88 |
| `0x800F8714` bomb subs | 0x800EBE00, 0x800EBE14, 0x800EBE78, 0x800EBEFC |
| `0x800F8724` turret-shot subs | 0x800EBF58, 0x800EBFB8, 0x800EA3F4, 0x800EC000, 0x800EC0F0 |
| `0x800F8738` drill subs | 0x800EC174, 0x800EC1D0, 0x800EC218, 0x800EC240, 0x800EC2B0, 0x800EC2D8, 0x800EC338 |
| `0x800F8754` beam subs | 0x800EC3BC, 0x800EC418, 0x800EC460, 0x800EC4B4, 0x800EC508 |
| `0x800F8768` end-crush subs | 0x800EC5AC, 0x800EC640, 0x800EC6BC (nop) |
| `0x800F8774` part steps | 0 HitReactionDispatch, 1 nop, 2 TURRET 0x800EC6C4, 3 POD 0x800ECA7C |
| `0x800F8784` turret subs | 0x800EC700, 0x800EC7DC, 0x800EC7F4, 0x800EC928, 0x800EC95C, 0x800EC9C8, 0x800ECA28 |
| `0x800F87A0` pod subs | 0x800ECAB8, 0x800ECB64, 0x800ECBC4, 0x800ECC18 |
| `0x800E9864` part offsets (switch on `var` − 16, 19 cases) | see `PartMain` |
| `0x800F885C` visual 13 states | 0x800EAB50 set-up, 0x800EAE90 main, 0x800ECDF8 delete |
| `0x800F8868` visual 13 steps (by `step2`) | 0 bomb 0x800ECE40, 1 shot 0x800ED064, 2 missile 0x800ED140, 3 beam 0x800ED19C |
| `0x800F8878` bomb subs (by `act4`) | 0x800ECE7C, 0x800ECF20, 0x800ECFA0, 0x800ECFF0, 0x800ED03C |
| `0x800F888C` shot subs | 0x800ED0F0, 0x800ED120 |
| `0x800F8894` missile subs | 0x800EAFFC, 0x800ED17C |
| `0x800F889C` beam subs | 0x800ED1D8, 0x800ED258 |
| `0x800F88C0` stage 10 states | 0x800EB114 set-up, 0x800EB2E8 main, 0x800ED264 death |
| `0x800F88CC` stage 10 steps | 0 wall 0x800ED364, 1 press plate 0x800ED434, 2 head block 0x800ED48C (nop) |
| `0x800F87B0`, `0x800F87F8` THINK patterns | 9 patterns of 8 bytes, the same in both tables: 0–7 = {3, 5}, {3, 6}, {3, 7}, {3, 8}, {3, 5}, {3, 6}, {3, 7}, {3, 8}; 8 = {3, 4}; each ends with `0xFF` |

## Main type 16

### Entry — 0x800EB4C8

```
STATES[state](self)
```

### Setup — 0x800E9E08 (state 0)

```
if var == 0:                                   # the body (the head of the machine)
    contact = 8; hp = 0; iframes = 0; anime_tbl = 0x800F8638; layer = mega.layer
    prio = 34; dmg = dmg_tbl_normal; flip = LEFT; latch = 0; speeds = 0
    ofs_x = 160; ofs_y = -64; display = 0
    +0x8F = +0x90 = +0x91 = +0x92 = 0; terrain = atk = hurt = 0
    SetAnime(0)
    +0x8C = +0x8D = +0x8E = 0                  # THINK mode, pattern, index
    for v in (16, 17, 18, 32, 33, 34): MakePart(self, v)        # 0x800ECC44
    for v in (0, 1, 16, 17, 32):       MakeStagePart(self, v)   # 0x800ECCD8
    state = 1; step = 2 (INTRO); step2 = 0
else:                                          # a part
    hp = 16; anime_tbl = 0x800F8638; contact = 0; layer = mega.layer; prio = 33
    +0x84 = 0; latch = 0
    if var & 0xF0 == 16:                       # pods 16, 17, 18
        SetAnime(5); dmg = dmg_tbl_normal
        flip = (var == 17) ? RIGHT : LEFT; step = 3
    else:                                      # turrets 32, 33, 34
        SetAnime(1); dmg = dmg_tbl_deflect
        flip = (var == 33) ? LEFT : RIGHT; step = 2
    speeds = 0; display = 0; terrain = atk = 0; hurt = 0x800F852C (-8, -8, 16, 16)
    state = 3; step2 = 0
```

### MakePart — 0x800ECC44 / MakeStagePart — 0x800ECCD8

```
MakePart(body, v):   o = GetMainObjectAfter(body, 0)     # 0x8002BCCC
    o.flags = 0x41; o.type = 16; o.var = v; o.flip = 0
    o.clut = body.clut & 0x7FFF; o.anime_tbl, sprite_data, tpage, layer = body's; o+0x80 = body
MakeStagePart(body, v): o = GetStageObject()
    o.flags = 0x41; o.type = 10; o.var = v; o.x, o.y = body.x, body.y (whole px)
    o+0x7C = body; clut, anime_tbl, sprite_data, tpage, layer = body's
```

### BodyMain — 0x800EA0B4 (state 1)

```
if step != 2 and hp <= 0: hp = 0; state = 2; display = 0; step = step2 = 0; return
if cam0.x >= 5440 and step != 9: step = 9; step2 = 0         # END CRUSH
prev = pos; AnimeTick; BODY_STEPS[step](self)
x = M(ofs_x, ...).x; y = M(..., ofs_y).y
if +0x8F >= 2: ContactMega(self)                # the body has no attack box: no effect
```

### THINK — 0x800EA860 (end of every attack step)

```
step2 = 0
if mode(+0x8C) == 0 and hp <= boss_hp_max / 2:   # 24
    p = GetRNG() & 7; mode = 1; step = LOW[p][0]; idx = 1; return
tbl = (mode == 0) ? HIGH : LOW                    # the two tables are the same
e = tbl[p][idx]
if e == 0xFF:
    if count(+0x92) < 3: p = GetRNG() & 7; count++
    else:                p = 8;            count = 0     # the PRESS pattern
    idx = 0; e = tbl[p][0]
step = e; idx++
```

So the steps run: pattern 0 (WAIT, BOMBS), then three random patterns (WAIT, then one of BOMBS,
TURRET SHOTS, DRILL, BEAM), then (WAIT, PRESS), then three random patterns, and so on.

### INTRO (step 2)

```
sub 0  0x800EB7CC: timer(+0x7C) = 300 (not read); PlayerEvent(20, RIGHT)
sub 1  0x800EB80C: when mega.step == 20:
         cam1.redraw = 1; game+0x24 = 1; game+0x25 = 0; game.boss(+0x20) = self
         boss_hp_max(+0x8C) = 48; BossLevel()
sub 2  0x800EB87C: cam1.y -= 8 each update; when < 825: cam1.y = 824
         PlaySound(2, 68); Rumble(5); Quake(6, 10, 2); timer = 180
sub 3  0x800EB910: every 8 updates (timer & 7 == 0): Quake(6, timer / 30 + 1, 2)
         timer--; at 0: PlaySound(2, 67)
sub 4  0x800EB9B8: cam1.y += 8; when >= 872: cam1.y = 872; +0x7E = 3
sub 5  0x800EBA04: every 3 updates PlaySound(0, 14); hp++
         when hp == 48: PlayerEventEnd(); THINK
```

### WAIT (step 3)

```
sub 0: timer = 60     sub 1: timer--; at 0: THINK
```

### PRESS (step 4)

```
if mega.special == 2 and step2 >= 2: return       # frozen during a special attack
sub 0 0x800EBB78: PlaySound(2, 69); +0x90 = 1; timer = 20
sub 1 0x800EBBC4: 20 updates
sub 2 0x800EBBF8: cam1.y32 -= 0xCCCC (0.8 px)     # the machine moves down
         when cam1.y < 801: cam1.y = 800; PlaySound(2, 68); Rumble(5); timer = 60
sub 3 0x800EBC80: every 4 updates Quake(2, 1 + (GetRNG() & 3), 2)
         timer--; at 0: PlaySound(2, 67); timer = 30 (not read)
sub 4 0x800EBD0C: cam1.y32 += 0x18000 (1.5 px)
         when cam1.y >= 872: cam1.y = 872; PlaySound(2, 69); +0x90 = 0; timer = 20
sub 5 0x800EBD88: 20 updates, THINK
```

### BOMBS (step 5)

```
sub 0: count(+0x7E) = 0
sub 1: timer = GetRNG() % 40 + 50                   # 50..89
sub 2: timer--; at 0: MakeShot(self, 0); count++
         count < 4: step2 = 1 (again) else step2 = 3
sub 3: THINK
```

### TURRET SHOTS (step 6)

```
sub 0 0x800EBF58: ofs = (160, -64); PlaySound(2, 69); +0x8F = 2; display = 1; count = 0
sub 1 0x800EBFB8: MoveToward(&ofs_y, 56, 2) -> next
sub 2 0x800EA3F4 PickTurret:
    if mega.var == 0 (X):
        d1 = dist2(body, (mega.x - 16, mega.y + 32))    # where turret 32 lines up with X
        d2 = dist2(body, (mega.x + 16, mega.y))         # turret 33
        d3 = dist2(body, (mega.x - 16, mega.y - 32))    # turret 34
        act4 = the smallest: 1 (d1), 2 (d2), 3 (d3)      # ties: see the code order
    else (Zero):
        act4 = (body.x >= mega.x) ? 2 : (body.y < mega.y ? 3 : 1)
    step2++
sub 3 0x800EC000: wait for act4 == 0 (the turret fired)
    count++; limit = Zero ? 4 : (difficulty >= 2 ? 3 : 2)
    count < limit: step2 = 2 (pick again)
    else: PlaySound(2, 69); +0x91 = 1; step2++
sub 4 0x800EC0F0: MoveToward(&ofs_y, -64, 2): display = 0; +0x91 = 0; +0x8F = 0; THINK
```

### DRILL (step 7)

```
sub 0 0x800EC174: ofs = (48, -64); PlaySound(2, 69); +0x8F = 2; display = 1
sub 1 0x800EC1D0: MoveToward(&ofs_y, 56, 2): act4 = 4 (turret 34 extends its drill)
sub 2 0x800EC218: wait act4 == 0
sub 3 0x800EC240: ofs_x32(+0x80) += 0x18000 (1.5 px)
         when ofs_x >= 208: ofs_x = 208; PlaySound(2, 72); act4 = 4 (retract)
sub 4 0x800EC2B0: wait act4 == 0; timer = 90
sub 5 0x800EC2D8: 90 updates; PlaySound(2, 69); +0x91 = 1
sub 6 0x800EC338: MoveToward(&ofs_y, -64, 2): display = 0; +0x91 = 0; +0x8F = 0; THINK
```

### BEAM (step 8)

```
sub 0 0x800EC3BC: ofs = (160, -64); PlaySound(2, 69); +0x8F = 2; display = 1
sub 1 0x800EC418: MoveToward(&ofs_y, 56, 2)
sub 2 0x800EC460: act4 = (body.x < mega.x) ? 6 : (body.y < mega.y ? 7 : 5)
         # 5 -> pod 16 (top left), 6 -> pod 17 (right), 7 -> pod 18 (bottom left)
sub 3 0x800EC4B4: wait act4 == 0; PlaySound(2, 69); +0x91 = 1
sub 4 0x800EC508: MoveToward(&ofs_y, -64, 2): +0x91 = 0; display = 0; +0x8F = 0; THINK
```

### END CRUSH (step 9; entered when cam0.x >= 5440)

```
each update, after the sub: +0x90 = 2; MoveToward(&ofs_y, -64, 4)
sub 0 0x800EC5AC: unless mega.special == 2: cam1.y32 -= 0x40000 (4 px)
         when cam1.y < 777: cam1.y = 776; PlaySound(2, 68); Rumble(5); timer = 60
sub 1 0x800EC640: every 8 updates Quake(6, 4 | (GetRNG() & 3), 2); 60 updates
sub 2: nothing (holds)
```

### BodyDeath — 0x800EB540 (state 2)

```
DEATH_SUBS[step](self); then x, y = M(ofs_x, ofs_y)
sub 0 0x800EB5C0: game.checkpoint(+0x1D) = 2; game+0x26 = 1
         timer(+0x7C) = 127; blink(+0x7E) = 25; gap(+0x61) = 25; flash off
         PlayerEvent(20, mega.flip)
sub 1 0x800EA1CC: timer--; at 0: o = GetSysObject(): flags 1, type 26, var 1
             o.x = cam0.x + cam1.x + 160; o.y = cam0.y - cam1.y + 912     # M(160, -40)
             +0x88 = o; step++
         blink--; at -1: flash toggle; gap -= 5 (set 0 only if >= 26); blink = max(gap, 5)
sub 2 0x800EA2E0: o = +0x88
         if o is gone: return
         if o+0x07 == 0: blink every 6 updates; return
         step++
         cam0.x = cam1.x = min_x = max_x = targets = 5456; cam2.x = 2728
         cam0.y = cam1.y = cam2.y = min_y = max_y = targets = 224; redraw all 3 layers
         SpawnInRect(5408, 5824, 224, 512, 0)
         mega.x = mega.prev_x = 5604; mega.y = mega.prev_y = 395
sub 3 0x800EB62C: if o is live: (o+0x07 == 0: blink every 6) ; return
         PlayerEventEnd(); DeleteObject3(self)
```

### PartMain — 0x800EA4F4 (state 3)

```
body = +0x80
if body.step == 2: return                       # nothing during the intro
pos = body.pos + OFS[var]:
    16 (-16, -32)  17 (+16, 0)  18 (-16, +32)    # pods
    32 (+16, -32)  33 (-16, 0)  34 (+16, +32)    # turrets
prev = pos
if body.state != 1:
    speed_x = 0; display = 0; SpawnExplosion
    SpawnJunk(4, pod ? {14, 14, 15, 15} : {12, 12, 13, 13}); state = 5; return
if body+0x8F == 0: display = 0; return
if body+0x8F == 1: display = body.display; return
display = body.display; AnimeTick; PART_STEPS[step](self)
old = hp
if pod and inv(+0x84) == 0:
    dmg = (var == 17) == (mega.x >= body.x) ? dmg_tbl_normal : dmg_tbl_deflect
r = ContactWeapon(self)
if r != 0: body.hp -= (s8)(old - hp)            # the full damage goes to the body
if r < 0: SpawnExplosion; SpawnJunk(4, {14, 14, 15, 15}); speed_x = 0
          state = 4; step = step2 = 0; return
if pod:
    if r > 0 and r != 127: dmg = dmg_tbl_deflect; inv = (r == 25 or r == 26) ? 2 : 30
    if inv: inv--; flash = (inv & 2) != 0 before the decrement
        if inv == 0: dmg = dmg_tbl_normal; flash off
ContactMega(self)
```

### PartDead — 0x800EB6B8 (state 4)

```
if step == 0: display = 0; terrain = atk = hurt = 0; step = 1
if body.state != 1: state = 5; return
if (body.act4, var) in ((5, 16), (6, 17), (7, 18)): body.act4 = 0   # free a waiting BEAM
```

### TURRET (part step 2; turrets 32–34)

```
sub 0 0x800EC700: a = body.act4
    a == 1 and var == 32, a == 2 and var == 33, a == 3 and var == 34: SetAnime(4); step2 = 1
    a == 4 and var == 34: SetAnime(2); step2 = 4
sub 1 0x800EC7DC: timer = 50
sub 2 0x800EC7F4: timer--; at 0: MakeShot(self, X ? var - 16 : var); step2 = 3
sub 3 0x800EC928: SetAnime(1); body.act4 = 0; step2 = 0
sub 4 0x800EC95C: anim_flag == 1: atk = 0x800F8530 (-32, -10, 16, 20); contact = 8
                     body.act4 = 0; step2 = 5
                  anim_flag == 2: PlaySound(2, 70)
sub 5 0x800EC9C8: body.act4 == 4: PlaySound(2, 71); SetAnime(3); step2 = 6
sub 6 0x800ECA28: anim_flag == 1: SetAnime(1); atk = 0; contact = 0; body.act4 = 0; step2 = 0
```

### POD (part step 3; pods 16–18)

```
sub 0 0x800ECAB8: (body.act4, var) in ((5, 16), (6, 17), (7, 18)): SetAnime(6); step2 = 1
sub 1 0x800ECB64: anim_flag == 1: timer = 180; step2 = 2; MakeShot(self, 48)
                  anim_flag == 2: PlaySound(2, 73)
sub 2 0x800ECBC4: timer--; at 0: SetAnime(7); step2 = 3
sub 3 0x800ECC18: anim_flag == 1: body.act4 = 0; step2 = 0
```

## Visual type 13

### MakeShot — 0x800EAA2C

```
o = GetVisualObject(); o.flags = 0x41; o.type = 13; o.var = v
o.flip = parent.flip; o.pos = parent.pos (whole px); clut & 0x7FFF; anime_tbl, sprite_data,
tpage, layer from parent; o+0x7C = parent
if v == 0:  o.x = cam0.x + cam1.x + 160 + (GetRNG() & 127); o.y = cam0.y - cam1.y + 936
if v == 48: o+0x84 = parent.step2
```

### Set-up — 0x800EAB50 (state 0)

```
prio = 32; dmg = dmg_tbl_minor; contact = 0; hp = 1; latch = 1; boxes = 0; state = 1
switch var & 0xF0:
 0  (bomb):    SetAnime(8); contact = 2; terrain = (0, 0, 2, 2); atk = (-3, -3, 6, 6)
               speed = (0, -2.0); gravity = 0x1800; MoveObject; step2 = 0
 16 (shot):    PlaySound(2, 74); SetAnime(10); contact = 4
               terrain = (0, 0, 4, 4); atk = (-4, -4, 8, 8)
               a = ratan2(self.y - mega.y, mega.x - self.x)
               speed_x = rcos(a) * 80; speed_y = rsin(a) * 80       # 5.0 px/frame
               flip = (speed_x != 0) ? RIGHT : LEFT; MoveObject; step2 = 1
 32 (missile): PlaySound(2, 75); SetAnime(9); contact = 4; hp = 2
               atk = (-20, -3, 40, 6); hurt = (-20, -4, 40, 8); timer(+0x88) = 60; +0x90 = 1
               var 33: speed_x = -1.5; flip = LEFT;  x -= 44
               else:   speed_x = +1.5; flip = RIGHT; x += 44
               MoveObject; step2 = 2
 48 (beam):    PlaySound(2, 76); SetAnime(11); contact = 6; box(+0x8C..0x8F) h = 24
               atk = &box; box.oy = -12; box.ox = 127 - 16 * anim_flag; box.w = 16 * anim_flag
               x = parent.x + (flip == LEFT ? -164 : +164); step2 = 3
unless var 48: off screen by 32 -> state 2; else DisplayCheck(16, 16); +0x92 -> display = 0
```

### Main — 0x800EAE90 (state 1)

```
prev = pos; if +0x91 == 0: AnimeTick
STEPS[step2](self)
if +0x90 and ContactWeapon(self) < 0: display = 0; SpawnExplosion; state = 2; return
if ContactMega(self):
    bomb or missile: SpawnExplosion; state = 2
    shot: display = 0; X: SpawnHitEffect, Zero: SpawnExplosion; state = 2
    beam: nothing
unless beam: off screen by 32 -> state 2; else DisplayCheck(16, 16); +0x92 -> display = 0
```

### Bomb (step2 0; subs by `act4`)

```
0 0x800ECE7C: MoveObject2; speed_y = max(speed_y, -4.0)
     on collide_flags or CornerCheck: PlaySound(2, 77); speed = (-1.0, +1.333 (0x15555))
     gravity = 0xB60; collide = 0; act4 = 1; MoveObject2
1 0x800ECF20: MoveObject2; on terrain: PlaySound(2, 77); speed = (-1.0, 0); +0x91 = 1
     timer = 60; act4 = 2
2 0x800ECFA0: MoveObject; 60 updates -> act4 = 3
3 0x800ECFF0: atk = (-16, -16, 32, 32); SpawnExplosion; +0x92 = 1 (hidden); timer = 30; act4 = 4
4 0x800ED03C: 30 updates -> state 2
```

### Shot (step2 1)

```
act4 0: MoveObject; act4 = 1      act4 1: MoveObject
on collide_flags or CornerCheck: (speed_y < 0: SpawnHitEffect); state = 2
```

### Missile (step2 2, Zero only)

```
act4 0 0x800EAFFC: timer--
    timer == 0: speed_y = 0; MoveObject; act4 = 1; return
    dy = mega.y - y: |dy| < 3: speed_y = 0 else speed_y = (mega.y < y) ? +1.5 : -1.5
    when it has passed Zero in x (var 33: x < mega.x; else mega.x < x): speed_y = 0; act4 = 1
    MoveObject
act4 1: MoveObject
```

### Beam (step2 3)

```
act4 0 0x800ED1D8: x = parent.x + (flip == LEFT ? -164 : +164); display = 1
    box.ox = 127 - 16 * anim_flag; box.w = 16 * anim_flag
    when parent.step2 != +0x84: atk = 0; display = 0; act4 = 1
act4 1: state = 2
```

### Delete — 0x800ECDF8 (state 2)

```
if beam: PlaySound(2, 20)
DeleteObject3(self)
```

## Stage object type 10

### Set-up — 0x800EB114 (state 0)

```
+0x75 = 1; +0xA2 = 1; +0x76 = 0; latch = 0
var 0, 1 (walls):   prio = 35; contact = 8; dmg = dmg_tbl_capsule; hp = 0
                    atk = (-32, 16, 16, 16); hurt = (-16, -64, 36, 112); terrain = (2, -8, 18, 56)
                    SetAnime(16); display = 0; step = 0
                    var 0: ofs = (16, -64), flip = RIGHT     var 1: ofs = (304, -64), flip = LEFT
var 16, 17 (press): contact = 127; dmg = dmg_tbl_capsule; hp = 0; atk = 0
                    hurt = (-80, -80, 160, 160); terrain = (0, 0, 80, 80); SetAnime(16); display = 0
                    step = 1; var 16: ofs = (80, -80), flip = LEFT; var 17: ofs = (240, -80), RIGHT
var 32 (head block): contact = 8; dmg = dmg_tbl_capsule; hp = 0; atk = hurt = 0
                    terrain = (0, 0, 28, 56); SetAnime(16); display = 0; step = 2
state++ (1); step2 = 0
```

### Main — 0x800EB2E8 (state 1)

```
prev = pos; body = +0x7C
if body.state != 1: state = 2; display = 0; step = 0; return
if var 32: pos = body.pos
           if body+0x8F == 0 or body+0x8F == 1: return
else:      pos = M(ofs_x, ofs_y); if body.step == 9: y += 16
if body.step == 2: return                        # not solid in the intro
STEPS[step](self)
ContactMega(self)
if press: SolidObject(self)
else if mega.squeeze_timer(+0xE5) == 0:
    SolidObject(self)
    if var 32 and body+0x91 == 1 and (self+0x72 & 4) and (mega+0x89 & 8): mega+0x71 = 0
SolidWeaponHit(self)       # 0x80032B34: shots that touch the terrain box
SolidPushShots(self)       # 0x8003208C
```

### Wall (step 0) — 0x800ED3A0

```
display = 1
switch body+0x90:
 0: MoveToward(&ofs_y, +64, 2)   # down
 1: MoveToward(&ofs_y, -64, 2)   # up (PRESS)
 2: MoveToward(&ofs_y, -64, 4)   # up fast (END CRUSH)
```

### Death — 0x800ED264 (state 2)

```
step 0: walls: SpawnJunk(4, {14, 14, 15, 15}); timer(+0x88) = 60; gap(+0x8A) = 5; step = 1
        others: step = 2
step 1: timer--; at 0: step = 2
        gap--; at 0: SpawnExplosionRand(self, 1, 16, 64); gap = 5
step 2: DeleteObject3(self)
```

## Engine helpers called

| address | name | use here |
|---|---|---|
| `0x80012D9C` | `Rumble` | intro landing, press landing, end crush (5) |
| `0x80016C48` | `PlaySound` | all sounds (bank 2; bank 0 id 14 for the HP tick) |
| `0x800179A4`, `0x80017A04` | `SetAnime`, `AnimeTick` | |
| `0x80025FD8` | `BossLevel` | called in the intro; its result is not read here |
| `0x80029478` | `Quake` | intro, press, end crush |
| `0x80029F38` | `SpawnInRect` | after the death: x 5408–5824, y 224–512 |
| `0x8002BCCC` | (new) `GetMainObjectAfter` | the six parts |
| `0x8002C3F8` | `GetSysObject` | engine object 26 |
| `0x8002C458` | `GetVisualObject` | shots |
| `0x8002C4C4` | `GetStageObject` | the five stage parts |
| `0x8002C9B0` | `DeleteObject3` | body, parts, shots, stage parts |
| `0x8002CBFC`, `0x8002CCB0` | `OffScreenCheck2`, `DisplayCheck` | shots (32 / 16) |
| `0x8002D230`, `0x8002D2B0` | `MoveObject2`, `MoveObject` | shots |
| `0x8002D2D4` | `GetRNG` | patterns, bomb place and delay, quakes |
| `0x8002EE10` | `CornerCheck` | bomb, shot |
| `0x80030ECC` | `ContactMega` | |
| `0x80031474` | `ContactWeapon` | pods, turrets, missile |
| `0x80031DA8` | `SolidObject` | stage parts |
| `0x8003208C` | (new) `SolidPushShots` [guess] | stage parts |
| `0x80032B34` | (new) `SolidWeaponHit` [likely] | stage parts |
| `0x8003D308`, `0x8003D330` | `PlayerEvent`, `PlayerEventEnd` | intro and death |
| `0x80040290` | `HitReactionDispatch` | step 0 (not used) |
| `0x800477E4` | `SpawnJunk` | part and wall debris |
| `0x80049D90`, `0x80049E50`, `0x80049EBC` | `SpawnHitEffect`, `SpawnExplosion`, `SpawnExplosionRand` | |
| `0x80065944`, `0x80065A14`, `0x80065B74` | `rsin`, `rcos`, `ratan2` | aimed shot |

## Object struct (the fields used here)

| offset | body (`var` 0) | parts (16–34) | visual 13 | stage 10 |
|---|---|---|---|---|
| `+0x07` act4 | message to the parts: 1–3 turret shot, 4 drill, 5–7 beam pod; 0 = done | | sub-step | |
| `+0x45` anim flag | | 1 fire / end, 2 sound | beam width / 16 | |
| `+0x5C` hp | 48, the boss HP | 16 | 1 (missile 2) | 0 |
| `+0x61` | death blink gap | | | |
| `+0x7C` | timer | | parent | body |
| `+0x7E` | counter / blink timer | | | |
| `+0x80` | `ofs_x` fraction | body | | |
| `+0x82` | `ofs_x` (machine x) | | | `ofs_x` |
| `+0x84` | | pod invulnerability | beam: parent `step2` at the start | |
| `+0x86` | `ofs_y` (machine y) | | | `ofs_y` |
| `+0x88` | engine object 26 | | timer | death timer |
| `+0x8A` | | | | explosion gap |
| `+0x8C`, `+0x8D`, `+0x8E` | THINK mode, pattern, index | | beam box ox, oy, w | |
| `+0x8F` | head state: 0 hidden, 2 out | | beam box h (24) | |
| `+0x90` | wall order: 0 down, 1 up, 2 up fast | | weapon test on | |
| `+0x91` | head going up | | animation stop | |
| `+0x92` | random-pattern count | | hidden | |
