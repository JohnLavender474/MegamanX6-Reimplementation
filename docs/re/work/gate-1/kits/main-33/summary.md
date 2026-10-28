# RE kit: main-33

- Object: main-33
- State: `gate-1_before-boss.state.gz` (SHA-1 `585948ebe43e9241ae0828aff8654b2e9b98ea04`), stage 16 area 0
- Class: main, type 33; entry function 0x800f3b38
- Functions walked: 31; state tables: 7

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800f26c0, 0x800f28ac, 0x800f2bdc, 0x800f39e8, 0x800f3bd0, 0x800f3cac, 0x800f3d74, 0x800f3eec, 0x800f40e0, 0x800f4228 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800f28ac, 0x800f2bdc, 0x800f2e70, 0x800f35bc, 0x800f3bd0, 0x800f3cac, 0x800f3d24, 0x800f3d74, 0x800f3e34, 0x800f42ac |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800f2e70 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800f4380 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800f3bb0 |
| 0x8002cab4 | OffScreenCheck | 1 if outside [-64, 384) x [-64, 304) of the layer camera | 0x800f259c, 0x800f3444 |
| 0x8002cb50 | DisplayCheck1 | display = 1 if inside [-32, W+32) x [-32, 272) | 0x800f259c, 0x800f3444 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800f42ac |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800f42ac |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800f35bc |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800f2bdc, 0x800f3e6c, 0x800f42ac |
| 0x8002d2d4 | GetRNG | random number | 0x800f2410, 0x800f26c0, 0x800f2e70, 0x800f3bd0 |
| 0x8002d380 | AimAt | AimAt(a, b): direction 0..31 from a to b (0 right, 8 up, 16 left, 24 down) | 0x800f3158, 0x800f3f38 |
| 0x8002d3b4 | SetSpeedDir | SetSpeedDir(obj, dir): speed = 1 px/frame in direction dir | 0x800f2e70, 0x800f3f38 |
| 0x8002d654 | BoxOverlap | box test between two objects, facing-aware | 0x800f42ac |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800f42ac |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800f2410, 0x800f2bdc, 0x800f35bc, 0x800f3e6c, 0x800f3ff8 |
| 0x8002f418 | TerrainTouchFlags | after the move: +0x70 = 2 left wall, 1 right wall, 8 floor, 4 ceiling | 0x800f2410, 0x800f32b0 |
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800f2bdc, 0x800f35bc, 0x800f4094 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800f259c, 0x800f3444, 0x800f42ac |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800f259c, 0x800f3444, 0x800f42ac |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800f259c, 0x800f3444 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800f259c, 0x800f3444 |
| 0x80049d90 | SpawnHitEffect | effect (flags 0x21, type 3, var 1) at the object, mirrored when speed_x > 0; an enemy shot hitting a wall (likely) | 0x800f42ac |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800f259c, 0x800f3444, 0x800f42ac |
| 0x8004e680 | DropItemRoll | the roll of DropItem | 0x800f259c, 0x800f3444 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fc674 | 0x800f3b38 | 0x800f2410, 0x800f259c, 0x800f3bb0 |
| 0x800fc680 | 0x800f3b38 | 0x800f32b0, 0x800f3444, 0x800f3bb0 |
| 0x800fc68c | 0x800f259c | 0x80040290, 0x800f3bd0, 0x800f3c64, 0x800f3c64, 0x800f3eb0, 0x800f3e6c |
| 0x800fc6a8 | 0x800f3c64 | 0x800f26c0, 0x800f28ac, 0x800f3cac, 0x800f3d24, 0x800f3d74, 0x800f2bdc, 0x800f3e34 |
| 0x800fc6cc | 0x800f3eb0 | 0x800f3eec, 0x800f2e70 |
| 0x800fc6e4 | 0x800f3444 | 0x80040290, 0x800f39e8, 0x800f35bc, 0x800f35bc, 0x800f35bc, 0x800f3e6c |
| 0x800fc73c | 0x800f41ec | 0x800f4228, 0x800f42ac, 0x800f4380 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e9bac | 0x800f33a4 | 0x800f32b0 | 6 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 36 | 0x800f41ec |

## Animation tables

### Table 0x800fc58c (27 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 | 1 | 0 | hold |
| 1 | 1 | 1 | 0 | hold |
| 2 | 0 2 3 4 5 5 6 7 0 8 0 9 0 8 0 | 10 8 8 8 9 1 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 | 1 |
| 3 | 1 10 11 12 13 13 14 15 1 16 1 17 1 16 1 | 10 8 8 8 9 1 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 | 1 |
| 4 | 1 64 65 66 67 67 68 16 1 15 1 16 1 15 1 | 10 8 8 8 9 1 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 | 1 |
| 5 | 0 18 19 20 21 22 23 24 23 24 | 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 | 0 |
| 6 | 1 25 26 27 28 29 30 31 30 31 | 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 | 0 |
| 7 | 24 33 32 | 3 3 3 | 0 0 0 | 0 |
| 8 | 31 34 35 | 3 3 3 | 0 0 0 | 0 |
| 9 | 31 35 34 | 3 3 3 | 0 0 0 | 0 |
| 10 | 24 21 20 19 0 19 0 | 10 2 2 2 2 2 10 | 0 0 0 0 0 0 0 | 0 |
| 11 | 31 28 27 26 1 26 1 | 10 2 2 2 2 2 10 | 0 0 0 0 0 0 0 | 0 |
| 12 | 36 37 38 39 40 41 42 43 41 42 43 40 39 38 37 36 | 6 6 6 3 18 3 3 2 3 3 2 15 3 6 6 6 | 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 | 0 |
| 13 | 44 45 46 47 48 49 50 51 49 50 51 48 47 46 45 44 | 6 6 6 3 18 3 3 2 3 3 2 15 3 6 6 6 | 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 | 0 |
| 14 | 6 7 0 8 0 9 0 8 0 | 2 2 2 2 2 2 2 2 2 | 1 0 0 0 0 0 0 0 0 | 0 |
| 15 | 14 15 1 16 1 17 1 16 1 | 2 2 2 2 2 2 2 2 2 | 1 0 0 0 0 0 0 0 0 | 0 |
| 16 | 68 16 1 15 1 16 1 15 1 | 2 2 2 2 2 2 2 2 2 | 1 0 0 0 0 0 0 0 0 | 0 |
| 17 | 52 53 54 55 | 2 1 2 1 | 0 0 0 0 | 0 |
| 18 | 56 | 1 | 0 | hold |
| 19 | 57 | 1 | 0 | hold |
| 20 | 58 | 1 | 0 | hold |
| 21 | 59 | 1 | 0 | hold |
| 22 | 60 | 1 | 0 | hold |
| 23 | 61 | 1 | 0 | hold |
| 24 | 62 | 1 | 0 | hold |
| 25 | 63 | 1 | 0 | hold |
| 26 | 69 70 71 | 3 3 3 | 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 8 | 0 | 0x800f2410, 0x800f26c0, 0x800f32b0, 0x800f3b38, 0x800f3d74 |
| 0x3 | display | 0 | 2 | 0x800f2410, 0x800f32b0 |
| 0x4 | state | 1 | 5 | 0x800f2410, 0x800f259c, 0x800f32b0, 0x800f3444, 0x800f3b38 |
| 0x5 | step | 2 | 11 | 0x800f2410, 0x800f259c, 0x800f28ac, 0x800f2e70, 0x800f32b0, 0x800f3444, 0x800f39e8, 0x800f3bd0, 0x800f3e6c |
| 0x6 | step2 | 5 | 24 | 0x800f2410, 0x800f26c0, 0x800f28ac, 0x800f2bdc, 0x800f2e70, 0x800f32b0, 0x800f3444, 0x800f35bc, 0x800f39e8, 0x800f3bd0, 0x800f3c64, 0x800f3cac, 0x800f3d24, 0x800f3d74, 0x800f3e34, 0x800f3e6c, 0x800f3eb0, 0x800f3eec |
| 0x8 | x | 5 | 1 | 0x800f2410, 0x800f259c, 0x800f32b0, 0x800f3444, 0x800f3ff8 |
| 0xa | x.px | 24 | 7 | 0x800f259c, 0x800f26c0, 0x800f28ac, 0x800f2bdc, 0x800f2e70, 0x800f3158, 0x800f3444, 0x800f35bc, 0x800f3f38, 0x800f3ff8, 0x800f4094 |
| 0xc | y | 5 | 1 | 0x800f2410, 0x800f259c, 0x800f32b0, 0x800f3444, 0x800f3ff8 |
| 0xe | y.px | 21 | 7 | 0x800f259c, 0x800f26c0, 0x800f28ac, 0x800f2bdc, 0x800f2e70, 0x800f3158, 0x800f3444, 0x800f35bc, 0x800f3f38, 0x800f3ff8, 0x800f4094 |
| 0x15 | flip | 6 | 10 | 0x800f2410, 0x800f259c, 0x800f26c0, 0x800f28ac, 0x800f2bdc, 0x800f2e70, 0x800f2fb8, 0x800f39e8, 0x800f40e0 |
| 0x16 | priority | 1 | 2 | 0x800f2410, 0x800f2e70, 0x800f32b0 |
| 0x18 | prev_x | 1 | 6 | 0x800f2410, 0x800f259c, 0x800f32b0, 0x800f3444, 0x800f3ff8 |
| 0x1c | prev_y | 1 | 6 | 0x800f2410, 0x800f259c, 0x800f32b0, 0x800f3444, 0x800f3ff8 |
| 0x20 | speed_x | 2 | 11 | 0x800f2410, 0x800f259c, 0x800f28ac, 0x800f32b0, 0x800f39e8, 0x800f3d74, 0x800f40e0 |
| 0x24 | speed_y | 2 | 10 | 0x800f2410, 0x800f259c, 0x800f28ac, 0x800f32b0, 0x800f39e8, 0x800f3d74, 0x800f40e0 |
| 0x28 | accel_x | 0 | 4 | 0x800f2410, 0x800f259c, 0x800f32b0, 0x800f3d74 |
| 0x2c | gravity | 0 | 5 | 0x800f2410, 0x800f259c, 0x800f32b0, 0x800f3d74 |
| 0x30 | anime_table | 1 | 2 | 0x800f2410, 0x800f2e70, 0x800f32b0 |
| 0x3c | sprite_data | 1 | 0 | 0x800f2e70 |
| 0x40 | tex_cord | 1 | 0 | 0x800f2e70 |
| 0x42 | clut_cord | 2 | 1 | 0x800f2e70, 0x800f32b0 |
| 0x45 | anime.flag | 3 | 2 | 0x800f28ac, 0x800f2e70 |
| 0x46 | anime.next | 8 | 0 | 0x800f28ac, 0x800f2e70, 0x800f3bd0, 0x800f3cac, 0x800f3d24, 0x800f3d74, 0x800f3e34 |
| 0x50 | attack_box | 0 | 3 | 0x800f2410, 0x800f39e8, 0x800f40e0 |
| 0x54 | hurt_box | 0 | 4 | 0x800f2410, 0x800f32b0, 0x800f39e8, 0x800f40e0 |
| 0x58 | damage_table | 0 | 2 | 0x800f2410, 0x800f32b0 |
| 0x5c | hp | 0 | 2 | 0x800f2410, 0x800f32b0 |
| 0x60 | contact_damage | 0 | 2 | 0x800f2410, 0x800f32b0 |
| 0x61 | iframes | 0 | 2 | 0x800f2410, 0x800f32b0 |
| 0x67 | latch | 0 | 2 | 0x800f2410, 0x800f32b0 |
| 0x68 | terrain_box | 16 | 5 | 0x800f2410, 0x800f26c0, 0x800f2bdc, 0x800f2fb8, 0x800f35bc, 0x800f39e8, 0x800f3d74, 0x800f40e0 |
| 0x70 | collide_flags | 8 | 1 | 0x800f2410, 0x800f2bdc, 0x800f32b0, 0x800f35bc, 0x800f3e6c, 0x800f3ff8 |
| 0x7c | work.7C | 3 | 6 | 0x800f2410, 0x800f28ac, 0x800f32b0, 0x800f39e8, 0x800f3c64, 0x800f3eec |
| 0x80 | work.80 | 25 | 21 | 0x800f26c0, 0x800f28ac, 0x800f2bdc, 0x800f2e70, 0x800f32b0, 0x800f3444, 0x800f35bc, 0x800f39e8, 0x800f3bd0, 0x800f3d24, 0x800f3d74 |
| 0x81 | self+0x81 | 14 | 0 | 0x800f259c, 0x800f26c0, 0x800f28ac, 0x800f2bdc, 0x800f2fb8, 0x800f3158, 0x800f35bc, 0x800f39e8, 0x800f3bd0, 0x800f3cac, 0x800f3d74, 0x800f3eec |
| 0x82 | self+0x82 | 2 | 2 | 0x800f26c0, 0x800f2bdc, 0x800f3444 |
| 0x83 | self+0x83 | 1 | 2 | 0x800f3158, 0x800f35bc |
| 0x84 | work.84 | 1 | 2 | 0x800f26c0, 0x800f28ac |
| 0x88 | work.88 | 2 | 0 | 0x800f28ac, 0x800f35bc |
| 0x8a | work.8A | 4 | 2 | 0x800f26c0, 0x800f2fb8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f80004c | current_main | 0x800f259c, 0x800f26c0, 0x800f28ac, 0x800f2bdc, 0x800f2e70, 0x800f2fb8, 0x800f3158, 0x800f32b0, 0x800f3444, 0x800f35bc, 0x800f39e8, 0x800f3bd0, 0x800f3cac, 0x800f3d24, 0x800f3d74, 0x800f3eec |
| 0x8007429c | dmg_tbl_one_hit | 0x800f4228 |
| 0x8007433c | dmg_tbl_deflect | 0x800f32b0 |
| 0x800743dc | dmg_tbl_normal | 0x800f2410 |
| 0x800970a0 | mega | 0x800f3158, 0x800f3f38, 0x800f42ac |
| 0x800970aa | mega.x.px | 0x800f2bdc |
| 0x800970ae | mega.y.px | 0x800f2bdc |
| 0x800fc58c |  | 0x800f2410, 0x800f32b0 |
| 0x800fc5f8 |  | 0x800f259c, 0x800f3444 |
| 0x800fc62c |  | 0x800f2410, 0x800f26c0, 0x800f3d74 |
| 0x800fc634 |  | 0x800f2410 |
| 0x800fc63c |  | 0x800f2410 |
| 0x800fc644 |  | 0x800f39e8, 0x800f40e0 |
| 0x800fc654 |  | 0x800f39e8, 0x800f40e0 |
| 0x800fc664 |  | 0x800f39e8, 0x800f40e0 |
| 0x800fc6a4 |  | 0x800f3bd0 |
| 0x800fc6c4 |  | 0x800f26c0 |
| 0x800fc6c8 |  | 0x800f26c0 |
| 0x800fc6d4 |  | 0x800f3eec |
| 0x800fc6d8 |  | 0x800f2e70 |
| 0x800fc6fc |  | 0x800f35bc |
| 0x800fc704 |  | 0x800f39e8, 0x800f40e0 |
| 0x800fc714 |  | 0x800f39e8, 0x800f40e0 |
| 0x800fc724 |  | 0x800f39e8, 0x800f40e0 |
| 0x800fc72c |  | 0x800f39e8, 0x800f40e0 |
| 0x800fc734 |  | 0x800f4228 |
| 0x800fc738 |  | 0x800f4228 |
