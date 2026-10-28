# Killer Moth, its exhaust flame and its powder shot: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`). Addresses are RAM addresses in overlay 03.
Names of engine helpers and struct fields are in `catalog/helpers.json` of mmx-demake-tools.
`current_main` (`0x1F80004C`) points to the main object being updated, so `current_main.f` is
`self.f` here. The code uses that pointer for the fields `+0x80`–`+0x8C`; the pseudo code writes
`self`. The player object is `mega` (`0x800970A0`); `mega.var` (`+0x02`) is 0 for X and non-zero
for Zero (from the D-1000 notes).

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800F7254` state | 0 | `moth_init` `0x800EB37C` |
| | 1 | `moth_main` `0x800EB490` |
| | 2 | `moth_delete` `0x800EB57C` (`DeleteObject`) |
| | 3 | `moth_delete2` `0x800EB59C` (`DeleteObject2`) |
| `0x800F7264` step | 0 | `HitReactionDispatch` `0x80040290` (engine) |
| | 1 | `moth_nop` `0x800EB984` |
| | 2 | `moth_hover` `0x800EB5BC` |
| | 3 | `moth_swoop` `0x800EB728` |
| | 4 | `moth_circle` `0x800EB7A8` |
| | 5 | `moth_enter` `0x800EB98C` |
| `0x800F727C` step2 (HOVER) | 0–3 | `hover_start` `0x800EB5F8`, `hover_wait` `0x800EB644`, `hover_windup` `0x800EB6C4`, `hover_windup_wait` `0x800EB708` |
| `0x800F728C` step2 (SWOOP) | 0–1 | `swoop_start` `0x800EB764`, `swoop_move` `0x800EAE94` |
| `0x800F7294` step2 (CIRCLE) | 0–1 | `circle_start` `0x800EB7E4`, `circle_move` `0x800EB870` |
| `0x800F72EC` step2 (ENTER) | 0–2 | `enter_wait_view` `0x800EB0A0`, `enter_delay` `0x800EB9C8`, `enter_fly_home` `0x800EB170` |
| `0x800F7300` powder state | 0–2 | `powder_init` `0x800EBC50`, `powder_move` `0x800EBCEC`, `powder_delete` `0x800EBD80` |
| `0x800F730C` flame state | 0–2 | `flame_init` `0x800EBDA0`, `flame_follow` `0x800EBE14`, `flame_delete` `0x800EBF4C` |
| main table entry 4 | | `m04_moth` `0x800EB2C8` |
| visual table entry 2 | | `v02_moth_powder` `0x800EB304` |
| misc table entry 4 | | `e04_moth_flame` `0x800EB340` |

Data:

| address | contents |
|---|---|
| `0x800F7180` | contact box (−8, −8, 16, 16) |
| `0x800F7184` | hurt box (−8, −8, 16, 16) |
| `0x800F7234` | animation pointer list, 8 entries |
| `0x800F729C` | ENTER delays, 8 × u16: 0, 8, 16, 32, 48, 64, 80, 96 |
| `0x800F72AC` | ENTER start points, 16 × {s16 x, s16 y}, relative to the camera |
| `0x800F72F8` | powder contact box (−6, −6, 12, 12) |
| `0x800F72FC` | powder terrain box (0, 0, 5, 5) |
| `0x800743DC` | damage table (standard, 64 weapon ids) |

## Moth

### `m04_moth` — `0x800EB2C8` (main table entry 4)

```
STATE_TABLE[self.state](self)
```

### `moth_init` — `0x800EB37C` (state 0)

```
self.contact_damage = 4 ; self.timer(+0x7C) = 0 ; self.hp = 8 ; self.iframes = 0 ; self.display = 0
if self.var == 0:
    self.flame_mode(+0x88) = 1 ; self.step = 2 (HOVER)
    self.terrain_box = 0 ; self.hurt_box = 0x800F7184 ; self.attack_box = 0x800F7180
else:
    self.flame_mode = 0 ; self.step = 5 (ENTER)
    self.terrain_box = self.hurt_box = self.attack_box = 0
self.anime_table = 0x800F7234 ; self.priority = 5 ; self.flip = 0 ; self.latch = 0
self.damage_table = 0x800743DC ; self.layer = mega.layer ; self.prev = position
SetAnime(self, 0)
self.speed_x = self.speed_y = self.accel_x = self.gravity = 0
self.state = 1 ; self.step2 = 0
moth_make_flame(self, 0)
```

### `moth_main` — `0x800EB490` (state 1)

```
self.prev = position
if self.step != 5: moth_face_player(self)
AnimeTick(self)
STEP_TABLE[self.step](self)
if ContactWeapon(self) < 0:
    SpawnExplosion(self) ; DropItem(self, 18) ; self.state = 2 ; return
ContactMega(self)
if OffScreenCheck2(self, 80, 160): self.state = 3 ; return
if self.step != 5: DisplayCheck(self, 48, 64)
```

### `moth_delete` — `0x800EB57C` (state 2), `moth_delete2` — `0x800EB59C` (state 3)

```
DeleteObject(self)        /  DeleteObject2(self)
```

### `moth_hover` — `0x800EB5BC` (step 2)

```
HOVER_TABLE[self.step2](self)

hover_start (0x800EB5F8):
    self.flame_mode = 1 ; SetAnime(self, 0) ; self.timer = 180 ; self.step2 += 1
hover_wait (0x800EB644):
    self.timer -= 1 (u16)
    if self.timer == 0: self.step2 += 1
    if |mega.x.px - self.x.px| < 48: self.step2 += 1        // both can add 1 in one update
hover_windup (0x800EB6C4):
    SetAnime(self, 2) ; self.flame_mode = 2 ; self.step2 += 1
hover_windup_wait (0x800EB708):
    if self.anime.flag == 1: self.step = 3 (SWOOP) ; self.step2 = 0
```

### `moth_swoop` — `0x800EB728` (step 3)

```
SWOOP_TABLE[self.step2](self)

swoop_start (0x800EB764):
    self.flame_mode = 3 ; SetAnime(self, 4) ; self.step2 += 1
swoop_move (0x800EAE94):
    h = 48 if mega.var == 0 else 64
    dx = mega.x.px - self.x.px
    dy = mega.y.px - (self.y.px + h)
    a = ratan2(-dy, dx)
    self.accel_x = 0 ; self.gravity = 0
    self.speed_x = rcos(a) * 160 / 4          // 2.5 px/frame at full
    self.speed_y = rsin(a) * 160 / 8          // 1.25 px/frame at full
    MoveObject(self)
    if |mega.x.px - self.x.px| < 3 and |(self.y.px + h) - mega.y.px| < 3:
        self.x.px = mega.x.px ; self.y.px = mega.y.px - h
        self.spin(+0x84) = -32 if self.flip == 0 else 32
        self.step = 4 (CIRCLE) ; self.step2 = 0
```

### `moth_circle` — `0x800EB7A8` (step 4)

```
CIRCLE_TABLE[self.step2](self)

circle_start (0x800EB7E4):
    self.centre_x(+0x8A) = self.x.px ; self.centre_y(+0x8C) = self.y.px - 16
    self.angle(+0x80) = 3072
    SetAnime(self, 6) ; self.flame_mode = 1
    self.timer = 20 ; self.shots(+0x7E) = 8 ; self.step2 += 1
circle_move (0x800EB870):
    self.angle = (self.angle + self.spin) & 0xFFF
    self.x.px = self.centre_x + (rcos(self.angle) * 16 >> 12)
    self.y.px = self.centre_y - (rsin(self.angle) * 16 >> 12)
    self.timer -= 1 (u16)
    if self.timer == 0:
        self.timer = 20
        PlaySound(2, 5, self)
        moth_make_powder(self, 0)
        self.shots -= 1 (u16)
        if self.shots == 0: self.step = 2 (HOVER) ; self.step2 = 0
```

### `moth_nop` — `0x800EB984` (step 1)

```
return
```

### `moth_enter` — `0x800EB98C` (step 5)

```
ENTER_TABLE[self.step2](self)

enter_wait_view (0x800EB0A0):
    c = camera_layer0
    if |c.x.px - (self.x.px - 160)| < 160 and |c.y.px - (self.y.px - 120)| < 120:
        self.timer = DELAYS[GetRNG() & 7]          // 0x800F729C
        self.step2 += 1
enter_delay (0x800EB9C8):
    self.timer -= 1
    if (s16) self.timer != -1: return               // timer + 1 updates
    self.home_x(+0x8A) = self.x.px ; self.home_y(+0x8C) = self.y.px
    self.flame_mode = 3 ; SetAnime(self, 4)
    i = GetRNG() & 15
    self.x.px = camera_layer0.x.px + POINTS[i].x ; self.y.px = camera_layer0.y.px + POINTS[i].y   // 0x800F72AC
    moth_face_player(self)
    self.hurt_box = 0x800F7184 ; self.attack_box = 0x800F7180
    self.step2 += 1
enter_fly_home (0x800EB170):
    a = ratan2(self.y.px - self.home_y, self.home_x - self.x.px)
    self.accel_x = 0 ; self.gravity = 0
    self.speed_x = rcos(a) * 160 / 8 ; self.speed_y = rsin(a) * 160 / 8     // 1.25 px/frame
    MoveObject(self)
    DisplayCheck(self, 48, 64)
    if |self.home_x - self.x.px| < 3 and |self.home_y - self.y.px| < 3:
        self.x.px = self.home_x ; self.y.px = self.home_y
        self.step = 2 (HOVER) ; self.step2 = 0
```

### `moth_make_flame` — `0x800EBABC`

```
f = GetMiscObject()
if f:
    f.flags = 0x41 ; f.type = 4 ; f.var = id (0)
    f.flip = self.flip ; f.tex_cord = self.tex_cord ; f.clut_cord = self.clut_cord & 0x7FFF
    f.anime_table = self.anime_table ; f.sprite_data = self.sprite_data ; f.layer = self.layer
    f.x = (self.x.px, frac 0) ; f.y = (self.y.px, frac 0)
    f.parent(+0x50) = self
```

### `moth_make_powder` — `0x800EBB70`

```
p = GetVisualObject()
if p:
    p.flags = 0x41 ; p.type = 2 ; p.var = id (0)
    p.parent(+0x7C) = self ; p.flip = self.flip
    p.x = (self.x.px, frac 0) ; p.y = (self.y.px, frac 0)
    p.clut_cord = self.clut_cord & 0x7FFF ; p.anime_table = self.anime_table
    p.sprite_data = self.sprite_data ; p.tex_cord = self.tex_cord ; p.layer = self.layer
```

### `moth_face_player` — `0x800EBC20`

```
self.flip = 0x40 if self.x.px < mega.x.px else 0
```

## Powder shot (visual type 2)

### `v02_moth_powder` — `0x800EB304`

```
POWDER_TABLE[self.state](self)
```

### `powder_init` — `0x800EBC50` (state 0)

```
self.contact_damage = 4 ; self.hp = 0 ; self.priority = 4 ; self.latch = 1 ; self.hurt_box = 0
SetAnime(self, 7)
self.speed_x = 0 ; self.speed_y = -0x3FFF (-0.25 px/frame, down) ; self.accel_x = self.gravity = 0
self.attack_box = 0x800F72F8 ; self.terrain_box = 0x800F72FC
self.state = 1 ; self.step = self.step2 = self.byte7 = 0 ; self.display = 0
```

### `powder_move` — `0x800EBCEC` (state 1)

```
AnimeTick(self) ; MoveObject(self) ; ContactMega(self)
if self.collide_flags: self.state += 1 ; return
CornerCheck(self)
if self.collide_flags: self.state += 1 ; return
if OffScreenCheck2(self, 32, 32): self.state += 1 ; return
DisplayCheck(self, 16, 16)
```

### `powder_delete` — `0x800EBD80` (state 2)

```
DeleteObject3(self)
```

## Exhaust flame (misc type 4)

### `e04_moth_flame` — `0x800EB340`

```
FLAME_TABLE[self.state](self)
```

### `flame_init` — `0x800EBDA0` (state 0)

```
self.last_mode(+0x54, u8) = 0xFF ; self(+0x56, u16) = 0      // written through current_misc
self.flip = 0 ; self.priority = 6
SetAnime(self, 1)
self.display = 0 ; self.step = 0 ; self.speed_x = self.accel_x = self.speed_y = self.gravity = 0
self.state = 1
```

### `flame_follow` — `0x800EBE14` (state 1)

```
m = self.parent
if m.state != 1: self.display = 0 ; self.state += 1 ; return
AnimeTick(self)
self.flip = m.flip ; self.x.px = m.x.px ; self.y.px = m.y.px
mode = m.flame_mode
if mode != self.last_mode:
    switch mode:
        0: self.display = 0
        1: self.display = 1 ; SetAnime(self, 1)
        2: self.display = 1 ; SetAnime(self, 3)
        3: self.display = 1 ; SetAnime(self, 5)
self.last_mode = mode
```

### `flame_delete` — `0x800EBF4C` (state 2)

```
DeleteObject3(self)
```

## Engine helpers called

All 21 helpers are named in `catalog/helpers.json` (see `kit/summary.md`). No unnamed call outside
the walk. New facts from this enemy:

| address | name | fact |
|---|---|---|
| `0x80065B74` | `ratan2(y, x)` | `y` is "up" (screen y inverted by the caller), result 0..4096 per turn |
| `0x8002D2B0` | `MoveObject` | used here for flying objects with `gravity` = 0 |
| `0x80016C48` | `PlaySound(2, 5, obj)` | the powder-drop sound of stage 03 |

## Object struct (the fields used here)

Main object (moth):

| off | name | meaning | confidence |
|---|---|---|---|
| `+0x02` | `var` | record id: 0 HOVER start, ≠ 0 ENTER start | certain |
| `+0x03` | `display` | 0 at the set-up | certain |
| `+0x04` | `state` | 0–3 | certain |
| `+0x05` | `step` | 0–5 | certain |
| `+0x06` | `step2` | sub-step | certain |
| `+0x08`/`+0x0A` | `x`, `x.px` | position | certain |
| `+0x0C`/`+0x0E` | `y`, `y.px` | position | certain |
| `+0x14` | `layer` | from the player | certain |
| `+0x15` | `flip` | 0 faces left, `0x40` faces right | certain |
| `+0x16` | `priority` | 5 | certain |
| `+0x18`/`+0x1C` | `prev_x`, `prev_y` | saved each update | certain |
| `+0x20`/`+0x24` | `speed_x`, `speed_y` | SWOOP and ENTER | certain |
| `+0x28`/`+0x2C` | `accel_x`, `gravity` | always 0 | certain |
| `+0x30` | `anime_table` | `0x800F7234` | certain |
| `+0x3C`, `+0x40`, `+0x42` | `sprite_data`, `tex_cord`, `clut_cord` | copied to the flame and the powder | certain |
| `+0x45` | `anime.flag` | wind-up end (flag 1) | certain |
| `+0x50`/`+0x54` | `attack_box`, `hurt_box` | both 16 × 16 | certain |
| `+0x58` | `damage_table` | `0x800743DC` | certain |
| `+0x5C` | `hp` | 8 | certain |
| `+0x60` | `contact_damage` | 4 | certain |
| `+0x61` | `iframes` | 0 | certain |
| `+0x67` | `latch` | 0 | certain |
| `+0x68` | `terrain_box` | 0 | certain |
| `+0x7C` | `timer` (u16) | HOVER 180, CIRCLE shot timer 20, ENTER delay | certain |
| `+0x7E` | `shots` (u16) | CIRCLE shots left (8) | certain |
| `+0x80` | `angle` (s32) | CIRCLE angle, 0..4095 | certain |
| `+0x84` | `spin` (s32) | CIRCLE angle step, ±32 | certain |
| `+0x88` | `flame_mode` (u8) | 0 off, 1 down, 2 diagonal, 3 sideways; read by the flame | certain |
| `+0x8A`/`+0x8C` | `centre_x`, `centre_y` (s16) | CIRCLE centre; in ENTER the home point | certain |

Misc object (flame): `+0x50` parent (the moth), `+0x54` last flame mode (u8, `0xFF` at the start),
`+0x56` u16 cleared at the start, not read [certain].

Visual object (powder): `+0x7C` parent (the moth, s32 pointer, not read), `+0x70` `collide_flags`,
`+0x68` terrain box [certain].
