# Meta Wheel F and its stud overlay: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`). Addresses are RAM addresses in overlay 02.
Names of engine helpers and struct fields are in `catalog/helpers.json` of mmx-demake-tools.

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800FAD04` state | 0 | `wheel_init` `0x800EB450` |
| | 1 | `wheel_main` `0x800EB5E4` |
| | 2 | `wheel_delete` `0x800EBC98` (`DeleteObject`) |
| | 3 | `wheel_delete2` `0x800EBCB8` (`DeleteObject2`) |
| `0x800FAD14` step | 0 | `HitReactionDispatch` `0x80040290` (engine) |
| | 1 | `wheel_roll` `0x800EBCD8` |
| | 2 | `wheel_fall` `0x800EBDC8` |
| | 3 | `wheel_turn` `0x800EBEA0` |
| `0x800FAD24` step2 (TURN) | 0 | `turn_start` `0x800EBEDC` |
| | 1 | `turn_wait` `0x800EBEF4` |
| | 2 | `turn_flip` `0x800EB78C` |
| misc table entry 2 | | `studs_update` `0x800EBFEC` |

## Wheel

### `wheel_update` — `0x800EBC24` (main table entry 2)

```
wheel_update(self):
    self.prev_x = self.x ; self.prev_y = self.y
    if self.collide_flags & 8: self.speed_y = 0          // on the floor
    STATE_TABLE[self.state](self)
    CollideCheck(self)
    wheel_pair_test(self)                                // 0x800EBF68
```

### `wheel_init` — `0x800EB450` (state 0)

```
wheel_init(self):
    self.hp = 32 ; self.timer(+0x7C) = 0 ; self.iframes = 0
    self.anime_table = 0x800FACD4 ; self.priority = 6 ; self.layer = mega.layer
    self.terrain_box = 0x800FAB7C       // (0, 0, 30, 30)
    self.attack_box  = 0x800FAB80       // (-20, -20, 40, 40)
    self.hurt_box    = 0x800FAB84       // (-23, -23, 46, 46)
    self.contact_damage = 4 ; self.damage_table = 0x8007447C
    self.step = 2 (FALL) ; self.step2 = 0
    self.accel_x = -0xC00 ; self.gravity = 0x2000
    self.latch = 0 ; self.flip = 0 ; self.speed_x = self.speed_y = 0
    self.state = 1 ; self.prev = position
    if self.var == 0: self.dir(+0x07) = 0    ; SetAnime(self, 2)    // rolls right
    else:             self.dir = 0x40        ; SetAnime(self, 0)    // rolls left
    self.last_sprite(+0x80) = self.anime.sprite
    s = GetMiscObject()
    if s:
        s.flags = 0x41 ; s.type = 2 ; s.var = self.anime.sprite
        s.tex_cord = self.tex_cord ; s.clut_cord = self.clut_cord & 0x7FFF
        s.anime_table = self.anime_table ; s.sprite_data = self.sprite_data
        s.layer = self.layer ; s.x = self.x ; s.y.px = self.y.px
        s.parent(+0x50) = self ; s.flip = self.flip
```

### `wheel_main` — `0x800EB5E4` (state 1)

```
wheel_main(self):
    self.last_sprite = self.anime.sprite
    wheel_fall_check(self)                               // 0x800EBF28
    STEP_TABLE[self.step](self)
    if self.dir == 0: self.speed_x = min(self.speed_x, 0xC000)      // 0.75 px/frame
    else:             self.speed_x = max(self.speed_x, -0xC000)
    if ContactWeapon(self) < 0 or (self.collide_flags & 0x0C) == 0x0C:   // killed or squeezed
        repeat 5: SpawnExplosionRand(self, 1, 16, 16)
        SpawnJunk(3, {8, 9, 10}, self)                   // anims at 0x800FAD00
        DropItem(self, 20)
        self.state = 2
        return
    ContactMega(self)
    if OffScreenCheck2(self, 128, 128): self.state = 3
    else: DisplayCheck(self, 96, 96)
    if self.collide_flags & 8: self.latch = 0
    else: self.latch = -1 if self.speed_y < 0 else 1
```

### `wheel_fall_check` — `0x800EBF28`

```
    if self.latch == 0 and not (self.collide_flags & 8):
        self.step = 2 ; self.step2 = 0 ; self.speed_y = 0 ; self.latch = -1
```

### `wheel_roll` — `0x800EBCD8` (step 1)

```
wheel_roll(self):
    move(self)                       // x += speed_x; y -= speed_y; speed_x ±= accel; speed_y -= gravity; clamp fall_max
    AnimeTick(self)
    wall = self.collide_flags & (1 if self.dir == 0 else 2)
    if wall:
        PlaySound(2, 4, self)
        self.x.frac(+0x08, 16 bit) = 0
        self.speed_x = 0 ; self.accel_x = 0
        self.step = 3 ; self.step2 = 0
```

`move` is inlined in ROLL and FALL: `speed_x += accel_x` when `dir != 0`, else `speed_x -= accel_x`.
With `accel_x = -0xC00` that is −0.047 px/frame² for `dir = 0x40` (left) and +0.047 for `dir = 0`.

### `wheel_fall` — `0x800EBDC8` (step 2)

```
wheel_fall(self):
    AnimeTick(self)
    if self.collide_flags & 8:
        PlaySound(2, 4, self)
        self.step = 1 ; self.step2 = 0 ; self.accel_x = -0xC00 ; self.speed_y = 0 ; self.latch = 0
    else:
        move(self)
```

### `wheel_turn` — `0x800EBEA0` (step 3) and its sub-steps

```
wheel_turn(self): TURN_TABLE[self.step2](self)

turn_start (0x800EBEDC):  self.timer = 90 ; self.step2 += 1
turn_wait  (0x800EBEF4):  self.timer -= 1 ; if self.timer == 0: self.timer = 20 ; self.step2 += 1
turn_flip  (0x800EB78C):
    self.timer -= 1
    if self.timer != 0: return
    self.step = 2 ; self.step2 = 0
    if self.dir == 0:
        self.accel_x = 0xC00 ; SetAnime(self, 0) ; self.dir = 0x40
        ticks = [4, 8, 12, 16, 20, 24, 28, 0][self.last_sprite]     // last_sprite >= 8: ticks keeps the caller's s0 (cannot happen)
    else:
        self.accel_x = -0xC00 ; SetAnime(self, 2) ; self.dir = 0
        ticks = [0, 28, 24, 20, 16, 12, 8, 4][self.last_sprite]     // as above
    repeat ticks: AnimeTick(self)          // start the new roll next to the current picture
```

The `accel_x` set here is replaced by `wheel_fall` on the next update when the wheel is on the floor.

### `wheel_pair_test` — `0x800EBF68`

```
for o in main slots after self, up to 0x80090C88:
    if o.flags and o.type == self.type: wheel_touch(o, self)
```

### `wheel_touch(a, b)` — `0x800EB924`

```
if not a.terrain_box or not b.terrain_box: return
if |a.x.px - b.x.px| >= a.tb.w + b.tb.w: return
if |a.y.px - b.y.px| >= a.tb.h + b.tb.h: return
if b.latch != 0: (if a.latch == 0: a.collide_flags |= 4)
elif a.latch != 0: b.collide_flags |= 4
if b.x.px < a.x.px: b.collide_flags |= 1 ; a.collide_flags |= 2
else:               b.collide_flags |= 2 ; a.collide_flags |= 1
```

## Stud overlay (misc type 2)

### `studs_update` — `0x800EBFEC`

```
state 0: studs_init ; state 1: studs_follow ; else: studs_flash
```

### `studs_init` — `0x800EC044`

```
self.display = 1 ; self.priority = 5 ; self.speed_y = 0 ; self.gravity = 0 ; self.state += 1
DisplayCheck1(self)
```

### `studs_follow` — `0x800EBA8C` (state 1)

```
w = self.parent
self.anime.sprite = STUDS[w.anime.sprite * 4 - 1 + w.anime.duration]     // table 0x800FAD30
self.x = w.x ; self.y = w.y
if w.step == 3:
    self.anime.sprite = STUDS[w.anime.sprite * 4 + 3]
    if w.step2 == 2:
        SetAnime(self, 4 + (w.anime.sprite & 3))      // sprites 0/4 → 4, 1/5 → 5, 2/6 → 6, 3/7 → 7
        self.state += 1
if OffScreenCheck2(self, 128, 128): DeleteObject3(self)
else: DisplayCheck(self, 96, 96)
if w.state == 2: DeleteObject3(self)
```

### `studs_flash` — `0x800EC084` (state 2)

```
w = self.parent
if w.step != 3: self.state = 1
AnimeTick(self)
off-screen and parent-death deletes as in studs_follow
```

## Engine helpers called

See `kit/summary.md` (17 helpers, all named in `catalog/helpers.json`). New facts from this enemy:
none; `SpawnJunk`'s second argument is a list of animation numbers (here 8, 9, 10).

## Object struct (fields specific to this enemy)

| off | name | meaning | confidence |
|---|---|---|---|
| `+0x07` | `dir` | 0 = rolls right, `0x40` = rolls left | certain |
| `+0x67` | `latch` | 0 on the floor; −1 rising or just left the floor; 1 falling | certain |
| `+0x7C` | `timer` (u16) | TURN countdown | certain |
| `+0x80` | `last_sprite` (u8) | sprite at the start of the update | certain |
| misc `+0x50` | `parent` | the wheel | certain |
