# RE kit: rocket-platform

- Object: rocket-platform
- State: `turtloid-2_mid.state.gz` (SHA-1 `7a68e6e00140207ee3a3c50918944fedb3e710d4`), stage 6 area 0
- Class: stage, type 5; entry function 0x800ecef4
- Functions walked: 17; state tables: 3

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800ed3e4, 0x800ed69c, 0x800ed850 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ecd20, 0x800ed0b4 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ecf40 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ecd20, 0x800ed92c |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ecfec, 0x800ed92c, 0x800eda70, 0x800edabc |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ecf40, 0x800eda70 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ed014, 0x800ed0b4 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800ecf40 |
| 0x8003208c | SolidPushShots | SolidPush of obj against the visual objects and the weapon objects that have +0x9D set; keeps a touch bit per object in obj+0x90 (guess) | 0x800ecf40 |
| 0x80032b34 | SolidWeaponHit | weapon objects whose terrain box overlaps obj's terrain box: 0x800312DC, then ApplyWeaponHit; returns the first non-zero result (likely) | 0x800ecf40 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x8002b3d4 | 0x800ed18c, 0x800ed69c, 0x800ed774, 0x800ed850 |
| 0x8002b410 | 0x800ed69c, 0x800ed774, 0x800ed850 |
| 0x80031f4c | 0x800ecf40 |
| 0x80032be8 | 0x800ecf40 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6094 | 0x800ecef4 | 0x800ecd20, 0x800ecf40, 0x800ecfec |
| 0x800f60a0 | 0x800ecf40 | 0x800ed00c, 0x800ed0b4, 0x800ed014, 0x800ed5c4, 0x800ed69c, 0x800ed774, 0x800ed3e4, 0x800ed850 |
| 0x800f60c0 | 0x800eda34 | 0x800ed92c, 0x800eda70, 0x800edabc |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| misc | 20 | 0x800eda34 |

## Animation tables

### Table 0x800f6084 (2 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 6 6 | 2 2 2 2 2 2 5 1 | 0 0 0 0 0 0 0 0 | 0 |
| 1 | 0 1 12 7 8 13 10 11 6 6 | 2 2 2 2 2 2 2 2 1 1 | 0 0 0 0 0 0 0 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 1 | 2 | 0x800ecd20, 0x800ed5c4 |
| 0x2 | var | 1 | 0 | 0x800ecd20 |
| 0x4 | state | 9 | 7 | 0x800ecd20, 0x800ecef4, 0x800ed3e4, 0x800ed5c4, 0x800ed69c, 0x800ed774, 0x800ed850 |
| 0x5 | step | 0 | 1 | 0x800ecd20 |
| 0x6 | step2 | 1 | 2 | 0x800ecd20, 0x800ecf40 |
| 0x8 | x | 2 | 0 | 0x800ecd20, 0x800ecef4 |
| 0xa | x.px | 3 | 0 | 0x800ecd20, 0x800ed014 |
| 0xc | y | 1 | 0 | 0x800ecef4 |
| 0xe | y.px | 3 | 0 | 0x800ecd20, 0x800ed0b4 |
| 0x14 | layer | 0 | 2 | 0x800ecd20, 0x800ed69c |
| 0x15 | flip | 0 | 1 | 0x800ecd20 |
| 0x16 | priority | 1 | 4 | 0x800ecd20, 0x800ed3e4, 0x800ed69c |
| 0x18 | prev_x | 1 | 4 | 0x800ecef4, 0x800ed3e4, 0x800ed69c |
| 0x1a | self+0x1a | 9 | 4 | 0x800ed69c, 0x800ed774, 0x800ed850 |
| 0x1c | prev_y | 0 | 1 | 0x800ecef4 |
| 0x1e | self+0x1e | 0 | 1 | 0x800ed69c |
| 0x20 | speed_x | 2 | 7 | 0x800ecd20, 0x800ed014, 0x800ed0b4, 0x800ed5c4 |
| 0x24 | speed_y | 2 | 4 | 0x800ecd20, 0x800ed0b4 |
| 0x28 | accel_x | 0 | 1 | 0x800ed0b4 |
| 0x30 | anime_table | 0 | 1 | 0x800ecd20 |
| 0x3c | sprite_data | 0 | 1 | 0x800ecd20 |
| 0x40 | tex_cord | 0 | 1 | 0x800ecd20 |
| 0x42 | clut_cord | 0 | 1 | 0x800ecd20 |
| 0x54 | hurt_box | 0 | 1 | 0x800ecd20 |
| 0x58 | damage_table | 0 | 1 | 0x800ecd20 |
| 0x60 | contact_damage | 0 | 1 | 0x800ecd20 |
| 0x61 | iframes | 0 | 1 | 0x800ecd20 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800ecd20 |
| 0x68 | terrain_box | 0 | 1 | 0x800ecd20 |
| 0x75 | self+0x75 | 0 | 1 | 0x800ecd20 |
| 0x76 | self+0x76 | 0 | 1 | 0x800ecd20 |
| 0x7c | work.7C | 2 | 1 | 0x800ecd20, 0x800ed014 |
| 0x7e | work.7E | 2 | 1 | 0x800ecd20, 0x800ed0b4 |
| 0x80 | work.80 | 2 | 2 | 0x800ed014, 0x800ed0b4 |
| 0x84 | work.84 | 2 | 3 | 0x800ecd20, 0x800ed014, 0x800ed0b4 |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800ecd20 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ecd20, 0x800ed92c |
| 0x1f800030 | sprite_bank_base+0x10 | 0x800ed5c4 |
| 0x80075b9c | dmg_tbl_capsule | 0x800ecd20 |
| 0x8008eab0 | stage_texture_swap | 0x800ed5c4 |
| 0x8008eafc | text_open | 0x800ed3e4 |
| 0x80095098 | bank_tpage | 0x800ecd20, 0x800ed92c |
| 0x800970a0 | mega | 0x800ecd20, 0x800ecf40, 0x800ed3e4 |
| 0x800971f8 | camera_layer0 | 0x800ed69c |
| 0x800cceed | game.checkpoint | 0x800ed5c4, 0x800ed92c, 0x800eda70 |
| 0x800ccef6 | game.stage_flag | 0x800ed3e4, 0x800ed69c, 0x800ed774, 0x800ed850 |
| 0x800f6084 |  | 0x800ecd20 |
| 0x800f608c |  | 0x800ecd20 |
| 0x800f6090 |  | 0x800ecd20 |
| 0x800f6ba4 |  | 0x800ed5c4, 0x800ed69c, 0x800ed774, 0x800ed850 |
| 0x800f6bc4 |  | 0x800ed18c, 0x800ed69c, 0x800ed774, 0x800ed850 |
