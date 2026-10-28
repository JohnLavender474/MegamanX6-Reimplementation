# Guardian and its shot: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`). Addresses are RAM addresses in overlay 10
(stage 7). Names of engine helpers and struct fields are in `catalog/helpers.json` of
mmx-demake-tools. Names of the object's own functions are mine.

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800FC460` state | 0 | `grd_init` `0x800EFE48` |
| | 1 | `grd_main` `0x800F006C` |
| | 2 | `grd_dead` `0x800F0434` |
| | 3, 4 | `grd_delete` `0x800F04A0` |
| `0x800FC474` step | 0 | `HitReactionDispatch` `0x80040290` (engine) |
| | 1 | `grd_resume` `0x800F0994` |
| | 2, 6 | `grd_nop` `0x800F04F0`, `0x800F0930` |
| | 3 | `grd_wait` `0x800F04F8` |
| | 4 | `grd_turn` `0x800F0648` |
| | 5 | `grd_attack` `0x800F073C` |
| | 7 | `grd_unused` `0x800F0938` |
| `0x800FC494` step2 (WAIT) | 0, 1, 2 | `wait_start` `0x800F0534`, `wait_hover` `0x800F0570`, `wait_near` `0x800F05EC` |
| `0x800FC4A0` step2 (TURN) | 0, 1, 2 | `turn_start` `0x800F0684`, `turn_flip` `0x800F06B8`, `turn_end` `0x800F0710` |
| `0x800FC4AC` step2 (ATTACK) | 0–5 | `atk_start` `0x800F0778`, `atk_open` `0x800F07AC`, `atk_aim` `0x800F07F4`, `atk_fire` `0x800F0258`, `atk_rest` `0x800F08A8`, `atk_close` `0x800F08FC` |
| `0x800FC4C4` step2 (step 7) | 0–3 | empty routines `0x800F0974`, `0x800F097C`, `0x800F0984`, `0x800F098C` |
| `0x800FC4DC` shot state | 0, 1, 2 | `shot_init` `0x800F0A00`, `shot_move` `0x800F0AB4`, `shot_delete` `0x800F0B90` |
| `0x800FC45C` | | debris animations `{8, 9, 10, 11}` |
| main table entry 30 | | `grd_update` `0x800F03F8` |
| visual table entry 35 | | `shot_update` `0x800F09C4` |

Boxes `{ox, oy, w, h}`:

| address | box | normal (`var` < 16) | large (`var` ≥ 16) |
|---|---|---|---|
| `0x800FC29C` / `0x800FC2B0` | attack, closed | (−17, −16, 10, 34) | (−24, −27, 17, 54) |
| `0x800FC2A0` / `0x800FC2B4` | attack, open | (−21, −26, 42, 16) | (−42, −33, 80, 17) |
| `0x800FC2A4` / `0x800FC2B8` | second attack | (−8, −2, 33, 6) | (−15, −2, 39, 6) |
| `0x800FC2A8` / `0x800FC2BC` | body hurt (normal table) | (−8, −7, 34, 16) | (−15, −7, 40, 16) |
| `0x800FC2AC` / `0x800FC2C0` | terrain (set-up only) | (0, 0, 16, 16) | (0, 0, 16, 16) |
| `0x800FC2C4` | shield hurt, closed (deflect table) | (−24, −27, 17, 54) | same |
| `0x800FC2C8` | shield hurt, open (deflect table) | (−31, −32, 62, 20) | same |
| `0x800FC2CC` | terrain while open | (0, 0, 16, 16) | same |
| `0x800FC4D4` | shot hurt and attack | (−3, −2, 5, 4) | |
| `0x800FC4D8` | shot terrain | (0, 0, 5, 5) | |

Box pointers (overlay globals, shared by all Guardians): `0x800FCFF0` closed attack, `0x800FCFF4`
open attack, `0x800FCFF8` body hurt, `0x800FCFFC` second attack.

Work: `+0x7C` timer / shots left; `+0x7E` timer; `+0x81` aim direction; `+0x8B` open flag; `+0x94`
resume step; `+0x99` set to 1 (not read here).

## Guardian

### `grd_update` — `0x800F03F8`

```
grd_update(self): STATE_TABLE[self.state](self)
```

### `grd_init` — `0x800EFE48` (state 0)

```
grd_init(self):
    self.hp = 6 ; self.iframes = 0
    self.anime_table = 0x800FC428 ; self.latch = 0 ; self.priority = 6
    self.hurt_box = self.attack_box = 0 ; self.collide_flags = 0
    self.speed_x = self.speed_y = self.accel_x = self.gravity = 0
    self.+0x99 = 1 ; self.+0x8B = 0
    self.layer = mega.layer ; self.prev = position
    if (self.var & 0xF0) == 0:
        self.terrain_box = 0x800FC2AC ; self.contact_damage = 6
        BOX_CLOSED = 0x800FC29C ; BOX_OPEN = 0x800FC2A0
        BOX_BODY = 0x800FC2A8 ; BOX_BAR = 0x800FC2A4
    else:                                                // the large Guardian: next sprite bank
        b = GetSpriteBank(self.type)                     // first bank of type 30
        self.tex_cord = bank_tpage[b + 1] >> 7
        cx = ((b + 1) * 4 + 32) % 16                     // C division rules as in the binary
        self.clut_cord = ((480 + (b + 9) / 4) << 6) | cx
        self.sprite_data = sprite_bank_base + sprite_bank_base->ofs[b + 1]
        self.contact_damage = 7 ; self.terrain_box = 0x800FC2C0
        BOX_CLOSED = 0x800FC2B0 ; BOX_OPEN = 0x800FC2B4
        BOX_BODY = 0x800FC2BC ; BOX_BAR = 0x800FC2B8
    SetAnime(self, 0)
    self.flip = (self.var & 0x0F) ? 0x40 : 0
    self.state = 1 ; self.step = 3 ; self.step2 = 0
```

### `grd_main` — `0x800F006C` (state 1)

```
grd_main(self):
    self.prev = position
    AnimeTick(self)
    STEP_TABLE[self.step](self)
    if self.state != 2:
        self.attack_box = self.+0x8B ? BOX_OPEN : BOX_CLOSED
        ContactMega(self)
        self.attack_box = BOX_BAR
        ContactMega(self)
    self.damage_table = dmg_tbl_deflect                  // the shield
    if self.+0x8B == 0: self.hurt_box = 0x800FC2C4 ; self.terrain_box = 0x800FC2AC
    else:               self.hurt_box = 0x800FC2C8 ; self.terrain_box = 0x800FC2CC
    r = ContactWeapon(self)
    if r >= 0:                                           // the body
        self.damage_table = dmg_tbl_normal
        self.hurt_box = BOX_BODY
        r = ContactWeapon(self)
    if r < 0:                                            // killed
        if r != -56: DropItem(self, 20)                  // no drop for weapon id 55
        goto explode
    if OffScreenCheck2(self, 96, 96): self.state = 4 ; return
    DisplayCheck(self, 64, 64)
    if (self.collide_flags & 0x0C) == 0x0C or (self.collide_flags & 0x03) == 0x03:
        goto explode                                     // squeezed: no drop
    CollideCheck(self)
    return
explode:
    SpawnExplosion(self) ; self.terrain_box = 0
    SpawnJunk(4, {8, 9, 10, 11}, self)
    self.+0x7C = 32 ; self.+0x7E = 6
    self.display = 0 ; self.state = 2
```

### `atk_fire` — `0x800F0258` (ATTACK step2 3)

```
atk_fire(self):
    d = self.+0x81
    if 9 <= d <= 23:   back = (self.flip != 0)           // left half: back when facing right
    elif d == 8 or d == 24: back = 0                     // straight up or down: always fires
    else:              back = (self.flip == 0)           // right half: back when facing left
    if back:
        self.+0x7C = 10 ; self.step2 += 1                // step2 4: no more shots
    if self.step2 == 4: return
    SetAnime(self, 4)
    s = GetVisualObject()
    if s:
        s.flags = 0x41 ; s.type = 35 ; s.var = self.var
        s.tex_cord = self.tex_cord ; s.clut_cord = self.clut_cord
        s.anime_table = self.anime_table ; s.sprite_data = self.sprite_data
        s.flip = self.flip ; s.layer = self.layer ; s.x = self.x ; s.y = self.y
        SetSpeedDir(s, d)
        s.state = 0
        s.speed_x *= 3 ; s.speed_y *= 3                  // 3 px/update
    self.+0x7C -= 1
    if self.+0x7C == 0: self.+0x7C = 10 ; self.step2 += 1        // step2 4
    else:               self.+0x7E = 20 ; self.step2 -= 1        // step2 2: aim again
```

### `grd_dead` — `0x800F0434` (state 2)

```
grd_dead(self):
    self.+0x7C -= 1
    if self.+0x7C == 0: self.state = 3 ; return
    self.+0x7E -= 1
    if self.+0x7E == 0:
        self.+0x7E = 6
        SpawnExplosionRand(self, 1, 16, 16)
```

### `grd_delete` — `0x800F04A0` (states 3 and 4)

```
grd_delete(self):
    self.+0x80 = self.+0x84 = self.+0x88 = self.+0x8C = self.+0x94 = 0     // 32-bit stores
    if self.state == 4: DeleteObject2(self) else: DeleteObject(self)
```

### `grd_nop` — `0x800F04F0` (step 2), `0x800F0930` (step 6)

```
grd_nop(self): return
```

### `grd_wait` — `0x800F04F8` (step 3)

```
grd_wait(self): WAIT_TABLE[self.step2](self)
```

### `wait_start` — `0x800F0534` (WAIT step2 0)

```
wait_start(self): SetAnime(self, 0) ; self.+0x7C = 120 ; self.step2 += 1
```

### `wait_hover` — `0x800F0570` (WAIT step2 1)

```
wait_hover(self):
    if (self.+0x7C & 31) == 0 and player_behind(self):
        self.step = 4 ; self.step2 = 0 ; return
    self.+0x7C -= 1
    if self.+0x7C == 0: self.step2 += 1
```

### `wait_near` — `0x800F05EC` (WAIT step2 2)

```
wait_near(self):
    if mega.x.px - self.x.px < 188:                      // signed: the player left counts at any distance
        self.step = player_behind(self) ? 4 : 5
        self.step2 = 0
```

### `grd_turn` — `0x800F0648` (step 4)

```
grd_turn(self): TURN_TABLE[self.step2](self)
```

### `turn_start` — `0x800F0684` (TURN step2 0)

```
turn_start(self): SetAnime(self, 2) ; self.step2 += 1
```

### `turn_flip` — `0x800F06B8` (TURN step2 1)

```
turn_flip(self):
    if self.anime.flag:                                  // frame 6 of animation 2
        self.flip ^= 0x40
        SetAnime(self, 1)
        self.+0x7C = 20 ; self.step2 += 1
```

### `turn_end` — `0x800F0710` (TURN step2 2)

```
turn_end(self):
    self.+0x7C -= 1
    if self.+0x7C == 0: self.step = 5 ; self.step2 = 0   // always attack after a turn
```

### `grd_attack` — `0x800F073C` (step 5)

```
grd_attack(self): ATTACK_TABLE[self.step2](self)
```

### `atk_start` — `0x800F0778` (ATTACK step2 0)

```
atk_start(self): SetAnime(self, 3) ; self.step2 += 1
```

### `atk_open` — `0x800F07AC` (ATTACK step2 1)

```
atk_open(self):
    if self.anime.flag == 2: self.+0x8B = 1              // shield up: body open
    if self.anime.flag == 1:
        self.+0x7C = 4 ; self.+0x7E = 30 ; self.step2 += 1
```

### `atk_aim` — `0x800F07F4` (ATTACK step2 2)

```
atk_aim(self):
    self.+0x7E -= 1
    if self.+0x7E != 0: return
    self.x.px += self.flip ? 6 : -6                      // aim from the muzzle
    self.+0x81 = AimAt(self, mega)
    self.x.px -= self.flip ? 6 : -6
    self.step2 += 1
```

### `atk_rest` — `0x800F08A8` (ATTACK step2 4)

```
atk_rest(self):
    self.+0x7C -= 1
    if self.+0x7C == 0: SetAnime(self, 12) ; self.step2 += 1
```

### `atk_close` — `0x800F08FC` (ATTACK step2 5)

```
atk_close(self):
    if self.anime.flag == 2: self.+0x8B = 0              // shield down: closed
    if self.anime.flag == 1: self.step = 3 ; self.step2 = 0
```

### `grd_unused` — `0x800F0938` (step 7), `grd_resume` — `0x800F0994` (step 1)

```
grd_unused(self): STEP7_TABLE[self.step2](self)          // four empty routines
grd_resume(self): self.step = self.+0x94                 // nothing writes +0x94 except the delete (0)
```

### `player_behind` — `0x800F09A0`

```
player_behind(self): return (self.flip ^ ((self.x.px < mega.x.px) << 6)) != 0
```

## Shot (visual type 35)

### `shot_update` — `0x800F09C4`

```
shot_update(s): SHOT_STATE_TABLE[s.state](s)
```

### `shot_init` — `0x800F0A00` (state 0)

```
shot_init(s):
    s.damage_table = dmg_tbl_one_hit
    s.clut_cord &= 0x7FFF
    s.state = 1 ; s.display = 1 ; s.priority = 0
    s.x.px += s.flip ? 6 : -6
    PlaySound(2, 79, s)
    s.terrain_box = 0x800FC4D8
    s.hurt_box = s.attack_box = 0x800FC4D4
    s.hp = 1 ; s.contact_damage = 3
    SetAnime(s, 5)
```

### `shot_move` — `0x800F0AB4` (state 1)

```
shot_move(s):
    AnimeTick(s) ; MoveObject(s)
    if ContactMega(s): s.state = 2 ; return
    if ContactWeapon(s) < 0: SpawnExplosion(s) ; s.state = 2
    else: s.clut_cord &= 0x7FFF                          // no hit flash
    if s.+0x70 == 0: CornerCheck(s)
    if s.+0x70 != 0: SpawnHitEffect(s) ; s.state = 2 ; return
    if OffScreenCheck2(s, 32, 32): s.state = 2 ; return
    DisplayCheck(s, 16, 16)
```

### `shot_delete` — `0x800F0B90` (state 2)

```
shot_delete(s): DeleteObject3(s)
```

## Engine helpers called

| address | name | use here |
|---|---|---|
| `0x80016C48` | `PlaySound` | bank 2, id 79 at each shot (from the shot) |
| `0x800179A4` / `0x80017A04` | `SetAnime` / `AnimeTick` | animations 0–5, 12; tick in `grd_main` before the step |
| `0x8002A47C` | `GetSpriteBank` | the large Guardian |
| `0x8002C458` | `GetVisualObject` | the shot |
| `0x8002C9B0` / `0x8002CA14` / `0x8002CA54` | `DeleteObject3` / `DeleteObject` / `DeleteObject2` | shot end / after death / off screen |
| `0x8002CBFC` / `0x8002CCB0` | `OffScreenCheck2` / `DisplayCheck` | 96 / 64 (body), 32 / 16 (shot) |
| `0x8002D2B0` | `MoveObject` | the shot only |
| `0x8002D380` / `0x8002D3B4` | `AimAt` / `SetSpeedDir` | aim |
| `0x8002EE10` | `CornerCheck` | shot on walls |
| `0x8002F288` | `CollideCheck` | after each main update |
| `0x80030ECC` / `0x80031474` | `ContactMega` / `ContactWeapon` | two contact tests; shield then body |
| `0x80040290` | `HitReactionDispatch` | step 0 |
| `0x800477E4` | `SpawnJunk` | 4 pieces |
| `0x80049D90` | `SpawnHitEffect` | shot on a wall |
| `0x80049E50` / `0x80049EBC` | `SpawnExplosion` / `SpawnExplosionRand` | death |
| `0x8004E654` | `DropItem` | table 20 |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x01` | `type` | for `GetSpriteBank` |
| `+0x02` | `var` | low digit: facing (0 left, else right); high digit ≠ 0: the large Guardian; copied to the shot |
| `+0x03` | `display` | 0 at death |
| `+0x04`, `+0x05`, `+0x06` | `state`, `step`, `step2` | |
| `+0x08`, `+0x0C` | `x`, `y` | 16.16; changed only for the aim (±6, then back) |
| `+0x14`, `+0x15`, `+0x16` | `layer`, `flip`, `priority` | |
| `+0x18`, `+0x1C` | `prev_x`, `prev_y` | |
| `+0x20`…`+0x2C` | speeds | cleared at set-up, never set |
| `+0x30`, `+0x3C`, `+0x40`, `+0x42` | animation table, sprite data, texture, palette | set for the large Guardian; copied to the shot |
| `+0x45` | `anime.flag` | 1 and 2 |
| `+0x50`, `+0x54`, `+0x58` | attack box, hurt box, damage table | changed twice each update |
| `+0x5C`, `+0x60`, `+0x61` | `hp`, `contact_damage`, `iframes` | 6; 6 or 7; 0 |
| `+0x67` | `latch` | 0 |
| `+0x68`, `+0x70` | terrain box, `collide_flags` | |
| `+0x7C`, `+0x7E` | timers | |
| `+0x81` | aim direction | 0..31 |
| `+0x8B` | open flag | |
| `+0x94` | resume step | |
| `+0x99` | 1 | not read here |
