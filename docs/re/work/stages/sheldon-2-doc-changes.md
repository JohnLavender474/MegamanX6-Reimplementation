# Proposed changes from the second Shield Sheldon rip (2026-10-21)

These are for files that this work stream does not edit. The facts are in
`docs/stages/sheldon.md` ("The lower part") and `docs/stages/sheldon-alt.md` (stage 22).

## `catalog/enemies.json`

| folder | proposed name | why |
|---|---|---|
| `enemy-4cfc820dcf` | `light-capsule` | stage type 26, live in `sheldon-9_lower-2` (record 42, `var` 1, and a second object `var` 16 with no record); 20 animations: the closed capsule, the opening, and the hologram of Dr. Light. Clean export. |

Do not name `enemy-9504756be1` (type 13, a search export in `sheldon-12_high-max`, where the High
Max texture covers banks 4–10; the sprites are not clean).

## `catalog/helpers.json`

See `~/mmx-data/x6/stages/sheldon-2-helpers-proposed.json` (two functions, two globals).

## `docs/TODO.md`, collision legend

Add a row (value 46 was in the table only as "rope is 46" in the 45 row):

| Value | Cells | Reading so far | Confirmed by |
|---|---|---|---|
| 46 | 6 (stage 22) | rope: one column at x 2016–2031, y 352–447 of the Sheldon hidden area, between hanging bars | code (`docs/players/air.md`); first stage map with it |

Add to the 45 row: "62 cells in stage 22 (Sheldon hidden area): one-cell bar lines over a spike
floor (63)".

## `docs/TODO.md`, other items

- [x] Shield Sheldon: the lower rooms (`sheldon-8`, `sheldon-9`) and the hidden area behind the
  teleporter crystal (stage 22: `sheldon-10` to `sheldon-12`).
- [ ] Shield Sheldon: find the code that copies texture entry `0x16` of member 102 into VRAM, and
  where in the stage it happens. Save states needed: one just before and one just after the
  change, and one before the boss gate that came through door 3, not through the lower rooms.
- [ ] Shield Sheldon: which code sets the camera y limits 496 (`sheldon-8`) and 752 (`sheldon-9`)
  in the lower rooms (no zone or gate record writes them).
- [ ] Which armour part is bit `0x04` of `game+0x69` (the Light capsule of the lower-left room of
  stage 7).
- [ ] Check the other stage members for a second texture entry (`0x16`); `learn` and `stages` now
  handle it, but only member 102 is known to use it. (A quick list: member 107 has entries `0x10014`
  and `0x10016`, member 137 `0x1040a`; their meaning is not read.)
- [ ] Hidden areas: where a hidden area leads after its boss.

## `docs/engine.md`

Under "Layout" (or a new "Stage textures" item):

- **A stage member can have two texture blocks** [certain for member 102]. Entry `0x10000` (256
  KB) is stored page by page (8 pages of 64 × 256 VRAM halfwords, one after the other) and is
  loaded at the stage start to VRAM x 320–831, y 256–511. Entry `0x16` (256 KB) holds the same
  area stored row by row (256 rows of 1024 bytes). In Shield Sheldon's stage, entry `0x16` stays in
  RAM for the whole stage (`0x8015F378` in `sheldon-1`, `0x8015A548` in `sheldon-7` to `sheldon-9`)
  and is in VRAM in the lower rooms (`sheldon-8`, `sheldon-9`); it draws the lower rooms and the boss
  part correctly. No word in RAM points to the copy; the code that loads it is not known.

Under "Boss overlay":

- Stage 22 (area 0, overlay 31, the Shield Sheldon hidden area) has the same `BossSpawner` (index 13
  of its stage-logic type table, `0x80052E44`). `sheldon-12_high-max`: `boss_choice` 1, overlay 44
  at `0x800FA000` (1 of 33 304 bytes differs), main type 55 `var` 1 [certain]. So the rule holds in
  two hidden areas.
- Overlays of the hidden areas: stage 21 area 1 = 30, stage 22 area 0 = 31; both fit
  overlay = 25 + 2 × (stage − 19) + area [likely].
- `StageStartReset` sets `nightmare_effect` only for stage < 19 and area 0, so a hidden area keeps
  the effect of the stage before it: stage 22 has the Nightmare Bugs (value 1) [certain for the
  value; main 22 is live in `sheldon-11`].

Under "Camera", "Camera zones":

- Stage 22 area 0 has no zone table (word 0) and no zone record. Stage 7 area 0 has 8 entries; entry
  7 (trigger x 5024–5055, y 800–1039; min x 4704) has no record.

## `docs/process/process-log.md`, a new section

```
## 80. Shield Sheldon: the lower rooms, and the hidden area (stage 22)

The user saved five more states: two in the lower rooms of section 1 (`sheldon-8_lower-1`,
`sheldon-9_lower-2`), and three in the hidden area behind the teleporter crystal
(`sheldon-10_section-2`, `sheldon-11_laser-shot`, `sheldon-12_high-max`).

**The `learn` fit test.** The hidden-area states fit at once: stage 22, overlay 31, member 121,
26 × 3 screens, 42 records, 0 differing cells. The two lower-room states failed: "texture block
found at 0 VRAM positions". The closest position was the right one, with 1519 rows wrong. A page
count showed that pages 2–6 were all different. A search of the extracted files for short pieces
of those VRAM rows found them in member 102, but 1024 bytes apart for each row: entry `0x16`, a
second texture block of the same size, stored row by row and not page by page. Read that way, it
matches all 8 pages in both states.

**What this changed.** The first drawing of the stage had garbled art in the lower rooms and in
the boss room; we had not noticed it, because only the start and the laser rooms were checked.
`learn` now tries entry `0x16` when entry `0x10000` is not in VRAM and writes `"texture": 22`;
`stages` draws with that block. A second catalog entry (from `sheldon-8`) draws the same layout
with the second texture. The layout and the 114 records are the same; only the art differs.

**The hidden area.** One long corridor: hanging bars (collision 45) and a rope (46, new in our
maps) over a spike floor and a pit, with nine fixed laser beamers in the ceiling, then boss gates
48 and 49. The same `BossSpawner` as in the Turtloid hidden area made High Max with `boss_choice`
1: overlay 44 is at `0x800FA000`. The prediction from section 70 held in a second hidden area.

Lessons:

- "0 positions found" with a closest match of 1519 bad rows means a different block, not noise.
  Count the bad rows per page before you change a threshold.
- When a piece of VRAM is found on disc at a regular stride, the stride tells the storage order.
- Check every part of a drawing, not only the part that the first state shows.
```

## Test expectation changes

None: no retail test failed because of the new states (see the report).
