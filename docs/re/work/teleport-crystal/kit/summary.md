# RE kit: teleport-crystal

- Object: teleport-crystal
- State: `yammark-1_start.state.gz` (SHA-1 `697358bffd35fe39ff9233a880cf24d12b886b6c`), stage 1 area 0
- Class: main, type 28; entry function 0x800eeec4
- Functions walked: 18; state tables: 4

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800ee70c, 0x800ee8c4, 0x800ee9c4 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ee5cc |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800eef10 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ef2e4 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800eef8c |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800eef8c |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800eef10 |
| 0x8002d654 | BoxOverlap | box test between two objects, facing-aware | 0x800eefcc |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800eeec4 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800eef10 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x800139a0 | 0x800ee9c4 |
| 0x800139f4 | 0x800ef144, 0x800ef1e8 |
| 0x80029f38 | 0x800ee9c4 |
| 0x8002b36c | 0x800ee9c4 |
| 0x8002b6b0 | 0x800ee70c, 0x800ee8c4, 0x800ee9c4 |
| 0x8003ad18 | 0x800eedb8 |
| 0x8003c4ec | 0x800ee70c, 0x800ee8c4 |
| 0x8003cdf0 | 0x800ee70c, 0x800ee8c4 |
| 0x8003ce30 | 0x800ee70c, 0x800ee8c4 |
| 0x8003f3dc | 0x800ee70c, 0x800ee8c4 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f77f8 | 0x800eeec4 | 0x800ee5cc, 0x800eef10, 0x800eef8c, 0x800eef8c |
| 0x800f7808 | 0x800eef10 | 0x80040290, 0x800eefcc, 0x800ef0cc, 0x800ef108, 0x800ef044, 0x800ef090 |
| 0x800f7820 | 0x800ef0cc | 0x800ee70c, 0x800ef144, 0x800ee800 |
| 0x800f78bc | 0x800ef108 | 0x800ee8c4, 0x800ef1e8, 0x800ee9c4, 0x800ef27c, 0x800eedb8, 0x800ef2e4 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f77ec (2 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 | 3 3 3 3 3 3 | 0 0 0 0 0 0 | 0 |
| 1 | 6 7 8 9 10 11 | 3 3 3 3 3 3 | 0 0 0 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 3 | 3 | 0x800ee70c, 0x800ee8c4, 0x800eedb8 |
| 0x2 | var | 5 | 0 | 0x800ee5cc, 0x800ee9c4, 0x800eef10, 0x800eefcc, 0x800ef144 |
| 0x3 | display | 0 | 1 | 0x800ee5cc |
| 0x4 | state | 2 | 1 | 0x800ee5cc, 0x800eeec4, 0x800eef8c |
| 0x5 | step | 1 | 5 | 0x800ee5cc, 0x800eef10, 0x800eefcc, 0x800ef044, 0x800ef090 |
| 0x6 | step2 | 9 | 12 | 0x800ee5cc, 0x800ee70c, 0x800ee8c4, 0x800ee9c4, 0x800eedb8, 0x800eefcc, 0x800ef044, 0x800ef090, 0x800ef0cc, 0x800ef108, 0x800ef144, 0x800ef1e8, 0x800ef27c |
| 0x8 | x | 2 | 0 | 0x800ee5cc, 0x800eef10 |
| 0xa | x.px | 2 | 0 | 0x800ef044, 0x800ef090 |
| 0xc | y | 2 | 0 | 0x800ee5cc, 0x800eef10 |
| 0x14 | layer | 0 | 1 | 0x800ee5cc |
| 0x15 | flip | 0 | 2 | 0x800ee5cc |
| 0x16 | priority | 0 | 1 | 0x800ee5cc |
| 0x18 | prev_x | 0 | 2 | 0x800ee5cc, 0x800eef10 |
| 0x1c | prev_y | 0 | 2 | 0x800ee5cc, 0x800eef10 |
| 0x30 | anime_table | 0 | 1 | 0x800ee5cc |
| 0x50 | attack_box | 0 | 1 | 0x800ee5cc |
| 0x54 | hurt_box | 0 | 1 | 0x800ee5cc |
| 0x5c | hp | 0 | 1 | 0x800ee5cc |
| 0x60 | contact_damage | 0 | 1 | 0x800ee5cc |
| 0x61 | iframes | 0 | 1 | 0x800ee5cc |
| 0x67 | latch | 0 | 1 | 0x800ee5cc |
| 0x68 | terrain_box | 0 | 1 | 0x800ee5cc |
| 0x70 | collide_flags | 0 | 1 | 0x800ee5cc |
| 0x7c | work.7C | 8 | 11 | 0x800ee5cc, 0x800ee70c, 0x800ee8c4, 0x800eedb8, 0x800eefcc, 0x800ef044, 0x800ef090, 0x800ef144, 0x800ef1e8, 0x800ef27c |
| 0x7e | work.7E | 16 | 3 | 0x800ee9c4, 0x800eedb8 |
| 0x80 | work.80 | 6 | 9 | 0x800ee70c, 0x800ee800, 0x800ee8c4, 0x800ee9c4, 0x800eedb8, 0x800ef144, 0x800ef1e8, 0x800ef27c |
| 0x82 | self+0x82 | 6 | 9 | 0x800ee70c, 0x800ee800, 0x800ee8c4, 0x800ee9c4, 0x800eedb8, 0x800ef144, 0x800ef1e8, 0x800ef27c |
| 0x84 | work.84 | 1 | 4 | 0x800ee800, 0x800eefcc, 0x800ef044, 0x800ef090 |
| 0x88 | work.88 | 2 | 2 | 0x800ee70c, 0x800ee800, 0x800ee8c4, 0x800eedb8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x8008eab0 |  | 0x800ee9c4 |
| 0x800970a0 | mega | 0x800ee5cc, 0x800ee70c, 0x800ee800, 0x800ee8c4, 0x800ee9c4, 0x800eedb8, 0x800eefcc |
| 0x800970aa | mega.x.px | 0x800ef044, 0x800ef090, 0x800ef144, 0x800ef1e8, 0x800ef27c |
| 0x800971f8 | camera_layer0 | 0x800ee70c, 0x800ee800, 0x800ee8c4, 0x800ee9c4, 0x800eedb8 |
| 0x80097420 |  | 0x800ee800, 0x800ee9c4, 0x800eedb8, 0x800ef144, 0x800ef1e8, 0x800ef27c |
| 0x800c4560 |  | 0x800ee9c4 |
| 0x800cced0 | game | 0x800ee70c, 0x800ee800, 0x800ee8c4, 0x800ee9c4, 0x800eedb8 |
| 0x800ccedf | game.stage+0x3 | 0x800ef144 |
| 0x800ccef6 | game.stage+0x1a | 0x800ef044 |
| 0x800e98f4 |  | 0x800ee5cc |
| 0x800f77b0 |  | 0x800ee5cc |
| 0x800f77b4 |  | 0x800ee5cc |
| 0x800f77b8 |  | 0x800ee5cc |
| 0x800f77ec |  | 0x800ee5cc |
| 0x800f782c |  | 0x800ee9c4, 0x800eedb8 |
