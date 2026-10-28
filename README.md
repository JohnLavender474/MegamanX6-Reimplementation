# Mega Man X6 (vibe-coded)

A fan remake of stages from Mega Man X6 (PlayStation, 2001), written in Java with
the help of AI.

This is not an emulator and not a port. The game code is new. The graphics, sounds,
music and maps come from the original disc, and the enemy and boss behaviour is
rebuilt from a study of the original game.

## What you can play

The game starts as X6 does: the Capcom logo, the opening movie, the title screen and
the main menu. GAME START plays the intro stage. After the intro, a stage select
screen opens with these stages:

| stage             | how much                                                          |
|-------------------|-------------------------------------------------------------------|
| Intro             | the full stage, to the fight with the D-1000                      |
| Commander Yammark | the full stage and the boss fight                                 |
| Blizzard Wolfang  | section 1 and the boss fight                                      |
| Blaze Heatnix     | section 1 with all Nightmare Snake fights; the boss is a stand-in |
| Ground Scaravich  | the stage with its random section route, and the boss fight       |
| Rainy Turtloid    | section 1 and the boss fight                                      |
| Shield Sheldon    | section 1 and the boss fight                                      |

What is not here yet: Zero, the weapons that you get from the bosses, the second
sections of most stages, the Nightmare (Gate) stages, the pause menu, special weapons, 
and much more.

## How close it is to X6

Where the study of the game gives exact values (speeds, timers, damage, enemy
patterns), the remake uses them. Where it does not, the remake uses a best guess.
Each guess and each missing part is listed in [`docs/gaps.md`](docs/gaps.md).

Some changes from X6 are on purpose, for example the "Ready to play? Press START"
screen at boot and the options screen where you can change the keys. They are listed
in [`docs/custom-changes.md`](docs/custom-changes.md).

## How the data was made

1. **The disc.** The source is a copy of Mega Man X6 (USA, v1.1). Its checksum
   matches the Redump preservation database, so we know it is a clean copy.
2. **Extraction.** The tools take the disc apart: the 9 files on the disc, the 59
   code overlays and the 243 data members in the game's main archive.
3. **Decoding.** The tools turn each kind of data into normal files:
   - the stages into maps for the Tiled map editor (tiles, collision, enemy
     placement and camera zones);
   - the sprites into picture strips with their animation timing;
   - the sound banks into WAV files, and the streamed music into WAV tracks;
   - the logo and opening movies into picture frames and a soundtrack;
   - the menu text into tables.
4. **The study of the game (reverse engineering).** To learn how each enemy, boss,
   item and the camera work, we read the game's machine code and its memory at
   points in play. The memory comes from about 90 emulator save states, taken at
   known places in each stage. The results are written as one behaviour sheet per
   object (about 60 enemies and bosses), plus notes on the engine, damage and
   sound. Each fact has a confidence tag: certain, likely or guess.

Earlier community work helped as a source of facts:

- [`mmx6`](https://github.com/anzaldoivan/mmx6): a decompilation of X6
- [`MegaManX6Recomp`](https://github.com/mstan/MegaManX6Recomp): a recompilation of X6
- [`MegaManX6_PS1_Modding`](https://github.com/Kuumba123/MegaManX6_PS1_Modding): Kuumba123's X6 modding notes
- [Redump](http://redump.org/): the disc preservation database

We used their facts and checked each one against the disc and the save states. We
did not copy their code.

The full credits are in `docs/prior-work.md` in `mmx-demake-tools`.

## Play

You need Java 17 or later and the data in `assets/`. Then:

```
make run
```

`make run STAGE=yammark` goes directly to a stage (`intro`, `yammark`, `wolfang`,
`heatnix`, `scaravich`, `turtloid` or `sheldon`).

## Controls

A controller and the keyboard work at the same time. You can change both on the
OPTION screen of the main menu.

| key     | controller | action                                               |
|---------|------------|------------------------------------------------------|
| W A S D | D-pad      | move, climb, aim                                     |
| K       | Cross      | jump (in the air: Falcon dash)                       |
| J       | Square     | buster; hold to charge                               |
| L       | Circle     | dash                                                 |
| I       | Triangle   | Z-Saber                                              |
| Q / O   | L1 / R1    | change weapon (no effect yet)                        |
| Enter   | Start      | Start                                                |
| R       |            | restart the stage                                    |
| F1      |            | show collision and hit boxes                         |
| Esc     |            | go back (stage select or title); quit from the menus |

On the menus, W and S (or Up and Down) choose, and Enter or K picks.

## Debug options

For developers, these Java properties change how the game starts
(`./gradlew run -Dmmx.stage=intro -Dmmx.checkpoint=2`):

| property                            | effect                                                                                                                                                    |
|-------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------|
| `-Dmmx.stage=KEY`                   | start this stage without the select screen                                                                                                                |
| `-Dmmx.front=NAME`                  | start the front end at `ready` (default), `logo`, `load`, `opening`, `title`, `menu` or `options`; `select` starts at the stage select screen             |
| `-Dmmx.checkpoint=N`                | start at checkpoint N (intro 0–2, Yammark 0–3, Wolfang 0–5, Heatnix 0–7, Scaravich 0–2, Turtloid 0–5, Sheldon 0–3)                                        |
| `-Dmmx.startX=X -Dmmx.startY=Y`     | start standing at a point, with wide camera limits                                                                                                        |
| `-Dmmx.debug=true`                  | start with the F1 view on                                                                                                                                 |
| `-Dmmx.hold=BITS`                   | hold buttons by their X6 bits (1 right, 2 left, 4 up, 8 down, 16 sabre, 32 buster, 128 jump, 256 dash, 512 next weapon, 1024 previous weapon, 2048 Start) |
| `-Dmmx.jumpEvery=N`                 | with `mmx.hold`: pulse jump and the held non-direction buttons every N updates                                                                            |
| `-Dmmx.shot=FILE -Dmmx.shotAfter=N` | save a PNG after N updates and quit (a list such as `300,600` saves one PNG each); keyboard and controller are off, and a red border marks the window     |
