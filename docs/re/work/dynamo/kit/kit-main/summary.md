# RE kit: dynamo

- Object: dynamo
- State: `mijinion-4_dynamo.state.gz` (SHA-1 `304866452aea348f0d8d335237bd04c9a2538f7f`), stage 22 area 1
- Class: main, type 63; entry function 0x800fb214
- Functions walked: 52; state tables: 11

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80012d9c | Rumble | controller vibration (likely) | 0x800fa5a0, 0x800fa6ec, 0x800fc09c |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800fa5a0, 0x800fa6ec, 0x800fa984, 0x800fabc0, 0x800fb350, 0x800fb54c, 0x800fbad0, 0x800fbbd0, 0x800fbdb8, 0x800fbe94, 0x800fbf78, 0x800fc09c, 0x800fc1a0, 0x800fc1fc, 0x800fc2f0, 0x800fc6e4, 0x800fc74c |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800fa454, 0x800fa5a0, 0x800fa6ec, 0x800fa984, 0x800fabc0, 0x800fb350, 0x800fb414, 0x800fb6b4, 0x800fb888, 0x800fb8ec, 0x800fb9f8, 0x800fbad0, 0x800fbbd0, 0x800fbc84, 0x800fbd4c, 0x800fbf78, 0x800fc09c, 0x800fc1a0, 0x800fc1fc, 0x800fc2f0, 0x800fc3fc, 0x800fc570, 0x800fc6e4, 0x800fc8cc |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800fabc0, 0x800fb414, 0x800fb54c, 0x800fb650, 0x800fb74c, 0x800fb824, 0x800fba6c, 0x800fbd4c, 0x800fbdb8, 0x800fbe94, 0x800fbf00, 0x800fbf78, 0x800fc038, 0x800fc13c, 0x800fc74c, 0x800fc9ac |
| 0x80021760 | TextOpen | TextOpen(id, unused, mode): queues the id when a box is open; mode 0 saves and clears game+0x24 and +0x1F, loads the text palettes, opens the window; mode != 0: a 60-update wait with no text | 0x800fb414, 0x800fc67c |
| 0x80025fd8 | BossLevel | game+0x8F = 3 in stage 12, else boss_level_tbl[game+0x8D + character] | 0x800fa028 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800fa028 |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x800fa864 |
| 0x8002c3f8 | GetSysObject | first free slot of the stage-logic class (96 x 0x30 at 0x8009B7B0), cleared; used for engine objects 24 and 26 | 0x800fb2b8, 0x800fc470 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800fa454, 0x800fa6ec, 0x800fb8ec |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800fc74c, 0x800fc898, 0x800fca64 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800fa23c, 0x800fabc0, 0x800fb414, 0x800fb4e8, 0x800fb54c, 0x800fb650, 0x800fb74c, 0x800fb824, 0x800fba6c, 0x800fbcf8, 0x800fc038, 0x800fc13c, 0x800fc3fc, 0x800fc470, 0x800fc570, 0x800fc67c, 0x800fc6e4, 0x800fc74c, 0x800fc9ac |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800fabc0, 0x800fb54c, 0x800fb650, 0x800fb74c, 0x800fb824, 0x800fba6c, 0x800fbd4c, 0x800fbdb8, 0x800fbe94, 0x800fbf78, 0x800fc038, 0x800fc13c |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800fb414, 0x800fc74c, 0x800fc9ac |
| 0x8002d2d4 | GetRNG | random number | 0x800fabc0, 0x800fb74c |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800fc7d8 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800fb214 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800fa23c, 0x800fabc0, 0x800fc9ac |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800fa23c, 0x800fabc0 |
| 0x8003d308 | PlayerEvent | PlayerEvent(step, facing): mega+0xD0 = 1, +0xD1 = step, +0xD2 = facing, +0x7A = 1, game.player_lock = 1 | 0x800fa028, 0x800fb2b8, 0x800fb350, 0x800fc3fc |
| 0x8003d330 | PlayerEventEnd | mega+0xD0 = 0, +0x7A = 0, game.player_lock = 0: the event step ends | 0x800fb54c |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800fa23c |
| 0x80040488 | BossMusicRequest | game+0x0E = 1 (a0 not read); was named HpBarShow | 0x800fb4e8 |
| 0x80040498 | BossMusicWait | when stream_state = 0: BossMusicStart and game+0x0E = 0; returns game+0x0E; was named HpBarBusy | 0x800fb54c |
| 0x800533f4 | TextSeen | TextSeen(n): bit n of talk_seen[talk_stage_idx[stage * 2 + area]] | 0x800fb414 |
| 0x80053440 | SetTextSeen | SetTextSeen(n): sets bit n of talk_seen[talk_stage_idx[stage * 2 + area]] | 0x800fb414 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800fa984, 0x800fabc0 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800fa984, 0x800fabc0 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800fa984, 0x800fabc0 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fcd70 | 0x800fb214 | 0x800fa028, 0x800fa23c, 0x800fc3c0 |
| 0x800fcd7c | 0x800fa23c | 0x80040290, 0x800fb274, 0x800fb27c, 0x800fb650, 0x800fb74c, 0x800fb824, 0x800fba6c, 0x800fbcf8, 0x800fc038, 0x800fc13c |
| 0x800fcda4 | 0x800fb27c | 0x800fb2b8, 0x800fb350, 0x800fb414, 0x800fb4e8, 0x800fb54c |
| 0x800fcdb8 | 0x800fb650 | 0x800fb6b4, 0x800fb6f0 |
| 0x800fcdc0 | 0x800fb824 | 0x800fb888, 0x800fb8ec, 0x800fb9f8 |
| 0x800fcdcc | 0x800fba6c | 0x800fbad0, 0x800fbbd0, 0x800fa454, 0x800fbc84 |
| 0x800fcddc | 0x800fbcf8 | 0x800fbd4c, 0x800fbdb8, 0x800fbe94, 0x800fbf00, 0x800fbf78 |
| 0x800fcdf0 | 0x800fc038 | 0x800fa5a0, 0x800fc09c, 0x800fa6ec |
| 0x800fcdfc | 0x800fc13c | 0x800fc1a0, 0x800fa864, 0x800fc1fc, 0x800fc2f0 |
| 0x800fce0c | 0x800fc3c0 | 0x800fc3fc, 0x800fc470, 0x800fc570, 0x800fc67c, 0x800fc6e4, 0x800fc74c |
| 0x800fce30 | 0x800fc7d8 | 0x800fa984, 0x800fabc0, 0x800fc898, 0x800fc8cc, 0x800fc9ac, 0x800fca64 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800fa014 | 0x800fac58 | 0x800fabc0 | 5 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 34 | 0x800fc7d8 |

## Animation tables

### Table 0x800fccd4 (15 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 4 6 9 8 10 7 8 10 7 8 10 7 8 10 7 | 22 4 4 4 4 4 4 4 4 5 5 5 3 3 3 2 2 2 1 1 22 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 1 | 15 11 13 11 | 14 14 14 14 | 0 0 0 0 | 0 |
| 2 | 15 23 24 25 26 27 28 29 30 31 32 134 135 135 | 6 4 2 2 2 2 2 4 1 1 1 2 1 1 | 0 0 0 0 0 0 0 0 0 1 0 0 0 0 | hold |
| 3 | 39 40 41 42 15 15 | 11 6 6 6 24 1 | 0 0 0 0 0 0 | hold |
| 4 | 15 43 44 45 46 47 48 49 50 51 52 53 54 55 18 18 18 18 18 18 18 18 18 18 18 18 18 16 15 15 | 6 6 6 6 6 6 6 6 4 4 4 4 6 6 14 16 6 4 4 4 4 4 4 4 4 4 2 14 6 1 | 0 0 1 0 0 0 0 0 2 0 0 0 0 0 3 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 5 | 18 18 18 18 18 18 18 16 15 15 | 10 12 6 4 4 4 4 14 6 1 | 3 0 0 0 0 0 0 0 0 0 | hold |
| 6 | 73 74 75 | 1 1 1 | 0 0 0 | 0 |
| 7 | 75 78 79 80 81 82 83 84 85 86 88 | 1 2 4 4 4 4 4 4 4 4 20 | 0 0 0 2 0 0 0 1 0 0 0 | hold |
| 8 | 15 89 90 91 92 93 | 21 6 1 1 1 1 | 0 0 0 0 0 0 | 2 |
| 9 | 89 15 | 6 1 | 0 0 | hold |
| 10 | 136 | 1 | 0 | hold |
| 11 | 125 | 1 | 0 | hold |
| 12 | 10 8 7 10 8 7 10 8 7 10 8 9 6 4 5 4 3 2 2 1 0 0 | 1 1 2 2 2 3 3 3 5 5 5 4 4 4 4 4 4 3 1 4 50 1 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 | hold |
| 13 | 98 101 100 99 | 1 1 1 1 | 0 0 0 0 | 0 |
| 14 | 118 119 120 121 122 123 124 | 2 2 2 2 1 1 2 | 0 0 0 0 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x3 | display | 0 | 1 | 0x800fb214 |
| 0x4 | state | 1 | 1 | 0x800fa23c, 0x800fb214 |
| 0x5 | step | 9 | 15 | 0x800fa23c, 0x800fa6ec, 0x800fb54c, 0x800fb6f0, 0x800fb74c, 0x800fb9f8, 0x800fbc84, 0x800fbf78, 0x800fc2f0, 0x800fc3c0, 0x800fc3fc, 0x800fc470, 0x800fc570, 0x800fc67c, 0x800fc6e4 |
| 0x6 | step2 | 28 | 31 | 0x800fa23c, 0x800fa454, 0x800fa5a0, 0x800fa6ec, 0x800fa864, 0x800fb27c, 0x800fb2b8, 0x800fb350, 0x800fb414, 0x800fb4e8, 0x800fb54c, 0x800fb650, 0x800fb6b4, 0x800fb6f0, 0x800fb74c, 0x800fb824, 0x800fb888, 0x800fb8ec, 0x800fb9f8, 0x800fba6c, 0x800fbad0, 0x800fbbd0, 0x800fbc84, 0x800fbcf8, 0x800fbd4c, 0x800fbdb8, 0x800fbe94, 0x800fbf00, 0x800fbf78, 0x800fc038, 0x800fc09c, 0x800fc13c, 0x800fc1a0, 0x800fc1fc, 0x800fc2f0 |
| 0x7 | act4 | 2 | 3 | 0x800fa454, 0x800fb888, 0x800fb9f8, 0x800fbad0, 0x800fbbd0 |
| 0x8 | x | 1 | 0 | 0x800fb214 |
| 0xa | x.px | 20 | 0 | 0x800fa454, 0x800fa5a0, 0x800fa6ec, 0x800fa864, 0x800fb2b8, 0x800fb888, 0x800fb8ec, 0x800fbad0, 0x800fbbd0, 0x800fbd4c, 0x800fbdb8, 0x800fc1fc, 0x800fc2f0, 0x800fc470, 0x800fc570 |
| 0xc | y | 1 | 0 | 0x800fb214 |
| 0xe | y.px | 7 | 2 | 0x800fa864, 0x800fb2b8, 0x800fb414, 0x800fb54c, 0x800fc470, 0x800fc570 |
| 0x15 | flip | 2 | 24 | 0x800fa454, 0x800fa5a0, 0x800fa6ec, 0x800fa864, 0x800fb888, 0x800fb8ec, 0x800fbad0, 0x800fbbd0, 0x800fbd4c, 0x800fbdb8, 0x800fbf00, 0x800fc1fc, 0x800fc2f0, 0x800fc570 |
| 0x18 | prev_x | 0 | 1 | 0x800fb214 |
| 0x1c | prev_y | 0 | 1 | 0x800fb214 |
| 0x20 | speed_x | 1 | 13 | 0x800fa5a0, 0x800fa864, 0x800fbad0, 0x800fbbd0, 0x800fbdb8, 0x800fbf00, 0x800fbf78, 0x800fc09c, 0x800fc1a0, 0x800fc1fc, 0x800fc2f0, 0x800fc570 |
| 0x24 | speed_y | 4 | 14 | 0x800fa5a0, 0x800fb350, 0x800fb414, 0x800fbad0, 0x800fbbd0, 0x800fbdb8, 0x800fbf00, 0x800fbf78, 0x800fc09c, 0x800fc1a0, 0x800fc1fc, 0x800fc2f0, 0x800fc570, 0x800fc74c |
| 0x28 | accel_x | 0 | 1 | 0x800fc1a0 |
| 0x2c | gravity | 0 | 11 | 0x800fa5a0, 0x800fa864, 0x800fbad0, 0x800fbbd0, 0x800fbdb8, 0x800fbf00, 0x800fbf78, 0x800fc09c, 0x800fc1a0, 0x800fc1fc, 0x800fc2f0 |
| 0x42 | clut_cord | 8 | 7 | 0x800fa23c, 0x800fc3fc, 0x800fc470, 0x800fc570 |
| 0x45 | anime.flag | 7 | 2 | 0x800fa6ec, 0x800fb8ec, 0x800fbdb8, 0x800fbe94, 0x800fbf00, 0x800fc74c |
| 0x46 | anime.next | 9 | 0 | 0x800fa6ec, 0x800fb414, 0x800fb8ec, 0x800fbbd0, 0x800fbc84, 0x800fbf78, 0x800fc09c, 0x800fc2f0, 0x800fc74c |
| 0x47 | anime.sprite | 2 | 0 | 0x800fa23c |
| 0x50 | attack_box | 0 | 1 | 0x800fa23c |
| 0x54 | hurt_box | 0 | 1 | 0x800fa23c |
| 0x58 | damage_table | 0 | 3 | 0x800fa23c, 0x800fa864 |
| 0x5c | hp | 3 | 1 | 0x800fb54c, 0x800fb74c |
| 0x61 | iframes | 2 | 3 | 0x800fc3fc, 0x800fc470 |
| 0x68 | terrain_box | 1 | 2 | 0x800fb414, 0x800fc570 |
| 0x70 | collide_flags | 6 | 0 | 0x800fb414, 0x800fbbd0, 0x800fbf78, 0x800fc09c, 0x800fc1fc, 0x800fc2f0 |
| 0x7c | work.7C | 8 | 17 | 0x800fa454, 0x800fa5a0, 0x800fa6ec, 0x800fa864, 0x800fb2b8, 0x800fb350, 0x800fb414, 0x800fb4e8, 0x800fb54c, 0x800fbad0, 0x800fbbd0, 0x800fc09c, 0x800fc1a0, 0x800fc3fc, 0x800fc470 |
| 0x7e | work.7E | 7 | 10 | 0x800fa454, 0x800fb888, 0x800fb8ec, 0x800fbad0, 0x800fbbd0, 0x800fc3fc, 0x800fc470, 0x800fc570 |
| 0x80 | work.80 | 3 | 2 | 0x800fb2b8, 0x800fb350, 0x800fc470, 0x800fc570, 0x800fc67c |
| 0x84 | work.84 | 1 | 2 | 0x800fa23c, 0x800fa864 |
| 0x88 | work.88 | 5 | 7 | 0x800fa23c, 0x800fa864, 0x800fc1fc, 0x800fc2f0 |
| 0x89 | self+0x89 | 1 | 2 | 0x800fb6b4, 0x800fb6f0 |
| 0x8a | work.8A | 6 | 2 | 0x800fa6ec, 0x800fb6f0, 0x800fb74c, 0x800fb9f8, 0x800fbc84, 0x800fbf78 |
| 0x8b | self+0x8b | 7 | 7 | 0x800fa6ec, 0x800fb6f0, 0x800fb74c, 0x800fb9f8, 0x800fbc84, 0x800fbf78 |
| 0x94 | self+0x94 | 1 | 1 | 0x800fb350, 0x800fb54c |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800fa028 |
| 0x8007433c | dmg_tbl_deflect | 0x800fa23c, 0x800fa864 |
| 0x80075a5c |  | 0x800fa028, 0x800fa23c |
| 0x80075afc |  | 0x800fa984 |
| 0x8008eafc | text_open | 0x800fb4e8, 0x800fc6e4 |
| 0x800970a0 | mega | 0x800fa454, 0x800fabc0, 0x800fc570 |
| 0x800970a5 | mega.var+0x3 | 0x800fb2b8 |
| 0x800970aa | mega.x.px | 0x800fa5a0, 0x800fb888, 0x800fb8ec, 0x800fbad0, 0x800fbbd0, 0x800fbd4c, 0x800fbdb8, 0x800fc2f0 |
| 0x800970b4 | mega.layer | 0x800fa028 |
| 0x800970b5 | mega.layer+0x1 | 0x800fc3fc |
| 0x8009711a | mega.spike+0x1 | 0x800fa23c |
| 0x800971f8 | camera_layer0 | 0x800fa028, 0x800fb350, 0x800fb54c |
| 0x8009723f | cam.limit_speed | 0x800fb2b8 |
| 0x800cced0 | game | 0x800fa028, 0x800fa864 |
| 0x800ccedf | game.exit_req | 0x800fb350, 0x800fc74c |
| 0x800ccef4 | game.boss_bar | 0x800fb4e8 |
| 0x800ccf5c | boss_hp_max | 0x800fb54c, 0x800fb74c |
| 0x800ccf5f | game.boss_level | 0x800fa454, 0x800fb8ec |
| 0x800fa004 |  | 0x800fa028 |
| 0x800fa00c |  | 0x800fb8ec |
| 0x800fa010 |  | 0x800fa454 |
| 0x800fca84 |  | 0x800fb414 |
| 0x800fca88 |  | 0x800fa028, 0x800fa23c |
| 0x800fca8c |  | 0x800fa23c |
| 0x800fca90 |  | 0x800fa23c |
| 0x800fca94 |  | 0x800fa028, 0x800fa23c |
| 0x800fca98 |  | 0x800fa23c |
| 0x800fccd4 |  | 0x800fa028 |
| 0x800fcd10 |  | 0x800fa6ec, 0x800fb6f0, 0x800fb74c, 0x800fb9f8, 0x800fbc84, 0x800fbf78 |
| 0x800fce24 |  | 0x800fa984 |
| 0x800fce28 |  | 0x800fa984 |
| 0x800fce2c |  | 0x800fc9ac |
| 0x800fce4c |  | 0x800fa028, 0x800fa5a0, 0x800fabc0, 0x800fc1fc |
| 0x800fce4e |  | 0x800fa5a0, 0x800fabc0, 0x800fc1fc |
| 0x800fce50 |  | 0x800fa028, 0x800fa5a0, 0x800fbad0, 0x800fc1fc |
| 0x800fce52 |  | 0x800fa028, 0x800fabc0 |
| 0x800fce54 |  | 0x800fa028, 0x800fabc0 |
| 0x800fce58 |  | 0x800fa028, 0x800fa23c, 0x800fabc0, 0x800fc9ac |
| 0x800fce5c |  | 0x800fa028, 0x800fa23c, 0x800fabc0, 0x800fc2f0, 0x800fc9ac |
| 0x800fce60 |  | 0x800fa028, 0x800fa984, 0x800fabc0, 0x800fc898, 0x800fc8cc |
| 0x800fce64 |  | 0x800fb54c, 0x800fc570 |
