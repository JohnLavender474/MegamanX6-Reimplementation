# RE kit: misc23

- Object: misc23
- State: `/home/johnlavender/mmx-data/x6/dumps/wolfang-3_falling-ice.state.gz` (SHA-1 `7a3a9a1b7d8520a56eb02742dc0bb36027f99492`), stage 2 area 0
- Class: misc, type 23; entry function 0x800f05f0
- Functions walked: 4; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800f062c |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800f0758 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800f0718 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6e80 | 0x800f05f0 | 0x800f062c, 0x800f0718, 0x800f0758 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800f062c |
| 0x2 | var | 2 | 0 | 0x800f062c, 0x800f0718 |
| 0x3 | display | 0 | 1 | 0x800f0718 |
| 0x4 | state | 2 | 1 | 0x800f05f0, 0x800f062c |
| 0x16 | priority | 0 | 1 | 0x800f062c |
| 0x3c | sprite_data | 0 | 1 | 0x800f062c |
| 0x40 | tex_cord | 0 | 1 | 0x800f062c |
| 0x42 | clut_cord | 0 | 1 | 0x800f062c |
| 0x47 | anime.sprite | 0 | 1 | 0x800f062c |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800f062c |
| 0x80095098 | bank_tpage | 0x800f062c |
