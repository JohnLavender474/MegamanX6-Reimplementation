# Doc changes from `mijinion-4_dynamo` (Dynamo, stage 22 area 1)

All paths are in `~/IdeaProjects/mmx-demake-tools` unless marked. Check the claims in
[`~/mmx-data/x6/re/dynamo/notes.md`](notes.md) ("What the docs depend on") before you apply them.

## 1. `docs/enemies/dynamo.md`

Replace the whole file with [`~/mmx-data/x6/re/dynamo/sheet.md`](sheet.md). What changed against the repo
version: the header (live state, kit command, overlay check, the game text names him, export);
the HP line (level 2 = 62, live); the room points in world values and "no read of
`nightmare_effect` or the difficulty"; INTRO sub 1 and 2 (the camera move of 16 px, the beam-in,
the world-y test, `talk_seen`); the texts; a new "Live check" table; the animation table with the
pictures and the unused sprites; the death beam-out; the summary line; the open questions (old 3
pictures, 6 texts and part of 1 are answered).

## 2. `docs/stages/mijinion-alt.md` (new file)

Copy [`~/mmx-data/x6/re/dynamo/area-page.md`](area-page.md) to `docs/stages/mijinion-alt.md`.

## 3. `docs/stages/sheldon-alt.md`

Old:

```
4. A state with `boss_choice` 2 (Dynamo) in any hidden area.
```

New:

```
4. A state with `boss_choice` 2 (Dynamo) in this area. Stage 22 area 1 has one:
   `mijinion-4_dynamo` ([`mijinion-alt.md`](mijinion-alt.md)).
```

## 4. `docs/stages/scaravich-alt.md`

Old:

```
overlay"). The other two hidden areas that have pages are stage 21 area 1 (Turtloid,
[`turtloid.md`](turtloid.md), "Alternate section") and stage 22 area 0
([`sheldon-alt.md`](sheldon-alt.md)). This area is one room with a maze, a short corridor between
```

New:

```
overlay"). The other hidden areas that have pages are stage 21 area 1 (Turtloid,
[`turtloid.md`](turtloid.md), "Alternate section"), stage 22 area 0
([`sheldon-alt.md`](sheldon-alt.md)) and stage 22 area 1 ([`mijinion-alt.md`](mijinion-alt.md),
the only one with camera zones). This area is one room with a maze, a short corridor between
```

## 5. `docs/engine.md`

Old:

```
  main type 55 `var` 1 [certain]. Overlays of the hidden areas: stage 21 area 1 = 30, stage 22 area
  0 = 31; both fit overlay = 25 + 2 × (stage − 19) + area [likely; two points only].
```

New:

```
  main type 55 `var` 1 [certain]. Overlays of the hidden areas: stage 21 area 0 = 29, stage 21
  area 1 = 30, stage 22 area 0 = 31, stage 22 area 1 = 32; all fit overlay = 25 + 2 × (stage − 19)
  + area [likely; four points]. `mijinion-4_dynamo` (stage 22 area 1, `boss_choice` 2): overlay 45
  at `0x800FA000` (18 of 11 878 bytes differ, all variables), main type 63 = Dynamo [certain]
  (`docs/enemies/dynamo.md`).
```

Old:

```
  the code and for the names of 0 and 1 (`turtloid-5_zero`, `turtloid-8_high-max`); the name for 2 is
  likely]:
```

New:

```
  the code and for the names of 0, 1 and 2 (`turtloid-5_zero`, `turtloid-8_high-max`,
  `mijinion-4_dynamo`; the text of messages 2 and 3 of every hidden-area text bank has the speaker
  "Dynamo")]:
```

## 6. `docs/TODO.md`

Old:

```
- [ ] Hidden areas: where a hidden area leads after its boss; a hidden area with `boss_choice` 2.
```

New:

```
- [ ] Hidden areas: where a hidden area leads after its boss (read from the code:
  `docs/enemies/high-max.md`, "Where the clear leads"; no state after a clear yet). A hidden area
  with `boss_choice` 2: done (`mijinion-4_dynamo`, `docs/stages/mijinion-alt.md`).
```

Old:

```
  Type 55 = High Max, type 63 = Dynamo [likely].
```

New:

```
  Type 55 = High Max, type 63 = Dynamo (`mijinion-4_dynamo`).
```

Old:

```
- [ ] A hidden-area save state with `boss_choice` 2, to export Dynamo.
```

New:

```
- [x] A hidden-area save state with `boss_choice` 2, to export Dynamo: `mijinion-4_dynamo`; export
  `dynamo` (`enemy-f49a77c40e`, 15 of 15 animations).
- [ ] Sprite exporter: `catalog/enemies.json` `stages` maps a stage number only, so stage 21 area 0
  (Scaravich) goes to `turtloid-alt` and stage 22 area 1 (Mijinion) to `sheldon-alt`. Allow a
  `"<stage>.<area>"` key (for example `"21.0": "scaravich-alt"`, `"22.1": "mijinion-alt"`) that
  wins over the stage key. The old folders `sprites/banks/stage-22/` and `sprites/stages/stage-22.json`
  (from a run before the map entry) can go.
- [ ] Sprite exporter: in `mijinion-4_dynamo` bank 3 (main type 8) is drawn from cells that
  Dynamo's texture covers, but the overwrite check does not mark it (wrong search export
  `enemy-06ef7e2514`). The same fault as `enemy-08bc9efa49` in `mijinion-3_boss-fight`.
- [ ] Dynamo: a state of the other moves (pillars, centre volley, leaping slash), of the stun and
  of the death.
```

## 7. `docs/enemies/light-capsule.md`

Old:

```
| 2 | `0x02` | Blade | arms | 5 | 5 | – |
```

New:

```
| 2 | `0x02` | Blade | arms | 5 | 5 | stage 22 area 1 (Mijinion's hidden area), record 27 (`docs/stages/mijinion-alt.md`) |
```

## 8. `docs/enemies/illumina.md`

Old:

```
| main 28, 1 | a teleporter crystal at (4384, 864) | the body at its death, when fight 2 took < 5401 updates |
```

New:

```
| main 28, 1 | a teleporter crystal at (4384, 864): the way into the hidden area, stage 22 area 1 (`docs/stages/mijinion-alt.md`) | the body at its death, when fight 2 took < 5401 updates |
```

## 9. `catalog/enemies.json`

In `names`, add (sorted by key):

```
"enemy-f49a77c40e": "dynamo",
```

In `stages`, nothing until the exporter takes an area key (TODO item above). Until then the
Mijinion hidden-area banks go to `banks/sheldon-alt/mijinion-4_dynamo/`.

## 10. `catalog/helpers.json`

Add the entries of `~/mmx-data/x6/re/dynamo/helpers-proposed.json`:

- `functions` (22 entries, all `"overlay": 45`) and `globals` (13 overlay 45 entries, plus
  `dmg_tbl_dynamo` `0x80075A5C`, `dmg_tbl_dynamo_blade` `0x80075AFC` and `game.dynamo_souls`
  `0x800CCF03`, without overlay). No name clashes with the catalog.
- Three addresses already have a name for another overlay. Add an `alt` to the existing entry
  (listed in `alt_additions`): `0x800FA984` (`sabre_block_terrain_box`, overlay 2) → `alt`
  overlay 45 `DynamoBladeInit`; `0x800FA004` (`high_max_substep_max`, overlay 44) → `alt` overlay
  45 `dynamo_hp_bonus`; `0x800FCE58` (`mijinion_rows_clone`, overlay 11) → `alt` overlay 45
  `dynamo_dead`.
- `0x800FA000` `boss_overlay`: overlay `[43, 44, 45]`, note "... 44 High Max, 45 Dynamo)"
  (`changes_to_existing`).

## 11. Shared sprite outputs (main session, not a doc)

Run `spriteexport --state mijinion-4_dynamo.state.gz` on a copy of `~/mmx-data/x6/sprites/` (it
will write `banks/sheldon-alt/mijinion-4_dynamo/` and `stages/sheldon-alt.json`), then:

- keep `enemies/enemy-f49a77c40e/` → named `dynamo` by the `names` entry above;
- delete `enemies/enemy-06ef7e2514/` (wrong, see TODO) and its `bank-3.png`; add to the
  `skipped` list of `mijinion-4_dynamo` in `stages/sheldon-alt.json`:
  `{"type": 8, "reason": "bank 3 drawn from cells that the Dynamo texture covers; wrong search export enemy-06ef7e2514 removed by hand (docs/TODO.md)"}`;
- `boss-gate`, `nightmare-virus`, `nightmare-virus-orb` get the state as a new source (the same
  IDs).

The checked copy is in `~/mmx-data/x6/re/dynamo/sprites-root/`.

## 12. `docs/process/process-log.md`

Add a section after the last numbered section (before "Lessons so far"):

```
## 90. Dynamo, live

The user saved a state in the boss room of Infinity Mijinion's hidden area (stage 22, area 1):
the first state with `boss_choice` 2. A helper agent checked the Dynamo sheet, which was read from
the disc only, against the live objects, and wrote the page of the area.

- The overlay at `0x800FA000` is overlay 45; only its variables differ. Every live value (HP 62 at
  boss level 2, the room points, the blade count, the move pattern, the camera) agrees with the
  sheet.
- The export gave the pictures: the "intro pose" is a beam-in, and the "leap away" is a beam-out.
- The area has the first camera zones of a hidden area, a Light capsule (Blade Armor arms) and
  a zone of the Nightmare Effect 3 fire.

Lessons:

- A sheet read from the disc only was right in all the facts that one state can check. The names
  of the moves still needed the pictures.
```
