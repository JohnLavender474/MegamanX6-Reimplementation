# Infinity Mijinion boss fight (stage 8, overlay 11, main type 58): notes

Date: 2026-10-23. Tags as in `docs/engine.md`: [certain], [likely], [guess].

## Method

- State: `~/mmx-data/x6/dumps/mijinion-3_boss-fight.state.gz` (SHA-1
  `fc50684428d3f921983d2730df60915ac9755ac2`). For comparison: `mijinion-2_illumina-helpers` (room
  collision), and the other agent's bank sheets of `mijinion-1_start` (bank 3 check; read only).
- Kit: `python3 -m mmxdemake.rekit --state mijinion-3_boss-fight.state.gz --type 58 --name infinity-mijinion`
  → `kit/` (123 functions, 24 state tables, neighbour visual 25). All functions were read.
- Effect 10: `rekit --class effect --type 10 --out kit-effect10/` (14 functions; its code is in the
  gap `0x800F7670`…`0x800F7E38` and `0x800FA418`, `0x800FACF8`…`0x800FB068`). Read. `0x8002C6F0`
  (called by `0x800FA418`) not read.
- Scripts (`scripts/`, run from that folder): `probe.py` (copied from `blizzard-wolfang`: all pools,
  output `scripts/probe-3.txt`), `bossfields.py` (every main 58 object, the overlay globals, game
  fields), `cells.py` (collision x 5440–6239, y 768–1247), `montage.py` (contact sheet of an export).
- Sprites: `spriteexport --root sprites-root` on the fight state only.

## Confirmation that main type 58 is Infinity Mijinion [certain]

- The only type-58 record (54, x 5968) is in the room after boss gate 27, the last record.
- The code calls `BossLevel`, `TextOpen(0, 255, game+0x2E)`, `BossMusicRequest` / `BossMusicWait`,
  makes `BossWarning` (logic 24) and `BossExplosion` (logic 26) and the Nightmare soul (main 15).
- It is `game+0x20` in the state; its HP is `boss_hp_max`.
- Its sprites (bank 15, texture `0x140`) are the purple dragonfly Maverick with green bubbles.
- Its damage table deflects slot 8 (ids 8, 17, 44: Ray Arrow, his weapon) and is stunned by Guard
  Shell (slot 7, Shield Sheldon), which fits the weakness chain of the other sheets.

## Live values (`mijinion-3_boss-fight`)

| what | value |
|---|---|
| game | stage 8 area 0 checkpoint 1; character 1 (Zero); difficulty 1; `game+0x8D`/`+0x8E` 3/3 → level 1; `boss_hp_max` 52; `game+0x24` 1; `game+0x2E` 1; stage time 6582; `nightmare_effect` 3 |
| player | step 2, (5868, 1098), HP 34, event fields `+0xD0` 0 (entry over) |
| camera | (5808, 927); target min x 5808, max x 5824, min y 832, max y 928 |
| boss (main slot 0) | state 1 **step 6 sub 1** (teleport blink), (6016, 1040), display 1, animation 0, `flip` 0, HP 52, contact 6, `+0x7C` 1, `+0x8A` 1 (no boxes), `+0x86` 0, `+0x87` 11, `+0x88` 1, lemons `+0x8B` 0, quarter `+0x8E` 0, clut `0x794C` |
| overlay globals | `0x800FDA74` = `0x8009B960` (the old warning slot), bars 0 0 0, clones 0, bubbles 0, `0x800FDA80` 5882, fight timer 31 |
| stage objects | gate `var` 27 (5808, 1088) state 2, gate `var` 0 (5488, 1088) state 2 |
| logic | 14 (talk zone), 5, 6 × 3, 27 |
| visual / effect | none |

Checks of the reading [certain unless marked]:

- HP 52 = 48 + {0, 4, 6, 8}[1]. Level 1 = `boss_level_tbl`[`game+0x8D`[character 1]] =
  {3, 3, 2, 1, …}[3].
- `+0x87` 11, `+0x86` 0: table A row 11 = (6, 7); `+0x88` 1: the first byte (step 6) was taken. The
  boss is in step 6.
- Position (6016, 1040) = room (208, 208): the entry place; step 6 has not yet moved him (sub-step 2
  moves him to (5976, 976)).
- `+0x8A` 1 and no boxes: step 6 sub-step 0 sets it. `+0x7C` 1: the 30-update blink is at its end.
- The fight timer 31: the entry ended 31 updates before the save (step 2 does not count).
- `0x800FDA80` = 5882: the sum of 14 random steps of RNG + 255 (each ≥ 255, so ≥ 3570) [likely].

## Key facts and where they come from

See `sheet.md` for the full reading and `pseudo.c` for the code. Points worth a second look:

1. **Weakness ids.** The switch at `0x800F5F00` has cases 7, 16, 53 for the stun. `dmg_tbl_mijinion`
   id 7 = `02 00` (class 2): `ApplyWeaponHit` returns 0, so `ContactWeapon` never gives 8 for id 7.
   The same in `dmg_tbl_deflect`. So only ids 16 and 53 stun [certain for the tables].
2. **Hang bars.** `CopyTiles` source `0x800FD45C` = (5632, 1200, 1, 1); `cells.py` shows collision 45
   at (5632, 1200) and 0 at (5632, 1216) (`0x800FD464`). 45 is the flat bar of
   `docs/players/air.md` (step 28). The arrow (visual 25) tests 45 with `0x800FA630`, so it stops at a
   bar.
3. **Bar growth.** `0x800F8390`: a hit sets `+0x8A` when the player is Zero (`mega+0x02` ≠ 0) or the
   result is 1 (id 0, X's lemon). `0x800FA664` sets it for an arrow overlap. `0x800F6CB0` then adds
   32 to the target length.
4. **Quarter test.** In `0x800F5FCC`: `+0x8E` 0 → `3 × max / 4`, 1 → `max / 2`, 2 → `max / 4` (all
   rounded toward 0); HP ≤ limit → step 12 sub-step 3 (`0x800F97BC`), which makes a clone without the
   cap test.
5. **Clone sabre kill.** `0x800F61D4`: index = result − 25 = id − 24; cases 0–4, 13, 15, 26, 27 =
   ids 24–28, 37, 39, 50, 51; only when the clone count ≥ 2; the 4th counted hit kills.
6. **Heal.** `0x800F900C`: `boss_hp_max × 0x55555556 >> 32` = max / 3; `0x800F90A0` gives 1 HP per 8
   updates.

## Hazards (for a reimplementation)

- `MijinionNextMove` reads a row until `0xFF`; every row has `0xFF` within 3 bytes [certain for the
  data].
- The stun bar slot search (`0x800F6A1C`) spawns into the last slot it looked at even if it is used;
  it is safe only because the caller first tests that not all three are used [certain for the code].
- A bar emitter deletes itself without the tile restore when the boss leaves state 1 (death): the
  collision 45 cells stay in the map until the area reloads [certain for the code; not seen].
- The quarter-HP clone ignores the clone cap; with the 4 clone sources the live count can pass
  {1, 3, 5, 7}. The clone count is an s16 with no upper test; the main pool (48 slots) is the real
  limit [certain for the code].
- Step 10 sets `+0x84` = 10000 for the deflect; only its end (`+0x84` = 1) or a stun/knock-back path
  changes it. A hit in step 10 cannot happen (deflect), so it is safe [certain for the code].
- `ApproachEvery` (`0x800FA584`) divides by its 4th argument; the only caller passes 1.

## Extractions

- `sprites-root/enemies/enemy-b75a43c056` (main 58, bank 15 live, table `0x800FD278`, 29 animations,
  cell 101 × 84, origin (59, 42)): **good**. Checked by eye: every animation matches its use in the
  code (hover, fire, dive, landing flash, appear, copy pose, hurt, the arrow in 3 directions, its end
  ring, runner ball, bolt and its floor hit, the two glows, the bubble with the small copy and its
  burst, the green bar light, the entry particle and the entry bubble, the sparks, the bar pieces).
  `bank-15-live-0.png` (168 sprites) is clean. **Copy it to `~/mmx-data/x6/sprites/enemies/` and name
  it `infinity-mijinion` in `catalog/enemies.json` `names`.** The shots (visual 25) and effects
  (effect 10) use the boss's table and sprites, so this export holds all their animations; no other
  export is needed.
- `enemy-08bc9efa49` (main type 8 of stage 8, bank 3, found by search, 7 animations) and
  `bank-3.png`: **wrong**. The sheet shows broken orange, red and cyan pieces. In `mijinion-1_start`
  (the other agent's root) bank 3 is a clean red-and-blue jet with blue thruster balls. The tool
  listed bank 3 as drawn ("type 8, 33 sprites"), not as overwritten. Cause [likely]: the boss pack
  texture covers its cells and the overwrite test misses this bank. Do not copy; list type 8 as
  skipped for this state in `sprites/stages/stage-8.json`.
- `boss-gate`, `nightmare-virus`, `nightmare-virus-orb`, `teleport-crystal`: the same IDs as in
  `sprites/`; nothing new.
- Banks 0, 1, 2, 8–11, `stage-8-live-0`: look right (bank 1 and 2 are the same as in
  `mijinion-1_start`). Banks 4–7 were skipped as overwritten by type 58 (correct).
- `sprites/stages/stage-8.json` does not exist yet in the shared folder; the main session makes it.

## Open questions

See the sheet. Also: the sound use is from the code order only; the WAV files were not listened to.
