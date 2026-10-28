# RE kit: sea-attacker-st

- Object: sea-attacker-st
- State: `turtloid-1.state.gz` (SHA-1 `da8082595c9fc59c1fff107999f384a674b9a63d`), stage 6 area 0
- Class: main, type 10; entry function 0x800eab54
- Functions walked: 22; state tables: 5

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800eae98 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ea880, 0x800ea9b0, 0x800eabcc, 0x800eae1c, 0x800eae98 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ea9b0, 0x800eacd0 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800eb1e4 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800eb2b4 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800eada0 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800eadc0 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ea880, 0x800ea9b0, 0x800eacd0 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ea880, 0x800ea9b0, 0x800eacd0 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ea9b0, 0x800eaf88 |
| 0x8002d2d4 | GetRNG | random number | 0x800eafd4 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800ea9b0 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ea9b0, 0x800eacd0 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800ea9b0, 0x800eacd0 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800eacd0 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800eacd0 |
| 0x80049d90 | SpawnHitEffect | effect (flags 0x21, type 3, var 1) at the object, mirrored when speed_x > 0; an enemy shot hitting a wall (likely) | 0x800ea9b0 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ea9b0, 0x800eacd0 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800eacd0 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800ea880 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f5c58 | 0x800eab54 | 0x800eabcc, 0x800eacd0, 0x800eada0, 0x800eadc0 |
| 0x800f5c68 | 0x800eacd0 | 0x80040290, 0x800eb1dc, 0x800eade0, 0x800eaf88 |
| 0x800f5c78 | 0x800eade0 | 0x800eae1c, 0x800eae98, 0x800eaf0c |
| 0x800f5c84 | 0x800eaf88 | 0x800eafd4, 0x800eb048, 0x800eb0b8, 0x800eb16c, 0x800eb1d4 |
| 0x800f5ca4 | 0x800eab90 | 0x800ea880, 0x800ea9b0, 0x800eb2b4 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 9 | 0x800eab90 |

## Animation tables

### Table 0x800f5c28 (10 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 15 | 20 20 | 0 0 | 0 |
| 1 | 1 7 8 8 8 7 1 1 | 6 15 2 1 3 15 5 1 | 0 0 0 1 0 0 0 2 | hold |
| 2 | 9 10 11 12 13 14 | 2 2 2 2 2 2 | 0 0 0 0 0 0 | 0 |
| 3 | 1 2 3 4 5 6 | 16 6 2 2 2 2 | 0 0 0 1 0 0 | 3 |
| 4 | 16 | 1 | 0 | hold |
| 5 | 17 | 1 | 0 | hold |
| 6 | 18 | 1 | 0 | hold |
| 7 | 19 | 1 | 0 | hold |
| 8 | 20 | 1 | 0 | hold |
| 9 | 9 10 11 21 22 | 2 2 2 2 2 | 0 0 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 1 | 0 | 0x800eabcc |
| 0x3 | display | 0 | 2 | 0x800eabcc |
| 0x4 | state | 1 | 2 | 0x800eab54, 0x800eabcc, 0x800eacd0 |
| 0x5 | step | 1 | 1 | 0x800eabcc, 0x800eacd0 |
| 0x6 | step2 | 8 | 8 | 0x800eabcc, 0x800eade0, 0x800eae1c, 0x800eae98, 0x800eaf0c, 0x800eaf88, 0x800eafd4, 0x800eb048, 0x800eb0b8, 0x800eb16c |
| 0x8 | x | 3 | 0 | 0x800eabcc, 0x800eacd0, 0x800eb16c |
| 0xa | x.px | 3 | 0 | 0x800eae1c, 0x800eb1e4, 0x800eb284 |
| 0xc | y | 2 | 0 | 0x800eabcc, 0x800eacd0 |
| 0xe | y.px | 2 | 0 | 0x800eb048, 0x800eb1e4 |
| 0x14 | layer | 1 | 1 | 0x800eabcc, 0x800eb1e4 |
| 0x15 | flip | 2 | 3 | 0x800eabcc, 0x800eb0b8, 0x800eb1e4, 0x800eb284 |
| 0x16 | priority | 0 | 2 | 0x800eabcc, 0x800eb048 |
| 0x18 | prev_x | 0 | 2 | 0x800eabcc, 0x800eacd0 |
| 0x1c | prev_y | 0 | 2 | 0x800eabcc, 0x800eacd0 |
| 0x20 | speed_x | 0 | 4 | 0x800eabcc, 0x800eb048, 0x800eb0b8, 0x800eb16c |
| 0x24 | speed_y | 0 | 4 | 0x800eabcc, 0x800eafd4, 0x800eb048, 0x800eb16c |
| 0x28 | accel_x | 0 | 1 | 0x800eabcc |
| 0x2c | gravity | 0 | 1 | 0x800eabcc |
| 0x30 | anime_table | 1 | 1 | 0x800eabcc, 0x800eb1e4 |
| 0x3c | sprite_data | 1 | 0 | 0x800eb1e4 |
| 0x40 | tex_cord | 1 | 0 | 0x800eb1e4 |
| 0x42 | clut_cord | 1 | 0 | 0x800eb1e4 |
| 0x45 | anime.flag | 2 | 0 | 0x800eae98, 0x800eafd4 |
| 0x50 | attack_box | 0 | 2 | 0x800eabcc, 0x800eafd4 |
| 0x54 | hurt_box | 0 | 2 | 0x800eabcc, 0x800eafd4 |
| 0x58 | damage_table | 0 | 1 | 0x800eabcc |
| 0x5c | hp | 0 | 1 | 0x800eabcc |
| 0x60 | contact_damage | 0 | 1 | 0x800eabcc |
| 0x61 | iframes | 0 | 1 | 0x800eabcc |
| 0x67 | latch | 0 | 1 | 0x800eabcc |
| 0x68 | terrain_box | 0 | 1 | 0x800eabcc |
| 0x7c | work.7C | 2 | 4 | 0x800eae98, 0x800eaf0c, 0x800eb048, 0x800eb0b8 |
| 0x80 | work.80 | 2 | 5 | 0x800eabcc, 0x800eaf0c, 0x800eb0b8 |
| 0x82 | self+0x82 | 1 | 1 | 0x800eafd4, 0x800eb048 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f80004c | current_main | 0x800eabcc, 0x800eaf0c, 0x800eafd4, 0x800eb048, 0x800eb0b8 |
| 0x8007429c | dmg_tbl_one_hit | 0x800ea880 |
| 0x800743dc | dmg_tbl_normal | 0x800eabcc |
| 0x800970a0 | mega | 0x800ea880 |
| 0x800970a8 | mega.x | 0x800eb16c |
| 0x800970aa | mega.x.px | 0x800ea9b0, 0x800eae1c, 0x800eb284 |
| 0x800970ae | mega.y.px | 0x800ea9b0, 0x800eb048 |
| 0x800970b4 | mega.layer | 0x800eabcc |
| 0x800ccf38 | game.difficulty | 0x800ea9b0 |
| 0x800f5b98 |  | 0x800eabcc |
| 0x800f5b9c |  | 0x800eabcc |
| 0x800f5ba0 |  | 0x800eafd4 |
| 0x800f5ba4 |  | 0x800eafd4 |
| 0x800f5c28 |  | 0x800eabcc |
| 0x800f5c50 |  | 0x800eacd0 |
| 0x800f5c98 |  | 0x800ea880 |
| 0x800f5c9c |  | 0x800ea880 |
| 0x800f5ca0 |  | 0x800ea880 |
