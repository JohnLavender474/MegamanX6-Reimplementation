# RE kit: laser-reflector-puzzle-laser

- Object: laser-reflector-puzzle-laser
- State: `sheldon-16_death-guardian.state.gz` (SHA-1 `ebbb24e5d705fdc172d4e32b09c32d5148b3206d`), stage 7 area 0
- Class: misc, type 7; entry function 0x800ec970
- Functions walked: 8; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ed360 |
| 0x8002c5f0 | GetMiscObjectFrom | GetMiscObjectFrom(start, back): free slot of the 96 misc objects, searching forward (a1 = 0) or backward from start (0 = first) (likely) | 0x800ecee4 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ed304, 0x800ed360, 0x800ed460, 0x800ed4ac |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ebcd8, 0x800ed2b0, 0x800ed460 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fbfb0 | 0x800ec970 | 0x800ed2b0, 0x800ed304, 0x800ed360, 0x800ed460, 0x800ed4ac |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the export has no table (a sprite-only export).

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 1 | 0 | 0x800ecee4 |
| 0x2 | var | 1 | 0 | 0x800ed2b0 |
| 0x3 | display | 0 | 1 | 0x800ebcd8 |
| 0x4 | state | 3 | 2 | 0x800ec970, 0x800ed2b0, 0x800ed360 |
| 0x8 | x | 1 | 0 | 0x800ebcd8 |
| 0xa | x.px | 8 | 5 | 0x800ebcd8, 0x800ecee4 |
| 0xc | y | 1 | 0 | 0x800ebcd8 |
| 0xe | y.px | 8 | 5 | 0x800ebcd8, 0x800ecee4 |
| 0x14 | layer | 1 | 0 | 0x800ecee4 |
| 0x15 | flip | 0 | 1 | 0x800ebcd8 |
| 0x16 | priority | 0 | 1 | 0x800ed360 |
| 0x30 | anime_table | 1 | 0 | 0x800ecee4 |
| 0x3c | sprite_data | 1 | 1 | 0x800ecee4, 0x800ed360 |
| 0x40 | tex_cord | 1 | 1 | 0x800ecee4, 0x800ed360 |
| 0x42 | clut_cord | 1 | 1 | 0x800ecee4, 0x800ed360 |
| 0x47 | anime.sprite | 0 | 3 | 0x800ebcd8, 0x800ed360 |
| 0x50 | attack_box | 1 | 0 | 0x800ecee4 |
| 0x54 | hurt_box | 6 | 0 | 0x800ebcd8, 0x800ecee4 |
| 0x56 | self+0x56 | 2 | 0 | 0x800ebcd8, 0x800ecee4 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ed360 |
| 0x80095098 | bank_tpage | 0x800ed360 |
| 0x8009716c | mega.grab+0x2 | 0x800ec970 |
| 0x800cceed | game.checkpoint | 0x800ed360, 0x800ed460 |
| 0x800e9914 |  | 0x800ebcd8 |
| 0x800e9934 |  | 0x800ebcd8 |
| 0x800e9954 |  | 0x800ebcd8 |
| 0x800e9974 |  | 0x800ebcd8 |
| 0x800fbc80 |  | 0x800ebcd8 |
| 0x800fbc90 |  | 0x800ecee4 |
