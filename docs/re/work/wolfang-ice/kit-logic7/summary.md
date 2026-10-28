# RE kit: logic7

- Object: logic7
- State: `/home/johnlavender/mmx-data/x6/dumps/wolfang-3_falling-ice.state.gz` (SHA-1 `7a3a9a1b7d8520a56eb02742dc0bb36027f99492`), stage 2 area 0
- Class: logic, type 7; entry function 0x800ed3a4
- Functions walked: 4; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800ed3a4, 0x800ed808, 0x800edb68 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6c28 | 0x800ed3a4 | 0x800edf1c, 0x800ed808 |
| 0x800f6c30 | 0x800ed3a4 | 0x800edb68 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e98f4 | 0x800ed460 | 0x800ed3a4 | 61 |
| 0x800e99ec | 0x800ed9b8 | 0x800ed808 | 49 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x4 | state | 5 | 13 | 0x800ed3a4, 0x800ed808, 0x800edb68, 0x800edf1c |
| 0x8 | x | 3 | 2 | 0x800edb68 |
| 0xa | x.px | 2 | 0 | 0x800edb68 |
| 0x14 | layer | 6 | 11 | 0x800ed808, 0x800edb68, 0x800edf1c |
| 0x18 | prev_x | 3 | 5 | 0x800ed808, 0x800edb68, 0x800edf1c |
| 0x1c | prev_y | 4 | 4 | 0x800ed3a4, 0x800ed808, 0x800edf1c |
| 0x1d | self+0x1d | 4 | 4 | 0x800ed808, 0x800edb68 |
| 0x1e | self+0x1e | 5 | 3 | 0x800ed3a4, 0x800ed808, 0x800edf1c |
| 0x1f | self+0x1f | 1 | 3 | 0x800ed808, 0x800edb68 |
| 0x20 | speed_x | 2 | 3 | 0x800ed3a4 |
| 0x21 | self+0x21 | 4 | 1 | 0x800ed3a4 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800970a0 | mega | 0x800ed3a4, 0x800ed808, 0x800edb68 |
