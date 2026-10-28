# Nightmare Mother: notes

State: `gate-2_nightmare-mother.state.gz` (SHA-1 `ea402436237a753f57ec7e0561ee2e2e12fd31db`).
User's words: "slot 2, Zero is fighting against the Nightmare Mother boss and just got damaged".

## Method

- `scripts/overlays.py`: RAM at `0x800E9860` = `rock_bin/22.bin` (11 of 81 224 bytes differ).
  Nothing matches at `0x800FA000`: 22.bin covers it (ends `0x800FD5A8`). No boss overlay.
- `scripts/objs.py`: all live objects. Main slots 0–4 are type 60 `var` 0–4.
- Kit: `python3 -m mmxdemake.rekit --state gate-2_nightmare-mother.state.gz --type 60 --name
  nightmare-mother` → `kit/` (88 functions).
- The `mmx6` decompilation (`src/shared/rock_22/func_800F*.c`, AGPL) was read for facts only, to
  speed up the reading; every fact in the sheet was checked against `kit/code.txt`. Functions
  with no C (`0x800F5C00`, `0x800F90D4`, `0x800F921C`, `0x800F9308`, `0x800F9410`, `0x800F9498`,
  `0x800F9584`, `0x800F97A4`, `0x800F97F0`, `0x800F9A8C`, `0x800F9DA4`, `0x800F9E1C`,
  `0x800F9F74`, the visual 30 dispatchers) were read from the kit only.
- `scripts/dmg.py`: damage tables (RAM equals the executable for both tables).
- `scripts/paths.py`: the PATH tables and the fireball speed tables.
- `scripts/floorfire.py`: draws bank 15 with texture `0x4A0` (shapes only; wrong colours).
- `scripts/preview.py`: the preview picture of the state is a placeholder (black, a "0").

## Live values (`gate-2`)

| object | values |
|---|---|
| game | stage 16 area 0, checkpoint 4, difficulty 1 (Normal), boss level 2, `boss_hp_max` 124, `game+0x20` = core, `game+0x24` = 1, `boss_choice` 2, `game+0x5F` `0x17`, `game+0x60` `0xFF` |
| core (slot 0) | state 1 step 10 sub 1, (8200, 192), HP 124, `+0x7C` = 2 (mode), `+0x07` 0, `+0x8D` 0, sprite 183 |
| shell `var` 1 (slot 1) | step 6 sub 0, (8080, 96) quadrant 0, x speed −4.0, timer 300, pause `+0x8E` 9, `+0x93` 96, HP 62, contact 6, table `dmg_tbl_deflect`, sprite 3 |
| shell `var` 2 (slot 2) | step 6 sub 0, (8320, 288) quadrant 3, speed 0, timer 300, pause 9, `+0x93` 144, HP 62 |
| eyes `var` 3, 4 | step 4 sub 0, on their shells, HP 62 each, contact 4, table `0x800757DC`, no hurt box, sprite 126 |
| level variables | `0x800FD59A` 60, `0x800FD59C` 3, `0x800FD5A0` 0x40000, `0x800FD5A4` 300 |
| camera | limits x 7984–8096, y 90–144; targets 8096 / 7984 / 144 / 16; `+0x47` 2, `+0x48` 8 |
| Zero | step 2, (8152, 330), HP 42 = max (`game+0x5B` + 1 = 42), `+0x61` 0, `+0xA4` 0, `+0x63` 0, `no_hit` 0, parts `0x00C00000`, `game+0x98` 168 |

The live state agrees with "about 32 updates after the intro end": the 40-update first pause is
at 9 and the slide timer has not started.

## The hit on Zero: the state does not show one [certain]

Zero has HP 42 of 42, is in step 2 (stand), and `+0x61` (invulnerability), `+0xA4` (hit lock),
`+0x63` (hit class) and HP bit 7 are all 0. No visual object is live, and Zero (8152, 330) is
outside every attack box (shell `var` 2 box x 8256–8384). The boss is also at full HP (124). So
the save is not just after a hit; nothing new for `docs/damage.md` or `docs/players/`. What a hit
here would do (by the docs, Normal, no halving): shell 6 → class 2 (heavy, 100 updates of
invulnerability); eye, fireball, orb 4 → class 1 (75 updates); spark ball, shell shot 6 → class 2;
floor fire, bolt 8 → class 2.

`playerexport` gives the same Zero export as the shared one (only the source state differs), so
it is not kept in `sprites-root` (the shared copy was put back).

## Path table (PATH, step 8)

Row = `0x800FD4E4`[q1 + 4 × q2]; R L U D = right, left, up, down; − = stay. Columns: mode 0–5.
Pairs of same quadrant give −1 (no row): a hazard (index −48 bytes), but the shells can not end in
the same quadrant [likely].

```
var1 table 0x800FD040
 row  0 (var1 TL, var2 TR): - D D R D -
 row  1 (var1 BL, var2 BR): U - U R - U
 row  2 (var1 TL, var2 BL): R D - R R -
 row  3 (var1 TR, var2 BR): L D L - - L
 row  4 (var1 BL, var2 TR): U - - R - R
 row  5 (var1 TL, var2 BR): - D - R R -
 row  6 (var1 TR, var2 TL): - D L D - D
 row  7 (var1 BR, var2 BL): U - L U U -
 row  8 (var1 BL, var2 TL): U R - R - R
 row  9 (var1 BR, var2 TR): U L L - L -
 row 10 (var1 TR, var2 BL): - D L - - L
 row 11 (var1 BR, var2 TL): U - L - L -
var2 table 0x800FD280
 row  0 (var1 TL, var2 TR): - D L D - D
 row  1 (var1 BL, var2 BR): U - L U U -
 row  2 (var1 TL, var2 BL): U R - R - R
 row  3 (var1 TR, var2 BR): U L L - L -
 row  4 (var1 BL, var2 TR): - D L - - L
 row  5 (var1 TL, var2 BR): U - L - L -
 row  6 (var1 TR, var2 TL): - D D R D -
 row  7 (var1 BR, var2 BL): U - U R - U
 row  8 (var1 BL, var2 TL): R D - R R -
 row  9 (var1 BR, var2 TR): L D L - - L
 row 10 (var1 TR, var2 BL): U - - R - R
 row 11 (var1 BR, var2 TL): - D - R R -
```

## Hazards and oddities

- Shell damage table written twice in a row (`0x800F61B4`/`0x800F61C0` then `0x800F61CC`): the
  `0x8007587C` write is dead. A reimplementation: shells are invulnerable.
- `0x800F7DC0` writes core `+0x90`/`+0x91`, the next WALL SLIDE overwrites them before
  `0x800FA01C` reads them: dead code.
- The core's main state calls `ContactMega` with no attack box (harmless) and `ContactWeapon` with
  no hurt box.
- The visual main (`0x800FA848`) calls `ContactMega`, and steps 0, 1 and 3 call it again: two
  calls per update (the second finds HP bit 7 or no overlap).
- Level 3: bar max 125, eye sum 124.
- Core `+0x8E` is decremented (`0x800FA01C`) and tested (`0x800F74B8`) but never set: always 0.
- Core `+0x93` = `mega+0xA4` each update in CYCLE sub 1; no reader found.

## Exports (in `sprites-root`, on a copy of `sprites/`)

- `enemies/enemy-c99ffb5c2b/` (type 60, live, bank 14): good, name `nightmare-mother`. Animations
  0–45 good; 46–51 wrong (46, 47 = table `0x800FCF50` drawn with bank 14 sprites; 48–51 = state
  table words). The tool takes the table up to the next code-used address; a `tables` entry or a
  cut at `0x800FCF50` would fix it.
- `enemies/enemy-9b63092464/`, `enemies/enemy-71ce84488a/` (type 15, Nightmare Virus tables
  `0x80075FB0` and `0x8007789C`, search, bank 1): **wrong**. Bank 1's cells hold other graphics
  (orange and red pieces); delete and list as skipped.
- `banks/stage-16/gate-2_nightmare-mother/`: `bank-14-live-0.png` good (216 sprites);
  `bank-1.png` wrong (same reason); `bank-0.png` (misc 128) and `stage-8-live-0.png` good.
- `enemies/boss-gate/animations.json`: one new source (stage 16, type 8), images the same.
- `stages/stage-16.json`: skipped types 6, 18, 13, 33, 34 ("cells overwritten by type 60") and 0.
  Add type 15 there by hand.
