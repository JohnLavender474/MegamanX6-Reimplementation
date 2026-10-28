# Grey block (stage 08 and 29, stage object type 12): behaviour sheet

The grey block is a grey metal block of 64 × 48 px. It appears out of the air, shakes, and
falls. On the floor it shakes the screen, waits, rises back to its start point and falls again. So
it is a crusher. It is solid: the player can stand on it, and it can pin the player against the
floor. Most weapons bounce off it. It exists only under Nightmare Effect 4. "Grey block" is a
descriptive name (`docs/stages/scaravich.md`); no official name is known.

Code in overlay `08` (Ground Scaravich, stage 5; entry `0x800EE1C4`, state table `0x800F5B14`) and
overlay `29` (the hidden area, stage 21 area 0; entry `0x800EAB54`, state table `0x800EC604`). The
two copies are the same after relocation: every differing instruction is a branch, a jump or an
address part [certain]. Save states `scaravich-2_totem-section` (overlay 08) and
`scaravich-7_alt-ledge`, `-5`, `-8`, `-9` (overlay 29). Kit: `python3 -m mmxdemake.rekit --state
scaravich-9_high-max.state.gz --type 12 --class stage`. Units, tags and the shared rules:
`docs/engine.md`. Records: `docs/stages/scaravich.md`, `docs/stages/scaravich-alt.md`. Addresses
below are those of overlay 29; add `0x3670` for overlay 08 code, `0x9510` for its data. All values
are [certain for the code] unless a line says otherwise.

**Stage 3.** Overlay `05` (Blaze Heatnix) has the same object as stage object 12 (entry
`0x800EA8E8`, set-up `0x800EA5C8`; overlay 29 address − `0x26C`). All 344 words of
`0x800EA5C8`–`0x800EAB28` equal the overlay 29 code after relocation (`heatnix-1_start`) [certain].
Records: 5 with `id` 3 in the first tower and 10 with `id` 4 in the shaft room and the side
corridor (`docs/stages/heatnix.md`). Sprite type 142 is bank 5 there; the bank sheet is the same
as that of stage 5. In `heatnix-4_cave` (checkpoint 1, camera (944, 1727)) records 111–114 are live
(`var` 3, `work.7E` = the record y) [certain for the values]:

| record | start y | live y | step | timer | floor | fall |
|---|---|---|---|---|---|---|
| 111 (1056) | 712 | 750 | RISE, `speed_y` +3 | −1 | 816 | 80 |
| 112 (1152) | 872 | 894 | RISE | −1 | 1056 | 160 |
| 113 (1184) | 1240 | 1293 | RISE | −1 | 1440 | 176 |
| 114 (1152) | 1528 | 1576 | REST, `speed_y` −4 | 119 | 1600 | 48 |
| 115 (992) | 2104 | not reached | – | – | 2272 | 144 |

The floors are from the layer-0 collision. Block 114 rests at the floor − 24 [certain]. Blocks
111–113 are stopped in the rise, 434–977 px above the view, as `OffScreenCheck2(64, 240)` predicts.
All four hold sprite 4 with flag 2.

## Spawn and set-up (state 0, `0x800EA834`)

- When `nightmare_effect` (`0x800CD30A`) is not 4: `DeleteObject3` (the record stays "spawned").
  Value 4 comes from the stage 4 (Metal Shark Player) pick, which writes 4 to stage 5; a hidden area
  keeps the value of the stage before it.
- Flags `0x41`, state 1, step 0, layer = `mega.layer`, `flip` 0. Sprite type 142 (bank 8 in stage 5,
  bank 7 in the hidden area; 8 sprites; the export `grey-block` (`enemy-8df182908d`) is the same
  from both states). Draw priority 7.
- **HP 16**, damage table `0x80075E1C` (below), contact damage 8, no attack box.
- Hurt box `0x800EC600` = (−32, −24, 64, 48): 64 × 48 px around the point.
- Terrain box `0x800EC5FC` = centre (0, 0), half 32 × 24: 64 × 48 px. It is set only while the
  animation flag is not 0 (below); at the set-up it is none.
- `+0x75` = 1, `+0xA2` = 1 (`SolidRectsBuild` adds the terrain box to `solid_rects`).
- `speed_y` = −4 px/frame (down). `work.7E` = the start y (the record y).
- Animation 0 starts.

## The `id` parameter

`var` = the record `id`. Each update, when `game.checkpoint` ≥ `var`: `display` = 0 and
`DeleteObject`. Stage 5 uses `var` 1 (sections 0–3) and 2 (sections 4–7). The hidden area uses
`var` 3; it has checkpoints 0 and 1 only, so its blocks never go away. Stage 3 uses `var` 3 (gone
at checkpoint 3) and 4 (gone at checkpoint 4).

## State machine

States (`0x800EC604`): 0 set-up, 1 main (`0x800EA9BC`), 2 death (`0x800EABF0`), 3 delete
(`0x800EABD0`, `DeleteObject2`; no code read sets state 3). Steps (`0x800EC614`): 0 FALL
(`0x800EAC84`), 1 REST (`0x800EAD10`), 2 RISE (`0x800EAD48`).

**Entry** (`0x800EAB54`): copy x, y to `prev_x`, `prev_y`. Do nothing while `mega+0xE0` = 2, or
while `OffScreenCheck2(64, 240)` = 1 (outside the view + 64 px in x or + 240 px in y). So an
off-screen block stops where it is. It is not deleted off screen.

**Main** (state 1), in order:

1. `AnimeTick`. Terrain box on when `anime.flag` ≠ 0, else off. Clear the hit flash.
2. The step (below).
3. `ContactWeapon`. A kill: `SpawnExplosion`, timer 0, state 2. A hit: hit flash.
4. `0x80032BE8` (enemy shots that touch the hurt box stop) and `SolidPushShots`.
5. `DisplayCheck(32, 32)` when on screen; the checkpoint test (above).
6. `DisplayCheck(32, 32)`, `CollideCheck` (its own terrain box against the tiles).
7. `SolidObject`, but not while `mega.squeeze_timer` (`mega+0xE5`) ≠ 0; then `0x80031F4C`.
8. `latch` = 0 on a floor, else 1 (moving up) or −1 (moving down).

### The appearance (animation 0)

Animation 0 (table `0x800EC5F8`): sprites 7, 0, 1, 2, 3, 4, 5, 4, 6 (flag 0), then 4, 5, 4, 6, 4,
5, 4, 6, 4 (flag 1), then 4 (flag 2, held); 2, 2, then 3 updates per frame [certain: the stage 3
copy `0x800F3DB0` in `heatnix-4_cave`].

| updates from the spawn | flag | what |
|---|---|---|
| 0–24 | 0 | the block appears (sprites 7, 0–5); no terrain box, so it is not solid yet; it can be hit |
| 25–51 | 1 | it shakes (sprites 4, 5, 6); solid |
| 52 and later | 2 | it falls (step 0); it stays on flag 2 |

The animation runs only once (only the set-up calls `SetAnime`). So after the first fall the block
does not shake again.

### FALL (step 0)

While `anime.flag` = 2: `MoveObject` (4 px/frame down). On a floor (`collide_flags` bit `0x08`):
`PlaySound(2, 82)`, `Rumble(5)`, timer = 180, step 1, and `Quake2(90, 2, 2)` when the block is
drawn.

### REST (step 1)

The timer counts down. At −1 (after 181 updates): `speed_y` = +3 px/frame (up), step 2.

### RISE (step 2)

`MoveObject` (3 px/frame up). When y ≤ the start y: y = the start y, `speed_y` = −4 px/frame, step
0. Because the flag is still 2, it falls at once again.

One cycle: the fall (d / 4 updates), 181 updates on the floor, the rise (d / 3 updates); d = the
fall height. Example: block 4 of the hidden area (y 376 → 552, d = 176): 44 + 181 + 59 = 284
updates [likely: one update per frame].

A block whose record point is already on the floor (hidden area record 6, (320, 712)) lands in its
first update and stays in place: rest, a 0 px rise, again [certain for the code; seen at (320, 712)
in step 1].

### Death (state 2)

60 updates of blinking (`display` toggles each update), then 4 × `SpawnExplosionRand(1, 16, 16)`
and `DeleteObject`. The record is free again, so the block comes back when its point leaves the
spawn band and enters it again [likely].

## Damage

Table `0x80075E1C` (new; 80 ids after it is the virus pattern table `0x80075EBC`):

| ids | damage |
|---|---|
| 4, 13, 35 | 4 |
| 55 | 127 |
| 7, 23, 79 | – (no effect) |
| all other ids | D (deflected) |

So 4 hits of ids 4, 13 or 35 destroy it. These are the uncharged and charged shots of weapon slot 4
and one id of 33–44 (`docs/engine.md`, "Weapon ids": the Shield Sheldon stun ids). Slot 4 is the
weapon of Metal Shark Player [likely]. The buster and the Z-sabre bounce off.

## The squeeze

The block passes its contact damage (8) to `SolidObject`, which writes it to `mega+0xE6` at each
contact. When the falling block pins the player against the floor, `PlayerHitCheck` takes 8 HP and
sets `mega.squeeze_timer` = 75 (`docs/engine.md`, "Solid objects"). While the timer is not 0, the
block skips `SolidObject`, so it falls through the player [likely for the effect: not seen in a
state].

## Sounds

| bank, id | when |
|---|---|
| 2, 82 | the block hits the floor |

## Summary for a reimplementation

- A 64 × 48 px solid block. It exists only under Nightmare Effect 4.
- Appear (25 frames, not solid), shake (27 frames), fall at 4 px/frame.
- On the floor: sound, rumble, screen shake for 90 frames; wait 181 frames; rise at 3 px/frame to
  the start point; fall again. Repeat for ever.
- 16 HP; only weapon ids 4, 13, 35 (4 each) and 55 hurt it. A crushed player takes 8.
- Frozen while off screen; deleted when `game.checkpoint` ≥ `id`.

## Open questions

1. A state with a player squeezed under a block (to confirm the 8 damage and the fall-through).
2. Weapon id 55 (127 damage here and in most tables).
3. Main program helpers `0x80031F4C` and `0x80032BE8` are not named yet (also open in
   `docs/TODO.md`).
4. The look of each frame in play. The live sheet of `heatnix-4_cave` shows sprite 7 empty,
   sprites 0–3 the block that grows down from its top edge, 4–6 the full block (5 darker)
   [likely].
