# Nightmare Bug (stage 10, main object type 22): behaviour sheet

The Nightmare Bug is a small wasp-like insect of the Nightmare Effect. It flies to a point beside the
player and stays there, bobbing up and down. It moves with the player through the stage. It deflects
almost all weapons, so it blocks the player's shots. After it deflects 6 shots, it flies up and away.
Only weapon ids 1, 10 and 54 can hurt it. The official name is from the Mega Man Knowledge Base
(Fandom) page "Nightmare Bug".

It is **not** `nightmare-insect`: that sheet is the large dragonfly of overlay 03 (main type 23),
with other code and other sprites. The two objects have one thing in common: weapon ids 1, 10 and 54
are the only ids that hurt the bug, and the only ids that kill the dragonfly for good [certain].

Overlay `10` (stage 7, Shield Sheldon), code at `0x800F3F54`–`0x800F44A0`. Save states
`sheldon-5_nightmare-hidden` and `sheldon-6_tile-shown` (stage 7, area 0, X, Normal). Kit:
`python3 -m mmxdemake.rekit --name nightmare-bug`. Units, confidence tags and the shared engine
rules: `docs/engine.md`. Export: `nightmare-bug` (`enemy-30445534f9`).

## When it exists [certain]

- The set-up reads `nightmare_effect` (`0x800CD30A`). When it is not **1**, the object deletes
  itself at once (`DeleteObject3`): the bug exists only while the stage has Nightmare Effect 1.
- `nightmare_effect` is written once, at the start of a stage (area 0, stage < 19), from a byte per
  stage: `game+0x168 + stage` (`0x8001E9D0`). In all `sheldon-*` states the byte for stage 7 is 1.
  The pick of Commander Yammark's stage (stage 1) writes 1 into stages 3 and 7; see
  `docs/engine.md`, "Nightmare Effect".
- The records of area 0 hold **11** bug records (type 22, `id` 0, kind 0, so on all
  difficulties): one at (464, 320) near the start, nine at x 3856–4632, and one at (0, 0)
  [certain for the records].
- **Stage 3.** Overlay `05` has the same object as main type 22 (code `0x800EA07C`–`0x800EA5C8`,
  entry `0x800EA3D4`; 339 words equal after relocation) [certain]. 19 records (90–108, `id` 0,
  kind 0; `docs/stages/heatnix.md`). In the `heatnix-*` states `nightmare_effect` is 4, so they
  delete themselves at once.

## Spawn and set-up [certain]

- **HP 8**, contact damage **2**, `iframes` 0. Draw priority 5, layer = `mega.layer`.
- **No terrain box**: no terrain collision and no gravity. It flies through walls and floors.
- Contact box `0x800FC6F4` and hurt box `0x800FC6F8` = (−6, −6, 11, 11): 11 × 11 px around the
  origin.
- Damage table `0x800746FC` (see Damage).
- **Place beside the player**, chosen once at spawn with two random numbers:
  - x offset = {−96, −80, −64, −48, +48, +64, +80, +96}[rand & 7] px (table `0x800E9DF0`);
  - y offset = {0, +16, +32}[rand % 3] px below the player's origin (table `0x800E9E00`).
- Animation 0, step FOLLOW. The record `id` is not read.

## State machine

States (table `0x800FC718`): 0 set-up `0x800F3F54`, 1 main `0x800F42F8`, 2 `DeleteObject3`
`0x800F43F4`, 3 blink `0x800F4414`. Steps (table `0x800FC728`): 0 engine hit reaction, 1 FOLLOW
`0x800F40FC`, 2 LEAVE `0x800F4468`.

### Main (state 1) [certain]

1. The step.
2. Weapon test (`ContactWeapon`):
   - **killed**: `DropItem` table 18, one explosion, state 2 (deleted);
   - else, when `mega+0xD4` = 1 (the player passes a boss gate, `docs/enemies/boss-gate.md`):
     state 2 (deleted at once, no explosion);
   - else, when the result is 127 (the shot was deflected): the block counter `+0x85` goes up by 1.
     When it was already 5 or more, step LEAVE. So the **6th** deflected shot sends it away.
3. Contact test (`ContactMega`).
4. Off screen 64 (`OffScreenCheck2(64, 64)`) → `DeleteObject2`. Else visible 32.

### FOLLOW (step 1) [certain]

Each update:

- It faces the player (`flip` = right when its x is less than the player's x; equal x: left).
- Target = player position + its x and y offsets.
- It aims at the target (`ratan2`) and flies straight at it at **5 px/frame**.
- When it is within 16 px (x) and 32 px (y) of the target, it stops. Then, on every second update
  (`frame_count` even), it moves 1 px up or down by a sine: the y goes from the stop point down
  **11 px** and back up in **64 updates**, again and again.
- Because the target moves with the player, the bug stays beside the player through the stage. It
  stops in a window of ±16 px × ±32 px, so it bobs and drifts a little while the player moves.
- Animation 0 (wings) and `MoveObject` each update.

### LEAVE (step 2) [certain]

- `speed_x` = 0, `speed_y` = **5 px/frame up**. It flies straight up until it is 64 px off screen,
  then `DeleteObject2`.
- The weapon and contact tests still run: it still deflects shots and still hurts on contact.

### Blink (state 3) [certain for the code]

The display bit toggles each update for 60 updates, then `DeleteObject3`. **No code sets state 3**
[likely]. The search:

- The bug code itself never writes 3 to its state [certain].
- In the main program, the player overlays 0 (X) and 1 (Zero), overlay 10 (stage 7), overlay 31
  (stage 22, where the bug is also present) and all other overlays of `ROCK_X6.BIN`, every store of
  the value 3 to `+0x04` was traced back to its base register. Each one writes the state of the
  object that runs the code (its own `a0`), not of another object [certain for the stores found;
  the scan follows the code in address order, so a value that comes through a branch can be missed].
- No loop over the main objects (`0x8008EF48`, stride `0x9C`) writes a state: the loops at
  `0x80020040` (the update loop), `0x80022BC0` (drawing), `0x8002B7A4`, `0x8002B940`, `0x8002BBF8`,
  `0x8002BCE0`, `0x8002BDD8`, `0x8002C238` (free slot and clear), `0x80031F88` and `0x800ED1D0`
  (overlay 10; sets state 4 of laser-puzzle parts, type 13, in a rectangle) do not write 3.
- No code compares an object type with 22 next to a state write.

So state 3 is probably left over from the object that the bug was made from. A reimplementation can
leave it out.

## How it blocks shots [certain]

- The hurt box is a normal 11 × 11 px hurt box. Every weapon id with class `0xFF` in its table is
  deflected: `ApplyWeaponHit` calls `WeaponHitReaction(shot, −1)` and returns 127. The shot bounces
  off ("tink", `docs/engine.md`), so it does not go on to the enemy behind the bug [likely for the
  shot's path after the deflect].
- The bug sits 48–96 px in front of or behind the player and up to 32 px below the player's origin,
  so a shot fired to its side passes through its box when the bug is at the same height.
- Ids with class 2 (no effect) pass through the bug: `ApplyWeaponHit` returns 0.

## Animations (table `0x800FC714`) [certain]

| # | what | frames |
|---|---|---|
| 0 | fly, wings beat | sprites 0–5, 2 updates each, loop (12 updates) |

The export keeps 1 animation. The table has only this one entry; the next words are the state
tables.

## Damage

- Taken: table `0x800746FC` [certain]:

| ids | effect |
|---|---|
| 1, 10, 54 | 4 damage |
| 55, 59, 60, 63, 64–79 | no effect (the shot passes) |
| all others (0, 2–9, 11–53, 56–58, 61, 62) | deflected: counts as a block |

- So 2 hits of ids 1, 10 or 54 kill it (Easy: 5 damage, still 2 hits). The X-Buster (ids 0, 9,
  18–20) and the Z-sabre (24–31) are deflected. Which weapons ids 1, 10 and 54 are is not known.
- The table is not `dmg_tbl_minor` or `dmg_tbl_one_hit`, so a kill adds 1 to the kill counter
  [likely, by the engine rule].
- Dealt: contact **2** (before the player's damage scaling: Easy 1, Hard 2).

## Despawn and respawn [certain for the code; likely for the effect]

| how it goes | routine | record | comes back |
|---|---|---|---|
| killed | `DeleteObject3` | stays "spawned" | not while the area is loaded |
| boss gate | `DeleteObject3` | stays "spawned" | not while the area is loaded |
| effect is not 1 | `DeleteObject3` | stays "spawned" | not while the area is loaded |
| LEAVE, or 64 px off screen | `DeleteObject2` | cleared | when the record point enters the spawn band again |

The player's death clears all record bits (`docs/engine.md`), so all bugs can come back after a
respawn [likely].

## How many [certain for the records; likely for the count in play]

- One bug per record at a time. Records [23] (464, 320) and [24]–[32] (x 3856–4632) make up to 10
  bugs. A bug follows the player and does not go off screen, so the bugs collect: after x 3856 the
  player can have the first bug plus up to nine more.
- In `sheldon-5` and `sheldon-6` one bug is live (from record [23], about 1100 px behind the player).
  In `sheldon-6` it is at its target: x offset +48, y offset +16. In the later `sheldon-7_lasers`
  record [23] is free again: the bug left (6 blocks) or went off screen.

## Sounds

None. The bug code calls no `PlaySound`.

## Summary for a reimplementation

- Exists only when the stage's Nightmare Effect is 1 (Shield Sheldon's stage in these saves).
- Picks a fixed point beside the player at spawn: x ±48/64/80/96 px, y 0/16/32 px below.
- Flies straight to that point at 5 px/frame; inside ±16 × ±32 px it stops and bobs 0–11 px down
  over 64 frames.
- 8 HP, 2 contact damage, 11 × 11 px body, no terrain collision.
- Deflects all weapons except ids 1, 10, 54 (4 damage each); some ids pass through.
- After 6 deflected shots it flies straight up at 5 px/frame and goes.
- Removed at once when the player enters a boss gate. Killed: drop table 18.

## Open questions

1. (answered: the stage pick, `docs/engine.md`) Stage 3 also gets effect 1 from the Yammark pick;
   whether the bug also appears there is not checked.
2. (answered: no writer found in any code, see "Blink (state 3)") Who sets state 3.
3. Which weapons are ids 1, 10 and 54.
4. The record at (0, 0) probably never spawns [guess].
5. Not checked in play: the bob, the 6-block leave, the count of bugs after x 3856.
