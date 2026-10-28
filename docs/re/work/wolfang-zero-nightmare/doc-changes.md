# Doc changes from `wolfang-2_ice-slope` (the purple Zero)

No new sheet is needed: the purple Zero is the Nightmare shadow (main type 26, `var` 10). The changes
below correct and complete `docs/enemies/nightmare-shadow.md` and `docs/engine.md`. Evidence:
`notes.md` in this folder.

## `docs/enemies/nightmare-shadow.md`

### Title and first paragraph

Replace:

```
# Nightmare shadow (stage 09, main object type 26): behaviour sheet

Nightmare shadows are flat silhouettes of the player character: X shadows when the player is X, Zero
shadows when the player is Zero. They appear only under one Nightmare Effect. An invisible spawner
```

with:

```
# Nightmare shadow (stages 02 and 09, main object type 26): behaviour sheet

Nightmare shadows are flat silhouettes of the other character: magenta **Zero** shadows when the
player is X, cyan shadows (X [guess]) when the player is Zero. They appear only under Nightmare
Effect 7, in Blizzard Wolfang's stage (stage 2) and Rainy Turtloid's stage (stage 6). An invisible spawner
```

### Second paragraph

Replace:

```
Overlay `09` (Rainy Turtloid), code at `0x800EE9E4`–`0x800EF284`, save state `turtloid-1.state.gz`
(stage 6, area 0).
```

with:

```
Overlay `09` (Rainy Turtloid), code at `0x800EE9E4`–`0x800EF284`, save state `turtloid-1.state.gz`
(stage 6, area 0). Overlay `04` (Blizzard Wolfang) has a copy with the same code and data at other
addresses: set-up `0x800EF738`, entry `0x800EFD58`, states `0x800F6DC8`, steps `0x800F6DD8`, appear
sub-steps `0x800F6DE8`, boxes `0x800F6D90`, animations `0x800F6DB8`. A live shadow (`var` 10, the
dash) is in `wolfang-2_ice-slope.state.gz`.
```

### "Spawn condition"

Replace:

```
  deletes itself at once (`DeleteObject3`). The byte is 0 in all the save states. The rain effect of
  Yammark's stage tests the same byte for 6, so it is very probably the stage's Nightmare Effect
  [likely]. Which defeated boss sets 7 is not read.
```

with:

```
  deletes itself at once (`DeleteObject3`). The byte is `nightmare_effect` (see "Nightmare Effect"
  in `docs/engine.md`). The value 7 comes from the pick of stage 7 (Shield Sheldon): its targets
  `2 6 6` give stages 2 and 6 the value 7 [certain for the rule and the bytes]. It is 7 in the three
  `wolfang-*` states and 0 in the `turtloid-*` states.
```

Replace the `var` table:

```
| `var` | role |
|---|---|
| 0–5 | spawner (invisible), one zone each (below) |
| 6–9, 12+ | nothing: the set-up does nothing and the object stays in state 0 [certain for the code] |
| 10 | X shadow (made by a spawner when the player is X) |
| 11 | Zero shadow (made when the player is Zero) |
```

with:

```
| `var` | role |
|---|---|
| 0–5 | spawner (invisible), one zone each (below) |
| 6–9, 12+ | nothing: the set-up does nothing and the object stays in state 0 [certain for the code] |
| 10 | shadow made when the player is X: a magenta **Zero** silhouette (animations 0, 1) |
| 11 | shadow made when the player is Zero: a cyan silhouette, X [guess] (animations 2, 3) |
```

Replace:

```
- No record of main type 26 is in the stage list of the save state (list entry 6; it has one record
  of kind 4, stage-object type 26, which is another object). What places the spawners is not known.
```

with:

```
- The spawners are records of the **fixed record list** (`fixed_set_tbl`, `0x80073554`): kind 0,
  type 26, x = y = 0, `id` = `var` [certain]. Stage 2 area 0 (list `0x800F6124`): records 11 and 12,
  `var` 0 and 1. Stage 6 area 0 (`0x800F5378`): records 10 and 11, `var` 2 and 3. Stage 21 area 1
  (`0x800EF1DC`, `turtloid-4_alt`): record 2, `var` 4. No dumped list has `var` 5. The normal stage
  list has no main record of type 26.
```

### "Spawner zones"

Replace:

```
| `var` | zone |
|---|---|
| 0 | 321 ≤ x ≤ 2559 |
| 1 | 3201 ≤ x ≤ 4159 |
| 2 | 1249 ≤ x ≤ 1631 |
| 3 | 4961 ≤ x ≤ 7311 |
| 4 | 1 ≤ x ≤ 1055 |
| 5 | 321 ≤ x ≤ 2143 and y < 1072 |
```

with:

```
| `var` | zone | stage of the record |
|---|---|---|
| 0 | 321 ≤ x ≤ 2559 | 2 (Blizzard Wolfang) |
| 1 | 3201 ≤ x ≤ 4159 | 2 |
| 2 | 1249 ≤ x ≤ 1631 | 6 (Rainy Turtloid) |
| 3 | 4961 ≤ x ≤ 7311 | 6 |
| 4 | 1 ≤ x ≤ 1055 | 21, area 1 |
| 5 | 321 ≤ x ≤ 2143 and y < 1072 | not found |
```

Add after the paragraph "The same tests are in the set-up ...":

```
Each overlay has the full table of six zones; each stage uses only the `var` values of its own
records.
```

### "Animations"

Replace:

```
| # | what | frames |
|---|---|---|
| 0 | X shadow, standing | sprite 1, hold |
| 1 | X shadow, dash (body horizontal) | sprite 0, hold |
| 2 | Zero shadow, standing (long hair) | sprite 3, hold |
| 3 | Zero shadow, dash | sprite 2, hold |

`SetAnime` is called with 0, 1, 2 and 3. No event flag is tested. The export shows animations 0–1 in
magenta and 2–3 in cyan, but the code gives all shadows the same palette (see open questions).
```

with:

```
| # | what | frames |
|---|---|---|
| 0 | `var` 10 (player X): Zero silhouette, standing (helmet fins, long hair) | sprite 1, hold |
| 1 | `var` 10: Zero silhouette, dash (hair behind) | sprite 0, hold |
| 2 | `var` 11 (player Zero): standing, X [guess] | sprite 3, hold |
| 3 | `var` 11: dash, X [guess] | sprite 2, hold |

`SetAnime` is called with 0, 1, 2 and 3. No event flag is tested. Sprites 0–1 are magenta and
sprites 2–3 cyan. The code gives all shadows one CLUT (the spawner's), but every piece of sprites 2
and 3 has piece flag `0x04`, so the draw uses that CLUT + 1 [certain]. The palettes are flat: colour
0 transparent, colours 1–15 all `0xFC1F` (magenta, r 248 g 0 b 248) or all `0xEFE9` (cyan, r 72 g 248
b 216), the same in stages 2 and 6. Both words have bit 15 set (the texel semi-transparency bit);
whether the draw is semi-transparent is not read. The silhouettes are about 64 px high, larger than
the player (about 40 px). The shape names come from the sprites and from the game (X sees a purple
Zero, `wolfang-2_ice-slope`) [likely].
```

### "Damage", the ids

Replace:

```
  With HP 4, one hit of weapon id 7 or 16 kills it. No other weapon (also not the X-Buster ids 0, 9,
  18–20 or the sabre ids 24–31) has an effect. Which weapons ids 7 and 16 are is not known.
```

with:

```
  With HP 4, one hit of weapon id 7 or 16 kills it. No other weapon (also not the X-Buster ids 0, 9,
  18–20 or the sabre ids 24–31) has an effect. Ids 7 and 16 are Guard Shell, uncharged and charged
  (slot 7, Shield Sheldon, `docs/players/weapons.md`) [likely]: the weapon of the Maverick whose
  pick causes the effect.
```

### "Shared state"

Replace:

```
| `0x800CD30A` | Nightmare Effect (proposed `game.nightmare`) | read: must be 7 |
```

with:

```
| `0x800CD30A` | `nightmare_effect` | read: must be 7 |
```

### "Summary for a reimplementation"

Replace:

```
- Only under Nightmare Effect 7. Invisible spawners with a player-x zone each.
- In the zone, every 300/400/500 frames (random), one shadow of the current character appears 32 px in
  front of the player.
```

with:

```
- Only under Nightmare Effect 7 (stages 2 and 6, after the stage 7 pick). Invisible spawners from the
  fixed record list, with a player-x zone each.
- In the zone, every 300/400/500 frames (random), one shadow appears 32 px in front of the player: a
  magenta Zero when the player is X, a cyan shadow when the player is Zero.
```

Replace:

```
- 6 contact damage. HP 4; only weapon ids 7 and 16 hurt it (one hit kills). Every other weapon has no
  effect.
```

with:

```
- 6 contact damage. HP 4; only weapon ids 7 and 16 (Guard Shell [likely]) hurt it (one hit kills).
  Every other weapon has no effect.
```

### "Open questions"

Replace the whole list with:

```
1. (answered: the fixed record list, `var` = record `id`) What places the spawners. Still open:
   which list uses `var` 5.
2. (answered [likely]: Guard Shell) Which weapons are ids 7 and 16.
3. (answered: the pick of stage 7, `docs/engine.md`) What writes `0x800CD30A`, and which pick gives 7.
4. (answered: piece flag `0x04` selects CLUT + 1) The cyan palette of sprites 2–3. Still open: the
   shape of sprites 2–3 (X is a guess; a state with Zero as the player under effect 7 shows it), and
   whether the shadows are drawn semi-transparent (bit 15 in the colour words).
5. Does a class 2 hit stop the player's shot, or does the shot go through?
```

## `docs/engine.md`, "Nightmare Effect"

In "The value is the stage that caused it", replace:

```
    read), 6 wind and rain (`rain-effect`), 7 Nightmare shadows, 8
    the overlay 03 object at `0x800EE154`.
```

with:

```
    read), 3 main type 19 of stage 2 (set-up `0x800EEEA0`, a fixed-list record; not read further),
    6 wind and rain (`rain-effect`), 7 Nightmare shadows (stages 2 and 6, magenta Zero silhouettes
    for X; `docs/enemies/nightmare-shadow.md`), 8 the overlay 03 object at `0x800EE154`.
```

In "Values seen", add after the `shark-1_ride` sentence:

```
    `wolfang-*` `0 6 7 4 2 4 7 5 4` (`4 4 4`), `nightmare_effect` 7: stage 2 has 7 from an earlier
    stage 7 pick, and the last pick (stage 2, targets `4 4 4`) did not write over it.
```

## `catalog/helpers.json`

See `helpers-proposed.json`: the type 26 functions in overlays 04 and 09, and a new note for
`nightmare_effect` (`0x800CD30A`) that adds the values 3 and 7 with their stages.
