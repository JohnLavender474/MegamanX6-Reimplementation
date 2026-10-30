# User levels: status, plan and decisions

Last update: 2026-10-29. This is the entry point for the user-levels work. A fresh agent reads
this file first, then the spec and the plan that it names.

## The goal

Users make their own Mega Man X6 levels and play them on this engine, on the desktop and in a web
browser. The existing X6 levels are templates. The full game in the browser is a nice-to-have, not
a goal. The editor is Tiled (desktop) with an X6 kit. A custom web editor comes later, on the same
file format and the same validator.

## Documents

| document | what |
|---|---|
| `docs/superpowers/specs/2026-10-29-user-levels-design.md` | the approved design (the binding authority): the X6 TMX profile, the loader, the validator, the goals, the desktop option, the web player, the Tiled kit, the relocatable boss room, the order of work |
| `docs/superpowers/plans/2026-10-29-user-levels-1-level-pipeline.md` | plan 1 (done): spec steps 1 to 5 |
| `docs/custom-changes.md`, "Level goals" and "User levels" | the user's design choices that this work added |
| `docs/gaps.md`, "User levels" | the guesses that this work added |
| this file | the status, what is left, and every decision with its reason |

## Status

| spec step | what | state |
|---|---|---|
| 1 | `TmxLoader` on LibGDX `XmlReader` | done (plan 1, Task 1) |
| 2 | profile, catalog, `LevelValidator` | done (plan 1, Tasks 2 to 4, and the final fix wave) |
| 3 | the intro data in `intro.tmx` | done (plan 1, Task 5) |
| 4 | goals | done (plan 1, Task 6) |
| 5 | desktop `-Dmmx.level=<file>` | done (plan 1, Task 7) |
| 6 | web port of the intro (TeaVM, Wasm) | not started: plan 2 |
| 7 | Tiled kit | not started: plan 3 |
| 8 | relocatable boss room (in isolation) | not started: plan 4 |
| later | the other six stages, more goal types, web editor, hosting, controllers on the web | not started |

Plan 1 is on the branch `user-levels` (14 commits from `main` at `393e376`, head `70b17f9` at the
time of writing). It is not merged and not pushed. The user decides how to integrate it. All 419
tests pass (`./gradlew test`).

One exit test of spec step 5 is still open: a copy of `intro.tmx` saved once in Tiled (not by a
script), then played with `-Dmmx.level`. Tiled can rewrite a file on save (property order, types,
sizes), and this test shows that the validator, and the fixed boss room region in particular,
accepts a real Tiled save. The user does this test.

## What plan 1 built

Paths are under `src/main/java/mmx/x6/`.

- `level/TmxLoader`: parses TMX text with `XmlReader` (`load(Path)`, `parse(String, String)`,
  `fromElement(XmlReader.Element, String)`). It never reads `.tsx` files: the collision tileset is
  the one whose `source` basename is `collision.tsx`. Numbers are trimmed.
- `level/RecordDef`: has `tmxId` (the Tiled object id), `goal` and `openedBy`. `tmxId` makes every
  record unique, also two records with equal fields at one position.
- `level/Level`: has `properties()` (map properties), `checkpoints()` (from the `x6 checkpoints`
  layer) and `goals()` (from the `goals` layer).
- `assets/levels/catalog.json` and `level/Catalog`: per theme the art tileset basename, its
  `tilecount`, the tile PNG, the allowed map property values and the fixed region; per entity the
  themes, `stage_bound`, `defeatable` and `openable`. Only the `intro` theme exists.
- `level/LevelValidator` (public entry `validate(String tmxText, Catalog, Function<String,String>
  templates)`) and the package-private `level/ContentRules`: findings with the results REJECT,
  REPAIR, WARN, LOCK, and the status REJECTED, PLAYABLE or CERTIFIED. Repairs are made on the
  parsed element tree, and the loader reads that tree. `validate` never throws.
- `stage/StageLoader`: `builtIn(DataPaths, StageDef)` validates a map that has `x6_profile` and
  builds the stage data from the map (`StageDef.withMapData`); a map without it (the six other
  stages) loads as before, without the validator. `file(DataPaths, Path)` always validates and
  turns read and load failures into the rejects `read` and `load`. `LoadedStage` carries the
  stage, the level, the report and the tile PNG name. `stage/Themes` maps a theme to its entity
  factory.
- `stage/Stages`: `INTRO` has no data of its own now. `SANDBOX` keeps the old intro values only for
  the grid worlds of the tests (`World.forGrid`). `level/IntroCheckpoints` is deleted.
- `game/Goals`, `level/GoalDef`, `level/GoalMode`, `game/GoalMember`: the goal runtime. Hooks:
  `Enemy.onKilled` and `SabreBlock.onKilled` call `reportKill`; `D1000` (death sub-step 2) and
  `Maverick` (boss soul) call `reportBossEnd`, which keeps the old `flow.clear()` when the level
  has no goals; `BossGate` opens only when its `opened_by` goal is done; `World.resetTo` resets the
  goal state.
- `assets/tiled/tmx/intro.tmx`: map properties (`x6_profile` 1, `x6_theme` `intro`, `x6_music` 9,
  `x6_boss_music` 21, `x6_bank2` `m094-e05`, `x6_bank5` `m094-e03`, `x6_boss_bank2` `m123-e01`,
  `x6_nightmare_effect` 0), the `x6 checkpoints` layer (objects 81 to 83), the `goals` layer with
  goal 80 (`D-1000 room`, rectangle 6064, 208, 384, 224, `all`, `ends_stage` true), and
  `goal` = 80 on the `D1000` object (id 57).
- Desktop: `-Dmmx.level=<path>` (`GameFlow`, `X6Game.startLevelFile`, `screen/LevelErrorScreen`).
  `-Dmmx.checkpoint` out of range is ignored with a line on stderr.

### The validator rules (engine compatibility only)

- REJECT: `parse`, `map`, `group-layer`, `tileset`, `layer-missing`, `layer`, `layer-duplicate`,
  `gid-range`, `profile`, `theme`, `property`, `property-type`, `checkpoint` (also a
  `CheckpointMarker` whose `id & 0x7F` has no checkpoint), `camera-zone` (no size above 0),
  `fixed-region`, `goal-target` (not a `Goal` on the `goals` layer), `goal-member`,
  `goal-listener`, `goal-cycle`, `goal-props`, and from `StageLoader.file`: `read`, `load`.
- REPAIR: `flip-bits`, `record-index` (camera zone `record` links follow), `goal-ref` (a stale
  link is cleared; also a WARN).
- WARN: `entity-unknown`, `ignored` (an extra tile layer, an image layer, an object on `goals` that
  is not a `Goal`), `goal-ref`.
- LOCK: `stage-bound` (one per stage-bound record).
- The intro validates as PLAYABLE with LOCK findings only.

## Decisions

### By the user

The spec, "Decisions by the user", lists decisions 1 to 17 with their reasons. In short:

1. Users edit and play their own levels; the existing levels are templates.
2. Tiled is the editor first; the web editor comes after the Tiled path works and the profile is
   stable. Then decide again whether users with only a browser are a goal.
3. TMX is the only exchange format.
4. Tile art comes from the built-in theme packs, selected by `x6_theme`.
5. The plain ints `kind`, `b`, `id`, `a` are the true record values; a named variant in Tiled is a
   view of them.
6. Broken files: reject, repair, warn, lock; status computed at each load; the game plays Playable
   levels.
7. The built-in maps are templates that load as Playable; stage-bound objects are locked.
8. Screens (256 px blocks) are computed from the tiles, not stored.
9. In Tiled, the validator runs as a Java jar on save and from an action.
10. Web backend: TeaVM (gdx-teavm), Wasm target, with no downgrade of the Java code.
11. Any browser except Safari and WebKit (all iOS and iPadOS browsers); the page blocks them.
12. No controllers on the web for now (a `TODO` item).
13. Localhost only for now; the user checks whether public hosting of the assets is advisable.
14. The first scope is the intro stage only.
15. Goals: Design B (a `Goal` area, members that point to their goal, `opened_by` listeners,
    `all`/`any`, `ends_stage`, goals that chain); state resets after a death; stages without a
    `goals` layer keep the boss end in code.
16. The boss room can move. This work is done and validated in isolation (plan 4), with test copies
    of `intro.tmx` that redo the last section in different ways.
17. The validator checks engine compatibility only. Spawns, checkpoint placement, whether a level
    can be finished and other design questions are the user's responsibility.

Also from the user: no effort or time estimates in any output (global `CLAUDE.md`, "Estimates").

### Calls made in the spec (marked "(My call.)" there)

- Tilesets are external; they are matched to the theme pack by `source` basename and `firstgid`,
  never by reading the `.tsx`. Embedded tilesets are rejected.
- The `intro` theme allows only the intro music, bank and Nightmare Effect values, because the web
  build ships only those assets.
- The six other stages load without the validator until each moves to the profile.
- After STAGE CLEAR in a `-Dmmx.level` run, a key press closes the game; `Progress` is not kept.
- If TeaVM Wasm with `f32` floats does not work, step 6 stops and reports; no JS target, no
  downgrade.
- The X6 object type stays in the TMX `type` attribute; `Goal` objects use the Tiled class `Goal`.
- The catalog is `assets/levels/catalog.json`; the Tiled kit goes in `tools/tiled/`.
- The web page checks the browser (OGG and Wasm-GC), then offers "Play the intro template" and
  "Load a level file"; no level library in the MVP.
- The room anchor of plan 4 is the top-left corner of the D-1000's `Goal`; the intro gate limits
  become offsets from the gate.
- The test maps of plan 4 go in `src/test/resources/levels/`.

### Rulings during plan 1

Each with what it costs if it is wrong.

1. The fixed boss room region is x 5744 to 6655, all rows (`docs/gaps.md`). It covers the hallway
   gate, the room and the tile store columns of `D1000.copyColumn`. Cost: a wrong bound accepts an
   edit that breaks the fight, or rejects an edit that is safe.
2. The fixed-region compare ignores Tiled ids, the record `index` and the camera zone `record`,
   because the `record-index` repair renumbers them after an edit outside the region. Cost: a
   reorder of records inside the region is not detected.
3. The fixed-region compare runs before the `goal-ref` repair, so a deleted goal 80 does not give
   a misleading region reject. Cost: such a level has no goals, so the boss ends it through the old
   code path.
4. `MetaDridler` is not defeatable in the catalog, because its `onKilled` only sets `hp = 1`. Its
   X6 behaviour stays. Cost: users cannot use it as a goal member.
5. A boss that is not a goal member, in a level that has goals, does not end the stage. Cost: a
   user who forgets the link has a level with no end.
6. The content rules are in the package-private `ContentRules`; bools are valid only as `true` or
   `false`; a type rule reads the value text. Cost: a file move.
7. The spec's warning for "two equal records at the same position" is dropped: `tmxId` makes every
   record unique, and both spawn.
8. Parked: an extra tile layer that is malformed, or an object with a non-numeric `x`, `y` or `id`,
   passes the validator and then gets the reject `load` on the error screen (no crash).

### Differences between plan 1 and the spec

- `AssetSource` (spec, "Loader") is not built. The loader no longer reads `.tsx` files, so plan 1
  did not need it. Plan 2 builds it for the web.
- The gid-to-collision table and the named variants (spec, "The catalog") are not in
  `catalog.json`. Only the Tiled kit uses them now, so plan 3 adds them.
- The fixed region also compares the map `width` and `height`, so a user level made from the intro
  cannot change its size until plan 4.
- Both intro `BossGate`s are inside the fixed region, so `opened_by` cannot be used in a user level
  until plan 4.

## What is left

Write each plan with the `superpowers:writing-plans` skill from the spec, then run it (the user
chose subagent-driven development for plan 1). Plans 2, 3 and 4 depend on plan 1 only, not on each
other.

### Plan 2: web port of the intro (spec step 6, "Web player")

Facts from the research of 2026-10-29:

- `gdx-backend-gwt` 1.14.2 uses GWT 2.11 (Java 11 syntax only). GWT 2.12/2.13 give Java 17 but are
  untested with LibGDX. GWT and the TeaVM JS target run Java `float` as a JS double, so physics
  drifts from the desktop. The TeaVM Wasm backend maps `float` to `f32`. This is why decision 10
  chose TeaVM Wasm.
- gdx-teavm 1.6.1 goes with LibGDX 1.14.2 (TeaVM 0.15/0.16). Check in the first step that the
  build uses a Wasm backend with `f32`. Wasm-GC needs Chrome or Edge 119+, or Firefox 120+.
- A move of the code down to Java 11 would touch 84 files (19 records, 115 switch statements with
  `->` rules, 32 switch expressions, 14 `instanceof` patterns, text blocks, 6 `copyOf` calls).
  It is ruled out.
- The blockers in the code that the intro reaches (spec, "Web player", "Build", has the full list):
  `java.nio.file` asset access (build `AssetSource`), `Gdx.files.absolute`, `new FileHandle(File)`,
  `Files.exists` as logic (an asset index), `ImageIO` in `anim/SpriteBounds` (compute offline),
  `String.format`, `Integer.getInteger`/`Boolean.getBoolean`/`System.getProperty` (a
  `LaunchOptions` value; URL query on the web), sounds from in-memory WAV bytes (`audio/WavRate`;
  on the web play the WAV as is with pitch `rate / 44100f`; all 716 WAVs are 44100 Hz mono 16-bit),
  sounds that load at the first play (preload the level's banks), per-pixel colour keying in
  `render/Assets` (write transparent PNGs offline in `tools/import_assets.py`), textures over 4096
  px, `Gdx.app.exit()` and screenshots (off on the web), the F1 key.
- The module split: `core`, `lwjgl3`, `teavm`; the tests stay on the JVM.
- The page: the browser check, two choices, the file upload through a TeaVM JS bridge, the
  "Press START" screen as the audio unlock (keyboard only).
- Exit test: the movement match. One input script plays the intro from checkpoint 0 on the desktop
  and in the web build; both log X's position and step each update; the logs must be equal. If not,
  stop and report the first difference.
- Localhost: the server must support HTTP Range requests (for the music seek). Python's
  `http.server` does not.

### Plan 3: Tiled kit (spec step 7, "Tiled kit")

- `tools/tiled/x6.tiled-project` (custom types `Goal` and `GoalMode`), `tools/tiled/extensions/x6.js`
  ("X6: Check level", the check after each save, "X6: Auto-collision", "X6: Add selection to goal",
  "X6: Play", and before each save: renumber `index`, sync the named variant), `make kit` (the
  validator as a jar `x6-check <file.tmx>`, exit code 1 for REJECTED).
- Add to `catalog.json`: the gid-to-collision table per theme, generated from the maps (each
  `art 0 main` gid has exactly one collision value in all 18 maps), and the named variants per
  entity (names from `docs/re/findings/enemies/*.md`).
- Tiled facts: Tiled drops unknown XML on save, so all data must be properties, layers or objects;
  custom types live in the project file; an `object` property draws an arrow and keeps its link on
  copy; the scripting API has `registerAction`, `assetAboutToBeSaved`, `assetSaved` and `Process`
  (to run the jar).

### Plan 4: relocatable boss room (spec step 8, "Relocatable boss room")

- Done on its own branch, merged only after its exit test passes.
- `D1000` constants become offsets from the room anchor; the `copyColumn` source columns move with
  the room; the intro gate limits become offsets from the gate (gate `id` 2 now uses the absolute
  camera y 224); the validator's boss room rules replace the fixed region; the kit gets "X6: Room"
  and "X6: Move boss room".
- Exit test: at least two copies of `intro.tmx` with a different last section; the fight plays to
  STAGE CLEAR in each, in a `StageRun` test and in an automated run with screenshots; the original
  intro gives the same positions per update as before.

### Later (spec, "Later")

The other six stages (their data into their maps, catalogs, sound home banks per entity type, the
boss bank and music by boss type, one entity registry), the goal conditions `clear_area` and
`reach`, `opened_by` on camera zones and a `Barrier` object, a collectable goal item, the custom
web editor, hosting beyond localhost, controllers on the web.

## Rules for the next agent

- The project rules in `CLAUDE.md` apply (ASD-STE100, no code comments except `TODO`s, automated
  runs for every window, commit only when the user asks, the `Co-Authored-By` trailer).
- Do not give effort or time estimates. Give counts of files, lines and call sites.
- The spec is the binding authority. Where this file and the spec differ, this file records a later
  decision ("Rulings during plan 1", "Differences between plan 1 and the spec").
- Do not add validator rules about level design (decision 17).
- The six other stages must play exactly as before; `StageSmokeTest` must stay green.
