# Reverse-engineering kit (task J) — design

Date: 2026-10-20. Status: approved for planning by the user (spec, plan, review and build without
step-by-step approval; pilot on one enemy).

## Purpose

Make the work of writing the three documents for an enemy or object (behaviour sheet, pseudo code,
reconstructed C) faster and repeatable, by a person or by an agent:

1. `rekit`, a command that collects everything the game code holds for one object: the functions it
   runs, its state tables, the engine helpers it calls, the data it reads, its animations, and the
   object struct fields it touches. It writes plain text files that a person reads.
2. `catalog/helpers.json`, the shared list of names for engine functions, globals and object fields
   that earlier work found. The kit uses it to annotate the code.
3. `docs/re-guide.md`, the written procedure: how to run the kit, what to read in which order, the
   templates of the three documents, the checks, and the short sheet for pure effects.

The first four objects (Junkroid, Hover Gunner, Meta Dridler, D-1000) took most of their time on
finding code and on working out helpers again. The kit removes that part. Reading and writing stay
human (or agent) work.

## Decisions already made

- The kit and the guide work without an AI: a command line tool and a written procedure.
- Pure effects (rain, Nightmare shadows) get a short sheet only.
- The kit output is game data (disassembly, data bytes). It goes outside the repo, by default to
  `~/mmx-data/x6/re/<name>/kit/`. The repo gets only code, tests, names and docs.
- The MIPS disassembler that lives in [`~/mmx-data/x6/re/tools/mips.py`](../../../work/tools/mips.py) (written in this project, not
  copied) moves into the repo as a tested module. The old scripts stay where they are.
- Pilot: Meta Wheel F (intro, main type 2), the one intro enemy without documents.
- Out of scope: decompiling to C automatically; misc-object classes (not mapped); objects whose
  type table is unknown.

## Facts this design depends on

| Fact | Source | Status |
|---|---|---|
| Main update table: pointer at scratchpad `+0x80`, 77 entries; stage-object table at `+0x94`, 32 entries; visual table at `+0x84`; misc table at `+0x90` | earlier tasks; loops `0x800205A0`, `0x800206C0` | read in code |
| An object's update function is called with the object in `a0`; most functions copy it to an `s` register at the start (`addu s0, a0, zero`) | Junkroid, Hover Gunner, D-1000 code | read in code |
| State dispatch uses tables of code pointers in the overlay data (Junkroid states 0–3, steps 0–3), loaded with `lui` + `addiu` and called with `jalr` | Junkroid pseudo code | read in code |
| A shot or effect of an enemy is a visual object whose update function lies next to the enemy's code (Junkroid's shot: `0x800EB23C`–`0x800EB44C`, right after the robot) | Junkroid | read in code |
| Function end: the `jr ra` (plus delay slot) after which no earlier branch reaches further | `dfn.py`, used for all four enemies | used |
| Object struct fields and engine helpers already named | `names.py`, the four `pseudocode.md` files | read in code |

## Units

### `mips` (new, from the data-folder script)

- `disassemble(word, pc) -> str`: the same text format as today's `dis` (register names, branch
  targets as absolute addresses, `nop`).
- `branch_target(word, pc) -> int | None`, `jump_target(word, pc) -> int | None`.
- Coverage: the opcodes of today's table; unknown words print as `.word 0x…`.

### `codewalk` (new)

- `function_end(ram, start, limit=4000) -> int`: the address after the delay slot of the closing
  `jr ra`, by the rule in "Facts". Refuse (`WalkError`) when no end is found within `limit` words.
- `code_tables(ram, func, lo, hi) -> list[(addr, [targets])]`: inside one function, each `lui` +
  `addiu` constant inside the region that points at a run of at least two words that are all code
  addresses inside the region (a state table). The run ends at the first word that is not. After the
  walk, each table is cut where the next table address that the walk found begins (the game keeps
  its state tables back to back).
- `walk(ram, roots, lo, hi, helpers, limit=400) -> Graph`: from the root functions, follow `jal`,
  `j` to another function, and state tables. A function inside `[lo, hi)` is expanded; a call to an
  address with a helper name, or outside the region, is recorded as a leaf and not expanded. Stop
  with a note after `limit` functions. `Graph` holds functions (start, end), calls (caller → callee),
  tables (address → targets, user function) and the order of discovery.
- `neighbours(ram, scratch, graph, lo, hi, margin=0x400) -> list[(table, type, func)]`: entries of the
  visual, stage-object and misc tables whose function lies between the smallest start and the largest
  end + `margin` of the graph (objects place their shots after their own code; an entry just before
  belongs to the previous object, as the Junkroid shot before Meta Wheel F shows). They are walked as extra roots (likely the object's shots
  and effects) and listed as such.
- `references(ram, graph, lo, hi) -> list[(addr, [users])]`: the `lui` + `addiu`/load/store constants
  in the walked functions that are not functions: data addresses inside the region, and globals
  outside it.
- `self_fields(ram, func) -> dict[offset, {"read", "write"}]`: loads and stores on the object
  register (`a0`, and any register given `a0` by `addu`/`or` in the first 16 words), by offset.

### `rekit` (new, the command)

- Arguments: `--name <export folder name>`, or `--state <file> --type <n> [--class main|stage]`;
  `--dumps`, `--extracted`, `--catalog`, `--sprites` (default `~/mmx-data/x6/sprites`), `--out`
  (default `~/mmx-data/x6/re/<name or class-type>/kit`).
- With `--name`, the kit reads `<sprites>/enemies/<name>/animations.json` and takes the first source's
  state, type and class.
- Steps: read the state (RAM check as `spriteexport`), find the overlay, take the entry function from
  the class's table, `walk`, `neighbours` (walked too), `references`, `self_fields` per function, and
  the animations of each table the export names (`anims`).
- Writes to `<out>.tmp`, then renames (as the other commands); `outdir.check` refuses a folder inside
  the repo:
  - `summary.md`: object, stage, state and SHA-1, class and type, entry function, counts, the helper
    calls with their names and notes, the neighbour entries, the animation table with each animation
    (sprites, ticks, flags, loop), the fields used with read/write counts and their known names, and
    the open items (unnamed callees outside the region, the walk limit if reached).
  - `code.txt`: every walked function in address order, with a header (who calls it, which table
    holds it), each line `address  word  instruction  ; note`. Notes: helper names on calls, field
    names on object-register accesses, data and global names on constants.
  - `calls.txt`: the call tree from the entry and from each neighbour root, indented.
  - `data.txt`: each referenced data address with the functions that use it and a hex dump up to the
    next referenced address or 256 bytes.
- Exit codes: 0 done; 1 failed check; 2 bad arguments or a refused `--out`.

### `catalog/helpers.json` (new)

```json
{
  "functions": {"0x800179A4": {"name": "SetAnime", "note": "start animation n of the object's table"}},
  "globals": {"0x800970A0": {"name": "mega", "note": "the player object"}},
  "fields": {"0x5C": {"name": "hp", "note": "hit points"}}
}
```

Seeded from `names.py` and the four `pseudocode.md` files (names and one-line notes only).

### `docs/re-guide.md` (new)

The procedure, for a person or an agent:
1. Run `rekit`. Read `summary.md`, then `calls.txt`, then `code.txt` from the entry down.
2. Write the three documents with the templates (sections of `docs/enemies/junkroid.md`,
   `pseudocode.md`, `reconstructed/`), the confidence tags, and the units rules.
3. Checks: every animation the code starts exists in the table; every event flag the code tests
   appears in a frame; boxes and speeds as numbers with units; anything unread is listed as open.
4. Add new helper names to `catalog/helpers.json`.
5. Short sheet for pure effects: what it is, when it spawns, its animations, any damage.

## Testing

- `test_mips`: one word per instruction group gives the expected text; unknown words; branch and
  jump targets.
- `test_codewalk` (synthetic code from `spritefix` helpers): function end with an early `jr ra`
  inside a branch; a state table found and walked; a helper call not expanded; a call outside the
  region recorded; the walk limit; neighbours inside and outside the margin; data and global
  references; self fields through `s0`.
- `test_rekit`: a synthetic world gives the four files with the expected key lines; `--name`
  resolves through an export folder; `--out` inside the repo gives exit 2; a type with no entry gives
  exit 1.
- `test_retail_rekit` (skipped without `MMX6_DUMPS` and `MMX6_EXTRACTED`): Meta Wheel F from the
  intro start state: the walk reaches the function that stores its table `0x800FACD4`, the summary
  names `SetAnime`, and the animation section lists 11 animations.

## Pilot

Run the kit on Meta Wheel F, and write its three documents with the guide alone. Compare with the
earlier enemies: same sections, same level of detail. Note in the process log what the kit saved
and what was still manual.
