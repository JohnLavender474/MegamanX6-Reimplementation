# Proposed changes from the hidden-area work (not applied)

Agent: hidden areas, `boss_choice`, Dynamo, Zero Nightmare and High Max open items. Files changed
in the repo: `docs/enemies/dynamo.md` (new), `docs/enemies/high-max.md`,
`docs/enemies/zero-nightmare.md`. Probes: [`~/mmx-data/x6/re/hidden-areas/scripts/`](scripts/) (`kit45.py` runs
the RE kit with `45.bin` mapped at `0x800FA000`; output in `kit-dynamo/`; shot code in
`shots.txt`).

## `catalog/helpers.json`

Add `functions` and `globals` of `helpers-proposed.json`; replace the three entries in its
`changed` block.

## `docs/engine.md`

### "Boss overlay" (the `boss_choice` bullets)

1. Replace "the name for 2 is likely" with: value 2 (and more) is Dynamo: overlay 45, main type 63,
   sheet `docs/enemies/dynamo.md` (from the code only; no save state) [certain for the code].
2. Replace "Overlays of the hidden areas: … [likely; two points only]" with: the stage overlay
   table `stage_overlay_tbl` (`0x8006DB78`, 2 halfwords per stage) gives stage 19 = 25/26, 20 =
   27/28, 21 = 29/30, 22 = 31/32 [certain].
3. Replace "The overlay 16 writers act only when `0x8008EAFC` = 0 [open: what that flag is]" with:
   each overlay 16 writer opens a message first (`TextOpen(2, 255, 0)` before mode 10,
   `TextOpen(1, 255, 0)` before mode 11) and writes `boss_choice` when the message is closed:
   `0x8008EAFC` is `text_open` (the window state `0x8008EAF0` + `0x0C`). Mode 10 also writes
   `game+0x66` = 3 [certain for the code].
4. Replace "and `0x80079F20` (138 / 139 / 140, meaning open)" with: and `stage_member_tbl`[stage]
   (`0x8007A0E0`, halfwords; written at `0x800529A4`): the `ROCK_X6.DAT` member 138 / 139 / 140 that
   the stage-logic loader `0x800528F4` reads with `0x80014F70` while the stage runs. Member 138 is in
   RAM in `turtloid-5_zero` and 139 in `turtloid-8_high-max` (64-byte chunks at `0x800E0D40` and
   `0x800CF998`…); so they are the boss data, and 140 is Dynamo's [certain for 138 and 139; likely
   for 140]. (The address `0x80079F20` was a wrong subtraction: `0x80080000` − 24352 =
   `0x8007A0E0`.) `boss_choice` also picks the hidden-area music: `0x80017FF0` passes 25 / 23 / 24 to
   `StreamPlay` (`0x80018060`).
5. Add: **What resets `boss_choice`.** No game code writes 0 to it. `NewGame` (game mode 0,
   `0x8001E088`) does not write `game+0x43B`. It is 0 after power-on because `Start`
   (`0x80054AD8`) clears `0x8008EAA8`…`0x800E985F`. `LoadGame` and overlay 18 (stage 14 area 0,
   `0x800ECFEC`) copy save `+0x3F` into it. So a new game started after another game without a
   reset keeps the old value [certain for the code; not tested in play].
6. Add: **Where a hidden area leads** — a short version of `docs/enemies/high-max.md`, "Where the
   clear leads": exit `0x10` → `StageExit`: `boss_choice` 0 → Zero playable, scene 2 (overlay 48),
   then the report (or scene 5 at 3000 souls); 1 → scene 4 (overlay 50), then the report, where
   `boss_choice` becomes 2 and scene 6 / 7 (Gate's lab) is queued; 2 → the report. Then the scene
   list, then the stage select. Never back to the stage.

### "Game state" table

- `+0x5F` (`game.armors`): add "bit `0x10` Zero can be played (`StageExit`, `boss_choice` 0); bit
  `0x20` Black Zero (Light capsule `id` 8 as Zero, the title code `0x800E3134`, Zero Nightmare's
  death at boss level 3) [certain]".
- New rows: `+0x33` (`0x800CCF03`) Dynamo's soul drops (at most 3); `+0x66` (`0x800CCF36`) story
  phase [likely]: 1 after the intro, 3 Gate's lab open, 4 after stage 16, 5 after stage 18; the
  Gate's lab scenes and the 3000-soul scene come only below 3; `+0x9E` (`0x800CCF6E`) message-seen
  bits (`TextSeen`); `+0x158` (`0x800CD028`) scene list (16 bytes, `0xFF` end).
- Note under the table: game mode 11 (`0x8001E81C`) and the scene ends clear `+0x26`…`+0x35`.
  ([`~/mmx-data/x6/re/scaravich-crystals/doc-changes.md`](../scaravich-crystals/doc-changes.md) calls `0x8001E81C` "game mode 12"; it is
  entry 11 of `game_modes` (`0x8007112C`), and it sets mode 12.)

### New subsection "Game modes" (after "Game state")

`MainLoop` (`0x8001E6E4`) runs `game_modes`[game byte 0] (`0x8007112C`) [certain]: 0 `NewGame`;
3 stage select set-up (stage 13 area 0, overlay 16); 4 stage select (overlay); 5 (`0x8001E7C0`:
saves the stage, stage 13 area 1); 8 stage load (`StageStartReset`); 9 (`0x8001E1BC`); 10 stage play
(sub-modes `0x80071184`: play, `StageExitWait`); 11 report set-up (stage 15, overlay 20); 12 report
(overlay); 13 game over [likely]; 19 scene set-up (stage 11, overlay 12 + scene overlay 46 + n at
`0x800FA000`, n = `game+0x1D`); 20 scene (overlay); 21 next scene of `game+0x158`, or mode 3.

### "Weapon ids"

- Add the owner rule and table from `docs/enemies/high-max.md`, "Weapon ids of the guard break
  and the stun": each entry of `weapon_types` that points into `0x801EA000`… is a function start
  in only one of the two player overlays (X 00, Zero 01) [certain].
- Row `50, 51`: replace "[guess: a sabre]" with "X's saber slash: 50 on the floor, 51 in the air or
  on a wall (slash objects of steps 37, 38, 45–47, 77; the saber and Magma Blade; X's overlay
  only) [certain for the code; the name likely]". Add: id 50 starts Zero Nightmare's STUN WARP.
- New rows: 44 = Zero's special attack (step 59) objects [certain for the code]; 58 = Zero's step
  62 shot, with the Buster Plus bonus: the Z-Buster [likely]; 24–31, 35–42, 48 = Zero's sabre hit
  objects (one routine `0x801ED2A8`) [certain for the code]; 6, 15, 42, 46 stun Dynamo [certain].

## `docs/TODO.md`

- Close "Hidden areas: where a hidden area leads after its boss" (answer: `docs/enemies/high-max.md`,
  "Where the clear leads"). Keep "a hidden area with `boss_choice` 2" as a save-state request.
- Close "What resets `boss_choice` for a new game; what `0x8008EAFC`, `0x80097424` and
  `0x80079F20` are": nothing resets it except the boot clear; `text_open`; `fade_dir` (0 = no
  fade; the exit waits for it); `0x80079F20` is really `0x8007A0E0`, the boss data member.
- Close "Zero Nightmare open items": id 50 = X's saber slash on the floor; `game+0x5F` bit `0x20`
  = Black Zero.
- Change "High Max open items" to: who sets X's shot type 56. The other guard-break ids now have
  owners and names (`docs/enemies/high-max.md`; names from `docs/players/weapons.md` and
  `docs/players/zero.md`).
- New: "Dynamo (`docs/enemies/dynamo.md`): check in play with a `boss_choice` 2 state; export from
  `ROCK_X6.DAT` member 140 or a live state."
- New: "Scenes 2, 4, 5, 6–12 (overlays 48–58): what each shows."
- New: "What `game+0x8D` counts (boss level 3 gives Black Zero after Zero Nightmare)."

## `docs/stages/turtloid.md` and `docs/stages/sheldon-alt.md`

In the boss-room text: "with `boss_choice` 2 or more the boss is Dynamo (`docs/enemies/dynamo.md`)";
and "after the boss: `docs/enemies/high-max.md`, 'Where the clear leads'".

## `catalog/enemies.json`

No change: Dynamo has no export. When one exists, name it `dynamo`.

## `docs/process/process-log.md` (proposed section)

**Hidden areas: where they lead, `boss_choice`, Dynamo.** A helper agent read `StageExit`, the game
mode table and the report screen (overlay 20). After a hidden-area boss the game never goes back to
the stage: with `boss_choice` 0 Zero becomes playable and scene 2 plays; with 1 scene 4 plays; then
the report screen, which turns 1 into 2 and queues the Gate's lab scene; with 2 the report comes at
once. Nothing resets `boss_choice` at a new game: only the boot clear makes it 0. `0x8008EAFC` is
the text-window flag: the overlay 16 writers wait for their message to close. The `0x80079F20` of
section 70 was a wrong subtraction; the real table `0x8007A0E0` names the boss data member (138 and
139 found in RAM). Dynamo (overlay 45) has no save state, so the agent mapped `45.bin` over a
hidden-area state and ran the kit on it: HP 50–66, boomerang blades, a row of 8 pillars, a centre
volley and a leaping slash; ids 6, 15, 42, 46 stun him and a hit in the stun drops a 200-soul
soul. Weapon ids now have owners: an entry of `weapon_types` is a function start in only one of
the two player overlays. Id 50 (Zero Nightmare's stun) is X's saber slash; `game+0x5F` bit `0x20`
is Black Zero.

Lessons:

- An overlay that no state holds can still go through the kit: map the disc file at its load
  address over a state of the same stage.
- When two overlays share one address range, a pointer table over that range can be split by
  checking which overlay has a function start at each entry.
- Check hand arithmetic of `lui` + negative `addiu` pairs; one wrong subtraction gave a wrong
  address in the docs.
