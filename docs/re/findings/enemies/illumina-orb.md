# Orb robot (stage 11 and 23, main object type 11): behaviour sheet (partial)

A small grey and blue robot that is closed into a teal ball. It drops to the floor, rolls along the
floor, walls and ceiling, and at a corner it can stop, open and fire at the player. It can be hit
only while it is open. At most five can be live at once. Illumina makes it on Hard (`var` 16,
`docs/enemies/illumina.md`); in Gate stage 2 it comes from records. "Orb robot" is a descriptive
name (the export is `illumina-orb`); no official name was found.

Code in overlay `23` (stage 17, entry `0x800ED734`, state table `0x800F8EFC`) and overlay `11`
(stage 8): the 28 functions of the two kits are identical after masking [certain]. Save state
`gate-3_stage-2.state.gz`: two orbs live (records 41 and 42; steps 11 and 11, animations 5 and 2:
both in the attack) [certain]. Kit: `python3 -m mmxdemake.rekit --state gate-3_stage-2.state.gz
--type 11` (the overlay 11 kit: [`~/mmx-data/x6/re/mijinion-bot/kit-main11/`](../../work/mijinion-bot/kit-main11/)). Export:
`illumina-orb` (`enemy-c3301f6267`). Units, tags and the shared rules: `docs/engine.md`. Records:
`docs/stages/gate-2.md`. Addresses are those of overlay 23. The rolling steps (3–10) and the shot
(visual 10) are read only in part.

## Spawn and set-up (state 0, `0x800EBFD8`) [certain]

- When four other main objects of type 11 are live (`0x800EDA60`), it deletes itself (state 3).
- `var & 0xF0` = `0x10`: it blinks for 60 updates (`work.84`).
- **HP 8**, **contact damage 4**, `dmg_tbl_minor`, draw priority 5, layer = the player's layer,
  animation table `0x800F8EBC`, animation 0 (the closed ball, sprites 0–3).
- Attack box `0x800F8EF0` = (−8, −8, 16, 16). **No hurt box**: it cannot be hit while closed.
  Terrain box: the 4 bytes at `+0x86` (from `0x800F8EF8`, all 0 at the start; the rolling steps
  change them).
- Speed y = 2 px/frame down, no gravity, `latch` 1. Sound 2/60. Step 2.

## State machine [certain unless marked]

States (`0x800F8EFC`): 0 set-up, 1 main `0x800EC118`, 2 and 3 delete.

Main (state 1), each update: the step (table `0x800F8F0C`), `AnimeTick`, `ContactMega`,
`ContactWeapon`. On a kill: explosion, `SpawnJunk(3, {9, 10, 11})`, `DropItem(obj, 18)`, state 2.
Off screen (160, 120): state 3. It also breaks when the terrain touches both sides (crushed). Then
`CollideCheck`.

| step | routine | what |
|---|---|---|
| 2 DROP | `0x800ED7EC` | falls at 2 px/frame; on the floor: speed x 3.5 px/frame toward the player, step 3 (right) or 7 (left) |
| 3–10 ROLL | `0x800EC2C8` … `0x800ED188` | eight move steps: the ball follows the floor, walls and ceiling at 3.5 px/frame (four surfaces, two directions) [likely]. At each change of surface, `0x800EDAD8`: a 1 in 4 roll (`GetRNG & 3` = 0) saves the step in `+0x8B` and goes to step 11 |
| 11 ATTACK | `0x800ED874` | sub-steps below; then back to the saved step |

ATTACK (table `0x800F8F50`):

1. Face the player; animation 1 (it opens: sprites 4–11).
2. At frame flag 1: hurt box `0x800F8EF4` = (−6, −6, 12, 12). At flag 2: aim at the player
   (`ratan2`), animation 3–7 by the angle (table `0x800F8F3C`: 16 sectors).
3. At flag 1 (`0x800ED3A0`): one shot (visual 10, `var` 0) at the angle. On Hard and above
   (difficulty ≥ 2) the step runs twice, so a second shot follows (`var` 1). At flag 2: animation 2
   (it closes).
4. At flag 1: no hurt box. At flag 2: animation 0, sound 2/60, back to the saved step.

## Animations

| # | sprites | use |
|---|---|---|
| 0 | 0–3 | closed ball, rolling |
| 1 | 4–11 | opens |
| 2 | 10 9 7 5 | closes |
| 3–7 | 11/26, 12/24, 13/25, 27/28, 29/30 | aim and fire, five angles |
| 8 | 14–19 | (the shot, visual 10) [likely] |
| 9–11 | 20–22 | debris |

## Damage and death

`dmg_tbl_minor`, 8 HP; only while open. Death: explosion, 3 debris pieces, drop table 18.

## Summary for a reimplementation

1. 16 × 16 px ball, 8 HP, contact 4, at most five live.
2. Drop at 2 px/frame; roll at 3.5 px/frame toward the player along every surface.
3. At a corner, 1 in 4: open, aim, fire one shot (two on Hard), close; hittable only while open.

## Open questions

1. The rolling steps 3–10 in full (the terrain box changes, the 5-point probe).
2. Visual 10 (the shot): speed, damage and box (`0x800ED4B8`, switch `0x800E98D4`).
3. The official name.
