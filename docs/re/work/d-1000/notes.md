# Notes: D-1000

Behaviour sheet: [`~/IdeaProjects/mmx-demake-tools/docs/enemies/d-1000.md`](../../findings/enemies/d-1000.md). Method and tools as in
`../hover-gunner/notes.md`.

Entry point: main table `0x800FA424` entry 48 = `0x800EFED0`. It splits on `var`: 0 = body (state
table `0x800FB198`), 1 = core (`0x800F1388`, state table `0x800FB280`). The body makes the core with
`GetMainObject` + type 48 + var 1. Projectiles and effects: visual type 1 (`0x800EF2EC`) with a
`var` jump table at `0x800E9A30`.

## Evidence files (`disasm/`)

| file | contents |
|---|---|
| `d1000_overlay02_800edd60-800f1720.txt` | all body, core and visual code |
| `d1000_data_800faf08-800fb2b8.txt` | boxes, animations, the animation table, all state and step tables, wall templates |
| `h_tilecopy_quake.txt` | `0x800262EC` (cell copy), `0x80027BA0` (block copy), `0x80029478` (quake) |
| `damage_tables.txt` | body table `0x80074B5C`, core shot table `0x8007429C`, guard table `0x8007433C` |

## Checked

- The boss HP: the HP bar maximum `GAME + 0x8C` is set to 32 at init, and the fill loop stops at it.
- The damage values per weapon id, from the table bytes (class 255 = deflect).
- The THINK choices, by reading the branch and delay-slot order twice.
- Main table entry 55 (`0x800F5170`) is a different object; D-1000 does not make it.

## Open questions

1. Weapon ids 18–22, 50–51: which shots they are (the robot notes infer 18–20 = full charge).
2. X's step 20 (the entry trigger), engine objects 24 and 26, and what happens in the death scene.
3. The RAM wind-up height tracking: how far does the body move in play?
4. The animation contents (24 animations at `0x800FB138`) are dumped but not matched to sprites.
5. Frame rate 60 per second assumed.
