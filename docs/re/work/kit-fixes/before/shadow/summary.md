# RE kit: nightmare-shadow

- Object: nightmare-shadow
- State: `turtloid-1.state.gz` (SHA-1 `da8082595c9fc59c1fff107999f384a674b9a63d`), stage 6 area 0
- Class: main, type 26; entry function 0x800ef004
- Functions walked: 11; state tables: 3

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ee9e4, 0x800eef04 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ee9e4 |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x800eeccc |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ee9e4, 0x800ef124, 0x800ef19c |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ef060 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ef1bc |
| 0x8002d2d4 | GetRNG | random number | 0x800ee9e4, 0x800eeccc |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ef004 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ef060 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800ef060 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ef060 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800eef04 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800eef04 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800eef04 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6360 | 0x800ef004 | 0x800ee9e4, 0x800ef060, 0x800ef124, 0x800ef19c |
| 0x800f6370 | 0x800ef060 | 0x80040290, 0x800eeccc, 0x800ef1bc, 0x800ef1dc |
| 0x800f6380 | 0x800ef1dc | 0x800ef218, 0x800ef264, 0x800eef04 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f6350 (4 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 1 | 1 | 0 | hold |
| 1 | 0 | 1 | 0 | hold |
| 2 | 3 | 1 | 0 | hold |
| 3 | 2 | 1 | 0 | hold |

Note: cut to the export's 4 animations; 4 more pointer-like words follow (often the next table)

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800ee9e4 |
| 0x1 | type | 4 | 0 | 0x800ee9e4 |
| 0x2 | var | 4 | 0 | 0x800ee9e4, 0x800eeccc, 0x800eef04, 0x800ef060 |
| 0x3 | display | 1 | 5 | 0x800ee9e4, 0x800ef060, 0x800ef218, 0x800ef264 |
| 0x4 | state | 1 | 3 | 0x800ee9e4, 0x800ef004, 0x800ef060 |
| 0x5 | step | 1 | 3 | 0x800ee9e4, 0x800eef04, 0x800ef060 |
| 0x6 | step2 | 3 | 4 | 0x800ee9e4, 0x800eef04, 0x800ef1dc, 0x800ef218, 0x800ef264 |
| 0x8 | x | 3 | 0 | 0x800ee9e4, 0x800eef04, 0x800ef004 |
| 0xa | x.px | 2 | 2 | 0x800eef04, 0x800ef060, 0x800ef124 |
| 0xc | y | 1 | 0 | 0x800ef004 |
| 0xe | y.px | 1 | 0 | 0x800eef04 |
| 0x14 | layer | 1 | 1 | 0x800ee9e4, 0x800eeccc |
| 0x15 | flip | 1 | 2 | 0x800ee9e4, 0x800eeccc, 0x800eef04 |
| 0x16 | priority | 0 | 1 | 0x800ee9e4 |
| 0x18 | prev_x | 0 | 1 | 0x800ef004 |
| 0x1c | prev_y | 0 | 1 | 0x800ef004 |
| 0x20 | speed_x | 0 | 1 | 0x800eef04 |
| 0x24 | speed_y | 0 | 1 | 0x800eef04 |
| 0x30 | anime_table | 1 | 1 | 0x800ee9e4, 0x800eeccc |
| 0x3c | sprite_data | 1 | 1 | 0x800ee9e4, 0x800eeccc |
| 0x40 | tex_cord | 1 | 1 | 0x800ee9e4, 0x800eeccc |
| 0x42 | clut_cord | 2 | 2 | 0x800ee9e4, 0x800eeccc, 0x800ef060 |
| 0x50 | attack_box | 0 | 3 | 0x800ee9e4, 0x800eef04, 0x800ef218 |
| 0x54 | hurt_box | 0 | 3 | 0x800ee9e4, 0x800eef04, 0x800ef218 |
| 0x58 | damage_table | 0 | 1 | 0x800ee9e4 |
| 0x5c | hp | 0 | 1 | 0x800ee9e4 |
| 0x60 | contact_damage | 0 | 1 | 0x800ee9e4 |
| 0x61 | iframes | 0 | 1 | 0x800ee9e4 |
| 0x67 | latch | 0 | 1 | 0x800ee9e4 |
| 0x68 | terrain_box | 0 | 1 | 0x800ee9e4 |
| 0x7c | work.7C | 5 | 10 | 0x800ee9e4, 0x800eeccc, 0x800eef04, 0x800ef060, 0x800ef124, 0x800ef218, 0x800ef264 |
| 0x80 | work.80 | 1 | 2 | 0x800ef060, 0x800ef124 |
| 0x82 | self+0x82 | 1 | 2 | 0x800ef060, 0x800ef124 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ee9e4 |
| 0x8007497c | dmg_tbl_nightmare_shadow | 0x800ee9e4 |
| 0x80095098 | bank_tpage | 0x800ee9e4 |
| 0x800970a0 | mega | 0x800ee9e4, 0x800eeccc, 0x800eef04 |
| 0x800970aa | mega.x.px | 0x800ee9e4, 0x800eeccc |
| 0x80097420 | frame_count | 0x800ef124 |
| 0x800cd30a | nightmare_effect | 0x800ee9e4 |
| 0x800e9864 |  | 0x800ee9e4 |
| 0x800e9894 |  | 0x800eeccc |
| 0x800f6328 |  | 0x800ef218 |
| 0x800f632c |  | 0x800eef04 |
| 0x800f6334 |  | 0x800ef218 |
| 0x800f6338 |  | 0x800eef04 |
| 0x800f6350 |  | 0x800ee9e4 |
