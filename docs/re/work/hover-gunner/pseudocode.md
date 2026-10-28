# Small jet, its shot and its muzzle flash: pseudo code

All addresses are RAM addresses in `SLUS-01395` with overlay `02` loaded at `0x800E9860`. Field names
follow `../junkroid/pseudocode.md` (struct table there). `FX` = 16.16 fixed point.
`self.wNN` = object byte at offset `0xNN` in the per-type work area (`+0x7C` and up).

The pointer at scratchpad `0x1F80004C` is the object loop's "current object" (set at `0x80020048`
while the loop walks the main objects from `0x8008EF48`). The jet code reads its own work bytes
through it; below this is written as `self`.

## Dispatch tables

| address | contents |
|---|---|
| `0x800FA424` + 5·4 | main table entry 5 = `jet_update` `0x800ECF3C` |
| `0x800FA558` + 4·4 | visual table entry 4 = `jet_shot_update` `0x800EDB48` |
| `0x800FA814` + 6·4 | misc table entry 6 = `jet_flash_update` `0x800EDC94` (state table `0x800FAEFC`) |
| `0x800FAE34` | `jet_state[5]` = init `0x800EC11C`, main `0x800ECF78`, delete `0x800ED09C`, wait_left `0x800ED174`, delete |
| `0x800FAE48` | `jet_step[8]` = engine `0x80040290`, nop `0x800ED1B4`, DRIFT `0x800ED1BC`, FLY `0x800ED2B8`, BURST `0x800ED3D4`, FAN `0x800ED590`, LEAVE `0x800ED844`, BOB `0x800ED8EC` |
| `0x800FAE68` | DRIFT sub-steps: `0x800ED1F8`, `0x800ED25C` |
| `0x800FAE70` | FLY sub-steps: `0x800ED2F4`, `0x800ED370` |
| `0x800FAE78` | BURST sub-steps: `0x800ED420`, `0x800EC2B8`, `0x800ED470`, `0x800EC484`, `0x800ED498`, `0x800ED520` |
| `0x800FAE90` | FAN sub-steps: `0x800ED5DC`, `0x800EC640`, `0x800ED67C`, `0x800ED7B8`, `0x800ED818`, `0x800ED82C` |
| `0x800FAEA8` | LEAVE sub-steps: `0x800ED880`, `0x800ED8C0` |
| `0x800FAEB0` | BOB sub-steps: `0x800ED980`, `0x800EDA04`, `0x800EDA7C`, `0x800EDAD4` |
| `0x800FAEDC` | `shot_state[4]` = init `0x800ECA60`, main `0x800EDBB4`, delete `0x800EDC6C`, nop |
| `0x800FAEFC` | `flash_state[3]` = init `0x800ECCF0`, main `0x800EDCE0`, delete `0x800EDD40` |
| `0x800FADC4` | `jet_animations[13]` |
| `0x800FADF8` | jet hurt box `{-9, -9, 23, 18}` |
| `0x800FAE00` | jet contact box `{0, 0, 11, 11}` |
| `0x800FAE04` | `range_table[4]` (s16) = 64, 80, 96, 112 |
| `0x800FAE0C` | debris animations `{8, 9, 10, 11, 12}` |
| `0x800FAE14` | 32-byte direction remap, id 3 only |
| `0x800FAEC0` | shot hurt/contact box `{-3, -6, 8, 8}` |
| `0x800FAEC8` | shot terrain box `{0, 0, 4, 4}` |
| `0x800FAECC`, `0x800FAEEC` | muzzle offsets (shot, flash), 8 pairs: (−13,0) (13,0) (−10,−12) (10,−12) (0,−16) (0,16) (−10,12) (10,12) |
| `0x800E98A4` | jump table: direction → pose (BURST aim) |
| `0x800E9924` | the same for FAN |
| `0x800E99A4` | jump table: aim direction → side directions A, B |
| `0x800E9A24` | jump table: shot number → next FAN direction |
| `0x800E9A44`, `0x800E9ABC` | jump tables: direction → muzzle offset (shot, flash) |

## Work bytes

| off | name | meaning |
|---|---|---|
| `+0x7C` | `bob_wait` (s16) | BOB: pause between moves; on the shot: parent pointer (u32 over `+0x7C..0x7F`) |
| `+0x7E` | `bob_time` (s16) | BOB: move time |
| `+0x80` | `timer` | general countdown |
| `+0x81` | `range_no` | 0..3, index into `range_table` |
| `+0x82` | `shots` | shots fired in this burst / fan |
| `+0x83` | `bursts` | bursts done |
| `+0x85` | `dir` | aim direction 0..31 (only even values used) |
| `+0x86`, `+0x87`, `+0x88` | `dir_a`, `dir_b`, `dir_c` | FAN: side A, side B, aim |
| `+0x89` | `turret_wait` | id 3: frames to the next attack |
| `+0x8A`..`+0x97` | saved | id 3: saved sub-step, `bob_wait`, y, speed_y, gravity |
| `+0x67` | `lock` | 0 = range trigger on; 1 = DRIFT; −1 = trigger off (LEAVE) |

Directions: 0 = right, 8 = up, 16 = left, 24 = down (counter-clockwise, 11.25° per unit).

## Engine helpers used (new here)

| address | name | what it does |
|---|---|---|
| `0x8002CAB4` | `OffScreenCheck` | 1 if outside `[-64, 384) × [-64, 304)` relative to the layer camera |
| `0x8002CB50` | `DisplayCheck1` | `display = 1` if inside `[-32, W+32) × [-32, 272)` |
| `0x8002D380` | `AimAt(a, b)` | `Dir32(a.x − b.x, a.y − b.y)`: direction from a to b |
| `0x8002D0D8` | `Dir32(dx, dy)` | 32-way direction by a ratio table at `0x800737A8` |
| `0x8002D3B4` | `SetSpeedDir(obj, dir)` | `speed_x = cos_table[dir]`, `speed_y = sin_table[dir]`, 1 px/frame (tables at `0x80073760`, `0x80073784`) |

## Jet

```
jet_update(self):                                   // 0x800ECF3C
    jet_state[self.state](self)                     // no CollideCheck: the jet ignores terrain

jet_init(self):                                     // 0x800EC11C, state 0
    self.layer = player.layer
    self.anime_table = jet_animations
    self.priority = 4
    self.attack_box = {0, 0, 11, 11}
    self.hurt_box = {-9, -9, 23, 18}
    self.damage_table = 0x800743DC                  // same as the green half-robot
    self.speed_x = self.speed_y = self.accel_x = self.gravity = 0
    self.lock = 0 ; self.terrain_box = NULL ; self.iframes = 0
    self.hp = 5 ; self.contact_damage = 3
    self.state = MAIN ; self.step2 = 0
    self.range_no = GetRNG() & 3
    self.turret_wait = 0
    if self.var & 0x80:                             // made by another object
        SetAnime(self, 0) ; self.step = DRIFT ; self.lock = 1
    else:
        SetAnime(self, 0) ; self.flip = LEFT ; self.lock = 0 ; self.step = FLY
    if (self.var & 0x0F) == 1:
        self.state = WAIT_LEFT
    if self.var == 3:
        SetAnime(self, 1)
        save step2, bob_wait, y, speed_y, gravity to +0x8A..
        self.step = BOB

jet_wait_left(self):                                // 0x800ED174, state 3 (id 1)
    if player.x.hi - self.x.hi >= 176:
        self.flip = RIGHT
        self.state = MAIN ; self.step = FLY

jet_main(self):                                     // 0x800ECF78, state 1
    self.prev = self.pos
    if ContactWeapon(self) < 0:
        SpawnExplosion(self)
        DropItem(self, 18)
        SpawnJunk(5, {8, 9, 10, 11, 12}, self)
        self.state = KILLED (2) ; return
    if self.var == 3:
        self.flip = (player.x < self.x) ? LEFT : RIGHT
    jet_range_trigger(self)                         // 0x800EC784
    if self.turret_wait < 60:
        jet_step[self.step](self)
    else:
        AnimeTick(self)
    ContactMega(self)
    if OffScreenCheck(self):                        // 64 px margin
        self.state = OUT (4)
    else:
        DisplayCheck1(self)

jet_range_trigger(self):                            // 0x800EC784
    if self.lock != 0 or self.step in (BURST, FAN): return
    range = range_table[self.range_no]
    if self.var == 3:
        if self.turret_wait == 0: self.step = BURST ; self.step2 = 0
        else: self.turret_wait -= 1
        return
    if abs(player.x.hi - self.x.hi) <= range:
        self.step = BURST ; self.step2 = 0

jet_delete(self):                                   // 0x800ED09C, states 2 and 4
    clear work bytes +0x80..+0x88
    if self.var & 0x80: DeleteObject3(self)
    elif self.state == KILLED: DeleteObject(self)
    else: DeleteObject2(self)
```

### DRIFT (bit 7 only)

```
drift_start(self):          // 0x800ED1F8
    self.lock = 1 ; self.step2 += 1
    self.speed_x = self.flip ? +0x8000 : -0x8000    // 0.5 px/frame
    other speeds = 0 ; self.timer = 48 ; SetAnime(self, 0)

drift_move(self):           // 0x800ED25C
    AnimeTick(self) ; MoveObject(self)
    if --self.timer == 0: self.step = FLY ; self.step2 = 0
```

### FLY

```
fly_start(self):            // 0x800ED2F4
    self.lock = 0 ; self.step2 += 1
    if (self.var & 0x0F) == 2: self.speed_x = 0
    else: self.speed_x = self.flip ? +0x40000 : -0x40000    // 4 px/frame
    other speeds = 0 ; SetAnime(self, 0) ; self.timer = 60

fly_move(self):             // 0x800ED370
    AnimeTick(self) ; MoveObject(self)
    if --self.timer == 0:
        self.lock = -1 ; self.step = LEAVE ; self.step2 = 0
```

### BURST

```
burst(self):                // 0x800ED3D4
    burst_sub[self.step2](self)
    AnimeTick(self)

burst_stop(self):           // 0x800ED420, sub 0
    all speeds = 0 ; self.step2 += 1
    self.timer = (self.var == 3) ? 1 : 20

burst_aim(self):            // 0x800EC2B8, sub 1
    if --self.timer != 0: return
    if player.x.hi - self.x.hi > 0: self.flip = RIGHT
    self.dir = AimAt(self, player)    // direction from the jet to the player
    if self.var == 3: self.dir = remap[self.dir]   // table 0x800FAE14
    jet_pose(self, self.dir & 0xFE)                 // see below
    self.step2 += 1

burst_wait_frame(self):     // 0x800ED470, sub 2
    if self.anime.flag != 0: self.step2 += 1

burst_fire(self):           // 0x800EC484, sub 3
    jet_fire(self)
    if self.var == 3:
        self.turret_wait = 90 ; self.step = BOB
        restore step2, bob_wait, y, speed_y, gravity from +0x8A..

burst_gap(self):            // 0x800ED498, sub 4
    if self.shots >= 3:
        self.step2 += 1 ; self.shots = 0 ; self.timer = 20
    elif --self.timer == 0:
        self.step2 -= 1                             // fire again

burst_end(self):            // 0x800ED520, sub 5
    if self.timer != 0: self.timer -= 1 ; return
    self.bursts += 1
    if self.bursts >= 2: self.step = FAN ; self.step2 = 0 ; self.bursts = 0
    else:                self.step = FLY ; self.step2 = 0

jet_fire(self):             // body of 0x800EC484 and 0x800ED67C
    shot = GetVisualObject()
    if shot:
        PlaySound(2, 0, self)
        shot.flags = 0x41 ; shot.type = 4
        shot.pos = self.pos ; shot.parent(+0x7C) = self ; shot.w84 = 0
        shot.var = self.dir
        SetSpeedDir(shot, self.dir & 0xFE)          // 1 px/frame; the shot doubles it
        self.timer = 8 ; self.step2 += 1 ; self.shots += 1
    flash = GetMiscObject()
    if flash:
        flash.flags = 0x41 ; flash.type = 6
        flash.pos = self.pos ; flash.parent(+0x50) = self ; flash.var = self.dir

jet_pose(self, d):          // jump table 0x800E98A4 (BURST) / 0x800E9924 (FAN)
    d = 0:        SetAnime(1) ; flip = RIGHT
    d = 2..6:     SetAnime(4) ; flip = RIGHT
    d = 8:        SetAnime(2)
    d = 10..14:   SetAnime(4) ; flip = LEFT
    d = 16:       SetAnime(1) ; flip = LEFT
    d = 18..22:   SetAnime(5) ; flip = LEFT
    d = 24:       SetAnime(3)
    d = 26..30:   SetAnime(5) ; flip = RIGHT
```

Note: if no visual slot is free, `jet_fire` does not advance `step2`, so the jet tries again next
frame.

### FAN

```
fan(self):                  // 0x800ED590
    fan_sub[self.step2](self)
    AnimeTick(self)

fan_start(self):            // 0x800ED5DC, sub 0
    all speeds = 0 ; self.step2 += 1 ; SetAnime(self, 1)
    self.timer = 20 ; self.shots = 0
    if player.x.hi - self.x.hi > 0: self.flip = RIGHT
    self.dir = AimAt(self, player)
    fan_sides(self)                                 // 0x800EC848

fan_sides(self):            // 0x800EC848, table 0x800E99A4
    self.dir_c = self.dir & 0xFE
    (dir_a, dir_b) by dir_c:
        0 → (30, 2)    2..6 → (0, 8)    8 → (6, 10)    10..14 → (8, 16)
        16 → (14, 18)  18..22 → (16, 24)  24 → (22, 26)  26..30 → (24, 0)

fan_pose(self):             // 0x800EC640, sub 1
    if --self.timer != 0: return
    jet_pose(self, self.dir & 0xFE) ; self.step2 += 1

fan_fire(self):             // 0x800ED67C, sub 2
    if self.anime.flag == 0: return
    jet_fire(self)                                  // sets timer = 8, shots += 1

fan_next(self):             // 0x800ED7B8, sub 3
    if self.shots < 9:
        self.dir = [dir_a, dir_c, dir_b, dir_c, dir_a, dir_c, dir_b, dir_c][self.shots - 1]
        self.step2 -= 2                             // back to fan_pose
    else:
        self.step2 += 1

fan_sub4(self):             // 0x800ED818
    self.step2 += 1

fan_end(self):              // 0x800ED82C, sub 5
    self.lock = -1 ; self.step = LEAVE ; self.step2 = 0
```

### LEAVE

```
leave_start(self):          // 0x800ED880
    speed_x = 0 ; speed_y = +0x20000 (2 px/frame up) ; SetAnime(self, 1) ; self.step2 += 1

leave_move(self):           // 0x800ED8C0
    AnimeTick(self) ; MoveObject(self)
```

### BOB (id 3 only, outline)

```
bob(self):                  // 0x800ED8EC: run sub-step, then save step2, bob_wait, y, speed_y, gravity
bob_start:   // 0x800ED980: face the player, SetAnime 1, bob_time = 32, bob_wait = 40, speed_y = -2 px/frame
bob_move:    // 0x800EDA04: wait bob_wait frames, then move for bob_time frames, then reverse speed_y
bob_sub2:    // 0x800EDA7C: MoveObject2 until speed_y = 0, then gravity = -0x1000 (pull up)
bob_sub3:    // 0x800EDAD4: wait, MoveObject2; above 1 px/frame up set gravity = +0x1000; at speed_y = 0 restart
```

## Shot (visual object type 4)

```
jet_shot_update(self):      // 0x800EDB48
    if self.w84 == 0 and self.collide_flags == 0: CornerCheck(self)
    shot_state[self.state](self)

shot_init(self):            // 0x800ECA60
    self.terrain_box = {0, 0, 4, 4}
    self.hurt_box = self.attack_box = {-3, -6, 8, 8}
    self.display = 1 ; self.priority = 1
    self.accel_x = self.gravity = 0 ; self.lock = 0
    self.damage_table = 0x800741FC                  // can be shot down
    copy tpage, clut (without flash bit), anime_table, sprite data, flip, layer from the parent
    self.contact_damage = 3 ; self.hp = 1
    self.speed_x *= 2 ; self.speed_y *= 2           // 2 px/frame
    self.pos += muzzle_offset[self.var & 0xFE]      // jump table 0x800E9A44, data 0x800FAECC
    SetAnime(self, 6)
    self.state += 1

shot_main(self):            // 0x800EDBB4
    AnimeTick(self) ; MoveObject(self)
    if ContactMega(self) != 0:              goto done
    if ContactWeapon(self) < 0:             SpawnExplosion(self) ; goto done
    if BoxOverlap(self, player):            goto done
    if self.collide_flags != 0:             SpawnExplosion(self) ; goto done
    if OffScreenCheck2(self, 32, 32):       goto done
    DisplayCheck(self, 16, 16) ; return
  done:
    self.state += 1

shot_delete(self):          // 0x800EDC6C
    DeleteObject3(self)
```

`muzzle_offset[d]`: d = 2..6 → (10, −12); 8 → (0, −16); 10..14 → (−10, −12); 18..22 → (−10, 12);
24 → (0, 16); 26..30 → (10, 12); any other d (0, 16) → (13, 0) if facing right, else (−13, 0).

## Muzzle flash (misc object type 6)

```
flash_init(self):           // 0x800ECCF0
    self.display = 1 ; all speeds = 0 ; self.priority = 0
    copy tpage, clut, anime_table, sprite data, flip, layer from the parent (+0x50)
    SetAnime(self, 7)
    self.pos += muzzle_offset[self.var & 0xFE]      // jump table 0x800E9ABC, data 0x800FAEEC
    self.state += 1

flash_main(self):           // 0x800EDCE0
    AnimeTick(self)
    if OffScreenCheck(self): self.state += 1 ; return
    DisplayCheck(self, 48, 16)
    if self.anime.next == 0: self.state += 1        // last frame reached

flash_delete(self):         // 0x800EDD40
    DeleteObject3(self)
```

## Animations (`0x800FADC4`)

Each frame is `(duration, flag, next, sprite)`.

| no | use | frames |
|---|---|---|
| 0 | fly | (1,0,+1,0) (1,0,−1,1) |
| 1 | pose: horizontal | (1,0,+1,2) (1,1,−1,3) |
| 2 | pose: up | (1,0,+1,10) (1,1,−1,11) |
| 3 | pose: down | (1,0,+1,14) (1,1,−1,15) |
| 4 | pose: diagonal up | (1,0,+1,4) (1,1,−1,5) |
| 5 | pose: diagonal down | (1,0,+1,12) (1,1,−1,13) |
| 6 | shot | (2,0,+1,16) (2,0,+1,17) (2,0,+1,18) (2,0,−3,19) |
| 7 | muzzle flash | (1,0,+1,20) (1,0,+1,21) (1,0,+1,22) (2,0,+1,23) (3,0,+1,24) (4,0,+1,25) (5,0,0,26) |
| 8–12 | debris | one frame each, sprites 27–31 |

The flag 1 on the second frame of poses 1–5 is the "fire" event that `burst_wait_frame` and
`fan_fire` wait for. The pose names "up" and "down" follow the direction cases (8 = up, 24 = down);
they are not checked against the sprites.
