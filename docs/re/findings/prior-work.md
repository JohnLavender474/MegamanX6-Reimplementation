# Prior work and credits

This project stands on the work of other people. This page names each source,
what it gave us, and how we used it. Use it as the basis for credits in any
write-up.

**Rule for every source below:** we read them for facts (formats, addresses,
names, hashes) and checked those facts against our own data. We never copied
code or text from them into this repo or into Mega Man Maverick. Some have
copyleft or non-commercial licences, and two have no licence at all.

Read-only clones are in `~/IdeaProjects/mmx-reference/`. The details of each
use are in `docs/process/process-log.md` (section numbers below).

## The gap this project fills

Each project below does well what it was built for: identifying the disc,
rebuilding or running the game's code, modding, or editing X4. None was built
to do what a demake needs:

- **extract X6 stage data** (layouts, collision, art with palettes, object
  placements) from the disc into engine-neutral files;
- **say what the data means**: which object is which enemy, item, gate or
  camera point; which collision value is a ladder or a gate;
- **describe behaviour in numbers** (states, timers, ranges, speeds, damage) that
  can be rebuilt in another engine;
- and do it **under licences that allow the result to be used** in Mega Man
  Maverick (MIT). Two sources are AGPL, one is non-commercial only, and two have
  no licence.

So this project took facts from them as leads, checked each fact against the
disc and save states, and built the rest.

## Disc identity

### Redump

- **What:** the preservation database of verified disc dumps.
- **Used for:** the reference SHA-1 of the Mega Man X6 USA v1.1 (SLUS-01395,
  "Rev 1") data track: `d4f7e08371027a87a3bf13311db5a4c56733f4ea`. Every
  extraction is checked against it (section 6).
- **Not enough on its own because:** it identifies the disc; it says nothing
  about what is inside.

## Mega Man X6

### mmx6: Mega Man X6 decompilation

- **Who / where:** Ivan Anzaldo, <https://github.com/anzaldoivan/mmx6>.
- **Licence:** AGPL-3.0.
- **What it is:** a matching decompilation in progress. Its aim is C source that
  rebuilds the game's executable and code overlays byte for byte. State of the
  snapshot we read (2026-10-16): about 3,480 machine-generated C drafts (m2c
  output, raw offsets, no names); the build still uses about 2,450 assembly
  placeholders. Its docs cover the disc and archive formats, memory maps and
  prior art.
- **Used for:**
  - the file manifest and hashes (`manifest/retail.jsonl`), our oracle for task A
    (7 of 9 files matched; sections 7, 8, 12);
  - the `ROCK_X6.BIN` overlay archive format, and the advice to take the XA
    Form 2 flag from the directory record (`docs/formats.md`; sections 5, 7, 10);
  - confidence ratings that mark some community RAM addresses as leads, not
    facts (`docs/prior-art.md`; section 7);
  - the program load address and overlay load addresses (memory map;
    sections 34, 36);
  - its draft C for Junkroid's routines, to check our own reading of the
    machine code (section 49).
- **Not enough on its own because:**
  - its C is machine-generated first drafts: fields are raw offsets
    (`M2C_FIELD(arg0, u8*, 0x15)`), with no names, types or meaning. Reading an
    enemy's behaviour from it still needs the work this project did: naming the
    fields, finding the state tables, converting units and checking in play;
  - it is early: most functions are still assembly in the build, and the
    snapshot is a single commit;
  - it is about code, not data: its docs listed `ROCK_X6.DAT`, where the stage
    data lives, as "Unknown", and say nothing about layouts, object lists,
    palettes or collision;
  - even when finished, a matching decompilation tells what the code does, not
    what each object is in the game;
  - AGPL-3.0: its code cannot go into Maverick (MIT).
- **How it differs from this project:** mmx6 recovers the exact code; this
  project recovers the stage data (layouts, objects, palettes, collision) and
  what the objects mean and do, for a demake. The two complement each other.

### MegaManX6Recomp

- **Who / where:** Matt Stanley, <https://github.com/mstan/MegaManX6Recomp>.
- **Licence:** PolyForm Noncommercial 1.0.0.
- **What it is:** a static recompilation of X6 for PC (with X + Zero co-op).
- **Used for:** the expected disc release and file names (`DISC.md`,
  `game.toml`), which confirmed that Redump "Rev 1" is the v1.1 that the tools
  expect (section 6); its debug server's `read_ram` command, as a lead for RAM
  dumps (section 5).
- **Not enough on its own because:** it makes the game run on a PC; it does not
  extract or document stage data, object meanings or enemy behaviour. Its
  non-commercial licence also rules out reuse of its code.

### MegaManX6_PS1_Modding

- **Who / where:** Kuumba123 (PogChampGuy),
  <https://github.com/Kuumba123/MegaManX6_PS1_Modding>.
- **Licence:** none given (all rights reserved).
- **Used for:** RAM symbols for the stage layout: `layoutWidth = 0x800cd338`,
  `layoutHeight = 0x800cd339`, `layoutSize = 0x8008ec10`,
  `bgLayers = 0x800971f8` (`symbols/layer.txt`; a lead in section 5, confirmed
  with our save state in section 33); the name `stageVar` for the object field
  that holds a record's `id` byte (`include/object.h`; the Junkroid investigation,
  section 48).
- **Not enough on its own because:** it is a small set of symbols and headers
  for modders, without the evidence behind each address (mmx6 rates many of
  them as leads, not facts). It does not describe the stage, object or palette
  formats. No licence.

## Mega Man X4 (same engine family)

### TeheManX4 Editor

- **Who / where:** Kuumba123 (PogChampGuy),
  <https://github.com/Kuumba123/TeheManX4_Editor>.
- **Licence:** none given (all rights reserved).
- **What it is:** a level, sprite and text editor for Mega Man X4.
- **Used for:** the stage archive entry types that X6 shares with X4 (`0` screens,
  `1` tile info, `0x10000` textures), the tile info bit layout, and the 4 bpp /
  8 bpp texture page layout (`Level.cs`; sections 19, 25); PCSX-Redux RAM
  access, as a lead (`Redux.cs`; section 5).
- **Not enough on its own because:** it is for X4, not X6. The shared formats
  carried over, but the rest differs: X4 keeps its stage layouts in the
  executable and its palettes in separate files, while in X6 we had to find the
  layout through a save state and the palettes in video memory. It is an
  interactive editor, not an extraction pipeline. No licence.

### mmx4: Mega Man X4 decompilation

- **Who / where:** sozud, <https://github.com/sozud/mmx4>.
- **Licence:** AGPL-3.0.
- **Used for:** how X4 spawns stage objects from the per-stage list and copies
  the record bytes into the live object, and how an X4 enemy turns to face the
  player; these confirmed our reading of the X6 Junkroid (sections 48, 49).
- **Not enough on its own because:** it is X4: the engine is related, but the
  enemies, stages and addresses are different. It shows how the engine works,
  not what X6 contains. AGPL-3.0.

## Enemy names

### Mega Man Knowledge Base (Fandom)

- **Where:** "List of Mega Man X6 enemies",
  <https://megaman.fandom.com/wiki/List_of_Mega_Man_X6_enemies>.
- **Licence:** CC BY-SA (Fandom wiki text).
- **Used for:** the official names of the intro enemies: Junkroid (the green,
  legless robot), Hover Gunner (the small jet), Meta Wheel F (the pink/purple
  wheel), Meta Dridler (the drill) and D-1000 (the intro boss). Our file and
  folder names use these names (`docs/enemies/junkroid.md`,
  `docs/enemies/hover-gunner.md`, …). Only the names were taken, no text.
- **Not enough on its own because:** it names and describes the enemies for
  players; it has no numbers (HP, timers, ranges, speeds) and no placements.

## Assets on the Maverick side

- **burailaplace:** the 8-bit Commander Yammark tileset already in the Mega Man
  Maverick repo (section 1). Credit is due if the converted stage uses it.

## Still to credit

- The gameplay video that showed Junkroid 27's fixed facing (section 47): add its
  title and author here.
