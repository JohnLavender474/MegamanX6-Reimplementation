# Doc changes for the Wolfloid (purple wolf)

The main session applies these. Exact text; "after" means a new line after the quoted line.

## 1. New file `docs/enemies/wolfloid.md`

Copy [`~/mmx-data/x6/re/purple-wolf/sheet.md`](sheet.md). When `docs/stages/wolfang.md` exists, move the
section "Records (section 1, member 96, area 0)" there and change the line

```
end of this sheet until then).
```

back to a plain link. Also change the line in the intro paragraph:

```
`docs/engine.md`. Records: `docs/stages/wolfang.md` (not written yet; the records are listed at the
```

## 2. `catalog/enemies.json`

Replace

```
    "enemy-bd435676eb": "purple-wolf",
```

with

```
    "enemy-bd435676eb": "wolfloid",
```

Then rename the export folder `~/mmx-data/x6/sprites/enemies/purple-wolf/` to `wolfloid/` (or run
`spriteexport` again) and the work folder [`~/mmx-data/x6/re/purple-wolf/`](./) to `wolfloid/` if you
want the names to match. The name is [likely] (see the sheet, "Open questions" 5); keep
`purple-wolf` if you want a confirmed source first.

## 3. `docs/engine.md`, drop tables

Replace

```
| 17 | 192 | 28 | 10 | 6 | 16 | 4 | |
```

with

```
| 17 | 192 | 28 | 10 | 6 | 16 | 4 | Wolfloid |
```

In "The damage tables", replace

```
`dmg_tbl_normal` (`0x800743DC`): the standard enemy table (Junkroid, Hover Gunner, Killer Moth, Worm
Spider, Batton Bone B81, Sea Attacker ST, open Monbando).
```

with

```
`dmg_tbl_normal` (`0x800743DC`): the standard enemy table (Junkroid, Hover Gunner, Killer Moth, Worm
Spider, Batton Bone B81, Sea Attacker ST, open Monbando, Wolfloid and its cannon ball).
```

## 4. `docs/objects-legend.md`, "Object types by sprite bank"

After

```
| Yammark | 49 | 15 | Commander Yammark (from his live object) | `commander-yammark` | user |
```

add

```
| Wolfang | 6 | 3 | Wolfloid: purple robot wolf with ice breath or a back cannon (`docs/enemies/wolfloid.md`) | `wolfloid` (was `purple-wolf`) | user ("purple wolf"); name from the Mega Man Knowledge Base [likely]; code reading |
```

## 5. `docs/stages/wolfang.md` (new, or the section when the page is written)

```
## Wolfloids (main type 6)

Section 1 (member 96, area 0) has 14 Wolfloid records (29–42). Id 0 stands on the ice slope that
rises to the right (values 25, 26), id 1 on the slope that rises to the left (27, 28), id 3 on flat
floor. On the slopes X meets the cannon wolf and Zero the breath wolf (`docs/enemies/wolfloid.md`).
More Wolfloids (ids 4 and 5) ride the falling ice blocks of the lower room (stage object 7, records
58 and 59), one block in two.
```

followed by the table "Records (section 1, member 96, area 0)" of the sheet.

## 6. `docs/re-guide.md` or `docs/TODO.md` (kit issue)

Add to `docs/TODO.md`:

```
- [ ] rekit: the entry of stage 2 main type 6 (`0x800EB5A0`) loads its state table `0x800F6A84`
  with the `lui`/`addiu` interleaved with the `prev_x`/`prev_y` stores; the kit lists it under
  "Globals and data" and does not walk the 4 states and the 8 steps that follow it (`0x800F6A94`).
  It also merges the visual `var` 1 table (`0x800F6AD8`) and `var` 2 table (`0x800F6AE4`) of
  visual type 5 into one 6-entry table (the dispatcher `0x800EB670` loads the `var` 2 table with
  its own `addiu`, at `0x800EB724`). Work-around used: `--at` for each function
  (`~/mmx-data/x6/re/purple-wolf/at/`).
- [ ] Stage object 7 of stage 2 (overlay 04): the falling ice blocks of the lower room and their
  generator (`var` 0, records 58 and 59; blocks are `var` 12); one block in two carries a Wolfloid.
  Needs a save state in the lower room (x about 5000–5400, y about 1300–1500).
```

## 7. `docs/process/process-log.md`

Add a line under the current section:

```
- Wolfloid (stage 2, main type 6, the "purple wolf"): sheet `docs/enemies/wolfloid.md`. The kit
  missed the state table at the entry, so the 10 wolf functions were read with `--at`. The
  slope wolves pick their weapon by the player character (X: cannon, Zero: breath). Checked against
  `wolfang-2_ice-slope`: the live wolf and its cannon ball match the code to the pixel.
```
