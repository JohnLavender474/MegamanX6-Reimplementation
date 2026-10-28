# Nightmare Virus (main program, main object type 15): behaviour sheet

The Nightmare Virus is a floating, ghost-like humanoid. It flies through walls. It waits, dashes at
the player in short straight lines, or stays in place and fires slow-aim orbs. When it dies it leaves
a **Nightmare Soul** that the player can collect. If nobody collects the soul, the virus comes back
to life. Some viruses (record `id` 32 and 33) fly to a waiting savable Reploid and possess it: the
Reploid becomes a hostile enemy and cannot be saved. The same object type also makes the large soul
that falls from the top of the screen after a boss fight.

The code is in the **main program** (`0x800404D8`–`0x800434BC`), so it is the same in every stage
(Commander Yammark, Rainy Turtloid). Save state `yammark-1_start.state.gz` (stage 1, area 0). Kit
command: `python3 -m mmxdemake.rekit --name nightmare-virus`. Written with `docs/re-guide.md`.
Units, confidence tags and the shared engine rules: `docs/engine.md`. Records: `docs/stages/yammark.md`.

## Spawn and set-up [certain]

- **HP by difficulty:** Easy **2**, Normal **10**, Hard and up **18**. Contact damage **4**.
- Boxes:
  - contact box `0x80075EDC` = (−12, −12, 24, 24): 24 × 24 px around the origin;
  - hurt box `0x80075EE4` = (−10, −10, 20, 20): 20 × 20 px around the origin;
  - **no terrain box**: no `CollideCheck`, no gravity. It moves through walls and floors.
- Damage table `dmg_tbl_normal`. Draw priority 3. It faces the player (standard rule; equal x → left).
- Visible within 48 px; off screen 64 → state 3 (despawn), except while the "keep" flag `+0x8D` is
  set (during a snatch, and for the soul kinds `0x10` and `0x30`).
- It finds its own position in the area's record list (`nv_record_index`, `0x80042C48`): the ordinal
  of its record among the records of type 15. This number (`+0x88`) is the index of its "soul taken"
  bit (see Death).

## The `id` parameter [certain]

The high nibble of `id` (`var`) picks the kind; the low nibble picks the step pattern.

| `id` | kind | start |
|---|---|---|
| `0x0n` (Yammark: 1; Sheldon: 0) | normal virus | the pattern of row n |
| `0x10`, `0x11` | boss reward soul: 200 (`0x10`) or 100 (`0x11`) souls | BOSS SOUL step |
| `0x2n` (Yammark: 32, 33; Sheldon: 33, 34) | **snatcher**: looks for a Reploid first, then the pattern of row n | SNATCH step |
| `0x30`–`0x32` | a soul only (no virus): `0x31` = 4 souls, `0x32` = 200 souls | SOUL step |

Pattern rows (table `0x80075EBC`); the steps repeat in a loop:

| row | steps |
|---|---|
| 0 | IDLE, SHOOT |
| 1 | IDLE, DASH |
| 2 | IDLE, DASH, SHOOT, DASH (every revived virus uses this row) |
| 3 | IDLE, IDLE (no user seen) |

So in Yammark's stage: `id` 1 = idles and dashes; `id` 32 = a snatcher that idles and shoots when
there is no Reploid; `id` 33 = a snatcher that idles and dashes. `id` `0x10`/`0x11` are made by the
boss code, and `0x31` by a possessed Reploid; no stage record uses them.

In Shield Sheldon's stage (stage 7, area 0) the type-15 records are: `id` 34 (a snatcher with row 2)
at (1768, 352), (2248, 400) and (3624, 880); `id` 33 at (4576, 928); `id` 0 at (4736, 864) [certain].
In the save states the first snatcher took Rescue Reploid 97; see `docs/enemies/rescue-reploid.md`,
"Stage 7". The area also has 7 records of type 15 with kind 4 (stage-object class, x 4024–4472);
they are a different object (stage-object type 15), not read.

In Blaze Heatnix's stage (stage 3, area 0; `docs/stages/heatnix.md`) the records use `id` 0, 1,
32 and 33; the soul row is 2. In `heatnix-4_cave` (Normal, checkpoint 1, `nightmare_effect` 4) no
virus is live, but the soul bits of row 2 are `0x2D`: the four Normal records of the first tower
above the player (60, 62, 63, 65) were made and killed; the Hard records 61 and 64 were not made.
Zero's souls are 32 more than 3632 frames before (4 × 8) [certain for the values].

## State machine

Standard states (table `0x80075FD8`): 0 `0x800404D8`, 1 `0x80040800`, 2 `0x80041BA8`,
3 `0x80041BC8`. Steps (table `0x80076018`): 0 engine hit reaction, 1 empty, 2 IDLE, 3 DASH,
4 SHOOT, 5 SOUL, 6 SNATCH, 7 BOSS SOUL.

### Main (state 1) [certain]

1. Save the position; `AnimeTick`; visible 48 (not in BOSS SOUL).
2. Run the step.
3. In SOUL: the pickup test (see SOUL). In BOSS SOUL: nothing.
4. Else: weapon test (death below), then the contact test.
5. Off screen 64 and `+0x8D` = 0 → state 3.

When a step ends, `nv_next_step` (`0x800429D4`) picks the next one. For a snatcher it first searches
for a Reploid; if it finds one it goes straight to the SNATCH approach. Else it takes the next entry of
its pattern row.

### IDLE (step 2) [certain]

Face the player, animation 0, wait 60 updates, then wait for the end of animation 0 (flag 1 on its
last frame; the loop is 48 updates). So it floats in place for **61–109 updates**.

### DASH (step 3) [certain]

- Face the player, animation 1. The number of dashes is `rand % 7 + 6` (**6–12**).
- Each dash: aim at the player (`ratan2`), speed **1 px/frame**, then a random factor: ×1, ×1.5 or
  ×2 (one third each). The dash lasts `rand % 13 + 4` updates (**4–16**). Then the next dash, aimed
  again.
- After the last dash: on **Hard and up**, a 50 % chance of the WARP strike; else the next step.

**WARP (Hard and up only):**

| sub-step | time | what |
|---|---|---|
| hide | 60 updates | invisible; no contact, no hurt box |
| place | 1 update | if it is more than 64 px off screen → despawn. Else it notes the player's position and moves to **64 px from it, in a random direction** (256 directions); 2 after-images start |
| images | 8 updates | invisible; two after-images slide in from 32 px left and right at 4 px/frame |
| strike | about 11 updates | visible, boxes on, animation 1; flies at the noted point at **6 px/frame** until within 7 px |
| coast | 8 updates | keeps the speed, so it passes the point by about 48 px |

### SHOOT (step 4) [certain]

- Only while the virus is inside the camera view with a 16 px inset (x from camera + 16 to + 304,
  y from camera + 16 to + 224). When it is outside, it goes to IDLE at once.
- Face the player, animation 4 (an arm sweep, then a held pose).

| difficulty | shots | delay before each shot |
|---|---|---|
| Easy | 3 | 90 updates |
| Normal | 4 | 60 updates |
| Hard and up | 5 | 30 updates |

- Each shot: face the player, sound bank 2 id 66, one orb. After the last shot, wait 40 updates.

### SNATCH (step 6, snatchers only) [certain for the code]

1. Face the player, animation 0, wait 60 updates and the end of the animation.
2. **Search** (`0x80042D98`): it looks through the 80 **stage objects** (`0x800C1220`, `0xA4` each)
   for **type 4** objects with flags ≠ 0, `display` ≠ 0 (on screen) and `+0x07` = 1 (waiting to be
   saved). It takes the nearest one closer than 1000 px. Stage-object type 4 is the savable Reploid
   (it draws with sprite type 141). No Reploid → the pattern step.
3. **Approach:** animation 1, it flies at **0.5 px/frame** to the point where the Reploid is 28 px
   below its origin. It checks the target each update; when the target is no longer waiting or visible
   (saved by the player, for example), it goes to the pattern step. It can be shot while it flies.
4. **Grab** (within 2 px): animation 2; it sets its "soul taken" bit; it writes **2 to the Reploid's
   `+0x07`**; the hurt box is removed (it cannot be shot now). At the animation flag (16 updates)
   sound bank 2 id 63 and a grab overlay (animation 3) appear.
5. 60 updates, sound bank 2 id 64, then it flashes for 90 updates and is deleted (state 2). It
   leaves **no soul**.

**What happens to the Reploid** (stage object type 4, code `0x8004E800`–`0x800500C0`, main
program) [certain for the code]:

- Its status in `game+0xD8` (4 bits per Reploid `id`) becomes **4 (lost)**. A Reploid with status 2,
  3 or 4 does not come back.
- It holds a pose for 130 updates, plays sound bank 2 id 65, flashes for 60 updates, and then
  becomes an **enemy**: HP 13, contact damage 6, a contact box and a hurt box, and it fires shots
  (visual type 24, sound bank 2 id 46) at the player in a loop (60 or 20 updates apart).
- When the player kills it: an explosion, status 3 (killed), and a soul of **4** (`id` `0x31`) at its
  position. That soul does not revive.
- When it goes 64 px off screen: status 4, gone.

### SOUL (step 5) [certain]

Entered when a weapon kills the virus and its "soul taken" bit is clear.

| phase | time | what |
|---|---|---|
| hop | about 32 updates | starts at 8 px/frame up, gravity 0.5 px/frame²; lands on its start height |
| bob 1 | 60 updates | animation 6, **worth 8**; bobs ±8 px (a 128-update sine) |
| bob 2 | 180 updates | animation 7, **worth 4** |
| revive | 8 + 30 updates | invisible; two after-images slide in; then animation 0 and a 30-update blink |

- **Pickup:** while the soul shows, a 10 × 10 px box (`0x80075EE0`, terrain-box format) is tested
  against the player's terrain box. On overlap: the soul count of the current character
  (`game+0xD2 + 2 × mega+0x02`, 16 bits) gets the value, sound bank 0 id 41, and the object is deleted.
- The soul does not hurt the player and cannot be shot.
- **Revival:** after bob 2 (about 272 updates after the kill) a new virus starts in place: full HP,
  contact 4, boxes on, `id` low nibble = 2 (pattern IDLE, DASH, SHOOT, DASH). Its "soul taken" bit
  stays set, so **a revived virus leaves no second soul**.
- Kind `0x30` souls (`id` `0x31`, `0x32`) skip bob 1 and blink out for 30 updates instead of
  reviving.
- A soul that goes 64 px off screen despawns (state 3) [certain for the code].

### BOSS SOUL (step 7, `id` `0x10` and `0x11`) [certain for the code; the spawner likely]

- It starts 32 px above the top of the view, at the player's x, and falls straight down at
  **1 px/frame**. Sound bank 0 id 43 when it comes into view.
- Every 40 updates, while it is more than 48 px above the player, it makes 3 light rays (below).
- When its y reaches the player's y: the soul count gets **200** (`0x10`) or **100** (`0x11`), sound
  bank 0 id 41, `game+0x0F` = 16, and it hides. The test uses only y, not x.
- The boss overlays make it (overlay 03 at `0x800F1984`): `id` `0x10` when the timer at `0x800F7DE0`
  is below 5401 updates (90 s), else `0x11` [likely: the timer is the boss-fight time].

## Animations (table `0x80075FB0`)

| # | what | frames |
|---|---|---|
| 0 | idle float | sprites 0–3, 12 updates each (11 + 1 on the last), flag 1 on the last; loop |
| 1 | fly | sprites 4–6, 8 each; loop |
| 2 | grab (snatch) | 0, 1 (8 each), 7 (4, flag 1), 8–11 (4 each); loops from frame 2 |
| 3 | grab overlay (misc 11) | sprite 12, hold |
| 4 | shoot | 0, 1, 13 (8 each), 14–18 (4 each); loops from frame 3 |
| 5 | orb | sprites 19–28, 2 each (two colours alternate); loop |
| 6 | soul worth 8 | sprites 32 33 34 33, 6 each; loop |
| 7 | soul worth 4 | sprites 29 30 31 30, 6 each; loop |
| 8 | soul worth 200 | sprites 35–38, 6 each; loop |
| 9 | soul worth 100 | sprites 39–42, 6 each; loop |

The export `nightmare-virus-orb` is **not** this object's orb: it uses table `0x8007789C`, which the
virus code does not use (14 engine functions load it). The orb is animation 5 above [certain for the
table; likely that the export is a false match].

## Shots and effects

### Orb (visual object type 12, `0x80041AF4`) [certain]

- Made by SHOOT. Copies the parent. Starts 4 px in front of the virus and 11 px below its origin.
- Aimed at the player when it is made; straight line at **5 px/frame**.
- Contact damage **4**; contact box `0x800760C4` = (−3, −3, 6, 6). **No hurt box**: weapons cannot
  stop it. Animation 5, priority 3.
- Ends: more than 32 px off screen. It does not stop at walls and does not end when it hits the
  player. Visible within 16 px.

### After-images and grab overlay (misc object type 11, `0x80041B30`) [certain]

- `var` 0 / 1: a copy of the virus (animation 0) that starts 32 px to the left / right and slides to
  the virus at 4 px/frame (8 updates), drawn on every second update. On arrival it takes 1 from the
  parent's counter `+0x07`. The virus waits until both have arrived (WARP and revival).
- `var` `0x10`: the grab overlay (animation 3, priority 1). It follows the virus while the parent's
  `+0x07` ≠ 0.
- Both end when the parent leaves state 1. No damage.

### Light rays (object type 13 of the class at `0x8009C9B0`, entry `0x80041B6C`) [likely]

- Made 3 at a time by the boss soul, at angles {a, a + 135°, a + 225°}, a = a random multiple of 45°.
- Each draws a polygon from the soul to the edge of the view (16 px steps), rotates by 2.9°–5.8° per
  update through 116°–173°, then shrinks and ends. It also ends when the soul reaches the player.
- Drawing only: the code has no contact test.

### The ray class (`0x8009C9B0`) [certain for the code]

The light rays are objects of a class of their own: 64 slots of `0x60` bytes at `0x8009C9B0`, made by
`GetRayObject` (`0x8002C7F0`) or by a record of kind 6 (`kind_table` entry 6).

- **Update.** `RayLoop` (`0x800207D4`, in the frame after the misc objects) runs
  `ray_table[type]` (scratchpad `+0x98`) for each slot with flags ≠ 0. While `game+0x18` (the stop
  flag of this class) is not 0, only objects with flag bit `0x08` run.
- **Table** (the same in Yammark and Turtloid, except entry 5): 0 `0x80054AA0`, 1 `0x80054AA8`,
  2 `0x8004B984`, 3 `0x8004BFE0`, 4 `0x8004C2F4` (the rays of the boss explosion, stage logic 26),
  5 the overlay's darkness effect (`docs/enemies/darkness-effect.md`), 6 `0x8004C6AC`,
  7 `0x8004CF68`, 8 `0x8004C708`, 9–10 and 12 overlay code, 11 `0x80054AB8`, 13 `0x80041B6C`
  (these light rays), 14 `0x8004D29C`, 15 `0x8004D77C`. Entries 0, 1 and 11 are stubs [likely].
- **Draw.** The draw pass (`0x80022DC0`) calls `0x800241D4` for a displayed ray with flag bit
  `0x02`, else `RayDrawQuad` (`0x80023ED8`). `RayDrawQuad` makes one flat quad (GPU code `0x28`;
  `0x2A`, semi-transparent, with flag bit `0x10`) from four corners at `+0x16`/`+0x1A`,
  `+0x1E`/`+0x22`, `+0x2E`/`+0x32`, `+0x26`/`+0x2A` (offsets from the object's x, y; screen
  coordinates when `+0x37` < 0, else relative to the camera of layer `+0x37`). The colour is the
  15-bit value `+0x34`, the blend mode flags bits 5–7 (`GetTPage`), the draw layer `+0x36` (255
  takes another path, not read). At most 1000 primitives per frame (scratchpad `+0x124`).

## Damage

- Taken: `dmg_tbl_normal`. Hits to kill:

| difficulty (HP) | lemon (3) | half charge (7) | full charge (15) | sabre (4) |
|---|---|---|---|---|
| Easy (2; damage × 1.25) | 1 | 1 | 1 | 1 |
| Normal (10) | 4 | 2 | 1 | 3 |
| Hard and up (18) | 6 | 3 | 2 | 5 |

- Dealt: contact 4, orb 4 (before the player's damage scaling).

## Death [certain]

- `SpawnExplosion` (one explosion), then:
  - "soul taken" bit clear → set the bit; the object becomes the SOUL (no new object);
  - bit set (a revived virus, or a respawned record whose virus was killed or snatched before) →
    deleted at once, no soul.
- No `DropItem`: the virus never drops an item. The Nightmare Soul is the virus object itself, not
  an item (main type 47). The soul is a counter per character at `game+0xD2`, not an item kind.
- The "soul taken" bits: `game+0x178 + row × 32`, row = byte `0x80075FE8[stage × 2 + area]`, bit =
  `+0x88`. In `yammark-2_boss` the bits of the first 5 viruses are set (`0x1F`). In the later
  `turtloid-1` the Yammark row is 0 again, so the bits are probably cleared when a stage starts
  [likely]. Stage 3 uses row 2 (`0x2D` in `heatnix-4_cave`).
- A killed virus is deleted with `DeleteObject` (state 2), an off-screen one with `DeleteObject2`
  (state 3). So its record keeps kind high digit `0x10` after a kill and comes back only on the
  third entry into the spawn band (`docs/engine.md`, "Spawning and despawning") [certain for the
  code; seen in `heatnix-4_cave`: records 60, 62, 63 `0x20`, record 65 `0x10` in the view, not live].

## Sounds

| bank, id | when |
|---|---|
| 2, 66 | orb fired |
| 2, 63 | grab |
| 2, 64 | end of the grab hold |
| 0, 41 | soul collected (also the boss soul) |
| 0, 43 | boss soul comes into view |
| 2, 65 / 2, 46 | (Reploid code) possessed pose / possessed Reploid shot |

## Nightmare Effect

The virus code reads no global flag that could be the Nightmare Effect. It reads only `game.stage`,
`game.area`, `game.difficulty`, the "soul taken" bits, the soul counters, the frame counter
(`0x80097420`), the camera and the player [certain for the code]. So the virus acts the same with and
without the Nightmare Effect. The spawner (`SpawnInRect`, `RecordSpawnFilter`) does not read
`nightmare_effect` either; in the main program the only access found is the store of
`StageStartReset` [certain for the direct addresses]. So the virus records spawn for every effect
value [likely]; in stage 3 they spawn with value 4 [certain: `heatnix-4_cave`].

## Summary for a reimplementation

- Floating enemy without terrain collision; HP 2 / 10 / 18 (Easy / Normal / Hard); contact 4; 24 × 24
  contact box, 20 × 20 hurt box.
- Loop of steps from its `id` row: IDLE (about 1–1.8 s), DASH (6–12 aimed dashes of 4–16 frames at
  1, 1.5 or 2 px/frame), SHOOT (3/4/5 aimed orbs, 5 px/frame, 4 damage, 90/60/30 frames apart, only
  while on screen).
- Hard: after dashes, 50 %: vanish 1 s, reappear 64 px from the player, strike at 6 px/frame.
- On death: a soul hops out; touch it for 8 souls (first second) or 4 (next 3 s); else the virus
  revives with the row IDLE, DASH, SHOOT, DASH, and gives no more souls.
- `id` 32/33: fly at 0.5 px/frame to the nearest waiting on-screen Reploid; when it reaches it, both
  the virus and the rescue are lost and the Reploid becomes an enemy (HP 13, contact 6, shoots).
- Each virus gives at most one soul per stage (a persistent bit per record).

## Open questions

1. `mega+0xD5`: when it is not 0, the soul pickup tests another body (`0x800CD340`, or `0x800CD3F0`).
   What is that state?
2. `game+0x0F` = 16 after the boss soul is the exit request "player step 25" (`docs/engine.md`); what
   step 25 does was not read.
3. Answered: the spawner reads only the record (flags, kind), not the "soul taken" bits. A killed
   virus record waits for its third entry into the spawn band (kind high digit).
4. The Nightmare Effect: it does not change the virus records (see "Nightmare Effect"); which
   objects it changes is per stage (`docs/engine.md`).
5. The draw codes 26–28 of the light rays (`+0x34`). `RayDrawQuad` reads `+0x34` as a 15-bit
   colour; the flag bit `0x02` path (`0x800241D4`) was not read.
6. Not checked in play: the sweep of the rays, the revival timing, the possessed Reploid's shots.
