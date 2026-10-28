# RE kit: shark-open/kit-st11-a1

- Object: shark-open/kit-st11-a1
- State: `shark-2_section-2.state.gz` (SHA-1 `2ed5581179aa6acd6cf436aac85487b11ea49bd0`), stage 4 area 1
- Class: stage, type 11; entry function 0x800eea90
- Functions walked: 4; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ee464 |
| 0x8002c9d4 | DeleteObjectKeep | record flags |= 0x80 (the spawner skips flags & 0x81), then DeleteObject3: single-use objects | 0x800eeafc |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800eeadc |
| 0x800ee790 | JunkBlockRun7 | stage 4 overlay 07: stage type 11 run state; area != 0: foot cell 58 and left side free -> x -= 1 (rides the belt); var bit 7 waits for player x >= 1088 | 0x800eea90 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f8d40 | 0x800eea90 | 0x800ee464, 0x800ee790, 0x800eeafc, 0x800eeadc |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f8d08 (8 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 | 12 | 0 | hold |
| 1 | 1 | 12 | 0 | hold |
| 2 | 2 | 1 | 0 | hold |
| 3 | 3 | 1 | 0 | hold |
| 4 | 4 | 1 | 0 | hold |
| 5 | 5 | 1 | 0 | hold |
| 6 | 6 | 1 | 0 | hold |
| 7 | 7 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800ee464 |
| 0x2 | var | 5 | 1 | 0x800ee464 |
| 0x4 | state | 2 | 1 | 0x800ee464, 0x800eea90 |
| 0x5 | step | 0 | 1 | 0x800ee464 |
| 0x6 | step2 | 0 | 1 | 0x800ee464 |
| 0x7 | act4 | 0 | 2 | 0x800ee464 |
| 0x8 | x | 1 | 0 | 0x800eea90 |
| 0xc | y | 1 | 0 | 0x800eea90 |
| 0x14 | layer | 0 | 1 | 0x800ee464 |
| 0x15 | flip | 0 | 1 | 0x800ee464 |
| 0x16 | priority | 0 | 2 | 0x800ee464 |
| 0x18 | prev_x | 0 | 1 | 0x800eea90 |
| 0x1c | prev_y | 0 | 1 | 0x800eea90 |
| 0x30 | anime_table | 0 | 1 | 0x800ee464 |
| 0x3c | sprite_data | 0 | 1 | 0x800ee464 |
| 0x40 | tex_cord | 0 | 1 | 0x800ee464 |
| 0x42 | clut_cord | 0 | 1 | 0x800ee464 |
| 0x47 | anime.sprite | 0 | 1 | 0x800ee464 |
| 0x50 | attack_box | 0 | 1 | 0x800ee464 |
| 0x54 | hurt_box | 0 | 1 | 0x800ee464 |
| 0x58 | damage_table | 0 | 1 | 0x800ee464 |
| 0x5c | hp | 0 | 1 | 0x800ee464 |
| 0x60 | contact_damage | 0 | 1 | 0x800ee464 |
| 0x61 | iframes | 0 | 1 | 0x800ee464 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800ee464 |
| 0x68 | terrain_box | 0 | 1 | 0x800ee464 |
| 0x75 | self+0x75 | 0 | 1 | 0x800ee464 |
| 0x76 | self+0x76 | 0 | 1 | 0x800ee464 |
| 0x80 | work.80 | 0 | 2 | 0x800ee464 |
| 0x83 | self+0x83 | 0 | 2 | 0x800ee464 |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800ee464 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ee464 |
| 0x800743dc | dmg_tbl_normal | 0x800ee464 |
| 0x80095098 | bank_tpage | 0x800ee464 |
| 0x800970b4 | mega.layer | 0x800ee464 |
| 0x800f8d08 |  | 0x800ee464 |
| 0x800f8d30 |  | 0x800ee464 |
| 0x800f8d38 |  | 0x800ee464 |
| 0x800f8d3c |  | 0x800ee464 |
