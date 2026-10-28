# Nightmare Mother (stage 22, main object type 60): behaviour sheet

Nightmare Mother is the boss of Gate's lab, stage 1 (stage 16, area 0). It is one main type with
five objects, chosen by `var`: an invisible core (`var` 0) that keeps the HP bar and runs the
fight, two large shells (`var` 1, 2: masses of purple and red spheres, 160 × 128 px) that slide
along the room walls, and two eyes (`var` 3, 4), one inside each shell. Only the eyes can be hurt,
and only while they are out of their shells. An eye comes out, attacks (fireballs, lightning orbs,
a floor fire or wall-crawling sparks) and goes back in. When an eye is destroyed, its shell slams
into the walls instead. The boss dies when both eyes are destroyed.

Overlay: `rock_bin/22.bin` (81 224 bytes) at `0x800E9860`; 11 bytes differ from RAM [certain].
The overlay runs up to `0x800FD5A8`, over the address `0x800FA000` where other stages load a boss
overlay: stage 16 loads no boss overlay, the boss code is in the stage overlay [certain]. Save
state: `gate-2_nightmare-mother.state.gz` (Zero, Normal, boss level 2), about 32 updates after the
end of the intro. Kit: `python3 -m mmxdemake.rekit --state gate-2_nightmare-mother.state.gz --type
60 --name nightmare-mother` (88 functions; it also walks the shots, visual type 30, entry
`0x800FA190`). Export: `enemy-c99ffb5c2b` (bank 14, live). Units, tags and the shared rules:
`docs/engine.md`. The stage: `docs/stages/gate-1.md`.

"The room" below: x 8000–8400, y 32–352 (the shells touch these walls: shell x 8080 and 8320
with half width 80, y 96 and 288 with half height 64) [likely]. The quadrant of an object
(`QuadrantOf`, `0x800FA140`, into `+0x8F`): bit 0 = x ≥ 8201 (right), bit 1 = y ≥ 193 (bottom),
so 0 top left, 1 top right, 2 bottom left, 3 bottom right [certain].

## Spawn and set-up (`0x800F5C00`) [certain]

The set-up first writes the camera target max y = 144 and the level values (by `game+0x8F`, the
boss level):

| variable | address | level 0 | 1 | 2 | 3 | use |
|---|---|---|---|---|---|---|
| shot wait | `0x800FD59A` | 120 | 120 | 60 | 40 | updates between two spark balls |
| attack count | `0x800FD59C` | 2 | 3 | 3 | 4 | spark balls per eye attack |
| shell speed | `0x800FD5A0` | 3 | 3 | 4 | 4 | px/frame |
| slide time | `0x800FD5A4` | 480 | 420 | 300 | 240 | updates of wall slide per cycle |

Texture `0xA0`, palette from bank 14 (`0x7948` live), animation table `0x800FCE98`, sprite data of
bank 14. All objects: layer = the player's layer, speeds 0, state 1.

| `var` | object | step | HP | contact | boxes | other |
|---|---|---|---|---|---|---|
| 0 | core | 2 (INTRO) | 0 (filled in the intro) | 6, but no attack box | none | at (8200, 192), sprite 183 (empty), no damage table, never visible |
| 1, 2 | shell | 3 (shell intro) | `boss_hp_max` / 2 (not used) | **6** | attack `0x800FC958` (−64, −48, 128, 96); hurt `0x800FC950` (−80, −64, 160, 128); terrain `0x800FC948` (half 80 × 64) | priority 7 (`var` 1) / 5 (`var` 2); damage table `0x8007587C`, but see "Damage"; animation 8; flag bit `0x10` |
| 3, 4 | eye | 4 (eye) | = its shell's HP | **4** | attack `0x800FC980` (−8, −8, 16, 16); no hurt box until it comes out; no terrain box | priority 6 / 4; damage table `0x800757DC`; animation 9; display on |

- **HP.** `boss_hp_max` (`game+0x8C`) = 116 + {4, 6, 8, 9}[level]: 120, 122, **124**, 125. Each
  shell gets half (rounded down), and each eye copies its shell's HP: 60, 61, **62**, 62 per eye.
  `gate-2`: `boss_hp_max` 124, both eyes 62 [certain].
- **The bar.** The core is in main slot 0, so it runs first. Each update it sets its own HP to 0
  (not in the intro), and then each eye adds its HP to the core (a byte add; −128 becomes 127),
  as Nightmare Snake does. `game+0x20` = the core, so the bar shows the sum of the two eye HPs.
  At level 3 the intro fills the bar to 125, but the sum is 124 [certain for the code].
- The core counts the destroyed eyes in `+0x8D`: an eye adds its `var` (3 or 4) when it dies. At
  7 (both) the core dies. The test "my eye is dead" of a shell is core `+0x8D` = shell `var` + 2
  [certain].
- `var` 1 and 2 are made by the core in the intro (`GetMainObjectAfter`): `var` 1 at (8320, 96),
  `var` 2 at (8320, 288), `+0x84` = the core. An eye is made by its shell (`var` + 2, at the
  shell's position, `+0x84` = the shell, `+0x88` = the core). No record makes them [certain].
- The boss does not read `nightmare_effect`. It reads the difficulty (`game+0x68`) in two places:
  the wall-slide mode and the shell shots (below) [certain].

## State machine

States (`0x800FCF58`): 0 set-up, 1 main (`0x800F5F50`), 2 and 3 death (`0x800F8E24`, steps in
`0x800FCF98`). The entry (`0x800F8DC8`) saves the position, runs the state, then `CollideCheck`.
All five `var`s share one step table (`0x800FCF68`):

| step | function | used by |
|---|---|---|
| 0 | `HitReactionDispatch` | (not used) |
| 1 | `0x800FA188` (nothing) | — |
| 2 | INTRO `0x800F9060` (subs `0x800FCFA4`) | core |
| 3 | SHELL INTRO `0x800F93A4` (subs `0x800FCFCC`) | shells |
| 4 | EYE `0x800F95EC` (subs `0x800FCFE4`) | eyes |
| 5 | SHELL WAIT `0x800F9838` (subs `0x800FD000`) | shells |
| 6 | WALL SLIDE `0x800F74B8` (subs `0x800FD014`) | shells |
| 7 | SLAM `0x800F9C20` (subs `0x800FD01C`) | a shell whose eye is dead |
| 8 | PATH `0x800F9C5C` (subs `0x800FD034`) | shells |
| 9 | RELEASE `0x800F9D68` (subs `0x800FD4C0`) | shells |
| 10 | CYCLE `0x800F9F38` (subs `0x800FD4D0`) | core |
| 11 | `0x800FA110` (damage table `0x800757DC` once, display) | not used by the walked code |

### Main (state 1, `0x800F5F50`) [certain]

1. While `mega+0xE0` = 0, the step runs; else only `AnimeTick` (not for the core).
2. `ContactMega`, then `ContactWeapon`.
3. **Core:** `+0x8D` = 7: `mega+0x7A` (no hit) = 1, state 2. Else HP = 0 (not in step 2) and
   display 0.
4. **Eye:** adds its HP to the core (not while the core is in step 2). A kill: core `+0x8D` +=
   `var`, `SpawnExplosion`, state 3 (it deletes itself in the next update). A hit (result not 0,
   not 127) while `+0x8C` = 0: **30 updates of invulnerability** (`dmg_tbl_deflect`), the palette
   flash every 2 updates; at the end `0x800757DC` again.
5. **Shell:** when the core is in state 2, it keeps its old position and copies the core's palette
   and display (it flashes and disappears with the core). Else it sets its damage table (see
   "Damage") and clears its flash when its eye is dead.

### INTRO (core, step 2) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800F909C` | `PlayerEvent(20, 64)`. |
| 1 | `0x800F90D4` | Waits until `game.exit_req` = 0 and the player is in step 20. Camera `+0x47` = 2 (the limit speed); camera `+0x48` kept in `0x800FD598`; `PlayerEvent(29, 64)`; engine object 24; a 128-update timer. |
| 2 | `0x800F6254` | At the end of the timer: `PlayerEvent(20, 64)`, camera target min x = target max x. When object 24 is gone: `boss_hp_max`, the two shells, `game+0x24` = 1, `game+0x25` = 0, `game+0x20` = the core. |
| 3 | `0x800F9170` | Waits (the shell `var` 2 advances it). |
| 4 | `0x800F91B0` | 121 updates. |
| 5 | `0x800F921C` | Waits until both shells report (core `+0x07` ≥ 2). Camera `+0x48` = 2, `+0x7C` = 2, camera target min x = 7984, `BossMusicRequest`. |
| 6 | `0x800F92B0` | Waits for `BossMusicWait`. |
| 7, 8 | `0x800F92EC`, `0x800F9300` | Next; then waits (shell `var` 1 advances it). |
| 9 | `0x800F9308` | **+1 HP per update**, sound (0, 14) every 2 updates, up to `boss_hp_max`. Then camera target min y = **16**, camera `+0x48` = the kept value, `PlayerEventEnd`, step 10 (CYCLE). |

Sound (2, 105) plays every 64 frames in subs 2–5.

### SHELL INTRO (shells, step 3) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800F93E0` | Waits `+0x7C` (`var` 1: 0, `var` 2: 60 updates), then display on. |
| 1 | `0x800F9410` | Animation 8 (the shell flickers in, then pulses). `var` 2: at the animation's flag 1 (update 106, its 18th frame), core sub + 1 (3 → 4). |
| 2 | `0x800F6418` | While the core sub is 4 or more: every 8 frames a **spark** (visual 30, `var` 11–18 at random). When the core sub is 5: the **eye** (`var` + 2). |
| 3 | `0x800F9498` | 120 updates, then core `+0x07` + 1. |
| 4 | `0x800F950C` | `var` 2: step 5. `var` 1: waits for core sub 7, then x speed = −speed. |
| 5 | `0x800F9584` | `var` 1 moves left until it touches the left wall: core sub + 1 (8 → 9), step 5. |

So at the start `var` 1 is top left and `var` 2 bottom right. The intro lasts about 128 + 106 +
121 + the music wait + the slide + 124 updates (HP fill) [likely for the sum].

### CYCLE (core, step 10) [certain]

The core runs the cycle; the shells read the core sub-step in SHELL WAIT (step 5, `0x800F9874`,
only while the core is in step 10 with `+0x07` = 0) and each reports its end with core `+0x07` + 1.

| core sub | routine | what the core does | what the shells do |
|---|---|---|---|
| 0 | `0x800F9F74` | Mode `+0x7C` = **2** when HP ≥ `boss_hp_max` / 2 and difficulty < 2 (Easy, Normal); else **0** (low HP, or always on Hard). Facing at random (the slide direction). `+0x9A` = 0. | — |
| 1 | `0x800FA01C` | Waits for both. Then mode = `GetRNG() % 6`, path row = `0x800FD4E4`[q1 + 4 × q2] (q1, q2: the quadrants where the shells ended). | WALL SLIDE (step 6) |
| 2 | `0x800F7DC0` | Waits for both. Writes `+0x90`, `+0x91` from the mode and a random value; the next WALL SLIDE writes them again before they are read, so this has no effect. | PATH (step 8) |
| 3 | `0x800FA0DC` | Waits for both, then sub 0. | RELEASE (step 9), or SLAM (step 7) when the shell's eye is dead |

`gate-2`: core step 10 sub 1, mode 2, `+0x07` = 0; both shells in step 6 with slide timer 300 and
the first 40-update pause at 9 [certain].

### WALL SLIDE (shells, step 6; `0x800F7630`, `0x800F7850`) [certain]

- Start: timer `+0x7C` = slide time (300 at level 2); in mode 2 a **40-update pause** (`+0x8E`).
- Each leg: from its quadrant, the shell moves to the next corner at the shell speed (4 px/frame
  at level 2). Core facing right (`0x40`): top left → down, bottom left → right, bottom right →
  up, top right → left (anticlockwise). Facing left: the other way (clockwise).
- At the wall: in mode 2 a 40-update pause, then the next leg. The timer counts only while the
  shell moves. At 0: the shell writes its quadrant to core `+0x90` (`var` 1) or `+0x91` (`var` 2),
  core `+0x07` + 1, step 5.
- **Hard only** (difficulty ≥ 2), while the shell's eye lives: every 96–144 updates (`+0x93` =
  (`GetRNG() & 3`) × 16 + 96), with a 1-in-2 chance, an **aimed shot** (visual 30 `var` 10).
- The shells touch the player only by contact (6). In mode 2 (high HP, Easy or Normal) they pause
  at each corner; in mode 0 they do not.

### PATH (shells, step 8; `0x800F7B88`, `0x800F9C98`, `0x800F9D3C`) [certain]

Each shell reads a direction from its table (`var` 1 `0x800FD040`, `var` 2 `0x800FD280`; 12 rows ×
6 modes × (x, y), 48 bytes per row), multiplied by the shell speed, and moves until it touches a
wall on that side. A (0, 0) entry: no move. Then it reports and goes to step 5. The rows are
chosen by the two quadrants, so the shells go to new corners; the table is in `notes.md`.

### RELEASE (shells, step 9) and EYE (eyes, step 4) [certain]

Shell subs (`0x800FD4C0`):

| sub | routine | what |
|---|---|---|
| 0 | `0x800F9DA4` | Waits until its eye is back (`+0x07` ≠ 0) or dead. Core `+0x9A` + 1. |
| 1 | `0x800F9E1C` | Waits until core `+0x9A` ≥ 2 (both shells). |
| 2 | `0x800F7C74` | By mode and quadrant: mode 0, not top left: moves down; mode 1, bottom left: up; mode 2, not top left: right; mode 3, top right: left; else no move. |
| 3 | `0x800F9E74` | Moves until the wall, then reports, step 5. |

Eye subs (`0x800FCFE4`):

| sub | routine | what |
|---|---|---|
| 0 | `0x800F9628` | Stays on its shell's position. At animation flag 16: sound 107. Waits until the shell is in step 9 sub 0. |
| 1 | `0x800F6594` | Chooses the attack (`+0x7E`), its count (`+0x93`) and the way out (below). **Hurt box `0x800FC954` (−16, −16, 32, 32) on.** Sound 107. |
| 2 | `0x800F69A0` | Moves out at **0.5 px/frame**: 144 updates up or down (72 px), or 176 updates left or right (88 px), with a turn animation (12 right, 13 left, 14 up, 15 down). Then the first attack. |
| 3 | `0x800F6F9C` | Repeats the attack until `+0x93` = 0. Then the speed reverses and the time is set again. |
| 4 | `0x800F96C4` | Moves back in. Then a closing animation (42–45 by the facing), sound 107. |
| 5 | `0x800F97A4` | At animation flag 4: animation 11 (looks around). |
| 6 | `0x800F97F0` | Shell `+0x07` + 1, **hurt box off**, sub 0. |

The way out and the attack (`r` = `GetRNG() & 3`; "count" = attack count, 3 at level 2):

| mode | way out | `var` 3 (top eye) | `var` 4 (bottom eye) |
|---|---|---|---|
| 0 | down | r 0, 2: orbs ×3; r 1: floor fire; r 3: sparks × count | r 2: orbs ×3; else sparks × count |
| 1 | up | r 0, 1: fire fountain; else sparks | sparks |
| 2 | right | r 0, 1: floor fire; else sparks | sparks |
| 3 | left | as mode 2 | sparks |
| 4 | player y > 208: right in quadrant 2 (sparks), else down (orbs ×3); player higher: up in quadrant 2 (fire fountain), else left (sparks) | the same | the same |
| 5 | as mode 4 with quadrant 3 and the other x direction | the same | the same |

The attacks (`+0x7E`):

| `+0x7E` | name | what |
|---|---|---|
| 0 | fire fountain | A burst (visual 30 `var` 7) 16 px above and 1–3 **fireballs** (`var` 0) 32 px above; sound 111. Again every 2–7 updates for **180 updates**. |
| 1 | orbs | Two **lightning orbs** (`var` 1) 16 px below, flying left and right at 2 px/frame. Again after 90 updates, 3 pairs in all. |
| 2 | floor fire | A burst (`var` 8) 16 px below and a falling ball (`var` 2) 32 px below; the ball makes the **floor fire** (`var` 5, 6). The eye waits **300 updates**. |
| 3 | sparks | One **spark ball** (`var` 3) 16 px from the eye on the side it faces; again after the shot wait (60 updates at level 2), count times in all. |

### SLAM (a shell whose eye is dead, step 7; `0x800FD01C`) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800F7938` | By mode: 0 down, 1 up, 2 right, 3 left, at 1 px/frame with 0.25 px/frame² acceleration; modes 4, 5: step 9 at once. The position is kept in `+0x94`, `+0x96`. |
| 1 | `0x800F999C` | **41 updates of shaking** (each even frame ±1 px at random, each odd frame back). |
| 2 | `0x800F7A3C` | Accelerates to at most 4 px/frame until it hits the wall: `Rumble(5)`, sound (0, 4), `Quake2(90, 5, 2)`. |
| 3 | `0x800F9A8C` | 121 updates; then 3 px/frame the other way. |
| 4 | `0x800F9B30` | Moves until the wall. |
| 5 | `0x800F9BD4` | Stops; step 9 (sub 0 passes at once, because the eye is dead). |

### Death (states 2 and 3) [certain]

- An eye in state 3: deleted (`0x800F8E60`).
- The core (state 2): `PlayerEvent(20, <player facing>)`, `+0x61` = 25, display on. **127
  updates**; the palette flash toggles after 25, 20, 15, 10, then every 5 updates. Then engine
  object 26 (`BossExplosion`). While it runs and the screen is not white, the core flashes every
  5 updates. When object 26 is gone: `game.exit_req` = **`0x10`**, `game+0x24` = 0, `game+0x20` =
  0, camera target min y = camera y; the core deletes itself.
- The shells copy the core's palette and display, so they flash and go dark with it; nothing
  deletes them before the stage ends [likely].
- No soul, no text, no drop.

## Animations (table `0x800FCE98`, 46 animations) [certain for the frames]

Export `enemy-c99ffb5c2b` (bank 14, 216 sprites). Animations 0–45 are good. The export also has
46–51, which are wrong: 46 and 47 are the two animations of table `0x800FCF50` (the floor fire,
drawn with the sprites of bank 15), and 48–51 are words of the state table `0x800FCF58` [certain].

| # | sprites | picture | used by |
|---|---|---|---|
| 0–7 | 165–189 | lightning bolts in 8 directions, fading | sparks `var` 11–18 (animation = `var` − 11) |
| 8 | 0–3 | the shell: a mass of purple and red spheres; flickers in, then pulses (loops from frame 12) | shells |
| 9 | 7, 183, 4–10, 125–127 | the eye flickers in and opens; flags 16 at frames 15 and 21 | eye set-up |
| 10 | 0–3 | the shell pulses | not used |
| 11 | 125, 7, 126, 127 | the eye looks around (loops) | eye between attacks |
| 12–15 | 8–16, 4–6 | the eye turns right, left, up, down (flags 0–3) | eye going out |
| 16–19 | 122–141 | the eye with spikes and pink lights | not used |
| 20–22 | 95–121 | cyan bursts | not used |
| 23, 24 | 68–85 | orange and cyan bursts | `var` 7, 8 |
| 25 | 39–49, 131–133 | a blue orb grows | `var` 1 |
| 26 | 39–49 | the orb | not used |
| 27 | 43–45, 131–133 | the orb shrinks; flag 1 at the end | `var` 1 end |
| 28 | 62–67 | a vertical lightning bolt, 96 px | `var` 9 |
| 29 | 25–33 | a small blue ball | `var` 10 |
| 30 | 34–38 | a small blue-white ball | `var` 3 |
| 31 | 50–54 | a blue ring that breaks up | `var` 4 |
| 32–35, 37–40 | 142–160, 190–215 | blue sparks along a wall | `var` 3 crawling |
| 36 | 17–24 | an orange fireball | `var` 0, 2 |
| 41 | 161–164, 7 | a blue electric ring around the eye | `var` 20, 21 (not made) |
| 42–45 | 4–16 | the eye closes (flag 4 at the end) | eye going in |

## Shots (visual object type 30)

Entry `0x800FA190`; states `0x800FD578`: set-up `0x800F7EE8`, main `0x800FA848` (`ContactMega`,
then the step of table `0x800FD558`), end `0x800FA8A0`. Each copies the eye's texture, palette,
animation table and sprite data. No shot calls `ContactWeapon`: the player cannot shoot them
down [certain].

| `var` | what | contact | box | motion and end |
|---|---|---|---|---|
| 0 | fireball | 4 | `0x800FC980` (−8, −8, 16, 16) | x speed from `0x800FD4F4` (−3.5 … 3, at random), y speed 7.5–11 px/frame up (`0x800FD514`), gravity 0.3125; ends 240 px off the screen |
| 1 | lightning orb | 4 | `0x800FC980`; terrain `0x800FC970` | 50 updates still, then 120 updates at ±2 px/frame (it turns at a wall); every 40–90 updates two **bolts** (`var` 9) 64 and 128 px below; it ends early when its eye is back in or dead |
| 2 | falling ball | 0 (no attack box) | terrain `0x800FC97C` | 2 px/frame down; at a wall or floor: floor fire `var` 5 at (8128, 344) and `var` 6 at (8328, 344); sound 111 |
| 3 | spark ball | 6 | `0x800FC98C` (−4, −4, 8, 8) | aimed at the player once, 2.5 px/frame; at a wall: `var` 4 (a flash) and it **crawls along the wall at 4 px/frame** towards the player, turns at the first corner, and ends at the second; sounds 108, 110 |
| 4, 7, 8, 11–18 | flash, bursts, intro sparks | 0 | none | end at the end of their animation |
| 5, 6 | floor fire | **8** | `0x800FC988` (−127, −8, 254, 16) / `0x800FC984` (−72, −8, 144, 16) | texture `0x4A0`, sprites of bank 15, table `0x800FCF50`; hurts for **300 updates**, then no box (`var` 5 fades its palette over 31 updates); together they cover x 8001–8400, y 336–352: the whole floor; sound 112 |
| 9 | lightning bolt | **8** | `0x800FC990` (−4, −48, 8, 96) | 40 updates; sound 109 |
| 10 | shell shot (Hard) | 6 | `0x800FC98C` | aimed once, 1.5 px/frame; ends at a wall, on a hit (with a hit effect) or 240 px off the screen; sound 108 |

## Damage

`D` = deflected, `–` = no effect, a number = damage (`docs/engine.md`, "The damage tables").

**Eyes:** `0x800757DC` (`dmg_tbl_nightmare_mother`) [certain, read from the executable]:

| ids | damage |
|---|---|
| 0–15 | 2 5 3 D 6 3 D – D 2 3 3 D 8 3 D |
| 16–31 | 3 D 5 5 5 D 8 D 3 3 6 3 3 3 3 3 |
| 32–47 | 3 3 D 8 3 3 3 3 3 3 D 3 D 2 4 8 |
| 48–63 | 3 2 3 3 2 3 4 127 4 8 4 4 15 5 15 4 |
| 64–79 | D × 15, then 127 (id 79) |

The ids that matter: lemon (0) 2; half charge (9) 2; full charge parts (18–20) 5; sabre (24, 25)
3, (26) 6, (27–31) 3; deflected: 3, 6, 8, 12, 15, 17, 21, 23, 34, 42, 44. After a hit, 30 updates
of `dmg_tbl_deflect`. With 62 HP per eye at level 2: 31 lemons, 13 full charges, or 21 sabre hits
(16 with the sabre part `0x400000`, as in `gate-2`) per eye, before the Easy bonus.

**Shells:** the set-up writes `0x8007587C` (D for every id except the sabre ids 24–29, 31 and ids
50, 51 with damage 0, id 55 127, ids 7, 23, 79 –). But the shell part of the main state sets
`0x8007587C` (in WALL SLIDE, with no pause and no flash) or `dmg_tbl_deflect`, and then always
writes `dmg_tbl_deflect` over it (`0x800F61C4`–`0x800F61CC`). So after their first update the
shells deflect every weapon [certain for the code]. A shell's HP is never read.

**Dealt** (before the difficulty scaling; Zero in `gate-2` has no halving): shell body 6 (hit
class 2, heavy); eye body 4; fireball 4, orb 4; spark ball 6, shell shot 6; floor fire 8,
lightning bolt 8.

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 105 | intro, every 64 frames |
| 106 | intro spark |
| 107 | the eye opens, goes out, goes in |
| 108 | spark ball, shell shot, orb |
| 109 | lightning bolt |
| 110 | spark ball hits a wall |
| 111 | fire fountain; falling ball |
| 112 | floor fire |
| bank 0, 4 | slam (with `Rumble(5)` and `Quake2(90, 5, 2)`) |
| bank 0, 14 | HP bar tick |

## Summary for a reimplementation

- A room boss of two shells (160 × 128 px, contact 6, cannot be hurt) with an eye in each (32 ×
  32 px hurt box, contact 4). HP 62 per eye at level 2 (`boss_hp_max` 124, half each); the bar is
  the sum. 30 updates of invulnerability after a hit.
- A cycle: (1) the shells slide round the room walls for 300 updates of movement at 4 px/frame,
  both the same way (clockwise or not, at random), with 40-update pauses at the corners while HP
  is above half on Easy and Normal; on Hard they also fire slow aimed shots. (2) Each shell moves
  once to a new corner by a 12 × 6 table. (3) Each eye comes out 72–88 px at 0.5 px/frame and does
  one attack: a 180-update fire fountain, 3 pairs of lightning orbs, a floor fire that covers the
  whole floor for 300 updates, or 3 wall-crawling spark balls. Then it goes back in, and the
  shells move to a wall.
- An eye is open to hits only while it is out of its shell.
- When an eye dies, its shell shakes for 41 updates and slams into a wall (screen shake), waits
  121 updates and moves back, in place of the eye attack.
- Both eyes dead: 127 updates of flashing, the boss explosion, exit request `0x10`.

## Open questions

1. One save state, before the first cycle. The attacks, the slam and the death are read from the
   code, not seen.
2. The floor fire uses sprites of bank 15 with texture `0x4A0`; no export draws it yet (the
   shapes drawn from VRAM look like a long flat wave, but the palette is copied only when the
   fire starts) [guess for the picture].
3. Visual 30 `var` 20 and 21 (the electric ring, animation 41) have set-up code, but no code in
   overlay 22 makes them.
4. What camera `+0x48` does (2 in the intro, the old value after it); Dynamo and High Max write
   it too.
5. The official name "Nightmare Mother" is from the user; check it against the Knowledge Base
   list.
