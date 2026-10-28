# RE kit: sheldon-lasers

- Object: sheldon-lasers
- State: `sheldon-7_lasers.state.gz` (SHA-1 `9839227b25dbb23eaef623c7fdb4423cd6d8dbe9`), stage 7 area 0
- Class: main, type 13; entry function 0x800ec8f8
- Functions walked: 46; state tables: 11

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800ea4cc, 0x800eabd8, 0x800eada8, 0x800eca5c |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ea13c, 0x800ea4cc, 0x800ecc74 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ea4cc, 0x800ea650, 0x800ec9b8 |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800ea650, 0x800ea7c8 |
| 0x80029f38 | SpawnInRect | SpawnInRect(x0, x1, y0, y1, flag): spawn the records inside a rectangle | 0x800ea13c |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ed360 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800eabd8, 0x800eada8 |
| 0x8002c530 | GetMiscObject | free misc object (debris) | 0x800eb0f0, 0x800eb734 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ecb00, 0x800ed290, 0x800ed304, 0x800ed360, 0x800ed460, 0x800ed4ac |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ea4cc, 0x800ebcd8, 0x800ec9b8, 0x800eca5c, 0x800ed2b0, 0x800ed460 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800eb0f0 |
| 0x8002d380 | AimAt | AimAt(a, b): direction 0..31 from a to b (0 right, 8 up, 16 left, 24 down) | 0x800ed0ac |
| 0x8002d654 | BoxOverlap | box test between two objects, facing-aware | 0x800eaa00 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800eb0f0 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800ea4cc |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ea4cc, 0x800ec9b8, 0x800eca5c |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x8002c32c | 0x800eaf28 |
| 0x8002c5f0 | 0x800ecee4 |
| 0x80030234 | 0x800ed014 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fbf20 | 0x800ec8f8 | 0x800ea13c, 0x800ec9b8, 0x800ea4cc, 0x800eca5c, 0x800ecb00 |
| 0x800fbf34 | 0x800ec9b8 | 0x80040290, 0x800ed288, 0x800ecdec, 0x800ecb3c, 0x800ecc38 |
| 0x800fbf48 | 0x800ea4cc | 0x80040290, 0x800ed288, 0x800ecdf4 |
| 0x800fbf54 | 0x800eca5c | 0x80040290, 0x800ed288, 0x800ecdec, 0x800ece54, 0x800ecea0 |
| 0x800fbf68 | 0x800ecb3c | 0x800ecb78, 0x800ecb90, 0x800ecbe4, 0x800ecc10 |
| 0x800fbf78 | 0x800ecc38 | 0x800ecc74, 0x800ecd40, 0x800ecdb0 |
| 0x800fbf84 | 0x800ece54 | 0x800ea650, 0x800ece90 |
| 0x800fbf8c | 0x800ecea0 | 0x800ea7c8, 0x800ecedc |
| 0x800fbfa0 | 0x800ec934 | 0x800eaf28, 0x800eb0f0, 0x800eb734, 0x800ed290 |
| 0x800fbfb0 | 0x800ec970 | 0x800ed2b0, 0x800ed304 |
| 0x800fbfb8 | 0x800ed324 | 0x800ed360, 0x800ed460, 0x800ed4ac |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 11 | 0x800ec934 |
| misc | 7 | 0x800ec970 |
| misc | 14 | 0x800ed324 |

## Animation tables

### Table 0x800fba58 (34 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 2 | 1 | 0 | hold |
| 1 | 1 | 1 | 0 | hold |
| 2 | 0 | 1 | 0 | hold |
| 3 | 7 | 1 | 0 | hold |
| 4 | 6 | 1 | 0 | hold |
| 5 | 5 | 1 | 0 | hold |
| 6 | 4 | 1 | 0 | hold |
| 7 | 3 | 1 | 0 | hold |
| 8 | 10 | 1 | 0 | hold |
| 9 | 9 | 1 | 0 | hold |
| 10 | 8 | 1 | 0 | hold |
| 11 | 15 | 1 | 0 | hold |
| 12 | 14 | 1 | 0 | hold |
| 13 | 13 | 1 | 0 | hold |
| 14 | 12 | 1 | 0 | hold |
| 15 | 11 | 1 | 0 | hold |
| 16 | 20 | 1 | 0 | hold |
| 17 | 19 | 1 | 0 | hold |
| 18 | 18 | 1 | 0 | hold |
| 19 | 17 | 1 | 0 | hold |
| 20 | 16 | 1 | 0 | hold |
| 21 | 31 | 1 | 0 | hold |
| 22 | 30 | 1 | 0 | hold |
| 23 | 29 | 1 | 0 | hold |
| 24 | 28 | 1 | 0 | hold |
| 25 | 27 | 1 | 0 | hold |
| 26 | 26 | 1 | 0 | hold |
| 27 | 25 | 1 | 0 | hold |
| 28 | 24 | 1 | 0 | hold |
| 29 | 23 | 1 | 0 | hold |
| 30 | 22 | 1 | 0 | hold |
| 31 | 21 | 1 | 0 | hold |
| 32 | 32 33 34 35 36 | 1 6 6 6 1 | 0 0 0 0 1 | hold |
| 33 | 37 38 39 40 41 | 1 6 6 6 1 | 0 0 0 0 1 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 1 | 0 | 0x800ed1cc |
| 0x2 | var | 12 | 1 | 0x800ea13c, 0x800ea4cc, 0x800ea650, 0x800ea7c8, 0x800eaa00, 0x800ec9b8, 0x800ecc74, 0x800ed1cc |
| 0x4 | state | 2 | 4 | 0x800ea13c, 0x800ec8f8, 0x800ed1cc |
| 0x5 | step | 5 | 7 | 0x800ea13c, 0x800ea4cc, 0x800ec9b8, 0x800eca5c, 0x800ece90 |
| 0x6 | step2 | 13 | 14 | 0x800ea13c, 0x800ea650, 0x800ea7c8, 0x800ec9b8, 0x800eca5c, 0x800ecb3c, 0x800ecb78, 0x800ecb90, 0x800ecbe4, 0x800ecc10, 0x800ecc38, 0x800ecc74, 0x800ecd40, 0x800ecdb0, 0x800ece54, 0x800ece90, 0x800ecea0, 0x800ed1cc |
| 0x8 | x | 4 | 0 | 0x800ea13c, 0x800ea4cc, 0x800ec9b8, 0x800eca5c |
| 0xa | x.px | 12 | 0 | 0x800ea7c8, 0x800eaa00, 0x800eabd8, 0x800eada8, 0x800ecc74 |
| 0xc | y | 4 | 0 | 0x800ea13c, 0x800ea4cc, 0x800ec9b8, 0x800eca5c |
| 0xe | y.px | 9 | 0 | 0x800ea7c8, 0x800eaa00, 0x800eabd8, 0x800eada8 |
| 0x14 | layer | 2 | 1 | 0x800ea13c, 0x800eabd8, 0x800eada8 |
| 0x15 | flip | 2 | 1 | 0x800ea13c, 0x800eabd8, 0x800eada8 |
| 0x16 | priority | 0 | 1 | 0x800ea13c |
| 0x18 | prev_x | 0 | 4 | 0x800ea13c, 0x800ea4cc, 0x800ec9b8, 0x800eca5c |
| 0x1c | prev_y | 0 | 4 | 0x800ea13c, 0x800ea4cc, 0x800ec9b8, 0x800eca5c |
| 0x20 | speed_x | 0 | 1 | 0x800ea13c |
| 0x24 | speed_y | 0 | 1 | 0x800ea13c |
| 0x28 | accel_x | 0 | 1 | 0x800ea13c |
| 0x2c | gravity | 0 | 1 | 0x800ea13c |
| 0x30 | anime_table | 2 | 1 | 0x800ea13c, 0x800eabd8, 0x800eada8 |
| 0x40 | tex_cord | 2 | 0 | 0x800eabd8, 0x800eada8 |
| 0x42 | clut_cord | 2 | 0 | 0x800eabd8, 0x800eada8 |
| 0x45 | anime.flag | 1 | 0 | 0x800ea650 |
| 0x50 | attack_box | 0 | 1 | 0x800ea13c |
| 0x54 | hurt_box | 0 | 5 | 0x800ea13c, 0x800ea4cc |
| 0x58 | damage_table | 0 | 3 | 0x800ea13c, 0x800ea4cc |
| 0x5c | hp | 0 | 2 | 0x800ea13c, 0x800ea4cc |
| 0x60 | contact_damage | 0 | 1 | 0x800ea13c |
| 0x61 | iframes | 0 | 1 | 0x800ea13c |
| 0x67 | latch | 0 | 1 | 0x800ea13c |
| 0x68 | terrain_box | 0 | 1 | 0x800ea13c |
| 0x7c | work.7C | 3 | 7 | 0x800ea13c, 0x800ecb78, 0x800ecb90, 0x800ecbe4, 0x800ecc10, 0x800ecd40, 0x800ecdb0 |
| 0x7e | work.7E | 1 | 2 | 0x800ea13c, 0x800ecb00 |
| 0x84 | work.84 | 1 | 3 | 0x800ea13c, 0x800eaa00, 0x800eabd8, 0x800eada8 |
| 0x86 | self+0x86 | 2 | 2 | 0x800ea13c, 0x800eaa00, 0x800eada8, 0x800ecd40 |
| 0x87 | self+0x87 | 2 | 2 | 0x800ea13c, 0x800ea4cc |
| 0x88 | work.88 | 7 | 7 | 0x800ea13c, 0x800ea4cc, 0x800eaa00, 0x800eabd8, 0x800eada8, 0x800ecc74, 0x800ecd40, 0x800ed0ac |
| 0x89 | self+0x89 | 7 | 10 | 0x800ea13c, 0x800ea4cc, 0x800eabd8, 0x800eada8, 0x800eca5c, 0x800ecbe4, 0x800ecc74, 0x800ecdb0, 0x800ecdf4 |
| 0x8a | work.8A | 1 | 3 | 0x800ea13c, 0x800eaa00, 0x800ecdf4 |
| 0x8b | self+0x8b | 1 | 3 | 0x800ea13c, 0x800ea4cc |
| 0x8c | work.8C | 1 | 4 | 0x800ea13c, 0x800ea4cc, 0x800ecdf4 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800eabd8, 0x800eada8, 0x800ed360 |
| 0x8007451c | dmg_tbl_sabre_only+0xa0 | 0x800ea4cc |
| 0x800745bc |  | 0x800ea4cc |
| 0x8008ef48 | main_objects | 0x800ed1cc |
| 0x80095098 |  | 0x800ed360 |
| 0x800950a0 | weapon_objects | 0x800eaa00 |
| 0x800970aa | mega.x.px | 0x800ecc74 |
| 0x800970b4 | mega.layer | 0x800ea13c |
| 0x8009716c | mega.grab+0x2 | 0x800ec970 |
| 0x800971f8 | camera_layer0 | 0x800ea7c8 |
| 0x8009721c | cam.target | 0x800ea650 |
| 0x80097220 | cam.target+0x4 | 0x800ea650 |
| 0x80097420 | frame_count | 0x800eb734 |
| 0x800cceed | game.checkpoint | 0x800ed360, 0x800ed460 |
| 0x800e9864 |  | 0x800ea13c |
| 0x800e987c |  | 0x800ed0ac |
| 0x800e98a4 |  | 0x800ea650 |
| 0x800e98bc |  | 0x800ea7c8 |
| 0x800e98d4 |  | 0x800eabd8 |
| 0x800e98f4 |  | 0x800eaf28 |
| 0x800e9914 |  | 0x800ebcd8 |
| 0x800e9934 |  | 0x800ebcd8 |
| 0x800e9954 |  | 0x800ebcd8 |
| 0x800e9974 |  | 0x800ebcd8 |
| 0x800fb964 |  | 0x800ea13c |
| 0x800fb968 |  | 0x800ea13c, 0x800ea4cc |
| 0x800fb9a8 |  | 0x800ea13c |
| 0x800fb9ac |  | 0x800ea13c |
| 0x800fba58 |  | 0x800ea13c |
| 0x800fbae0 |  | 0x800ea13c |
| 0x800fbb60 |  | 0x800ea13c, 0x800ea4cc |
| 0x800fbc80 |  | 0x800ebcd8 |
| 0x800fbc90 |  | 0x800ecee4 |
| 0x800fbca0 |  | 0x800eb0f0, 0x800eb734 |
| 0x800fbcb0 |  | 0x800eb0f0, 0x800eb734 |
| 0x800fbcc0 |  | 0x800eaa00, 0x800eada8 |
| 0x800fbd40 |  | 0x800eaa00, 0x800eada8 |
| 0x800fbe90 |  | 0x800ea650 |
| 0x800fbe98 |  | 0x800ea650 |
| 0x800fbea0 |  | 0x800ea650 |
| 0x800fbea8 |  | 0x800ea650 |
| 0x800fbeb0 |  | 0x800ea650 |
| 0x800fbeb8 |  | 0x800ea650 |
| 0x800fbec0 |  | 0x800ea7c8 |
| 0x800fbec8 |  | 0x800ea7c8 |
| 0x800fbed0 |  | 0x800ea7c8 |
| 0x800fbed8 |  | 0x800ea7c8 |
| 0x800fbee0 |  | 0x800ea7c8 |
| 0x800fbee8 |  | 0x800ea7c8 |
| 0x800fbef0 |  | 0x800ea7c8 |
| 0x800fbef8 |  | 0x800ea13c |
| 0x800fbf9c |  | 0x800eaf28 |
| 0x800fcfec |  | 0x800ea13c, 0x800eabd8, 0x800eada8 |
