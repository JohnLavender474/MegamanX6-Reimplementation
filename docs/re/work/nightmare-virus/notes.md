# Notes: method, evidence, open questions

## Method

1. Kit output (already present): `python3 -m mmxdemake.rekit --name nightmare-virus` on
   `yammark-1_start.state.gz`. Entry `0x80041AB8` (main type 15), 77 functions, 13 state tables,
   2 neighbours (visual 12, misc 11).
2. Read `summary.md`, `calls.txt`, then all of `code.txt` from the entry, one function at a time.
3. Read `data.txt` for the boxes, the pattern table `0x80075EBC`, the row table `0x80075FE8`.
4. To answer "what happens to the Reploid", ran the kit on stage-object type 4 into the scratchpad
   (`--type 4 --class stage --out <scratch>`; entry `0x8004F834`) and read its init, main, the rescue
   test and the snatched state. The Rescue Reploid has its own folder, `rescue-reploid/`.
5. Searched the executable for other spawners of main type 15 (`GetMainObject` followed by
   `li ..., 15`): only the Reploid (`0x8004FF90`). Searched the overlays `rock_bin/*.bin`: 24 hits in
   the stage overlays (for example overlay 03 at file offset `0x814C`, RAM `0x800F19AC`).
6. Searched for users of the export table `0x8007789C` (the `nightmare-virus-orb` export).
7. Read the four save states for live data (`game` at `0x800CCED0`).

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt` | all virus, orb, image and ray functions |
| `kit/data.txt` | boxes `0x80075EDC`–`0x80075EE7`, `0x800760C4`; pattern `0x80075EBC`; row table `0x80075FE8`; tables `0x80075FD8`–`0x80076107` |
| scratchpad kit run of stage object 4 | Reploid code `0x8004E800`–`0x800500C0` (not kept; re-run with the command above) |

## Key facts and where they come from

- HP 10, Easy 2, Hard and up 18: `nv_init` `0x800404FC`–`0x80040530` (a branch on difficulty 1, then
  `< 2`, then `== 0` with `v0 = 2` in the delay slot). Same code in `revive_blink` `0x800425A0`.
- `id` high nibble switch: `nv_init` `0x800405D0` (`andi 0xF0`), cases 0x00, 0x10, 0x20, 0x30.
- Pattern rows: `data.txt` `0x80075EBC`: `02 04 08`, `02 03 08`, `02 03 04 03 08`, `02 02 08`.
- Snatch target: `nv_find_reploid` `0x80042D98`: base `0x800C1220`, end `+13120` (80 × `0xA4`),
  tests flags, type 4, display, `+0x07` == 1, squared distance < `0xF4240`.
- Write to the Reploid: `sb 2, 7(target)` at `0x80040DF4` and `0x80040F7C`.
- Reploid reaction: `0x8004EB58`: `+0x07` == 2 → status nibble 4 (`0x8004FF00`, at `game+0xD8`),
  state 6, step 2. Step 2 `0x8004EFD0`: pose, 130 frames, sound (2, 65), 60 frames flash. Step 3
  `0x8004FC40`: HP 13, contact 6, boxes. Step 5 `0x8004F144`: fires visual type 24 at anim flag 1.
  Weapon kill `0x8004FBA8`: main object 15 with `var` 0x31 at its position, status 3.
- The Reploid draws with sprite type 141 (`GetSpriteBank(141)` at `0x8004E8A8`), which links the
  legend's "type 141" to stage-object type 4.
- Soul values: `soul_start` (`0x8A` = 4, 200, 8), `soul_bob1` (8 → 4), `nv_init` (200, 100).
- Soul counter: `game + 0xD2 + 2 × mega.var` (`0x80040984`, `0x80042968`).
- Boss soul spawner (overlay 03, `0x800F1984`): `game.stage` ≠ 12: timer `0x800F7DE0` < 5401 →
  `var` 0x10 (200), else 0x11 (100). The same pattern is in the other boss overlays.
- `done` bits: `game + 0x178 + row × 32`, row = `0x80075FE8[stage × 2 + area]`.

## Verified against the save states

| state | `game.soul` (X) | `done` row 0 | Reploid nibbles (`+0xD8`) |
|---|---|---|---|
| `yammark-1_start` | 0 | `00` | all 0 |
| `yammark-2_boss` | 28 | `1F` (viruses 0–4) | ids 0,1,2,5,8,9,10,12,13,14 = 2 (saved), others 0 |
| `yammark-3_caterpirade` | 228 | `1F` | same |
| `turtloid-1` | 228 | (row 6 not read) | same |

- 28 = 2 × 8 + 3 × 4 fits five kills with five pickups (two fast, three late) [likely].
- 228 − 28 = 200: the boss soul of Yammark, beaten in under 5401 frames [likely].
- No live virus is in any save state, so no runtime check of the motion was possible.

## Kit problems

- `0x800760E8` read as one 6-entry table. It has 3 entries; `0x800760F4` is a separate table (the ray
  states) and `0x80076100` the ray steps. So `calls.txt` lists the ray code under misc 11.
- The ray object (type 13, class at `0x8009C9B0`, entry `0x80041B6C`) is not a neighbour entry; the
  function `0x80041B6C` was not walked (gap between `0x80041B6C` and `0x80041BA8` in `code.txt`).
- `0x8002C7F0` and `0x8002DCF4` are walked as object code; they are engine helpers.
- The export `nightmare-virus-orb` uses table `0x8007789C` "found by search". The virus code never
  uses it; 14 engine functions load it (for example `0x80049DF0`, next to `SpawnExplosion`). The real
  orb is animation 5 of `0x80075FB0`.
- In the Reploid run: `0x80097174` is named `mega.layer+0xc0`; it is `mega+0xD4`.

## Open questions

1. `mega+0xD5` and the bodies at `0x800CD340` / `0x800CD3F0` (size `0xB0`).
2. `game+0x0F` = 16 after the boss soul.
3. Whether the stage spawner reads the `done` bits (does a killed virus come back?).
4. The ray drawing colour codes 26–28 at `+0x34`.
5. Which code links the Nightmare Effect to the stage; the virus code reads no such flag.
