# Nightmare Press: pseudo code

Units: px, px per update (16.16 for speeds and the offset), updates. `L0` = layer-0 camera
(`0x800971F8`), `L1` = layer-1 camera (`0x8009724C`). `mega` = `0x800970A0`, `game` = `0x800CCED0`.

## Dispatch tables

| table | entries |
|---|---|
| kind-7 class `0x8007136C` | 2: `PressUpdate` `0x80046E14` (0, 1, 3, 4: overlay addresses, not used in stage 4) |
| press states `0x800762C0` | 0 `PressInit` `0x800472D0`, 1 `PressRun` `0x80047394` |
| press sub-steps `0x800762C8` | 0 `PressIntro` `0x800473EC`, 1 `PressWait` `0x80047460`, 2 `PressFall` `0x8004755C`, 3 `PressRise` `0x80047674`, 4 `PressBottom` `0x800476E0`, 5 `PressHold` `0x80047760` |
| stage type 9 states `0x800F35FC` | 0 `BlockInit` `0x800E9878`, 1 `BlockRun` `0x800E9A30`, 2 `DeleteObject` stub `0x800E9EE4` |
| stage type 9 sub-steps `0x800F3608` | 0 `BlockFollow` `0x800E9D5C` |
| actor2 `0x80071380` | 0 `RideUpdate` `0x800F2414` (overlay 06) |

### PressUpdate — 0x80046E14 (kind-7 type 2)

```
if game.stage >= 19:  if game.checkpoint != 0: return
else:                 if game.stage != 4 and game.checkpoint != 2: return
if mega[0xE0] == 2: return
L1.old_y (+0x1A) = L1.y
press_states[obj.state](obj)
if L0[0x20] != L0[0x22]: L0[0x34] = 0
L1.y = L0.y + (s8)obj.var - obj.offset_px          ; offset_px = obj+0x1E
if (mega.step == 31 and mega.sub < 4) or mega.step == 63:   ; super jump / ceiling hold
    dy = L1.y - L1.old_y
    if dy > 0: mega.y -= dy                          ; the press rises: carry the player up
box = mega.terrain_box (+0x68)
; choose the kill line
if game.stage >= 19:
    if box == 0: goto end
    if mega.x >= 2896 or game.checkpoint == 1: DeleteObject3(obj); return
    line = L0.y - L1.y + 240
elif game.stage == 4 and game.area != 0:
    if box == 0: goto end
    if mega.x >= 4480 or game.checkpoint == 1: DeleteObject3(obj); return
    if mega.x >= 4257: return
    line = L0.y - L1.y + 240
elif game.stage == 4:                                ; area 0
    if box == 0: goto end
    if mega.y >= 1280 or game.checkpoint >= 2: DeleteObject3(obj); return
    if game.checkpoint == 0: if mega.x >= 4993: return; line = L0.y - L1.y + 256
    else:                    if mega.x >= 5057: return; line = L0.y - L1.y + 736
else:                                                ; another stage, checkpoint 2
    if box == 0: goto end
    if game.checkpoint >= 3: DeleteObject3(obj); return
    if 1857 <= mega.x < 1920:  line = L0.y - L1.y + 2001
    elif 2304 <= mega.x <= 3776: line = L0.y - L1.y + 2065
    elif 4240 <= mega.x <= 6016: line = L0.y - L1.y + 2097
    else: goto end
; the crush test
top = mega.y + box.oy - box.h                         ; X standing: y - 17, dashing: y - 9
if line >= top:
    mega[0xE6] = 127                                  ; squeeze damage
    mega.y += line - top                              ; push the player down out of the press
    mega[0x71] |= 0x04                                ; "blocked above by an object"
end:
if L0[0x20] != L0[0x22]: L0[0x34] = 0
```

### PressInit — 0x800472D0 (state 0)

```
obj.offset = 0; obj.speed = +2.0
if game.stage >= 19:   obj.count = 16; obj.timer = 0; obj.sub = 0
elif game.stage == 4:  obj.count = 8;  obj.timer = 0; obj.sub = 0
else:
    L1.mode (L0+0x58) = 9; L1+0x4C (L0+0xA0) = 1
    obj.speed = difficulty < 2 ? -1.0 : -1.25
    obj.timer = game.checkpoint == 0 ? 120 : 50
    obj.sub = 1; obj.count = 0
obj.state += 1
```

### PressRun — 0x80047394 (state 1)

```
game[0x27] = 0
if mega[0xE0] != 2: press_steps[obj.sub](obj)
```

### PressIntro — 0x800473EC (sub-step 0)

```
if mega.step == 0: return                            ; wait for the beam-in to end
obj.offset += obj.speed; obj.count -= 1
if obj.count == 0:
    obj.speed = difficulty < 2 ? +1.0 : +1.25
    obj.timer = 180; obj.sub = 1
```

### PressWait — 0x80047460 (sub-step 1)

```
if game[0x26]:                                       ; the Ride Armor is squeezed
    Quake(60, 2, 2)
    obj.count = 80.0 / |obj.speed|
    obj.sub = 5; obj.speed = -1.0; obj.timer = 50; obj.saved = obj.offset
    return
obj.timer -= 1
if obj.timer == 0:
    obj.count = 80.0 / |obj.speed|
    if obj.speed > 0: obj.sub = 2
    else: obj.sub = 3; PlaySound(2, 34, obj)
```

### PressFall — 0x8004755C (sub-step 2)

```
if game[0x26]:                                       ; stopped by the Ride Armor
    Quake(60, 2, 2)
    obj.count = 80.0 / |obj.speed| - obj.count         ; updates already moved
    obj.timer = 50; obj.saved = obj.offset
    PlaySound(2, 35, 0); Rumble(5)
    obj.speed = -1.0; obj.sub = 5
    return
obj.offset += obj.speed; obj.count -= 1
if obj.count == 0:                                   ; the slam
    Quake(60, 2, 2); PlaySound(2, 35, 0); Rumble(5)
    obj.sub = 4; obj.speed = -1.0; obj.timer = 40; obj.saved = obj.offset
```

### PressRise — 0x80047674 (sub-step 3)

```
game[0x27] = 1
obj.offset += obj.speed; obj.count -= 1
if obj.count == 0:
    obj.speed = difficulty < 2 ? +1.0 : +1.25
    obj.timer = 50; obj.sub = 1
```

### PressBottom — 0x800476E0 (sub-step 4)

```
obj.timer -= 1
if obj.timer == 0:
    obj.speed = difficulty < 2 ? -1.0 : -1.25
    obj.timer = game.checkpoint == 0 ? 120 : 50
    obj.sub = 1; obj.offset = obj.saved
```

### PressHold — 0x80047760 (sub-step 5)

```
obj.timer -= 1
if obj.timer == 0:
    obj.speed = difficulty < 2 ? -1.0 : -1.25
    obj.sub = 3; PlaySound(2, 34, obj); obj.offset = obj.saved
```

### BlockInit — 0x800E9878 (stage type 9, state 0)

```
if mega.step == 0: return
obj.act2 = 2; obj.flag = 0x41; obj.state += 1; obj.muteki = 0
obj.col_box = &obj.box7C; obj.dmg_box = &obj.box80   ; copied from 0x800F4018 / 0x800F401C (zero)
v = (s8)obj.var
if v < 10:      obj.col_box = &block_col[v] (0x800F35D4); obj.dmg_box = &block_box[v] (0x800F35A8)
elif v == 112:  box7C = (0, 0, 32, 8); box80 = (-32, -8, 64, 16); obj.atk_box = 0x800F35D0
else:           hi = v >> 4 (4 bits), lo = v & 15
                box7C = (0, 0, hi*8, lo*8); box80 = (-hi*8, -lo*8, hi*16, lo*16)
obj.dmg_tbl = 0x80075B9C; obj.atk = 127; obj.anime_tbl = 0x800F35A0
obj.flip = 0; obj[0x65] = 0; obj[0x76] = 0; obj[0xA2] = 1; obj[0x88] = 0; obj[0x75] = 1
obj.y0 (+0x86) = obj.y
SetAnime(obj, 0); obj.act3 = 0
```

### BlockRun — 0x800E9A30 (stage type 9, state 1)

```
if game.stage == 4:
    if obj.y >= 672 and game.checkpoint == 0 and game.area == 0: return
elif game.stage < 19 and game.checkpoint != 2: return
0x80032B34(obj)                                       ; not read
if mega[0xE0] != 2: block_steps[obj.act3](obj)        ; BlockFollow
DisplayCheck(obj, 160, 160)
near = (game.area == 0 and obj.var == 0) or (|mega.x - obj.x| < 160 and obj.disp == 1)
if near and mega[0xE5] == 0:
    SolidObject(obj)
    if game[0x27] and (obj[0x72] & 0x04) and (mega[0x70] & 0x08):
        mega[0x71] = 0                                ; no squeeze while the press rises
    if obj[0x76] and (obj[0x72] & 0x03):              ; the player is at a side
        y = mega.y; mega.y = y - mega.speed_y
        SolidObject(obj)
        if obj[0x76] == 0: mega.y -= mega.speed_y
        else: mega.y = y
0x80032BE8(obj); 0x8003208C(obj); 0x80031F4C(obj)    ; always (not read)
obj.disp = 0
```

### BlockFollow — 0x800E9D5C (stage type 9, sub-step 0)

```
obj.y = obj.y0 + L0.y - L1.y
if game.stage == 4:
    if game.area == 0:
        if obj.var == 0: if game.checkpoint > 0: DeleteObject3(obj)
        else:            if game.checkpoint >= 2: DeleteObject3(obj)
    else:
        if obj.var == 112 and (obj[0x72] & 0x04): ContactMega(obj)
        if mega.x >= 4480 or game.checkpoint > 0: DeleteObject3(obj)
elif game.stage >= 19 and game.checkpoint > 0: DeleteObject3(obj)
```

### The player side (main program)

```
PlayerUpdate (0x80034FBC), each update, before the step:
    mega[0x89] = mega[0x70] | mega[0x71]   ; terrain flags of the last CollideCheck | object flags
    if game.stop[0x10]: skip to the end
    if mega[0xE5]: mega[0xE5] -= 1
    PlayerHitCheck(mega)
    ... step ...
    mega[0x71] = 0; mega[0x72] = 0; mega[0x4A] = 0

PlayerHitCheck (0x80039228):
    if mega.step in (0, 1): return
    if (mega[0x70] & 0x0C) == 0x0C: total += HP & 0x7F; HP = 0x80        ; terrain crush
    if (mega[0x70] & 0x03) == 0x03: total += HP & 0x7F; HP = 0x80
    if HP != 0x80 and ((mega[0x89] & 0x0C) == 0x0C or (mega[0x89] & 0x03) == 0x03):   ; squeeze
        mega[0x7A] = 0                                                  ; no_hit off
        if X and mega[0xE0] == 1: mega[0xE0] = 0; game.lock = 0; 0x8002ABA8()
        mega[0xE5] = 75
        hp = HP & 0x7F; HP = hp
        if mega[0xE6] >= hp: total += hp
        HP = hp - mega[0xE6]
        clear +0x76 and +0x72 of the 80 stage objects
        if (s8)HP > 0: HP |= 0x80; mega[0xCA] = 0; class = 1 (4 in forms 1-4); +0xA5 = flip; total += +0xE6
        else: HP = 0x80
    spikes ...
    if HP == 0x80: death (state 2)                                      ; same update
    invulnerability count ...
```

### RideUpdate — 0x800F2414 (actor2 type 0, overlay 06; only the part used here)

```
ride.buttons = mega.buttons (when mega[0xD0] == 0)
if mega[0xCC]: 0x8002CD6C(ride); return
ride.old = ride.pos
ride[0xA8] = ride[0x70] | ride[0x71]; ride[0x71] = 0
ride_states[ride.state](ride)            ; 0x800F3A50
ride.hp &= 0x7F
game[0x26] = ((ride[0xA8] & 0x0C) == 0x0C) ? 1 : 0
```

## Engine helpers called

| address | name | use here |
|---|---|---|
| `0x80029478` | `Quake` | (60, 2, 2) at the slam and at a hold |
| `0x80016C48` | `PlaySound` | bank 2: 34 rise start, 35 slam / hold |
| `0x80012D9C` | `Rumble` | 5 at the slam and at a hold |
| `0x8002C9B0` | `DeleteObject3` | press and type 9 deletion |
| `0x80031DA8` | `SolidObject` | type 9 body |
| `0x80030ECC` | `ContactMega` | type 9 `var` 112, from below |
| `0x8002CCB0` | `DisplayCheck` | type 9, (160, 160) |
| `0x800179A4` | `SetAnime` | type 9 animation 0 |

## Object struct (the fields used here)

Press (kind-7 object, `0x30` bytes):

| offset | name | meaning |
|---|---|---|
| `+0x00` | flag | 1 when live |
| `+0x01` | type | 2 |
| `+0x02` | var | 32: base offset of layer 1 (px) |
| `+0x04` | state | 0 init, 1 run |
| `+0x05` | sub | sub-step 0–5 |
| `+0x10` | set | record pointer |
| `+0x14` | timer | u16, updates |
| `+0x16` | count | u16, updates left to move |
| `+0x18` | speed | 16.16 px/update, + = down |
| `+0x1C` | offset | 16.16 px; `+0x1E` = px; + = lower |
| `+0x20` | saved | offset at the last stop |

Type 9 body: `+0x02` var, `+0x03` display, `+0x0A`/`+0x0E` x/y, `+0x30` animation table, `+0x50`
attack box (`var` 112), `+0x54` hurt box, `+0x58` damage table, `+0x60` contact damage 127, `+0x61`,
`+0x65`, `+0x68` terrain box, `+0x72` touch flags, `+0x75`, `+0x76` carry flag, `+0x7C`/`+0x80` own
boxes, `+0x86` y0, `+0x88`, `+0xA2`.

Player: `+0x05` step, `+0x06` sub-step, `+0x0E` y, `+0x24` speed_y, `+0x5C` HP, `+0x61`
invulnerability (not tested by the squeeze), `+0x68` terrain box, `+0x70` terrain flags, `+0x71`
object flags, `+0x89` both, `+0xD5` ride flag, `+0xE0`, `+0xE5` squeeze timer, `+0xE6` squeeze damage.
