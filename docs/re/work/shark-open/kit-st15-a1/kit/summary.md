# RE kit: shark-open/kit-st15-a1

- Object: shark-open/kit-st15-a1
- State: `shark-2_section-2.state.gz` (SHA-1 `2ed5581179aa6acd6cf436aac85487b11ea49bd0`), stage 4 area 1
- Class: stage, type 15; entry function 0x800f00b4
- Functions walked: 9; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800efb14 |
| 0x8002943c | Quake2 | screen shake variant: camera work +0x36/+0x45/+0x3E/+0x3C/+0x3A, flag 0x10 | 0x800efb14 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800eec50 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800eec50 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800f0114 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800eeed8 |
| 0x8002cb50 | DisplayCheck1 | display = 1 if inside [-32, W+32) x [-32, 272) | 0x800eeed8 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800eeed8 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ef100, 0x800efb14 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800eeed8 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800f0114 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x8003208c | SolidPushShots | SolidPush of obj against the visual objects and the weapon objects that have +0x9D set; keeps a touch bit per object in obj+0x90 (guess) | 0x800eeed8 |
| 0x80032b34 | SolidWeaponHit | weapon objects whose terrain box overlaps obj's terrain box: 0x800312DC, then ApplyWeaponHit; returns the first non-zero result (likely) | 0x800eeed8 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800eeed8, 0x800efd00, 0x800f0114 |
| 0x80049ebc | SpawnExplosionRand | SpawnExplosionRand(obj, type, mx, my): explosion at a random offset | 0x800eeed8, 0x800efd00, 0x800f0114 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x80031f4c | 0x800eeed8 |
| 0x80032be8 | 0x800eeed8 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f8d94 | 0x800f00b4 | 0x800eec50, 0x800eeed8, 0x800f0114 |
| 0x800f8da0 | 0x800eeed8 | 0x800ef100, 0x800efb14, 0x800efd00, 0x800f01b4 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table found.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800eec50 |
| 0x2 | var | 8 | 0 | 0x800eec50, 0x800eeed8, 0x800ef100, 0x800efd00, 0x800f0114 |
| 0x3 | display | 1 | 1 | 0x800efb14, 0x800f0114 |
| 0x4 | state | 1 | 5 | 0x800eec50, 0x800eeed8, 0x800efd00, 0x800f00b4 |
| 0x5 | step | 0 | 1 | 0x800eec50 |
| 0x6 | step2 | 1 | 8 | 0x800eec50, 0x800eeed8, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x8 | x | 1 | 2 | 0x800ef100, 0x800efb14, 0x800f00b4 |
| 0xa | x.px | 42 | 7 | 0x800eec50, 0x800eeed8, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0xc | y | 1 | 1 | 0x800efb14, 0x800f00b4 |
| 0xe | y.px | 29 | 1 | 0x800eec50, 0x800eeed8, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x14 | layer | 0 | 1 | 0x800eec50 |
| 0x15 | flip | 0 | 1 | 0x800eec50 |
| 0x16 | priority | 0 | 1 | 0x800eec50 |
| 0x18 | prev_x | 0 | 1 | 0x800f00b4 |
| 0x1c | prev_y | 0 | 1 | 0x800f00b4 |
| 0x20 | speed_x | 6 | 9 | 0x800eec50, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x24 | speed_y | 2 | 5 | 0x800eec50, 0x800eeed8, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x28 | accel_x | 0 | 8 | 0x800eec50, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x2c | gravity | 0 | 6 | 0x800eec50, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x30 | anime_table | 0 | 1 | 0x800eec50 |
| 0x3c | sprite_data | 0 | 1 | 0x800eec50 |
| 0x40 | tex_cord | 0 | 1 | 0x800eec50 |
| 0x42 | clut_cord | 1 | 2 | 0x800eec50, 0x800eeed8 |
| 0x47 | anime.sprite | 0 | 4 | 0x800eec50 |
| 0x50 | attack_box | 0 | 3 | 0x800eec50, 0x800eeed8, 0x800efd00 |
| 0x54 | hurt_box | 0 | 1 | 0x800eec50 |
| 0x58 | damage_table | 0 | 3 | 0x800eec50 |
| 0x5c | hp | 0 | 1 | 0x800eec50 |
| 0x60 | contact_damage | 0 | 1 | 0x800eec50 |
| 0x61 | iframes | 0 | 1 | 0x800eec50 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800eec50 |
| 0x67 | latch | 0 | 2 | 0x800eeed8 |
| 0x68 | terrain_box | 1 | 1 | 0x800eec50, 0x800eeed8 |
| 0x70 | collide_flags | 11 | 0 | 0x800eeed8, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x75 | self+0x75 | 0 | 1 | 0x800eec50 |
| 0x7c | work.7C | 2 | 1 | 0x800ef100, 0x800efd00 |
| 0x80 | work.80 | 24 | 1 | 0x800eec50, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x81 | self+0x81 | 12 | 0 | 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x82 | self+0x82 | 25 | 0 | 0x800eeed8, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x83 | self+0x83 | 12 | 1 | 0x800eec50, 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x84 | work.84 | 3 | 2 | 0x800efd00 |
| 0x85 | self+0x85 | 5 | 7 | 0x800eec50, 0x800eeed8, 0x800ef100, 0x800efb14, 0x800efd00, 0x800f0114 |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800eec50 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800eec50 |
| 0x80075b9c | dmg_tbl_capsule | 0x800eec50 |
| 0x80075d7c |  | 0x800eec50 |
| 0x80095098 | bank_tpage | 0x800eec50 |
| 0x800970a0 | mega | 0x800ef100, 0x800efb14, 0x800efd00 |
| 0x800970b4 | mega.layer | 0x800eec50 |
| 0x80097180 | mega.special | 0x800f00b4 |
| 0x800c1220 | stage_objects | 0x800eeb1c, 0x800eec50 |
| 0x800cced0 | game | 0x800eeed8 |
| 0x800cd30a | nightmare_effect | 0x800eec50 |
| 0x800f8d68 |  | 0x800eec50 |
| 0x800f8d80 |  | 0x800eeed8, 0x800efd00 |
| 0x800f8d84 |  | 0x800eeed8, 0x800efd00, 0x800f0114 |
| 0x800f8d88 |  | 0x800eec50 |
| 0x800f8d8c |  | 0x800eec50 |
| 0x800f8d90 |  | 0x800eeed8, 0x800efd00 |
