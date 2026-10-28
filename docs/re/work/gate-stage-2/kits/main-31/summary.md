# RE kit: main type 31

- Object: main type 31
- State: `gate-3_stage-2.state.gz` (SHA-1 `992d5d0609e736bba918340805dd9c642d889217`), stage 17 area 0
- Class: main, type 31; entry function 0x800ea3a0
- Functions walked: 10; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ea21c, 0x800ea3ec, 0x800ea49c, 0x800ea544 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800e9e24 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ea028 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800e9e24 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800e9e24 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ea3ec |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ea028 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ea028 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ea028 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800ea21c, 0x800ea49c, 0x800ea544, 0x800ea6f8 |
| 0x8002d2d4 | GetRNG | random number | 0x800e9e24, 0x800ea028, 0x800ea21c, 0x800ea49c, 0x800ea544, 0x800ea6f8 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ea028 |
| 0x80030ccc | TerrainPointCheck | TerrainPointCheck(obj, x, y): non-zero when the point is solid (rectangle list first, then 0x80030234) | 0x800ea21c |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ea028 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ea028 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800ea028, 0x800ea21c, 0x800ea49c, 0x800ea544, 0x800ea6f8 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ea028, 0x800ea21c, 0x800ea49c, 0x800ea544, 0x800ea6f8 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800ea028 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800ea648 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800ea648 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800ea648 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f8bc0 | 0x800ea3a0 | 0x800e9e24, 0x800ea028, 0x800ea3ec |
| 0x800f8bcc | 0x800ea028 | 0x800ea428, 0x800ea49c, 0x800ea21c, 0x800ea544, 0x800ea648, 0x800ea6f8 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e9864 | 0x800ea2b4 | 0x800ea21c | 28 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800f8b34 (19 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 | 1 | 0 | hold |
| 1 | 1 2 3 4 5 6 7 8 | 6 6 6 6 6 6 6 6 | 0 0 0 0 0 0 0 0 | 0 |
| 2 | 9 | 1 | 0 | hold |
| 3 | 10 | 1 | 0 | hold |
| 4 | 11 | 1 | 0 | hold |
| 5 | 12 | 1 | 0 | hold |
| 6 | 13 | 1 | 0 | hold |
| 7 | 14 | 1 | 0 | hold |
| 8 | 15 | 1 | 0 | hold |
| 9 | 16 | 1 | 0 | hold |
| 10 | 17 | 1 | 0 | hold |
| 11 | 18 | 1 | 0 | hold |
| 12 | 19 | 1 | 0 | hold |
| 13 | 20 | 1 | 0 | hold |
| 14 | 21 | 1 | 0 | hold |
| 15 | 22 | 1 | 0 | hold |
| 16 | 23 | 1 | 0 | hold |
| 17 | 24 | 1 | 0 | hold |
| 18 | 25 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 0 | 1 | 0x800e9e24 |
| 0x2 | var | 4 | 1 | 0x800e9e24, 0x800ea028, 0x800ea428 |
| 0x3 | display | 1 | 2 | 0x800ea028, 0x800ea3ec |
| 0x4 | state | 2 | 6 | 0x800e9e24, 0x800ea028, 0x800ea21c, 0x800ea3a0, 0x800ea49c, 0x800ea544, 0x800ea6f8 |
| 0x5 | step | 0 | 1 | 0x800e9e24 |
| 0x6 | step2 | 2 | 6 | 0x800e9e24, 0x800ea028, 0x800ea21c, 0x800ea428, 0x800ea544, 0x800ea648 |
| 0x8 | x | 2 | 0 | 0x800ea21c, 0x800ea3a0 |
| 0xa | x.px | 2 | 0 | 0x800ea21c, 0x800ea648 |
| 0xc | y | 1 | 0 | 0x800ea3a0 |
| 0xe | y.px | 2 | 0 | 0x800ea21c, 0x800ea648 |
| 0x14 | layer | 0 | 1 | 0x800e9e24 |
| 0x15 | flip | 0 | 4 | 0x800e9e24, 0x800ea21c |
| 0x16 | priority | 0 | 1 | 0x800e9e24 |
| 0x18 | prev_x | 0 | 1 | 0x800ea3a0 |
| 0x1c | prev_y | 0 | 1 | 0x800ea3a0 |
| 0x20 | speed_x | 0 | 5 | 0x800e9e24, 0x800ea21c, 0x800ea648 |
| 0x24 | speed_y | 3 | 7 | 0x800e9e24, 0x800ea028, 0x800ea21c, 0x800ea49c, 0x800ea544, 0x800ea648 |
| 0x28 | accel_x | 0 | 1 | 0x800e9e24 |
| 0x2c | gravity | 0 | 4 | 0x800e9e24, 0x800ea428, 0x800ea544 |
| 0x30 | anime_table | 0 | 1 | 0x800e9e24 |
| 0x3c | sprite_data | 0 | 1 | 0x800e9e24 |
| 0x40 | tex_cord | 0 | 1 | 0x800e9e24 |
| 0x42 | clut_cord | 1 | 2 | 0x800e9e24, 0x800ea028 |
| 0x50 | attack_box | 0 | 2 | 0x800e9e24, 0x800ea428 |
| 0x54 | hurt_box | 0 | 2 | 0x800e9e24, 0x800ea428 |
| 0x58 | damage_table | 0 | 1 | 0x800e9e24 |
| 0x5c | hp | 2 | 1 | 0x800e9e24, 0x800ea028 |
| 0x60 | contact_damage | 0 | 1 | 0x800e9e24 |
| 0x61 | iframes | 0 | 1 | 0x800e9e24 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800e9e24 |
| 0x67 | latch | 0 | 2 | 0x800ea028 |
| 0x68 | terrain_box | 0 | 1 | 0x800e9e24 |
| 0x70 | collide_flags | 6 | 0 | 0x800ea028, 0x800ea21c, 0x800ea49c, 0x800ea544, 0x800ea6f8 |
| 0x80 | work.80 | 1 | 2 | 0x800ea21c, 0x800ea544 |
| 0x88 | work.88 | 2 | 4 | 0x800e9e24, 0x800ea428, 0x800ea648 |
| 0x89 | self+0x89 | 2 | 2 | 0x800e9e24, 0x800ea21c |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800e9e24 |
| 0x800743dc | dmg_tbl_normal | 0x800e9e24 |
| 0x80095098 | bank_tpage | 0x800e9e24 |
| 0x800970a0 | mega | 0x800ea21c, 0x800ea648 |
| 0x800970b4 | mega.layer | 0x800e9e24 |
| 0x80097420 | frame_count | 0x800ea544 |
| 0x800f8b34 |  | 0x800e9e24 |
| 0x800f8b80 |  | 0x800ea428 |
| 0x800f8b90 |  | 0x800ea428 |
| 0x800f8ba0 |  | 0x800e9e24 |
| 0x800f8bb0 |  | 0x800ea028, 0x800ea49c, 0x800ea6f8 |
| 0x800f8bb4 |  | 0x800ea028, 0x800ea49c, 0x800ea6f8 |
| 0x800f8bb8 |  | 0x800ea028, 0x800ea21c, 0x800ea544 |
| 0x800f8bbc |  | 0x800ea028, 0x800ea21c, 0x800ea544 |
