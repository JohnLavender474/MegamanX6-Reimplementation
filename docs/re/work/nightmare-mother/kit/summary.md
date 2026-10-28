# RE kit: nightmare-mother

- Object: nightmare-mother
- State: `gate-2_nightmare-mother.state.gz` (SHA-1 `ea402436237a753f57ec7e0561ee2e2e12fd31db`), stage 16 area 0
- Class: main, type 60; entry function 0x800f8dc8
- Functions walked: 88; state tables: 16

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80012d9c | Rumble | controller vibration (likely) | 0x800f7a3c |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800f6254, 0x800f6594, 0x800f69a0, 0x800f6f9c, 0x800f7a3c, 0x800f7ee8, 0x800f85e4, 0x800f9170, 0x800f91b0, 0x800f921c, 0x800f9308, 0x800f9628, 0x800f96c4 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800f5c00, 0x800f6594, 0x800f7ee8, 0x800f85e4, 0x800f8834, 0x800f89f4, 0x800f8bac, 0x800f96c4, 0x800f97a4, 0x800fa4f8 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800f5f50, 0x800f6418, 0x800f6594, 0x800f69a0, 0x800f6f9c, 0x800f74b8, 0x800f7938, 0x800f7a3c, 0x800f7b88, 0x800f7c74, 0x800f85e4, 0x800f8834, 0x800f89f4, 0x800f8bac, 0x800f9410, 0x800f9498, 0x800f950c, 0x800f9584, 0x800f9628, 0x800f96c4, 0x800f97a4, 0x800f97f0, 0x800f9874, 0x800f999c, 0x800f9a8c, 0x800f9b30, 0x800f9bd4, 0x800f9c98, 0x800f9da4, 0x800f9e1c, 0x800f9e74, 0x800fa24c, 0x800fa328, 0x800fa384, 0x800fa498, 0x800fa4f8, 0x800fa614, 0x800fa6ac, 0x800fa6f4, 0x800fa73c, 0x800fa7cc |
| 0x80025fd8 | BossLevel | game+0x8F = 3 in stage 12, else boss_level_tbl[game+0x8D + character] | 0x800f6254 |
| 0x8002943c | Quake2 | screen shake variant: camera work +0x36/+0x45/+0x3E/+0x3C/+0x3A, flag 0x10 | 0x800f7a3c |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800f5c00, 0x800f7ee8, 0x800fa384 |
| 0x8002b3d4 | PaletteCopy | PaletteCopy(slot, src, n): n palettes of 32 bytes into the palette buffer (*scratchpad+0x28) at slot; bg_refresh |= 1 | 0x800f7ee8 |
| 0x8002b54c | PaletteBlend | PaletteBlend(dst, src, n, step): blend a palette toward another (likely); High Max shots run it 31 updates with bg_refresh |= 1 | 0x800fa384 |
| 0x8002bccc | GetMainObjectAfter | GetMainObjectAfter(obj, back): first free main slot after obj (a1 = 0 searches forward), cleared; overlay 07 parts (likely) | 0x800f6254, 0x800f6418 |
| 0x8002c3f8 | GetSysObject | first free slot of the stage-logic class (96 x 0x30 at 0x8009B7B0), cleared; used for engine objects 24 and 26 | 0x800f8ecc, 0x800f90d4 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800f6418, 0x800f69a0, 0x800f6f9c, 0x800f74b8, 0x800f85e4, 0x800f8bac, 0x800fa24c |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800f8e60, 0x800f8fb4, 0x800fa8a0 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800fa498, 0x800fa73c |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800f8fb4, 0x800fa498, 0x800fa6f4, 0x800fa73c, 0x800fa7cc |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800f7a3c, 0x800fa498 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800f69a0, 0x800f7850, 0x800f85e4, 0x800f8834, 0x800f89f4, 0x800f8bac, 0x800f9584, 0x800f96c4, 0x800f9b30, 0x800f9c98, 0x800f9e74, 0x800fa24c, 0x800fa73c |
| 0x8002d2d4 | GetRNG | random number | 0x800f6418, 0x800f6594, 0x800f69a0, 0x800f6f9c, 0x800f74b8, 0x800f7dc0, 0x800f7ee8, 0x800f8bac, 0x800f9874, 0x800f999c, 0x800f9f74, 0x800fa01c, 0x800fa4f8 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800f8dc8, 0x800fa190 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800f5f50, 0x800fa1ec, 0x800fa438, 0x800fa64c, 0x800fa73c, 0x800fa848 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800f5f50 |
| 0x8003d308 | PlayerEvent | PlayerEvent(step, facing): mega+0xD0 = 1, +0xD1 = step, +0xD2 = facing, +0x7A = 1, game.player_lock = 1 | 0x800f6254, 0x800f8e60, 0x800f909c, 0x800f90d4 |
| 0x8003d330 | PlayerEventEnd | mega+0xD0 = 0, +0x7A = 0, game.player_lock = 0: the event step ends | 0x800f9308 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800f5f50 |
| 0x80040488 | BossMusicRequest | game+0x0E = 1 (a0 not read); was named HpBarShow | 0x800f921c |
| 0x80040498 | BossMusicWait | when stream_state = 0: BossMusicStart and game+0x0E = 0; returns game+0x0E; was named HpBarBusy | 0x800f92b0 |
| 0x80049d90 | SpawnHitEffect | effect (flags 0x21, type 3, var 1) at the object, mirrored when speed_x > 0; an enemy shot hitting a wall (likely) | 0x800fa73c |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800f5f50 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800f7ee8 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800f7ee8 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800f7ee8 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fcf58 | 0x800f8dc8 | 0x800f5c00, 0x800f5f50, 0x800f8e24, 0x800f8e24 |
| 0x800fcf68 | 0x800f5f50 | 0x80040290, 0x800fa188, 0x800f9060, 0x800f93a4, 0x800f95ec, 0x800f9838, 0x800f74b8, 0x800f9c20, 0x800f9c5c, 0x800f9d68, 0x800f9f38, 0x800fa110 |
| 0x800fcf98 | 0x800f8e24 | 0x800f8e60, 0x800f8ecc, 0x800f8fb4 |
| 0x800fcfa4 | 0x800f9060 | 0x800f909c, 0x800f90d4, 0x800f6254, 0x800f9170, 0x800f91b0, 0x800f921c, 0x800f92b0, 0x800f92ec, 0x800f9300, 0x800f9308 |
| 0x800fcfcc | 0x800f93a4 | 0x800f93e0, 0x800f9410, 0x800f6418, 0x800f9498, 0x800f950c, 0x800f9584 |
| 0x800fcfe4 | 0x800f95ec | 0x800f9628, 0x800f6594, 0x800f69a0, 0x800f6f9c, 0x800f96c4, 0x800f97a4, 0x800f97f0 |
| 0x800fd000 | 0x800f9838 | 0x800f9874, 0x800f997c, 0x800f9984, 0x800f998c, 0x800f9994, 0x800f7630, 0x800f7850 |
| 0x800fd01c | 0x800f9c20 | 0x800f7938, 0x800f999c, 0x800f7a3c, 0x800f9a8c, 0x800f9b30, 0x800f9bd4 |
| 0x800fd034 | 0x800f9c5c | 0x800f7b88, 0x800f9c98, 0x800f9d3c |
| 0x800fd4c0 | 0x800f9d68 | 0x800f9da4, 0x800f9e1c, 0x800f7c74, 0x800f9e74 |
| 0x800fd4d0 | 0x800f9f38 | 0x800f9f74, 0x800fa01c, 0x800f7dc0, 0x800fa0dc, 0x800fa108 |
| 0x800fd534 | 0x800fa1ec | 0x800f85e4, 0x800f8834, 0x800f89f4 |
| 0x800fd540 | 0x800fa438 | 0x800fa24c, 0x800fa328, 0x800fa384 |
| 0x800fd54c | 0x800fa64c | 0x800fa4f8, 0x800f8bac, 0x800fa614 |
| 0x800fd558 | 0x800fa848 | 0x800fa1ec, 0x800fa438, 0x800fa498, 0x800fa64c, 0x800fa6ac, 0x800fa6f4, 0x800fa73c, 0x800fa7cc |
| 0x800fd578 | 0x800fa190 | 0x800f7ee8, 0x800fa848, 0x800fa8a0 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e9bc4 | 0x800f5df4 | 0x800f5c00 | 5 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 30 | 0x800fa190 |

## Animation tables

### Table 0x800fce98 (52 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 184 185 186 183 | 2 2 2 2 | 0 0 0 1 | hold |
| 1 | 168 169 170 183 | 2 2 2 2 | 0 0 0 1 | hold |
| 2 | 171 172 173 183 | 2 2 2 2 | 0 0 0 1 | hold |
| 3 | 187 188 189 183 | 2 2 2 2 | 0 0 0 1 | hold |
| 4 | 165 166 167 183 | 2 2 2 2 | 0 0 0 1 | hold |
| 5 | 174 175 176 183 | 2 2 2 2 | 0 0 0 1 | hold |
| 6 | 177 178 179 183 | 2 2 2 2 | 0 0 0 1 | hold |
| 7 | 180 181 182 183 | 2 2 2 2 | 0 0 0 1 | hold |
| 8 | 0 183 1 183 2 183 3 183 2 183 1 183 0 1 2 3 2 1 | 10 10 10 10 2 2 2 2 2 2 2 2 10 10 10 10 10 10 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 | 12 |
| 9 | 7 183 7 183 7 183 7 183 7 183 7 183 7 6 5 4 5 6 7 8 9 10 9 8 7 125 7 126 127 126 7 125 7 126 127 126 7 | 10 10 10 10 5 5 5 5 2 2 2 2 30 4 4 8 4 4 4 4 4 8 4 4 4 4 4 4 4 4 4 10 10 10 10 10 10 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16 0 0 0 0 0 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 31 |
| 10 | 0 1 2 3 2 1 | 10 10 10 10 10 10 | 0 0 0 0 0 0 | 0 |
| 11 | 125 7 126 127 126 7 | 10 10 10 10 10 10 | 0 0 0 0 0 0 | 0 |
| 12 | 8 9 10 | 10 10 1 | 0 0 0 | hold |
| 13 | 6 5 4 | 10 10 1 | 1 1 1 | hold |
| 14 | 11 12 13 | 10 10 1 | 2 2 2 | hold |
| 15 | 16 15 14 | 10 10 1 | 3 3 3 | hold |
| 16 | 122 4 123 124 123 4 | 4 4 4 4 4 4 | 0 0 0 0 0 0 | 0 |
| 17 | 135 134 136 137 136 134 | 4 4 4 4 4 4 | 0 0 0 0 0 0 | 0 |
| 18 | 128 13 129 130 129 13 | 4 4 4 4 4 4 | 0 0 0 0 0 0 | 0 |
| 19 | 138 139 140 141 140 139 | 4 4 4 4 4 4 | 0 0 0 0 0 0 | 0 |
| 20 | 95 96 97 98 99 101 102 103 | 2 2 2 2 2 2 2 1 | 0 0 0 0 0 0 0 0 | hold |
| 21 | 113 114 115 116 117 118 119 120 121 | 2 2 2 2 2 2 2 2 1 | 0 0 0 0 0 0 0 0 0 | hold |
| 22 | 104 105 106 107 108 109 110 111 112 | 2 2 2 2 2 2 2 2 1 | 0 0 0 0 0 0 0 0 0 | hold |
| 23 | 68 69 70 71 72 73 74 75 76 | 2 2 2 2 2 2 2 2 1 | 0 0 0 0 0 0 0 0 1 | hold |
| 24 | 77 78 79 80 81 82 83 84 85 | 2 2 2 2 2 2 2 2 1 | 0 0 0 0 0 0 0 0 1 | hold |
| 25 | 133 132 45 44 43 39 40 41 42 46 47 48 49 | 2 2 2 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 0 0 0 | 5 |
| 26 | 39 40 41 42 46 47 48 49 | 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 | 0 |
| 27 | 43 44 45 131 132 133 | 2 2 2 2 2 1 | 0 0 0 0 0 1 | hold |
| 28 | 62 63 64 65 66 67 | 2 2 2 2 2 2 | 0 0 0 0 0 0 | 0 |
| 29 | 25 26 27 28 29 30 31 32 33 | 1 1 1 1 1 1 1 1 1 | 0 0 0 0 0 0 0 0 0 | 0 |
| 30 | 34 35 36 37 38 | 10 10 10 10 10 | 0 0 0 0 0 | 0 |
| 31 | 50 51 52 53 54 54 | 1 1 1 2 1 1 | 0 0 0 0 0 1 | hold |
| 32 | 55 56 57 58 59 60 61 | 1 1 1 1 1 1 1 | 0 0 0 0 0 0 0 | 0 |
| 33 | 142 143 144 145 146 147 148 | 1 1 1 1 1 1 1 | 0 0 0 0 0 0 0 | 0 |
| 34 | 149 150 151 152 153 154 | 6 6 4 4 4 1 | 0 0 0 0 0 1 | hold |
| 35 | 155 156 157 158 159 160 | 6 6 4 4 4 1 | 0 0 0 0 0 1 | hold |
| 36 | 17 18 19 20 21 22 23 24 | 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 | 0 |
| 37 | 190 191 192 193 194 195 196 | 1 1 1 1 1 1 1 | 0 0 0 0 0 0 0 | 0 |
| 38 | 197 198 199 200 201 202 | 6 6 4 4 4 1 | 0 0 0 0 0 1 | hold |
| 39 | 203 204 205 206 207 208 209 | 1 1 1 1 1 1 1 | 0 0 0 0 0 0 0 | 0 |
| 40 | 210 211 212 213 214 215 | 6 6 4 4 4 1 | 0 0 0 0 0 1 | hold |
| 41 | 161 7 162 7 163 7 164 | 1 1 1 1 1 1 1 | 0 0 0 0 0 0 0 | 0 |
| 42 | 10 9 8 | 10 10 1 | 0 0 4 | hold |
| 43 | 4 5 6 | 10 10 1 | 1 1 4 | hold |
| 44 | 13 12 11 | 10 10 1 | 2 2 4 | hold |
| 45 | 14 15 16 | 10 10 1 | 3 3 4 | hold |
| 46 | 0 1 2 3 4 | 5 5 5 5 5 | 0 0 0 0 0 | 0 |
| 47 | 5 6 7 8 9 | 5 5 5 5 5 | 0 0 0 0 0 | 0 |
| 48 | 60 60 0 0 | 9 13 33 256 | 128 128 136 0 | hold |
| 49 | 39 36 36 36 36 160 36 174 0 | 224 8 124 160 128 92 4 88 256 | 255 0 88 112 255 0 0 0 0 | hold |
| 50 | 39 0 | 232 256 | 255 0 | hold |
| 51 | 39 0 | 232 256 | 255 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 1 | 1 | 0x800f5c00 |
| 0x1 | type | 3 | 0 | 0x800f5c00 |
| 0x2 | var | 19 | 0 | 0x800f5c00, 0x800f5f50, 0x800f6418, 0x800f6594, 0x800f74b8, 0x800f7630, 0x800f7b88, 0x800f9410, 0x800f950c, 0x800f9874, 0x800f9da4 |
| 0x3 | display | 0 | 11 | 0x800f5c00, 0x800f5f50, 0x800f8e60, 0x800f8ecc, 0x800f8fb4, 0x800f93e0, 0x800fa110 |
| 0x4 | state | 2 | 3 | 0x800f5c00, 0x800f5f50, 0x800f8dc8, 0x800f8e60 |
| 0x5 | step | 4 | 19 | 0x800f5c00, 0x800f5f50, 0x800f7630, 0x800f7938, 0x800f7c74, 0x800f8e24, 0x800f8e60, 0x800f8ecc, 0x800f9308, 0x800f950c, 0x800f9584, 0x800f9874, 0x800f9bd4, 0x800f9d3c, 0x800f9e74 |
| 0x6 | step2 | 54 | 57 | 0x800f5c00, 0x800f5f50, 0x800f6254, 0x800f6418, 0x800f6594, 0x800f69a0, 0x800f6f9c, 0x800f74b8, 0x800f7630, 0x800f7850, 0x800f7938, 0x800f7a3c, 0x800f7b88, 0x800f7c74, 0x800f7dc0, 0x800f9060, 0x800f909c, 0x800f90d4, 0x800f91b0, 0x800f921c, 0x800f92b0, 0x800f92ec, 0x800f9308, 0x800f93a4, 0x800f93e0, 0x800f9410, 0x800f9498, 0x800f950c, 0x800f9584, 0x800f95ec, 0x800f9628, 0x800f96c4, 0x800f97a4, 0x800f97f0, 0x800f9838, 0x800f9874, 0x800f999c, 0x800f9a8c, 0x800f9b30, 0x800f9bd4, 0x800f9c20, 0x800f9c5c, 0x800f9c98, 0x800f9d3c, 0x800f9d68, 0x800f9da4, 0x800f9e1c, 0x800f9e74, 0x800f9f38, 0x800f9f74, 0x800fa01c, 0x800fa0dc |
| 0x7 | act4 | 5 | 7 | 0x800f5c00, 0x800f7dc0, 0x800f921c, 0x800f9874, 0x800f9bd4, 0x800f9da4, 0x800fa01c, 0x800fa0dc |
| 0x8 | x | 1 | 1 | 0x800f5f50, 0x800f8dc8 |
| 0xa | x.px | 26 | 5 | 0x800f5c00, 0x800f6418, 0x800f69a0, 0x800f6f9c, 0x800f74b8, 0x800f7938, 0x800f8ecc, 0x800f9628, 0x800f999c, 0x800fa140 |
| 0xc | y | 1 | 1 | 0x800f5f50, 0x800f8dc8 |
| 0xe | y.px | 24 | 4 | 0x800f5c00, 0x800f6418, 0x800f69a0, 0x800f6f9c, 0x800f74b8, 0x800f7938, 0x800f8ecc, 0x800f9628, 0x800f999c, 0x800fa140 |
| 0x14 | layer | 14 | 1 | 0x800f5c00, 0x800f6418, 0x800f69a0, 0x800f6f9c, 0x800f74b8 |
| 0x15 | flip | 11 | 3 | 0x800f5c00, 0x800f6418, 0x800f69a0, 0x800f6f9c, 0x800f74b8, 0x800f9f74 |
| 0x16 | priority | 0 | 3 | 0x800f5c00 |
| 0x18 | prev_x | 1 | 1 | 0x800f5f50, 0x800f8dc8 |
| 0x1c | prev_y | 1 | 1 | 0x800f5f50, 0x800f8dc8 |
| 0x20 | speed_x | 7 | 29 | 0x800f5c00, 0x800f6594, 0x800f6f9c, 0x800f7630, 0x800f7850, 0x800f7938, 0x800f7a3c, 0x800f7b88, 0x800f7c74, 0x800f950c, 0x800f9a8c, 0x800f9b30, 0x800f9bd4, 0x800f9c98, 0x800f9e74 |
| 0x24 | speed_y | 7 | 26 | 0x800f5c00, 0x800f6594, 0x800f6f9c, 0x800f7630, 0x800f7850, 0x800f7938, 0x800f7a3c, 0x800f7b88, 0x800f7c74, 0x800f9a8c, 0x800f9b30, 0x800f9bd4, 0x800f9c98, 0x800f9e74 |
| 0x28 | accel_x | 0 | 5 | 0x800f5c00, 0x800f7938, 0x800f9bd4 |
| 0x2c | gravity | 0 | 5 | 0x800f5c00, 0x800f7938, 0x800f9bd4 |
| 0x30 | anime_table | 14 | 1 | 0x800f5c00, 0x800f6418, 0x800f69a0, 0x800f6f9c, 0x800f74b8 |
| 0x3c | sprite_data | 14 | 1 | 0x800f5c00, 0x800f6418, 0x800f69a0, 0x800f6f9c, 0x800f74b8 |
| 0x40 | tex_cord | 14 | 1 | 0x800f5c00, 0x800f6418, 0x800f69a0, 0x800f6f9c, 0x800f74b8 |
| 0x42 | clut_cord | 22 | 8 | 0x800f5c00, 0x800f5f50, 0x800f6418, 0x800f69a0, 0x800f6f9c, 0x800f74b8, 0x800f8ecc, 0x800f8fb4 |
| 0x45 | anime.flag | 6 | 0 | 0x800f69a0, 0x800f6f9c, 0x800f9410, 0x800f9628, 0x800f96c4, 0x800f97a4 |
| 0x47 | anime.sprite | 0 | 1 | 0x800f5c00 |
| 0x50 | attack_box | 0 | 3 | 0x800f5c00 |
| 0x54 | hurt_box | 0 | 5 | 0x800f5c00, 0x800f6594, 0x800f97f0 |
| 0x58 | damage_table | 0 | 9 | 0x800f5c00, 0x800f5f50, 0x800fa110 |
| 0x5c | hp | 4 | 3 | 0x800f5c00, 0x800f5f50, 0x800f6418, 0x800f9308, 0x800f9f74 |
| 0x60 | contact_damage | 0 | 4 | 0x800f5c00 |
| 0x61 | iframes | 2 | 4 | 0x800f5c00, 0x800f8e60, 0x800f8ecc |
| 0x62 | self+0x62 | 0 | 1 | 0x800f5c00 |
| 0x68 | terrain_box | 0 | 3 | 0x800f5c00 |
| 0x70 | collide_flags | 21 | 0 | 0x800f7850, 0x800f7a3c, 0x800f9584, 0x800f9b30, 0x800f9c98, 0x800f9e74 |
| 0x7c | work.7C | 22 | 47 | 0x800f6254, 0x800f6418, 0x800f6594, 0x800f69a0, 0x800f6f9c, 0x800f74b8, 0x800f7630, 0x800f7850, 0x800f7c74, 0x800f7dc0, 0x800f8e60, 0x800f8ecc, 0x800f90d4, 0x800f9170, 0x800f91b0, 0x800f921c, 0x800f9308, 0x800f93e0, 0x800f9498, 0x800f9628, 0x800f96c4, 0x800f9874, 0x800f999c, 0x800f9a8c, 0x800f9bd4, 0x800f9d3c, 0x800f9e74, 0x800f9f74, 0x800fa01c, 0x800fa0dc |
| 0x7e | work.7E | 5 | 39 | 0x800f6594, 0x800f69a0, 0x800f6f9c, 0x800f7630, 0x800f7c74, 0x800f7dc0, 0x800f8e60, 0x800f8ecc, 0x800f8fb4, 0x800f9628, 0x800f9874, 0x800f9bd4, 0x800f9d3c, 0x800f9e74, 0x800f9f74, 0x800fa01c, 0x800fa0dc |
| 0x80 | work.80 | 2 | 2 | 0x800f6254, 0x800f8ecc, 0x800f8fb4, 0x800f90d4 |
| 0x84 | work.84 | 20 | 0 | 0x800f5f50, 0x800f6418, 0x800f74b8, 0x800f7630, 0x800f7850, 0x800f7938, 0x800f7b88, 0x800f7c74, 0x800f9410, 0x800f9498, 0x800f950c, 0x800f9584, 0x800f9628, 0x800f97f0, 0x800f9874, 0x800f9d3c, 0x800f9da4, 0x800f9e1c, 0x800f9e74 |
| 0x88 | work.88 | 2 | 0 | 0x800f5f50, 0x800f6594 |
| 0x8c | work.8C | 5 | 3 | 0x800f5f50 |
| 0x8d | self+0x8d | 1 | 1 | 0x800f5c00, 0x800f5f50 |
| 0x8e | self+0x8e | 3 | 6 | 0x800f5c00, 0x800f7630, 0x800f7850, 0x800f9874, 0x800fa01c |
| 0x8f | self+0x8f | 11 | 3 | 0x800f6594, 0x800f7630, 0x800f7c74, 0x800fa140 |
| 0x90 | self+0x90 | 1 | 7 | 0x800f7dc0, 0x800fa01c |
| 0x91 | self+0x91 | 1 | 4 | 0x800f7dc0, 0x800fa01c |
| 0x92 | self+0x92 | 1 | 1 | 0x800fa110 |
| 0x93 | self+0x93 | 8 | 30 | 0x800f5c00, 0x800f6594, 0x800f69a0, 0x800f6f9c, 0x800f74b8, 0x800f9628, 0x800f9874, 0x800fa01c |
| 0x94 | self+0x94 | 1 | 1 | 0x800f7938, 0x800f999c |
| 0x96 | self+0x96 | 1 | 1 | 0x800f7938, 0x800f999c |
| 0x9a | self+0x9a | 0 | 2 | 0x800f5c00, 0x800f9f74 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800f5c00, 0x800f7ee8 |
| 0x1f800028 | palette_buffer | 0x800f7ee8, 0x800fa384 |
| 0x8007433c | dmg_tbl_deflect | 0x800f5f50 |
| 0x800757dc |  | 0x800f5c00, 0x800f5f50, 0x800fa110 |
| 0x8007587c |  | 0x800f5c00, 0x800f5f50 |
| 0x800970a0 | mega | 0x800f5f50, 0x800f7ee8, 0x800fa01c |
| 0x800970a5 | mega.var+0x3 | 0x800f90d4 |
| 0x800970aa | mega.x.px | 0x800f85e4 |
| 0x800970ae | mega.y.px | 0x800f6594, 0x800f7dc0, 0x800f85e4 |
| 0x800970b4 | mega.layer | 0x800f5c00 |
| 0x800970b5 | mega.layer+0x1 | 0x800f8e60 |
| 0x800971f8 | camera_layer0 | 0x800f6254, 0x800f90d4, 0x800f921c, 0x800f9308 |
| 0x80097220 | cam.target_max_y | 0x800f5c00 |
| 0x80097420 | frame_count | 0x800f6254, 0x800f6418, 0x800f9170, 0x800f91b0, 0x800f921c, 0x800f999c |
| 0x800c4560 | bg_refresh | 0x800fa384 |
| 0x800cced0 | game | 0x800f6254, 0x800f8fb4, 0x800f9f74 |
| 0x800ccedf | game.exit_req | 0x800f90d4 |
| 0x800ccf38 | game.difficulty | 0x800f74b8 |
| 0x800ccf5c | boss_hp_max | 0x800f9308 |
| 0x800ccf5f | game.boss_level | 0x800f5c00 |
| 0x800fc948 |  | 0x800f5c00 |
| 0x800fc950 |  | 0x800f5c00 |
| 0x800fc954 |  | 0x800f6594 |
| 0x800fc958 |  | 0x800f5c00 |
| 0x800fc970 |  | 0x800f7ee8 |
| 0x800fc97c |  | 0x800f7ee8 |
| 0x800fc980 |  | 0x800f5c00, 0x800f7ee8 |
| 0x800fc984 |  | 0x800f7ee8 |
| 0x800fc988 |  | 0x800f7ee8 |
| 0x800fc98c |  | 0x800f7ee8 |
| 0x800fc990 |  | 0x800f7ee8 |
| 0x800fce98 |  | 0x800f5c00, 0x800f7ee8 |
| 0x800fcf50 |  | 0x800f7ee8 |
| 0x800fd040 |  | 0x800f7b88 |
| 0x800fd280 |  | 0x800f7b88 |
| 0x800fd4e4 |  | 0x800fa01c |
| 0x800fd4f4 |  | 0x800f7ee8 |
| 0x800fd514 |  | 0x800f7ee8 |
| 0x800fd598 |  | 0x800f90d4, 0x800f9308 |
| 0x800fd59a |  | 0x800f5c00, 0x800f69a0, 0x800f6f9c |
| 0x800fd59c |  | 0x800f5c00, 0x800f6594 |
| 0x800fd5a0 |  | 0x800f5c00, 0x800f7630, 0x800f7b88, 0x800f7c74, 0x800f950c |
| 0x800fd5a4 |  | 0x800f5c00, 0x800f9874 |
