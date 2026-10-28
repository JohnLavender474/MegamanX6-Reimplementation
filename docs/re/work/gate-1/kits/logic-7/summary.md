# RE kit: logic-7

- Object: logic-7
- State: `gate-1_before-boss.state.gz` (SHA-1 `585948ebe43e9241ae0828aff8654b2e9b98ea04`), stage 16 area 0
- Class: logic, type 7; entry function 0x800e9bd8
- Functions walked: 3; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800e9bd8, 0x800ea03c |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fb6ec | 0x800e9bd8 | 0x800ea750, 0x800ea03c |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e9864 | 0x800e9c94 | 0x800e9bd8 | 61 |
| 0x800e995c | 0x800ea1ec | 0x800ea03c | 49 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x4 | state | 5 | 7 | 0x800e9bd8, 0x800ea03c, 0x800ea750 |
| 0x14 | layer | 1 | 9 | 0x800ea03c, 0x800ea750 |
| 0x18 | prev_x | 0 | 4 | 0x800ea03c, 0x800ea750 |
| 0x1c | prev_y | 4 | 4 | 0x800e9bd8, 0x800ea03c, 0x800ea750 |
| 0x1d | self+0x1d | 0 | 3 | 0x800ea03c |
| 0x1e | self+0x1e | 5 | 3 | 0x800e9bd8, 0x800ea03c, 0x800ea750 |
| 0x1f | self+0x1f | 0 | 2 | 0x800ea03c |
| 0x20 | speed_x | 2 | 3 | 0x800e9bd8 |
| 0x21 | self+0x21 | 4 | 1 | 0x800e9bd8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800970a0 | mega | 0x800e9bd8, 0x800ea03c |
| 0x800fb6f4 |  | 0x800e9bd8 |
