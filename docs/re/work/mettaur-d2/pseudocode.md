# Mettaur D2 and its shot: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`). Addresses are RAM addresses in overlay 10
(stage 7). Names of engine helpers and struct fields are in `catalog/helpers.json` of
mmx-demake-tools. Names of the object's own functions are mine.

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800FC64C` state | 0 | `met_init` `0x800F1D4C` |
| | 1 | `met_main` `0x800F1E8C` |
| | 2, 4 | `met_delete` `0x800F23E4` |
| | 3 | `met_hidden` `0x800F2014` |
| `0x800FC634` step | 0 | `HitReactionDispatch` `0x80040290` (engine) |
| | 1 | `met_walk` `0x800F22FC` |
| | 2 | `met_fall` `0x800F2150` |
| | 3 | `met_guard` `0x800F1C10` |
| | 4 | `met_shoot` `0x800F21F8` |
| | 5 | `met_jump` `0x800F20B4` |
| `0x800FC668` shot state | 0, 1, 2 | `shot_init` `0x800F2460`, `shot_move` `0x800F2518`, `shot_delete` `0x800F25E0` |
| `0x800FC630` | | debris animations `{9, 10, 11, 12}` |
| main table entry 27 | | `met_update` `0x800F1FD8` |
| visual table entry 32 | | `shot_update` `0x800F2424` |

Boxes: terrain `0x800FC4F4` (0, 5, 11, 8); attack `0x800FC4F8` (−11, −16, 21, 29); guard box
`0x800FC4FC` (−12, −4, 21, 18); hurt `0x800FC500` (−12, −6, 21, 20); shot box `0x800FC660`
(−5, −4, 9, 7); shot terrain `0x800FC664` (0, 0, 9, 7).

Work bytes: `+0x7C` guard timer (16 bit); `+0x80` busy bits (1 = guard, 2 = shoot); `+0x81` shot
counter; `+0x82` guard cooldown; `+0x83` shot cooldown.

## Mettaur

### `met_update` — `0x800F1FD8` (main table entry 27)

```
met_update(self): STATE_TABLE[self.state](self)
```

### `met_init` — `0x800F1D4C` (state 0)

```
met_init(self):
    self.state += 1 ; self.step = 1
    self.prev = position ; self.layer = mega.layer
    self.anime_table = 0x800FC5F8 ; self.priority = 6
    self.terrain_box = 0x800FC4F4 ; self.attack_box = 0x800FC4F8 ; self.hurt_box = 0x800FC500
    self.damage_table = dmg_tbl_normal
    self.iframes = 0 ; self.latch = 0 ; self.collide_flags = 0
    self.hp = 3 ; self.contact_damage = 1
    self.speed_x = self.accel_x = self.speed_y = self.gravity = 0
    self.+0x80 = self.+0x81 = self.+0x82 = self.+0x83 = 0
    self.flip = (mega.x >= self.x) ? 0x40 : 0          // 32-bit compare: equal x faces right
    if self.var == 2 or self.var == 3:
        self.state = 3                                  // wait in a solid tile
        self.hurt_box = self.attack_box = 0
        self.x.px = (self.x.px & ~15) + 8
        self.y.px = (self.y.px & ~15) + 20
        SetAnime(self, 0)
    else:
        SetAnime(self, 1)
```

### `met_main` — `0x800F1E8C` (state 1)

```
met_main(self):
    self.prev = position
    met_edge_check(self)
    met_shot_trigger(self)
    met_guard_trigger(self)
    STEP_TABLE[self.step](self)
    ContactMega(self)
    r = ContactWeapon(self)
    if r < 0:                                           // killed
        SpawnExplosion(self)
        SpawnJunk(4, {9, 10, 11, 12}, self)
        if r != -56: DropItem(self, 18)                 // no drop for weapon id 55
        self.state += 1                                 // 2: DeleteObject
        return
    if OffScreenCheck2(self, 64, 64): self.state = 4 ; return
    DisplayCheck(self, 37, 37)
    if (self.collide_flags & 0x0C) == 0x0C or (self.collide_flags & 0x03) == 0x03:
        SpawnExplosion(self)
        SpawnJunk(4, {9, 10, 11, 12}, self)
        self.display = 0
        self.state += 1                                 // squeezed: no drop
        return
    CollideCheck(self)
```

### `met_edge_check` — `0x800F1884`

```
met_edge_check(self):
    if self.latch != 0: return
    if not (self.collide_flags & 0x08):                 // no floor: fall
        SetAnime(self, 3)
        self.gravity = 0x4200 ; self.step = 2 ; self.step2 = 0
        self.speed_y = 0 ; self.accel_x = 0 ; self.latch = -1
        return
    b = self.terrain_box                                 // (0, 5, 11, 8)
    px = self.flip ? self.x.px + b.ox + b.w : self.x.px - b.ox - b.w   // x +- 11
    py = self.y.px + b.oy + b.h + 8                                     // y + 21
    if TerrainPointCheck(self, px, py): return           // floor goes on
    if self.var == 1:                                    // turn at the ledge
        self.flip = (self.flip == 0) ? 0x40 : 0
        self.speed_x = -self.speed_x
    elif (self.collide_flags & 0x03) == 0:               // jump off the ledge
        SetAnime(self, 2)
        self.step = 5 ; self.step2 = 0
        self.speed_y = 0x38000 ; self.gravity = 0x4200
        self.speed_x = self.flip ? 0x10000 : -0x10000
        self.latch = 1
```

### `met_shot_trigger` — `0x800F1748`

```
met_shot_trigger(self):
    if self.latch != 0 or (self.+0x80 & 3): return
    if self.+0x83 != 0: self.+0x83 -= 1 ; return        // shot cooldown
    if abs(self.y.px - mega.y.px) >= 40: self.+0x81 = 0 ; return
    if self.flip: near = mega.x.px < self.x.px + 80      // no lower bound: behind counts
    else:         near = mega.x.px > self.x.px - 80
    self.+0x81 = near ? self.+0x81 + 1 : 0
    if self.+0x81 >= 61:
        self.step = 4 ; self.step2 = 0 ; SetAnime(self, 7)
        self.+0x81 = 0 ; self.+0x80 |= 2
```

### `met_guard_trigger` — `0x800F1A24`

```
met_guard_trigger(self):
    if self.latch != 0 or (self.+0x80 & 1): return
    if self.+0x82 != 0: self.+0x82 -= 1 ; return        // guard cooldown
    for w in weapon_objects[0 .. 31]:
        if w.flags == 0 or w.type != 0: continue        // type 0: the X-Buster lemon
        if self.flip:                                    // faces right
            ok = mega.flip == 0 and self.x < mega.x
                 and abs(w.y.px - self.y.px) < 24 and w.x.px < self.x.px + 128
        else:
            ok = mega.flip != 0 and mega.x < self.x
                 and abs(w.y.px - self.y.px) < 24 and w.x.px > self.x.px - 128
        if ok: self.+0x80 |= 1
        if self.+0x80 & 1:
            self.step = 3 ; self.step2 = 0 ; SetAnime(self, 5)
            return
```

### `met_guard` — `0x800F1C10` (step 3)

```
met_guard(self):
    switch self.step2:
    0:  self.step2 = 1 ; SetAnime(self, 5) ; self.+0x7C = 60
        -> fall into 1
    1:  if self.anime.flag:                              // helmet down (frame 3)
            self.damage_table = dmg_tbl_deflect
            self.hurt_box = self.attack_box = 0x800FC4FC
        self.+0x7C -= 1
        if self.+0x7C == 0: self.step2 = 2 ; SetAnime(self, 6)
    2:  if self.anime.next == 0:                         // raise animation done
            SetAnime(self, 1)
            self.damage_table = dmg_tbl_normal
            self.hurt_box = 0x800FC500 ; self.attack_box = 0x800FC4F8
            self.step = 1 ; self.step2 = 0
            self.+0x82 = 120 ; self.+0x80 &= ~1
    AnimeTick(self)
```

### `met_hidden` — `0x800F2014` (state 3, `var` 2 and 3)

```
met_hidden(self):
    if not TerrainPointCheck(self, self.x.px, self.y.px):
        self.state = 1 ; self.step = 1 ; self.var = 0
        self.hurt_box = 0x800FC500 ; self.attack_box = 0x800FC4F8
    if OffScreenCheck2(self, 64, 64): self.state = 2 ; return
    if self.var == 2: DisplayCheck(self, 37, 37)
```

### `met_jump` — `0x800F20B4` (step 5)

```
met_jump(self):
    AnimeTick(self)
    if self.step2 == 0:
        if self.anime.flag: self.step2 = 1               // frame 2 of animation 2
        return
    MoveObject2(self)
    if self.speed_y < 0:                                 // top of the jump
        self.step2 += 1 ; SetAnime(self, 3)
        self.gravity = 0x4200 ; self.step = 2 ; self.step2 = 0
        self.speed_y = 0 ; self.accel_x = 0 ; self.latch = -1
```

### `met_fall` — `0x800F2150` (step 2)

```
met_fall(self):
    AnimeTick(self)
    if self.step2 == 0:
        self.damage_table = dmg_tbl_normal
        MoveObject2(self)
        if self.collide_flags & 0x08:                    // landed
            SetAnime(self, 4)
            self.speed_y = self.gravity = self.speed_x = self.accel_x = 0
            self.step2 += 1
    elif self.anime.next == 0:
        SetAnime(self, 1) ; self.step = 1 ; self.step2 = 0 ; self.latch = 0
```

### `met_shoot` — `0x800F21F8` (step 4)

```
met_shoot(self):
    AnimeTick(self)
    if self.anime.flag:                                  // frame 3 of animation 7
        self.anime.flag = 0
        s = GetVisualObject()
        if s:
            PlaySound(2, 46, self)
            s.flags = 0x41 ; s.type = 32
            s.x = self.x ; s.y = self.y
            s.sprite_data = self.sprite_data ; s.tex_cord = self.tex_cord ; s.clut_cord = self.clut_cord
            s.layer = self.layer ; s.anime_table = self.anime_table ; s.flip = self.flip
    if self.anime.next == 0:
        self.step = 1 ; self.step2 = 0 ; SetAnime(self, 1)
        self.+0x83 = 120 ; self.+0x80 &= ~2
```

### `met_walk` — `0x800F22FC` (step 1)

```
met_walk(self):
    if self.step2 == 0:
        self.damage_table = dmg_tbl_normal
        self.hurt_box = 0x800FC500 ; self.attack_box = 0x800FC4F8
        self.step2 += 1 ; SetAnime(self, 1)
        self.speed_x = self.flip ? 0x6000 : -0x6000
        self.accel_x = self.speed_y = self.gravity = 0
    AnimeTick(self)
    MoveObject(self)
    if self.flip and (self.collide_flags & 0x01):      self.flip = 0    ; self.speed_x = -self.speed_x
    elif not self.flip and (self.collide_flags & 0x02): self.flip = 0x40 ; self.speed_x = -self.speed_x
```

### `met_delete` — `0x800F23E4` (states 2 and 4)

```
met_delete(self): if self.state == 4: DeleteObject2(self) else: DeleteObject(self)
```

## Shot (visual type 32)

### `shot_update` — `0x800F2424`

```
shot_update(s): SHOT_STATE_TABLE[s.state](s)
```

### `shot_init` — `0x800F2460` (state 0)

```
shot_init(s):
    s.display = 1 ; s.state += 1
    s.speed_x = s.flip ? 0x28000 : -0x28000
    s.accel_x = s.speed_y = s.gravity = 0
    s.x.px += s.flip ? 16 : -16
    s.priority = 6
    s.terrain_box = 0x800FC664
    s.damage_table = dmg_tbl_one_hit
    s.hurt_box = s.attack_box = 0x800FC660
    s.contact_damage = 2
    s.y.px += 3
    SetAnime(s, 8)
```

### `shot_move` — `0x800F2518` (state 1)

```
shot_move(s):
    AnimeTick(s) ; MoveObject(s)
    if ContactMega(s): s.state = 2 ; return
    if ContactWeapon(s) < 0: s.state = 2 ; return       // shot down: no explosion
    if s.+0x70 == 0: CornerCheck(s)
    if s.+0x70 != 0: SpawnHitEffect(s) ; s.state += 1 ; return
    if OffScreenCheck2(s, 32, 32): s.state += 1 ; return
    DisplayCheck(s, 16, 16)
```

### `shot_delete` — `0x800F25E0` (state 2)

```
shot_delete(s): DeleteObject3(s)
```

## Engine helpers called

| address | name | use here |
|---|---|---|
| `0x80016C48` | `PlaySound` | bank 2, id 46 at each shot |
| `0x800179A4` | `SetAnime` | animations 0–8 |
| `0x80017A04` | `AnimeTick` | in the steps (not in `met_main`) |
| `0x8002C458` | `GetVisualObject` | the shot |
| `0x8002C9B0` / `0x8002CA14` / `0x8002CA54` | `DeleteObject3` / `DeleteObject` / `DeleteObject2` | shot end / death / off screen |
| `0x8002CBFC` | `OffScreenCheck2` | 64 (body), 32 (shot) |
| `0x8002CCB0` | `DisplayCheck` | 37 (body), 16 (shot) |
| `0x8002D230` | `MoveObject2` | fall and jump |
| `0x8002D2B0` | `MoveObject` | walk, shot |
| `0x8002EE10` | `CornerCheck` | shot on walls |
| `0x8002F288` | `CollideCheck` | after each main update |
| `0x80030CCC` | `TerrainPointCheck` | ledge probe; the wait of `var` 2 and 3 |
| `0x80030ECC` | `ContactMega` | body and shot |
| `0x80031474` | `ContactWeapon` | body and shot |
| `0x80040290` | `HitReactionDispatch` | step 0 |
| `0x800477E4` | `SpawnJunk` | 4 pieces |
| `0x80049D90` | `SpawnHitEffect` | shot on a wall |
| `0x80049E50` | `SpawnExplosion` | death, squeeze |
| `0x8004E654` | `DropItem` | table 18 |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x02` | `var` | 1 = turn at ledges; 2, 3 = wait in a solid tile (2 visible); other = jump off ledges |
| `+0x03` | `display` | |
| `+0x04`, `+0x05`, `+0x06` | `state`, `step`, `step2` | |
| `+0x08`, `+0x0C` | `x`, `y` | 16.16 |
| `+0x14`, `+0x15`, `+0x16` | `layer`, `flip`, `priority` | |
| `+0x18`, `+0x1C` | `prev_x`, `prev_y` | |
| `+0x20`…`+0x2C` | `speed_x`, `speed_y`, `accel_x`, `gravity` | |
| `+0x30`, `+0x3C`, `+0x40`, `+0x42` | animation table, sprite data, texture, palette | copied to the shot |
| `+0x45`, `+0x46` | `anime.flag`, `anime.next` | |
| `+0x50`, `+0x54`, `+0x58` | attack box, hurt box, damage table | |
| `+0x5C`, `+0x60`, `+0x61` | `hp`, `contact_damage`, `iframes` | |
| `+0x67` | `latch` | 0 ground, 1 jump, −1 fall |
| `+0x68`, `+0x70` | terrain box, `collide_flags` | |
| `+0x7C` | guard timer | |
| `+0x80` | busy bits | 1 guard, 2 shoot |
| `+0x81` | shot counter | |
| `+0x82` | guard cooldown | |
| `+0x83` | shot cooldown | |
