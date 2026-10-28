# Doc changes: Nightmare Mother

New file: `docs/enemies/nightmare-mother.md` = [`~/mmx-data/x6/re/nightmare-mother/sheet.md`](sheet.md).

## `docs/engine.md`

Old:

```
`rock_bin/05.bin` matches RAM in `heatnix-2_mini-boss` (4 of 44084 bytes differ)]. Shield Sheldon's and Metal
Shark Player's code is in their stage overlays; neither stage loads a boss overlay.
```

New:

```
`rock_bin/05.bin` matches RAM in `heatnix-2_mini-boss` (4 of 44084 bytes differ)]. Shield Sheldon's and Metal
Shark Player's code is in their stage overlays; neither stage loads a boss overlay. Stage 16 area 0
(Gate's lab, stage 1) = 22 [certain: `rock_bin/22.bin` matches RAM in `gate-2_nightmare-mother`, 11 of
81 224 bytes differ]. Overlay 22 runs up to `0x800FD5A8`, over `0x800FA000`: Nightmare Mother's code
(main type 60) is in the stage overlay, and no boss overlay is loaded (`docs/enemies/nightmare-mother.md`).
```

Old:

```
changed to 127). So the bar shows the sum of the gem HP. The body dies when its four gems are dead
(a counter in `+0x07`), not by HP [certain for the code].
```

New:

```
changed to 127). So the bar shows the sum of the gem HP. The body dies when its four gems are dead
(a counter in `+0x07`), not by HP [certain for the code]. Nightmare Mother (overlay 22, main 60) uses
the same sum: the core (`var` 0, slot 0) sets its HP to 0 each update and the two eyes (`var` 3, 4)
add theirs; each eye adds its `var` to core `+0x8D` when it dies, and the core dies at 7 [certain for
the code].
```

Old:

```
| 48–63 | 2 D 2 2 1 8 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

**An object with no damage table** (`+0x58` = 0): `ApplyWeaponHit` then reads the table at RAM
```

New:

```
| 48–63 | 2 D 2 2 1 8 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_nightmare_mother` (`0x800757DC`, the eyes of Nightmare Mother,
`docs/enemies/nightmare-mother.md`):

| ids | damage |
|---|---|
| 0–15 | 2 5 3 D 6 3 D – D 2 3 3 D 8 3 D |
| 16–31 | 3 D 5 5 5 D 8 D 3 3 6 3 3 3 3 3 |
| 32–47 | 3 3 D 8 3 3 3 3 3 3 D 3 D 2 4 8 |
| 48–63 | 3 2 3 3 2 3 4 127 4 8 4 4 15 5 15 4 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_nightmare_mother_shell` (`0x8007587C`): D for every id, except ids 24–29, 31, 50 and 51
(damage 0), id 55 (127) and ids 7, 23, 79 (–). The shells' set-up writes it, but their main state
writes `dmg_tbl_deflect` over it each update, so it has no effect after the first update [certain
for the code].

**An object with no damage table** (`+0x58` = 0): `ApplyWeaponHit` then reads the table at RAM
```
