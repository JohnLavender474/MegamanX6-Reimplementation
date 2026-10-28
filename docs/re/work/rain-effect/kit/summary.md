# RE kit: rain-effect

- Object: rain-effect
- State: `yammark-1_start.state.gz` (SHA-1 `697358bffd35fe39ff9233a880cf24d12b886b6c`), stage 1 area 0
- Class: main, type 37; entry function 0x800ef578
- Functions walked: 8; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ef304 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ef6d4 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ef304 |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x800ef76c |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ef304, 0x800ef600 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ef6d4 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ef6d4 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ef6d4 |
| 0x8002d2d4 | GetRNG | random number | 0x800ef304 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f78ec | 0x800ef578 | 0x800ef304, 0x800ef5c4, 0x800ef600 |
| 0x800f78f8 | 0x800ef5c4 | 0x800ef620, 0x800ef6d4 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f78e8 (1 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 | 3 3 3 3 3 | 0 0 0 0 0 | 0 |

Note: cut to the export's 1 animations; 5 more pointer-like words follow (often the next table)

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x1 | type | 4 | 0 | 0x800ef304 |
| 0x2 | var | 1 | 0 | 0x800ef304 |
| 0x3 | display | 0 | 1 | 0x800ef304 |
| 0x4 | state | 1 | 2 | 0x800ef304, 0x800ef578, 0x800ef6d4 |
| 0x5 | step | 1 | 2 | 0x800ef304, 0x800ef5c4 |
| 0x6 | step2 | 0 | 1 | 0x800ef304 |
| 0x8 | x | 2 | 0 | 0x800ef304, 0x800ef578 |
| 0xa | x.px | 2 | 1 | 0x800ef304, 0x800ef6d4, 0x800ef76c |
| 0xc | y | 2 | 0 | 0x800ef304, 0x800ef578 |
| 0xe | y.px | 2 | 1 | 0x800ef304, 0x800ef6d4, 0x800ef76c |
| 0x14 | layer | 1 | 1 | 0x800ef304, 0x800ef76c |
| 0x15 | flip | 0 | 1 | 0x800ef304 |
| 0x16 | priority | 0 | 1 | 0x800ef304 |
| 0x18 | prev_x | 0 | 2 | 0x800ef304, 0x800ef578 |
| 0x1c | prev_y | 0 | 2 | 0x800ef304, 0x800ef578 |
| 0x20 | speed_x | 0 | 2 | 0x800ef304 |
| 0x24 | speed_y | 0 | 2 | 0x800ef304 |
| 0x28 | accel_x | 0 | 1 | 0x800ef304 |
| 0x2c | gravity | 0 | 1 | 0x800ef304 |
| 0x30 | anime_table | 1 | 1 | 0x800ef304, 0x800ef76c |
| 0x3c | sprite_data | 1 | 1 | 0x800ef304, 0x800ef76c |
| 0x40 | tex_cord | 1 | 1 | 0x800ef304, 0x800ef76c |
| 0x42 | clut_cord | 1 | 1 | 0x800ef304, 0x800ef76c |
| 0x50 | attack_box | 0 | 1 | 0x800ef304 |
| 0x54 | hurt_box | 0 | 1 | 0x800ef304 |
| 0x5c | hp | 0 | 1 | 0x800ef304 |
| 0x60 | contact_damage | 0 | 1 | 0x800ef304 |
| 0x61 | iframes | 0 | 1 | 0x800ef304 |
| 0x67 | latch | 0 | 1 | 0x800ef304 |
| 0x68 | terrain_box | 0 | 1 | 0x800ef304 |
| 0x7c | work.7C | 1 | 4 | 0x800ef304, 0x800ef620 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ef304 |
| 0x80095098 |  | 0x800ef304 |
| 0x800970a0 | mega | 0x800ef4d4, 0x800ef620 |
| 0x800970b4 | mega.layer | 0x800ef304 |
| 0x800971f8 | camera_layer0 | 0x800ef304, 0x800ef6d4 |
| 0x800ccedc | game.stage | 0x800ef4d4 |
| 0x800cd30a |  | 0x800ef304 |
| 0x800f78e8 |  | 0x800ef304 |
| 0x800f7900 |  | 0x800ef4d4 |
