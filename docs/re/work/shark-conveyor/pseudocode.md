# Conveyor belt (stage 4 area 1, overlay 07): pseudo code

Units: px, px per update, updates. `mega` = `0x800970A0`, `game` = `0x800CCED0`, `L0` = layer-0
camera `0x800971F8` (`+0x08` x 16.16, `+0x0A` x, `+0x0E` y, `+0x1C` max x, `+0x1E` min x, `+0x24`
target max x, `+0x26` target min x, `+0x4C` redraw), `L1` = `0x8009724C`, `L2` = `0x800972A0`.

## Dispatch

| table | entries |
|---|---|
| stage logic types (scratchpad `+0x8C` = `0x800F82A8`) | 7 `0x800F0530`, 9 `BeltUpdate` `0x800ED998`, 10 `SceneMove` `0x800EDD00` |
| kind-7 types `0x8007136C` (overlay 07) | 2 `PressUpdate` `0x80046E14`, 3 `AutoScroll` `0x800E9C98` |
| `AutoScroll` states `0x800F7FE0` | 0 `0x800E9CF8`, 1 `0x800E9D38`, 2 `0x800E9D78` |
| `SceneMove` states `0x800F8C28` | 0 `0x800EDD3C`, 1 `0x800EDD7C`, 2 `0x800EDBB8` |

## BeltUpdate — 0x800ED998 (stage logic 9)

```
if obj.state == 0:
    belt_frame (0x800F985C) = belt_turns (0x800E3142)
    BeltSaveFrames()
    BeltDrawRollers()
    obj.state = 1

if mega[0xE0] == 2: return
if mega.state != 1: return
if mega.hp == 0: return

if mega.step == 20:                        ; event hold (the scene move)
    if obj[0x14]: PlaySound(5, 3, 0); obj[0x14] = 0
    return
if mega.step == 0: return                  ; beam-in: the belt waits

if game.checkpoint == 0:
    L0.min_x = L0.target_min_x = L0.x      ; the camera cannot go back left

if mega.terrain_box == 0: return           ; no box (riding): nothing

if game.checkpoint >= 2:
    PlaySound(5, 3, 0)
    DeleteObject3(obj)                     ; the end of the belt
    return

if obj[0x14] == 0 or mega[0x14C] != 0:
    PlaySound(5, 6, 0)                     ; the belt sound
    obj[0x14] = 1

BeltRotateTiles()
if frame_count % 5 == 0:
    belt_frame += 1
    BeltDrawRollers()

foot_y = mega.y + box.oy + box.h           ; bottom of the terrain box
if TileCollisionAt(mega, mega.x, foot_y) == 58:
    mega.x -= 1                            ; whole pixels only
```

## BeltSaveFrames — 0x800ED4D0

```
for i in 0..7:
    (x, y) = belt_places[i]                ; 0x800F8C08: (32,560) (64,560) (96,560) (128,560)
                                           ;             (32,592) (64,592) (96,592) (128,592)
    belt_tiles[i][0] = tile_id(L0, x,      y)        ; 0x800F981C + 8 i
    belt_tiles[i][1] = tile_id(L0, x + 16, y)
    belt_tiles[i][2] = tile_id(L0, x,      y + 16)
    belt_tiles[i][3] = tile_id(L0, x + 16, y + 16)
```

`tile_id` = the cell word of the screen (scratchpad `+0x08` + screen × 512 + row × 32 + column × 2)
& `0x3FFF`; the screen from `0x80028078(0, x >> 8, y >> 8)`.

## BeltRotateTiles — 0x800ED764

```
if frame_count % 5 != 0: return
defs = scratchpad[0x0C]                    ; 4-byte tile definitions
tmp[k] = defs[belt_tiles[0][k]]  for k in 0..3
for i in 1..7:
    defs[belt_tiles[i-1][k]] = defs[belt_tiles[i][k]]  for k in 0..3
defs[belt_tiles[7][k]] = tmp[k]  for k in 0..3
belt_turns += 1
```

## BeltDrawRollers — 0x800ED8AC

```
belt_frame &= 7
for i in 0..19:
    dst = roller_dst[i]                    ; 0x800F88E8 + 8 i: dx, dy, w, h (cells)
    src = roller_src[i][belt_frame]        ; 0x800F8988 + i * 32 + belt_frame * 4: sx, sy
    CopyTiles(0, dst.dx, dst.dy, {src.sx, src.sy, dst.w, dst.h})
L0.redraw = 1
```

## AutoScroll — 0x800E9C98 (kind-7 type 3; the mini-boss room, not the belt)

```
if mega[0xCC] != 0 or mega[0xE0] == 2: return
switch obj.state:
case 0:
    if game.checkpoint == 1:
        L1.mode = 5
        obj.target_x (+0x14) = L0.max_x
        obj.state = 1
case 1:
    if mega.step == 2:                     ; X stands (after the beam-in or the scene move)
        mega.walk_speed (+0xF4) += 0.5
        obj.state = 2
case 2:
    if game[0x26]:
        mega.walk_speed -= 0.5
        DeleteObject3(obj)
    elif L0.x < obj.target_x:
        L0.x_fixed += 0.5
        L0.max_x = L0.min_x = L0.target_max_x = L0.target_min_x = L0.x
```

## Junk block on the belt — 0x800EE790 (stage type 11, run state, area ≠ 0 part)

```
left = obj.x - obj.half_w - 1
if BlockAt(obj, left, obj.y) == 0 and TileCollisionAt(obj, left, obj.y) == 0:
    if obj[0x07] == 0:
        if TileCollisionAt(obj, obj.x, obj.y + box.oy + box.h) == 58:
            obj.x -= 1
    elif mega.x >= 1088:
        obj[0x07] = 0                      ; var bit 7: wait for the player
```
