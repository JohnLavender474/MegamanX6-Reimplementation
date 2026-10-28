# RE kit: health-capsule

- Object: health-capsule
- State: `turtloid-1.state.gz` (SHA-1 `da8082595c9fc59c1fff107999f384a674b9a63d`), stage 6 area 0
- Class: stage, type 13; entry function 0x800ee43c
- Functions walked: 19; state tables: 4

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800ee150, 0x800ee27c |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ee03c, 0x800ee150, 0x800ee628, 0x800ee6cc, 0x800ee7c8, 0x800ee880, 0x800ee8d4 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ee554, 0x800ee880, 0x800ee8d4, 0x800ee934 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ee03c |
| 0x8002c398 | GetEffectObject | first free of 96 effect slots of 0x70 at 0x80091298, cleared | 0x800ee150 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ee534, 0x800ee9c4 |
| 0x8002cb50 | DisplayCheck1 | display = 1 if inside [-32, W+32) x [-32, 272) | 0x800ee554, 0x800ee748, 0x800ee978 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800ee4c4 |
| 0x8003d308 | MusicPlay | MusicPlay(track, volume?) (guess) | 0x800ee5a8 |
| 0x8003d330 | BossMusic | starts the boss music (guess) | 0x800ee628, 0x800ee6cc |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ee748 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x80031f4c | 0x800ee4c4 |
| 0x8003208c | 0x800ee4c4 |
| 0x80032be8 | 0x800ee4c4 |
| 0x8004e78c | 0x800ee150, 0x800ee628, 0x800ee6cc |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f62e4 | 0x800ee43c | 0x800ee03c, 0x800ee4c4, 0x800ee534 |
| 0x800f62f0 | 0x800ee4c4 | 0x800ee554, 0x800ee748 |
| 0x800f62f8 | 0x800ee554 | 0x800ee5a8, 0x800ee150, 0x800ee628, 0x800ee698, 0x800ee27c, 0x800ee6cc |
| 0x800f6310 | 0x800ee978 | 0x800ee880, 0x800ee8d4, 0x800ee934, 0x800ee7c8, 0x800ee978, 0x800ee9c4 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 3 | 4 | 0x800ee03c, 0x800ee150, 0x800ee628, 0x800ee6cc |
| 0x2 | var | 1 | 0 | 0x800ee748 |
| 0x3 | display | 0 | 1 | 0x800ee748 |
| 0x4 | state | 3 | 2 | 0x800ee03c, 0x800ee43c, 0x800ee748 |
| 0x5 | step | 4 | 3 | 0x800ee4c4, 0x800ee628, 0x800ee6cc, 0x800ee748 |
| 0x6 | step2 | 6 | 8 | 0x800ee150, 0x800ee27c, 0x800ee554, 0x800ee5a8, 0x800ee628, 0x800ee698, 0x800ee6cc, 0x800ee748 |
| 0x8 | x | 1 | 0 | 0x800ee43c |
| 0xa | x.px | 2 | 0 | 0x800ee330, 0x800ee5a8 |
| 0xc | y | 1 | 0 | 0x800ee43c |
| 0xe | y.px | 2 | 0 | 0x800ee330, 0x800ee5a8 |
| 0x15 | flip | 0 | 1 | 0x800ee03c |
| 0x16 | priority | 0 | 1 | 0x800ee03c |
| 0x18 | prev_x | 0 | 1 | 0x800ee43c |
| 0x1c | prev_y | 0 | 1 | 0x800ee43c |
| 0x30 | anime_table | 0 | 1 | 0x800ee03c |
| 0x3c | sprite_data | 0 | 1 | 0x800ee03c |
| 0x40 | tex_cord | 0 | 1 | 0x800ee03c |
| 0x42 | clut_cord | 1 | 2 | 0x800ee03c, 0x800ee4c4 |
| 0x46 | anime.next | 1 | 0 | 0x800ee628 |
| 0x54 | hurt_box | 0 | 1 | 0x800ee03c |
| 0x68 | terrain_box | 2 | 1 | 0x800ee03c, 0x800ee330 |
| 0x75 | self+0x75 | 0 | 1 | 0x800ee03c |
| 0x7c | work.7C | 3 | 7 | 0x800ee150, 0x800ee27c, 0x800ee698, 0x800ee6cc |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ee03c |
| 0x80095098 |  | 0x800ee03c |
| 0x800970a0 | mega | 0x800ee150, 0x800ee27c, 0x800ee5a8, 0x800ee748 |
| 0x800cced0 | game | 0x800ee150, 0x800ee27c |
| 0x800ccef7 | game.stage_flag2 | 0x800ee5a8, 0x800ee628, 0x800ee6cc |
| 0x800f62b0 |  | 0x800ee03c |
| 0x800f62d8 |  | 0x800ee03c |
| 0x800f62e0 |  | 0x800ee748 |
