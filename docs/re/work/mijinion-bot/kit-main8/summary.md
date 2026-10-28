# RE kit: main8

- Object: main8
- State: `/home/johnlavender/mmx-data/x6/dumps/mijinion-2_illumina-helpers.state.gz` (SHA-1 `d4d3f533583adc4daf98ad9d9cb742eed247cf06`), stage 8 area 0
- Class: main, type 8; entry function 0x800ea488
- Functions walked: 28; state tables: 7

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ea350 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800e9b94, 0x800e9f28, 0x800ea350, 0x800ea84c |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ea500, 0x800eabd4 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800e9b94 |
| 0x8002bbf4 | GetMainObject | free main object slot | 0x800eaa58 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800eab04 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ea630, 0x800eac80 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ea5f0 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ea610 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ea350, 0x800ea500, 0x800eabd4 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ea350, 0x800ea500, 0x800eabd4 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800e9f28, 0x800ea220, 0x800ea650, 0x800ea7f0, 0x800ea84c, 0x800ea924, 0x800eabd4 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ea500, 0x800eabd4 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ea500, 0x800eabd4 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ea500 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ea500 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ea500, 0x800eabd4 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ea500 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800e9f28, 0x800ea350, 0x800ea84c |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800e9f28, 0x800ea350, 0x800ea84c |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800ea350, 0x800ea84c |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fbdb8 | 0x800ea488 | 0x800e9b94, 0x800ea500, 0x800ea5f0, 0x800ea610, 0x800ea630 |
| 0x800fbdcc | 0x800ea500 | 0x80040290, 0x800eaa50, 0x800ea650, 0x800e9d68, 0x800ea758, 0x800ea810, 0x800ea220 |
| 0x800fbde8 | 0x800ea650 | 0x800ea71c, 0x800ea750 |
| 0x800fbdf0 | 0x800ea758 | 0x800e9f28, 0x800ea7f0 |
| 0x800fbdf8 | 0x800ea810 | 0x800ea84c, 0x800ea924 |
| 0x800fbe00 | 0x800ea220 | 0x800ea944, 0x800ea980, 0x800ea9b4 |
| 0x800fbe14 | 0x800ea4c4 | 0x800ea350, 0x800eabd4, 0x800eac80 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 6 | 0x800ea4c4 |

## Animation tables

### Table 0x800fbd98 (7 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 10 11 12 13 14 | 2 2 2 2 2 | 0 0 0 0 0 | 0 |
| 1 | 5 6 7 8 9 0 1 2 3 4 | 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 | 5 |
| 2 | 15 16 17 18 19 20 21 22 23 24 | 2 2 2 2 2 2 2 2 2 2 | 0 0 0 0 0 0 0 0 0 0 | 5 |
| 3 | 25 26 27 28 29 | 2 2 2 2 2 | 0 0 0 0 0 | 0 |
| 4 | 30 | 1 | 0 | hold |
| 5 | 31 | 1 | 0 | hold |
| 6 | 32 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x1 | type | 3 | 0 | 0x800e9b94, 0x800eaa58 |
| 0x2 | var | 9 | 0 | 0x800e9b94, 0x800e9d68, 0x800e9f28, 0x800ea500 |
| 0x3 | display | 0 | 2 | 0x800e9b94 |
| 0x4 | state | 1 | 3 | 0x800e9b94, 0x800e9d68, 0x800ea488, 0x800ea500 |
| 0x5 | step | 1 | 9 | 0x800e9b94, 0x800e9d68, 0x800ea220, 0x800ea500, 0x800ea758 |
| 0x6 | step2 | 9 | 13 | 0x800e9b94, 0x800e9d68, 0x800e9f28, 0x800ea220, 0x800ea650, 0x800ea71c, 0x800ea758, 0x800ea810, 0x800ea84c, 0x800ea944, 0x800ea980, 0x800ea9b4 |
| 0x8 | x | 2 | 0 | 0x800e9b94, 0x800ea500 |
| 0xa | x.px | 12 | 5 | 0x800e9b94, 0x800e9d68, 0x800ea220, 0x800ea650, 0x800ea84c, 0x800eaa58, 0x800eab04, 0x800eaba4 |
| 0xc | y | 2 | 0 | 0x800e9b94, 0x800ea500 |
| 0xe | y.px | 5 | 4 | 0x800e9d68, 0x800ea84c, 0x800ea9b4, 0x800eaa58, 0x800eab04 |
| 0x14 | layer | 2 | 1 | 0x800e9b94, 0x800eaa58, 0x800eab04 |
| 0x15 | flip | 8 | 3 | 0x800e9b94, 0x800e9f28, 0x800ea220, 0x800ea650, 0x800ea71c, 0x800ea944, 0x800eab04, 0x800eaba4 |
| 0x16 | priority | 0 | 1 | 0x800e9b94 |
| 0x18 | prev_x | 0 | 2 | 0x800e9b94, 0x800ea500 |
| 0x1c | prev_y | 0 | 2 | 0x800e9b94, 0x800ea500 |
| 0x20 | speed_x | 0 | 5 | 0x800e9b94, 0x800e9f28, 0x800ea71c, 0x800ea84c, 0x800ea944 |
| 0x24 | speed_y | 1 | 8 | 0x800e9b94, 0x800e9f28, 0x800ea71c, 0x800ea84c, 0x800ea944, 0x800ea9b4 |
| 0x28 | accel_x | 0 | 5 | 0x800e9b94, 0x800e9f28, 0x800ea71c, 0x800ea84c, 0x800ea944 |
| 0x2c | gravity | 0 | 5 | 0x800e9b94, 0x800e9f28, 0x800ea71c, 0x800ea84c, 0x800ea944 |
| 0x30 | anime_table | 2 | 1 | 0x800e9b94, 0x800eaa58, 0x800eab04 |
| 0x3c | sprite_data | 2 | 0 | 0x800eaa58, 0x800eab04 |
| 0x40 | tex_cord | 2 | 0 | 0x800eaa58, 0x800eab04 |
| 0x42 | clut_cord | 2 | 1 | 0x800e9b94, 0x800eaa58, 0x800eab04 |
| 0x50 | attack_box | 0 | 1 | 0x800e9b94 |
| 0x54 | hurt_box | 0 | 1 | 0x800e9b94 |
| 0x58 | damage_table | 0 | 1 | 0x800e9b94 |
| 0x5c | hp | 0 | 1 | 0x800e9b94 |
| 0x60 | contact_damage | 0 | 1 | 0x800e9b94 |
| 0x61 | iframes | 0 | 1 | 0x800e9b94 |
| 0x67 | latch | 0 | 1 | 0x800e9b94 |
| 0x68 | terrain_box | 0 | 1 | 0x800e9b94 |
| 0x7c | work.7C | 2 | 5 | 0x800ea944, 0x800ea980, 0x800ea9b4 |
| 0x80 | work.80 | 2 | 0 | 0x800e9d68, 0x800ea758 |
| 0x84 | work.84 | 0 | 1 | 0x800ea84c |
| 0x88 | work.88 | 2 | 3 | 0x800e9b94, 0x800ea220, 0x800ea650 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f80004c | current_main | 0x800e9b94, 0x800e9d68, 0x800ea220, 0x800ea650, 0x800ea758, 0x800ea84c |
| 0x8007429c | dmg_tbl_one_hit | 0x800ea350 |
| 0x800743dc | dmg_tbl_normal | 0x800e9b94 |
| 0x800970a0 | mega | 0x800ea350, 0x800ea84c, 0x800ea9b4 |
| 0x800970a2 | mega.var | 0x800e9f28 |
| 0x800970aa | mega.x.px | 0x800ea220, 0x800eaba4 |
| 0x800970b4 | mega.layer | 0x800e9b94 |
| 0x80097202 | cam.x | 0x800ea220, 0x800ea650 |
| 0x800ccf38 | game.difficulty | 0x800e9b94 |
| 0x800fbd0c |  | 0x800e9b94 |
| 0x800fbd10 |  | 0x800e9b94 |
| 0x800fbd98 |  | 0x800e9b94 |
| 0x800fbdb4 |  | 0x800ea500 |
| 0x800fbe0c |  | 0x800ea350 |
| 0x800fbe10 |  | 0x800ea350 |
