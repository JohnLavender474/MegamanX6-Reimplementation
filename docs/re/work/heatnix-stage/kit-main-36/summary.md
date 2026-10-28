# RE kit: flame-jet

- Object: flame-jet
- State: `heatnix-1_start.state.gz` (SHA-1 `200340c52e4237622a65f401feed3316575c5e1e`), stage 3 area 0
- Class: main, type 36; entry function 0x800e9cbc
- Functions walked: 13; state tables: 4

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800e9b6c, 0x800e9ee8 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800e9a04, 0x800e9b6c, 0x800e9e24, 0x800e9ee8 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800e98c0 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800e9dc0 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800e98c0 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800e98c0 |
| 0x8002d2d4 | GetRNG | random number | 0x800e9ee8 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800e98c0 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800e98c0 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800e98c0 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f3cec | 0x800e9cbc | 0x800e9cf8, 0x800e98c0, 0x800e9dc0 |
| 0x800f3cf8 | 0x800e98c0 | 0x80040290, 0x800e9de0, 0x800e9eac, 0x800e9de8 |
| 0x800f3d08 | 0x800e9eac | 0x800e9ee8, 0x800e9fd4, 0x800e9a04, 0x800e9b6c |
| 0x800f3d18 | 0x800e9de8 | 0x800e9e24, 0x800e9e80 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f3cbc (5 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 13 0 1 2 3 3 4 5 6 7 | 1 4 4 4 3 1 4 4 4 4 | 0 0 0 0 0 1 0 0 0 0 | 6 |
| 1 | 8 9 10 11 12 13 13 | 4 4 4 4 4 4 1 | 0 0 0 0 0 0 1 | hold |
| 2 | 14 15 16 17 17 18 19 20 21 | 4 4 4 3 1 4 4 4 4 | 0 0 0 0 1 0 0 0 0 | 5 |
| 3 | 22 23 24 25 26 27 27 | 4 4 4 4 4 4 1 | 0 0 0 0 0 0 1 | hold |
| 4 | 13 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 1 | 1 | 0x800e9cf8 |
| 0x2 | var | 9 | 0 | 0x800e9a04, 0x800e9b6c, 0x800e9ee8, 0x800e9fd4 |
| 0x3 | display | 0 | 2 | 0x800e98c0, 0x800e9cf8 |
| 0x4 | state | 2 | 2 | 0x800e98c0, 0x800e9cbc, 0x800e9cf8 |
| 0x5 | step | 1 | 3 | 0x800e98c0, 0x800e9b6c, 0x800e9cf8, 0x800e9e80 |
| 0x6 | step2 | 7 | 9 | 0x800e9a04, 0x800e9b6c, 0x800e9cf8, 0x800e9de8, 0x800e9e24, 0x800e9e80, 0x800e9eac, 0x800e9ee8, 0x800e9fd4 |
| 0x8 | x | 2 | 0 | 0x800e98c0, 0x800e9cf8 |
| 0xc | y | 2 | 0 | 0x800e98c0, 0x800e9cf8 |
| 0x14 | layer | 0 | 1 | 0x800e9cf8 |
| 0x15 | flip | 0 | 2 | 0x800e9cf8 |
| 0x16 | priority | 0 | 1 | 0x800e9cf8 |
| 0x18 | prev_x | 0 | 2 | 0x800e98c0, 0x800e9cf8 |
| 0x1c | prev_y | 0 | 2 | 0x800e98c0, 0x800e9cf8 |
| 0x20 | speed_x | 0 | 1 | 0x800e9cf8 |
| 0x24 | speed_y | 0 | 1 | 0x800e9cf8 |
| 0x28 | accel_x | 0 | 1 | 0x800e9cf8 |
| 0x2c | gravity | 0 | 1 | 0x800e9cf8 |
| 0x30 | anime_table | 0 | 1 | 0x800e9cf8 |
| 0x45 | anime.flag | 2 | 0 | 0x800e9b6c, 0x800e9fd4 |
| 0x47 | anime.sprite | 11 | 0 | 0x800e98c0, 0x800e9a04, 0x800e9b6c, 0x800e9fd4 |
| 0x50 | attack_box | 0 | 3 | 0x800e98c0, 0x800e9cf8 |
| 0x54 | hurt_box | 0 | 3 | 0x800e98c0, 0x800e9cf8 |
| 0x58 | damage_table | 0 | 1 | 0x800e9cf8 |
| 0x5c | hp | 0 | 2 | 0x800e9cf8, 0x800e9e24 |
| 0x60 | contact_damage | 0 | 1 | 0x800e9cf8 |
| 0x61 | iframes | 0 | 1 | 0x800e9cf8 |
| 0x67 | latch | 0 | 1 | 0x800e9cf8 |
| 0x68 | terrain_box | 0 | 1 | 0x800e9cf8 |
| 0x7c | work.7C | 2 | 7 | 0x800e9a04, 0x800e9b6c, 0x800e9e24, 0x800e9e80, 0x800e9ee8 |
| 0x7e | work.7E | 1 | 3 | 0x800e9a04, 0x800e9e24, 0x800e9ee8 |
| 0x84 | work.84 | 1 | 3 | 0x800e98c0, 0x800e9a04, 0x800e9e24 |
| 0x85 | self+0x85 | 1 | 5 | 0x800e9a04, 0x800e9b6c, 0x800e9ee8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f80004c | current_main | 0x800e98c0, 0x800e9a04, 0x800e9b6c, 0x800e9e24, 0x800e9ee8 |
| 0x800743dc | dmg_tbl_normal | 0x800e9cf8 |
| 0x800970b4 | mega.layer | 0x800e9cf8 |
| 0x800f3cbc |  | 0x800e9cf8 |
| 0x800f3cd0 |  | 0x800e9a04, 0x800e9b6c, 0x800e9fd4 |
