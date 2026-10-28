# RE kit: killer-moth

- Object: killer-moth
- State: `yammark-1_start.state.gz` (SHA-1 `697358bffd35fe39ff9233a880cf24d12b886b6c`), stage 1 area 0
- Class: main, type 4; entry function 0x800eb2c8
- Functions walked: 32; state tables: 8

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800eb870 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800eb37c, 0x800eb5f8, 0x800eb6c4, 0x800eb764, 0x800eb7e4, 0x800eb9c8, 0x800ebc50, 0x800ebda0, 0x800ebe14 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800eb490, 0x800ebcec, 0x800ebe14 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800ebb70 |
| 0x8002c530 | GetMiscObject | free misc object (debris) | 0x800ebabc |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ebd80, 0x800ebf4c |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800eb57c |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800eb59c |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800eb490, 0x800ebcec |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800eb170, 0x800eb490, 0x800ebcec |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800eae94, 0x800eb170, 0x800ebcec |
| 0x8002d2d4 | GetRNG | random number | 0x800eb0a0, 0x800eb9c8 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800ebcec |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800eb490, 0x800ebcec |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800eb490 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800eb490 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800eb490 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800eb490 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800eae94, 0x800eb170, 0x800eb870 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800eae94, 0x800eb170, 0x800eb870 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800eae94, 0x800eb170 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f7254 | 0x800eb2c8 | 0x800eb37c, 0x800eb490, 0x800eb57c, 0x800eb59c |
| 0x800f7264 | 0x800eb490 | 0x80040290, 0x800eb984, 0x800eb5bc, 0x800eb728, 0x800eb7a8, 0x800eb98c |
| 0x800f727c | 0x800eb5bc | 0x800eb5f8, 0x800eb644, 0x800eb6c4, 0x800eb708 |
| 0x800f728c | 0x800eb728 | 0x800eb764, 0x800eae94 |
| 0x800f7294 | 0x800eb7a8 | 0x800eb7e4, 0x800eb870 |
| 0x800f72ec | 0x800eb98c | 0x800eb0a0, 0x800eb9c8, 0x800eb170 |
| 0x800f7300 | 0x800eb304 | 0x800ebc50, 0x800ebcec, 0x800ebd80 |
| 0x800f730c | 0x800eb340 | 0x800ebda0, 0x800ebe14, 0x800ebf4c |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 2 | 0x800eb304 |
| misc | 4 | 0x800eb340 |

## Animation tables

### Table 0x800f7234 (8 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 | 1 | 0 | hold |
| 1 | 11 12 13 14 15 37 | 1 2 2 1 1 2 | 0 0 0 0 0 0 | 0 |
| 2 | 1 1 | 7 1 | 0 1 | hold |
| 3 | 16 17 18 19 20 37 | 1 2 2 1 1 2 | 0 0 0 0 0 0 | 0 |
| 4 | 2 | 4 | 0 | hold |
| 5 | 21 22 23 24 25 37 | 1 2 2 1 1 2 | 0 0 0 0 0 0 | 0 |
| 6 | 0 26 27 28 29 30 31 32 33 34 35 36 | 3 3 3 3 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 7 | 3 4 5 6 7 8 9 10 37 | 4 4 4 4 4 4 4 4 4 | 0 0 0 0 0 0 0 0 0 | 0 |

Note: cut to the export's 8 animations; 4 more pointer-like words follow (often the next table)

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 1 | 0 | 0x800eb37c |
| 0x3 | display | 0 | 1 | 0x800eb37c |
| 0x4 | state | 1 | 2 | 0x800eb2c8, 0x800eb37c, 0x800eb490 |
| 0x5 | step | 3 | 5 | 0x800eae94, 0x800eb170, 0x800eb37c, 0x800eb490, 0x800eb708 |
| 0x6 | step2 | 12 | 12 | 0x800eae94, 0x800eb0a0, 0x800eb170, 0x800eb37c, 0x800eb5bc, 0x800eb5f8, 0x800eb644, 0x800eb6c4, 0x800eb708, 0x800eb728, 0x800eb764, 0x800eb7a8, 0x800eb7e4, 0x800eb98c, 0x800eb9c8 |
| 0x8 | x | 2 | 0 | 0x800eb37c, 0x800eb490 |
| 0xa | x.px | 12 | 4 | 0x800eae94, 0x800eb0a0, 0x800eb170, 0x800eb644, 0x800eb7e4, 0x800eb9c8, 0x800ebabc, 0x800ebb70, 0x800ebc20 |
| 0xc | y | 2 | 0 | 0x800eb37c, 0x800eb490 |
| 0xe | y.px | 11 | 3 | 0x800eae94, 0x800eb0a0, 0x800eb170, 0x800eb7e4, 0x800eb9c8, 0x800ebabc, 0x800ebb70 |
| 0x14 | layer | 2 | 1 | 0x800eb37c, 0x800ebabc, 0x800ebb70 |
| 0x15 | flip | 3 | 3 | 0x800eae94, 0x800eb37c, 0x800ebabc, 0x800ebb70, 0x800ebc20 |
| 0x16 | priority | 0 | 1 | 0x800eb37c |
| 0x18 | prev_x | 0 | 2 | 0x800eb37c, 0x800eb490 |
| 0x1c | prev_y | 0 | 2 | 0x800eb37c, 0x800eb490 |
| 0x20 | speed_x | 0 | 3 | 0x800eae94, 0x800eb170, 0x800eb37c |
| 0x24 | speed_y | 0 | 3 | 0x800eae94, 0x800eb170, 0x800eb37c |
| 0x28 | accel_x | 0 | 3 | 0x800eae94, 0x800eb170, 0x800eb37c |
| 0x2c | gravity | 0 | 3 | 0x800eae94, 0x800eb170, 0x800eb37c |
| 0x30 | anime_table | 2 | 1 | 0x800eb37c, 0x800ebabc, 0x800ebb70 |
| 0x3c | sprite_data | 2 | 0 | 0x800ebabc, 0x800ebb70 |
| 0x40 | tex_cord | 2 | 0 | 0x800ebabc, 0x800ebb70 |
| 0x42 | clut_cord | 2 | 0 | 0x800ebabc, 0x800ebb70 |
| 0x45 | anime.flag | 1 | 0 | 0x800eb708 |
| 0x50 | attack_box | 0 | 3 | 0x800eb37c, 0x800eb9c8 |
| 0x54 | hurt_box | 0 | 3 | 0x800eb37c, 0x800eb9c8 |
| 0x58 | damage_table | 0 | 1 | 0x800eb37c |
| 0x5c | hp | 0 | 1 | 0x800eb37c |
| 0x60 | contact_damage | 0 | 1 | 0x800eb37c |
| 0x61 | iframes | 0 | 1 | 0x800eb37c |
| 0x67 | latch | 0 | 1 | 0x800eb37c |
| 0x68 | terrain_box | 0 | 2 | 0x800eb37c |
| 0x7c | work.7C | 2 | 6 | 0x800eb0a0, 0x800eb37c, 0x800eb5f8, 0x800eb644, 0x800eb7e4, 0x800eb9c8 |
| 0x7e | work.7E | 0 | 1 | 0x800eb7e4 |
| 0x84 | work.84 | 0 | 1 | 0x800eae94 |
| 0x88 | work.88 | 0 | 5 | 0x800eb37c, 0x800eb5f8, 0x800eb6c4, 0x800eb764 |
| 0x8a | work.8A | 2 | 0 | 0x800eb170 |
| 0x8c | work.8C | 2 | 0 | 0x800eb170 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f80004c | current_main | 0x800eae94, 0x800eb170, 0x800eb37c, 0x800eb5f8, 0x800eb6c4, 0x800eb764 |
| 0x1f800064 | current_misc | 0x800ebe14 |
| 0x800743dc | dmg_tbl_normal | 0x800eb37c |
| 0x800970a0 | mega | 0x800eae94 |
| 0x800970aa | mega.x.px | 0x800eb644, 0x800ebc20 |
| 0x800970b4 | mega.layer | 0x800eb37c |
| 0x800971f8 | camera_layer0 | 0x800eb0a0, 0x800eb9c8 |
| 0x800f7180 |  | 0x800eb37c, 0x800eb9c8 |
| 0x800f7184 |  | 0x800eb37c, 0x800eb9c8 |
| 0x800f7234 |  | 0x800eb37c |
| 0x800f729c |  | 0x800eb0a0 |
| 0x800f72ac |  | 0x800eb9c8 |
| 0x800f72f8 |  | 0x800ebc50 |
| 0x800f72fc |  | 0x800ebc50 |
