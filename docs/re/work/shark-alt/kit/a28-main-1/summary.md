# RE kit: a28-main-1

- Object: a28-main-1
- State: `shark-10_alt.state.gz` (SHA-1 `114fb5c590494f5a3bc9dc0ee53f3e58ec1371f6`), stage 20 area 1
- Class: main, type 1; entry function 0x800eba48
- Functions walked: 12; state tables: 3

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800eb8a4 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800eb8a4, 0x800eba94, 0x800ebcd4 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800eb8a4, 0x800ebdd8 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800eb8a4 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ebe8c |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ebba4 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ebba4 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800eb724, 0x800ebdd8 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800eb724, 0x800ebdd8 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ebc38 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ebdd8 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800ebdd8 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800eba48 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800eb724, 0x800ebdd8 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800eb724, 0x800ebdd8 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800eb724 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800eb724 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800eb724, 0x800ebdd8 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800eb724 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f2eec | 0x800eba48 | 0x800eba94, 0x800eb724, 0x800ebba4, 0x800ebba4 |
| 0x800f2efc | 0x800eb724 | 0x80040290, 0x800ebbe4, 0x800eb8a4, 0x800ebc38 |
| 0x800f2f14 | 0x800ebeac | 0x800ebcd4, 0x800ebdd8, 0x800ebe8c |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 0 | 0x800ebeac |

## Animation tables

### Table 0x800f2ec8 (8 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 | 1 | 0 | hold |
| 1 | 1 2 3 3 2 1 | 6 4 1 15 6 6 | 0 0 1 0 0 2 | hold |
| 2 | 8 | 1 | 0 | hold |
| 3 | 9 | 1 | 0 | hold |
| 4 | 10 | 1 | 0 | hold |
| 5 | 7 | 1 | 0 | hold |
| 6 | 4 | 1 | 0 | hold |
| 7 | 5 6 7 | 2 2 2 | 0 0 0 | 0 |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 2 | 1 | 0x800eb724, 0x800eba94, 0x800ebbe4 |
| 0x4 | state | 2 | 3 | 0x800eb724, 0x800eba48, 0x800eba94, 0x800ebba4 |
| 0x5 | step | 1 | 6 | 0x800eb724, 0x800eb8a4, 0x800eba94, 0x800ebbe4, 0x800ebc38, 0x800ebc8c |
| 0x6 | step2 | 0 | 3 | 0x800eb8a4, 0x800eba94, 0x800ebc38 |
| 0x8 | x | 4 | 0 | 0x800eb724, 0x800eb8a4, 0x800eba94 |
| 0xa | x.px | 2 | 0 | 0x800eb8a4, 0x800ebbe4 |
| 0xc | y | 3 | 0 | 0x800eb724, 0x800eb8a4, 0x800eba94 |
| 0x14 | layer | 0 | 1 | 0x800eba94 |
| 0x15 | flip | 2 | 4 | 0x800eb724, 0x800eb8a4, 0x800eba94 |
| 0x16 | priority | 0 | 1 | 0x800eba94 |
| 0x18 | prev_x | 0 | 2 | 0x800eb724, 0x800eba94 |
| 0x1c | prev_y | 0 | 2 | 0x800eb724, 0x800eba94 |
| 0x20 | speed_x | 0 | 2 | 0x800eba94, 0x800ebc38 |
| 0x24 | speed_y | 0 | 3 | 0x800eba94, 0x800ebc38, 0x800ebc8c |
| 0x28 | accel_x | 0 | 3 | 0x800eba94, 0x800ebc38, 0x800ebc8c |
| 0x2c | gravity | 0 | 3 | 0x800eba94, 0x800ebc38, 0x800ebc8c |
| 0x30 | anime_table | 1 | 1 | 0x800eb8a4, 0x800eba94 |
| 0x3c | sprite_data | 1 | 0 | 0x800eb8a4 |
| 0x40 | tex_cord | 1 | 0 | 0x800eb8a4 |
| 0x42 | clut_cord | 1 | 0 | 0x800eb8a4 |
| 0x45 | anime.flag | 2 | 0 | 0x800eb8a4 |
| 0x47 | anime.sprite | 1 | 0 | 0x800eb724 |
| 0x50 | attack_box | 0 | 1 | 0x800eba94 |
| 0x54 | hurt_box | 0 | 1 | 0x800eba94 |
| 0x58 | damage_table | 0 | 1 | 0x800eba94 |
| 0x5c | hp | 0 | 1 | 0x800eba94 |
| 0x60 | contact_damage | 0 | 1 | 0x800eba94 |
| 0x61 | iframes | 0 | 1 | 0x800eba94 |
| 0x67 | latch | 1 | 3 | 0x800eba94, 0x800ebc38, 0x800ebc8c |
| 0x68 | terrain_box | 0 | 1 | 0x800eba94 |
| 0x70 | collide_flags | 3 | 1 | 0x800eb724, 0x800eba94, 0x800ebc38, 0x800ebc8c |
| 0x7c | work.7C | 2 | 4 | 0x800eb8a4, 0x800eba94 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x8007433c | dmg_tbl_deflect | 0x800ebcd4 |
| 0x800743dc | dmg_tbl_normal | 0x800eba94 |
| 0x800970a0 | mega | 0x800eba94 |
| 0x800970a8 | mega.x | 0x800eb724 |
| 0x800970aa | mega.x.px | 0x800eb8a4, 0x800ebbe4 |
| 0x800ccf38 | game.difficulty | 0x800eb8a4, 0x800eba94, 0x800ebcd4 |
| 0x800f2e80 |  | 0x800eba94 |
| 0x800f2e84 |  | 0x800eba94 |
| 0x800f2e88 |  | 0x800eba94 |
| 0x800f2ec8 |  | 0x800eba94 |
| 0x800f2ee8 |  | 0x800eb724 |
| 0x800f2f0c |  | 0x800ebcd4 |
| 0x800f2f10 |  | 0x800ebcd4 |
