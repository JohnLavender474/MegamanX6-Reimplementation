# RE kit: worm-spider

- Object: worm-spider
- State: `yammark-1_start.state.gz` (SHA-1 `697358bffd35fe39ff9233a880cf24d12b886b6c`), stage 1 area 0
- Class: main, type 3; entry function 0x800ea264
- Functions walked: 41; state tables: 10

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800e9b80, 0x800ea66c, 0x800ea870 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800e99d4, 0x800e9b80, 0x800e9c98, 0x800e9dcc, 0x800e9ef4, 0x800ea0b0, 0x800ea480, 0x800ea608, 0x800ea764, 0x800ea870, 0x800ea978, 0x800eaa78 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ea0b0, 0x800ea318, 0x800eace0, 0x800eae0c |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800ea0b0 |
| 0x8002c530 | GetMiscObject | free misc object (debris) | 0x800eab3c, 0x800eabf0 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800eadcc, 0x800eadec, 0x800eae0c |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ea404 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ea424 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800e9dcc, 0x800ea318, 0x800eace0 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800e9dcc, 0x800ea0b0, 0x800ea318, 0x800eace0 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ea978, 0x800eaa04, 0x800eaa78, 0x800eab14 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800eace0 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800eace0 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ea318, 0x800eace0 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800ea318, 0x800eace0 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ea318 |
| 0x80049d90 | SpawnHitEffect | effect (flags 0x21, type 3, var 1) at the object, mirrored when speed_x > 0; an enemy shot hitting a wall (likely) | 0x800eace0 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ea318, 0x800eace0 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ea318 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800e9dcc |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800e9dcc |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800e9dcc |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f70e0 | 0x800ea264 | 0x800e99d4, 0x800ea318, 0x800ea404, 0x800ea424 |
| 0x800f70f0 | 0x800ea318 | 0x80040290, 0x800eab34, 0x800ea444, 0x800ea564, 0x800ea5cc, 0x800ea728, 0x800ea834, 0x800ea93c |
| 0x800f7110 | 0x800ea444 | 0x800ea480, 0x800ea4dc |
| 0x800f7118 | 0x800ea564 | 0x800e9b80, 0x800e9c98, 0x800ea5a0 |
| 0x800f7124 | 0x800ea5cc | 0x800ea608, 0x800ea66c, 0x800ea6fc |
| 0x800f7130 | 0x800ea728 | 0x800ea764, 0x800ea7e4 |
| 0x800f7138 | 0x800ea834 | 0x800ea870, 0x800ea8e8, 0x800ea910 |
| 0x800f7144 | 0x800ea93c | 0x800ea978, 0x800eaa04, 0x800eaa78, 0x800eab14 |
| 0x800f7160 | 0x800ea2a0 | 0x800e9dcc, 0x800eace0, 0x800eadcc |
| 0x800f716c | 0x800ea2dc | 0x800e9ef4, 0x800ea0b0, 0x800eadec, 0x800eae0c |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 3 | 0x800ea2a0 |
| misc | 5 | 0x800ea2dc |

## Animation tables

### Table 0x800f7064 (24 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 36 | 1 | 0 | hold |
| 1 | 48 49 | 3 3 | 0 0 | 0 |
| 2 | 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 0 0 | 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 3 3 3 1 | 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 | hold |
| 3 | 0 | 1 | 0 | hold |
| 4 | 5 | 12 | 1 | hold |
| 5 | 6 7 8 9 10 11 | 12 12 12 12 12 12 | 0 0 0 0 0 0 | 0 |
| 6 | 12 | 12 | 0 | hold |
| 7 | 76 76 1 76 2 76 1 76 2 76 3 76 4 76 3 76 4 76 1 76 2 76 1 76 2 2 | 1 1 3 2 3 2 3 2 3 2 3 2 3 2 3 2 3 2 3 2 3 2 3 2 2 1 | 1 0 0 0 0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 0 0 0 0 0 3 | hold |
| 8 | 7 45 46 45 7 44 | 4 3 2 3 4 5 | 0 0 0 0 0 0 | 0 |
| 9 | 37 38 39 40 40 41 42 43 47 47 | 1 2 2 1 2 2 2 1 5 1 | 0 0 0 2 0 0 0 0 0 1 | hold |
| 10 | 13 14 15 16 17 18 23 24 25 16 17 18 19 20 21 16 17 22 23 24 25 16 17 18 | 2 4 3 3 3 2 2 4 3 3 3 2 2 4 3 3 3 2 2 4 3 3 3 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 11 | 27 28 29 | 14 14 14 | 0 0 0 | 0 |
| 12 | 30 31 32 27 28 29 | 3 5 4 14 14 14 | 0 0 0 0 0 0 | 3 |
| 13 | 33 34 35 35 | 5 4 3 1 | 0 0 0 1 | hold |
| 14 | 115 | 4 | 0 | hold |
| 15 | 116 117 | 3 3 | 0 0 | 0 |
| 16 | 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 101 102 103 77 77 | 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 3 3 3 1 | 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 | hold |
| 17 | 77 | 4 | 0 | hold |
| 18 | 104 | 12 | 1 | hold |
| 19 | 105 106 107 108 109 110 | 12 12 12 12 12 12 | 0 0 0 0 0 0 | 0 |
| 20 | 111 | 12 | 0 | hold |
| 21 | 106 113 114 113 106 112 | 4 3 2 3 4 5 | 0 0 0 0 0 0 | 0 |
| 22 | 75 74 73 72 72 71 70 69 68 67 66 65 64 63 62 61 60 59 58 57 56 55 54 53 52 51 50 50 | 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 3 2 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 | hold |
| 23 | 103 102 101 100 99 98 97 96 95 94 93 92 91 90 89 88 87 86 85 84 83 82 81 80 79 78 78 | 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 3 2 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 9 | 0 | 0x800e99d4, 0x800e9b80, 0x800e9c98, 0x800ea480, 0x800ea608, 0x800ea66c, 0x800ea764, 0x800ea870 |
| 0x3 | display | 0 | 1 | 0x800e99d4 |
| 0x4 | state | 1 | 2 | 0x800e99d4, 0x800ea264, 0x800ea318 |
| 0x5 | step | 1 | 9 | 0x800e99d4, 0x800ea318, 0x800ea4dc, 0x800ea5a0, 0x800ea6fc, 0x800ea7e4, 0x800ea910 |
| 0x6 | step2 | 17 | 19 | 0x800e99d4, 0x800e9b80, 0x800e9c98, 0x800ea444, 0x800ea480, 0x800ea4dc, 0x800ea564, 0x800ea5a0, 0x800ea5cc, 0x800ea608, 0x800ea66c, 0x800ea6fc, 0x800ea728, 0x800ea764, 0x800ea7e4, 0x800ea834, 0x800ea870, 0x800ea8e8, 0x800ea910, 0x800ea93c, 0x800ea978, 0x800eaa04, 0x800eaa78 |
| 0x8 | x | 2 | 0 | 0x800e99d4, 0x800ea318 |
| 0xa | x.px | 5 | 0 | 0x800ea4dc, 0x800eaa78, 0x800eab3c, 0x800eabf0, 0x800eacb0 |
| 0xc | y | 2 | 0 | 0x800e99d4, 0x800ea318 |
| 0xe | y.px | 6 | 5 | 0x800e99d4, 0x800ea4dc, 0x800ea978, 0x800eaa78, 0x800eab3c |
| 0x14 | layer | 2 | 1 | 0x800e99d4, 0x800eab3c, 0x800eabf0 |
| 0x15 | flip | 2 | 3 | 0x800e99d4, 0x800eab3c, 0x800eabf0, 0x800eacb0 |
| 0x16 | priority | 0 | 1 | 0x800e99d4 |
| 0x18 | prev_x | 0 | 2 | 0x800e99d4, 0x800ea318 |
| 0x1c | prev_y | 0 | 2 | 0x800e99d4, 0x800ea318 |
| 0x20 | speed_x | 0 | 3 | 0x800e99d4, 0x800ea978, 0x800eaa78 |
| 0x24 | speed_y | 0 | 3 | 0x800e99d4, 0x800ea978, 0x800eaa78 |
| 0x28 | accel_x | 0 | 3 | 0x800e99d4, 0x800ea978, 0x800eaa78 |
| 0x2c | gravity | 0 | 3 | 0x800e99d4, 0x800ea978, 0x800eaa78 |
| 0x30 | anime_table | 2 | 1 | 0x800e99d4, 0x800eab3c, 0x800eabf0 |
| 0x3c | sprite_data | 2 | 0 | 0x800eab3c, 0x800eabf0 |
| 0x40 | tex_cord | 2 | 0 | 0x800eab3c, 0x800eabf0 |
| 0x42 | clut_cord | 2 | 0 | 0x800eab3c, 0x800eabf0 |
| 0x45 | anime.flag | 9 | 0 | 0x800e9b80, 0x800e9c98, 0x800ea8e8, 0x800eaa04 |
| 0x50 | attack_box | 0 | 5 | 0x800e99d4, 0x800e9b80, 0x800e9c98, 0x800ea870 |
| 0x54 | hurt_box | 0 | 5 | 0x800e99d4, 0x800e9b80, 0x800e9c98, 0x800ea870 |
| 0x58 | damage_table | 0 | 1 | 0x800e99d4 |
| 0x5c | hp | 0 | 1 | 0x800e99d4 |
| 0x60 | contact_damage | 0 | 1 | 0x800e99d4 |
| 0x61 | iframes | 0 | 1 | 0x800e99d4 |
| 0x67 | latch | 0 | 1 | 0x800e99d4 |
| 0x68 | terrain_box | 0 | 3 | 0x800e99d4, 0x800ea870 |
| 0x7c | work.7C | 5 | 10 | 0x800e9c98, 0x800ea5a0, 0x800ea608, 0x800ea66c, 0x800ea6fc, 0x800ea764, 0x800ea7e4, 0x800ea8e8, 0x800ea910 |
| 0x81 | self+0x81 | 0 | 2 | 0x800e9b80 |
| 0x83 | self+0x83 | 0 | 2 | 0x800e9b80 |
| 0x84 | work.84 | 0 | 1 | 0x800e99d4 |
| 0x86 | self+0x86 | 1 | 2 | 0x800e99d4, 0x800eaa04 |
| 0x87 | self+0x87 | 1 | 2 | 0x800ea318, 0x800ea7e4 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f80004c | current_main | 0x800e99d4, 0x800e9b80, 0x800ea318, 0x800ea7e4, 0x800eaa04 |
| 0x1f800064 | current_misc | 0x800ea0b0, 0x800eae0c |
| 0x8007429c | dmg_tbl_one_hit | 0x800e9dcc |
| 0x800743dc | dmg_tbl_normal | 0x800e99d4 |
| 0x800970a0 | mega | 0x800e9dcc, 0x800ea4dc |
| 0x800970aa | mega.x.px | 0x800eaa78, 0x800eacb0 |
| 0x800970b4 | mega.layer | 0x800e99d4 |
| 0x800ccf38 | game.difficulty | 0x800e9c98, 0x800ea764 |
| 0x800e9864 |  | 0x800e9ef4 |
| 0x800e98ac |  | 0x800ea0b0 |
| 0x800f6cbc |  | 0x800e99d4, 0x800e9c98 |
| 0x800f6cc0 |  | 0x800e99d4, 0x800e9c98 |
| 0x800f7064 |  | 0x800e99d4 |
| 0x800f70c4 |  | 0x800e9b80, 0x800e9c98 |
| 0x800f7154 |  | 0x800e9dcc |
| 0x800f7158 |  | 0x800e9dcc |
| 0x800f715c |  | 0x800e9dcc |
