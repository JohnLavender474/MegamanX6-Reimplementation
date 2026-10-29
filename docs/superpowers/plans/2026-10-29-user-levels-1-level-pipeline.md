# User levels, plan 1: the level pipeline (desktop) Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Load, validate and play X6 TMX profile levels on the desktop: the loader on `XmlReader`, the catalog and `LevelValidator`, the intro data in `intro.tmx`, the goal system, and `-Dmmx.level=<file>`.

**Architecture:** `TmxLoader` parses TMX text with LibGDX `XmlReader` and never reads `.tsx` files. `LevelValidator` checks the TMX text against `assets/levels/catalog.json`, applies repairs to the parsed element tree, and returns findings and a status. `StageLoader` is the one entry point that loads a built-in stage or a level file, validates profile maps, and builds the effective `StageDef` from the map data. `Goals` (owned by `World`) keeps the goal state, keyed by record.

**Tech Stack:** Java 17, LibGDX 1.14.2 (`com.badlogic.gdx.utils.XmlReader`, `JsonReader`), JUnit 5, Gradle.

**Spec:** `docs/superpowers/specs/2026-10-29-user-levels-design.md` (steps 1 to 5 of "Order of work"). Later plans: 2 the web port (step 6), 3 the Tiled kit (step 7), 4 the relocatable boss room (step 8).

## Global Constraints

- Java 17, LibGDX 1.14.2. Logic at 60 updates per second. X6 values from the docs unchanged.
- No code comments, except `TODO`s in the project format: what the edge case or guess is, and where it is tracked, for example `// TODO: ... (docs/superpowers/specs/2026-10-29-user-levels-design.md, "Loader")`.
- Guesses go in `docs/gaps.md`. The user's design choices go in `docs/custom-changes.md`.
- Every window opened for a check is an automated run (`-Dmmx.shot=...`), screenshots go in the session scratchpad.
- The validator checks engine compatibility only (spec decision 17). Do not add rules about spawns, checkpoint placement or whether a level can be finished.
- The six other stages must play exactly as before. `StageSmokeTest` stays green after every task.
- Work on the branch `user-levels`, not on `main`. Do not push. Each commit message ends with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Tests: `./gradlew test` (results in `build/test-results/test/*.xml`). Tests that need real data call `Assumptions.assumeTrue(DataPaths.assetsExist())`.

## Review Focus

1. CSV data with `\r\n` line ends, spaces and a trailing comma (a Tiled save on Windows): the loader reads the same gids. Test in Task 1.
2. A property value of the wrong form (`"abc"` for an int, `"yes"` for a bool) in a user file: the validator gives a reject finding; it never throws. Test in Task 4.
3. A generic enemy that is a goal member and is despawned, spawned again and then killed: it counts once, and only the kill counts. Test in Task 6.
4. A death and restart after the goal is done: the goal state resets, and the next boss death ends the stage exactly once (`lockCount` is 1). Test in Task 6.
5. A level file with a name and folder unlike any built-in map (`/tmp/x/my-level.tmx`): the tile art comes from the theme, not the file name. Test in Task 7.

---

## File map

| file | change |
|---|---|
| `src/main/java/mmx/x6/level/TmxLoader.java` | rewrite on `XmlReader`; new parse entry points |
| `src/main/java/mmx/x6/level/RecordDef.java` | add `tmxId`, `goal`, `openedBy` |
| `src/main/java/mmx/x6/level/Level.java` | add `properties`, `checkpoints`, `goals` |
| `src/main/java/mmx/x6/level/GoalDef.java`, `GoalMode.java` | new |
| `src/main/java/mmx/x6/level/Catalog.java` | new: the parsed `catalog.json` |
| `src/main/java/mmx/x6/level/Finding.java`, `ValidationReport.java`, `LevelValidator.java` | new |
| `src/main/java/mmx/x6/level/IntroCheckpoints.java` | delete (data moves to `intro.tmx`) |
| `src/main/java/mmx/x6/stage/StageDef.java` | add `withMapData` |
| `src/main/java/mmx/x6/stage/Stages.java` | `INTRO` without data; new `SANDBOX` |
| `src/main/java/mmx/x6/stage/StageLoader.java`, `LoadedStage.java`, `Themes.java` | new |
| `src/main/java/mmx/x6/game/Goals.java`, `GoalMember.java` | new |
| `src/main/java/mmx/x6/game/World.java`, `game/enemy/Enemy.java`, `game/boss/D1000.java`, `game/boss/Maverick.java`, `game/enemy/BossGate.java` | goal hooks |
| `src/main/java/mmx/x6/DataPaths.java` | `catalogJson()`, `tilesPngNamed(String)` |
| `src/main/java/mmx/x6/GameFlow.java`, `X6Game.java`, `screen/LevelErrorScreen.java` (new) | `-Dmmx.level` |
| `assets/levels/catalog.json` | new |
| `assets/tiled/tmx/intro.tmx` | map properties, `x6 checkpoints`, `goals`, `goal` on `D1000` |
| `src/test/java/mmx/x6/level/TmxFixture.java` | new: builds small valid profile maps as text |

---

### Task 1: `TmxLoader` on `XmlReader`, with Tiled ids on records

**Files:**
- Modify: `src/main/java/mmx/x6/level/TmxLoader.java` (whole file), `src/main/java/mmx/x6/level/RecordDef.java`
- Create (temporary): `src/test/java/mmx/x6/level/LegacyTmxLoader.java` (a copy of the current `TmxLoader`, class renamed)
- Test: `src/test/java/mmx/x6/level/TmxLoaderGoldenTest.java` (temporary), `src/test/java/mmx/x6/level/TmxLoaderTest.java`

**Interfaces:**
- Produces:
  - `RecordDef(int index, String name, String type, int kind, int b, int id, int a, int x, int y, int tmxId, int goal, int openedBy)`, plus the old 9-argument constructor that sets the 3 new fields to `0`.
  - `TmxLoader.load(Path tmx) throws IOException` (reads the text, calls `parse`).
  - `TmxLoader.parse(String tmxText, String name) throws IOException`.
  - `TmxLoader.fromElement(XmlReader.Element map, String name) throws IOException`.
  - The collision tileset is the `<tileset>` whose `source` basename is `collision.tsx`, or, with no `source`, whose `name` is `collision`. No `.tsx` file is read.

- [ ] **Step 1: Copy the current loader to the test sources** as `LegacyTmxLoader` (same code, class renamed, package `mmx.x6.level`).

- [ ] **Step 2: Write the failing golden test** `TmxLoaderGoldenTest.newLoaderMatchesLegacy`, parameterized over every `assets/tiled/tmx/*.tmx` file (18 files), with `assumeTrue(DataPaths.assetsExist())`. For each map assert: equal `width`, `height`; for each of the 5 layers equal `name`, `width`, `height`, `x6Mode`, `x6OffsetX`, `x6OffsetY`, and `Arrays.equals(gids)`; equal collision `raw(cx, cy)` for every cell; equal `cameraZones()`; and for `records()` and `fixedRecords()` equal lists after mapping each record to its first 9 fields. Also assert that every record's `tmxId` is greater than 0.

- [ ] **Step 3: Add a CSV form test** `TmxLoaderTest.csvWithCrLfSpacesAndTrailingComma`: a 2 by 2 map text whose `art 0 main` data is `"\r\n1, 2,\r\n 3,4,\r\n"`; assert `gids` is `{1, 2, 3, 4}`. Build the text with `TmxFixture` (Task 3 creates it; for this task write the string inline).

- [ ] **Step 4: Run the tests to verify they fail**

Run: `./gradlew test --tests 'mmx.x6.level.TmxLoader*'`
Expected: compile failure (`tmxId()` and `parse` do not exist).

- [ ] **Step 5: Implement `RecordDef` and the new `TmxLoader`.** Read `id` (the Tiled object id) into `tmxId`, and the properties `goal` and `opened_by` into `goal` and `openedBy` (`0` when absent). Keep the current semantics: `x`/`y` truncated from doubles, `zone_id & 0x7F`, the property defaults, `& 0x1FFFFFFF` on collision gids, `List.copyOf` results, the `IOException` messages for a missing layer or collision tileset.

- [ ] **Step 6: Run the tests to verify they pass**

Run: `./gradlew test`
Expected: all tests pass, including the golden test for 18 maps and `StageSmokeTest`.

- [ ] **Step 7: Commit**

```bash
git add src/main/java/mmx/x6/level src/test/java/mmx/x6/level
git commit -m "Port TmxLoader to XmlReader and keep the Tiled object id on records"
```

- [ ] **Step 8: Remove `LegacyTmxLoader` and `TmxLoaderGoldenTest`** (spec: the old loader is removed after the test passes). Run `./gradlew test`; expected PASS. Commit with the message `Remove the legacy TMX loader after the golden check`.

---

### Task 2: Map properties and the `x6 checkpoints` layer in `Level`

**Files:**
- Modify: `src/main/java/mmx/x6/level/Level.java`, `src/main/java/mmx/x6/level/TmxLoader.java`
- Test: `src/test/java/mmx/x6/level/TmxLoaderTest.java`

**Interfaces:**
- Consumes: `TmxLoader.parse` (Task 1).
- Produces:
  - `Level(..., List<RecordDef> fixedRecords, Map<String, String> properties, List<Checkpoint> checkpoints, List<GoalDef> goals)`; the existing 9- and 10-argument constructors keep working with empty values. (`GoalDef` is created in Task 6; in this task add the component typed `List<GoalDef>` only after Task 6, or create `GoalDef` as an empty record now and fill it in Task 6. Choose the second.)
  - `Level.properties()`: the map's `<properties>`, name to `value` text.
  - `Level.checkpoints()`: from the object layer `x6 checkpoints`, sorted by the property `checkpoint`. Each point object gives `new Checkpoint(x, y, cam_x, cam_y, min_x, max_x, min_y, max_y, face_right, beam_thru, layer1, layer2Offset)`; `layer1` is `new Point(layer1_x, layer1_y)` when both exist, else `null`; the same for `layer2_offset_x`/`layer2_offset_y`.

- [ ] **Step 1: Write the failing test** `TmxLoaderTest.readsMapPropertiesAndCheckpoints`: a map text with map properties `x6_profile=1`, `x6_theme=intro`, and an `x6 checkpoints` layer with two points in reverse order (`checkpoint` 1 at (2112, 1072) with `cam_x` 1968, `cam_y` 944, `min_x` 1968, `max_x` 5120, `min_y` 944, `max_y` 944, `face_right` true, `beam_thru` 6; `checkpoint` 0 at (144, 976) with 0, 944, 0, 5120, 896, 944, true, 1). Assert `properties().get("x6_theme")` is `"intro"`, and `checkpoints()` equals `List.of(new Checkpoint(144, 976, 0, 944, 0, 5120, 896, 944, true, 1), new Checkpoint(2112, 1072, 1968, 944, 1968, 5120, 944, 944, true, 6))`.

- [ ] **Step 2: Run it to verify it fails.** Run: `./gradlew test --tests mmx.x6.level.TmxLoaderTest`. Expected: compile failure.

- [ ] **Step 3: Implement** the new `Level` components and the parsing in `TmxLoader`.

- [ ] **Step 4: Run** `./gradlew test`. Expected: all pass.

- [ ] **Step 5: Commit** (`Read map properties and the x6 checkpoints layer`).

---

### Task 3: Catalog, `LevelValidator` structure rules, `TmxFixture`

**Files:**
- Create: `src/main/java/mmx/x6/level/Catalog.java`, `Finding.java`, `ValidationReport.java`, `LevelValidator.java`
- Create: `src/test/java/mmx/x6/level/TmxFixture.java`, `src/test/java/mmx/x6/level/LevelValidatorTest.java`
- Create: `assets/levels/catalog.json`
- Modify: `src/main/java/mmx/x6/DataPaths.java` (add `Path catalogJson()` = `root/levels/catalog.json`)

**Interfaces:**
- Produces:
  - `Catalog.parse(String json)`; `Catalog.Theme theme(String key)` (null if unknown); `Catalog.Entity entity(String name)` (null if unknown); `Set<String> themeKeys()`.
  - `record Catalog.Theme(String key, String artSource, int artTilecount, String tilesPng, Map<String, String> properties, Region fixedRegion)`; `record Catalog.Region(int x0, int x1, String template)` (`fixedRegion` may be null).
  - `record Catalog.Entity(String name, List<String> themes, boolean stageBound, boolean defeatable, boolean openable)`.
  - `record Finding(Result result, String code, String message)`; `enum Finding.Result { REJECT, REPAIR, WARN, LOCK }`.
  - `enum ValidationReport.Status { CERTIFIED, PLAYABLE, REJECTED }`; `record ValidationReport(List<Finding> findings, XmlReader.Element map)` with `Status status()`: REJECTED if any REJECT; else PLAYABLE if any WARN or LOCK; else CERTIFIED. `map` is null when the text does not parse.
  - `LevelValidator.validate(String tmxText, Catalog catalog, Function<String, String> templates)`; `templates` maps a TMX file name to its text (used by Task 4), and may return null. It never throws.
  - `TmxFixture.valid()` returns a builder; `build()` returns TMX text. The default is a valid profile map: 16 by 16 tiles, `infinite="0"`, map properties `x6_profile` 1, `x6_theme` `intro` and the six intro values, tilesets `../tsx/intro-tiles.tsx` at 1 and `../tsx/collision.tsx` at 3265, the 5 tile layers with CSV zeros, empty `records`, `fixed records` and `camera zones` layers, and an `x6 checkpoints` layer with `checkpoint` 0. Builder methods: `mapAttr(String, String)`, `mapProperty(String name, String type, String value)`, `removeMapProperty(String)`, `tilesets(String xml)`, `removeLayer(String)`, `layerData(String layer, String csv)`, `layerAttr(String layer, String attr, String value)`, `objectLayer(String name, String objectsXml)`, `raw(String xmlBeforeMapEnd)`.
  - `TmxFixture.CATALOG`: a catalog JSON string with the `intro` theme (the real values below, `fixed_region` absent) and the entities `MetaWheelF` (generic, defeatable), `BossGate` (stage-bound, openable), `D1000` (stage-bound, defeatable).

`assets/levels/catalog.json` content (the full intro catalog):

```json
{
  "themes": {
    "intro": {
      "art_source": "intro-tiles.tsx",
      "art_tilecount": 3264,
      "tiles_png": "intro-tiles.png",
      "properties": {
        "x6_music": "9", "x6_boss_music": "21", "x6_bank2": "m094-e05", "x6_bank5": "m094-e03",
        "x6_boss_bank2": "m123-e01", "x6_nightmare_effect": "0"
      },
      "fixed_region": {"x0": 5744, "x1": 6655, "template": "intro.tmx"}
    }
  },
  "entities": {
    "MetaWheelF": {"themes": ["intro"], "stage_bound": false, "defeatable": true, "openable": false},
    "MetaDridler": {"themes": ["intro"], "stage_bound": false, "defeatable": true, "openable": false},
    "Junkroid": {"themes": ["intro"], "stage_bound": false, "defeatable": true, "openable": false},
    "SabreBlock": {"themes": ["intro"], "stage_bound": false, "defeatable": true, "openable": false},
    "HoverGunner": {"themes": ["intro"], "stage_bound": false, "defeatable": true, "openable": false},
    "BossGate": {"themes": ["intro"], "stage_bound": true, "defeatable": false, "openable": true},
    "D1000": {"themes": ["intro"], "stage_bound": true, "defeatable": true, "openable": false},
    "IntroWreckA": {"themes": ["intro"], "stage_bound": true, "defeatable": false, "openable": false},
    "IntroWreckB": {"themes": ["intro"], "stage_bound": true, "defeatable": false, "openable": false},
    "IntroHunters": {"themes": ["intro"], "stage_bound": true, "defeatable": false, "openable": false},
    "TalkZone": {"themes": ["intro"], "stage_bound": true, "defeatable": false, "openable": false},
    "PaletteCycle": {"themes": ["intro"], "stage_bound": true, "defeatable": false, "openable": false},
    "CheckpointMarker": {"themes": ["intro"], "stage_bound": true, "defeatable": false, "openable": false},
    "CameraZone": {"themes": ["intro"], "stage_bound": true, "defeatable": false, "openable": false}
  }
}
```

The fixed region `x0` 5744 to `x1` 6655 (all rows) covers the hallway gate (x 5744), the room and the tile store columns that `D1000.copyColumn` reads (up to x 6576 + 16). It is my call; record it in `docs/gaps.md` in Task 4.

Structure rules in this task (code: result):

| code | rule | result |
|---|---|---|
| `parse` | the text is not well-formed XML, or the root is not `map` | REJECT |
| `map` | `orientation` is not `orthogonal`, `tilewidth`/`tileheight` not 16, `infinite` not `0`, or `width`/`height` not a multiple of 16 | REJECT |
| `group-layer` | the map has a `<group>` element | REJECT |
| `tileset` | not exactly 2 tilesets; a tileset with no `source` (embedded); the first not at `firstgid` 1 with the theme's `art_source` basename; the second not `collision.tsx` at `art_tilecount + 1` | REJECT |
| `layer-missing` | one of the 5 tile layers is missing | REJECT |
| `layer` | a tile layer's size differs from the map, or its `data` is not `encoding="csv"` with no `compression` | REJECT |
| `gid-range` | an art layer gid (flip bits cleared) above `art_tilecount`; a `collision` gid not 0 and not in `art_tilecount + 1` to `art_tilecount + 64` | REJECT |
| `flip-bits` | a gid with any of the top 3 bits set | REPAIR: clear the bits in the element tree (one finding per layer) |

The tileset rule needs the theme: when `x6_theme` is missing or unknown, emit only the Task 4 finding for it and skip the tileset and gid rules.

- [ ] **Step 1: Write `TmxFixture` and the failing tests** in `LevelValidatorTest`: `validFixtureIsCertified` (status CERTIFIED, no findings); one test per row above that changes the fixture to break that rule and asserts a finding with that code and result, for example `embeddedTilesetIsRejected`, `wrongCollisionFirstGidIsRejected`, `base64DataIsRejected`, `artGidAboveTilecountIsRejected`, `flipBitsAreRepaired` (also assert that the report's `map` element has the cleared gid and the status is CERTIFIED), `notXmlIsRejectedWithoutThrowing` (input `"not xml"`, status REJECTED, `map` null).

- [ ] **Step 2: Run** `./gradlew test --tests mmx.x6.level.LevelValidatorTest`. Expected: compile failure.

- [ ] **Step 3: Implement** `Catalog`, `Finding`, `ValidationReport`, `LevelValidator` (structure rules only), `DataPaths.catalogJson()`, and `assets/levels/catalog.json`.

- [ ] **Step 4: Run** `./gradlew test`. Expected: all pass.

- [ ] **Step 5: Commit** (`Add the level catalog and the structure rules of LevelValidator`).

---

### Task 4: Validator content rules (properties, checkpoints, records, fixed region)

**Files:**
- Modify: `src/main/java/mmx/x6/level/LevelValidator.java`
- Test: `src/test/java/mmx/x6/level/LevelValidatorTest.java`, `src/test/java/mmx/x6/level/IntroProfileTest.java` (new)
- Modify: `docs/gaps.md`

**Interfaces:**
- Consumes: Task 3.
- Produces: the rules below. A value check never throws: a bad int or bool text gives a REJECT finding.

| code | rule | result |
|---|---|---|
| `profile` | map property `x6_profile` missing or not the int `1` | REJECT |
| `theme` | `x6_theme` missing or not in the catalog | REJECT |
| `property` | one of `x6_music`, `x6_boss_music`, `x6_bank2`, `x6_bank5`, `x6_boss_bank2`, `x6_nightmare_effect` missing, or its value not equal to the theme's value | REJECT |
| `property-type` | a record property `kind`, `b`, `id`, `a`, `index` not an int; a checkpoint property of the wrong type (ints, `face_right` a bool) | REJECT |
| `checkpoint` | no `x6 checkpoints` layer; checkpoint numbers not unique or not exactly 0 to n-1; a checkpoint object that is not a point; a required checkpoint property missing | REJECT |
| `record-index` | the `index` values of `records` are not unique and exactly 0 to n-1 | REPAIR: renumber in document order; camera zone `record` values follow (old index to the new index of the first record that had it) |
| `entity-unknown` | a `records` or `fixed records` object name not in the catalog, or not in this theme | WARN |
| `stage-bound` | a record whose catalog entry is `stage_bound` | LOCK (one finding per object) |
| `fixed-region` | the theme has a `fixed_region`, and the map differs from the template inside it: any tile of the 5 layers in columns `x0/16` to `x1/16`, or the set of objects in `records`, `fixed records` and `camera zones` whose x is in `x0` to `x1` (compared by name, type, x, y, size and properties; not by Tiled id), or the map `width`/`height` | REJECT (`template` text from `templates`; null text gives REJECT `fixed-region` with the message "template not found") |

- [ ] **Step 1: Write the failing tests** in `LevelValidatorTest`: one per row, for example `missingProfileIsRejected`, `musicOtherThanThemeIsRejected`, `intPropertyWithLettersIsRejected` (record `kind` = `"abc"`, no exception), `boolPropertyWithYesIsRejected` (`face_right` = `"yes"`), `checkpointGapIsRejected` (numbers 0 and 2), `duplicateIndexIsRenumberedAndZoneFollows`, `unknownEntityWarns` (status PLAYABLE), `stageBoundRecordIsLocked`.
- [ ] **Step 2: Write the failing fixed-region tests** in `LevelValidatorTest`, with a fixture catalog whose `intro` theme has `fixed_region` `{x0: 32, x1: 63, template: "t.tmx"}` and a `templates` function that returns the unchanged fixture text for `t.tmx`: `changedTileInRegionIsRejected` (a gid in column 3), `changedTileOutsideRegionIsAccepted` (a gid in column 0), `missingTemplateIsRejected` (`templates` returns null).
- [ ] **Step 3: Run** `./gradlew test --tests mmx.x6.level.LevelValidatorTest`. Expected: the new tests fail.
- [ ] **Step 4: Implement** the rules.
- [ ] **Step 5: Run** `./gradlew test`. Expected: all pass.
- [ ] **Step 6: Add to `docs/gaps.md`** a line under a new heading "User levels": the fixed boss room region `x` 5744 to 6655 is a guess from the gate at x 5744, the room walls and the tile store columns up to x 6592.
- [ ] **Step 7: Commit** (`Add the content rules of LevelValidator`).

---

### Task 5: `StageLoader`, `Themes`, and the intro data in `intro.tmx`

**Files:**
- Create: `src/main/java/mmx/x6/stage/StageLoader.java`, `LoadedStage.java`, `Themes.java`
- Modify: `src/main/java/mmx/x6/stage/StageDef.java`, `Stages.java`, `src/main/java/mmx/x6/game/World.java:76`, `src/main/java/mmx/x6/X6Game.java:94-110`, `src/main/java/mmx/x6/DataPaths.java`
- Delete: `src/main/java/mmx/x6/level/IntroCheckpoints.java`
- Modify: `assets/tiled/tmx/intro.tmx`
- Modify tests: `src/test/java/mmx/x6/stage/StageRun.java:24`, `StageSmokeTest.java:31-33`, `StagesTest.java:28`, `src/test/java/mmx/x6/game/StageFlowTest.java`, `src/test/java/mmx/x6/level/TmxLoaderTest.java:150-152`, `CameraControllerTest`, `CameraZonesTest` (wherever they use `IntroCheckpoints` or load a stage)
- Test: `src/test/java/mmx/x6/stage/StageLoaderTest.java`

**Interfaces:**
- Consumes: Tasks 1 to 4.
- Produces:
  - `record LoadedStage(StageDef stage, Level level, ValidationReport report)`; `report` is null for a non-profile map; `stage` and `level` are null when the report is REJECTED.
  - `StageLoader.builtIn(DataPaths p, StageDef def) throws IOException`: reads `p.tmx(def.tmxFile())`; if the map has the property `x6_profile`, validates it (catalog from `p.catalogJson()`, templates from `p.tmx(name)`), throws `IOException` with the findings if REJECTED, and returns `def.withMapData(level)`; otherwise loads it as today with `report` null.
  - `StageLoader.file(DataPaths p, Path tmx) throws IOException`: always validates; on success the stage is `new StageDef("level", <file name>, 0, <file name>, ...)` with the map data and `Themes.factory(theme)`.
  - `StageDef.withMapData(Level level)`: a copy with `checkpoints`, `music`, `bossMusic`, `bank2`, `bank5`, `bossBank2`, `nightmareEffect` from `level.checkpoints()` and `level.properties()`.
  - `Themes.factory(String theme)`: `"intro"` gives `IntroEntityFactory::new`; unknown gives null. `Themes.tilesPng(Catalog, String theme)`.
  - `DataPaths.tilesPngNamed(String png)` = `root/tiled/png/<png>`.
  - `Stages.INTRO` = `new StageDef("intro", "INTRO STAGE", 0, "intro.tmx", List.of(), 0, 0, null, null, null, 0, IntroEntityFactory::new)`.
  - `Stages.SANDBOX`: the old intro values (the three old checkpoints inline, music 9, 21, `m094-e05`, `m094-e03`, `m123-e01`, 0, `IntroEntityFactory::new`), key `sandbox`, not in `ALL` or `BOSSES`; the `World(Level, ...)` constructor uses it. It exists only for grid worlds in tests (`World.forGrid`).

The `intro.tmx` edit (by hand, keep Tiled's format: one space indent, `type` attributes on properties):
- Add map `<properties>` right after the `<map>` start tag: `x6_profile` int 1, `x6_theme` string `intro`, `x6_music` int 9, `x6_boss_music` int 21, `x6_bank2` string `m094-e05`, `x6_bank5` string `m094-e03`, `x6_boss_bank2` string `m123-e01`, `x6_nightmare_effect` int 0. (The catalog compares the value text, so int and string types both give the same text.)
- Add `<objectgroup id="10" name="x6 checkpoints">` after the `camera zones` group, with point objects 81, 82, 83 for checkpoints 0, 1, 2 with the values of the old `IntroCheckpoints.ALL` (property names from the spec, "Checkpoints").
- Set `nextlayerid="11"` and `nextobjectid="84"`. Object id 80 is kept for the goal in Task 6.

`X6Game.startStage(StageDef def)`: `LoadedStage s = StageLoader.builtIn(paths, def)`; use `s.stage()` for the sound banks and the `StageScreen`, and `def` (not `s.stage()`) for `flow.afterStageEscape`, `flow.afterStageClear` and `Stages.BOSSES.indexOf`, because `GameFlow` compares with `== Stages.INTRO`. The tile art: `paths.tilesPngNamed(theme png)` when the map has `x6_theme`, else `paths.tilesPng(def.tmxFile())`.

- [ ] **Step 1: Write the failing tests** in `StageLoaderTest` (assets needed): `introDataComesFromTheMap`: `StageLoader.builtIn(DataPaths.assets(), Stages.INTRO).stage()` has checkpoints equal to `List.of(new Checkpoint(144, 976, 0, 944, 0, 5120, 896, 944, true, 1), new Checkpoint(2112, 1072, 1968, 944, 1968, 5120, 944, 944, true, 6), new Checkpoint(5904, 364, 5744, 224, 5744, 5744, 224, 224, true, 1))`, music 9, boss music 21, banks `m094-e05`, `m094-e03`, `m123-e01`, Nightmare Effect 0, and report status PLAYABLE. `yammarkIsNotValidated`: `builtIn(..., Stages.YAMMARK).report()` is null and `stage()` is `Stages.YAMMARK` (the same instance).
- [ ] **Step 2: Write the failing test** `src/test/java/mmx/x6/level/IntroProfileTest.java` (assets needed): `introIsPlayableWithLocksOnly` (validate `intro.tmx` with the real catalog and a `templates` function that reads `assets/tiled/tmx/<name>`; status PLAYABLE; every finding is LOCK `stage-bound`); `movedD1000IsRejected` (the intro text with the `D1000` object's `x="6162"` changed to `x="6000"`; a REJECT `fixed-region`); `editOutsideRegionIsAccepted` (one `art 0 main` gid in column 10 changed; no REJECT).
- [ ] **Step 3: Run** `./gradlew test --tests 'mmx.x6.stage.StageLoaderTest' --tests 'mmx.x6.level.IntroProfileTest'`. Expected: compile failure, then failures on the missing profile.
- [ ] **Step 4: Implement** the classes, edit `intro.tmx`, change `Stages`, `World`, `X6Game`, delete `IntroCheckpoints`, and move the tests listed above to `StageLoader.builtIn` (replace `IntroCheckpoints.ALL.get(n)` with `StageLoader.builtIn(...).stage().checkpoints().get(n)`).
- [ ] **Step 5: Run** `./gradlew test`. Expected: all pass, including `IntroProfileTest` and `StageSmokeTest` for all seven stages.
- [ ] **Step 6: Visual check.** `./gradlew run -Dmmx.stage=intro -Dmmx.hold=1 -Dmmx.jumpEvery=40 -Dmmx.shot=<scratchpad>/intro.png -Dmmx.shotAfter=60,600` and the same with `-Dmmx.checkpoint=2`; look at the PNGs: the intro starts and looks as before.
- [ ] **Step 7: Commit** (`Load the intro stage data from intro.tmx through StageLoader`).

---

### Task 6: Goals

**Files:**
- Modify: `src/main/java/mmx/x6/level/GoalDef.java`, create `GoalMode.java`; modify `TmxLoader.java`, `LevelValidator.java`
- Create: `src/main/java/mmx/x6/game/Goals.java`, `GoalMember.java`
- Modify: `src/main/java/mmx/x6/game/World.java` (field, constructor, `resetTo`), `game/enemy/Enemy.java:50-54`, `game/boss/D1000.java:499-502`, `game/boss/Maverick.java:160`, `game/enemy/BossGate.java:45`
- Modify: `assets/tiled/tmx/intro.tmx`
- Test: `src/test/java/mmx/x6/game/GoalsTest.java`, `src/test/java/mmx/x6/stage/IntroGoalTest.java`, `LevelValidatorTest`, `src/test/java/mmx/x6/level/CatalogClassesTest.java`
- Modify: `docs/custom-changes.md`, `docs/gaps.md`

**Interfaces:**
- Consumes: `RecordDef.tmxId()`, `goal()`, `openedBy()` (Task 1); `Level.goals()` (Task 2).
- Produces:
  - `enum GoalMode { ALL, ANY }` (TMX text `all`, `any`).
  - `record GoalDef(int tmxId, int x0, int y0, int x1, int y1, GoalMode mode, boolean endsStage, int parent)`, from the `goals` layer (objects with `type="Goal"`; `parent` from the `goal` property, `0` when absent).
  - `Goals(List<GoalDef> goals, List<RecordDef> records, List<RecordDef> fixedRecords)`; `boolean isEmpty()`; `void reportKill(World w, RecordDef r)`; `void reportBossEnd(World w, RecordDef r)`; `boolean done(int goalTmxId)`; `void reset()`.
  - `World.goals` (public final), built in the constructor from the level; `World.resetTo` calls `goals.reset()`.
  - `interface GoalMember {}`: implemented by `Enemy`, `D1000` and `Maverick`, the classes that report.

Behaviour:
- The members of a goal are the records (both record layers) and the goals whose `goal`/`parent` is that goal's id. A member record is done after a report for that record (`RecordDef` equality; `tmxId` makes records unique). A member goal is done when it is done.
- A goal is done when all (`ALL`) or at least one (`ANY`) of its members are done. A goal with no members is never done. When a goal becomes done, evaluate its parent; when a goal with `endsStage` becomes done, call `w.flow.clear()`.
- `reportKill` (from `Enemy.onKilled`, after the current lines): marks the record if it is a member; it never ends the stage by itself. `Enemy.despawn` does not report.
- `reportBossEnd` (from `D1000` death sub-step 2 in place of `w.flow.clear()`, and from the `Maverick` boss soul callback in place of `w.flow::clear`): if `isEmpty()`, call `w.flow.clear()` (today's behaviour); else mark the record as a kill. (My call: a boss that is no goal member in a level that has goals does not end the stage.)
- `BossGate` line 45: `canOpen(id, w) && (record.openedBy() == 0 || w.goals.done(record.openedBy()))`.
- Goal rules in `LevelValidator` (spec table "Validator rules for goals"): `goal-ref` (a `goal` or `opened_by` id that is not an object in the map: REPAIR clear + WARN); `goal-target` (the id is an object that is not a `Goal`: REJECT); `goal-member` (a record with `goal` whose catalog entity is not `defeatable`: REJECT); `goal-listener` (a record with `opened_by` whose entity is not `openable`: REJECT); `goal-cycle` (a cycle in the goal parents: REJECT); `goal-props` (`mode` not `all`/`any`, `ends_stage` not a bool: REJECT).
- `intro.tmx`: add `<objectgroup id="11" name="goals">` with object `id="80" name="D-1000 room" type="Goal" x="6064" y="208" width="384" height="224"`, properties `mode` (string, `propertytype="GoalMode"`, `all`) and `ends_stage` (bool, `true`); add `<property name="goal" type="object" value="80"/>` to the `D1000` object (id 57); set `nextlayerid="12"`. The template of the fixed region is `intro.tmx` itself, so the intro still validates.

- [ ] **Step 1: Write the failing unit tests** in `GoalsTest` (plain records, a `World.forGrid` world): `allNeedsEveryMember`, `anyNeedsOneMember`, `chainEndsStageThroughParent` (goal A `ALL` with record r, goal B `endsStage` with member A; a kill of r gives `flow.state() == CLEAR`), `despawnDoesNotCount` (a member enemy despawns, spawns again, is killed: done; before the kill: not done), `bossEndWithoutGoalsClearsStage`, `bossEndThatIsNotAMemberDoesNotClear`, `resetClearsState`.
- [ ] **Step 2: Write the failing stage tests** in `IntroGoalTest` (assets needed, `StageRun.at(Stages.INTRO, 2)`): `d1000DeathClearsThroughGoal` (drive the fight as the existing D-1000 tests do, or set the boss state to its death; assert `CLEAR` and `world.goals.done(80)`); `deathAfterGoalResetsAndClearsOnce` (after `done(80)`, call `world.flow.restart(2)`; assert `done(80)` is false; finish the boss again; assert `CLEAR` and `world.lockCount == 1`).
- [ ] **Step 3: Write the failing validator tests** in `LevelValidatorTest`, one per goal rule code, and `CatalogClassesTest.catalogAgreesWithClasses` (for each `intro` entity in `assets/levels/catalog.json` that `IntroEntityFactory` creates: `defeatable` is true exactly when the entity is a `GoalMember`; `openable` is true exactly when it is a `BossGate`; also `Themes.factory` is not null for every catalog theme key).
- [ ] **Step 4: Run** `./gradlew test`. Expected: the new tests fail.
- [ ] **Step 5: Implement** the classes, the hooks, the validator rules and the `intro.tmx` change.
- [ ] **Step 6: Run** `./gradlew test`. Expected: all pass, including `StageSmokeTest` and the existing D-1000 and Maverick tests.
- [ ] **Step 7: Docs.** `docs/custom-changes.md`: a "Level goals" entry (the `goals` layer, `goal`, `opened_by`, `mode`, `ends_stage`; X6 itself ends stages only in object code, and the intro ends on `game.exit_req = 0x40` after the High Max scene; here the D-1000's goal ends it; a boss that is not a goal member in a level with goals does not end the stage). `docs/gaps.md` ("User levels"): the goal rectangle (6064, 208, 384, 224) is a guess from the room walls at x 6064 and x 6448 and the camera limits.
- [ ] **Step 8: Commit** (`Add level goals and end the intro through the D-1000 goal`).

---

### Task 7: Desktop `-Dmmx.level=<file>`

**Files:**
- Modify: `src/main/java/mmx/x6/GameFlow.java`, `src/main/java/mmx/x6/X6Game.java`, `src/main/java/mmx/x6/screen/StageScreen.java` (the CLEAR key branch at about line 147)
- Create: `src/main/java/mmx/x6/screen/LevelErrorScreen.java`
- Test: `src/test/java/mmx/x6/stage/StageLoaderTest.java`, `src/test/java/mmx/x6/X6FlowTest.java`
- Modify: `README.md` (debug options table), `CLAUDE.md` ("Build, run, test"), `docs/custom-changes.md`

**Interfaces:**
- Consumes: `StageLoader.file` (Task 5).
- Produces:
  - `GameFlow(String stageKey, String front, String levelPath)`; `String levelPath()`; `first()` returns `Screen.STAGE` when `levelPath` is set. The old 2-argument constructor stays.
  - `X6Game`: when `System.getProperty("mmx.level")` is set, `StageLoader.file(paths, Path.of(it))`; REJECTED shows `LevelErrorScreen`; else start the stage as in `startStage`, with the theme's tile PNG. After STAGE CLEAR, the next key press calls `Gdx.app.exit()`; `Progress` is a new object and is not kept.
  - `LevelErrorScreen(List<Finding> findings, Runnable close)`: draws "LEVEL REJECTED" and one line per REJECT finding (`code: message`) with `BitmapFont`; any key calls `close` (`Gdx.app.exit()`); reads keys only when `AutoRun.active()` is false; calls `AutoRun.drawBorder` after its last draw; saves its shots and quits through `ShotClock` and `Shots`.

- [ ] **Step 1: Write the failing tests**: `StageLoaderTest.fileLevelInOtherFolderUsesThemeTiles` (copy `intro.tmx` text to a temp dir as `my-level.tmx`; `StageLoader.file(DataPaths.assets(), path)`; status PLAYABLE; `stage().tmxFile()` is `my-level.tmx`; `Themes.tilesPng(catalog, "intro")` is `intro-tiles.png`; the checkpoints equal the intro checkpoints). `StageLoaderTest.rejectedFileHasNoStage` (a copy without the `collision` layer: status REJECTED, `stage()` null). `X6FlowTest.levelPathStartsStage` (`new GameFlow(null, null, "/x.tmx").first() == Screen.STAGE`).
- [ ] **Step 2: Run** `./gradlew test`. Expected: the new tests fail.
- [ ] **Step 3: Implement** the flow, `X6Game`, `LevelErrorScreen` and the key branch.
- [ ] **Step 4: Run** `./gradlew test`. Expected: all pass.
- [ ] **Step 5: Visual checks** (automated runs, PNGs in the scratchpad): copy `intro.tmx` to `<scratchpad>/levels/my-level.tmx` and move one `MetaWheelF` record 64 px left (outside the fixed region); run `./gradlew run -Dmmx.level=<scratchpad>/levels/my-level.tmx -Dmmx.hold=1 -Dmmx.jumpEvery=40 -Dmmx.shot=<scratchpad>/level.png -Dmmx.shotAfter=60,600`; the level plays. Delete the `collision` layer in a second copy and run it with `-Dmmx.shot=<scratchpad>/rejected.png -Dmmx.shotAfter=30`; the PNG shows "LEVEL REJECTED" and the `layer-missing` line.
- [ ] **Step 6: Docs.** `README.md`: a row `-Dmmx.level=PATH` ("play a level file in the X6 TMX profile; it is checked first"); a short "User levels" section (copy `intro.tmx`, edit it in Tiled outside the boss room region, play it with `-Dmmx.level`). `CLAUDE.md`: the option in "Build, run, test", and `StageLoader`, `LevelValidator`, `Goals` in the code layout table. `docs/custom-changes.md`: `-Dmmx.level` and its end (a key press after STAGE CLEAR closes the game; no progress).
- [ ] **Step 7: Commit** (`Play level files from -Dmmx.level with the validator first`).
