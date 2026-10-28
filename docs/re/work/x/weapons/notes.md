# Notes: X's special weapons (player RE step 4)

Date: 2026-10-22. The sheet is [`~/IdeaProjects/mmx-demake-tools/docs/players/weapons.md`](../../../findings/players/weapons.md).

## Method

- Kit: `python3 -m mmxdemake.rekit --player --state sheldon-1_lemons.state.gz --out
  ~/mmx-data/x6/re/x/weapons/kit` (own output folder, so the shared `../kit/` of the other agents
  stays as it is). 393 functions, 178 in overlay 0.
- Scripts (in `scripts/`, run from that folder):
  - `wp.py <state> <addr> [n]`: loaders and a disassembler for any RAM address (main-program
    weapon code, which the kit does not walk).
  - `dz.py <state> <addr>…`: compact disassembly with the `catalog/helpers.json` names.
  - `fn.sh`, `fc.sh <addr>…`: print kit functions (full / compact).
  - `types.py`: the weapon type table `0x80071220` (83 entries).
  - `box.py <addr>…`: 4-byte boxes (ox, oy, w, h).
  - `scan.py <state> <lo> <hi> <regex>`: grep the disassembly of a RAM range.
- Tables from `../attacks/scripts/tables.py` (spawn, max, single, counts, times).
- No state has a special-weapon shot live except `shark-9_boss-revive`: one Meteor Rain drop (type 6
  var 2). So most numbers are from the code.

## Key facts and where

| fact | where |
|---|---|
| types 24–31, 33–42, 44, 48, 58 point into the middle of X functions in X's RAM (`0x801ED2A8` is inside `0x801ED1B8`); in Zero's RAM they are function starts (`addiu sp`). So they are Zero's weapon objects | `types.py`, `wp.py turtloid-6_zero-player 801ed2a8 6` |
| X's spawn table gives `ShotNone` (`jr ra`, `0x8003FA58`) for 11, 17, 21–42, 44, 46–63 | `0x80073F74` |
| `WeaponCost(m, id)`: X `6 × cost_x[id]` (`0x80073D74`), Energy Saver (part `0x20`) `3 ×`, forms 1, 3, 4 `4 ×` (2/3, at least 1). Zero `cost_z[id]` (`0x80073DF4`) × 6 / 3 (part `0x20`) / 4 (Black Zero) | `0x8003EB0C` |
| `cost_x` (bar units): 1: 2, 2: 8, 3: 2, 4: 4, 5: 4, 6: 2, 7: 2, 8: 3, 10: 6, 11: 8, 12: 4, 13: 8, 14: 8, 15: 8, 16: 6, 17: 8, all others 0 | `0x80073D74` |
| energy max `+0x14A` = 6 × `game+0x61` (48 → 288 … 54 → 324) | states |
| `HasEnergy`: energy ≠ 0 and energy ≥ cost | `0x8003FA00` |
| `SpendEnergy`: energy[slot] −= cost; nothing when `+0x144` = 3 (except ids 44, 47, 49) | `0x8003F740` |
| Meteor Rain cost 8 in form 1: `shark-8` 324 − 268 = 56 = 7 × 8; `shark-9` 88 = 11 × 8 | states |
| Giga gauge `+0xA8` += max / 8 at each hurt (8 hurts fill it); part `0x800`: the lowest gauge of slots 0–8 + 12 | `PlayerHitCheck` `0x80039524` |
| key table `0x8006D600`: 16 pad masks, one per internal bit; same in the executable and all states | `0x8003D7AC` |
| X only: internal bits `0x10` and `0x20` are exchanged after the table | `0x8003D82C` |
| `+0xA6`: 1 = charged Ice Burst live (step 39 sets it, type 11 end clears it); 2 = charged Guard Shell (step 42, type 16 end); 3 = charged Meteor Rain (type 15 var 0, its end). Blocks 2/11, 7/16, 6/15; 1 also drops the charge and sets glow palette 41 | `0x8003FB48`, `0x8003FCD4`, `0x801F15B0`, `0x801EFCF4`, `0x801F21AC`, `0x80046550`, `0x8003E614`, `0x8003F480` |
| Zero's Yammar Option (type 1 with `mega.var` ≠ 0) ends when `+0xA6` ≠ 1 | `0x800434E0` |
| `+0x144` = item kind (`+0x2D` of the pause-menu item object) for 300 updates (480 for kind 6), timer `+0x148`; `PlayerUpdate` clears it at 0 | `0x80034288` (writer), `0x80034F64` |
| `+0x144` 1: `ContactMega` skips attackers in the visual (enemy shot) pool; 2: +1 weapon damage (`ApplyWeaponHit`); 3: free weapons; 6: form 2 uses the plain buster ids | `0x80030F30`, `0x800316F4`, `0x8003F740`, `0x8003DF74`, `0x8003F7BC` |
| effect type 27 (`0x8004AF94`): one-update loader: streams one frame block (`layout_base+0x10`, entry by var) to VRAM x 320, y 32 (tpage `0x520`) or y 64 (`0x540` for vars 7, 8, 9), then deletes itself | stage 7 effect table `0x800FB644` |
| type 20 (Ultimate full charge) does not stop at a hit: it makes a type-21 part (≤ 3 live in form 4, else ≤ 1) and goes on; only a deflect, a wall or the screen edge end it | `0x801ED36C` |
| shot side of a hit: only the types that read `+0x98` stop. Readers: 0 (lemon), 2, 52 (Ice Burst), 9/18, 19, 20 (part spawn), 45, 43 (deflect only), 50/51 (count), 1/10 option (3 hits), 4 (count), 53 (stop). The others ignore the hit table byte 2 | kit `shot_hit` readers, main-program types |

## Check against the states

- All states: weapon slot table `0x80073ED8` = bit per slot (`1 << (slot − 1)`). `+0xC9` = `0x21`
  (slots 1, 6) in `sheldon-*`, `shark-*`; `0x01` in `turtloid-1`; 0 in `intro`, `yammark-1`.
- `shark-8_boss-stun`: slot 6, `+0x94` = 6, `+0x97` = 6, energy[6] 268 of 324; no weapon object.
- `shark-9_boss-revive`: slot 6, energy[6] 236, `+0x98` = `+0x99` = 1. **One live type 6 var 2**
  (weapon slot 2, `0x800951E0`): state 1, step 3, speed_y −4.0, gravity 0, animation 3, boxes
  `0x801F3A4C` (28 × 28) / `0x801F3A50`, hit tag 1, `+0x8D` = 1 (it moves), parent (var 1) already
  deleted, `+0x98` = −1 (a deflected hit) and the object still falls. This agrees with the code:
  var 2 falls at 4.0 px/update and ignores its hit result [certain].
- The key table is the same in every state (`0x2000, 0x8000, 0x1000, 0x4000, 0x80, 0x10, 0x02,
  0x40, 0x20, 0x08, 0x04`): no state has a changed key configuration.

## Contradictions with earlier notes

- `catalog/helpers.json` `WeaponCost`: "table 0x80073DF4" is Zero's table. X's table is
  `0x80073D74`.
- `docs/players/attacks.md` and `engine.md`: "20 ends at the first hit". The code makes a part
  (type 21) and goes on; see above.
- `docs/engine.md` weapon ids: "33–44 … which weapon makes them is open". They are Zero's objects
  (Zero overlay); X cannot make them.
- `docs/players/steps-extra.md` step 75: "60 updates after the weapon object signals". The timer
  `+0xE1` counts from the start of the step in both sub-steps, so the freeze ends 60 updates after
  the start (or at the signal, when that comes later).
- `attacks.md` "Buttons": `0x200`/`0x400` "which pad button is which is open": with the default
  table `0x200` = R1 (next), `0x400` = L1 (previous).

## Open

- The pause-menu item kinds 1, 2, 3, 6 (`+0x144`): which items they are in the game.
- Who sets the Guard Shell part's `+0x9C` bit 1 (the block): the enemy-shot code, likely.
- Type 43's `act4` = 1 path (it makes a type 21 var 1 and ends): when `act4` is set.
- The look of every special weapon (no state has one on screen, except the Meteor Rain drop).
- Zero's ids 33–44 and 48: names and behaviour (Zero agent).
