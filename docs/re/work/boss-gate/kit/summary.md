# RE kit: boss-gate

- Object: boss-gate
- State: `intro-3_boss.state.gz` (SHA-1 `a35a84fc9767b61d51fecbe6cd24bf9720fbe5ee`), stage 0 area 0
- Class: stage, type 8; entry function 0x80050dfc
- Functions walked: 23; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80012d9c | Rumble | controller vibration (likely) | 0x80050250 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x80050250, 0x80050540, 0x80050670, 0x80050ea0, 0x80050f58, 0x80051018 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x8005011c, 0x80050ea0 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x80050670, 0x80050f58 |
| 0x8002943c | Quake2 | screen shake variant: camera work +0x36/+0x45/+0x3E/+0x3C/+0x3A, flag 0x10 | 0x80050250 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x8005011c |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x80050250 |
| 0x8002cb50 | DisplayCheck1 | display = 1 if inside [-32, W+32) x [-32, 272) | 0x80050dfc |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x80031da8 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x80078dd4 | 0x80050dfc | 0x8005011c, 0x80050e58, 0x800510c4 |
| 0x80078de0 | 0x80050e58 | 0x80050250, 0x80050540, 0x80050670, 0x80050ea0, 0x80050f58, 0x80051018 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x80078dcc (2 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 58 59 60 61 60 62 59 58 63 64 65 66 67 58 63 64 65 66 67 58 59 60 61 60 62 59 58 68 69 70 | 1 2 2 3 2 1 3 3 3 3 3 3 3 3 3 3 3 3 3 3 1 2 2 3 2 1 3 4 4 4 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 2 | hold |
| 1 | 70 69 68 58 59 60 61 60 62 59 58 67 66 65 64 63 58 67 66 65 64 63 58 59 60 61 60 62 59 58 | 4 4 4 3 1 2 2 3 2 1 3 3 3 3 3 3 3 3 3 3 3 3 3 1 2 2 3 2 1 2 | 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 | hold |

Note: cut to the export's 2 animations; 9 more pointer-like words follow (often the next table)

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x8005011c |
| 0x2 | var | 10 | 1 | 0x8003ccbc, 0x8005011c, 0x80050250, 0x80050540, 0x80050670, 0x80050ea0, 0x800510c4 |
| 0x4 | state | 1 | 2 | 0x8005011c, 0x80050dfc, 0x80050f58 |
| 0x5 | step | 1 | 8 | 0x8005011c, 0x80050250, 0x80050540, 0x80050670, 0x80050e58, 0x80050ea0, 0x80050f58, 0x80051018 |
| 0x8 | x | 1 | 0 | 0x80050dfc |
| 0xa | x.px | 39 | 0 | 0x8002dcf4, 0x8002de54, 0x8002dfa0, 0x8002e1a0, 0x800324c8, 0x80050670, 0x80050c88 |
| 0xc | y | 1 | 0 | 0x80050dfc |
| 0xe | y.px | 18 | 0 | 0x8002dcf4, 0x8002de54, 0x8002e0dc, 0x8002e1a0, 0x800324c8, 0x80050670, 0x80050c88 |
| 0x14 | layer | 0 | 1 | 0x8005011c |
| 0x15 | flip | 4 | 1 | 0x8002dcf4, 0x8002de54, 0x8002dfa0, 0x8002e1a0, 0x8005011c |
| 0x16 | priority | 0 | 1 | 0x8005011c |
| 0x18 | prev_x | 0 | 1 | 0x80050dfc |
| 0x1a | self+0x1a | 2 | 0 | 0x800324c8 |
| 0x1c | prev_y | 0 | 1 | 0x80050dfc |
| 0x1e | self+0x1e | 1 | 0 | 0x800324c8 |
| 0x30 | anime_table | 0 | 1 | 0x8005011c |
| 0x3c | sprite_data | 0 | 1 | 0x8005011c |
| 0x40 | tex_cord | 0 | 1 | 0x8005011c |
| 0x42 | clut_cord | 0 | 1 | 0x8005011c |
| 0x45 | anime.flag | 4 | 2 | 0x80050670, 0x80050f58 |
| 0x60 | contact_damage | 2 | 0 | 0x8002e1a0, 0x80032208 |
| 0x66 | self+0x66 | 9 | 3 | 0x8002dfa0, 0x8002e0dc, 0x80032208 |
| 0x67 | latch | 0 | 1 | 0x8005011c |
| 0x68 | terrain_box | 12 | 4 | 0x8002dcf4, 0x8002de54, 0x8002dfa0, 0x8002e0dc, 0x8002e1a0, 0x80031da8, 0x800324c8, 0x8003ccbc, 0x8005011c, 0x80050c88 |
| 0x72 | self+0x72 | 3 | 1 | 0x8002e1a0, 0x80031da8, 0x80051104 |
| 0x73 | self+0x73 | 1 | 1 | 0x8002e1a0, 0x80031da8 |
| 0x74 | self+0x74 | 0 | 1 | 0x8002e1a0 |
| 0x75 | self+0x75 | 1 | 1 | 0x8002e1a0, 0x8005011c |
| 0x76 | self+0x76 | 1 | 2 | 0x8002e1a0, 0x80031da8 |
| 0x77 | self+0x77 | 1 | 2 | 0x8002e1a0, 0x80031da8 |
| 0x78 | self+0x78 | 0 | 2 | 0x8002e1a0 |
| 0x7c | work.7C | 1 | 1 | 0x8005011c, 0x80050250 |
| 0x80 | work.80 | 1 | 1 | 0x80050540, 0x80050ea0 |
| 0x84 | work.84 | 2 | 8 | 0x80050250, 0x80050670, 0x80050ea0 |
| 0x88 | work.88 | 2 | 4 | 0x8005011c, 0x80050250 |
| 0xa0 | self+0xa0 | 1 | 3 | 0x80050540, 0x80051018 |
| 0xd5 | self+0xd5 | 1 | 0 | 0x8003ccbc |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x8005011c |
| 0x80011694 |  | 0x80050670 |
| 0x80073d50 |  | 0x8003ccbc |
| 0x80078dc8 |  | 0x8005011c |
| 0x80078dcc |  | 0x8005011c |
| 0x8008eab0 |  | 0x80050ea0 |
| 0x8008ef48 | main_objects | 0x8002b79c |
| 0x80090c90 | main_objects_end+0x8 | 0x8002b2f8, 0x80050540 |
| 0x80090d56 | main_objects_end+0xce | 0x80050540 |
| 0x80090d5c | main_objects_end+0xd4 | 0x8002b2f8, 0x80051018 |
| 0x80093c98 | visual_objects | 0x8002b79c |
| 0x80095098 |  | 0x8005011c |
| 0x800950a0 | weapon_objects | 0x80031da8 |
| 0x800970a0 | mega | 0x80031da8, 0x80032208, 0x800324c8, 0x80050c88, 0x80050e58, 0x80050f58, 0x80051104 |
| 0x800970b4 | mega.layer | 0x8005011c |
| 0x80097186 | mega.layer+0xd2 | 0x8002e1a0 |
| 0x800971f8 | camera_layer0 | 0x80050250, 0x80050540, 0x80050670, 0x80050ea0, 0x80050f58 |
| 0x80097420 |  | 0x80050250 |
| 0x8009742c |  | 0x80050540 |
| 0x80097430 |  | 0x8002b2f8, 0x80051018 |
| 0x800c4560 |  | 0x80051018 |
| 0x800c4568 |  | 0x80031da8 |
| 0x800cced0 | game | 0x80050250, 0x80050670, 0x80050e58 |
| 0x800cceec | game.stage+0x10 | 0x80050f58 |
| 0x800ccef6 | game.stage+0x1a | 0x80050250 |
| 0x800ccef8 | game.stage+0x1c | 0x80050250 |
| 0x800cd340 |  | 0x80031da8 |
| 0x800cf850 |  | 0x80050540 |
| 0x800cf854 |  | 0x8002b2f8, 0x80051018 |
