# Nightmare Insect and its shots: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`), and two engine functions read with `--at`
(`0x80031474` `ContactWeapon`, `0x800315CC` `ApplyWeaponHit`, `0x80030CCC`). Addresses are RAM
addresses in overlay 03 (Commander Yammark). Names of engine helpers and struct fields are in
`catalog/helpers.json` of mmx-demake-tools. The function names here are mine.

Speeds: `0x10000` = 1 px/frame. `speed_y` positive = up. `rcos`/`rsin` return 4096 = 1.0, so
`rcos(a) * 80` = 5 px/frame at full length, `* 48` = 3 px/frame.

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800F7698` main state | 0 | `ni_init` `0x800ED760` |
| | 1 | `ni_main` `0x800EC8F0` |
| | 2 | `ni_delete` `0x800ED830` (`DeleteObject`) |
| | 3 | `ni_delete` `0x800ED830` (`DeleteObject2`; no code sets state 3) |
| `0x800F76A8` main step | 0 | `HitReactionDispatch` `0x80040290` (engine) |
| | 1 | `ni_idle` `0x800ECA2C` |
| | 2 | `ni_blade` `0x800ED870` (BLADE attack) |
| | 3 | `ni_boomerang` `0x800ED8AC` (BOOMERANG attack) |
| | 4 | `ni_lay` `0x800ED8E8` (LAY attack) |
| | 5 | `ni_revive` `0x800ED924` (husk, then revive) |
| `0x800F76C0` BLADE step2 | 0, 1, 2 | `blade_start` `0x800ED984`, `blade_throw` `0x800ED9B8`, `blade_end` `0x800EDA84` |
| `0x800F76CC` BOOMERANG step2 | 0..3 | `boom_start` `0x800EDACC`, `boom_throw` `0x800EDB00`, `boom_wait` `0x800EDBD4`, `boom_end` `0x800EDC24` |
| `0x800F76DC` LAY step2 | 0..3 | `lay_start` `0x800EDC6C`, `lay_spawn` `0x800ECC38`, `lay_end` `0x800EDCA4`, `lay_hold` `0x800EDCEC` (not reached) |
| visual table entry 27 | | `kid_update` `0x800EDD2C` |
| `0x800F7704` visual state | 0, 1, 2 | `kid_init` `0x800ECDD4`, `kid_main` `0x800ED584`, `kid_delete` `0x800EDD0C` |
| `0x800F7738` visual step | 0, 1, 2 | `blade_run` `0x800EDF80`, `boom_run` `0x800EDFBC`, `hatch_run` `0x800EE118` |
| `0x800F7710` blade step2 | 0..4 | `blade_fly` `0x800ED020`, `blade_stop` `0x800EDD88`, `blade_chase` `0x800ED0F4`, `blade_stop2` `0x800EDE78`, `blade_exit` `0x800EDF30` |
| `0x800F7724` boomerang step2 | 0, 1 | `boom_out` `0x800ED220`, `boom_back` `0x800ED2F4` |
| `0x800F772C` hatchling step2 | 0, 1, 2 | `hatch_fall` `0x800EDFF8`, `hatch_walk` `0x800ED428`, `hatch_burst` `0x800EE080` |

The kit also lists "misc type 56 at `0x800ECBAC`". That address is a branch target inside `ni_idle`
(the `sb v0,5(s0)` of the X branch). The object makes no misc object. It is not a neighbour.

## Main object (type 23)

### `ni_update` — `0x800ED704` (main table entry 23)

```
ni_update(self):
    self.prev_x = self.x ; self.prev_y = self.y
    STATE_TABLE[self.state](self)
    CollideCheck(self)                 // terrain_box is 0, so no terrain contact
```

### `ni_init` — `0x800ED760` (state 0)

```
ni_init(self):
    self.hp = 32 ; self.contact_damage = 8 ; self.iframes = 0
    self.damage_table = 0x8007479C
    self.speed_x = self.speed_y = 0 ; self.collide_flags = 0
    self.layer = mega.layer
    self.flip = 0 (left) if mega.x < self.x else 0x40 (right)
    self.anime_table = 0x800F7650 ; self.priority = 5
    self.terrain_box = 0
    self.hurt_box   = 0x800F744C          // (-34, -16, 60, 32)
    self.attack_box = 0x800F7448          // (-28, -16, 48, 32)
    SetAnime(self, 0)                     // hover
    self.timer(+0x7C) = 0
    self.state = 1 ; self.step = 1 ; self.returned(+0x07) = 0
```

### `ni_main` — `0x800EC8F0` (state 1)

```
ni_main(self):
    STEP_TABLE[self.step](self)
    r = ContactWeapon(self)                     // < 0: killed, value = -(weapon id + 1)
    if r in (-2, -11, -55):                     // killed by weapon id 1, 10 or 54
        SpawnExplosion(self)
        SpawnJunk(4, {8, 9, 10, 11}, self)      // 0x800F7690
        DropItem(self, 21)
        self.state = 2
        return
    if r < 0:                                   // killed by any other weapon
        SpawnExplosion(self)
        SpawnJunk(4, {8, 9, 10, 11}, self)
        if game.difficulty < 2: SetAnime(self, 5)     // husk, 206 frames
        else:                   SetAnime(self, 15)    // husk, 103 frames
        self.hp = 32
        self.hurt_box = 0 ; self.attack_box = 0
        self.step = 5
        return
    ContactMega(self)
    if not OffScreenCheck2(self, 64, 64):
        DisplayCheck(self, 48, 48)
```

No code sets state 3: the object never deletes itself off screen.

### `ni_idle` — `0x800ECA2C` (step 1)

```
ni_idle(self):
    self.flip = 0 if mega.x < self.x else 0x40         // face the player
    AnimeTick(self)
    old = self.timer ; self.timer += 1
    if old < 61: return                                 // 62 frames between attacks
    if self.display:
        dx = abs(mega.x.px - self.x.px)
        if mega.var != 0:                               // Zero
            self.step = 4 (LAY) if dx < 64 else 3 (BOOMERANG)
            if count_hatchlings() >= 4: self.step = 3
        else:                                           // X
            self.step = 2 (BLADE) if dx < 64 else 4 (LAY)
            if count_hatchlings() >= 4: self.step = 2
    self.step2 = 0
```

`count_hatchlings()` is inlined three times: it counts the visual slots (32 × `0xA0` from
`0x80093C98`) whose word 0 has `flags != 0` and `(word & 0x00FFFF00) == 0x00021B00`, that is
type 27 and var 2.

### `ni_blade` — `0x800ED870` (step 2) and its sub-steps

```
blade_start (0x800ED984): SetAnime(self, 1) ; self.step2 += 1
blade_throw (0x800ED9B8):
    AnimeTick(self)
    if self.anime.flag == 1:                        // frame 12, 48 frames after the start
        spawn_kid(self, var = 0)
        self.step2 += 1
blade_end   (0x800EDA84):
    AnimeTick(self)
    if self.anime.flag == 2:                        // frame 20, 76 frames after the start
        SetAnime(self, 0) ; self.timer = 0 ; self.step = 1
```

`spawn_kid(self, var)` (inlined 3 times):

```
    k = GetVisualObject()
    if k:
        k.flags = 0x41 ; k.type = 27 ; k.var = var
        k.tex_cord = self.tex_cord ; k.clut_cord = self.clut_cord & 0x7FFF
        k.sprite_data = self.sprite_data ; k.layer = self.layer
        k.x = self.x ; k.y = self.y ; k.anime_table = self.anime_table
        k.parent(+0x7C, 32 bit) = self ; k.flip = self.flip
```

The step2 increment runs also when `GetVisualObject` returns 0.

### `ni_boomerang` — `0x800ED8AC` (step 3) and its sub-steps

```
boom_start (0x800EDACC): SetAnime(self, 14) ; self.step2 += 1
boom_throw (0x800EDB00):
    AnimeTick(self)
    if self.anime.flag == 1:                        // 48 frames after the start
        spawn_kid(self, var = 1)                    // var = the flag value 1
        self.step2 += 1
boom_wait  (0x800EDBD4):
    AnimeTick(self)                                 // anim 14 loops frames 13..16
    if self.returned(+0x07) == 1:
        self.returned = 0 ; SetAnime(self, 2) ; self.step2 += 1
boom_end   (0x800EDC24):
    AnimeTick(self)
    if self.anime.flag == 2:                        // 15 frames
        SetAnime(self, 0) ; self.timer = 0 ; self.step = 1
```

### `ni_lay` — `0x800ED8E8` (step 4) and its sub-steps

```
lay_start (0x800EDC6C): SetAnime(self, 3) ; self.timer = 0 ; self.step2 += 1
lay_spawn (0x800ECC38):
    AnimeTick(self)
    old = self.timer ; self.timer += 1
    if old >= 161 or count_hatchlings() >= 8:
        SetAnime(self, 4) ; self.step2 += 1
        return
    if self.timer % 20 == 0: spawn_kid(self, var = 2)   // the new value: 20, 40, ..., 160 (8 at most)
lay_end   (0x800EDCA4):
    AnimeTick(self)
    if self.anime.flag == 2:                        // 31 frames
        SetAnime(self, 0) ; self.timer = 0 ; self.step = 1
lay_hold  (0x800EDCEC): AnimeTick(self)             // no code sets step2 = 3
```

### `ni_revive` — `0x800ED924` (step 5)

```
ni_revive(self):
    AnimeTick(self)
    if self.anime.flag == 3:                        // last frame of anim 5 or 15
        self.hurt_box = 0x800F744C ; self.attack_box = 0x800F7448
        SetAnime(self, 0) ; self.timer = 0 ; self.step = 1
```

### `ni_delete` — `0x800ED830` (states 2 and 3)

```
    if self.state == 2: DeleteObject(self) else: DeleteObject2(self)
```

## Visual object type 27 (the shots and the hatchlings)

Work fields of this object: `+0x7C` parent (32 bit), `+0x8C` timer (16 bit), `+0x8E` / `+0x90`
target x / y in px (16 bit).

### `kid_update` — `0x800EDD2C`

```
    self.prev_x = self.x ; self.prev_y = self.y
    KID_STATE[self.state](self)
    CollideCheck(self)
```

### `kid_init` — `0x800ECDD4` (state 0)

```
kid_init(self):
    self.priority = 5 ; self.state += 1 ; self.step2 = 0 ; self.timer(+0x8C) = 0
    if self.var == 0:                                       // BLADE
        SetAnime(self, 7) ; PlaySound(2, 62, self)
        self.speed_x = +0x30000 if self.flip else -0x30000  // 3 px/frame
        self.speed_y = 0
        self.x.px += 16 if self.flip else -16
        self.contact_damage = 6
        self.attack_box = 0x800F76EC                        // (-32, -8, 64, 16)
        self.hurt_box   = 0x800F76F0                        // (-16, -8, 32, 16)
        self.terrain_box = 0 ; self.damage_table = 0x8007433C
        self.step = 0
    elif self.var == 1:                                     // BOOMERANG
        SetAnime(self, 7) ; PlaySound(2, 62, self)
        a = ratan2(mega.y.px - self.y.px, mega.x.px - self.x.px)
        self.speed_x = rcos(a) * 80 ; self.speed_y = -rsin(a) * 80     // 5 px/frame
        self.x.px += 16 if self.flip else -16
        self.contact_damage = 6
        boxes, terrain_box, damage_table as var 0
        self.step = 1
    elif self.var == 2:                                     // HATCHLING
        SetAnime(self, 6)
        self.speed_x = 0
        self.speed_y = -0x18000                             // 1.5 px/frame down
        if game.difficulty >= 2: self.speed_y = -0x30000    // 3 px/frame down
        self.x.px += 32 if self.flip else -32
        self.hp = 3 ; self.contact_damage = 3
        self.terrain_box = 0x800F76FC                       // (0, 0, 8, 8)
        self.hurt_box    = 0x800F76F8                       // (-4, -4, 8, 8)
        self.attack_box  = 0x800F76F4                       // (-4, -4, 8, 8)
        self.damage_table = 0x800741FC
        self.step = 2
        self.y.px -= 16
```

The angle is in the 4096-per-turn unit of `ratan2`; the screen y grows down, so `speed_y` takes the
minus sign.

### `kid_main` — `0x800ED584` (state 1)

```
kid_main(self):
    p = self.parent
    if ContactWeapon(self) < 0 or p.state == 2 or p.step == 5:
        if self.var == 2:
            SpawnJunk(2, {12, 13}, self)                    // 0x800F7694
            SpawnExplosion(self)
        self.state = 2
        return
    ContactMega(self)
    KID_STEP[self.step](self)
    if not OffScreenCheck2(self, 64, 64):
        DisplayCheck1(self)
    elif self.var == 2 and self.y.px - bg_layer[self.layer].y >= 301:
        DeleteObject3(self)                                 // fell out at the bottom
    if self.var == 2:
        self.latch = 0 if self.collide_flags & 8 else (-1 if self.speed_y < 0 else 1)
```

### `kid_delete` — `0x800EDD0C` (state 2): `DeleteObject3(self)`

### BLADE (step 0)

```
blade_fly (0x800ED020):
    AnimeTick(self) ; MoveObject(self)
    old = self.timer ; self.timer += 1
    if old >= 91 or (abs(mega.x.px - self.x.px) < 8 and abs(mega.y.px - self.y.px) < 32):
        self.timer = 0 ; self.step2 += 1
blade_stop (0x800EDD88):                                    // no MoveObject: it stands still
    AnimeTick(self)
    old = self.timer ; self.timer += 1
    if old < 91: return
    self.target = (mega.x.px, mega.y.px)
    aim at mega, 5 px/frame
    self.timer = 0
    self.step2 += 1 if game.difficulty >= 2 else 3          // Hard: chase; else: exit
blade_chase (0x800ED0F4):                                   // Hard only
    AnimeTick(self) ; MoveObject(self)
    old = self.timer ; self.timer += 1
    if old >= 91 or (abs(target.x - self.x.px) < 8 and abs(target.y - self.y.px) < 32):
        aim at mega, 5 px/frame                             // the speed is not used: next step stops
        self.step2 += 1                                     // the timer is not reset
blade_stop2 (0x800EDE78):
    AnimeTick(self)
    old = self.timer ; self.timer += 1
    if old < 91: return
    aim at mega, 5 px/frame ; self.timer = 0 ; self.step2 += 1
blade_exit (0x800EDF30):
    AnimeTick(self) ; MoveObject(self)
    if OffScreenCheck2(self, 64, 64) == 1: DeleteObject3(self)
```

### BOOMERANG (step 1)

```
boom_out (0x800ED220):
    AnimeTick(self) ; MoveObject(self)
    old = self.timer ; self.timer += 1
    if old >= 91 or (abs(mega.x.px - self.x.px) < 32 and abs(mega.y.px - self.y.px) < 16):
        self.timer = 0 ; self.step2 += 1
boom_back (0x800ED2F4):
    p = self.parent
    tx = p.x.px + 16 if self.flip else p.x.px - 16 ; ty = p.y.px
    a = ratan2(ty - self.y.px, tx - self.x.px)
    self.speed_x = rcos(a) * 48 ; self.speed_y = -rsin(a) * 48       // 3 px/frame
    AnimeTick(self) ; MoveObject(self)
    if abs(tx - self.x.px) < 16 and abs(ty - self.y.px) < 16:
        p.returned(+0x07) = 1
        self.state = 2
```

### HATCHLING (step 2)

```
hatch_fall (0x800EDFF8):
    AnimeTick(self) ; MoveObject(self)                      // constant fall, no gravity
    if self.collide_flags & 8:                              // on the floor
        self.timer = 0
        self.speed_x = +0x18000 if self.flip else -0x18000  // 1.5 px/frame
        if game.difficulty >= 2: self.speed_x *= 2          // 3 px/frame
        self.speed_y = 0 ; self.step2 += 1
hatch_walk (0x800ED428):
    AnimeTick(self) ; MoveObject(self)
    b = self.terrain_box
    fx = self.x.px + b.ox + b.w if self.flip else self.x.px - b.ox - b.w     // front edge, 8 px
    fy = self.y.px + b.oy + b.h + 8                         // 8 px below the feet
    floor = TerrainPointCheck(self, fx, fy) & 0xFF          // 0x80030CCC
    wall  = self.collide_flags & (1 if self.flip else 2)
    if floor == 0 or wall:
        self.flip ^= 0x40 ; self.speed_x = -self.speed_x
        MoveObject(self)
    old = self.timer ; self.timer += 1
    if old >= 241: self.timer = 0 ; self.step2 += 1
hatch_burst (0x800EE080):                                   // no MoveObject: it stops
    if self.timer & 1 == 0: self.clut_cord ^= 0x8000        // flash every 2 frames
    old = self.timer ; self.timer += 1
    if old < 51: return
    self.attack_box = 0x800F7700                            // (-16, -16, 32, 32)
    self.hurt_box = 0
    SpawnJunk(2, {12, 13}, self) ; SpawnExplosion(self)
    self.state = 2
```

The flip toggle is written as `flip = (flip == 0) << 6`.

## Engine function read outside the walk

### `0x80030CCC` (proposed `TerrainPointCheck(obj, x, y)`)

```
    for i in 0 .. *(short *)0x800E41A6 - 1:
        r = 0x800E41B0 + 8 * i                  // {short x0, x1, y0, y1}
        if r.x0 <= x <= r.x1 and r.y0 <= y <= r.y1: return 56
    return 0x80030234(...)                      // not read: terrain map lookup [likely]
```

### `ContactWeapon` and `ApplyWeaponHit` (what this object depends on)

- `ApplyWeaponHit` returns `~weapon_id` = `-(weapon id + 1)` when the hit kills the object
  (`nor v0,zero,s0` at `0x80031964` and `0x80031978`). So `-2`, `-11`, `-55` are ids 1, 10, 54.
- Class `0xFF` (−1) in the damage table: `WeaponHitReaction(-1)` (deflect), return 127. Class 2: no
  effect, return 0.
- `ContactWeapon` adds 1 to `game + 0x94` on a kill, except for the damage tables `0x800741FC` and
  `0x8007429C` (proposed `game.kill_count`) [likely].

## Engine helpers called

| address | name | use here |
|---|---|---|
| `0x80016C48` | `PlaySound` | bank 2 id 62, blade and boomerang throw |
| `0x800179A4` | `SetAnime` | all animation changes |
| `0x80017A04` | `AnimeTick` | every step |
| `0x8002C458` | `GetVisualObject` | the three shot types |
| `0x8002C9B0` | `DeleteObject3` | shot delete |
| `0x8002CA14` / `0x8002CA54` | `DeleteObject` / `DeleteObject2` | main delete (state 2 / 3) |
| `0x8002CB50` | `DisplayCheck1` | shots on screen |
| `0x8002CBFC` | `OffScreenCheck2` | 64 × 64 px margin, main and shots |
| `0x8002CCB0` | `DisplayCheck` | main, 48 × 48 px margin |
| `0x8002D2B0` | `MoveObject` | shots |
| `0x8002F288` | `CollideCheck` | after each update |
| `0x80030CCC` | (unnamed) | hatchling floor-ahead test |
| `0x80030ECC` | `ContactMega` | contact damage |
| `0x80031474` | `ContactWeapon` | hits |
| `0x80040290` | `HitReactionDispatch` | step 0 (no code here sets step 0) |
| `0x800477E4` | `SpawnJunk` | debris 8–11 (main), 12–13 (hatchling) |
| `0x80049E50` | `SpawnExplosion` | one explosion per death |
| `0x8004E654` | `DropItem` | table 21, real death only |
| `0x80065944` / `0x80065A14` / `0x80065B74` | `rsin` / `rcos` / `ratan2` | aiming |

## Object struct (the fields used here)

Main object:

| offset | name | use |
|---|---|---|
| `0x03` | `display` | attacks start only when 1 |
| `0x04` | `state` | 0 init, 1 main, 2 delete |
| `0x05` | `step` | 1 idle, 2 blade, 3 boomerang, 4 lay, 5 revive |
| `0x06` | `step2` | sub-step of steps 2–4 |
| `0x07` | (proposed `returned`) | set to 1 by the boomerang when it is back |
| `0x08` / `0x0A` | `x` / `x.px` | position |
| `0x0C` | `y` | position |
| `0x14` | `layer` | copied from the player |
| `0x15` | `flip` | 0 left, `0x40` right; faces the player |
| `0x16` | `priority` | 5 |
| `0x18` / `0x1C` | `prev_x` / `prev_y` | saved each frame |
| `0x20` / `0x24` | `speed_x` / `speed_y` | set to 0, never changed |
| `0x30` | `anime_table` | `0x800F7650` |
| `0x3C` / `0x40` / `0x42` | `sprite_data` / `tex_cord` / `clut_cord` | copied to the shots |
| `0x45` | `anime.flag` | events 1, 2, 3 |
| `0x50` / `0x54` | `attack_box` / `hurt_box` | 0 while it is a husk |
| `0x58` | `damage_table` | `0x8007479C` |
| `0x5C` | `hp` | 32, refilled on revive |
| `0x60` | `contact_damage` | 8 |
| `0x61` | `iframes` | 0 |
| `0x68` | `terrain_box` | 0 |
| `0x70` | `collide_flags` | 0 |
| `0x7C` | `work.7C` | timer (16 bit) |

Visual object type 27 (extra): `+0x02 var` (0 blade, 1 boomerang, 2 hatchling), `+0x5C hp`,
`+0x67 latch`, `+0x7C` parent pointer, `+0x8C` timer, `+0x8E` / `+0x90` target x / y.
