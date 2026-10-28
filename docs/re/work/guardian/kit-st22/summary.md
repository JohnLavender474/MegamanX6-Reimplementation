# RE kit: guardian-st22

- Object: guardian-st22
- State: `sheldon-11_laser-shot.state.gz` (SHA-1 `133d8f1d9bd8925ead66dcf5bbd36e92a909a872`), stage 22 area 0
- Class: main, type 30; entry function 0x800ef1bc
- Functions walked: 33; state tables: 7

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800ef7c4 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800eec0c, 0x800ef01c, 0x800ef2f8, 0x800ef448, 0x800ef47c, 0x800ef53c, 0x800ef66c, 0x800ef7c4 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800eee30, 0x800ef878 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800eec0c |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800ef01c |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ef954 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ef264 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ef264 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800eee30, 0x800ef878 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800eee30, 0x800ef878 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ef878 |
| 0x8002d380 | AimAt | AimAt(a, b): direction 0..31 from a to b (0 right, 8 up, 16 left, 24 down) | 0x800ef5b8 |
| 0x8002d3b4 | SetSpeedDir | SetSpeedDir(obj, dir): speed = 1 px/frame in direction dir | 0x800ef01c |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800ef878 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800eee30 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800eee30, 0x800ef878 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800eee30, 0x800ef878 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800eee30 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800eee30 |
| 0x80049d90 | SpawnHitEffect | effect (flags 0x21, type 3, var 1) at the object, mirrored when speed_x > 0; an enemy shot hitting a wall (likely) | 0x800ef878 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800eee30, 0x800ef878 |
| 0x80049ebc | SpawnExplosionRand | SpawnExplosionRand(obj, type, mx, my): explosion at a random offset | 0x800ef1f8 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800eee30 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f373c | 0x800ef1bc | 0x800eec0c, 0x800eee30, 0x800ef1f8, 0x800ef264, 0x800ef264 |
| 0x800f3750 | 0x800eee30 | 0x80040290, 0x800ef758, 0x800ef2b4, 0x800ef2bc, 0x800ef40c, 0x800ef500, 0x800ef6f4, 0x800ef6fc |
| 0x800f3770 | 0x800ef2bc | 0x800ef2f8, 0x800ef334, 0x800ef3b0 |
| 0x800f377c | 0x800ef40c | 0x800ef448, 0x800ef47c, 0x800ef4d4 |
| 0x800f3788 | 0x800ef500 | 0x800ef53c, 0x800ef570, 0x800ef5b8, 0x800ef01c, 0x800ef66c, 0x800ef6c0 |
| 0x800f37a0 | 0x800ef6fc | 0x800ef738, 0x800ef740, 0x800ef748, 0x800ef750 |
| 0x800f37b8 | 0x800ef788 | 0x800ef7c4, 0x800ef878, 0x800ef954 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 35 | 0x800ef788 |

## Animation tables

### Table 0x800f3704 (13 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 | 3 3 3 3 | 0 0 0 1 | 0 |
| 1 | 4 5 6 7 8 9 10 11 | 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 1 | 0 |
| 2 | 12 13 14 15 16 17 17 | 2 2 2 2 2 1 1 | 0 0 0 0 0 0 1 | hold |
| 3 | 18 19 20 21 22 23 24 25 26 25 26 26 | 2 6 3 3 4 2 2 2 2 2 9 1 | 0 0 0 0 0 0 0 2 0 0 0 1 | hold |
| 4 | 27 28 29 30 26 | 2 2 2 2 1 | 0 0 0 0 1 | hold |
| 5 | 31 32 33 34 35 36 | 3 3 3 3 3 3 | 0 0 0 0 0 0 | 0 |
| 6 | 26 23 22 37 22 21 20 19 18 38 39 39 | 6 2 2 2 4 3 3 6 2 2 1 1 | 0 0 2 0 0 0 0 0 0 0 0 1 | hold |
| 7 | 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 | 2 2 2 1 1 2 2 1 1 2 2 1 1 2 2 1 2 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 2 |
| 8 | 56 | 1 | 1 | hold |
| 9 | 57 | 1 | 1 | hold |
| 10 | 58 | 1 | 1 | hold |
| 11 | 59 | 1 | 1 | hold |
| 12 | 26 23 22 37 22 21 20 19 18 18 | 6 2 2 2 4 3 3 10 3 1 | 0 0 2 0 0 0 0 0 0 1 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x1 | type | 4 | 0 | 0x800eec0c |
| 0x2 | var | 3 | 0 | 0x800eec0c, 0x800ef01c |
| 0x3 | display | 0 | 1 | 0x800eee30 |
| 0x4 | state | 3 | 4 | 0x800eec0c, 0x800eee30, 0x800ef1bc, 0x800ef1f8, 0x800ef264 |
| 0x5 | step | 1 | 6 | 0x800eec0c, 0x800eee30, 0x800ef334, 0x800ef3b0, 0x800ef4d4, 0x800ef6c0, 0x800ef758 |
| 0x6 | step2 | 16 | 15 | 0x800eec0c, 0x800ef01c, 0x800ef2bc, 0x800ef2f8, 0x800ef334, 0x800ef3b0, 0x800ef40c, 0x800ef448, 0x800ef47c, 0x800ef4d4, 0x800ef500, 0x800ef53c, 0x800ef570, 0x800ef5b8, 0x800ef66c, 0x800ef6c0, 0x800ef6fc |
| 0x8 | x | 3 | 0 | 0x800eec0c, 0x800eee30, 0x800ef01c |
| 0xa | x.px | 6 | 2 | 0x800ef3b0, 0x800ef5b8, 0x800ef764 |
| 0xc | y | 3 | 0 | 0x800eec0c, 0x800eee30, 0x800ef01c |
| 0x14 | layer | 1 | 1 | 0x800eec0c, 0x800ef01c |
| 0x15 | flip | 7 | 3 | 0x800eec0c, 0x800ef01c, 0x800ef47c, 0x800ef5b8, 0x800ef764 |
| 0x16 | priority | 0 | 1 | 0x800eec0c |
| 0x18 | prev_x | 0 | 2 | 0x800eec0c, 0x800eee30 |
| 0x1c | prev_y | 0 | 2 | 0x800eec0c, 0x800eee30 |
| 0x20 | speed_x | 0 | 1 | 0x800eec0c |
| 0x24 | speed_y | 0 | 1 | 0x800eec0c |
| 0x28 | accel_x | 0 | 1 | 0x800eec0c |
| 0x2c | gravity | 0 | 1 | 0x800eec0c |
| 0x30 | anime_table | 1 | 1 | 0x800eec0c, 0x800ef01c |
| 0x3c | sprite_data | 1 | 1 | 0x800eec0c, 0x800ef01c |
| 0x40 | tex_cord | 1 | 1 | 0x800eec0c, 0x800ef01c |
| 0x42 | clut_cord | 1 | 1 | 0x800eec0c, 0x800ef01c |
| 0x45 | anime.flag | 5 | 0 | 0x800ef47c, 0x800ef570, 0x800ef6c0 |
| 0x50 | attack_box | 0 | 4 | 0x800eec0c, 0x800eee30 |
| 0x54 | hurt_box | 0 | 3 | 0x800eec0c, 0x800eee30 |
| 0x58 | damage_table | 0 | 3 | 0x800eee30 |
| 0x5c | hp | 0 | 1 | 0x800eec0c |
| 0x60 | contact_damage | 0 | 2 | 0x800eec0c |
| 0x61 | iframes | 0 | 1 | 0x800eec0c |
| 0x67 | latch | 0 | 1 | 0x800eec0c |
| 0x68 | terrain_box | 0 | 4 | 0x800eec0c, 0x800eee30 |
| 0x70 | collide_flags | 1 | 1 | 0x800eec0c, 0x800eee30 |
| 0x7c | work.7C | 6 | 11 | 0x800eee30, 0x800ef01c, 0x800ef1f8, 0x800ef2f8, 0x800ef334, 0x800ef47c, 0x800ef4d4, 0x800ef570, 0x800ef66c |
| 0x7e | work.7E | 2 | 6 | 0x800eee30, 0x800ef01c, 0x800ef1f8, 0x800ef570, 0x800ef5b8 |
| 0x80 | work.80 | 0 | 1 | 0x800ef264 |
| 0x81 | self+0x81 | 2 | 1 | 0x800ef01c, 0x800ef5b8 |
| 0x84 | work.84 | 0 | 1 | 0x800ef264 |
| 0x88 | work.88 | 0 | 1 | 0x800ef264 |
| 0x8b | self+0x8b | 2 | 3 | 0x800eec0c, 0x800eee30, 0x800ef570, 0x800ef6c0 |
| 0x8c | work.8C | 0 | 1 | 0x800ef264 |
| 0x94 | self+0x94 | 1 | 1 | 0x800ef264, 0x800ef758 |
| 0x99 | self+0x99 | 0 | 1 | 0x800eec0c |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800eec0c |
| 0x8007429c | dmg_tbl_one_hit | 0x800ef7c4 |
| 0x8007433c | dmg_tbl_deflect | 0x800eee30 |
| 0x800743dc | dmg_tbl_normal | 0x800eee30 |
| 0x80095098 |  | 0x800eec0c |
| 0x800970a0 | mega | 0x800ef5b8 |
| 0x800970aa | mega.x.px | 0x800ef3b0, 0x800ef764 |
| 0x800970b4 | mega.layer | 0x800eec0c |
| 0x800f3578 |  | 0x800eec0c |
| 0x800f357c |  | 0x800eec0c |
| 0x800f3580 |  | 0x800eec0c |
| 0x800f3584 |  | 0x800eec0c |
| 0x800f3588 |  | 0x800eec0c, 0x800eee30 |
| 0x800f358c |  | 0x800eec0c |
| 0x800f3590 |  | 0x800eec0c |
| 0x800f3594 |  | 0x800eec0c |
| 0x800f3598 |  | 0x800eec0c |
| 0x800f359c |  | 0x800eec0c |
| 0x800f35a0 |  | 0x800eee30 |
| 0x800f35a8 |  | 0x800eee30 |
| 0x800f3704 |  | 0x800eec0c |
| 0x800f3738 |  | 0x800eee30 |
| 0x800f37b0 |  | 0x800ef7c4 |
| 0x800f37b4 |  | 0x800ef7c4 |
| 0x800f39ec |  | 0x800eec0c, 0x800eee30 |
| 0x800f39f0 |  | 0x800eec0c |
| 0x800f39f4 |  | 0x800eec0c, 0x800eee30 |
| 0x800f39f8 |  | 0x800eec0c, 0x800eee30 |
