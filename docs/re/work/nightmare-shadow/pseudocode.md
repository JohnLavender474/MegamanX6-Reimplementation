# Nightmare shadow: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`). Addresses are RAM addresses in overlay 09.
Names of engine helpers and struct fields are in `catalog/helpers.json` of mmx-demake-tools.

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800F6360` state | 0 | `shadow_init` `0x800EE9E4` |
| | 1 | `shadow_main` `0x800EF060` |
| | 2 | `shadow_split` `0x800EF124` |
| | 3 | `shadow_delete` `0x800EF19C` (`DeleteObject3`) |
| `0x800F6370` step | 0 | `HitReactionDispatch` `0x80040290` (engine) |
| | 1 | `shadow_spawn` `0x800EECCC` |
| | 2 | `shadow_dash` `0x800EF1BC` |
| | 3 | `shadow_appear` `0x800EF1DC` |
| `0x800F6380` step2 (APPEAR) | 0 | `appear_blink` `0x800EF218` |
| | 1 | `appear_stand` `0x800EF264` |
| | 2 | `appear_wait` `0x800EEF04` |
| `0x800E9864` zone (init) | 0–5 | zone tests; 6–9 → no action; 10, 11 → shadow set-up |
| `0x800E9894` zone (spawn) | 0–5 | the same zone tests |

## Zone test

```
in_zone(var):                       // unsigned compares on mega.x.px
    0: 321  <= mega.x <= 2559
    1: 3201 <= mega.x <= 4159
    2: 1249 <= mega.x <= 1631
    3: 4961 <= mega.x <= 7311
    4: 1    <= mega.x <= 1055
    5: 321  <= mega.x <= 2143 and mega.y < 1072
```

### `shadow_update` — `0x800EF004` (main table entry 26)

```
self.prev_x = self.x ; self.prev_y = self.y
STATE_TABLE[self.state](self)
CollideCheck(self)                   // no terrain box: no effect
```

### `shadow_init` — `0x800EE9E4` (state 0)

```
if byte[0x800CD30A] != 7: DeleteObject3(self); return
self.flags = 0x41
load tex_cord, clut_cord, sprite_data from the sprite bank of self.type
ok = 0
switch self.var:
  case 0..5:
    if not in_zone(self.var): break
    ok = 1 ; self.step = 1 ; self.display = 0
    self.timer = (GetRNG() % 3 + 1) * 100 + 200          // 300, 400, 500
  case 10: SetAnime(self, 0) ; self.timer = 40 ; ok = 1 ; goto shadow
  case 11: self.timer = 40 ; SetAnime(self, 2) ; ok = 1
  shadow:
    self.hurt_box = 0 ; self.attack_box = 0 ; self.step = 3 ; self.display = 1
  default: break                     // var 6..9, 12+: stay in state 0
if not ok: return
self.hp = 4 ; self.iframes = 0 ; self.anime_table = 0x800F6350 ; self.priority = 4
self.terrain_box = 0 ; self.latch = 0 ; self.damage_table = 0x8007497C
self.contact_damage = 6 ; self.layer = mega.layer
self.state = 1 ; self.step2 = 0
self.flip = (mega.x >= self.x) ? 0x40 : 0
```

### `shadow_main` — `0x800EF060` (state 1)

```
STEP_TABLE[self.step](self)
r = ContactWeapon(self)
if r < 0:                                    // killed
    self.state = 2 ; self.timer = 60 ; self.display = 1
    self.split_r(+0x80) = self.x.px ; self.split_l(+0x82) = self.x.px
    self.clut_cord &= 0x7FFF
    return
ContactMega(self)
if OffScreenCheck2(self, 128, 128) == 1 and self.var in (10, 11):
    self.state = 3
```

### `shadow_spawn` — `0x800EECCC` (step 1, spawner)

```
if self.var >= 6 or not in_zone(self.var): return
if self.timer != 0: self.timer -= 1 ; return
o = GetMainObject()
if o:
    o.flags = 0x41 ; o.type = 26 ; o.flip = self.flip
    o.var = (mega.var == 0) ? 10 : 11
    o.x.px = mega.flip ? mega.x.px + 32 : mega.x.px - 32
    o.y.px = mega.y.px
    o.clut_cord = self.clut_cord & 0x7FFF ; o.anime_table = self.anime_table
    o.sprite_data = self.sprite_data ; o.tex_cord = self.tex_cord ; o.layer = self.layer
self.timer = (GetRNG() % 3 + 1) * 100 + 200
```

### `shadow_dash` — `0x800EF1BC` (step 2)

```
MoveObject(self)
```

### `shadow_appear` — `0x800EF1DC` (step 3)

```
APPEAR_TABLE[self.step2](self)
```

### `appear_blink` — `0x800EF218` (step2 0)

```
self.display ^= 1
self.timer -= 1
if self.timer == 0:
    self.hurt_box = 0x800F6334 ; self.attack_box = 0x800F6328     // (-10, -17, 19, 38)
    self.step2 += 1
```

### `appear_stand` — `0x800EF264` (step2 1)

```
self.display = 1 ; self.timer = 50 ; self.step2 += 1
```

### `appear_wait` — `0x800EEF04` (step2 2)

```
self.timer -= 1
if self.timer != 0: return
SetAnime(self, self.var == 11 ? 3 : 1)
a = ratan2(mega.y.px - self.y.px, mega.x.px - self.x.px)
self.speed_x = rcos(a) << 6                      // 4 px/frame at 1.0
self.speed_y = -(rsin(a) << 6)
self.hurt_box = 0x800F6338 ; self.attack_box = 0x800F632C       // (-11, -14, 18, 34)
self.step = 2 ; self.step2 = 0
self.flip = (mega.x >= self.x) ? 0x40 : 0
```

### `shadow_split` — `0x800EF124` (state 2)

```
self.split_r += 1 ; self.split_l -= 1
self.x.px = (frame_count(0x80097420) & 1) ? self.split_l : self.split_r
self.timer -= 1
if self.timer == 0: DeleteObject3(self)
```

### `shadow_delete` — `0x800EF19C` (state 3)

```
DeleteObject3(self)
```

## Engine helpers called

| address | name |
|---|---|
| `0x800179A4` | `SetAnime` |
| `0x8002A47C` | `GetSpriteBank` |
| `0x8002BBF4` | `GetMainObject` |
| `0x8002C9B0` | `DeleteObject3` |
| `0x8002CBFC` | `OffScreenCheck2` |
| `0x8002D2B0` | `MoveObject` |
| `0x8002D2D4` | `GetRNG` (0–255) |
| `0x8002F288` | `CollideCheck` |
| `0x80030ECC` | `ContactMega` |
| `0x80031474` | `ContactWeapon` |
| `0x80040290` | `HitReactionDispatch` |
| `0x80065944`, `0x80065A14`, `0x80065B74` | `rsin`, `rcos`, `ratan2` |

## Object struct (the fields used here)

| off | name | meaning | confidence |
|---|---|---|---|
| `+0x02` | `var` | 0–5 spawner zone; 10 X shadow; 11 Zero shadow | certain |
| `+0x03` | `display` | set by the code (blink), no `DisplayCheck` | certain |
| `+0x7C` | `timer` (u16) | spawn timer; blink 40; stand 50; death 60 | certain |
| `+0x80` | `split_r` (u16) | death: right copy x | certain |
| `+0x82` | `split_l` (u16) | death: left copy x | certain |
