# RE kit: stage type 8

- Object: stage type 8
- State: `gate-3_stage-2.state.gz` (SHA-1 `992d5d0609e736bba918340805dd9c642d889217`), stage 17 area 0
- Class: stage, type 8; entry function 0x80050dfc
- Functions walked: 14; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80012d9c | Rumble | controller vibration (likely) | 0x80050250 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x80050250, 0x80050540, 0x80050670, 0x80050ea0, 0x80050f58, 0x80051018, 0x80051490 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x8005011c, 0x80050ea0, 0x80051fe8 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x80050670, 0x80050f58, 0x80051490 |
| 0x8002943c | Quake2 | screen shake variant: camera work +0x36/+0x45/+0x3E/+0x3C/+0x3A, flag 0x10 | 0x80050250 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x8005011c |
| 0x8002b2f8 | PaletteFxClear | PaletteFxClear(ch): fade_r/g/b[ch] = 0, fade_mask[ch] = 0 (16 words), bg_refresh |= 1 | 0x80050540 |
| 0x8002b79c | ClearEnemies | delete all main objects except type 47, and all visual objects | 0x80050250 |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x80050250 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x80051490 |
| 0x8002cb50 | DisplayCheck1 | display = 1 if inside [-32, W+32) x [-32, 272) | 0x80050dfc |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x80051490 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x80050250, 0x800510c4 |
| 0x8003ccbc | PlayerSetTerrainBox | set the player's terrain box | 0x80050ea0 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x80078dd4 | 0x80050dfc | 0x8005011c, 0x80050e58, 0x800510c4 |
| 0x80078de0 | 0x80050e58 | 0x80050250, 0x80050540, 0x80050670, 0x80050ea0, 0x80050f58, 0x80051018 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x80011694 | 0x80050728 | 0x80050670 | 51 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| effect | 37 | 0x80051490 |

## Animation tables

No animation table found.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x8005011c |
| 0x2 | var | 9 | 1 | 0x8005011c, 0x80050250, 0x80050540, 0x80050670, 0x80050ea0, 0x800510c4 |
| 0x4 | state | 1 | 2 | 0x8005011c, 0x80050dfc, 0x80050f58 |
| 0x5 | step | 1 | 8 | 0x8005011c, 0x80050250, 0x80050540, 0x80050670, 0x80050e58, 0x80050ea0, 0x80050f58, 0x80051018 |
| 0x8 | x | 1 | 0 | 0x80050dfc |
| 0xa | x.px | 29 | 0 | 0x80050670, 0x80050c88 |
| 0xc | y | 1 | 0 | 0x80050dfc |
| 0xe | y.px | 13 | 0 | 0x80050670, 0x80050c88 |
| 0x14 | layer | 0 | 1 | 0x8005011c |
| 0x15 | flip | 0 | 1 | 0x8005011c |
| 0x16 | priority | 0 | 1 | 0x8005011c |
| 0x18 | prev_x | 0 | 1 | 0x80050dfc |
| 0x1c | prev_y | 0 | 1 | 0x80050dfc |
| 0x30 | anime_table | 0 | 1 | 0x8005011c |
| 0x3c | sprite_data | 0 | 1 | 0x8005011c |
| 0x40 | tex_cord | 0 | 1 | 0x8005011c |
| 0x42 | clut_cord | 0 | 1 | 0x8005011c |
| 0x45 | anime.flag | 4 | 2 | 0x80050670, 0x80050f58 |
| 0x67 | latch | 0 | 1 | 0x8005011c |
| 0x68 | terrain_box | 1 | 1 | 0x8005011c, 0x80050c88 |
| 0x72 | self+0x72 | 2 | 0 | 0x80051104 |
| 0x75 | self+0x75 | 0 | 1 | 0x8005011c |
| 0x7c | work.7C | 1 | 1 | 0x8005011c, 0x80050250 |
| 0x80 | work.80 | 1 | 1 | 0x80050540, 0x80050ea0 |
| 0x84 | work.84 | 2 | 8 | 0x80050250, 0x80050670, 0x80050ea0 |
| 0x88 | work.88 | 2 | 4 | 0x8005011c, 0x80050250 |
| 0xa0 | self+0xa0 | 1 | 3 | 0x80050540, 0x80051018 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x8005011c |
| 0x80078dc8 |  | 0x8005011c |
| 0x80078dcc |  | 0x8005011c |
| 0x8008eab0 | stage_texture_swap | 0x80050ea0 |
| 0x80090c90 | fade_mask | 0x80050540 |
| 0x80090d56 | fade_sub_b | 0x80050540 |
| 0x80090d5c | fade_b | 0x80051018 |
| 0x80095098 | bank_tpage | 0x8005011c |
| 0x800970a0 | mega | 0x80050c88, 0x80050e58, 0x80050f58, 0x80051104 |
| 0x800970b4 | mega.layer | 0x8005011c |
| 0x80097182 | mega.capsule | 0x80051490 |
| 0x800971f8 | camera_layer0 | 0x80050250, 0x80050540, 0x80050670, 0x80050ea0, 0x80050f58 |
| 0x80097420 | frame_count | 0x80050250 |
| 0x8009742c | fade_sub_g | 0x80050540 |
| 0x80097430 | fade_g | 0x80051018 |
| 0x800c4560 | bg_refresh | 0x80051018 |
| 0x800cced0 | game | 0x80050250, 0x80050670, 0x80050e58 |
| 0x800cceec | game.player_lock | 0x80050f58 |
| 0x800ccef6 | game.stage_flag | 0x80050250 |
| 0x800ccef8 | game.stage_flag3 | 0x80050250 |
| 0x800cf850 | fade_mode | 0x80050540 |
| 0x800cf854 | fade_r | 0x80051018 |
