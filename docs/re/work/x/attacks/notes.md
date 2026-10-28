# Notes: X's attacks (player RE step 3)

## Method

- Kit: `../kit/` (X, `intro-1_start`), no rebuild needed: all buster code is in the kit (main
  program and overlay 0). Functions printed with `scripts/fn.sh <addr>` (lower case).
- Code outside the kit: `~/mmx-data/x6/re/player/disasm.py <state> <addr> <n>` (effect type 2
  `0x80049270`–`0x80049A10`, `ContactWeapon`, `ApplyWeaponHit`, `WeaponHitReaction`, the effect
  loop `0x80020378`, the class-table set-up `0x80013E90`). `scripts/findimm.py <lo> <hi> <text…>`
  greps the disassembly of a RAM range (used to find the effect pool loop and the input writers).
- States: `scripts/mega.py` (the player's shot fields), `scripts/objs.py` (live weapon and effect
  objects), `scripts/efftbl.py` (the stage's effect table and raw object bytes), `scripts/tables.py`
  (the per-weapon tables), `scripts/anims.py` (shot and flash animations, table `0x8007789C`).
  `scripts/probe.py` holds the loaders.
- Sprites: `scripts/shotsheet.py <out>` (sheets of the live objects' banks) and
  `scripts/shotstrips.py <out>` (animation strips). Neither changes a repo tool; they call
  `mmxdemake.sprites.draw` with a `Bank` made from the object's own `+0x3C`, `+0x40`, `+0x42`.

## Key facts and where

| fact | where |
|---|---|
| button 0x20 is the buster: press fires `+0x95` (0 = lemon), release fires the charged id | `0x8003F954` |
| button 0x10 is the weapon button: `+0x94` (50 with the buster selected) | `0x8003E9E8`, `0x8003DF58` |
| button 0x40 starts the Giga attack when `+0xA8` = `+0x14A` | `0x8003E344` |
| `+0x7E` = buttons held in the update before; released = old & ~held | `0x8002151C` (demo input), the tests `0x100010`, `0x200020` |
| shot wait `+0x92`, shot pose `+0x91` from `0x80073EFC` | `0x8003E7F4` |
| at most `0x80074074[w]` shots counted in `+0x98` (Rapid 5: `0x800740B4`) | `0x8003E734` |
| charge: `+0x9E` +1 per update held, level 1 at 25, level 2 at 90 (Quick Charge 12, 45) | `0x8003FFF4` |
| charge sound bank 1 id 7 at level 1, stopped by the reset | `0x8003FFF4`, `0x8003F480` |
| any new press, any release, the super jump, a hit, a ride drop the charge | `0x8003ED98`, `ChargeStop` callers |
| charged id by form: 1 Falcon 19, 3 Blade 43, 4 Ultimate 20, else 18; half 9 | `0x8003F7BC` |
| Ultimate Buster (part `0x80000`): no charge; the buster button always gives the full id; `+0x92` × 4 | `0x8003EC78`, `0x8003F7BC`, `0x8003E82C` |
| 9, 18, 19, 20 are spawned as effect type 2 (muzzle flash) with var = id; the flash makes the weapon object at its flagged frame | `0x8003FA8C`, `0x80049854` |
| the lemon is a weapon object at once, plus a flash (effect 2 var 0) that makes no shot | `0x8003FA60`, `0x801ECF3C` |
| lemon 6.75 px/update (Speed Shot 8.75), half 8.0 (10.0), 18 10.0 (12.0), 19 10.0, 20 11.0 (13.0) | `0x801ECEE0`, `0x801ECF9C`, `0x801EE80C`, `0x801ED244` |
| lemon deflect: x speed reversed, y speed ±4.8 at random, no more hits | `0x801EE538` |
| 9 and 18 go on after a kill; 0, 19, 20 end at the first hit | `0x800737D0` byte 3, `0x80031AF0` |
| effect class table = scratchpad `+0x88` = `0x8006DDDC`[stage × 8 + area × 4] | `0x80013EDC`, loop `0x80020378` |
| effect type 2 = `0x80049478` in stage 7 (table `0x800FB644`) | `scripts/efftbl.py` |

## Check against the three states

All three: stage 7 area 0, X form 1, step 2 (stand), `+0xEC` = 0 (no parts), `game+0xC9` =
`0x18` (bits 0 and 1 clear), `+0xC9` = `0x21`, `+0x93` = 0, `+0x94` = 50, X at (160, 427) facing
right, sprite 85 or 86, buster offset (−21, −9) for sprites 85–87 [certain].

| state | `+0x8E` | `+0x91` | `+0x92` | `+0x96` | `+0x97` | `+0x98` | anim | objects |
|---|---|---|---|---|---|---|---|---|
| `sheldon-1_lemons` | 1 | 14 | 0 | −1 | 0 | 2 | 88 | weapon 0: type 0, state 1, x 221.5, y 418, 6.75, anim 21; weapon 1: type 0, state 3 (ended), x 282.25, `collide_flags` 5; effect 0: type 3 var 3 at x 282 (the wall spark) |
| `sheldon-2_half-charge` | 1 | 9 | 0 | −1 | 9 | 1 | 90 | weapon 0: type 9, state 3 (ended), display 0, x 286, 8.0, anim 26, `collide_flags` 5; effect 0: type 3 var 3 at x 286 |
| `sheldon-3_full-charge` | 1 | 30 | 10 | 19 | 19 | 0 | 91 | effect 0: type 2 var 19, state 1, step 0, x 195, y 418, anim 25, current frame (1 tick, flag 0, sprite 36); no weapon object |

- **Lemon** [certain]: fired 6 updates before the state (`+0x91` 20 → 14). Spawn point 160 + 21 =
  181; 181 + 6 × 6.75 = 221.5, the x of weapon 0. Weapon 1: 282.25 − 181 = 101.25 = 15 × 6.75, so
  it was fired 15 updates before and has hit the wall at x ≈ 282 (`collide_flags` 5, state 3). The
  count `+0x98` = 2 because weapon 1 is still in state 3 (its delete, which counts down, runs
  next). Animation 88 at frame 3 (2 + 4 ticks done) matches 6 updates.
- **Half charge** [certain]: fired 16 updates before (`+0x91` 25 → 9). Spawn point 160 + 21 + 17 =
  198; 286 − 198 = 88 = 11 × 8.0, so the shot moved 11 times: made by the flash 4–5 updates after
  the release (flash animation 22: 2 + 1 + 1 ticks to the flagged frame). It has just ended at the
  same wall. The flash is already gone (its hold frame came after 7 updates). `+0x98` = 1 is the
  shot itself (the flash added it; the shot's delete runs next).
- **Full charge** [certain]: fired 5 updates before (`+0x91` 35 → 30, `+0x92` 15 → 10). `+0x96`
  stays 19 because `TryShoot` returns at once while `+0x92` ≠ 0, so the id of the last shot is
  still there. The flash (effect type 2 var 19) is at x 160 + 21 + 14 = 195 and is at frame 3 of
  animation 25 (2 + 1 + 2 ticks done = 5 updates). Its next frame (frame 4) has the event flag:
  in the next update the flash makes weapon type 19 and adds 1 to `+0x98`. **So no weapon object
  is live because the Falcon shot is not spawned yet: the full charge leaves the buster 6 updates
  after the release, through the flash effect.** It is not in a different pool: when it appears,
  it is a weapon object (type 19) like the others [certain for the code; the state is 1 update
  before the spawn].
- Form 1 gives id 19 for the full charge (`0x8003F7BC`), and `max[19]` = 2: the Falcon full charge
  cannot fire while 2 shots are counted.

## Contradictions with earlier notes

- `docs/players/movement.md` says animation 88 is the special-weapon pose. It is also the stand
  shot for the lemon (`0x801F3698`[0] = 88; the lemon state is in 88). `WeaponPose` uses 88 for
  weapons 5 and 14 too.
- `catalog/helpers.json` `mega.shot_wait`: "no ladder grab from the ground while not 0" is a side
  use. Its main use: `TryShoot` does nothing while it is not 0 (the fire rate).
- Part 2b `player.h` named `+0x92` `ladder_timer` and `+0x96` `weapon`; `+0x96` is the id to fire
  in this update (−1 none), and also the id for the pose code.
- `docs/engine.md` names `mega+0xC9` "Zero's skill bits". For X it holds the owned special
  weapons (bit per slot, `0x80073ED8`); X has `0x21` in these states.
- The type table `0x80071220` is indexed by weapon id [certain now]: the spawn copies the id into
  the type byte (`0x8003FA60`, `0x80049898`).

## Open

- `+0xA6` (values 1–3 block weapons 2/11, 6/15, 7/16; 1 also drops the charge) and `+0x144` (3:
  no energy cost, 6: Shadow form uses the buster id table) [open: what sets them].
- Type 20's parts (type 21, `0x801ED36C`, `0x801EEA8C`); types 43, 45, 46, 50, 51 (50, 51 run with
  the X overlay steps 37, 38, 76, 77 and look like a saber: guess).
- The Falcon shot's hit tag goes up every 4 updates, but any hit ends it: why [open]. Effect
  type 27 var 8 (made with it) not read.
- `+0x9A` (shot count mod 3): no reader found in the X kit.
- Which pad buttons are bits `0x10`, `0x20`, `0x40`, `0x200`, `0x400` (the key config).
- The Falcon full charge sprite is not exported: it uses its own frame table (`+0x38`, streamed
  like the player, tpage `0x540`), and no state has it live.
