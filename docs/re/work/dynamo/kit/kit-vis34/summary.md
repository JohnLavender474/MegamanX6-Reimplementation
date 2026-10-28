# RE kit: dynamo

- Object: dynamo
- State: `mijinion-4_dynamo.state.gz` (SHA-1 `304866452aea348f0d8d335237bd04c9a2538f7f`), stage 22 area 1
- Class: visual, type 34; entry function 0x800fc7d8
- Functions walked: 7; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800fa984, 0x800fabc0 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800fa984, 0x800fabc0, 0x800fc8cc |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800fabc0, 0x800fc9ac |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800fc898, 0x800fca64 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800fabc0, 0x800fc9ac |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800fabc0 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800fc9ac |
| 0x8002d2d4 | GetRNG | random number | 0x800fabc0 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800fc7d8 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800fabc0, 0x800fc9ac |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800fabc0 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800fa984, 0x800fabc0 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800fa984, 0x800fabc0 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800fa984, 0x800fabc0 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fce30 | 0x800fc7d8 | 0x800fa984, 0x800fabc0, 0x800fc898, 0x800fc8cc, 0x800fc9ac, 0x800fca64 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800fa014 | 0x800fac58 | 0x800fabc0 | 5 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 4 | 0 | 0x800fa984, 0x800fc7d8 |
| 0x3 | display | 0 | 3 | 0x800fa984, 0x800fc7d8, 0x800fc8cc |
| 0x4 | state | 4 | 8 | 0x800fa984, 0x800fabc0, 0x800fc7d8, 0x800fc8cc, 0x800fc9ac |
| 0x5 | step | 2 | 3 | 0x800fa984, 0x800fabc0, 0x800fc8cc, 0x800fc9ac |
| 0x6 | step2 | 9 | 4 | 0x800fa984, 0x800fabc0 |
| 0xa | x.px | 9 | 1 | 0x800fa984, 0x800fabc0 |
| 0xe | y.px | 8 | 3 | 0x800fa984, 0x800fabc0, 0x800fc8cc |
| 0x14 | layer | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x15 | flip | 2 | 8 | 0x800fa984, 0x800fabc0, 0x800fc8cc |
| 0x16 | priority | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x20 | speed_x | 5 | 12 | 0x800fa984, 0x800fabc0, 0x800fc8cc |
| 0x24 | speed_y | 1 | 9 | 0x800fa984, 0x800fabc0, 0x800fc8cc |
| 0x28 | accel_x | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x2c | gravity | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x30 | anime_table | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x3c | sprite_data | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x40 | tex_cord | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x42 | clut_cord | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x50 | attack_box | 0 | 4 | 0x800fa984, 0x800fabc0, 0x800fc8cc, 0x800fc9ac |
| 0x54 | hurt_box | 0 | 3 | 0x800fa984, 0x800fabc0, 0x800fc8cc |
| 0x58 | damage_table | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x5c | hp | 0 | 3 | 0x800fa984, 0x800fabc0, 0x800fc8cc |
| 0x60 | contact_damage | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x67 | latch | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x68 | terrain_box | 0 | 2 | 0x800fa984, 0x800fc8cc |
| 0x70 | collide_flags | 1 | 0 | 0x800fc7d8 |
| 0x84 | work.84 | 1 | 0 | 0x800fc7d8 |
| 0x8c | work.8C | 6 | 9 | 0x800fabc0, 0x800fc8cc, 0x800fc9ac |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x80075afc |  | 0x800fa984 |
| 0x800970a0 | mega | 0x800fabc0 |
| 0x800fce24 |  | 0x800fa984 |
| 0x800fce28 |  | 0x800fa984 |
| 0x800fce2c |  | 0x800fc9ac |
| 0x800fce4c |  | 0x800fabc0 |
| 0x800fce4e |  | 0x800fabc0 |
| 0x800fce52 |  | 0x800fabc0 |
| 0x800fce54 |  | 0x800fabc0 |
| 0x800fce58 |  | 0x800fabc0, 0x800fc9ac |
| 0x800fce5c |  | 0x800fabc0, 0x800fc9ac |
| 0x800fce60 |  | 0x800fa984, 0x800fabc0, 0x800fc898, 0x800fc8cc |
