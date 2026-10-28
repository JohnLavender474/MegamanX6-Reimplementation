# RE kit: main-6

- Object: main-6
- State: `gate-1_before-boss.state.gz` (SHA-1 `585948ebe43e9241ae0828aff8654b2e9b98ea04`), stage 16 area 0
- Class: main, type 6; entry function 0x800ee0e0
- Functions walked: 21; state tables: 4

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800edc50, 0x800edf08, 0x800ee290 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ec93c, 0x800ecb88, 0x800ecde8, 0x800ed14c, 0x800ed4b0, 0x800ed7ac, 0x800edaa8, 0x800edc50, 0x800edddc, 0x800edf08, 0x800ee290 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ec7c4, 0x800edaa8, 0x800edddc, 0x800ee3a0 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800ec93c, 0x800ecb88, 0x800ecde8, 0x800ed14c, 0x800ed4b0, 0x800ed7ac |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ee148, 0x800ee360, 0x800ee380, 0x800ee48c |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ee148 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ee148 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ec7c4, 0x800edaa8, 0x800edddc, 0x800ee3a0 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ec7c4, 0x800edaa8, 0x800edddc, 0x800ee3a0 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ec7c4, 0x800edaa8, 0x800edddc, 0x800ee3a0 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800ee1b0 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ee0e0 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ec7c4, 0x800edaa8, 0x800edddc, 0x800ee3a0 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ec7c4, 0x800ee3a0 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ec7c4 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ec7c4 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ec7c4, 0x800ee3a0 |
| 0x8004a2b4 | SmokePuff | SmokePuff(mode, a1, obj, margin, n): n effect objects (flags 0x21, type 9, var 0) at random points of obj's terrain box (mode 0); called every 8 updates by the Wolfloid ball and the Rescue Reploid bomb; smoke is a guess | 0x800ee3a0 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ec7c4 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fbb04 | 0x800ee0e0 | 0x800ec670, 0x800ec7c4, 0x800ee148, 0x800ee148 |
| 0x800fbb14 | 0x800ec7c4 | 0x80040290, 0x800ee140, 0x800ec93c, 0x800ecb88, 0x800ecde8, 0x800ed14c, 0x800ed4b0, 0x800ed7ac |
| 0x800fbb4c | 0x800ee1b0 | 0x800ee290, 0x800edaa8, 0x800ee360 |
| 0x800fbb58 | 0x800ee1b0 | 0x800edc50, 0x800edddc, 0x800ee380, 0x800edf08, 0x800ee3a0, 0x800ee48c |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e9a20 | 0x800ec720 | 0x800ec670 | 6 |
| 0x800e9a38 | 0x800eca0c | 0x800ec93c | 5 |
| 0x800e9a50 | 0x800ece2c | 0x800ecde8 | 5 |
| 0x800e9a68 | 0x800ed190 | 0x800ed14c | 5 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 5 | 0x800ee1b0 |

## Animation tables

### Table 0x800fba80 (33 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 | 10 | 0 | hold |
| 1 | 1 5 6 5 6 5 6 5 6 5 6 5 6 | 8 2 2 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 2 | 1 2 3 4 | 5 4 2 2 | 0 0 0 0 | 2 |
| 3 | 7 | 10 | 0 | hold |
| 4 | 8 11 12 11 12 11 12 11 12 11 12 11 12 | 8 2 2 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 5 | 8 9 8 10 | 3 9 2 10 | 0 0 0 0 | hold |
| 6 | 13 | 10 | 0 | hold |
| 7 | 14 21 22 21 22 21 22 21 22 21 22 21 22 | 8 2 2 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 8 | 14 15 16 17 | 5 4 2 2 | 0 0 0 0 | 2 |
| 9 | 18 | 10 | 0 | hold |
| 10 | 19 23 24 23 24 23 24 23 24 23 24 23 24 | 8 2 2 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 11 | 19 20 19 25 | 3 9 2 10 | 0 0 0 0 | hold |
| 12 | 26 27 28 29 30 31 32 33 34 | 2 2 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 | hold |
| 13 | 35 36 37 38 39 40 41 42 | 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 | 0 |
| 14 | 43 44 45 | 4 5 6 | 0 0 0 | hold |
| 15 | 46 47 48 49 50 51 52 53 54 | 2 2 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 | hold |
| 16 | 55 56 57 58 59 60 61 62 | 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 | 0 |
| 17 | 63 64 65 | 4 5 6 | 0 0 0 | hold |
| 18 | 66 67 68 69 68 67 | 5 4 4 5 4 4 | 0 0 0 0 0 0 | 0 |
| 19 | 70 | 10 | 0 | hold |
| 20 | 71 | 10 | 0 | hold |
| 21 | 72 | 10 | 0 | hold |
| 22 | 73 | 10 | 0 | hold |
| 23 | 74 | 10 | 0 | hold |
| 24 | 75 | 10 | 0 | hold |
| 25 | 76 83 84 83 84 83 84 83 84 83 84 83 84 | 8 2 2 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 26 | 76 77 78 79 | 5 4 2 2 | 0 0 0 0 | 2 |
| 27 | 80 | 10 | 0 | hold |
| 28 | 81 85 86 85 86 85 86 85 86 85 86 85 86 | 8 2 2 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 29 | 81 82 81 87 | 3 9 2 10 | 0 0 0 0 | hold |
| 30 | 88 89 90 91 92 93 94 95 96 | 2 2 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 | hold |
| 31 | 97 98 99 100 101 102 103 104 | 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 | 0 |
| 32 | 105 106 107 | 4 5 6 | 0 0 0 | hold |

Note: cut: animation 33 uses sprite 166; the sprite set has 108

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 8 | 0 | 0x800ec93c, 0x800ecb88, 0x800ecde8, 0x800ed14c, 0x800ed4b0, 0x800ed7ac |
| 0x2 | var | 6 | 1 | 0x800ec670, 0x800ec7c4, 0x800ec93c, 0x800ecb88, 0x800ee148 |
| 0x3 | display | 0 | 2 | 0x800ec670, 0x800ee0e0 |
| 0x4 | state | 2 | 3 | 0x800ec670, 0x800ec7c4, 0x800ee0e0 |
| 0x5 | step | 1 | 7 | 0x800ec670, 0x800ec7c4 |
| 0x6 | step2 | 18 | 25 | 0x800ec670, 0x800ec93c, 0x800ecb88, 0x800ecde8, 0x800ed14c, 0x800ed4b0, 0x800ed7ac |
| 0x7 | act4 | 3 | 2 | 0x800ec670, 0x800ec7c4, 0x800ec93c, 0x800ecb88 |
| 0x8 | x | 1 | 0 | 0x800ee0e0 |
| 0xa | x.px | 30 | 0 | 0x800ec93c, 0x800ecb88, 0x800ecde8, 0x800ed14c, 0x800ed4b0, 0x800ed7ac |
| 0xc | y | 1 | 0 | 0x800ee0e0 |
| 0xe | y.px | 0 | 2 | 0x800ec93c, 0x800ecb88 |
| 0x14 | layer | 0 | 1 | 0x800ec670 |
| 0x15 | flip | 18 | 40 | 0x800ec93c, 0x800ecb88, 0x800ecde8, 0x800ed14c, 0x800ed4b0, 0x800ed7ac |
| 0x16 | priority | 0 | 1 | 0x800ec670 |
| 0x18 | prev_x | 0 | 1 | 0x800ee0e0 |
| 0x1c | prev_y | 0 | 1 | 0x800ee0e0 |
| 0x20 | speed_x | 0 | 1 | 0x800ec670 |
| 0x24 | speed_y | 0 | 2 | 0x800ec670 |
| 0x28 | accel_x | 0 | 1 | 0x800ec670 |
| 0x2c | gravity | 0 | 2 | 0x800ec670 |
| 0x30 | anime_table | 0 | 1 | 0x800ec670 |
| 0x46 | anime.next | 6 | 0 | 0x800ec93c, 0x800ecb88, 0x800ecde8, 0x800ed14c, 0x800ed4b0, 0x800ed7ac |
| 0x50 | attack_box | 0 | 1 | 0x800ec670 |
| 0x54 | hurt_box | 0 | 1 | 0x800ec670 |
| 0x58 | damage_table | 0 | 1 | 0x800ec670 |
| 0x5c | hp | 0 | 1 | 0x800ec670 |
| 0x60 | contact_damage | 0 | 1 | 0x800ec670 |
| 0x61 | iframes | 0 | 1 | 0x800ec670 |
| 0x67 | latch | 0 | 1 | 0x800ec670 |
| 0x68 | terrain_box | 0 | 4 | 0x800ec670, 0x800ec93c, 0x800ecb88 |
| 0x70 | collide_flags | 1 | 0 | 0x800ec7c4 |
| 0x7c | work.7C | 18 | 18 | 0x800ec93c, 0x800ecb88, 0x800ecde8, 0x800ed14c, 0x800ed4b0, 0x800ed7ac |
| 0x80 | work.80 | 3 | 0 | 0x800ec7c4, 0x800ec93c, 0x800ecb88 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800743dc | dmg_tbl_normal | 0x800ec670, 0x800edf08 |
| 0x800970a0 | mega | 0x800ec670, 0x800ec93c, 0x800ecb88, 0x800ecde8, 0x800ed14c, 0x800ed4b0, 0x800ed7ac |
| 0x800970aa | mega.x.px | 0x800ecde8, 0x800ed14c, 0x800edf08 |
| 0x800fb7a0 |  | 0x800ec670, 0x800ec93c, 0x800ecb88 |
| 0x800fb7a4 |  | 0x800ec670 |
| 0x800fb7a8 |  | 0x800ec670 |
| 0x800fba78 |  | 0x800ec7c4 |
| 0x800fba80 |  | 0x800ec670 |
| 0x800fbb34 |  | 0x800edaa8 |
| 0x800fbb38 |  | 0x800edc50 |
| 0x800fbb3c |  | 0x800edc50 |
| 0x800fbb40 |  | 0x800edf08 |
| 0x800fbb44 |  | 0x800edf08 |
| 0x800fbb48 |  | 0x800edf08 |
