# Teleporter crystal (main type 28): pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`) and `--at` reads of the engine calls.
Addresses are RAM addresses in overlay 03 (Yammark). Overlay 09 (Turtloid) has the same code at
`0x800EF6FC`–`0x800F0434`, with the same data. Names of engine helpers and fields are in
`catalog/helpers.json` of mmx-demake-tools; new names are proposed in `notes.md`.

## Dispatch tables (overlay data)

| table | index | routine |
|---|---|---|
| `0x800F77F8` state | 0 | `tc_init` `0x800EE5CC` |
| | 1 | `tc_main` `0x800EEF10` |
| | 2, 3 | `tc_delete` `0x800EEF8C` (`DeleteObject` / `DeleteObject2`; never reached) |
| `0x800F7808` step | 0 | `HitReactionDispatch` (engine; not used) |
| | 1 | `tc_touch` `0x800EEFCC` |
| | 2 | `tc_area` `0x800EF0CC` (sequence A, step2 table `0x800F7820`) |
| | 3 | `tc_move` `0x800EF108` (sequence B, step2 table `0x800F78BC`) |
| | 4 | `tc_line_delay` `0x800EF044` |
| | 5 | `tc_line` `0x800EF090` |
| `0x800F7820` step2 (A) | 0, 1, 2 | `area_start` `0x800EE70C`, `area_split` `0x800EF144`, `area_wait` `0x800EE800` |
| `0x800F78BC` step2 (B) | 0–5 | `move_start` `0x800EE8C4`, `move_split` `0x800EF1E8`, `move_jump` `0x800EE9C4`, `move_join` `0x800EF27C`, `move_end` `0x800EEDB8`, `move_kill` `0x800EF2E4` |
| `0x800E98F4` `var` switch of `tc_init` | 0, 1, 2 / 3 / 4 | `0x800EE69C`, `0x800EE6B0`, `0x800EE6C4` (all the same) / `0x800EE6D8` / `0x800EE6E0` |

## Data

- attack box `0x800F77B0` = (0, 0, 16, 24); hurt box `0x800F77B4` = (−18, −28, 28, 40); terrain box
  `0x800F77B8` = the same 4 bytes (−18, −28, 28, 40).
- animation table `0x800F77EC`: anim 0 `0x800F77BC`, anim 1 `0x800F77D4`.
- destination table `0x800F782C`, 4 entries of 32 bytes (all values are halfwords):

| entry | player x, y | layer 0 camera | layer 1 | layer 2 | x min, x max, y min, y max (layer 0) |
|---|---|---|---|---|---|
| 0 | 72, 1115 | 0, 960 | 1664, 0 | 0, 240 | 0, 368, 960, 960 |
| 1 | 120, 2074 | 0, 1904 | 0, 1904 | 0, 476 | 0, 6080, 1904, 1904 |
| 2 | 1952, 1115 | 1920, 960 | 1664, 0 | 480, 240 | 1920, 2608, 960, 960 |
| 3 | 1952, 1114 | 1920, 960 | 1664, 0 | 480, 240 | 1920, 2608, 960, 960 |

Bytes `+0x18`–`+0x1F` of each entry are 0 and not read.

## Functions

### tc_update — 0x800EEEC4 (main table entry 28)

```
call state_table[self.state](self)
CollideCheck(self)
```

### tc_init — 0x800EE5CC (state 0)

```
contact_damage = 4; hp = 32; iframes = 0; latch = 0; collide_flags = 0
layer = mega.layer; prev_x/prev_y = x/y
flip = (mega.x < self.x) ? 0 : 0x40
anime_table = 0x800F77EC; priority = 6
terrain_box = 0x800F77B8; hurt_box = 0x800F77B4; attack_box = 0x800F77B0
if var < 5:
    var 0, 1, 2: SetAnime(self, 0); step = 1
    var 3: display = 0; step = 4
    var 4: display = 0; step = 5
(var >= 5: step stays 0)
work.7C = 0; state = 1; step2 = 0
```

### tc_main — 0x800EEF10 (state 1)

```
prev_x/prev_y = x/y
AnimeTick(self)
call step_table[step](self)
if var is not 3 or 4: DisplayCheck(self, 64, 64)
```

No weapon test, no contact test, no off-screen test.

### tc_touch — 0x800EEFCC (step 1)

```
if BoxOverlap(self, mega):            // self attack box vs mega hurt box; 0 while mega.iframes != 0
    work.84 = mega.x.px
    step2 = 0
    if var == 2: work.7C = 0; step = 3      // sequence B
    else:        step = 2                   // sequence A
```

### tc_line_delay — 0x800EF044 (step 4, var 3)

```
if game[+0x26] == 0 and mega.x.px >= self.x.px:
    work.84 = mega.x.px; work.7C = 90; step = 3; step2 = 0
```

### tc_line — 0x800EF090 (step 5, var 4)

```
if mega.x.px >= self.x.px:
    work.84 = mega.x.px; work.7C = 0; step = 3; step2 = 0
```

### area_start — 0x800EE70C (A0)

```
if mega.hp == 0: return
camera layers 0, 1, 2: +0x44 = 0
game[+0x10 .. +0x17] = 2; game[+0x19] = 2
self.flags |= 0x08
work.88 = mega.terrain_box; mega.terrain_box = 0
mega.step = 2
0x8003F3DC(mega); 0x8003CDF0(mega); 0x8003CE30(mega); 0x8003C4EC(mega); ClearPlayerShots()
work.7C = 0; work.80 = work.82 = mega.x.px
step2++
PlaySound(1, 1, self)
```

### area_split — 0x800EF144 (A1)

```
work.7C++; work.80++; work.82--
mega.x.px = (frame_count & 1) ? work.82 : work.80
if work.7C >= 29:
    FadeOut(8)
    game.stage[+3] (0x800CCEDF) = var | 0x40
    step2++
```

### area_wait — 0x800EE800 (A2; stays here)

```
work.80++; work.82--
mega.x.px = (frame_count & 1) ? work.82 : work.80
if fade_dir (0x80097424) == 0:
    camera layers 0, 1, 2: +0x44 = 1
    mega.x.px = work.84
    game[+0x10 .. +0x17] = 0; game[+0x19] = 0
    mega.terrain_box = work.88
```

### move_start — 0x800EE8C4 (B0)

```
if work.7C != 0: work.7C--; return
PlaySound(1, 1, self)
(then as area_start, without the HP test and without the second sound)
step2++
```

### move_split — 0x800EF1E8 (B1)

```
as area_split, but it does not write 0x800CCEDF
```

### move_jump — 0x800EE9C4 (B2)

```
work.80++; work.82--; mega.x.px = (frame_count & 1) ? work.82 : work.80
if fade_dir != 0: return
switch var:
    2: if mega.var == 0 (X): work.7E = 0; game.checkpoint (+0x1D) = 1
       else (Zero):          work.7E = 1; game.checkpoint = 2
       0x8008EAB0 = 1
    3: game.checkpoint = 3; work.7E = 2
    4: game.checkpoint = 3; work.7E = 3
    (else: work.7E and the checkpoint stay)
e = 0x800F782C + work.7E * 32
mega.x.px = e.x; mega.y.px = e.y
work.80 = e.x + 64; work.82 = e.x - 64
if mega.var != 0: mega.y.px -= 1
mega.no_hit (+0x7A) = 0; game[+0x1C] = 0; mega[+0xE0] = 0
mega.prev_x/prev_y = mega.x/y
0x8002B36C(0)                      // reset the layer-0 tile cache
0x800C4560 = 1                     // BG refresh request
mega[+0xE4] = 0
camera layers 0, 1, 2: +0x44 = 1
camera layer 0..2 x.px, y.px = e.cam[0..2]
camera layer 0: min_x (+0x1E, +0x26) = e.min_x; max_x (+0x1C, +0x24) = e.max_x
                min_y (+0x22, +0x2A) = e.min_y; max_y (+0x20, +0x28) = e.max_y
camera layers 0, 1, 2: +0x4C = 1
mega.latch = 0
ClearPlayerShots()
mega[+0xE4] = 0; mega[+0xA4] = 0; mega.iframes = 0
SpawnInRect(cam.x - 48, cam.x + 368, cam.y - 48, cam.y + 288, 0)   // 0x80029F38
FadeIn(8)
camera layers 0, 1, 2: +0x44 = 0
step2++
PlaySound(1, 1, self)
```

### move_join — 0x800EF27C (B3)

```
work.80--; work.82++; mega.x.px = (frame_count & 1) ? work.82 : work.80
if fade_dir == 0: work.7C = 0; step2++
```

### move_end — 0x800EEDB8 (B4)

```
work.7C++; work.80--; work.82++; mega.x.px = (frame_count & 1) ? work.82 : work.80
if work.7C >= 29:
    camera layers 0, 1, 2: +0x44 = 1
    mega.x.px = table[work.7E].x
    mega.terrain_box = work.88
    0x8003AD18(mega)                 // back to a normal player step (ground or air)
    game[+0x10 .. +0x17] = 0; game[+0x19] = 0
    self.flags &= ~0x08
    step2++
```

### move_kill — 0x800EF2E4 (B5)

```
DeleteObject3(self)                  // the record keeps its "spawned" bit
```

## Engine helpers called

| address | name | what it does here |
|---|---|---|
| `0x80016C48` | `PlaySound` | bank 1, id 1 |
| `0x800179A4` / `0x80017A04` | `SetAnime` / `AnimeTick` | animation 0 only |
| `0x8002C9B0` | `DeleteObject3` | end of sequence B |
| `0x8002CCB0` | `DisplayCheck` | 64, 64 (var 0–2 only) |
| `0x8002D654` | `BoxOverlap` | own attack box against the player's hurt box; 0 while the player's `+0x61` ≠ 0 |
| `0x8002F288` | `CollideCheck` | every update (the crystal does not move) |
| `0x800139A0` | FadeIn (new) | `fade_dir` = +1 when it is 0; speed to `0x800C8861`; starts task `0x80013530` (brightness from 255) |
| `0x800139F4` | FadeOut (new) | the same with `fade_dir` = −1 (brightness from 0) |
| `0x80029F38` | SpawnInRect (new name; the engine spawner) | spawns the records in the rectangle |
| `0x8002B36C` | ResetLayerCache (new) | layer n: 16 words at `0x80090C90 + n*64 + 0x3C` = −1; halfwords `0x80090D5C`, `0x80097430`, `0x800CF854` (+2n) = 0 |
| `0x8002B6B0` | ClearPlayerShots (new) | clears the 32 weapon objects (`0x800950A0`) except type 55, and the effect objects whose type flag (`0x80073738[type]`) ≠ 0; `mega+0x98`, `+0x99`, `+0xA6` = 0 |
| `0x8003F3DC`, `0x8003CDF0`, `0x8003CE30`, `0x8003C4EC` | player clean-up (not named) | X only: weapon state; clears `mega+0x84`–`+0x92`, `+0xA2`, `+0xA1` |
| `0x8003AD18` | player resume (not named) | calls `0x8003CD9C`, `0x8003A628`; else `mega.step` = 2 |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x00` | flags | bit 3 set during a teleport |
| `+0x02` | var | record `id`: the mode |
| `+0x03` | display | 0 for var 3, 4 |
| `+0x04`–`+0x06` | state, step, step2 | |
| `+0x08`, `+0x0C` | x, y | |
| `+0x14`–`+0x16` | layer, flip, priority | |
| `+0x18`, `+0x1C` | prev_x, prev_y | |
| `+0x30` | anime_table | |
| `+0x50`, `+0x54`, `+0x68` | boxes | |
| `+0x5C`, `+0x60`, `+0x61`, `+0x67`, `+0x70` | hp, contact damage, iframes, latch, collide_flags | set, not used |
| `+0x7C` | work.7C | timer (split, join, delay 90) |
| `+0x7E` | work.7E | destination entry (0–3) |
| `+0x80`, `+0x82` | work.80, work.82 | the two images' x (px) |
| `+0x84` | work.84 | the player's x at the trigger |
| `+0x88` | work.88 | the player's terrain box pointer, saved |

## Player and global fields written

| address | name | write |
|---|---|---|
| `mega+0x05` | step | 2 at the start |
| `mega+0x0A`, `+0x0E` | x.px, y.px | the split images; the destination |
| `mega+0x18`, `+0x1C` | prev_x, prev_y | = position after the jump |
| `mega+0x61` | iframes | 0 after the jump |
| `mega+0x67` | latch | 0 |
| `mega+0x68` | terrain box | 0 during the teleport, then restored |
| `mega+0x7A` | no_hit | 0 |
| `mega+0xA4`, `+0xE0`, `+0xE4` | ? | 0 |
| `0x800CCEDF` | game+0x0F (exit request) | `var` \| `0x40` (sequence A) |
| `0x800CCEE0`–`0x800CCEE7`, `0x800CCEE9` | game+0x10–0x17, 0x19 (stop flags) | 2, then 0 |
| `0x800CCEEC` | game+0x1C | 0 |
| `0x800CCEED` | game+0x1D (checkpoint) | 1, 2 or 3 |
| `0x800CCEF6` | game+0x26 | read only (var 3) |
| `0x8008EAB0` | ? | 1 (var 2) |
| `0x800C4560` | BG refresh request | 1 |
| `0x800971F8` + n·`0x54` | camera layer n | `+0x0A`, `+0x0E` position; `+0x1C`–`+0x2A` limits (layer 0); `+0x44` follow; `+0x4C` |
