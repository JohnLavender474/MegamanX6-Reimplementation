# Rock block (stage 08, stage object type 6): behaviour sheet

A blue carved stone block that is solid on all sides and breaks after 8 HP of weapon damage.
It has three sizes. Each block is there only half of the time: a coin roll at its spawn deletes it.
It does not move and has no attack. "Rock block" is a descriptive name; no official name was
found.

Overlay `08` (stage 5, area 0). Entry `0x800EDE30`, state table `0x800F5AAC` (0 set-up
`0x800ED9DC`, 1 main `0x800EDC3C`, 2 delete `0x800EDE7C`). 27 records (kind 4, `b` 6). No save state
has a live block; in `scaravich-4_low-path` one record has flags `0x01` and no object (a coin loss
or a broken block) [certain for the RAM]. Kit: `python3 -m mmxdemake.rekit --state
scaravich-3_section.state.gz --type 6 --class stage`. Units, tags and shared rules:
`docs/engine.md`.

## Spawn and set-up (state 0, `0x800ED9DC`) [certain]

1. **Coin roll:** `GetRNG() & 1` = 0 → `DeleteObject3` (the record stays "spawned": no new roll
   until a reset).
2. `var` 0 → `var` = `GetRNG() % 3` (no record has `var` 0).
3. Flags `0x41`, state 1, step 2, `step2` 0, layer = `mega.layer`, sprite type **133** (bank 5, the
   bank of the falling rocks), animation table `0x800F5A20`, priority 35, **HP 8**,
   `dmg_tbl_normal`, `iframes` 0, no attack box, all speeds 0.
4. `+0x75` = 1 (wall slide on its sides) and `+0xA2` = 1 (a `solid_rects` rectangle), as the
   sabre block (`docs/enemies/sabre-block.md`).
5. **Contact damage 127.** The block has no attack box, so `ContactMega` does nothing. The value is
   the squeeze damage: `SolidObject` writes it to `mega+0xE6`, and a squeeze between the block and
   another wall takes that much HP (`docs/engine.md`, "Solid objects") [certain for the code; a
   squeeze needs a moving body, and the block does not move].

| `var` | records | size | animation, sprite | terrain box (half sizes) | hurt box |
|---|---|---|---|---|---|
| 3 | 23 | 64 × 80 | 10, sprite 17 (carved block) | `0x800F5A90` (0, 0, 32, 40) | `0x800F5A70` (−32, −40, 64, 80) |
| 6 | 1 | 64 × 80 | 10, sprite 17; priority 34 | as `var` 3 | as `var` 3 |
| 4 | 2 | 32 × 80 | 11, sprite 18 (column) | `0x800F5A94` (0, 0, 16, 40) | `0x800F5A74` (−16, −40, 32, 80) |
| 5 (and 0–2) | 1 | 64 × 32 | 12, sprite 19 (slab) | `0x800F5A98` (0, 0, 32, 16) | `0x800F5A78` (−32, −16, 64, 32) |

## The `id` parameter

`var` picks the size (table above); `var` 6 differs from 3 only in the draw priority (34 instead of
35, so it is drawn in front of or behind another object) [certain for the values].

## Main (state 1, `0x800EDC3C`) [certain]

1. Keep the old HP; `ContactWeapon`. A kill → "Break".
2. `ContactMega` (no effect); hit flash when the HP changed.
3. The step: `step2` 0 is an empty function (`0x800EDE9C`). The block does nothing by itself.
4. `AnimeTick`, `SolidObject` (solid for the player on all sides; carries a player on top).
5. When `display` is set: `0x80031F4C` (pushes and carries enemies).
6. `OffScreenCheck2(48, 48)`; inside → `DisplayCheck(48, 48)`. Outside: nothing. **There is no
   off-screen delete:** a spawned block stays live while the area is loaded [certain for the code].

There is no `CollideCheck`: the block does not snap to the floor (unlike the sabre block).

## Break [certain]

- 6 debris pieces (`SpawnJunk`, animations 13–18 from `0x800F5AA4`).
- 3 × `SpawnExplosionRand(obj, 1, mx, my)` with (mx, my) = (32, 48) for `var` 3 and 6, (16, 32) for
  `var` 4, (32, 16) for the slab.
- State 2: `DeleteObject3`. The record keeps its "spawned" bit: a broken block does not come back
  until the records are reset.
- No item roll, no sound of its own.

## Damage

`dmg_tbl_normal`, HP 8: X 3 lemons, 2 half charges, 1 full charge; Zero 2 sabre hits. It deals no
damage (only the squeeze value above).

## Animations (table `0x800F5A20`)

| # | sprite | use |
|---|---|---|
| 10 | 17 | 64 × 80 carved block |
| 11 | 18 | 32 × 80 column with four slots |
| 12 | 19 | 64 × 32 slab with two slots |
| 13–18 | 20–25 (one frame each) | blue debris (the break) |

Animations 0–9 are the falling-rock animations of the same bank (sprites 0–16). Sprites:
`~/mmx-data/x6/sprites/banks/scaravich/scaravich-3_section/bank-5.png`.

## Summary for a reimplementation

1. A static solid block (64 × 80, 32 × 80 or 64 × 32 px) that the player can stand on and
   wall-slide on. It has no call to `0x80032BE8` or `SolidPushShots`, so it does not stop enemy
   shots or push the player's shots [certain for the code; the effect in play is not checked].
2. 50 % chance to exist at each spawn; once gone (coin or break) it stays gone until a reset.
3. 8 HP, any weapon. Break: 6 debris, 3 explosions, no item.

## Open questions

1. The places of the 27 records against the map (which paths they close).
2. Whether the coin roll can close a path that the player must use.
3. The official name.
