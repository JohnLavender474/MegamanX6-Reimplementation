# RE kit: a28-logic-30

- Object: a28-logic-30
- State: `shark-10_alt.state.gz` (SHA-1 `114fb5c590494f5a3bc9dc0ee53f3e58ec1371f6`), stage 20 area 1
- Class: logic, type 30; entry function 0x800ebee8
- Functions walked: 1; state tables: 0

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002c9d4 | DeleteObjectKeep | record flags |= 0x80 (the spawner skips flags & 0x81), then DeleteObject3: single-use objects | 0x800ebee8 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

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
| 0x2 | var | 2 | 0 | 0x800ebee8 |
| 0x4 | state | 0 | 1 | 0x800ebee8 |
| 0x5 | step | 0 | 1 | 0x800ebee8 |
| 0x8 | x | 1 | 0 | 0x800ebee8 |
| 0xc | y | 1 | 0 | 0x800ebee8 |
| 0x10 | record | 1 | 0 | 0x800ebee8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800cd340 | actor2 | 0x800ebee8 |
