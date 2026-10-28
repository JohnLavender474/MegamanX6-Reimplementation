# RE kit: rescue-reploid

- Object: rescue-reploid
- State: `yammark-1_start.state.gz` (SHA-1 `697358bffd35fe39ff9233a880cf24d12b886b6c`), stage 1 area 0
- Class: stage, type 4; entry function 0x8004f834
- Functions walked: 25; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x8004ec30, 0x8004ed4c, 0x8004eecc, 0x8004efd0, 0x8004f144 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x8004e800, 0x8004ea78, 0x8004efd0, 0x8004f144, 0x8004f8c0, 0x8004f9a0, 0x8004fc40, 0x8004fd58 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x8004ea78, 0x8004ec30, 0x8004f9a0, 0x8004fad0, 0x8004fb60 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x8004e800 |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x8004ff7c |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x8004fff8 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x8004f8a0, 0x8004fad0 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x8004fb40 |
| 0x8002cab4 | OffScreenCheck | 1 if outside [-64, 384) x [-64, 304) of the layer camera | 0x8004fb60 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x8004ea78, 0x8004fad0 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x8004ea78, 0x8004fad0, 0x8004fb60 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x8004fad0 |
| 0x8002d2d4 | GetRNG | random number | 0x8004eecc |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x8004e800 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x8004fb60 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x8004fb60 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x8004fb60 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x8004fb60 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x8004ea78 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x80078c7c | 0x8004f834 | 0x8004e800, 0x8004ea78, 0x8004f880, 0x8004f8c0, 0x8004f9a0, 0x8004fad0, 0x8004fb60, 0x8004fb40, 0x8004f8a0 |
| 0x80078ca0 | 0x8004fb60 | 0x80040290, 0x8004fe50, 0x8004efd0, 0x8004fc40, 0x8004fd58, 0x8004f144 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 2 | 3 | 0x8004e800, 0x8004ec30, 0x8004ed4c |
| 0x2 | var | 9 | 0 | 0x8004e800, 0x8004ea78, 0x8004eecc, 0x8004fb60, 0x8004fff8 |
| 0x3 | display | 0 | 2 | 0x8004ea78, 0x8004fb60 |
| 0x4 | state | 1 | 11 | 0x8004e800, 0x8004ea78, 0x8004ec30, 0x8004ed4c, 0x8004f834, 0x8004f8c0, 0x8004f9a0, 0x8004fb60 |
| 0x5 | step | 5 | 9 | 0x8004e800, 0x8004ea78, 0x8004ec30, 0x8004ed4c, 0x8004eecc, 0x8004efd0, 0x8004f144, 0x8004fb60, 0x8004fc40, 0x8004fd58 |
| 0x6 | step2 | 4 | 6 | 0x8004ea78, 0x8004ec30, 0x8004ed4c, 0x8004efd0 |
| 0x7 | act4 | 1 | 3 | 0x8004e800, 0x8004ea78 |
| 0x8 | x | 6 | 0 | 0x8004e800, 0x8004f144, 0x8004f834, 0x8004fe8c |
| 0xa | x.px | 4 | 0 | 0x8002dcf4, 0x8004ff7c, 0x8004fff8 |
| 0xc | y | 2 | 0 | 0x8004e800, 0x8004f834 |
| 0xe | y.px | 4 | 1 | 0x8002dcf4, 0x8004e800, 0x8004ea78, 0x8004ff7c, 0x8004fff8 |
| 0x14 | layer | 2 | 1 | 0x8004e800, 0x8004ff7c, 0x8004fff8 |
| 0x15 | flip | 1 | 1 | 0x8002dcf4, 0x8004e800 |
| 0x16 | priority | 0 | 1 | 0x8004e800 |
| 0x18 | prev_x | 0 | 2 | 0x8004e800, 0x8004f834 |
| 0x1c | prev_y | 0 | 2 | 0x8004e800, 0x8004f834 |
| 0x20 | speed_x | 0 | 2 | 0x8004e800, 0x8004f9a0 |
| 0x24 | speed_y | 0 | 2 | 0x8004e800, 0x8004f9a0 |
| 0x28 | accel_x | 0 | 1 | 0x8004e800 |
| 0x2c | gravity | 0 | 1 | 0x8004e800 |
| 0x30 | anime_table | 1 | 1 | 0x8004e800, 0x8004fff8 |
| 0x3c | sprite_data | 1 | 1 | 0x8004e800, 0x8004fff8 |
| 0x40 | tex_cord | 1 | 1 | 0x8004e800, 0x8004fff8 |
| 0x42 | clut_cord | 2 | 2 | 0x8004e800, 0x8004fb60, 0x8004fff8 |
| 0x45 | anime.flag | 2 | 0 | 0x8004f144, 0x8004fad0 |
| 0x50 | attack_box | 0 | 2 | 0x8004fc40 |
| 0x54 | hurt_box | 0 | 2 | 0x8004fc40 |
| 0x58 | damage_table | 0 | 1 | 0x8004e800 |
| 0x5c | hp | 0 | 1 | 0x8004fc40 |
| 0x60 | contact_damage | 0 | 1 | 0x8004fc40 |
| 0x68 | terrain_box | 2 | 3 | 0x8002dcf4, 0x8004e800, 0x8004f8c0 |
| 0x70 | collide_flags | 1 | 0 | 0x8004e800 |
| 0x7c | work.7C | 1 | 1 | 0x8004e800, 0x8004ea78 |
| 0x7e | work.7E | 8 | 2 | 0x8004e800, 0x8004ea78, 0x8004efd0, 0x8004f144, 0x8004f8c0, 0x8004f9a0, 0x8004fc40, 0x8004fd58 |
| 0x7f | self+0x7f | 3 | 5 | 0x8004ec30, 0x8004ed4c, 0x8004efd0 |
| 0x80 | work.80 | 1 | 2 | 0x8004ec30, 0x8004ed4c |
| 0x81 | self+0x81 | 2 | 6 | 0x8004f144, 0x8004f8c0, 0x8004f9a0, 0x8004fc40, 0x8004fd58 |
| 0x82 | self+0x82 | 27 | 7 | 0x8004e800, 0x8004ea78, 0x8004efd0, 0x8004f144, 0x8004f8c0, 0x8004f9a0, 0x8004fc40, 0x8004fd58, 0x8004fe8c, 0x8004fff8 |
| 0x83 | self+0x83 | 1 | 2 | 0x8004efd0, 0x8004fb60 |
| 0x84 | work.84 | 11 | 2 | 0x8004e800, 0x8004ea78, 0x8004efd0, 0x8004f144, 0x8004f8c0, 0x8004f9a0, 0x8004fc40, 0x8004fd58 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x8004e800 |
| 0x800743dc | dmg_tbl_normal | 0x8004e800 |
| 0x80078908 | drop_items+0x18 | 0x8004fc40 |
| 0x8007890c | drop_items+0x1c | 0x8004fc40 |
| 0x80078910 | drop_items+0x20 | 0x8004e800 |
| 0x80078914 | drop_items+0x24 | 0x8004fc40 |
| 0x80078918 | drop_items+0x28 | 0x8004fc40 |
| 0x8007891c | drop_items+0x2c | 0x8004e800 |
| 0x80078be8 |  | 0x8004eecc |
| 0x80078bec |  | 0x8004e800 |
| 0x80095098 |  | 0x8004e800 |
| 0x800970a0 | mega | 0x8004ec30, 0x8004ed4c, 0x8004eecc |
| 0x800970a8 | mega.x | 0x8004f144, 0x8004fe8c |
| 0x800970b4 | mega.layer | 0x8004e800 |
| 0x80097174 | mega.layer+0xc0 | 0x8004f144 |
| 0x80097420 |  | 0x8004ea78 |
| 0x800cced0 | game | 0x8004ec30, 0x8004ed4c, 0x8004eecc, 0x8004fe58, 0x8004ff00, 0x8004ff4c |
| 0x800cd340 |  | 0x8004eecc |
