# RE kit: shark-alt-kind7-2

- Object: shark-alt-kind7-2
- State: `shark-10_alt.state.gz` (SHA-1 `114fb5c590494f5a3bc9dc0ee53f3e58ec1371f6`), stage 20 area 1
- Class: kind7, type 2; entry function 0x80046e14
- Functions walked: 1; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x80046e14 |
| 0x800472d0 | PressInit | press state 0: speed 2.0, count 8 (stage 4) or 16 (stage >= 19); else layer-1 mode 9 and start in sub 1 | 0x80046e14 |
| 0x80047394 | PressRun | press state 1: game+0x27 = 0, sub-step from 0x800762C8 unless mega+0xE0 = 2 | 0x80046e14 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800762c0 | 0x80046e14 | 0x800472d0, 0x80047394 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 1 | 0 | 0x80046e14 |
| 0x4 | state | 1 | 0 | 0x80046e14 |
| 0x1e | self+0x1e | 1 | 0 | 0x80046e14 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800970a0 | mega | 0x80046e14 |
| 0x800971f8 | camera_layer0 | 0x80046e14 |
| 0x800cced0 | game | 0x80046e14 |
