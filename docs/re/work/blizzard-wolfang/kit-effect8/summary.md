# RE kit: wolfang-effect8

- Object: wolfang-effect8
- State: `wolfang-7_boss-fight.state.gz` (SHA-1 `93b62b93d2f85faffec3f8fbde1fe8bec5ee1379`), stage 2 area 0
- Class: effect, type 8; entry function 0x800f599c
- Functions walked: 4; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800f5788 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800f5888 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800f5920 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800f5888 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f76dc | 0x800f599c | 0x800f5788, 0x800f5888, 0x800f5920 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 1 | 0 | 0x800f5788 |
| 0x3 | display | 0 | 2 | 0x800f5788, 0x800f5888 |
| 0x4 | state | 2 | 2 | 0x800f5788, 0x800f5888, 0x800f599c |
| 0x5 | step | 1 | 1 | 0x800f5788, 0x800f5888 |
| 0x6 | step2 | 0 | 1 | 0x800f5788 |
| 0xa | x.px | 6 | 2 | 0x800f5788 |
| 0xe | y.px | 2 | 1 | 0x800f5788 |
| 0x15 | flip | 3 | 0 | 0x800f5788 |
| 0x16 | priority | 0 | 1 | 0x800f5788 |
| 0x50 | attack_box | 1 | 0 | 0x800f5888 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800f76e8 |  | 0x800f5888 |
