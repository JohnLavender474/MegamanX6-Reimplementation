# Batton Bone B81 and its bomb: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`). Addresses are RAM addresses in overlay 09.
Names of engine helpers and struct fields are in `catalog/helpers.json` of mmx-demake-tools. Names of
the object's own functions are mine.

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800F5FB0` state | 0 | `bat_init` `0x800EBFA4` |
| | 1 | `bat_main` `0x800EC108` |
| | 2, 3 | `bat_delete` `0x800EC2D8` |
| `0x800F5FC8` step | 0 | `HitReactionDispatch` `0x80040290` (engine) |
| | 1 | `bat_resume` `0x800EC32C` |
| | 2 | `bat_chase` `0x800EC338` |
| | 3 | `bat_drop` `0x800EC48C` |
| | 4 | `bat_flyaway` `0x800EC5C4` |
| | 5 | `bat_straight` `0x800EC688` |
| | 6 | `bat_scatter` `0x800EC75C` |
| | 7 | `bat_homing` `0x800EC930` |
| | 8 | `bat_rise` `0x800EC8F8` |
| `0x800F5FEC` step2 (CHASE) | 0, 1 | `chase_start` `0x800EC380`, `chase_move` `0x800EC3B8` |
| `0x800F5FF4` step2 (DROP) | 0, 1, 2 | `drop_start` `0x800EC4C8`, `drop_release` `0x800EC518`, `drop_end` `0x800EC574` |
| `0x800F6000` step2 (FLY AWAY) | 0, 1 | `flyaway_start` `0x800EC600`, `flyaway_move` `0x800EC65C` |
| `0x800F6008` step2 (STRAIGHT) | 0, 1 | `straight_start` `0x800EC6C4`, `straight_move` `0x800EC730` |
| `0x800F6010` step2 (SCATTER) | 0, 1, 2 | `scatter_start` `0x800EC798`, `scatter_wait` `0x800EC894`, `scatter_move` `0x800EC8C0` |
| `0x800F601C` step2 (HOMING) | 0, 1 | `homing_start` `0x800EC978`, `homing_move` `0x800EC9D0` |
| `0x800F602C` bomb state | 0, 1, 2, 3 | `bomb_init` `0x800ECB84`, `bomb_main` `0x800ECC10`, `bomb_delete` `0x800ECCF8`, `bomb_idle` `0x800ECD18` |
| `0x800F5FC0` | 0, 1 | hurt boxes `0x800F5F04` (body), `0x800F5F08` (bomb) |
| `0x800F5FAC` | | debris animations `{4, 5, 6, 7}` |
| main table entry 18 | | `bat_update` `0x800EC29C` |
| visual table entry 17 | | `bomb_update` `0x800ECB48` |
| misc table entry 56 | | `0x800ECBAC`: inside `bomb_init`, not usable (see notes) |

## Bat

### `bat_update` — `0x800EC29C` (main table entry 18)

```
bat_update(self): STATE_TABLE[self.state](self)
```

### `bat_init` — `0x800EBFA4` (state 0)

```
bat_init(self):
    self.flags = 0x41 ; self.hp = 3 ; self.contact_damage = 3 ; self.iframes = 0
    self.layer = mega.layer ; self.damage_table = 0x800743DC
    self.anime_table = 0x800F5F8C ; self.priority = 6
    self.hurt_box   = 0x800F5F04            // (-11, -10, 21, 23) body
    self.attack_box = 0x800F5F0C            // (-11, -10, 20, 42) body + bomb
    self.speed_x = self.speed_y = self.accel_x = self.gravity = 0
    self.latch = 0 ; self.terrain_box = 0
    self.+0x80 = self.+0x84 = self.+0x88 = self.+0x8C = self.+0x90 = self.+0x94 = 0
    self.prev = position ; self.state += 1
    switch self.var:
        0:  SetAnime(self, 0) ; self.step = 2                       // CHASE, holds the bomb
        1:  SetAnime(self, 3) ; self.step = 5 ; self.no_bomb = 1    // STRAIGHT
        4:  SetAnime(self, 3) ; self.step = 7 ; self.no_bomb = 1    // HOMING
        else: SetAnime(self, 3) ; self.step = 6 ; self.no_bomb = 1  // SCATTER
    self.step2 = 0
```

`no_bomb` is `+0x88`.

### `bat_main` — `0x800EC108` (state 1)

```
bat_main(self):
    self.prev = position
    STEP_TABLE[self.step](self)
    hit = ContactMega(self) & 0xFF
    if self.var == 4 and hit == 1: self.step = 8 ; self.step2 = 0
    if self.step != 0: self.resume_step(+0x94) = self.step
    for i in 0 .. (2 - self.no_bomb) - 1:
        self.hurt_box = HURT_BOXES[i]                  // 0x800F5FC0
        r = ContactWeapon(self)
        if r < 0:                                      // killed
            if i == 0 and self.no_bomb == 0: bomb_spawn(self)
            SpawnExplosion(self)
            SpawnJunk(4, {4, 5, 6, 7}, self)
            DropItem(self, 20)
            self.state += 1                            // 2
            return
        if r != 0: break                               // hit, not killed
    if OffScreenCheck2(self, 32, 32): self.state = 3
    else: DisplayCheck(self, 32, 32)
```

### `bat_delete` — `0x800EC2D8` (states 2 and 3)

```
    if halfword(0x800CCEDC) == 2 or self.state == 3: DeleteObject2(self)   // game.stage 2, area 0
    else: DeleteObject(self)
```

### `bat_resume` — `0x800EC32C` (step 1)

```
    self.step = self.resume_step
```

### `bat_face` — `0x800ECA70`

```
    self.flip = 0 if mega.x < self.x else 0x40
```

### `bat_chase` — `0x800EC338` (step 2)

```
bat_chase(self): bat_face(self) ; CHASE_TABLE[self.step2](self)

chase_start (0x800EC380):  AnimeTick(self) ; self.timer(+0x7C) = 200 ; self.step2 += 1

chase_move (0x800EC3B8):
    AnimeTick(self)
    MoveObject(self)
    SetSpeedDir(self, AimAtPoint(self, mega.x, mega.y - 80 px))     // 1 px/frame
    self.speed_x += self.speed_x / 2                                  // C division, towards 0
    if |mega.x - self.x| <= |self.speed_x| + 2 px: goto drop
    self.timer -= 1
    if self.timer != 0: return
  drop:
    self.step = 3 ; self.step2 = 0
```

### `bat_drop` — `0x800EC48C` (step 3)

```
drop_start (0x800EC4C8):
    SetAnime(self, 1) ; self.drop_timer(+0x80) = 5 ; self.speed_x = 0
    self.hurt_box = self.attack_box = 0x800F5F04 ; self.step2 += 1

drop_release (0x800EC518):
    AnimeTick(self)
    self.drop_timer -= 1
    if self.drop_timer == 0:
        self.no_bomb = 1 ; bomb_spawn(self) ; self.drop_timer = 13 ; self.step2 += 1

drop_end (0x800EC574):
    AnimeTick(self)
    self.drop_timer -= 1
    if self.drop_timer == 0: SetAnime(self, 3) ; self.step = 4 ; self.step2 = 0
```

### `bat_flyaway` — `0x800EC5C4` (step 4)

```
flyaway_start (0x800EC600):
    SetAnime(self, 3)
    self.speed_x = FLY_SPEED[(self.flip >> 4) & 4]     // 0x800F5F14: -0x30000, +0x30000
    self.speed_y = 0x5000 ; self.step2 += 1

flyaway_move (0x800EC65C): AnimeTick(self) ; MoveObject(self)
```

### `bat_straight` — `0x800EC688` (step 5)

```
straight_start (0x800EC6C4):
    SetAnime(self, 3) ; self.speed_x = FLY_SPEED[(self.flip >> 4) & 4]
    self.attack_box = 0x800F5F04 ; self.speed_y = 0 ; self.no_bomb = 1 ; self.step2 += 1

straight_move (0x800EC730): AnimeTick(self) ; MoveObject(self)
```

### `bat_scatter` — `0x800EC75C` (step 6)

```
scatter_start (0x800EC798):
    SetAnime(self, 3)
    r = ((GetRNG() << 8) + GetRNG()) & 0xFFFF
    self.speed_x = SCATTER_SPEED[(self.flip >> 4) & 4]  // 0x800F5F1C: -0x38000, +0x38000
    if GetRNG() & 1: self.speed_x += r else: self.speed_x -= r
    self.accel_x = -0x1000                              // not used by MoveObject
    self.speed_y = 0x18000
    self.attack_box = 0x800F5F04 ; self.no_bomb = 1
    if self.var == 2: self.timer = GetRNG() & 0x3F ; self.step2 += 1
    else:             self.step2 += 2

scatter_wait (0x800EC894):
    if self.timer == 0: self.step2 += 1 else: self.timer -= 1

scatter_move (0x800EC8C0):
    AnimeTick(self) ; MoveObject(self) ; self.speed_y += 0x100
```

### `bat_homing` — `0x800EC930` (step 7)

```
bat_homing(self): bat_face(self) ; HOMING_TABLE[self.step2](self)

homing_start (0x800EC978):
    self.attack_box = 0x800F5F04 ; SetAnime(self, 3) ; AnimeTick(self)
    self.no_bomb = 1 ; self.timer = 300 ; self.step2 += 1

homing_move (0x800EC9D0):
    AnimeTick(self)
    self.timer -= 1
    if self.timer == 0: self.step = 8 ; self.step2 = 0
    self.aim_count(+0x90) = (self.aim_count + 1) & 3
    if self.aim_count == 0: SetSpeedDir(self, AimAtPoint(self, mega.x, mega.y))
    MoveObject(self)
```

### `bat_rise` — `0x800EC8F8` (step 8)

```
    self.speed_x = 0 ; self.speed_y = 0x20000 ; MoveObject(self) ; AnimeTick(self)
```

### `bomb_spawn` — `0x800ECAA0`

```
    b = GetVisualObject()
    if b:
        b.flags = 0x41 ; b.type = 17 ; b.var = 0
        b.tex_cord = self.tex_cord ; b.clut_cord = self.clut_cord
        b.anime_table = self.anime_table ; b.sprite_data = self.sprite_data
        b.layer = self.layer ; b.flip = self.flip ; b.x = self.x ; b.y = self.y
        b.parent(+0x7C) = self ; b.priority = 6
```

## Bomb (visual type 17)

### `bomb_update` — `0x800ECB48` (visual table entry 17)

```
bomb_update(self): BOMB_TABLE[self.state](self)
```

### `bomb_init` — `0x800ECB84` (state 0)

```
    self.speed_y = -0x10000 ; self.gravity = 0x3000
    self.terrain_box = 0x800F6024                     // (-2, 22, 7, 14)
    self.damage_table = 0x800741FC
    self.hurt_box = self.attack_box = 0x800F6028      // (-8, 10, 13, 26)
    self.contact_damage = 5 ; self.display = 1 ; self.hp = 1
    self.speed_x = 0 ; self.accel_x = 0 ; self.state += 1
    self.clut_cord &= 0x7FFF
    SetAnime(self, 2)
```

### `bomb_main` — `0x800ECC10` (state 1)

```
    AnimeTick(self)
    MoveObject2(self)
    if ContactWeapon(self) < 0: SpawnExplosion(self) ; self.state += 1 ; return
    if ContactMega(self) != 0:  SpawnExplosion(self) ; self.state = 2 ; return
    if self.collide_flags != 0 or (CornerCheck(self), self.collide_flags != 0):
        self.y.px += 32 ; SpawnExplosion(self) ; self.state += 1 ; return
    if OffScreenCheck2(self, 32, 32): self.state += 1 ; return
    DisplayCheck(self, 16, 16)
```

### `bomb_delete` — `0x800ECCF8` (state 2), `bomb_idle` — `0x800ECD18` (state 3)

```
bomb_delete: DeleteObject3(self)
bomb_idle:   return
```

## Engine helpers called

| address | name | use here |
|---|---|---|
| `0x800179A4` | `SetAnime` | start an animation |
| `0x80017A04` | `AnimeTick` | advance the animation |
| `0x8002C458` | `GetVisualObject` | the bomb slot |
| `0x8002C9B0` | `DeleteObject3` | bomb delete |
| `0x8002CA14` | `DeleteObject` | bat delete after death |
| `0x8002CA54` | `DeleteObject2` | bat delete off screen (and after death on stage 2 / area 0) |
| `0x8002CBFC` | `OffScreenCheck2` | 32 × 32 px margin (bat and bomb) |
| `0x8002CCB0` | `DisplayCheck` | 32 px (bat), 16 px (bomb) |
| `0x8002D230` | `MoveObject2` | bomb fall with gravity and `fall_max` |
| `0x8002D2B0` | `MoveObject` | bat movement (no acceleration, no gravity) |
| `0x8002D2D4` | `GetRNG` | random byte 0–255 |
| `0x8002D354` | (new) `AimAtPoint` | `AimAtPoint(obj, x, y)` = `Dir32(obj.x − x, obj.y − y)`: direction 0..31 from obj to the point |
| `0x8002D3B4` | `SetSpeedDir` | 1 px/frame in a direction |
| `0x8002EE10` | `CornerCheck` | bomb against the tile map |
| `0x80030ECC` | `ContactMega` | contact damage; result 1 = hit X [likely] |
| `0x80031474` | `ContactWeapon` | weapon hits; < 0 killed |
| `0x80040290` | `HitReactionDispatch` | step 0 |
| `0x800477E4` | `SpawnJunk` | 4 debris pieces |
| `0x80049E50` | `SpawnExplosion` | bat and bomb explosions |
| `0x8004E654` | `DropItem` | table 20 |

## Object struct (the fields used here)

| off | name | meaning | confidence |
|---|---|---|---|
| `+0x00` | `flags` | `0x41` | certain |
| `+0x02` | `var` | record id: 0 bomb, 1 straight, 2 scatter with wait, 4 homing, others scatter | certain |
| `+0x04` | `state` | 0–3 | certain |
| `+0x05` | `step` | 0–8 | certain |
| `+0x06` | `step2` | sub-step | certain |
| `+0x08`, `+0x0C` | `x`, `y` | position | certain |
| `+0x14` | `layer` | from `mega.layer` | certain |
| `+0x15` | `flip` | 0 left, `0x40` right; index `(flip >> 4) & 4` into the speed tables | certain |
| `+0x16` | `priority` | 6 | certain |
| `+0x18`, `+0x1C` | `prev_x`, `prev_y` | saved each update | certain |
| `+0x20`, `+0x24` | `speed_x`, `speed_y` | | certain |
| `+0x28` | `accel_x` | written in SCATTER, not used | certain |
| `+0x2C` | `gravity` | 0 (bat), `0x3000` (bomb) | certain |
| `+0x30` | `anime_table` | `0x800F5F8C` | certain |
| `+0x3C`, `+0x40`, `+0x42` | `sprite_data`, `tex_cord`, `clut_cord` | copied to the bomb | certain |
| `+0x50`, `+0x54` | `attack_box`, `hurt_box` | | certain |
| `+0x58` | `damage_table` | `0x800743DC` / bomb `0x800741FC` | certain |
| `+0x5C` | `hp` | 3 / bomb 1 | certain |
| `+0x60` | `contact_damage` | 3 / bomb 5 | certain |
| `+0x61` | `iframes` | 0 | certain |
| `+0x67` | `latch` | 0 (cleared only) | certain |
| `+0x68` | `terrain_box` | 0 / bomb `0x800F6024` | certain |
| bomb `+0x70` | `collide_flags` | non-zero: hit terrain | likely |
| `+0x7C` | `timer` (s16) | CHASE 200, SCATTER wait 0–63, HOMING 300; bomb: parent pointer (32 bit) | certain |
| `+0x80` | `drop_timer` (s32) | DROP 5, then 13 | certain |
| `+0x84`, `+0x8C` | — | cleared only | certain |
| `+0x88` | `no_bomb` (s32) | 0 holds the bomb, 1 none; number of hurt boxes = 2 − `no_bomb` | certain |
| `+0x90` | `aim_count` (s32) | 0..3, HOMING aims when it is 0 | certain |
| `+0x94` | `resume_step` (s32) | last non-zero step; RESUME restores it | certain |
