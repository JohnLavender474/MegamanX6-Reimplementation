# RE kit: main type 59

- Object: main type 59
- State: `gate-5_gate-fight.state.gz` (SHA-1 `68a442ae9239e06c8539d8eea50087a0bfce575b`), stage 18 area 0
- Class: main, type 59; entry function 0x800f49f0
- Functions walked: 74; state tables: 21

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80012d9c | Rumble | controller vibration (likely) | 0x800f4240 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800efbb4, 0x800f0098, 0x800f03bc, 0x800f04cc, 0x800f0654, 0x800f0778, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3a0c, 0x800f407c, 0x800f4c74, 0x800f5120 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ef590, 0x800ef820, 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efcdc, 0x800effa4, 0x800f0098, 0x800f03bc, 0x800f0654, 0x800f0778, 0x800f0884, 0x800f0b90, 0x800f0d4c, 0x800f12fc, 0x800f14b8, 0x800f1ac0, 0x800f1c7c, 0x800f23c4, 0x800f2590, 0x800f2b5c, 0x800f2d24, 0x800f3348, 0x800f3854, 0x800f3a0c, 0x800f3bfc, 0x800f3d74, 0x800f407c, 0x800f43b8, 0x800f4790 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efdcc, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3660, 0x800f3854, 0x800f3d74, 0x800f4240, 0x800f4560, 0x800f4b2c, 0x800f4bb0, 0x800f4c74, 0x800f4d54, 0x800f4e1c, 0x800f4e7c, 0x800f4ee0, 0x800f4ff4, 0x800f56a4, 0x800f57e4 |
| 0x80021760 | TextOpen | TextOpen(id, unused, mode): queues the id when a box is open; mode 0 saves and clears game+0x24 and +0x1F, loads the text palettes, opens the window; mode != 0: a 60-update wait with no text | 0x800f4b2c |
| 0x80025fd8 | BossLevel | game+0x8F = 3 in stage 12, else boss_level_tbl[game+0x8D + character] | 0x800ef400 |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800f43b8, 0x800f4560, 0x800f4bb0 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ef590, 0x800ef820, 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efcdc, 0x800effa4, 0x800f0098, 0x800f03bc, 0x800f0654, 0x800f0778, 0x800f0884, 0x800f0b90, 0x800f0d4c, 0x800f12fc, 0x800f14b8, 0x800f1ac0, 0x800f1c7c, 0x800f23c4, 0x800f2590, 0x800f2b5c, 0x800f2d24, 0x800f3348, 0x800f3854, 0x800f3a0c, 0x800f3bfc, 0x800f3d74, 0x800f407c, 0x800f43b8, 0x800f4790 |
| 0x8002b2f8 | PaletteFxClear | PaletteFxClear(ch): fade_r/g/b[ch] = 0, fade_mask[ch] = 0 (16 words), bg_refresh |= 1 | 0x800efa58 |
| 0x8002b6b0 | ClearPlayerShots | delete the weapon objects except type 55, and flagged effects | 0x800f5120 |
| 0x8002c3f8 | GetSysObject | first free slot of the stage-logic class (96 x 0x30 at 0x8009B7B0), cleared; used for engine objects 24 and 26 | 0x800f4a94, 0x800f51cc |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800f04cc, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f4bb0, 0x800f4f44 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800f53c8, 0x800f53f0, 0x800f5454, 0x800f54b8, 0x800f551c, 0x800f558c, 0x800f55f4, 0x800f5684, 0x800f5764, 0x800f5784, 0x800f57a4, 0x800f57c4, 0x800f5894 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ef590, 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efdcc, 0x800f0778, 0x800f0884, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3660, 0x800f3854, 0x800f3d74, 0x800f4240, 0x800f4560, 0x800f4b2c, 0x800f4bb0, 0x800f4c74, 0x800f4d54, 0x800f4e1c, 0x800f4e7c, 0x800f4ee0, 0x800f4ff4, 0x800f5120, 0x800f51cc, 0x800f52cc, 0x800f5344, 0x800f53f0, 0x800f5454, 0x800f54b8, 0x800f551c, 0x800f558c, 0x800f56a4, 0x800f57e4 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efdcc, 0x800f0778, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3660, 0x800f3854, 0x800f3d74, 0x800f4240, 0x800f4560, 0x800f4b2c, 0x800f4bb0, 0x800f4c74, 0x800f4d54, 0x800f4e1c, 0x800f4e7c, 0x800f4ee0, 0x800f4ff4, 0x800f56a4, 0x800f57e4 |
| 0x8002d2d4 | GetRNG | random number | 0x800efdcc, 0x800f0098, 0x800f03bc, 0x800f04cc, 0x800f0654 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800f0a24 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800f49f0 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ef590, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3660, 0x800f3d74, 0x800f4240, 0x800f56a4 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ef590, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3660, 0x800f3d74, 0x800f56a4 |
| 0x8003ccbc | PlayerSetTerrainBox | set the player's terrain box | 0x800f0884 |
| 0x8003d308 | PlayerEvent | PlayerEvent(step, facing): mega+0xD0 = 1, +0xD1 = step, +0xD2 = facing, +0x7A = 1, game.player_lock = 1 | 0x800ef400, 0x800ef820, 0x800f0884, 0x800f4a94 |
| 0x8003d330 | PlayerEventEnd | mega+0xD0 = 0, +0x7A = 0, game.player_lock = 0: the event step ends | 0x800f4c74 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ef590 |
| 0x80040488 | BossMusicRequest | game+0x0E = 1 (a0 not read); was named HpBarShow | 0x800efbb4 |
| 0x80040498 | BossMusicWait | when stream_state = 0: BossMusicStart and game+0x0E = 0; returns game+0x0E; was named HpBarBusy | 0x800f4c74 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3660, 0x800f3d74, 0x800f4560, 0x800f56a4 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800f0098, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3348, 0x800f3a0c |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800f0098, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3348, 0x800f3a0c |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800f0098, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3a0c |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f75f0 | 0x800f49f0 | 0x800ef400, 0x800ef590, 0x800f50e4 |
| 0x800f75fc | 0x800ef590 | 0x80040290, 0x800f4a50, 0x800f4a58, 0x800f4d54, 0x800efdcc, 0x800f4e1c, 0x800f4e7c, 0x800f4ee0, 0x800f4ff4 |
| 0x800f7620 | 0x800f4a58 | 0x800f4a94, 0x800ef820, 0x800ef95c, 0x800f4b2c, 0x800f4bb0, 0x800efa58, 0x800efbb4, 0x800f4c74 |
| 0x800f7640 | 0x800f4d54 | 0x800efcdc, 0x800f4db8 |
| 0x800f7648 | 0x800f4e1c | 0x800effa4, 0x800f0098, 0x800f02ac |
| 0x800f7654 | 0x800f4e7c | 0x800f03bc, 0x800f04cc |
| 0x800f765c | 0x800f4ee0 | 0x800f0654, 0x800f4f44 |
| 0x800f7664 | 0x800f4ff4 | 0x800f0778, 0x800f5070 |
| 0x800f768c | 0x800f50e4 | 0x800f5120, 0x800f51cc, 0x800f0884, 0x800f52cc, 0x800f5344, 0x800f53c8 |
| 0x800f76cc | 0x800f0a24 | 0x800f0b90, 0x800f0d4c, 0x800f53f0 |
| 0x800f76d8 | 0x800f0a24 | 0x800f12fc, 0x800f14b8, 0x800f5454 |
| 0x800f76e4 | 0x800f0a24 | 0x800f1ac0, 0x800f1c7c, 0x800f54b8 |
| 0x800f76f0 | 0x800f0a24 | 0x800f23c4, 0x800f2590, 0x800f551c |
| 0x800f76fc | 0x800f0a24 | 0x800f2b5c, 0x800f2d24, 0x800f558c |
| 0x800f7708 | 0x800f0a24 | 0x800f3348, 0x800f3660, 0x800f55f4 |
| 0x800f7714 | 0x800f0a24 | 0x800f5614, 0x800f3854, 0x800f5684 |
| 0x800f7720 | 0x800f0a24 | 0x800f3a0c, 0x800f56a4, 0x800f5764 |
| 0x800f772c | 0x800f0a24 | 0x800f3bfc, 0x800f3d74, 0x800f5784 |
| 0x800f7738 | 0x800f0a24 | 0x800f407c, 0x800f4240, 0x800f57a4 |
| 0x800f7744 | 0x800f0a24 | 0x800f43b8, 0x800f4560, 0x800f57c4 |
| 0x800f7750 | 0x800f0a24 | 0x800f4790, 0x800f57e4, 0x800f5894 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e98c8 | 0x800f0a88 | 0x800f0a24 | 35 |
| 0x800e9958 | 0x800f0e88 | 0x800f0d4c | 21 |
| 0x800e99b0 | 0x800f1610 | 0x800f14b8 | 21 |
| 0x800e9a08 | 0x800f1db8 | 0x800f1c7c | 21 |
| 0x800e9a60 | 0x800f2154 | 0x800f1c7c | 5 |
| 0x800e9a78 | 0x800f270c | 0x800f2590 | 21 |
| 0x800e9ad0 | 0x800f2e90 | 0x800f2d24 | 21 |
| 0x800e9b28 | 0x800f356c | 0x800f3348 | 6 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 26 | 0x800f0a24 |

## Animation tables

### Table 0x800f74a0 (34 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 | 8 8 | 0 0 | 0 |
| 1 | 2 3 | 8 8 | 0 0 | hold |
| 2 | 4 5 6 7 7 8 7 8 7 8 7 8 | 6 8 6 1 3 4 4 4 4 4 4 4 | 0 0 0 1 0 0 0 0 0 0 0 0 | hold |
| 3 | 9 10 11 | 4 4 4 | 0 0 0 | 0 |
| 4 | 12 13 14 15 13 14 15 13 14 15 16 17 16 17 18 19 18 19 20 21 22 23 24 | 4 1 2 2 1 2 2 1 2 2 1 1 1 1 1 1 1 1 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 | 21 |
| 5 | 25 26 27 28 29 30 31 32 33 34 35 36 30 31 32 33 34 35 36 30 31 32 33 34 35 36 29 28 27 26 25 | 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 6 | 38 37 38 39 40 41 | 2 12 4 4 3 3 | 0 0 0 0 0 0 | 4 |
| 7 | 147 148 149 | 1 1 2 | 0 0 0 | 0 |
| 8 | 150 | 1 | 0 | hold |
| 9 | 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 | 1 1 1 1 2 2 2 2 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 8 |
| 10 | 50 146 51 146 52 146 53 146 54 146 55 146 56 146 57 146 | 2 1 2 1 2 1 2 1 2 1 2 1 2 1 2 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 11 | 58 59 60 61 | 3 3 3 3 | 0 0 0 0 | 0 |
| 12 | 165 166 167 168 167 166 173 175 177 177 | 3 2 1 1 1 4 4 4 3 3 | 0 0 0 0 0 0 0 0 0 0 | 8 |
| 13 | 62 63 64 65 | 2 2 2 2 | 0 0 0 0 | 0 |
| 14 | 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 | 1 1 1 1 2 2 2 2 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 8 |
| 15 | 74 146 75 146 76 146 77 146 78 146 79 146 80 146 81 146 | 2 1 2 1 2 1 2 1 2 1 2 1 2 1 2 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 16 | 82 83 84 85 | 3 3 3 3 | 0 0 0 0 | 0 |
| 17 | 193 194 195 196 195 194 197 198 199 199 | 3 2 1 1 1 4 4 4 3 3 | 0 0 0 0 0 0 0 0 0 0 | 8 |
| 18 | 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 101 | 1 1 1 1 2 2 2 2 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 8 |
| 19 | 94 146 95 146 96 146 97 146 98 146 99 146 100 146 101 146 | 2 1 2 1 2 1 2 1 2 1 2 1 2 1 2 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 20 | 102 103 104 105 | 3 3 3 3 | 0 0 0 0 | 0 |
| 21 | 179 180 181 182 181 180 187 189 191 191 | 3 2 1 1 1 4 4 4 3 3 | 0 0 0 0 0 0 0 0 0 0 | 8 |
| 22 | 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 | 1 1 1 1 2 2 2 2 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 8 |
| 23 | 114 146 115 146 116 146 117 146 118 146 119 146 120 146 121 146 | 2 1 2 1 2 1 2 1 2 1 2 1 2 1 2 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 24 | 122 123 124 125 | 3 3 3 3 | 0 0 0 0 | 0 |
| 25 | 183 184 185 186 185 184 188 190 192 192 | 3 2 1 1 1 4 4 4 3 3 | 0 0 0 0 0 0 0 0 0 0 | 8 |
| 26 | 126 127 128 129 130 131 132 133 134 135 136 137 138 139 140 141 | 1 1 1 1 2 2 2 2 3 3 3 3 3 3 3 3 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 8 |
| 27 | 134 146 135 146 136 146 137 146 138 146 139 146 140 146 141 146 | 2 1 2 1 2 1 2 1 2 1 2 1 2 1 2 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 28 | 142 143 144 145 | 3 3 3 3 | 0 0 0 0 | 0 |
| 29 | 169 170 171 172 171 170 174 176 178 178 | 3 2 1 1 1 4 4 4 3 3 | 0 0 0 0 0 0 0 0 0 0 | 8 |
| 30 | 152 153 154 155 156 157 164 158 159 160 161 162 163 164 | 2 3 3 3 4 4 8 2 3 3 3 4 4 8 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 31 | 11 7 0 12 8 1 13 9 2 14 10 3 | 1 3 3 1 3 3 1 3 3 1 3 3 | 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 32 | 4 5 6 | 8 8 8 | 0 0 0 | 0 |
| 33 | 15 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 4 | 0 | 0x800f04cc, 0x800f4bb0, 0x800f4f44 |
| 0x1 | type | 36 | 0 | 0x800ef590, 0x800ef820, 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efcdc, 0x800effa4, 0x800f0098, 0x800f03bc, 0x800f0654, 0x800f0778, 0x800f0884 |
| 0x3 | display | 0 | 2 | 0x800ef400, 0x800f49f0 |
| 0x4 | state | 2 | 2 | 0x800ef400, 0x800ef590, 0x800f49f0 |
| 0x5 | step | 9 | 15 | 0x800ef400, 0x800ef590, 0x800efdcc, 0x800f02ac, 0x800f04cc, 0x800f0884, 0x800f4c74, 0x800f4db8, 0x800f4f44, 0x800f5070, 0x800f50e4, 0x800f5120, 0x800f51cc, 0x800f52cc, 0x800f5344 |
| 0x6 | step2 | 19 | 22 | 0x800ef590, 0x800ef820, 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efcdc, 0x800efdcc, 0x800effa4, 0x800f0098, 0x800f02ac, 0x800f03bc, 0x800f04cc, 0x800f0654, 0x800f0778, 0x800f4a58, 0x800f4a94, 0x800f4b2c, 0x800f4bb0, 0x800f4c74, 0x800f4d54, 0x800f4db8, 0x800f4e1c, 0x800f4e7c, 0x800f4ee0, 0x800f4f44, 0x800f4ff4, 0x800f5070 |
| 0x7 | act4 | 4 | 3 | 0x800f04cc |
| 0x8 | x | 1 | 0 | 0x800f49f0 |
| 0xa | x.px | 12 | 4 | 0x800ef400, 0x800ef820, 0x800efcdc, 0x800effa4, 0x800f0098, 0x800f02ac, 0x800f03bc, 0x800f0654, 0x800f0884, 0x800f4a94, 0x800f51cc, 0x800f52cc, 0x800f5344 |
| 0xc | y | 1 | 0 | 0x800f49f0 |
| 0xe | y.px | 6 | 3 | 0x800ef400, 0x800ef95c, 0x800f0098, 0x800f02ac, 0x800f0884, 0x800f4a94, 0x800f51cc |
| 0x14 | layer | 0 | 1 | 0x800ef400 |
| 0x15 | flip | 0 | 14 | 0x800ef400, 0x800ef820, 0x800efcdc, 0x800effa4, 0x800f0098, 0x800f03bc, 0x800f0654, 0x800f0884 |
| 0x16 | priority | 0 | 1 | 0x800ef400 |
| 0x18 | prev_x | 0 | 1 | 0x800f49f0 |
| 0x1c | prev_y | 0 | 1 | 0x800f49f0 |
| 0x20 | speed_x | 2 | 5 | 0x800ef400, 0x800ef590, 0x800f0098, 0x800f02ac, 0x800f0778 |
| 0x24 | speed_y | 2 | 7 | 0x800ef400, 0x800ef590, 0x800ef820, 0x800ef95c, 0x800f0098, 0x800f02ac, 0x800f0778 |
| 0x28 | accel_x | 0 | 3 | 0x800ef400, 0x800ef590, 0x800f0778 |
| 0x2c | gravity | 0 | 3 | 0x800ef400, 0x800ef590, 0x800f0778 |
| 0x30 | anime_table | 0 | 1 | 0x800ef400 |
| 0x3c | sprite_data | 0 | 12 | 0x800ef590, 0x800ef820, 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efcdc, 0x800effa4, 0x800f0098, 0x800f03bc, 0x800f0654, 0x800f0778, 0x800f0884 |
| 0x40 | tex_cord | 0 | 12 | 0x800ef590, 0x800ef820, 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efcdc, 0x800effa4, 0x800f0098, 0x800f03bc, 0x800f0654, 0x800f0778, 0x800f0884 |
| 0x42 | clut_cord | 7 | 18 | 0x800ef590, 0x800ef820, 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efcdc, 0x800effa4, 0x800f0098, 0x800f03bc, 0x800f0654, 0x800f0778, 0x800f0884, 0x800f5120, 0x800f51cc |
| 0x45 | anime.flag | 1 | 0 | 0x800efbb4 |
| 0x46 | anime.next | 2 | 0 | 0x800efbb4, 0x800f4b2c |
| 0x50 | attack_box | 0 | 1 | 0x800ef400 |
| 0x54 | hurt_box | 0 | 1 | 0x800ef400 |
| 0x58 | damage_table | 0 | 3 | 0x800ef400, 0x800ef590, 0x800f0778 |
| 0x5c | hp | 7 | 5 | 0x800ef400, 0x800ef590, 0x800efdcc, 0x800f4c74 |
| 0x60 | contact_damage | 0 | 1 | 0x800ef400 |
| 0x61 | iframes | 2 | 4 | 0x800ef400, 0x800f5120, 0x800f51cc |
| 0x62 | self+0x62 | 0 | 1 | 0x800ef400 |
| 0x67 | latch | 0 | 1 | 0x800ef400 |
| 0x68 | terrain_box | 0 | 2 | 0x800ef400, 0x800ef95c |
| 0x7c | work.7C | 15 | 25 | 0x800ef820, 0x800efa58, 0x800efcdc, 0x800effa4, 0x800f0098, 0x800f03bc, 0x800f04cc, 0x800f0654, 0x800f0778, 0x800f0884, 0x800f4a94, 0x800f4b2c, 0x800f4bb0, 0x800f4c74, 0x800f4db8, 0x800f4f44, 0x800f5070, 0x800f5120, 0x800f51cc, 0x800f52cc, 0x800f5344 |
| 0x7e | work.7E | 2 | 5 | 0x800f0884, 0x800f5120, 0x800f51cc |
| 0x80 | work.80 | 2 | 2 | 0x800ef820, 0x800f0884, 0x800f4a94, 0x800f51cc |
| 0x84 | work.84 | 3 | 4 | 0x800ef400, 0x800ef590, 0x800f4ff4, 0x800f5070 |
| 0x86 | self+0x86 | 6 | 6 | 0x800ef400, 0x800efdcc, 0x800f02ac, 0x800f04cc, 0x800f4db8, 0x800f4f44, 0x800f5070 |
| 0x87 | self+0x87 | 6 | 8 | 0x800ef400, 0x800efdcc, 0x800f02ac, 0x800f04cc, 0x800f4db8, 0x800f4f44, 0x800f5070 |
| 0x88 | work.88 | 4 | 2 | 0x800f0098, 0x800f02ac, 0x800f52cc, 0x800f5344 |
| 0x8a | work.8A | 3 | 1 | 0x800f0098, 0x800f02ac |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ef590, 0x800ef820, 0x800ef95c, 0x800efa58, 0x800efbb4, 0x800efcdc, 0x800effa4, 0x800f0098, 0x800f03bc, 0x800f0654, 0x800f0778, 0x800f0884, 0x800f0b90, 0x800f0d4c, 0x800f12fc, 0x800f14b8, 0x800f1ac0, 0x800f1c7c, 0x800f23c4, 0x800f2590, 0x800f2b5c, 0x800f2d24, 0x800f3348, 0x800f3854, 0x800f3a0c, 0x800f3bfc, 0x800f3d74, 0x800f407c, 0x800f43b8, 0x800f4790 |
| 0x8007433c | dmg_tbl_deflect | 0x800f0778, 0x800f0b90, 0x800f12fc, 0x800f1ac0, 0x800f23c4, 0x800f2b5c |
| 0x8007573c | dmg_tbl_mijinion+0xa0 | 0x800ef400, 0x800ef590, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3348, 0x800f3a0c, 0x800f3bfc, 0x800f407c |
| 0x8008eafc | text_open | 0x800f4bb0 |
| 0x80090d5c | fade_b | 0x800efa58 |
| 0x800970a0 | mega | 0x800ef590, 0x800f0098, 0x800f0884, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3a0c, 0x800f5120 |
| 0x800970a5 | mega.var+0x3 | 0x800f4a94 |
| 0x800970aa | mega.x.px | 0x800ef820, 0x800efcdc, 0x800effa4, 0x800f03bc, 0x800f0654, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3d74 |
| 0x800970ae | mega.y.px | 0x800f1c7c |
| 0x800970b4 | mega.layer | 0x800ef400 |
| 0x8009723f | cam.limit_speed | 0x800f4a94 |
| 0x80097244 | cam.redraw | 0x800f4560 |
| 0x80097430 | fade_g | 0x800efa58 |
| 0x800c4560 | bg_refresh | 0x800efa58 |
| 0x800cced0 | game | 0x800ef400, 0x800ef590, 0x800efdcc, 0x800f0098, 0x800f4b2c |
| 0x800ccedf | game.exit_req | 0x800ef820, 0x800f53c8 |
| 0x800ccee0 | game.stop | 0x800f5120 |
| 0x800ccef4 | game.boss_bar | 0x800efbb4 |
| 0x800ccf5c | boss_hp_max | 0x800f4c74 |
| 0x800ccf5f | game.boss_level | 0x800f14b8 |
| 0x800cf854 | fade_r | 0x800efa58 |
| 0x800e98b8 |  | 0x800ef400 |
| 0x800e98bc |  | 0x800efdcc |
| 0x800e98c0 |  | 0x800f0098 |
| 0x800e99ac |  | 0x800f14b8 |
| 0x800f6f2c |  | 0x800ef400 |
| 0x800f6f30 |  | 0x800ef400 |
| 0x800f74a0 |  | 0x800ef400 |
| 0x800f7518 |  | 0x800f43b8, 0x800f4560 |
| 0x800f7528 |  | 0x800efdcc, 0x800f02ac, 0x800f04cc, 0x800f4db8, 0x800f4f44, 0x800f5070 |
| 0x800f7548 |  | 0x800f4560 |
| 0x800f7588 |  | 0x800f4bb0 |
| 0x800f766c |  | 0x800f5344 |
| 0x800f76a4 |  | 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24 |
| 0x800f76a8 |  | 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24 |
| 0x800f76ac |  | 0x800f3348 |
| 0x800f76b0 |  | 0x800f3348 |
| 0x800f76b8 |  | 0x800f3a0c |
| 0x800f76bc |  | 0x800f3a0c |
| 0x800f76c0 |  | 0x800f3d74 |
| 0x800f76c4 |  | 0x800f3d74 |
| 0x800f76c8 |  | 0x800f4240 |
| 0x800f7768 |  | 0x800f43b8 |
| 0x800f77d8 |  | 0x800ef400, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3660, 0x800f3d74, 0x800f4240, 0x800f56a4, 0x800f57e4 |
| 0x800f77dc |  | 0x800ef400, 0x800ef590, 0x800f0d4c, 0x800f14b8, 0x800f1c7c, 0x800f2590, 0x800f2d24, 0x800f3660, 0x800f3d74, 0x800f4240, 0x800f5070, 0x800f56a4, 0x800f57e4 |
| 0x800f77e4 |  | 0x800ef400, 0x800efdcc, 0x800f02ac |
| 0x800f77e8 |  | 0x800ef400, 0x800ef590, 0x800f2590, 0x800f551c |
| 0x800f77ec |  | 0x800ef400, 0x800efdcc, 0x800f04cc, 0x800f0b90, 0x800f12fc, 0x800f1ac0, 0x800f23c4, 0x800f2b5c, 0x800f53f0, 0x800f5454, 0x800f54b8, 0x800f551c, 0x800f558c |
| 0x800f77f0 |  | 0x800ef400, 0x800efdcc, 0x800f23c4, 0x800f551c |
| 0x800f77f4 |  | 0x800ef400, 0x800efdcc, 0x800f2b5c, 0x800f558c |
| 0x800f77f8 |  | 0x800ef400, 0x800f4240 |
| 0x800f77fa |  | 0x800f4240, 0x800f4560 |
| 0x800f77fc |  | 0x800f4240, 0x800f4560 |
| 0x800f7800 |  | 0x800ef400, 0x800f0b90, 0x800f0d4c, 0x800f12fc, 0x800f14b8, 0x800f1ac0, 0x800f1c7c, 0x800f23c4, 0x800f2590, 0x800f2b5c, 0x800f2d24, 0x800f3348, 0x800f3854, 0x800f3a0c, 0x800f3bfc, 0x800f3d74, 0x800f407c, 0x800f43b8, 0x800f4790, 0x800f5614 |
| 0x800f7804 |  | 0x800ef400, 0x800ef590 |
| 0x800f7806 |  | 0x800ef400, 0x800ef590, 0x800f2590, 0x800f2d24 |
| 0x800f7808 |  | 0x800ef590 |
