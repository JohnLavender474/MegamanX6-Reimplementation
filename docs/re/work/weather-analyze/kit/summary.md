# RE kit: weather-analyze

- Object: weather-analyze
- State: `turtloid-1.state.gz` (SHA-1 `da8082595c9fc59c1fff107999f384a674b9a63d`), stage 6 area 0
- Class: main, type 14; entry function 0x800eb5f8
- Functions walked: 21; state tables: 6

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800eb2d4, 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebbd4, 0x800ebd74, 0x800ebe60 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800eb2d4, 0x800eb974, 0x800ebbd4, 0x800ebe60 |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800eb2d4 |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x800eb7e0 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800eba50, 0x800ebcd4, 0x800ebf64 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800eba50, 0x800ebcd4, 0x800ebf64 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800eb2d4, 0x800eb974, 0x800ebbd4, 0x800ebe60 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800eb2d4, 0x800eb89c, 0x800eb8f0, 0x800eb974, 0x800ebbd4, 0x800ebe60 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800eb2d4, 0x800eb974, 0x800ebbd4, 0x800ebe60 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800eb5f8, 0x800eb914, 0x800eba90, 0x800ebd14 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800eb2d4, 0x800eb974, 0x800ebe60 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800eb2d4, 0x800eb974 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800eb2d4, 0x800eb974 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800eb2d4, 0x800eb974 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800eb2d4, 0x800eb974 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f5de4 | 0x800eb5f8 | 0x800eb6c0, 0x800eb2d4, 0x800eb89c, 0x800eb8f0 |
| 0x800f5ebc | 0x800eb2d4 | 0x80040290, 0x800eb7d8, 0x800eb7e0 |
| 0x800f5ec8 | 0x800eb914 | 0x800eb4d4, 0x800eb974, 0x800eba50, 0x800eba50 |
| 0x800f5ed8 | 0x800eb974 | 0x80040290, 0x800eba40, 0x800eba48 |
| 0x800f5ee4 | 0x800eba90 | 0x800ebaf0, 0x800ebbd4, 0x800ebcd4, 0x800ebcd4 |
| 0x800f5ef4 | 0x800ebd14 | 0x800ebd74, 0x800ebe60, 0x800ebf64, 0x800ebf64 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f5d9c (18 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 | 4 4 4 | 0 0 0 | 0 |
| 1 | 4 5 | 14 14 | 0 0 | 0 |
| 2 | 19 20 | 14 14 | 0 0 | 0 |
| 3 | 21 22 | 14 14 | 0 0 | 0 |
| 4 | 23 24 | 14 14 | 0 0 | 0 |
| 5 | 6 18 7 18 8 18 9 18 | 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 | 0 |
| 6 | 10 18 11 18 12 18 13 18 | 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 | 0 |
| 7 | 14 18 15 18 16 18 17 18 | 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 | 0 |
| 8 | 25 18 26 18 27 18 28 18 | 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 | 0 |
| 9 | 3 | 10 | 0 | hold |
| 10 | 34 | 10 | 0 | hold |
| 11 | 35 | 10 | 0 | hold |
| 12 | 36 | 10 | 0 | hold |
| 13 | 29 | 10 | 0 | hold |
| 14 | 30 | 10 | 0 | hold |
| 15 | 31 | 10 | 0 | hold |
| 16 | 32 | 10 | 0 | hold |
| 17 | 33 | 10 | 0 | hold |

Note: cut to the export's 18 animations; 4 more pointer-like words follow (often the next table)

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 1 | 0 | 0x800eb7e0 |
| 0x1 | type | 1 | 0 | 0x800eb7e0 |
| 0x2 | var | 3 | 1 | 0x800eb4d4, 0x800eb5f8, 0x800eb6c0 |
| 0x3 | display | 0 | 8 | 0x800eb4d4, 0x800eb5f8, 0x800eb6c0, 0x800eb914, 0x800eba90, 0x800ebaf0, 0x800ebd14, 0x800ebd74 |
| 0x4 | state | 12 | 9 | 0x800eb2d4, 0x800eb4d4, 0x800eb5f8, 0x800eb6c0, 0x800eb89c, 0x800eb914, 0x800eb974, 0x800eba50, 0x800eba90, 0x800ebaf0, 0x800ebbd4, 0x800ebcd4, 0x800ebd14, 0x800ebd74, 0x800ebe60, 0x800ebf64 |
| 0x5 | step | 2 | 4 | 0x800eb2d4, 0x800eb4d4, 0x800eb6c0, 0x800eb974, 0x800ebaf0, 0x800ebd74 |
| 0x6 | step2 | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x7 | act4 | 2 | 2 | 0x800eb6c0, 0x800eb7e0 |
| 0x8 | x | 4 | 0 | 0x800eb5f8, 0x800eb914, 0x800eba90, 0x800ebd14 |
| 0xa | x.px | 2 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0xc | y | 4 | 0 | 0x800eb5f8, 0x800eb914, 0x800eba90, 0x800ebd14 |
| 0xe | y.px | 2 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x14 | layer | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x15 | flip | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x16 | priority | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x17 | anime | 6 | 0 | 0x800ebbd4, 0x800ebe60 |
| 0x18 | prev_x | 0 | 4 | 0x800eb5f8, 0x800eb914, 0x800eba90, 0x800ebd14 |
| 0x1c | prev_y | 0 | 4 | 0x800eb5f8, 0x800eb914, 0x800eba90, 0x800ebd14 |
| 0x20 | speed_x | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x24 | speed_y | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x28 | accel_x | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x2c | gravity | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x30 | anime_table | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x3c | sprite_data | 0 | 3 | 0x800eb4d4, 0x800ebaf0, 0x800ebd74 |
| 0x40 | tex_cord | 0 | 3 | 0x800eb4d4, 0x800ebaf0, 0x800ebd74 |
| 0x42 | clut_cord | 1 | 4 | 0x800eb2d4, 0x800eb4d4, 0x800ebaf0, 0x800ebd74 |
| 0x50 | attack_box | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x54 | hurt_box | 1 | 5 | 0x800eb4d4, 0x800eb6c0, 0x800eb7e0, 0x800ebaf0, 0x800ebd74 |
| 0x58 | damage_table | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x5c | hp | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x60 | contact_damage | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x61 | iframes | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x67 | latch | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x68 | terrain_box | 0 | 4 | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x70 | collide_flags | 1 | 0 | 0x800eb7e0 |
| 0x80 | work.80 | 6 | 0 | 0x800eb4d4, 0x800eb974, 0x800ebaf0, 0x800ebbd4, 0x800ebd74, 0x800ebe60 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800743dc | dmg_tbl_normal | 0x800eb4d4, 0x800eb6c0 |
| 0x800970aa | mega.x.px | 0x800eb2d4 |
| 0x800970b4 | mega.layer | 0x800eb4d4, 0x800eb6c0, 0x800ebaf0, 0x800ebd74 |
| 0x800971f8 | camera_layer0 | 0x800eb2d4 |
| 0x8009721c | camera_layer0+0x24 | 0x800eb2d4 |
| 0x80097222 | camera_layer0+0x2a | 0x800eb2d4 |
| 0x800cced0 | game | 0x800eb2d4 |
| 0x800ccef6 | game.stage+0x1a | 0x800eb6c0 |
| 0x800ccef8 | game.stage+0x1c | 0x800eb89c |
| 0x800f5cb0 |  | 0x800eb6c0 |
| 0x800f5cb4 |  | 0x800eb7e0 |
| 0x800f5cb8 |  | 0x800eb4d4 |
| 0x800f5cbc |  | 0x800eb4d4 |
| 0x800f5cc0 |  | 0x800ebd74 |
| 0x800f5d94 |  | 0x800eb974 |
| 0x800f5d98 |  | 0x800eb2d4 |
| 0x800f5d9c |  | 0x800eb6c0 |
| 0x800f5df4 |  | 0x800eb6c0 |
| 0x800f5e10 |  | 0x800eb4d4 |
| 0x800f5e80 |  | 0x800eb2d4 |
| 0x800f5e90 |  | 0x800eb2d4 |
| 0x800f5eac |  | 0x800eb2d4 |
| 0x800f6ba0 |  | 0x800eb2d4, 0x800eb6c0, 0x800eb89c |
