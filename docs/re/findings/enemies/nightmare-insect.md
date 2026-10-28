# Nightmare Insect (stage 03, main object type 23): behaviour sheet

Nightmare Insect is a large mechanical dragonfly. It hovers in one place and does not move. It turns
to face the player. It has three attacks: a blade wave, a boomerang blade and a stream of small
hatchlings that walk on the floor and burst. When most weapons kill it, it leaves a green outline (a
husk) and comes back with full HP. Only weapon ids 1, 10 and 54 destroy it for good.

Overlay `03` (Commander Yammark), code at `0x800EC8F0`–`0x800EE154`. Written with the RE kit
(`python3 -m mmxdemake.rekit --name nightmare-insect`) and `docs/re-guide.md`. Units, confidence
tags and the shared engine rules: `docs/engine.md`. Records: `docs/stages/yammark.md`. Distances to
the player are in whole pixels, between the object origins.

This is not the small wasp of Shield Sheldon's stage that follows the player and blocks shots: that
is `nightmare-bug` (overlay 10, main type 22). The two objects have one link: weapon ids 1, 10 and
54 are the only ids that hurt the bug, and the only ids that kill this dragonfly for good [certain].

## Spawn and set-up [certain]

- **HP 32**, contact damage **8**, `iframes` 0.
- No terrain box and no gravity. Speed 0 at all times: the body never moves.
- Boxes:
  - contact box `0x800F7448` = (−28, −16, 48, 32): 48 × 32 px, from x −28 to +20 and y −16 to +16;
  - hurt box `0x800F744C` = (−34, −16, 60, 32): 60 × 32 px, from x −34 to +26.
- Draw priority 5. Faces the player at the start.
- Damage table `dmg_tbl_nightmare_insect` (`0x8007479C`).
- The record `id` (`var`) is not read. All copies act the same.
- It starts in IDLE with the timer at 0, so the first attack comes after 62 frames.

## State machine

States (table `0x800F7698`): 0 `0x800ED760`, 1 `0x800EC8F0`, 2 and 3 `0x800ED830`. No code sets
state 3, so it never deletes itself off screen.

### Main (state 1)

1. Runs its step (table `0x800F76A8`): 0 engine hit reaction, 1 IDLE, 2 BLADE, 3 BOOMERANG, 4 LAY,
   5 REVIVE.
2. **Weapon test.** When a hit kills it:
   - weapon id **1, 10 or 54**: the real death (see Death);
   - any other weapon: one explosion, 4 debris pieces (animations 8–11), HP back to **32**, contact and
     hurt boxes off, the husk animation (5 on Easy and Normal, 15 on Hard), then REVIVE.
3. Otherwise: contact test. When it is inside the screen plus 64 px, it is drawn inside the screen
   plus 48 px (`DisplayCheck(48, 48)`).

### IDLE (step 1) [certain]

- Each frame it faces the player and plays animation 0 (hover).
- A timer counts up from 0. After **62 frames**, and only while it is drawn (`display` = 1), it picks
  an attack by the player character and the horizontal distance `dx`:

| player | `dx` < 64 px | `dx` ≥ 64 px | when 4 or more hatchlings exist |
|---|---|---|---|
| X (`mega.var` = 0) | BLADE | LAY | BLADE |
| Zero (`mega.var` ≠ 0) | LAY | BOOMERANG | BOOMERANG |

- "Hatchlings" are all visual objects of type 27 with var 2 in the 32 visual slots, also those of
  other Nightmare Insects.
- When it is not drawn, it stays in IDLE and tests again each frame.

### BLADE (step 2, X only) [certain]

| sub-step | time | what |
|---|---|---|
| 0 | 1 frame | animation 1 |
| 1 | 48 frames | at frame 12 of animation 1 (flag 1): throws a **blade wave** (var 0) |
| 2 | 28 frames | at the last frame (flag 2): animation 0, back to IDLE |

About **77 frames** in all. Then 62 frames of IDLE.

### BOOMERANG (step 3, Zero only) [certain]

| sub-step | time | what |
|---|---|---|
| 0 | 1 frame | animation 14 |
| 1 | 48 frames | at flag 1: throws a **boomerang blade** (var 1) |
| 2 | until the blade is back | animation 14 loops frames 13–16 (16 frames per loop) |
| 3 | 15 frames | animation 2 (closes the wings); at flag 2: back to IDLE |

The boomerang sets the work byte `+0x07` of the insect to 1 when it is back. If the boomerang is gone
for another reason, the insect waits in sub-step 2 (see open questions).

### LAY (step 4) [certain]

| sub-step | time | what |
|---|---|---|
| 0 | 1 frame | animation 3 (rears up), timer 0 |
| 1 | up to 162 frames | one **hatchling** (var 2) every 20 frames, at timer 20, 40, …, 160 (8 at most; the first after 20 frames, because the test reads the timer after its increment at `0x800ECC68`) |
| 2 | 31 frames | animation 4 (back to the hover pose); at flag 2: back to IDLE |

- Sub-step 1 ends at timer 161, or at once when **8 or more** hatchlings exist.
- Animation 3 loops frames 5–11 while it lays. Its flag 1 is not used.
- Sub-step 3 (`0x800EDCEC`, `AnimeTick` only) is never reached.

### REVIVE (step 5) [certain]

- The husk animation plays: animation 5 (Easy, Normal) lasts **205 frames**, animation 15 (Hard)
  **102 frames** to its last frame (flag 3). The outline flickers faster and faster to the real
  picture.
- While it runs, the insect has no contact box and no hurt box. Its shots and hatchlings are removed
  (see below).
- At flag 3: the boxes come back, animation 0, timer 0, IDLE. HP is already 32.

## Animations (table `0x800F7650`)

| # | what | frames |
|---|---|---|
| 0 | hover, jet flame at the tail | sprites 0–3, 4 frames each, loop |
| 1 | blade throw: wings up, a blue blade forms and is thrown | 21 frames, 80 in all; flag 1 at frame 12 (after 48), flag 2 at frame 20 (after 76), hold |
| 2 | close after the boomerang | sprites 4–7, 16 in all; flag 2 at the end (after 15), hold |
| 3 | rear up to lay (body vertical) | sprites 4–11, 20–23, 4 each; flag 1 on the last; loop to frame 5 |
| 4 | back from the lay pose | sprites 8–11, 4–7, 32 in all; flag 2 at the end (after 31), hold |
| 5 | husk: green outline that flickers back to the body (Easy, Normal) | 21 frames, 206 in all (first 120); flag 3 on the last (after 205), hold |
| 6 | hatchling walk | sprites 24–28, 6 each, loop |
| 7 | spinning blue blade (blade wave and boomerang) | sprites 29–36, 1 each, loop |
| 8–11 | debris of the body | 1 frame each |
| 12, 13 | debris of a hatchling (13 is the red head) | 1 frame each |
| 14 | boomerang throw, as 1 without the closing frames | 17 frames; flag 1 at frame 12 (after 48); loop to frame 13 |
| 15 | husk, faster (Hard) | 20 frames, 103 in all (first 60); flag 3 on the last (after 102), hold |

## Shots and effects (visual object type 27, `0x800EDD2C`)

All three are made by `GetVisualObject` and copy the parent (`docs/engine.md`). Draw priority 5
[certain].

Common rules (`0x800ED584`) [certain]:

- It is removed at once when a weapon kills it, **when the insect is dead (state 2), or when the
  insect is a husk (step 5)**. A hatchling makes 2 debris pieces (animations 12, 13) and one explosion
  when this happens; the blades just go.
- Contact test each frame.
- Drawn when inside the screen plus 32 px (`DisplayCheck1`).
- A hatchling that is more than 64 px off screen and 301 px or more below the camera top is deleted.

### Blade wave (var 0) [certain]

- Starts 16 px in front of the insect. Sound bank 2, id 62. Animation 7.
- Contact damage **6**. Contact box `0x800F76EC` = (−32, −8, 64, 16): 64 × 16 px. Hurt box
  `0x800F76F0` = (−16, −8, 32, 16): 32 × 16 px. `dmg_tbl_deflect`: it cannot be destroyed.
- Flight (the timer counts frames in each part):

| part | what | ends |
|---|---|---|
| 0 | flies straight ahead at **3 px/frame** | after 92 frames, or when it is within 8 px (x) and 32 px (y) of the player |
| 1 | stops in place | after 92 frames; then it aims at the player: **5 px/frame** |
| 2 (Hard only) | flies to the aimed point | after the rest of 92 frames, or within 8 px (x) and 32 px (y) of that point |
| 3 (Hard only) | stops in place for the rest of the same 92 frames; then aims at the player again | |
| 4 | flies straight at 5 px/frame | deleted when more than 64 px off screen |

On Easy and Normal it goes from part 1 to part 4. On Hard parts 2 and 3 share one 92-frame timer: the
blade stops where the player was and then aims again.

### Boomerang blade (var 1, Zero only) [certain]

- Starts 16 px in front of the insect. Sound bank 2, id 62. Animation 7.
- Same damage (6), boxes and damage table as the blade wave: it cannot be destroyed.
- It aims at the player when made and flies straight at **5 px/frame**, for 92 frames or until it is
  within 32 px (x) and 16 px (y) of the player.
- Then it homes on a point 16 px in front of the insect (on the side of its own flip) at
  **3 px/frame**, aiming again each frame. Within 16 px (x and y) of that point it tells the insect it
  is back and goes.
- It is never deleted off screen.

### Hatchling (var 2) [certain]

- Starts 32 px in front of and 16 px above the insect, facing the same way. Animation 6. No sound.
- **HP 3**, contact damage **3**. Terrain box `0x800F76FC` = (0, 0, 8, 8): a 16 × 16 px body. Contact
  box `0x800F76F4` and hurt box `0x800F76F8` = (−4, −4, 8, 8): 8 × 8 px.
- `dmg_tbl_minor`: one X-Buster lemon (3) kills it. Its kills are not counted in the kill counter.
- FALL: drops straight down at **1.5 px/frame** (Hard: **3 px/frame**), no gravity.
- WALK (on the floor): **1.5 px/frame** (Hard: **3 px/frame**) in its facing direction. It tests a
  point 8 px ahead of its centre and 8 px below its feet. When there is no floor there, or a wall is
  ahead, it turns round. So it stays on its ledge. After **242 frames** of walking it stops.
- BURST: it stands still and flashes (palette bit 15 every 2 frames) for **52 frames**. Then its
  contact box becomes `0x800F7700` = (−16, −16, 32, 32), its hurt box is removed, it makes 2 debris
  pieces (12, 13) and one explosion, and it goes. The 32 × 32 box is set in the last frame only (see
  open questions).

## Damage

- Taken: `dmg_tbl_nightmare_insect` (the same as `dmg_tbl_normal` except id 34 = 8), 32 HP: 11 lemons,
  5 half charges, 3 full charges, 8 sabre hits, before the modifiers.
- `ApplyWeaponHit` returns `−(weapon id + 1)` on a kill. The insect tests for −2, −11 and −55, that
  is ids 1, 10 and 54 [certain]. Which weapons these ids are is not known.
- A kill by any other weapon only makes the husk. Each such kill still adds 1 to the kill counter
  [certain].
- Dealt: body 8, blade wave and boomerang 6, hatchling 3.

## Death

- **Husk** (most weapons): one explosion, 4 debris pieces (animations 8–11), no item drop, then
  REVIVE with full HP after 205 frames (Hard: 102). All its shots and hatchlings go at once.
- **Real death** (weapon ids 1, 10, 54): one explosion, 4 debris pieces (8–11), drop table 21, then
  the object is deleted.

## Sounds

Bank 2, id 62: blade wave and boomerang blade thrown. The body and the hatchlings play no sound here.

## Summary for a reimplementation

- A large dragonfly that hovers in place (no movement), faces the player, 32 HP, contact damage 8.
- Every 62 frames on screen it attacks. As X: BLADE when near (< 64 px), else LAY. As Zero: LAY
  when near, else BOOMERANG. With 4+ hatchlings out, no LAY.
- Blade wave: 3 px/frame ahead, stops over the player for 92 frames, then 5 px/frame at the player
  (Hard: one extra stop and aim). Indestructible, 6 damage.
- Boomerang: 5 px/frame at the player, then homes back at 3 px/frame. Indestructible, 6 damage.
- LAY: one hatchling every 20 frames for 160 frames (8 at most). Hatchling: 3 HP, falls 1.5 px/frame,
  walks 1.5 px/frame and turns at ledges and walls, bursts after 242 + 52 frames. Hard doubles the
  speeds.
- Most kills leave a husk that revives with full HP after 205 frames (Hard 102). Only weapon ids 1, 10
  and 54 destroy it (drop table 21).

## Open questions

1. Which weapons are ids 1, 10 and 54 (the only real kills)?
2. The hatchling sets a 32 × 32 px contact box when it bursts, but it is deleted in the next frame
   before `ContactMega` runs again. Does the burst hurt the player at all?
3. If the boomerang blade is removed before it is back, the insect waits in BOOMERANG sub-step 2
   for ever. The code has no such path: weapons cannot kill the blade, and the husk removes the blade
   but also sets step 5. An engine-side removal would cause it. With no free visual slot,
   `GetVisualObject` returns 0 and the insect goes on to sub-step 2 with no blade [certain], so it
   hangs there [likely].
4. The contact and hurt boxes are not centred (x −28 to +20, −34 to +26). Whether the engine mirrors
   them with `flip` is not read.
5. The hatchling writes the fall latch `+0x67`, but this code does not read it.
