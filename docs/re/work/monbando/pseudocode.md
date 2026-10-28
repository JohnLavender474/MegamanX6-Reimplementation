# Monbando and its shots: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`). Addresses are RAM addresses in overlay 09.
Names of engine helpers and struct fields are in `catalog/helpers.json` of mmx-demake-tools.

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800F5B44` state | 0 | `monb_init` `0x800EA41C` |
| | 1 | `monb_main` `0x800E9D64` |
| | 2 | `monb_delete` `0x800EA514` (`DeleteObject`) |
| | 3 | `monb_delete` `0x800EA514` (`DeleteObject2`) |
| `0x800F5B54` step | 0 | `HitReactionDispatch` `0x80040290` (engine) |
| | 1 | `monb_wait` `0x800EA554` |
| | 2 | `monb_fire` `0x800EA5FC` |
| | 3 | `monb_recover` `0x800EA708` |
| | 4 | `monb_fall` `0x800EA750` |
| `0x800F5B8C` shot state | 0 | `shot_init` `0x800E9EF0` |
| | 1 | `shot_move` `0x800EA178` |
| | 2 | `shot_delete` `0x800EA814` (`DeleteObject3`) |
| visual table entry 7 | | `shot_update` `0x800EA834` |

## Monbando

### `monbando_update` — `0x800EA3D0` (main table entry 9)

```
monbando_update(self):
    STATE_TABLE[self.state](self)
    CollideCheck(self)
```

### `monb_init` — `0x800EA41C` (state 0)

```
monb_init(self):
    self.hp = 32 ; self.contact_damage = 6 ; self.iframes = 0
    self.damage_table = 0x8007433C
    self.speed_x = self.speed_y = self.accel_x = self.gravity = 0
    self.latch = 0 ; self.collide_flags = 0 ; self.layer = mega.layer
    self.prev = position
    self.flip = 0 (left) if mega.x < self.x else 0x40 (right)
    self.anime_table = 0x800F5B14 ; self.priority = 4
    self.terrain_box = 0x800F5A48       // (0, 0, 24, 24)
    self.attack_box  = 0x800F5A4C       // (-16, -24, 32, 48)
    self.hurt_box    = 0x800F5A50       // (-24, -24, 48, 48)
    SetAnime(self, 0)
    self.timer(+0x7C) = 0 ; self.state = 1 ; self.step = 4 (FALL) ; self.step2 = 0
```

### `monb_main` — `0x800E9D64` (state 1)

```
monb_main(self):
    self.damage_table = 0x8007433C if self.anime.sprite == 0 else 0x800743DC
    self.prev = position
    STEP_TABLE[self.step](self)
    if self.step not in (2, 3):
        self.flip = 0 if mega.x < self.x else 0x40
    monb_fall_check(self)
    r = ContactWeapon(self)
    if r < 0: die(self) ; return                           // killed
    if r == 127: self.timer = 0                            // blocked hit restarts the wait
    if (self.collide_flags & 0x0C) == 0x0C: die(self) ; return   // squeezed
    ContactMega(self)
    if OffScreenCheck2(self, 64, 64): self.state = 3
    else: DisplayCheck(self, 32, 32)
```

`die` is inlined twice in the binary:

```
die(self):
    SpawnExplosion(self)
    SpawnJunk(4, {7, 8, 9, 10}, self)                      // list at 0x800F5B40
    DropItem(self, 21)
    self.state = 2
```

### `monb_delete` — `0x800EA514` (states 2 and 3)

```
    if self.state == 2: DeleteObject(self) else: DeleteObject2(self)
```

### `monb_wait` — `0x800EA554` (step 1)

```
monb_wait(self):
    AnimeTick(self)
    old = self.timer ; self.timer = old + 1
    if old < 61: return
    if GetRNG() & 1:
        if mega.var(0x800970A2) == 0: SetAnime(self, 3) ; self.step2 = 1     // missile (X)
        else:                         SetAnime(self, 2) ; self.step2 = 2     // punch (Zero)
    else:
        SetAnime(self, 4) ; self.step2 = 0                                   // ground slam
    self.step = 2
```

### `monb_fire` — `0x800EA5FC` (step 2)

```
monb_fire(self):
    AnimeTick(self)
    if self.anime.flag != 1: return
    n = 3 if self.step2 == 0 else 1
    delay = 0
    repeat n:
        s = GetVisualObject()
        if s:
            s.flags = 0x41 ; s.type = 7 ; s.var = self.step2
            s.tex_cord = self.tex_cord ; s.clut_cord = self.clut_cord & 0x7FFF
            s.anime_table = self.anime_table ; s.sprite_data = self.sprite_data
            s.flip = self.flip ; s.x = self.x ; s.y = self.y
            s.state = 0 ; s.step2(+0x06) = delay ; s.parent(+0x7C) = self
        delay += 6
    self.step = 3
```

### `monb_recover` — `0x800EA708` (step 3)

```
    AnimeTick(self)
    if self.anime.sprite == 0: self.step = 1 ; SetAnime(self, 1) ; self.timer = 0
```

### `monb_fall` — `0x800EA750` (step 4)

```
    if (self.collide_flags & 8) and self.anime.flag == 2:
        self.step = 1 ; self.speed_x = self.accel_x = self.speed_y = self.gravity = 0
        self.latch = 0 ; SetAnime(self, 1)
    else:
        MoveObject2(self)
    AnimeTick(self)
```

### `monb_fall_check` — `0x800EA7CC`

```
    if not (self.collide_flags & 8) and self.latch == 0:
        self.step = 4 ; self.gravity = 0x4200 ; self.speed_y = 0 ; self.accel_x = 0 ; self.latch = 1
```

## Shots (visual type 7)

### `shot_update` — `0x800EA834`

```
    self.prev = position
    SHOT_TABLE[self.state](self)
```

### `shot_init` — `0x800E9EF0` (state 0)

```
shot_init(self):
    f = (self.flip != 0)                        // 1 = right
    if self.var == 0:                           // ground wave
        Quake2(8, 2, 2) ; PlaySound(2, 29, self) ; Rumble(5) ; SetAnime(self, 5)
        self.speed_x = +0x28000 if f else -0x28000 ; self.speed_y = -0x28000
        self.accel_x = self.gravity = 0
        self.x.px += 36 if f else -36 ; self.y.px += 8
        self.terrain_box = 0x800F5B70           // (0, 0, 10, 14)
        self.attack_box  = 0x800F5B68           // (-8, -4, 16, 16)
        self.hurt_box = 0 ; self.hp = 0 ; self.contact_damage = 4
        self.damage_table = 0x800741FC ; self.priority = 4
    elif self.var == 1:                         // missile
        PlaySound(2, 28, self) ; SetAnime(self, 6)
        self.speed_x = +0x30000 if f else -0x30000
        self.accel_x = self.speed_y = self.gravity = 0
        self.x.px += 32 if f else -32 ; self.y.px -= 16
        self.hp = 6 ; self.contact_damage = 4
        self.terrain_box = 0x800F5B7C           // (0, 0, 16, 8)
        self.hurt_box    = 0x800F5B78           // (-16, -4, 32, 8)
        self.attack_box  = 0x800F5B74           // (-16, -4, 32, 8)
        self.damage_table = 0x800741FC if game.difficulty < 2 else 0x8007433C
        self.priority = 6 ; self.aim_t(+0x8C) = 50
    else:                                       // punch box
        PlaySound(2, 27, self) ; SetAnime(self, 6)
        self.speed_x = self.accel_x = self.speed_y = self.gravity = 0
        self.x.px += 48 if f else -48 ; self.y.px -= 16
        self.contact_damage = 8
        self.terrain_box = 0x800F5B88           // (0, 0, 16, 8)
        self.hurt_box    = 0x800F5B84           // (-16, -8, 32, 16)
        self.attack_box  = 0x800F5B80           // (-16, -8, 32, 16)
        self.damage_table = 0x8007433C ; self.hp = 0 ; self.priority = 6
    self.clut_cord &= 0x7FFF ; self.display = 0 ; self.collide_flags = 0 ; self.iframes = 0
    self.state += 1
```

### `shot_move` — `0x800EA178` (state 1)

```
shot_move(self):
    self.clut_cord &= 0x7FFF
    if self.step2 != 0: self.step2 -= 1 ; return            // start delay
    AnimeTick(self) ; MoveObject(self)
    r = ContactWeapon(self)
    if self.var == 0:                                         // wave
        old = self.collide_flags ; self.display = 1
        CollideCheck(self) ; ContactMega(self)
        if self.collide_flags == 0: CornerCheck(self)
        if old & 8:
            if self.collide_flags & 8: self.speed_y = 0 ; self.gravity = 0
            if not (self.collide_flags & 8): self.state = 2 ; return     // left the floor
        if self.collide_flags & 2 or self.collide_flags & 1: self.state = 2 ; return
        if OffScreenCheck2(self, 32, 32): self.state = 2 ; return
        DisplayCheck(self, 16, 16)
    elif self.var == 1:                                       // missile
        self.display = 1
        if self.aim_t != 0:
            old = self.aim_t ; self.aim_t -= 1
            if old < 40:
                if self.y.px + 8 < mega.y.px: self.y.px += 2
                elif self.y.px - 8 > mega.y.px: self.y.px -= 2
        if ContactMega(self) or r < 0 or self.collide_flags != 0:
            SpawnExplosion(self) ; self.state = 2 ; return
        CornerCheck(self)
        if self.collide_flags != 0: SpawnExplosion(self) ; self.state = 2 ; return
        if OffScreenCheck2(self, 32, 32): self.state = 2 ; return
        DisplayCheck(self, 16, 16)
    else:                                                     // punch box
        self.display = 0
        ContactMega(self)
        p = self.parent
        if p.anime.flag == 3 or p.state == 2 or p.state == 3: self.state = 2 ; return
        if OffScreenCheck2(self, 32, 32) == 1: self.state = 2
```

### `shot_delete` — `0x800EA814` (state 2)

```
    DeleteObject3(self)
```

## Engine helpers called

See `kit/summary.md` (21 helpers, all named in `catalog/helpers.json`). New:

| address | proposed name | what |
|---|---|---|
| `0x8002943C` | `Quake2` | same as `Quake` (`0x80029478`), but stores its 3 arguments at camera work `+0x36`, `+0x45`/`+0x3E`, `+0x3C`/`+0x3A` and sets bit `0x10` of `+0x34` (Quake: `+0x37`, `+0x46`/`+0x3F`, `+0x3D`/`+0x3B`, bit `0x01`) |

Facts on known helpers: `ContactWeapon` returns 127 for a hit on a class `0xFF` (deflect) entry
[likely]; `SpawnJunk`'s list here has 4 entries.

## Object struct (the fields used here)

Monbando (main object):

| off | name | meaning | confidence |
|---|---|---|---|
| `+0x04` | `state` | 0–3 | certain |
| `+0x05` | `step` | 0–4 | certain |
| `+0x06` | `step2` | attack kind 0 wave, 1 missile, 2 punch | certain |
| `+0x08`, `+0x0C` | `x`, `y` | position; read, copied to the shots | certain |
| `+0x14` | `layer` | from `mega.layer` | certain |
| `+0x15` | `flip` | faces the player | certain |
| `+0x16` | `priority` | 4 | certain |
| `+0x18`, `+0x1C` | `prev_x`, `prev_y` | position before the update | certain |
| `+0x20`–`+0x2C` | speeds, `accel_x`, `gravity` | 0, gravity `0x4200` in FALL | certain |
| `+0x30` | `anime_table` | `0x800F5B14` | certain |
| `+0x3C`, `+0x40`, `+0x42` | `sprite_data`, `tex_cord`, `clut_cord` | copied to the shots | certain |
| `+0x45` | `anime.flag` | 1 fire, 2 landing done, 3 punch back | certain |
| `+0x47` | `anime.sprite` | 0 = guard pose | certain |
| `+0x50`, `+0x54`, `+0x68` | boxes | see `monb_init` | certain |
| `+0x58` | `damage_table` | guard or normal | certain |
| `+0x5C` | `hp` | 32 | certain |
| `+0x60` | `contact_damage` | 6 | certain |
| `+0x61` | `iframes` | 0 | certain |
| `+0x67` | `latch` | 1 while falling | certain |
| `+0x70` | `collide_flags` | 8 floor, 4 ceiling | certain |
| `+0x7C` | `timer` (s16) | wait counter | certain |

Shot (visual object, same layout):

| off | name | meaning | confidence |
|---|---|---|---|
| `+0x02` | `var` | kind 0/1/2 | certain |
| `+0x03` | `display` | 0 punch box, 1 wave and missile | certain |
| `+0x06` | `step2` | start delay (0, 6, 12) | certain |
| `+0x7C` | `parent` (pointer) | Monbando | certain |
| `+0x8C` | `aim_t` (s16) | missile homing timer, 50 | certain |

Globals read: `mega.var` `0x800970A2` (character, likely), `mega.x` `0x800970A8`, `mega.y.px`
`0x800970AE`, `mega.layer` (`mega+0x14`), `game.difficulty` `0x800CCF38`.
