# RE kit: main type 2

- Object: main type 2
- State: `gate-4_before-gate.state.gz` (SHA-1 `f51fcc1d7dd4d15662dc4a3ecd18beb124cd7419`), stage 18 area 0
- Class: main, type 2; entry function 0x800ea3b0
- Functions walked: 18; state tables: 3

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ea464, 0x800ea554 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800e9bdc, 0x800e9f18, 0x800ea218 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800e9f18, 0x800ea464, 0x800ea554, 0x800ea810 |
| 0x8002c530 | GetMiscObject | free misc object (debris) | 0x800e9bdc |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ea218, 0x800ea810 |
| 0x8002ca14 | DeleteObject | record.flags &= 0x70, then DeleteObject3 | 0x800ea424 |
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800ea444 |
| 0x8002cb50 | DisplayCheck1 | display = 1 if inside [-32, W+32) x [-32, 272) | 0x800ea7d0 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800e9d70, 0x800ea218, 0x800ea810 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800e9d70, 0x800ea218, 0x800ea810 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ea3b0 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800e9d70 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800e9d70 |
| 0x80040290 | HitReactionDispatch | engine sub-state 0: hit reaction by +0x63 through 0x80011574 | 0x800e9d70 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800e9d70 |
| 0x80049ebc | SpawnExplosionRand | SpawnExplosionRand(obj, type, mx, my): explosion at a random offset | 0x800e9d70 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800e9d70 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f664c | 0x800ea3b0 | 0x800e9bdc, 0x800e9d70, 0x800ea424, 0x800ea444 |
| 0x800f665c | 0x800e9d70 | 0x80040290, 0x800ea464, 0x800ea554, 0x800ea62c |
| 0x800f666c | 0x800ea62c | 0x800ea668, 0x800ea680, 0x800e9f18 |

## Switch tables

| table | jr at | function | cases |
|---|---|---|---|
| 0x800e9864 | 0x800e9f94 | 0x800e9f18 | 8 |
| 0x800e9884 | 0x800ea03c | 0x800e9f18 | 8 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| misc | 2 | 0x800ea778 |

## Animation tables

### Table 0x800f661c (11 animations)

Found by the code search (no live object uses it).

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|
| 0 | 0 1 2 3 4 5 6 7 | 4 4 4 4 4 4 4 4 | 0 0 0 0 0 0 0 0 | 0 |
| 1 | 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 | 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 2 | 7 6 5 4 3 2 1 0 | 4 4 4 4 4 4 4 4 | 0 0 0 0 0 0 0 0 | 0 |
| 3 | 8 9 10 11 20 21 22 23 16 17 18 19 12 13 14 15 | 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 | 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 | 0 |
| 4 | 8 9 10 11 10 9 8 8 | 4 3 2 1 2 3 3 1 | 0 0 0 0 0 0 0 1 | hold |
| 5 | 12 13 14 15 14 13 12 12 | 4 3 2 1 2 3 3 1 | 0 0 0 0 0 0 0 1 | hold |
| 6 | 16 17 18 19 18 17 16 16 | 4 3 2 1 2 3 3 1 | 0 0 0 0 0 0 0 1 | hold |
| 7 | 20 21 22 23 22 21 20 20 | 4 3 2 1 2 3 3 1 | 0 0 0 0 0 0 0 1 | hold |
| 8 | 24 | 1 | 0 | hold |
| 9 | 25 | 1 | 0 | hold |
| 10 | 26 | 1 | 0 | hold |

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x1 | type | 1 | 0 | 0x800ea6f4 |
| 0x2 | var | 1 | 0 | 0x800e9bdc |
| 0x4 | state | 1 | 3 | 0x800e9bdc, 0x800e9d70, 0x800ea3b0 |
| 0x5 | step | 1 | 5 | 0x800e9bdc, 0x800e9d70, 0x800e9f18, 0x800ea464, 0x800ea554, 0x800ea6b4 |
| 0x6 | step2 | 3 | 7 | 0x800e9bdc, 0x800e9f18, 0x800ea464, 0x800ea554, 0x800ea62c, 0x800ea668, 0x800ea680, 0x800ea6b4 |
| 0x7 | act4 | 5 | 4 | 0x800e9bdc, 0x800e9d70, 0x800e9f18, 0x800ea464, 0x800ea554 |
| 0x8 | x | 5 | 3 | 0x800e9bdc, 0x800ea3b0, 0x800ea464, 0x800ea554 |
| 0xa | x.px | 2 | 0 | 0x800ea0b0 |
| 0xc | y | 4 | 2 | 0x800e9bdc, 0x800ea3b0, 0x800ea464, 0x800ea554 |
| 0xe | y.px | 2 | 0 | 0x800e9bdc, 0x800ea0b0 |
| 0x14 | layer | 1 | 1 | 0x800e9bdc |
| 0x15 | flip | 1 | 1 | 0x800e9bdc |
| 0x16 | priority | 0 | 1 | 0x800e9bdc |
| 0x18 | prev_x | 0 | 2 | 0x800e9bdc, 0x800ea3b0 |
| 0x1c | prev_y | 0 | 2 | 0x800e9bdc, 0x800ea3b0 |
| 0x20 | speed_x | 4 | 5 | 0x800e9bdc, 0x800e9d70, 0x800ea464, 0x800ea554 |
| 0x24 | speed_y | 5 | 8 | 0x800e9bdc, 0x800e9d70, 0x800ea3b0, 0x800ea464, 0x800ea554, 0x800ea6b4 |
| 0x28 | accel_x | 4 | 5 | 0x800e9bdc, 0x800e9f18, 0x800ea464, 0x800ea554 |
| 0x2c | gravity | 2 | 1 | 0x800e9bdc, 0x800ea464, 0x800ea554 |
| 0x30 | anime_table | 1 | 1 | 0x800e9bdc |
| 0x3c | sprite_data | 1 | 0 | 0x800e9bdc |
| 0x40 | tex_cord | 1 | 0 | 0x800e9bdc |
| 0x42 | clut_cord | 1 | 0 | 0x800e9bdc |
| 0x47 | anime.sprite | 3 | 0 | 0x800e9bdc, 0x800e9d70 |
| 0x50 | attack_box | 0 | 1 | 0x800e9bdc |
| 0x54 | hurt_box | 0 | 1 | 0x800e9bdc |
| 0x58 | damage_table | 0 | 1 | 0x800e9bdc |
| 0x5c | hp | 0 | 1 | 0x800e9bdc |
| 0x60 | contact_damage | 0 | 1 | 0x800e9bdc |
| 0x61 | iframes | 0 | 1 | 0x800e9bdc |
| 0x67 | latch | 3 | 5 | 0x800e9bdc, 0x800e9d70, 0x800ea0b0, 0x800ea554, 0x800ea6b4 |
| 0x68 | terrain_box | 2 | 1 | 0x800e9bdc, 0x800ea0b0 |
| 0x70 | collide_flags | 10 | 3 | 0x800e9d70, 0x800ea0b0, 0x800ea3b0, 0x800ea464, 0x800ea554, 0x800ea6b4 |
| 0x7c | work.7C | 2 | 5 | 0x800e9bdc, 0x800e9f18, 0x800ea668, 0x800ea680 |
| 0x80 | work.80 | 2 | 2 | 0x800e9bdc, 0x800e9d70, 0x800e9f18 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x8007447c | dmg_tbl_sabre_only | 0x800e9bdc |
| 0x80090c88 | main_objects_end | 0x800ea6f4 |
| 0x800970b4 | mega.layer | 0x800e9bdc |
| 0x8009719c | fall_max | 0x800ea464, 0x800ea554 |
| 0x800f64c4 |  | 0x800e9bdc |
| 0x800f64c8 |  | 0x800e9bdc |
| 0x800f64cc |  | 0x800e9bdc |
| 0x800f661c |  | 0x800e9bdc |
| 0x800f6648 |  | 0x800e9d70 |
| 0x800f6678 |  | 0x800ea218 |
