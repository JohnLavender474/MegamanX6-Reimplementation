# Doc changes for the ice stack block (stage 2, stage object 7)

Apply after you check the claims (notes.md, "Check against the save state"). All paths are in
`~/IdeaProjects/mmx-demake-tools/` unless they start with `~`.

## 1. New file `docs/enemies/ice-stack-block.md`

Copy [`~/mmx-data/x6/re/wolfang-stage7/sheet.md`](sheet.md) and remove its line 3 ("Proposed as ...").

## 2. `docs/enemies/wolfloid.md`

### 2a. "The carried wolf": replace the last paragraph

Old:

```
The blocks come from stage object 7 `var` 0 (records 58 and 59 of section 1), which starts when the
player is in the lower room (x ≥ 5137 and y ≥ 1473, or x ≥ 5345 and y ≥ 1489) [likely: read in
part]. What the block's states 2 and 4 mean exactly is in that object's code [guess: 2 = it
stopped, 4 = it broke].
```

New:

```
The blocks are pit blocks of the ice stack block (`docs/enemies/ice-stack-block.md`, stage object
7 `var` 12). The two pit generators (`var` 0, records 58 and 59 of section 1) start when the player
is in a pit (x ≥ 5137 and y ≥ 1473, or x ≥ 5345 and y ≥ 1489) and make a block every 128 updates
(64 from Hard) in a random column [certain]. The block states for the wolf [certain]:

- state 2: the block has landed and became map tiles (about 48 updates after the landing). The
  wolf becomes a normal flat wolf (id 2 or 3) and stands on the new tiles; gravity 0 is enough,
  because the tiles are under it.
- state 4: the block fell on the head of a player who stood on the floor and broke. The wolf
  dies with its drop.

Only `var` 12 blocks call `SolidMainObjects` (`0x80031F4C`), so the falling block is also solid for
the wolf [likely]. In `wolfang-5_ice-pit` a cannon wolf (`var` 5) rides a block that has just
landed in column 5: wolf y 1551 = the block's y of the update before (1590) − 39 [certain].
```

### 2b. "Open questions", item 2

Old:

```
2. The carrier: stage object 7 (falling ice blocks and their generator) has no sheet. The meaning
   of its states 2 and 4 for the wolf is a guess. A released wolf keeps gravity 0; whether it can
   stay in the air when its block is gone is not checked.
```

New:

```
2. The carrier is described in `docs/enemies/ice-stack-block.md`. A released wolf keeps gravity
   0; it stands on the tiles that its block became, so it does not hang in the air. When the
   player enters the ice shaft while a pit block still carries a wolf, the shaft controller clears
   the block slot and the wolf follows y = 0 − 39 [likely; not seen in play].
```

### 2c. `docs/enemies/wolfloid.md`, the step 2/3 note on the carried wolf

No change needed. (The wolf code was read before; it is consistent with the block code.)

## 3. `docs/TODO.md`

Old:

```
- [ ] Stage object 7 of stage 2 (overlay 04, state 0 `0x800EBAA0`, 13 `var` cases): the falling ice
  blocks of the lower room and their generator (`var` 0, records 58 and 59; blocks are `var` 12);
  one block in two carries a Wolfloid. Not the ice blocks of main type 17. Needs a save state in
  the lower room (x about 5000–5400, y about 1300–1500).
```

New:

```
- [x] Stage object 7 of stage 2 (overlay 04): the ice stack block
  (`docs/enemies/ice-stack-block.md`): the pit generators (`var` 0, records 58 and 59), the pit
  blocks (`var` 12, one in two with a Wolfloid), and the rows with one gap of the ice shaft
  (`var` 1–5, 10, 11) that stage logic 8 makes. Save state `wolfang-5_ice-pit`.
- [ ] Ice shaft of stage 2 (stage logic 8, x 5760–6000, y 312–1264): no save state has it. Check in
  play: the gap filler kills a player in the gap; a row on the player's head does 16 damage and
  breaks; the extra gap fillers at row x ± 144 (side niches?); the camera field `+0x2C`.
- [ ] Tool: rename the export `enemy-583d456ecb` (stage type 7 of stage 2) to `ice-stack-block`
  (`catalog/enemies.json`, `names`) and run `spriteexport` on `wolfang-5_ice-pit.state.gz`
  (the sheets are good: checked by eye in `~/mmx-data/x6/re/wolfang-stage7/sprites-root`).
```

## 4. `docs/objects-legend.md`, the table "Object types by sprite bank"

Add after the row `| Wolfang | 144 | 8 | scenery: ...`:

```
| Wolfang | 134 | 4 | ice stack block, stage-object type 7: pit generators, falling blocks that become map tiles (one in two carries a Wolfloid), the rows with one gap of the ice shaft (stage logic 8), and the sparkle (misc type 9) (`docs/enemies/ice-stack-block.md`) | `ice-stack-block` (was `enemy-583d456ecb`; from its live object: stage-object class, type 7, table `0x800F6B6C`) | user (falling ice blocks in a pit); code reading |
```

## 5. `catalog/object-names.json`, member `"96"`

Add two entries:

```
"stage:7": "IceStackBlock",
"logic:8": "IceShaftController"
```

so the member reads:

```
"96": {"main:6": "Wolfloid", "main:17": "IceBlock", "main:19": "NightmareFireRing",
       "main:26": "NightmareShadow", "main:28": "TeleporterCrystal", "misc:23": "SnowScenery",
       "stage:7": "IceStackBlock", "logic:7": "IceSlide", "logic:8": "IceShaftController",
       "logic:12": "IceFallController"}
```

(Keep the line layout of the file; the order inside the member is free.)

## 6. `catalog/enemies.json`, `names`

```
"enemy-583d456ecb": "ice-stack-block",
```

## 7. `catalog/helpers.json`

Add the entries of `~/mmx-data/x6/re/wolfang-stage7/helpers-proposed.json` (sorted by address,
one per line). The entry `0x800EBAA0` replaces the old guess `WolfangStageObject7Init`.
`0x80031F4C` and `0x80032BE8` are main-program code: no `overlay`.

## 8. `docs/engine.md`, "Solid objects"

Add to the list "Other callers of `SolidObject`":

```
- `IceStackBlockMain` (`0x800EBF18`, overlay 04): only when `mega.squeeze_timer` = 0 and |dx| <
  48. It reads the touch flag `0x04` (player under the block) after the call and breaks the block
  when a copy of the player (`0x800F76EC`) stands on floor tiles (`docs/enemies/ice-stack-block.md`).
```

And after the paragraph on `SolidPushShots` (or at the end of the section):

```
- `SolidMainObjects(obj)` (`0x80031F4C`): the same push, carry and squeeze undo for the 48 main
  objects with a terrain box (a touch bit per object in `obj+0x98`) [likely for the carry].
- `SolidVisualHit(obj)` (`0x80032BE8`): every live visual object whose box touches `obj` gets
  `collide_flags` |= `0x0F`, so a shot that checks its flags ends on the object [certain for the
  code].
```

## 9. Process log (`docs/process/process-log.md`)

```
- Stage 2 stage object 7 (ice stack block) and stage logic 8 (ice shaft controller) read from
  `wolfang-5_ice-pit` with the kit and `--at` (`~/mmx-data/x6/re/wolfang-stage7/`). New sheet
  `docs/enemies/ice-stack-block.md`; Wolfloid carrier part closed.
```
