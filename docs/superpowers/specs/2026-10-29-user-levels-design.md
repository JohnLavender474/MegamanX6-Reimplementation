# Design: user-made X6 levels (intro-only MVP)

Date: 2026-10-29. Repo: `~/IdeaProjects/mmx-x6`.

The status of this work, what is left, and the decisions made after this spec are in
`docs/superpowers/status/user-levels.md`.

## Intent

Users make their own Mega Man X6 levels and play them on this engine, on the desktop and in a web
browser. The existing X6 levels are templates to start from. The full game in the browser is a
nice-to-have, not a goal.

The first scope is the intro stage only (`assets/tiled/tmx/intro.tmx`). The intro validates the
whole path (file format, loader, validator, goals, web player, Tiled kit). The other six stages
follow after the intro works.

The editor is Tiled (desktop), with a project file and a JavaScript extension. A custom web editor
comes later, on the same file format and the same validator. The web is a player only in this
design: the user uploads a level file and plays it.

## Goals

1. A strict "X6 TMX profile": the rules that make a TMX file a valid X6 level.
2. A loader that reads the profile on the desktop and in the browser (no `javax.xml`, no
   `java.nio.file` in the shared code).
3. One validator, `LevelValidator`, that the desktop game, the web player and the Tiled kit all
   use. No level loads unless it passes.
4. A goal system in the level data: goal areas, goal members and objects that a goal opens.
5. The intro's stage data (checkpoints, music, sound banks) moves from `Stages.java` into
   `intro.tmx`.
6. A desktop option to play any profile file: `-Dmmx.level=<path>`.
7. A web player (TeaVM, Wasm target) that plays the intro template and uploaded intro-theme levels.
8. A Tiled kit: a `.tiled-project`, an extension `x6.js` and a validator jar.

## Non-goals

- The custom web editor (a later project, on this profile).
- Stages other than the intro, and enemies from other stages in a level.
- More than one theme (tileset) per level. User art packs.
- The front end (logo, opening movie, title, menu) and the stage select screen in the web build.
- Controllers in the web build (keyboard only). Safari and all WebKit browsers (iOS, iPadOS).
- Public hosting. The web build runs on localhost only.
- A backend: accounts, uploads to a server, level search or ratings.
- Scripts, event sheets or free logic graphs in levels.
- The goal conditions `clear_area` and `reach`, `opened_by` on camera zones, and a `Barrier` object.
  The design leaves room for them (see "Later").

## Anti-goals

These make the work a failure even if it runs:

- Web movement that differs from the desktop movement. The user wants an exact match.
- A level that passes the validator and then crashes the game, or whose layers, objects or
  properties the engine reads wrongly. The validator is the only gate, and it must be complete for
  the layers, objects and properties that the profile allows.
- A validator that judges the design of a level. Spawn placement, checkpoint placement, whether
  the level can be finished and similar questions are the user's responsibility, not the
  validator's.
- Changed behavior in the six other stages, or in the intro as it plays today from `make run`.
- A second level format next to TMX. There is one format for built-in and user levels.
- Data that only lives in a place that Tiled drops on save (custom XML elements or attributes).

## Constraints

- Java 17, LibGDX 1.14.2. Logic at 60 updates per second. The X6 values from the docs stay
  unchanged.
- The project rules in `CLAUDE.md` apply: guesses go in `docs/gaps.md`, the user's design choices go
  in `docs/custom-changes.md`, no code comments except `TODO`s in the project format, automated
  runs for every window that Claude opens.
- Tiled drops unknown XML on save. All X6 data must be custom properties, object layers or objects.
- Tiled 1.10.2 saved 17 maps and Tiled 1.12.2 saved `intro.tmx`. The loader accepts both.
- All 90 `<data>` blocks in the 18 maps use `encoding="csv"` with no compression.
- Since Tiled 1.9 the TMX `type` attribute of an object is its Tiled class. The X6 maps store the
  X6 object type (`main`, `stage`, `misc`, `logic`) in that attribute.

## Decisions by the user

These come from the conversation of 2026-10-29. The research behind them is in the session
reports; the main facts are repeated where they matter.

1. Main goal: users edit and play their own levels; the existing levels are templates.
2. Editor: Tiled first (desktop), with a project file and an extension. The web editor comes after
   the Tiled path works and the profile is stable. At that point, decide again whether users with
   only a browser are still a goal.
3. Exchange format: TMX only. TMJ is not read or written in this design.
4. Tile art on load comes from the built-in theme packs, selected by the map property `x6_theme`.
   The `.tsx` paths in the file are not used to find the art.
5. Record values: the plain int properties `kind`, `b`, `id` and `a` stay the true values. A named
   variant in Tiled is only a view of them.
6. Broken files: each problem gets one result (reject, repair, warn, lock). A level has one status
   (Certified, Playable, Rejected), computed again at each load. The game plays Playable levels.
7. Templates: the built-in maps load as Playable. Stage-bound objects in them are locked.
8. Screens (256 px blocks) are computed from the tiles and are not stored.
9. The validator runs in Tiled as a Java jar, on save and from an action.
10. Web backend: TeaVM through gdx-teavm, Wasm target. No downgrade of the Java code. Reason: GWT
    (all versions) and the TeaVM JS target run Java `float` as a JS double, so physics drifts from
    the desktop. The TeaVM Wasm target maps `float` to Wasm `f32`.
11. Browsers: any browser except Safari and WebKit. The page blocks them with an error.
12. No controllers on the web for now. This is a `TODO` item.
13. Hosting: localhost only for now. The user checks separately whether public hosting of the
    assets is advisable.
14. First scope: the intro stage only.
15. Goals: Design B (a `Goal` area with fixed condition types, and links from each object to its
    goal), with these sub-decisions:
    - Members point to their goal. A goal does not keep a list of members.
    - All goal state resets after a death in the MVP.
    - Stages with no `goals` layer keep the boss end in code. A user level with no goal that ends
      the stage is valid (see decision 17).
    - A boss counts as defeated at the end of its death sequence (where `flow.clear()` is called
      now), so the explosion and the boss soul still play. A generic enemy counts at `onKilled`.
    - `any` (OR) is in the MVP.
    - A member outside its goal area is valid (see decision 17).
    - The goal system is a custom change, recorded in `docs/custom-changes.md`.
16. The boss room can move. Users may move the D-1000 room or build a new layout for the last
    section. `D1000` and the intro gates then take their positions from the level data, not from
    fixed coordinates, and the kit gets camera zone tools. This change is done and validated in
    isolation (see "Relocatable boss room"): copies of `intro.tmx` redo the last section in
    different ways, and only that set of rules and tools is merged after the boss fight works in
    every copy.
17. The validator checks engine compatibility only: are the layers, objects and properties set up
    in a way that the engine supports? It does not check spawns, checkpoint placement, whether the
    level can be finished, or other design questions. Those are the user's responsibility.

## The X6 TMX profile (version 1)

### Map

- Orthogonal, 16 px tiles, `infinite="0"`, CSV data, no compression.
- Width and height in tiles are whole multiples of 16 (one screen is 16 by 16 tiles, 256 px).
  The built-in maps already follow this rule.
- Map properties:

| property | type | meaning |
|---|---|---|
| `x6_profile` | int | profile version, `1` |
| `x6_theme` | string | theme key, `intro` in the MVP; selects the tile art and the entity factory |
| `x6_music` | int | the stage music track (intro: `9`) |
| `x6_boss_music` | int | the boss music track (intro: `21`) |
| `x6_bank2` | string | sound bank 2 folder (intro: `m094-e05`) |
| `x6_bank5` | string | sound bank 5 folder (intro: `m094-e03`) |
| `x6_boss_bank2` | string | boss sound bank 2 folder (intro: `m123-e01`) |
| `x6_nightmare_effect` | int | the Nightmare Effect value (intro: `0`) |

(My call.) In the MVP, the catalog gives the allowed values of the music, bank and Nightmare
Effect properties per theme. For the `intro` theme these are only the intro values above, because
the web build ships only those assets. The validator rejects other values.

### Tilesets

- Exactly two tilesets: the theme's art tileset at `firstgid="1"`, then `collision`.
- (My call.) Both are external tilesets (`source=`). The loader and the validator match them to the
  theme pack by the basename of `source` (intro: `intro-tiles.tsx` and `collision.tsx`) and by
  `firstgid`. They never read the `.tsx` content, so the folder of the file does not matter and the
  web player needs only the uploaded `.tmx` text. The `firstgid` of `collision` must be the art
  tileset's `tilecount` + 1, with the `tilecount` from the theme pack. Embedded tilesets are
  rejected.
- Tile flip and rotation bits are not allowed. The repair clears them, with a warning. (Today the
  loader clears them silently with `& 0x1FFFFFFF`.)

### Tile layers

The five layers as now, by exact name: `art 2 background`, `art 1 decoration`, `art 0 main`,
`art 0 front`, `collision`. The layer properties `x6_mode`, `x6_offset_x` and `x6_offset_y` stay
as they are. Group layers that hold these layers are not allowed.

### Object layers

| layer | contents |
|---|---|
| `records` | X6 records, as now. The object `name` selects the entity class. Int properties `kind`, `b`, `id`, `a`, `index`. New optional properties: `goal` (type `object`) and `opened_by` (type `object`) |
| `fixed records` | as now |
| `camera zones` | as now |
| `x6 checkpoints` | new: one point object per checkpoint (see below) |
| `goals` | new: `Goal` rectangles (see "Goals") |

Any other object layer (for example the user's `TODO` layer) is allowed and ignored.

### Checkpoints

The `Checkpoint` data from `level/IntroCheckpoints.java` moves to the `x6 checkpoints` layer:

- One point object per checkpoint, at the player start (`playerX`, `playerY`).
- Int property `checkpoint`: the checkpoint number. `0` is the stage start. The numbers are
  unique and have no gaps. A `CheckpointMarker` record still selects an entry by `id & 0x7F`
  (`Spawner.updateMarker`).
- Properties: `cam_x`, `cam_y`, `min_x`, `max_x`, `min_y`, `max_y` (int), `face_right` (bool),
  `beam_thru` (int), and optional `layer1_x`, `layer1_y`, `layer2_offset_x`, `layer2_offset_y`
  (int; absent means `null` in `Checkpoint`).

The intro has three checkpoints today. Their values move unchanged.

### The X6 `type` attribute and Tiled classes (my call)

The records keep the X6 object type in the TMX `type` attribute, as the built-in maps have it. The
`.tiled-project` therefore does not define per-entity classes for records. The named-variant view
and the `goal`/`opened_by` properties on records are free custom properties that `x6.js` adds and
keeps in sync. The new `Goal` objects on the `goals` layer use the Tiled class `Goal`. A later
profile version can move the X6 type to a property if the web editor needs per-entity classes.

## Loader

- `TmxLoader` is ported from DOM (`javax.xml`, `org.w3c.dom`) to LibGDX `XmlReader`. Its input is
  the TMX text and a way to read the referenced `.tsx` texts, not a `Path`.
- File access goes through a small `AssetSource` interface (text, exists, handle). The desktop
  game uses `Gdx.files.internal`; the JVM tests use a `java.nio` source; the web uses
  `Gdx.files.internal` on the TeaVM backend. The other `java.nio.file` users in the shared code
  move to it as the web port needs them.
- `RecordDef` gets the Tiled object id (`tmxId`), `goal` and `openedBy` (`0` when absent). A
  second constructor keeps the current signature, so the existing call sites do not change.
- `Level` gets the map properties, the checkpoints and the goals.
- The X6 data for the intro comes from the map. For the six other stages, `StageDef` keeps its
  Java data until each stage moves. (A `TODO` marks the two sources, pointing at this spec.)
- The tile art comes from the theme pack named by `x6_theme`, not from the TMX file name
  (today `X6Game.startStage` calls `paths.tilesPng(stage.tmxFile())`). The theme pack for `intro`
  is the current intro tile sheet.
- Golden test: for all 18 maps, the new loader gives the same `Level` as the old loader (apart
  from the new fields). The old loader is removed after the test passes.

## Validator

`level/LevelValidator` takes the TMX text (and the catalog) and returns a list of findings and a
status. Its only question is engine compatibility (decision 17): can the engine read and run
these layers, objects and properties as the profile defines them? A rule belongs in the validator
only if breaking it makes the engine crash, read data wrongly, or silently ignore data. Each
finding has a result:

1. **Reject**: the file is not usable. Examples: it does not parse; a required layer is missing or
   renamed; the theme is unknown; the art tileset is not at `firstgid="1"` or its `tilecount` does
   not match; there is a third tileset; the map is infinite; a gid is out of range; there is no
   checkpoint `0` (the engine needs a start); a property has the wrong type or a value outside the
   catalog; a goal rule below gives reject.
2. **Repair**: a fix that keeps the meaning, with a report. Examples: flip bits are cleared;
   duplicate or missing record `index` values are renumbered and the camera zone `record` links
   follow; a stale `goal` or `opened_by` id is cleared.
3. **Warn**: the level plays, but the engine ignores some data. Examples: a record whose name is
   not in the catalog (no entity is made); two equal records at the same position (`Spawner` keeps
   records in `HashSet`s, so only one spawns). A collision cell that differs from the
   auto-collision value is not a finding: the auto-collision is a kit tool, not a rule.
4. **Lock**: an object that the profile cannot model is kept unchanged. A later web editor shows it
   locked. In Tiled, a lock is a warning that names the object.

Status: **Rejected** if any reject; else **Playable** if any warn or lock; else **Certified**. The
status is never stored in the file.

(My call.) In the MVP the validator runs on the intro and on user levels (`-Dmmx.level`, web
upload). The six other built-in stages load as today, with no validator, until each stage moves
to the profile. A `TODO` at the bypass points at this spec.

The game (desktop and web) runs the validator before it loads a level. It applies the repairs in
memory. It refuses a Rejected level with an error screen that lists the findings. The desktop game
does not write the repaired file back. The Tiled kit can.

### The catalog

A JSON file (my call: `assets/levels/catalog.json`) holds the rule data:

- per theme: the art tileset name and `tilecount`, the tile PNG, and the gid-to-collision table;
- per entity name: generic or stage-bound, the themes it belongs to, whether it can be a goal
  member (defeatable) or a goal listener (openable), and its named variants (`kind`, `b`, `id`,
  `a` presets, names from `docs/re/findings/enemies/*.md`).

A build step generates the gid-to-collision table from the maps. In all 18 maps each `art 0 main`
gid has exactly one collision value. Gids that never occur on `art 0 main` have no known value; the
auto-collision leaves those cells as the user painted them, with a warning.

Intro catalog for the MVP:

- generic: `MetaWheelF`, `MetaDridler`, `Junkroid`, `SabreBlock`, `HoverGunner`;
- stage-bound (locked in templates): `BossGate`, `D1000`, `IntroWreckA`, `IntroWreckB`,
  `IntroHunters`, `TalkZone`, `PaletteCycle`, `CheckpointMarker`, `CameraZone` records as they
  are. (`IntroEntityFactory` makes only the generic five, `BossGate` and `D1000` today; the others
  stay inert as now.)

A unit test checks that the catalog's "defeatable" and "openable" entries agree with the entity
classes.

Until step 8 is merged, the boss room is a fixed region: the validator rejects a level in which
`D1000`, a `BossGate`, a `CameraZone` record or a camera zone in the boss room region differs from
the template, or in which a tile in that region differs (the region covers the room, the hallway
before it and the tile store columns that `D1000.copyColumn` reads). Step 8 replaces this rule.

## Goals (Design B, MVP part)

### Data

- A `Goal` is a rectangle object of class `Goal` on the `goals` layer.
- Properties: `mode` (string enum `GoalMode`: `all` or `any`), `ends_stage` (bool), `goal`
  (optional `object`: this goal is a member of another goal).
- A record that is a goal member has the property `goal` (type `object`) with the id of its `Goal`.
- A record that a goal opens has the property `opened_by` (type `object`) with the id of its
  `Goal`. In the MVP only `BossGate` can listen.

Example for the intro (the values of the rectangle are a guess from the room walls at x 6064 and
x 6448 and the camera limits; they go in `docs/gaps.md`):

```xml
<objectgroup id="10" name="goals">
 <object id="80" name="D-1000 room" type="Goal" x="6064" y="208" width="384" height="224">
  <properties>
   <property name="mode" type="string" propertytype="GoalMode" value="all"/>
   <property name="ends_stage" type="bool" value="true"/>
  </properties>
 </object>
</objectgroup>
```

The `D1000` record (object id 57) gets `<property name="goal" type="object" value="80"/>`.

### Runtime

- `level/GoalDef` (from the `goals` layer) and `Level.goals()`.
- `game/Goals`, owned by `World`, keeps per goal `done` and per member record `defeated`. The
  state is keyed by record (`tmxId`), never by `Entity`, because enemies spawn again.
- Inputs:
  - `Enemy.onKilled` reports a kill (`Enemy.despawn` does not).
  - `D1000` reports at its death sub-step 2, where it calls `w.flow.clear()` now
    (`game/boss/D1000.java:501`).
  - `Maverick` reports in the boss soul callback, where it calls `w.flow::clear` now
    (`game/boss/Maverick.java:160`).
- When all (`all`) or one (`any`) of a goal's members are done, the goal is done. Then its parent
  goal is evaluated. If `ends_stage`, `w.flow.clear()` runs once.
- `BossGate.canOpen` also returns false while its `opened_by` goal is not done.
- When the level has no goals, the boss classes call `w.flow.clear()` as today. The six other
  stages do not change.
- `World.resetTo` resets `Goals`, as it resets `flag26` and `flag28`.

### Validator rules for goals

| rule | result |
|---|---|
| `goal` or `opened_by` points to an id that does not exist | repair (clear) and warn |
| the target is not a `Goal` | reject |
| a member is not defeatable in the catalog | reject |
| the goal-to-parent chain has a cycle (the evaluation would not end) | reject |
| a listener is not openable in the catalog | reject |
| `mode` is not `all` or `any`, or `ends_stage` is not a bool | reject |

Not checked (decision 17): goals with no members, members that never spawn on Normal, members
outside their goal rectangle, a level with no goal that ends the stage.

## Desktop

- `-Dmmx.level=<path to a .tmx>` plays that file, from checkpoint `0` (or `-Dmmx.checkpoint`). It
  skips the front end, as `-Dmmx.stage` does. The level runs through the validator first.
- (My call.) After STAGE CLEAR in a `-Dmmx.level` run, the next key press closes the game. A user
  level never changes `Progress` and never opens the stage select screen. So "X6: Play" in the kit
  ends when the level ends.
- `make run STAGE=intro` keeps working and now reads the intro data from the map.
- The current play of the intro does not change. The existing tests stay green.

## Web player

### Build

- The project splits into modules: `core` (all game code), `lwjgl3` (the desktop launcher,
  screenshots, the `java.nio` and AWT users, `make dist`) and `teavm` (the web launcher). The
  tests stay on the JVM.
- gdx-teavm for LibGDX 1.14.2, Wasm target. The builder checks in the first step that the build
  uses the Wasm backend that maps `float` to `f32`.
- (My call.) If gdx-teavm cannot give a working Wasm build with `f32` floats for this LibGDX
  version, the builder stops step 6, reports the cause, and does not change to the JS target or
  downgrade the code. Steps 1 to 5 and the desktop part of step 7 go on. The user then decides.
- The intro-only slice needs these blockers removed from the code that the intro reaches (the
  blocker list is from the 2026-10-29 audit):
  - `java.nio.file` asset access (through `AssetSource`), `Gdx.files.absolute`,
    `new FileHandle(File)`, `Files.exists` as logic (an asset index);
  - `TmxLoader` DOM (see "Loader");
  - `ImageIO` in `anim/SpriteBounds` (compute the bounds offline into `animations.json`);
  - `String.format`, `Integer.getInteger`, `Boolean.getBoolean`, `System.getProperty` (a
    `LaunchOptions` value: `-D` on the desktop, the URL query on the web);
  - sounds from in-memory WAV bytes (`audio/WavRate`): on the web, play the WAV as it is with pitch
    `rate / 44100f` (all 716 WAVs are 44100 Hz mono 16-bit);
  - sounds that load at the first play: create all clips of the level's banks before the level
    starts;
  - per-pixel colour keying with `Pixmap` (`render/Assets`): transparent PNGs written offline by
    `tools/import_assets.py`;
  - textures over 4096 px that the intro uses: repacked offline;
  - `Gdx.app.exit()` paths and screenshots: off on the web;
  - the F1 debug key: moved on the web, because the browser takes F1.
- The web assets for the intro: the intro tiles, the sprites of the intro objects and of X, the
  intro sound banks, the two music tracks. No movies.

### Page

My call for the MVP:

- The page first checks the browser. If `new Audio().canPlayType('audio/ogg; codecs="vorbis"')`
  returns `""`, or Wasm-GC is missing, it shows an error that names Chrome, Edge or Firefox, and
  does not start the game.
- Then the page shows two choices: "Play the intro template" and "Load a level file"
  (`<input type=file>`, `.tmx`). The file text goes to the game through a TeaVM JS bridge.
- The game runs the validator. A Rejected level shows the findings on an error screen. A Playable
  or Certified level shows its status, then starts on the "Ready to play? Press START" screen. The
  key press there also unlocks the browser audio.
- After STAGE CLEAR or a quit key, the game goes back to the two choices.
- No level library, no IndexedDB and no share URLs in the MVP.

### Movement match

Exit test of the web port: a fixed input script plays the intro from checkpoint `0` on the desktop
and in the web build. Both log X's position and step each update. The logs must be equal for the
whole run. If they are not equal, the builder stops and reports the first difference. The user then
decides. Open points that this test answers: `java.util.Random` sequences on TeaVM, and
`Math.sin`/`atan2` results in the browser.

## Tiled kit

- `tools/tiled/x6.tiled-project` (my call for the path): `propertyTypes` for the `Goal` class and
  the `GoalMode` enum, generated from the catalog; the extension folder.
- `tools/tiled/extensions/x6.js`:
  - "X6: Check level": runs the validator jar with `Process` and shows the findings;
  - the same check after each save (`tiled.assetSaved`);
  - "X6: Auto-collision": fills the `collision` layer from `art 0 main` with the catalog table;
  - "X6: Add selection to goal": sets `goal` on the selected records;
  - "X6: Play": starts the desktop game with `-Dmmx.level=<current file>`;
  - before each save (`tiled.assetAboutToBeSaved`): renumber `index`, sync the named variant to
    `kind`/`b`/`id`/`a`.
- `make kit` builds the validator as a command-line jar (`x6-check <file.tmx>`; exit code 0 for
  Certified or Playable, 1 for Rejected; the findings on standard output).

## Order of work

Each step ends with its exit test. The steps after 2 depend on the steps before them.

1. **Loader port.** `TmxLoader` on `XmlReader` with `AssetSource`; `RecordDef` ids; the golden test
   on all 18 maps. `make test` green.
2. **Profile, catalog, validator.** The profile rules, `catalog.json` for the intro, the
   gid-to-collision table, `LevelValidator` with unit tests for each rule. `intro.tmx` validates
   as Playable.
3. **Intro data into the map.** Map properties and the `x6 checkpoints` layer in `intro.tmx`; the
   loader reads them; `Stages.INTRO` and `IntroCheckpoints` no longer hold that data. A test shows
   that the intro checkpoints and music are the same as before. `StageSmokeTest` green.
4. **Goals.** The `goals` layer and the links in `intro.tmx`; `Goals` in `World`; the report calls
   in `Enemy`, `D1000`, `Maverick`; `BossGate` `opened_by`. Tests: the D-1000 death gives STAGE
   CLEAR through the goal; a death and restart after the goal is done; the other stages unchanged;
   validator tests for each goal rule.
5. **Desktop `-Dmmx.level`.** A copy of the intro, saved under another name and changed in Tiled,
   plays from the option. An automated run with screenshots shows it.
6. **Web port of the intro.** The module split, the blocker fixes above, the TeaVM Wasm build, the
   page. Exit test: the movement match, and the intro playable to STAGE CLEAR in Chrome and Firefox
   on localhost.
7. **Tiled kit.** The project file, `x6.js` and `make kit`. Exit test: a new level made from the
   intro template in Tiled, checked by the kit, played on the desktop with "X6: Play" and in the
   browser by upload.

8. **Relocatable boss room (in isolation).** On its own branch, after step 4. Not merged until
   its exit test passes. See the next section.

## Relocatable boss room

### What is fixed today

- `D1000` (`game/boss/D1000.java`) uses absolute coordinates: `START_X = 6288`, `START_Y = 160`,
  `STAND_Y = 304`, `CROUCH_Y`, `FLOOR_Y = 416`, `ROOM_LEFT = 6136`, `ROOM_RIGHT = 6392`,
  `LEFT_WALL_INNER = 6080`, `RIGHT_WALL_INNER = 6448`, `WALL_TOP_ROW = 208 / 16`, `WALL_ROWS = 13`.
  It ignores its record position (`D1000.java:48-50`).
- The wall dents copy tile columns from a tile store to the right of the room:
  `copyColumn(w, 6448, 6576)`, `copyColumn(w, 6064, 6528)`, `copyColumn(w, 6064, 6512)`,
  `copyColumn(w, 6448, 6560)` (`D1000.java:355-363`).
- The intro gates use `BossGate.limits(id, gx, gy)`: gate `id` 1 sets `max x = gx + 80`; gate `id`
  2 sets x to `gx` and y to `224` (absolute) (`BossGate.java:129-130`).

### The change

- The room has an anchor. (My call: the anchor is the top-left corner of the `Goal` rectangle
  that `D1000` belongs to.) Every `D1000` constant above becomes an offset from the anchor, with
  values that give exactly today's coordinates for the anchor of the intro (x 6064, y 208).
- The tile store columns move with the room: the source columns of `copyColumn` are offsets from
  the anchor too. A level that moves the room must also move those columns (the kit's room tool
  copies them).
- The gates take their camera limits from the level data, not from the absolute value `224`. (My
  call: the gate limits become offsets from the gate position, with values that give today's
  limits in the intro. Other gate ids keep their current rules, because other stages use them.)
- The validator rules for the boss room replace the fixed-region rule. They check only what the
  engine needs: the room and its tile store columns fit in the map (so `copyColumn` cannot read or
  write outside it); the tile store columns match the template; `D1000` is a member of a `Goal`
  (its anchor); at most one `D1000` per goal. The room layout, the walls and the gates are the
  user's design.
- The kit gets camera zone tools: "X6: Room" draws the camera zones for a selected rectangle (the
  limits are the rectangle minus the 320 by 240 view, `-1` where the room is open), and "X6: Move
  boss room" moves the room tiles, the tile store columns, `D1000`, the gates, the goal and the
  camera zones by one offset.

### Validation in isolation

- Make at least two copies of `intro.tmx` as test maps (my call: under
  `src/test/resources/levels/`). Each copy redoes the last section in a different way, for
  example: the room moved to another x and y; a different hallway and gate layout before the room;
  the room higher or lower than the hallway.
- Exit test: in each copy and in the original intro, the boss fight plays to STAGE CLEAR in a
  `StageRun` test and in an automated run with screenshots of the fight. The original intro plays
  exactly as before (the same positions per update as a run before the change).
- Merge only the rules, the `D1000`/`BossGate` changes and the kit tools of this step, after the
  exit test passes. The test maps stay as test data.

## Tests

- JVM tests as now (`make test`). New: the loader golden test; validator unit tests (one per rule,
  with small TMX strings); goal tests with `StageRun` on the intro; a catalog/class agreement test.
- `StageSmokeTest` stays green for all seven stages after each step.
- Screenshots from automated runs (`-Dmmx.shot`) for visual checks, as `CLAUDE.md` says.
- The web movement-match test (step 6).

## Docs

- `docs/custom-changes.md`: the goal system (X6 ends stages only in object code; the intro ends on
  `game.exit_req = 0x40` after the High Max scene); `-Dmmx.level`; the web player's page and its
  browser block; no controllers on the web.
- `docs/gaps.md`: the intro goal rectangle values; auto-collision for gids with no known value; the
  web build's sound pitch in place of the WAV header rate; anything else that the work guesses.
- `README.md`: how to make a level in Tiled with the kit, how to play it on the desktop and in the
  browser.
- `CLAUDE.md`: the new modules, `make kit`, the web build command, `-Dmmx.level`.

## Left to the builder

- Class and package names other than those given here, and the exact module layout.
- The asset index format and how the web build preloads the intro assets.
- The JS bridge details for the file upload.
- How the auto-collision handles cells under empty art (keep the painted value is the default).
- The text and layout of the error screen and the browser-block message.

## Later

In this order, after the intro MVP works:

1. The other six stages: their data into their maps, their catalogs, sound home banks per entity
   type, the boss bank and music by boss type, one entity registry in place of the per-stage
   factories.
2. Goal conditions `clear_area` (all records in the rectangle that pass the difficulty filter, each
   counted once) and `reach` (X enters the rectangle).
3. `opened_by` on camera zones (the camera zone tools of step 8 come first), and a `Barrier` object that clears its cells when its goal is done.
4. A collectable goal item. Several goals that end the stage. Goals that stay done after a
   checkpoint.
5. The custom web editor on this profile and validator.
6. Hosting beyond localhost, after the user's decision on the assets.
7. Controllers on the web.
