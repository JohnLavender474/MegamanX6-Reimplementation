# RE kit: wolfang-zero-nightmare

- Object: wolfang-zero-nightmare
- State: `wolfang-2_ice-slope.state.gz` (SHA-1 `9fe41277436027d7fe980750634fcbdd7ac8afd1`), stage 2 area 0
- Class: main, type 26; entry function 0x800efd58
- Functions walked: 11; state tables: 3

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ef738, 0x800efc58 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ef738 |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x800efa20 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ef738, 0x800efe78, 0x800efef0 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800efdb4 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800eff10 |
| 0x8002d2d4 | GetRNG | random number | 0x800ef738, 0x800efa20 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800efd58 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800efdb4 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800efdb4 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800efdb4 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800efc58 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800efc58 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800efc58 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6dc8 | 0x800efd58 | 0x800ef738, 0x800efdb4, 0x800efe78, 0x800efef0 |
| 0x800f6dd8 | 0x800efdb4 | 0x80040290, 0x800efa20, 0x800eff10, 0x800eff30 |
| 0x800f6de8 | 0x800eff30 | 0x800eff6c, 0x800effb8, 0x800efc58 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e9ad0 | 0x800ef840 | 0x800ef738 | 12 |
| 0x800e9b00 | 0x800efa5c | 0x800efa20 | 6 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f6db8 (4 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 1 | 1 | 0 | hold |
| 1 | 0 | 1 | 0 | hold |
| 2 | 3 | 1 | 0 | hold |
| 3 | 2 | 1 | 0 | hold |

Note: cut: animation 4 uses sprite 39; the sprite set has 4

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800ef738 |
| 0x1 | type | 4 | 0 | 0x800ef738 |
| 0x2 | var | 4 | 0 | 0x800ef738, 0x800efa20, 0x800efc58, 0x800efdb4 |
| 0x3 | display | 1 | 5 | 0x800ef738, 0x800efdb4, 0x800eff6c, 0x800effb8 |
| 0x4 | state | 1 | 3 | 0x800ef738, 0x800efd58, 0x800efdb4 |
| 0x5 | step | 1 | 3 | 0x800ef738, 0x800efc58, 0x800efdb4 |
| 0x6 | step2 | 3 | 4 | 0x800ef738, 0x800efc58, 0x800eff30, 0x800eff6c, 0x800effb8 |
| 0x8 | x | 3 | 0 | 0x800ef738, 0x800efc58, 0x800efd58 |
| 0xa | x.px | 2 | 2 | 0x800efc58, 0x800efdb4, 0x800efe78 |
| 0xc | y | 1 | 0 | 0x800efd58 |
| 0xe | y.px | 1 | 0 | 0x800efc58 |
| 0x14 | layer | 1 | 1 | 0x800ef738, 0x800efa20 |
| 0x15 | flip | 1 | 2 | 0x800ef738, 0x800efa20, 0x800efc58 |
| 0x16 | priority | 0 | 1 | 0x800ef738 |
| 0x18 | prev_x | 0 | 1 | 0x800efd58 |
| 0x1c | prev_y | 0 | 1 | 0x800efd58 |
| 0x20 | speed_x | 0 | 1 | 0x800efc58 |
| 0x24 | speed_y | 0 | 1 | 0x800efc58 |
| 0x30 | anime_table | 1 | 1 | 0x800ef738, 0x800efa20 |
| 0x3c | sprite_data | 1 | 1 | 0x800ef738, 0x800efa20 |
| 0x40 | tex_cord | 1 | 1 | 0x800ef738, 0x800efa20 |
| 0x42 | clut_cord | 2 | 2 | 0x800ef738, 0x800efa20, 0x800efdb4 |
| 0x50 | attack_box | 0 | 3 | 0x800ef738, 0x800efc58, 0x800eff6c |
| 0x54 | hurt_box | 0 | 3 | 0x800ef738, 0x800efc58, 0x800eff6c |
| 0x58 | damage_table | 0 | 1 | 0x800ef738 |
| 0x5c | hp | 0 | 1 | 0x800ef738 |
| 0x60 | contact_damage | 0 | 1 | 0x800ef738 |
| 0x61 | iframes | 0 | 1 | 0x800ef738 |
| 0x67 | latch | 0 | 1 | 0x800ef738 |
| 0x68 | terrain_box | 0 | 1 | 0x800ef738 |
| 0x7c | work.7C | 5 | 10 | 0x800ef738, 0x800efa20, 0x800efc58, 0x800efdb4, 0x800efe78, 0x800eff6c, 0x800effb8 |
| 0x80 | work.80 | 1 | 2 | 0x800efdb4, 0x800efe78 |
| 0x82 | self+0x82 | 1 | 2 | 0x800efdb4, 0x800efe78 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ef738 |
| 0x8007497c | dmg_tbl_nightmare_shadow | 0x800ef738 |
| 0x80095098 | bank_tpage | 0x800ef738 |
| 0x800970a0 | mega | 0x800ef738, 0x800efa20, 0x800efc58 |
| 0x800970aa | mega.x.px | 0x800ef738, 0x800efa20 |
| 0x80097420 | frame_count | 0x800efe78 |
| 0x800cd30a | nightmare_effect | 0x800ef738 |
| 0x800f6d90 |  | 0x800eff6c |
| 0x800f6d94 |  | 0x800efc58 |
| 0x800f6d9c |  | 0x800eff6c |
| 0x800f6da0 |  | 0x800efc58 |
| 0x800f6db8 |  | 0x800ef738 |
