# Ride Armor: proposed changes to files this agent does not own

## `catalog/helpers.json`

Add the entries in `helpers-proposed.json` (same format). Change:

- `0x800CD340` `actor2`: note → "second body slot, 0xB0 bytes; type → `actor2_types`
  (`0x80071380`); stage 4: the Ride Armor".
- `0x80097175` `mega.ride`: note → values 1 (the ride is actor2) or 2 (another slot) as a request;
  −1/−2 riding. While riding: contact hits, item pickups and soul pickups use actor2; X has no
  terrain, dash or hurt box; no shot, charge or weapon select.
- `0x80097178` `mega.ride_anime`: list the values (see the proposed note).
- `0x8002CD6C`: name `Display` [likely].

## `docs/engine.md`

- Open question 10: answered. `actor2` is a fixed second body slot (`0xB0` bytes at `0x800CD340`)
  with its own type table `actor2_types` (`0x80071380`, 5 entries) and update `Actor2Update`
  (`0x80020A08`), called each frame after the input and before the player. In stage 4 (overlay
  06, type 0) and in overlay 28 (type 1) it is the Ride Armor; see `docs/players/ride-armor.md`.
- Object table: add a row "actor2 (second body) | 1 | `0xB0` | `0x800CD340` | `actor2_types`
  `0x80071380`, 5 entries | `RideSpawner` (stage logic 30 of stage 4)".
- "Damage to the player": while `mega+0xD5` < 0, `ContactMega` tests actor2's hurt box: the hit
  class is 1 (damage < 5) or 2, no `PlayerDamageScale`, `hp −= damage`, `game+0x98 += damage`;
  HP ≤ 0 → `0x80` and `game+0x1C` = 1. It does nothing while actor2 HP bit 7 or actor2 `+0x85`
  is set.
- Items (`0x8004E26C`, `0x8004EECC`) and souls (`0x80040910`): the overlap and alive test use
  actor2 while X rides; the refill goes to X.

## `docs/players/ladder-hurt-events.md`

- "Step 18: ride": replace "What the ride is (a ride armour [guess]) is open" with a link to
  `docs/players/ride-armor.md`: the Ride Armor of Metal Shark Player's stage, actor2 type 0. The
  ride writes `+0xD5` = 1 and `+0xD8` (animations 41–57) from `RideIdle` / its steps; it sets
  `+0xD5` = 0 at the leave (step 11) and at its destruction.

## `docs/players/map.md`

- Step 18 row: "ride [certain]; the ride object is actor2 (`docs/players/ride-armor.md`)".

## `docs/stages/shark.md` (stage-rip agent)

- Record at `0x800F2B1C`: stage logic 30, id 0, (1872, 395) = `RideSpawner`: the Ride Armor
  (actor2 type 0), single use per area load (record flags bit 7).
- `game.stage_flag` (`0x800CCEF6`) in stage 4 is written by the ride every update: 1 while the
  ride touches a ceiling and a floor at once. Main type 28 (teleport crystal, `0x800EC7F8`) reads
  it and opens only when it is 0 and X's x ≥ the crystal x. What this means in play is open.

## Crusher sheet (crusher agent)

While X rides, X has no terrain box, so `PlayerHitCheck`'s crush and squeeze tests do not see
walls [likely]. The ride has its own rules:

1. **Left screen-edge crush** (`RideMain` `0x800F1F10`): X inside, the ride blocked on its right
   (`+0xA8` bit `0x01`, terrain or object push) and `ride.x − cam.x` < 32 → `mega.hp` = ride HP =
   `0x80` (X dies at once), 8 explosions, the slot is cleared (the record stays used).
2. **Pit:** ride `y − 32` ≥ `cam.max_y + 240` → the ride is removed; X inside → `mega.hp` = `0x80`.
3. No squeeze damage on the ride. Ceiling + floor at once only sets `game.stage_flag`.
4. Solid objects push and carry the ride (`SolidObject` `0x80031EB8`: `SolidPush(solid, actor2)`).

## `re/x/attacks/reconstructed/include/player.h`

`+0xD5` is named `dash_box`; it is the ride state (`mega.ride`). Proposed: rename it `ride` and
split `padd6[7]` to give `+0xD8` `ride_anime`. `ride.c` uses offset macros so it does not depend
on this.

## `spriteexport` (Death Guardian agent owns it)

The tool does not export actor2: it reads only the main, stage, visual and misc pools, and it
skips banks of type ≥ `0x80` (`FIRST_MISC`), so bank 3 (type 225, the Ride Armor, 77 sprites)
shows only in `banks/stage-4/shark-1_ride/bank-3.png`. A change: add a live source for the slot
`0x800CD340` (1 object, `0xB0` bytes; `+0x30` table, `+0x3C` sprites, `+0x40` texture, `+0x42`
palette, same layout as a main object) to `collect`, with `class: "actor2"`. The animation table
reads with the normal `read_table` / `fitting` (the probe `tools/ride_sprites.py` does this:
31 animations, 77 sprites, cell 165 × 125, origin (111, 80)). `playerexport` already exports X's
ride animations 41–64 (head only).
