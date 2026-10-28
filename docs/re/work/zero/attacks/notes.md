# Notes: Zero's attacks and techniques (player steps 48–63)

Date: 2026-10-22. Sheet: [`~/IdeaProjects/mmx-demake-tools/docs/players/zero.md`](../../../findings/players/zero.md).

## Method

- Kit: `../kit/` (`python3 -m mmxdemake.rekit --player --state turtloid-6_zero-player.state.gz`).
  The kit walks all 64 functions of overlay 1 (`0x801EA004`–`0x801EE39C`), the weapon objects
  of Zero included (they are overlay functions that the walk found by their calls).
- Code outside the kit: `python3 -m mmxdemake.rekit --state <state> --at <addr>` (one function)
  and `~/mmx-data/x6/re/player/disasm.py <state> <addr> <n>` (raw words).
- Scripts in `scripts/` (run from `~`, not from this folder):
  - `zprobe.py` loaders; `wtypes.py` weapon type table `0x80071220`;
  - `tables.py` sabre object table `0x801EE68C`, combo sounds `0x801EE3BC`, hit boxes
    `0x801EE414` / `0x801EE580`, terrain boxes `0x801EE2D4`;
  - `anims.py <n…>` frame timelines of Zero's animations (table `0x801EF6D0`, read from the kit);
  - `boxes.py` box index per frame of each sabre hit object; `effanims.py` shared effect
    animations (`0x8007798C`);
  - `cost.py` energy cost tables; `jt.py` the damage bonus jump tables of `ApplyWeaponHit`;
  - `live.py` the live player fields and weapon objects in the five Zero states;
  - `findref.py <state> <addr…>` jal or word references in RAM; `findimm.py` byte loads/stores
    by offset.

## Key facts and where

| fact | where |
|---|---|
| sabre hit objects: weapon types 24–31, 35–42, 48 all run `0x801ED2A8` | `weapon_types` (`wtypes.py`) |
| other Zero types: 33 `0x801ED928` (icicle), 34 `0x801EDAF4` (fire wave), 44 `0x801EDFC8` (Rekkoha beam), 58 `0x801EDCC8` (Z-Buster shot) | `weapon_types` |
| Zero also makes main-program types 1, 4, 7 (X's slot 1, 4, 7 weapons) | `0x801EA254`, `0x801EBA7C`, `0x801ECBE8` |
| `SpawnSabre(m, type)` `0x801ECD90`: weapon object flag 1, var = `mega+0xCB`, `+0x84` = Zero's animation, flip | kit |
| `SpawnSabreOnFlag(m, type)` `0x801ECD54`: frame flag `0x80` → clear it, `SpawnSabre` | kit |
| `SabreStart(m)` `0x801ECD3C`: `+0xDD` = 1, `+0x8E` = 1, `+0xCB` = 0, `+0x8C` = 0 | kit |
| `TrySabre(m)` `0x801ECCD4`: Zero, not locked, attack (`0x10`) pressed, `+0x8E` = 0 → `SabreStart`, 1 | kit |
| sabre object table `0x801EE68C` (4 bytes per type − 24: animation, palette, "extend" flag, ?) | `tables.py` |
| hit box per frame: the object's frame flag indexes `0x801EE414` (91 boxes), with part `0x800000` `0x801EE580` (67 boxes, 12 px longer in front) | `0x801ED6F0` |
| combo: `+0xCB` = hit number; flag `0x20` frames take the next press; flag `0x10` frames: the opposite direction held turns and slashes once | `0x801EB12C` |
| combo sounds `0x801EE3BC`: (sound, voice) = (7, 3), (8, 4), (7, 5) by `+0xCB` | `tables.py` |
| skill bits = `mega+0xC9` = `game+0x60` (`PlayerLoadForm`); one byte for X's weapons and Zero's techniques | `0x8003D718` |
| `game+0x60` bit n = the Maverick of stage n + 1 beaten | `docs/engine.md` (observed 0x01 → 0x21 → 0x29); states: `0x69` with stages 1, 4, 6, 7 cleared |
| Zero energy slots (`mega+0x93`): 0 sabre, 1 Hyoroga, 2 Rakukojin, 3 Yammar Option, 4 Guard Shell, 5 Rekkoha; L/R (`0x200`/`0x400`) step through owned ones (bit table `0x80073EC0`: 0, 2, 8, 1, 0x40, 0x80) | `0x8003DBDC` |
| Zero costs (`0x80073DF4`): slot 1–4: 2, slot 5: 12; × 6 (× 3 with part `0x20`, × 4 as Black Zero); bar 288 | `0x8003EB0C`, `cost.py` |
| Guard Shell starts only from the pause-menu close when slot 4 is selected | caller `0x80034D78` (`findref.py`) |
| Black Zero (`game+0x5F` bit `0x20`): sabre +1 damage (ids 24–31, 37–41), damage taken / 2, no hit knock-back, cost × 4, black palette, sabre palette 4 | `0x800315CC`, `0x80030DC0`, `0x80035C58`, `0x80039740`, `0x8003EB64`, `0x8003C63C`, `0x801ED56C` |

## Live check (`scripts/live.py`)

| state | stage | `game+0x60` | `mega+0xC9` | step | animation | `+0xCB` | HP | energy (10 slots) | weapon objects |
|---|---|---|---|---|---|---|---|---|---|
| `turtloid-6_zero-player` | 6 | `0x01` | `0x01` | 2.0 | 5 (idle) | 2 | 11 | 288 each | none |
| `turtloid-7_boss` | 6 | `0x01` | `0x01` | 2.0 | 5 | 2 | 28 | 288 | none |
| `turtloid-8_high-max` | 277 (21.1) | `0x21` | `0x21` | 17.1 (hurt) | 34 | 0 | 26 | 288 | none |
| `scaravich-1_start` | 5 | `0x69` | `0x69` | 2.0 | 5 | 0 | 32 | 288 | none |
| `scaravich-2_totem-section` | 5 | `0x69` | `0x69` | 2.0 | 5 | 0 | 32 | 288 | none |

- All five: character 1, form 5, `game+0x5F` = `0x11` (no Black Zero), parts 0, `+0x144` = 0,
  `+0x8E` = 0, `+0x93` = 0, `+0xA6` = 0, `+0x14A` = 288 [certain].
- `mega+0xC9` = `game+0x60` in every state: the copy of `PlayerLoadForm` [certain].
- `+0xCB` = 2 in the two Turtloid stage states: the last action was a full three-hit combo (the
  byte is not cleared at the end) [likely].
- The physics constants `+0xF0`…`+0x134` are the same as X's (walk 2.0, jump 5.8125, dash 6.5,
  …) [certain].
- No state has a live sabre object or a technique in progress, so the timings below are from the
  code only. A state in step 48, 55 or 59 would test them. (step 59: `heatnix-3_rekkoha` agrees
  with the code: waves, positions, hit tags, hit count, energy; `../../zero-rekkoha/notes.md`.)

## Open

- `mega+0xCD` is set to 1 when ids 26–31, 35, 46, 51, 57, 61, 62 hit (class not 7,
  `ApplyWeaponHit` `0x80031678`). No reader found.
- Sabre object `+0x9C` (= sabre table byte 1 of the next type, with part `0x1000000` or Black
  Zero): no reader found.
- `mega+0x144`: 6 gives 5 Z-Buster shots in one step 62 (else 1); 2 gives +1 damage to all hits;
  3 makes the energy free. What sets it is not known (a cheat or test mode, guess).
- Type 36 (Sentsuizan) gets a terrain box from `0x801EE2D4` on its frames with flags 75–79; the
  reason is not known.
- What the main-program weapon objects types 1, 4, 7 do for Zero (Yammar Option, Metal Anchor,
  Guard Shell): see the X weapons sheet (`docs/players/weapons.md`) when it exists.
- Rekkoha (`../../zero-rekkoha/notes.md`): `mega+0xE0` = 2 during step 59 (no reader read);
  `mega+0xD3`; who uploads the backdrop CLUT 416. `pseudocode.md` calls effect type 26 a "light":
  it is the top part of the beam; the type 17 objects are a scrolling backdrop (sprites 83/84),
  not animations.

## Recursion and re-entry

- **Step 48 calls itself** [certain]. `ZeroStep48` (`0x801EAE7C`) starts a new swing in its tail
  (animation 88, step 48, sub-step 0) and then calls itself at `0x801EB0E8`, so the first update of
  the new swing runs in the same update as the press. The reconstruction keeps this
  (`StartGroundSabre` → `ZeroStep48`).
- **Why it ends after one level:** in the second call, sub-step 0 becomes 1 and the code jumps to the
  tail (`0x801EAFE4`). The tail tests flag bit `0x01` (`ZF_HOLD`) of the current frame. Frame 0 of
  animation 88 is `(1 tick, flag 0x01, 1, sprite 60)` (Zero's table `0x801EF6D0`, entry 88 →
  `0x801EEF88`), so the tail sets the busy flag `+0x8E` = 1 and goes to `GroundEnd`; `TrySabre` is
  not reached. The stop depends on the animation data, not on a test in the code: a frame 0 without
  `0x01` and with `0x40` would recurse until the stack overflows.
- **The same pattern, without recursion:** `TryShoenzan` → `ZeroStep55`, `TrySentsuizan` →
  `ZeroStep57`, `TryEnsuizan` → `ZeroStep58`, `TryRekkoha` → `ZeroStep59`, `TryZBuster` →
  `ZeroStep62`: start the new step, then run its first update at once. These steps do not call back
  into their `Try` function.
- **For a reimplementation:** do not copy the recursion. Set the new animation and step, then run the
  step body once without the "start a new swing" checks, or let the next update run it.
