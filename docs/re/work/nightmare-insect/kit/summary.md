# RE kit: nightmare-insect

- Object: nightmare-insect
- State: `yammark-1_start.state.gz` (SHA-1 `697358bffd35fe39ff9233a880cf24d12b886b6c`), stage 1 area 0
- Class: main, type 23; entry function 0x800ed704
- Functions walked: 37; state tables: 10

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800ecdd4 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ec8f0, 0x800ecc38, 0x800ecdd4, 0x800ed760, 0x800ed924, 0x800ed984, 0x800eda84, 0x800edacc, 0x800edbd4, 0x800edc24, 0x800edc6c, 0x800edca4 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800eca2c, 0x800ecc38, 0x800ed020, 0x800ed0f4, 0x800ed220, 0x800ed2f4, 0x800ed428, 0x800ed924, 0x800ed9b8, 0x800eda84, 0x800edb00, 0x800edbd4, 0x800edc24, 0x800edca4, 0x800edcec, 0x800edd88, 0x800ede78, 0x800edf30, 0x800edff8 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800ecc38, 0x800ed9b8, 0x800edb00 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ed584, 0x800edd0c, 0x800edf30 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ed830 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ed830 |
| 0x8002cb50 | DisplayCheck1 | display = 1 if inside [-32, W+32) x [-32, 272) | 0x800ed584 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ec8f0, 0x800ed584, 0x800edf30 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ec8f0 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ed020, 0x800ed0f4, 0x800ed220, 0x800ed2f4, 0x800ed428, 0x800edf30, 0x800edff8 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ed704, 0x800edd2c |
| 0x80030ccc | TerrainPointCheck | TerrainPointCheck(obj, x, y): non-zero when the point is solid (rectangle list first, then 0x80030234) | 0x800ed428 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ec8f0, 0x800ed584 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800ec8f0, 0x800ed584 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ec8f0 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ec8f0, 0x800ed584, 0x800ee080 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ec8f0, 0x800ed584, 0x800ee080 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ec8f0 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800ecdd4, 0x800ed0f4, 0x800ed2f4, 0x800edd88, 0x800ede78 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800ecdd4, 0x800ed0f4, 0x800ed2f4, 0x800edd88, 0x800ede78 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800ecdd4, 0x800ed0f4, 0x800ed2f4, 0x800edd88, 0x800ede78 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f7698 | 0x800ed704 | 0x800ed760, 0x800ec8f0, 0x800ed830, 0x800ed830 |
| 0x800f76a8 | 0x800ec8f0 | 0x80040290, 0x800eca2c, 0x800ed870, 0x800ed8ac, 0x800ed8e8, 0x800ed924 |
| 0x800f76c0 | 0x800ed870 | 0x800ed984, 0x800ed9b8, 0x800eda84 |
| 0x800f76cc | 0x800ed8ac | 0x800edacc, 0x800edb00, 0x800edbd4, 0x800edc24 |
| 0x800f76dc | 0x800ed8e8 | 0x800edc6c, 0x800ecc38, 0x800edca4, 0x800edcec |
| 0x800f7704 | 0x800edd2c | 0x800ecdd4, 0x800ed584, 0x800edd0c |
| 0x800f7710 | 0x800edf80 | 0x800ed020, 0x800edd88, 0x800ed0f4, 0x800ede78, 0x800edf30 |
| 0x800f7724 | 0x800edfbc | 0x800ed220, 0x800ed2f4 |
| 0x800f772c | 0x800ee118 | 0x800edff8, 0x800ed428, 0x800ee080 |
| 0x800f7738 | 0x800ed584 | 0x800edf80, 0x800edfbc, 0x800ee118 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 27 | 0x800edd2c |

## Animation tables

### Table 0x800f7650 (16 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 | 4 4 4 4 | 0 0 0 0 | 0 |
| 1 | 4 5 6 7 8 9 10 11 12 13 14 15 16 16 17 18 19 4 5 6 7 | 4 4 4 4 4 4 4 4 4 4 4 4 1 3 4 4 4 4 4 4 4 | 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 2 | hold |
| 2 | 4 5 6 7 7 | 4 4 4 3 1 | 0 0 0 0 2 | hold |
| 3 | 4 5 6 7 8 9 10 11 20 21 22 23 | 4 4 4 4 4 4 4 4 4 4 4 4 | 0 0 0 0 0 0 0 0 0 0 0 1 | 5 |
| 4 | 8 9 10 11 4 5 6 7 7 | 4 4 4 4 4 4 4 3 1 | 0 0 0 0 0 0 0 0 2 | hold |
| 5 | 37 38 37 38 37 38 37 38 37 38 37 38 37 39 37 39 37 39 37 39 39 | 120 10 10 10 10 6 6 6 6 2 2 2 2 2 2 2 2 2 2 1 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 | hold |
| 6 | 24 25 26 27 28 | 6 6 6 6 6 | 0 0 0 0 0 | 0 |
| 7 | 29 30 31 32 33 34 35 36 | 1 1 1 1 1 1 1 1 | 0 0 0 0 0 0 0 0 | 0 |
| 8 | 40 | 1 | 0 | hold |
| 9 | 41 | 1 | 0 | hold |
| 10 | 42 | 1 | 0 | hold |
| 11 | 43 | 1 | 0 | hold |
| 12 | 44 | 1 | 0 | hold |
| 13 | 45 | 1 | 0 | hold |
| 14 | 4 5 6 7 8 9 10 11 12 13 14 15 16 16 17 18 19 | 4 4 4 4 4 4 4 4 4 4 4 4 1 4 4 4 4 | 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 | 13 |
| 15 | 37 38 37 38 37 38 37 38 37 38 37 38 37 39 37 39 37 39 37 39 | 60 5 5 5 5 3 3 3 3 1 1 1 1 1 1 1 1 1 1 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x3 | display | 1 | 0 | 0x800eca2c |
| 0x4 | state | 2 | 2 | 0x800ec8f0, 0x800ed704, 0x800ed760, 0x800ed830 |
| 0x5 | step | 1 | 10 | 0x800ec8f0, 0x800eca2c, 0x800ed760, 0x800ed924, 0x800eda84, 0x800edc24, 0x800edca4 |
| 0x6 | step2 | 10 | 8 | 0x800eca2c, 0x800ecc38, 0x800ed870, 0x800ed8ac, 0x800ed8e8, 0x800ed984, 0x800ed9b8, 0x800edacc, 0x800edb00, 0x800edbd4, 0x800edc6c |
| 0x7 | act4 | 1 | 2 | 0x800ed760, 0x800edbd4 |
| 0x8 | x | 6 | 0 | 0x800eca2c, 0x800ecc38, 0x800ed704, 0x800ed760, 0x800ed9b8, 0x800edb00 |
| 0xa | x.px | 2 | 0 | 0x800eca2c |
| 0xc | y | 4 | 0 | 0x800ecc38, 0x800ed704, 0x800ed9b8, 0x800edb00 |
| 0x14 | layer | 3 | 1 | 0x800ecc38, 0x800ed760, 0x800ed9b8, 0x800edb00 |
| 0x15 | flip | 3 | 4 | 0x800eca2c, 0x800ecc38, 0x800ed760, 0x800ed9b8, 0x800edb00 |
| 0x16 | priority | 0 | 1 | 0x800ed760 |
| 0x18 | prev_x | 0 | 1 | 0x800ed704 |
| 0x1c | prev_y | 0 | 1 | 0x800ed704 |
| 0x20 | speed_x | 0 | 1 | 0x800ed760 |
| 0x24 | speed_y | 0 | 1 | 0x800ed760 |
| 0x30 | anime_table | 3 | 1 | 0x800ecc38, 0x800ed760, 0x800ed9b8, 0x800edb00 |
| 0x3c | sprite_data | 3 | 0 | 0x800ecc38, 0x800ed9b8, 0x800edb00 |
| 0x40 | tex_cord | 3 | 0 | 0x800ecc38, 0x800ed9b8, 0x800edb00 |
| 0x42 | clut_cord | 3 | 0 | 0x800ecc38, 0x800ed9b8, 0x800edb00 |
| 0x45 | anime.flag | 6 | 0 | 0x800ed924, 0x800ed9b8, 0x800eda84, 0x800edb00, 0x800edc24, 0x800edca4 |
| 0x50 | attack_box | 0 | 3 | 0x800ec8f0, 0x800ed760, 0x800ed924 |
| 0x54 | hurt_box | 0 | 3 | 0x800ec8f0, 0x800ed760, 0x800ed924 |
| 0x58 | damage_table | 0 | 1 | 0x800ed760 |
| 0x5c | hp | 0 | 2 | 0x800ec8f0, 0x800ed760 |
| 0x60 | contact_damage | 0 | 1 | 0x800ed760 |
| 0x61 | iframes | 0 | 1 | 0x800ed760 |
| 0x68 | terrain_box | 0 | 1 | 0x800ed760 |
| 0x70 | collide_flags | 0 | 1 | 0x800ed760 |
| 0x7c | work.7C | 3 | 8 | 0x800eca2c, 0x800ecc38, 0x800ed760, 0x800ed924, 0x800eda84, 0x800edc24, 0x800edc6c, 0x800edca4 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800741fc | dmg_tbl_minor | 0x800ecdd4 |
| 0x8007433c | dmg_tbl_deflect | 0x800ecdd4 |
| 0x8007479c | dmg_tbl_nightmare_insect | 0x800ed760 |
| 0x80093c98 | visual_objects | 0x800eca2c, 0x800ecc38 |
| 0x800970a0 | mega | 0x800eca2c, 0x800ecdd4, 0x800ed020, 0x800ed0f4, 0x800ed220, 0x800ed760, 0x800edd88, 0x800ede78 |
| 0x800971f8 | camera_layer0 | 0x800ed584 |
| 0x800ccf38 | game.difficulty | 0x800ec8f0, 0x800ecdd4, 0x800edd88, 0x800edff8 |
| 0x800f7448 |  | 0x800ed760, 0x800ed924 |
| 0x800f744c |  | 0x800ed760, 0x800ed924 |
| 0x800f7650 |  | 0x800ed760 |
| 0x800f7690 |  | 0x800ec8f0 |
| 0x800f7694 |  | 0x800ed584, 0x800ee080 |
| 0x800f76ec |  | 0x800ecdd4 |
| 0x800f76f0 |  | 0x800ecdd4 |
| 0x800f76f4 |  | 0x800ecdd4 |
| 0x800f76f8 |  | 0x800ecdd4 |
| 0x800f76fc |  | 0x800ecdd4 |
| 0x800f7700 |  | 0x800ee080 |
