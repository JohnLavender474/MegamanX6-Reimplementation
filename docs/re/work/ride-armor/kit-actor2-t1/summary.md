# RE kit: actor2 type 1

- Object: actor2 type 1
- State: `shark-1_ride.state.gz` (SHA-1 `fcf098dcbeb64cf58e6131364bb12e806937d130`), stage 4 area 0
- Class: actor2, type 0; entry function 0x800f01a4
- Functions walked: 1; state tables: 0

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800f01a4 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800f01a4 |

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

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|

## Globals and data

| address | name | used by |
|---|---|---|
