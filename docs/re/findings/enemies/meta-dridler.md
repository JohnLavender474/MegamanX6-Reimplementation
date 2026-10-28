# Meta Dridler (stage 02, main object type 0): behaviour sheet

Meta Dridler is the indestructible drill of the intro stage. It hangs in a ceiling cavity, drops,
drills for a moment, and slowly rises back. The code lets three of them drop a rock block onto the
floor under them. Each one has two attached effect objects (misc object type 0).

Overlay `02` (intro). Read by hand from the disassembly (before the RE kit). Units, confidence tags
and the shared engine rules: `docs/engine.md`. Records (45–53): `docs/stages/intro.md`. The record
point is the drill's resting ("home") position at the top of its path.

## Set-up (spawn frame) [certain]

- HP 1, but the damage table is `dmg_tbl_deflect`: player shots bounce off, and the code ignores the
  hit result. **It cannot be destroyed.**
- Contact damage **8**.
- Draw priority 33. Faces left; it never turns.
- `home_y` = the spawn y.
- Two effect objects are made at the drill's position (see "Effects").
- Starts in WAIT.

## The `id` parameter [certain]

- 0: the normal cycle.
- 1, 2, 3: the same, and a rock block when it lands on a floor (see "Rock block").

## Geometry [certain for the numbers]

Meta Dridler always faces left.

| Box | Data | Area | In blocks |
|---|---|---|---|
| Contact box | (−14, −40, 28, 80) | X − 14 … X + 14, Y − 40 … Y + 40 | 1.75 × 5 |
| Hurt box | (−16, −40, 32, 80) | X − 16 … X + 16, Y − 40 … Y + 40 | 2 × 5 |
| Terrain box | centre (0, +6), half size 11 × 32 | X − 11 … X + 11, Y − 26 … Y + 38 | 1.4 × 4 |

So the drill tip (bottom of the terrain box) is 38 px below the origin.

## Cycle [certain for the order and numbers; durations in frames]

1. **WAIT** at `home_y`: 90 frames (Hard: 45). The drop warning effect shows from 30 frames (Hard:
   15) before the drop until the drill lands.
2. **DROP**: sound (bank 2, id 1). Start speed +1/16 px/frame up, gravity 1/8 px/frame² (`0x2000`).
   It falls until the terrain box touches a floor, or until it is 128 px (8 blocks) below `home_y`
   (then it stops there, in the air). A free fall of 128 px takes 47 frames and ends at about
   5.8 px/frame.
3. **DRILL** (id 0, or any drill that stopped at the 128 px limit): sound (bank 2, id 2), 60 frames
   in place. The rise warning effect shows from 30 frames before the rise until the drill is back
   at `home_y`.
   **ROCK** (id 1–3 that landed on a floor): see below; also 60 frames.
4. **RISE**: sound (bank 2, id 3), repeated every 60 frames. Speed 0.5 px/frame up, no gravity, until
   it is back at `home_y` (128 px takes 256 frames, ≈ 4.3 s). Then WAIT again.

One full cycle with the full 128 px drop: 90 + 47 + 60 + 256 ≈ 453 frames (≈ 7.5 s). Hard:
45 + 47 + 60 + 256 ≈ 408 frames.

Every frame, in every step: contact test (the drill hurts while it waits, falls, drills and rises).
Off screen 80 (x) / 160 (y) → deleted (the record can spawn it again).

## Rock block (id 1, 2, 3) [certain for the code and data; whether it happens in play is open]

When a drill with id 1, 2 or 3 lands **on a floor** (not at the 128 px limit):

- The game copies a block of 4 × 3 map cells (64 × 48 px) from a template area 256 px lower in the
  stage to the space under the drill:

  | id | record | destination (top-left) | template (top-left) |
  |---|---|---|---|
  | 1 | 46 | (4640, 880) | (4640, 1136) |
  | 2 | 47 | (4864, 880) | (4864, 1136) |
  | 3 | 48 | (4928, 880) | (4928, 1136) |

  The template cells are rock tiles (tiles 2237–2248): the top row is art only, the two lower rows
  are solid (collision 56). The destination cells are empty at the start of the stage, just above
  the floor surface. So the copy puts a rock block of 4 × 2 solid cells on the floor under the drill.
  Drills 47 and 48 are 64 px apart, so their two blocks join (x 4864 … 4991).
- At once: 4 explosions around (X, Y + 32), within ±64 px in x and ±32 px in y; 6 debris pieces;
  a screen shake (arguments 8, 4, 2).
- Then, for 60 frames: 3 more explosions every 8 frames, and 6 debris pieces with a small shake every
  16 frames. Then RISE.

The camera shake is only started when two layer-0 camera fields are equal (`+0x0E` and `+0x20` at
`0x800971F8`); this could mean "the camera is not scrolling vertically" [guess].

## Effects (misc object type 0, two per drill) [certain for the logic]

Both follow the drill (same x and y) and have no collision.

- Effect 0: shown while the drill rises or waits (animation 1, sprites 4–8, 14) and while it drops
  (animation 4, sprites 9–14); hidden while it drills on the ground.
- Effect 1: the warnings (timing in "Cycle"). Drop warning: animation 2 (sprites 19–22); rise warning:
  animation 5 (sprites 15–18); hidden otherwise. It moves 1 px up when the drill shows sprite 23.

The drill itself shows animation 0 (sprites 0 and 23, 2 frames each) while waiting, animation 3
(sprites 0–3) while falling and drilling, and animation 6 while rising. What each sprite looks like is
not checked yet.

## Damage

- Taken: none (`dmg_tbl_deflect`).
- Dealt: contact 8.

## Summary for a reimplementation

- Indestructible ceiling drill, 8 contact damage, hurts in every phase. Box 2 × 5 blocks.
- Wait 1.5 s (Hard 0.75 s, with a 0.5 s / 0.25 s warning), drop with gravity up to 8 blocks or to the
  floor, drill 1 s, rise slowly (0.5 px/frame) back, repeat.
- Three of the intro drills (ids 1–3) can put a 4 × 2-cell rock block on the floor when they land on
  it, with explosions, debris and screen shake.

## Open questions

1. From the static data, the rock drills may never land: a full 128 px drop from y 736 leaves the
   drill tip (Y + 38) at y 902, and the first solid cell below is the floor surface at y 944. If so,
   they stop in the air and drill, like id 0, and the rock block never appears. Watch drills 46, 47
   (Normal and Hard only) and 48 in play: do rocks appear on the floor under them?
2. The meaning of the shake arguments.
