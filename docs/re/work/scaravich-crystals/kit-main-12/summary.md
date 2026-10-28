# RE kit: scaravich-main-12

- Object: scaravich-main-12
- State: `scaravich-1_start.state.gz` (SHA-1 `4b9cab59f2dd853bd5034ef66217cebb0de9b9bb`), stage 5 area 0
- Class: main, type 12; entry function 0x800ed5c4
- Functions walked: 13; state tables: 3

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800139a0 | FadeIn | FadeIn(speed): fade_dir = +1 | 0x800ed258 |
| 0x800139f4 | FadeOut | FadeOut(speed): fade_dir = -1 | 0x800ed854 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800ed148, 0x800ed258 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ed610 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ed7ec |
| 0x80029f38 | SpawnInRect | SpawnInRect(x0, x1, y0, y1, flag): spawn the records inside a rectangle | 0x800ed258 |
| 0x8002b36c | ResetLayerCache | ResetLayerCache(layer): redraw state of a BG layer | 0x800ed258 |
| 0x8002b6b0 | ClearPlayerShots | delete the weapon objects except type 55, and flagged effects | 0x800ed148, 0x800ed258 |
| 0x8002c398 | GetEffectObject | first free of 96 effect slots of 0x70 at 0x80091298, cleared | 0x800ecf44 |
| 0x8002c4c4 | GetStageObject | first free of the 80 stage-object slots at 0x800C1220 | 0x800ed950 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ed950 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ed7ac |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ed7ac |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ed704 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ed7ec |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ed5c4 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ed704 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800ed704 |
| 0x8003ad18 | ToStand | clear the dash flags, step 2, the stand animation (94 when shooting) | 0x800ed4b0 |
| 0x8003c4ec | PlayerPaletteRestore | the palettes of the weapon slot mega+0x93 and of the part bits | 0x800ed148 |
| 0x8003cdf0 | ClearActionFlags | mega+0x84, +0x86, +0x8C, +0x8E..+0x92, +0xA2 = 0; palette | 0x800ed148 |
| 0x8003ce30 | ClearChargeGlow | mega+0xA1 = 0 | 0x800ed148 |
| 0x8003f3dc | ChargeStop | X: stop the charge (palette by mega+0x93) (likely) | 0x800ed148 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ed704 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ecf44 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x8002d30c | 0x800ecf44 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f58f0 | 0x800ed5c4 | 0x800ed610, 0x800ed704, 0x800ed7ac, 0x800ed7ac |
| 0x800f5900 | 0x800ed704 | 0x80040290, 0x800ed7ec, 0x800ecf44, 0x800ed818 |
| 0x800f59a0 | 0x800ed818 | 0x800ed148, 0x800ed854, 0x800ed258, 0x800ed8e8, 0x800ed4b0, 0x800ed950 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f58d8 (5 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 | 1 | 0 | hold |
| 1 | 1 2 3 4 2 3 4 2 3 4 5 6 0 | 1 2 2 2 2 2 2 2 2 2 2 3 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 2 | 7 | 1 | 0 | hold |
| 3 | 8 | 1 | 0 | hold |
| 4 | 9 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 2 | 2 | 0x800ed148, 0x800ed4b0 |
| 0x3 | display | 0 | 1 | 0x800ed704 |
| 0x4 | state | 2 | 1 | 0x800ed5c4, 0x800ed610, 0x800ed7ac |
| 0x5 | step | 2 | 3 | 0x800ecf44, 0x800ed610, 0x800ed704 |
| 0x6 | step2 | 6 | 6 | 0x800ed148, 0x800ed258, 0x800ed4b0, 0x800ed610, 0x800ed818, 0x800ed854, 0x800ed8e8 |
| 0x8 | x | 2 | 0 | 0x800ed610, 0x800ed704 |
| 0xa | x.px | 2 | 0 | 0x800ecf44 |
| 0xc | y | 2 | 0 | 0x800ed610, 0x800ed704 |
| 0xe | y.px | 2 | 0 | 0x800ecf44 |
| 0x14 | layer | 0 | 1 | 0x800ed610 |
| 0x15 | flip | 0 | 2 | 0x800ed610 |
| 0x16 | priority | 0 | 1 | 0x800ed610 |
| 0x18 | prev_x | 0 | 2 | 0x800ed610, 0x800ed704 |
| 0x1c | prev_y | 0 | 2 | 0x800ed610, 0x800ed704 |
| 0x20 | speed_x | 0 | 1 | 0x800ed610 |
| 0x24 | speed_y | 0 | 1 | 0x800ed610 |
| 0x28 | accel_x | 0 | 1 | 0x800ed610 |
| 0x2c | gravity | 0 | 1 | 0x800ed610 |
| 0x30 | anime_table | 0 | 1 | 0x800ed610 |
| 0x50 | attack_box | 0 | 3 | 0x800ecf44, 0x800ed610, 0x800ed704 |
| 0x54 | hurt_box | 0 | 2 | 0x800ecf44, 0x800ed610 |
| 0x58 | damage_table | 0 | 1 | 0x800ed610 |
| 0x5c | hp | 0 | 1 | 0x800ed610 |
| 0x60 | contact_damage | 0 | 1 | 0x800ed610 |
| 0x61 | iframes | 0 | 1 | 0x800ed610 |
| 0x67 | latch | 0 | 1 | 0x800ed610 |
| 0x68 | terrain_box | 0 | 1 | 0x800ed610 |
| 0x70 | collide_flags | 0 | 1 | 0x800ed610 |
| 0x7c | work.7C | 10 | 9 | 0x800ecf44, 0x800ed148, 0x800ed4b0, 0x800ed610, 0x800ed704, 0x800ed854, 0x800ed8e8 |
| 0x80 | work.80 | 4 | 6 | 0x800ed148, 0x800ed258, 0x800ed4b0, 0x800ed854, 0x800ed8e8 |
| 0x82 | self+0x82 | 4 | 6 | 0x800ed148, 0x800ed258, 0x800ed4b0, 0x800ed854, 0x800ed8e8 |
| 0x84 | work.84 | 1 | 1 | 0x800ed148, 0x800ed4b0 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800743dc | dmg_tbl_normal | 0x800ed610 |
| 0x800970a0 | mega | 0x800ed148, 0x800ed258, 0x800ed4b0, 0x800ed610 |
| 0x800970aa | mega.x.px | 0x800ed854, 0x800ed8e8 |
| 0x800971f8 | camera_layer0 | 0x800ed148, 0x800ed4b0 |
| 0x80097420 | frame_count | 0x800ed258, 0x800ed4b0, 0x800ed854, 0x800ed8e8 |
| 0x800c4560 | bg_refresh | 0x800ed258 |
| 0x800cced0 | game | 0x800ed148, 0x800ed258, 0x800ed4b0, 0x800ed950 |
| 0x800f5888 |  | 0x800ed610 |
| 0x800f588c |  | 0x800ed610 |
| 0x800f5890 |  | 0x800ed610 |
| 0x800f58d8 |  | 0x800ed610 |
| 0x800f58ec |  | 0x800ecf44 |
| 0x800f5910 |  | 0x800ed258, 0x800ed4b0 |
| 0x800f5990 |  | 0x800ed950 |
