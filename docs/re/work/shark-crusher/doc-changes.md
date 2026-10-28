# Proposed doc changes (Nightmare Press agent)

I did not edit these files. The main session or the owner agent should apply them.

## `docs/engine.md`

### "Object classes" table: add two rows

| class | slots | size | base | update table | made by |
|---|---|---|---|---|---|
| kind-7 objects | 4 | `0x30` | `0x800C4778` | `0x8007136C` (5 entries), loop `0x800208EC` (after `WeaponLoop`; with `game+0x19` set, only flag bit `0x08`) | record kind 7, `GetSysObject` (`0x8002C950`) |
| ride (`actor2`) | 1 | — | `0x800CD340` | `0x80071380` by `actor2+1`, loop `0x80020A08` (first in the frame) | stage logic type 30 of stage 4 (`0x800EE158`) |

And: `weapon_types` (`0x80071220`) has at most 83 entries, because the kind-7 table starts at
`0x8007136C`.

### "Spawning and despawning", record kinds

The kind table `0x8007347C` (`0x8002A3AC`): 0–2 main, 3 stage logic, 4 stage objects, 5 misc
(`0x8002C530`), 6 ray (`GetRayObject`), 7 kind-7 objects (`0x8002C950`). Kind 8 and up point into the
overlay area (not used in stage 4) [certain for the table].

### New: the frame order (`0x80020C40`) [certain for the call order]

`0x8002E84C`, `0x8003CD44`, ride (`0x80020A08`), player (`0x80034DCC`), `WeaponLoop`, kind-7 loop
(`0x800208EC`), `0x8002000C`, `0x80020258`, `EffectLoop`, stage objects (`0x800205A0`), misc
(`0x800206C0`), `0x800207D4`, `0x80029D18`, `StageLogicLoop`, `0x80020D18`, `CollideCheck(mega)`,
layer cameras 0, 1, 2 (`0x8002927C`, `0x8002935C`, `0x800293CC`), `0x8002ACA8`, `0x80020D74`.
(Unnamed entries: their class is my reading of the base addresses only.)

### "Solid objects": corrections and additions

- `SolidPush` returns 1 at once when the pushed body has no terrain box (`+0x68` = 0). So while the
  player rides (no terrain box), `SolidObject` skips the player and acts only on `actor2`.
- `SolidTouch` sets `mega+0xE6` (`0x80097186`) = the contact damage for any contact, also a contact
  with `actor2`. The flags go to the touched body's `+0x71` (the player's or the ride's).
- After the `actor2` part, `SolidObject` copies the ride's x and y to the player while the player is
  in step 18.
- The "squeeze" bullet: add the Nightmare Press (`docs/enemies/nightmare-press.md`) as a user of
  `mega+0x71` bit `0x04` with `mega+0xE6` = 127; and add stage type 9 of overlay 06 to the readers of
  `mega+0xE5`.

### "Game state" table

- `+0x26`: add "stage 4: 1 while the Ride Armor (`actor2`) has the floor and the ceiling in its flags
  (`0x800F2414`); the Nightmare Press then stops and rises".
- `+0x27`: add "stage 4: 1 in each update of a Nightmare Press rise; stage type 9 then clears
  `mega+0x71` for a player on the floor under it".
- `+0x1D` checkpoint: the press uses it for its kill line and its waits.

### Camera layers

- Layer-1 update `0x8002935C`: mode table `0x80071E88` by `layer1+0x04`. Layer-2 update `0x800293CC`:
  table `0x80071EB0`. Mode 9 (`0x800297BC`): x = layer-0 x + `layer+0x40`; y is not changed.
- Layer camera `+0x1A`: the old y (the press writes it before it moves layer 1).

## `docs/players/ladder-hurt-events.md` ("The check each update")

The text is right in substance. Proposed exact wording of items 1 and 2:

1. **Crush:** terrain only: `mega+0x70` & `0xC` = `0xC`, or `mega+0x70` & `0x3` = `0x3` (the flags of
   the last `CollideCheck`, objects not included): the damage total += HP & `0x7F`, HP = `0x80`,
   whatever the invulnerability.
2. **Squeeze:** when HP is not `0x80`: `+0x89` & `0xC` = `0xC` or `+0x89` & `0x3` = `0x3`, with `+0x89`
   = `+0x70` | `+0x71` set by `PlayerUpdate` at `0x80035008` before this check. So one side from the
   terrain and the other from an object (for example the floor and the Nightmare Press), or both
   from objects. Then as now (`no_hit` = 0, `+0xE5` = 75, HP −= `+0xE6`, …). The squeeze does not
   test `+0x61`, `+0xE5` or the difficulty: with `+0xE6` = 127 it is an instant death. Death (item 4)
   follows in the same update.

Also: in the fields table, `+0xE5` "Read by stage objects only": readers known now are the rocket
platform, the Caterpirade, and stage type 9 of overlay 06. `+0xE6`: add "the Nightmare Press writes
127".

## For the Ride Armor agent (`docs/players/ride-armor.md`)

Please add (their sheet, not mine):

- `actor2` update: `0x80020A08` → table `0x80071380` → type 0 = `RideUpdate` `0x800F2414` (overlay
  06), states `0x800F3A50` (0 `0x800EE1D4`, 1 `0x800F1EC4`, 2 `0x800F250C`, 3 `0x800F2588`).
- `RideUpdate`: ride `+0xA8` = `+0x70` | `+0x71`, `+0x71` = 0, the state, HP &= `0x7F`, then
  `game+0x26` = 1 when `+0xA8` & `0xC` = `0xC`, else 0. State 2 also clears `game+0x26`.
- Ride terrain box `0x800F3A44` = (0, 3, 15, 29): 30 × 58 px, top y − 26, bottom y + 32 (checked: y 384,
  floor 416).
- Under the Nightmare Press, the ride stops the press (the press waits 50 updates and goes back up).
  X inside is never crushed: the press test and the type 9 bodies need a player terrain box, and the
  player has none while riding (`mega+0x68` = 0 in `shark-1_ride`).
- Seen in passing, not read in full: ride state 1 (`0x800F1EC4`) kills X and the ride
  (`mega+0x5C` = `0x80`, ride HP `0x80`, 4 × `SpawnExplosionRand`) when the player rides, ride `+0xA8`
  bit `0x1` is set and ride x − camera x < 32.

## For the stage agent (`docs/stages/shark.md`, catalogs)

- `fixed_set_tbl` stage 4 area 0 (`0x800F2E34`): records 0–8 stage logic 3; 9–58 stage type 9
  (the press bodies and the lower blocks, flags `0x30`); 59 kind 7 type 2 `id` 32 (the press,
  `0x800F300C`).
- The press art is BG layer 1 (`art 1 decoration`): x 1600–5056, y 128–256 at the layout position;
  the game draws it `offset − 32` px lower (offset 16–96).
- Stage logic type 14 records in stage 4 (`id` 129, 130, 131 at y 256): `0x80052F64` is an event zone
  (`PlayerEvent(20, …)`, `mega+0x14E`, sound 42), so the name `BackgroundSetting` may be wrong for this
  stage [likely].

## `catalog/enemies.json`

No `names` entry: `spriteexport` gives the press no ID, because it has no sprites. Sheet slug:
`nightmare-press`.

## `docs/TODO.md` (proposed items)

- [ ] A save state of X standing under the Nightmare Press while it falls (floor y 320 or 352), to see
      the death update.
- [ ] A save state of the Ride Armor holding the press (`game+0x26` = 1, press sub-step 5).
- [ ] Stage 4 area 1 and the stage ≥ 19 user of `PressUpdate`.
- [ ] Read `0x80032B34`, `0x80032BE8`, `0x8003208C`, `0x80031F4C`.

## `docs/process/process-log.md` (proposed line)

- Nightmare Press (stage 4): found from the layer-1 camera; kind-7 class (`0x800C4778`) found; crush
  rule = press push (`mega+0x71` bit 4, `+0xE6` 127) + floor → squeeze in `PlayerHitCheck`; Ride Armor
  sets `game+0x26` and stops the press. Sheet `docs/enemies/nightmare-press.md`.
