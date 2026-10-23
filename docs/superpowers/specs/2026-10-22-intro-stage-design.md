# Design: the X6 intro stage, playable in LibGDX

Date: 2026-10-22. Repo: `~/IdeaProjects/mmx-x6` (new).

## Intent

A playable approximation of the Mega Man X6 intro stage (stage 0, DAT member 94). The stage loads
from the data that `mmx-demake-tools` extracted, X plays it from the spawn to the death of the
D-1000, and the stage ends with a simple exit.

The rule for all behavior: **when the docs give data (a behavior sheet, pseudo code, a constant, a
table), follow it. When they do not, make a best guess.** Corners can be cut where the result is
still playable.

It is a standalone project. It does not depend on Megaman-Maverick, and it does not try to match
Maverick's architecture.

## Goals

1. A class that parses `intro.tmx` and makes the collision layer, the objects (records), the
   enemies and the camera rules.
2. A class for X: an approximation of his moveset and behavior as the docs define it.
3. Rendering, logic, sound and music, so that the stage plays from the beginning to the end.

## Non-goals

- All text and dialogue: Alia's talk zones (records 15, 16), the Hunters' scene (record 17), the
  text boxes and portraits.
- Zero, High Max and every event after the death of the D-1000.
- The wrecks (records 54, 55, scenery), the palette cycles, the unused `IntroBgSwitch`.
- Difficulty 2 and 3. The game runs at difficulty 1 (Normal): records of kind 0 and 1 spawn, kind 2
  does not (`docs/engine.md`, "Spawning and despawning", difficulty filter).
- X's special weapons (`docs/players/weapons.md` slots 1–8), giga attacks, other armor forms,
  Zero as a player, the Ride Armor.
- Menus, a title screen, saves, gamepad support, a fixed-point copy of the PS1 math.

## Anti-goals

- No game data in the repo. All maps, sprites, sounds and music load at run time from
  `~/mmx-data/x6/` (the same rule as `mmx-demake-tools`). Add a `.gitignore` for the common data
  formats.
- No code copied from the projects in `~/IdeaProjects/mmx-reference/`. Only facts from the docs.

## Constraints

- Java (not Kotlin), LibGDX, desktop (LWJGL3) backend, Gradle build with the wrapper.
- The installed JDK is OpenJDK 25. Choose a Gradle and LibGDX version that build and run on it.
- The data root is `~/mmx-data/x6/` by default; allow an override (a system property or a program
  argument).
- The game logic runs at a fixed 60 updates per second (one X6 update per vertical blank,
  `docs/engine.md`, "Frame rate"). All doc values are in px per update and in updates, so use them
  as they are. Rendering can run at any rate.
- The view is 320 × 240 game pixels (`docs/engine.md`, "Camera"), scaled by an integer factor
  (default 3) with nearest-neighbor filtering.
- X6 coordinates have y down. LibGDX has y up. The builder chooses where to convert, but the logic
  must use the doc values without sign mistakes.

## Sources

The facts live in `~/IdeaProjects/mmx-demake-tools/docs/`. The builder reads them; this document
does not copy them.

| topic | doc |
|---|---|
| the stage: records, camera zones, checkpoints, BG layers, events | `docs/stages/intro.md` |
| shared object rules: spawn band, despawn, camera, checkpoints, tile collision, boss entry, animations, damage, death and drops | `docs/engine.md` |
| collision values (machine-readable) | `catalog/collision.json` |
| player ground movement | `docs/players/movement.md` |
| player air movement, wall slide, wall kick, Falcon dash | `docs/players/air.md` |
| buster, charge, shots | `docs/players/attacks.md` |
| the sabre (weapon id 50, 51) | `docs/players/weapons.md` ("The other ids that X makes") |
| ladder, hurt, death, beam-in | `docs/players/ladder-hurt-events.md` |
| armor forms (X is form 1, Falcon) | `docs/players/steps-extra.md` |
| damage in both directions | `docs/damage.md`, `docs/contact-model.md` |
| enemies | `docs/enemies/junkroid.md`, `hover-gunner.md`, `meta-wheel-f.md`, `meta-dridler.md`, `sabre-block.md`, `boss-gate.md`, `d-1000.md` |
| sound banks and music tracks | `docs/sound.md` |

Data under `~/mmx-data/x6/`:

| data | path |
|---|---|
| map | `tmx/m094-intro/intro.tmx`, with `tiles.png` and `collision.png` |
| X (Falcon Armor) | `sprites/players/x-falcon/` (`anim-<k>.png` strips and `animations.json` with the cell size, the origin, ticks and flags) |
| X's shots | `sprites/players/x-shots/` |
| enemies, gates, boss | `sprites/enemies/<name>/` (for example `junkroid`, `hover-gunner`, `meta-wheel-f`, `meta-dridler`, `boss-gate`, `d-1000`, `d-1000-parts`, `d-1000-sets`, `d-1000-extra`) |
| sounds | `sounds/<mNNN-eKK>/vag-NN.wav` and `bank.json` (a `PlaySound(bank, id)` maps to a WAV through the bank's `ids`) |
| music | `music/bgm/cNN-tNN.wav` (the track table in `docs/sound.md` gives each track's file and its start and end in seconds) |

## What it does

### 1. The TMX loader

One class reads `intro.tmx` and makes a `Level` (the name is the builder's call) with:

- **Art layers.** The four tile layers `art 2 background`, `art 1 decoration`, `art 0 main` and
  `art 0 front`, with their `offsetx`/`offsety` and `parallaxx`/`parallaxy` (the map has
  `parallaxoriginx` 160, `parallaxoriginy` 120). Draw order: art 2, art 1, art 0 main, the
  objects and X, then art 0 front. The builder checks against `docs/stages/intro.md`
  ("Backgrounds") that the layer 1 and 2 scroll is "layer 0 / 2 + 560" and "layer 0 / 4 + 640",
  and uses the doc formula where Tiled's parallax gives a different position.
- **No texture swap.** `tiles.png` already holds both art sets, so zone 1's swap needs no code.
- **No tile animations.** The TMX has none, and there is no export of the palette cycles. The
  tiles are static.
- **Collision grid.** The `collision` layer: each cell's value is its gid minus the `firstgid` of
  the `collision` tileset (3233). The meaning of each value is in `catalog/collision.json` and
  `docs/engine.md`, "Tile collision". The builder confirms the gid-to-value mapping against
  `mmx-demake-tools` (the `tmx` command) before relying on it.
- **Records.** The `records` object group: each object has `name`, `type` and the properties
  `index`, `kind`, `b`, `id`, `a`, with the record point in `x`, `y`. The `fixed records` group
  holds the second list (the palette cycles: ignore). The `camera zones` group holds the drawn
  trigger boxes. Where the TMX and `docs/stages/intro.md` disagree, the doc wins.
- **Camera zones.** The 13 entries with their trigger box, the min/max x/y values ("keep" = no
  change), and the special rules: zone 1 is one use; zone 3 deletes itself when the camera x is
  more than 3840. Read them from the TMX if the values are there, else from the table in
  `docs/stages/intro.md`.
- **Checkpoints.** The three `checkpoint_start_tbl` records (player x, y, camera, limits, facing)
  from `docs/stages/intro.md`. They are not in the TMX; a constant table in code is fine.

### 2. X

X wears the Falcon Armor (form 1, `docs/players/steps-extra.md`). The physics constants are the
same for all forms.

In scope, each from its sheet:

- Stand, walk start, walk, stop, land, crouch.
- Dash (ground), with its distance, speed and dash jump.
- Jump, variable jump height, fall, air control.
- Wall slide and wall kick, on the walls that allow it (`catalog/collision.json`: value 57 and 60
  have no wall slide).
- The Falcon dash in the air (step 35, `docs/players/air.md`). The intro text says the Falcon
  dash is "incomplete", but the code gives form 1 the Falcon dash; the docs win.
- Ladders: grab, climb up and down, the ladder top (values 32, 33), the ladder shot.
- The X-Buster: lemon shots (the limit on live shots), half charge, Falcon full charge (shot id
  19), the charge effect.
- The sabre: the weapon button with the buster selected gives weapon id 50 (floor) or 51 (air,
  wall), `docs/players/weapons.md`. The intro needs it for the sabre blocks and Meta Wheel F.
- Hurt: the hit reaction (Falcon: always the light class 4), the invincibility time, the knockback.
- Death: by HP, by pits (falling out of the map), by spikes if the map has them. Then a respawn at
  the last checkpoint (`docs/engine.md`, "Checkpoints" and "Spawning and despawning", "Death").
  Lives are not tracked: X respawns without limit.
- Beam-in at the start and after each respawn. It can be simple: a beam that drops to the floor,
  then the land animation.
- HP: the start value and the HP bar from the docs; if the docs do not give them, a best guess.

The animations come from `x-falcon/animations.json` by the index that the sheets name. The
sheets' "Animations used here" tables give the indexes.

### 3. Collision

- X and the objects use boxes, not physics bodies (`docs/contact-model.md`).
- Floor tests at the center point of the box, the slope profiles (values 1–12 and 17–28), value 16
  under slopes, one-way floors (33, 34), solids (56–60; 59 reads as 56), spikes (62, 63). Slopes
  are floor only: never a wall or a ceiling (`docs/engine.md`, "Tile collision").
- A grounded X stays on a slope that goes down (`0x8002F900`).
- Boss gates are run-time solids over value 59 cells.

### 4. Camera

- The layer 0 camera follows X and stays inside the current limits. The limits ease to their
  targets at the doc speed (`CameraLimitEase`, `docs/engine.md`, "Camera").
- At spawn and respawn, the checkpoint record sets the camera and both the current and the target
  limits.
- The camera zones set targets when X is in their trigger box, with the rules in the loader
  section.
- The BG layers follow layer 0 with their modes and offsets, with the 16 px/frame follow cap.

### 5. Objects and enemies

- **Spawn and despawn** follow `docs/engine.md`: a record spawns when its point comes within 48 px
  of the view; one record makes one object at a time; an object that leaves comes back when its
  record enters the band again. After a death, all objects are cleared and the records reset.
- **Enemies** at difficulty 1, each from its sheet: Junkroid (records 25–30), Hover Gunner
  (records 20, 22, 24), Meta Wheel F (records 31, 32, 34, 35), Meta Dridler (records 45, 46, 47,
  48, 49–53). Each sheet gives HP, contact damage, attacks, the damage table (for example Meta
  Wheel F is hurt by the sabre only; Meta Dridler cannot be hurt).
- **Sabre blocks** (records 40–44): `docs/enemies/sabre-block.md`.
- **Death, debris and drops**: the explosion from the docs. Drops (health capsules) only if the
  sheets or `docs/engine.md` give the rule; else no drops.
- **Damage** follows `docs/damage.md` in both directions.

### 6. Boss gates and the boss

- **Gates** (records 18, 19): `docs/enemies/boss-gate.md`. A gate opens when X touches it, X walks
  through on his own (input locked), the gate closes, and the camera limits move to the next
  room: the gate 1 hallway (x 5744, y 224) and the boss room (x 6064–6144, y 224). Checkpoint 2
  sits in the hallway.
- **Boss entry**: after gate 19, the WARNING (stage logic 24, `docs/engine.md`, "Boss entry and
  exit"), the D-1000 falls in from (6288, 160), the HP bars fill, then the fight starts. The
  WARNING can be simple text.
- **The D-1000** follows `docs/enemies/d-1000.md`: its parts, its attacks, its weak point, its HP
  and damage table. The sprite exports for it are split into several folders (`d-1000`,
  `d-1000-parts`, `d-1000-sets`, `d-1000-extra`); the builder maps them to the parts.
- **The death**: the blink and the first big explosion from the sheet. The Zero part of the death
  is cut.

### 7. The exit

When the D-1000's death explosion ends: the music stops, X stands still (input locked), the screen
shows "STAGE CLEAR". A key press restarts the stage from checkpoint 0. This is the "simple exit".

### 8. Sound and music

- **Music** (`docs/sound.md`, "Music (XA)"): track 9 during the stage, track 28 when a boss gate
  opens (`BossGate` plays it), track 21 for the boss fight. Each track is a part of a WAV in
  `music/bgm/` (start and end in seconds); it loops. Music restarts at track 9 after a respawn
  before gate 19.
- **Sounds**: each `PlaySound(bank, id)` that a used sheet names, mapped to a WAV through the
  bank folders (`docs/sound.md`, "The bank slots": bank 0 common, 1 the player, 2 the stage
  member, intro `m094-e05`, 3 the player's second bank, 5 `m094-e03`; the D-1000 room is member
  123). At least: X's jump, land, dash, wall kick, shot, charge, full charge release, sabre, hurt,
  death, beam-in; enemy hit, enemy explosion, enemy shots; the gate open and close; the WARNING;
  the boss's attacks and death. A sound with no doc mapping is skipped or uses a close match.
- Sounds play at the WAV's own pitch (a tone plays at its centre note, `docs/sound.md`). Volume by
  distance is not needed.

### 9. HUD and controls

- HUD: X's HP bar; the boss HP bar during the fight. Use the game's art if an export exists, else
  plain rectangles.
- Keyboard controls (the builder's defaults, rebindable in one place in code):

| action | key |
|---|---|
| move, climb | arrow keys |
| jump | Z |
| shoot (hold to charge) | X |
| dash | C |
| sabre | A |
| restart the stage | R |
| quit | Escape |

- A debug toggle (F1) that draws the collision grid, the object boxes, the camera limits and the
  camera zone boxes. It helps to check the TMX loader.

## Decided by the user

- A new repo at `~/IdeaProjects/mmx-x6`, Java, LibGDX.
- Follow the doc data where it exists; else approximate. Cutting corners is fine.
- The stage ends at the death of the D-1000. No dialogue and no special events.
- In scope: the player spawn (simple is fine), playable X, the collision layer with slopes, the
  tiles drawn, the boss gates, the camera logic, the boss, a simple exit, and at least minimal
  sounds and music.

## Calls made by Claude (the user can change them)

- Standalone; no dependency on Megaman-Maverick.
- Difficulty 1: kinds 0 and 1 spawn.
- The Falcon dash is in, because the code gives it to form 1.
- No tile animations, because none are exported.
- The exit: "STAGE CLEAR", then a key press restarts.
- No lives; unlimited respawns at the last checkpoint.
- The keyboard map and the F1 debug view above.
- Scale ×3, nearest-neighbor.

## Known gaps in the data

The docs and the exports do not cover these. Make a best guess for each, and add a `TODO` that
names the gap:

- **Shot sprites.** `sprites/players/x-shots/` has no `animations.json`, and there is no export of
  the Falcon full charge (shot id 19). `sprites/enemies/d-1000-sets/` has strips for the lemon, the
  half charge and the full charge flashes; use those, or `x-shots/` cut into cells by hand. Use the
  plain full charge (id 18) art for shot 19.
- **Sabre hit box.** The docs give it only as "box from the frame flag", with no numbers. Guess a
  box from the slash sprite.
- **Item drops.** The drop rule is documented, but the drop weight tables are not. No drops.
- **D-1000 sprites.** There is no combined export. `d-1000` holds animations 0–5 and
  `d-1000-parts` holds 6–23; together they cover every animation of the sheet.

## Left to the builder

- The package and class layout beyond the three classes in "Goals", the names, and the internal
  architecture (an entity list, state machines, a fixed-step loop).
- How the sprite strips load (a `TextureAtlas` built at run time, or one texture per strip).
- How the collision code is structured, provided the slope and floor rules match the docs.
- The order of work. A sensible order: the loader and the debug view, then X on flat ground, then
  slopes and walls, the camera, the enemies, the gates, the boss, then sound and music.
- Tests: unit tests for the TMX loader (record count, camera zone values, collision values) and
  for the slope profiles. The game itself is checked by play.
- Any value that the docs mark [open] or do not give: a best guess, with a `TODO` comment that
  names the doc section.
