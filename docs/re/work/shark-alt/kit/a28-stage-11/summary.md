# RE kit: a28-stage-11

- Object: a28-stage-11
- State: `shark-10_alt.state.gz` (SHA-1 `114fb5c590494f5a3bc9dc0ee53f3e58ec1371f6`), stage 20 area 1
- Class: stage, type 11; entry function 0x800ea698
- Functions walked: 10; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ea760 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ea7b4 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ea06c |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ea398, 0x800ea84c |
| 0x8002c9d4 | DeleteObjectKeep | record flags |= 0x80 (the spawner skips flags & 0x81), then DeleteObject3: single-use objects | 0x800ea704 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ea6e4 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ea398, 0x800ea7b4 |
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800ea398 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ea398 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800ea398 |
| 0x80031f4c | SolidMainObjects | SolidMainObjects(obj): obj solid for the 48 main objects that have a terrain box: push out (0x800323D4), carry the ones on top (0x80032D24, touch bit per object in obj+0x98), put back a main object squeezed into the terrain (floor+ceiling or both walls) | 0x800ea398 |
| 0x8003208c | SolidPushShots | SolidPush of obj against the visual objects and the weapon objects that have +0x9D set; keeps a touch bit per object in obj+0x90 (guess) | 0x800ea398 |
| 0x80032be8 | SolidVisualHit | SolidVisualHit(obj): each live visual object whose box touches obj (WeaponBoxTest with obj+0x80/+0x82) gets collide_flags |= 0xF; returns the type of the last one | 0x800ea398 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ea398 |
| 0x80049ebc | SpawnExplosionRand | SpawnExplosionRand(obj, type, mx, my): explosion at a random offset | 0x800ea398 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ea398 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f2ccc | 0x800ea698 | 0x800ea06c, 0x800ea398, 0x800ea704, 0x800ea6e4 |
| 0x800f2cdc | 0x800ea724 | 0x800ea760, 0x800ea7b4, 0x800ea84c |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| effect | 12 | 0x800ea724 |

## Animation tables

No animation table found.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800ea06c |
| 0x2 | var | 6 | 1 | 0x800ea06c, 0x800ea398 |
| 0x3 | display | 1 | 0 | 0x800ea398 |
| 0x4 | state | 2 | 2 | 0x800ea06c, 0x800ea398, 0x800ea698 |
| 0x5 | step | 0 | 1 | 0x800ea06c |
| 0x6 | step2 | 0 | 1 | 0x800ea06c |
| 0x7 | act4 | 1 | 3 | 0x800ea06c, 0x800ea398 |
| 0x8 | x | 1 | 0 | 0x800ea698 |
| 0xa | x.px | 6 | 1 | 0x800ea398 |
| 0xc | y | 1 | 0 | 0x800ea698 |
| 0xe | y.px | 5 | 0 | 0x800ea398 |
| 0x14 | layer | 0 | 1 | 0x800ea06c |
| 0x15 | flip | 0 | 1 | 0x800ea06c |
| 0x16 | priority | 0 | 2 | 0x800ea06c |
| 0x18 | prev_x | 0 | 1 | 0x800ea698 |
| 0x1c | prev_y | 0 | 1 | 0x800ea698 |
| 0x30 | anime_table | 0 | 1 | 0x800ea06c |
| 0x3c | sprite_data | 0 | 1 | 0x800ea06c |
| 0x40 | tex_cord | 0 | 1 | 0x800ea06c |
| 0x42 | clut_cord | 2 | 3 | 0x800ea06c, 0x800ea398 |
| 0x47 | anime.sprite | 0 | 1 | 0x800ea06c |
| 0x50 | attack_box | 0 | 1 | 0x800ea06c |
| 0x54 | hurt_box | 0 | 1 | 0x800ea06c |
| 0x58 | damage_table | 0 | 1 | 0x800ea06c |
| 0x5c | hp | 2 | 1 | 0x800ea06c, 0x800ea398 |
| 0x60 | contact_damage | 0 | 1 | 0x800ea06c |
| 0x61 | iframes | 0 | 1 | 0x800ea06c |
| 0x65 | last_hit_tag | 0 | 1 | 0x800ea06c |
| 0x68 | terrain_box | 1 | 1 | 0x800ea06c, 0x800ea398 |
| 0x75 | self+0x75 | 0 | 1 | 0x800ea06c |
| 0x76 | self+0x76 | 0 | 1 | 0x800ea06c |
| 0x80 | work.80 | 0 | 2 | 0x800ea06c |
| 0x82 | self+0x82 | 1 | 0 | 0x800ea398 |
| 0x83 | self+0x83 | 1 | 2 | 0x800ea06c, 0x800ea398 |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800ea06c |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ea06c |
| 0x800743dc | dmg_tbl_normal | 0x800ea06c |
| 0x80095098 | bank_tpage | 0x800ea06c |
| 0x800970aa | mega.x.px | 0x800ea398 |
| 0x800970b4 | mega.layer | 0x800ea06c |
| 0x800c1220 | stage_objects | 0x800ea254 |
| 0x800cced0 | game | 0x800ea398 |
| 0x800f2c94 |  | 0x800ea06c |
| 0x800f2cb4 |  | 0x800ea398 |
| 0x800f2cb8 |  | 0x800ea398 |
| 0x800f2cbc |  | 0x800ea06c |
| 0x800f2cc4 |  | 0x800ea06c |
| 0x800f2cc8 |  | 0x800ea06c |
| 0x800f2ce8 |  | 0x800ea760 |
| 0x800f2cf4 |  | 0x800ea760 |
