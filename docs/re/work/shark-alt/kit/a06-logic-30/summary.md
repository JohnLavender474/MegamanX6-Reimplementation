# RE kit: a06-logic-30

- Object: a06-logic-30
- State: `shark-1_ride.state.gz` (SHA-1 `fcf098dcbeb64cf58e6131364bb12e806937d130`), stage 4 area 0
- Class: logic, type 30; entry function 0x800ee158
- Functions walked: 1; state tables: 0

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002c9d4 | DeleteObjectKeep | record flags |= 0x80 (the spawner skips flags & 0x81), then DeleteObject3: single-use objects | 0x800ee158 |

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
| 0x2 | var | 2 | 0 | 0x800ee158 |
| 0x4 | state | 0 | 1 | 0x800ee158 |
| 0x5 | step | 0 | 1 | 0x800ee158 |
| 0x8 | x | 1 | 0 | 0x800ee158 |
| 0xc | y | 1 | 0 | 0x800ee158 |
| 0x10 | record | 1 | 0 | 0x800ee158 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800cd340 | actor2 | 0x800ee158 |
