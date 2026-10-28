# Doc changes for Gate stage 1 (stage 16)

New file: `docs/stages/gate-1.md` = [`~/mmx-data/x6/re/gate-1/area-page.md`](area-page.md) (copy as is). It links
`docs/enemies/nightmare-mother.md` (the other agent's sheet).

Catalog: `learn` entries and the `texture_split` are in `notes.md`, "Catalog entries that `learn`
adds".

## `catalog/enemies.json`

Old:

```
"8": "mijinion", "21": "turtloid-alt"
```

New:

```
"8": "mijinion", "16": "gate-1", "21": "turtloid-alt"
```

## `catalog/object-names.json`, `members`

The member 112 line goes after `"103"` (the keys are sorted as numbers). The names of main 33 and
main 34 are descriptive, by type number, until the official names are known. `NightmareMother`
for main 60: check it against the other agent's sheet.

Old:

```
            "logic:5": "TunnelBgAnimator"},
```

New:

```
            "logic:5": "TunnelBgAnimator"},
    "112": {"main:6": "Wolfloid", "main:13": "LaserReflectorPuzzle", "main:18": "BattonBoneB81",
            "main:29": "LavaController", "main:33": "Gate1Type33", "main:34": "Gate1Type34",
            "main:60": "NightmareMother", "logic:7": "IceSlide"},
```

## `docs/engine.md`

Old:

```
Shark Player's code is in their stage overlays; neither stage loads a boss overlay.
```

New:

```
Shark Player's code is in their stage overlays; neither stage loads a boss overlay. Gate's lab:
stage 16 = overlay 22 (member 112), stage 17 = 23 (113), stage 18 = 24 (114); none of the three
has an area 1 [certain for `stage_overlays` and `stage_members`; 16: `learn` finds the layout in
overlay 22 in `gate-1_before-boss`]. Stage 16 is Gate stage 1 (`docs/stages/gate-1.md`, boss the
Nightmare Mother), stage 18 is Gate (`docs/damage.md`).
```

## `docs/players/weapons.md`

Old:

```
(slot 0 is the Giga gauge). The maximum is `mega+0x14A` = 6 × `game+0x61` (288 at the start, 324
with all upgrades seen; 6 units = one bar unit).
```

New:

```
(slot 0 is the Giga gauge). The maximum is `mega+0x14A` = 6 × `game+0x61` for X and 6 ×
`game+0x62` for Zero (288 at the start, 324 with all upgrades seen; 6 units = one bar unit)
[certain for 80 save states: every X state agrees with `+0x61`, every Zero state with `+0x62`;
`gate-1_before-boss`: Zero, `+0x61` = 56, `+0x62` = 52, maximum 312; the writer is not read].
```

## `docs/players/zero.md`

Old:

```
| `mega+0xA8 + 2n` | energy of gauge n; full = 288 (`+0x14A`) in all five states |
```

New:

```
| `mega+0xA8 + 2n` | energy of gauge n; full = `+0x14A` = 6 × `game+0x62` (Zero's own upgrade byte; `docs/players/weapons.md`): 288 in the first five states, 312 in `gate-1_before-boss` |
```

## `docs/enemies/boss-gate.md`

Old:

```
| Turtloid | id 34, x 8624: opens only when `game+0x28` ≠ 0 (Weather Analyze machine 3 destroyed); camera fixed at x 8624 | id 23, x 8944: camera x 8944–9040, y 640–688, time stamp |
```

New:

```
| Turtloid | id 34, x 8624: opens only when `game+0x28` ≠ 0 (Weather Analyze machine 3 destroyed); camera fixed at x 8624 | id 23, x 8944: camera x 8944–9040, y 640–688, time stamp |
| Gate stage 1 | id 39, x 7664: camera fixed at (7664, 0); lowers BG layer 1 (the lava) | id 26, x 7984: camera x 7984–8096, max y 160 |
```

Old:

```
6. What the layer-1 fields `+0x42` / `+0x03` and id 31's arrays are (main type 25 of id 7 is
   answered: Nightmare Snake); in which stages ids 7,
   18, 25, 31 and 39 are used.
```

New:

```
6. What the layer-1 fields `+0x42` / `+0x03` and id 31's arrays are (main type 25 of id 7 is
   answered: Nightmare Snake); in which stages ids 7,
   18 and 31 are used. Ids 25 and 39 are gates of Gate stage 1 (stage 16, records 103 and 104):
   there BG layer 1 is a lava band and `0x8009728E` is its y; the lava controller (main 29) raises
   it from −80, and the two gates put it back to −80 (`docs/stages/gate-1.md`) [likely].
```

## `docs/enemies/wolfloid.md`

Old:

```
X meets the cannon, Zero meets the breath. Out of range, it howls every 200 updates.
```

New:

```
X meets the cannon, Zero meets the breath. Out of range, it howls every 200 updates.

Gate stage 1 (stage 16, overlay 22) has 17 Wolfloids (main type 6, `id` 0–3, records in
`docs/stages/gate-1.md`). Its animation table (`0x800FBA80`) has the same 33 animations, the set-up
`0x800EC670` the same values (HP 12, contact 4, gravity `0x40000`, the step by `var` and
`mega.var`), and every function of the overlay 4 kit is also in the overlay 22 kit [likely: the
same code]. Not checked in a save state with a live wolf.
```

## `docs/enemies/batton-bone-b81.md`

Old:

```
or follow X for 5 seconds. The bomb is a separate object (visual type 17). It falls and explodes on the
floor, on X, or when a shot hits it.
```

New:

```
or follow X for 5 seconds. The bomb is a separate object (visual type 17). It falls and explodes on the
floor, on X, or when a shot hits it.

Gate stage 1 (stage 16, overlay 22) has 17 bats with a bomb (main type 18, all `id` 0, records in
`docs/stages/gate-1.md`). The overlay 22 kit has the same 33 functions as the overlay 09 kit, of the
same sizes and identical after the addresses are masked, and the same animation table [certain for
the code match].
```

## `docs/enemies/laser-reflector-puzzle.md`

Old:

```
neighbours. The probe (weapon type 79) is main-program code; it was read with `--at`. Units,
confidence tags and the shared rules: `docs/engine.md`. Records: `docs/stages/sheldon.md`.
```

New:

```
neighbours. The probe (weapon type 79) is main-program code; it was read with `--at`. Units,
confidence tags and the shared rules: `docs/engine.md`. Records: `docs/stages/sheldon.md`.

Gate stage 1 (stage 16, overlay 22) has the same object as main type 13: the same 34 animations,
42 functions of the same sizes in the same order (34 identical after the addresses are masked), the
damage tables `dmg_tbl_reflector` and `dmg_tbl_reflector_wait`, and the neighbours visual 11 and
misc 7 [likely: the same code]. Its 24 records have `id` 0–6 only: fixed beamers (`0x0n`) with no
reflector and no door, so there they are laser traps (`docs/stages/gate-1.md`).
```

## `docs/TODO.md`

Old:

```
  Dynamo's texture covers, but the overwrite check does not mark it (wrong search export
  `enemy-06ef7e2514`). The same fault as `enemy-08bc9efa49` in `mijinion-3_boss-fight`.
```

New:

```
  Dynamo's texture covers, but the overwrite check does not mark it (wrong search export
  `enemy-06ef7e2514`). The same fault as `enemy-08bc9efa49` in `mijinion-3_boss-fight`.
- [ ] Sprite exporter: in `gate-1_before-boss` (and `gate-2_nightmare-mother`) the boss pack
  (member 135, entry `0x10411`) is in VRAM pages 7–14 and covers the cells of banks 1–6 (main 15,
  6, 18, 13, 33, 34). No live object uses those cells, so the overwrite check does not see it, and
  the seven search exports are noise (`enemy-9b63092464`, `enemy-71ce84488a`, `enemy-6d90265515`,
  `enemy-51c2def92c`, `enemy-80ea7f5439`, `enemy-5e5c1e01f6`, `enemy-4c7523f990`; removed by hand
  and listed as skipped in `sprites/stages/gate-1.json`). Fix: when `boss_pack_loaded`
  (`0x800CC868`) is 1, mark the banks whose cells are in the pages of the boss pack. Gate stage 1
  needs a save state at checkpoint 0–3 for its enemy sprites and the colours of its start
  (`docs/stages/gate-1.md`).
```
