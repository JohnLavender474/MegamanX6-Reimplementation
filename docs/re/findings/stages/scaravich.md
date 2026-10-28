# Stage: Ground Scaravich (stage 5, overlay 08)

## Short answer: is the section order random?

**Yes, it is random, but in a fixed frame.** The stage has 8 sections. The four blue teleporter
totem poles send the player to 4 of them. The first two poles always go to two different sections
of the group 0–3. The last two poles always go to two different sections of the group 4–7. So in
one run, no section comes twice. The game makes two dice rolls only. The first roll is at the
first pole, and it picks the sections of poles 1 and 2. The second roll is at the third pole, and
it picks the sections of poles 3 and 4. The roll uses the game's one shared random number
generator. That generator does not use the clock, the frame counter, the buttons, the checkpoint,
the souls or the difficulty. It is a fixed number sequence that every random event in the game moves
forward: enemy actions, explosions, debris, item drops. So the result is fixed by the full history
of play since power-on, but a player cannot see it or control it in practice. After the first pole,
the second pole's section is known. A death does not make a new roll. The boss is after the fourth
pole, on the main path. The teleporter crystal in section 7 goes to the hidden area (stage 21).

The rest of this page gives the detail [certain unless marked].

## Save states and overlay

Overlay `08` at RAM `0x800E9860` [certain: the record lists and the object code of the states
are in it; `docs/engine.md`, "Stage overlays", said "stage 5 = 08" without proof]. Stage 5 has one
area: `enemy_set_tbl` and `fixed_set_tbl` have no list for area 1. Save states (in
`~/mmx-data/x6/dumps/`; Zero, form 5; difficulty 1; `nightmare_effect` 4, from Metal Shark
Player's pick):

| state | Zero at (x, y) | HP | camera (x, y) | `game+0x2F`, `+0x30`, `+0x31` | what |
|---|---|---|---|---|---|
| `scaravich-1_start` | 64, 154 | 32 | 0, 0 | 0, 0, 0 | the start; area 0, checkpoint 0. No main or stage object is live |
| `scaravich-2_totem-section` | 1744, 874 | 32 | 1600, 704 | 0, 6, 1 | just after the first teleporter pole: section 2. Area 0, checkpoint 0, exit request 0 |
| `scaravich-3_section` | 800, 394 | 24 | 640, 224 | 0, 6, 1 | the main path, floor A, at the return point of pole 1, after section 2. The pole base (stage object 3 `var` 0, HP 5) and its five heads (`var` 14–10, HP 16 each, at y 400, 368, 336, 304, 272) are live at (1136, 336): the pole 1 fight did not start. Four grey blocks of section 2 (stage object 12 `var` 1) are live. Checkpoint 0. Lives 4 (6 in `scaravich-2`) |
| `scaravich-4_low-path` | 80, 2522 | 16 | 0, 2352 | 1, 6, 1 | section 1, at its entry: pole 2 sent Zero to section 1, as row 6 says. Limits x 0–1600, y 2352. Checkpoint 0 |
| `scaravich-10_section-4-sabre` | 119, 2874 | 12 | 0, 2778 | 3, 3, 2 | section 4, near its entry: pole 4 sent Zero to section 4, as row 3 says. Checkpoint 1, `stage_texture_swap` 2. Zero is in step 48 (first sabre hit, animation 88) with a live sabre object (weapon id 24) |
| `scaravich-11_boss` | 1724, 1354 | 32 | 1600, 1183 | 4, 3, 2 | the boss room, checkpoint 2, limits x 1600–1664. The boss (main 50) is live, boss HP 40 |
| `scaravich-13_boss-death` | 1697, 1354 | 12 | 1600, 1183 | 4, 3, 2 | the boss room just after the boss death: boss state 2 step 2, `BossExplosion` live (see "The boss", death) |
| `scaravich-12_boss-fight` | 1890, 1256 | 28 | 1664, 1155 | 4, 3, 2 | the boss fight: Zero hangs on the right upper bar (step 28). The boss is in step 5 (pellet throw), HP 17, with a size-3 ball (visual 14 `var` 3, HP 48) |

`scaravich-3` and `scaravich-4` are two different places [certain]. The stage time (`game+0x90`)
goes up from state to state (235, 1401, 10908, 11965), so states 1–4 are probably one run [likely].
The states `scaravich-5` to `scaravich-9` are in stage 21, area 0 (the hidden area,
`docs/stages/scaravich-alt.md`), with `game+0x2F`, `+0x30`, `+0x31` = 2, 3, 2 [certain].

The map: DAT member 100, 16 × 16 screens (4096 × 4096 px), extracted with `learn` from
`scaravich-3_section`. The second texture block (entry `0x16`) is in video memory in
`scaravich-10` and `scaravich-11` (catalog entry "Ground Scaravich, second texture"). With it, floors
D and E, the boss corridor, the boss room and sections 4–7 draw correctly; with the first block,
floors A–C and sections 0–3 draw correctly; floor C is the same with both [certain, by eye: the
floor signs A-1…E-2]. So the swap of zone 2 on floor C is for everything after pole 2. The TMX map
uses block `0x16` in these regions (`texture_split`): layers 0 and 1, (640, 960)–(2048, 1440),
(0, 2720)–(1920, 3584), (1920, 2560)–(4096, 3584) and (2880, 0)–(4096, 1696).

## Objects of the routing

| object | class, type | `var` | records | what |
|---|---|---|---|---|
| teleporter pole | stage object 3 | 1–4 | 18–21, at (1136, 336), (784, 576), (784, 1056), (1200, 1296) | the blue pole; a touch sends the player to a section |
| pole base | stage object 3 | 0 | none: main 12 makes it | the pole to fight, at the place of the teleporter pole |
| pole heads | stage object 3 | 10–14 | none: the base makes them | five heads on the base, at y + 64, 32, 0, −32, −64 |
| section goal | main 12 | `id` 0–7 (not read) | 22–29, one in each section | destroy it to go back to the main path |
| teleporter crystal | main 28 | `id` 1 | 15, at (3064, 1488) in section 7 | exit `0x41`: the hidden area (below) |
| grey block | stage object 12 | 1, 2 | 180–204 | Nightmare Effect 4 only (below) |

The names "teleporter pole", "pole base", "section goal" are descriptive, not official.

## The sequence of one pole

1. **The blue pole.** Stage object 3, `var` 1–4, sprite type 130. The set-up deletes it when
   `game+0x2F` (poles cleared) ≥ `var`. Its trigger box is (−16, −80, 32, 160): the full height of
   the pole. It has no hurt box and does not take hits.
2. **The touch** (sub-step 0, `0x800EB298`): `BoxOverlap(pole, player)`. It fails while the player
   is invincible (`mega+0x61`). Then the same freeze as the teleporter crystal: sound 1/1, camera
   follow off, stop flags 2, the player's terrain box off, player step 2, charge and shots cleared
   (`docs/enemies/teleport-crystal.md`, "Start of a teleport").
3. **The split and the roll** (sub-step 1, `0x800EB398`): 29 frames of the two flickering images.
   Then the **section roll** (next part), `FadeOut(8)`, and the **random blocks** of the section.
4. **The arrival** (sub-step 2, `0x800EBEA0`, when the fade-out ends): the player, the three
   camera layers and the layer-0 limits come from the section table `0x800F5754` (32 bytes per
   section); Zero gets y − 1. `SpawnInRect` around the new view, `FadeIn(8)`, sound 1/1.
5. Sub-steps 3–5: the images come together, then 29 frames, `ToStand`, the stop flags clear, and the
   pole deletes itself with `DeleteObject3`.
6. **The section.** The player goes to the end of the section and destroys the **section goal**
   (main 12; HP 32, contact damage 4, normal damage table). After the kill: 60 frames of effects
   and debris, then the same teleport sequence. The destination is the return table `0x800F5910`
   entry `game+0x2F`: so the player goes back to the pole that they came from. At the end, the goal
   makes stage object 3 `var` 0 (the pole base) at `0x800F5990`[`game+0x2F`] (the place of that
   pole) and deletes itself.
7. **The fight.** The base (HP 5, solid while the player is within 48 px) makes five heads (`var`
   14 to 10, from the bottom to the top). Each head takes hits (HP 16, contact damage 4) and shoots
   (visual type 8). Each head death takes 1 HP from the base. When the base HP is 0: 120 frames of
   effects and debris, an item roll (`DropItem`), and `game+0x2F` += 1 (only in stage 5 and at y
   336, 576, 1056 or 1296). The head attacks were not read in detail.

| `game+0x2F` | return point (player x, y) | camera x, y | layer-0 limits x, y |
|---|---|---|---|
| 0 | 800, 395 | 640, 224 | 0–960, 0–1200 |
| 1 | 1120, 635 | 960, 464 | 640–960, 0–1200 |
| 2 | 1120, 1115 | 960, 944 | 640–960, 0–1200 |
| 3 | 800, 1355 | 640, 1184 | 640–960, 0–1200 |

So the main path (the "hub") is a column at x 640–1280 that goes down past the four poles. Because
`game+0x2F` counts the cleared poles and each pole needs `game+0x2F` < `var`, the poles are used in
the order 1, 2, 3, 4 [certain for the code; that the map forces this order is likely].

## What decides the section

Three bytes of the game work [certain]:

| field | address | meaning |
|---|---|---|
| `game+0x2F` | `0x800CCEFF` | poles cleared, 0–4 |
| `game+0x30` | `0x800CCF00` | the route row, 0–11 |
| `game+0x31` | `0x800CCF01` | roll phase: 0 no roll yet, 1 after the first roll, 2 after the second roll |

At the end of the split (step 3 above), before `FadeOut`:

```
if var == 1 and game+0x31 == 0:  game+0x31 = 1;  game+0x30 = GetRNG() % 12
if var == 3 and game+0x31 == 1:  game+0x31 = 2;  game+0x30 = GetRNG() % 12
section = route_tbl[game+0x30][var - 1]          (route_tbl = 0x800F5644)
```

The route table (section for pole `var` 1, 2, 3, 4):

| row | 1 | 2 | 3 | 4 | | row | 1 | 2 | 3 | 4 | | row | 1 | 2 | 3 | 4 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 0 | 1 | 4 | 5 | | 4 | 0 | 2 | 4 | 6 | | 8 | 0 | 3 | 4 | 7 |
| 1 | 1 | 0 | 5 | 4 | | 5 | 1 | 2 | 5 | 6 | | 9 | 1 | 3 | 5 | 7 |
| 2 | 2 | 0 | 6 | 4 | | 6 | 2 | 1 | 6 | 5 | | 10 | 2 | 3 | 6 | 7 |
| 3 | 3 | 0 | 7 | 4 | | 7 | 3 | 1 | 7 | 5 | | 11 | 3 | 2 | 7 | 6 |

- Columns 1 and 2 hold all 12 ordered pairs of two different sections of 0–3. Columns 3 and 4 are
  columns 1 and 2 + 4.
- Poles 3 and 4 use a **new** row (the second roll). So the second pair does not depend on the first
  pair.
- Result: pole 1 → one of 0–3, pole 2 → one of 0–3 but not that of pole 1, pole 3 → one of 4–7,
  pole 4 → one of 4–7 but not that of pole 3. No section repeats in one run.
- `GetRNG() % 12` on 0–255 gives rows 0–3 22/256 each and rows 4–11 21/256 each. So pole 1 and pole
  3 are exactly 1/4 for each section. Pole 2 (and pole 4) is 66/256 for section 0 (4), 64/256 for
  1 (5), 63/256 for 2 (6) and 3 (7).
- Not used: the frame counter, the buttons, the time, the checkpoint, the souls, the difficulty, the
  character, `game+0x2F`, the record `id`. The section goal's `id` is not read at all.
- The second roll needs `game+0x31` = 1. If pole 2 or pole 3 were touched before pole 1 (phase 0),
  no roll is made and row 0 is used (`game+0x30` = 0) [certain for the code; probably not
  possible on the map].

**The second roll seen.** In `scaravich-5`…`-9` (stage 21): `game+0x2F` = 2, `+0x30` = 3,
`+0x31` = 2 [certain]; in `scaravich-4` they were 1, 6, 1. So the roll at pole 3 changed the phase
from 1 to 2 and the row from 6 to 3. Row 3 is (3, 0, 7, 4): pole 3 → section 7 (the crystal), pole 4
→ section 4. The three bytes survive the move to stage 21 [certain].

### Restarts

`StageStartReset` (`0x8001E914`) does not clear `game+0x2F`…`+0x31`. The only clear found is game
mode 12 (`0x8001E81C`), which clears `game+0x26`…`+0x35` and sets stage 15 (the move out of the
stage) [certain for the code]. So after a death in the stage, the cleared poles stay cleared, and
the same row is used again: the same sections [likely: no other clear was found].

Between `scaravich-2` and `scaravich-3` Zero died two times (lives 6 → 4) at checkpoint 0, and
`game+0x2F`…`+0x31` stayed 0, 6, 1 [certain]. Section 2 was entered again (with a new block
pattern), and Zero came back to return point 0 [certain]. So after a death pole 1 gave section 2
again, with no new roll [likely].

### The random blocks

Each section has 3–11 places where a block of the map can appear. At each teleport into the section,
for each place: `GetRNG() & 1` = 0 → `CopyTiles(0, x, y, src)` copies a block from a store in the
map into layer 0 (`docs/engine.md`: `CopyTiles`). So the layout of a section changes at each visit.
The blocks are written into the screen data in RAM (base: scratchpad `+0x08`, `0x80190F10`).
They stay during play: the section 2 blocks of `scaravich-3` are still there in `scaravich-4`
[certain]. A restart after a death removes them [likely]: in `scaravich-2`, section 2 has blocks at
places 1, 2, 4; in `scaravich-3` (after two deaths) it has blocks at places 2–6, and place 1 has no
block.

| state | blocks (B block, . none; places in code order) |
|---|---|
| `scaravich-1` | none |
| `scaravich-2` | section 2 `BB.B..` |
| `scaravich-3` | section 2 `.BBBBB` |
| `scaravich-4` | section 1 `..BBBBBBB.`; section 2 `.BBBBB` |

| section | entry (player x, y) | camera (x, y) | layer-0 limits x, y | block places | section goal |
|---|---|---|---|---|---|
| 0 | 80, 1707 | 0, 1536 | 0–1568, 1536 | 7 | (1840, 1632) |
| 1 | 80, 2523 | 0, 2352 | 0–1600, 2352 | 10 | (1904, 2512) |
| 2 | 1744, 875 | 1600, 704 | 1600–1664, 0–720 | 6 | (1648, 64) |
| 3 | 2352, 155 | 2240, 16 | 2240–2304, 0–720 | 3 | (2288, 848) |
| 4 | 80, 2875 | 0, 2720 | 0–1280, 2720–2928 | 6 | (1584, 3344) |
| 5 | 2000, 3355 | 1920, 3184 | 1920–3520, 2560–3200 | 8 | (3824, 2624) |
| 6 | 2992, 635 | 2880, 464 | 2880–3200, 0–464 | 11 | (3408, 176) |
| 7 | 3680, 1291 | 3520, 1184 | 2880–3520, 960–1440 | 7 | (3824, 1536) |

The goal of each section is the main 12 record inside that region [likely, by place]. Sections 2
and 3 are vertical shafts (x limits only 64 px apart); the others are wide rooms. The camera zones
of the sections can change these limits later.

## The RNG

`GetRNG` (`0x8002D2D4`) has a 16-bit state at `0x80090E70`:

```
hi = ((3 * s) & 0xFFFF) >> 8;  lo = (s + hi) & 0xFF;  s = lo | (hi << 8);  return lo
```

`0x8002D30C` is the same step, but it returns 1 instead of 0. The state is set to 3383 once at boot
(`0x80012838`). Only calls move it, but one of them runs in each play frame: `PlayFrame`
(`0x8001EA28`) calls `GetRNG` once before `GameFrame` (`docs/engine.md`, "Main loop"). The
sequence repeats after 43 534 steps (3383 is on the cycle). More than 150 call places use it in the
main program and overlay 08 (enemies, explosions, debris, item drops, the boss death, these random
blocks). A demo recorder (`0x80021694`, `0x8002170C`) saves `frame_count`, the RNG state and the game
work, and then the buttons of each frame; `0x80021400` restores them. So the game is designed to
replay exactly from a known state and the same buttons [likely for the use; certain for the saves].

**Can the dumps predict the order?** [certain for the numbers]

- `scaravich-1_start`: RNG state `0x88FA`. The roll of pole 1 is the value of the RNG at the
  moment of the touch. That depends on how many RNG calls come first (each enemy action,
  explosion or debris piece). Between the two dumps the state moved 1176 steps, modulo the cycle of
  43 534. With one call per play frame and 44 993 frames, the real count is 1176 + n × 43 534
  (n ≥ 1; probably 44 710 or 88 244). The call numbers below are also modulo the cycle. So the
  next section is **not** predictable from this RAM alone. It is predictable with the exact button
  input (or a trace of the calls).
- `scaravich-2_totem-section`: `game+0x30` = 6 (row 6: 2, 1, 6, 5), phase 1. Pole 1 went to
  section 2, as the row says, and Zero is at the section 2 entry (1744, 875 − 1). **Pole 2 will go
  to section 1** (no new roll). Poles 3 and 4 are not known yet (the second roll).
- Check: in the second dump, the six block places of section 2 have a block at places 1, 2 and 4,
  and none at 3, 5 and 6. In the first dump, none has a block. Two positions of the RNG sequence
  between the dumps give a roll of 6 followed by these 6 coin bits (calls 35 and 921 of 1176). This
  agrees with the code, but a chance match is also possible (about 1.5 expected), so it is not a
  proof.

## The teleporter crystal and the hidden area

One teleporter crystal (main 28, `id` 1, record 15) is at (3064, 1488), inside section 7. It is the
same object as in the other stages (`docs/enemies/teleport-crystal.md`; entry `0x800EECFC` in
overlay 08). It sets `game.exit_req` = `0x41`. `StageExit` (`0x8001E32C`) sends `0x41` to
`0x80042F7C`, which sets stage = (stage − 1) / 2 + 19 and area = (stage − 1) & 1: stage 21, area 0
for stage 5. The same rule gives stage 21 area 1 for stage 6 and stage 22 area 0 for stage 7, as
seen in the Turtloid and Sheldon states [certain for the code]. There is no randomness in it.

So the crystal is on the route only when section 7 is picked by pole 3 or pole 4: rows 3, 7, 8,
9, 10, 11 of the second roll, 127/256 (about 50 %) [certain for the code].

The other exit requests in `StageExit` (`0x40` and the rest): with area 0 and the per-stage byte
`0x80071190`[stage] = 0, area += 1; else the stage ends. Stage 5 has no area 1, and no `0x40`
crystal (`id` 0) is in its records.

## The way to the boss

After pole 4 (`game+0x2F` = 4) the player goes right on floor E to boss gate 1 (`id` 19,
(1264, 1344)), through the corridor with checkpoint marker 2 (x 1424), and through boss gate 2
(`id` 20, (1584, 1344)) into the boss room. The boss is main 50, record 515 (see "The boss").
Checkpoint marker 1 (944, 816) is on floor C, between poles 2 and 3.

## The map

The extents below are the parts that the camera can show: x from min x to max x + 320, y from min
y to max y + 240. They come from the section table `0x800F5754`, the camera zones and the
checkpoint records [certain for the values]. The floors and the drops come from the collision map
[likely].

### The main path (the "hub")

The start is at the top left (checkpoint 0: player 64, 16; limits x 0–960, y 0–1200). Stairs go
down and right to floor A. The main path is a column at x 640–1280 with five floors. Each floor is a
corridor 160 px high. The way down is a 64 px gap at one end of each floor, on alternate sides:

| floor | floor y | corridor y | way down | objects |
|---|---|---|---|---|
| A | 416 | 256–415 | gap x 1184–1247 (right) | pole 1 (1136, 336); return point 0 (800, 395) |
| B | 656 | 496–655 | gap x 672–735 (left) | pole 2 (784, 576); return point 1 (1120, 635); item main 47 `id` 33 (752, 648) |
| C | 896 | 736–895 | gap x 1184–1247 (right) | checkpoint marker `id` 1 (944, 816) |
| D | 1136 | 976–1135 | gap x 672–735 (left) | pole 3 (784, 1056); return point 2 (1120, 1115); raised block x 864–991 (top y 1072 / 1104) |
| E | 1376 | 1216–1375 | corridor to the right (x ≥ 1280) | pole 4 (1200, 1296); return point 3 (800, 1355); spikes x 1056–1167; two platforms x 1088–1151 at y 1248, 1296; item main 47 `id` 33 (1232, 1368); boss gate 19 (1264, 1344) |

- A pole trigger box is 32 × 160 px (x − 16 … x + 15, y − 80 … y + 79). On each floor it is the full
  height of the corridor, and the pole is between the way in and the way down. So the player cannot
  pass a pole without touching it, and the poles come in the order 1, 2, 3, 4 [likely, from the map].
- The touch fails while the player is invincible (`mega+0x61`) [certain for the code]. So a player
  who blinks after a hit can maybe walk through a pole [guess, not tested].
- Each return point is on the far side of its pole from the way down: R0 and R3 are left of poles 1
  and 4, and R1 and R2 are right of poles 2 and 3. So the pole base is between the player and the way
  down [certain for the positions; likely that it blocks the way].

### The sections

| section | camera extents x | camera extents y | shape | entry (player) | section goal | block places |
|---|---|---|---|---|---|---|
| 0 | 0–1888 | 1536–1776 | corridor, left to right | 80, 1707 | 1840, 1632 | 7 |
| 1 | 0–1920 | 2352–2592 | corridor, left to right | 80, 2523 | 1904, 2512 | 10 |
| 2 | 1600–1984 | 0–960 | shaft, bottom to top | 1744, 875 | 1648, 64 | 6 |
| 3 | 2240–2624 | 0–960 | shaft, top to bottom | 2352, 155 | 2288, 848 | 3 |
| 4 | 0–1600 | 2720–3424 | slope down to the right; zones 3–6 move max y 2928 → 3104 → 3184 | 80, 2875 | 1584, 3344 | 6 |
| 5 | 1920–3840 | 2560–3440 | slope up to the right; zones 8–10, 12 move max y 3200 → 2976 → 2784 | 2000, 3355 | 3824, 2624 | 8 |
| 6 | 2880–3520 | 0–704 | room, bottom left to top right | 2992, 635 | 3408, 176 | 11 |
| 7 | 2880–3840 | 960–1680 | room; teleporter crystal at (3064, 1488) | 3680, 1291 | 3824, 1536 | 7 |

- The block stores (the sources of the random blocks) are outside the sections [certain for the
  source rectangles `0x800F56A4`…`0x800F574C`]: sections 0 and 1 (512–767, 1856–1983); sections 2
  and 3 (1984–2015, 32–127); sections 4 and 5 (1680–1903, 2816–3583); sections 6 and 7 (2880–3167,
  736–879).
- Section 4, place 6 copies (1040, 3280), 9 × 10 cells, onto itself, so it changes nothing. Section
  4, place 5 copies 9 × 21 cells from (1680, 3248) to (656, 3104) [certain].

### The boss corridor and the boss room

| part | x | y | set by |
|---|---|---|---|
| boss gate 1 (stage object 8, `id` 19) | 1264 (box 1248–1279) | 1344 (box 1312–1375) | record 16 |
| corridor | 1264–1584 (one screen) | 1104–1424 | gate 19: camera x 1264, y 1104–1184 |
| checkpoint 2 | marker x 1424 | 1328 | record 1 |
| boss gate 2 (`id` 20) | 1584 | 1344 | record 17 |
| boss room | 1584–2000 | 1136–1424 | gate 20: x 1584–1680, y 1136–1184; then the boss: x 1632, later 1600–1664 |

- The room walls are at x 1568–1599 and 1984–2015 [certain, `scaravich-10`]. The four small
  "platforms" are **flat bars** (collision 45; the player hangs on them, step 28): (1616–1663,
  1280), (1664–1711, 1216), (1872–1919, 1216) and (1920–1967, 1280) [certain: the collision map
  and Zero on the bar in `scaravich-12`].
- The boss set-up removes both walls: its two `CopyTiles` copy the empty strip (1600, 1136, 2 × 15
  cells) over x 1568–1599 and 1984–2015, y 1136–1375. The wall (56) and the gate 20 door cells (59)
  become 0; the copy also puts one bar cell (45) at (1584, 1280) and (2000, 1280). All these cells are
  outside the view (limits x 1600–1664) [certain: 60 cells differ between `scaravich-10` and
  `scaravich-11`, none between `scaravich-11` and `scaravich-12`]. Why: the boss and the ball leave
  the room through the walls [guess].
- The room uses the second texture block, which zone 2 loads at checkpoint 1 [certain: the room
  draws correctly from `scaravich-10`]. The boss sprites (main type 50, 7 animations) are in
  `sprites/enemies/ground-scaravich/`, from `scaravich-11_boss`.

## Camera zones [certain for the data]

`camera_zone_tbl` (`0x80079D70`) word 10 (stage 5, area 0) = `0x800796D0`: 13 entries (ids 0–12).
Each entry is used by exactly one record. `CameraZoneUpdate` (`0x8005211C`) writes entry `+0x08` →
`cam+0x26` (min x), `+0x0A` → `+0x24` (max x), `+0x0C` → `+0x2A` (min y), `+0x0E` → `+0x28` (max y),
and sets `cam+0x47` = 2.

| `id` | record (x, y) | trigger x | trigger y | min x | max x | min y | max y | place |
|---|---|---|---|---|---|---|---|---|
| 0 | 2 (824, 336) | 800–831 | 256–415 | 0 | keep | keep | keep | floor A, return point 0 |
| 1 | 3 (1056, 448) | 896–927 | 544–623 | 640 | keep | keep | keep | floor B |
| 2 | 4 (944, 816), `id` 130 | 944–975 | 736–895 | 768 | keep | 704 | 1200 | floor C, with checkpoint marker 1; texture swap, one use |
| 3 | 5 (672, 3072) | 656–687 | 2960–3119 | keep | keep | keep | 3104 | section 4 |
| 4 | 6 (560, 2960) | 544–575 | 2912–3071 | keep | keep | keep | 2928 | section 4 (on the way back) |
| 5 | 7 (1040, 3200) | 1040–1071 | 3152–3295 | keep | keep | keep | 3184 | section 4 |
| 6 | 8 (928, 3152) | 896–927 | 3088–3247 | keep | keep | keep | 3104 | section 4 (on the way back) |
| 7 | 9 (912, 1024) | 896–927 | 976–1071 | 640 | keep | keep | keep | floor D, on the raised block |
| 8 | 10 (2816, 2992) | 2816–2847 | 2944–3071 | keep | keep | keep | 3200 | section 5 (on the way back) |
| 9 | 11 (2848, 3152) | 2848–2943 | 3152–3183 | keep | keep | keep | 2976 | section 5 |
| 10 | 12 (3296, 2944) | 3296–3423 | 2944–2975 | keep | keep | keep | 2784 | section 5 |
| 11 | 13 (1200, 912) | 1184–1247 | 896–959 | 768 | 960 | 704 | 1200 | the gap from floor C to floor D |
| 12 | 14 (3264, 2800) | 3264–3295 | 2720–2863 | keep | keep | keep | 3200 | section 5 (on the way back) |

- The entries are at `0x800796D0` + 16 × `id`. The "way back" labels come from the order of the
  triggers along the path [likely].
- Zone 2 sets `stage_texture_swap` = 1 and deletes itself, at the same x as checkpoint marker 1, as
  in the intro [certain for the code]. The flag is 0 in states 1–4 [certain].
- Sections 0–3, 6 and 7 have no zone. Their limits come only from the section table.

## Checkpoints [certain]

Records 0 and 1 are `CheckpointMarker`s: `id` 1 at (944, 816) on floor C, `id` 2 at (1424, 1328)
in the corridor between the gates. `checkpoint_start_tbl` (`0x80073300`) word 10 = `0x800731A4`,
3 records:

| cp | record | player x, y | camera 0 | camera 1 | camera 2 | limits x | limits y | facing | `beam_thru` |
|---|---|---|---|---|---|---|---|---|---|
| 0 | `0x800724F8` | 64, 16 | 0, 0 | 0, 0 | 0, 0 | 0–960 | 0–1200 | right | 1 |
| 1 | `0x8007251C` | 848, 720 | 768, 704 | 768, 704 | 192, 176 | 768–960 | 704–1200 | right | 1 |
| 2 | `0x80072540` | 1424, 1232 | 1264, 1184 | 1264, 1184 | 316, 296 | 1264 | 1184 | right | 16 |

- All BG layer offsets are 0. The live limits of `scaravich-1` are the checkpoint 0 values.
- All four main-stage states have checkpoint 0, so the grey blocks with `var` 1 are not deleted.

## The boss (main 50) [certain for the code unless marked]

Main type 50, record 515 at (1776, 1328), the last record. Entry `0x800F1C60`, states `0x800F5F34`:
0 set-up `0x800EFF9C`, 1 main `0x800F0200` (steps `0x800F5F40`, 12 entries), 2 death
`0x800F1C9C`. It uses `BossLevel`, `BossMusicRequest`/`BossMusicWait`, `TextOpen` and
`PlayerEvent`, and it makes stage logic 24 (`BossWarning`) and 26 (`BossExplosion`). So it is Ground
Scaravich [likely].

- **Set-up:** contact damage 4, damage table `0x80074C9C`, HP 0. Stage 5: `CopyTiles(0, 1568,
  1136, 0x800F5F24)` and `CopyTiles(0, 1984, 1136, 0x800F5F24)`; the source is {1600, 1136, 2, 15},
  a strip of 32 × 240 px that removes the side walls (see "The boss corridor and the boss room").
  The boss starts at (2048, 1360) + `0x800F6AF8`/`0x800F6AFA` (0, 0 in stage 5). It makes visual
  14 `var` 1 with the boss as parent (`+0x7C`): the ball, size 1 [certain]. Then state 1, step 9.
- **Stage 12:** the same set-up uses the offsets (976, −928) and the source {2576, 208, 2, 15},
  copied to (2544, 208) and (2960, 208); the entry wait is 10 updates (100 in stage 5), and the
  camera min y is 208. So this is also the stage 12 rematch [certain for the code].
- **Entry** (step 9, sub-steps `0x800F5FD8`):
  0. `PlayerEvent(20, right)`.
  1. When exit request = 0 and player step = 20: `PlayerEvent(29, right)` at once, timer 100 (stage
     12: 10). Camera: `cam+0x48` saved in `+0x86`, then 1; `cam+0x47` = 2; target min x = max x =
     1632. It makes `BossWarning`.
  2. At the end of the timer `PlayerEvent(20, right)`. When the warning is gone: `game+0x24` = 1,
     `game+0x20` = the boss, boss HP max (`game+0x8C`) = 40, `BossLevel`, `speed_x` = −3 px/frame.
  3. `0x800F09B4`: walk in at 3 px/frame with the rolling sounds (2, 52), (2, 34). Below x 1888 (not
     on sprites 4, 5): `TextOpen(0, 255, game+0x2E)`, then `game+0x2E` = 1. So the text shows only
     the first time; after a retry `TextOpen` waits 60 updates with no text. Stage 12: no text.
     Animation 0.
  4. When the text is closed: `BossMusicRequest`.
  5. When the music starts: camera x 1600–1664.
  6. The HP bar fills by 1 per update to 40, with sound (0, 14) every 3 updates. Then
     `PlayerEventEnd` and step 2. `cam+0x48` is restored from `+0x86`.
- The live HP is the boss's `+0x5C`; `game+0x8C` is only the bar maximum [certain, `scaravich-12`].
- **Fight timer:** `0x800F6AF4`. The set-up clears it (`0x800F015C`). State 1 adds 1 after each
  update, but not in step 9 (the entry) (`0x800F05F8`). So it counts the fight from the end of the
  entry. 26 in `scaravich-11_boss`, 4190 in `scaravich-13_boss-death` [certain].
- **Death** (state 2, steps `0x800F5F70`) [certain for the code; the state agrees]:
  0. `0x800F1CD8`: `PlayerEvent(20, Zero's facing)`, `mega+0xCA` = 0, palette bit 15 off,
     animation 5, timer `+0x7C` = 127, flash counter `+0x7E` = `+0x61` = 25, step 1.
  1. `0x800F1D54`: each update the timer goes down by 1. The flash: when `+0x7E` goes below 0,
     palette bit 15 changes (with the bit set, `DrawObject4` uses the fixed CLUT `0x7840`, as for
     the hit flash), `+0x61` −= 5 and `+0x7E` = `+0x61`, at least 5.
     So the flash intervals are 26, 21, 16, 11, 6, then 6 each. When the timer is 0: step 2 and
     stage logic 26 (`BossExplosion`) at the boss position; `+0x8C` = that object.
  2. `0x800F063C`: `+0x03` = 0 in each update. While `BossExplosion` is live and its `+0x07` = 0,
     the flash goes on (every 6 updates). When `+0x07` = 1 (full white), the boss does nothing
     more. When `BossExplosion` is deleted: `+0x80`…`+0x94` = 0, `game+0x24` = 0, `game+0x20`
     (the boss) = 0. Then, in stage 12: exit request `0x80`, `game[0x2C + checkpoint]` = 1 and
     checkpoint += 8. In other stages: a main object type 15 (the Nightmare Virus soul), flag
     `0x41`, `var` `0x10` when the fight timer < 5401 (90 s), else `0x11`. Then `DeleteObject3`.
- **`scaravich-13_boss-death`** (Zero just won): Zero in step 20 at (1697, 1354), facing left,
  `game+0x1C` (lock) = 1. The boss is in state 2 step 2 at (1656, 1360), animation 5, HP 0,
  palette `0xF948` (bit 15 set), `+0x8C` = `0x8009B7B0`. That `BossExplosion` (stage logic 26,
  logic slot 0) is in state 2 (the 100-update explosion part), with `+0x14` = 79 [likely: its timer,
  so 21 updates into state 2]. One ray type 4 (flag `0x83`) at the boss position and 13 effects
  type 4 (explosions) are live. The fight timer 4190 < 5401, so the soul will have `var` `0x10`.
  The explosion is exported from this state as `sprites/enemies/explosion/` (effect type 4). The
  ray has no sprite data (see `README.md`).

### The fight [certain for the code]

The boss pushes a ball (visual 14) of 4 sizes. Each time it leaves the room it gets a new ball, and
the ball grows with the count `+0x87` of new balls and run-offs: Easy/Normal 0–3 → size 1, 4–6 →
2, 7–8 → 3, 9 and more → 4; Hard 0–2, 3–4, 5, 6+. The first time the HP is below 20, a size-3 ball
count goes back to 0 and a size-4 count to 4 (Hard 3).

| step | what |
|---|---|
| 2 | walk out with the ball (3.0, 2.5, 2.0, 1.5 px/frame for sizes 1–4) until x 2176 or 1424 (off screen) |
| 3 | delete the ball; after size × 10 frames a new ball; then step 4 (HP ≥ 20, or 9 in 16) or step 11 |
| 4 | walk in to x 1728 (from the left) or 1856 (from the right); then 5 in 16 step 5, 5 in 16 step 6 (90 frames), 6 in 16 walk on (step 2) |
| 5 | wait 40, then kick a pellet (visual 14 `var` 0, 4 damage): boss level 0–1 straight at 3 px/frame, 2–3 aimed; 1, 1, 2, 3 pellets for boss level 0–3, 50 frames apart; then step 2 |
| 6 | stand (animation 0), then step 2 |
| 7 | the ball is destroyed: run off at 5 px/frame (animation 4), count + 1, step 3 |
| 8 | a hit in step 2 or 4 (once per ball): run on for a time (32–128 px / speed), turn, wait 40 |
| 10 | stun (weapon ids 1, 10, 54 in steps 2, 4, 6): hop (4.0 up, gravity 0.125), 180 frames of flashing and no contact damage, then step 8 |
| 11 | HP < 20 only: walk in and kick the ball (animation 3) boss level + 1 times. The kick picks one of three rolls: at the player at 3 px/frame, a lob that lands and rolls, or bounces. After each kick a new ball falls from the top in front of the boss (random size) and is kicked again; then step 7 |

- Step 10 sub-steps 2–4 have no writer in the walked code [likely: not used].
- Hit flash: 30 frames of `dmg_tbl_deflect` after a hit, 2 frames after sabre ids 24 and 25.
  Damage table `0x80074C9C`: weapon ids 1 and 54 5, id 10 8, sabre ids 24 and 25 1, other sabre ids 2.
- The ball (visual 14, entry `0x800F32E8`): sizes 1–4 have HP 8, 16, 48, 127, contact damage 4, 6,
  8, 16 and boxes of 32, 48, 80, 112 px; table `0x800F6288`. It rolls only while the boss walks.
  A destroyed ball breaks (2 s of explosions, debris). Texture boss + `0x100`, sprites of bank 15
  (the second type-50 bank), animations `0x800F6234` (21). On Hard and up: a purple ball that
  deflects everything (animations 11–14), and a small gold ball that deflects shots and drops a ball
  on the player when hit (animation 10, `var` 6).
- Sprites of the ball: `sprites/enemies/scaravich-ball/` (made by the probe script
  [`~/mmx-data/x6/re/scaravich-boss/scripts/ball_export.py`](../../work/scaravich-boss/scripts/ball_export.py); `spriteexport` does not export it).
- In `scaravich-12_boss-fight` the ball (`var` 3, HP 48) is carried at boss + (48, −36); the
  half-HP reset has run (`+0x8A` = 1, `+0x87` = 0), so the next ball is size 1 [certain].
- **Start:** the record spawns when the camera x is 1408 or more, so during the gate 20 pass
  [likely]. The walked code does not read `game+0x2F`.

**The boss gates.** Both gates are the shared boss gate (`0x80050DFC`). Ids 19 and 20 have no open
condition [certain]. Gate 19 sets x 1264, y 1104–1184. Gate 20 sets x 1584–1680, y 1136–1184 and
the time stamp. The gates do not need `game+0x2F` = 4: pole 4 (x 1200) fills the corridor in front
of gate 19 (x 1264) while `game+0x2F` < 4 [likely].

## The grey blocks (stage object 12)

The grey blocks that the user sees in the sections are stage object 12 (state table `0x800F5B14`).
Its set-up deletes the block unless `nightmare_effect` = 4 (Metal Shark Player's stage was picked
last; `docs/engine.md`, "Nightmare Effect"). HP 16, contact damage 8, sprite type 142; solid for the
player and for shots (`SolidObject`, `SolidPushShots`); destroyed by weapons (60 frames of blinking,
then explosions). When its animation flag is 2, it falls (`speed_y` = −4 px/frame) and at the floor
it plays sound 2/82, `Rumble(5)` and a screen shake (`Quake2`). Then it waits 181 updates, rises
at 3 px/frame to its start point and falls again: it is a crusher (`docs/enemies/grey-block.md`;
only weapon ids 4, 13, 35 and 55 hurt it). The block deletes itself when
`game.checkpoint` ≥ `var`: the 9 blocks with `var` 1 are in sections 0–3, the 16 blocks with `var`
2 in sections 4–7 [certain for the code; the look is from the user]. In `scaravich-2`, stage object
12 `var` 1 is live in section 2.

## Records

List entry 5, area 0 of `enemy_set_tbl` (`0x800F3AD4`): 516 records (the end marker is at
`0x800F4AF4`; an earlier count of 401 was wrong). List of `fixed_set_tbl` (`0x800F4AFC`): 36
records [certain].

| class, type | records | what |
|---|---|---|
| stage logic 6 | 2 | checkpoint markers `id` 1, 2 |
| stage logic 0 | 13 | camera zones `id` 0–12 (`id` 130 = 2 with bit 7: texture swap) |
| main 28 | 1 | teleporter crystal `id` 1 |
| stage object 8 | 2 | boss gates `id` 19, 20 |
| stage object 3 | 4 | teleporter poles `var` 1–4 |
| main 12 | 8 | section goals `id` 0–7 (export `section-goal`: a golden statue) |
| main 15 | 135 (kind 0: 79, kind 1: 14, kind 2: 42) | Nightmare Virus (main program code, `docs/enemies/nightmare-virus.md`): `id` 0 (44 normal, 34 Hard only), 1 (26, 5), 2 (9, 2), 33 (8, difficulty 1 and up; the snatchers), 34 (6, 1) |
| stage object 4 | 15 | Rescue Reploids, global ids 64–78 |
| stage object 12 | 25 | grey blocks `var` 1 (9), 2 (16) |
| main 31 | 279 | falling rocks (`docs/enemies/falling-rock.md`) |
| stage object 6 | 27 | rock blocks (`docs/enemies/rock-block.md`): `id` 3 (23), 4 (2), 5 (1), 6 (1); in the sections |
| main 47 | 3 | items: `id` 4 (3600, 2704) in section 5; `id` 33 (752, 648) and (1232, 1368) on the main path |
| stage object 26 | 1 | `id` 0 (3200, 504), section 6; not read |
| main 50 | 1 | the boss (1776, 1328) |
| fixed list: stage logic 3 (`id` 0–4), 29 (19 records, `id` 0–4), 14 (`id` 129–139), main 37 (at 0, 0) | 36 | see "Stage logic" below |

## Stage logic and effects [certain for the code unless marked]

- **Palette cycles** (stage logic 3, `PaletteCycle`; the rules: `docs/stages/turtloid.md`, "Palette
  cycles"). Five records, `id` 0–4, all live in the four main-stage states. Scripts (table
  `0x800F375C`): `id` 0: colour sets 0, 1, 2, 1, 0, 3 (16 frames each), loop; 1: 4, 5, 6, 5 (12
  each); 2: 7, 8, 9, 8, 7, 10 (16 each); 3: 12, 13, 14, 15, 14, 13 (12 each); 4: 16, 17, 18, 19, 18,
  17 (12 each). What they light is not checked.
- **Shaft tile animations and the room dimmer** (stage logic 29, 19 records;
  `docs/enemies/shaft-tile-animator.md`). `var` 0–3 copy tile blocks in a loop on BG layer 1 of the
  shafts of sections 2 and 3. `var` 4 darkens palettes 147–178 in sections 0 and 1 for about 2 s in
  each cycle of about 9.5 s.
- **Talk zones** (stage logic 14, `id` 129–139): all optional calls. One is at the start of each
  section, one in the hub near pole 1 (976, 256) and one near the boss gates (1344, 1264) [likely,
  by place].
- **Rain** (main 37, overlay 08 entry `0x800EFD04`): it deletes itself because `nightmare_effect` is
  4, not 6. Its zone table (`0x800F5D98`) has zone 0 for stage 1, zone 1 for stage 19, zones 2–6 for
  sections 3, 7, 1, 4 and 5 of this stage, and zone 7 for stage 21.
- **The totem pole fight** (stage object 3 `var` 0 and 10–14): `docs/enemies/totem-pole.md`.
- **Nightmare Virus souls.** In `scaravich-3`, 10 soul bits are set (ordinals 31, 32, 34, 36, 38,
  39, 41, 43, 44, 45): exactly the normal and difficulty-1 records of section 2. The Hard-only
  records there have no bit. So the ordinal counts every type 15 record, also the Hard-only ones.

## Open questions

1. A state just before and after a pole touch, or a `GetRNG` trace, to confirm the roll call.
2. A state after a death in a section (the respawn and the reloaded map, seen directly).
3. After stage 21: does the player come back to section 7 or to the pole 3 return point?
4. The boss: weapon id 54 (a stun id); step 10 sub-steps 2–4 (no writer found); stage logic 27.
5. Can an invincible player walk through a pole (the touch fails during `mega+0x61`)? If so, pole 4
   can be skipped and gate 19 opens with `game+0x2F` < 4.
6. Stage object 26 and main 47 `id` 4; which object draws bank 7 (the list entry of type 31).

No fact on this page came from `~/IdeaProjects/mmx-reference/`.
