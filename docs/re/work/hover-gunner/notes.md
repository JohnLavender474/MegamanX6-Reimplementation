# Notes: small jet

Behaviour sheet: [`~/IdeaProjects/mmx-demake-tools/docs/enemies/hover-gunner.md`](../../findings/enemies/hover-gunner.md).

## Method

The same as for the green half-robot (`../junkroid/notes.md`): a RAM image of `SLUS_013.95`
plus overlay `02`, and the shared tools in `../tools/`:

- `dfn.py <addr>`: one function (stops at the last `jr ra`).
- `lin.py <from> <to>`: a linear listing; `; ====` marks each stack-frame set-up.
- `tbl.py <addr> <n>`: words and bytes.
- `names.py`: the engine names found so far; `dfn.py` and `lin.py` print them after `jal`.

Entry point: main table `0x800FA424` entry 5 = `0x800ECF3C`. From there: the state table, the
step table, each sub-step table, then the shot through `GetVisualObject` + type 4 (visual table
`0x800FA558` entry 4) and the flash through `GetMiscObject` + type 6 (misc table `0x800FA814`).

## A correction to an early reading

The jet's work bytes at `+0x80…+0x95` are read through the scratchpad word `0x1F80004C`. At first
this looked like a shared "squadron" block. It is not: the object loop at `0x8002000C` stores the
address of the current object there (`0x80020048`). So all those bytes belong to the jet itself.

## Evidence files (`disasm/`)

| file | contents |
|---|---|
| `jet_overlay02_800ec11c-800edd60.txt` | all jet, shot and flash code |
| `jet_data_800fad54-800faf08.txt` | animations, boxes, range table, remap table, state tables |
| `jet_jumptables_800e98a4-800e9aa4.txt` | the compiled `switch` tables (pose, sides, fan order, muzzle offsets) |
| `h_offscreen_aim_dirspeed_boxoverlap.txt` | `0x8002CAB4`, `0x8002CB50`, `0x8002D0D8`, `0x8002D380`, `0x8002D3B4`, `0x8002D654` |
| `engine_tables.txt` | the 9-entry direction speed tables; drop table 18 |

## Checked

- The box test (`0x8002D654`): boxes are corner + size; the corner is `x + ox` facing left and the
  far edge is `x − ox` facing right. This also confirms the robot sheet's reading.
- The direction convention: 0 = right, 8 = up, 16 = left, 24 = down, from `0x8002D3B4` (speed
  tables) and `0x8002D0D8` (signs), and it agrees with the pose cases (8 → pose "up", muzzle offset
  (0, −16)).
- The five jet records and their `kind` in the intro list.

## Open questions

1. The contact box `(0, 0, 11, 11)` covers only the lower-rear part of the hurt box. Check in play
   whether touching the front of the jet hurts X.
2. Frame rate 60 per second assumed (see the robot notes).
3. Timings are a hand simulation of the code order. A save state with a live jet would confirm them.
4. `var` 2, 3 and bit 7 are not used in the intro. Look for them in later stages.
5. Sound bank 2 effect 0: which sound is it?
6. Which sprite is which pose (sprites 2–15): check against the sprite sheet when sprites are
   extracted.
