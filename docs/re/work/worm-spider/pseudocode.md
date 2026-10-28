# Worm Spider, its effects and its shot: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`). Addresses are RAM addresses in overlay 03.
Names of engine helpers and struct fields are in `catalog/helpers.json` of mmx-demake-tools.
`self` is the object; the code reaches `+0x80`–`+0x87` through `current_main`, which is `self`.

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800F70E0` state | 0 | `worm_init` `0x800E99D4` |
| | 1 | `worm_main` `0x800EA318` |
| | 2 | `worm_delete` `0x800EA404` (`DeleteObject`) |
| | 3 | `worm_delete2` `0x800EA424` (`DeleteObject2`) |
| `0x800F70F0` step | 0 | `HitReactionDispatch` `0x80040290` (engine) |
| | 1 | `worm_nop` `0x800EAB34` |
| | 2 | `worm_wait` `0x800EA444` |
| | 3 | `worm_emerge` `0x800EA564` |
| | 4 | `worm_attack` `0x800EA5CC` |
| | 5 | `worm_sway` `0x800EA728` |
| | 6 | `worm_retract` `0x800EA834` |
| | 7 | `worm_jump` `0x800EA93C` |
| `0x800F7110` WAIT | 0, 1 | `wait_start` `0x800EA480`, `wait_near` `0x800EA4DC` |
| `0x800F7118` EMERGE | 0, 1, 2 | `emerge_start` `0x800E9B80`, `emerge_grow` `0x800E9C98`, `emerge_wait` `0x800EA5A0` |
| `0x800F7124` ATTACK | 0, 1, 2 | `attack_start` `0x800EA608`, `attack_fire` `0x800EA66C`, `attack_wait` `0x800EA6FC` |
| `0x800F7130` SWAY | 0, 1 | `sway_start` `0x800EA764`, `sway_wait` `0x800EA7E4` |
| `0x800F7138` RETRACT | 0, 1, 2 | `retract_start` `0x800EA870`, `retract_anim` `0x800EA8E8`, `retract_wait` `0x800EA910` |
| `0x800F7144` JUMP | 0–3 | `jump_start` `0x800EA978`, `jump_air` `0x800EAA04`, `jump_lob` `0x800EAA78`, `jump_fall` `0x800EAB14` |
| `0x800F7160` shot state (visual type 3, entry `0x800EA2A0`) | 0, 1, 2 | `shot_init` `0x800E9DCC`, `shot_main` `0x800EACE0`, `shot_delete` `0x800EADCC` |
| `0x800F716C` effect state (misc type 5, entry `0x800EA2DC`) | 0–3 | `fx_init` `0x800E9EF4`, `fx_main` `0x800EA0B0`, `fx_delete` `0x800EADEC`, `fx_blink` `0x800EAE0C` |
| `0x800E9864` `fx_init` switch by var (0–17) | | 0 → `0x800E9F48`, 1 → `0x800E9F68`, 2 → `0x800E9F94`, 3 → `0x800E9FF8`, 4–15 → end, 16 → `0x800EA05C`, 17 → `0x800EA070` |
| `0x800E98AC` `fx_main` switch by var (0–17) | | 0, 1 → `0x800EA144`; 2, 3 → `0x800EA164`; 4–15 → end; 16 → `0x800EA20C`; 17 → `0x800EA234` |

## Worm

### `m03_worm_spider` — `0x800EA264` (main table entry 3)

```
STATE_TABLE[self.state](self)
```

### `worm_init` — `0x800E99D4` (state 0)

```
self.contact_damage = 4 ; self.anime_table = 0x800F7064 ; self.hp = 16
self.iframes = 0 ; self.display = 0 ; self.hit_f(+0x87) = 0
self.terrain_box = self.hurt_box = self.attack_box = NULL
self.layer = mega.layer ; self.flip = 0 ; self.priority = 7 ; self.latch = 0
self.damage_table = 0x800743DC
self.speed_x = self.speed_y = self.accel_x = self.gravity = 0
self.prev = position
worm_face(self)
self.state = 1 ; self.step2 = 0
self.box(+0x80) = *(HITBOX *)0x800F6CC0            // (-4, -26, 8, 52)
switch self.var:
  0: SetAnime(self, 0) ; self.step = 2 ; self.y.px -= 32
  1: SetAnime(self, 14) ; self.step = 2 ; self.y.px += 32
  2: SetAnime(self, 7) ; self.jumps(+0x86) = 3 ; self.base_y(+0x84) = self.y.px
     fx_spawn(self, 16)                              // water ripple
     self.step = 7 ; self.terrain_box = NULL
     self.attack_box = 0x800F6CBC ; self.hurt_box = 0x800F6CC0
     self.y.px += 26
```

### `worm_face` — `0x800EACB0`

```
self.flip = 0x40 if self.x.px < mega.x.px else 0
```

### `worm_main` — `0x800EA318` (state 1)

```
self.prev = position
AnimeTick(self)
STEP_TABLE[self.step](self)
r = ContactWeapon(self)
if r > 0:   self.hit_f = 1
elif r < 0: SpawnExplosion(self) ; DropItem(self, 18) ; self.state = 2 ; return
else:       self.hit_f = 0
ContactMega(self)
if OffScreenCheck2(self, 80, 160): self.state = 3
else: DisplayCheck(self, 48, 64)
```

### `worm_wait` — `0x800EA444` (step 2)

```
wait_start (0x800EA480): SetAnime(self, 0 if var == 0 else 14 if var == 1) ; self.step2 += 1
wait_near  (0x800EA4DC):
    if |mega.x.px - self.x.px| <= 96 and |mega.y.px - self.y.px| <= 96:
        self.step = 3 ; self.step2 = 0
```

### `worm_emerge` — `0x800EA564` (step 3)

```
H = 0x800F70C4      // 28 heights: 2 4 12 12 12 12 16 16 16 16 20 28 28 28 28 36 36 36 44 44 44 44 46 48 48 48 48 48

emerge_start (0x800E9B80):
    PlaySound(2, 6, self)
    self.attack_box = self.hurt_box = &self.box
    if var == 0:
        SetAnime(self, 2)
        self.box.oy = 26 - H[self.anime.flag - 1] ; self.box.h = H[self.anime.flag - 1]
        fx_spawn(self, 0)                            // debris
    elif var == 1:
        SetAnime(self, 16)
        self.box.oy = -30 ; self.box.h = H[self.anime.flag - 1]
        fx_spawn(self, 1)
    self.step2 += 1

emerge_grow (0x800E9C98):
    if var == 0: self.box.oy = 26 - H[flag - 1] ; self.box.h = H[flag - 1]
    elif var == 1: self.box.oy = -30 ; self.box.h = H[flag - 1]
    if self.anime.flag == 28:
        self.attack_box = 0x800F6CBC ; self.hurt_box = 0x800F6CC0
        SetAnime(self, 4 if var == 0 else 18 if var == 1)
        self.timer(+0x7C) = 30 if game.difficulty < 2 else 15
        self.step2 += 1

emerge_wait (0x800EA5A0):
    if --self.timer == 0: self.step = 4 ; self.step2 = 0
```

### `worm_attack` — `0x800EA5CC` (step 4)

```
attack_start (0x800EA608): SetAnime(self, 8 if var == 0 else 21 if var == 1) ; self.timer = 30 ; self.step2 += 1
attack_fire  (0x800EA66C):
    if --self.timer == 0:
        PlaySound(2, 7, self)
        fx_spawn(self, 2 if var == 0 else 3 if var == 1)     // spark at the head
        self.timer = 20 ; self.step2 += 1
attack_wait  (0x800EA6FC): if --self.timer == 0: self.step = 5 ; self.step2 = 0
```

### `worm_sway` — `0x800EA728` (step 5)

```
sway_start (0x800EA764):
    SetAnime(self, 5 if var == 0 else 19 if var == 1)
    self.timer = 90 if game.difficulty < 2 else 45 ; self.step2 += 1
sway_wait (0x800EA7E4):
    if self.hit_f == 1: self.step = 6 ; self.step2 = 0
    if --self.timer == 0: self.step = 4 ; self.step2 = 0
```

### `worm_retract` — `0x800EA834` (step 6)

```
retract_start (0x800EA870):
    PlaySound(2, 6, self)
    self.terrain_box = self.attack_box = self.hurt_box = NULL
    SetAnime(self, 22 if var == 0 else 23 if var == 1) ; self.step2 += 1
retract_anim (0x800EA8E8): if self.anime.flag == 1: self.timer = 120 ; self.step2 += 1
retract_wait (0x800EA910): if --self.timer == 0: self.step = 3 ; self.step2 = 0
```

### `worm_jump` — `0x800EA93C` (step 7, var 2)

```
jump_start (0x800EA978):
    SetAnime(self, 7)
    self.y.px = self.base_y - 26
    fx_spawn_y(self, self.base_y, 17)                // water spray
    self.speed_x = 0 ; self.accel_x = 0
    self.speed_y = 0x88888 ; self.gravity = 0x48D1
    MoveObject2(self) ; self.step2 += 1
jump_air (0x800EAA04):
    MoveObject2(self)
    if self.anime.flag == 3:
        if --self.jumps != 0: self.step2 = 0
        else: self.step2 += 1
jump_lob (0x800EAA78):
    SetAnime(self, 7)
    self.y.px = self.base_y - 26
    fx_spawn_y(self, self.base_y, 17)
    self.speed_y = 0x80000 ; self.accel_x = 0 ; self.gravity = 0x8000
    self.speed_x = (mega.x.px - self.x.px) << 11
    MoveObject2(self) ; self.step2 += 1
jump_fall (0x800EAB14): MoveObject2(self)
```

### `fx_spawn(self, var)` — `0x800EAB3C` and `fx_spawn_y(self, y, var)` — `0x800EABF0`

```
e = GetMiscObject()
if e:
    e.flags = 0x41 ; e.type = 5 ; e.var = var
    e.flip = self.flip ; e.tex_cord = self.tex_cord ; e.clut_cord = self.clut_cord & 0x7FFF
    e.anime_table = self.anime_table ; e.sprite_data = self.sprite_data ; e.layer = self.layer
    e.x = self.x.px (fraction 0) ; e.y = self.y.px (fx_spawn) or y (fx_spawn_y) (fraction 0)
    e.parent(+0x50) = self
```

## Effects (misc type 5)

### `fx_init` — `0x800E9EF4` (state 0)

```
p = self.parent ; self.priority = 6 ; self.flip = p.flip
switch self.var:
  0:  self.pos = p.pos ; SetAnime(self, 1)                       // debris, floor
  1:  self.pos = p.pos ; SetAnime(self, 15)                      // debris, ceiling
  2:  self.pos = p.pos ; SetAnime(self, 9)
      self.x.px += 12 if self.flip else -12 ; self.y.px -= 30   // spark, floor
  3:  self.pos = p.pos ; SetAnime(self, 9)
      self.x.px += -12 if self.flip else 12 ; self.y.px += 30   // spark, ceiling
  16: SetAnime(self, 11)                                         // ripple
  17: self.priority = 5 ; SetAnime(self, 13)                     // spray
self.speed_x = self.accel_x = self.speed_y = self.gravity = 0
self.state = 1 ; self.step = 0
```

### `fx_main` — `0x800EA0B0` (state 1)

```
DisplayCheck(self, 16, 16)
p = self.parent
if p.state != 1:
    if self.var == 16: self.state = 3 ; self.timer(+0x54) = 60
    else: self.display = 0 ; self.state += 1           // delete
    return
AnimeTick(self)
switch self.var:
  0, 1: if p.anime.flag == 27: delete
  2, 3: if self.anime.flag == 1: delete
        elif self.anime.flag == 2: shot_spawn(self)
  16:   if p.anime.flag == 1: SetAnime(self, 12)       // splash at the start of a jump
  17:   if self.anime.flag == 1: delete
(delete = self.display = 0 ; self.state += 1)
```

### `shot_spawn` (inline in `fx_main`, `0x800EA17C`)

```
s = GetVisualObject()
if s:
    s.flags = 0x41 ; s.type = 3 ; s.var = 0 ; s.flip = self.flip
    s.owner(+0x7C) = self.parent
    s.x = self.x.px ; s.y = self.y.px (fractions 0)
    s.clut_cord = self.clut_cord & 0x7FFF ; s.anime_table = self.anime_table
    s.sprite_data = self.sprite_data ; s.tex_cord = self.tex_cord ; s.layer = self.layer
```

### `fx_delete` — `0x800EADEC` (state 2): `DeleteObject3(self)`

### `fx_blink` — `0x800EAE0C` (state 3)

```
AnimeTick(self)
if --self.timer == 0: DeleteObject3(self) ; return
self.display = self.timer & 1
```

## Shot (visual type 3)

### `shot_init` — `0x800E9DCC` (state 0)

```
self.contact_damage = 4 ; self.hp = 1 ; self.damage_table = 0x8007429C
self.priority = 5 ; self.latch = 1
SetAnime(self, 10)
self.terrain_box = 0x800F7154 ; self.attack_box = 0x800F7158 ; self.hurt_box = 0x800F715C
self.state = 1 ; self.step = self.step2 = self.byte7 = 0
a = ratan2(self.y.px - mega.y.px, mega.x.px - self.x.px)
self.accel_x = self.gravity = 0
self.speed_x = rcos(a) * 80 ; self.speed_y = rsin(a) * 80       // 5 px/frame
if OffScreenCheck2(self, 32, 32): self.state += 1
else: DisplayCheck(self, 16, 16)
```

### `shot_main` — `0x800EACE0` (state 1)

```
AnimeTick(self) ; MoveObject(self)
if ContactWeapon(self) < 0: SpawnExplosion(self) ; self.state += 1
else: self.clut_cord &= 0x7FFF
ContactMega(self)
if self.collide_flags(+0x70) or (CornerCheck(self), self.collide_flags):
    effect_0x80049D90(self) ; self.state += 1 ; return
if OffScreenCheck2(self, 32, 32): self.state += 1
else: DisplayCheck(self, 16, 16)
```

### `shot_delete` — `0x800EADCC` (state 2): `DeleteObject3(self)`

## Engine helpers called

All 21 helpers of `kit/summary.md` are named in `catalog/helpers.json`. Two more, outside the walk:

| address | proposed name | what |
|---|---|---|
| `0x80049D90` | `SpawnHitEffect` | gets a slot from `0x8002C398`; flags `0x21`, type 3, var 1, layer = `mega.layer`, animation table `0x8007789C`, clut `0x7802`, priority 0; flip `0x40` when `speed_x > 0`; at the object's position |
| `0x8002C398` | `GetEffectObject` | first free of 96 slots of `0x70` bytes at `0x80091298`, cleared |

## Object struct (fields specific to this enemy)

| off | name | meaning | confidence |
|---|---|---|---|
| `+0x7C` | `timer` (u16) | EMERGE, ATTACK, SWAY, RETRACT countdown | certain |
| `+0x80` | `box` (HITBOX) | own contact/hurt box; oy and h change in EMERGE | certain |
| `+0x84` | `base_y` (s16) | record y of the water jumper | certain |
| `+0x86` | `jumps` (u8) | small jumps left (starts at 3) | certain |
| `+0x87` | `hit_f` (u8) | 1 when `ContactWeapon` > 0 this update | certain |
| misc `+0x50` | `parent` | the worm | certain |
| misc `+0x54` | `timer` (u16) | ripple blink countdown | certain |
| visual `+0x7C` | `owner` | the worm (not read by the shot) | certain |
| visual `+0x70` | `collide_flags` | set by `CornerCheck`; ends the shot | certain |

Other fields used (from `kit/summary.md`): `var`, `display`, `state`, `step`, `step2`, `x`, `y`,
`layer`, `flip`, `priority`, `prev_x`, `prev_y`, `speed_x`, `speed_y`, `accel_x`, `gravity`,
`anime_table`, `sprite_data`, `tex_cord`, `clut_cord`, `anime.flag`, `attack_box`, `hurt_box`,
`damage_table`, `hp`, `contact_damage`, `iframes`, `latch`, `terrain_box`.
