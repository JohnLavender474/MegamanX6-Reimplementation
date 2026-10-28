# RE kit: main-13

- Object: main-13
- State: `gate-1_before-boss.state.gz` (SHA-1 `585948ebe43e9241ae0828aff8654b2e9b98ea04`), stage 16 area 0
- Class: main, type 13; entry function 0x800f19e4
- Functions walked: 42; state tables: 10

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ef5b8, 0x800efcc4, 0x800efe94, 0x800f1b48 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ef228, 0x800ef5b8, 0x800f1d60 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ef5b8, 0x800ef73c, 0x800f1aa4 |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800ef73c, 0x800ef8b4 |
| 0x80029f38 | SpawnInRect | SpawnInRect(x0, x1, y0, y1, flag): spawn the records inside a rectangle | 0x800ef228 |
| 0x8002c32c | GetWeaponObject | first free of the 32 x 0xA0 weapon slots at 0x800950A0, cleared (boxes 0) | 0x800f0014 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800efcc4, 0x800efe94 |
| 0x8002c530 | GetMiscObject | free misc object (debris) | 0x800f01dc, 0x800f0820 |
| 0x8002c5f0 | GetMiscObjectFrom | GetMiscObjectFrom(start, back): free slot of the 96 misc objects, searching forward (a1 = 0) or backward from start (0 = first) (likely) | 0x800f1fd0 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800f1bec, 0x800f237c, 0x800f23f0 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ef5b8, 0x800f0dc4, 0x800f1aa4, 0x800f1b48, 0x800f239c |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800f01dc |
| 0x8002d380 | AimAt | AimAt(a, b): direction 0..31 from a to b (0 right, 8 up, 16 left, 24 down) | 0x800f2198 |
| 0x8002d654 | BoxOverlap | box test between two objects, facing-aware | 0x800efaec |
| 0x80030234 | TileCollisionAt | TileCollisionAt(obj, x, y): collision value of the obj layer tile at (x, y); 59 reads as 56; 0 when off map | 0x800f2100 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800f01dc |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ef5b8 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ef5b8, 0x800f1aa4, 0x800f1b48 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fc264 | 0x800f19e4 | 0x800ef228, 0x800f1aa4, 0x800ef5b8, 0x800f1b48, 0x800f1bec |
| 0x800fc278 | 0x800f1aa4 | 0x80040290, 0x800f2374, 0x800f1ed8, 0x800f1c28, 0x800f1d24 |
| 0x800fc28c | 0x800ef5b8 | 0x80040290, 0x800f2374, 0x800f1ee0 |
| 0x800fc298 | 0x800f1b48 | 0x80040290, 0x800f2374, 0x800f1ed8, 0x800f1f40, 0x800f1f8c |
| 0x800fc2ac | 0x800f1c28 | 0x800f1c64, 0x800f1c7c, 0x800f1cd0, 0x800f1cfc |
| 0x800fc2bc | 0x800f1d24 | 0x800f1d60, 0x800f1e2c, 0x800f1e9c |
| 0x800fc2c8 | 0x800f1f40 | 0x800ef73c, 0x800f1f7c |
| 0x800fc2d0 | 0x800f1f8c | 0x800ef8b4, 0x800f1fc8 |
| 0x800fc2e4 | 0x800f1a20 | 0x800f0014, 0x800f01dc, 0x800f0820, 0x800f237c |
| 0x800fc2f4 | 0x800f1a5c | 0x800f239c, 0x800f23f0 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e9a7c | 0x800ef4b0 | 0x800ef228 | 6 |
| 0x800e9abc | 0x800ef78c | 0x800ef73c | 6 |
| 0x800e9ad4 | 0x800ef8f4 | 0x800ef8b4 | 6 |
| 0x800e9aec | 0x800efdb4 | 0x800efcc4 | 8 |
| 0x800e9b0c | 0x800f0160 | 0x800f0014 | 8 |
| 0x800e9b2c | 0x800f0e28 | 0x800f0dc4 | 8 |
| 0x800e9b4c | 0x800f16fc | 0x800f0dc4 | 8 |
| 0x800e9b6c | 0x800f17c0 | 0x800f0dc4 | 8 |
| 0x800e9b8c | 0x800f18fc | 0x800f0dc4 | 8 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 11 | 0x800f1a20 |
| misc | 7 | 0x800f1a5c |

## Animation tables

### Table 0x800fbd9c (34 animations)

Found by the code search (no live object uses it).

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
| 0x0 | flags | 1 | 0 | 0x800f22b8 |
| 0x2 | var | 12 | 1 | 0x800ef228, 0x800ef5b8, 0x800ef73c, 0x800ef8b4, 0x800efaec, 0x800f1aa4, 0x800f1d60, 0x800f22b8 |
| 0x4 | state | 2 | 4 | 0x800ef228, 0x800f19e4, 0x800f22b8 |
| 0x5 | step | 5 | 7 | 0x800ef228, 0x800ef5b8, 0x800f1aa4, 0x800f1b48, 0x800f1f7c |
| 0x6 | step2 | 13 | 14 | 0x800ef228, 0x800ef73c, 0x800ef8b4, 0x800f1aa4, 0x800f1b48, 0x800f1c28, 0x800f1c64, 0x800f1c7c, 0x800f1cd0, 0x800f1cfc, 0x800f1d24, 0x800f1d60, 0x800f1e2c, 0x800f1e9c, 0x800f1f40, 0x800f1f7c, 0x800f1f8c, 0x800f22b8 |
| 0x8 | x | 4 | 0 | 0x800ef228, 0x800ef5b8, 0x800f1aa4, 0x800f1b48 |
| 0xa | x.px | 12 | 0 | 0x800ef8b4, 0x800efaec, 0x800efcc4, 0x800efe94, 0x800f1d60 |
| 0xc | y | 4 | 0 | 0x800ef228, 0x800ef5b8, 0x800f1aa4, 0x800f1b48 |
| 0xe | y.px | 9 | 0 | 0x800ef8b4, 0x800efaec, 0x800efcc4, 0x800efe94 |
| 0x14 | layer | 2 | 1 | 0x800ef228, 0x800efcc4, 0x800efe94 |
| 0x15 | flip | 2 | 1 | 0x800ef228, 0x800efcc4, 0x800efe94 |
| 0x16 | priority | 0 | 1 | 0x800ef228 |
| 0x18 | prev_x | 0 | 4 | 0x800ef228, 0x800ef5b8, 0x800f1aa4, 0x800f1b48 |
| 0x1c | prev_y | 0 | 4 | 0x800ef228, 0x800ef5b8, 0x800f1aa4, 0x800f1b48 |
| 0x20 | speed_x | 0 | 1 | 0x800ef228 |
| 0x24 | speed_y | 0 | 1 | 0x800ef228 |
| 0x28 | accel_x | 0 | 1 | 0x800ef228 |
| 0x2c | gravity | 0 | 1 | 0x800ef228 |
| 0x30 | anime_table | 2 | 1 | 0x800ef228, 0x800efcc4, 0x800efe94 |
| 0x40 | tex_cord | 2 | 0 | 0x800efcc4, 0x800efe94 |
| 0x42 | clut_cord | 2 | 0 | 0x800efcc4, 0x800efe94 |
| 0x45 | anime.flag | 1 | 0 | 0x800ef73c |
| 0x50 | attack_box | 0 | 1 | 0x800ef228 |
| 0x54 | hurt_box | 0 | 5 | 0x800ef228, 0x800ef5b8 |
| 0x58 | damage_table | 0 | 3 | 0x800ef228, 0x800ef5b8 |
| 0x5c | hp | 0 | 2 | 0x800ef228, 0x800ef5b8 |
| 0x60 | contact_damage | 0 | 1 | 0x800ef228 |
| 0x61 | iframes | 0 | 1 | 0x800ef228 |
| 0x67 | latch | 0 | 1 | 0x800ef228 |
| 0x68 | terrain_box | 0 | 1 | 0x800ef228 |
| 0x7c | work.7C | 3 | 7 | 0x800ef228, 0x800f1c64, 0x800f1c7c, 0x800f1cd0, 0x800f1cfc, 0x800f1e2c, 0x800f1e9c |
| 0x7e | work.7E | 1 | 2 | 0x800ef228, 0x800f1bec |
| 0x84 | work.84 | 1 | 3 | 0x800ef228, 0x800efaec, 0x800efcc4, 0x800efe94 |
| 0x86 | self+0x86 | 2 | 2 | 0x800ef228, 0x800efaec, 0x800efe94, 0x800f1e2c |
| 0x87 | self+0x87 | 2 | 2 | 0x800ef228, 0x800ef5b8 |
| 0x88 | work.88 | 7 | 7 | 0x800ef228, 0x800ef5b8, 0x800efaec, 0x800efcc4, 0x800efe94, 0x800f1d60, 0x800f1e2c, 0x800f2198 |
| 0x89 | self+0x89 | 7 | 10 | 0x800ef228, 0x800ef5b8, 0x800efcc4, 0x800efe94, 0x800f1b48, 0x800f1cd0, 0x800f1d60, 0x800f1e9c, 0x800f1ee0 |
| 0x8a | work.8A | 1 | 3 | 0x800ef228, 0x800efaec, 0x800f1ee0 |
| 0x8b | self+0x8b | 1 | 3 | 0x800ef228, 0x800ef5b8 |
| 0x8c | work.8C | 1 | 4 | 0x800ef228, 0x800ef5b8, 0x800f1ee0 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800efcc4, 0x800efe94 |
| 0x8007451c | dmg_tbl_reflector | 0x800ef5b8 |
| 0x800745bc | dmg_tbl_reflector_wait | 0x800ef5b8 |
| 0x8008ef48 | main_objects | 0x800f22b8 |
| 0x800950a0 | weapon_objects | 0x800efaec |
| 0x800970aa | mega.x.px | 0x800f1d60 |
| 0x800970b4 | mega.layer | 0x800ef228 |
| 0x8009716c | mega.grab+0x2 | 0x800f1a5c |
| 0x800971f8 | camera_layer0 | 0x800ef8b4 |
| 0x8009721c | cam.target_max_x | 0x800ef73c |
| 0x80097220 | cam.target_max_y | 0x800ef73c |
| 0x80097420 | frame_count | 0x800f0820 |
| 0x800e9a94 |  | 0x800f2198 |
| 0x800fbca8 |  | 0x800ef228 |
| 0x800fbcac |  | 0x800ef228, 0x800ef5b8 |
| 0x800fbcec |  | 0x800ef228 |
| 0x800fbcf0 |  | 0x800ef228 |
| 0x800fbd9c |  | 0x800ef228 |
| 0x800fbe24 |  | 0x800ef228 |
| 0x800fbea4 |  | 0x800ef228, 0x800ef5b8 |
| 0x800fbfc4 |  | 0x800f0dc4 |
| 0x800fbfd4 |  | 0x800f1fd0 |
| 0x800fbfe4 |  | 0x800f01dc, 0x800f0820 |
| 0x800fbff4 |  | 0x800f01dc, 0x800f0820 |
| 0x800fc004 |  | 0x800efaec, 0x800efe94 |
| 0x800fc084 |  | 0x800efaec, 0x800efe94 |
| 0x800fc1d4 |  | 0x800ef73c |
| 0x800fc1dc |  | 0x800ef73c |
| 0x800fc1e4 |  | 0x800ef73c |
| 0x800fc1ec |  | 0x800ef73c |
| 0x800fc1f4 |  | 0x800ef73c |
| 0x800fc1fc |  | 0x800ef73c |
| 0x800fc204 |  | 0x800ef8b4 |
| 0x800fc20c |  | 0x800ef8b4 |
| 0x800fc214 |  | 0x800ef8b4 |
| 0x800fc21c |  | 0x800ef8b4 |
| 0x800fc224 |  | 0x800ef8b4 |
| 0x800fc22c |  | 0x800ef8b4 |
| 0x800fc234 |  | 0x800ef8b4 |
| 0x800fc23c |  | 0x800ef228 |
| 0x800fc2e0 |  | 0x800f0014 |
| 0x800fd590 |  | 0x800ef228, 0x800efcc4, 0x800efe94 |
