# RE kit: stage type 13

- Object: stage type 13
- State: `gate-4_before-gate.state.gz` (SHA-1 `f51fcc1d7dd4d15662dc4a3ecd18beb124cd7419`), stage 18 area 0
- Class: stage, type 13; entry function 0x800ecf5c
- Functions walked: 20; state tables: 5

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ecc70, 0x800ecd9c |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ecb5c, 0x800ecc70, 0x800ed148, 0x800ed1ec, 0x800ed2e8, 0x800ed3a0, 0x800ed3f4 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ed074, 0x800ed3a0, 0x800ed3f4, 0x800ed454 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ecb5c |
| 0x8002c398 | GetEffectObject | first free of 96 effect slots of 0x70 at 0x80091298, cleared | 0x800ecc70 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ed054, 0x800ed4e4 |
| 0x8002cb50 | DisplayCheck1 | display = 1 if inside [-32, W+32) x [-32, 272) | 0x800ed074, 0x800ed268, 0x800ed498 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800ecfe4 |
| 0x80031f4c | SolidMainObjects | SolidMainObjects(obj): obj solid for the 48 main objects that have a terrain box: push out (0x800323D4), carry the ones on top (0x80032D24, touch bit per object in obj+0x98), put back a main object squeezed into the terrain (floor+ceiling or both walls) | 0x800ecfe4 |
| 0x8003208c | SolidPushShots | SolidPush of obj against the visual objects and the weapon objects that have +0x9D set; keeps a touch bit per object in obj+0x90 (guess) | 0x800ecfe4 |
| 0x80032be8 | SolidVisualHit | SolidVisualHit(obj): each live visual object whose box touches obj (WeaponBoxTest with obj+0x80/+0x82) gets collide_flags |= 0xF; returns the type of the last one | 0x800ecfe4 |
| 0x8003d308 | PlayerEvent | PlayerEvent(step, facing): mega+0xD0 = 1, +0xD1 = step, +0xD2 = facing, +0x7A = 1, game.player_lock = 1 | 0x800ed0c8 |
| 0x8003d330 | PlayerEventEnd | mega+0xD0 = 0, +0x7A = 0, game.player_lock = 0: the event step ends | 0x800ed148, 0x800ed1ec |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ed268 |
| 0x8004e78c | SetStopFlags | SetStopFlags(n): game+0x10..+0x17 and +0x19 = n | 0x800ecc70, 0x800ed148, 0x800ed1ec |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6b58 | 0x800ecf5c | 0x800ecb5c, 0x800ecfe4, 0x800ed054 |
| 0x800f6b64 | 0x800ecfe4 | 0x800ed074, 0x800ed268 |
| 0x800f6b6c | 0x800ed074 | 0x800ed0c8, 0x800ecc70, 0x800ed148, 0x800ed1b8, 0x800ecd9c, 0x800ed1ec |
| 0x800f6b84 | 0x800ed498 | 0x800ed3a0, 0x800ed3f4, 0x800ed454 |
| 0x800f6b90 | 0x800ecfa8 | 0x800ed2e8, 0x800ed498, 0x800ed4e4 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| effect | 14 | 0x800ecfa8 |

## Animation tables

No animation table found.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 3 | 4 | 0x800ecb5c, 0x800ecc70, 0x800ed148, 0x800ed1ec |
| 0x2 | var | 1 | 0 | 0x800ed268 |
| 0x3 | display | 0 | 1 | 0x800ed268 |
| 0x4 | state | 3 | 2 | 0x800ecb5c, 0x800ecf5c, 0x800ed268 |
| 0x5 | step | 4 | 3 | 0x800ecfe4, 0x800ed148, 0x800ed1ec, 0x800ed268 |
| 0x6 | step2 | 6 | 8 | 0x800ecc70, 0x800ecd9c, 0x800ed074, 0x800ed0c8, 0x800ed148, 0x800ed1b8, 0x800ed1ec, 0x800ed268 |
| 0x8 | x | 1 | 0 | 0x800ecf5c |
| 0xa | x.px | 2 | 0 | 0x800ece50, 0x800ed0c8 |
| 0xc | y | 1 | 0 | 0x800ecf5c |
| 0xe | y.px | 2 | 0 | 0x800ece50, 0x800ed0c8 |
| 0x15 | flip | 0 | 1 | 0x800ecb5c |
| 0x16 | priority | 0 | 1 | 0x800ecb5c |
| 0x18 | prev_x | 0 | 1 | 0x800ecf5c |
| 0x1c | prev_y | 0 | 1 | 0x800ecf5c |
| 0x30 | anime_table | 0 | 1 | 0x800ecb5c |
| 0x3c | sprite_data | 0 | 1 | 0x800ecb5c |
| 0x40 | tex_cord | 0 | 1 | 0x800ecb5c |
| 0x42 | clut_cord | 1 | 2 | 0x800ecb5c, 0x800ecfe4 |
| 0x46 | anime.next | 1 | 0 | 0x800ed148 |
| 0x54 | hurt_box | 0 | 1 | 0x800ecb5c |
| 0x68 | terrain_box | 2 | 1 | 0x800ecb5c, 0x800ece50 |
| 0x75 | self+0x75 | 0 | 1 | 0x800ecb5c |
| 0x7c | work.7C | 3 | 7 | 0x800ecc70, 0x800ecd9c, 0x800ed1b8, 0x800ed1ec |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ecb5c |
| 0x80095098 | bank_tpage | 0x800ecb5c |
| 0x800970a0 | mega | 0x800ecc70, 0x800ecd9c, 0x800ed0c8, 0x800ed268 |
| 0x800cced0 | game | 0x800ecc70, 0x800ecd9c |
| 0x800ccef7 | game.stage_flag2 | 0x800ed0c8, 0x800ed148, 0x800ed1ec |
| 0x800f6b24 |  | 0x800ecb5c |
| 0x800f6b4c |  | 0x800ecb5c |
| 0x800f6b54 |  | 0x800ed268 |
