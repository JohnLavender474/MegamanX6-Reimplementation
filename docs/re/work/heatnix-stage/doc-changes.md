# Doc changes: Blaze Heatnix's stage, start (heatnix-1_start)

Exact text for the repo. The main session applies it after a check. Line anchors are quotes of the
current text (repo state of 2026-10-23, with the uncommitted changes of that day).

## 1. New file `docs/stages/heatnix.md`

The full text is [`~/mmx-data/x6/re/heatnix-stage/stage-page.md`](stage-page.md). Copy it as is.

## 2. New file `docs/enemies/flame-jet.md`

The full text is [`~/mmx-data/x6/re/heatnix-stage/sheet-flame-jet.md`](sheet-flame-jet.md). Copy it as is.

## 3. `catalog/enemies.json`, `names`

Add (sorted by key):

```json
"enemy-7f9bbc9a47": "flame-jet",
```

Do not name `enemy-6e15315f49` (main type 25, the mini boss) here: the mini-boss work folder
([`~/mmx-data/x6/re/heatnix-mini-boss/`](../heatnix-mini-boss/)) owns it.

## 4. `catalog/object-names.json`, `members`

Add a member 97 line (after `"96"`):

```json
"97": {"main:22": "NightmareBug", "main:28": "TeleporterCrystal", "main:36": "FlameJet",
       "main:52": "BlazeHeatnix", "stage:12": "GreyBlock"},
```

`main:25` is left out on purpose (the mini-boss work folder names it). `BlazeHeatnix` is [likely]:
the record is after the last boss gate and banks 14–15 are its type; the code was not read.
No clash: the names are the same as for the same objects in members 96, 100, 102.

## 5. `catalog/stages.json`, entry "Blaze Heatnix, section 1"

Add (the regions are by eye; see `docs/stages/heatnix.md`, "The texture blocks"):

```json
"texture_split": {"texture": 22, "regions": [[0, 1280, 1536, 2816, 3328], [0, 2816, 0, 4608, 3328], [1, 1280, 1536, 2816, 3328], [1, 2816, 0, 4608, 3328], [1, 2048, 1280, 2816, 1536], [2, 0, 1792, 4608, 3328]]}
```

Then run `tmx` again (the main session; `stages` does not read `texture_split`). Check the layer
1 and 2 regions again when a state after checkpoint 3 exists.

## 6. `catalog/helpers.json`

Merge `~/mmx-data/x6/re/heatnix-stage/helpers-proposed.json` (25 functions, 4 globals, all
overlay 5). One address is already used: `0x800EA3D4` is `RoomDimmer` (overlay 8). The proposed
file keeps that entry as it is and adds an `alt` for overlay 5 (`NightmareBug5`). Tested: a kit
run with the merged file in a scratch catalog shows `FlameJet*`, `NightmareBugInit5`,
`NightmareBugFollow5` and `NightmareBug5`. `0x800E98C0` is also the global
`ice_stack_var_switch` (overlay 4); the new entry is a function (overlay 5), so the two do not
meet (the kit keeps functions and globals apart).

## 7. `docs/engine.md`

### "Nightmare Effect", "The value is the stage that caused it"

Replace:

```
    6 wind and rain (`rain-effect`), 7 Nightmare shadows (stages 2 and 6, magenta Zero silhouettes
    for X; `docs/enemies/nightmare-shadow.md`), 8 the overlay 03 object at `0x800EE154`.
```

with:

```
    4 the grey block, Nightmare Iron (stage object 12 of stages 3 and 5 and of the stage 21 hidden
    area; `docs/enemies/grey-block.md`), 6 wind and rain (`rain-effect`), 7 Nightmare shadows
    (stages 2 and 6, magenta Zero silhouettes for X; `docs/enemies/nightmare-shadow.md`), 8 the
    overlay 03 object at `0x800EE154`. Stage 3 (overlay 05) has both the grey block (value 4) and
    the Nightmare Bug (value 1, main type 22) [certain for the two reads].
```

### "Values seen"

After:

```
    `wolfang-*` `0 6 7 4 2 4 7 5 4` (`4 4 4`), `nightmare_effect` 7: stage 2 has 7 from an earlier
    stage 7 pick, and the last pick (stage 2, targets `4 4 4`) did not write over it.
```

add:

```
    `heatnix-*` `0 6 3 4 2 4 7 5 3` (`2 8 8`), `nightmare_effect` 4: the stage 3 pick wrote 3 to
    stages 2 and 8; stage 3 keeps the 4 of the earlier stage 4 pick (`3 5 8`).
```

## 8. `docs/objects-legend.md`

### "Object types by sprite bank"

Add after the last Wolfang row (`| Wolfang | 54 | 14 | ...`):

```
| Heatnix | 128 | 0 | Light capsule and boss gate (shared bank) | none (visual type); the gate via `boss-gate` | code reading |
| Heatnix | 15 | 1 | Nightmare Virus | `nightmare-virus` (the same export) | code reading |
| Heatnix | 141 | 2 | Rescue Reploid (stage-object type 4) | none (drawn with sprite type 141) | code reading |
| Heatnix | 36 | 3 | flame jet: a fire nozzle in the first tower, up or down (`docs/enemies/flame-jet.md`) | `flame-jet` (`enemy-7f9bbc9a47`, from the code search) | code reading; descriptive name |
| Heatnix | 22 | 4 | Nightmare Bug (only when `nightmare_effect` = 1; not in the states) | `nightmare-bug` (the same export) | code reading |
| Heatnix | 142 | 5 | grey block, stage-object type 12 (Nightmare Iron, only when `nightmare_effect` = 4; the same bank as Turtloid bank 11) | `grey-block` (no live object at the start; `banks/.../bank-5.png`) | code reading |
| Heatnix | 25 | 6 | main type 25, the mini boss (green orbs, fire, sparks); its shot is visual type 29 | `enemy-6e15315f49` | see the mini-boss work folder |
| Heatnix | 28 | 7 | teleporter crystal | `teleport-crystal` (the same export) | code reading |
| Heatnix | 0 | 8–13 | no texture; the sprite data of bank 14 | none | code reading |
| Heatnix | 52 | 14–15 | not loaded at the start (probably Blaze Heatnix) | none yet | plausible |
```

### "Objects of each stage"

Replace:

```
The record lists of each stage are in `docs/stages/` (`intro.md`, `yammark.md`, `turtloid.md`).
```

with:

```
The record lists of each stage are in `docs/stages/` (`intro.md`, `yammark.md`, `turtloid.md`,
`heatnix.md`, and the others there).
```

## 9. `docs/enemies/grey-block.md`

After the paragraph that ends "`add 0x3670 for overlay 08 code, 0x9510 for its data`. All values
are [certain for the code] unless a line says otherwise." add:

```
**Stage 3.** Overlay `05` (Blaze Heatnix) has the same object as stage object 12 (entry
`0x800EA8E8`, set-up `0x800EA5C8`; overlay 29 address − `0x26C`). All 344 words of
`0x800EA5C8`–`0x800EAB28` equal the overlay 29 code after relocation (`heatnix-1_start`) [certain].
Records: 5 with `id` 3 in the first tower and 10 with `id` 4 in the shaft room and the side
corridor (`docs/stages/heatnix.md`). Sprite type 142 is bank 5 there; the bank sheet is the same
as that of stage 5.
```

In "The `id` parameter", replace:

```
`var` 3; it has checkpoints 0 and 1 only, so its blocks never go away.
```

with:

```
`var` 3; it has checkpoints 0 and 1 only, so its blocks never go away. Stage 3 uses `var` 3 (gone
at checkpoint 3) and 4 (gone at checkpoint 4).
```

## 10. `docs/enemies/nightmare-bug.md`

At the end of "When it exists", add:

```
- **Stage 3.** Overlay `05` has the same object as main type 22 (code `0x800EA07C`–`0x800EA5C8`,
  entry `0x800EA3D4`; 339 words equal after relocation) [certain]. 19 records (90–108, `id` 0,
  kind 0; `docs/stages/heatnix.md`). In the `heatnix-*` states `nightmare_effect` is 4, so they
  delete themselves at once.
```

## 11. `docs/enemies/teleport-crystal.md`

After the "**Turtloid.**" paragraph, add:

```
**Heatnix.** Overlay `05` has the same code as main type 28 (entry `0x800EE488`; all 846 words
of `0x800EDB90`–`0x800EE8C8` equal overlay 03 after relocation; `heatnix-1_start`) [certain for
the code; the data was not compared]. Record 126 has `id` 1 (sequence A, area change).
```

## 12. `docs/enemies/boss-gate.md`

In "Open questions", item 1, after "(the open conditions of ids 7, 8, 10, 12, 14, 15 and 44)" add:

```
Ids 7, 8, 10, 12, 14 and 15 are all gates of Blaze Heatnix's stage (`docs/stages/heatnix.md`);
there `game+0x26` is probably set by the mini bosses (main type 25) [guess].
```

In item 6, replace "in which stages ids 7, 18, 25, 31 and 39 are used" with "in which stages ids
18, 25, 31 and 39 are used (id 7: stage 3, second list, at (944, 608); the main type 25 that it
makes is the mini boss of that stage)".

## 13. `docs/process/process-log.md`

One line: "2026-10-23: Blaze Heatnix start triage (`heatnix-1_start`): stage page, flame jet
(main 36) sheet; grey block, Nightmare Bug and teleporter of overlay 05 are copies; texture split
proposed."
