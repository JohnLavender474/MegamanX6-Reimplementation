# Pseudo code: Dr. Light capsule (stage object type 26)

## Dispatch tables

- No state table: the entry tests `state` 0 / not 0.
- Steps `0x80078E18` (12): 0 `cap_near` 0x80051924, 1 `cap_voice` 0x80051A44, 2 `cap_text`
  0x80051AC8, 3 `cap_talk` 0x80051B28, 4 `cap_close` 0x80051B74, 5 `cap_closed` 0x80051C08,
  6 `cap_enter` 0x80051C68, 7 `cap_ph2` 0x80051D28, 8 `cap_ph3` 0x80051E40, 9 `cap_hold`
  0x80051E9C, 10 `cap_give` 0x80051ED4, 11 `cap_leave` 0x80051FA8.

`Pause(n)` = `game[0x12] = game[0x13] = game[0x16] = game[0x17] = n`.
`Solid(self)` = `0x80032B34; 0x80032BE8 (lid: after); if !mega[0xD1]: 0x80031DA8; 0x8003208C; 0x80031F4C`.

### entry — 0x800515B4
```
prev = pos
if var != 16:
    if state == 0: cap_init(self) else cap_main(self)
    return
// the lid
parent = self[0x7C]
if parent.flags == 0: DeleteObject3(self); return
if state == 0:
    terrain = 0x80078DFC; hurt = 0x80078E04; [0xA2] = 1; dmg_tbl = 0x80075B9C; [0x75] = 1
    x.px = parent.x.px; y.px = parent.y.px - 78; state = 1
0x80032B34(self); 0x80032BE8(self); if !mega[0xD1]: 0x80031DA8(self)
0x8003208C(self); 0x80031F4C(self)
```

### cap_init — 0x80051158
```
taken = 0
if var == 8:
    if mega.char == X: taken = (game.armors & 8) or mega[0xE7]
    else: taken = game.armors & 0x20
else:
    bit = part_bit[var]                        // 0x80078E08
    if (game.armors & 4) and (bit & 0x0F): taken = 1
    if (game.armors & 2) and (bit & 0xF0): taken = 1
    if game.armorParts & bit: taken = 1
if taken: DeleteObject(self); return
dmg_tbl = 0x80075B9C; [0xA2] = 1; display = 1; flags |= 8
terrain = 0x80078DF8; hurt = 0x80078E00; anime_tbl = 0x800791A0
layer = mega.layer; tex, sprite, clut from sprite bank 128; pri = 4; flip = 0; [0x75] = 1
mega[0xE2] = 0; SetAnime(0)
loop: CollideCheck; if floor: break; y.px += 16
lid = GetStageObject(); if lid: lid.flags = 9; lid.type = 26; lid.var = 16; lid[0x7C] = self; lid.layer = layer
if var == 8: SpawnEffect(self, 35, mega.char == X ? 16 : 17)
else: SpawnEffect(self, 35, pic_var[var])    // 0x80078E10: 3 4 5 6 3 4 5 6
step = 0; state = 1; DisplayCheck(136, 136)
```

### SpawnEffect — 0x80051828
```
e = GetEffectObject(); if e: e.flags = 0x49; e.type = t; e.var = v; e[0x50] = self; e.layer = layer
```

### cap_main — 0x80051890
```
AnimeTick; step_tbl[step](self)
0x80032B34; if !mega[0xD1]: 0x80031DA8; 0x8003208C; 0x80031F4C; 0x80032BE8
DisplayCheck(136, 136)
```

### cap_near — 0x80051924 (step 0)
```
dx = |mega.x.px - x.px|; dy = mega.y.px - y.px
if dx > 64: return
if dy > 0 and dy > 16: return
if dy <= 0 and -dy > 104: return
SetAnime(10)
if mega.x > x: Sound(20, 0); flip = 0x40 else Sound(20, 64); flip = 0
Pause(1); step2 = 0; step++
```

### cap_voice — 0x80051A44 (step 1)
```
if step2 == 0:
    if anime.flag: StreamStop(3); anime.flag = 0; step2++
elif stream_state == 0:
    StreamPlay(28, 127); step2 = 0; step++
```

### cap_text — 0x80051AC8 (step 2)
```
if stream_state == 2: TextOpen(msg[var] /* 0x80078E48 */, 255, 0); step++
```

### cap_talk — 0x80051B28 (step 3)
```
if text_state /* 0x8008EAFC */ == 1: SetAnime(11); step2 = 0; step++
```

### cap_close — 0x80051B74 (step 4)
```
if step2 == 0:
    if text_state == 0: StreamStop(3); step2++
elif stream_state == 0:
    StageMusicRestart(); SetAnime(12); step2 = 0; step++
```

### cap_closed — 0x80051C08 (step 5)
```
if anime.flag: 0x8003D330(); Pause(0); step++
```

### cap_enter — 0x80051C68 (step 6)
```
if self[0x72] & 8:
    SetAnime(1); Sound(24, mega.flip); SpawnEffect(self, 36, 0)
    mega[0xE2] = 1; Pause(1); mega.x = x; step++
```

### cap_ph2 — 0x80051D28 (step 7)
```
mega.x = x
if mega[0xE2] == 2: SpawnEffect(self, 37, 0..3); step++
```

### cap_ph3 — 0x80051E40 (step 8)
```
mega.x = x
if mega[0xE2] == 3: SetAnime(2); timer = 40; step++
```

### cap_hold — 0x80051E9C (step 9)
```
mega.x = x
if timer == 0: step++ else timer--
```

### cap_give — 0x80051ED4 (step 10)
```
mega[0xE2] = 4; mega.x = x; SetAnime(13)
if var == 8:
    if mega.char != X: game.armors |= 0x20; 0x8003C4EC(&mega)
else: game.armorParts |= part_bit[var]
game[0x16] = game[0x12] = game[0x13] = game[0x17] = 0
step++
```

### cap_leave — 0x80051FA8 (step 11)
```
if OffScreenCheck2(136, 136) == 1: DeleteObjectKeep(self)     // 0x8002C9D4
```

### DeleteObjectKeep — 0x8002C9D4
```
if record: record.flags |= 0x80
DeleteObject3(self)
```

## Engine helpers called

| address | name | use |
|---|---|---|
| `0x800179A4` / `0x80017A04` | SetAnime / AnimeTick | |
| `0x8002A47C` | GetSpriteBank | bank 128 |
| `0x8002C398` | GetEffectObject | part picture, beam, rings |
| `0x8002C4C4` | GetStageObject (new name) | the lid |
| `0x8002C9B0` / `0x8002CA14` / `0x8002C9D4` | DeleteObject3 / DeleteObject / DeleteObjectKeep (new) | |
| `0x8002CBFC` / `0x8002CCB0` | OffScreenCheck2 / DisplayCheck | |
| `0x8002F288` | CollideCheck | floor snap |
| `0x80031DA8`, `0x80031F4C`, `0x8003208C`, `0x80032B34`, `0x80032BE8` | solid-object collision [likely] | |
| `0x8003D308` | `PlayerEvent(step, facing)` (20 / 24) | the player goes to event step 20 (hold) or 24 |
| `0x8003D330` | `PlayerEventEnd` | the player takes input again |
| `0x80018060` | StreamPlay (new name) [likely] | stream 28 |
| `0x80018774` | StreamStop (new name) [likely] | |
| `0x8001854C` | StageMusicRestart (new name) [likely] | |
| `0x80021760` | TextOpen (new name) [likely] | message |
| `0x8003C4EC` | palette refresh [guess] | Black Zero |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x02` | var | `id` 0–8; 16 = the lid |
| `+0x05` / `+0x06` | step / step2 | |
| `+0x15` | flip | faces the player at the open |
| `+0x45` | anime.flag | tested, cleared in step 1 |
| `+0x54` / `+0x58` / `+0x68` | hurt box, damage table, terrain box | |
| `+0x72` | contact flags of the solid helper; bit 3 = the player stands on it [likely] | |
| `+0x75`, `+0xA2` | 1 at set-up (solid-object settings [guess]) | |
| `+0x7C` | lid: the capsule; capsule: timer (40) | |
| player `+0xD1`, `+0xE2`, `+0xE7` | no solid test / capsule phase 0–4 / X ultimate flag [guess] | |
