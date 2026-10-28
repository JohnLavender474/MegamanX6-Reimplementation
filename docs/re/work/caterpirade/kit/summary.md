# RE kit: caterpirade

- Object: caterpirade
- State: `yammark-3_caterpirade.state.gz` (SHA-1 `eb47e7dea70a84414b2e9ee94b8fe0e26cc9ae24`), stage 1 area 0
- Class: stage, type 1; entry function 0x800ec300
- Functions walked: 17; state tables: 4

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800ebf6c, 0x800ec800 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ec368, 0x800ec4bc, 0x800ec564, 0x800ec66c, 0x800ec738, 0x800ec800, 0x800ec8a0 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ec144, 0x800ec520, 0x800ec66c, 0x800ec738, 0x800ec7a4, 0x800ec800, 0x800ec8a0 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ec454 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ec434 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ebf6c |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ebf6c |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ec144, 0x800ec474, 0x800ec5f4, 0x800ec738, 0x800ec7a4, 0x800ec800, 0x800ec8a0 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ebf6c |
| 0x80030ccc | TerrainPointCheck | TerrainPointCheck(obj, x, y): non-zero when the point is solid (rectangle list first, then 0x80030234) | 0x800ec144 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ebf6c |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800ebf6c |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ebf6c |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ebf6c |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x80031da8 | 0x800ebf6c |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f7404 | 0x800ec300 | 0x800ec368, 0x800ebf6c, 0x800ec454, 0x800ec434 |
| 0x800f7414 | 0x800ebf6c | 0x800ec144, 0x800ec474, 0x800ec5f4, 0x800ec66c, 0x800ec6fc |
| 0x800f7428 | 0x800ec474 | 0x800ec4bc, 0x800ec520, 0x800ec564 |
| 0x800f7434 | 0x800ec6fc | 0x800ec738, 0x800ec7a4, 0x800ec800, 0x800ec8a0 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f73d4 (6 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 | 12 12 12 | 0 0 0 | 0 |
| 1 | 3 4 4 | 4 1 1 | 0 0 1 | hold |
| 2 | 5 6 7 8 9 10 11 12 13 14 15 16 16 | 2 2 2 2 2 2 2 2 2 2 3 1 1 | 0 0 0 0 0 0 0 0 0 0 0 0 1 | hold |
| 3 | 16 17 18 19 20 21 22 23 | 8 8 8 8 8 8 8 8 | 0 0 0 0 0 0 0 0 | 0 |
| 4 | 15 14 13 12 11 10 9 8 7 6 5 5 | 2 2 2 2 2 2 2 2 2 2 1 1 | 0 0 0 0 0 0 0 0 0 0 0 1 | hold |
| 5 | 16 17 18 19 20 21 22 23 | 8 8 8 8 8 10 12 1 | 0 0 0 0 0 0 0 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x4 | state | 2 | 3 | 0x800ebf6c, 0x800ec300, 0x800ec368 |
| 0x5 | step | 0 | 1 | 0x800ec368 |
| 0x6 | step2 | 2 | 7 | 0x800ebf6c, 0x800ec144, 0x800ec368, 0x800ec564, 0x800ec5f4, 0x800ec66c, 0x800ec8a0 |
| 0x7 | act4 | 7 | 9 | 0x800ebf6c, 0x800ec368, 0x800ec474, 0x800ec4bc, 0x800ec520, 0x800ec564, 0x800ec6fc, 0x800ec738, 0x800ec7a4, 0x800ec800, 0x800ec8a0 |
| 0x8 | x | 3 | 0 | 0x800ec300, 0x800ec564, 0x800ec5f4 |
| 0xa | x.px | 2 | 0 | 0x800ec144 |
| 0xc | y | 1 | 0 | 0x800ec300 |
| 0xe | y.px | 1 | 0 | 0x800ec144 |
| 0x15 | flip | 3 | 6 | 0x800ec144, 0x800ec368, 0x800ec564, 0x800ec5f4, 0x800ec66c |
| 0x16 | priority | 0 | 1 | 0x800ec368 |
| 0x18 | prev_x | 0 | 1 | 0x800ec300 |
| 0x1c | prev_y | 0 | 1 | 0x800ec300 |
| 0x20 | speed_x | 1 | 8 | 0x800ebf6c, 0x800ec368, 0x800ec564, 0x800ec5f4, 0x800ec66c, 0x800ec7a4 |
| 0x24 | speed_y | 1 | 2 | 0x800ebf6c, 0x800ec300, 0x800ec368 |
| 0x28 | accel_x | 0 | 4 | 0x800ebf6c, 0x800ec368, 0x800ec738, 0x800ec7a4 |
| 0x2c | gravity | 0 | 1 | 0x800ec368 |
| 0x30 | anime_table | 0 | 1 | 0x800ec368 |
| 0x42 | clut_cord | 2 | 2 | 0x800ebf6c |
| 0x45 | anime.flag | 3 | 0 | 0x800ec520, 0x800ec66c, 0x800ec8a0 |
| 0x47 | anime.sprite | 1 | 1 | 0x800ebf6c, 0x800ec800 |
| 0x50 | attack_box | 0 | 3 | 0x800ec368, 0x800ec4bc, 0x800ec800 |
| 0x54 | hurt_box | 0 | 3 | 0x800ec368, 0x800ec4bc, 0x800ec800 |
| 0x58 | damage_table | 0 | 3 | 0x800ec368, 0x800ec4bc, 0x800ec800 |
| 0x5c | hp | 2 | 1 | 0x800ebf6c, 0x800ec368 |
| 0x60 | contact_damage | 0 | 1 | 0x800ec368 |
| 0x61 | iframes | 0 | 1 | 0x800ec368 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800ec368 |
| 0x67 | latch | 0 | 4 | 0x800ebf6c, 0x800ec368, 0x800ec5f4 |
| 0x68 | terrain_box | 2 | 3 | 0x800ec144, 0x800ec368, 0x800ec4bc, 0x800ec800 |
| 0x70 | collide_flags | 5 | 0 | 0x800ebf6c, 0x800ec144, 0x800ec300, 0x800ec5f4 |
| 0x72 | self+0x72 | 3 | 0 | 0x800ebf6c |
| 0x75 | self+0x75 | 0 | 1 | 0x800ec368 |
| 0x80 | work.80 | 4 | 9 | 0x800ec144, 0x800ec368, 0x800ec520, 0x800ec564, 0x800ec66c, 0x800ec738, 0x800ec7a4 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x8007433c | dmg_tbl_deflect | 0x800ec4bc |
| 0x800743dc | dmg_tbl_normal | 0x800ec368, 0x800ec800 |
| 0x800970a0 | mega | 0x800ebf6c |
| 0x800970a8 | mega.x | 0x800ec564, 0x800ec5f4 |
| 0x800970b4 | mega.layer | 0x800ec368 |
| 0x800f73d4 |  | 0x800ec368 |
| 0x800f73ec |  | 0x800ec368, 0x800ec800 |
| 0x800f73f0 |  | 0x800ec4bc |
| 0x800f73f4 |  | 0x800ec368, 0x800ec800 |
| 0x800f73f8 |  | 0x800ec4bc |
| 0x800f73fc |  | 0x800ec368, 0x800ec800 |
| 0x800f7400 |  | 0x800ec4bc |
