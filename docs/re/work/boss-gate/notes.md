# Notes: boss gate (stage object type 8, main program)

## Method

1. Kit output already in `kit/` (from `intro-3_boss.state.gz`, `--type 8 --class stage`): 23
   functions, 2 state tables, the animation table `0x80078DCC`.
2. Read `summary.md`, `calls.txt`, then all of `code.txt` (gate functions `0x8005011C`–`0x80051158`
   and the engine solid-object code `0x8002DCF4`–`0x800328B0`).
3. Decoded the 51-entry jump table `0x80011694` (in `data.txt`, and read again from
   `SLUS_013.95`) and followed each case by hand.
4. Read extra code from the executable with `mmxdemake.mips.disassemble` (scratch script):
   - `0x8003A1C8`: the player routine that answers `mega+0xD4` = 1 (sets −1, step 19, flip right);
   - `0x80029280`–`0x80029358`: the camera update; it skips the `cam+0x34` handlers while
     `mega+0xD4` = −2;
   - `0x80028EBC`: the camera limit easing (current `+0x1C..+0x22` toward target `+0x24..+0x2A`
     by `cam+0x47` px/frame);
   - `0x80029E7C`–`0x8002A30C`: the stage spawner; it does not test bit 6 of the id.
   - a scan of all `lb`/`sb` with immediates `0xCEE0`, `0xCEE2`, `0xCEEC`, `0xCEF6`, `0xCEF8` and
     with offset `0xD4` to find the other users of these flags.
5. Dumped `camera_layer0`, `game`, the player fields and the gate objects from 5 save states.
6. Looked at `~/mmx-data/x6/sprites/enemies/boss-gate/anim-0.png` to name the animation frames.

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt` | all gate functions and the engine solid-object routines |
| `kit/data.txt` | jump table `0x80011694`, terrain box `0x80078DC8`, animation table, player boxes `0x80073D50` |
| `pseudocode.md` | every function, the per-id cases, the globals |

## Key facts and where they come from

- Terrain box (0, 0, 16, 32): `gate_init` writes `0x80078DC8` to `+0x68`; `data.txt`.
- Solid wall: `gate_wait` and `gate_closed` call `0x80031DA8`, which pushes `mega` (and
  `0x800CD340`) out of the box (`0x80032208`) and sets touch bits (`0x8002E1A0`).
- `var &= 0x3F`: `gate_init`, `andi v1,v1,0x3f` at `0x80050228`.
- Trigger from the left only: `0x80050D08` `slt v0, gate.x, mega.x` → return 0.
- Player has no collision in the pass: `sw zero,104(a2)` at `0x80050580` (a2 = `mega`).
- Push: `mega.x += work84` at `0x80050EB4`; `work84` = `0xA400` at `0x800506F4` and per case.
- End of pass: `cam+0x0A == cam+0x26` at `0x80050ED0`.
- Clear enemies: `0x8002B79C` clears 48 main slots (`0x9C` bytes) unless type 47, then 32 visual
  slots (`0xA0` bytes).
- Animation timing: sums of the frame durations in `summary.md`: open 67 (flag 1) / 75 (flag 2),
  close 8 / 76.

## Verified against the save states

| state | camera max x / min x / max y / min y | gates |
|---|---|---|
| `intro-3_boss` | 6144 / 6064 / 224 / 224 (id 1: X+80, X; id 2 gave y 224) | slot 0 id 2 x 5744, slot 1 id 1 x 6064, both state 2, work84 `0xA400` |
| `yammark-2_boss` | 5136 / 5104 / 1904 / 1904 (id 4: X+32, X; id 3 gave Y−64 = 1904) | id 4 x 5104 and id 3 (record 67) x 4848, state 2; id 3 work84 `0xD400`; `game+0x43C` = `0x4B62`, `game+0x90` = `0x4C03` |
| `turtloid-1` | – | records: id 34 (8624, 688), id 23 (8944, 688), not spawned |

`cam+0x47` is 2 after the pass and 4 at the stage starts; the gates' `work80` = 2, so the value
before the pass was also 2 in these stages (a kind-3 camera record may set it) [guess].

## What the kit saved, and what stayed manual

- Saved: the functions, the two state tables, helper names, field names, the animation table.
- Manual: the jump table `0x80011694` (the kit dumps it as data, not as code targets, and does not
  walk the 40 cases as separate entries; it treats `0x80050670` as one function, which is correct);
  the step routines' arguments `a1` = `game`, `a2` = `mega` (the kit names `a2`-relative offsets
  raw, e.g. `sb v0,212(a2)` is `mega+0xD4`); globals named relative to `game.stage`
  (`game.stage+0x10` is `0x800CCEEC` = `game+0x1C`), which hides the `game` offsets.
- The kit names `0x80097186` `mega.layer+0xd2`; it is `mega+0xE6`.

## Open questions

See the sheet. In short: the stage flags `0x800CCEF6` / `0x800CCEF8`; the player fields of the
trigger test and player step 19; the reader of `game+0x43C`; `game+0x10` / `+0x12`; id bits 6–7;
whether the gate is ever deleted.
