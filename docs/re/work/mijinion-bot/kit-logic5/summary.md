# RE kit: logic5

- Object: logic5
- State: `/home/johnlavender/mmx-data/x6/dumps/mijinion-2_illumina-helpers.state.gz` (SHA-1 `d4d3f533583adc4daf98ad9d9cb742eed247cf06`), stage 8 area 0
- Class: logic, type 5; entry function 0x800fb13c
- Functions walked: 3; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fd508 | 0x800fb13c | 0x800fb178, 0x800fb1b4 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x4 | state | 2 | 1 | 0x800fb13c, 0x800fb178 |
| 0x14 | layer | 3 | 5 | 0x800fb178, 0x800fb1b4 |
| 0x15 | flip | 1 | 3 | 0x800fb178, 0x800fb1b4 |
| 0x16 | priority | 1 | 1 | 0x800fb1b4 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800971f8 | camera_layer0 | 0x800fb1b4 |
| 0x800972a4 | cam1.old_y+0x3e | 0x800fb178 |
