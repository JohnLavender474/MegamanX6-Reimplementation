# RE kit: s5-stage-12

- Object: s5-stage-12
- State: `scaravich-1_start.state.gz` (SHA-1 `4b9cab59f2dd853bd5034ef66217cebb0de9b9bb`), stage 5 area 0
- Class: stage, type 12; entry function 0x800ee1c4
- Functions walked: 8; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80012d9c | Rumble | controller vibration (likely) | 0x800ee2f4 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800ee2f4 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800edea4 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ee02c |
| 0x8002943c | Quake2 | screen shake variant: camera work +0x36/+0x45/+0x3E/+0x3C/+0x3A, flag 0x10 | 0x800ee2f4 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800edea4 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800edea4 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ee02c, 0x800ee260 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ee240 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ee02c, 0x800ee1c4 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ee02c |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ee2f4, 0x800ee3b8 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ee02c |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800ee02c |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800ee02c |
| 0x8003208c | SolidPushShots | SolidPush of obj against the visual objects and the weapon objects that have +0x9D set; keeps a touch bit per object in obj+0x90 (guess) | 0x800ee02c |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ee02c |
| 0x80049ebc | SpawnExplosionRand | SpawnExplosionRand(obj, type, mx, my): explosion at a random offset | 0x800ee260 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x80031f4c | 0x800ee02c |
| 0x80032be8 | 0x800ee02c |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f5b14 | 0x800ee1c4 | 0x800edea4, 0x800ee02c, 0x800ee260, 0x800ee240 |
| 0x800f5b24 | 0x800ee02c | 0x800ee2f4, 0x800ee380, 0x800ee3b8 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800edea4 |
| 0x2 | var | 1 | 0 | 0x800ee02c |
| 0x3 | display | 2 | 3 | 0x800ee02c, 0x800ee260, 0x800ee2f4 |
| 0x4 | state | 2 | 2 | 0x800edea4, 0x800ee02c, 0x800ee1c4 |
| 0x5 | step | 3 | 4 | 0x800edea4, 0x800ee02c, 0x800ee2f4, 0x800ee380, 0x800ee3b8 |
| 0x6 | step2 | 0 | 1 | 0x800edea4 |
| 0x8 | x | 1 | 0 | 0x800ee1c4 |
| 0xc | y | 1 | 0 | 0x800ee1c4 |
| 0xe | y.px | 2 | 1 | 0x800edea4, 0x800ee3b8 |
| 0x14 | layer | 0 | 1 | 0x800edea4 |
| 0x15 | flip | 0 | 1 | 0x800edea4 |
| 0x16 | priority | 0 | 1 | 0x800edea4 |
| 0x18 | prev_x | 0 | 1 | 0x800ee1c4 |
| 0x1c | prev_y | 0 | 1 | 0x800ee1c4 |
| 0x24 | speed_y | 1 | 3 | 0x800edea4, 0x800ee02c, 0x800ee380, 0x800ee3b8 |
| 0x30 | anime_table | 0 | 1 | 0x800edea4 |
| 0x3c | sprite_data | 0 | 1 | 0x800edea4 |
| 0x40 | tex_cord | 0 | 1 | 0x800edea4 |
| 0x42 | clut_cord | 3 | 4 | 0x800edea4, 0x800ee02c, 0x800ee260 |
| 0x45 | anime.flag | 2 | 0 | 0x800ee02c, 0x800ee2f4 |
| 0x50 | attack_box | 0 | 1 | 0x800edea4 |
| 0x54 | hurt_box | 0 | 1 | 0x800edea4 |
| 0x58 | damage_table | 0 | 1 | 0x800edea4 |
| 0x5c | hp | 2 | 1 | 0x800edea4, 0x800ee02c |
| 0x60 | contact_damage | 0 | 1 | 0x800edea4 |
| 0x61 | iframes | 0 | 1 | 0x800edea4 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800edea4 |
| 0x67 | latch | 0 | 2 | 0x800ee02c |
| 0x68 | terrain_box | 0 | 3 | 0x800edea4, 0x800ee02c |
| 0x70 | collide_flags | 2 | 0 | 0x800ee02c, 0x800ee2f4 |
| 0x75 | self+0x75 | 0 | 1 | 0x800edea4 |
| 0x7c | work.7C | 2 | 4 | 0x800ee02c, 0x800ee260, 0x800ee2f4, 0x800ee380 |
| 0x7e | work.7E | 2 | 1 | 0x800edea4, 0x800ee3b8 |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800edea4 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800edea4 |
| 0x80075e1c |  | 0x800edea4 |
| 0x80095098 | bank_tpage | 0x800edea4 |
| 0x800970b4 | mega.layer | 0x800edea4 |
| 0x80097180 | mega.special | 0x800ee1c4 |
| 0x80097185 | mega.squeeze_timer | 0x800ee02c |
| 0x800cceed | game.checkpoint | 0x800ee02c |
| 0x800cd30a | nightmare_effect | 0x800edea4 |
| 0x800f5b08 |  | 0x800edea4 |
| 0x800f5b0c |  | 0x800ee02c |
| 0x800f5b10 |  | 0x800edea4 |
