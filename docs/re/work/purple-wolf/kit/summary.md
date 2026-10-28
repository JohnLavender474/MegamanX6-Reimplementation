# RE kit: purple-wolf

- Object: purple-wolf
- State: `wolfang-2_ice-slope.state.gz` (SHA-1 `9fe41277436027d7fe980750634fcbdd7ac8afd1`), stage 2 area 0
- Class: main, type 6; entry function 0x800eb5a0
- Functions walked: 11; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800eb110, 0x800eb3c8, 0x800eb750 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800eaf68, 0x800eb110, 0x800eb29c, 0x800eb3c8, 0x800eb750 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800eaf68, 0x800eb29c, 0x800eb860 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800eb820, 0x800eb840, 0x800eb94c |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800eaf68, 0x800eb29c, 0x800eb860 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800eaf68, 0x800eb29c, 0x800eb860 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800eaf68, 0x800eb29c, 0x800eb860 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800eb670 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800eb5a0 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800eaf68, 0x800eb29c, 0x800eb860 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800eb860 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800eb860 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x8004a2b4 | 0x800eb860 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6acc | 0x800eb670 | 0x800eb750, 0x800eaf68, 0x800eb820 |
| 0x800f6ad8 | 0x800eb670 | 0x800eb110, 0x800eb29c, 0x800eb840, 0x800eb3c8, 0x800eb860, 0x800eb94c |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 5 | 0x800eb670 |

## Animation tables

### Table 0x800f6a00 (33 animations)

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

Note: cut: animation 33 uses sprite 172; the sprite set has 108

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x3 | display | 0 | 1 | 0x800eb5a0 |
| 0x4 | state | 1 | 0 | 0x800eb5a0 |
| 0x8 | x | 1 | 0 | 0x800eb5a0 |
| 0xc | y | 1 | 0 | 0x800eb5a0 |
| 0x18 | prev_x | 0 | 1 | 0x800eb5a0 |
| 0x1c | prev_y | 0 | 1 | 0x800eb5a0 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800743dc | dmg_tbl_normal | 0x800eb3c8 |
| 0x800970aa | mega.x.px | 0x800eb3c8 |
| 0x800f6a84 |  | 0x800eb5a0 |
| 0x800f6ab4 |  | 0x800eaf68 |
| 0x800f6ab8 |  | 0x800eb110 |
| 0x800f6abc |  | 0x800eb110 |
| 0x800f6ac0 |  | 0x800eb3c8 |
| 0x800f6ac4 |  | 0x800eb3c8 |
| 0x800f6ac8 |  | 0x800eb3c8 |
