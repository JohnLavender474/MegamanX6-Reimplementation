# Intro robot and its shot: pseudo code

All addresses are RAM addresses in `SLUS-01395` with overlay `02` loaded at `0x800E9860`. The field names
are mine (see the struct table at the end). `FX` = 16.16 fixed point; `x.hi` = the integer pixel half.

## Dispatch tables (overlay data)

| address | contents |
|---|---|
| `0x800FA424` | intro main-object update table; entry 1 = `robot_update` `0x800EAFB0` |
| `0x800FA558` | intro visual-object update table; entry 0 = `robot_shot_update` `0x800EB414` |
| `0x800FAB48` | `robot_state_funcs[4]` = init, main, despawn, despawn |
| `0x800FAB58` | `robot_step_funcs[4]` = `0x80040290` (engine), idle, shoot, fall |
| `0x800FAB70` | `robot_shot_state_funcs[3]` = init, main, delete |
| `0x800FAB24` | `robot_animations[8]` (pointers to frame lists `0x800FAAE8`…`0x800FAB18`) |
| `0x800FAADC` | robot terrain box `{0, 16, 16, 8}` |
| `0x800FAAE0`, `0x800FAAE4` | robot attack box and hurt box, both `{-12, -20, 24, 40}` |
| `0x800FAB44` | debris animation list `{2, 3, 4, 5}` |
| `0x800FAB68` | shot attack/hurt box `{-4, -4, 8, 8}` |
| `0x800FAB6C` | shot terrain box `{0, 0, 6, 6}` |

## Robot

### `robot_update` — `0x800EAFB0`

```
robot_update(self):
    robot_state_funcs[self.state](self)
    CollideCheck(self)                       // 0x8002F288: terrain push-out, sets self.collide_flags
```

### `robot_init` — `0x800EAFFC` (state 0)

```
robot_init(self):
    self.hp = 12
    self.damage_table = 0x800743DC
    self.contact_damage = 4
    self.player_iframes_field_61 = 0
    self.layer = player.layer
    self.prev_x = self.x ; self.prev_y = self.y
    self.speed_x = self.speed_y = self.accel_x = self.gravity = 0
    self.fall_latch = 0
    self.collide_flags = 0
    self.flip = (player.x < self.x) ? FLIP_LEFT(0) : FLIP_RIGHT(0x40)
    self.anime_table = robot_animations
    self.priority = 6
    self.terrain_box = 0x800FAADC
    self.hurt_box    = 0x800FAAE4            // +0x54
    self.attack_box  = 0x800FAAE0            // +0x50
    SetAnime(self, 0)                        // 0x800179A4
    self.step = STEP_IDLE (1)
    self.shot_timer = 1                      // +0x7C
    self.state = STATE_MAIN (1)
    self.step2 = 0
    if difficulty >= 2:                      // byte at 0x800CCF38
        self.var = 1                         // every robot is active on Hard
```

### `robot_main` — `0x800EAC8C` (state 1)

```
robot_main(self):
    self.prev_x = self.x ; self.prev_y = self.y
    robot_step_funcs[self.step](self)

    if self.var != 0 and self.anime.sprite == 0:
        self.flip = (player.x < self.x) ? 0 : 0x40

    robot_check_fall(self)                   // 0x800EB1F4

    if ContactWeapon(self) < 0:              // 0x80031474, negative = killed
        DropItem(self, 21)                   // 0x8004E654
        SpawnExplosion(self)                 // 0x80049E50
        SpawnJunk(4, robot_debris_anims, self)   // 0x800477E4
        DropItem(self, 14)
        self.state = STATE_KILLED (2)
        return

    f = self.collide_flags
    if (f & 0x0C) == 0x0C or (f & 0x03) == 0x03:     // crushed
        DropItem(self, 21)
        SpawnExplosion(self)
        SpawnJunk(4, robot_debris_anims, self)
        self.state = STATE_KILLED (2)
        return

    ContactMega(self)                        // 0x80030ECC, result not used
    if OffScreenCheck2(self, 64, 64):        // 0x8002CBFC
        self.state = STATE_OFFSCREEN (3)
    else:
        DisplayCheck(self, 32, 32)           // 0x8002CCB0, sets self.display
```

### `robot_despawn` — `0x800EB10C` (states 2 and 3)

```
robot_despawn(self):
    if self.state == STATE_KILLED:
        DeleteObject(self)                   // 0x8002CA14: clear spawn bit of the record, clear header
    else:
        DeleteObject2(self)                  // 0x8002CA54: same, and record kind 0x1? -> 0x0?
```

### `robot_idle` — `0x800EB14C` (step 1)

```
robot_idle(self):
    if self.var == 0: return                 // dormant variant
    if abs(self.x.hi - player.x.hi) < 112:
        self.step = STEP_SHOOT (2)
```

### `robot_shoot` — `0x800EAE0C` (step 2)

```
robot_shoot(self):
    if abs(self.x.hi - player.x.hi) > 112:   // code: not (< 113)
        SetAnime(self, 0)
        self.shot_timer = 120
        self.step = STEP_IDLE
        return                               // no animation tick on this frame

    if self.shot_timer != 0:
        self.shot_timer -= 1
        if self.shot_timer == 0:
            SetAnime(self, 1)

    if self.anime.flag == 1:                 // fire frame
        if difficulty >= 2: PlaySound(2, 33, self)
        else:               PlaySound(2, 26, self)
        shot = GetVisualObject()             // 0x8002C458, NULL if all 32 slots are in use
        if shot:
            shot.flags = 0x41 ; shot.type = 0 ; shot.var = 0
            shot.tex_cord  = self.tex_cord
            shot.clut_cord = self.clut_cord & 0x7FFF
            shot.anime_table = self.anime_table
            shot.sprite_data = self.sprite_data
            shot.flip = self.flip
            shot.x = self.x ; shot.y = self.y
            shot.state = 0

    if self.anime.flag == 2:                 // last frame of the shoot animation
        SetAnime(self, 0)
        self.shot_timer = 120
        self.step = STEP_IDLE
        self.step2 = 0

    AnimeTick(self)                          // 0x80017A04
```

### `robot_fall` — `0x800EB1A0` (step 3)

```
robot_fall(self):
    if self.collide_flags & 0x08:            // ground
        self.step = STEP_IDLE ; self.step2 = 0
        self.speed_x = self.accel_x = self.speed_y = self.gravity = 0
        self.fall_latch = 0
    else:
        MoveObject2(self)                    // 0x8002D230
```

### `robot_check_fall` — `0x800EB1F4`

```
robot_check_fall(self):
    if (self.collide_flags & 0x08) == 0 and self.fall_latch == 0:
        self.step = STEP_FALL (3)
        self.gravity = 0x4200
        self.speed_y = 0 ; self.accel_x = 0
        self.fall_latch = 1
```

## Shot (visual object type 0)

### `robot_shot_update` — `0x800EB414`

```
robot_shot_update(self):
    robot_shot_state_funcs[self.state](self)
```

### `robot_shot_init` — `0x800EB23C` (state 0)

```
robot_shot_init(self):
    self.display = 1
    self.speed_x = (self.flip != 0) ? +0x20000 : -0x20000    // 2 px/frame
    self.accel_x = self.speed_y = self.gravity = 0
    self.x.hi += (self.flip != 0) ? +16 : -16
    self.priority = 4
    self.terrain_box = 0x800FAB6C
    self.hurt_box = self.attack_box = 0x800FAB68
    self.y.hi -= 12
    self.hp = 1 ; self.contact_damage = 1 ; self.field_61 = 0
    self.clut_cord &= 0x7FFF
    if difficulty >= 2:
        self.damage_table = 0x8007433C ; SetAnime(self, 7)
    else:
        self.damage_table = 0x800741FC ; SetAnime(self, 6)
    self.state += 1
```

### `robot_shot_main` — `0x800EB340` (state 1)

```
robot_shot_main(self):
    AnimeTick(self)
    MoveObject(self)                         // 0x8002D2B0: x += vx ; y -= vy
    if ContactMega(self) != 0           -> burst
    if ContactWeapon(self) < 0          -> burst
    if self.collide_flags != 0          -> burst
    CornerCheck(self)                        // 0x8002EE10
    if self.collide_flags != 0          -> burst
    if OffScreenCheck2(self, 32, 32):
        self.state = 2 ; return
    DisplayCheck(self, 16, 16)
    return
burst:
    SpawnExplosion(self)
    self.state = 2
```

### `robot_shot_delete` — `0x800EB3F4` (state 2)

```
robot_shot_delete(self):
    DeleteObject3(self)                      // 0x8002C9B0: clear bytes 0..7
```

## Engine helpers called

| address | name (mine or from the X6 modding headers) | what it does | reversed |
|---|---|---|---|
| `0x800179A4` | `SetAnime(obj, n)` | `frame = anime_table[n]`; `anime = n`; `new_anime = 0xFF`; load the 4-byte frame (duration, flag, next, sprite) to `+0x44` | full |
| `0x80017A04` | `AnimeTick(obj)` | duration −1; at 0, `frame += next * 4` (relative, signed) and load it | full |
| `0x8002D2B0` | `MoveObject` | `x += vx; y -= vy` | full |
| `0x8002D230` | `MoveObject2` | `x += vx; y -= vy; vx += flip ? ax : -ax; vy -= gravity`; clamp `vy` to the global at `0x8009719C` (−6.5 px/f) | full |
| `0x8002F288` | `CollideCheck` | terrain collision from the move since `prev_x/prev_y`; sets `collide_flags` | outline |
| `0x8002EE10` | `CornerCheck` | if flags are 0: test the 4 corners of the terrain box against the tile map; corner bits 6, 10, 5, 9 | full |
| `0x800304D8` | (terrain box set-up) | `ref = (x ± box.ox, y + box.oy)`, half sizes `box.w`, `box.h` | full |
| `0x80031474` | `ContactWeapon` | for each of 32 weapon objects at `0x800950A0`: box test, then `0x800315CC`; returns 0, weapon id + 1, 126 (already hit), 127 (blocked) or < 0 (killed) | full |
| `0x800315CC` | (apply weapon hit) | damage table lookup, part and difficulty modifiers, `hp -= dmg`, flash bit, kill result | full |
| `0x800319EC` | (weapon hit reaction) | sound and spark for the player's shot; mode −1 = deflect | outline |
| `0x80030ECC` | `ContactMega` | attack box vs the player; applies `contact_damage` through `0x80030DC0` | outline |
| `0x80030DC0` | (player damage scale) | Easy × 3/4, Hard × 5/4, halve for armour / Shock Buffer, minimum 1 | full |
| `0x8002D654` | (box overlap) | box test between two objects, facing-aware | full |
| `0x8002CBFC` | `OffScreenCheck2(obj, rx, ry)` | 1 if outside `[-rx, 320+rx) × [-ry, 240+ry)` relative to the layer camera | full |
| `0x8002CCB0` | `DisplayCheck(obj, rx, ry)` | `display = 1` if inside `[-rx, W+rx) × [-ry, 240+ry)` (W from `0x8009B79C`) | full |
| `0x8002CA14` | `DeleteObject` | `record.flags &= 0x70`, then `DeleteObject3` | full |
| `0x8002CA54` | `DeleteObject2` | as above; also clears the record kind high nibble when it is `0x10` | full |
| `0x8002C9B0` | `DeleteObject3` | zero bytes 0..7 of the object | full |
| `0x8002C458` | `GetVisualObject` | first free slot of 32 × 0xA0 at `0x80093C98`, cleared | full |
| `0x8002C530` | `GetMiscObject` | free misc object (debris) | not reversed |
| `0x80016C48` | `PlaySound(bank, id, obj)` | sound effect request | outline |
| `0x8004E654` / `0x8004E680` | `DropItem(obj, table)` | difficulty < 2 only; weighted roll on `0x80078860 + table*6`; item object type 47, kind from `0x800788F0` | full |
| `0x80049E50` / `0x80049E70` | `SpawnExplosion(obj)` | effect type 0 at the object position, random variant | outline |
| `0x800477E4` | `SpawnJunk(n, anims, obj)` | `n` misc objects type 3, random facing, +0..3 px offset | full |
| `0x8002D2D4` | `GetRNG` | random number | not reversed |
| `0x80040290` | (hit-reaction dispatcher) | step 0 entry; jumps by `+0x63` through `0x80011574` | outline |

## Object struct (main and visual objects, the fields used here)

Base layout follows the X6 modding headers; names and the meaning of fields that those headers leave
unnamed are mine.

| off | size | name | meaning | confidence |
|---|---|---|---|---|
| 0x00 | 1 | `flags` | slot in use; 0x41 for spawned objects | certain |
| 0x01 | 1 | `type` | index in the update table (robot 1, shot 0) | certain |
| 0x02 | 1 | `var` | record `id`; 0 = dormant robot | certain |
| 0x03 | 1 | `display` | draw this frame | certain |
| 0x04 | 1 | `state` | top-level state | certain |
| 0x05 | 1 | `step` | sub-state | certain |
| 0x06 | 1 | `step2` | sub-sub-state (only cleared) | certain |
| 0x08 | 4 | `x` | FX position, `+0x0A` = pixels | certain |
| 0x0C | 4 | `y` | FX position, `+0x0E` = pixels | certain |
| 0x10 | 4 | `record` | pointer to the stage list record | certain |
| 0x14 | 1 | `layer` | BG layer / camera index | certain |
| 0x15 | 1 | `flip` | 0 = left, 0x40 = right | certain |
| 0x16 | 1 | `priority` | draw priority | likely |
| 0x17 | 1 | `anime` | current animation id | certain |
| 0x18 | 4 | `prev_x` | position before this frame | certain |
| 0x1C | 4 | `prev_y` | | certain |
| 0x20 | 4 | `speed_x` | FX, px/frame | certain |
| 0x24 | 4 | `speed_y` | FX, positive = up | certain |
| 0x28 | 4 | `accel_x` | FX, applied in the facing direction | certain |
| 0x2C | 4 | `gravity` | FX, subtracted from `speed_y` | certain |
| 0x30 | 4 | `anime_table` | animation pointer list | certain |
| 0x34 | 4 | `anime_frame` | current frame pointer | certain |
| 0x3C | 4 | `sprite_data` | sprite data pointer | likely |
| 0x40 | 2 | `tex_cord` | texture page | likely |
| 0x42 | 2 | `clut_cord` | palette; bit 15 = hit flash | certain |
| 0x44 | 1 | `anime.duration` | frames left in this frame | certain |
| 0x45 | 1 | `anime.flag` | event flag (1 = fire, 2 = end) | certain |
| 0x46 | 1 | `anime.next` | relative index of the next frame | certain |
| 0x47 | 1 | `anime.sprite` | sprite number | certain |
| 0x48 | 1 | `new_anime` | set to 0xFF by `SetAnime` | certain |
| 0x50 | 4 | `attack_box` | box used for contact with the player | likely |
| 0x54 | 4 | `hurt_box` | box used for weapon hits | likely |
| 0x58 | 4 | `damage_table` | 2 bytes per weapon id: class, damage | certain |
| 0x5C | 1 | `hp` | hit points | certain |
| 0x60 | 1 | `contact_damage` | damage to the player | certain |
| 0x61 | 1 | `iframes` | player-side invincibility counter; set to 0 here | likely |
| 0x63 | 1 | `hit_class` | reaction class of the last hit | certain |
| 0x65 | 1 | `last_hit_tag` | tag of the last weapon that hit | certain |
| 0x67 | 1 | `fall_latch` | set when the fall starts | certain |
| 0x68 | 4 | `terrain_box` | terrain box pointer | certain |
| 0x70 | 1 | `collide_flags` | 8 ground, 4 ceiling, 1/2 walls | likely |
| 0x7A | 1 | `no_hit` | non-zero: weapons do not hit | certain |
| 0x7C | 2 | `shot_timer` | robot only | certain |

Boxes are 4 bytes `{s8 ox, s8 oy, u8 w, u8 h}`. Hit boxes: corner at `(x ± ox, y + oy)`, size `w × h`.
Terrain boxes: centre at `(x ± ox, y + oy)`, half sizes `w`, `h`.
