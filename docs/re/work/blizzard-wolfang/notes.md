# Blizzard Wolfang boss fight (stage 2, overlay 04, main type 54): notes

Date: 2026-10-23. Tags as in `docs/engine.md`: [certain], [likely], [guess].

## Method

- States: `~/mmx-data/x6/dumps/wolfang-6_boss-warning.state.gz` (SHA-1 `d52eadc6bf7ff990afd74461830c40b884eb3b52`)
  and `wolfang-7_boss-fight.state.gz` (SHA-1 `93b62b93d2f85faffec3f8fbde1fe8bec5ee1379`). For the room
  map and the misc 23 check also `wolfang-5_ice-pit`.
- Kit: `python3 -m mmxdemake.rekit --state wolfang-7_boss-fight.state.gz --type 54 --name blizzard-wolfang`
  → `kit/` (120 functions, 22 state tables, neighbour visual 20). All functions were read.
- Effect 8 (the mouth ice, made by `0x800F52AC`): `rekit --class effect --type 8 --out kit-effect8/`;
  its step function `0x800F5940` with `--at` (not walked by the kit).
- `--at 0x8002ACA8` (`PaletteFade`): it reads three subtract flags, `fade_mode[0]` (`0x800CF850`) for
  r, `0x8009742C` for g, `0x80090D56` for b. The kit names these two addresses `white_fade_dir+0x7`
  and `sound_bank_vab+0x6`, which is wrong; `scripts/xref.py` lists their other users (all in the
  main program: `0x8002BBC0`, `0x800505D0`, `0x800523B8`… write them together with `fade_mode`).
- Scripts (`scripts/`, run from that folder): `probe.py` (copied from `scaravich-boss`: all pools),
  `bossfields.py` (the boss fields, fades, visual and effect objects), `cells.py` (room collision
  x 6272–6767, y 256–591, and the differences between states), `xref.py` (main-program references to
  an address). Outputs: `probe.txt`, `bossfields.txt`.
- Sprites: `spriteexport --root sprites-root` on both states; a third run on `wolfang-5_ice-pit` into
  the scratch folder only, to compare the misc 23 sheet.

## Live values

| what | `wolfang-6_boss-warning` | `wolfang-7_boss-fight` |
|---|---|---|
| `game+0x90` (stage time) | 36156 | 36232 (+76) |
| fight timer `0x800F7840` | 0 | 78 |
| X | step 20, (6380, 491), lock 1, `+0x7A` 1 | step 2, (6380, 491), lock 0 |
| camera layer 0 | (6320, 320), limits x 6320–6384, y 304–320 | the same |
| boss (main slot 0, type 54) | state 1 step 2 sub 2, (6485, 416), display 0, HP 0, `+0x7C` −315, `+0x80` = `BossWarning` (logic slot 1), `+0x8A` −1 | state 1 **step 10 sub 2**, (6560, 480), display 1, animation 13 (sprite 21), facing left, HP 48, `+0x7E` 3, `+0x8A` 0, `+0x8B` 14, `+0x8C` 1, `+0x80` = the old warning slot |
| `game+0x8C` / `+0x8F` / `+0x8D` / `+0x2E` / `+0x24` | 40 / 0 / 4 / 0 / 0 | 48 / 0 / 4 / 1 / 1 |
| `game+0x20` | 0 | `0x8008EF48` (the boss) |
| palette fade channel 0 | all 0 | offsets 0, `fade_mode[0]` 1, mask words 1 `0xF0000000`, 2 `0xFF`, 3–14 all set |
| visual objects | none | visual 20 `var` 0 (slot 0): sub-step 1 (thrown), (6508, 415), sx −3.3913, sy 5.4545, gravity 0.3636, HP 2, contact 4, `dmg_tbl_minor`, parent the boss |
| logic | 24 `BossWarning` (state 1 step 1), 22 rays type 3 (the WARNING), 7 (ice slide), 8, 14 (talk zone 133), 27, 11 × type 3 | 7, 8, 14, 27, 11 × type 3; no warning |
| stage objects | gates `id` 5 and 6 (state 2, closed) | the same |
| misc | 7 × misc 23 (`SnowScenery`) | the same |

Checks of the reading [certain unless marked]:

- `wolfang-6`: step 2 sub-step 2 is the wait for `BossWarning`; `+0x7C` = −315 means 335 updates since
  sub-step 1 (the timer of 20 runs on below 0). X in step 20 after its 20 px event walk. Display 0:
  the boss is not drawn before its landing. `game+0x8C` = 40 is an old value; the boss writes 48
  only when the warning is gone [likely].
- `wolfang-7`: HP 48 = 48 + 0 (level 0); `+0x8B` 14 → table 1 row 14 = step 10; `+0x7E` 3 = the X
  ball count of level 0; the ball is `var` 0 (X: the lob) and its `speed_x` × 46 = 6380 − (6560 −
  24); its `speed_y` 5.4545 = 8.364 − 8 × 0.3636 (8 updates after the throw). Boss y 480 = the floor
  (512) − the terrain half height 32. The fade mask has the boss CLUT (88) bits from sub-step 7, and
  the offsets are back to 0: the entry ended.
- The fight timer (78) is larger than the stage-time difference (76), although the whole entry (more
  than 600 updates: fade 124, fall about 37, eyes 180, text, fade 124, howl 154, HP 48) is between
  the two states. So `game+0x90` does not count during the entry [likely: while the player is
  locked]. The "76 updates later" of the task is the stage-time difference, not real updates.
- `+0x80` of the boss still points at logic slot 1 in `wolfang-7` (the warning slot, now free).

## Findings

The full reading is in `sheet.md` (proposed `docs/enemies/blizzard-wolfang.md`) and `pseudo.c`.
Key points and where they come from:

1. **Main type 54 is Blizzard Wolfang** [certain]: the only type-54 record (66, x 6485) is in the
   room after boss gate 6; its code calls `BossLevel`, `BossMusicRequest`, `TextOpen`, makes
   `BossWarning` and `BossExplosion` and the Nightmare soul (main 15); it is `game+0x20` in
   `wolfang-7`; its sprites (bank 14) are the wolf Maverick; its damage table takes slot 3 (Magma
   Blade, Heatnix) as the weakness and deflects slot 2 (Ice Burst, his own weapon).
2. **Entry** `0x800F48D4`: the room fades to black with channel 0 of the palette effects in subtract
   mode (not a white fade). Only the eyes show (animation 7). The boss palette joins the fade when the
   music starts, then all fades back. No camera change, no `CopyTiles`.
3. **Moves** from two tables of rows (`0x800F6FB0`, `0x800F7030`) by `+0x8A` (−1, 0, 1); the switch at
   HP ≤ max / 2. Step 9 only from the floor slide when the player is on a wall (player steps 9, 11,
   52).
4. **Stun ids 3, 12, 34** (`ContactWeapon` results 4, 13, 35), in any step; `+0x88` = 10000 holds the
   deflect table through the stun, 40 more after it.
5. **Ice spikes only at boss level ≥ 2** (`WolfangInit`, `game+0x8F` < 2 → state 5).
6. **Difficulty:** only the debris boxes test it (Hard and up). The Nightmare Effect is not read.
7. **Character:** ball type and waits by `mega+0x02` (X: lob, 40 updates between balls; Zero: aimed
   4 px/update, 70 updates). The count table `0x800F75F0` has the same values for both.

## Hazards (for a reimplementation)

- `WolfangNextMove` reads past a row only until `0xFF`; every row has `0xFF` within 4 bytes [certain
  for the data]. A row without `0xFF` would run into the next row.
- The stun sets `+0x88` = 10000 and the stun end sets 40; if the stun step were left by another path
  the boss would deflect for 166 s. No other path leaves step 3 [certain for the code].
- The ice objects, balls and effects test `parent+0x04` = 1 each update; the boss slot is freed at
  the end of the death (`DeleteObject3`), after they are gone, so no stale parent [certain for the
  order].
- `ContactMega` runs during the hit timer: the boss hurts while flashing (unlike Ground Scaravich).
- The ceiling slide's cling time is what is left of the trail counter (1–8 updates): data-dependent.

## Extractions

- `sprites-root/enemies/enemy-6757bcd2a2/` (main 54, bank 14, table `0x800F7478`, 34 animations,
  cell 134 × 114, origin (77, 61)): **good**. Checked by eye: every animation matches its use
  (stand, dash, jump, wall cling, ceiling poses upside down, stun, the ice spikes on the floor, the
  wall and the ceiling, the ice ball, the mouth ice, the debris). Animation 7 is almost empty: the
  red eye glint (sprites 98–100) and an empty sprite 69; this is right, not a broken export. The
  same ID from both states. Not yet in `sprites/`.
- `boss-gate`, `nightmare-virus`, `nightmare-virus-orb`, `teleport-crystal`: the same IDs as in
  `sprites/`; nothing new (only the two states would be added to their sources).
- `enemy-6e86b66bc1` (misc 23, `SnowScenery`, sprite-only, entry 8) and the bank sheet
  `entry-8-live-0.png` of both states: **wrong**. The sheet shows the snow-bank pieces mixed with
  orange and blue strips. In `wolfang-5_ice-pit` the same misc 23 (entry 8) gives `enemy-cd98e86270`
  (already in `sprites/`) with clean snow banks and icicle strips. Cause [likely]: the boss texture
  (type 54, no texture entry, loaded with the boss pack) covers the VRAM cells of banks 4–8; the tool
  skips banks 4–8 for this reason ("cells overwritten by type 54"), but the overwrite check does not
  cover the extra sprite source of a live misc object. The ID differs from `enemy-cd98e86270` although
  the entry is the same [guess: the fingerprint of a sprite-only export includes the drawn cells].
- Bank sheets: `bank-0`, `-1`, `-2`, `-3`, `-9`, `bank-14-live-0`, `stage-8-live-0` look right; banks
  4–8 were not drawn (overwritten, correct).
- The ice balls, debris, wall runners (visual 20), the mouth ice (effect 8), the ice spikes and the
  trail images (main 54 `var` 1, 16–18) use the boss's animation table and sprite data, so the boss
  export has all their animations (26–33). The tool did not make a separate export of the live
  visual 20 (same table); none is needed.
- The shared-table problem of `docs/TODO.md` (the lemon item) does not touch the boss export (its
  table has exactly 34 entries and only the boss's bank). The `nightmare-virus-orb` export comes
  from the shared shot table `0x8007789C` (2 of 40 animations kept), as before; it is the known
  export, not a new one.

## Open questions

See the sheet. Also: the stage-12 rematch uses another overlay; its copy of this code was not read.
