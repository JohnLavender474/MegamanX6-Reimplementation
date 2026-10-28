# Notes: player ladder, hurt, beams, ride, events (part 2c)

Sheet: [`~/IdeaProjects/mmx-demake-tools/docs/players/ladder-hurt-events.md`](../../../findings/players/ladder-hurt-events.md).

## Method

- Kits: `../kit/` (X, `intro-1_start`) and `../../zero/kit/` (Zero, `turtloid-6_zero-player`).
  `../movement/fn.sh <addr>` prints a function from the X kit.
- Code outside the kit: `scripts/dumpmain.py <state> <lo> <hi> <out>` writes a plain disassembly
  (run with `PYTHONPATH=~/IdeaProjects/mmx-demake-tools` from the repo). Outputs here:
  `main-x.dis` (main program, X state), `ovl0-x.dis` (X overlay), `ovl1-zero.dis` (Zero overlay).
- RAM words: `scripts/peek.py <state> <hexaddr>:<bytes> …`; pointer search:
  `scripts/findword.py <state> <hexword> …` (found the player state table `0x80073ABC` from the
  address of `PlayerUpdate`).
- Overlay scans (inline Python over `~/mmx-data/x6/extracted/retail/rock_bin/*.bin`): calls of
  `PlayerEvent` (`jal 0x8003D308` = word `0x0C00F4C2`) with the `addiu a0, zero, n` before it;
  stores to `mega+0xD5`, `+0xD8` and of 18 to `+0x05`; loads of `0x80097185` (`+0xE5`).
- Read in order: steps 14–16 and the ladder entries, `PlayerUpdate`, `PlayerHitCheck`
  (`0x80039228`), `PlayerHurt` (`0x8003963C`), step 17 and the handlers, `ContactMega`, the player
  states (0, 2, 3), the death steps, steps 0 and 1, `PlayerExit`, `BeamOut`, the event switch and
  steps 20–25, 29, 30, the ride (`0x8003A268`, step 18), step 5, Zero's idle.
- Animation data: `~/mmx-data/x6/sprites/players/{x,zero}/animations.json`. **The list is not
  indexed by animation number** (X has no 85, 180–182; Zero has no 118): look up by `index`.

## Key facts and where

| fact | where |
|---|---|
| "hurt" `0x80039324` of `docs/engine.md` is a branch target inside `PlayerHitCheck`: the squeeze damage | `0x800392BC`…`0x800393F4` |
| HP bit 7 = hit waiting (HP > 0); `0x80` = dead | `ContactMega` `0x80031200`, `0x8003121C`; `PlayerHitCheck` `0x800394FC` |
| hit class from the attacker's `+0x62`, or by form and damage | `0x800310B8`…`0x8003119C` |
| hurt tables (anim, motion, iframes, handlers) | `0x80073C70`, `…C78`, `…CD8`, `…C30` (RAM in 3 states equal) |
| hurt ends at `+0x61` = 60 | `0x800378CC`, `0x80035C8C`, `0x80037958` |
| blink at half speed with part `0x400` | `0x800394B4` |
| spikes: tiles 62, 63 set `+0x79` unless form 2 | `0x80030B5C`, tables `0x80010E78`, `0x80010E98` |
| pit death | `0x800282D0` (in the camera clamp `0x8002820C`) |
| death = state 2, 4 steps at `0x80073D20` | `0x80039468`…`0x80039490` |
| init table `0x80073CF4` (beam / no beam in stage 12) | `0x8003CBE4` |
| beam fall −8.0, beam-out +8.0 | `0x8003D87C`, `0x8003B978` |
| step 0 = beam-in, step 1 = beam-out | `0x80035144`, `0x8003674C`, `0x8003B954` |
| exit request bits | `0x8003B870` |
| ride: `+0xD5` > 0 → step 18, `+0xD5` negated | `0x8003A268` |
| `actor2` hit while riding | `0x80030F64`…`0x80031058` |
| event switch 54 cases | `0x8003AB08`, table `0x800113C4` |
| ladder speeds `+0x118` (1.0) and `+0x11C` (1.5) | `0x8003B660`, `0x8003B7C8`, `0x8003B824` |
| player step 32 = X's ladder shot | `player_steps[32]` = `0x801EB730`; `0x801EB6E8` writes step 32 |

## Checks against the save states

- `intro-1_start`, `yammark-1_start` (X) and `turtloid-6_zero-player` (Zero): the hurt tables at
  `0x80073C70`…`0x80073CE3` are the same bytes [certain].
- `mega+0xE7`: 1, 1, 5. `+0xD1` = 20 and `+0xD2` = `0x40` in the intro and Zero states (the last
  event was a hold facing right; `+0xD0` = 0). `+0xE6` = 10 in the Zero state (a squeeze damage
  from before the save). `+0xD5` = 0 in all three.
- All three are in state 1, step 2; nothing else here could be checked against a save state (no
  save state in a ladder, a hit, a beam or an event).

## Simulations (for the numbers in the sheet)

- Hurt class 1/4: the step runs on the hit update (`PlayerHitCheck` is before the step). `+0x61`
  75 → 60 in 15 more updates: 16 updates. Moves: 2.5, 2.3125, …, 0.0625 (14 updates), then the sign
  test stops it: Σ = 35 − 0.1875 × 91 = 17.94 px.
- Class 2: 100 → 60: 41 updates. Rise 2.75 at −0.2578: apex after 10.7 updates, about 16 px; on
  flat ground about 21 updates in the air (0.75 × 21 ≈ 16 px), then 0.5 px per update for the rest.
- Class 5: 63 → 60: 4 updates × 14 px = 56 px.
- Death: step 0 (1 update), step 1 (8), step 2 (39: waves at 0, 19, 38), step 3 (21).
- Beam-in landing (X animation 2): ticks 4, 4, 3, 2 × 6, 4, 4, 4, 5, 8, 3 (flag 1), 3, 33, hold:
  the flag frame starts at 48, the hold at 87.
- Step 30: apex 2.90625 / 0.0322 ≈ 90 updates, height ≈ 131 px; landing slide Σ (2 − k/16), k =
  0…31 = 33 px.

## Corrections to shared documents

- `docs/engine.md` "Damage to the player" / "Solid objects": `0x80039324` is not the general hurt;
  it is the squeeze damage (both sides blocked with an object push). `+0xE5` = 75 is the squeeze
  timer, `+0xE6` the squeeze damage.
- `docs/engine.md` open question 4 ("which engine code sets step 0 (hit reaction)"): step 0 is the
  beam-in; the hit reaction is step 17 (`PlayerHurt`).
- `docs/engine.md` open question 10 (`actor2`): it is the ride's body while `mega+0xD5` < 0
  [likely].
- `docs/players/map.md`: steps 0, 1, 14–18, 21–24, 29, 30, and step 32 (see the sheet).
- `catalog/helpers.json`: `mega.D5` is the ride state; `mega.hurt_timer` (`+0xE5`) is the squeeze
  timer; `mega.next_damage` (`+0xE6`) is the squeeze damage.

## Open

- The ride object (overlays 06, 28), the users of hit classes 3 and 5, and of `PlayerGrabDamage`.
- Who asks for steps 21, 23, 30.
- The form numbers; the part names (`0x400`, `0x800`).
- `+0xCF`, `+0x8D`, the blink drawing, `0x800C4568`, `game+0x98`.
- The death orb (misc type 17), and what follows state 3.
