# RE kit: scaravich-crystals

- Object: scaravich-crystals
- State: `scaravich-1_start.state.gz` (SHA-1 `4b9cab59f2dd853bd5034ef66217cebb0de9b9bb`), stage 5 area 0
- Class: main, type 28; entry function 0x800eecfc
- Functions walked: 18; state tables: 4

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800139a0 | FadeIn | FadeIn(speed): fade_dir = +1 | 0x800ee7fc |
| 0x800139f4 | FadeOut | FadeOut(speed): fade_dir = -1 | 0x800eef7c, 0x800ef020 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800ee544, 0x800ee6fc, 0x800ee7fc |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ee404 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800eed48 |
| 0x80029f38 | SpawnInRect | SpawnInRect(x0, x1, y0, y1, flag): spawn the records inside a rectangle | 0x800ee7fc |
| 0x8002b36c | ResetLayerCache | ResetLayerCache(layer): redraw state of a BG layer | 0x800ee7fc |
| 0x8002b6b0 | ClearPlayerShots | delete the weapon objects except type 55, and flagged effects | 0x800ee544, 0x800ee6fc, 0x800ee7fc |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ef11c |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800eedc4 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800eedc4 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800eed48 |
| 0x8002d654 | BoxOverlap | box test between two objects, facing-aware | 0x800eee04 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800eecfc |
| 0x8003ad18 | ToStand | clear the dash flags, step 2, the stand animation (94 when shooting) | 0x800eebf0 |
| 0x8003c4ec | PlayerPaletteRestore | the palettes of the weapon slot mega+0x93 and of the part bits | 0x800ee544, 0x800ee6fc |
| 0x8003cdf0 | ClearActionFlags | mega+0x84, +0x86, +0x8C, +0x8E..+0x92, +0xA2 = 0; palette | 0x800ee544, 0x800ee6fc |
| 0x8003ce30 | ClearChargeGlow | mega+0xA1 = 0 | 0x800ee544, 0x800ee6fc |
| 0x8003f3dc | ChargeStop | X: stop the charge (palette by mega+0x93) (likely) | 0x800ee544, 0x800ee6fc |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800eed48 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f5b78 | 0x800eecfc | 0x800ee404, 0x800eed48, 0x800eedc4, 0x800eedc4 |
| 0x800f5b88 | 0x800eed48 | 0x80040290, 0x800eee04, 0x800eef04, 0x800eef40, 0x800eee7c, 0x800eeec8 |
| 0x800f5ba0 | 0x800eef04 | 0x800ee544, 0x800eef7c, 0x800ee638 |
| 0x800f5c3c | 0x800eef40 | 0x800ee6fc, 0x800ef020, 0x800ee7fc, 0x800ef0b4, 0x800eebf0, 0x800ef11c |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f5b6c (2 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 | 3 3 3 3 3 3 | 0 0 0 0 0 0 | 0 |
| 1 | 6 7 8 9 10 11 | 3 3 3 3 3 3 | 0 0 0 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 3 | 3 | 0x800ee544, 0x800ee6fc, 0x800eebf0 |
| 0x2 | var | 5 | 0 | 0x800ee404, 0x800ee7fc, 0x800eed48, 0x800eee04, 0x800eef7c |
| 0x3 | display | 0 | 1 | 0x800ee404 |
| 0x4 | state | 2 | 1 | 0x800ee404, 0x800eecfc, 0x800eedc4 |
| 0x5 | step | 1 | 5 | 0x800ee404, 0x800eed48, 0x800eee04, 0x800eee7c, 0x800eeec8 |
| 0x6 | step2 | 9 | 12 | 0x800ee404, 0x800ee544, 0x800ee6fc, 0x800ee7fc, 0x800eebf0, 0x800eee04, 0x800eee7c, 0x800eeec8, 0x800eef04, 0x800eef40, 0x800eef7c, 0x800ef020, 0x800ef0b4 |
| 0x8 | x | 2 | 0 | 0x800ee404, 0x800eed48 |
| 0xa | x.px | 2 | 0 | 0x800eee7c, 0x800eeec8 |
| 0xc | y | 2 | 0 | 0x800ee404, 0x800eed48 |
| 0x14 | layer | 0 | 1 | 0x800ee404 |
| 0x15 | flip | 0 | 2 | 0x800ee404 |
| 0x16 | priority | 0 | 1 | 0x800ee404 |
| 0x18 | prev_x | 0 | 2 | 0x800ee404, 0x800eed48 |
| 0x1c | prev_y | 0 | 2 | 0x800ee404, 0x800eed48 |
| 0x30 | anime_table | 0 | 1 | 0x800ee404 |
| 0x50 | attack_box | 0 | 1 | 0x800ee404 |
| 0x54 | hurt_box | 0 | 1 | 0x800ee404 |
| 0x5c | hp | 0 | 1 | 0x800ee404 |
| 0x60 | contact_damage | 0 | 1 | 0x800ee404 |
| 0x61 | iframes | 0 | 1 | 0x800ee404 |
| 0x67 | latch | 0 | 1 | 0x800ee404 |
| 0x68 | terrain_box | 0 | 1 | 0x800ee404 |
| 0x70 | collide_flags | 0 | 1 | 0x800ee404 |
| 0x7c | work.7C | 8 | 11 | 0x800ee404, 0x800ee544, 0x800ee6fc, 0x800eebf0, 0x800eee04, 0x800eee7c, 0x800eeec8, 0x800eef7c, 0x800ef020, 0x800ef0b4 |
| 0x7e | work.7E | 16 | 3 | 0x800ee7fc, 0x800eebf0 |
| 0x80 | work.80 | 6 | 9 | 0x800ee544, 0x800ee638, 0x800ee6fc, 0x800ee7fc, 0x800eebf0, 0x800eef7c, 0x800ef020, 0x800ef0b4 |
| 0x82 | self+0x82 | 6 | 9 | 0x800ee544, 0x800ee638, 0x800ee6fc, 0x800ee7fc, 0x800eebf0, 0x800eef7c, 0x800ef020, 0x800ef0b4 |
| 0x84 | work.84 | 1 | 4 | 0x800ee638, 0x800eee04, 0x800eee7c, 0x800eeec8 |
| 0x88 | work.88 | 2 | 2 | 0x800ee544, 0x800ee638, 0x800ee6fc, 0x800eebf0 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x8008eab0 | stage_texture_swap | 0x800ee7fc |
| 0x800970a0 | mega | 0x800ee404, 0x800ee544, 0x800ee638, 0x800ee6fc, 0x800ee7fc, 0x800eebf0, 0x800eee04 |
| 0x800970aa | mega.x.px | 0x800eee7c, 0x800eeec8, 0x800eef7c, 0x800ef020, 0x800ef0b4 |
| 0x800971f8 | camera_layer0 | 0x800ee544, 0x800ee638, 0x800ee6fc, 0x800ee7fc, 0x800eebf0 |
| 0x80097420 | frame_count | 0x800ee638, 0x800ee7fc, 0x800eebf0, 0x800eef7c, 0x800ef020, 0x800ef0b4 |
| 0x800c4560 | bg_refresh | 0x800ee7fc |
| 0x800cced0 | game | 0x800ee544, 0x800ee638, 0x800ee6fc, 0x800ee7fc, 0x800eebf0 |
| 0x800ccedf | game.exit_req | 0x800eef7c |
| 0x800ccef6 | game.stage_flag | 0x800eee7c |
| 0x800e98ec |  | 0x800ee404 |
| 0x800f5b30 |  | 0x800ee404 |
| 0x800f5b34 |  | 0x800ee404 |
| 0x800f5b38 |  | 0x800ee404 |
| 0x800f5b6c |  | 0x800ee404 |
| 0x800f5bac |  | 0x800ee7fc, 0x800eebf0 |
