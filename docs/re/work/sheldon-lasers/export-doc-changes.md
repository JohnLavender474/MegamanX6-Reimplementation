# Proposed changes after the laser sprite export

Files that the sprite agent may not edit. The main session decides.

## `catalog/enemies.json` (`names`)

| ID | proposed name | what |
|---|---|---|
| `enemy-07d032a495` | `laser-reflector-puzzle` | puzzle parts, main type 13 (as before) |
| `enemy-e5920c31ed` | `laser-reflector-puzzle-laser` | laser, misc 7 / visual 11, entry 16, 250 sprites, sprite-only |
| `enemy-3c4d6ef799` | `hidden-glass-floor` | Sheldon main type 32, bank 11, 2 sprites (0 glass floor, 1 brick block), sprite-only |
| `enemy-250854b686` | `d-1000-extra` (or merge with the D-1000 sheet) | intro visual type 1, entry 16 of the intro, 49 sprites (D-1000 body frames, shots, blue beam), sprite-only |

## `docs/enemies/laser-reflector-puzzle.md`, "Open questions" item 4

Replace with: "The rounding of the last body piece length (`0x800EBCD8`, `0x800EC76C`..) was not
read in full." The sprites are now exported (section "Laser sprites").

## `docs/TODO.md`

- Laser puzzle line: remove "the laser sprites 210–245 (second sprite source)"; done
  (`enemy-e5920c31ed`).
- Mark done: "`spriteexport`: an object can store its state table in `+0x30` (Sheldon main type
  32) ...". The tool now makes a sprite-only export when every word of the table is a function
  start (`enemy-3c4d6ef799`).
- New: `rekit --name` reads `sources[*].table`; a sprite-only export of an extra source (misc /
  visual) has no `table`, so `rekit --name` on it fails with a `KeyError`. Use `--type`/`--at`, or
  teach `rekit.resolve` the misc and visual classes.
- New: look at what else the intro entry 16 holds (49 sprites, D-1000 parts) and whether
  `d-1000` / `d-1000-parts` should take it.

## `docs/engine.md`

- Object classes / sprites: "`sprite_bank_base` (scratch `+0x20`) can have one entry more than the
  banks of the type list: 17 entries for 16 banks in stages 0, 6, 7 and 22; no extra entry in
  stage 21 (18 for 18) and stage 1 (16 for 16). In stage 7 and stage 22 entry 16 is the laser of the puzzle (250 sprites); in the
  intro it holds 49 extra D-1000 sprites (visual type 1). An object that uses it takes the data
  from the entry and the texture and palette from another object (the laser: its parent)."
  [certain for the code of stage 7 `0x800EAC78`; the count per stage observed in 26 states]
- Drawing: "An object with `flip` `0x40` is drawn mirrored about its x (pixel offset `rx` goes to
  `−rx − 1`)" [likely: the laser pieces join only so in 3 states; no frame buffer in the states].
- Save-state VRAM: the frame buffer area (x 0–319) is black in `sheldon-7_lasers`, so a drawing
  cannot be compared with the screen from a save state.

## `docs/process/process-log.md`

Section text: "Laser sprites. Read the laser draw code (`0x800EB0F0` draw part, `0x800EBCD8`,
`0x800ECEE4`). Pieces take data from `sprite_bank_base` entry 16 and texture / palette from the
parent. Added two sprite-only rules to `spriteexport` (extra sprite source; state table). Checked:
old and new code on all 26 dumps into two new roots; every old file byte-identical; only 3 new
export folders, 4 new `entry-16-live-0.png` sheets and the stage indexes changed. Probes:
`re/sheldon-lasers/probes/laser_sources.py`, `source_survey.py`, `laser_screen.py`,
`laser_scene.py`, `vram_png.py`, `export_all.sh`."
