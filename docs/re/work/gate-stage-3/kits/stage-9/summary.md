# RE kit: stage type 9

- Object: stage type 9
- State: `gate-4_before-gate.state.gz` (SHA-1 `f51fcc1d7dd4d15662dc4a3ecd18beb124cd7419`), stage 18 area 0
- Class: stage, type 9; entry function 0x800ecaf0
- Functions walked: 4; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ec4d0 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ecb3c |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ec688 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800ec688 |
| 0x80031f4c | SolidMainObjects | SolidMainObjects(obj): obj solid for the 48 main objects that have a terrain box: push out (0x800323D4), carry the ones on top (0x80032D24, touch bit per object in obj+0x98), put back a main object squeezed into the terrain (floor+ceiling or both walls) | 0x800ec688 |
| 0x8003208c | SolidPushShots | SolidPush of obj against the visual objects and the weapon objects that have +0x9D set; keeps a touch bit per object in obj+0x90 (guess) | 0x800ec688 |
| 0x80032b34 | SolidWeaponHit | weapon objects whose terrain box overlaps obj's terrain box: 0x800312DC, then ApplyWeaponHit; returns the first non-zero result; no no_hit or one-hit test, no kill count | 0x800ec688 |
| 0x80032be8 | SolidVisualHit | SolidVisualHit(obj): each live visual object whose box touches obj (WeaponBoxTest with obj+0x80/+0x82) gets collide_flags |= 0xF; returns the type of the last one | 0x800ec688 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f69a4 | 0x800ecaf0 | 0x800ec4d0, 0x800ec688, 0x800ecb3c |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f6948 (2 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 6 6 | 2 2 2 2 2 2 5 1 | 0 0 0 0 0 0 0 0 | 0 |
| 1 | 0 1 12 7 8 13 10 11 6 6 | 2 2 2 2 2 2 2 2 1 1 | 0 0 0 0 0 0 0 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800ec4d0 |
| 0x2 | var | 2 | 0 | 0x800ec4d0, 0x800ec688 |
| 0x3 | display | 1 | 2 | 0x800ec688 |
| 0x4 | state | 2 | 1 | 0x800ec4d0, 0x800ecaf0 |
| 0x5 | step | 0 | 1 | 0x800ec4d0 |
| 0x6 | step2 | 1 | 1 | 0x800ec4d0, 0x800ec688 |
| 0x8 | x | 1 | 0 | 0x800ecaf0 |
| 0xa | x.px | 1 | 0 | 0x800ec688 |
| 0xc | y | 1 | 0 | 0x800ecaf0 |
| 0xe | y.px | 2 | 0 | 0x800ec4d0, 0x800ec688 |
| 0x15 | flip | 0 | 1 | 0x800ec4d0 |
| 0x18 | prev_x | 0 | 1 | 0x800ecaf0 |
| 0x1c | prev_y | 0 | 1 | 0x800ecaf0 |
| 0x30 | anime_table | 0 | 1 | 0x800ec4d0 |
| 0x50 | attack_box | 0 | 1 | 0x800ec4d0 |
| 0x54 | hurt_box | 0 | 2 | 0x800ec4d0 |
| 0x58 | damage_table | 0 | 1 | 0x800ec4d0 |
| 0x60 | contact_damage | 0 | 1 | 0x800ec4d0 |
| 0x61 | iframes | 0 | 1 | 0x800ec4d0 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800ec4d0 |
| 0x68 | terrain_box | 0 | 2 | 0x800ec4d0 |
| 0x72 | self+0x72 | 4 | 0 | 0x800ec688 |
| 0x75 | self+0x75 | 0 | 1 | 0x800ec4d0 |
| 0x76 | self+0x76 | 4 | 1 | 0x800ec4d0, 0x800ec688 |
| 0x7c | work.7C | 0 | 4 | 0x800ec4d0 |
| 0x7d | self+0x7d | 0 | 2 | 0x800ec4d0 |
| 0x7e | work.7E | 0 | 2 | 0x800ec4d0 |
| 0x7f | self+0x7f | 0 | 4 | 0x800ec4d0 |
| 0x80 | work.80 | 0 | 2 | 0x800ec4d0 |
| 0x81 | self+0x81 | 0 | 2 | 0x800ec4d0 |
| 0x82 | self+0x82 | 0 | 2 | 0x800ec4d0 |
| 0x83 | self+0x83 | 0 | 2 | 0x800ec4d0 |
| 0x86 | self+0x86 | 0 | 1 | 0x800ec4d0 |
| 0x88 | work.88 | 0 | 1 | 0x800ec4d0 |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800ec4d0 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x80075b9c | dmg_tbl_capsule | 0x800ec4d0 |
| 0x800970a0 | mega | 0x800ec688 |
| 0x800970a5 | mega.var+0x3 | 0x800ec4d0 |
| 0x800cced0 | game | 0x800ec688 |
| 0x800f6948 |  | 0x800ec4d0 |
| 0x800f6950 |  | 0x800ec4d0 |
| 0x800f6978 |  | 0x800ec4d0 |
| 0x800f697c |  | 0x800ec4d0 |
| 0x800f69b0 |  | 0x800ec688 |
| 0x800f778c |  | 0x800ec4d0 |
| 0x800f7790 |  | 0x800ec4d0 |
