# Sabre block (intro, stage object type 2): behaviour sheet

A grey and orange armoured block with a red eye in the centre. It closes the path in five places in
the intro stage. It is solid on all four sides: the player stands on it, is pushed by it and can
wall-slide on it. Only sabre attacks hurt it; X's buster shots bounce off. It has 32 HP. When it
breaks, it makes explosions and three pieces of debris. It drops no item. "Sabre block" is a
descriptive name; no official name was found.

Code in overlay `02` (stage 0, area 0): entry `SabreBlockUpdate` `0x800E9D80`, state table
`0x800FA98C`. Type 2 is an **overlay type**: entry 2 of the intro stage-object table
(`intro_stage_object_types`, `0x800FA794`, scratchpad `+0x94`) is `0x800E9D80`. In the dumps of
stages 1, 4 (both areas), 5, 6, 7, 15, 21 and 22, entry 2 is the empty stub `0x80054830` [certain].
Records: kind 4, `b` 2, `id` 0: records 40 (2016, 984), 41 (2848, 1080), 42 (3104, 1112), 43 (4000,
792), 44 (5488, 424) (`docs/stages/intro.md`). No other stage list has kind 4 / `b` 2 [certain for
the 13 lists in `~/mmx-data/x6/stages/*/objects.json`].

No save state has a live block: `intro-1` and `intro-2` are before block 1, and in `intro-3_boss`
all five records have flags `0x81` (destroyed, `DeleteObjectKeep`) [certain]. So all facts are from
the code, the map and the record flags. Read with the RE kit (`python3 -m mmxdemake.rekit --state
intro-1_start.state.gz --type 2 --class stage`). Units, tags and the shared rules:
`docs/engine.md`.

## Spawn and set-up (state 0, `SabreBlockInit` `0x800E9C20`) [certain]

- Flags `0x41`, state 1, step 2, step2 0, `flip` 0, layer = `mega.layer`, draw priority 7.
- The record `id` is not read. All five records have `id` 0.
- Sprite type 129 = bank 1 of the intro type list (`0x800FA02C`: 128, 129, 0, 1, 2, 5, …). Texture
  `bank_tpage[1] >> 7` (`0xA0` in the dumps), palette `0x7884` (CLUT x 64, y 482), sprite data
  `sprite_bank_base` + entry 1. Animation table `0x800FA970`; `SetAnime(obj, 0)`.
- **HP 32**, damage table `dmg_tbl_sabre_only` (`0x8007447C`), contact damage 0, no attack box (no
  contact damage), `iframes` 0.
- **Hurt box** `0x800FA988` = (−32, −32, 64, 64). **Terrain box** `0x800FA984` = (0, 0, 32, 32):
  centre on the origin, half sizes 32 × 32, so **64 × 64 px**. Sprite 0 has the same size.
- `+0xA2` = 1: `SolidRectsBuild` adds a rectangle for it (below).
- `+0x75` = 1: at a side contact, `SolidTouch` copies it to `mega+0x4A` (`0x8002E370`), so X can
  wall-slide and wall-kick on the block's sides [certain for the store; likely for the effect].

## Main (state 1, `SabreBlockMain` `0x800E9DCC`) [certain]

1. Clear the hit-flash bit (`+0x42 &= 0x7FFF`), keep the old HP.
2. `ContactWeapon`. A kill (result < 0) goes to "Break"; the rest of the update does not run.
3. When the HP changed: set the hit-flash bit (1 update of flash).
4. `0x80032BE8` (stops the enemy shots that touch the hurt box) and `SolidPushShots`.
5. `OffScreenCheck2(64, 64)`: more than 64 px outside the view → state 3. Else `DisplayCheck(64,
   64)`.
6. `CollideCheck` (its own terrain box against the tiles), `SolidObject` (the player), `0x80031F4C`
   (pushes and carries enemies), `ContactMega` (no effect: no attack box).

There is no step routine. The block does not move by itself.

## Geometry and position

- **It moves up 8 px when it spawns** [likely; from the code, not seen live]. `CollideCheck` runs
  its floor test also when the object did not move. Every record y is 8 px below a 16-px cell line,
  so the bottom edge (Y + 32) is 8 px inside a floor cell; the floor case pushes it up by `y & 15`
  = 8. After that the block stands on the floor:

  | record | centre | box (x, y) | place [certain for the map] |
  |---|---|---|---|
  | 40 | (2016, 976) | 1984–2047, 944–1007 | on the ladder top (33 at x 2000, 34 at 1984 and 2016). A wall is at x 2112 and only 32 px are free above the block. The way on is down the ladder, under the block |
  | 41 | (2848, 1072) | 2816–2879, 1040–1103 | fills a 64-px corridor; it stands on a 64-px pillar over a pit |
  | 42 | (3104, 1104) | 3072–3135, 1072–1135 | fills a 64-px corridor under a ceiling block |
  | 43 | (4000, 784) | 3968–4031, 752–815 | fills a 64-px corridor |
  | 44 | (5488, 416) | 5456–5519, 384–447 | fills a 64-px corridor |

  So X cannot pass any of the five without breaking it [likely: X is about 40 px high].
- **Solid for the player as an object, not as tiles** [certain]. It does not call `CopyTiles`.
  `SolidObject` pushes the player out of the 64 × 64 box on all four sides and carries a player who
  stands on it (`docs/engine.md`, "Solid objects").
- **Solid rectangle** [certain]. `SolidRectsBuild` adds {X − 32, X + 31, Y − 32, Y + 31} to
  `solid_rects` each frame. Only `TerrainPointCheck` and `CornerCheckRects` read it (as value 56).
  In intro RAM only X-overlay code at `0x801F14B4` calls `CornerCheckRects` (one state of an X weapon
  object [guess]). So the rectangle makes the block a wall for some weapon parts only.
- Enemy shots stop on it. Enemies with a terrain box are pushed and carried.

## Damage: why only the sabre [certain for the table]

`dmg_tbl_sabre_only` (`0x8007447C`, the Meta Wheel F table). The block uses `ContactWeapon` with its
hurt box.

| X weapon (id) | class | damage | hits to break (Normal / Easy) |
|---|---|---|---|
| buster 0, half charge 9, full charges 18, 19, 20/21, 43, Shadow buster 45 | `0xFF` | – | deflected: the shot bounces, sound (0, 6) |
| special weapons 1–6, 8, charged 10–17, their parts 52, 53, 54 | `0xFF` | – | deflected |
| Guard Shell 7 | 2 | – | no effect, the shot goes through |
| Ultimate Giga 22, Falcon dash 23, Falcon Giga 49 | `0xFF` | – | deflected |
| **sabre slash 50** (on the floor) | 0 | **4** | **8 / 7** |
| **sabre slash 51** (air, wall) | 0 | **6** | **6 / 5** |
| Mach Dash 32 | 0 | 3 | 11 / 11 |
| Shadow charged slash 46 | 0 | 4 (+1 with the Buster Plus part) | 8 (7) |
| Shadow Giga 47 | 0 | 6 | 6 / 5 |
| Blade up-slash 56, Blade Giga wave 57 | 0 | 1 | 32 / 32 |
| id 55 (not known) | 0 | 127 | 1 |

- Zero's ids (24–31, 33–42, 44) also hurt it (3–8 each). Zero is not played in the intro [likely].
- In the intro X has form 0 and no special weapons, so the sabre (50, 51) is the only way [likely].
- Easy: damage × 1.25, rounded down (`ApplyWeaponHit`).
- A slash can hit more than once (ids 50 and 51: up to 4 hits per slash, `docs/players/weapons.md`)
  [likely].
- Ids 51, 46 and 57 start the 5-frame hit stop, also on a deflect (`docs/engine.md`, "Hit stop").
  Id 50 does not.
- No knock-back.

## Break (kill update, `0x800E9DFC`) [certain for the code]

1. 5 × `SpawnExplosionRand(obj, 1, 16, 16)`: each offset is `GetRNG() & 16` with a random sign, so
   each explosion is at x ± 0 or 16 and y ± 0 or 16.
2. `SpawnJunk(3, sabre_block_junk = {1, 2, 3}, obj)`: 3 debris pieces (misc type 3) with
   animations 1, 2, 3.
3. `SpawnExplosion(obj)`: one more explosion at the centre.
4. State 2. At the next update, `DeleteObjectKeep` (`0x800E9EFC`): record flags `|= 0x80`.

- **Explosions** (effect type 4, `ExplosionInit` `0x8004A194`, `ExplosionMain` `0x8004A250`):
  animation table `0x80077D54`; each one plays `PlaySound(0, id)` with id 0 or 1.
  `SpawnExplosionRand` alternates the id with `explosion_sound_toggle` (`0x800E4470`);
  `SpawnExplosion` picks it at random. The block itself plays no sound.
- **Debris** (misc type 3, `JunkInit` `0x80047B34`, `JunkMain` `0x80047C30`): it copies the block's
  sprite, texture and palette (without the flash bit). Priority 1, random `flip`, start offset 0–3
  px. `speed_x` = `junk_speed_x`[rng & 7] (−3.0, −2.0, 1.5, 2.5, −3.5, −2.5, 2.0, 3.0) + the
  parent's `speed_x` / 2. `speed_y` = `junk_speed_y`[rng & 7] (3.5, 4.5, 6.0, 3.0, 4.0, 5.0, 5.5,
  2.5) up. Gravity 0.3125 px/frame², fall limit 6.5 px/frame. It does not touch the terrain, is
  drawn on every second update and is deleted 64 px off screen.
- **No item drop** (no `DropItem` call). **No tile change.**

## Respawn

- **Scrolled away before it breaks** [certain for the code]: more than 64 px outside the view →
  state 3 → `DeleteObject2` (`0x800E9EDC`); the record is free again. It spawns again with full HP
  32.
- **Broken** [certain]: `DeleteObjectKeep`. The spawner skips the record, so the block does not come
  back while the area is loaded.
- **Death and restart** [certain for the code; likely that a death goes through `StageStart`]:
  `RecordsReset` clears the record flags (`docs/engine.md`, "Checkpoints"), so all broken blocks
  come back.

## Animations [certain]

Table `0x800FA970`, 4 animations of one frame, each held (`next` 0):

| anim | frame (duration, flag, next, sprite) | use |
|---|---|---|
| 0 | 12, 0, 0, 0 | the block (64 × 64) |
| 1 | 1, 0, 0, 1 | debris: large piece (48 × 48) |
| 2 | 1, 0, 0, 2 | debris: medium piece (48 × 32) |
| 3 | 1, 0, 0, 3 | debris: small piece (32 × 16) |

Sprites: bank 1 (type 129) of the intro,
`~/mmx-data/x6/sprites/banks/intro/intro-1_start/bank-1.png` (4 sprites). There is no export in
`~/mmx-data/x6/sprites/enemies/`: no live object, and `spriteexport` does not take type 129 (≥
`0x80`) as a main type.

## Summary for a reimplementation

1. A 64 × 64 px solid block, centred 8 px above its record point (on the floor). Solid on all sides
   for the player; X can stand on it and wall-slide on it. It stops enemy shots.
2. 32 HP. Only sabre attacks hurt it: X's slash does 4 (floor) or 6 (air or wall); Easy × 1.25.
   Buster shots and special weapons bounce off with a "tink"; Guard Shell goes through. One update
   of hit flash, no knock-back.
3. Break: 5 explosions at offsets of 0 or ±16 px, then 1 at the centre (sound bank 0, id 0 or 1
   each), and 3 debris pieces (sprites 1–3) thrown up and to the side, under gravity 0.3125, that
   flicker and fall through the floor. No item.
4. A broken block stays broken until a death or a stage restart. A damaged block that goes 64 px
   off screen comes back with full HP.
5. Intro only, 5 places: each one closes a 64-px corridor or the top of a ladder.

## Open questions

1. A live check of the 8-px move up and of the corridor fill (no save state with a live block).
2. What bank 0 sounds 0 and 1 are (all shared explosions use them).
3. The X weapon object at `0x801F1450` that reads `solid_rects`.
4. Weapon id 55 (127 damage).
5. The official name.
