# RE kit: shark-mini-boss

- Object: shark-mini-boss
- State: `shark-4_mini-boss.state.gz` (SHA-1 `edbd4700710484154fd2fb9a071f9361b0a7f3b7`), stage 4 area 1
- Class: stage, type 10; entry function 0x800ed494
- Functions walked: 12; state tables: 4

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800eb114 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ed264 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800eb2e8 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800eb2e8 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ed264 |
| 0x80049ebc | SpawnExplosionRand | SpawnExplosionRand(obj, type, mx, my): explosion at a random offset | 0x800ed264 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x8003208c | 0x800eb2e8 |
| 0x80032b34 | 0x800eb2e8 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f88c0 | 0x800ed494 | 0x800eb114, 0x800eb2e8, 0x800ed264 |
| 0x800f88cc | 0x800eb2e8 | 0x800ed364, 0x800ed434, 0x800ed48c |
| 0x800f88d8 | 0x800ed364 | 0x800ed3a0, 0x800ed42c |
| 0x800f88e0 | 0x800ed434 | 0x800ed470, 0x800ed484 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f8638 (17 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 25 | 1 | 0 | hold |
| 1 | 26 | 1 | 0 | hold |
| 2 | 27 28 29 28 27 26 30 30 31 32 | 5 4 3 4 5 6 1 1 2 1 | 0 0 0 0 0 0 2 0 0 1 | hold |
| 3 | 31 30 26 | 6 6 1 | 0 0 1 | hold |
| 4 | 26 27 28 29 28 27 | 3 2 1 1 1 2 | 0 0 0 0 0 0 | 0 |
| 5 | 33 | 1 | 0 | hold |
| 6 | 34 35 36 35 34 33 37 37 38 39 39 | 5 4 3 4 5 6 1 1 2 20 1 | 0 0 0 0 0 0 2 0 0 0 1 | hold |
| 7 | 38 37 33 | 6 6 1 | 0 0 1 | hold |
| 8 | 0 1 2 3 | 8 8 8 8 | 0 0 0 0 | 0 |
| 9 | 4 5 6 7 8 | 2 1 1 1 4 | 0 0 0 0 0 | 0 |
| 10 | 9 10 11 12 13 | 2 2 2 2 2 | 0 0 0 0 0 | 0 |
| 11 | 14 15 16 17 18 19 20 21 22 23 | 1 1 1 1 1 1 1 1 1 1 | 3 5 7 9 11 13 15 15 15 15 | 7 |
| 12 | 40 | 1 | 0 | hold |
| 13 | 41 | 1 | 0 | hold |
| 14 | 42 | 1 | 0 | hold |
| 15 | 43 | 1 | 0 | hold |
| 16 | 24 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 3 | 2 | 0x800ecd7c |
| 0x2 | var | 7 | 0 | 0x800eb114, 0x800eb2e8, 0x800ed264 |
| 0x3 | display | 0 | 5 | 0x800eb114, 0x800eb2e8, 0x800ed3a0 |
| 0x4 | state | 3 | 2 | 0x800eb114, 0x800eb2e8, 0x800ed494 |
| 0x5 | step | 4 | 7 | 0x800eb114, 0x800eb2e8, 0x800ed264 |
| 0x6 | step2 | 3 | 2 | 0x800eb114, 0x800ed364, 0x800ed434, 0x800ed470 |
| 0x8 | x | 1 | 0 | 0x800eb2e8 |
| 0xa | x.px | 0 | 2 | 0x800eb2e8 |
| 0xc | y | 1 | 0 | 0x800eb2e8 |
| 0xe | y.px | 0 | 3 | 0x800eb2e8 |
| 0x15 | flip | 0 | 2 | 0x800eb114 |
| 0x16 | priority | 0 | 1 | 0x800eb114 |
| 0x18 | prev_x | 0 | 1 | 0x800eb2e8 |
| 0x1c | prev_y | 0 | 1 | 0x800eb2e8 |
| 0x50 | attack_box | 0 | 3 | 0x800eb114 |
| 0x54 | hurt_box | 0 | 3 | 0x800eb114 |
| 0x58 | damage_table | 0 | 3 | 0x800eb114 |
| 0x5c | hp | 0 | 3 | 0x800eb114 |
| 0x60 | contact_damage | 0 | 3 | 0x800eb114 |
| 0x61 | iframes | 0 | 3 | 0x800eb114 |
| 0x65 | last_hit_tag | 0 | 3 | 0x800eb114 |
| 0x67 | latch | 0 | 1 | 0x800eb114 |
| 0x68 | terrain_box | 0 | 3 | 0x800eb114 |
| 0x72 | self+0x72 | 1 | 0 | 0x800eb2e8 |
| 0x75 | self+0x75 | 0 | 1 | 0x800eb114 |
| 0x76 | self+0x76 | 0 | 1 | 0x800eb114 |
| 0x7c | work.7C | 2 | 0 | 0x800eb2e8, 0x800ed3a0 |
| 0x82 | self+0x82 | 1 | 2 | 0x800eb114, 0x800eb2e8 |
| 0x86 | self+0x86 | 1 | 2 | 0x800eb114, 0x800eb2e8 |
| 0x88 | work.88 | 1 | 1 | 0x800ed264 |
| 0x8a | work.8A | 1 | 2 | 0x800ed264 |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800eb114 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x80075b9c | dmg_tbl_capsule | 0x800eb114 |
| 0x800970a0 | mega | 0x800eb2e8 |
| 0x800971f8 | camera_layer0 | 0x800eb2e8 |
| 0x800f8688 |  | 0x800ed264 |
| 0x800f88a4 |  | 0x800eb114 |
| 0x800f88a8 |  | 0x800eb114 |
| 0x800f88ac |  | 0x800eb114 |
| 0x800f88b4 |  | 0x800eb114 |
| 0x800f88b8 |  | 0x800eb114 |
| 0x800f88bc |  | 0x800eb114 |
