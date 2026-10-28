# RE kit: wolfang-stage7

- Object: wolfang-stage7
- State: `wolfang-5_ice-pit.state.gz` (SHA-1 `0b1d405ebeb62ae58a007d761d5f6fe2f8f8cf39`), stage 2 area 0
- Class: stage, type 7; entry function 0x800ecde8
- Functions walked: 21; state tables: 3

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ec628, 0x800ecc10, 0x800ed238 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ebaa0, 0x800ec4dc, 0x800ed238 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ec4dc, 0x800ed2f4 |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800ec4dc |
| 0x8002943c | Quake2 | screen shake variant: camera work +0x36/+0x45/+0x3E/+0x3C/+0x3A, flag 0x10 | 0x800ece88 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ebaa0 |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x800ebaa0 |
| 0x8002bfd0 | GetStageObjectAfter | GetStageObjectAfter(obj, 0): first free stage-object slot after obj (from 0x800C1220 when obj = 0), cleared; the totem base makes its heads with it | 0x800ebaa0, 0x800ec7dc |
| 0x8002c4c4 | GetStageObject | first free of the 80 stage-object slots at 0x800C1220 | 0x800ecc10 |
| 0x8002c530 | GetMiscObject | free misc object (debris) | 0x800ecf20, 0x800ed08c |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ebf18, 0x800ece48, 0x800ece68, 0x800ed384 |
| 0x8002cab4 | OffScreenCheck | 1 if outside [-64, 384) x [-64, 304) of the layer camera | 0x800ed2f4 |
| 0x8002cb50 | DisplayCheck1 | display = 1 if inside [-32, W+32) x [-32, 272) | 0x800ebf18 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ebf18 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ed238, 0x800ed2f4 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ec1dc, 0x800ec628, 0x800ece88 |
| 0x8002d2d4 | GetRNG | random number | 0x800ebaa0, 0x800ec7dc |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ebf18, 0x800ec1dc, 0x800ec628, 0x800ecc10 |
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800ec628 |
| 0x80030ccc | TerrainPointCheck | TerrainPointCheck(obj, x, y): non-zero when the point is solid (rectangle list first, then 0x80030234) | 0x800ebaa0, 0x800ec4dc, 0x800ecfe8 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800ebf18 |
| 0x8003208c | SolidPushShots | SolidPush of obj against the visual objects and the weapon objects that have +0x9D set; keeps a touch bit per object in obj+0x90 (guess) | 0x800ebf18 |
| 0x80032b34 | SolidWeaponHit | weapon objects whose terrain box overlaps obj's terrain box: 0x800312DC, then ApplyWeaponHit; returns the first non-zero result; no no_hit or one-hit test, no kill count | 0x800ebf18 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ebf18 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x80031f4c | 0x800ebf18 |
| 0x80032be8 | 0x800ebf18 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6b9c | 0x800ecde8 | 0x800ebaa0, 0x800ebf18, 0x800ece48, 0x800ecc10, 0x800ece68 |
| 0x800f6bd8 | 0x800ebf18 | 0x800ec1dc, 0x800ece88, 0x800ecf20, 0x800ec4dc, 0x800ecfe8, 0x800ed044, 0x800ec628, 0x800ed08c, 0x800ed150, 0x800ec7dc |
| 0x800f6c1c | 0x800ed1ec | 0x800ed238, 0x800ed2f4, 0x800ed384 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e98c0 | 0x800ebc68 | 0x800ebaa0 | 13 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| misc | 9 | 0x800ed1ec |

## Animation tables

### Table 0x800f6b6c (8 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 | 1 | 0 | hold |
| 1 | 4 | 1 | 0 | hold |
| 2 | 15 16 0 16 0 16 15 15 | 2 2 3 2 3 2 1 1 | 0 0 0 0 0 0 0 1 | hold |
| 3 | 17 18 4 18 4 18 17 17 | 2 2 3 2 2 3 1 1 | 0 0 0 0 0 0 0 1 | hold |
| 4 | 5 6 7 8 9 10 11 12 13 14 | 1 1 1 1 1 1 2 2 2 1 | 0 0 0 0 0 0 0 0 0 1 | hold |
| 5 | 1 | 1 | 0 | hold |
| 6 | 2 | 1 | 0 | hold |
| 7 | 3 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800ebaa0 |
| 0x2 | var | 7 | 0 | 0x800ebaa0, 0x800ebf18, 0x800ec4dc, 0x800ecc10 |
| 0x3 | display | 4 | 6 | 0x800ebaa0, 0x800ebf18, 0x800ec628, 0x800ecc10, 0x800ece88, 0x800ecfe8 |
| 0x4 | state | 1 | 8 | 0x800ebaa0, 0x800ebf18, 0x800ec4dc, 0x800ec7dc, 0x800ecc10, 0x800ecde8, 0x800ecfe8 |
| 0x5 | step | 0 | 1 | 0x800ebaa0 |
| 0x6 | step2 | 3 | 13 | 0x800ebaa0, 0x800ebf18, 0x800ec1dc, 0x800ec628, 0x800ece88, 0x800ecf20, 0x800ecfe8, 0x800ed044, 0x800ed08c, 0x800ed150 |
| 0x7 | act4 | 6 | 7 | 0x800ebaa0, 0x800ebf18, 0x800ec7dc, 0x800ecc10 |
| 0x8 | x | 1 | 3 | 0x800ec1dc, 0x800ec628, 0x800ecde8, 0x800ece88 |
| 0xa | x.px | 29 | 2 | 0x800ebaa0, 0x800ebf18, 0x800ec1dc, 0x800ec4dc, 0x800ec628, 0x800ec7dc, 0x800ecc10, 0x800ecf20, 0x800ecfe8, 0x800ed08c, 0x800ed150 |
| 0xc | y | 1 | 3 | 0x800ec628, 0x800ecde8, 0x800ece88 |
| 0xe | y.px | 32 | 2 | 0x800ebaa0, 0x800ec1dc, 0x800ec4dc, 0x800ec628, 0x800ecc10, 0x800ece88, 0x800ecf20, 0x800ecfe8, 0x800ed08c |
| 0x14 | layer | 2 | 1 | 0x800ebaa0, 0x800ecf20, 0x800ed08c |
| 0x15 | flip | 2 | 1 | 0x800ebaa0, 0x800ecf20, 0x800ed08c |
| 0x16 | priority | 0 | 3 | 0x800ebaa0, 0x800ecf20, 0x800ed08c |
| 0x18 | prev_x | 0 | 1 | 0x800ecde8 |
| 0x1c | prev_y | 1 | 1 | 0x800ec628, 0x800ecde8 |
| 0x20 | speed_x | 1 | 5 | 0x800ebaa0, 0x800ec1dc, 0x800ec628, 0x800ece88 |
| 0x24 | speed_y | 1 | 6 | 0x800ebaa0, 0x800ec1dc, 0x800ec628, 0x800ece88 |
| 0x30 | anime_table | 5 | 1 | 0x800ebaa0, 0x800ec7dc, 0x800ecf20, 0x800ed08c |
| 0x3c | sprite_data | 5 | 1 | 0x800ebaa0, 0x800ec7dc, 0x800ecf20, 0x800ed08c |
| 0x40 | tex_cord | 5 | 1 | 0x800ebaa0, 0x800ec7dc, 0x800ecf20, 0x800ed08c |
| 0x42 | clut_cord | 6 | 2 | 0x800ebaa0, 0x800ebf18, 0x800ec7dc, 0x800ecf20, 0x800ed08c |
| 0x45 | anime.flag | 1 | 0 | 0x800ec4dc |
| 0x50 | attack_box | 0 | 1 | 0x800ebaa0 |
| 0x54 | hurt_box | 0 | 4 | 0x800ebaa0, 0x800ed044 |
| 0x58 | damage_table | 0 | 4 | 0x800ebaa0, 0x800ecf20, 0x800ed044, 0x800ed08c |
| 0x5c | hp | 0 | 2 | 0x800ebaa0, 0x800ecfe8 |
| 0x60 | contact_damage | 0 | 4 | 0x800ebaa0, 0x800ec628 |
| 0x61 | iframes | 0 | 2 | 0x800ebaa0 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800ebaa0 |
| 0x68 | terrain_box | 0 | 6 | 0x800ebaa0, 0x800ecc10, 0x800ed044 |
| 0x70 | collide_flags | 7 | 0 | 0x800ec1dc, 0x800ec628, 0x800ecc10 |
| 0x72 | self+0x72 | 2 | 0 | 0x800ebf18 |
| 0x75 | self+0x75 | 0 | 1 | 0x800ebaa0 |
| 0x7c | work.7C | 3 | 0 | 0x800ebf18, 0x800ecc10, 0x800ece88 |
| 0x80 | work.80 | 8 | 3 | 0x800ebaa0, 0x800ec1dc, 0x800ec628, 0x800ed044 |
| 0x81 | self+0x81 | 4 | 0 | 0x800ec1dc, 0x800ec628 |
| 0x82 | self+0x82 | 8 | 0 | 0x800ec1dc, 0x800ec628 |
| 0x83 | self+0x83 | 4 | 3 | 0x800ebaa0, 0x800ec1dc, 0x800ec628, 0x800ed044 |
| 0x84 | work.84 | 4 | 5 | 0x800ebaa0, 0x800ec4dc, 0x800ec7dc, 0x800ecf20, 0x800ed08c |
| 0x85 | self+0x85 | 0 | 1 | 0x800ebaa0 |
| 0xa2 | self+0xa2 | 0 | 1 | 0x800ebaa0 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ebaa0 |
| 0x80075b9c | dmg_tbl_capsule | 0x800ebaa0, 0x800ecf20, 0x800ed044, 0x800ed08c |
| 0x80095098 | bank_tpage | 0x800ebaa0 |
| 0x800970a0 | mega | 0x800ebf18, 0x800ed150 |
| 0x800970b4 | mega.layer | 0x800ebaa0 |
| 0x80097180 | mega.special | 0x800ecde8 |
| 0x800971f8 | camera_layer0 | 0x800ec7dc, 0x800ed150 |
| 0x80097244 | cam.redraw | 0x800ec4dc |
| 0x800c1220 | stage_objects | 0x800eb96c |
| 0x800ccf38 | game.difficulty | 0x800ec7dc |
| 0x800f6b6c | wolfloid_states+0xe8 | 0x800ebaa0 |
| 0x800f6b8c |  | 0x800ebf18 |
| 0x800f6b90 |  | 0x800ebaa0, 0x800ed044 |
| 0x800f6b94 |  | 0x800ebaa0, 0x800ed044 |
| 0x800f6b98 |  | 0x800ebaa0 |
| 0x800f6bb0 |  | 0x800ebaa0 |
| 0x800f6c00 |  | 0x800ec4dc |
| 0x800f6c08 |  | 0x800ec4dc |
| 0x800f6c10 |  | 0x800ecc10 |
| 0x800f76ec |  | 0x800ebf18 |
