# Totem pole (stage 08, stage object type 3, `var` 0 and 10–14): behaviour sheet

The fight at a teleporter pole of Ground Scaravich's stage, after the player destroys a
section goal. A grey pole (the base, `var` 0) holds five totem heads (`var` 10–14). Four heads are
pink faces. They turn away from the player (shots bounce off), turn back, and shoot: X gets a slow
aimed orb, Zero gets a straight arrow. The middle head (`var` 12) is a golden bird face. It stays in
the pole until two heads are destroyed (at once on Hard), then it opens its wings, flies out and
chases the player through walls. Each destroyed head takes 1 HP from the base (HP 5). When all five
are destroyed, the base explodes for 120 frames, rolls an item and counts the pole as cleared.
"Totem pole", "base", "head" and "bird head" are descriptive names; no official name was found.

Overlay `08` (stage 5, area 0). Entry `0x800EC5AC` (stage-object table entry 3), state table
`0x800F5600`. The same type is the blue teleporter pole (`var` 1–4, `docs/stages/scaravich.md`).
Save state `scaravich-3_section.state.gz`: Zero at (800, 394), the base and all five heads live at
(1136, 336) (pole 1), base HP 5, all heads HP 16 [certain]. Kit: `python3 -m mmxdemake.rekit
--state scaravich-3_section.state.gz --type 3 --class stage`. Units, tags and shared rules:
`docs/engine.md`.

## Spawn and set-up (state 0, `0x800EA820`) [certain]

In stage 5 no record makes the base. (In Gate stage 2, overlay 23, entry `0x800EFA94`, the same 28
functions after masking, six records make the base directly: kind 4, `b` 3, `id` 0, `a` 48;
`docs/stages/gate-2.md`.) The section goal (main 12) makes stage object 3 `var` 0 at the place of
the pole that the player came from (`docs/stages/scaravich.md`, step 6). The base makes the heads.

Common set-up for every `var`: flags `0x41`, state 1, step 2, `act4` (`+0x07`) 0, layer =
`mega.layer`, sprite type 130 (bank 3), animation table `0x800F5560`, **HP 16**, **contact damage
4**, draw priority 5, `dmg_tbl_normal`, hurt box `0x800F55F8` = (−14, −16, 28, 32), attack box
`0x800F55EC` = (−16, −16, 32, 32), no terrain box, `flip` 0, all speeds 0. A `var` 1–4 pole is
deleted when `game+0x2F` ≥ `var` (that rule does not apply to `var` 0 and 10–14).

| `var` | what | set-up |
|---|---|---|
| 0 | base (grey pole) | terrain box `0x800F55E4` = (0, 0, 14, 80): 28 × 160 px; **HP 5**; priority 6; no attack box; animation 19 (sprite 35); `step2` 0. It makes the five heads (below) |
| 10, 11 | top two heads (pink face) | animation 0 (sprite 0); `step2` 1; timer `+0x80` = **180** |
| 12 | middle head (golden bird face) | animation 6 (sprite 10); priority 4; `step2` 3 |
| 13 | lower head (pink face) | animation 0; `step2` 1; timer **90** |
| 14 | bottom head (pink face, other shading) | animation 3 (sprite 5); `step2` 1; timer **90** |

**The heads** (`0x800EAA14` loop): five times `GetStageObjectAfter(base, 0)` (`0x8002BFD0`: the
first free stage-object slot after the base). Each head: flags `0x41`, type 3, `var` = 14 − n,
the base's texture, palette, animation table, sprite data and `flip`, x = base x, y = base y +
{64, 32, 0, −32, −64}[n] (word table copied from `0x800E9864`), parent pointer `+0x7C` = the base.
So `var` 14 is at the bottom (y + 64) and `var` 10 at the top (y − 64). In the save state the base
is in slot 4 and the heads in slots 5–9 [certain]. Because the heads are in later slots, they run
after the base in the same frame.

## The `var` parameter

`var` picks the role (table above). The record `id` is not used: no record has `var` 0 or 10–14
[certain for the 516 records].

## State machine [certain]

States (table `0x800F5600`): 0 set-up `0x800EA820`, 1 main `0x800EAC90`, 2 wreck `0x800EC614`,
3 delete `0x800EC6BC` (`DeleteObject3`). Main sub-steps by `step2` (table `0x800F5610`): 0 base
`0x800EAEF8`, 1 head `0x800EC6DC`, 2 teleporter pole `0x800ECAB4` (`var` 1–4, see the stage page),
3 bird head `0x800ECAF0`.

### Main (state 1, all `var`)

1. Clear the hit-flash bit; keep the old HP; parent = `+0x7C`.
2. `var` ≥ 10: face the player (`flip` = `0x40` when x < player x).
3. **Damage table by frame:** when the current frame's event flag has any of the bits `0xF0`
   (flag 128), `dmg_tbl_deflect`; else `dmg_tbl_normal`. So a head is armoured in the frames with
   flag 128, and the base (animation 19, flag 128, held) is always armoured.
4. `ContactWeapon`. A kill of a head (`var` ≠ 0) goes to "Head death". A kill result of the base is
   ignored (it cannot happen: the base always deflects).
5. Hit flash when the HP changed.
6. The sub-step (`step2`).
7. `DisplayCheck(96, 128)`. There is no off-screen delete: the objects stay live and act also off
   screen (in the save state all six have `display` 0 and still count their timers) [certain].
8. When |player x − x| < 48: `SolidObject`. Only the base has a terrain box, so only the base is
   solid: a column of 28 × 160 px.
9. `ContactMega` unless `step2` = 2. The base has no attack box: it does no contact damage.

### Pink heads (`var` 10, 11, 13, 14; `step2` 1, `0x800EC6DC`)

Each update the timer `+0x80` goes down by 1, then the sub-step in `act4` (table `0x800F5620`)
runs. T = **90**, or **40** when the base HP < 3 (three heads destroyed) or on Hard (difficulty ≥ 2).

| `act4` | function | what |
|---|---|---|
| 0 | `0x800EC724` | wait for the timer. Then the "turn away" animation (1; `var` 14: 4), timer = T, sound 2/11 when on screen, `act4` 1 |
| 1 | `0x800EC7F0` | wait for the timer. Then the "turn back" animation (27; `var` 14: 28), timer = T, sound 2/11 when on screen, `act4` 2 |
| 2 | `0x800EC8BC` | wait for event flag 1 (end of the turn back). Pick the attack (below), `act4` 3 |
| 3 | `0x800EC9B4` | wait for event flag 1 (the fire frame). Make the shot (visual type 8, `var` = `+0x86`), `act4` 0 |

The attack in `act4` 2:

| player | base HP | animation (`var` 14) | `+0x86` | shot |
|---|---|---|---|---|
| X (`mega+0x02` = 0) | any | 14 (15) | 0 | aimed orb |
| Zero | ≥ 3 | 9 (10) | 1 | straight arrow |
| Zero | < 3 | 14 (15) | 0 | aimed orb |

Timing [certain for the code; the frame counts follow from the animation table]:

- First wait: 90 frames (`var` 13, 14) or 180 frames (`var` 10, 11). The two pairs are half a cycle
  apart: in the save state `var` 13, 14 are in `act4` 0 (face out) and `var` 10, 11 in `act4` 1
  (turned away), all with timer 14 [certain].
- Cycle: turned away for T frames, then face out for T frames. One shot in each face-out phase. So
  one shot per head every 180 frames (3.0 s), or every 80 frames (1.33 s) on Hard and when the base
  HP < 3.
- Turn away (animation 1: sprites 1, 2, 3, 4 for 6, 6, 6, 1 frames; flags 0, 0, 128, 128): armoured
  from frame 12 of the turn.
- Turn back (animation 27: sprites 3, 2, 1, 0, 0 for 6, 6, 6, 1, 1 frames; flags 128, 0, 0, 0, 1):
  armoured for the first 6 frames; flag 1 after 19 frames.
- Orb attack (animation 14: sprites 20, 20, 21, 22, 23, 22 for 9, 1, 1, 2, 3, 2 frames; flag 1 on
  frame 1; loop to frame 2): the orb comes out about 9 frames after the turn back ends. The mouth
  animation then loops until the next turn away.
- Arrow attack (animation 9: sprites 0, 0, 15, 15, 18, 0 for 4, 1, 1, 26, 6, 1 frames; flag 1 on
  frame 1; hold): the arrow comes out about 4 frames after the turn back ends.

### Bird head (`var` 12; `step2` 3, `0x800ECAF0`)

Sub-steps in `act4` (table `0x800F5630`). It has no terrain box: it flies through walls.

| `act4` | function | what |
|---|---|---|
| 0 | `0x800ECB2C` | in the pole, armoured (animation 6, sprite 10, flag 128). When the base HP < 4 (two heads destroyed) or on Hard: timer 10, `speed_x` = 2 px/frame to the side the head faces (to the player), sound 2/12 when on screen, `act4` 1 |
| 1 | `0x800ECBC4` | 10 frames of move. Then animation 17 (wings out), aim at the player position of this frame: 2 px/frame on the line to it (`ratan2`, `rcos`, `rsin` × 32), `act4` 2 |
| 2 | `0x800EB098` | fly. When it is within 8 px of the aimed point on both axes: `accel_x`, gravity = its speed / 16 (a constant brake), timer 16, `act4` 3 |
| 3 | `0x800ECCA0` | brake: the speed goes down to about 0 in 16 frames; at timer −1 all speeds 0, timer 40, `act4` 4 |
| 4 | `0x800EB1A0` | hover 40 frames, then aim again at the player (2 px/frame), `act4` 2 |

- In `act4` 2–4: sound 2/13 every 32 frames while on screen.
- Animation 17 (sprites 11, 12, 31, 32, 33, 34, 38; flag 128 only on frame 0 for 6 frames; loop to
  frame 3): the bird is armoured for 6 frames after it opens its wings, then it can be hit.
- It never goes back into the pole [certain for the code].

### Base (`var` 0; `step2` 0, `0x800EAEF8`)

- While HP > 0: timer `+0x80` = 120 each update.
- At HP ≤ 0 (all five heads destroyed): 120 frames. Every 8 frames an explosion (effect type 4,
  flags `0x21`) at x + (`GetRNG2` mod 16 − 16) and y + (`GetRNG2` mod 128 − 64). Every 32 frames 4
  debris pieces (`SpawnJunk`, animations 22, 23, 24, 25 from `0x800F55DC`).
- At the end: `display` 0, state 3 (`DeleteObject3` in the next update), `DropItem(table 21)`, and
  when `game.stage` = 5 and the base y is 336, 576, 1056 or 1296: `game+0x2F` += 1 (pole cleared).

### Head death (main routine, `0x800EAD30`)

- The base HP −1, `SpawnExplosion`, 3 debris pieces (animations 22, 23, 24), hurt box
  `0x800F55FC` = (−14, −16, 28, 32), `dmg_tbl_deflect`, no attack box, state 2 (wreck).
- By `var`: 10, 11, 13 → animation 20 (sprite 36, a broken face); 14 → animation 21 (sprite 37);
  12 → 5 debris pieces, sound 2/20, `display` 0, state 3: the bird leaves no wreck.

### Wreck (state 2, `0x800EC614`)

- `SolidObject` (no effect: no terrain box), `ContactWeapon` with `dmg_tbl_deflect`: shots bounce
  off the broken face, `CollideCheck`, `DisplayCheck(64, 32)`. No contact damage.
- When the base state is 3 (the end of the base death): 5 × `SpawnExplosionRand(obj, 1, 16, 16)`,
  4 debris pieces, `DeleteObject3`.

## Animations (table `0x800F5560`, 31 animations) [certain for the data]

| # | sprites | use |
|---|---|---|
| 0 | 0 | pink face, out (vulnerable) |
| 1 | 1 2 3 4 | turn away (armoured from sprite 3) |
| 3 | 5 | bottom face, out |
| 4 | 6 7 8 9 | bottom face, turn away |
| 6 | 10 | bird head in the pole (armoured) |
| 9, 10 | 0 0 15 15 18 0 / 5 5 16 16 19 5 | arrow attack (flag 1 = fire) |
| 11 | 17 | arrow (visual type 8, `var` 1) |
| 14, 15 | 20 20 21 22 23 22 / 24 24 25 26 27 26 | orb attack (flag 1 = fire; loops) |
| 16 | 28 29 30 | orb (visual type 8, `var` 0) |
| 17 | 11 12 31 32 33 34 38 | bird head opens its wings and flies (loop from frame 3) |
| 19 | 35 | base (grey pole), flag 128 held |
| 20, 21 | 36 / 37 | broken face (wreck) |
| 22–25 | 39, 40, 41, 37 | debris |
| 26 | 42 | full teleporter pole (`var` 1–4) |
| 27, 28 | 3 2 1 0 0 / 8 7 6 5 5 | turn back (flag 1 at the end) |

Not started by this code: 2, 5, 7, 8, 12, 13, 18, 29, 30 [certain for the `SetAnime` calls of this
object; another object may use them]. Sprites: `~/mmx-data/x6/sprites/banks/scaravich/scaravich-3_section/stage-3-live-0.png`
(44 sprites); the export is `~/mmx-data/x6/sprites/enemies/totem-pole/`.

## Shots: visual type 8 (`0x800ECF08`, states `0x800F587C`)

Set-up `0x800EC41C`, main `0x800ECDC4`, delete `0x800ECEE8`. Copies the parent (texture, palette
without the flash bit, animation table, sprite data, `flip`, position) [certain]. Contact damage
**4**. No gravity.

| `var` | shot | speed | start | boxes | animation, priority |
|---|---|---|---|---|---|
| 0 | orb | **1.5 px/frame** aimed at the player at the fire frame (`rcos`/`rsin` × 24) | x − 18 (facing left) or x + 18, y + 8 | attack and hurt (−4, −4, 8, 8); terrain half 6 × 6 | 16, 3 |
| 1 | arrow | **1.5 px/frame** straight, in the facing direction | x ∓ 3, y | attack and hurt (−7, −4, 14, 8); terrain half 14 × 6 | 11, 6 |

Main, each update: `AnimeTick`, `MoveObject`, clear the flash bit, then:

- touches the player (`ContactMega`): orb → `SpawnHitEffect`; arrow → `SpawnExplosion`; delete;
- arrow only: `ContactWeapon`; a kill → explosion, delete. The orb cannot be shot;
- terrain (`collide_flags` or `CornerCheck`): orb → hit effect; arrow → explosion; delete;
- 32 px off screen → delete; else visible within 16 px.

**Hazard:** the arrow has no damage table (`+0x58` = 0 after `GetVisualObject`) and HP 0, so
`ApplyWeaponHit` reads the class and damage from RAM address 0 + id × 2. That RAM is almost all 0
in the dumps (class 0, damage 0), and HP 0 − 0 ≤ 0 is a kill: so a weapon touch destroys the arrow
[likely; reads address 0]. A reimplementation should give the arrow 1 HP and a "minor" table.

## Damage

- Heads and bird: `dmg_tbl_normal`, HP 16, only in frames without flag 128. X: 6 lemons (3), 3 half
  charges (7), 2 full charges (15). Zero's sabre ids 24–31 do 4–6 [certain for the table], so 3–4
  hits. Easy: × 1.25.
- Base: always `dmg_tbl_deflect`. Wrecks: `dmg_tbl_deflect`.
- Dealt: head and bird contact 4, orb 4, arrow 4 (before the difficulty scaling). The base does no
  contact damage, but it is solid.

## Sounds [certain for the numbers]

| bank, id | when |
|---|---|
| 2, 11 | a head turns away or back (on screen only) |
| 2, 10 | orb fired (on screen only) |
| 2, 25 | arrow fired (on screen only) |
| 2, 12 | the bird leaves the pole |
| 2, 13 | the bird flies (every 32 frames, on screen) |
| 2, 20 | the bird is destroyed |

## Summary for a reimplementation

1. A solid grey column (28 × 160 px, no damage, deflects all shots) with five heads at y − 64, −32,
   0, +32, +64. HP of the column = 5 = the number of heads.
2. Four face heads: 16 HP, contact 4. Each one turns away (armoured) for T frames and faces out
   (vulnerable) for T frames, T = 90 (40 on Hard or with ≤ 2 heads left). The top pair starts 90
   frames later than the lower pair.
3. When it faces out it fires once: X gets a 1.5 px/frame orb aimed at X (cannot be shot); Zero gets
   a 1.5 px/frame straight arrow (can be shot), and orbs when ≤ 2 heads are left.
4. The middle bird head: armoured in the pole; when ≤ 3 heads are left (or on Hard) it flies out
   and chases: aim, 2 px/frame to the aimed point, brake 16 frames, hover 40 frames, again. It
   flies through walls.
5. A destroyed face stays as a shot-blocking wreck; the bird disappears.
6. When all five heads are gone: 120 frames of explosions and debris, an item roll (table 21),
   then the pole counts as cleared (`game+0x2F` += 1) and the next blue pole can be used.

## Open questions

1. The look in play: which way the face turns (sprites 1–4) and whether the player sees the base
   behind the heads.
2. The two pairs: does the 90-frame offset hold after the first cycle in all cases (timers are
   reset only by the turns)?
3. The arrow's hits from RAM address 0 (hazard above): check in play that one shot destroys it.
4. Whether Hard makes the bird leave at the first update (the condition is tested every update in
   `act4` 0) [likely yes].
5. The official names.
