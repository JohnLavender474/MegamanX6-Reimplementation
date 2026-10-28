# RE kit: shark-mini-boss

- Object: shark-mini-boss
- State: `shark-4_mini-boss.state.gz` (SHA-1 `edbd4700710484154fd2fb9a071f9361b0a7f3b7`), stage 4 area 1
- Class: stage, type 11; entry function 0x800eea90
- Functions walked: 6; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ee464 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ee790 |
| 0x8002c9d4 | DeleteObjectKeep | record flags |= 0x80 (the spawner skips flags & 0x81), then DeleteObject3: single-use objects | 0x800eeafc |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800eeadc |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ee790 |
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800ee790 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800ee790 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800ee790 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ee790 |
| 0x80049ebc | SpawnExplosionRand | SpawnExplosionRand(obj, type, mx, my): explosion at a random offset | 0x800ee790 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ee790 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x80031f4c | 0x800ee790 |
| 0x8003208c | 0x800ee790 |
| 0x80032be8 | 0x800ee790 |

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
| 0x2 | var | 6 | 1 | 0x800ee464, 0x800ee790 |
| 0x3 | display | 1 | 0 | 0x800ee790 |
| 0x4 | state | 2 | 2 | 0x800ee464, 0x800ee790, 0x800eea90 |
| 0x5 | step | 0 | 1 | 0x800ee464 |
| 0x6 | step2 | 0 | 1 | 0x800ee464 |
| 0x7 | act4 | 1 | 3 | 0x800ee464, 0x800ee790 |
| 0x8 | x | 1 | 0 | 0x800eea90 |
| 0xa | x.px | 6 | 1 | 0x800ee790 |
| 0xc | y | 1 | 0 | 0x800eea90 |
| 0xe | y.px | 5 | 0 | 0x800ee790 |
| 0x14 | layer | 0 | 1 | 0x800ee464 |
| 0x15 | flip | 0 | 1 | 0x800ee464 |
| 0x16 | priority | 0 | 2 | 0x800ee464 |
| 0x18 | prev_x | 0 | 1 | 0x800eea90 |
| 0x1c | prev_y | 0 | 1 | 0x800eea90 |
| 0x30 | anime_table | 0 | 1 | 0x800ee464 |
| 0x3c | sprite_data | 0 | 1 | 0x800ee464 |
| 0x40 | tex_cord | 0 | 1 | 0x800ee464 |
| 0x42 | clut_cord | 2 | 3 | 0x800ee464, 0x800ee790 |
| 0x47 | anime.sprite | 0 | 1 | 0x800ee464 |
| 0x50 | attack_box | 0 | 1 | 0x800ee464 |
| 0x54 | hurt_box | 0 | 1 | 0x800ee464 |
| 0x58 | damage_table | 0 | 1 | 0x800ee464 |
| 0x5c | hp | 2 | 1 | 0x800ee464, 0x800ee790 |
| 0x60 | contact_damage | 0 | 1 | 0x800ee464 |
| 0x61 | iframes | 0 | 1 | 0x800ee464 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800ee464 |
| 0x68 | terrain_box | 1 | 1 | 0x800ee464, 0x800ee790 |
| 0x75 | self+0x75 | 0 | 1 | 0x800ee464 |
| 0x76 | self+0x76 | 0 | 1 | 0x800ee464 |
| 0x80 | work.80 | 0 | 2 | 0x800ee464 |
| 0x82 | self+0x82 | 1 | 0 | 0x800ee790 |
| 0x83 | self+0x83 | 1 | 2 | 0x800ee464, 0x800ee790 |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800ee464 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ee464 |
| 0x800743dc | dmg_tbl_normal | 0x800ee464 |
| 0x80095098 | bank_tpage | 0x800ee464 |
| 0x800970aa | mega.x.px | 0x800ee790 |
| 0x800970b4 | mega.layer | 0x800ee464 |
| 0x800c1220 | stage_objects | 0x800ee64c |
| 0x800cced0 | game | 0x800ee790 |
| 0x800f8d08 |  | 0x800ee464 |
| 0x800f8d28 |  | 0x800ee790 |
| 0x800f8d2c |  | 0x800ee790 |
| 0x800f8d30 |  | 0x800ee464 |
| 0x800f8d38 |  | 0x800ee464 |
| 0x800f8d3c |  | 0x800ee464 |
