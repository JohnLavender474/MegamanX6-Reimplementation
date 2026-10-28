# Notes: method, evidence, open questions

## Method

1. Kit output (given): `python3 -m mmxdemake.rekit --name teleport-crystal --state
   yammark-1_start.state.gz --type 28`. Entry `0x800EEEC4`, 18 functions, 4 state tables, no
   neighbours, one animation table.
2. Turtloid: `python3 -m mmxdemake.rekit --name teleport-crystal-turtloid --state
   turtloid-1.state.gz --type 28`. Entry `0x800EFFF4`, 18 functions, 4 state tables.
3. Compared the two `code.txt` files with the addresses and the overlay constants masked: 846
   instructions each; only the 11 `addiu` low halves of the overlay data addresses differ. The data
   (`data.txt`): the boxes, the animation frames and the destination table (`0x800F6474` in 09)
   are byte-for-byte the same. So the code and the data are the same [certain].
4. Read the unwalked engine calls with `--at` (`0x800139A0`, `0x800139F4`, `0x8002B36C`,
   `0x8002B6B0`, `0x80029F38`, `0x8003AD18`, `0x8003C4EC`, `0x8003CDF0`, `0x8003CE30`,
   `0x8003F3DC`, `0x8002D654`) and a few readers of the globals, with small scripts in the session
   scratchpad (`xref.py`, `xref2.py`, `dis.py`, `peek.py`).
5. Read the RAM of the three save states for `game` (`0x800CCED0`), the camera (`0x800971F8`) and
   `0x80097420`.
6. Facts only from `~/IdeaProjects/mmx-reference/mmx6`: the `X4_EngineObj` layout names `+0x1D`
   `checkpoint` and `+0x26` `character_state`; `config/dedup_registry.txt` lists the same crystal
   functions in overlays 03, 04, 05, 06, 08, 09, 10, 11 and 24 (rock_NN). No code was copied.
7. The Fandom list could not be read (HTTP 402). The object is not an enemy; the descriptive name
   `teleport-crystal` is kept.

## Evidence files

| file | contents |
|---|---|
| `kit/code.txt`, `kit/data.txt`, `kit/summary.md` | Yammark copy |
| `../teleport-crystal-turtloid/kit/` | Turtloid copy |

## Key facts and where they come from

- Trigger: `tc_touch` calls `BoxOverlap(self, mega)`. `BoxOverlap` (`0x8002D654`) returns 0 when
  `b+0x61` ≠ 0, and else tests `a+0x50` (attack box) against `b+0x54` (hurt box). No test of the
  player's state, the ground or the button [certain].
- Mode by `var`: switch at `0x800E98F4` in `tc_init`; `tc_touch` tests `var == 2`; `move_jump`
  switches on 2, 3, 4 [certain].
- Sequence A writes `0x800CCEDF = var | 0x40` (`area_split`, `0x800EF1C4`).
- Effect of bit `0x40` on the player: `0x8003B870` (called from the player code at `0x80037BEC`,
  `0x80037EA4`, `0x8003A63C` when `0x800CCEDF` ≠ 0) sets `mega.state = 3`, `step = 0` when bit
  `0x40` is set; bit `0x10` gives `mega.step = 25`; else a beam-out (sound 1/11, `speed_y` 8
  px/frame) [certain for the code].
- Other users of `0x800CCEDF`: `0x8002820C` (camera clamp: the player's y is held ≥ 0 only while it
  is 0); overlays 02, 23, 44 write `0x40`; overlay 24 writes 1; overlay 45 writes `0x10`; overlay 02
  `func_800F1F50` writes `0x10` and `0x80` (from the reference file names) [likely].
- No code that reads the low bits of `0x800CCEDF` was found. A search for `andi` masks after a load
  of `0x800CCEDF` or `game+0x0F` found only the `0x10` test of `0x8003B870` [certain for the
  search; it does not see indirect accesses].
- Camera limits: `0x8002820C` clamps `x` to `[+0x1E, +0x1C]` and `y` to `[+0x22, +0x20]`. Save
  state `yammark-1_start`: `+0x1C` = 1200, `+0x1E` = 0, `+0x20` = 720, `+0x22` = 208; the copy at
  `+0x24`–`+0x2A` has the same values [certain].
- Stop flags: `game+0x10` ≠ 0 skips the player's control part at `0x80035018`; `game+0x19` ≠ 0
  skips an object loop at `0x80020944` [likely for the meaning]. Other code writes the same 9 bytes
  together (`0x8004E798`, `0x8004FE64`, `0x800534EC`).
- `0x80097420` is a word that counts up each frame (values `0x029C0C`, `0x0A9F06`, `0x0AABAE` in
  the three states) [likely: frame counter]. `0x80097424` is the fade direction (`+1`/`−1`, 0 when
  done) [certain for the writes in FadeIn/FadeOut; likely that the fade task clears it].
- The fade task `0x80013530` starts at 255 for fade-in and 0 for fade-out; the step is the speed
  byte `0x800C8861` = 8. So about 32 frames per fade [guess].

## Timing (sequence B, `var` 2)

| part | frames |
|---|---|
| B0 start | 1 |
| B1 split | 29 (the images go to ±29 px) |
| B2 wait for fade-out, then jump | fade-out time (about 32 [guess]) |
| B3 join, during fade-in | fade-in time (about 32 [guess]) |
| B4 join | 29 |
| B5 delete | 1 |

`var` 3 waits 90 frames in B0 first. The join starts at ±64 px and closes 1 px per frame per image;
after about 61 frames the images are at about ±3 px, then B4 snaps the x to the table value.

## Checks done

- Animation 0 is the only one started; it exists (sprites 0–5, 3 frames each, loop). No event
  flags are tested.
- The boxes and the destination table are in `data.txt`.
- The fields table matches `summary.md` (the kit's `self+0x82` is work.82).

## Open questions

1. Which code reads `0x800CCEDF` = `0x40 | id` and loads the next area; whether `id` is the area
   number. So the destination of sequence A (Yammark record 34, `id` 1) is not known.
2. Which stage uses the destination table (`id` 2–4). It is the same in overlays 03 and 09. The
   y values (1115, 2074) and limits (x max 6080) do not match an obvious place in Yammark section 1.
3. The meaning of `game+0x26` (tested by `id` 3), `0x8008EAB0` (set by `id` 2), `mega+0xA4`,
   `+0xE0`, `+0xE4`, `game+0x1C`, and camera `+0x4C`.
4. Bank 1 sound id 1: which sound.
5. The player step 2 and state 3: what they show.
6. The real length of a fade at speed 8.
