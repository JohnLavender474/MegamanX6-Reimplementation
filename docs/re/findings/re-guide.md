# Guide: reverse-engineering one enemy or object

This guide is for a person. An agent follows the same steps. The result is three documents for one
object, written the same way as the first four (Junkroid, Hover Gunner, Meta Dridler, D-1000).

## Before you start

You need:

- the extracted game files (`~/mmx-data/x6/extracted/retail`) and a save state of the stage area
  (`~/mmx-data/x6/dumps/`);
- an export of the object from `python3 -m mmxdemake.spriteexport` (its folder name in
  `~/mmx-data/x6/sprites/enemies/` is the object's name below);
- the official name, from the Mega Man Knowledge Base list of X6 enemies (names only).

Game data stays outside the repo. Only the behaviour sheet goes into the repo.

| Document | Where |
|---|---|
| behaviour sheet | `docs/enemies/<name>.md` (repo) |
| notes, pseudo code, reconstructed C, kit output | `~/mmx-data/x6/re/<name>/` (outside the repo) |

## Step 1: run the kit

```
python3 -m mmxdemake.rekit --name <name>
```

Without an export: `--name <name> --state <file> --type <n> [--class <class>]`; then the name only
names the output folder. The classes are `main`, `stage`, `visual`, `effect`, `logic` (stage logic),
`misc`, `kind7` and `actor2` (see "Object classes" in `docs/engine.md`). A sprite-only export of a
`visual` or `misc` source has no animation table; the kit uses the class and type of the export.
The output is in
`~/mmx-data/x6/re/<name>/kit/`:

| File | Contents |
|---|---|
| `summary.md` | the entry function, the engine helpers it calls (with notes), state tables, switch tables, neighbour entries (likely shots and effects), the animation table, the object fields it uses, globals and data |
| `code.txt` | every function of the object, annotated: helper names on calls, field names on object accesses, names on constants |
| `calls.txt` | the call tree from the entry and from each neighbour |
| `data.txt` | a hex dump of each data address the code uses, and the cases of each switch table |

### Finding the type number

Without an export, take the type from `~/mmx-data/x6/sprites/stages/<stage>.json` (the skipped
types are listed there with a reason) or from the "Object types by sprite bank" table in
`docs/objects-legend.md`. Main types are 0–76; stage objects (`--class stage`) are 0–31; visual
0–57, effect 0–39, stage logic 0–44, misc 0–63, `kind7` and `actor2` 0–4. A type outside the range
stops the kit with exit 2.

### When the kit stops with exit 1

The message says why:

- `... is an empty slot`: that type has no object in this stage (its entry is a stub). Pick another.
- `... has no entry function`: the type is not used in this stage. Use a save state of a stage that
  has the object.
- `... is not a function start in this overlay`: the `kind7` and `actor2` tables are in the main
  program and hold the entries of all stages; that type belongs to another stage overlay.
- `no end found for the function at ...`: the walk ran into data. Look at that address with `--at`
  (below) and note it as an open question.
- `... overlays match RAM`: the save state is not from a stage, or the extracted files are missing.

### Looking at code the kit did not walk

The summary lists "Calls to unnamed code outside the walk". To read one of them:

```
python3 -m mmxdemake.rekit --state <file> --at 0x80021760
```

It prints that function, with helper names on its calls and the cases of its switch tables. Add a
name to `catalog/helpers.json` when you know what it does (see "Names in `catalog/helpers.json`"
below).

## Reading the output

- **`calls.txt`:** `(see above)` means the function was already listed higher up, with its calls.
  `(not walked)` means a call to unnamed code outside the object's region: read it with `--at`.
- **Delay slots:** the instruction right after a jump, branch or call runs before the jump takes
  effect. So in `jal SetAnime` / `addiu a1,zero,2` the second line sets the argument.
- **Constants:** the compiler builds a 32-bit address in two steps, `lui` (the upper 16 bits) and
  `addiu` or a load (the lower 16 bits, **signed**). So `lui v0,0x8010` + `addiu v0,v0,-21292` is
  `0x800FACD4`. The kit shows the result as the note on the second line.
- **Field names** (`; hp`, `; speed_x`) use the main-object layout. In functions marked
  `neighbour code`, the offsets are shown raw (`self+0x50`), because shots and effects use some
  offsets for other things (the stud overlay of Meta Wheel F keeps its parent at `+0x50`).
- **Neighbour entries** are likely, not certain: check in the code that the object really makes them
  (for example a `GetMiscObject` followed by `sb ..., 1(v0)` with the type number). The kit looks in
  the visual, effect, misc and stage-object tables; the entries of other main types, stage objects
  and stage-logic objects mark the end of the object's code.
- **Switch tables** (a `jr` through a table, after an `sltiu` bound): the table is listed under
  "Switch tables", `code.txt` marks each case label (`; case 3 of switch 0x800e9864`) and `data.txt`
  lists the cases. The case number is the table index; when the code subtracts a value before the
  `sltiu`, add it back.
- **State tables** end at the next table that the walked code uses, or at an address that other code
  in the same region uses (the start of another object's table).
- **Overlay names:** a function named in `catalog/helpers.json` for one overlay is part of the
  object's code: the kit walks it and shows its name on the `; ====` line. Engine helpers (names for
  all overlays) are not walked; they are listed under "Engine helpers called".
- **Animation tables** are cut where the export ends, or (without an export) where an animation uses
  a sprite that the sprite set does not have. The note under the table says which. Without an export,
  a table that no live object uses has the line "Found by the code search".

## Step 2: read, in this order

1. `summary.md`, all of it. Note the state tables: each entry is one state or sub-state.
2. `calls.txt`: which function runs which.
3. `code.txt`, from the entry down. For each function, write one line: what it does. Then read
   each state function in full.
4. `data.txt` for every constant the code uses: boxes (4 bytes `{s8 ox, s8 oy, u8 w, u8 h}`; in a
   terrain box (`+0x68`) `(ox, oy)` is the centre and `w`, `h` are half sizes), damage tables, speed
   values.

Units, conventions and confidence tags are in `docs/engine.md` ("Conventions"). Read that page once
before the first object.

## Step 3: write the three documents

### Behaviour sheet (`docs/enemies/<name>.md`)

The sheet has only what is particular to this object. The shared rules (units, confidence tags, box
format, the standard update, spawning, damage tables, weapon ids, drop tables, the player's damage
scaling) are in `docs/engine.md`. Write "standard" or the table name and link there; do not copy the
rule. Records and positions go in `docs/stages/<stage>.md`, not in the sheet. Working notes (how a
number was found, step-by-step simulations) go in `notes.md`; the sheet gives the result.

Sections (keep the order; leave out a section that does not apply):

1. Title: `# <Official name> (stage <nn>, <class> object type <n>): behaviour sheet`, with `<nn>` the
   overlay number. One paragraph: what it is. One paragraph: overlay, save state, the kit command,
   and the links to `docs/engine.md` and the stage page.
2. Spawn and set-up: HP, contact damage, boxes, draw priority, what is not standard.
3. The `id` parameter: what the record `id` changes.
4. State machine: the state table addresses (one line when the states are standard), the main
   routine in order, then one sub-section per step, with its timing.
5. Animations: the table from `summary.md`, with what each animation is.
6. Shots and effects (the neighbour entries), each with speed, damage and box. "Copies the parent"
   and "standard shot ends" link to `docs/engine.md`.
7. Damage: the table name and the hits to kill; only the ids that matter for this object. Damage
   dealt by each source, before the difficulty scaling. A table that `docs/engine.md` does not have
   yet goes there in full: read it from the executable (file offset = RAM address − `0x80010000` +
   `0x800`, 80 ids of 2 bytes) and name it in `catalog/helpers.json`.
8. Death: explosions, debris, the drop table number.
9. Sounds.
10. Summary for a reimplementation (5–10 lines).
11. Open questions about this object. Engine questions go in `docs/engine.md`.

### Notes and pseudo code (`~/mmx-data/x6/re/<name>/`)

- `notes.md`: the method (kit run, files read), the evidence files, the key facts and where they come
  from, what was checked against the save state, the open questions.
- `pseudocode.md`: the dispatch tables, then one section per function (`### name — 0x<address>
  (state n)`) in readable pseudo code; then "Engine helpers called" and "Object struct (the fields
  used here)" tables.

### Reconstructed C (`~/mmx-data/x6/re/<name>/reconstructed/`)

- `README.md`: what is certain, what is a guess, the file layout. Say that it is a plausible
  reconstruction, not original code, not compiled.
- `include/object.h`, `include/game.h`, `include/sound.h`: copy from an earlier enemy and add what
  this one needs.
- `src/st<nn>/m<type>_<name>.c`: one function per game function, in the same order, with the same
  constants.

### Hazards: write them down twice

The reconstructed C is the one place where code comments are wanted. Mark each **hazard** with an
inline comment at the line, and describe it in `notes.md` under "Recursion and re-entry" or
"Hazards". A hazard is code whose safety depends on something that the code does not test, for
example:

- **Recursion or re-entry:** a function that calls itself, directly or through a helper (a step that
  starts a new move and runs the step again). Say what ends it, with the address of the call. Say
  whether the game does it too, or whether it is only in the reconstruction (code that the game has
  inline).
- **Data that keeps it safe:** a loop or a call chain that ends only because of a value in a table or
  an animation frame (an event flag, a hold frame, an end marker). Quote the data and say what would
  break if the data changed.
- **Missing checks:** an index with no bound test, a pointer that can be 0 (an object with no damage
  table reads RAM address 0), a counter that can wrap, a branch the code never reaches.
- **Order:** a result that depends on the order of the update loops, or on two objects writing the
  same global (shared box globals).

Each comment says what the hazard is and why it is safe in the game. In `notes.md`, also say what a
reimplementation should do instead. To find recursion, run
`python3 ~/mmx-data/x6/re/tools/find_recursion.py`: it lists the self calls and the call
cycles of up to three functions in all reconstructed C.

## Step 4: checks

- Every animation that the code starts (`SetAnime(obj, n)`) exists in the table, and every event
  flag the code tests appears in a frame.
- Every number has a unit (px, px/frame, frames).
- Every box and table that the sheet quotes is in `data.txt`.
- Anything not read is listed under open questions, not guessed silently.
- The fields table matches `summary.md`.

## Step 5: share what you found

- Add new engine function, global and field names to `catalog/helpers.json` (name and a one-line
  note). The next kit run uses them. Give an overlay address its overlay (see below).
- Add new engine rules to `docs/engine.md` (and the drop table's user to its row), and the object's
  records to `docs/stages/<stage>.md`.
- Add a line to the process log (`docs/process/process-log.md`).

## Names in `catalog/helpers.json`

One entry per line, sorted by address: `"0x80016C48": {"name": "PlaySound", "note": "..."}`. An
address in an overlay (`0x800E9860` and up: the stage overlays, the boss overlays at `0x800FA000`,
the player overlays at `0x801EA000`) means other code in another overlay, so give it the overlay
number:

```
"0x800F765C": {"name": "SheldonTurret", "note": "...", "overlay": 10},
"0x800E9878": {"name": "PressBlockInit", "note": "...", "overlay": [6, 7]},
```

The kit uses the name only when that overlay is loaded (stage overlay, boss overlay or player
overlay of the save state). To name the same address in another overlay, add an `alt` list:
`"alt": [{"overlay": 7, "name": "...", "note": "..."}]`. An entry without `overlay` applies in all
stages (engine code and data in the main program).

## Short sheet (pure effects only)

For an object that only shows something (rain, shadows, splashes), write only
`docs/enemies/<name>.md` with:

1. What it is and when it appears.
2. Its animations (from `summary.md`).
3. Whether it can hurt the player, and how much.
4. Open questions.
