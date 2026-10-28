# RE kit: monbando

- Object: monbando
- State: `turtloid-1.state.gz` (SHA-1 `da8082595c9fc59c1fff107999f384a674b9a63d`), stage 6 area 0
- Class: main, type 9; entry function 0x800ea3d0
- Functions walked: 13; state tables: 3

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80012d9c | Rumble | controller vibration (likely) | 0x800e9ef0 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800e9ef0 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800e9ef0, 0x800ea41c, 0x800ea554, 0x800ea708, 0x800ea750 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ea178, 0x800ea554, 0x800ea5fc, 0x800ea708, 0x800ea750 |
| 0x8002943c | Quake2 | screen shake variant: camera work +0x36/+0x45/+0x3E/+0x3C/+0x3A, flag 0x10 | 0x800e9ef0 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800ea5fc |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ea814 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ea514 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ea514 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800e9d64, 0x800ea178 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800e9d64, 0x800ea178 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ea750 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ea178 |
| 0x8002d2d4 | GetRNG | random number | 0x800ea554 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800ea178 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ea178, 0x800ea3d0 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800e9d64, 0x800ea178 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800e9d64, 0x800ea178 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800e9d64 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800e9d64 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800e9d64, 0x800ea178 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800e9d64 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f5b44 | 0x800ea3d0 | 0x800ea41c, 0x800e9d64, 0x800ea514, 0x800ea514 |
| 0x800f5b54 | 0x800e9d64 | 0x80040290, 0x800ea554, 0x800ea5fc, 0x800ea708, 0x800ea750 |
| 0x800f5b8c | 0x800ea834 | 0x800e9ef0, 0x800ea178, 0x800ea814 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 7 | 0x800ea834 |

## Animation tables

### Table 0x800f5b14 (11 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 34 36 37 | 23 23 23 | 0 0 2 | 0 |
| 1 | 34 35 0 | 4 4 1 | 0 0 0 | hold |
| 2 | 1 2 3 1 0 | 2 6 16 7 1 | 0 1 0 3 0 | hold |
| 3 | 1 4 5 6 7 6 1 0 | 6 11 8 24 2 36 7 1 | 0 0 0 0 1 0 0 0 | hold |
| 4 | 8 10 9 10 11 12 13 14 27 28 29 1 0 | 6 2 3 10 3 3 4 4 5 6 9 7 1 | 0 0 0 0 0 0 1 0 0 0 0 0 0 | hold |
| 5 | 15 16 17 18 19 20 21 22 | 1 1 1 1 1 1 1 1 | 0 0 0 0 0 0 0 0 | 0 |
| 6 | 23 24 25 26 | 1 1 1 1 | 0 0 0 0 | 0 |
| 7 | 30 | 1 | 0 | hold |
| 8 | 31 | 1 | 0 | hold |
| 9 | 32 | 1 | 0 | hold |
| 10 | 33 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x4 | state | 2 | 2 | 0x800e9d64, 0x800ea3d0, 0x800ea41c, 0x800ea514 |
| 0x5 | step | 2 | 6 | 0x800e9d64, 0x800ea41c, 0x800ea554, 0x800ea5fc, 0x800ea708, 0x800ea750, 0x800ea7cc |
| 0x6 | step2 | 2 | 4 | 0x800ea41c, 0x800ea554, 0x800ea5fc |
| 0x8 | x | 4 | 0 | 0x800e9d64, 0x800ea41c, 0x800ea5fc |
| 0xc | y | 3 | 0 | 0x800e9d64, 0x800ea41c, 0x800ea5fc |
| 0x14 | layer | 0 | 1 | 0x800ea41c |
| 0x15 | flip | 2 | 4 | 0x800e9d64, 0x800ea41c, 0x800ea5fc |
| 0x16 | priority | 0 | 1 | 0x800ea41c |
| 0x18 | prev_x | 0 | 2 | 0x800e9d64, 0x800ea41c |
| 0x1c | prev_y | 0 | 2 | 0x800e9d64, 0x800ea41c |
| 0x20 | speed_x | 0 | 2 | 0x800ea41c, 0x800ea750 |
| 0x24 | speed_y | 0 | 3 | 0x800ea41c, 0x800ea750, 0x800ea7cc |
| 0x28 | accel_x | 0 | 3 | 0x800ea41c, 0x800ea750, 0x800ea7cc |
| 0x2c | gravity | 0 | 3 | 0x800ea41c, 0x800ea750, 0x800ea7cc |
| 0x30 | anime_table | 1 | 1 | 0x800ea41c, 0x800ea5fc |
| 0x3c | sprite_data | 1 | 0 | 0x800ea5fc |
| 0x40 | tex_cord | 1 | 0 | 0x800ea5fc |
| 0x42 | clut_cord | 1 | 0 | 0x800ea5fc |
| 0x45 | anime.flag | 2 | 0 | 0x800ea5fc, 0x800ea750 |
| 0x47 | anime.sprite | 2 | 0 | 0x800e9d64, 0x800ea708 |
| 0x50 | attack_box | 0 | 1 | 0x800ea41c |
| 0x54 | hurt_box | 0 | 1 | 0x800ea41c |
| 0x58 | damage_table | 0 | 2 | 0x800e9d64, 0x800ea41c |
| 0x5c | hp | 0 | 1 | 0x800ea41c |
| 0x60 | contact_damage | 0 | 1 | 0x800ea41c |
| 0x61 | iframes | 0 | 1 | 0x800ea41c |
| 0x67 | latch | 1 | 3 | 0x800ea41c, 0x800ea750, 0x800ea7cc |
| 0x68 | terrain_box | 0 | 1 | 0x800ea41c |
| 0x70 | collide_flags | 3 | 1 | 0x800e9d64, 0x800ea41c, 0x800ea750, 0x800ea7cc |
| 0x7c | work.7C | 1 | 4 | 0x800e9d64, 0x800ea41c, 0x800ea554, 0x800ea708 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800741fc | dmg_tbl_minor | 0x800e9ef0 |
| 0x8007433c | dmg_tbl_deflect | 0x800e9d64, 0x800e9ef0, 0x800ea41c |
| 0x800743dc | dmg_tbl_normal | 0x800e9d64 |
| 0x800970a0 | mega | 0x800ea41c |
| 0x800970a2 | mega.var | 0x800ea554 |
| 0x800970a8 | mega.x | 0x800e9d64 |
| 0x800970ae | mega.y.px | 0x800ea178 |
| 0x800ccf38 | game.difficulty | 0x800e9ef0 |
| 0x800f5a48 |  | 0x800ea41c |
| 0x800f5a4c |  | 0x800ea41c |
| 0x800f5a50 |  | 0x800ea41c |
| 0x800f5b14 |  | 0x800ea41c |
| 0x800f5b40 |  | 0x800e9d64 |
| 0x800f5b68 |  | 0x800e9ef0 |
| 0x800f5b70 |  | 0x800e9ef0 |
| 0x800f5b74 |  | 0x800e9ef0 |
| 0x800f5b78 |  | 0x800e9ef0 |
| 0x800f5b7c |  | 0x800e9ef0 |
| 0x800f5b80 |  | 0x800e9ef0 |
| 0x800f5b84 |  | 0x800e9ef0 |
| 0x800f5b88 |  | 0x800e9ef0 |
