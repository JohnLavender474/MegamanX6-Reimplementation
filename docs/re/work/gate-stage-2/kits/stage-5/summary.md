# RE kit: stage type 5

- Object: stage type 5
- State: `gate-3_stage-2.state.gz` (SHA-1 `992d5d0609e736bba918340805dd9c642d889217`), stage 17 area 0
- Class: stage, type 5; entry function 0x800e9b8c
- Functions walked: 7; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800e99b8, 0x800e9d4c |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800e9bd8 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800e99b8 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800e9c84 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800e9bd8 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800e9cac, 0x800e9d4c |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800e9bd8 |
| 0x80031f4c | SolidMainObjects | SolidMainObjects(obj): obj solid for the 48 main objects that have a terrain box: push out (0x800323D4), carry the ones on top (0x80032D24, touch bit per object in obj+0x98), put back a main object squeezed into the terrain (floor+ceiling or both walls) | 0x800e9bd8 |
| 0x8003208c | SolidPushShots | SolidPush of obj against the visual objects and the weapon objects that have +0x9D set; keeps a touch bit per object in obj+0x90 (guess) | 0x800e9bd8 |
| 0x80032b34 | SolidWeaponHit | weapon objects whose terrain box overlaps obj's terrain box: 0x800312DC, then ApplyWeaponHit; returns the first non-zero result; no no_hit or one-hit test, no kill count | 0x800e9bd8 |
| 0x80032be8 | SolidVisualHit | SolidVisualHit(obj): each live visual object whose box touches obj (WeaponBoxTest with obj+0x80/+0x82) gets collide_flags |= 0xF; returns the type of the last one | 0x800e9bd8 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f8ab4 | 0x800e9b8c | 0x800e99b8, 0x800e9bd8, 0x800e9c84 |
| 0x800f8ac0 | 0x800e9bd8 | 0x800e9ca4, 0x800e9d4c, 0x800e9cac |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table found.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800e99b8 |
| 0x2 | var | 1 | 0 | 0x800e99b8 |
| 0x3 | display | 0 | 1 | 0x800e9c84 |
| 0x4 | state | 2 | 1 | 0x800e99b8, 0x800e9b8c |
| 0x5 | step | 0 | 1 | 0x800e99b8 |
| 0x6 | step2 | 1 | 2 | 0x800e99b8, 0x800e9bd8 |
| 0x8 | x | 2 | 0 | 0x800e99b8, 0x800e9b8c |
| 0xa | x.px | 3 | 0 | 0x800e99b8, 0x800e9cac |
| 0xc | y | 1 | 0 | 0x800e9b8c |
| 0xe | y.px | 3 | 0 | 0x800e99b8, 0x800e9d4c |
| 0x14 | layer | 0 | 1 | 0x800e99b8 |
| 0x15 | flip | 0 | 1 | 0x800e99b8 |
| 0x16 | priority | 0 | 1 | 0x800e99b8 |
| 0x18 | prev_x | 0 | 1 | 0x800e9b8c |
| 0x1c | prev_y | 0 | 1 | 0x800e9b8c |
| 0x20 | speed_x | 2 | 6 | 0x800e99b8, 0x800e9cac, 0x800e9d4c |
| 0x24 | speed_y | 2 | 4 | 0x800e99b8, 0x800e9d4c |
| 0x28 | accel_x | 0 | 1 | 0x800e9d4c |
| 0x30 | anime_table | 0 | 1 | 0x800e99b8 |
| 0x3c | sprite_data | 0 | 1 | 0x800e99b8 |
| 0x40 | tex_cord | 0 | 1 | 0x800e99b8 |
| 0x42 | clut_cord | 0 | 1 | 0x800e99b8 |
| 0x54 | hurt_box | 0 | 1 | 0x800e99b8 |
| 0x58 | damage_table | 0 | 1 | 0x800e99b8 |
| 0x60 | contact_damage | 0 | 1 | 0x800e99b8 |
| 0x61 | iframes | 0 | 1 | 0x800e99b8 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800e99b8 |
| 0x68 | terrain_box | 0 | 1 | 0x800e99b8 |
| 0x75 | self+0x75 | 0 | 1 | 0x800e99b8 |
| 0x76 | self+0x76 | 0 | 1 | 0x800e99b8 |
| 0x7c | work.7C | 2 | 1 | 0x800e99b8, 0x800e9cac |
| 0x7e | work.7E | 2 | 1 | 0x800e99b8, 0x800e9d4c |
| 0x80 | work.80 | 2 | 2 | 0x800e9cac, 0x800e9d4c |
| 0x84 | work.84 | 2 | 3 | 0x800e99b8, 0x800e9cac, 0x800e9d4c |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800e99b8 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800e99b8 |
| 0x80075b9c | dmg_tbl_capsule | 0x800e99b8 |
| 0x80095098 | bank_tpage | 0x800e99b8 |
| 0x800970a0 | mega | 0x800e99b8, 0x800e9bd8 |
| 0x800f8aa4 |  | 0x800e99b8 |
| 0x800f8aac |  | 0x800e99b8 |
| 0x800f8ab0 |  | 0x800e99b8 |
