# Notes: Nightmare shadow (Turtloid, main type 26)

Sheet: `docs/enemies/nightmare-shadow.md` in mmx-demake-tools. It was planned as a short sheet, but
the object has contact damage 6, HP 4 and a damage table, so it is not a pure effect. The full set
(sheet, notes, pseudo code, reconstructed C) is written.

## Method

1. Kit output in `kit/` (11 functions, entry `0x800EF004`, `turtloid-1.state.gz`). Read
   `summary.md`, `calls.txt`, `code.txt`, `data.txt`.
2. Export `~/mmx-data/x6/sprites/enemies/nightmare-shadow/anim-0..3.png`, scaled ×4: anim 0 X
   standing, anim 1 X dash, anim 2 Zero standing, anim 3 Zero dash. 0–1 magenta, 2–3 cyan.
3. Damage table `0x8007497C` read from `SLUS_013.95` (file offset `0x6517C`): only ids 7 (0, 4) and
   16 (0, 6) are not class 2. The kit dump of `data.txt` runs past the 160-byte table.
4. `0x800CD30A` = 0 in all save states. `0x80097420` grows across save states (`0x29C0C`,
   `0x94D35`, `0xAABAE`, `0x91D`): a frame counter [likely].
5. Stage list of the save state (`objects.read_list`, entry 6): 137 records; no kind 0–2 record with
   type 26; one kind 4 record with type 26 (a stage object, not this one).
6. Other `li 26` in overlay 09 (`0x800EAEBC`, `0x800F2CDC`, `0x800F47E0`, `0x800FA374`): none makes
   a main object of type 26 (`0x800F47E0` makes a sys object of type 26).

## Evidence

| file | contents |
|---|---|
| `kit/code.txt` | the 11 functions |
| `kit/data.txt` | boxes `0x800F6328`–`0x800F633B`, animation table `0x800F6350`, the state tables, the zone dispatch tables `0x800E9864` and `0x800E9894` |

## Key facts

- Flag 7 at `0x800CD30A`: `0x800EE9FC`.
- Zones: `x − lo <u n` tests, `0x800EEAF4`–`0x800EEB94` (and the copy at `0x800EED10`).
- Timer `(rng % 3 + 1) * 100 + 200`: `0x800EEBA8` (multiply by `0x55555556`).
- Spawn 32 px in front, y = player y: `0x800EEE20`–`0x800EEE5C`.
- `var` 10/11 by `mega+0x02`: `0x800EEE08`.
- Blink 40, stand 50: `0x800EEC0C`, `0x800EF218`, `0x800EF270`.
- Dash 4 px/frame: `rcos`/`rsin` (4096 = 1.0) shifted left 6 = `0x40000`.
- Off screen 128/128, only `var` 10/11: `0x800EF0E0`–`0x800EF110`.
- Death 60 frames, split ±1 px/frame: `0x800EF0B0`, `0x800EF124`.

## Checks (guide step 4)

- `SetAnime` with 0, 1, 2, 3: all in the 4-animation table. No event flag is tested.
- Units given for all numbers.
- All boxes in `data.txt`.
- Fields in the pseudo code match `summary.md` (`+0x7C` timer, `+0x80`, `+0x82`).

## Open questions

1. What places the spawners (`var` 0–5).
2. Which weapons are ids 7 and 16.
3. The writer of `0x800CD30A`.
4. The cyan palette of the Zero shadow in the export.
5. Whether a class 2 hit stops the shot.
