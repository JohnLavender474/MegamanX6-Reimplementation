# RE kit: ice-block

- Object: ice-block
- State: `/home/johnlavender/mmx-data/x6/dumps/wolfang-3_falling-ice.state.gz` (SHA-1 `7a3a9a1b7d8520a56eb02742dc0bb36027f99492`), stage 2 area 0
- Class: main, type 17; entry function 0x800eed40
- Functions walked: 8; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ee78c |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ee334 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ee78c, 0x800eedd8, 0x800eee14 |
| 0x8002943c | Quake2 | screen shake variant: camera work +0x36/+0x45/+0x3E/+0x3C/+0x3A, flag 0x10 | 0x800ee78c |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x800ee8d8 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800eed40, 0x800eedb8 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ee5c4 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ee78c |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800eedd8 |
| 0x8002d2d4 | GetRNG | random number | 0x800ee334, 0x800ee8d8 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ee78c |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ee5c4 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ee5c4 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ee5c4 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ee5c4, 0x800ee78c |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ee5c4 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6ce8 | 0x800eed40 | 0x800ee334, 0x800ee5c4, 0x800eedb8 |
| 0x800f6cf4 | 0x800ee5c4 | 0x80040290, 0x800eedd8, 0x800ee78c, 0x800ee8d8, 0x800eee14 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e9ab0 | 0x800ee430 | 0x800ee334 | 8 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f6cbc (9 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 6 7 | 8 8 8 8 8 8 8 8 | 0 0 0 0 0 0 0 0 | 0 |
| 1 | 8 9 10 11 12 13 14 15 | 8 8 8 8 8 8 8 8 | 0 0 0 0 0 0 0 0 | 0 |
| 2 | 0 | 1 | 0 | hold |
| 3 | 8 | 1 | 0 | hold |
| 4 | 16 | 1 | 0 | hold |
| 5 | 17 | 1 | 0 | hold |
| 6 | 18 | 1 | 0 | hold |
| 7 | 19 | 1 | 0 | hold |
| 8 | 20 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 4 | 0 | 0x800ee334, 0x800ee5c4, 0x800ee8d8 |
| 0x3 | display | 2 | 9 | 0x800ee334, 0x800ee78c, 0x800eee14 |
| 0x4 | state | 1 | 6 | 0x800ee334, 0x800ee5c4, 0x800ee78c, 0x800ee8d8, 0x800eed40 |
| 0x5 | step | 1 | 7 | 0x800ee334, 0x800ee5c4, 0x800eee14 |
| 0x6 | step2 | 0 | 1 | 0x800ee334 |
| 0x7 | act4 | 1 | 2 | 0x800ee334, 0x800eedd8 |
| 0x8 | x | 2 | 0 | 0x800ee334, 0x800ee5c4 |
| 0xa | x.px | 8 | 0 | 0x800ee334, 0x800ee5c4, 0x800ee8d8 |
| 0xc | y | 2 | 0 | 0x800ee334, 0x800ee5c4 |
| 0xe | y.px | 4 | 0 | 0x800ee8d8 |
| 0x14 | layer | 0 | 1 | 0x800ee334 |
| 0x15 | flip | 0 | 3 | 0x800ee334 |
| 0x16 | priority | 0 | 1 | 0x800ee334 |
| 0x18 | prev_x | 0 | 2 | 0x800ee334, 0x800ee5c4 |
| 0x1c | prev_y | 0 | 2 | 0x800ee334, 0x800ee5c4 |
| 0x20 | speed_x | 2 | 5 | 0x800ee334, 0x800ee78c |
| 0x24 | speed_y | 1 | 6 | 0x800ee334, 0x800ee5c4, 0x800ee78c |
| 0x28 | accel_x | 0 | 1 | 0x800ee334 |
| 0x2c | gravity | 0 | 3 | 0x800ee334, 0x800ee78c |
| 0x30 | anime_table | 4 | 1 | 0x800ee334, 0x800ee8d8 |
| 0x3c | sprite_data | 4 | 0 | 0x800ee8d8 |
| 0x40 | tex_cord | 4 | 0 | 0x800ee8d8 |
| 0x42 | clut_cord | 4 | 0 | 0x800ee8d8 |
| 0x50 | attack_box | 0 | 9 | 0x800ee334, 0x800ee78c, 0x800eee14 |
| 0x54 | hurt_box | 0 | 9 | 0x800ee334, 0x800ee78c, 0x800eee14 |
| 0x58 | damage_table | 0 | 1 | 0x800ee334 |
| 0x5c | hp | 0 | 1 | 0x800ee334 |
| 0x60 | contact_damage | 0 | 1 | 0x800ee334 |
| 0x61 | iframes | 0 | 1 | 0x800ee334 |
| 0x67 | latch | 0 | 4 | 0x800ee334, 0x800ee5c4 |
| 0x68 | terrain_box | 0 | 2 | 0x800ee334 |
| 0x70 | collide_flags | 3 | 1 | 0x800ee334, 0x800ee5c4, 0x800ee78c |
| 0x7c | work.7C | 6 | 10 | 0x800ee334, 0x800ee78c, 0x800ee8d8, 0x800eee14 |
| 0x7e | work.7E | 12 | 3 | 0x800ee334, 0x800ee8d8 |
| 0x80 | work.80 | 10 | 1 | 0x800ee334, 0x800ee78c, 0x800eee14 |
| 0x81 | self+0x81 | 1 | 3 | 0x800ee334, 0x800ee8d8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x80075cdc |  | 0x800ee334 |
| 0x8008ceed |  | 0x800ee8d8 |
| 0x800970a0 | mega | 0x800ee334, 0x800ee5c4, 0x800eed40 |
| 0x800970aa | mega.x.px | 0x800ee5c4 |
| 0x800cced0 | game | 0x800ee5c4 |
| 0x800cceed | game.checkpoint | 0x800ee5c4 |
| 0x800f6c48 |  | 0x800ee334 |
| 0x800f6c50 |  | 0x800ee334, 0x800ee78c, 0x800eee14 |
| 0x800f6c58 |  | 0x800ee334, 0x800ee78c, 0x800eee14 |
| 0x800f6cbc |  | 0x800ee334 |
| 0x800f6ce0 |  | 0x800ee5c4, 0x800ee78c |
