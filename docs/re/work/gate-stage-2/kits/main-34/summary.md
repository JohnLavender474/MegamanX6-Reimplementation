# RE kit: main type 34

- Object: main type 34
- State: `gate-3_stage-2.state.gz` (SHA-1 `992d5d0609e736bba918340805dd9c642d889217`), stage 17 area 0
- Class: main, type 34; entry function 0x800eaf7c
- Functions walked: 42; state tables: 10

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800eb1ec, 0x800ebd5c |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ea778, 0x800ea9f4, 0x800eab5c, 0x800eacb8, 0x800eb1ec, 0x800eb2f4, 0x800eb404, 0x800eb540, 0x800eba64 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800eab5c, 0x800eb0dc, 0x800eb148, 0x800eb1ec, 0x800eb2f4, 0x800eb404, 0x800eb460, 0x800eb4bc, 0x800eb540, 0x800eb584, 0x800eb5d4, 0x800ebab4, 0x800ebb80 |
| 0x8002c398 | GetEffectObject | first free of 96 effect slots of 0x70 at 0x80091298, cleared | 0x800eb980 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800eb684, 0x800eb780, 0x800eb854 |
| 0x8002c530 | GetMiscObject | free misc object (debris) | 0x800eae14 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800eb958, 0x800ebb24, 0x800ebe1c |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800eb054 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800eb054 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ea8ec, 0x800eab5c, 0x800ebb80 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ea8ec, 0x800eab5c, 0x800ebab4, 0x800ebb80 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800eab5c, 0x800eb5d4, 0x800ebb80 |
| 0x8002d2d4 | GetRNG | random number | 0x800eae14 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800eab5c |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800eaf7c, 0x800ebb80 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ea8ec, 0x800eab5c, 0x800ebb80 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ea8ec, 0x800eab5c, 0x800ebb80 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800ea8ec |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ea8ec |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800eab5c, 0x800ebb80 |
| 0x80049ebc | SpawnExplosionRand | SpawnExplosionRand(obj, type, mx, my): explosion at a random offset | 0x800eafc8 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800eafc8 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f8d18 | 0x800eaf7c | 0x800ea778, 0x800ea8ec, 0x800eafc8, 0x800eb054, 0x800eb054 |
| 0x800f8d2c | 0x800ea8ec | 0x80040290, 0x800eb094, 0x800eb0a0, 0x800eb1b0, 0x800eb3c8, 0x800eb504, 0x800eb5d4 |
| 0x800f8d48 | 0x800eb0a0 | 0x800eb0dc, 0x800eb148 |
| 0x800f8d50 | 0x800eb1b0 | 0x800eb1ec, 0x800eb2f4 |
| 0x800f8d58 | 0x800eb3c8 | 0x800eb404, 0x800eb460, 0x800eb4bc |
| 0x800f8d64 | 0x800eb504 | 0x800eb540, 0x800eb584 |
| 0x800f8d8c | 0x800eb91c | 0x800ea9f4, 0x800eab5c, 0x800eb958, 0x800eb978 |
| 0x800f8d9c | 0x800eba28 | 0x800eba64, 0x800ebab4, 0x800ebb24 |
| 0x800f8dc8 | 0x800ebb44 | 0x800eacb8, 0x800ebb80, 0x800ebe1c, 0x800ebe3c |
| 0x800f8dd8 | 0x800ebb80 | 0x800ebca4, 0x800ebd5c, 0x800eae14 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 37 | 0x800eb91c |
| visual | 38 | 0x800ebb44 |
| effect | 21 | 0x800eba28 |

## Animation tables

### Table 0x800f8cd8 (14 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 0 2 | 21 22 21 20 | 0 0 0 0 | 0 |
| 1 | 0 3 4 5 | 1 2 4 7 | 0 0 0 0 | hold |
| 2 | 0 6 7 5 | 1 2 4 7 | 0 0 0 0 | hold |
| 3 | 8 9 10 11 12 13 | 4 4 4 4 6 8 | 0 0 0 0 0 0 | hold |
| 4 | 14 15 16 17 | 2 2 2 2 | 0 0 0 0 | 0 |
| 5 | 0 18 19 18 0 20 21 20 21 20 21 20 21 22 | 5 6 7 6 7 2 2 1 1 1 1 1 1 10 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | hold |
| 6 | 22 23 24 25 26 27 0 | 4 4 4 5 3 3 3 | 0 0 0 0 0 0 0 | hold |
| 7 | 28 29 30 31 | 1 1 1 1 | 0 0 0 0 | 0 |
| 8 | 32 | 1 | 0 | hold |
| 9 | 33 | 1 | 0 | hold |
| 10 | 34 | 1 | 0 | hold |
| 11 | 35 | 1 | 0 | hold |
| 12 | 36 | 1 | 0 | hold |
| 13 | 37 38 39 40 41 | 3 3 3 3 3 | 0 0 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800ea778 |
| 0x2 | var | 8 | 0 | 0x800ea778, 0x800eb0dc, 0x800eb1ec, 0x800eb2f4, 0x800eb684 |
| 0x3 | display | 0 | 1 | 0x800ea8ec |
| 0x4 | state | 5 | 4 | 0x800ea778, 0x800ea8ec, 0x800eaf7c, 0x800eafc8, 0x800eb054 |
| 0x5 | step | 1 | 10 | 0x800ea778, 0x800ea8ec, 0x800eb094, 0x800eb0dc, 0x800eb148, 0x800eb2f4, 0x800eb4bc, 0x800eb584, 0x800eb5d4, 0x800eb634 |
| 0x6 | step2 | 9 | 13 | 0x800ea778, 0x800eb0a0, 0x800eb0dc, 0x800eb148, 0x800eb1b0, 0x800eb1ec, 0x800eb2f4, 0x800eb3c8, 0x800eb404, 0x800eb460, 0x800eb4bc, 0x800eb504, 0x800eb540, 0x800eb584, 0x800eb5d4, 0x800eb634 |
| 0x8 | x | 2 | 0 | 0x800ea778, 0x800ea8ec |
| 0xa | x.px | 1 | 0 | 0x800eb148 |
| 0xc | y | 2 | 0 | 0x800ea778, 0x800ea8ec |
| 0x14 | layer | 3 | 1 | 0x800ea778, 0x800eb684, 0x800eb780, 0x800eb854 |
| 0x15 | flip | 3 | 2 | 0x800ea778, 0x800eb684, 0x800eb780, 0x800eb854 |
| 0x16 | priority | 0 | 1 | 0x800ea778 |
| 0x18 | prev_x | 0 | 2 | 0x800ea778, 0x800ea8ec |
| 0x1c | prev_y | 0 | 2 | 0x800ea778, 0x800ea8ec |
| 0x20 | speed_x | 0 | 3 | 0x800ea778, 0x800eb5d4, 0x800eb634 |
| 0x24 | speed_y | 0 | 3 | 0x800ea778, 0x800eb5d4, 0x800eb634 |
| 0x28 | accel_x | 0 | 3 | 0x800ea778, 0x800eb5d4, 0x800eb634 |
| 0x2c | gravity | 0 | 3 | 0x800ea778, 0x800eb5d4, 0x800eb634 |
| 0x30 | anime_table | 0 | 1 | 0x800ea778 |
| 0x3c | sprite_data | 3 | 0 | 0x800eb684, 0x800eb780, 0x800eb854 |
| 0x40 | tex_cord | 3 | 0 | 0x800eb684, 0x800eb780, 0x800eb854 |
| 0x42 | clut_cord | 3 | 0 | 0x800eb684, 0x800eb780, 0x800eb854 |
| 0x50 | attack_box | 0 | 1 | 0x800ea778 |
| 0x54 | hurt_box | 0 | 1 | 0x800ea778 |
| 0x58 | damage_table | 0 | 1 | 0x800ea778 |
| 0x5c | hp | 0 | 1 | 0x800ea778 |
| 0x60 | contact_damage | 0 | 1 | 0x800ea778 |
| 0x61 | iframes | 0 | 1 | 0x800ea778 |
| 0x67 | latch | 1 | 3 | 0x800ea778, 0x800eb5d4, 0x800eb634 |
| 0x68 | terrain_box | 0 | 1 | 0x800ea778 |
| 0x70 | collide_flags | 3 | 0 | 0x800ea8ec, 0x800eb5d4, 0x800eb634 |
| 0x7c | work.7C | 7 | 18 | 0x800ea778, 0x800ea8ec, 0x800eafc8, 0x800eb0dc, 0x800eb1ec, 0x800eb2f4, 0x800eb404, 0x800eb460, 0x800eb4bc, 0x800eb540, 0x800eb584 |
| 0x7e | work.7E | 1 | 3 | 0x800ea8ec, 0x800eafc8 |
| 0x80 | work.80 | 0 | 1 | 0x800ea778 |
| 0x84 | work.84 | 7 | 4 | 0x800ea778, 0x800eb1ec, 0x800eb2f4, 0x800eb684 |
| 0x88 | work.88 | 0 | 1 | 0x800ea778 |
| 0x8c | work.8C | 0 | 1 | 0x800ea778 |
| 0x90 | self+0x90 | 0 | 1 | 0x800ea778 |
| 0x94 | self+0x94 | 1 | 1 | 0x800ea778, 0x800eb094 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x800741fc | dmg_tbl_minor | 0x800eacb8 |
| 0x8007433c | dmg_tbl_deflect | 0x800ea9f4 |
| 0x800743dc | dmg_tbl_normal | 0x800ea778 |
| 0x800970a0 | mega | 0x800ea778, 0x800eae14 |
| 0x800970aa | mega.x.px | 0x800eb148 |
| 0x800cced0 | game | 0x800eb1ec |
| 0x800ccf08 | game.character | 0x800eb2f4, 0x800eb684 |
| 0x800f8be4 |  | 0x800ea778 |
| 0x800f8be8 |  | 0x800ea778 |
| 0x800f8bec |  | 0x800ea778 |
| 0x800f8bf0 |  | 0x800eb684 |
| 0x800f8cd8 |  | 0x800ea778, 0x800eb684, 0x800eb780, 0x800eb854 |
| 0x800f8d10 |  | 0x800ea8ec |
| 0x800f8d74 |  | 0x800ea9f4 |
| 0x800f8d7c |  | 0x800eab5c |
| 0x800f8dac |  | 0x800eacb8 |
| 0x800f8db0 |  | 0x800ebca4 |
| 0x800f8db8 |  | 0x800ebca4 |
