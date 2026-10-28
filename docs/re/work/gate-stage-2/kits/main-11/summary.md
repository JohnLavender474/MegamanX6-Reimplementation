# RE kit: main type 11

- Object: main type 11
- State: `gate-3_stage-2.state.gz` (SHA-1 `992d5d0609e736bba918340805dd9c642d889217`), stage 17 area 0
- Class: main, type 11; entry function 0x800ed734
- Functions walked: 28; state tables: 4

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ebfd8, 0x800ed4b8, 0x800ed9e4 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ebfd8, 0x800ed3a0, 0x800ed4b8, 0x800ed8b0, 0x800ed8f0, 0x800ed9e4 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ec118, 0x800edc38 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800edb68 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800edce8 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ed7ac |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ed7cc |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ec118, 0x800ed4b8, 0x800edc38 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ec118, 0x800ed4b8, 0x800edc38 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ed7ec |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800edc38 |
| 0x8002d2d4 | GetRNG | random number | 0x800ed4b8, 0x800edad8 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800edc38 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ec118 |
| 0x80030ccc | TerrainPointCheck | TerrainPointCheck(obj, x, y): non-zero when the point is solid (rectangle list first, then 0x80030234) | 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ec118, 0x800edc38 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ec118 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ec118 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ec118 |
| 0x80049d90 | SpawnHitEffect | effect (flags 0x21, type 3, var 1) at the object, mirrored when speed_x > 0; an enemy shot hitting a wall (likely) | 0x800edc38 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ec118 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ec118 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800ed4b8 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800ed4b8 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800ed3a0, 0x800ed8f0 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f8efc | 0x800ed734 | 0x800ebfd8, 0x800ec118, 0x800ed7ac, 0x800ed7cc |
| 0x800f8f0c | 0x800ec118 | 0x80040290, 0x800eda58, 0x800ed7ec, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800ed874 |
| 0x800f8f50 | 0x800ed874 | 0x800ed8b0, 0x800ed8f0, 0x800ed3a0, 0x800ed9e4 |
| 0x800f8f6c | 0x800ed770 | 0x800ed4b8, 0x800edc38, 0x800edce8 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e98d4 | 0x800ed55c | 0x800ed4b8 | 5 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 10 | 0x800ed770 |

## Animation tables

### Table 0x800f8ebc (12 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 | 2 2 2 2 | 0 0 0 0 | 0 |
| 1 | 4 5 6 6 7 8 9 10 11 11 | 6 6 1 3 4 3 3 3 29 1 | 0 0 1 0 0 0 0 0 0 2 | hold |
| 2 | 10 9 7 5 5 5 | 5 6 7 1 24 1 | 0 0 0 1 0 2 | hold |
| 3 | 11 26 26 11 11 | 10 1 3 9 1 | 0 1 0 0 2 | hold |
| 4 | 12 24 24 12 12 | 10 1 3 9 1 | 0 1 0 0 2 | hold |
| 5 | 13 25 25 13 13 | 10 1 3 9 1 | 0 1 0 0 2 | hold |
| 6 | 27 28 28 27 27 | 10 1 3 9 1 | 0 1 0 0 2 | hold |
| 7 | 29 30 30 29 29 | 10 1 3 9 1 | 0 1 0 0 2 | hold |
| 8 | 14 15 16 17 18 19 | 2 2 2 2 2 2 | 0 0 0 0 0 0 | 0 |
| 9 | 20 | 1 | 0 | hold |
| 10 | 21 | 1 | 0 | hold |
| 11 | 22 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x1 | type | 1 | 0 | 0x800eda60 |
| 0x2 | var | 1 | 0 | 0x800ebfd8 |
| 0x3 | display | 1 | 3 | 0x800ebfd8, 0x800ec118 |
| 0x4 | state | 1 | 5 | 0x800ebfd8, 0x800ec118, 0x800ed734 |
| 0x5 | step | 3 | 28 | 0x800ebfd8, 0x800ec118, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800ed7ec, 0x800ed9e4, 0x800edad8 |
| 0x6 | step2 | 5 | 6 | 0x800ebfd8, 0x800ed3a0, 0x800ed874, 0x800ed8b0, 0x800ed8f0, 0x800ed9e4, 0x800edad8 |
| 0x8 | x | 1 | 0 | 0x800ec118 |
| 0xa | x.px | 21 | 0 | 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800ed3a0, 0x800ed7ec, 0x800ed8f0, 0x800edb38, 0x800edb68 |
| 0xc | y | 1 | 0 | 0x800ec118 |
| 0xe | y.px | 11 | 0 | 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800ed3a0, 0x800ed8f0, 0x800edb68 |
| 0x14 | layer | 1 | 1 | 0x800ebfd8, 0x800edb68 |
| 0x15 | flip | 9 | 20 | 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800ed7ec, 0x800edb38, 0x800edb68 |
| 0x16 | priority | 0 | 1 | 0x800ebfd8 |
| 0x18 | prev_x | 0 | 1 | 0x800ec118 |
| 0x1c | prev_y | 0 | 1 | 0x800ec118 |
| 0x20 | speed_x | 0 | 18 | 0x800ec118, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800ed7ec |
| 0x24 | speed_y | 1 | 18 | 0x800ebfd8, 0x800ec118, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800ed7ec |
| 0x28 | accel_x | 0 | 9 | 0x800ebfd8, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188 |
| 0x2c | gravity | 0 | 10 | 0x800ebfd8, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800ed7ec |
| 0x30 | anime_table | 1 | 1 | 0x800ebfd8, 0x800edb68 |
| 0x3c | sprite_data | 1 | 0 | 0x800edb68 |
| 0x40 | tex_cord | 1 | 0 | 0x800edb68 |
| 0x42 | clut_cord | 1 | 0 | 0x800edb68 |
| 0x45 | anime.flag | 4 | 0 | 0x800ed3a0, 0x800ed8f0, 0x800ed9e4 |
| 0x50 | attack_box | 0 | 1 | 0x800ebfd8 |
| 0x54 | hurt_box | 0 | 3 | 0x800ebfd8, 0x800ed8f0, 0x800ed9e4 |
| 0x58 | damage_table | 0 | 1 | 0x800ebfd8 |
| 0x5c | hp | 0 | 1 | 0x800ebfd8 |
| 0x60 | contact_damage | 0 | 1 | 0x800ebfd8 |
| 0x67 | latch | 0 | 3 | 0x800ebfd8, 0x800ec118 |
| 0x68 | terrain_box | 0 | 1 | 0x800ebfd8 |
| 0x70 | collide_flags | 10 | 1 | 0x800ebfd8, 0x800ec118, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800ed7ec |
| 0x7c | work.7C | 3 | 2 | 0x800ed3a0, 0x800ed8b0, 0x800ed8f0 |
| 0x80 | work.80 | 1 | 0 | 0x800ed3a0 |
| 0x84 | work.84 | 3 | 3 | 0x800ebfd8, 0x800ec118 |
| 0x86 | self+0x86 | 16 | 9 | 0x800ebfd8, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188 |
| 0x87 | self+0x87 | 8 | 0 | 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188 |
| 0x88 | work.88 | 20 | 8 | 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188 |
| 0x89 | self+0x89 | 12 | 17 | 0x800ebfd8, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188 |
| 0x8a | work.8A | 24 | 17 | 0x800ebfd8, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188 |
| 0x8b | self+0x8b | 1 | 1 | 0x800ed9e4, 0x800edad8 |
| 0x8c | work.8C | 2 | 0 | 0x800ed8f0, 0x800edb68 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f80004c | current_main | 0x800ebfd8, 0x800ec118, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188, 0x800ed3a0, 0x800ed8f0, 0x800ed9e4, 0x800edad8, 0x800edb68 |
| 0x800741fc | dmg_tbl_minor | 0x800ebfd8 |
| 0x8008ef48 | main_objects | 0x800eda60 |
| 0x800970a0 | mega | 0x800ed3a0, 0x800ed8f0 |
| 0x800970aa | mega.x.px | 0x800ed7ec, 0x800edb38 |
| 0x800970b4 | mega.layer | 0x800ebfd8 |
| 0x800ccf38 | game.difficulty | 0x800ed3a0 |
| 0x800f8ebc |  | 0x800ebfd8 |
| 0x800f8eec |  | 0x800ec118 |
| 0x800f8ef0 |  | 0x800ebfd8 |
| 0x800f8ef4 |  | 0x800ed8f0 |
| 0x800f8ef8 |  | 0x800ebfd8, 0x800ec2c8, 0x800ec4f0, 0x800ec70c, 0x800ec920, 0x800ecb34, 0x800ecd48, 0x800ecf60, 0x800ed188 |
| 0x800f8efa |  | 0x800ec2c8, 0x800ec70c, 0x800ecb34, 0x800ecf60 |
| 0x800f8efb |  | 0x800ec4f0, 0x800ec920, 0x800ecd48, 0x800ed188 |
| 0x800f8f3c |  | 0x800ed8f0 |
| 0x800f8f60 |  | 0x800ed4b8 |
| 0x800f8f64 |  | 0x800ed4b8 |
