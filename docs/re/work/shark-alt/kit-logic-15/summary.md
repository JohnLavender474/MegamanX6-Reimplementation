# RE kit: shark-alt-logic-15

- Object: shark-alt-logic-15
- State: `shark-10_alt.state.gz` (SHA-1 `114fb5c590494f5a3bc9dc0ee53f3e58ec1371f6`), stage 20 area 1
- Class: logic, type 15; entry function 0x800f0984
- Functions walked: 4; state tables: 0

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800f0984 |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800f089c |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800f0984 |
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800f0984 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x80028078 | 0x800f04c0 |

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
| 0x4 | state | 2 | 1 | 0x800f0984 |
| 0x14 | layer | 2 | 2 | 0x800f0984 |
| 0x35ec | self+0x35ec | 1 | 0 | 0x800f089c |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800008 | layout_base+0x4 | 0x800f04c0 |
| 0x800970a0 | mega | 0x800f0984 |
| 0x800971f8 | camera_layer0 | 0x800f089c |
| 0x80097202 | cam.x | 0x800f0984 |
| 0x80097420 | frame_count | 0x800f0754, 0x800f0984 |
| 0x800cd340 | actor2 | 0x800f0984 |
| 0x800e3142 | belt_turns | 0x800f0984 |
| 0x800f34fc |  | 0x800f089c |
| 0x800f3504 |  | 0x800f089c |
| 0x800f3524 |  | 0x800f04c0 |
| 0x800f35ac |  | 0x800f0754 |
| 0x800f35ec |  | 0x800f089c, 0x800f0984 |
