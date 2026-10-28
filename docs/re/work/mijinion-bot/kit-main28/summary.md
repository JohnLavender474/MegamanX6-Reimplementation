# RE kit: main28

- Object: main28
- State: `/home/johnlavender/mmx-data/x6/dumps/mijinion-2_illumina-helpers.state.gz` (SHA-1 `d4d3f533583adc4daf98ad9d9cb742eed247cf06`), stage 8 area 0
- Class: main, type 28; entry function 0x800f57f4
- Functions walked: 18; state tables: 4

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800139a0 | FadeIn | FadeIn(speed): fade_dir = +1 | 0x800f52f4 |
| 0x800139f4 | FadeOut | FadeOut(speed): fade_dir = -1 | 0x800f5a74, 0x800f5b18 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800f503c, 0x800f51f4, 0x800f52f4 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800f4efc |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800f5840 |
| 0x80029f38 | SpawnInRect | SpawnInRect(x0, x1, y0, y1, flag): spawn the records inside a rectangle | 0x800f52f4 |
| 0x8002b36c | ResetLayerCache | ResetLayerCache(layer): redraw state of a BG layer | 0x800f52f4 |
| 0x8002b6b0 | ClearPlayerShots | delete the weapon objects except type 55, and flagged effects | 0x800f503c, 0x800f51f4, 0x800f52f4 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800f5c14 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800f58bc |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800f58bc |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800f5840 |
| 0x8002d654 | BoxOverlap | box test between two objects, facing-aware | 0x800f58fc |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800f57f4 |
| 0x8003ad18 | ToStand | clear the dash flags, step 2, the stand animation (94 when shooting) | 0x800f56e8 |
| 0x8003c4ec | PlayerPaletteRestore | the palettes of the weapon slot mega+0x93 and of the part bits | 0x800f503c, 0x800f51f4 |
| 0x8003cdf0 | ClearActionFlags | mega+0x84, +0x86, +0x8C, +0x8E..+0x92, +0xA2 = 0; palette | 0x800f503c, 0x800f51f4 |
| 0x8003ce30 | ClearChargeGlow | mega+0xA1 = 0 | 0x800f503c, 0x800f51f4 |
| 0x8003f3dc | ChargeStop | X: stop the charge (palette by mega+0x93) (likely) | 0x800f503c, 0x800f51f4 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800f5840 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fcc7c | 0x800f57f4 | 0x800f4efc, 0x800f5840, 0x800f58bc, 0x800f58bc |
| 0x800fcc8c | 0x800f5840 | 0x80040290, 0x800f58fc, 0x800f59fc, 0x800f5a38, 0x800f5974, 0x800f59c0 |
| 0x800fcca4 | 0x800f59fc | 0x800f503c, 0x800f5a74, 0x800f5130 |
| 0x800fcd40 | 0x800f5a38 | 0x800f51f4, 0x800f5b18, 0x800f52f4, 0x800f5bac, 0x800f56e8, 0x800f5c14 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e9a24 | 0x800f4fc4 | 0x800f4efc | 5 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800fcc70 (2 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 | 3 3 3 3 3 3 | 0 0 0 0 0 0 | 0 |
| 1 | 6 7 8 9 10 11 | 3 3 3 3 3 3 | 0 0 0 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 3 | 3 | 0x800f503c, 0x800f51f4, 0x800f56e8 |
| 0x2 | var | 5 | 0 | 0x800f4efc, 0x800f52f4, 0x800f5840, 0x800f58fc, 0x800f5a74 |
| 0x3 | display | 0 | 1 | 0x800f4efc |
| 0x4 | state | 2 | 1 | 0x800f4efc, 0x800f57f4, 0x800f58bc |
| 0x5 | step | 1 | 5 | 0x800f4efc, 0x800f5840, 0x800f58fc, 0x800f5974, 0x800f59c0 |
| 0x6 | step2 | 9 | 12 | 0x800f4efc, 0x800f503c, 0x800f51f4, 0x800f52f4, 0x800f56e8, 0x800f58fc, 0x800f5974, 0x800f59c0, 0x800f59fc, 0x800f5a38, 0x800f5a74, 0x800f5b18, 0x800f5bac |
| 0x8 | x | 2 | 0 | 0x800f4efc, 0x800f5840 |
| 0xa | x.px | 2 | 0 | 0x800f5974, 0x800f59c0 |
| 0xc | y | 2 | 0 | 0x800f4efc, 0x800f5840 |
| 0x14 | layer | 0 | 1 | 0x800f4efc |
| 0x15 | flip | 0 | 2 | 0x800f4efc |
| 0x16 | priority | 0 | 1 | 0x800f4efc |
| 0x18 | prev_x | 0 | 2 | 0x800f4efc, 0x800f5840 |
| 0x1c | prev_y | 0 | 2 | 0x800f4efc, 0x800f5840 |
| 0x30 | anime_table | 0 | 1 | 0x800f4efc |
| 0x50 | attack_box | 0 | 1 | 0x800f4efc |
| 0x54 | hurt_box | 0 | 1 | 0x800f4efc |
| 0x5c | hp | 0 | 1 | 0x800f4efc |
| 0x60 | contact_damage | 0 | 1 | 0x800f4efc |
| 0x61 | iframes | 0 | 1 | 0x800f4efc |
| 0x67 | latch | 0 | 1 | 0x800f4efc |
| 0x68 | terrain_box | 0 | 1 | 0x800f4efc |
| 0x70 | collide_flags | 0 | 1 | 0x800f4efc |
| 0x7c | work.7C | 8 | 11 | 0x800f4efc, 0x800f503c, 0x800f51f4, 0x800f56e8, 0x800f58fc, 0x800f5974, 0x800f59c0, 0x800f5a74, 0x800f5b18, 0x800f5bac |
| 0x7e | work.7E | 16 | 3 | 0x800f52f4, 0x800f56e8 |
| 0x80 | work.80 | 6 | 9 | 0x800f503c, 0x800f5130, 0x800f51f4, 0x800f52f4, 0x800f56e8, 0x800f5a74, 0x800f5b18, 0x800f5bac |
| 0x82 | self+0x82 | 6 | 9 | 0x800f503c, 0x800f5130, 0x800f51f4, 0x800f52f4, 0x800f56e8, 0x800f5a74, 0x800f5b18, 0x800f5bac |
| 0x84 | work.84 | 1 | 4 | 0x800f5130, 0x800f58fc, 0x800f5974, 0x800f59c0 |
| 0x88 | work.88 | 2 | 2 | 0x800f503c, 0x800f5130, 0x800f51f4, 0x800f56e8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x8008eab0 | stage_texture_swap | 0x800f52f4 |
| 0x800970a0 | mega | 0x800f4efc, 0x800f503c, 0x800f5130, 0x800f51f4, 0x800f52f4, 0x800f56e8, 0x800f58fc |
| 0x800970aa | mega.x.px | 0x800f5974, 0x800f59c0, 0x800f5a74, 0x800f5b18, 0x800f5bac |
| 0x800971f8 | camera_layer0 | 0x800f503c, 0x800f5130, 0x800f51f4, 0x800f52f4, 0x800f56e8 |
| 0x80097420 | frame_count | 0x800f5130, 0x800f52f4, 0x800f56e8, 0x800f5a74, 0x800f5b18, 0x800f5bac |
| 0x800c4560 | bg_refresh | 0x800f52f4 |
| 0x800cced0 | game | 0x800f503c, 0x800f5130, 0x800f51f4, 0x800f52f4, 0x800f56e8 |
| 0x800ccedf | game.exit_req | 0x800f5a74 |
| 0x800ccef6 | game.stage_flag | 0x800f5974 |
| 0x800fcc34 |  | 0x800f4efc |
| 0x800fcc38 |  | 0x800f4efc |
| 0x800fcc3c |  | 0x800f4efc |
| 0x800fcc70 |  | 0x800f4efc |
| 0x800fccb0 |  | 0x800f52f4, 0x800f56e8 |
