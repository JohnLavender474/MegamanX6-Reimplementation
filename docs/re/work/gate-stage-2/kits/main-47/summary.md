# RE kit: main type 47

- Object: main type 47
- State: `gate-3_stage-2.state.gz` (SHA-1 `992d5d0609e736bba918340805dd9c642d889217`), stage 17 area 0
- Class: main, type 47; entry function 0x8004e5d8
- Functions walked: 8; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x8004dcb8, 0x8004e0a8, 0x8004e26c |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x8004d7b8 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x8004da7c |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x8004e7e0 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x8004da7c |
| 0x8002cab4 | OffScreenCheck | 1 if outside [-64, 384) x [-64, 304) of the layer camera | 0x8004da7c |
| 0x8002cb50 | DisplayCheck1 | display = 1 if inside [-32, W+32) x [-32, 272) | 0x8004d7b8, 0x8004da7c |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x8004da7c |
| 0x8002dcf4 | TerrainBoxOverlap | TerrainBoxOverlap(a, b): the terrain boxes of a and b overlap | 0x8004e26c |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x8004d7b8, 0x8004da7c, 0x8004e5d8 |
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x8004da7c |
| 0x8004ddd4 | WeaponEnergyRefill | (obj, kind): kind 5 = 4 units (6 with part 0x40), 6 = 16 (22 with part 0x40), 7 = full (mega+0x14A); x 6 into the weapon gauges from mega+0xAA (likely: the weapon energy pickups; the gauge choice not read to the end) | 0x8004e26c |
| 0x8004e78c | SetStopFlags | SetStopFlags(n): game+0x10..+0x17 and +0x19 = n | 0x8004dcb8, 0x8004e0a8 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800788f8 | 0x8004e5d8 | 0x8004d7b8, 0x8004da7c, 0x8004e7c0, 0x8004e7e0 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x80011614 | 0x8004d96c | 0x8004d7b8 | 12 |
| 0x80011644 | 0x8004e304 | 0x8004e26c | 12 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800787c0 (12 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 | 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 1 | 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 | 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 2 | 27 28 27 28 29 30 33 32 33 | 6 5 5 5 5 7 7 7 7 | 0 0 0 0 0 0 0 0 0 | 5 |
| 3 | 0 1 2 3 0 1 2 3 2 4 5 1 6 7 8 9 8 | 3 2 1 1 3 2 1 1 4 3 3 5 2 6 7 8 7 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 13 |
| 4 | 61 62 63 64 65 66 67 68 69 70 71 72 | 6 6 6 6 6 6 6 6 6 6 6 6 | 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 5 | 34 35 36 37 38 37 38 39 40 41 42 43 44 39 | 8 7 6 5 4 5 6 7 7 7 7 7 7 7 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 8 |
| 6 | 10 11 12 13 14 15 10 15 16 17 18 19 20 21 22 23 24 25 26 15 | 6 6 6 6 6 7 8 9 3 3 3 3 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 8 |
| 7 | 45 47 49 51 53 55 57 59 | 7 7 7 7 7 7 7 7 | 0 0 0 0 0 0 0 0 | 0 |
| 8 | 96 97 98 89 90 91 92 93 94 95 94 93 92 91 90 89 98 97 | 6 5 4 6 5 4 6 5 4 6 5 4 4 3 3 4 4 5 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 9 | 99 100 101 102 101 100 99 100 101 102 101 100 99 100 101 102 101 100 99 100 101 102 101 100 99 100 101 103 104 105 106 101 100 | 3 4 5 6 5 4 3 4 5 6 5 4 3 4 5 6 5 4 3 4 5 6 5 4 3 4 5 2 1 1 2 5 4 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 10 | 107 108 109 114 109 108 107 108 109 114 109 108 107 108 109 114 109 108 107 108 109 114 109 108 107 108 109 110 111 112 113 109 108 | 3 4 5 6 5 4 3 4 5 6 5 4 3 4 5 6 5 4 3 4 5 6 5 4 3 4 5 2 1 1 2 5 4 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 11 | 115 116 117 118 | 5 5 5 5 | 0 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 2 | 4 | 0x8004d7b8, 0x8004dcb8, 0x8004e0a8 |
| 0x2 | var | 10 | 1 | 0x8004d7b8, 0x8004e26c |
| 0x3 | display | 0 | 4 | 0x8004d7b8, 0x8004da7c, 0x8004dcb8, 0x8004e7c0 |
| 0x4 | state | 2 | 12 | 0x8004d7b8, 0x8004da7c, 0x8004dcb8, 0x8004e0a8, 0x8004e26c, 0x8004e5d8 |
| 0x5 | step | 0 | 5 | 0x8004d7b8, 0x8004da7c, 0x8004dcb8, 0x8004e0a8, 0x8004e26c |
| 0x6 | step2 | 2 | 6 | 0x8004da7c, 0x8004dcb8, 0x8004e0a8 |
| 0x8 | x | 1 | 0 | 0x8004e5d8 |
| 0xa | x.px | 2 | 1 | 0x8004da7c |
| 0xc | y | 1 | 0 | 0x8004e5d8 |
| 0xe | y.px | 1 | 0 | 0x8004da7c |
| 0x10 | record | 3 | 0 | 0x8004d7b8, 0x8004da7c |
| 0x14 | layer | 0 | 1 | 0x8004d7b8 |
| 0x15 | flip | 0 | 1 | 0x8004d7b8 |
| 0x16 | priority | 0 | 1 | 0x8004d7b8 |
| 0x18 | prev_x | 0 | 1 | 0x8004e5d8 |
| 0x1c | prev_y | 0 | 1 | 0x8004e5d8 |
| 0x20 | speed_x | 0 | 2 | 0x8004d7b8 |
| 0x24 | speed_y | 1 | 5 | 0x8004d7b8, 0x8004da7c |
| 0x28 | accel_x | 0 | 2 | 0x8004d7b8 |
| 0x2c | gravity | 0 | 5 | 0x8004d7b8, 0x8004da7c |
| 0x30 | anime_table | 0 | 1 | 0x8004d7b8 |
| 0x3c | sprite_data | 0 | 1 | 0x8004d7b8 |
| 0x40 | tex_cord | 0 | 1 | 0x8004d7b8 |
| 0x42 | clut_cord | 0 | 5 | 0x8004d7b8 |
| 0x67 | latch | 1 | 5 | 0x8004d7b8, 0x8004da7c |
| 0x68 | terrain_box | 1 | 2 | 0x8004d7b8, 0x8004da7c |
| 0x70 | collide_flags | 4 | 0 | 0x8004d7b8, 0x8004da7c |
| 0x80 | work.80 | 3 | 3 | 0x8004d7b8, 0x8004da7c |
| 0x82 | self+0x82 | 4 | 4 | 0x8004d7b8, 0x8004e26c |
| 0x83 | self+0x83 | 1 | 1 | 0x8004dcb8, 0x8004e0a8 |
| 0x84 | work.84 | 1 | 2 | 0x8004dcb8, 0x8004e0a8 |
| 0x85 | self+0x85 | 1 | 2 | 0x8004d7b8, 0x8004da7c |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f80001c | layout_base+0x18 | 0x8004d7b8 |
| 0x800787c0 |  | 0x8004d7b8 |
| 0x80078828 |  | 0x8004d7b8 |
| 0x800970a0 | mega | 0x8004dcb8, 0x8004e0a8, 0x8004e26c |
| 0x80097180 | mega.special | 0x8004e5d8 |
| 0x80097420 | frame_count | 0x8004da7c |
| 0x800cced0 | game | 0x8004dcb8, 0x8004e0a8, 0x8004e26c |
| 0x800ccf3a | game.tanks | 0x8004d7b8 |
| 0x800ccf3c | game.tanks+0x2 | 0x8004d7b8 |
| 0x800cd340 | actor2 | 0x8004e26c |
