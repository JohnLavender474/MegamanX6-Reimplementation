# RE kit: caterpirade-main-7

- Object: caterpirade-main-7
- State: `yammark-3_caterpirade.state.gz` (SHA-1 `eb47e7dea70a84414b2e9ee94b8fe0e26cc9ae24`), stage 1 area 0
- Class: main, type 7; entry function 0x800ec25c
- Functions walked: 1; state tables: 0

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ec25c |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ec25c |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x8002c4c4 | 0x800ec25c |

## State tables

| table | used by | targets |
|---|---|---|

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 1 | 0 | 0x800ec25c |
| 0x2 | var | 1 | 0 | 0x800ec25c |
| 0xa | x.px | 1 | 0 | 0x800ec25c |
| 0xe | y.px | 1 | 0 | 0x800ec25c |
| 0x10 | record | 1 | 0 | 0x800ec25c |
| 0x3c | sprite_data | 1 | 0 | 0x800ec25c |
| 0x40 | tex_cord | 1 | 0 | 0x800ec25c |
| 0x42 | clut_cord | 1 | 0 | 0x800ec25c |

## Globals and data

| address | name | used by |
|---|---|---|
