# RE kit: main19

- Object: main19
- State: `/home/johnlavender/mmx-data/x6/dumps/wolfang-3_falling-ice.state.gz` (SHA-1 `7a3a9a1b7d8520a56eb02742dc0bb36027f99492`), stage 2 area 0
- Class: main, type 19; entry function 0x800ef594
- Functions walked: 7; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80012d9c | Rumble | controller vibration (likely) | 0x800ef160 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800eeea0 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800eeea0 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ef70c |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800eeea0 |
| 0x8002943c | Quake2 | screen shake variant: camera work +0x36/+0x45/+0x3E/+0x3C/+0x3A, flag 0x10 | 0x800ef160 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800eeea0 |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x800ef160 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800eeea0, 0x800ef698, 0x800ef6b8 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ef5d0 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ef5d0 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ef70c |
| 0x8002d2d4 | GetRNG | random number | 0x800eeea0, 0x800ef160 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ef5d0 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ef5d0 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ef5d0 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ef5d0 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ef5d0 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6d5c | 0x800ef594 | 0x800eeea0, 0x800ef5d0, 0x800ef698, 0x800ef6b8 |
| 0x800f6d84 | 0x800ef5d0 | 0x80040290, 0x800ef70c, 0x800ef160 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f6d58 (1 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 2 1 3 0 4 1 5 0 6 1 7 0 8 | 3 3 3 3 3 3 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |

Note: cut: animation 1 uses sprite 174; the sprite set has 10

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800eeea0 |
| 0x1 | type | 4 | 0 | 0x800eeea0 |
| 0x2 | var | 2 | 0 | 0x800eeea0, 0x800ef5d0 |
| 0x3 | display | 1 | 2 | 0x800eeea0, 0x800ef6b8 |
| 0x4 | state | 1 | 2 | 0x800eeea0, 0x800ef594, 0x800ef5d0 |
| 0x5 | step | 1 | 2 | 0x800eeea0, 0x800ef5d0 |
| 0x6 | step2 | 0 | 1 | 0x800eeea0 |
| 0x8 | x | 2 | 0 | 0x800eeea0, 0x800ef5d0 |
| 0xc | y | 2 | 0 | 0x800eeea0, 0x800ef5d0 |
| 0x14 | layer | 0 | 1 | 0x800eeea0 |
| 0x15 | flip | 0 | 1 | 0x800eeea0 |
| 0x16 | priority | 0 | 1 | 0x800eeea0 |
| 0x18 | prev_x | 0 | 2 | 0x800eeea0, 0x800ef5d0 |
| 0x1c | prev_y | 0 | 2 | 0x800eeea0, 0x800ef5d0 |
| 0x20 | speed_x | 0 | 2 | 0x800eeea0 |
| 0x24 | speed_y | 0 | 2 | 0x800eeea0 |
| 0x28 | accel_x | 0 | 1 | 0x800eeea0 |
| 0x2c | gravity | 0 | 1 | 0x800eeea0 |
| 0x30 | anime_table | 1 | 1 | 0x800eeea0, 0x800ef160 |
| 0x3c | sprite_data | 1 | 1 | 0x800eeea0, 0x800ef160 |
| 0x40 | tex_cord | 1 | 1 | 0x800eeea0, 0x800ef160 |
| 0x42 | clut_cord | 1 | 1 | 0x800eeea0, 0x800ef160 |
| 0x50 | attack_box | 0 | 1 | 0x800eeea0 |
| 0x54 | hurt_box | 0 | 1 | 0x800eeea0 |
| 0x58 | damage_table | 0 | 1 | 0x800eeea0 |
| 0x5c | hp | 0 | 1 | 0x800eeea0 |
| 0x60 | contact_damage | 0 | 1 | 0x800eeea0 |
| 0x61 | iframes | 0 | 1 | 0x800eeea0 |
| 0x67 | latch | 0 | 1 | 0x800eeea0 |
| 0x68 | terrain_box | 0 | 2 | 0x800eeea0 |
| 0x70 | collide_flags | 0 | 1 | 0x800eeea0 |
| 0x7c | work.7C | 1 | 2 | 0x800eeea0, 0x800ef6b8 |
| 0x7e | work.7E | 1 | 3 | 0x800eeea0, 0x800ef160 |
| 0x80 | work.80 | 3 | 5 | 0x800eeea0, 0x800ef160 |
| 0x81 | self+0x81 | 1 | 2 | 0x800eeea0, 0x800ef160 |
| 0x82 | self+0x82 | 1 | 3 | 0x800eeea0, 0x800ef160 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800eeea0 |
| 0x8007465c | dmg_tbl_reflector_wait+0xa0 | 0x800eeea0 |
| 0x80095098 | bank_tpage | 0x800eeea0 |
| 0x800970a0 | mega | 0x800ef160 |
| 0x800970aa | mega.x.px | 0x800ef160 |
| 0x800970b4 | mega.layer | 0x800eeea0 |
| 0x800971f8 | camera_layer0 | 0x800ef160 |
| 0x80097244 | cam.redraw | 0x800eeea0 |
| 0x800cced0 | game | 0x800eeea0, 0x800ef160 |
| 0x800f6d08 |  | 0x800eeea0 |
| 0x800f6d10 |  | 0x800eeea0 |
| 0x800f6d18 |  | 0x800eeea0 |
| 0x800f6d58 |  | 0x800eeea0 |
| 0x800f6d6c |  | 0x800eeea0 |
| 0x800f6d7c |  | 0x800eeea0 |
