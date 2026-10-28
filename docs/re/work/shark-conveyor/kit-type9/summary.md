# RE kit: shark-conveyor

- Object: shark-conveyor
- State: `shark-2_section-2.state.gz` (SHA-1 `2ed5581179aa6acd6cf436aac85487b11ea49bd0`), stage 4 area 1
- Class: stage, type 9; entry function 0x800ee3f8
- Functions walked: 5; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800eddd8 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ee2bc |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ee444 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800edf90 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ee2bc |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800edf90 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x80031f4c | 0x800edf90 |
| 0x8003208c | 0x800edf90 |
| 0x80032b34 | 0x800edf90 |
| 0x80032be8 | 0x800edf90 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f8cd8 | 0x800ee3f8 | 0x800eddd8, 0x800edf90, 0x800ee444, 0x800ee2bc |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f8c7c (2 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 6 6 | 2 2 2 2 2 2 5 1 | 0 0 0 0 0 0 0 0 | 0 |
| 1 | 0 1 12 7 8 13 10 11 6 6 | 2 2 2 2 2 2 2 2 1 1 | 0 0 0 0 0 0 0 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800eddd8 |
| 0x2 | var | 4 | 0 | 0x800eddd8, 0x800edf90, 0x800ee2bc |
| 0x3 | display | 1 | 2 | 0x800edf90 |
| 0x4 | state | 2 | 1 | 0x800eddd8, 0x800ee3f8 |
| 0x5 | step | 0 | 1 | 0x800eddd8 |
| 0x6 | step2 | 1 | 1 | 0x800eddd8, 0x800edf90 |
| 0x8 | x | 1 | 0 | 0x800ee3f8 |
| 0xa | x.px | 1 | 0 | 0x800edf90 |
| 0xc | y | 1 | 0 | 0x800ee3f8 |
| 0xe | y.px | 2 | 1 | 0x800eddd8, 0x800edf90, 0x800ee2bc |
| 0x15 | flip | 0 | 1 | 0x800eddd8 |
| 0x18 | prev_x | 0 | 1 | 0x800ee3f8 |
| 0x1c | prev_y | 0 | 1 | 0x800ee3f8 |
| 0x30 | anime_table | 0 | 1 | 0x800eddd8 |
| 0x50 | attack_box | 0 | 1 | 0x800eddd8 |
| 0x54 | hurt_box | 0 | 2 | 0x800eddd8 |
| 0x58 | damage_table | 0 | 1 | 0x800eddd8 |
| 0x60 | contact_damage | 0 | 1 | 0x800eddd8 |
| 0x61 | iframes | 0 | 1 | 0x800eddd8 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800eddd8 |
| 0x68 | terrain_box | 0 | 2 | 0x800eddd8 |
| 0x72 | self+0x72 | 5 | 0 | 0x800edf90, 0x800ee2bc |
| 0x75 | self+0x75 | 0 | 1 | 0x800eddd8 |
| 0x76 | self+0x76 | 4 | 1 | 0x800eddd8, 0x800edf90 |
| 0x7c | work.7C | 0 | 4 | 0x800eddd8 |
| 0x7d | self+0x7d | 0 | 2 | 0x800eddd8 |
| 0x7e | work.7E | 0 | 2 | 0x800eddd8 |
| 0x7f | self+0x7f | 0 | 4 | 0x800eddd8 |
| 0x80 | work.80 | 0 | 2 | 0x800eddd8 |
| 0x81 | self+0x81 | 0 | 2 | 0x800eddd8 |
| 0x82 | self+0x82 | 0 | 2 | 0x800eddd8 |
| 0x83 | self+0x83 | 0 | 2 | 0x800eddd8 |
| 0x86 | self+0x86 | 1 | 1 | 0x800eddd8, 0x800ee2bc |
| 0x88 | work.88 | 0 | 1 | 0x800eddd8 |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800eddd8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x80075b9c | dmg_tbl_capsule | 0x800eddd8 |
| 0x800970a0 | mega | 0x800edf90 |
| 0x800970a5 | mega.var+0x3 | 0x800eddd8 |
| 0x800970aa | mega.x.px | 0x800ee2bc |
| 0x800971f8 | camera_layer0 | 0x800ee2bc |
| 0x800cced0 | game | 0x800edf90, 0x800ee2bc |
| 0x800f8c7c |  | 0x800eddd8 |
| 0x800f8c84 |  | 0x800eddd8 |
| 0x800f8cac |  | 0x800eddd8 |
| 0x800f8cb0 |  | 0x800eddd8 |
| 0x800f8ce4 |  | 0x800edf90 |
| 0x800f9860 |  | 0x800eddd8 |
| 0x800f9864 |  | 0x800eddd8 |
