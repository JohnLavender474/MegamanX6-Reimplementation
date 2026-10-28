# Blizzard Wolfang's stage: the ice objects (notes)

Stage 2, area 0, overlay 04, map member 96 ("Blizzard Wolfang, section 1"). States:
`wolfang-1_x-sabre`, `wolfang-2_ice-slope`, `wolfang-3_falling-ice`.

## Result in short

| object | class, type | what |
|---|---|---|
| ice block | main 17 (overlay 04) | all the ice hazards: avalanche blocks (`var` 4), falling ice (`var` 6), bouncing blocks (`var` 7), and their invisible spawners (`var` 0, 2, 3); `var` 1 and 5 are an unused right-hand avalanche |
| ice fall controller | stage logic 12 | 15 trigger zones; makes the spawners; camera shake; rumble sounds |
| ice slide | stage logic 7 | the slippery floor (60) and the push down the ice slopes (25–28); the same code as Shield Sheldon's glass slide |
| fire ring | main 19 | Nightmare Effect 3 only; not related to the ice |
| snow scenery | misc 23 (sprite type 144, bank 8) | 7 fixed-list sprites (snow mounds, ice wall edges) at x 5120–5888; not related |

No record of the main list is type 17. The "blocks on the ice slope" are the avalanche blocks:
they fly at (−8, +4) px/frame, parallel to the 1:2 ice slopes [certain for the numbers]. The
falling ice is the same type, mode 6.

## Method

- Live objects: `scripts/live.py <state> main stage visual misc misc2 logic kind7 actor2`.
- Kits (output in this folder):
  - `rekit --state wolfang-3_falling-ice.state.gz --type 17 --name ice-block --out kit-main17`
  - `--type 12 --class logic --out kit-logic12`, `--type 7 --class logic --out kit-logic7`,
    `--type 27 --class logic --out kit-logic27` (engine fade object; not related),
    `--type 19 --out kit-main19`, `--type 23 --class misc --out kit-misc23`.
- Data: `scripts/table.py` (the trigger table `0x800F6E08` and the live "done" bytes),
  `scripts/dmg.py <state> 0x80075CDC` (damage table), `scripts/ice17.py` (type 17 fields),
  `scripts/slide.py` (logic 7 and player fields), `scripts/snd.py` (sound words in RAM),
  `scripts/coll.py x0 x1 y0 y1` and `scripts/surf.py` (collision from the TMX), `scripts/cmpram.py`
  (code compare across overlays).
- Sprites: `spriteexport --state wolfang-3_falling-ice.state.gz --root sprites-root`. It made the
  same IDs as the shared export (no new object). `ice-block` now has a live source; bank 5 checked
  by eye: sprites 0–7 a 64 px boulder at 8 angles, 8–15 a 48 px boulder, 16–20 chips.
  `enemy-cd98e86270` (misc 23) checked by eye: snow mounds and thin ice wall edges.
  `enemy-78c8f556f5` (type 19): an orange-red fire ring.

## Evidence and checks against the states

- `wolfang-3_falling-ice` (checkpoint 1, X (3263, 363), camera (3103, 224)) [certain for the RAM]:
  - main slot 2: type 17 `var` 2, step 3 (spawner), (3320, 211), count `+0x7E` = 26, no boxes,
    `display` 0. Trigger row 6 is (3320, 224) + rand(−16 … 15): 211 fits.
  - main slots 3–6: type 17 `var` 6, step 1, x 3320–3326, y 508 / 418 / 328 / 238, `act4` 33 / 23 /
    13 / 3, `speed_y` −9.0. So 9 px per update and one piece per 10 updates [certain].
    Start y = 238 − 3 × 9 = 211 = the spawner y.
  - sizes 1, 1, 0, 0 with boxes `0x800F6C54`/`5C` (small) and `0x800F6C50`/`58` (large); sprites
    8 and 0 (animations 3 and 2).
  - logic 12: state 1, `+0x05` = 1 (the falling-ice rumble flag), done byte of row 6 only.
  - the column falls between the ice pillars x 3232–3279 (X stands on it, feet y 387 ≈ pillar top
    384) and x 3360–3407; under the ceiling row y 208 (collision 57).
  - logic 7 state 2 (slide), mode 1, v 0, a −0.09375: X has just stopped a short slide on the
    pillar.
- `wolfang-2_ice-slope` (checkpoint 0, X (640, 1131)): no type 17 object; logic 12 done byte of
  row 0 set, shake counter 0. So the avalanche of row 0 (trigger x 497–591) is over (it lasts 480
  updates); row 1 (x 769–863) is still ahead. X stands on flat ice (60) at x 640 between two slope
  pieces (25, 26 at x 608–639; the long slope starts at x 736) [certain for the RAM and the map].
- `wolfang-1_x-sabre`: no type 17, no done byte.
- The slopes: collision 25, 26 (rises to the right) and 27, 28 (falls to the right), 2 cells per
  16 px: a 1:2 slope. Avalanche speed (−8, +4) is 1:2 [certain].
- Avalanche points vs. the slope surface (`scripts/surf.py`): rows 0, 1, 4, 5 are 63 px above it,
  row 2 55 px, row 3 128 px above flat ice.
- Logic 7 code: words of `0x800ED3A4`…`0x800EDF3C` (overlay 04) against `0x800F0BB0` (overlay 10,
  `sheldon-6_tile-shown`) and `0x800F0530` (overlay 07, `shark-3_press-low`): 742 words, every
  difference is a `j`/`jal` target or an address `lui`/`addiu`/`lw`; the case lists of both jump
  tables are the same [certain].
- Sounds: bank 5 is `m096-e03`; 5/8 = `vag-01` (21224 samples, loop from 10444), 5/9 = `vag-02`
  (loop from 448), 5/3 = `vag-03` (56 samples, no loop). All three on SPU voice 14. So 5/3 cuts the
  loops [likely]. 2/17 = `m096-e05` `vag-03` (11032 samples, one-shot).

## Key facts and where they come from

- Type 17 modes, speeds, timers: `kit-main17/code.txt`, `0x800EE334` (set-up switch
  `0x800E9AB0`), `0x800EE8D8` (spawner; mod 7 / mod 10 / mod 31 by multiply constants
  `0x92492493`, `0x66666667`, `0x84210843`).
- Boxes: `kit-main17/data.txt` `0x800F6C48`, `0x800F6C50`, `0x800F6C58`.
- Damage table `0x80075CDC`: `scripts/dmg.py`. Ids 4, 13 = 3; 55 = 127; 7, 23 = –; else D.
- Trigger table: `kit-logic12/data.txt` `0x800F6E0C` (the kit splits it at `0x800F6E08`/`0C`
  because the code reads x0 at `+0` and y at `+4` through two bases); `scripts/table.py`.
- The slope push: `kit-logic7/code.txt` `0x800ED468`…`0x800ED630`.

## Hazards

- `IceBlock` (`0x800EED40`) calls `DeleteObject3` at the end of the ice part and then still runs
  the state function on the cleared slot (state 0 → the set-up writes fields into a free slot).
  Safe in the game because a free slot is cleared again by `GetMainObject`. A reimplementation
  should return after the delete.
- The spawner calls `GetMainObject` each update before the mod test; the slot is only filled on
  the right count. Safe because the slot stays free (byte 0 is 0). A reimplementation should test
  the count first.
- `IceFallControllerMain`: rows 12–14 `return` (not `continue`) when the player y is too small, so
  the rows after them are not tested in that update. Safe because they are the last rows. The
  test of row 15 is dead code.
- A trigger row is marked done also when no main slot was free: that spawner is lost until the
  controller set-up runs again (a death). Rare (48 slots).
- The sound stop rule 2 (`sheet-ice-fall-controller.md`) plays 5/3 in every update with no
  falling piece; as read it cuts the avalanche rumble in the update that starts it. Check in play.
- Mode 6 pieces and modes 4, 5 have no terrain test: they pass through floors and walls by design.
- `var` ≥ 8 has no set-up case and would run step 0 (`HitReactionDispatch`) with the default
  boxes. No trigger makes it.

## Ice physics (stage logic 7)

The flat slide was in `docs/stages/sheldon.md` (glass). New here: the slope part and the proof that
the code is shared. Not in `docs/players/*.md` or `docs/engine.md` before. Details:
`doc-changes.md` §2. Facts from `slide.py`: `mega+0x104` = 8.125 in all three states (X has the
dash part), so the slope cap is 8.125 here. X's terrain box is `00 02 0B 13` (centre +2, half
11 × 19), so the foot point is y + 21.

## Not found / open

1. The reader of `game+0x27` (set to 120 by an ice block kill, cleared by the controller set-up);
   only these two accesses with offset `0x27` are in overlay 04.
2. Whether the avalanche blocks hit a standing X (the random y gives a block bottom 19–57 px above
   the slope).
3. The rumble stop rule in play.
4. What the controller's final `CopyTiles(0, 4672, 1056, {4672, 912, 3, 1})` changes; the meaning
   of the 4 source values is a guess.
5. Mode 1 / 5: no trigger uses them; the end rule of mode 5 (player y ≥ 529, x ≥ 5025) points to
   another part of the stage.
6. Official names: none checked (no web access used).
7. No state shows an avalanche or a bouncing block live; those modes are from the code only. A
   state at x 769–863 (row 1) or near x 4720 below y 448 would show them.
