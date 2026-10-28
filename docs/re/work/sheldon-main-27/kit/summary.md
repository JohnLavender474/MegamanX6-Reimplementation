# RE kit: sheldon-main-27

- Object: sheldon-main-27
- State: `sheldon-6_tile-shown.state.gz` (SHA-1 `30a84a4ee7ae3b8cb97d2351487bac96270b9ef3`), stage 7 area 0
- Class: main, type 27; entry function 0x800f1fd8
- Functions walked: 17; state tables: 3

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800f21f8 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800f1748, 0x800f1884, 0x800f1a24, 0x800f1c10, 0x800f1d4c, 0x800f20b4, 0x800f2150, 0x800f21f8, 0x800f22fc, 0x800f2460 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800f1c10, 0x800f20b4, 0x800f2150, 0x800f21f8, 0x800f22fc, 0x800f2518 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800f21f8 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800f25e0 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800f23e4 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800f23e4 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800f1e8c, 0x800f2014, 0x800f2518 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800f1e8c, 0x800f2014, 0x800f2518 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800f20b4, 0x800f2150 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800f22fc, 0x800f2518 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800f2518 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800f1e8c |
| 0x80030ccc | TerrainPointCheck | TerrainPointCheck(obj, x, y): non-zero when the point is solid (rectangle list first, then 0x80030234) | 0x800f1884, 0x800f2014 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800f1e8c, 0x800f2518 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800f1e8c, 0x800f2518 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800f1e8c |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800f1e8c |
| 0x80049d90 | SpawnHitEffect | effect (flags 0x21, type 3, var 1) at the object, mirrored when speed_x > 0; an enemy shot hitting a wall (likely) | 0x800f2518 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800f1e8c |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800f1e8c |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fc634 | 0x800f1e8c | 0x80040290, 0x800f22fc, 0x800f2150, 0x800f1c10, 0x800f21f8, 0x800f20b4 |
| 0x800fc64c | 0x800f1fd8 | 0x800f1d4c, 0x800f1e8c, 0x800f23e4, 0x800f2014, 0x800f23e4 |
| 0x800fc668 | 0x800f2424 | 0x800f2460, 0x800f2518, 0x800f25e0 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 32 | 0x800f2424 |

## Animation tables

### Table 0x800fc5f8 (14 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 | 1 | 0 | hold |
| 1 | 1 2 3 4 5 6 7 8 9 10 | 8 8 8 3 5 8 8 8 3 5 | 0 0 0 0 0 0 0 0 0 0 | 0 |
| 2 | 0 11 12 13 14 14 | 6 10 2 2 3 1 | 0 0 1 0 0 0 | hold |
| 3 | 15 16 | 2 2 | 0 0 | 0 |
| 4 | 17 11 18 0 18 18 | 3 10 2 3 2 1 | 0 0 0 0 0 0 | hold |
| 5 | 0 18 19 20 21 22 23 24 23 24 | 6 1 2 2 1 2 2 2 2 1 | 0 0 0 1 0 0 0 0 0 0 | hold |
| 6 | 24 23 22 21 20 19 18 0 18 0 | 6 1 2 2 1 2 2 2 2 1 | 0 0 0 0 0 0 0 0 0 0 | hold |
| 7 | 0 25 26 27 28 29 0 0 | 6 6 8 2 2 2 10 1 | 0 0 0 1 0 0 0 0 | hold |
| 8 | 30 31 32 | 2 2 2 | 0 0 0 | 0 |
| 9 | 33 | 1 | 0 | hold |
| 10 | 34 | 1 | 0 | hold |
| 11 | 35 | 1 | 0 | hold |
| 12 | 36 | 1 | 0 | hold |
| 13 | 37 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 3 | 1 | 0x800f1884, 0x800f1d4c, 0x800f2014 |
| 0x3 | display | 0 | 1 | 0x800f1e8c |
| 0x4 | state | 5 | 6 | 0x800f1d4c, 0x800f1e8c, 0x800f1fd8, 0x800f2014, 0x800f23e4 |
| 0x5 | step | 1 | 10 | 0x800f1748, 0x800f1884, 0x800f1a24, 0x800f1c10, 0x800f1d4c, 0x800f1e8c, 0x800f2014, 0x800f20b4, 0x800f2150, 0x800f21f8 |
| 0x6 | step2 | 10 | 13 | 0x800f1748, 0x800f1884, 0x800f1c10, 0x800f20b4, 0x800f2150, 0x800f21f8, 0x800f22fc |
| 0x8 | x | 3 | 0 | 0x800f1d4c, 0x800f1e8c, 0x800f21f8 |
| 0xa | x.px | 6 | 1 | 0x800f1748, 0x800f1884, 0x800f1d4c, 0x800f2014 |
| 0xc | y | 3 | 0 | 0x800f1d4c, 0x800f1e8c, 0x800f21f8 |
| 0xe | y.px | 4 | 1 | 0x800f1748, 0x800f1884, 0x800f1d4c, 0x800f2014 |
| 0x14 | layer | 1 | 1 | 0x800f1d4c, 0x800f21f8 |
| 0x15 | flip | 7 | 4 | 0x800f1748, 0x800f1884, 0x800f1d4c, 0x800f21f8, 0x800f22fc |
| 0x16 | priority | 0 | 1 | 0x800f1d4c |
| 0x18 | prev_x | 0 | 2 | 0x800f1d4c, 0x800f1e8c |
| 0x1c | prev_y | 0 | 2 | 0x800f1d4c, 0x800f1e8c |
| 0x20 | speed_x | 3 | 6 | 0x800f1884, 0x800f1d4c, 0x800f2150, 0x800f22fc |
| 0x24 | speed_y | 1 | 6 | 0x800f1884, 0x800f1d4c, 0x800f20b4, 0x800f2150, 0x800f22fc |
| 0x28 | accel_x | 0 | 5 | 0x800f1884, 0x800f1d4c, 0x800f20b4, 0x800f2150, 0x800f22fc |
| 0x2c | gravity | 0 | 6 | 0x800f1884, 0x800f1d4c, 0x800f20b4, 0x800f2150, 0x800f22fc |
| 0x30 | anime_table | 1 | 1 | 0x800f1d4c, 0x800f21f8 |
| 0x3c | sprite_data | 1 | 0 | 0x800f21f8 |
| 0x40 | tex_cord | 1 | 0 | 0x800f21f8 |
| 0x42 | clut_cord | 1 | 0 | 0x800f21f8 |
| 0x45 | anime.flag | 3 | 1 | 0x800f1c10, 0x800f20b4, 0x800f21f8 |
| 0x46 | anime.next | 3 | 0 | 0x800f1c10, 0x800f2150, 0x800f21f8 |
| 0x50 | attack_box | 0 | 6 | 0x800f1c10, 0x800f1d4c, 0x800f2014, 0x800f22fc |
| 0x54 | hurt_box | 0 | 6 | 0x800f1c10, 0x800f1d4c, 0x800f2014, 0x800f22fc |
| 0x58 | damage_table | 0 | 5 | 0x800f1c10, 0x800f1d4c, 0x800f2150, 0x800f22fc |
| 0x5c | hp | 0 | 1 | 0x800f1d4c |
| 0x60 | contact_damage | 0 | 1 | 0x800f1d4c |
| 0x61 | iframes | 0 | 1 | 0x800f1d4c |
| 0x67 | latch | 3 | 5 | 0x800f1748, 0x800f1884, 0x800f1a24, 0x800f1d4c, 0x800f20b4, 0x800f2150 |
| 0x68 | terrain_box | 2 | 1 | 0x800f1884, 0x800f1d4c |
| 0x70 | collide_flags | 6 | 1 | 0x800f1884, 0x800f1d4c, 0x800f1e8c, 0x800f2150, 0x800f22fc |
| 0x7c | work.7C | 1 | 2 | 0x800f1c10 |
| 0x80 | work.80 | 5 | 4 | 0x800f1748, 0x800f1a24, 0x800f1c10, 0x800f1d4c, 0x800f21f8 |
| 0x81 | self+0x81 | 2 | 7 | 0x800f1748, 0x800f1d4c |
| 0x82 | self+0x82 | 1 | 3 | 0x800f1a24, 0x800f1c10, 0x800f1d4c |
| 0x83 | self+0x83 | 1 | 3 | 0x800f1748, 0x800f1d4c, 0x800f21f8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x8007429c | dmg_tbl_one_hit | 0x800f2460 |
| 0x8007433c | dmg_tbl_deflect | 0x800f1c10 |
| 0x800743dc | dmg_tbl_normal | 0x800f1c10, 0x800f1d4c, 0x800f2150, 0x800f22fc |
| 0x800950a0 | weapon_objects | 0x800f1a24 |
| 0x800970a0 | mega | 0x800f1748, 0x800f1d4c |
| 0x800fc4f4 |  | 0x800f1d4c |
| 0x800fc4f8 |  | 0x800f1c10, 0x800f1d4c, 0x800f2014, 0x800f22fc |
| 0x800fc4fc |  | 0x800f1c10 |
| 0x800fc500 |  | 0x800f1c10, 0x800f1d4c, 0x800f2014, 0x800f22fc |
| 0x800fc5f8 |  | 0x800f1d4c |
| 0x800fc630 |  | 0x800f1e8c |
| 0x800fc660 |  | 0x800f2460 |
| 0x800fc664 |  | 0x800f2460 |
