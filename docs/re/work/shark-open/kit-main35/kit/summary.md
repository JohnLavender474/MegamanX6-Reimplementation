# RE kit: shark-open/kit-main35

- Object: shark-open/kit-main35
- State: `shark-2_section-2.state.gz` (SHA-1 `2ed5581179aa6acd6cf436aac85487b11ea49bd0`), stage 4 area 1
- Class: main, type 35; entry function 0x800f04a4
- Functions walked: 7; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800f036c |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800f036c |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800f04f0 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800f0510 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800f036c |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800f036c |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800f04a4 |
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800f0994, 0x800f0cf4 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800f036c |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800f036c |
| 0x800f01bc | NightmareBlock2Setup | overlay 07 only, set-up of main type 35 of stage 4 area 1 (state table 0x800F8DE0): DeleteObject3 unless nightmare_effect = 2, else HP 2. Sprites: a blue ice-like block (32 x 62). Descriptive name; no official name known | 0x800f04a4 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f8de0 | 0x800f04a4 | 0x800f01bc, 0x800f036c, 0x800f04f0, 0x800f0510, 0x800f10a8, 0x800f0994, 0x800f0cf4 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e99a8 | 0x800f0b44 | 0x800f0994 | 49 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f8dc8 (4 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 | 1 | 0 | hold |
| 1 | 1 | 1 | 0 | hold |
| 2 | 2 | 1 | 0 | hold |
| 3 | 0 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 1 | 1 | 0x800f036c |
| 0x2 | var | 2 | 0 | 0x800f036c |
| 0x3 | display | 0 | 1 | 0x800f036c |
| 0x4 | state | 3 | 9 | 0x800f036c, 0x800f04a4, 0x800f0994, 0x800f0cf4, 0x800f10a8 |
| 0x8 | x | 4 | 2 | 0x800f036c, 0x800f0cf4 |
| 0xa | x.px | 5 | 1 | 0x800f036c, 0x800f0cf4 |
| 0xc | y | 1 | 0 | 0x800f036c |
| 0xe | y.px | 2 | 0 | 0x800f036c |
| 0x14 | layer | 6 | 11 | 0x800f0994, 0x800f0cf4, 0x800f10a8 |
| 0x18 | prev_x | 3 | 6 | 0x800f036c, 0x800f0994, 0x800f0cf4, 0x800f10a8 |
| 0x1c | prev_y | 4 | 2 | 0x800f036c, 0x800f0994, 0x800f10a8 |
| 0x1d | self+0x1d | 4 | 4 | 0x800f0994, 0x800f0cf4 |
| 0x1e | self+0x1e | 5 | 2 | 0x800f0994, 0x800f10a8 |
| 0x1f | self+0x1f | 1 | 3 | 0x800f0994, 0x800f0cf4 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800970a0 | mega | 0x800f0994, 0x800f0cf4 |
| 0x80097244 | cam.redraw | 0x800f036c |
| 0x800f8dd8 |  | 0x800f036c |
