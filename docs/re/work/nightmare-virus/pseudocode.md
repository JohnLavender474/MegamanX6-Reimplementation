# Nightmare Virus and its helper objects: pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`, from `yammark-1_start.state.gz`). All
addresses are main-program RAM addresses (shared by all stages). Names of engine helpers and fields
are in `catalog/helpers.json` of mmx-demake-tools. `step` = `+0x05`, `step2` = `+0x06`.

## Dispatch tables (main-program data)

| table | used by | entries |
|---|---|---|
| `0x80075FD8` state | `nv_update` `0x80041AB8` | 0 `nv_init` `0x800404D8`, 1 `nv_main` `0x80040800`, 2 `0x80041BA8` (`DeleteObject`), 3 `0x80041BC8` (`DeleteObject2`) |
| `0x80076018` step | `nv_main` | 0 `HitReactionDispatch`, 1 `0x800429CC` (empty), 2 IDLE `0x80041F30`, 3 DASH `0x80041BE8`, 4 SHOOT `0x8004202C`, 5 SOUL `0x80042250`, 6 SNATCH `0x80042658`, 7 BOSS SOUL `0x8004289C` |
| `0x80076038` IDLE | | `0x80041F6C`, `0x80041FB0`, `0x80041FE4`, `0x8004200C` |
| `0x80076048` DASH | | `0x80041C24`, `0x80040A04`, `0x80041C9C`, `0x80041D38`, `0x80041D5C`, `0x80041E4C`, `0x80040B84`, `0x80041EBC`, `0x80041F10` |
| `0x8007606C` SHOOT | | `0x800420CC`, `0x80042150`, `0x80042214` |
| `0x80076078` SOUL | | `0x8004228C`, `0x80042320`, `0x800423A0`, `0x80042420`, `0x800424E0`, `0x80042538`, `0x8004260C` |
| `0x80076094` SNATCH | | `0x80042694`, `0x800426D8`, `0x8004270C`, `0x80040CB0`, `0x80040E3C`, `0x80042780`, `0x800427E0`, `0x8004283C` |
| `0x800760B4` BOSS SOUL | | `0x800428D8`, `0x80040FB4`, `0x8004292C`, `0x800429C4` (empty) |
| `0x800760C8` orb state | `orb_update` `0x80041AF4` (visual entry 12) | `0x80041290`, `0x80042FA8`, `0x80043018` |
| `0x800760D4` image state | `img_update` `0x80041B30` (misc entry 11) | `0x80043038`, `0x800430D0`, `0x80043154` |
| `0x800760E0` image kind (`+0x05`) | `img_main` | 0 `img_converge` `0x80043174`, 1 `img_follow` `0x800432F4` |
| `0x800760E8` converge (`+0x06`) | `img_converge` | `0x800431C8`, `0x8004322C`, `0x800432C8` (3 entries; the kit read 6) |
| `0x800760F4` ray state | `ray_update` `0x80041B6C` (not walked by the kit) | `0x80043350`, `0x80043460`, `0x8004349C` |
| `0x80076100` ray step (`+0x05`) | `ray_main` | `0x800413D0`, `0x80041770` |
| `0x80075EBC` step pattern | `nv_next_step` | 4 rows of 8 bytes (below) |
| `0x80011590` jump table | `bsoul_descend` | 8 cases of 3 ray angles |

## One line per function

| address | name | what it does |
|---|---|---|
| `0x80041AB8` | `nv_update` | main entry 15: state dispatch |
| `0x800404D8` | `nv_init` | HP, boxes, table; branches on `var & 0xF0` |
| `0x80040800` | `nv_main` | anim, display, step, then soul pickup or weapon/contact test, off-screen test |
| `0x80041BA8` / `0x80041BC8` | `nv_delete` / `nv_delete2` | `DeleteObject` / `DeleteObject2` |
| `0x800429CC` | `nv_none` | step 1: empty |
| `0x80041F30` | `nv_idle` | IDLE sub-dispatch |
| `0x80041F6C` | `idle_start` | face player, anim 0, timer 60 |
| `0x80041FB0` | `idle_wait` | count down 60 |
| `0x80041FE4` | `idle_anim_end` | wait for anim flag 1 |
| `0x8004200C` | `idle_next` | `nv_next_step` |
| `0x80041BE8` | `nv_dash` | DASH sub-dispatch |
| `0x80041C24` | `dash_start` | face, anim 1, dash count = rand % 7 + 6 |
| `0x80040A04` | `dash_aim` | aim at player, 1 px/f (×1, ×1.5 or ×2), time rand % 13 + 4 |
| `0x80041C9C` | `dash_move` | move; at end next dash, or warp (Hard, 50 %), or next step |
| `0x80041D38` | `warp_hide` | hide, boxes off, timer 60 |
| `0x80041D5C` | `warp_place` | after 60: despawn if off screen, else place 64 px from the player, 2 images |
| `0x80041E4C` | `warp_wait_images` | wait until both images arrived; boxes on |
| `0x80040B84` | `warp_strike` | fly at the saved player point at 6 px/f until within 7 px |
| `0x80041EBC` | `warp_coast` | 8 more frames of movement |
| `0x80041F10` | `dash_next` | `nv_next_step` |
| `0x8004202C` | `nv_shoot` | leave to IDLE when outside the camera inset; else sub-dispatch |
| `0x800420CC` | `shoot_start` | face, anim 4, delay and count by difficulty |
| `0x80042150` | `shoot_fire` | at delay end: face, sound, orb; count down |
| `0x80042214` | `shoot_end` | wait 40, next step |
| `0x80042250` | `nv_soul` | SOUL sub-dispatch |
| `0x8004228C` | `soul_start` | anim and value by `var`; hop up 8 px/f, gravity 0.5 |
| `0x80042320` | `soul_hop` | land on the start y |
| `0x800423A0` | `soul_bob1` | bob 60 frames, then value 4, anim 7 |
| `0x80042420` | `soul_bob2` | bob 180 frames, then fade (`0x3X`) or start the revival |
| `0x800424E0` | `revive_wait_images` | wait for the 2 images, anim 0, timer 30 |
| `0x80042538` | `revive_blink` | blink 30, then a new virus (`var` low = 2) |
| `0x8004260C` | `soul_fade` | blink 30, then delete |
| `0x80042658` | `nv_snatch` | SNATCH sub-dispatch |
| `0x80042694` | `snatch_start` | face, anim 0, timer 60 |
| `0x800426D8` | `snatch_wait` | count down 60 |
| `0x8004270C` | `snatch_search` | at anim flag 1: find a Reploid or go to the pattern |
| `0x80040CB0` | `snatch_approach_first` | anim 1, then as below; on reach jumps to sub 5 |
| `0x80040E3C` | `snatch_approach` | 0.5 px/f to the point 28 px above the Reploid; grab within 2 px |
| `0x80042780` | `snatch_grab` | at anim flag 1: sound, grab overlay, timer 60 |
| `0x800427E0` | `snatch_hold` | after 60: sound, timer 90 |
| `0x8004283C` | `snatch_flash` | flash 90 frames, then delete |
| `0x8004289C` | `nv_boss_soul` | BOSS SOUL sub-dispatch |
| `0x800428D8` | `bsoul_start` | top of the screen above the player, 1 px/f down |
| `0x80040FB4` | `bsoul_descend` | sound once; rays every 40 frames; until 48 px above the player |
| `0x8004292C` | `bsoul_land` | at the player's y: add souls, sound, `game+0x0F` = 16 |
| `0x800429C4` | `bsoul_done` | empty |
| `0x800429D4` | `nv_next_step` | snatch first (`0x2X`), else the next entry of the pattern row |
| `0x80042A9C` | `nv_spawn_image` | misc 11 with `var` = a1; parent at `+0x50`; `act4++` for kinds 0 |
| `0x80042B68` | `nv_spawn_orb` | visual 12, copies the parent; parent at `+0x7C` |
| `0x80042C18` | `nv_face_player` | standard face rule (equal x → left) |
| `0x80042C48` | `nv_record_index` | ordinal of the own record among type-15 records of the area |
| `0x80042CE8` | `nv_soul_not_taken` | 1 when the "done" bit of this virus is clear |
| `0x80042D3C` | `nv_mark_taken` | set the "done" bit |
| `0x80042D98` | `nv_find_reploid` | nearest waiting stage object 4 |
| `0x80042E68` | `nv_reploid_valid` | target still waiting and visible |
| `0x80042EB4` | `nv_spawn_ray` | object 13 in the `0x8009C9B0` class |
| `0x8002C7F0` | `GetRayObject` (engine) | first free of 64 × `0x60` slots at `0x8009C9B0`, cleared |
| `0x8002DCF4` | `TerrainBoxOverlap` (engine) | 1 when the terrain boxes of a0 and a1 overlap |
| `0x80041AF4` | `orb_update` | visual entry 12: state dispatch |
| `0x80041290` | `orb_init` | contact 4, box, anim 5, offset, aim 5 px/f |
| `0x80042FA8` | `orb_main` | anim, move, contact, off screen 32 → delete; visible 16 |
| `0x80043018` | `orb_delete` | `DeleteObject3` |
| `0x80041B30` | `img_update` | misc entry 11: state dispatch |
| `0x80043038` | `img_init` | kind 0: anim 0; kind `0x10`: anim 3, priority 1 |
| `0x800430D0` | `img_main` | delete when the parent's state ≠ 1; display; sub; anim |
| `0x80043174` | `img_converge` | flicker by frame parity; sub-dispatch |
| `0x800431C8` | `img_cv_start` | start 32 px left (var 0) or right (var 1), 4 px/f inwards |
| `0x8004322C` | `img_cv_move` | move until the start x is reached |
| `0x800432C8` | `img_cv_done` | parent `act4--`, delete |
| `0x800432F4` | `img_follow` | copy parent x, y, display, palette while parent `act4` ≠ 0 |
| `0x80043154` | `img_delete` | `DeleteObject3` |
| `0x80041B6C` | `ray_update` | state dispatch on `0x800760F4` |
| `0x80043350` | `ray_init` | random sweep end, speed, colour and width |
| `0x80043460` | `ray_main` | sub-dispatch on `+0x05` |
| `0x800413D0` | `ray_sweep` | draw ray to the screen edge; rotate; stop at the end angle or when the parent changes step |
| `0x80041770` | `ray_shrink` | draw a shorter ray each update; then delete |
| `0x8004349C` | `ray_delete` | `DeleteObject3` |

## Virus

### `nv_init` — 0x800404D8 (state 0)

```
contact = 4 ; hp = 10
if difficulty == 0: hp = 2  elif difficulty >= 2: hp = 18
anime_table = 0x80075FB0 ; hurt_box = 0x80075EE4 ; attack_box = 0x80075EDC ; terrain_box = 0
iframes = display = flip = latch = 0 ; priority 3 ; damage_table = dmg_tbl_normal
speeds = 0 ; layer = mega.layer ; prev = pos ; face player
+0x8D = +0x8A = +0x8F = +0x8E = 0 ; state = 1 ; step2 = 0
switch var & 0xF0:
  0x00: +0x88 = nv_record_index() ; nv_next_step()
  0x10: priority 1 ; texture from the sprite bank ; contact 0 ; +0x8E = 1 ; attack_box 0 ; +0x8D = 1
        low 0: anim 8, +0x8A = 200 ; low 1: anim 9, +0x8A = 100 (other: return) ; step = 7
  0x20: +0x88 = nv_record_index() ; +0x8C = 1 ; step = 6
  0x30: texture ; contact 0 ; +0x8E = 1 ; attack_box 0 ; step = 5
```

### `nv_main` — 0x80040800 (state 1)

```
prev = pos ; AnimeTick
if step != 7: DisplayCheck(48, 48)
STEP[step]()
if step == 5:
    body = mega ; if mega+0xD5: body = 0x800CD340 ; if body+5 == 0: body = 0x800CD3F0
    terrain_box = +0x8E ? 0x80075EE0 : 0
    if TerrainBoxOverlap(self, body):
        game.soul[mega.var] (0x800CCFA2 + 2*char) += +0x8A ; PlaySound(0, 41) ; terrain_box = 0 ; state = 2 ; return
    terrain_box = 0
elif step != 7:
    if hurt_box and ContactWeapon() < 0:
        if not nv_soul_not_taken(): SpawnExplosion ; state = 2 ; return
        SpawnExplosion ; nv_mark_taken ; contact 0 ; +0x8E = 1 ; boxes 0 ; step 5 ; step2 0 ; clear flash ; return
    ContactMega
if OffScreenCheck2(64, 64) == 1 and +0x8D == 0: state = 3
```

### `nv_next_step` — 0x800429D4

```
step2 = 0
if var & 0xF0 == 0x20:
    nv_find_reploid() ; if +0x80: SetAnime 1 ; step 6 ; step2 3 ; return
row = 0x80075EBC + (var & 0x0F) * 8
if row[+0x8F] == 8: +0x8F = 0
step = row[+0x8F] ; +0x8F += 1
```

Rows: 0 = {2, 4, 8} IDLE, SHOOT; 1 = {2, 3, 8} IDLE, DASH; 2 = {2, 3, 4, 3, 8} IDLE, DASH, SHOOT,
DASH; 3 = {2, 2, 8} IDLE, IDLE (not used by any record seen).

### IDLE (step 2)

```
start: face ; anim 0 ; timer 60 ; wait: timer-- → 0 ; anim_end: flag == 1 ; next: nv_next_step
```

### DASH (step 3)

```
dash_start: face ; anim 1 ; +0x7E = rand % 7 + 6
dash_aim:   timer = rand % 13 + 4 ; a = ratan2(y - mega.y, mega.x - x)
            speed = (rcos a << 4, rsin a << 4)  (1 px/f) ; flip by speed_x
            r = rand % 3: 1 → speed × 1.5 ; 2 → speed × 2
dash_move:  MoveObject ; timer-- → 0: +0x7E-- ; ≠ 0 → step2 1
            = 0: if difficulty >= 2 and rand & 1 → step2 3 else nv_next_step
warp_hide:  display 0 ; boxes 0 ; timer 60
warp_place: display 0 ; timer-- → 0: if OffScreenCheck2(64,64): state 3
            face ; a = (rand << 20) >> 16 ; +0x84/+0x86 = mega pos
            pos = saved + (rcos a >> 6, rsin a >> 6)  (64 px) ; act4 = 0 ; image 0 ; image 1
warp_wait_images: display 0 ; when act4 == 0: anim 1 ; boxes on ; DisplayCheck(48,48)
warp_strike: speed = 96 × (rcos a, rsin a) toward the saved point (6 px/f) ; move
            when |dx| < 7 and |dy| < 7: timer 8
warp_coast: timer-- ≠ 0: move ; else next ; dash_next: nv_next_step
```

### SHOOT (step 4)

```
if x < cam.x + 16 or x > cam.x + 304 or y < cam.y + 16 or y > cam.y + 224: step 2 ; step2 0 ; return
start: face ; anim 4 ; (timer, count) = Easy (90, 3), Normal (60, 4), Hard+ (30, 5)
fire:  timer-- → 0: face ; PlaySound(2, 66) ; nv_spawn_orb ; count-- → 0: timer 40, next
       else timer = 90 / 60 / 30
end:   timer-- → 0: nv_next_step
```

### SOUL (step 5)

```
start: var 0x31: anim 7, value 4 ; var 0x32: anim 8, value 200 ; else anim 6, value 8
       speed_y = 8 px/f up ; gravity 0.5 ; speed_x 0 ; +0x86 = y
hop:   MoveObject2 ; when y >= +0x86: y = +0x86
       var 0x31/0x32: timer 180, step2 3 ; else timer 60, step2 2
bob1:  y = +0x86 + rsin((frame & 0x7F) << 5) >> 9 ; 60 frames → anim 7, value 4, timer 180
bob2:  same bob ; 180 frames →
       var & 0xF0 == 0x30: timer 30, step2 6 (fade)
       else: +0x8E 0, boxes 0, display 0, face, act4 0, image 0, image 1
revive_wait_images: display 0 ; act4 == 0: face, anim 0, timer 30
revive_blink: display = !(timer & 1) ; 30 → +0x8F 0 ; var = (var & 0xF0) + 2 ; nv_next_step ;
       contact 4 ; hp as in init ; boxes on ; terrain 0
fade:  display = !(timer & 1) ; 30 → display 0 ; state 2
```

### SNATCH (step 6)

```
start: face ; anim 0 ; timer 60 ; wait: 60
search: at anim flag 1: +0x8D 0 ; nv_find_reploid ; none: +0x8F 0, nv_next_step ; else +0x8D 1, next
approach (first sets anim 1):
       +0x8D 0 ; if not nv_reploid_valid(+0x80): nv_next_step ; return
       +0x8D 1 ; a = ratan2(y + 28 - r.y, r.x - x) ; speed = (rcos a << 4) >> 1 (0.5 px/f) ; move
       when |r.x - x| < 2 and |r.y - (y + 28)| < 2:
           anim 2 ; nv_mark_taken ; r.act4 (+0x07) = 2 ; hurt_box 0 ; clear flash
           first: step2 = 5 ; later: step2++
       first, not reached: step2++
grab:  at anim flag 1: act4 1 ; PlaySound(2, 63) ; image 0x10 ; timer 60
hold:  60 → PlaySound(2, 64) ; timer 90
flash: palette bit 15 toggles each update ; 90 → act4 0 ; display 0 ; state 2
```

### BOSS SOUL (step 7)

```
start:   display 1 ; y = cam.y - 32 ; x = mega.x ; +0x7E 90 ; timer 1 ; speed (0, -1 px/f) ; act4 0
descend: if act4 == 0 and y >= cam.y - 8: PlaySound(0, 43) ; act4 1
         if +0x7E and --+0x7E == 0: speed_y = -1 px/f
         if y >= mega.y - 48: MoveObject2 ; next ; return
         MoveObject2 ; timer-- → 0: 3 rays at {k·45°, k·45° + 135°, k·45° + 225°}, k = rand & 7 ; timer 40
land:    MoveObject2 ; when y >= mega.y: game.soul[char] += +0x8A ; PlaySound(0, 41) ;
         game+0x0F = 16 ; display 0 ; next
done:    nothing (the object stays, hidden)
```

## Helpers

```
nv_record_index: recs = *(0x8007349C + stage*8 + area*4) ; n = 0
    for each record until kind == 15: if rec.type == 15: if rec == self.record: return n ; n++ (≥ 256 → 0)
    return 0
row(stage, area) = 0x80075FE8[stage*2 + area] ; bits at game + 0x178 + row*32
nv_soul_not_taken: !(bits[idx >> 3] >> (idx & 7) & 1)   (idx = +0x88)
nv_mark_taken:     bits[idx >> 3] |= 1 << (idx & 7)
nv_find_reploid:   +0x80 = 0 ; best = 1 000 000
    for each of the 80 stage objects (0x800C1220, 0xA4 each):
        flags ≠ 0, type == 4, display ≠ 0, +0x07 == 1 → d = dx² + dy² ; d < best → best, +0x80 = obj
nv_reploid_valid:  flags ≠ 0, type == 4, display ≠ 0, +0x07 == 1
```

## Orb (visual 12)

```
init: contact 4 ; hp 1 ; damage_table 0 ; priority 3 ; latch 1 ; anim 5 ; terrain 0 ;
      attack box 0x800760C4 (-3,-3,6,6) ; hurt box 0 ; state 1
      x += flip ? 4 : -4 ; y += 11 ; aim at the player: speed = 80 × (rcos, rsin) = 5 px/f
      OffScreenCheck2(32) → state 2 else DisplayCheck(16)
main: AnimeTick ; MoveObject ; ContactMega ; OffScreenCheck2(32) → state 2 else DisplayCheck(16)
```

## Image and grab overlay (misc 11)

```
init: display 0 ; priority 3 ; kind 0: anim 0, +0x05 0 ; kind 0x10: priority 1, anim 3, +0x05 1
main: parent (+0x50) state ≠ 1 → display 0, delete ; DisplayCheck(16) ; SUB[+0x05] ; AnimeTick
converge: display = (frame + var) & 1 ;
    start: +0x54 = x ; var low 0: speed +4, x -= 32 ; else speed -4, x += 32
    move: until x reaches +0x54 (8 updates) ; done: parent.act4-- ; delete
follow: parent.act4 == 0 → delete ; else copy parent x, y, display, palette
```

## Ray (object 13 of the class at 0x8009C9B0)

```
init: flags 3 ; state 1 ; var = rand & 1 ; +0x34 = 26 + (rand & 1) ; +0x37 = -1 ; pos 0
      +0x48 = +0x46 - (rand % 60 + 120) * 11 ; +0x4A = (rand % 4 + 3) * 11 ; +0x4E = rand % 3
sweep: from parent (+0x5C) pos, steps of 16 px along +0x46 while inside
       [cam.x-16, cam.x+336] × [cam.y-48, cam.y+256], at most 50 ; 4 points at ±width
       +0x34 = (frame + var) & 1 ? 26 + (rand & 1) : 28 ; +0x46 -= +0x4A
       if +0x46 <= +0x48 or parent.step2 ≠ +0x07: +0x4C = steps × 4 ; +0x05++
shrink: same polygon with +0x4C steps, +0x4C-- ; 0 → display 0 ; state++ (delete)
```

## Engine helpers called

| address | name |
|---|---|
| `0x80016C48` | `PlaySound` |
| `0x800179A4` / `0x80017A04` | `SetAnime` / `AnimeTick` |
| `0x8002A47C` | `GetSpriteBank` |
| `0x8002C458` / `0x8002C530` / `0x8002C7F0` | `GetVisualObject` / `GetMiscObject` / `GetRayObject` (new) |
| `0x8002C9B0` / `0x8002CA14` / `0x8002CA54` | `DeleteObject3` / `DeleteObject` / `DeleteObject2` |
| `0x8002CBFC` / `0x8002CCB0` | `OffScreenCheck2` / `DisplayCheck` |
| `0x8002D230` / `0x8002D2B0` | `MoveObject2` / `MoveObject` |
| `0x8002D2D4` | `GetRNG` |
| `0x8002DCF4` | `TerrainBoxOverlap` (new) |
| `0x80030ECC` / `0x80031474` | `ContactMega` / `ContactWeapon` |
| `0x80040290` | `HitReactionDispatch` |
| `0x80049E50` | `SpawnExplosion` |
| `0x80065944` / `0x80065A14` / `0x80065B74` | `rsin` / `rcos` / `ratan2` |

## Object struct (the fields used here)

| offset | use |
|---|---|
| `+0x02` var | kind (high nibble) and pattern row (low nibble) |
| `+0x03` display | |
| `+0x04`–`+0x06` | state, step, step2 |
| `+0x07` act4 | count of after-images still moving; 1 while grabbing; in the Reploid: 1 waiting, 2 snatched |
| `+0x15` flip, `+0x16` priority | |
| `+0x20`–`+0x2C` | speeds, accel, gravity |
| `+0x50` / `+0x54` / `+0x68` | attack / hurt / terrain box (terrain only in SOUL) |
| `+0x58`, `+0x5C`, `+0x60`, `+0x61`, `+0x67` | damage table, HP, contact damage, iframes, latch |
| `+0x7C` | timer |
| `+0x7E` | counter: dashes left, shots left, boss-soul timer |
| `+0x80` | target Reploid (pointer) |
| `+0x84`, `+0x86` | saved point (warp target; soul base y) |
| `+0x88` | ordinal among the type-15 records (index of the "done" bit) |
| `+0x8A` | soul value |
| `+0x8C` | set to 1 for kind `0x20`; never read |
| `+0x8D` | no despawn off screen when ≠ 0 |
| `+0x8E` | soul pickup box on |
| `+0x8F` | index in the pattern row |
