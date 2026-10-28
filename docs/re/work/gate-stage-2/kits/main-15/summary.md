# RE kit: main type 15

- Object: main type 15
- State: `gate-3_stage-2.state.gz` (SHA-1 `992d5d0609e736bba918340805dd9c642d889217`), stage 17 area 0
- Class: main, type 15; entry function 0x80041ab8
- Functions walked: 69; state tables: 12

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x80040800, 0x80040fb4, 0x80042150, 0x80042780, 0x800427e0, 0x8004292c |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800404d8, 0x80040cb0, 0x80040e3c, 0x80041290, 0x80041c24, 0x80041e4c, 0x80041f6c, 0x800420cc, 0x8004228c, 0x800423a0, 0x800424e0, 0x80042694, 0x800429d4, 0x80043038 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x80040800, 0x80042fa8, 0x800430d0 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800404d8 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x80042b68 |
| 0x8002c530 | GetMiscObject | free misc object (debris) | 0x80042a9c |
| 0x8002c7f0 | GetRayObject | first free of 64 x 0x60 slots at 0x8009C9B0 (Nightmare Virus rays) | 0x80042eb4 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x80043018, 0x80043154 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x80041ba8 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x80041bc8 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x80040800, 0x80041290, 0x80041d5c, 0x80042fa8 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x80040800, 0x80041290, 0x80041e4c, 0x80042fa8, 0x800430d0 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x80040fb4, 0x80042320, 0x8004292c |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x80040b84, 0x80040cb0, 0x80040e3c, 0x80041c9c, 0x80041ebc, 0x80042fa8, 0x8004322c |
| 0x8002d2d4 | GetRNG | random number | 0x80040a04, 0x80040fb4, 0x80041c24, 0x80041c9c, 0x80041d5c |
| 0x8002dcf4 | TerrainBoxOverlap | TerrainBoxOverlap(a, b): the terrain boxes of a and b overlap | 0x80040800 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x80040800, 0x80042fa8 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x80040800 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x80040800 |
| 0x80042d98 | FindReploid | nearest flagged, visible stage object of type 4 with +0x07 == 1 | 0x8004270c, 0x800429d4 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x80040800 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x80041290, 0x80041d5c, 0x800423a0, 0x80042420 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x80041290, 0x80041d5c |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x80041290 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x80075fd8 | 0x80041ab8 | 0x800404d8, 0x80040800, 0x80041ba8, 0x80041bc8 |
| 0x80076018 | 0x80040800 | 0x80040290, 0x800429cc, 0x80041f30, 0x80041be8, 0x8004202c, 0x80042250, 0x80042658, 0x8004289c |
| 0x80076038 | 0x80041f30 | 0x80041f6c, 0x80041fb0, 0x80041fe4, 0x8004200c |
| 0x80076048 | 0x80041be8 | 0x80041c24, 0x80040a04, 0x80041c9c, 0x80041d38, 0x80041d5c, 0x80041e4c, 0x80040b84, 0x80041ebc, 0x80041f10 |
| 0x8007606c | 0x8004202c | 0x800420cc, 0x80042150, 0x80042214 |
| 0x80076078 | 0x80042250 | 0x8004228c, 0x80042320, 0x800423a0, 0x80042420, 0x800424e0, 0x80042538, 0x8004260c |
| 0x80076094 | 0x80042658 | 0x80042694, 0x800426d8, 0x8004270c, 0x80040cb0, 0x80040e3c, 0x80042780, 0x800427e0, 0x8004283c |
| 0x800760b4 | 0x8004289c | 0x800428d8, 0x80040fb4, 0x8004292c, 0x800429c4 |
| 0x800760c8 | 0x80041af4 | 0x80041290, 0x80042fa8, 0x80043018 |
| 0x800760d4 | 0x80041b30 | 0x80043038, 0x800430d0, 0x80043154 |
| 0x800760e0 | 0x800430d0 | 0x80043174, 0x800432f4 |
| 0x800760e8 | 0x80043174 | 0x800431c8, 0x8004322c, 0x800432c8 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x80011590 | 0x800410b8 | 0x80040fb4 | 8 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 12 | 0x80041af4 |
| misc | 11 | 0x80041b30 |

## Animation tables

### Table 0x80075fb0 (10 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 3 | 12 12 12 11 1 | 0 0 0 0 1 | 0 |
| 1 | 4 5 6 | 8 8 8 | 0 0 0 | 0 |
| 2 | 0 1 7 8 9 10 11 | 8 8 4 4 4 4 4 | 0 0 1 0 0 0 0 | 2 |
| 3 | 12 | 1 | 0 | hold |
| 4 | 0 1 13 14 15 16 17 18 | 8 8 8 4 4 4 4 4 | 0 0 0 0 0 0 0 0 | 3 |
| 5 | 19 24 20 25 21 26 22 27 23 28 | 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 | 0 |
| 6 | 32 33 34 33 | 6 6 6 6 | 0 0 0 0 | 0 |
| 7 | 29 30 31 30 | 6 6 6 6 | 0 0 0 0 | 0 |
| 8 | 35 36 37 38 | 6 6 6 6 | 0 0 0 0 | 0 |
| 9 | 39 40 41 42 | 6 6 6 6 | 0 0 0 0 | 0 |

Note: cut: animation 10 uses sprite 162; the sprite set has 43

### Table 0x8007789c (2 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 14 15 16 17 18 18 | 1 1 1 1 1 96 | 0 0 0 0 0 0 | 4 |
| 1 | 29 30 31 32 33 33 | 2 2 2 2 2 96 | 0 0 0 0 0 0 | 4 |

Note: cut: animation 2 uses sprite 49; the sprite set has 43

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 1 | 0 | 0x80042e68 |
| 0x1 | type | 9 | 0 | 0x800404d8, 0x80042e68 |
| 0x2 | var | 9 | 1 | 0x800404d8, 0x8004228c, 0x80042320, 0x80042420, 0x80042538, 0x800429d4 |
| 0x3 | display | 1 | 14 | 0x800404d8, 0x80041d38, 0x80041d5c, 0x80041e4c, 0x80042420, 0x800424e0, 0x80042538, 0x8004260c, 0x8004283c, 0x800428d8, 0x8004292c, 0x80042e68 |
| 0x4 | state | 1 | 5 | 0x800404d8, 0x80040800, 0x80041ab8, 0x80041d5c, 0x8004260c, 0x8004283c |
| 0x5 | step | 3 | 6 | 0x800404d8, 0x80040800, 0x8004202c, 0x800429d4 |
| 0x6 | step2 | 33 | 35 | 0x800404d8, 0x80040800, 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x80040fb4, 0x80041be8, 0x80041c24, 0x80041c9c, 0x80041d38, 0x80041d5c, 0x80041e4c, 0x80041f30, 0x80041f6c, 0x80041fb0, 0x80041fe4, 0x8004202c, 0x800420cc, 0x80042150, 0x80042250, 0x8004228c, 0x80042320, 0x800423a0, 0x80042420, 0x800424e0, 0x80042658, 0x80042694, 0x800426d8, 0x8004270c, 0x80042780, 0x800427e0, 0x8004289c, 0x800428d8, 0x8004292c, 0x800429d4, 0x80042eb4 |
| 0x7 | act4 | 6 | 7 | 0x80040fb4, 0x80041d5c, 0x80041e4c, 0x80042420, 0x800424e0, 0x80042780, 0x8004283c, 0x800428d8, 0x80042a9c, 0x80042e68 |
| 0x8 | x | 2 | 0 | 0x800404d8, 0x80040800 |
| 0xa | x.px | 11 | 2 | 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x80041d5c, 0x8004202c, 0x800428d8, 0x80042a9c, 0x80042b68, 0x80042c18 |
| 0xc | y | 2 | 0 | 0x800404d8, 0x80040800 |
| 0xe | y.px | 15 | 5 | 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x80040fb4, 0x80041d5c, 0x8004202c, 0x8004228c, 0x80042320, 0x800423a0, 0x80042420, 0x800428d8, 0x8004292c, 0x80042a9c, 0x80042b68 |
| 0x10 | record | 1 | 0 | 0x80042c48 |
| 0x14 | layer | 2 | 1 | 0x800404d8, 0x80042a9c, 0x80042b68 |
| 0x15 | flip | 2 | 11 | 0x800404d8, 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x80042a9c, 0x80042b68, 0x80042c18 |
| 0x16 | priority | 0 | 2 | 0x800404d8 |
| 0x18 | prev_x | 0 | 2 | 0x800404d8, 0x80040800 |
| 0x1c | prev_y | 0 | 2 | 0x800404d8, 0x80040800 |
| 0x20 | speed_x | 6 | 9 | 0x800404d8, 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x8004228c, 0x800428d8 |
| 0x24 | speed_y | 2 | 10 | 0x800404d8, 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x80040fb4, 0x8004228c, 0x800428d8 |
| 0x28 | accel_x | 0 | 6 | 0x800404d8, 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x8004228c |
| 0x2c | gravity | 0 | 6 | 0x800404d8, 0x80040a04, 0x80040b84, 0x80040cb0, 0x80040e3c, 0x8004228c |
| 0x30 | anime_table | 2 | 1 | 0x800404d8, 0x80042a9c, 0x80042b68 |
| 0x3c | sprite_data | 2 | 2 | 0x800404d8, 0x80042a9c, 0x80042b68 |
| 0x40 | tex_cord | 2 | 2 | 0x800404d8, 0x80042a9c, 0x80042b68 |
| 0x42 | clut_cord | 7 | 6 | 0x800404d8, 0x80040800, 0x80040cb0, 0x80040e3c, 0x8004283c, 0x80042a9c, 0x80042b68 |
| 0x45 | anime.flag | 3 | 0 | 0x80041fe4, 0x8004270c, 0x80042780 |
| 0x50 | attack_box | 0 | 8 | 0x800404d8, 0x80040800, 0x80041d38, 0x80041e4c, 0x80042420, 0x80042538 |
| 0x54 | hurt_box | 1 | 8 | 0x800404d8, 0x80040800, 0x80040cb0, 0x80040e3c, 0x80041d38, 0x80041e4c, 0x80042420, 0x80042538 |
| 0x58 | damage_table | 0 | 1 | 0x800404d8 |
| 0x5c | hp | 2 | 4 | 0x800404d8, 0x80042538 |
| 0x60 | contact_damage | 0 | 5 | 0x800404d8, 0x80040800, 0x80042538 |
| 0x61 | iframes | 0 | 1 | 0x800404d8 |
| 0x67 | latch | 0 | 1 | 0x800404d8 |
| 0x68 | terrain_box | 0 | 6 | 0x800404d8, 0x80040800, 0x80042538 |
| 0x7c | work.7C | 17 | 31 | 0x80040a04, 0x80040b84, 0x80040fb4, 0x80041c9c, 0x80041d38, 0x80041d5c, 0x80041ebc, 0x80041f6c, 0x80041fb0, 0x800420cc, 0x80042150, 0x80042214, 0x80042320, 0x800423a0, 0x80042420, 0x800424e0, 0x80042538, 0x8004260c, 0x80042694, 0x800426d8, 0x80042780, 0x800427e0, 0x8004283c, 0x800428d8 |
| 0x7e | work.7E | 4 | 6 | 0x80040fb4, 0x80041c24, 0x80041c9c, 0x800420cc, 0x80042150, 0x800428d8 |
| 0x80 | work.80 | 6 | 0 | 0x80040cb0, 0x80040e3c, 0x8004270c, 0x800429d4 |
| 0x84 | work.84 | 3 | 1 | 0x80040b84, 0x80041d5c |
| 0x86 | self+0x86 | 7 | 2 | 0x80040b84, 0x80041d5c, 0x8004228c, 0x80042320, 0x800423a0, 0x80042420 |
| 0x88 | work.88 | 2 | 2 | 0x800404d8, 0x80042ce8, 0x80042d3c |
| 0x8a | work.8A | 2 | 4 | 0x800404d8, 0x80040800, 0x8004228c, 0x800423a0, 0x8004292c |
| 0x8c | work.8C | 0 | 1 | 0x800404d8 |
| 0x8d | self+0x8d | 1 | 8 | 0x800404d8, 0x80040800, 0x80040cb0, 0x80040e3c, 0x8004270c |
| 0x8e | self+0x8e | 1 | 5 | 0x800404d8, 0x80040800, 0x80042420 |
| 0x8f | self+0x8f | 3 | 5 | 0x800404d8, 0x80042538, 0x8004270c, 0x800429d4 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800404d8 |
| 0x1f800064 | current_misc | 0x800431c8, 0x8004322c |
| 0x8007349c | enemy_set_tbl | 0x80042c48 |
| 0x800743dc | dmg_tbl_normal | 0x800404d8 |
| 0x80075ebc | virus_pattern_tbl | 0x800429d4 |
| 0x80075edc | virus_pattern_tbl+0x20 | 0x800404d8, 0x80041e4c, 0x80042538 |
| 0x80075ee0 | virus_pattern_tbl+0x24 | 0x80040800 |
| 0x80075ee4 | virus_pattern_tbl+0x28 | 0x800404d8, 0x80041e4c, 0x80042538 |
| 0x80075fb0 | virus_pattern_tbl+0xf4 | 0x800404d8 |
| 0x80075fe8 | soul_row_tbl | 0x80042ce8, 0x80042d3c |
| 0x800760c4 | soul_row_tbl+0xdc | 0x80041290 |
| 0x80095098 | bank_tpage | 0x800404d8 |
| 0x800970a0 | mega | 0x80040800, 0x80040a04, 0x80041290, 0x80041d5c, 0x8004292c |
| 0x800970aa | mega.x.px | 0x800428d8, 0x80042c18 |
| 0x800970ae | mega.y.px | 0x80040fb4 |
| 0x800970b4 | mega.layer | 0x800404d8 |
| 0x800971f8 | camera_layer0 | 0x8004202c |
| 0x80097206 | cam.y | 0x80040fb4, 0x800428d8 |
| 0x80097420 | frame_count | 0x800423a0, 0x80042420, 0x80043174 |
| 0x800cced0 | game | 0x80040800, 0x8004292c, 0x80042c48, 0x80042ce8, 0x80042d3c |
| 0x800ccf38 | game.difficulty | 0x800404d8, 0x80041c9c, 0x800420cc, 0x80042538 |
| 0x800cd340 | actor2 | 0x80040800 |
