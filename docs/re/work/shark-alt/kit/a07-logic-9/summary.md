# RE kit: a07-logic-9

- Object: a07-logic-9
- State: `shark-2_section-2.state.gz` (SHA-1 `2ed5581179aa6acd6cf436aac85487b11ea49bd0`), stage 4 area 1
- Class: logic, type 9; entry function 0x800ed998
- Functions walked: 4; state tables: 0

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ed998 |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800ed8ac |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ed998 |
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800ed998 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x80028078 | 0x800ed4d0 |

## State tables

| table | used by | targets |
|---|---|---|

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| -0x67a4 | self+-0x67a4 | 1 | 0 | 0x800ed8ac |
| 0x4 | state | 2 | 1 | 0x800ed998 |
| 0x14 | layer | 2 | 2 | 0x800ed998 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800008 | layout_base+0x4 | 0x800ed4d0 |
| 0x800970a0 | mega | 0x800ed998 |
| 0x800971f8 | camera_layer0 | 0x800ed8ac, 0x800ed998 |
| 0x80097420 | frame_count | 0x800ed764, 0x800ed998 |
| 0x800cced0 | game | 0x800ed998 |
| 0x800e3142 | belt_turns | 0x800ed998 |
| 0x800f88e8 |  | 0x800ed8ac |
| 0x800f8988 |  | 0x800ed8ac |
| 0x800f8c08 |  | 0x800ed4d0 |
| 0x800f981c |  | 0x800ed764 |
| 0x800f985c | belt_frame | 0x800ed8ac, 0x800ed998 |
