# RE kit: nightmare-bug

- Object: nightmare-bug
- State: `sheldon-1_lemons.state.gz` (SHA-1 `068a884c44bee4b9b21e0428f1e0c0f12b10cabc`), stage 7 area 0
- Class: main, type 22; entry function 0x800f42ac
- Functions walked: 7; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800f3f54 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800f40fc, 0x800f4468 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800f3f54, 0x800f43f4, 0x800f4414 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800f42f8 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800f42f8 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800f42f8 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800f40fc, 0x800f4468 |
| 0x8002d2d4 | GetRNG | random number | 0x800f3f54 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800f42f8 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800f42f8 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800f42f8 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800f42f8 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800f42f8 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800f40fc |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800f40fc |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800f40fc |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fc718 | 0x800f42ac | 0x800f3f54, 0x800f42f8, 0x800f43f4, 0x800f4414 |
| 0x800fc728 | 0x800f42f8 | 0x80040290, 0x800f40fc, 0x800f4468 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800fc714 (1 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 | 2 2 2 2 2 2 | 0 0 0 0 0 0 | 0 |

Note: cut to the export's 1 animations; 4 more pointer-like words follow (often the next table)

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x3 | display | 1 | 1 | 0x800f4414 |
| 0x4 | state | 1 | 1 | 0x800f42ac, 0x800f42f8 |
| 0x5 | step | 1 | 1 | 0x800f42f8 |
| 0x8 | x | 1 | 0 | 0x800f42ac |
| 0xa | x.px | 3 | 0 | 0x800f40fc |
| 0xc | y | 1 | 0 | 0x800f42ac |
| 0xe | y.px | 3 | 1 | 0x800f40fc |
| 0x15 | flip | 0 | 2 | 0x800f40fc |
| 0x18 | prev_x | 0 | 1 | 0x800f42ac |
| 0x1c | prev_y | 0 | 1 | 0x800f42ac |
| 0x20 | speed_x | 0 | 3 | 0x800f40fc, 0x800f4468 |
| 0x24 | speed_y | 0 | 3 | 0x800f40fc, 0x800f4468 |
| 0x7c | work.7C | 1 | 1 | 0x800f4414 |
| 0x80 | work.80 | 1 | 0 | 0x800f40fc |
| 0x82 | self+0x82 | 1 | 0 | 0x800f40fc |
| 0x84 | work.84 | 1 | 1 | 0x800f40fc |
| 0x85 | self+0x85 | 1 | 1 | 0x800f42f8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800746fc |  | 0x800f3f54 |
| 0x800970a0 | mega | 0x800f40fc |
| 0x800970b4 | mega.layer | 0x800f3f54 |
| 0x80097420 | frame_count | 0x800f40fc |
| 0x800cd30a | nightmare_effect | 0x800f3f54 |
| 0x800e9df0 |  | 0x800f3f54 |
| 0x800e9e00 |  | 0x800f3f54 |
| 0x800fc6f4 |  | 0x800f3f54 |
| 0x800fc6f8 |  | 0x800f3f54 |
| 0x800fc714 |  | 0x800f3f54 |
