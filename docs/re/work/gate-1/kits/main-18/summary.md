# RE kit: main-18

- Object: main-18
- State: `gate-1_before-boss.state.gz` (SHA-1 `585948ebe43e9241ae0828aff8654b2e9b98ea04`), stage 16 area 0
- Class: main, type 18; entry function 0x800ee7a4
- Functions walked: 33; state tables: 9

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ee4ac, 0x800ee9d0, 0x800eea7c, 0x800eeb08, 0x800eebcc, 0x800eeca0, 0x800eee80, 0x800ef08c |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ee888, 0x800ee8c0, 0x800eea20, 0x800eea7c, 0x800eeb64, 0x800eec38, 0x800eedc8, 0x800eee00, 0x800eee80, 0x800eeed8, 0x800ef118 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800eefa8 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ef200 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ee7e0 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ee7e0 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ee610, 0x800ef118 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ee610, 0x800ef118 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ef118 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ee8c0, 0x800eeb64, 0x800eec38, 0x800eedc8, 0x800eee00, 0x800eeed8 |
| 0x8002d2d4 | GetRNG | random number | 0x800eeca0 |
| 0x8002d354 | AimAtPoint | AimAtPoint(obj, x, y): direction 0..31 from obj to the point | 0x800ee8c0, 0x800eeed8 |
| 0x8002d3b4 | SetSpeedDir | SetSpeedDir(obj, dir): speed = 1 px/frame in direction dir | 0x800ee8c0, 0x800eeed8 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800ef118 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ee610, 0x800ef118 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ee610, 0x800ef118 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ee610 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ee610 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ee610, 0x800ef118 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ee610 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fbc1c | 0x800ee7a4 | 0x800ee4ac, 0x800ee610, 0x800ee7e0, 0x800ee7e0 |
| 0x800fbc34 | 0x800ee610 | 0x80040290, 0x800ee834, 0x800ee840, 0x800ee994, 0x800eeacc, 0x800eeb90, 0x800eec64, 0x800eee38, 0x800eee00 |
| 0x800fbc58 | 0x800ee840 | 0x800ee888, 0x800ee8c0 |
| 0x800fbc60 | 0x800ee994 | 0x800ee9d0, 0x800eea20, 0x800eea7c |
| 0x800fbc6c | 0x800eeacc | 0x800eeb08, 0x800eeb64 |
| 0x800fbc74 | 0x800eeb90 | 0x800eebcc, 0x800eec38 |
| 0x800fbc7c | 0x800eec64 | 0x800eeca0, 0x800eed9c, 0x800eedc8 |
| 0x800fbc88 | 0x800eee38 | 0x800eee80, 0x800eeed8 |
| 0x800fbc98 | 0x800ef050 | 0x800ef08c, 0x800ef118, 0x800ef200, 0x800ef220 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 17 | 0x800ef050 |

## Animation tables

### Table 0x800fbbf8 (8 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 | 3 3 4 3 3 5 | 0 0 0 0 0 0 | 0 |
| 1 | 6 7 3 8 9 10 | 2 2 2 3 3 3 | 0 0 0 0 0 0 | hold |
| 2 | 11 12 11 13 | 2 2 2 2 | 0 0 0 0 | 0 |
| 3 | 14 15 16 17 18 19 | 5 2 2 5 3 3 | 0 0 0 0 0 0 | 0 |
| 4 | 20 | 1 | 0 | hold |
| 5 | 21 | 1 | 0 | hold |
| 6 | 22 | 1 | 0 | hold |
| 7 | 23 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800ee4ac |
| 0x2 | var | 3 | 0 | 0x800ee4ac, 0x800ee610, 0x800eeca0 |
| 0x4 | state | 4 | 2 | 0x800ee4ac, 0x800ee610, 0x800ee7a4, 0x800ee7e0 |
| 0x5 | step | 2 | 8 | 0x800ee4ac, 0x800ee610, 0x800ee834, 0x800ee8c0, 0x800eea7c, 0x800eeed8 |
| 0x6 | step2 | 15 | 14 | 0x800ee4ac, 0x800ee610, 0x800ee840, 0x800ee888, 0x800ee8c0, 0x800ee994, 0x800ee9d0, 0x800eea20, 0x800eea7c, 0x800eeacc, 0x800eeb08, 0x800eeb90, 0x800eebcc, 0x800eec64, 0x800eeca0, 0x800eed9c, 0x800eee38, 0x800eee80, 0x800eeed8 |
| 0x8 | x | 5 | 0 | 0x800ee4ac, 0x800ee610, 0x800ee8c0, 0x800eef78, 0x800eefa8 |
| 0xc | y | 3 | 0 | 0x800ee4ac, 0x800ee610, 0x800eefa8 |
| 0x14 | layer | 1 | 1 | 0x800ee4ac, 0x800eefa8 |
| 0x15 | flip | 4 | 2 | 0x800eeb08, 0x800eebcc, 0x800eeca0, 0x800eef78, 0x800eefa8 |
| 0x16 | priority | 0 | 1 | 0x800ee4ac |
| 0x18 | prev_x | 0 | 2 | 0x800ee4ac, 0x800ee610 |
| 0x1c | prev_y | 0 | 2 | 0x800ee4ac, 0x800ee610 |
| 0x20 | speed_x | 3 | 8 | 0x800ee4ac, 0x800ee8c0, 0x800ee9d0, 0x800eeb08, 0x800eebcc, 0x800eeca0, 0x800eee00 |
| 0x24 | speed_y | 1 | 6 | 0x800ee4ac, 0x800eeb08, 0x800eebcc, 0x800eeca0, 0x800eedc8, 0x800eee00 |
| 0x28 | accel_x | 0 | 2 | 0x800ee4ac, 0x800eeca0 |
| 0x2c | gravity | 0 | 1 | 0x800ee4ac |
| 0x30 | anime_table | 1 | 1 | 0x800ee4ac, 0x800eefa8 |
| 0x3c | sprite_data | 1 | 0 | 0x800eefa8 |
| 0x40 | tex_cord | 1 | 0 | 0x800eefa8 |
| 0x42 | clut_cord | 1 | 0 | 0x800eefa8 |
| 0x50 | attack_box | 0 | 5 | 0x800ee4ac, 0x800ee9d0, 0x800eebcc, 0x800eeca0, 0x800eee80 |
| 0x54 | hurt_box | 0 | 3 | 0x800ee4ac, 0x800ee610, 0x800ee9d0 |
| 0x58 | damage_table | 0 | 1 | 0x800ee4ac |
| 0x5c | hp | 0 | 1 | 0x800ee4ac |
| 0x60 | contact_damage | 0 | 1 | 0x800ee4ac |
| 0x61 | iframes | 0 | 1 | 0x800ee4ac |
| 0x67 | latch | 0 | 1 | 0x800ee4ac |
| 0x68 | terrain_box | 0 | 1 | 0x800ee4ac |
| 0x7c | work.7C | 4 | 6 | 0x800ee888, 0x800ee8c0, 0x800eeca0, 0x800eed9c, 0x800eee80, 0x800eeed8 |
| 0x80 | work.80 | 2 | 5 | 0x800ee4ac, 0x800ee9d0, 0x800eea20, 0x800eea7c |
| 0x84 | work.84 | 0 | 1 | 0x800ee4ac |
| 0x88 | work.88 | 3 | 7 | 0x800ee4ac, 0x800ee610, 0x800eea20, 0x800eebcc, 0x800eeca0, 0x800eee80 |
| 0x8c | work.8C | 0 | 1 | 0x800ee4ac |
| 0x90 | self+0x90 | 3 | 3 | 0x800ee4ac, 0x800eeed8 |
| 0x94 | self+0x94 | 1 | 2 | 0x800ee4ac, 0x800ee610, 0x800ee834 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800741fc | dmg_tbl_minor | 0x800ef08c |
| 0x800743dc | dmg_tbl_normal | 0x800ee4ac |
| 0x800970a0 | mega | 0x800ee8c0, 0x800eeed8 |
| 0x800970a8 | mega.x | 0x800eef78 |
| 0x800970b4 | mega.layer | 0x800ee4ac |
| 0x800ccedc | game.stage | 0x800ee7e0 |
| 0x800fbb70 |  | 0x800ee4ac, 0x800ee9d0, 0x800eebcc, 0x800eeca0, 0x800eee80 |
| 0x800fbb78 |  | 0x800ee4ac |
| 0x800fbb80 |  | 0x800eeb08, 0x800eebcc |
| 0x800fbb88 |  | 0x800eeca0 |
| 0x800fbbf8 |  | 0x800ee4ac |
| 0x800fbc2c |  | 0x800ee610 |
| 0x800fbc90 |  | 0x800ef08c |
| 0x800fbc94 |  | 0x800ef08c |
