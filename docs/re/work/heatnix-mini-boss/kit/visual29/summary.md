# RE kit: heatnix-mini-boss-visual29

- Object: heatnix-mini-boss-visual29
- State: `heatnix-2_mini-boss.state.gz` (SHA-1 `7e6ef1a4f80ea529d2cd7abc5e544d1b495ceb54`), stage 3 area 0
- Class: visual, type 29; entry function 0x800eda5c
- Functions walked: 7; state tables: 2

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ec148 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ebf1c |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800ec148, 0x800edab8 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800eda3c |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ec2d0 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800ec2d0 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800edab8 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800eda5c |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800ec2d0 |
| 0x80031474 | ContactWeapon | weapon hits: 0 (also already hit), weapon id + 1, 127 blocked, < 0 killed; dmg_tbl_one_hit only by shots with +0x9C != 0 | 0x800ec2d0 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800ec2d0 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800ebf1c |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800ebf1c |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800ebf1c |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f4008 | 0x800eda5c | 0x800ebf1c, 0x800ec2d0, 0x800eda3c |
| 0x800f4014 | 0x800ec2d0 | 0x800ec148, 0x800edab8, 0x800edae4 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x2 | var | 3 | 0 | 0x800ebf1c, 0x800ec148, 0x800ec2d0 |
| 0x3 | display | 1 | 3 | 0x800ec148 |
| 0x4 | state | 2 | 3 | 0x800ebf1c, 0x800ec2d0, 0x800eda5c |
| 0x5 | step | 2 | 4 | 0x800ebf1c, 0x800ec2d0 |
| 0x6 | step2 | 0 | 1 | 0x800ebf1c |
| 0x8 | x | 1 | 0 | 0x800eda5c |
| 0xa | x.px | 4 | 1 | 0x800ebf1c, 0x800ec148, 0x800ec2d0 |
| 0xc | y | 1 | 0 | 0x800eda5c |
| 0xe | y.px | 2 | 2 | 0x800ebf1c, 0x800ec148, 0x800ec2d0 |
| 0x15 | flip | 0 | 2 | 0x800ebf1c |
| 0x16 | priority | 0 | 1 | 0x800ebf1c |
| 0x18 | prev_x | 0 | 1 | 0x800eda5c |
| 0x1c | prev_y | 0 | 1 | 0x800eda5c |
| 0x20 | speed_x | 0 | 2 | 0x800ebf1c |
| 0x24 | speed_y | 0 | 2 | 0x800ebf1c |
| 0x50 | attack_box | 0 | 4 | 0x800ebf1c |
| 0x54 | hurt_box | 0 | 3 | 0x800ebf1c |
| 0x58 | damage_table | 0 | 1 | 0x800ebf1c |
| 0x5c | hp | 0 | 2 | 0x800ebf1c |
| 0x60 | contact_damage | 0 | 3 | 0x800ebf1c |
| 0x68 | terrain_box | 0 | 2 | 0x800ebf1c |
| 0x70 | collide_flags | 1 | 0 | 0x800ec2d0 |
| 0x7c | work.7C | 2 | 0 | 0x800ec148, 0x800ec2d0 |
| 0x8c | work.8C | 0 | 1 | 0x800ebf1c |
| 0x90 | self+0x90 | 1 | 0 | 0x800ec2d0 |
| 0x92 | self+0x92 | 2 | 2 | 0x800ec2d0 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x8007433c | dmg_tbl_deflect | 0x800ebf1c |
| 0x800743dc | dmg_tbl_normal | 0x800ebf1c |
| 0x800970a0 | mega | 0x800ebf1c |
| 0x800970aa | mega.x.px | 0x800ec2d0 |
| 0x800971f8 | camera_layer0 | 0x800ec2d0 |
| 0x80097420 | frame_count | 0x800ec148, 0x800ec2d0 |
| 0x800e988c |  | 0x800ec148 |
| 0x800f3f0c |  | 0x800ec2d0 |
| 0x800f3ff4 |  | 0x800ebf1c |
| 0x800f3ff8 |  | 0x800ebf1c |
| 0x800f3ffc |  | 0x800ebf1c |
| 0x800f4000 |  | 0x800ebf1c |
| 0x800f4004 |  | 0x800ebf1c |
