# RE kit: s5-stage-3

- Object: s5-stage-3
- State: `scaravich-1_start.state.gz` (SHA-1 `4b9cab59f2dd853bd5034ef66217cebb0de9b9bb`), stage 5 area 0
- Class: stage, type 3; entry function 0x800ec5ac
- Functions walked: 28; state tables: 6

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x800139a0 | FadeIn | FadeIn(speed): fade_dir = +1 | 0x800ebea0 |
| 0x800139f4 | FadeOut | FadeOut(speed): fade_dir = -1 | 0x800eb398 |
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): sound effect request | 0x800eac90, 0x800eb098, 0x800eb1a0, 0x800eb298, 0x800ebea0, 0x800ec724, 0x800ec7f0, 0x800ec9b4, 0x800ecb2c, 0x800ecca0 |
| 0x800179a4 | SetAnime | SetAnime(obj, n): start animation n of obj+0x30; load frame to +0x44 | 0x800ea820, 0x800eac90, 0x800ec41c, 0x800ec724, 0x800ec7f0, 0x800ec8bc, 0x800ecbc4 |
| 0x80017a04 | AnimeTick | duration - 1; at 0 move by next (relative) and load the frame | 0x800eb098, 0x800eb1a0, 0x800ec724, 0x800ec7f0, 0x800ec8bc, 0x800ec9b4, 0x800ecbc4, 0x800ecca0, 0x800ecdc4 |
| 0x80027ba0 | CopyTiles | CopyTiles(layer, dx, dy, src{x, y, w, h}): CopyCell for each cell | 0x800eb398 |
| 0x80029f38 | SpawnInRect | SpawnInRect(x0, x1, y0, y1, flag): spawn the records inside a rectangle | 0x800ebea0 |
| 0x8002a47c | GetSpriteBank | index of an object type in the stage type list | 0x800ea820 |
| 0x8002b36c | ResetLayerCache | ResetLayerCache(layer): redraw state of a BG layer | 0x800ebea0 |
| 0x8002b6b0 | ClearPlayerShots | delete the weapon objects except type 55, and flagged effects | 0x800eb298, 0x800ebea0 |
| 0x8002c398 | GetEffectObject | first free of 96 effect slots of 0x70 at 0x80091298, cleared | 0x800eaef8 |
| 0x8002c458 | GetVisualObject | first free of 32 x 0xA0 slots at 0x80093C98, cleared | 0x800ec9b4 |
| 0x8002c9b0 | DeleteObject3 | zero bytes 0..7 of the object | 0x800ec614, 0x800ec6bc, 0x800ecda4, 0x800ecee8 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800ecdc4 |
| 0x8002ccb0 | DisplayCheck | DisplayCheck(obj, rx, ry): display = 1 if inside [-rx, W+rx) x [-ry, 240+ry) | 0x800eac90, 0x800ec614, 0x800ecdc4 |
| 0x8002d230 | MoveObject2 | x += vx; y -= vy; vx += facing accel; vy -= gravity; clamp vy | 0x800eb098, 0x800ecbc4, 0x800ecca0 |
| 0x8002d2b0 | MoveObject | x += vx; y -= vy | 0x800ecdc4 |
| 0x8002d2d4 | GetRNG | random number | 0x800eb398 |
| 0x8002d654 | BoxOverlap | box test between two objects, facing-aware | 0x800eb298 |
| 0x8002ee10 | CornerCheck | test the 4 corners of the terrain box against the tile map | 0x800ecdc4 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800ec614 |
| 0x80030ecc | ContactMega | attack box vs the player; applies contact_damage | 0x800eac90, 0x800ecdc4 |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800eac90, 0x800ec614, 0x800ecdc4 |
| 0x80031da8 | SolidObject | solid object vs the player: push the player out of the terrain box and carry it | 0x800eac90, 0x800ec614 |
| 0x8003ad18 | ToStand | clear the dash flags, step 2, the stand animation (94 when shooting) | 0x800ec2f0 |
| 0x8003c4ec | PlayerPaletteRestore | the palettes of the weapon slot mega+0x93 and of the part bits | 0x800eb298 |
| 0x8003cdf0 | ClearActionFlags | mega+0x84, +0x86, +0x8C, +0x8E..+0x92, +0xA2 = 0; palette | 0x800eb298 |
| 0x8003ce30 | ClearChargeGlow | mega+0xA1 = 0 | 0x800eb298 |
| 0x8003f3dc | ChargeStop | X: stop the charge (palette by mega+0x93) (likely) | 0x800eb298 |
| 0x800477e4 | SpawnJunk | SpawnJunk(n, anims, obj): n misc debris objects; anims = list of animation numbers | 0x800eac90, 0x800eaef8, 0x800ec614 |
| 0x80049d90 | SpawnHitEffect | effect (flags 0x21, type 3, var 1) at the object, mirrored when speed_x > 0; an enemy shot hitting a wall (likely) | 0x800ecdc4 |
| 0x80049e50 | SpawnExplosion | explosion effect at the object, random variant | 0x800eac90, 0x800ecdc4 |
| 0x80049ebc | SpawnExplosionRand | SpawnExplosionRand(obj, type, mx, my): explosion at a random offset | 0x800ec614 |
| 0x8004e654 | DropItem | DropItem(obj, table): difficulty < 2 only; weighted item roll | 0x800eaef8 |
| 0x80065944 | rsin | Psy-Q GTE sine (4096 = 1.0) | 0x800eb1a0, 0x800ec41c, 0x800ecbc4 |
| 0x80065a14 | rcos | Psy-Q GTE cosine (4096 = 1.0) | 0x800eb1a0, 0x800ec41c, 0x800ecbc4 |
| 0x80065b74 | ratan2 | Psy-Q GTE arc tangent | 0x800eb1a0, 0x800ec41c, 0x800ecbc4 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x8002bfd0 | 0x800ea820 |
| 0x8002d30c | 0x800eaef8 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f5600 | 0x800ec5ac | 0x800ea820, 0x800eac90, 0x800ec614, 0x800ec6bc |
| 0x800f5610 | 0x800eac90 | 0x800eaef8, 0x800ec6dc, 0x800ecab4, 0x800ecaf0 |
| 0x800f5620 | 0x800ec6dc | 0x800ec724, 0x800ec7f0, 0x800ec8bc, 0x800ec9b4 |
| 0x800f5630 | 0x800ecaf0 | 0x800ecb2c, 0x800ecbc4, 0x800eb098, 0x800ecca0, 0x800eb1a0 |
| 0x800f5854 | 0x800ecab4 | 0x800eb298, 0x800eb398, 0x800ebea0, 0x800ecd3c, 0x800ec2f0, 0x800ecda4 |
| 0x800f587c | 0x800ecf08 | 0x800ec41c, 0x800ecdc4, 0x800ecee8 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|
| visual | 8 | 0x800ecf08 |

## Animation tables

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 4 | 5 | 0x800ea820, 0x800eb298, 0x800ec2f0 |
| 0x2 | var | 28 | 0 | 0x800ea820, 0x800eac90, 0x800eb398, 0x800ebea0, 0x800ec2f0, 0x800ec724, 0x800ec7f0, 0x800ec8bc |
| 0x3 | display | 8 | 4 | 0x800ea820, 0x800eac90, 0x800eaef8, 0x800eb098, 0x800eb1a0, 0x800eb298, 0x800ec614, 0x800ec724, 0x800ec7f0, 0x800ec9b4, 0x800ecb2c, 0x800ecca0 |
| 0x4 | state | 2 | 6 | 0x800ea820, 0x800eac90, 0x800eaef8, 0x800ec5ac |
| 0x5 | step | 0 | 1 | 0x800ea820 |
| 0x6 | step2 | 2 | 6 | 0x800ea820, 0x800eac90 |
| 0x7 | act4 | 15 | 15 | 0x800ea820, 0x800eb098, 0x800eb1a0, 0x800eb298, 0x800eb398, 0x800ebea0, 0x800ec2f0, 0x800ec6dc, 0x800ec724, 0x800ec7f0, 0x800ec8bc, 0x800ec9b4, 0x800ecab4, 0x800ecaf0, 0x800ecb2c, 0x800ecbc4, 0x800ecca0, 0x800ecd3c |
| 0x8 | x | 4 | 0 | 0x800ea820, 0x800eac90, 0x800ec5ac, 0x800ec9b4 |
| 0xa | x.px | 5 | 0 | 0x800eac90, 0x800eaef8, 0x800eb098, 0x800eb1a0, 0x800ecbc4 |
| 0xc | y | 2 | 0 | 0x800ec5ac, 0x800ec9b4 |
| 0xe | y.px | 6 | 0 | 0x800ea820, 0x800eaef8, 0x800eb098, 0x800eb1a0, 0x800ecbc4 |
| 0x14 | layer | 0 | 1 | 0x800ea820 |
| 0x15 | flip | 4 | 4 | 0x800ea820, 0x800eac90, 0x800eb098, 0x800ec9b4, 0x800ecb2c |
| 0x16 | priority | 0 | 3 | 0x800ea820 |
| 0x18 | prev_x | 0 | 1 | 0x800ec5ac |
| 0x1c | prev_y | 0 | 1 | 0x800ec5ac |
| 0x20 | speed_x | 3 | 5 | 0x800ea820, 0x800eb098, 0x800eb1a0, 0x800ecb2c, 0x800ecbc4, 0x800ecca0 |
| 0x24 | speed_y | 1 | 4 | 0x800ea820, 0x800eb098, 0x800eb1a0, 0x800ecbc4, 0x800ecca0 |
| 0x28 | accel_x | 0 | 3 | 0x800ea820, 0x800eb098, 0x800ecca0 |
| 0x2c | gravity | 0 | 3 | 0x800ea820, 0x800eb098, 0x800ecca0 |
| 0x30 | anime_table | 2 | 1 | 0x800ea820, 0x800ec9b4 |
| 0x3c | sprite_data | 2 | 1 | 0x800ea820, 0x800ec9b4 |
| 0x40 | tex_cord | 2 | 1 | 0x800ea820, 0x800ec9b4 |
| 0x42 | clut_cord | 5 | 6 | 0x800ea820, 0x800eac90, 0x800ec614, 0x800ec9b4 |
| 0x45 | anime.flag | 3 | 0 | 0x800eac90, 0x800ec8bc, 0x800ec9b4 |
| 0x50 | attack_box | 0 | 4 | 0x800ea820, 0x800eac90 |
| 0x54 | hurt_box | 0 | 3 | 0x800ea820, 0x800eac90 |
| 0x58 | damage_table | 0 | 3 | 0x800ea820, 0x800eac90 |
| 0x5c | hp | 3 | 2 | 0x800ea820, 0x800eac90, 0x800eaef8 |
| 0x60 | contact_damage | 0 | 1 | 0x800ea820 |
| 0x61 | iframes | 0 | 1 | 0x800ea820 |
| 0x65 | last_hit_tag | 0 | 1 | 0x800ea820 |
| 0x67 | latch | 0 | 1 | 0x800ea820 |
| 0x68 | terrain_box | 0 | 2 | 0x800ea820 |
| 0x75 | self+0x75 | 0 | 1 | 0x800ea820 |
| 0x7c | work.7C | 6 | 0 | 0x800eac90, 0x800ec614, 0x800ec724, 0x800ec7f0, 0x800ec8bc, 0x800ecb2c |
| 0x80 | work.80 | 16 | 18 | 0x800ea820, 0x800eaef8, 0x800eb098, 0x800eb1a0, 0x800eb298, 0x800eb398, 0x800ec2f0, 0x800ec6dc, 0x800ec724, 0x800ec7f0, 0x800ecb2c, 0x800ecbc4, 0x800ecca0, 0x800ecd3c |
| 0x82 | self+0x82 | 5 | 10 | 0x800eb098, 0x800eb1a0, 0x800eb298, 0x800eb398, 0x800ebea0, 0x800ec2f0, 0x800ecbc4, 0x800ecd3c |
| 0x84 | work.84 | 5 | 10 | 0x800eb098, 0x800eb1a0, 0x800eb298, 0x800eb398, 0x800ebea0, 0x800ec2f0, 0x800ecbc4, 0x800ecd3c |
| 0x86 | self+0x86 | 1 | 4 | 0x800ea820, 0x800ec8bc, 0x800ec9b4 |
| 0x88 | work.88 | 1 | 1 | 0x800eb298, 0x800ec2f0 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800020 | sprite_bank_base | 0x800ea820 |
| 0x8007433c | dmg_tbl_deflect | 0x800eac90 |
| 0x800743dc | dmg_tbl_normal | 0x800ea820, 0x800eac90 |
| 0x80095098 | bank_tpage | 0x800ea820 |
| 0x800970a0 | mega | 0x800eb1a0, 0x800eb298, 0x800ebea0, 0x800ec2f0, 0x800ec41c, 0x800ecbc4 |
| 0x800970a2 | mega.var | 0x800ec8bc |
| 0x800970a8 | mega.x | 0x800eac90 |
| 0x800970aa | mega.x.px | 0x800eac90, 0x800eb398, 0x800ecd3c |
| 0x800970b4 | mega.layer | 0x800ea820 |
| 0x80097174 | mega.gate | 0x800ec5ac |
| 0x800971f8 | camera_layer0 | 0x800eb298, 0x800ebea0, 0x800ec2f0 |
| 0x80097244 | cam.redraw | 0x800eb398 |
| 0x80097420 | frame_count | 0x800eb098, 0x800eb1a0, 0x800eb398, 0x800ebea0, 0x800ec2f0, 0x800ecca0, 0x800ecd3c |
| 0x800c4560 | bg_refresh | 0x800ebea0 |
| 0x800cced0 | game | 0x800eaef8, 0x800eb298, 0x800eb398, 0x800ebea0, 0x800ec2f0 |
| 0x800cceff | game.stage_flag3+0x7 | 0x800ea820 |
| 0x800ccf38 | game.difficulty | 0x800ec724, 0x800ec7f0, 0x800ecb2c |
| 0x800e9864 |  | 0x800ea820 |
| 0x800e9874 |  | 0x800ea820 |
| 0x800e98b4 |  | 0x800eac90 |
| 0x800e98cc |  | 0x800eb398 |
| 0x800f5560 |  | 0x800ea820 |
| 0x800f55dc |  | 0x800eac90, 0x800eaef8, 0x800ec614 |
| 0x800f55e4 |  | 0x800ea820 |
| 0x800f55ec |  | 0x800ea820 |
| 0x800f55f4 |  | 0x800ea820 |
| 0x800f55f8 |  | 0x800ea820 |
| 0x800f55fc |  | 0x800eac90 |
| 0x800f5644 |  | 0x800eb398, 0x800ebea0, 0x800ec2f0 |
| 0x800f56a4 |  | 0x800eb398 |
| 0x800f56ac |  | 0x800eb398 |
| 0x800f56b4 |  | 0x800eb398 |
| 0x800f56bc |  | 0x800eb398 |
| 0x800f56c4 |  | 0x800eb398 |
| 0x800f56cc |  | 0x800eb398 |
| 0x800f56d4 |  | 0x800eb398 |
| 0x800f56dc |  | 0x800eb398 |
| 0x800f56e4 |  | 0x800eb398 |
| 0x800f56ec |  | 0x800eb398 |
| 0x800f56f4 |  | 0x800eb398 |
| 0x800f56fc |  | 0x800eb398 |
| 0x800f5704 |  | 0x800eb398 |
| 0x800f570c |  | 0x800eb398 |
| 0x800f5714 |  | 0x800eb398 |
| 0x800f571c |  | 0x800eb398 |
| 0x800f5724 |  | 0x800eb398 |
| 0x800f572c |  | 0x800eb398 |
| 0x800f5734 |  | 0x800eb398 |
| 0x800f573c |  | 0x800eb398 |
| 0x800f5744 |  | 0x800eb398 |
| 0x800f574c |  | 0x800eb398 |
| 0x800f5754 |  | 0x800ebea0 |
| 0x800f586c |  | 0x800ec41c |
| 0x800f5870 |  | 0x800ec41c |
| 0x800f5874 |  | 0x800ec41c |
| 0x800f5878 |  | 0x800ec41c |
