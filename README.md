# mmx-x6

A playable approximation of Mega Man X6 stages in Java and LibGDX, with X in the Falcon Armor:

- the intro stage, from the beam-in to the death of the D-1000;
- Commander Yammark's stage, to the fight with Commander Yammark;
- Rainy Turtloid's stage (section 1), to the fight with Rainy Turtloid;
- Blaze Heatnix's stage (section 1), with all the Nightmare Snake fights, to a stand-in for Blaze
  Heatnix (there is no data for him yet);
- Blizzard Wolfang's stage (section 1), Ground Scaravich's stage (with the random section route)
  and Shield Sheldon's stage (section 1), each to its boss fight.

The game boots on a "Ready to play? Press START" screen. After Start, it continues as X6 does: the Capcom logo, a load screen, the opening movie, the title (PRESS
START) and the main menu. GAME START starts the intro stage. Each stage ends with a STAGE CLEAR
screen; after it, the stage select screen shows the boss stages.

The behaviour follows the reverse-engineering docs in `~/IdeaProjects/mmx-demake-tools/docs/`
where they give data, and best guesses elsewhere. Every guess and gap is listed in
[`docs/gaps.md`](docs/gaps.md). The design is in `docs/superpowers/specs/`, the build plan in
`docs/superpowers/plans/`.

## Data

No game data is in this repo. The game reads the files that `mmx-demake-tools` extracts:

- `tmx/m094-intro/`, `tmx/m095-commander-yammark-section-1/`, `tmx/m096-blizzard-wolfang-section-1/`,
  `tmx/m097-blaze-heatnix-section-1/`, `tmx/m100-ground-scaravich/`,
  `tmx/m101-rainy-turtloid-section-1/`, `tmx/m102-shield-sheldon-section-1/` (map, tiles, collision). When the same folder is in
  `assets/tmx/` of this repo (git ignores it), the game reads that copy instead. The Yammark and
  Turtloid copies there are made with a texture split (see `docs/gaps.md`, "TMX").
- `sprites/players/x-falcon`, `sprites/players/x-shots`, `sprites/enemies/*`,
  `sprites/banks/intro/intro-1_start/bank-1.png`, `sprites/banks/scaravich/scaravich-3_section/bank-5.png`
- `sounds/` (VAB banks as WAV) and `music/bgm/` (XA tracks as WAV)

The data root is `~/mmx-data/x6` by default. To use another place, pass it as the first program
argument or as `-Dmmx.data=<path>`. A missing file stops the game with one error line.

## Run

```
make run
make run STAGE=turtloid
make test
```

`make run` and `make test` call `./gradlew run` and `./gradlew test`. `make run` starts at the
ready screen; Start (Enter) goes on to the Capcom logo. `STAGE` (`intro`, `yammark`, `wolfang`, `heatnix`, `scaravich`, `turtloid`, `sheldon`)
skips the front end and the stage select screen.

The front end reads the movies from `<data root>/movies/` and the title and menu pictures from
`<data root>/re/front-end/images/`. When they are missing, the game starts at the stage select
screen.

Java 17 or later (the build targets 17; it is tested on JDK 25). The Gradle wrapper downloads
Gradle 9.1.0.

## Keys

| key | action |
|---|---|
| W A S D | move, climb, aim the crouch |
| K | jump (in the air: Falcon dash) |
| J | buster; hold to charge |
| L | dash |
| I | Z-Saber (the triangle button) |
| Q | left trigger: previous weapon (no effect yet) |
| O | right trigger: next weapon (no effect yet) |
| R | restart the stage |
| F1 | debug view: collision, boxes, slash boxes |
| Enter | Start |
| Esc | in the intro stage: back to the title; in a boss stage: back to the stage select screen; on the front end and the select screen: quit |

On the front end, Enter is Start and K is Cross. Start leaves the ready screen and skips the logo. Start or a face button (J, K,
L, I) skips the opening. Start on the title opens the menu. On the menu, W and S choose and Enter or
K picks. GAME START starts the game. OPTION opens the options screen, where you can rebind the keys and
the controller buttons for the current session (CONTROLLER SETTINGS needs a connected controller).

The keys above are the defaults. A controller works with the keyboard at the same time: its D-pad
moves, and the bottom, right, left and top face buttons are Cross (jump), Circle (dash), Square
(buster) and Triangle (weapon).

On the stage select screen, Up and Down choose, and Start, Cross or Square starts.

## Debug options

| property | effect |
|---|---|
| `-Dmmx.stage=KEY` | start this stage (`intro`, `yammark`, `wolfang`, `heatnix`, `scaravich`, `turtloid`, `sheldon`) without the select screen |
| `-Dmmx.front=NAME` | start the front end at `ready` (default), `logo`, `load`, `opening`, `title`, `menu` or `options`; `select` starts at the stage select screen |
| `-Dmmx.checkpoint=N` | start at checkpoint N (intro 0–2, Yammark 0–3, Wolfang 0–5, Heatnix 0–7, Scaravich 0–2, Turtloid 0–5, Sheldon 0–3) |
| `-Dmmx.startX=X -Dmmx.startY=Y` | start standing at a point, with wide camera limits |
| `-Dmmx.debug=true` | start with the F1 view on |
| `-Dmmx.hold=BITS` | hold buttons by their X6 bits (1 right, 2 left, 4 up, 8 down, 16 sabre, 32 buster, 128 jump, 256 dash, 512 next weapon, 1024 previous weapon) |
| `-Dmmx.jumpEvery=N` | with `mmx.hold`: pulse jump and the held non-direction buttons every N updates |
| `-Dmmx.shot=FILE -Dmmx.shotAfter=N` | save a PNG of the view after N updates and quit; with a list (`300,600`), one PNG per value (`FILE-300.png`, …). On the front end, the PNG is the whole window and `mmx.hold` and `mmx.jumpEvery` work too. A shot run is an automated run: the keyboard is off, a red border marks the window and its title says "AUTOMATED RUN! NO INPUT" |

Example: `./gradlew run -Dmmx.stage=intro -Dmmx.checkpoint=2` starts in the hallway before the D-1000.
