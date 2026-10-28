# Nightmare Pressure (stage 07, main object type 16): behaviour sheet

Nightmare Pressure is the mini boss at the end of section 2 of Metal Shark Player's stage. It is a
large press machine that fills the top of a one-screen room. The machine itself is drawn on **BG
layer 1**: the code moves it by changing the layer 1 camera. On the machine are a **head** (the
body object, `var` 0) and six **parts** on the head: three **pods** that can be hurt, and three
armoured **turrets**. The head comes down from the machine for the attacks that use the parts,
and goes back up after each one. The machine also presses down on the whole room, and it drops
bombs. Five solid stage objects (type 10) give the machine its walls, its press plates and the
block under the head.

Stage overlay `07` (`rock_bin/07.bin`, loaded at `0x800E9860`; stage 4, area 1) [certain: the
overlay file matches RAM in both states below]. Save states (in `~/mmx-data/x6/dumps/`):
`shark-4_mini-boss.state.gz` (checkpoint 1, X form 1 at (137, 1115), camera (10, 944); the fight
has started: HP full, first WAIT) and `shark-5_after-mini-boss.state.gz` (checkpoint 2, X at the
boss gate). Written with the RE kit (`python3 -m mmxdemake.rekit --state shark-4_mini-boss.state.gz
--type 16`, and `--type 10 --class stage`) and `docs/re-guide.md`. Units, confidence tags and the
shared engine rules: `docs/engine.md`. The stage: `docs/stages/shark.md`. The name: "Nightmare Pressure"
(ナイトメア・プレッシャー, Naitomea Puresshā), the mid-boss of the Recycle Lab main route [Fandom:
the stage page "Recycle Lab" lists "Nightmare Pressure (sub-boss)", and the page "Nightmare" has a
section on it; both read as wikitext through the MediaWiki API, `api.php?action=parse`].

The objects of the fight:

| class, type, `var` | object | made by |
|---|---|---|
| main 16, 0 | the head (the boss object: HP, the attack steps) | record 75 of area 1 |
| main 16, 16, 17, 18 | pods: top left, middle right, bottom left (they can be hurt) | the head set-up |
| main 16, 32, 33, 34 | turrets: top right, middle left, bottom right (armoured) | the head set-up |
| stage 10, 0, 1 | side walls (left, right) | the head set-up |
| stage 10, 16, 17 | press plates (left half, right half) | the head set-up |
| stage 10, 32 | head block (solid under the head) | the head set-up |
| visual 13, 0 / 16–18 / 32–34 / 48 | bomb / aimed shot / Zero's missile / beam | the head, the turrets, the pods |

## Coordinates

"Room" is the view: world − camera. The camera is fixed at (10, 944) for the fight (limits
10–10, 944–944) [certain for the state]. The floor is at room y 192 (X's feet at y 1136)
[likely].

**The machine.** Its objects use machine offsets `(ox, oy)` [certain]:

- world x = `cam0.x + cam1.x + ox`, world y = `cam0.y + 952 − cam1.y + oy`;
- at rest `cam1.y` = 872 and `cam1.x` = 0: room `(ox, 80 + oy)`. So the **machine origin** is at
  room y 80. When the code lowers `cam1.y`, the machine and all its objects go down by the same
  amount.

| `cam1.y` | machine origin (room y) | when |
|---|---|---|
| 872 | 80 | rest |
| 824 | 128 | intro drop |
| 800 | 152 | PRESS |
| 776 | 176 | END CRUSH (not reached, see below) |

## Spawn and set-up [certain]

- **Trigger:** the standard spawner. Record 75 of the area 1 list (`0x800F7BD8`): main 16 `id` 0,
  kind 0 (all difficulties), at (160, 1064), the middle of the room. The head is made when the
  record comes within 48 px of the view, so when the camera arrives in the room.
- **The head** (`var` 0): HP 0 (it fills in the intro to 48), contact damage 8 but **no attack box
  and no hurt box**: the head itself cannot hurt X and cannot be hit. `dmg_tbl_normal` (not used:
  no hurt box). Draw priority 34. Animation table `0x800F8638` (17 animations). Machine offset
  (160, −64): the head is inside the machine, not drawn (`display` 0). Animation 0.
- The set-up makes the six parts (main slots after the head) and the five stage parts. Then state
  1, step 2 (INTRO).
- **HP: 48, with no boss level bonus.** The intro writes `boss_hp_max` = 48 and fills the HP to
  48; it calls `BossLevel`, but nothing here reads the level [certain]. The HP of the head is the
  sum that the parts take (see "Damage").
- **No WARNING:** the intro does not make engine object 24 and does not call `PlayerEvent(29, …)`
  (D-1000 and Shield Sheldon do).

## The parts (main 16, `var` 16–34) [certain]

Each part: HP 16, contact 0, no attack box (except the drill), hurt box `0x800F852C` (−8, −8, 16,
16): 16 × 16 px. Priority 33. Its place is the head + an offset, every update:

| `var` | part | offset from the head | facing | damage table |
|---|---|---|---|---|
| 16 | pod, top left | (−16, −32) | left | `dmg_tbl_normal` when X is left of the head, else deflect |
| 17 | pod, middle right | (+16, 0) | right | `dmg_tbl_normal` when X is right of the head (or level), else deflect |
| 18 | pod, bottom left | (−16, +32) | left | as 16 |
| 32 | turret, top right | (+16, −32) | right | `dmg_tbl_deflect` always |
| 33 | turret, middle left | (−16, 0) | left | `dmg_tbl_deflect` always |
| 34 | turret, bottom right | (+16, +32) | right | `dmg_tbl_deflect` always |

So the head is a 2 × 3 grid of 32 × 32 px cells, with the pods and the turrets in a checkerboard.
A pod can be hurt only from the side it faces.

- While the head is in the machine (head state `+0x8F` 0), the parts are not drawn and **cannot
  be hit** (the part update returns before `ContactWeapon`). The head comes out only in TURRET
  SHOTS, DRILL and BEAM. During the intro the parts do nothing.
- **Pod hit:** 30 updates of invulnerability with `dmg_tbl_deflect` (2 updates after weapon ids 24
  and 25, the first sabre hits); it flashes every 2 updates. Each pod has its own timer.
- **Pod killed:** explosion, 4 debris (animations 14, 14, 15, 15); it is gone for the rest of the
  fight. When a BEAM waits for that pod, the dead pod frees the head at once (no beam).

## The stage parts (stage 10) [certain for the code; the room places are likely]

They copy the head's graphics and keep the head in `+0x7C`. While the head is in the intro they are
placed but do nothing (not solid). All use `dmg_tbl_capsule` (class 7: shots are absorbed, no damage
[likely]) and call `SolidObject`, so they are solid on all sides. They also test the player's shots
against their terrain box (`0x80032B34`) [likely].

| `var` | part | machine offset | terrain box | contact | what |
|---|---|---|---|---|---|
| 0, 1 | side wall, left / right | (16, y) / (304, y) | centre (2, −8), 36 × 112 | 8 | the only stage part that is drawn (animation 16: a purple pillar with a drill). Attack box (−32, 16, 16, 16) on the inner side near the bottom. `y` moves: see below |
| 16, 17 | press plate, left / right | (80, −80) / (240, −80) | 160 × 160 | **127** | not drawn (the layer draws the machine). Together they cover room x 0–320, from the machine origin up. No attack box: the 127 is the squeeze damage (instant death, `docs/engine.md`, "Squeeze") |
| 32 | head block | the head | 56 × 112 | 8 | solid only while the head is out; reaches from the head origin 56 px down (to the floor when the head is fully out) |

**The walls move** (their `y` offset, 2 px/frame unless marked): to +64 (down) while the head's
`+0x90` is 0; to −64 (up) while it is 1 (the PRESS); to −64 at 4 px/frame while it is 2 (END
CRUSH). They start at −64 and come down at the start of the fight (64 updates). At +64 the wall box
reaches from room y 80 to the floor [likely]. Under a lifted wall, at the bottom of the PRESS,
there is a 56 px gap [likely].

**The head block** under the rising head: when X is under it and on the floor, it clears X's
object-touch flags (`mega+0x71`), so the rising head does not squeeze X.

## State machine

States `0x800F868C`: 0 set-up, 1 head main, 2 head death, 3 part main, 4 part dead, 5 delete.
Head steps `0x800F86A4`: 0 engine hit reaction (not used), 1 nothing, 2 INTRO, 3 WAIT, 4 PRESS,
5 BOMBS, 6 TURRET SHOTS, 7 DRILL, 8 BEAM, 9 END CRUSH.

### Head main (state 1) [certain]

1. If not in the intro and HP ≤ 0: death (state 2).
2. If the layer 0 camera x ≥ 5440 (and not yet): step 9, END CRUSH.
3. `AnimeTick`, the step, then the place from the machine offset.

The head does not call `ContactWeapon`: all damage comes through the pods.

### INTRO (step 2) [certain]

| sub | what |
|---|---|
| 0 | `PlayerEvent(20, 64)`: X is held, facing right. |
| 1 | When X is in step 20: HP bar on (`game+0x24` = 1), `game+0x20` = the head, `boss_hp_max` = 48, `BossLevel`. |
| 2 | The machine **drops 48 px at 8 px/frame** (`cam1.y` 872 → 824, 6 updates). Sound 68, `Rumble(5)`, `Quake(6, 10, 2)`. |
| 3 | **180 updates** of shaking: every 8 updates `Quake(6, t / 30 + 1, 2)` (t = updates left: 7 down to 1). At the end: sound 67. |
| 4 | The machine rises 8 px/frame back to 872 (6 updates). |
| 5 | **+1 HP per update** to 48 (48 updates), a tick (bank 0, id 14) every 3 updates. Then `PlayerEventEnd` and THINK. |

The intro is about 240 updates (4 s) after X reaches step 20.

### THINK [certain]

THINK has no wait; it runs at the end of every attack and picks the next step from a pattern of
two steps. The patterns (`0x800F87B0`; the low-HP table `0x800F87F8` is byte-identical):

| pattern | steps |
|---|---|
| 0, 4 | WAIT, BOMBS |
| 1, 5 | WAIT, TURRET SHOTS |
| 2, 6 | WAIT, DRILL |
| 3, 7 | WAIT, BEAM |
| 8 | WAIT, PRESS |

- The first pattern is 0 (WAIT, BOMBS).
- At the end of a pattern: a random pattern (`GetRNG() & 7`) three times, then pattern 8 (the
  PRESS), then three random ones again. So the order is: bombs, 3 random attacks, PRESS, 3 random
  attacks, PRESS, … Each random attack is bombs, turret shots, drill or beam with 1/4 each.
- At the first THINK with HP ≤ 24 (half), it changes to the low-HP table with a new random
  pattern (the PRESS count does not change). The tables are the same, so **the fight does not
  change at half HP**.
- The difficulty is read only in TURRET SHOTS.

### WAIT (step 3)

**60 updates**, then THINK. Every attack comes after a 60-update wait.

### PRESS (step 4) [certain]

1. Sound 69. The walls go up (`+0x90` = 1). 20 updates.
2. The machine moves **down 72 px at 0.8 px/frame** (`cam1.y` 872 → 800, 90 updates). At the
   bottom: sound 68, `Rumble(5)`.
3. **60 updates** at the bottom: every 4 updates `Quake(2, 1–4, 2)`. Then sound 67.
4. It rises **1.5 px/frame** (48 updates) to 872. Sound 69; the walls go down (`+0x90` = 0).
5. 20 updates, then THINK.

At the bottom the press plates end at room y 152, **40 px above the floor** [likely]. X is 38 px
tall (terrain box half height 19), so a standing X has 2 px of space and is not squeezed; a jump
hits the plates [likely: from the boxes; a play check is needed]. The 127 of the plates acts only
as a squeeze (X between a plate and the floor) or when X's centre is inside a plate. While X does
a special attack (`mega.special` = 2) the PRESS stops after its first 20 updates (as the Nightmare
Press, `docs/enemies/nightmare-press.md`).

### BOMBS (step 5) [certain]

Four bombs, each after a random wait of **50–89 updates** (`GetRNG() % 40 + 50`), then THINK.
Each bomb (visual 13 `var` 0) starts at a random room x 160–287 (`160 + (GetRNG() & 127)`) and
room y 64 (machine offset y −16, inside the press plates). See "Shots".

### TURRET SHOTS (step 6) [certain]

1. The head comes out at machine x 160 (room x 160): sound 69, drawn; it moves **down 120 px at
   2 px/frame** (offset −64 → 56, 60 updates). The head is then at room y 136; its block reaches
   the floor.
2. It picks a turret (`act4` 1, 2, 3 = turret 32, 33, 34):
   - X: the turret for which the head is nearest to the place where that turret would line up
     with X (head at X + (−16, +32) for 32, X + (+16, 0) for 33, X + (−16, −32) for 34). In
     practice the turret at the height of X, on X's side.
   - Zero: 33 when the head x ≥ Zero's x; else 34 when the head is above Zero; else 32.
3. That turret plays animation 4 (it charges) for **50 updates**, then fires: X gets an **aimed
   shot** (visual `var` 16, 17, 18), Zero a **missile** (visual `var` 32, 33, 34). Animation 1.
4. Shots per attack: **2** (Easy, Normal), **3** (Hard and above), **4** for Zero. The turret is
   picked again before each shot. About 51 updates per shot.
5. Sound 69; the head goes back up at 2 px/frame (60 updates). THINK.

### DRILL (step 7) [certain for the code; the safe place is a guess]

1. The head comes out at **machine x 48** (the left side): sound 69; down 120 px at 2 px/frame.
2. Turret 34 (bottom right) extends its drill (animation 2): at its flag 2 (update 27) sound 70;
   at flag 1 (update 31) it gets the attack box `0x800F8530` (−32, −10, 16, 20) and **contact 8**.
3. The head **moves right at 1.5 px/frame** from machine x 48 to 208 (107 updates), with the drill
   out and its block solid (it pushes X). At x 208: sound 72.
4. The drill goes in (animation 3, sound 71, 12 updates); no attack box, contact 0.
5. **90 updates**, sound 69, the head goes up (60 updates), THINK.

At the end the head block reaches room x 180–236 (machine x 208 ± 28) and, with the mirror rule of
`docs/engine.md`, the drill box room x 240–256 at room y 158–178, which covers a standing X. The
right wall is at room x about 288–324. The space between them is small; the wall's own attack box
(contact 8) is also on that side near the floor. So the drill is probably avoided on the wall
[guess: play check].

### BEAM (step 8) [certain]

1. The head comes out at machine x 160 (as TURRET SHOTS).
2. It picks a pod: **17 (right) when X is right of the head**; else **18 (bottom left) when X is
   below the head**; else **16 (top left)**.
3. The pod opens (animation 6): at flag 2 (update 27) sound 73; at flag 1 (update 51) it fires
   the **beam** (visual `var` 48). The beam stays for **180 updates**; then the pod closes
   (animation 7, 12 updates) and the beam ends.
4. Sound 69, the head goes up, THINK.

The pod is open for the beam, but it can be hurt (from its side) whenever the head is out. A
killed pod frees the head: no beam.

### END CRUSH (step 9) [certain for the code; not reached]

Entered when the layer 0 camera x is ≥ 5440 while the head lives. The walls go up at 4 px/frame,
the stage parts sit 16 px lower, and the machine moves **down 4 px/frame** to `cam1.y` 776
(machine origin at room y 176: the plates 16 px above the floor, so a standing X is crushed
[likely]); sound 68, `Rumble(5)`, then 60 updates of `Quake(6, 4–7, 2)`, and it holds. In this
room the camera is fixed at x 10, so this step is not reached in the fight [likely; open].

### Death (state 2) [certain]

1. **At once:** `game.checkpoint` = **2**; `game+0x26` = 1 (the stage 4 flag that the presses and
   the teleporter read, `docs/engine.md`); `PlayerEvent(20, <X's facing>)`: X is held. Palette
   flash off.
2. The parts explode (explosion and 4 debris each: pods animations 14, 14, 15, 15; turrets 12, 12,
   13, 13) and are deleted. The walls make 4 debris (14, 14, 15, 15) and an explosion every 5
   updates for 60 updates (`SpawnExplosionRand(…, 1, 16, 64)`), then end; the plates and the
   head block end at once. Shots end by their own rules.
3. **127 updates:** the head flashes, at gaps of 26, 21, 16, 11, then 6 updates. Then engine
   object 26 (stage-logic class, `var` 1) at the room middle (machine (160, −40): room (160, 40)).
4. While object 26 runs, the head flashes every 6 updates. **When object 26 sets its `+0x07`**
   (the white screen; read by the stage work), the head code, once:
   - puts the cameras at **(5456, 224)** for layers 0 and 1 and (2728, 224) for layer 2, with
     every limit and target at 5456 and 224, and asks a redraw of the three layers;
   - calls `SpawnInRect(5408, 5824, 224, 512, 0)`: the records of the area before the boss gate;
   - puts **X at (5604, 395)** (also its previous position).
5. When object 26 is gone: `PlayerEventEnd` (X can move), and the head deletes itself
   (`DeleteObject3`, so record 75 keeps its "spawned" bit and does not come back).

No drop, no Nightmare Soul, no exit request (`game+0x0F` is not written), no clear flag. `game+0x20`
(the boss object) is not cleared by this code [certain]. `shark-5_after-mini-boss` agrees:
checkpoint 2, `game+0x26` = 1, X at (5604, 395), cameras (5456, 224) / (5456, 224) / (2728, 224),
record 75 spawned, the boss gate `id` 21 live at x 5776 [certain].

## Animations (table `0x800F8638`, 17 animations) [certain for the frames]

The export draws all of them from bank 3 (sprite type 16, 44 sprites). Sprite 25 is the head
panel; the parts draw over it.

| # | sprites | what | used by |
|---|---|---|---|
| 0 | 25 | head panel | head |
| 1 | 26 | turret, closed | turrets |
| 2 | 27 28 29 28 27 26 30 30 31 32 (31 updates; flag 2 at 27, flag 1 at 31) | drill out | turret 34, DRILL |
| 3 | 31 30 26 (12; flag 1 at the end) | drill in | turret 34 |
| 4 | 26 27 28 29 28 27 (loops, 10) | turret charging | turrets, TURRET SHOTS |
| 5 | 33 | pod, closed | pods |
| 6 | 34 35 36 35 34 33 37 37 38 39 39 (51; flag 2 at 27, flag 1 at 51) | pod opens and fires | pods, BEAM |
| 7 | 38 37 33 (12; flag 1 at the end) | pod closes | pods |
| 8 | 0–3 (8 each, loops) | bomb | visual `var` 0 |
| 9 | 4–8 (loops) | missile | visual `var` 32–34 |
| 10 | 9–13 (2 each, loops) | energy ball | visual `var` 16–18 |
| 11 | 14–23 (1 each; flags 3, 5, …, 15; loops on the last 3) | beam, growing to full length | visual `var` 48 |
| 12, 13 | 40, 41 | turret debris | death |
| 14, 15 | 42, 43 | pod and wall debris | death |
| 16 | 24 | side wall | stage 10 (only the walls are drawn) |

## Shots (visual type 13, entry `0x800EB504`, states `0x800F885C`)

Each copies its parent's graphics ("copies the parent", `docs/engine.md`). Priority 32. HP 1,
`dmg_tbl_minor`; only the missile takes weapon hits. Except the beam, a shot ends more than 32 px
off screen; visible within 16 px.

| `var` | shot | box, damage | movement and end |
|---|---|---|---|
| 0 | bomb | attack (−3, −3, 6, 6), terrain (0, 0, 2, 2); contact **2** | Falls: start 2 px/frame down, gravity 0.094 px/frame² (`0x1800`), at most 4 px/frame. On the floor: sound 77, it bounces: 1 px/frame left, 1.33 px/frame up, gravity 0.044 (`0xB60`). On the floor again: sound 77, it **rolls left 1 px/frame for 60 updates**. Then it **explodes**: attack box (−16, −16, 32, 32), contact 2, for 30 updates (the bomb is not drawn; an explosion effect shows). It cannot be shot. Touching X ends it (explosion). |
| 16, 17, 18 | aimed shot (X) | attack (−4, −4, 8, 8), terrain (0, 0, 4, 4); contact **4** | Sound 74. Aimed at X when fired, **5 px/frame** straight. Ends on terrain (a hit spark when it moves down) or on X. It cannot be shot. |
| 32, 33, 34 | missile (Zero) | attack (−20, −3, 40, 6), hurt (−20, −4, 40, 8); contact **4**; HP 2 | Sound 75. Starts 44 px to the side of the turret; **1.5 px/frame** sideways (33 to the left, 32 and 34 to the right). For 60 updates, or until it passes Zero in x, it follows Zero's height at 1.5 px/frame (it stops within 2 px). Then straight on. Ends on Zero or when shot (explosion). |
| 48 | beam | attack box in the object: height 24 (oy −12), width = 16 × the animation flag (48 to **240 px** in 7 updates), the far end at a fixed place; contact **6** | Sound 76. Its x stays 164 px to the pod's facing side of the pod. With the engine mirror rule the beam starts 37 px from the pod and reaches 240 px out, at the pod's height [likely]. It does not end on X. It ends when the pod closes (after 180 updates): sound 20. |

## Damage [certain]

**HP.** The head has 48 HP and no hurt box. A hit on a pod takes the pod's lost HP from the head:
the code takes `old pod HP − new pod HP` as a signed byte, and the engine lets the pod HP go below
0. So **the head loses the full damage of every hit, also past the pod's 16 HP**. The head dies at
HP ≤ 0, also when one or two pods are still there.

**Pods** (`dmg_tbl_normal`, `docs/engine.md`, from their facing side only):

| ids | damage |
|---|---|
| 0 (lemon) | 3 |
| 9 (half charge) | 7 |
| 18, 19 (full charge; 19 = Falcon) / 20 | 15 / 16 |
| 22 | 32 |
| 24–31 (sabre) | 4 (25: 6) |
| 46, 60, 62 | 15 |
| 55 | 127 |
| 7 | no effect |
| 64–78 | deflected |

Hits to kill the boss (before the part bonuses and Easy × 1.25), shooting one pod at a time:

| weapon | per pod | to kill the boss |
|---|---|---|
| lemon (3) | 6 (18 HP to the head) | 16 |
| half charge (7) | 3 (21) | 7 |
| full charge (15) | 2 (30) | 4 (the third pod is not needed) |
| sabre (4) | 4 (16) | 12 |

There is **no stun and no special weakness**: no weapon id has its own code. The turrets
(`dmg_tbl_deflect`), the walls and the plates (`dmg_tbl_capsule`) cannot be hurt.

**Damage to X** (before the difficulty scaling): drill 8; side wall attack box 8; beam 6; aimed shot
and missile 4; bomb and its blast 2; the press plates 127 as a squeeze (instant death); the head
block and the walls 8 as a squeeze.

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 68 | the machine lands: intro drop, PRESS bottom, END CRUSH |
| 67 | end of the shaking (intro, PRESS) |
| 69 | the head starts to come out or to go up; PRESS start and end |
| 70, 71, 72 | drill out, drill in, end of the drill ram |
| 73 | pod charges the beam |
| 74 | aimed shot |
| 75 | missile (Zero) |
| 76 | beam |
| 20 | beam ends |
| 77 | a bomb bounces |
| bank 0, 14 | HP bar tick |

## Summary for a reimplementation

- A one-screen room under a press machine (BG layer 1). The machine drops 48 px in the intro, then
  fills a 48 HP bar (no boss level bonus). X is held for the intro.
- Every attack: 60 updates of wait, then one of: bombs (4, falling and rolling left, then a 32 px
  blast), turret shots (the head comes down 120 px; 2 aimed 5 px/frame shots, 3 on Hard), drill
  (the head comes down at the left and rams right 160 px at 1.5 px/frame with a drill), beam (a pod
  fires a 240 px beam for 180 updates on X's side). Every fourth attack (after the first bombs) is
  the PRESS: the whole machine comes down 72 px to 40 px above the floor for 60 updates.
- Only the three pods (16 × 16 px, from their facing side, only while the head is out) can be hurt;
  every hit takes its full damage from the 48 HP. Standard enemy damage table.
- Death: checkpoint 2 at once, X held, 127 updates of flashing, engine object 26 (the white
  screen); at its signal the code moves the camera to (5456, 224) and X to (5604, 395), before the
  boss gate; when the object ends X is free.

## Open questions

1. Who reaches step 9 (END CRUSH, camera x ≥ 5440 with the head live).
2. The mirror of the boxes of objects that face right (beam start, drill and wall boxes on the inner
   side): `docs/engine.md` gives it as likely.
3. The press gap: 40 px for a 38 px X (play check). Also whether a jump into the plates kills
   (X's centre in a plate gives `mega+0xE6` = 127).
4. The drill ram: where X can stand (play check).
5. (answered) Class 7 of `dmg_tbl_capsule` (walls, plates, head block): `ApplyWeaponHit` sets the
   shot's `+0x70` |= `0x0F` and returns 127, with no damage and no sound. So the player's shots stop
   on them as on a wall (`docs/enemies/nightmare-press.md`, "The four other solid helpers").
6. Engine object 26 and its `+0x07` (the white screen and the fade back; the stage work reads it).
7. (partly answered) `0x8003208C` pushes and carries the visual and weapon objects with `+0x9D` ≠ 0.
   The writers of `+0x9D` on a shot are the code of weapon id 2 (`0x801EEEF0`, X's overlay; slot 2)
   and of weapon id 79 (`0x800446A4`) [certain for the stores]; which visual objects set it was not
   searched.
