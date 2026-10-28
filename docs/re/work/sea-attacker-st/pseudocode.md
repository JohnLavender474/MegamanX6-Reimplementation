# Sea Attacker ST and its bubble shot: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`). Addresses are RAM addresses in overlay 09.
Names of engine helpers and struct fields are in `catalog/helpers.json` of mmx-demake-tools.
`self.face_tm` (`+0x80`) and `self.rise_ofs` (`+0x82`) are written through `current_main`
(`0x1F80004C`), which is `self` during the update [likely].

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800F5C58` state | 0 | `sea_init` `0x800EABCC` |
| | 1 | `sea_main` `0x800EACD0` |
| | 2 | `sea_delete` `0x800EADA0` (`DeleteObject`) |
| | 3 | `sea_delete2` `0x800EADC0` (`DeleteObject2`) |
| `0x800F5C68` step | 0 | `HitReactionDispatch` `0x80040290` (engine) |
| | 1 | `sea_nop` `0x800EB1DC` |
| | 2 | `sea_shooter` `0x800EADE0` |
| | 3 | `sea_diver` `0x800EAF88` |
| `0x800F5C78` step2 (shooter) | 0 | `shooter_wait` `0x800EAE1C` |
| | 1 | `shooter_fire` `0x800EAE98` |
| | 2 | `shooter_rest` `0x800EAF0C` |
| `0x800F5C84` step2 (diver) | 0 | `diver_curl` `0x800EAFD4` |
| | 1 | `diver_rise` `0x800EB048` |
| | 2 | `diver_hover` `0x800EB0B8` |
| | 3 | `diver_dash` `0x800EB16C` |
| | 4 | `diver_drop` `0x800EB1D4` (empty) |
| `0x800F5CA4` shot state | 0 | `shot_init` `0x800EA880` |
| | 1 | `shot_move` `0x800EA9B0` |
| | 2 | `shot_delete` `0x800EB2B4` (`DeleteObject3`) |
| visual table entry 9 | | `v09_sea_shot` `0x800EAB90` |

## Sea Attacker ST

### `m10_sea_attacker` — `0x800EAB54` (main table entry 10)

```
STATE_TABLE[self.state](self)
```

### `sea_init` — `0x800EABCC` (state 0)

```
self.contact_damage = 4 ; self.hp = 6 ; self.iframes = 0 ; self.display = 0
self.anime_table = 0x800F5C28 ; self.priority = 7
self.damage_table = 0x800743DC
self.attack_box = 0x800F5B98          // (-1, -16, 10, 28)
self.hurt_box   = 0x800F5B9C          // (-2, -16, 11, 30)
self.flip = 0 ; self.latch = 0
self.speed_x = self.speed_y = self.accel_x = self.gravity = 0
self.terrain_box = 0 ; self.layer = mega.layer
self.prev_x = self.x ; self.prev_y = self.y
self.face_tm = 30
sea_face_mega(self)
if self.var == 0: SetAnime(self, 0) ; self.step = 2      // shooter
else:             SetAnime(self, 3) ; self.step = 3      // diver
sea_face_mega(self)
self.state = 1 ; self.step2 = 0
```

### `sea_main` — `0x800EACD0` (state 1)

```
self.prev_x = self.x ; self.prev_y = self.y
AnimeTick(self)
STEP_TABLE[self.step](self)
if ContactWeapon(self) < 0:                         // killed
    SpawnJunk(5, {4, 5, 6, 7, 8}, self)             // list at 0x800F5C50
    SpawnExplosion(self)
    DropItem(self, 19)
    self.state = 2
    return
ContactMega(self)
if OffScreenCheck2(self, 64, 64): self.state = 3
else: DisplayCheck(self, 32, 32)
```

### `sea_delete` — `0x800EADA0` (state 2), `sea_delete2` — `0x800EADC0` (state 3)

```
DeleteObject(self)        /  DeleteObject2(self)
```

### `sea_shooter` — `0x800EADE0` (step 2)

```
SHOOTER_TABLE[self.step2](self)
```

### `shooter_wait` — `0x800EAE1C` (step 2, sub-step 0)

```
d = mega.x.px - self.x.px
if |d| <= 96:
    sea_face_mega(self)
    self.step2 += 1
    SetAnime(self, 1)
```

### `shooter_fire` — `0x800EAE98` (step 2, sub-step 1)

```
if self.anime.flag == 1:
    PlaySound(2, 26, self)
    sea_shot_spawn(self)
elif self.anime.flag == 2:
    SetAnime(self, 0)
    self.timer(+0x7C) = 20
    self.step2 += 1
```

### `shooter_rest` — `0x800EAF0C` (step 2, sub-step 2)

```
self.face_tm -= 1
if self.face_tm == 0: sea_face_mega(self) ; self.face_tm = 30
self.timer -= 1
if self.timer == 0: self.step2 = 0
```

### `sea_diver` — `0x800EAF88` (step 3)

```
DIVER_TABLE[self.step2](self)
MoveObject(self)                    // x += speed_x ; y -= speed_y
```

### `diver_curl` — `0x800EAFD4` (step 3, sub-step 0)

```
if self.anime.flag == 1:
    self.attack_box = 0x800F5BA0     // (-8, -8, 16, 16)
    self.hurt_box   = 0x800F5BA4     // (-8, -8, 16, 16)
    self.speed_y = 0x3C000           // 3.75 px/frame up
    self.rise_ofs = GetRNG() & 0x1F  // 0..31 px
    self.step2 += 1
```

### `diver_rise` — `0x800EB048` (step 3, sub-step 1)

```
if mega.y.px - self.y.px >= self.rise_ofs:
    self.priority = 18
    sea_face_mega(self)
    self.timer = 40
    self.speed_x = self.speed_y = 0
    self.step2 += 1
```

### `diver_hover` — `0x800EB0B8` (step 3, sub-step 2)

```
self.face_tm -= 1
if self.face_tm == 0: sea_face_mega(self) ; self.face_tm = 30
self.timer -= 1
if self.timer == 0:
    sea_face_mega(self)
    self.speed_x = +0x3C000 if self.flip else -0x3C000     // 3.75 px/frame to the facing side
    self.step2 += 1
```

### `diver_dash` — `0x800EB16C` (step 3, sub-step 3)

```
d = mega.x - self.x                 // 16.16 fixed point
if |d| > 0x18000: return            // 1.5 px
self.speed_x = 0
self.speed_y = -0x3C000             // 3.75 px/frame down
self.step2 += 1
```

### `diver_drop` — `0x800EB1D4` (sub-step 4), `sea_nop` — `0x800EB1DC` (step 1)

```
return
```

### `sea_shot_spawn` — `0x800EB1E4`

```
s = GetVisualObject()
if s:
    s.flags = 0x41 ; s.type = 9 ; s.var = 0
    s.flip = self.flip
    s.x = (self.x.px, sub 0) ; s.y = (self.y.px, sub 0)
    s.clut_cord = self.clut_cord & 0x7FFF
    s.anime_table = self.anime_table ; s.sprite_data = self.sprite_data
    s.tex_cord = self.tex_cord ; s.layer = self.layer
```

### `sea_face_mega` — `0x800EB284`

```
self.flip = 0x40 if self.x.px < mega.x.px else 0      // face right / left
```

## Bubble shot (visual type 9)

### `v09_sea_shot` — `0x800EAB90`

```
SHOT_TABLE[self.state](self)
```

### `shot_init` — `0x800EA880` (state 0)

```
self.contact_damage = 4 ; self.hp = 1
self.damage_table = 0x8007429C          // 127 for every id
self.priority = 5
SetAnime(self, 2)
self.latch = 1
self.terrain_box = 0x800F5C98           // (0, 0, 2, 2)
self.attack_box  = 0x800F5C9C           // (-2, -2, 4, 4)
self.hurt_box    = 0x800F5CA0           // (-2, -2, 4, 4)
self.state = 1 ; self.step = self.step2 = self.sub(+0x07) = 0
self.x.px += 6 if self.flip == 0 else -6
self.y.px -= 20
ratan2(self.y.px - mega.y.px, mega.x.px - self.x.px)   // result not used
self.accel_x = self.gravity = 0
self.speed_x = 0 ; self.speed_y = 0x40000             // 4 px/frame up
if OffScreenCheck2(self, 32, 32): self.state += 1
else: DisplayCheck(self, 16, 16)
```

### `shot_move` — `0x800EA9B0` (state 1)

```
AnimeTick(self)
MoveObject(self)
switch self.sub:
case 0:                                         // RISE
    if not (mega.y.px < self.y.px):
        SetAnime(self, 9)
        self.wait(+0x88) = 40 if game.difficulty < 2 else 8
        self.speed_x = self.speed_y = 0
        self.sub += 1
case 1:                                         // WAIT
    self.wait -= 1
    if self.wait == 0:
        SetAnime(self, 2)
        self.speed_x = -0x40000 if mega.x.px < self.x.px else 0x40000
        self.sub += 1
default: pass                                   // FLY
if ContactWeapon(self) < 0:
    SpawnExplosion(self) ; self.state += 1 ; return
self.clut_cord &= 0x7FFF
ContactMega(self)                               // result not used
if self.collide_flags(+0x70) == 0:
    CornerCheck(self)
    if self.collide_flags == 0:
        if OffScreenCheck2(self, 32, 32): self.state += 1
        else: DisplayCheck(self, 16, 16)
        return
SpawnShotBurst(self)                            // 0x80049D90
self.state += 1
```

### `shot_delete` — `0x800EB2B4` (state 2)

```
DeleteObject3(self)
```

## Engine code read outside the walk

### `SpawnShotBurst` — `0x80049D90` (name proposed)

```
e = GetEffectObject()                   // 0x8002C398
if e:
    e.flags = 0x21 ; e.type = 3 ; e.var = 1
    e.state = e.step = e.step2 = 0 ; e(+0x38) = 0
    e.layer = mega.layer
    e.anime_table = 0x8007789C ; e.tex_cord = 0 ; e.clut_cord = 0x7802
    e.priority = 0 ; e.flip = 0
    e.sprite_data = *(0x1F80001C) + (*(0x1F80001C))[1]
    if obj.speed_x > 0: e.flip = 0x40
    e.x.px = obj.x.px ; e.y.px = obj.y.px
```

### `GetEffectObject` — `0x8002C398` (name proposed)

```
for p in 0x80091298 .. 0x80093C98 step 0x70:     // 96 slots
    if p.flags == 0: clear 0x70 bytes ; return p
return 0
```

## Engine helpers called

See `kit/summary.md` (19 helpers, all named in `catalog/helpers.json`), plus the two above.

| address | name | use here |
|---|---|---|
| `0x800179A4` | `SetAnime` | animations 0, 1, 3 (body), 2, 9 (shot) |
| `0x80017A04` | `AnimeTick` | each update, body and shot |
| `0x80016C48` | `PlaySound` | bank 2, id 26 |
| `0x8002C458` | `GetVisualObject` | the shot |
| `0x8002CA14`, `0x8002CA54`, `0x8002C9B0` | `DeleteObject`, `DeleteObject2`, `DeleteObject3` | death, off screen, shot end |
| `0x8002CBFC`, `0x8002CCB0` | `OffScreenCheck2`, `DisplayCheck` | 64/32 px (body), 32/16 px (shot) |
| `0x8002D2B0` | `MoveObject` | diver, shot |
| `0x8002D2D4` | `GetRNG` | diver rise margin |
| `0x8002EE10` | `CornerCheck` | shot against the terrain |
| `0x80030ECC`, `0x80031474` | `ContactMega`, `ContactWeapon` | body and shot |
| `0x80040290` | `HitReactionDispatch` | step 0 (not used) |
| `0x800477E4`, `0x80049E50`, `0x8004E654` | `SpawnJunk`, `SpawnExplosion`, `DropItem` | death |
| `0x80065B74` | `ratan2` | shot set-up, result not used |

## Object struct (the fields used here)

Main object (body):

| off | name | meaning | confidence |
|---|---|---|---|
| `+0x02` | `var` | 0 shooter, ≠ 0 diver | certain |
| `+0x04` / `+0x05` / `+0x06` | `state` / `step` / `step2` | as in the tables above | certain |
| `+0x08`–`+0x0E` | `x`, `y` | position | certain |
| `+0x14` | `layer` | from the player | certain |
| `+0x15` | `flip` | 0 left, `0x40` right | certain |
| `+0x16` | `priority` | 7; 18 after RISE | certain |
| `+0x18`, `+0x1C` | `prev_x`, `prev_y` | | certain |
| `+0x20`, `+0x24` | `speed_x`, `speed_y` | | certain |
| `+0x28`, `+0x2C` | `accel_x`, `gravity` | 0 | certain |
| `+0x30` | `anime_table` | `0x800F5C28` | certain |
| `+0x45` | `anime.flag` | FIRE and CURL events | certain |
| `+0x50`, `+0x54` | `attack_box`, `hurt_box` | body boxes; ball boxes after CURL | certain |
| `+0x58` | `damage_table` | `0x800743DC` | certain |
| `+0x5C`, `+0x60`, `+0x61` | `hp`, `contact_damage`, `iframes` | 6, 4, 0 | certain |
| `+0x67` | `latch` | 0 | certain |
| `+0x68` | `terrain_box` | 0 (none) | certain |
| `+0x7C` | `timer` (u16) | rest 20, hover 40 | certain |
| `+0x80` | `face_tm` (u16) | face-the-player period, 30 | certain (likely through `current_main`) |
| `+0x82` | `rise_ofs` (s16) | 0..31 px above the player | certain (likely through `current_main`) |

Visual object (bubble shot):

| off | name | meaning | confidence |
|---|---|---|---|
| `+0x07` | `sub` | RISE 0, WAIT 1, FLY 2 | certain |
| `+0x15` | `flip` | from the parent; sets the spawn offset | certain |
| `+0x42` | `clut_cord` | bit 15 cleared each update | certain |
| `+0x50`, `+0x54`, `+0x68` | boxes | `0x800F5C9C`, `0x800F5CA0`, `0x800F5C98` | certain |
| `+0x58` | `damage_table` | `0x8007429C` | certain |
| `+0x70` | `collide_flags` | set by `CornerCheck` | certain |
| `+0x88` | `wait` (u16) | 40 or 8 | certain |
