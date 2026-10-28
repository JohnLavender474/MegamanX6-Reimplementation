# Proposed changes outside `docs/stages/scaravich.md`

From the Ground Scaravich routing work (stage 5, overlay 08). Not applied.

## `catalog/savestates.json` (two new entries)

```json
{
 "name": "scaravich-1_start.state.gz",
 "note": "Zero (form 5) at the start of Ground Scaravich's stage (stage 5, area 0, checkpoint 0), difficulty 1, nightmare_effect 4",
 "sha1": "4b9cab59f2dd853bd5034ef66217cebb0de9b9bb"
},
{
 "name": "scaravich-2_totem-section.state.gz",
 "note": "Zero just after the first teleporter totem pole of Ground Scaravich's stage: section 2 (game+0x30 = 6, +0x31 = 1), area 0, checkpoint 0",
 "sha1": "4a226ab149b8d7615ddae50e76d304794ad53bb3"
}
```

## `catalog/helpers.json`

Add the entries of `helpers-proposed.json` (same format).

## `docs/engine.md`

1. "Stage overlays": change "stage 5 = 08" to certain: "[certain for 02, 03, 06, 07, 08, 09,
   10]". Stage 5 has one area (no area 1 list in `enemy_set_tbl` or `fixed_set_tbl`).
2. "Game state" table, add rows:
   - `+0x2F` | `0x800CCEFF` | stage 5: teleporter totem poles cleared (0–4) |
   - `+0x30` | `0x800CCF00` | stage 5: totem route row, `GetRNG() % 12` |
   - `+0x31` | `0x800CCF01` | stage 5: totem roll phase 0, 1, 2 |
   And a note: `+0x26`…`+0x35` are cleared by game mode 12 (`0x8001E81C`, the move to stage 15);
   `StageStartReset` does not clear them.
3. "The standard main-object update", the line on `GetRNG`: replace with
   "`GetRNG` (`0x8002D2D4`) returns 0–255 from a 16-bit state at `0x80090E70`:
   `hi = ((3s) & 0xFFFF) >> 8; lo = (s + hi) & 0xFF; s = lo | hi << 8`. `GetRNG1` (`0x8002D30C`)
   is the same step but returns 1 instead of 0. The state is 3383 at boot (`0x80012838`) and only
   calls move it (no step per frame). A demo recorder (`0x80021694`, `0x8002170C`) saves the state
   with `frame_count` and the game work [certain]."
4. "Warps inside an area": add "Stage 5: the teleporter totem poles and the section goals
   (`docs/stages/scaravich.md`)".
5. `game.exit_req` row: add "`0x41` in stages 1–8 → `HiddenAreaOf` (`0x80042F7C`): stage =
   (stage − 1) / 2 + 19, area = (stage − 1) & 1; `0x40` → area + 1 when area is 0 and
   `0x80071190`[stage] is 0, else the stage ends".

## `docs/enemies/teleport-crystal.md`

Open question 1: answered for `0x41` (the rule above). Add overlay 08 (stage 5) to the list of
overlays with the same object (entry `0x800EECFC`); stage 5 has one crystal, `id` 1, in section 7.

## `docs/TODO.md`

- Save states for stage 5: just before a pole touch; after a death in a section; after pole 3;
  the boss room.
- Stage 5 map with `learn` / `stages`.
- Stage 5 objects not read: main 15, 31, 37; stage logic 3, 14, 27, 29; the pole head attacks.

## `docs/process/process-log.md`

One entry: "Ground Scaravich routing: the totem poles (stage object 3) pick sections from a 12-row
table with two `GetRNG() % 12` rolls (pole 1 and pole 3); state in `game+0x2F`…`+0x31`; new doc
`docs/stages/scaravich.md`."
