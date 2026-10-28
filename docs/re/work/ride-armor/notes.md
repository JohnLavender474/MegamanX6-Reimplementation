# Ride Armor (actor2 type 0, Metal Shark Player's stage): RE notes

Date: 2026-10-21. State: `shark-1_ride.state.gz` (SHA-1 `fcf098dcbeb64cf58e6131364bb12e806937d130`),
stage 4 area 0, overlay 06 at `0x800E9860` (6 of 42 944 bytes differ from the disc: variables).

## Kits and probes in this folder

| path | what |
|---|---|
| `x-kit/` | `rekit --player --state shark-1_ride.state.gz --out …` (X kit of this state) |
| `main-1-kit/` | `rekit --type 1`: main type 1 is **Junkroid** (live, `var` 1), not the ride |
| `kit-actor2-t0/` | `tools/kit_at.py`: the rekit walk from `0x800F2414` (actor2 type 0), 35 functions, 4 state tables |
| `kit-actor2-t1/` | the same from `0x800F01A4` in this state (a mid-function address of overlay 06: not a real entry here) |
| `kit-ov28-actor2-t1/` | `tools/ov28.py`: overlay 28's file laid over the state RAM, walk from `0x800F01A4` |
| `kit-punch/` | walk of weapon types 61/62 (`0x80046830`, main program) |
| `sprites/` | `tools/ride_sprites.py`: 31 animation strips of the ride (`anim-<k>.png`) and `animations.json` |
| `tools/` | `ramtool.py` (disasm / hex), `refs.py` (lui+lo reference search), `owner.py`, `tables.py` (type tables of the stage), `anims.py`, `kit_at.py`, `ov28.py`, `ride_sprites.py` |

## What `actor2` is

- One fixed slot of `0xB0` bytes at `0x800CD340` (`ClearEnemies` `0x8002B79C` clears 176 bytes
  there). It is not in a pool. Its type byte (`+1`) indexes `actor2_types` (`0x80071380`, main
  program data, 5 words): 0 → `0x800F2414`, 1 → `0x800F01A4` (both inside the stage overlay area),
  2 → `0x80020F24`, 3 → `0x80020F8C`, 4 → `0x8002103C` (main program; not read).
- Update: `Actor2Update` (`0x80020A08`), called from the game frame (`0x80020C40`) and the demo
  frame (`0x80021500`). Order in the frame: input (`mega+0x7C`, `+0x80`) → **actor2** →
  `0x80034DCC` (player) → `WeaponLoop` → … So the ride moves first and X follows in the same update.
  With `game.stop` byte `+0x10` (`0x800CCEE0`, the player's stop flag) set, the type function runs
  only when flags bit `0x08` is set; else the slot is only drawn (`0x8002CD6C`) when `display` ≠ 0.
- Drawn by the draw loop at `0x80022D60…`: misc objects, then actor2 (`+3` ≠ 0 → `0x800232D4`).
- Other main-program users: `ContactMega` (`0x80030ECC`: while `mega+0xD5` < 0 the attacker is
  tested against actor2), `SolidObject` (`0x80031DA8`: `SolidPush(solid, actor2)` and carry, when
  actor2 flags ≠ 0), the soul pickup (`0x80040910`), the item pickup (`0x8004E26C`) and
  `0x8004EECC` (both: "body" = actor2 while `mega+0xD5` ≠ 0, for the overlap and the alive test;
  the refill goes to `mega`).
- The body choice in those three: `body = mega.ride ? (actor2.step ? &actor2 : &actor2 + 0xB0) :
  &mega`. `0x800CD3F0` is not an object (a byte list used at `0x80025F8C`); while X rides, the
  ride step is never 0, so the fallback is not reached [certain for the code].

So actor2 is a **general second body slot**, not a ride-only object: its type table has 5
entries, three of them in the main program. In stage 4 it holds the Ride Armor.

## Spawn

- Stage-logic type 30 of overlay 06 = `RideSpawner` (`0x800EE158`): actor2 flags `0x41`, type =
  record `id`, var = record `id`, record pointer, x, y; then `DeleteObjectKeep` (record flags |=
  `0x80`: never again while the area is loaded).
- The record (`0x800F2B1C`): `81 1E 00 03 50 07 8B 01` = flags `0x81` (spawned, used), type 30,
  id 0, kind 3, x 1872, y 395.
- Overlay 28 has the same code (the 35 functions have the same sizes; the spawner writes actor2 at
  `0x800EBF04`; the init asks for sprite type 225 too). There the entry is actor2 type 1
  (`0x800F01A4`). Overlay 28 = stage 20 area 1 by `overlay = 25 + 2 × (stage − 19) + area`
  [likely; the formula has two points].

## Init (`RideInit`, `0x800EE1D4`, state 0)

display 1, state 1, step 0, flags |= `0x40`, layer 0. Record `id` 0 → face right (`0x40`), else
left. **var = 0 always**. Animation table `0x800F3D4C` (31 animations), sprite bank of type 225
(bank 3 here, 77 sprites), priority 3, terrain box `0x800F3A44` (centre (0, 3), half 15 × 29),
HP 32, contact damage 0, no attack, hurt or damage box, all work cleared.

## Main (`RideMain`, `0x800F1EC4`, state 1)

See `pseudo.md`. The ride writes its x, y and flip into `mega` every update while bit `0x40` of
`+0x97` is set.

## Fields

| field | meaning |
|---|---|
| `+0x2` var | 0 here (the model); ≠ 0 selects table `0x800F3AAC`, not reached |
| `+0x5C` HP | 32; bit 7 = hit waiting; `0x80` = 0 HP |
| `+0x61` | invulnerable to `BoxOverlap` (1 after leaving) |
| `+0x7D` | air dash used (var ≠ 0 path) |
| `+0x7E` | shot cooldown (var ≠ 0 path), −1 per update |
| `+0x7F` | bits of the 3 shots of type 60 (var ≠ 0 path) |
| `+0x81` | punch queued |
| `+0x84` | facing towards the attacker (written by `ContactMega`; not read by the ride) |
| `+0x85` | contact lock: 1 after a hit until `+0x8C` = 0 |
| `+0x86` | live shot count (var ≠ 0 path); cleared by `ContactMega` |
| `+0x87` | punch combo index 0/1/2 |
| `+0x88`, `+0x8A` | copy of `mega+0x7C` (held) and `mega+0x80` (pressed); 0 during an event |
| `+0x8C` | hit blink timer 121; 255 at destruction |
| `+0x8D` | destruction: 0 alive, 1 eject now, 2 exploding |
| `+0x8E`, `+0x8F`, `+0x9A` | double-tap dash: armed, window 12, direction |
| `+0x90` | dash timer 30; explosion timer 30 |
| `+0x91`, `+0x92` | 0-HP countdown: interval, counter (both 30 at the start) |
| `+0x93` | knockback phase 0/1/2 |
| `+0x94`, `+0x95`, `+0xA6` | charge: phase 0 idle / 1 holding / 2 ready, timer 60, the buttons held |
| `+0x96` | bits `0xC000`, `0x8200` tested; step 12 writes the low byte 40 (no reader of the meaning) |
| `+0x97` | bits: `0x01` leave request (no writer in the ride), `0x02` knockback, `0x04` destroyed, `0x08` dash continues on the dash button, `0x10` no charged punch (no writer), `0x40` X inside, `0x80` 0 HP reached |
| `+0x98` | air and walk speed, 352 (1.375) or 1056 (4.125, after a dash) as `x / 256` |
| `+0x9C`…`+0xA2` | saved speeds during the knockback (`>> 8`) |
| `+0xA4` | knockback speed `0x400` (4.0) |
| `+0xA8` | `collide_flags` \| `+0x71` (object push) of this update |

## Weapon objects made by the ride

Types 61 and 62 (`RidePunch`, `0x80046830`, main program): no sprite, follow the ride (`+0x7C`
parent), attack box = `0x80076254`[ride frame flag >> 4]; contact damage byte 3; deleted when the
ride's animation (`+0x17`) or step is not the pair for its var (`0x80076270`: 8, 9, 10, 11, 15;
`0x80076278`: steps 3, 9, 3, 6, 13) or the ride state is not 1. With var ≠ 0 the hit tag
(`+0x64`) goes up every 6 updates: a new hit every 6 updates.

| var | type | made in | animation | X animation |
|---|---|---|---|---|
| 0 | 61 | step 3, first punch | 8 | 49 |
| 2 | 62 | step 3, second punch | 10 | 50 |
| 1 | 62 | step 9, dash punch | 9 | 55 |
| 3 | 62 | step 6, air punch | 11 | 51 |
| 4 | 62 | step 13, charged punch | 15 | 54 |

`dmg_tbl_normal`: id 61 = 5, id 62 = 15 (class 0). `dmg_tbl_deflect`: both deflected.

Attack boxes (`{ox, oy, w, h}`, mirrored facing right) by frame flag high nibble: 1 (−64, −7, 56,
26); 2 (−71, −51, 49, 64); 3 (−79, −65, 74, 96); 4 (−71, −44, 62, 63); 5 = 3; 6 (−106, −15,
114, 41).

Types 59 and 60 (X overlay `0x801EE05C`) are made only by the var ≠ 0 path.

## Open

- Bits `0x01` and `0x10` of `+0x97`: no writer in the ride code; another object may write them.
- `game.stage_flag` (`0x800CCEF6`) = 1 while the ride's `+0xA8` has `0x0C` (ceiling and floor);
  the teleport crystal (main 28, `0x800EC7F8`) opens only when it is 0.
- What the var ≠ 0 model is (shots of types 59/60, air dash step 15, hover step 14, steps 16, 17).
- The three main-program actor2 types 2–4.
