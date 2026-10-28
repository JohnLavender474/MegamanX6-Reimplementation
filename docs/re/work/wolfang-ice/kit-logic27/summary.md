# RE kit: logic27

- Object: logic27
- State: `/home/johnlavender/mmx-data/x6/dumps/wolfang-3_falling-ice.state.gz` (SHA-1 `7a3a9a1b7d8520a56eb02742dc0bb36027f99492`), stage 2 area 0
- Class: logic, type 27; entry function 0x800541d4
- Functions walked: 4; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002c7f0 | GetRayObject | first free of 64 x 0x60 slots at 0x8009C9B0 (Nightmare Virus rays) | 0x8005400c |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x80054240 |
| 0x8002d2d4 | GetRNG | random number | 0x8005400c |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x8007ad28 | 0x800541d4 | 0x80054210, 0x8005400c, 0x80054240 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x4 | state | 1 | 1 | 0x800541d4, 0x80054210 |
| 0x5 | step | 1 | 4 | 0x8005400c, 0x80054210 |
| 0x14 | layer | 6 | 7 | 0x8005400c, 0x80054210 |
| 0x15 | flip | 0 | 1 | 0x80054210 |
| 0x16 | priority | 0 | 2 | 0x8005400c, 0x80054210 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x80097424 | fade_dir | 0x80054210 |
