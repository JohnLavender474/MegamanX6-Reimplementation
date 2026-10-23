# X6 Intro Stage Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** A Java LibGDX desktop game that loads the X6 intro stage from `~/mmx-data/x6/` and lets
X play it from the spawn to the death of the D-1000.

**Architecture:** One Gradle project. Pure-Java data and logic classes (TMX loader, collision,
camera, player, enemies) that do not need a GL context, so JUnit can test them. A thin LibGDX
layer (screen, renderers, audio) on top. All game logic uses X6 coordinates (y down, px, one
update = 1/60 s) and the doc values unchanged; only the renderer converts.

**Tech Stack:** Java 17 bytecode (runs on the installed JDK 25), Gradle 9.1.0 wrapper, LibGDX
1.14.2 (core + LWJGL3 backend), JUnit 5.11.

**Spec:** `docs/superpowers/specs/2026-10-22-intro-stage-design.md`. The facts come from
`~/IdeaProjects/mmx-demake-tools/docs/` (below: "the docs"). Read the doc section that a task
names before you write its code.

## Global Constraints

- No game data in the repo. Everything loads at run time from the data root:
  `-Dmmx.data=<path>`, else the first program argument, else `~/mmx-data/x6`.
- No code copied from `~/IdeaProjects/mmx-reference/`.
- Logic runs at a fixed 60 updates per second. Speeds are px per update, times are updates.
- Logic coordinates: X6 coordinates, y down. Rendering uses an `OrthographicCamera` with
  `setToOrtho(true, 320, 240)` and texture regions flipped in y.
- View 320 × 240 game px, drawn to a `FrameBuffer`, shown ×3 (window 960 × 720), `Nearest` filter,
  window not resizable.
- Difficulty 1 (Normal): records of kind 0 and 1 spawn; kind 2 does not.
- X is in the Falcon Armor (form 1): `x-falcon` sprites, hit class 4, player damage halved.
- Out of scope: text, talk zones, the Hunters' scene, the wrecks, palette cycles, Zero, High Max,
  item drops, special weapons, lives.
- A value that the docs do not give gets a best guess and a `TODO` comment that names the doc
  section or the spec's "Known gaps" item. No other comments (user rule).
- Tests that need the real data call `Assumptions.assumeTrue(DataPaths.defaultRootExists())`, so
  the build passes on a machine with no data.

## Review Focus

1. **Bad data root.** The game is started with a wrong or missing data root. Expected: one clear
   error line that names the path that was tried, and exit code 1. Test in Task 1.
2. **Death and respawn reset.** X dies after breaking sabre blocks, passing gate 18 or during the
   boss. Expected: all records reset (broken blocks and gates come back closed), enemies cleared,
   camera and limits from the checkpoint, stage music again, the boss entry runs again. Test in
   Task 8.
3. **Long frames.** The window loses focus or a frame takes 1 s. Expected: at most 4 updates per
   rendered frame; no burst of motion through walls. Test in Task 4.
4. **Restart key mid-sequence.** R is pressed during a gate pass, the WARNING or the boss death.
   Expected: the same clean reset as a death, to checkpoint 0, with no stale lock, no stale music.
   Test in Task 10.
5. **Missing sound.** A `PlaySound(bank, id)` maps to no WAV (an id past the list, a VAB of another
   bank, a `same_as` copy). Expected: the sound is skipped, one log line per (bank, id), no crash.
   Test in Task 5.

---

## File structure

```
build.gradle, settings.gradle, gradle.properties, gradlew, gradlew.bat, gradle/wrapper/*, .gitignore
src/main/java/mmx/x6/
  DesktopLauncher.java            window, data root, start
  X6Game.java                     Game: loads assets, opens StageScreen
  DataPaths.java                  data root and the file paths under it
  input/Buttons.java              X6 button bits; held / pressed / released from the keyboard
  level/TmxLoader.java            intro.tmx -> Level
  level/Level.java, TileLayer.java, RecordDef.java, CameraZoneDef.java, CollisionGrid.java
  level/Checkpoint.java, IntroCheckpoints.java
  physics/TileRules.java          collision values: slopes, floors, walls, ceilings
  physics/Body.java               position + terrain box
  physics/TerrainMover.java       moves a Body against a CollisionGrid; returns contact flags
  physics/Rect.java               axis-aligned box helper (overlap, flip by facing)
  anim/AnimDef.java, AnimSet.java, Animator.java    animations.json and frame ticking
  game/StepClock.java             fixed-step accumulator
  game/CameraController.java      layer 0 camera, limits, easing, BG layer positions
  game/CameraZones.java
  game/World.java                 player, entities, spawner, camera, solids, game state
  game/Entity.java                base class of every object
  game/Spawner.java
  game/EntityFactory.java         RecordDef -> Entity
  game/DamageTables.java          the five tables from docs/engine.md
  game/Damage.java                player damage scale, weapon hit on an object
  game/StageFlow.java             beam-in, death, respawn, restart, STAGE CLEAR
  game/player/Player.java         the step machine
  game/player/PlayerConst.java    physics constants
  game/player/BusterShot.java, ChargeFlash.java, SabreSlash.java
  game/enemy/Junkroid.java, JunkroidShot.java, HoverGunner.java, HoverGunnerShot.java,
             MetaWheelF.java, MetaDridler.java, SabreBlock.java, BossGate.java
  game/boss/D1000.java, D1000Core.java, D1000Shot.java, D1000Bomb.java, FirePillar.java,
            BossWarning.java
  game/fx/Explosion.java, Debris.java
  render/Assets.java              textures, AnimSet loading, sprite cut-outs
  render/LevelRenderer.java       tile layers
  render/EntityRenderer.java      draws an Animator frame at an origin with flip
  render/Hud.java                 HP bars, WARNING, STAGE CLEAR text
  render/DebugOverlay.java        F1 view
  audio/SoundBanks.java, MusicPlayer.java, Sfx.java
  screen/StageScreen.java         the loop: input, fixed updates, render
src/test/java/mmx/x6/...          one test class per logic class
```

Shared types used by many tasks (defined where marked):

- `Rect(float x, float y, float w, float h)` record, x/y = top-left (Task 3). The doc box
  notation "(dx, dy, w, h)" means `Rect(originX + dx, originY + dy, w, h)`; mirror dx for a left
  facing when the doc box is not symmetric: `dx' = -dx - w`.
- `Body`: `float x, y` (origin), `float boxOffX, boxOffY, boxHalfW, boxHalfH` (the doc "centre
  (cx, cy), half size w × h"), `boolean hasBox` (Task 3).
- `Entity` (Task 7): `float x, y, prevX, prevY; boolean faceRight; int hp; int contactDamage;
  int hitClass; DamageTables.Table damageTable; RecordDef record; boolean dead;`
  `abstract void update(World w)`, `Rect hurtBox()`, `Rect contactBox()` (null = none),
  `Animator anim()`, `String spriteSet()`, `int drawPriority()`, `void onWeaponHit(World w,
  int weaponId, int result)`, `boolean blocksShots()`.
- `World` (Task 7) owns: `Level level`, `CollisionGrid grid`, `Player player`,
  `List<Entity> entities`, `Spawner spawner`, `CameraController camera`, `CameraZones zones`,
  `Sfx sfx`, `MusicPlayer music`, `Buttons buttons`, `StageFlow flow`, `Random rng`,
  `int checkpoint`, `int lockCount`.
  Methods: `void add(Entity e)`, `void clearEnemies()`, `List<Rect> solids()`,
  `boolean inSpawnBand(float x, float y)`, `boolean offScreen(float x, float y, float mx, float my)`.

---

### Task 1: Project scaffold and data root

**Files:**
- Create: `settings.gradle`, `build.gradle`, `gradle.properties`, `.gitignore`, `gradlew`,
  `gradlew.bat`, `gradle/wrapper/gradle-wrapper.jar`, `gradle/wrapper/gradle-wrapper.properties`
- Create: `src/main/java/mmx/x6/DataPaths.java`, `DesktopLauncher.java`, `X6Game.java`
- Test: `src/test/java/mmx/x6/DataPathsTest.java`

**Interfaces:**
- Produces: `DataPaths(Path root)`; `static DataPaths resolve(String[] args)`;
  `static boolean defaultRootExists()`; `void validate() throws DataRootException` (a checked
  exception whose message names the root and the first missing file);
  `Path tmx()` = `root/tmx/m094-intro/intro.tmx`; `Path tilesPng()`; `Path playerDir(String name)`
  = `root/sprites/players/<name>`; `Path enemyDir(String name)` = `root/sprites/enemies/<name>`;
  `Path bankSheet(String state, int bank)` = `root/sprites/banks/intro/<state>/bank-<bank>.png`;
  `Path soundBank(String folder)` = `root/sounds/<folder>`; `Path songsJson()` =
  `root/music/bgm/songs.json`; `Path musicRecord(String file)` = `root/music/bgm/<file>`.

- [ ] **Step 1: Copy the wrapper and write the build**

Copy `gradlew`, `gradlew.bat` and `gradle/wrapper/gradle-wrapper.jar` from
`~/IdeaProjects/Megaman-Maverick`; set `distributionUrl` to
`https\://services.gradle.org/distributions/gradle-9.1.0-bin.zip`. `build.gradle`: plugins
`java` and `application`; `tasks.withType(JavaCompile).configureEach { options.release = 17 }`
(no toolchain); repositories `mavenCentral()`; dependencies
`com.badlogicgames.gdx:gdx:1.14.2`, `gdx-backend-lwjgl3:1.14.2`,
`gdx-platform:1.14.2:natives-desktop`, test `org.junit.jupiter:junit-jupiter:5.11.4` and
`junit-platform-launcher`; `test { useJUnitPlatform() }`; `application { mainClass =
'mmx.x6.DesktopLauncher' }`; `run { workingDir = rootDir; jvmArgs '-XstartOnFirstThread' only on
macOS }`. `.gitignore`: `build/`, `.gradle/`, `*.png`, `*.wav`, `*.tmx`, `*.state.gz`, `*.bin`,
`.idea/`, `*.iml`.

- [ ] **Step 2: Write the failing tests**

```java
@Test void argumentWinsOverDefault() {
    DataPaths p = DataPaths.resolve(new String[]{"/tmp/x6data"});
    assertEquals(Path.of("/tmp/x6data/tmx/m094-intro/intro.tmx"), p.tmx());
}
@Test void systemPropertyWinsOverArgument() {
    System.setProperty("mmx.data", "/opt/d");
    try { assertEquals(Path.of("/opt/d"), DataPaths.resolve(new String[]{"/tmp/a"}).root()); }
    finally { System.clearProperty("mmx.data"); }
}
@Test void defaultIsHomeMmxData() {
    assertEquals(Path.of(System.getProperty("user.home"), "mmx-data", "x6"),
                 DataPaths.resolve(new String[0]).root());
}
@Test void validateNamesTheMissingPath(@TempDir Path dir) {
    DataRootException e = assertThrows(DataRootException.class, () -> new DataPaths(dir).validate());
    assertTrue(e.getMessage().contains(dir.toString()));
    assertTrue(e.getMessage().contains("intro.tmx"));
}
```

- [ ] **Step 3: Run the tests to see them fail**

Run: `./gradlew test --tests mmx.x6.DataPathsTest`
Expected: FAIL (compile errors: `DataPaths` not defined).

- [ ] **Step 4: Implement `DataPaths`, `DesktopLauncher` and `X6Game`**

`validate()` checks `tmx()`, `tilesPng()`, `playerDir("x-falcon")`, `songsJson()`.
`DesktopLauncher.main`: resolve, `validate()`; on `DataRootException` print
`mmx-x6: <message>` to stderr and `System.exit(1)`. Else `Lwjgl3ApplicationConfiguration`:
title `MMX6 Intro`, `setWindowedMode(960, 720)`, `setResizable(false)`, `useVsync(true)`,
`setForegroundFPS(60)`. `X6Game extends Game`; for now it clears the screen black.

- [ ] **Step 5: Run the tests and the app**

Run: `./gradlew test --tests mmx.x6.DataPathsTest` → PASS.
Run: `./gradlew run --args=/nonexistent` → prints `mmx-x6: ... /nonexistent ...`, exit code 1.
Run: `./gradlew run` → a black 960 × 720 window titled `MMX6 Intro`.

- [ ] **Step 6: Commit**

```bash
git add -A && git commit -m "Add the Gradle project and the data root"
```

---

### Task 2: TMX loader

**Files:**
- Create: `src/main/java/mmx/x6/level/{TmxLoader,Level,TileLayer,RecordDef,CameraZoneDef,CollisionGrid,Checkpoint,IntroCheckpoints}.java`
- Test: `src/test/java/mmx/x6/level/TmxLoaderTest.java`, `CollisionGridTest.java`

**Interfaces:**
- Produces:
  - `record RecordDef(int index, String name, String type, int kind, int b, int id, int a, int x, int y)`
  - `record CameraZoneDef(int record, int zoneId, int x0, int x1, int y0, int y1, int minX, int maxX, int minY, int maxY, boolean textureSwap)`;
    `x1 = x + width - 1`, `y1 = y + height - 1` (both ends included); `-1` = keep.
  - `record TileLayer(String name, int width, int height, int[] gids, int x6Mode, int x6OffsetX, int x6OffsetY)`
    with `int gid(int cx, int cy)` (0 outside; flip bits masked with `0x1FFFFFFF`).
  - `final class CollisionGrid`: `CollisionGrid(int w, int h, int[] values)`, `int width()`,
    `int height()`, `int raw(int cx, int cy)` (0 outside the map), `int at(float px, float py)`
    (cell of the point; 59 reads as 56; 0 outside), `void set(int cx, int cy, int v)`,
    `CollisionGrid copy()`.
  - `record Level(int width, int height, TileLayer art2, TileLayer art1, TileLayer art0Main, TileLayer art0Front, CollisionGrid collision, List<RecordDef> records, List<CameraZoneDef> cameraZones)`
  - `final class TmxLoader { static Level load(Path tmx) throws IOException }`
  - `record Checkpoint(int playerX, int playerY, int camX, int camY, int minX, int maxX, int minY, int maxY, boolean faceRight, int beamThru)`
  - `final class IntroCheckpoints { static final List<Checkpoint> ALL; static final int L1_OX = 0, L1_OY = 560, L2_OX = 0, L2_OY = 640; }`

- [ ] **Step 1: Write the failing tests**

```java
@Test void loadsTheIntroMap() throws Exception {
    assumeTrue(DataPaths.defaultRootExists());
    Level l = TmxLoader.load(DataPaths.resolve(new String[0]).tmx());
    assertEquals(416, l.width()); assertEquals(96, l.height());
    assertEquals(57, l.records().size());
    assertEquals(13, l.cameraZones().size());
    assertEquals(1, l.art1().x6Mode()); assertEquals(560, l.art1().x6OffsetY());
    assertEquals(3, l.art2().x6Mode()); assertEquals(640, l.art2().x6OffsetY());
}
@Test void collisionValuesAreGidMinusFirstGid() throws Exception {
    assumeTrue(DataPaths.defaultRootExists());
    Level l = TmxLoader.load(DataPaths.resolve(new String[0]).tmx());
    Map<Integer, Integer> count = new TreeMap<>();
    for (int y = 0; y < 96; y++) for (int x = 0; x < 416; x++) count.merge(l.collision().raw(x, y), 1, Integer::sum);
    assertEquals(33603, count.get(0)); assertEquals(6, count.get(1)); assertEquals(6, count.get(4));
    assertEquals(7, count.get(5)); assertEquals(7, count.get(8)); assertEquals(52, count.get(16));
    assertEquals(7, count.get(32)); assertEquals(1, count.get(33)); assertEquals(2, count.get(34));
    assertEquals(5200, count.get(56)); assertEquals(961, count.get(57));
    assertEquals(20, count.get(59)); assertEquals(21, count.get(63)); assertEquals(17, count.get(45));
}
@Test void cameraZone0() throws Exception {
    assumeTrue(DataPaths.defaultRootExists());
    CameraZoneDef z = zone(0);
    assertEquals(List.of(2272, 2303, 1040, 1135, -1, -1, 944, -1),
                 List.of(z.x0(), z.x1(), z.y0(), z.y1(), z.minX(), z.maxX(), z.minY(), z.maxY()));
}
@Test void cameraZone1IsTheTextureSwapZone() { /* zoneId 1: minX 1968, textureSwap true, x 2128–2159 */ }
@Test void cameraZone9() { /* x 5152–5168, minX 3136, minY 704, maxY 704 */ }
@Test void sabreBlockRecord40() { /* name "SabreBlock", kind 4, b 2, x 2016, y 984 */ }
@Test void d1000Record() { /* name "D1000", kind 0, b 48, x 6162, y 346 */ }
@Test void checkpoints() {
    assertEquals(new Checkpoint(144, 976, 0, 944, 0, 5120, 896, 944, true, 1), IntroCheckpoints.ALL.get(0));
    assertEquals(new Checkpoint(2112, 1072, 1968, 944, 1968, 5120, 944, 944, true, 6), IntroCheckpoints.ALL.get(1));
    assertEquals(new Checkpoint(5904, 364, 5744, 224, 5744, 5744, 224, 224, true, 1), IntroCheckpoints.ALL.get(2));
}
```

`CollisionGridTest`: `at(-1, 0) == 0`, `at(16*416, 0) == 0`, a 59 cell reads 56 through `at` and
59 through `raw`, `set` then `copy` keeps the value and the copy is independent.

- [ ] **Step 2: Run the tests to see them fail**

Run: `./gradlew test --tests 'mmx.x6.level.*'` → FAIL (classes not defined).

- [ ] **Step 3: Implement the loader**

Parse with `javax.xml.parsers.DocumentBuilder` (no GL needed). Layers by name: `art 2 background`,
`art 1 decoration`, `art 0 main`, `art 0 front`, `collision`. Collision value = gid − `firstgid`
of the tileset named `collision` (3233), 0 for gid 0. Records from object group `records`:
properties `index`, `kind`, `b`, `id`, `a`; the object `name` and `type` attributes. Camera zones
from object group `camera zones`: properties `record`, `zone_id`, `min_x`, `max_x`, `min_y`,
`max_y`, `texture_swap` (≠ 0 → true). Ignore `fixed records`.

- [ ] **Step 4: Run the tests** → PASS.

- [ ] **Step 5: Commit** `git commit -m "Load the intro TMX: layers, collision, records, camera zones"`

---

### Task 3: Tile rules and terrain movement

Source: `docs/engine.md` "Tile collision", `catalog/collision.json`, `docs/players/air.md`
"Which walls".

**Files:**
- Create: `src/main/java/mmx/x6/physics/{TileRules,Body,TerrainMover,Rect}.java`
- Test: `src/test/java/mmx/x6/physics/{TileRulesTest,TerrainMoverTest}.java`

**Interfaces:**
- Produces:
  - `TileRules`: `static boolean isSlope(int v)` (1–12, 17–28); `static int slopeSurface(int v, int xInCell)`;
    `static boolean isFlatFloor(int v)` (33, 34, 56, 57, 58, 60, 62, 63);
    `static boolean isOneWay(int v)` (33, 34); `static boolean isWall(int v)` and
    `static boolean isCeiling(int v)` (56, 57, 58, 60, 62, 63);
    `static boolean allowsWallCling(int v)` (56, 58); `static boolean isSpike(int v)` (62, 63);
    `static boolean isObjectSolid(int v)` (3–6, 10, 11, 16, 56–58, 60, 62, 63);
    `static boolean isLadder(int v)` (32); `static boolean isLadderTop(int v)` (33);
    `static boolean isBar(int v)` (37–45).
  - `Rect` with `boolean overlaps(Rect o)`, `float right()`, `float bottom()`,
    `static Rect ofDoc(float ox, float oy, float dx, float dy, float w, float h, boolean faceRight)`
    (mirrors dx when not facing right).
  - `Body` (fields in "Shared types"), `float left()`, `float right()`, `float top()`,
    `float bottom()` of the terrain box, `Rect box()`.
  - `TerrainMover(CollisionGrid grid)`;
    `int move(Body b, float dx, float dy, boolean grounded, List<Rect> solids)` returns the flags
    `RIGHT = 1, LEFT = 2, CEILING = 4, FLOOR = 8` and sets `boolean lastWallCling`,
    `boolean lastSpike`, `boolean lastOnSlope`;
    `boolean blocked(Body b, int side, List<Rect> solids)` tests 1 px past the side without a move.

- [ ] **Step 1: Write the failing tests**

```java
@Test void slopeProfiles() {
    assertEquals(15, TileRules.slopeSurface(1, 0));  assertEquals(12, TileRules.slopeSurface(1, 15));
    assertEquals(3,  TileRules.slopeSurface(4, 0));  assertEquals(0,  TileRules.slopeSurface(4, 15));
    assertEquals(0,  TileRules.slopeSurface(5, 0));  assertEquals(3,  TileRules.slopeSurface(5, 15));
    assertEquals(12, TileRules.slopeSurface(8, 0));  assertEquals(15, TileRules.slopeSurface(8, 15));
    assertEquals(15, TileRules.slopeSurface(9, 0));  assertEquals(8,  TileRules.slopeSurface(9, 15));
    assertEquals(7,  TileRules.slopeSurface(10, 0)); assertEquals(0,  TileRules.slopeSurface(10, 15));
    assertEquals(0,  TileRules.slopeSurface(11, 0)); assertEquals(7,  TileRules.slopeSurface(11, 15));
    assertEquals(TileRules.slopeSurface(3, 9), TileRules.slopeSurface(19, 9));
}
@Test void slopesAreNeverWalls() { for (int v = 1; v <= 12; v++) assertFalse(TileRules.isWall(v)); }
@Test void wallCling() { assertTrue(TileRules.allowsWallCling(56)); assertFalse(TileRules.allowsWallCling(57)); }
```

`TerrainMoverTest` builds small grids in code (a helper `grid(String... rows)` where each char is a
value: `.` 0, `#` 56, `n` 57, `/` slope rising: cells `1 2 3 4`, `\` falling `5 6 7 8`, `u` 16,
`T` 33, `^` 63) and a body with centre (0, 2), half 11 × 19 (X's box):

```java
@Test void landsOnFlatFloor()          // falling 4 px/update onto '#' row at y 64: bottom() == 64, FLOOR set
@Test void stopsAtWallAndFlagsCling()  // moving right into '#': right() == wall x, RIGHT set, lastWallCling true
@Test void noClingOn57()               // same into 'n': RIGHT set, lastWallCling false
@Test void walksUpAFourCellSlope()     // grounded, dx = 2 for 40 updates over "/ / / /" cells: bottom() follows slopeSurface at the centre x within 1 px every update, FLOOR every update, never RIGHT
@Test void staysOnSlopeGoingDown()     // grounded, dx = 2 down "\ \ \ \": FLOOR every update (the 16-px-lower test), bottom() never above the surface
@Test void cellUnderSlopeUsesCellAbove() // centre over 'u' under '/': snaps to the slope surface in the cell above
@Test void slopeIsNotACeiling()        // rising into a slope cell from below: no CEILING
@Test void oneWayFloorFromAboveOnly()  // 'T': falling from above lands; rising from below passes
@Test void spikeFlag()                 // landing on '^': FLOOR and lastSpike true
@Test void solidRectIsAWallAndAFloor() // a 64x64 Rect in solids: blocks a right move and holds a falling body
@Test void blockedProbe()              // body touching a wall: blocked(b, RIGHT, ...) true, no move
```

- [ ] **Step 2: Run** `./gradlew test --tests 'mmx.x6.physics.*'` → FAIL.

- [ ] **Step 3: Implement**

`slopeSurface`: for values 1–4 (and +16): `hLeft − xInCell / 4` with hLeft 15, 11, 7, 3; 5–8:
`hLeft + xInCell / 4` with hLeft 0, 4, 8, 12; 9, 10: `hLeft − xInCell / 2` with 15, 7; 11, 12:
`hLeft + xInCell / 2` with 0, 8 (integer division).

`move` order (docs/engine.md "CollideCheck"):
1. x: `b.x += dx`. Leading edge cell x = `right() − 1` (dx > 0) or `left()` (dx < 0); test 3
   points: `top() + 2`, centre y, `bottom() − 4`; a wall value or a solid rect → snap flush,
   set RIGHT/LEFT, `lastWallCling = allowsWallCling(v)` (a solid rect: true, the sabre block sets
   `+0x75`). Spike value at any point → `lastSpike`.
2. y down (dy ≥ 0): the centre point at the feet (`centre x`, `bottom()`): value 16 → use the
   cell 16 px higher. Slope → surface = cell top + `slopeSurface`; snap when `grounded` or
   `bottom() ≥ surface`; FLOOR, `lastOnSlope`, return (no corner step). Else test the feet at
   `left() + 1`, centre, `right() − 1` for `isFlatFloor`; one-way values only when the bottom
   before the move was ≤ the cell top. Snap the bottom to the cell top. Solid rects: land on their
   top under the same "was above" rule. When `grounded` and nothing is found, test 16 px lower
   for a slope or a flat floor and snap down (`0x8002F900`).
3. y up (dy < 0): 3 points on `top()` (`left() + 1`, centre, `right() − 1`): ceiling values and
   solid rects → snap, CEILING.

- [ ] **Step 4: Run** → PASS.

- [ ] **Step 5: Commit** `git commit -m "Add the tile rules and the terrain mover with slopes"`

---

### Task 4: Animations, rendering and the stage screen

Source: `README.md` of mmx-demake-tools (sprite export format), `docs/engine.md` "Animations",
`docs/stages/intro.md` "Backgrounds".

**Files:**
- Create: `src/main/java/mmx/x6/anim/{AnimDef,AnimSet,Animator}.java`,
  `game/StepClock.java`, `render/{Assets,LevelRenderer,EntityRenderer,DebugOverlay}.java`,
  `screen/StageScreen.java`, `input/Buttons.java`
- Modify: `X6Game.java` (open `StageScreen`)
- Test: `src/test/java/mmx/x6/anim/{AnimSetTest,AnimatorTest}.java`, `game/StepClockTest.java`,
  `input/ButtonsTest.java`

**Interfaces:**
- Produces:
  - `record AnimDef(int index, List<Frame> frames, Integer loopTo)`; `record Frame(int sprite, int ticks, int flag)`.
  - `AnimSet`: `static AnimSet parse(String json)` (LibGDX `JsonReader`); `int cellW, cellH,
    originX, originY`; `AnimDef get(int index)` (null when absent); `Set<Integer> indices()`.
  - `Animator`: `Animator(AnimSet set)`; `void play(int anim)`; `void play(int anim, int frame)`;
    `void tick()`; `int anim()`, `int frameIndex()`, `int sprite()`, `int flag()` (the current
    frame's flag), `boolean flagEntered()` (true on the update the frame with a non-zero flag
    became current), `boolean atLastFrame()`, `boolean finished()` (a non-looping animation is on
    its held last frame).
  - `StepClock`: `int advance(float deltaSeconds)` returns the updates to run now; at most 4.
  - `Buttons`: bits `RIGHT 0x1, LEFT 0x2, UP 0x4, DOWN 0x8, WEAPON 0x10, BUSTER 0x20, JUMP 0x80,
    DASH 0x100`; `void sample(IntPredicate keyDown)` once per update; `int held()`,
    `int pressed()`, `int released()`; LEFT with RIGHT gives neither, UP with DOWN gives neither.
    Keys: arrows, Z jump, X buster, C dash, A weapon (sabre); R restart, Escape quit and F1 debug are
    read by `StageScreen`.
  - `Assets`: `Assets(DataPaths p)`; `AnimSet anim(String setName)`; `TextureRegion frame(String
    setName, int anim, int frameIndex)`; set names `x-falcon`, `junkroid`, `hover-gunner`,
    `meta-wheel-f`, `meta-dridler`, `boss-gate`, `d1000` (merged, below), `shots`, `sabre-block`.
  - `EntityRenderer.draw(SpriteBatch b, TextureRegion r, AnimSet s, float x, float y, boolean
    faceRight)`: top-left = (x − originX, y − originY); mirrored about x (`rx → −rx − 1`,
    `docs/engine.md` "Animations", "Flip") when the facing differs from the art. The art faces
    left for every set (X6 draws `flip 0x40` = right as the mirror); confirm it by viewing
    `x-falcon/anim-5.png` and `junkroid/anim-0.png` before you write the rule.

- [ ] **Step 1: Write the failing tests**

```java
@Test void animatorTicksDurations() {     // frames (sprite 1, 6 ticks), (2, 4), (3, 1, flag 1), loopTo null
    Animator a = new Animator(set); a.play(1);
    for (int i = 0; i < 6; i++) { assertEquals(1, a.sprite()); a.tick(); }
    assertEquals(2, a.sprite());
    for (int i = 0; i < 4; i++) a.tick();
    assertEquals(3, a.sprite()); assertTrue(a.flagEntered()); assertEquals(1, a.flag());
    a.tick(); assertTrue(a.finished());
}
@Test void animatorLoopsTo()          // loopTo 1: after the last frame the next frame is frame 1
@Test void parsesTheJunkroidSet()     // real data: cell 66 x 61, origin 32, 29, 8 animations, anim 1 has 6 frames with flags 1 and 2
@Test void stepClockCapsAtFour()      { StepClock c = new StepClock(); assertEquals(4, c.advance(1.0f)); assertEquals(0, c.advance(0.001f)); }
@Test void stepClockSixtyPerSecond()  { /* 60 calls of 1/60 s give 60 updates in total */ }
@Test void buttonsPressedAndReleased(){ /* JUMP down two samples: pressed only on the first; up: released once */ }
@Test void oppositeDirectionsCancel() { /* LEFT and RIGHT down: held() has neither */ }
```

- [ ] **Step 2: Run** `./gradlew test` → FAIL.

- [ ] **Step 3: Implement**

`Assets`: an animation strip `anim-<k>.png` holds the frames of animation k in order, one cell
each (`cellW` wide); `frame(set, anim, i)` = the cell i of that strip. Merged `d1000`: animations
0–5 from `d-1000`, 6–23 from `d-1000-parts`, each with its own cell and origin (keep a per-
animation AnimSet reference). `shots`: hand-made `AnimSet`s in code for the `x-shots` strips with
no json (one cell per frame; frame width = strip width / frame count): lemon anim 21 (6 frames,
ticks 1, 2, 2, 2, 2, 2, loop to 1), half charge 26 (4 × 2 ticks, loop), full charge 27 (3 × 3,
loop; used also for shot 19, see spec "Known gaps"), half flash 22 (6 frames, 1 tick each, flag
on frame 3: the shot appears after 4 updates), full flash 25 (6 frames, 1 tick each, flag on
frame 5: after 6 updates), lemon flash 39 (`TODO` frame count: check the strip by eye, 4 frames
of 17 px if they fit). Origin = the cell centre. `sabre-block`: cut from
`bankSheet("intro-1_start", 1)`: sprite 0 = the opaque bounding box in x 0–70, sprites 1–3 the
next three opaque boxes left to right (find them by scanning the alpha channel of a `Pixmap`
for columns with opaque pixels below row 10, the label row).

`LevelRenderer`: loads `tiles.png` (64 columns of 16 × 16) and draws a layer from its camera
position (lx, ly): only the cells in view. Layer positions come from `CameraController`
(Task 6); until then use the layer 0 position for all layers. Draw order: art 2, art 1,
art 0 main, entities, player, art 0 front, HUD.

`StageScreen`: `FrameBuffer(RGBA8888, 320, 240, false)`, color texture `Nearest`; each render:
`StepClock.advance(delta)` updates (each: `Buttons.sample`, world update), then draw into the FBO
with a y-down camera, then draw the FBO texture to the window at ×3 (flip y). Until Task 7, a
free camera moved by the arrow keys at 8 px/update shows the map. F1 toggles `DebugOverlay`:
collision cells by value color (slopes yellow, walls white, one-way cyan, ladder green, spikes
red, gate magenta, bar orange), at 50 % alpha.

- [ ] **Step 4: Run the tests** → PASS. **Run the app:** `./gradlew run`: the intro map shows;
the arrow keys scroll it; F1 shows the collision colors over the art.

- [ ] **Step 5: Commit** `git commit -m "Draw the intro map with a fixed-step screen and animations"`

---

### Task 5: Sound and music

Source: `docs/sound.md` "The bank slots", "The sound word", "Music (XA)".

**Files:**
- Create: `src/main/java/mmx/x6/audio/{SoundBanks,Sfx,MusicPlayer}.java`
- Test: `src/test/java/mmx/x6/audio/{SoundBanksTest,MusicPlayerTest}.java`

**Interfaces:**
- Produces:
  - `SoundBanks`: `static SoundBanks load(DataPaths p)`; `Optional<Path> wav(int bank, int id)`;
    `void useBossBank(boolean on)` (bank 2 → `m123-e01` when on, else `m094-e05`).
    Folders by bank slot: 0 `m087-e04`, 1 `m087-e11`, 2 `m094-e05`, 3 `m087-e13`, 5 `m094-e03`.
    A `bank.json` with `same_as` resolves to that folder. The WAV is the `file` of the entry in
    `ids` with that `id`; missing entry or file → empty.
  - `Sfx` (interface, so logic tests use a recording fake): `void play(int bank, int id)`,
    `void stop(int bank, int id)`. `CachedSfx(SoundBanks banks, Function<Path, Sound> loader,
    Consumer<String> log)` implements it (the app passes `p -> Gdx.audio.newSound(Gdx.files.absolute(p.toString()))`
    and `System.err::println`); sounds are cached by path; an empty `wav` or a loader exception
    logs `sound <bank>,<id> not found` once per (bank, id) and returns.
  - `MusicPlayer` (interface + `GdxMusicPlayer`): `void play(int track)`, `void stop()`,
    `boolean playing()`. Track data from `songs.json`: `record`, `start_seconds`, `end_seconds`,
    `loop`. Tracks used: 9 stage, 28 boss gate, 21 boss. `GdxMusicPlayer` opens the record WAV as
    a `Music`, seeks to `start_seconds`, and in `update()` loops back to `start_seconds` at
    `end_seconds`.

- [ ] **Step 1: Write the failing tests**

```java
@Test void playerJumpSound()   { assumeTrue(...); assertTrue(banks.wav(1, 1).isPresent()); }
@Test void landSound()         { assertTrue(banks.wav(1, 2).isPresent()); }
@Test void unknownIdIsEmpty()  { assertTrue(banks.wav(1, 999).isEmpty()); }
@Test void unknownBankIsEmpty(){ assertTrue(banks.wav(4, 0).isEmpty()); }
@Test void bossBankSwitch()    { banks.useBossBank(true); banks.wav(2, 5).ifPresent(p -> assertTrue(p.toString().contains("m123-e01")));
                                 banks.useBossBank(false); banks.wav(2, 26).ifPresent(p -> assertTrue(p.toString().contains("m094-e05"))); }
@Test void missingSoundLoggedOnce() {   // (Review Focus 5) no data needed: a SoundBanks over an empty temp dir
    List<String> log = new ArrayList<>();
    Sfx s = new CachedSfx(SoundBanks.load(new DataPaths(tmp)), p -> { throw new AssertionError(); }, log::add);
    s.play(1, 5); s.play(1, 5); s.play(2, 9);
    assertEquals(List.of("sound 1,5 not found", "sound 2,9 not found"), log);
}
@Test void stageTrack()        { MusicTrack t = MusicTracks.load(p).get(9); assertEquals("c07-t02.wav", t.record()); assertTrue(t.loop()); }
@Test void bossTrack()         { assertEquals("c05-t02.wav", MusicTracks.load(p).get(21).record()); }
@Test void gateTrack()         { assertEquals("c03-t02.wav", MusicTracks.load(p).get(28).record()); }
```

(`MusicTracks` is the json part of `MusicPlayer`, `record MusicTrack(int track, String record,
double start, double end, boolean loop)`.)

- [ ] **Step 2: Run** → FAIL. **Step 3: Implement.** **Step 4: Run** → PASS.

- [ ] **Step 5: Commit** `git commit -m "Add the sound banks and the XA music player"`

---

### Task 6: Camera, camera zones and BG layers

Source: `docs/engine.md` "Camera" (limits, zones, BG modes, the cap), `docs/stages/intro.md`
"Camera", "Checkpoints".

**Files:**
- Create: `src/main/java/mmx/x6/game/{CameraController,CameraZones}.java`
- Modify: `render/LevelRenderer.java` (use the layer positions)
- Test: `src/test/java/mmx/x6/game/{CameraControllerTest,CameraZonesTest}.java`

**Interfaces:**
- Consumes: `Checkpoint`, `CameraZoneDef`, `IntroCheckpoints.L1_*`, `L2_*`.
- Produces:
  - `CameraController`: `void reset(Checkpoint cp)` (position, current and target limits, BG
    layers set directly, `follow = false`); `void setTargets(int minX, int maxX, int minY,
    int maxY)` (−1 keeps); `void setFollow(boolean f)`; `void update(float px, float py)`;
    `float x(), y()`; `int minX(), maxX(), minY(), maxY()` (current);
    `float layer1X(), layer1Y(), layer2X(), layer2Y()`; `float easeSpeed` (default 2).
  - `CameraZones(List<CameraZoneDef> zones)`: `void reset()`; `void update(float px, float py,
    CameraController cam)`.

- [ ] **Step 1: Write the failing tests**

```java
@Test void resetUsesTheCheckpoint()  { cam.reset(IntroCheckpoints.ALL.get(0)); assertEquals(0, cam.x()); assertEquals(944, cam.y()); }
@Test void followCentresAndClamps()  { /* cp0, follow on, player (1000, 1100): x = 840, y = clamp(1100 - 128) = 944 */ }
@Test void limitsEaseTwoPerUpdate()  { /* cp0, setTargets(1968, -1, -1, -1): minX 2, 4, 6 ... after 1 update minX == 2 */ }
@Test void playerAtLeftOfMinX()      { /* minX 1968 and player x 2000: camera x == 1968 */ }
@Test void layerFormulas()           { /* camera (6064, 224) after reset at cp2-like values: layer1 = (3032, 672), layer2 = (1516, 696) */ }
@Test void layerCapSixteen()         { /* layer 2 old x 0, camera x jumps by 400 (target 100): moves 16 */ }
@Test void layerCapFarJump()         { /* target change >= 328: layer placed 320 short of the target */ }
@Test void zone0SetsMinY()           { /* player in 2272..2303 x 1040..1135: target minY 944, others kept */ }
@Test void zone1IsOneUse()           { /* after one apply, a later setTargets(-1, ...) then re-entry does not apply minX 1968 again */ }
@Test void zone3OnlyWhileCameraAtMost3840() { /* camera x 3900: entering 4000..4031 x 752..815 does not set maxY 704 */ }
@Test void zonesResetBringZone1Back()
```

The values `intro-1` (layer 0 at 0, 944 → layer 1 (0, 1032), layer 2 (0, 876)) and `intro-2`
(183, 944 → (91, 1032), (45, 876)) from `docs/stages/intro.md` are two more assertions for
`layerFormulas` (floor division).

- [ ] **Step 2: Run** → FAIL.

- [ ] **Step 3: Implement**

`update`: (1) each current limit moves toward its target by `easeSpeed`; (2) when `follow`:
target = (px − 160, py − 128) (`TODO` docs/engine.md "Camera": the follow rule is not read; this
centres X); (3) clamp x to [minX, maxX] and y to [minY, maxY]; (4) layer 1 = (floorDiv(x, 2) +
L1_OX, floorDiv(y, 2) + L1_OY) with the cap; layer 2 = (floorDiv(x, 4) + L2_OX, floorDiv(y, 4)
+ L2_OY) with the cap. Cap: |change| < 16 → target; 16–327 → 16 toward it; ≥ 328 → target ∓ 320.
Zones: a live zone applies while the player is inside its box (ends included): `setTargets`, and
`easeSpeed = 2`. Zone 1 (`textureSwap`) then turns off until `reset()`. Zone 3 applies only while
`cam.x() ≤ 3840`.

- [ ] **Step 4: Run** → PASS. **Run the app:** the free camera of Task 4 now drives the BG layers
with the X6 formulas; the background and decoration line up with the main layer at x 0 and at
x 6064.

- [ ] **Step 5: Commit** `git commit -m "Add the camera limits, the camera zones and the BG layer scroll"`

---

### Task 7: X's movement

Source: `docs/players/movement.md`, `docs/players/air.md` (not the hover, Mach Dash, super jump,
rope), `docs/players/ladder-hurt-events.md` "Ladder", `docs/engine.md` "The player".

**Files:**
- Create: `src/main/java/mmx/x6/game/{World,Entity}.java`,
  `game/player/{Player,PlayerConst}.java`
- Modify: `screen/StageScreen.java` (a `World` with the player at checkpoint 0; the camera follows
  the player; remove the free camera, keep it behind F2 for debugging)
- Test: `src/test/java/mmx/x6/game/player/PlayerMovementTest.java`

**Interfaces:**
- Consumes: `TerrainMover`, `Body`, `Buttons`, `Animator`, `CameraController`, `Sfx`.
- Produces:
  - `PlayerConst`: `WALK_START 0.5f, WALK 2.0f, JUMP 5.8125f, FALL_MAX -6.5f, FALL_GRAVITY
    0.2578125f, JUMP_GRAVITY 0.2578125f, DASH_START 6.5f, DASH_MIN 4.125f, DASH_DECEL -0.1875f,
    DASH_TIME 30, WALL_SLIDE -1.4375f, WALL_KICK_UP 5.625f, LADDER_FIRST 1.0f, LADDER 1.5f,
    BAR_MOVE 2.0f, FALCON_DASH_TIME 18, MAX_HP 32`; terrain box centre (0, 2), half 11 × 19.
  - `enum PlayerStep { BEAM_IN, STAND, WALK_START, WALK, DASH, DASH_END, CROUCH, LAND, JUMP, FALL,
    WALL_GRAB, WALL_SLIDE, WALL_KICK, FALCON_DASH, LADDER, BAR, HURT, GATE_WALK, EVENT_HOLD, DEAD }`
  - `Player extends Entity`: `Player(World w, Checkpoint cp)`; `void update(World w)` reads
    `w.buttons`; `PlayerStep step()`; `float speedX, speedY` (speedY positive = up, as the docs);
    `boolean grounded()`; `Body body()`; `boolean locked()` (true while `w.lockCount > 0` or a
    gate or event step); `void startEvent(PlayerStep s, boolean faceRight)`; `void endEvent()`.
  - Sounds (bank, id): land (1, 2), dash (1, 5) and its stop, dash end (1, 6), jump (1, 1) at the
    flag `0x80` frame of animation 10, wall kick (1, 3), wall grab (1, 4).

- [ ] **Step 1: Write the failing tests**

Tests run the player on synthetic grids (helper from Task 3) with a scripted `Buttons`:

```java
@Test void walkStartThenWalk()      // RIGHT held: x moves 0.5 per update while animation 7 runs (6 updates), then 2.0
@Test void fullJumpHeight()         // JUMP held from the floor: max rise between 67.5 and 69.5 px, top after 23 updates (±1)
@Test void tapJumpIsShort()         // JUMP held one update: rise < 7 px
@Test void releaseCutsTheRise()     // JUMP released on update 5: speedY == 0 on update 6
@Test void fallSpeedCaps()          // falling 60 updates: speedY never below -6.5
@Test void dashDistance()           // DASH pressed: 5 still updates (animation 16), then moves 6.5, 6.3125 ... min 4.125, at most 30 moving updates; then DASH_END slides down to 0 by 0.1875
@Test void dashJumpKeepsDashSpeed() // DASH then JUMP: air speed 4.125 with RIGHT held until landing
@Test void wallGrabThenSlide()      // in the air, RIGHT held into '#': WALL_GRAB 8 updates, then WALL_SLIDE at 1.4375 down per update
@Test void noWallGrabOn57()         // same into 'n': stays in FALL
@Test void noWallGrabForTenUpdatesAfterJump()
@Test void wallKick()               // JUMP next to '#': speedY 5.625, x moves 2.0 away for 10 fixed updates
@Test void falconDash()             // in the air, DASH pressed: FALCON_DASH, ~8 still updates (animation 169), then >= 4.125 per update facing, ends after 18 moves or at a wall; releasing DASH does not end it
@Test void oneAirDashPerAir()       // a second DASH in the same air time does nothing
@Test void ladderClimb()            // UP held under a '32' column: 9 updates at 1.0, then 1.5 up; at the '33' top: climbs onto it and stands
@Test void ladderDownFromTop()      // DOWN held on '33': goes down the ladder
@Test void barHang()                // UP held under a '45' cell while falling: BAR, y snaps to the bar; RIGHT moves 2.0; JUMP + DOWN drops
@Test void walkOffLedgeFalls()      // no grace time: FALL on the first update with no floor
@Test void slopeWalk()              // walking over "/ / / /" keeps grounded and FLOOR each update
@Test void landSoundPlayed()        // recording Sfx: (1, 2) once on landing
```

- [ ] **Step 2: Run** → FAIL.

- [ ] **Step 3: Implement the step machine**

One `switch (step)` per update, each case following its doc section and the "try" order of
`docs/players/movement.md` "The ground steps" (requests, ladder, dash or jump, crouch,
direction). Air control as `docs/players/air.md` "Air control": no inertia. The flags `+0x84`
(dash speed in the air), `+0x86` (no air dash), `+0x8B` (no wall test for 10 updates) are fields
`airDashSpeed`, `airDashUsed`, `wallDelay`. Blocked sides each update come from
`TerrainMover.blocked`, not from the last move. The animation numbers are the docs' (5 stand,
6 low HP, 7, 8 walk, 10 jump, 11 fall, 12 land, 13 wall grab, 14 kick, 15 slide, 16, 17 dash,
27–32 ladder, 81, 82 crouch, 163 bar, 169, 171–173 Falcon dash); the shooting versions (+112)
come in Task 8. Bar hang: only value 45 (flat) is in the intro; `docs/players/air.md` "Hang".
`CameraClamp`: clamp the player x to [minX + 8, maxX + 312] (`docs/engine.md` "Camera").

- [ ] **Step 4: Run** → PASS. **Run the app:** X stands at (144, 976); for now he starts in STAND
(the beam-in is Task 10); walk, jump, dash, wall-kick and climb the ladder at x 2000 by hand.

- [ ] **Step 5: Commit** `git commit -m "Add X's ground, air, wall, ladder and bar movement"`

---

### Task 8: Buster, sabre, damage, hurt and death

Source: `docs/players/attacks.md`, `docs/players/weapons.md` (ids 50, 51, the pose steps
45–47 in `docs/players/steps-extra.md`), `docs/damage.md`, `docs/engine.md` "Damage to an
object", "The damage tables", "Damage to the player", `docs/players/ladder-hurt-events.md`
"Hit reaction", "Death", "Other deaths".

**Files:**
- Create: `src/main/java/mmx/x6/game/{DamageTables,Damage,StageFlow}.java`,
  `game/player/{BusterShot,ChargeFlash,SabreSlash}.java`, `game/fx/{Explosion,Debris}.java`
- Modify: `game/player/Player.java`, `game/World.java`
- Test: `src/test/java/mmx/x6/game/{DamageTest,DamageTablesTest,StageFlowTest}.java`,
  `game/player/{BusterTest,SabreTest,HurtTest}.java`

**Interfaces:**
- Produces:
  - `DamageTables`: `enum Table { NORMAL, SABRE_ONLY, D1000, DEFLECT, MINOR }`;
    `static int value(Table t, int weaponId)` returns the damage, `DEFLECT = -1` (`D`), or
    `NO_EFFECT = -2` (`–`). Copy the five tables from `docs/engine.md` "The damage tables" as
    `int[80]` literals.
  - `Damage`: `static int toPlayer(int contactDamage)` (Normal × 1, then halved for the armor,
    minimum 1; 0 and 127 unchanged); `static int hitObject(Entity e, int weaponId)` returns
    `0` no hit, `127` blocked/deflected, `weaponId + 1` hit, `-(weaponId + 1)` killed; it lowers
    `e.hp`.
  - `BusterShot extends Entity`: `BusterShot(World w, int weaponId, float x, float y, boolean
    faceRight)` for ids 0, 9, 19; `boolean deflected`.
  - `SabreSlash extends Entity`: id 50 (floor) or 51 (air, wall); follows X; up to 4 hits, no box
    for 4 updates after a hit.
  - `StageFlow`: `void killPlayer()`, `void restart(int checkpoint)` (the full reset of Review
    Focus 2), `boolean deathSequenceDone()`.
  - `Explosion(World w, float x, float y)` (sound (0, 0) or (0, 1)); `Debris(World w, AnimSet set,
    int anim, float x, float y)`.

- [ ] **Step 1: Write the failing tests**

```java
@Test void playerDamageHalvedForArmor() {
    assertEquals(2, Damage.toPlayer(4)); assertEquals(1, Damage.toPlayer(1));
    assertEquals(3, Damage.toPlayer(6)); assertEquals(4, Damage.toPlayer(8));
    assertEquals(1, Damage.toPlayer(3)); assertEquals(0, Damage.toPlayer(0)); assertEquals(127, Damage.toPlayer(127));
}
@Test void tables() {
    assertEquals(3, DamageTables.value(NORMAL, 0)); assertEquals(15, DamageTables.value(NORMAL, 19));
    assertEquals(4, DamageTables.value(NORMAL, 50));
    assertEquals(DEFLECT, DamageTables.value(SABRE_ONLY, 0)); assertEquals(4, DamageTables.value(SABRE_ONLY, 50));
    assertEquals(6, DamageTables.value(SABRE_ONLY, 51));
    assertEquals(DEFLECT, DamageTables.value(D1000, 0)); assertEquals(DEFLECT, DamageTables.value(D1000, 9));
    assertEquals(1, DamageTables.value(D1000, 19)); assertEquals(1, DamageTables.value(D1000, 50));
    assertEquals(NO_EFFECT, DamageTables.value(DEFLECT, 7));
}
@Test void lemonLimitThree()          // BUSTER tapped every 6 updates: the 4th press with 3 live lemons fires nothing
@Test void lemonWaitSix()             // presses on consecutive updates: one lemon per 6 updates
@Test void lemonSpeed()               // x += 6.75 per update; ends at a wall
@Test void chargeLevels()             // BUSTER held 25 updates: level 1 and sound (1, 7); 90: level 2
@Test void halfChargeRelease()        // release at level 1: a flash, then shot 9 after 4 updates at 8.0
@Test void falconFullCharge()         // release at level 2: shot 19 after 6 updates at 10.0; refused with 2 live shots
@Test void hitDropsCharge()
@Test void deflectedLemonFliesOff()   // against SABRE_ONLY: x speed reversed, y speed ±4.8, no box
@Test void halfChargePiercesAKill()   // id 9 kills a 3-HP NORMAL entity and goes on; a lemon stops
@Test void sabreHitsUpToFour()        // a 64x64 SABRE_ONLY target with 32 HP: one slash = at most 4 hits, 4 updates apart
@Test void sabreFloorAndAir()         // WEAPON on the floor: id 50, animation 183; in the air: id 51, animation 184
@Test void hurtClassFour()            // contact damage 4: HP 32 -> 30, animation 35, speedX 2.5 away, 16 updates of reaction, invulnerable until 75 updates after the hit
@Test void noDamageWhileInvulnerable()
@Test void spikesKill()               // landing on 63 with no invulnerability: DEAD
@Test void pitKills()                 // camera at maxY and player y - 8 >= maxY + 256: DEAD
@Test void respawnResetsRecords()     // (Review Focus 2) a kept (broken) sabre block record, a closed gate, a dead player at cp 1: after restart(1) the records are free, entities cleared, camera == cp 1, music track 9 asked, player at (2112, 1072)
```

- [ ] **Step 2: Run** → FAIL.

- [ ] **Step 3: Implement**

The shot try, the charge and the order in one update as `docs/players/attacks.md` "Order in one
update". Buster muzzle: 21 px in front of and 9 px above X's origin; +17 px forward for 9, +14
for 19. Boxes: lemon (−10, −6, 18 × 11), 9 (−13, −7, 29 × 13), 19 uses the 18 box (−34, −19,
64 × 36) (`TODO` spec "Known gaps": no Falcon shot art or box). Shots end 12 / 8 px (lemon) or
32 px outside the view. Shooting pose: the stand, walk, jump, fall, land, dash, wall animations
+ 112 while `shotPose > 0` (20 or 35 updates), keeping the frame (`Animator.play(anim, frame)`).

Sabre: the WEAPON press with no shot wait starts pose step 45 (floor, animation 183), 46 (air,
184) or 47 (wall, 187); the slash entity appears at the first frame with a flag and ends with
the animation. Box: (−8, −32, 56 × 48) in front of X (`TODO` spec "Known gaps": "box from the
frame flag" has no numbers). A floor slash can be cancelled by jump or crouch after the flag
`0x40` frame.

Hurt: `docs/players/ladder-hurt-events.md` class 4 row. Death: freeze 9 updates in animation
33, then 39 updates of orbs (8 `Debris`-like orbs per wave, 3 waves, using any small round
sprite; `TODO` the orb object is not read), then 21; then `StageFlow.restart(world.checkpoint)`
after a 30-update black fade (`TODO` the restart path is not read). Sounds: hurt (3, 0 or 1) and
(3, 17); death (3, 16) and (3, 14).

`Explosion`: 16 updates of an expanding circle drawn by `ShapeRenderer` (`TODO` the explosion
sprites are not exported). `Debris`: speeds from `docs/enemies/sabre-block.md` "Break"
(`junk_speed_x`, `junk_speed_y`, gravity 0.3125), drawn every second update, deleted 64 px off
screen.

- [ ] **Step 4: Run** → PASS. **Run the app:** shoot lemons, charge, slash; walk into the
first spikes or a pit and see the death and the respawn at checkpoint 0.

- [ ] **Step 5: Commit** `git commit -m "Add the buster, the sabre, damage, hurt, death and respawn"`

---

### Task 9: Spawner and the enemies

Source: `docs/engine.md` "Spawning and despawning", "The standard main-object update", "Solid
objects"; `docs/enemies/junkroid.md`, `hover-gunner.md`, `meta-wheel-f.md`, `meta-dridler.md`,
`sabre-block.md`; `docs/stages/intro.md` "Objects", "Checkpoints".

**Files:**
- Create: `src/main/java/mmx/x6/game/{Spawner,EntityFactory}.java`,
  `game/enemy/{Junkroid,JunkroidShot,HoverGunner,HoverGunnerShot,MetaWheelF,MetaDridler,SabreBlock}.java`
- Modify: `game/World.java` (spawner each update, checkpoint markers, solids from sabre blocks)
- Test: `src/test/java/mmx/x6/game/SpawnerTest.java`, `game/enemy/*Test.java` (one per enemy)

**Interfaces:**
- Produces:
  - `Spawner(List<RecordDef> records, int difficulty, EntityFactory f)`;
    `void update(World w)`; `void released(RecordDef r)` (despawn: can spawn again);
    `void kept(RecordDef r)` (never again until reset); `void reset()`.
  - `EntityFactory`: `Entity create(World w, RecordDef r)`; null for names it does not make
    (`CameraZone`, `TalkZone`, `PaletteCycle`, `IntroHunters`, `IntroWreckA`, `IntroWreckB`;
    `CheckpointMarker` is handled by `World`).
  - Each enemy class: a constructor `(World w, RecordDef r)` and the boxes, HP, damage and table
    of its sheet.

- [ ] **Step 1: Write the failing tests**

```java
@Test void spawnBandIs48px()           // a record 47 px right of the view spawns; 49 px does not
@Test void kindFilterNormal()          // kinds 0 and 1 spawn at difficulty 1; kind 2 does not
@Test void oneObjectPerRecord()        // a spawned record does not spawn again while its entity lives
@Test void releasedRecordSpawnsAgain() // after released(), it spawns again when it re-enters the band
@Test void keptRecordNeverSpawns()     // after kept(), never, until reset()
@Test void checkpointMarker()          // player x >= 2112: world.checkpoint == 1; x >= 5760: 2
@Test void junkroidShotTiming()        // active Junkroid, player 100 px away: first shot 11 updates after entering range, then every 153; dx >= 113: back to idle, timer 120
@Test void junkroidDormant()           // id 0: never shoots, never turns
@Test void junkroidHp()                // 4 lemons kill it; explosion + 4 debris
@Test void hoverGunnerFlyAndBurst()    // id 0: flies left at 4.0; |dx| <= range (64..112) -> stops; shots at 22, 31, 40 after the stop, aimed in 16 directions at 2.0
@Test void hoverGunnerId1Waits()       // invisible until the player is >= 176 px to its right
@Test void hoverGunnerLeaves()         // after burst, burst, fan of 9: rises at 2.0, deleted 64 px above the view
@Test void metaWheelRollsAndTurns()    // id 1: rolls left, accel 0.046875, top 0.75; at a wall: sound (2, 4), stops 111 updates, rolls right
@Test void metaWheelSabreOnly()        // a lemon is deflected; slash 50 does 4
@Test void metaDridlerCycle()          // WAIT 90, DROP (gravity 0.125, start 1/16 up) to the floor or 128 px, DRILL 60, RISE 0.5/update to home; contact 8 (4 after the armor halving)
@Test void metaDridlerCannotDie()      // DEFLECT table; HP never changes
@Test void sabreBlockIsSolid()         // world.solids() holds its 64x64 rect after the 8-px lift; X stands on it and cannot walk through it
@Test void sabreBlockBreaks()          // 8 floor slashes (4 each, 32 HP): 6 explosions, 3 debris, record kept
@Test void sabreBlockRespawnsFullHpAfterLeaving()
```

- [ ] **Step 2: Run** → FAIL.

- [ ] **Step 3: Implement**

Spawn band: the record point within 48 px of the 320 × 240 view. Despawn when an entity's own
off-screen rule fires (64 px by default; Meta Wheel F 128; Meta Dridler 80 × 160; sabre block
64): `released`. Kill: `released` (enemies come back), sabre block: `kept`. The rock-block copy of
Meta Dridler ids 1–3 (`docs/enemies/meta-dridler.md` "Rock block") is in: when such a drill lands
on a floor, copy the 4 × 3 template cells (art tiles of `art 0 main` and collision) from 256 px
lower; the docs think it may never happen, so it is cheap to keep. The wheel-against-wheel rule
is in (`docs/enemies/meta-wheel-f.md`). Contact damage reaches the player through
`Damage.toPlayer` and the player's hurt (Task 8). Shots of enemies use `DamageTables.MINOR` and
die to any player shot. Sounds: Junkroid fire (2, 26); Hover Gunner shot (2, 0); Meta Wheel F
wall and landing (2, 4); Meta Dridler drop (2, 1), drill (2, 2), rise (2, 3); deflect (0, 6).
Debris: the enemy's debris animations of its sheet.

- [ ] **Step 4: Run** → PASS. **Run the app:** play from the start to x 5700 at difficulty 1;
the blue-marker records (kind 2) do not appear.

- [ ] **Step 5: Commit** `git commit -m "Add the spawner, the intro enemies and the sabre blocks"`

---

### Task 10: Boss gates, beam-in and restart

Source: `docs/enemies/boss-gate.md`, `docs/players/ladder-hurt-events.md` "Beam-in",
`docs/engine.md` "Stage start", `docs/stages/intro.md` "Boss gates".

**Files:**
- Create: `src/main/java/mmx/x6/game/enemy/BossGate.java`
- Modify: `game/player/Player.java` (BEAM_IN, GATE_WALK), `game/StageFlow.java`,
  `screen/StageScreen.java` (R restarts, Escape quits)
- Test: `src/test/java/mmx/x6/game/enemy/BossGateTest.java`, `game/StageFlowTest.java` (add)

**Interfaces:**
- Produces: `BossGate(World w, RecordDef r)`; `enum GateStep { WAIT, START, OPEN, PASS, CLOSE,
  CLOSED }`; `GateStep step()`. `StageFlow.restart(0)` is the R key.

- [ ] **Step 1: Write the failing tests**

```java
@Test void gateIsAWallFromTheLeft()   // a 32x64 solid centred on (5744, 384); no floor or ceiling from it
@Test void opensOnlyFromTheLeftOnTheFloor() // a jump against it does not trigger; standing and walking right does
@Test void triggerClearsEnemiesAndLocks()   // entities other than gates and blocks removed; player locked, faces right; music track 28
@Test void openTakes75Updates()       // sounds (0, 16) at the start, (0, 17) after 67; limits of id 2: minX = maxX = 5744, minY = maxY = 224
@Test void passPushesThePlayer()      // X has no terrain box; x += 0.640625 per update until camera x == new minX; then (0, 17) and CLOSE
@Test void closeTakes76Updates()      // (0, 16) after 8; then CLOSED: solid again for good; player unlocked
@Test void gate19Limits()             // id 1: minX 6064, maxX 6144 (X + 80)
@Test void beamInFallsThroughTerrain()// cp 1 (beamThru 6): 8 px down per update, no terrain for 6 updates, then lands; locked until animation 2 holds; camera follow starts at the landing
@Test void restartMidGatePass()       // (Review Focus 4) R during PASS: player at cp 0, unlocked after the beam-in, gates CLOSED -> WAIT again, music track 9, lockCount 0
```

- [ ] **Step 2: Run** → FAIL.

- [ ] **Step 3: Implement**

Gate steps and timings as `docs/enemies/boss-gate.md`. Limits for the two intro ids only: id 2
→ (X, X, 224, 224); id 1 → (X, X + 80, keep, keep). The player's step 19 (GATE_WALK) does not
walk by itself: the gate moves X (`TODO` boss-gate.md open question 2); X shows the walk
animation. Beam-in: animation 1 (the beam), sound (1, 13), `speedY = -8`, the checkpoint's
`beamThru` updates with no terrain box, then on the floor animation 2 and follow on; at its hold
frame the lock ends and X stands. The READY banner is skipped.

- [ ] **Step 4: Run** → PASS. **Run the app:** reach gate 18, watch it open, the pass and the
close; same for gate 19; press R anywhere for a clean restart.

- [ ] **Step 5: Commit** `git commit -m "Add the boss gates, the beam-in and the restart key"`

---

### Task 11: The D-1000 fight, the HUD and the exit

Source: `docs/enemies/d-1000.md`, `docs/stages/intro.md` "The D-1000 boss room",
`docs/engine.md` "Boss entry and exit".

**Files:**
- Create: `src/main/java/mmx/x6/game/boss/{D1000,D1000Core,D1000Shot,D1000Bomb,FirePillar,BossWarning}.java`,
  `render/Hud.java`
- Modify: `game/EntityFactory.java` (`D1000`), `game/StageFlow.java` (STAGE CLEAR),
  `game/World.java` (`int bossHp, bossHpMax`, `boolean showBossBar`)
- Test: `src/test/java/mmx/x6/game/boss/{D1000Test,D1000CoreTest}.java`

**Interfaces:**
- Produces: `D1000(World w, RecordDef r)`; `enum BossStep { ENTRY, THINK, WALK, CROUCH, BOMB,
  SUMMON, RAM, STUN, DEATH }`; `BossStep step()`; `int entrySub()`; `static BossStep pick(int r,
  int hp, boolean afterCrouch)`. `D1000Core(World w, D1000 body, float x)`.
  `StageFlow.clear()`: STAGE CLEAR state.

- [ ] **Step 1: Write the failing tests**

```java
@Test void thinkTable() {
    assertEquals(WALK,   D1000.pick(7, 32, true));  assertEquals(SUMMON, D1000.pick(8, 32, true));
    assertEquals(RAM,    D1000.pick(7, 32, false)); assertEquals(SUMMON, D1000.pick(8, 32, false));
    assertEquals(SUMMON, D1000.pick(5, 16, true));  assertEquals(BOMB,   D1000.pick(6, 16, true));
    assertEquals(RAM,    D1000.pick(5, 16, false)); assertEquals(BOMB,   D1000.pick(6, 16, false));
}
@Test void startsAboveTheRoom()       // body at (6288, 160), HP 0, player in EVENT_HOLD
@Test void entryFillsHp()             // after the WARNING and the fall: HP +1 per update to 32, sound (0, 14) every 3; music track 21; then THINK and the player unlocked
@Test void lemonAndHalfAreDeflected() // ids 0 and 9 bounce off the body
@Test void fullChargeDoesOne()        // id 19 on the body: HP - 1, then 30 updates of DEFLECT
@Test void coreHitStuns()             // id 19 (or 50) on the active core: STUN; 65 updates after landing HP - 8
@Test void fourCoreHitsWin()          // HP 32 -> 0 after 4 stuns -> DEATH
@Test void coreSideAwayFromPlayer()   // player x < 6264: core at (6368, 256); else (6160, 256)
@Test void deathBlinksThenExplodes()  // 127 updates of blink, then the explosion; enemy shots removed; player cannot be hurt
@Test void clearAfterTheExplosion()   // StageFlow state CLEAR, music stopped, player locked; a key press restarts at cp 0
@Test void deathInTheFightRespawnsAtCp2() // the entry runs again after gate 19
```

- [ ] **Step 2: Run** → FAIL.

- [ ] **Step 3: Implement**

Follow `docs/enemies/d-1000.md` step by step: the entry sub-steps 0–5 (the player event steps 20
and 29: hold, walk right at 1 px/update for 40 updates), THINK 60 updates, WALK, CROUCH, BOMB
and the fire pillar, SUMMON (4 core shots 8 updates apart at 2.0, signal at 124), RAM (wind-up
180, accel 1 up to 4, the wall column swap with `CollisionGrid.set` and the art cells of the
templates at x 6528 / 6576, 180 updates at the wall, restore from 6512 / 6560), STUN and the core
(moves to the other side, 60 + 100 updates). Boxes: body contact and hurt (−56, −64, 112 × 92),
terrain centre (0, −24) half 64 × 48 from y 288; core hurt (−10, −10, 20 × 20). Body animations
0–5 (set 0) and 6–11 (set 1); core 12, 15, 16 (`docs/enemies/d-1000.md` "Sprites"). Sounds from
the boss bank (`SoundBanks.useBossBank(true)` when the body spawns): landing (2, 5), stand up
(2, 4), bomb (2, 6), core removed (2, 20); WARNING (0, 19) at update 6 then every 61.
`BossWarning`: "WARNING" text that blinks for 470 updates (`TODO` the 22 rays are not drawn).
`BossExplosion` (stage logic 26) is simplified: 9 updates of 8 explosions, then explosions every
4 updates around the body (±96 px) for 100 updates, a white fade (+25 of 4095 per update), a
short white hold, a fade back; then `StageFlow.clear()`. `Hud`: X's HP bar (32 units, 2 px
each, left side, from y 40) and the boss bar on the right while `showBossBar`, plain rectangles;
"STAGE CLEAR" and "PRESS ANY KEY" centred.

- [ ] **Step 4: Run** → PASS. **Run the app:** play the whole stage, beat the D-1000 with full
charges on the core, see STAGE CLEAR, press a key, the stage restarts.

- [ ] **Step 5: Commit** `git commit -m "Add the D-1000 fight, the HUD and the STAGE CLEAR exit"`

---

### Task 12: Play-through check

**Files:** none new (fixes only).

- [ ] **Step 1:** Run `./gradlew test` → all PASS (data tests run on this machine).
- [ ] **Step 2:** `./gradlew run`; play from the beam-in to STAGE CLEAR. Check, with F1 on: the
  slopes near the start, the ladder and sabre block 40 at x 2000, the spikes, the camera zones at
  x 2112, 3616, 4000, 5200 and the shaft, both gates, the boss.
- [ ] **Step 3:** Die once in each part (before cp 1, after cp 1, in the boss fight) and check the
  respawn point and the reset.
- [ ] **Step 4:** Write `README.md`: what the game is, the data root, `./gradlew run`, the keys,
  F1, the known gaps (from the spec).
- [ ] **Step 5: Commit** `git commit -m "Add the README after a full play-through"`
