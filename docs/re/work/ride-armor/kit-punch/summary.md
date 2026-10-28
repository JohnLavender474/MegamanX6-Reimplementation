# RE kit: weapon 61/62 ride punch

- Object: weapon 61/62 ride punch
- State: `shark-1_ride.state.gz` (SHA-1 `fcf098dcbeb64cf58e6131364bb12e806937d130`), stage 4 area 0
- Class: actor2, type 0; entry function 0x80046830
- Functions walked: 4; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x8004672c, 0x800468bc |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x80076280 | 0x80046830 | 0x8004686c, 0x8004672c, 0x800468bc |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 2 | 0 | 0x8004672c |
| 0x3 | display | 0 | 1 | 0x8004686c |
| 0x4 | state | 1 | 1 | 0x80046830, 0x8004686c |
| 0xa | x.px | 0 | 1 | 0x8004672c |
| 0xe | y.px | 0 | 1 | 0x8004672c |
| 0x14 | layer | 0 | 1 | 0x8004686c |
| 0x45 | anime.flag | 1 | 1 | 0x8004672c |
| 0x50 | attack_box | 0 | 3 | 0x8004672c, 0x8004686c |
| 0x54 | hurt_box | 0 | 1 | 0x8004686c |
| 0x5c | hp | 0 | 1 | 0x8004686c |
| 0x60 | contact_damage | 0 | 1 | 0x8004686c |
| 0x64 | self+0x64 | 1 | 2 | 0x8004672c, 0x8004686c |
| 0x68 | terrain_box | 0 | 1 | 0x8004686c |
| 0x7c | work.7C | 1 | 0 | 0x8004672c |
| 0x88 | work.88 | 1 | 2 | 0x8004672c |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x80076254 |  | 0x8004672c |
| 0x80076270 |  | 0x8004672c |
| 0x80076278 |  | 0x8004672c |
