# Proposed changes outside `docs/enemies/metal-shark-player.md`

The main session applies these (the files have other owners).

## `catalog/enemies.json`

- `names`: `"enemy-3f9602d9d5": "metal-shark-player"` (main type 56, stage 4 area 1; export from
  `shark-7_boss.state.gz` and `shark-9_boss-revive.state.gz`, bank 14, live object 0, 41
  animations; strips 34–37 need the next bank, strip 40 is not an animation).

## `catalog/helpers.json`

- Add the entries of `helpers-proposed.json` (no key collides with an existing entry).
- The kit names `0x8007555C` `dmg_tbl_high_max_ball+0xa0`. It is a table of its own:
  `dmg_tbl_shark` (in the proposal).
- The kit names `0x800F765C` `SheldonTurret` (a stage 7 / overlay 10 entry). In overlay 07 that
  address is the start of the spark (visual 22 step 5, sub 0). Overlay addresses in
  `helpers.json` are applied in every stage; a note "overlay 10 only" on that entry, or a per-overlay
  section in the catalog, would stop the wrong name.

## `docs/engine.md`

1. **Weapon ids: the special-weapon slot** (section "Weapon ids"). Replace the row
   "`2, 11, 33 / 4, 13, 35 / 8, 17, 44` … [guess]" with:

   | id | weapon |
   |---|---|
   | 1–8 | the special weapon of slot n, uncharged; 10–17 = slot n + 9, charged [certain for the code: `0x8003DF58` sets `mega+0x94` = `mega+0x93`, + 9 when `+0x9B` = 2 or with the Ultimate Buster part `0x80000` in forms 1, 3, 4] |
   | 2, 11, 33 / 4, 13, 35 / 6, 15, 42 / 8, 17, 44 | the stun ids of Rainy Turtloid / Shield Sheldon / Metal Shark Player / Commander Yammark: slot n uncharged, slot n charged, and one id of 33–44. In `shark-8_boss-stun` Metal Shark Player was stunned by id 6 with slot 6 selected (`mega+0x93` = 6, `+0x94` = 6, last shot `+0x97` = 6, slot 6 energy used); the user names slot 6 Rainy Turtloid's weapon [certain for the ids; the weapon name from the user] |

   and add: "The slots follow the stage numbers [likely]: `mega+0xC9` (weapons owned) is `0x21` in
   the Shark states (slots 1 and 6) and `game+0x60` (Mavericks beaten) is `0x21` after the Yammark
   (stage 1) and Turtloid (stage 6) clears. So slot n is the weapon of the stage n Maverick: slot 4
   (stage 4, Metal Shark Player) stuns Shield Sheldon, slot 6 (Rainy Turtloid) stuns Metal Shark
   Player, slot 2 stuns Rainy Turtloid, slot 8 stuns Commander Yammark. The third id is not a
   simple function of the slot: ids 35 and 42 run the same weapon code (`0x801ED2A8`, X's overlay)
   [certain for the table `0x80071220`]; which weapon makes them is open."
2. **The damage tables**: add `dmg_tbl_shark` (`0x8007555C`, just before `dmg_tbl_turtloid`):

   | ids | damage |
   |---|---|
   | 0–15 | 1 2 2 2 D 2 5 – 2 1 2 2 2 D 2 8 |
   | 16–31 | 2 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
   | 32–47 | 2 2 2 4 2 2 2 2 2 2 6 2 D 1 4 8 |
   | 48–63 | 2 1 2 2 1 2 1 127 3 8 4 4 15 5 15 3 |
   | 64–79 | D × 15, then 127 (id 79) |
3. **Stage overlays**: overlay 07 is stage 4 area 1 [certain: `shark-6_before-boss` and
   `shark-7_boss` match `rock_bin/07.bin` at `0x800E9860` except for 153 / 156 variable bytes].
   Stage 4 has no boss overlay: nothing is loaded at `0x800FA000` in the boss states. The list
   becomes "intro 02, stage 1 = 03, stage 4 = 06 and 07, …" with 07 certain.
4. **Animations, "More than one sprite set"**: add "Metal Shark Player (`0x800F4144` /
   `0x800F6E78`, overlay 07): set 1 = bank + 1, texture `0x540`, for animations 34–37 (the side-kick
   silhouettes and the stun)."
5. **Animations**: `SetAnimeFrame(obj, n, frame)` (`0x800179D0`, main program) starts animation n at
   a given frame; `PlayerSetAnimeAt` already calls it. Some objects use frames after the first loop
   of an animation (Metal Shark Player animations 27 and 33), so the kit's tables, which stop at the
   first loop, can miss frames.
6. **Engine objects**: the boss intro sequence "PlayerEvent(29, 64), engine object 24, … engine
   object 26 at death, soul var 0x10 under 5401 updates" is now seen in four bosses (Turtloid,
   Sheldon, Yammark, Metal Shark Player). It could go to `docs/engine.md` once, with the sheets
   linking to it.

## `docs/stages/shark.md` (section 2 owner)

- "Area 1 (overlay 07)": main 56 `id` 0 at (6231, 350) is Metal Shark Player [certain:
  `shark-7_boss`]; link `docs/enemies/metal-shark-player.md`. The boss room: camera x 6064–6096, y
  224 (limits in `shark-7_boss`), X on the floor at y 395.
- Overlay 07 is certain (above).
- The state list: `shark-6_before-boss`, `shark-7_boss`, `shark-8_boss-stun`, `shark-9_boss-revive`
  (all stage 4 area 1, checkpoint 2, difficulty 1, X form 1).

## `docs/TODO.md`, `docs/process/process-log.md`

- Process log: "Metal Shark Player (main 56, overlay 07) sheet from `shark-7/8/9` states; stun ids
  6/15/42 tied to weapon slot 6 by `shark-8_boss-stun`; three side kicks (main 56 `var` 1–3)."
- TODO: play checks of the sheet's "Open questions" (side-kick names; ids 35 and 42).
