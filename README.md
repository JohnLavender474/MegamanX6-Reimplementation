# mmx-x6

A playable approximation of the Mega Man X6 intro stage in Java and LibGDX: X (Falcon Armor) from
the beam-in to the death of the D-1000, then a STAGE CLEAR screen.

The behaviour follows the reverse-engineering docs in `~/IdeaProjects/mmx-demake-tools/docs/`
where they give data, and best guesses elsewhere. Every guess and gap is listed in
[`docs/gaps.md`](docs/gaps.md). The design is in `docs/superpowers/specs/`, the build plan in
`docs/superpowers/plans/`.

## Data

No game data is in this repo. The game reads the files that `mmx-demake-tools` extracts:

- `tmx/m094-intro/` (map, tiles, collision)
- `sprites/players/x-falcon`, `sprites/players/x-shots`, `sprites/enemies/*`,
  `sprites/banks/intro/intro-1_start/bank-1.png`
- `sounds/` (VAB banks as WAV) and `music/bgm/` (XA tracks as WAV)

The data root is `~/mmx-data/x6` by default. To use another place, pass it as the first program
argument or as `-Dmmx.data=<path>`. A missing file stops the game with one error line.

## Run

```
./gradlew run
./gradlew test
```

Java 17 or later (the build targets 17; it is tested on JDK 25). The Gradle wrapper downloads
Gradle 9.1.0.

## Keys

| key | action |
|---|---|
| arrow keys | move, climb, aim the crouch |
| Z | jump (in the air: Falcon dash) |
| X | buster; hold to charge |
| C | dash |
| A | Z-Saber |
| R | restart the stage |
| F1 | debug view: collision, boxes, slash boxes |
| Esc | quit |

## Debug options

| property | effect |
|---|---|
| `-Dmmx.checkpoint=N` | start at checkpoint N (0, 1, 2) |
| `-Dmmx.startX=X -Dmmx.startY=Y` | start standing at a point, with wide camera limits |
| `-Dmmx.debug=true` | start with the F1 view on |
| `-Dmmx.hold=BITS` | hold buttons by their X6 bits (1 right, 2 left, 4 up, 8 down, 16 sabre, 32 buster, 128 jump, 256 dash) |
| `-Dmmx.jumpEvery=N` | with `mmx.hold`: pulse jump and the held non-direction buttons every N updates |
| `-Dmmx.shot=FILE -Dmmx.shotAfter=N` | save a PNG of the view after N updates and quit |

Example: `./gradlew run -Dmmx.checkpoint=2` starts in the hallway before the boss.
