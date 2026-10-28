# RE kit: logic12

- Object: logic12
- State: `/home/johnlavender/mmx-data/x6/dumps/wolfang-3_falling-ice.state.gz` (SHA-1 `7a3a9a1b7d8520a56eb02742dc0bb36027f99492`), stage 2 area 0
- Class: logic, type 12; entry function 0x800f049c
- Functions walked: 8; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80012d9c | Rumble | controller vibration (likely) | 0x800effd8 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800effd8, 0x800f049c |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800f049c |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800effd8, 0x800f062c |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x800effd8 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800f049c, 0x800f05d0 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800f0758 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800f0718 |
| 0x8002d2d4 | GetRNG | random number | 0x800effd8 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6dfc | 0x800f049c | 0x800f058c, 0x800effd8, 0x800f05d0 |
| 0x800f6e80 | 0x800f05f0 | 0x800f062c, 0x800f0718, 0x800f0758 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| misc | 23 | 0x800f05f0 |

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x4 | state | 2 | 1 | 0x800f049c, 0x800f058c |
| 0x5 | step | 3 | 4 | 0x800effd8, 0x800f058c |
| 0x6 | step2 | 2 | 3 | 0x800effd8, 0x800f058c |
| 0x7 | act4 | 7 | 4 | 0x800effd8, 0x800f058c |
| 0x14 | layer | 4 | 3 | 0x800effd8, 0x800f049c, 0x800f058c |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800effd8, 0x800f062c |
| 0x8008ef48 | main_objects | 0x800effd8 |
| 0x80095098 | bank_tpage | 0x800effd8, 0x800f062c |
| 0x800970a0 | mega | 0x800effd8, 0x800f049c |
| 0x800970ae | mega.y.px | 0x800effd8 |
| 0x800971f8 | camera_layer0 | 0x800f049c |
| 0x800cced0 | game | 0x800effd8, 0x800f058c |
| 0x800f6df4 |  | 0x800f049c |
| 0x800f6e08 |  | 0x800effd8 |
| 0x800f6e0c |  | 0x800effd8 |
