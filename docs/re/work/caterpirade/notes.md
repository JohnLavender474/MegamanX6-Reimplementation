# Notes: method, evidence, open questions

## Method

1. Kit output already present: `kit/` (stage class, type 1, entry `0x800EC300`, 17 functions,
   4 state tables, animation table `0x800F73D4`).
2. Ran the kit on main type 7: `python3 -m mmxdemake.rekit --name caterpirade-main-7 --state
   yammark-3_caterpirade.state.gz --type 7` → [`~/mmx-data/x6/re/caterpirade-main-7/kit/`](../caterpirade-main-7/kit/) (1 function,
   `0x800EC25C`).
3. Read with `--at`: `0x8002C4C4` (stage-object allocator), `0x80031DA8` (solid object), `0x80032208`
   (push), `0x800324C8` (carry), `0x8002E1A0` (touch flags), `0x8002DCF4`, `0x8002DE54`, `0x8002DFA0`.
4. Small scripts in the session scratchpad: dump of the live stage-object slots, the player object,
   the record list; a scan of main-program RAM for `lb/lbu/sb` with offsets `0xE5`, `0xE6`, `0x72`,
   `0x138` and for `jal` to the helpers.
5. Short reads of player code: `0x80035010`–`0x80035104` (per-frame clears, `mega+0xE5` countdown),
   `0x80039300`–`0x800393F0` (player damage), `0x8002F300`–`0x8002F34C` (CollideCheck floor skip).
6. Looked at the export (`~/mmx-data/x6/sprites/enemies/caterpirade/anim-0..5.png`) to name the
   animations.

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt` | the 17 functions, annotated |
| `kit/data.txt` | the six boxes `0x800F73EC`–`0x800F7403`, the four dispatch tables, the damage tables |
| `../caterpirade-main-7/kit/code.txt` | the spawner `0x800EC25C` |

## Key facts and where they come from

- Main type 7 → stage type 1: `0x800EC25C` calls `0x8002C4C4` (scans `0x800C1220`, 80 × `0xA4`,
  clears `0xA4` bytes), `sb 1,1(v0)`, copies `+0x00`, `+0x02`, `+0x0A`, `+0x0E`, `+0x10`, `+0x40`,
  `+0x42`, `+0x3C`; then `DeleteObject3(self)`. No slot → `DeleteObject2(self)`.
- HP 12, contact 4, priority 7, gravity `0x2000`: `0x800EC368`.
- Walk speed `0x8000`, ball speed `0x10000`, ball deceleration `-0x200`, timers 40 / 40 / 128 / 90:
  `0x800EC5F4`, `0x800EC564`, `0x800EC738`, `0x800EC144`, `0x800EC520`.
- Hurt → CURL: HP compare in `0x800EBF6C` (`0x800EBFC4`).
- Platform: `0x800EC074`–`0x800EC0E8` and the helpers listed above.
- `mega+0xE5` = 75 at `0x8003932C`, countdown at `0x80035020`. `mega+0xE6` subtracted from HP at
  `0x8003936C`. Loop clearing `+0x72`/`+0x76` of 80 stage objects at `0x8003937C`.
- Player per-frame clears of `+0x71`, `+0x72`, `+0x4A` at `0x800350F8`.
- `CollideCheck` (`0x8002F328`): for `mega`, `+0x72 & 8` with `y − prev_y ≥ 0` branches past the
  floor-tile call `0x8002F840`.
- Callers of `0x80031DA8`: `0x8005026C`, `0x800510E4`, `0x800516B8`, `0x800518E4` (main program),
  `0x800EC0B0` (this overlay). Callers of `0x8002C4C4`: `0x800513C0`, `0x800EC268`.

## Timing (worked out)

- Anim 2 to its flag: 10 × 2 + 3 + 1 = 24 updates. Anim 4: 10 × 2 + 1 = 21. Anim 1: 4 + 1 = 5.
  Anim 5 total 63 < 90, so it holds sprite 23 when BALL sub-step 2 starts, and sub-step 2 ends at once.
- Roll distance: 128 × 1 px + Σ_{k=0..89}(1 − k/128) = 128 + 58.7 ≈ 187 px.
- Cycle: 1 + 24 + 40 + 128 + 90 + 1 + 21 = 305 updates; immune 284.

## Verified against the save state

- Live slot 0 (`0x800C1220`): flags `0x41`, type 1, var 0, state 1, step `+0x06` = 0 (WALK),
  `flip` `0x40`, `speed_x` `0x8000`, `hp` 12, contact 4, priority 7, boxes `0x800F73F4`/`0x800F73FC`/
  `0x800F73EC`, `dmg_tbl_normal`, `+0x72` = 8, `+0x76` = 1. Record pointer `0x800F6540`.
- Player: y 339, terrain box (0, 2, 11, 19) → feet at 360 = Caterpirade top (374 − 14).
  `mega+0x71` = 8, `+0x72` = 8, `+0xE6` = 4, `+0xE5` = 0, `+0x138` = 0.
- Record 59 in RAM: `01 07 00 10 20 02 7C 01`; in the stage file flags 0 and kind 0. So the
  spawned mark here is flags bit 0 and kind `|= 0x10`, not flags bit 7 (Yammark's record in
  `yammark-2_boss`: `01 31 00 10`; the boss gates (kind 4): `01 08 .. 04`).

## Kit gaps met

- `0x800EC420` `sb a2,20(a0)` (layer, in a `jal` delay slot) has no field note, and `+0x14` is
  missing from the fields table.
- Player fields through a register base (`s3` = `mega` at `0x800EC07C`) are not annotated
  (`229(s3)`, `114(s3)`).
- The main-type-7 run lists no neighbour; the stage object it makes (via the unnamed `0x8002C4C4`)
  is not found.
- `data.txt` at `0x800F7400` runs from the last box into the dispatch tables.

## Open questions

1. `mega+0x71` = 15 and `mega+0xE6` when the player's centre is inside the body.
2. The second actor `0x800CD340`, player step 18, and the global `0x800C4568`.
3. Spike tiles under the Caterpirade paths in this stage.
4. Who reads a stage object's `latch` (`+0x67`).
5. The ball at a wall (no own test).
