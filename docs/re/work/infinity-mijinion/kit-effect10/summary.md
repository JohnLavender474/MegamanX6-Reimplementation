# RE kit: infinity-mijinion

- Object: infinity-mijinion
- State: `mijinion-3_boss-fight.state.gz` (SHA-1 `fc50684428d3f921983d2730df60915ac9755ac2`), stage 8 area 0
- Class: effect, type 10; entry function 0x800fb020
- Functions walked: 14; state tables: 4

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800f7670, 0x800f79d0 |
| 0x800179d0 | SetAnimeFrame | SetAnimeFrame(obj, n, frame): start animation n of obj+0x30 at frame `frame` | 0x800fae34 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800facf8 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800fad90 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800fafd0 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800f7670, 0x800f7be4, 0x800facf8 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800f77a8, 0x800fafd0 |
| 0x8002d2d4 | GetRNG | random number | 0x800f77a8, 0x800f79d0, 0x800fae34 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800f77a8, 0x800f79d0, 0x800fae34 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800f77a8, 0x800f79d0, 0x800fae34 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800f77a8 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x8002c6f0 | 0x800fa418 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fd4dc | 0x800fb020 | 0x800f7670, 0x800facf8, 0x800fad90 |
| 0x800fd4e8 | 0x800facf8 | 0x800fadb0, 0x800faf40, 0x800faf94 |
| 0x800fd4f4 | 0x800fadb0 | 0x800fae34, 0x800f77a8, 0x800faf0c |
| 0x800fd500 | 0x800faf94 | 0x800f79d0, 0x800fafd0 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 2 | 2 | 0x800f7670 |
| 0x2 | var | 2 | 0 | 0x800f7670 |
| 0x3 | display | 0 | 12 | 0x800f7670, 0x800f7be4, 0x800facf8, 0x800fadb0, 0x800faf0c, 0x800faf40, 0x800fafd0 |
| 0x4 | state | 6 | 6 | 0x800f7670, 0x800facf8, 0x800faf0c, 0x800faf40, 0x800fafd0, 0x800fb020 |
| 0x5 | step | 1 | 3 | 0x800f7670, 0x800facf8 |
| 0x6 | step2 | 5 | 4 | 0x800f7670, 0x800f77a8, 0x800f79d0, 0x800fadb0, 0x800fae34, 0x800faf94 |
| 0x8 | x | 1 | 0 | 0x800f7be4 |
| 0xa | x.px | 4 | 4 | 0x800f77a8, 0x800f7be4, 0x800fa418, 0x800fae34, 0x800faf40 |
| 0xe | y.px | 4 | 4 | 0x800f77a8, 0x800f7be4, 0x800fa418, 0x800fae34, 0x800faf40 |
| 0x14 | layer | 1 | 0 | 0x800fa418 |
| 0x15 | flip | 4 | 6 | 0x800f77a8, 0x800f79d0, 0x800f7be4, 0x800fa418 |
| 0x16 | priority | 0 | 2 | 0x800f7670 |
| 0x20 | speed_x | 1 | 6 | 0x800f77a8, 0x800f79d0 |
| 0x24 | speed_y | 0 | 3 | 0x800f77a8, 0x800f79d0 |
| 0x30 | anime_table | 1 | 0 | 0x800fa418 |
| 0x3c | sprite_data | 1 | 0 | 0x800fa418 |
| 0x40 | tex_cord | 1 | 0 | 0x800fa418 |
| 0x42 | clut_cord | 1 | 0 | 0x800fa418 |
| 0x45 | anime.flag | 1 | 0 | 0x800faf40 |
| 0x47 | anime.sprite | 0 | 1 | 0x800f7be4 |
| 0x50 | attack_box | 5 | 0 | 0x800f77a8, 0x800facf8, 0x800fae34, 0x800faf0c, 0x800faf40 |
| 0x54 | hurt_box | 3 | 3 | 0x800f7670, 0x800fadb0 |
| 0x56 | self+0x56 | 1 | 3 | 0x800f77a8, 0x800fae34 |
| 0x58 | damage_table | 1 | 2 | 0x800f77a8, 0x800fae34 |
| 0x5a | self+0x5a | 11 | 1 | 0x800f79d0, 0x800fae34 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x8009716c | mega.grab+0x2 | 0x800fb020 |
