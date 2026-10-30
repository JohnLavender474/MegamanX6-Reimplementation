# Working notes for Claude: MegamanX6-Reimplementation

Write in plain, technical English (ASD-STE100), as the docs here do. The user's global rules also
apply (no code comments except the `TODO` format below; drafts to paste go in fenced blocks).

## What this repo is

A playable approximation of Mega Man X6 (PlayStation) stages in Java 17 and LibGDX 1.14.2, with X in
the Falcon Armor. It has seven stages: the intro (to the D-1000), Commander Yammark, Blizzard
Wolfang (section 1), Blaze Heatnix (section 1, with a stand-in boss), Ground Scaravich (with the
random section route), Rainy Turtloid (section 1) and Shield Sheldon (section 1). The game boots on a "Ready to play? Press START" screen (a custom change), then as X6 does (logo, load,
opening movie, title, menu); GAME START plays the intro, and a clear of a stage opens the stage
select screen with the boss stages.

The behaviour follows the reverse-engineering (RE) docs of the sister repo `mmx-demake-tools` where
they give data, and best guesses elsewhere. **Every guess, simplification and gap is listed in
`docs/gaps.md`.** The changes that differ from X6 on purpose (the user's design choices) are in
`docs/custom-changes.md`, with their values in `game/CustomChanges.java`. The design and plans are in `docs/superpowers/specs/` and
`docs/superpowers/plans/`. `README.md` has the keys, the debug options and a short account of how the data was made.
The user-levels work (level files, the validator, goals, the web player, the Tiled kit) has its
status, what is left and its decisions in `docs/superpowers/status/user-levels.md`; read it before you
work on it. `docs/CLAUDE.md` says which docs go where.

## Build, run, test

```
make run                     # ./gradlew run; the ready screen, then (Start) the Capcom logo and the front end
make run STAGE=yammark       # intro, yammark, wolfang, heatnix, scaravich, turtloid, sheldon: skip the front end
make test                    # ./gradlew test
./gradlew run -Dmmx.stage=heatnix -Dmmx.checkpoint=6
./gradlew run -Dmmx.level=/path/my-level.tmx   # play a level file (validated first; a rejected file shows LevelErrorScreen)
make dist                    # dist-linux and dist-windows: executables in game-builds/ (tools/build_game.sh)
```

- `make dist-linux` and `make dist-windows` delete the old JAR, build the fat JAR
  (`build/libs/<appName>-<projectVersion>.jar`, from `gradle.properties`), then run
  `tools/build_game.sh`. That script copies the method of the sister repo Megaman-Maverick: packr
  4.0.0 with an OpenLogic JRE 17 (downloaded once to `game-builds/` and `game-builds/jdk/`),
  `--resources assets` (the launcher starts in its own folder, so `DataPaths` finds `assets/`), then
  a zip. The output is `game-builds/<appName>-<projectVersion>-<platform>/` and `.zip`; git ignores
  `game-builds/`. Each version keeps its own build, and a build of the same version replaces it.

- Test results are in `build/test-results/test/*.xml`. With `-q`, Gradle prints only failures.
- Tests that need the real data call `Assumptions.assumeTrue(DataPaths.assetsExist())`, so the
  build passes on a machine with no data.
- Screenshots with no window input (for a visual check):
  `./gradlew run -Dmmx.stage=turtloid -Dmmx.checkpoint=5 -Dmmx.hold=1 -Dmmx.shot=/path/x.png
  -Dmmx.shotAfter=600,900` saves one PNG per update count and quits. `-Dmmx.hold` takes the X6
  button bits (1 right, 2 left, 128 jump, 32 buster); `-Dmmx.jumpEvery=40` pulses the jump.
  `-Dmmx.front=title` (or `ready`, `logo`, `load`, `opening`, `menu`, `options`, `select`) starts the front end there; the
  same shot options work on it.
- **Automated runs** (any run with `-Dmmx.shot`): keyboard and controller input are off on every screen (only
  `-Dmmx.hold` and `-Dmmx.jumpEvery` drive the game), a red border is drawn around the window, and
  the window title is only "AUTOMATED RUN! NO INPUT". `AutoRun` holds this rule. Every window that
  Claude opens for a check must be an automated run, so that the user can tell it from their own
  play and no key press changes it. A new screen must read keys only when `AutoRun.active()` is
  false, and must call `AutoRun.drawBorder` after its last draw (after the screenshot is read), and
  must save its shots and quit through `ShotClock` and `Shots`, so that no automated window stays
  open. Each screen counts its updates from 0. From boot to the intro: `-Dmmx.hold=2048
  -Dmmx.jumpEvery=40` (Start and Cross pulse); with no hold, the front end loops between the opening
  and the title.
- Look at the PNGs with the Read tool. Put them in the session scratchpad, not in the repo.

## Code layout (`src/main/java/mmx/x6/`)

| package                                      | what                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
|----------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `.`                                          | `DesktopLauncher`, `X6Game` (screens, per-stage sound banks), `GameFlow` (which screen comes next), `DataPaths` (the `assets/` paths)                                                                                                                                                                                                                                                                                                                    |
| `frontend`                                   | `FrontEnd` (the ready screen, then the X6 front-end states: logo, load, opening, title, menu; headless), `Fade` (`FadeIn`/`FadeOut`), `FrontEndText` (the misc 19 text table), `MovieFrames` (one PNG frame at a time); drawn by `screen/FrontEndScreen`                                                                                                                                                                                                 |
| `stage`                                      | `StageDef` (TMX, checkpoints, music, sound banks, Nightmare Effect, entity factory), `Stages` (the seven stages; `Stages.SANDBOX` for the grid worlds in tests), `StageLoader` (loads a built-in stage or a level file; `LoadedStage`), `Themes`                                                                                                                                                                                                         |
| `level`                                      | `LevelValidator` (the content rules for a level file; `Finding`, `ValidationReport`), `Catalog` (the rule data in `assets/levels/catalog.json`), `TmxLoader`, `Level`, `CollisionGrid`, `TileLayer`, `RecordDef`, `CameraZoneDef`, `Checkpoint`, `GoalDef`                                                                                                                                                                                               |
| `physics`                                    | `TerrainMover` (tile collision, slopes, solids), `TileRules` (collision values), `Body`, `Rect`                                                                                                                                                                                                                                                                                                                                                          |
| `game`                                       | `World` (one update; entity loop; weapons, contacts, solids, carry, pause), `CameraController` (follow, `CameraLimitEase`, BG layer modes), `CameraZones`, `Goals` (level goals), `Spawner` (spawn band, fixed records, checkpoint markers), `StageFlow` (death fade, restart, clear), `Damage`, `DamageTables`, `Progress` (lives, souls, Reploids, parts), `Entity`, `Solid`, `Weapon`, `CustomChanges` (the user's changes, `docs/custom-changes.md`) |
| `game/player`                                | `Player` (all X steps), `PlayerConst`, `BusterShot`, `BusterPoints` (copied from the executable), `SabreSlash`                                                                                                                                                                                                                                                                                                                                           |
| `game/enemy`                                 | shared objects (`BossGate`, `RescueReploid`, `NightmareVirus`, `LightCapsule`, `TeleporterCrystal`, `Projectile`, `IceSlide` (stage logic 7), `NightmareShadow`, `NightmareBug`), the intro enemies, `CommonEntityFactory`; one sub-package per stage with its factory                                                                                                                                                                                   |
| `game/boss`                                  | `Maverick` (warning, HP fill, hit invulnerability, death blink, explosion, boss soul), the D-1000 parts                                                                                                                                                                                                                                                                                                                                                  |
| `game/fx`                                    | explosions, debris, the boss explosion, death orbs                                                                                                                                                                                                                                                                                                                                                                                                       |
| `anim`, `render`, `screen`, `audio`, `input` | animation sets and players (sprite-only sheets; `BankSheetAnims` cuts sprites from a numbered bank sheet), drawing, screens, sound banks and music, buttons (`Controls`: keyboard and controller each frame, with the session `Bindings`)                                                                                                                                                                                                                |

Tests (`src/test/java/mmx/x6/`): `game/Sim` (a headless world on an ASCII grid), `stage/StageRun`
(a headless world on a real stage TMX: `at(stage, checkpoint)`, `until`, `runRight`, `find`),
`stage/StageSmokeTest` (runs every checkpoint of every stage for 2000 updates of scripted play),
`physics/Grids` (grids and an X body for mover tests).

## Conventions

- Logic runs at a fixed 60 updates per second (`StepClock`). Speeds are px per update; times are
  updates. Use the X6 values from the docs unchanged; only the renderer converts.
- X6 coordinates: y goes down. Doc boxes are `(dx, dy, w, h)` from the origin, for an object that
  faces left; sprites face left in the exports, and the renderer mirrors them when `faceRight`.
- One entity class per object type, named as the docs name it. The TMX record name selects it in the
  stage factory (`CommonEntityFactory` for the shared types).
- Difficulty is Normal (1): records of kind 0 and 1 spawn. X is the only player (no Zero paths).
- A value or rule that the docs do not give gets a best guess, a `TODO` at the code, and a line in
  `docs/gaps.md`. The `TODO` says what the edge case or guess is and points at the entry, for
  example `// TODO: the bubble start points use 16 even points (docs/gaps.md, "Rainy Turtloid")`. No
  other comments.
- Commit only when the user asks. End each commit message with
  `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`. When files changed in more than one
  piece of work, split the commits by piece; check that each commit builds and its tests pass.

## Sources

### The RE docs: `~/IdeaProjects/mmx-demake-tools/docs/`

- `engine.md`: the shared engine (object classes and states, spawning, camera and camera zones,
  `CameraLimitEase`, BG layer modes, solid objects, damage tables, the player interface, palette
  effects, checkpoints, boss entry and exit). Read the section that a task names before you code it.
- `stages/<stage>.md`: records, checkpoints, camera zones, gates, texture swaps, save-state facts.
- `enemies/<object>.md`: one behaviour sheet per object; `players/*.md`: X and Zero.
- `damage.md`, `sound.md`, `objects-legend.md`, `re-guide.md` (the confidence tags [certain],
  [likely], [guess] and how sheets are written), `TODO.md`.
- The docs change often (the RE work goes on). See "Rules from experience", rule 1.
- `make docs` (`tools/import_docs.py`) copies these docs to `docs/re/findings/`, and the `.md`,
  `.py`, `.sh`, `.c` and `.h` files of the data root's `re/<topic>/`, `notes/` and `stages/` to
  `docs/re/work/<topic>/`, `work/notes/` and `work/stages/`. It replaces both folders as a whole,
  leaves out the old docs snapshots in `re/` and all `.dis` files, and changes a path
  in backticks to a copied file into a relative link (`docs/re/README.md`). The copies are for
  reading only: the tools repo and the data root are the masters, so do not edit the copies, and
  diff against the tools repo's git history for rule 1. Run `make docs` when the user asks for new
  docs.
- The tools catalog `catalog/` (`stages.json` with `texture_split`, `enemies.json`,
  `object-names.json`, `helpers.json` with names of RAM addresses). Do not edit the tools repo or
  its catalog unless the user asks; propose changes to the user.

### The assets: `assets/`

The game reads nothing else. `make assets` (`tools/import_assets.py`) copies the data root into it:
`sprites/` (`players/`, `enemies/`, `banks/`, `stages/`, and `frontend/` from `re/front-end/images/`),
`sounds/` (WAV, because `WavRate` sets the pitch in the WAV header), `music/` (from `music/bgm/`; ffmpeg
changes each track to OGG Vorbis, and `songs.json` and `tracks.json` point at the `.ogg` files), `movies/`, and `tiled/tmx/`, `tiled/tsx/`, `tiled/png/` (from
`tmx/`: each embedded tileset moves out to `<map>-tiles.tsx` or `collision.tsx`). It keeps files that
exist (`--update` replaces them) and never replaces a map in `tiled/tmx/`.

### The data root: `~/mmx-data/x6/` (the source of `make assets`)

| folder                                                                         | what                                                                                                                                                                                                                                                                                         |
|--------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `tmx/mNNN-<name>/`                                                             | Tiled maps made by `mmxdemake.tmx`: `<name>.tmx`, `tiles.png`, `collision.png`; `summary.txt` lists them. Layers: `art 2 background`, `art 1 decoration` (with `x6_mode`, `x6_offset_*`), `art 0 main`, `art 0 front`, `collision`; object groups `records`, `fixed records`, `camera zones` |
| `sprites/enemies/<name>/`, `sprites/players/<name>/`                           | exports: `animations.json` (frames with sprite, ticks, flag; `loop_to`; cell and origin) and one `anim-N.png` strip per animation; `enemy-<hash>` folders are unnamed exports                                                                                                                |
| `sprites/banks/<stage>/<state>/`                                               | raw sprite bank sheets                                                                                                                                                                                                                                                                       |
| `sounds/mNNN-eKK/`                                                             | VAB banks as WAV with `bank.json` (slot, sound words, programs and tones); `summary.txt`                                                                                                                                                                                                     |
| `music/bgm/`                                                                   | XA music as WAV and `songs.json` (track → record, start, end)                                                                                                                                                                                                                                |
| `dumps/*.state.gz`                                                             | PCSXR save states named `<stage>-<n>_<what>`                                                                                                                                                                                                                                                 |
| `extracted/retail/`                                                            | the disc files: `iso/SLUS_013.95` (the executable), `rock_bin/NN.bin` (overlays), `rock_dat/`, `index.json`                                                                                                                                                                                  |
| `re/<topic>/`                                                                  | RE work folders: `notes.md`, pseudocode, kits, scripts, `doc-changes.md`                                                                                                                                                                                                                     |
| `stages/`, `tiles/`, `collision/`, `renders*/`, `probe/`, `preview/`, `notes/` | stage drawings (`stage.ora`, `objects.json`), tile and collision sheets, probes                                                                                                                                                                                                              |
| `disc/`                                                                        | the disc image                                                                                                                                                                                                                                                                               |

The project's maps are in `assets/tiled/tmx/`. The user annotates them in Tiled (a `TODO` point
layer). The Yammark and Turtloid copies are made again with a texture split (`docs/gaps.md`, "TMX").

### Reading the real game

- Save-state RAM: `cd ~/IdeaProjects/mmx-demake-tools && python3 -c "from mmxdemake.savestate import
  read_ram, ram_at; exe = open('/home/johnlavender/mmx-data/x6/extracted/retail/iso/SLUS_013.95',
  'rb').read(); ram = read_ram('<state.gz>', exe); print(ram_at(ram, 0x800971F8, 0x54).hex())"`.
  Pass the executable's bytes, not its path.
- Disassembly of a save state: `python3 ~/mmx-data/x6/re/x-crouch-shot/scripts/dis.py <state.gz>
  <hex addr> <count>`.
- The executable on disk: file offset = address − text address (u32 at offset `0x18`) + `0x800`.
- Main structures: `game` `0x800CCED0`, `mega` (the player) `0x800970A0`, `camera_layer0`
  `0x800971F8` (layer 1 `+0x54`, layer 2 `+0xA8`).
- Tools (`python3 -m mmxdemake.<tool>`, see the tools `README.md`): `tmx` (maps; `--catalog`,
  `--out`), `spriteexport`, `playerexport`, `stages`, `learn`, `sound`, `music`, `rekit`, `render`,
  `tiles`, `collision`, `text`, `extract`. The shared outputs in `~/mmx-data/x6` belong to the tools
  repo: write tool output to a scratch folder (for example `--catalog <scratch copy> --out
  <scratch>`), then copy what this repo needs into `assets/`.

## Rules from experience

These come from bugs that the user found in play (2026-10-23 to 2026-10-25).

1. **Recheck docs that changed after the code.** Before you build on shared code, diff each doc
   section that it cites against the doc version that it was written from
   (`git -C ~/IdeaProjects/mmx-demake-tools log -p --since=<date> -- docs/<file>`). The camera rule
   `CameraLimitEase` went into `engine.md` one day after the camera code; three stages were then
   built on the old fixed 2 px ease, which made a slow pan and an unfair pit death.
2. **Log simplifications as guesses, not only unread rules.** "A 2 px ease", "solids act as tiles"
   and "one-way platform" are guesses and belong in `docs/gaps.md`, so that a reviewer can compare
   them with new docs.
3. **Write a named engine routine once, as the spec gives it, and use it everywhere.** For example
   `SolidObject` is `World.solidObject` (push along the shorter overlap after the object moves,
   squeeze undo, 75-update timer). Do not approximate a routine with similar code from another part
   (the first version used the player's move code and buried X in the floor).
4. **Test both directions.** For solids: the object moves into the player, not only the player into
   the object. For the camera: a fast fall past a limit change. For gates and events: the player
   arrives during another lock (a snake set-up during a gate pass released the player).
5. **Check exports by eye before you trust them.** The TMX exports of Yammark and Turtloid had no
   texture split (both boss rooms were garbage tiles), one X animation strip was broken, and sound
   docs gave the wrong pitch. Take screenshots across each stage and boss room
   (`-Dmmx.shotAfter=...`) and compare layers with both texture blocks when a stage has a texture
   swap (a camera zone with `texture_swap` = 1).
6. **Prefer the save states over a reading of the text.** When the docs and a save state differ, or
   a value is [likely] or [guess], read the RAM or the code (for example the boss sound banks came
   from bank 2's word list in the boss save states, not from the member order).
7. **Follow the docs literally for order and state.** Checkpoint markers set `game.checkpoint` to
   their id when they are in the spawn band (Yammark's are out of order); a rule like "only raise"
   is a mistake.
8. **Scripted tests must reach the place the way a player can.** The headless runs walk and jump
   right; a pit, a ledge or a gate may need a placed start (`placeStanding`). When a scripted run
   fails, trace it (position, step, camera, entities) before you change game code.
9. **Do not change the user's design choices back without asking**, but tell the user when a choice
   differs from the real game and say what the real behaviour is. Record each choice in
   `docs/custom-changes.md`.

## Doing a task

- New object: read its sheet and the shared sections it names; check its export
  (`animations.json`: indices, ticks, flags); write the class and register it in the stage factory;
  add a test with `Sim` or `StageRun`; run `StageSmokeTest`; take screenshots; add the gaps.
- New stage: add a `StageDef` to `Stages` (checkpoints from the stage doc's `checkpoint_start_tbl`
  table, music from `stage_music`, banks from the save states), copy or make its TMX in
  `assets/tiled/tmx/` (with its `.tsx` and tile sheet), check the texture swap, then the objects.
- Bug report from the user: find the root cause in the docs and the real code first (save states,
  disassembly), then fix, then add a test that fails without the fix. Say which of these it is: a
  defect in the docs or data, a gap that was filled with a guess, or a mistake against the docs.
