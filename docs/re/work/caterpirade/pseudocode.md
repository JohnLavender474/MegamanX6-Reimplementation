# Caterpirade (stage object 1) and its spawner (main type 7): pseudo code

Source: `kit/code.txt`, `../caterpirade-main-7/kit/code.txt`, and `--at` reads of main-program code.
Addresses are RAM addresses (overlay 03 and the main program).

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| main table entry 7 | | `cat_spawner` `0x800EC25C` |
| stage table entry 1 | | `cat_update` `0x800EC300` |
| `0x800F7404` state | 0 | `cat_init` `0x800EC368` |
| | 1 | `cat_main` `0x800EBF6C` |
| | 2 | `cat_delete` `0x800EC454` (`DeleteObject`) |
| | 3 | `cat_delete2` `0x800EC434` (`DeleteObject2`) |
| `0x800F7414` step (`+0x06`) | 0 | `cat_walk` `0x800EC144` |
| | 1 | `cat_curl` `0x800EC474` |
| | 2 | `cat_fall` `0x800EC5F4` |
| | 3 | `cat_turn` `0x800EC66C` |
| | 4 | `cat_ball` `0x800EC6FC` |
| `0x800F7428` CURL sub (`+0x07`) | 0 | `curl_start` `0x800EC4BC` |
| | 1 | `curl_anim` `0x800EC520` |
| | 2 | `curl_wait` `0x800EC564` |
| `0x800F7434` BALL sub (`+0x07`) | 0 | `ball_roll` `0x800EC738` |
| | 1 | `ball_slow` `0x800EC7A4` |
| | 2 | `ball_open` `0x800EC800` |
| | 3 | `ball_uncurl` `0x800EC8A0` |

## Spawner

### `cat_spawner` — `0x800EC25C` (main type 7)

```
s = GetStageObject()                      // 0x8002C4C4
if s == 0: DeleteObject2(self) ; return   // try again later
s.type = 1 ; s.flags = self.flags ; s.var = self.var
s.x.px = self.x.px ; s.y.px = self.y.px ; s.record = self.record
s.tex_cord = self.tex_cord ; s.clut_cord = self.clut_cord ; s.sprite_data = self.sprite_data
DeleteObject3(self)                       // record keeps its spawned mark
```

### `GetStageObject` — `0x8002C4C4` (main program)

```
for p in 0x800C1220 .. 0x800C1220 + 80*0xA4 step 0xA4:
    if p.flags == 0:
        clear 0xA4 bytes ; p+0x50 = p+0x54 = p+0x68 = 0
        return p
return 0
```

## Caterpirade

### `cat_update` — `0x800EC300` (stage table entry 1)

```
if self.collide_flags & 8: self.speed_y = 0
self.prev_x = self.x ; self.prev_y = self.y
STATE[self.state](self)
```

### `cat_init` — `0x800EC368` (state 0)

```
self.step(+5) = 2 ; self.mode(+6) = 2 (FALL) ; self.sub(+7) = 0 ; self.state += 1
self.priority = 7 ; self.layer = mega.layer
self.terrain_box = 0x800F73EC   // (0,0,14,14)
self.hurt_box    = 0x800F73FC   // (-12,-8,24,18)
self.attack_box  = 0x800F73F4   // (-18,-7,36,20)
self.anime_table = 0x800F73D4 ; self.damage_table = dmg_tbl_normal
self.gravity = 0x2000 ; self.hp = 12 ; self.contact_damage = 4
self.iframes = 0 ; self+0x75 = 0 ; self.flip = 0 ; self.last_hit_tag = 0
self.speed_x = self.speed_y = self.accel_x = 0 ; self.latch = 0 ; self.timer(+0x80) = 40
SetAnime(self, 0)
```

### `cat_main` — `0x800EBF6C` (state 1)

```
self.clut &= 0x7FFF ; hp0 = self.hp
r = ContactWeapon(self)
if r < 0:
    SpawnExplosion(self) ; DropItem(self, 16) ; self.state = 2 ; return
if self.hp != hp0:
    PlaySound(2, 8, self)
    self.speed_x = 0 ; self.accel_x = 0 ; self.anime.sprite = 0
    self.mode = 1 (CURL) ; self.sub = 0 ; self.clut |= 0x8000
STEP[self.mode](self)
if OffScreenCheck2(self, 64, 64): self.state = 3
else: DisplayCheck(self, 32, 32)
DisplayCheck(self, 32, 32)
CollideCheck(self)
if mega+0xE5 == 0 and self.mode != 4 and self.mode != 1:
    old = mega+0x72 ; mega+0x72 = 8
    was = self+0x72
    SolidObjectMega(self)                         // 0x80031DA8
    mega+0x72 = old | (self+0x72 & 8)
    if (was & 8) and !(self+0x72 & 8): mega+0x72 |= 8
ContactMega(self)
if self.collide_flags & 8: self.latch = 0
else: self.latch = (self.speed_y < 0) ? -1 : 1
```

### `cat_walk` — `0x800EC144` (step 0)

```
AnimeTick(self) ; MoveObject2(self)
tb = self.terrain_box
px = self.flip ? x.px + tb.ox + tb.w : x.px - tb.ox - tb.w
py = y.px + tb.oy + tb.h + 8
if !TerrainPointCheck(self, px, py)          -> turn
elif !flip and collide_flags & 2              -> turn
elif  flip and collide_flags & 1              -> turn
turn: self.mode = 3 ; self.timer = 40
```

### `cat_fall` — `0x800EC5F4` (step 2)

```
if collide_flags & 8:
    self.mode = 0
    if mega.x < self.x: speed_x = -0x8000 ; flip = 0
    else:               speed_x = +0x8000 ; flip = 0x40
    self.latch = 0
else: MoveObject2(self)
```

### `cat_turn` — `0x800EC66C` (step 3)

```
if self.timer != 0:
    if --self.timer == 0: SetAnime(self, 1)
else:
    AnimeTick(self)
    if anime.flag:
        SetAnime(self, 0) ; self.mode = 0
        self.flip = (flip == 0) ? 0x40 : 0 ; self.speed_x = -self.speed_x
```

### `cat_curl` — `0x800EC474` (step 1)

```
MoveObject2(self) ; CURL_SUB[self.sub](self)
```

- `curl_start` `0x800EC4BC`: `SetAnime(2)`; terrain `0x800F73F0`, hurt `0x800F7400`, attack
  `0x800F73F8`; `dmg_tbl_deflect`; sub += 1.
- `curl_anim` `0x800EC520`: `AnimeTick`; on flag: timer = 40, sub += 1.
- `curl_wait` `0x800EC564`: when `--timer == 0`: `SetAnime(3)`; face the player with
  `speed_x` = ∓`0x10000`; timer = 128; mode = 4; sub = 0.

### `cat_ball` — `0x800EC6FC` (step 4)

```
BALL_SUB[self.sub](self)
```

- `ball_roll` `0x800EC738`: `AnimeTick`, `MoveObject2`; when `--timer == 0`: `accel_x` = −`0x200`,
  `SetAnime(5)`, timer = 90, sub += 1.
- `ball_slow` `0x800EC7A4`: `AnimeTick`, `MoveObject2`; when `--timer == 0`: `speed_x` = `accel_x`
  = 0, sub += 1.
- `ball_open` `0x800EC800`: `MoveObject2`, `AnimeTick`; when sprite is 23 or 16:
  `PlaySound(2, 9)`, normal boxes, `dmg_tbl_normal`, `SetAnime(4)`, sub += 1.
- `ball_uncurl` `0x800EC8A0`: `MoveObject2`, `AnimeTick`; on flag: `SetAnime(0)`, mode = 2, sub = 0.

## Shared solid-object code (main program)

### `SolidObjectMega` — `0x80031DA8`

```
if obj.terrain_box == 0 or byte[0x800C4568] != 0: return
sx, sy = mega.x.px, mega.y.px
if SolidPush(obj, mega) == 0:                       // 0x80032208
    if obj+0x76 > 0: SolidCarry(obj, mega, obj+0x72) // 0x800324C8
    SolidTouch(obj, mega, 0)                        // 0x8002E1A0
    if mega moved:
        CollideCheck(mega)
        if (mega.collide & 0xC) == 0xC or (mega.collide & 3) == 3: mega.x.px, mega.y.px = sx, sy
        else: mega.prev = mega.pos
if obj not in player weapons (0x800950A0 .. +0x1400) and byte[0x800CD340] != 0:
    a = 0x800CD340
    SolidPush(obj, a)
    if obj+0x77 > 0: SolidCarry(obj, a, obj+0x73)
    SolidTouch(obj, a, 1)
    if mega.step == 18: mega.x, mega.y = a.x, a.y
```

### `SolidPush` — `0x80032208`

```
if b.terrain_box == 0: return 1
if a+0x66 == 0 and CentreInside(a, b):            // 0x8002DE54
    b+0x71 = 15 ; if b == mega: mega+0xE6 = a.contact_damage
    return 1
keep = a+0x66
if Overlap(a, b):                                 // 0x8002DCF4
    (adjust a+0x66 by the side; details not decoded)
    PushX(a, b) -> b+0x6C ; PushY(a, b) -> b+0x6E  // 0x8002DFA0, 0x8002E0DC
    if |b+0x6C| < |b+0x6E| (or a+0x66 > 0): b.x.px += b+0x6C
    else:                                   b.y.px += b+0x6E
a+0x66 = keep
return 0
```

### `SolidCarry` — `0x800324C8`

```
if flags & 8:
    dx = a.x.px - a.prev_x.px
    if dx and mega+0x138 == 0: mega+0x138 = dx ; b.x.px += dx ; b+0x13E += dx
    dy = a.y.px - a.prev_y.px
    if dy > 0: b.y.px += dy
    elif dy < 0 and overlapping in y: b.y.px = a.y + a.oy - a.h - b.h - b.oy
elif flags & 4 and (b.step == 31 and b.step2 < 4 or b.step == 63):
    same x carry ; b.y.px = a.y + a.oy + a.h + b.h - b.oy
elif b.step in (11, 52, 9, 47):
    if a moved in x: put b against the side given by flags & 2 / & 1
    if a moved down or b+0xE7 == 2: b.y.px += dy
```

### `SolidTouch` — `0x8002E1A0`

```
t = 0
if |dy| == a.h + b.h and |dx| < a.w + b.w: t = (a.y < b.y) ? 4 : 8
if |dx| == a.w + b.w and |dy| < a.h + b.h: t |= (a.x < b.x) ? 2 : 1 ; b+0x4A = a+0x75
a+0x72+which = t ; b+0x71 |= t
if which == 0: a+0x76 = (t & 0xB) or (t & 4 and b.step in (31, 63)) ? 1 : 0
if which == 1: a+0x77 = (t & 0xB) ? 1 : 0     // which == 2 -> +0x78
if t: mega+0xE6 = a.contact_damage
```

## Engine helpers called

| address | name | use here |
|---|---|---|
| `0x8002C4C4` | GetStageObject (new name) | spawner |
| `0x8002C9B0` | DeleteObject3 | spawner deletes itself |
| `0x8002CA14` / `0x8002CA54` | DeleteObject / DeleteObject2 | states 2 / 3; spawner without slot |
| `0x800179A4` / `0x80017A04` | SetAnime / AnimeTick | all steps |
| `0x8002D230` | MoveObject2 | all moving steps |
| `0x80030CCC` | TerrainPointCheck | ledge probe |
| `0x8002F288` | CollideCheck | main, and on the player inside `SolidObjectMega` |
| `0x80031474` | ContactWeapon | main |
| `0x80030ECC` | ContactMega | main |
| `0x80031DA8` | SolidObjectMega (new name) | platform |
| `0x8002CBFC` / `0x8002CCB0` | OffScreenCheck2 / DisplayCheck | 64 / 32 |
| `0x80049E50` | SpawnExplosion | death |
| `0x8004E654` | DropItem | table 16 |
| `0x80016C48` | PlaySound | bank 2, ids 8 and 9 |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x00` | flags | `0x41` |
| `+0x01` | type | 1 (made by main type 7) |
| `+0x02` | var | record `id` (not read) |
| `+0x04` | state | 0–3 |
| `+0x05` | step | set to 2, not read |
| `+0x06` | mode (kit: step2) | step index 0–4 |
| `+0x07` | sub (kit: act4) | sub-step |
| `+0x08`/`+0x0C` | x / y | 16.16 |
| `+0x10` | record | copied from the spawner |
| `+0x14` | layer | `mega.layer` |
| `+0x15` | flip | 0 left, `0x40` right |
| `+0x16` | priority | 7 |
| `+0x18`/`+0x1C` | prev_x / prev_y | carry uses the px halves |
| `+0x20`/`+0x24`/`+0x28`/`+0x2C` | speed_x / speed_y / accel_x / gravity | |
| `+0x30` | anime_table | `0x800F73D4` |
| `+0x42` | clut_cord | bit 15 = hit flash |
| `+0x45` / `+0x47` | anime.flag / anime.sprite | |
| `+0x50`/`+0x54`/`+0x58` | attack_box / hurt_box / damage_table | two sets |
| `+0x5C` | hp | 12 |
| `+0x60` | contact_damage | 4; also copied to `mega+0xE6` on a touch |
| `+0x61` | iframes | 0 |
| `+0x65` | last_hit_tag | 0 |
| `+0x66` | push memory | read by `SolidPush` (0 here) |
| `+0x67` | latch | 0 / ±1 |
| `+0x68` | terrain_box | `0x800F73EC` / `0x800F73F0` |
| `+0x70` | collide_flags | 1 right wall, 2 left wall, 8 floor |
| `+0x72`/`+0x73`/`+0x74` | touch flags vs player / actor `0x800CD340` / third | set by `SolidTouch` |
| `+0x75` | side kind | copied to `mega+0x4A`; 0 |
| `+0x76`/`+0x77`/`+0x78` | carry flags | set by `SolidTouch`; cleared by player damage |
| `+0x80` | timer | 40 / 128 / 90 |

## Player fields used

| offset | use |
|---|---|
| `mega+0x05`, `+0x06` | player step and sub-step (tested: 9, 11, 18, 31, 47, 52, 63) |
| `mega+0x4A` | side kind of the touched object; cleared per frame |
| `mega+0x6C`, `+0x6E` | push amounts x / y |
| `mega+0x71` | touched-object flags (OR); cleared per frame |
| `mega+0x72` | bit 8 = standing on an object; `CollideCheck` skips floor tiles; cleared per frame |
| `mega+0xE5` | post-hit timer, 75 |
| `mega+0xE6` | damage of the next player hit |
| `mega+0xE7` | tested = 2 in the side carry |
| `mega+0x138` | carry x move of this frame (one carrier per frame) |
| `mega+0x13C`/`+0x140` | a second copy of the player x / y (16.16); `+0x13E` moved with the carry |
