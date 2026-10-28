# Nightmare Pressure (stage 4 area 1, overlay 07): working notes

## Method

- Kits: `python3 -m mmxdemake.rekit --state shark-4_mini-boss.state.gz --type 16 --name shark-mini-boss --out ~/mmx-data/x6/re/shark-mini-boss/kit`
  (95 functions, 20 state tables, neighbour visual 13), the same with `--type 10 --class stage`
  (`kit-stage10/`, 12 functions) and `--type 11 --class stage` (`kit-stage11/`: the junk block,
  not part of the fight). `--at` for `0x8002BCCC`, `0x80032B34`, `0x8003208C`.
- Read: all of `kit/code.txt`, `kit-stage10/code.txt`, both `data.txt`, `calls.txt`.
- Probes (`probes/`): `objprobe.py` (overlay check, game fields, live main / stage / visual / sys
  objects), `camprobe.py` (the three layer cameras, X, game fields; 4 states), `megabox.py`
  (X's boxes), `records.py` (the area 1 record lists).
- Name: the Fandom page "Recycle Lab" lists "Nightmare Pressure (sub-boss)" (seen through a web
  search summary; the page itself returned HTTP 402).

## Overlay

`rock_bin/07.bin` (65 564 bytes) matches RAM at `0x800E9860` in `shark-4` (150 bytes differ:
variables) and `shark-5` (152 differ) [certain]. So overlay 07 is stage 4 area 1, base
`0x800E9860`.

## Key facts and where they come from

| fact | source |
|---|---|
| Record 75 of `enemy_set_tbl` stage 4 area 1 (`0x800F7BD8`): main 16 `id` 0, kind 0, at (160, 1064); flags `01`, kind `10` in `shark-4` and `shark-5` | `records.py` |
| HP max 48 is a constant (`0x800EB840`); the HP fill stops at 48 (`0x800EBA48`); `BossLevel` is called but its result is not read | code |
| Each part has 16 HP; a hit on a part takes `old − new` (signed byte) from the body, so the full damage, also past the part's HP | `0x800EA74C` |
| Pods 16–18 have `dmg_tbl_normal` only on the side that faces X (17: X right of the body or level; 16, 18: X left); turrets 32–34 have `dmg_tbl_deflect` always | `0x800EA6CC`, set-up |
| Parts take no hits while the head is in (`body+0x8F` < 2): `PartMain` returns before `ContactWeapon` | `0x800EA670` |
| The machine is BG layer 1: its objects are at `cam0 + cam1.x`, `cam0.y + 952 − cam1.y` | `0x800EA16C` and the stage 10 code |
| `cam1.y` 872 at rest, 824 in the intro drop, 800 in the PRESS, 776 in END CRUSH | code; 872 and layer 1 mode 5 in `shark-4` |
| THINK tables `0x800F87B0` and `0x800F87F8` are byte-identical | `data.txt` |
| Death: checkpoint 2, `game+0x26` = 1, engine object 26, camera (5456, 224), X (5604, 395), `SpawnInRect(5408, 5824, 224, 512, 0)` | code; `shark-5`: checkpoint 2, `game+0x26` = 1, X (5604, 395), cameras (5456, 224), (5456, 224), (2728, 224), boss gate `id` 21 live at x 5776 |

## Checked against the save states

`shark-4_mini-boss`:

- body main slot 0, state 1, step 3 (WAIT), sub 1, timer 39: the first THINK has run (pattern 0,
  index 1). HP 48 = `boss_hp_max` 48; `game+0x20` = the body; `game+0x24` = 1.
- parts in slots 1–6 at the offsets of the code (body (170, 960): 16 (154, 928), 17 (186, 960),
  18 (154, 992), 32 (186, 928), 33 (154, 960), 34 (186, 992)); flips as the set-up (17, 32, 34
  right); pods `dmg_tbl_normal`, turrets `dmg_tbl_deflect`; hurt box `0x800F852C`; HP 16.
- stage 10 in stage slots 2–6: walls at x 26 / 314 (`ofs_y` −18, moving down; y 1004 is the
  position from the update before the move), press plates at (90, 944) / (250, 944), head block at
  the body. Boxes and tables as the set-up.
- cameras: layer 0 (10, 944) fixed; layer 1 (0, 872) mode 5; layer 2 (5, 944).
- X: (137, 1115), standing (step 2), terrain box centre (0, 2), half (11, 19): feet at y 1136.
  Hurt box (−10, −17, 19, 38).
- `game.difficulty` 1, boss level 0 (`+0x8D` 5).

`shark-5_after-mini-boss`: see the table above. No main 16, no stage 10 live. Record 75 still has
the "spawned" bit (the body ends with `DeleteObject3`, which does not clear it). `game+0x20`
still points to slot 0 (the death code does not clear it); `game+0x24` is 0 (not written by this
code at the death: another object did it [likely object 26]).

## Room numbers (derived)

Room = world − camera (10, 944). Floor: y 192 in the room (X's feet at 1136) [likely: from X's box].

- Machine origin: room y 80 at rest (`cam1.y` 872), 152 pressed (800), 176 at END CRUSH (776).
- Press plates (stage 16/17): terrain 160 × 160, bottom edge at the machine origin: room y 80 at
  rest, **152 pressed: 40 px above the floor**; X is 38 px tall, so a standing X has 2 px [likely].
- Head (body): `ofs_y` −64 (in, room y 16) to 56 (out, room y 136). Head block 56 × 112 centred
  on the body: room y 80–192, to the floor.
- Walls (stage 0/1): room x 16 and 304; terrain centre (2, −8), half (18, 56); `ofs_y` +64 in the
  fight: room y 88–200 by the box (to the floor), −64 in the PRESS.
- Bombs: start at room x 160–287, room y 64 (in the press plates).

## Open questions

1. Who reaches step 9 (END CRUSH: `cam0.x` ≥ 5440 while the body lives)? In this room the camera is
   fixed at x 10. A test outside the fight [guess: left from another use of the code].
2. The mirror of `HITBOX` for `flip` `0x40` (the beam starts 37 px from the pod; the wall and
   drill boxes on the inner side) [likely, from `docs/engine.md`].
3. Engine object 26 and its `+0x07` (the white screen: the stage agent reads it).
4. `0x8003208C` (`SolidPushShots`): it pushes visual and weapon objects with `+0x9D` set; which
   objects set `+0x9D` is not read.
5. Class 7 of `dmg_tbl_capsule` on the walls and plates (shots probably stop on them).
