# Rescue Reploid (stage 03, stage object type 4): behaviour sheet

The Rescue Reploid is the injured Reploid that X or Zero can save by touch. It stands on the floor or
hovers in the air. When the player touches it, the player gets one extra life and some HP, and the
Reploid teleports out. A Nightmare Virus can catch it first. Then it becomes an enemy that shoots at
the player; when the player destroys it, the Reploid is dead for the rest of the game. The game has no
official enemy name for it; "Rescue Reploid" is a plain descriptive name. The sprites are sprite
bank 141 ("savable Reploid", `docs/objects-legend.md`).

Overlay `03` (Commander Yammark), but **all the code is in the main program** (`0x8004E800`–
`0x800500C0`), so every stage uses the same object [certain]. Save state `yammark-1_start`; kit
`python3 -m mmxdemake.rekit --name rescue-reploid --state yammark-1_start.state.gz --type 4
--class stage`. Units, tags and shared rules: `docs/engine.md`. Records 37–52: `docs/stages/yammark.md`.
Stage 7 (Shield Sheldon) records and an infection seen in four save states: see "Stage 7" below.
Export: `rescue-reploid` (`enemy-61922ae859`, from `sheldon-6_tile-shown`; the live Reploid there
is infected, but the export draws every animation of the table, also the "HELP!" idle).

## Spawn and set-up [certain]

- Class: stage object (`0xA4` slots at `0x800C1220`). Flags `0x41`, draw priority 2, layer =
  `mega.layer`. `flip` stays 0: the object has separate left and right animations (below).
- Damage table `dmg_tbl_normal`, but no hurt box and no attack box while it waits to be saved, so it
  cannot be hit and cannot hurt [certain for the code].
- Terrain box `0x80078910` = (0, 0, 8, 16): a 16 × 32 px body. Set B (`id` 16–31) uses another
  terrain box, `0x8007891C`, and has no hover branch (`0x8004EA0C`) [certain].
- **Already done:** the set-up reads the Reploid's status (below). Status 2, 3 or 4 → state 8
  (removed at once; the record stays marked "spawned", so it does not come back in this visit).
- **Standing or hovering:** one `CollideCheck` at the record point. With the floor flag it stands
  (`+0x7E` = 0). Without it, it hovers (`+0x7E` = 1) and keeps its start y in `+0x7C`.
- `act4` (`+0x07`) = 1: "can be caught by a Nightmare Virus".

## The `id` parameter [certain for the code, likely for the meaning]

The `id` is the **global Reploid number**. It indexes `game.reploids` (`0x800CCFA8` = `game+0xD8`,
16 words, 4 bits per Reploid: word `id / 8`, bits `(id % 8) × 4`). The two helpers are
`GetReploidStatus` (`0x8004FF4C`) and `SetReploidStatus` (`0x8004FF00`).

| status | meaning | set by |
|---|---|---|
| 0 | not met yet | – |
| 2 | rescued | the rescue (state 1) |
| 3 | dead (destroyed while infected) | the weapon kill in state 6 |
| 4 | infected ("missing") | the infection, and again when the infected Reploid leaves the screen |

Other effects of the `id`:

- `id >> 4` (the group of 16) picks the muzzle offset of the shot (table at `0x80011674`, 8 groups)
  [certain]. 8 groups × 16 = 128 Reploids, probably 16 per Maverick stage [guess].
- `id` 16–31 (`id & 0xF0` = `0x10`, flag `+0x84` = 1) use a second animation set and a second attack
  (a lobbed bomb) [certain for the code]. No record of this section has such an `id`.
- `id` `0x80`: a special Reploid. No status read or write, cannot be caught, never removed off screen
  [certain for the code; no record uses it here].

The 16 Reploids of this section have `id` 0–15. Proof from the save state `yammark-2_boss`: the
status words hold 2 for ids 0, 1, 2, 5, 8, 9, 10, 12, 13, 14 and 0 for ids 3, 4, 6, 7, 11, 15, and
`game.lives` went from 1 (start) to 8 [certain for the values].

## State machine

State table `0x80078C7C` (9 entries). This is not the standard main-object update. The entry
(`0x8004F834`) saves `prev_x`/`prev_y` and calls the state; there is no `CollideCheck` after it.

| state | routine | what |
|---|---|---|
| 0 | `0x8004E800` | set-up (above) |
| 1 | `0x8004EA78` | WAIT: waits to be saved |
| 2 | `0x8004F880` | HEAL: gives the HP |
| 3 | `0x8004F8C0` | TELEPORT START |
| 4 | `0x8004F9A0` | TELEPORT WAIT |
| 5 | `0x8004FAD0` | BEAM UP |
| 6 | `0x8004FB60` | INFECTED (an enemy, with steps) |
| 7 | `0x8004FB40` | off screen: `DeleteObject2` (comes back later) |
| 8 | `0x8004F8A0` | gone: `DeleteObject3` (record stays "spawned") |

### WAIT (state 1)

Each update, in this order:

1. `AnimeTick`.
2. **Face the player** (`0x8004FE8C`): `+0x82` = `0x40` when the player's x ≥ the Reploid's x, else 0.
   When it changes, the idle animation of the new side starts.
3. **Rescue test** (`0x8004EECC`). The test object is the player (`mega`, `0x800970A0`); when
   `mega+0xD5` ≠ 0 it is the object at `0x800CD340` (or `0x800CD3F0`) [guess: another player body].
   The rescue happens when that object's HP (`+0x5C` & `0x7F`) is not 0 **and** its terrain box
   overlaps the Reploid's terrain box (`0x8002DCF4`). Then:
   - status 2 (except `id` `0x80`);
   - `game.lives` (`0x800CCF09` = `game+0x39`) +1, maximum 9. Sound bank 0 id 21 when it is still
     below 10 (no sound at the cap);
   - a random index r = 0–3 (equal chances, `GetRNG & 3`); heal amount = `{2, 4, 8, 16}[r]` HP
     (`0x80078BE8`);
   - HP not full: the game pauses the other objects (`game+0x10`…`+0x17`, `+0x19` = 1), the object
     sets flag bit 3 (it keeps running in the pause [guess]) and goes to HEAL. HP full: HP =
     maximum, straight to TELEPORT START;
   - **sub tank:** the first sub tank that the player owns (`game.tanks`, `game+0x6A`, bits `0x1000`
     and `0x2000`) and that is not full (`game+0x63` / `+0x64` & `0x7F` < 32) gets **r units** (0–3,
     not the heal amount), maximum 32. Sound bank 0 id 24 when it becomes full [certain for the code];
   - `act4` = 0.
4. **Infection test:** `act4` = 2 (written by the Nightmare Virus code, see below) → status 4,
   state 6, step 2.
5. **Hover:** when it hovers, y = start y + `rsin((frame & 127) × 32) >> 10`, with `frame` the global
   counter `0x80097420`: a sine of ±4 px with a period of 128 updates.
6. Off screen 48 (`OffScreenCheck2`) → display off, state 7, except `id` `0x80`. Else visible
   16 (set A) or 32 (set B).

### HEAL (state 2)

- Every 4 updates: HP +1 (bit 7 of the HP byte is kept), sound bank 0 id 13, heal amount −1.
- It ends when the amount is used up or the HP reaches the maximum (`game+0x5B` + character, the
  maximum HP of X or Zero). Then: pause off (all flags 0), flag bit 3 off, TELEPORT START.
- So the heal lasts 4 × n updates (n = 2, 4, 8 or 16) [certain].

### TELEPORT START, WAIT, BEAM UP (states 3–5)

- State 3 (1 update): faces the player, starts the teleport animation (9 / 17 standing, 10 / 18
  hovering; set B 25 / 32), terrain box off, timer `+0x81` = 80.
- State 4: waits **80 updates**. Then `speed_y` = 8 px/frame upwards, the beam animation (8; set B
  29), state 5.
- State 5: while the frame has flag 1 (after the first 8 updates of the beam), `MoveObject`: it rises
  8 px/frame. More than 64 px off screen → `DeleteObject3`.

### INFECTED (state 6)

Each update: `AnimeTick`, the step (table `0x80078CA0`), then the weapon test, the contact test and
the screen test:

- **Killed** (`ContactWeapon` < 0): a main object of type 15 with `var` `0x31` at its position
  (`0x8004FF7C`): a Nightmare Soul worth 4 souls (`nightmare-virus.md`). Then status 3,
  `SpawnExplosion`, state 8. No drop table.
- Else `ContactMega`. Off screen 64 (`OffScreenCheck`) → status 4 again, state 8: **an infected
  Reploid that leaves the screen is lost.** Else visible 32. While `+0x83` ≠ 0 the palette flash bit
  is set each update (a white glow).

| step | routine | what |
|---|---|---|
| 0 | `HitReactionDispatch` | standard |
| 1 | `0x8004FE50` | empty (not used) |
| 2 | `0x8004EFD0` | CHANGE: the infection animation (1 / 12 standing, 6 / 15 hovering; set B 24 / 31) for **130 updates**; then sound bank 2 id 65 and the white glow for **60 updates** |
| 3 | `0x8004FC40` | ARM: HP 13, contact damage 6, boxes, infected idle animation (19–22; set B 28 / 35), timer 60 (set B 20) |
| 4 | `0x8004FD58` | WAIT: counts the timer down, then the attack animation (2 / 13 standing, 7 / 16 hovering; set B 26 / 33) |
| 5 | `0x8004F144` | ATTACK: frame flag 1 → fire (below); flag 2 → face the player, idle animation, timer 60 (set B 20), back to step 4 |

Boxes of the infected Reploid [certain]:

| set | attack box | hurt box |
|---|---|---|
| A (`id` not 16–31) | `0x80078908` = (−8, −10, 16, 20) | `0x8007890C` = (−8, −10, 16, 20) |
| B (`id` 16–31) | `0x80078914` = (−16, −24, 32, 32) | `0x80078918` = (−16, −24, 32, 32) |

The hurt box is null before step 3, so it cannot be hit during the change [likely: the slot is
cleared on spawn and nothing else sets it].

**Fire** (flag 1): only when `mega+0xD4` = 0. Sound bank 2 id 46. Set A: two visual objects of type
24, `var` 0 (muzzle flash) and `var` 1 (bullet). Set B: one, `var` 16 (bomb).

**Timing of set A** (animation 2): 50 updates, then 3 shots 51 updates apart, the end 51 updates after
the third; then the 60-update wait. One cycle: **about 263 updates, 3 shots**. Animation 16 (hovering,
facing right) starts with 30 updates instead of 50.

### The Nightmare Virus and `act4`

The snatcher Nightmare Virus (main type 15, record `id` 32 and 33; see `nightmare-virus.md`, SNATCH)
calls `FindReploid` (`0x80042D98`) [certain]. It takes the nearest stage object of type 4 that is
visible (`display` ≠ 0) and has `act4` = 1, within 1000 px (squared distance < 1 000 000), and keeps
it at virus `+0x80`. `0x80042E68` tests that the target is still valid. When the virus reaches the
Reploid it writes `act4` = 2 (`sb 2, 7(s2)` at `0x80040DF4`) [certain].

## Stage 7 (Shield Sheldon): an infection in the save states

Save states `sheldon-4_no-armor`, `sheldon-5_nightmare-hidden`, `sheldon-6_tile-shown` and
`sheldon-7_lasers` (stage 7, area 0, X, Normal) show one Reploid before, during and after an
infection. It is the same main-program object as above [certain: the live object is stage-object
type 4 with the animation table `0x80078BEC`].

**Records** (area 0) [certain]:

- 8 Reploids, stage-object type 4, `id` **96–103** (`0x60`–`0x67`): group 6 (`id >> 4`), set A.
  Points: 96 (728, 400), 97 (1720, 432), 98 (2168, 432), 99 (3544, 848), 100 (3768, 832),
  101 (4408, 864), 102 (4496, 816), 103 (4616, 864).
- Their status nibbles are in the word `game+0xD8 + 12 × 4`.
- Group 6 muzzle offset (`0x80011674` + 24): **(−10, −2)** facing left, **(+8, −2)** facing right.
- Nightmare Virus records in the area: three snatchers `id` 34 (`0x22`, pattern row 2: IDLE, DASH,
  SHOOT, DASH) at (1768, 352), (2248, 400) and (3624, 880); `id` 33 at (4576, 928); `id` 0 at
  (4736, 864) (`docs/enemies/nightmare-virus.md`).

**What the states show** [certain for the values]:

| state | player x | Reploid 96 | Reploid 97 | live | souls (X) |
|---|---|---|---|---|---|
| `sheldon-4` | 160 | 2 saved | 0 | – | 312 |
| `sheldon-5` | 1574 | 2 | **4 infected** | Reploid 97: state 6, step 5 (ATTACK), HP 13, contact 6; one bullet | 312 |
| `sheldon-6` | 1661 | 2 | 4 | the same, still firing | 312 |
| `sheldon-7` | 3034 | 2 | **3 dead** | – | **316** |

- The snatcher record at (1768, 352) is marked spawned in `sheldon-5`, and no virus is live. So the
  virus took Reploid 97 (record point 1720, 432) and then deleted itself, as SNATCH says [likely:
  it is the nearest snatcher record, 48 px right of and 80 px above the Reploid].
- Reploid 97 stands (`+0x7E` = 0) and faces left (`+0x82` = 0): idle animation 19, attack
  animation 2.
- The bullet is at (1570, 430) in `sheldon-5` and at (1594, 430) in `sheldon-6`, with `speed_x`
  −4 px/frame: a set-A bullet (visual type 24, `var` 1) at y 432 − 2, the group-6 muzzle height.
- Between `sheldon-6` and `sheldon-7` the player destroyed Reploid 97: its status is 3, and X's
  soul count is 4 higher. This agrees with the `var` `0x31` soul (4 souls); other souls in that
  time are not excluded.
- Reploid 98 is 2 (saved) in `sheldon-7`.

So the infection in Shield Sheldon's stage follows the rules above with no change: change pose 130
updates, glow 60, then HP 13, contact 6, 3 bullets of 4 px/frame per cycle of about 263 updates;
killed: 4 souls and no item; off screen: lost (status 4).

## Animations (table `0x80078BEC`, 36 entries) [certain]

Frames are {duration, flag, sprite}. "L" = player on the left (`+0x82` = 0), "R" = on the right.

| # | set | what | frames |
|---|---|---|---|
| 0 / 11 | A, B (23 / 30) | idle standing L / R | 3 sprites, 16 updates each, loop (`0x19`–`0x1B` / `0x22`–`0x24`) |
| 5 / 14 | A | idle hovering L / R | 3 sprites, 16 each, loop |
| 1 / 12 | A, B (24 / 31) | infection change standing L / R | 3 sprites, 16 each, loop |
| 6 / 15 | A | infection change hovering L / R | 30, 2, 1, 1, loop |
| 19 / 20 / 21 / 22 | A | infected idle standing L / R, hovering L / R | 3 sprites, 16 each, loop |
| 28 / 35 | B | infected idle L / R | 3 sprites, 16 each, loop |
| 2 / 13 / 7 / 16 | A | attack standing L / R, hovering L / R | 50 (30 for #16), then 3 × {1 flag 1, 20, 30}, then flag 2 (hold) |
| 26 / 33 | B | attack L / R | 6, 7, 8, 7, 1 flag 1, 8, then flag 2 (hold) |
| 9 / 17 / 10 / 18 | A, B (25 / 32) | teleport start standing L / R, hovering L / R | 3 sprites, 16 each, loop |
| 8 | A | beam up | sprites `0x12`–`0x15`, 2 each, then `0x16` with flag 1 (hold) |
| 29 | B | beam up | as 8, 5 frames of 2, then flag 1 (hold) |
| 3 | shot | muzzle flash | 4, 4, then flag 1 (hold) |
| 4 | shot | bullet | 2 sprites, 2 each, loop |
| 27 | shot | bomb | 4 frames (5, 4, 5, 4), loop |
| 34 | – | as 27 with other sprites | no user found |

All flags that the code tests (1 and 2 in the attack, 1 in the beam and the muzzle) are in the
frames.

## Shots (visual object type 24, code `0x8004F2EC` / `0x8004F624` / `0x800500C0`, table `0x80078CCC`)

Made by `0x8004FFF8`: copies the parent (`docs/engine.md`); `flip` = the parent's `+0x82`; the parent's
`id` goes to `+0x88`; parent pointer at `+0x7C`. HP 1, contact damage 4, no damage table, draw
priority 1. Start point = parent + the group offset (`0x80011674`, by `id >> 4`); for this section
(group 0): (−14, −6) facing left, (+14, −6) facing right.

- **`var` 0, muzzle flash:** animation 3, 9 updates, then removed. No boxes.
- **`var` 1, bullet:** 20 px further ahead than the muzzle. Hidden for 6 updates, then attack box
  `0x80078CB8` = (−2, −2, 4, 4) and terrain box `0x80078CBC` = (0, 0, 2, 2). Speed **4 px/frame**
  straight ahead, **8 px/frame on Hard** (difficulty ≥ 2). It ends on terrain (`collide_flags` or
  `CornerCheck`: `SpawnHitEffect`) or 32 px off screen. It cannot be shot, and it does **not** end when
  it hits the player (the `ContactMega` result is not used) [certain for the code].
- **`var` 16, bomb (set B):** animation 27; start (±28, −24) from the parent; terrain box
  `0x80078CC0` = (0, 0, 4, 4), attack box `0x80078CC4` = (−4, −4, 8, 8), hurt box `0x80078CC8` =
  (−4, −4, 8, 8); HP 2, contact damage 3. `speed_y` 3 px/frame up, gravity `0x1800` (0.094 px/frame²),
  `speed_x` = (player x − x) / 70 px/frame (`MoveObject2`), so it lands near the player. Every
  8 updates it calls `0x8004A2B4` (a trail effect [guess]). It explodes on terrain, on a weapon kill or
  on contact with the player; it is removed 16 px off screen.

### The shot code, checked 2026-10-22 [certain for the code]

- The visual table (scratchpad `+0x84`) entry 24 is `0x800500E0`, a dispatcher: it runs
  `0x80078CCC[state]`, that is state 0 `0x8004F2EC` (set-up), state 1 `0x8004F624` (update),
  state 2 `0x800500C0` (`DeleteObject3`). The table has 3 entries.
- The set-up picks the kind by `var`: 0 is the muzzle flash, 1 the bullet, and **any other value**
  the bomb (the bomb maker uses 16). The bullet's 6-update wait is the counter `+0x88` = 6. The
  bullet speed is `speed_x` = ±`0x40000` (4 px/frame), doubled when `game.difficulty` ≥ 2. The
  bomb's `speed_x` is `(player x − x) × 0xEA0EA0EB >> 6` in 16.16, that is the distance / 70.
- At the end of the set-up, `OffScreenCheck2(32, 32)` = 1 sends a new shot at once to state 2.

## Damage

- Taken (infected only): `dmg_tbl_normal`, **HP 13**: lemon 3 → 5 hits; half charge 7 → 2; full
  charge 15 → 1; sabre 4 → 4.
- Dealt: infected contact 6; bullet 4; bomb 3 (before the difficulty scaling).

## Death

Infected and destroyed: one explosion, a soul of 4 (main type 15 `var` `0x31`) at its position,
status 3. Rescued: no death; it teleports out.

## Sounds

Bank 0: 21 extra life, 13 HP tick, 24 sub tank full. Bank 2: 65 the infection glow, 46 a shot.

## Summary for a reimplementation

- 16 Reploids in this section, `id` 0–15 = global Reploid numbers; keep a status per id (0 new,
  2 saved, 3 dead, 4 infected) across the game; do not spawn a Reploid with status 2–4.
- Stands, or hovers ±4 px on a 128-frame sine when no floor is under its start point.
- Player touch (terrain boxes, HP > 0): +1 life (max 9), +2/4/8/16 HP at 1 HP per 4 frames with the
  world paused, 0–3 units into the first non-full sub tank; then waits 80 frames and beams up at
  8 px/frame.
- Caught by a Nightmare Virus: 130 frames change, 60 frames white glow, then an enemy: 13 HP,
  contact 6, fires a 4 px/frame bullet (8 on Hard, damage 4) 3 times per ~263 frames.
- Infected and off screen: lost (status 4). Destroyed: dead (status 3).

## Open questions

1. Where the Reploids that carry a part give it (not in this code).
2. What `0x800CD340` / `0x800CD3F0` is (the rescue tester when `mega+0xD5` ≠ 0).
3. `mega+0xD4` is the player's boss-gate state (`docs/enemies/boss-gate.md`): the Reploid does not
   shoot while the player passes a gate.
