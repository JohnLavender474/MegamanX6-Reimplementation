# `wolfang-4_x-crouch-shot` (X crouches and shoots, Blizzard Wolfang stage)

State: `~/mmx-data/x6/dumps/wolfang-4_x-crouch-shot.state.gz` (SHA-1 `7f1f4fa14fc1e61fa5189f8f2c6039544fde4020`).
Scripts: `scripts/dump_state.py` (player, weapon and effect slots), `scripts/dis.py` (disassemble RAM).

## What is live [certain]

- X, form `mega+0xE7` = 1 (Falcon, member 87), step 26 sub-step 1, at (2960, 395), `flip` `0x40`.
- `mega+0x34` = `0x801F40B0` = animation 108 (`0x801F40AC`) frame 1. `mega+0x44` = `5a 00 00 d4`:
  90 of 96 ticks left, sprite 212. Frame 0 (2 ticks) + 6 ticks = 8 updates since the animation started.
- `mega+0x8E` = 1 (shot pose), `+0x8F` = 0, `+0x91` = 12 (20 − 8: the lemon was fired 8 updates ago),
  `+0x98` = 1, `+0x38` = `0x80103ACC` (frame set 0), `+0x3C` = `0x80147C68`, CLUT `0x7800`.
- Weapon slot 1: id 0 (lemon), state 1, at (3045, 399), `speed_x` 6.75, `+0x30` = `0x8007789C`
  (`shot_anime_tbl`), `+0x34` = `0x80076D94` = animation 21 frame 4 (sprite 8), `+0x3C` =
  `0x80149B1C`, texture 0, CLUT `0x7802`.
- No live effect objects (the lemon muzzle flash, animation 39, has ended).

## Lemon start point in the crouch [certain]

Buster offset table `0x8007673C` (2 signed bytes per sprite): sprite 212 = (−31, 4); also 209, 211
and 213. Mirrored by the facing: the buster is 31 px in front of X and 4 px below the origin.
Check: lemon y 399 = 395 + 4; lemon x 3045 = 2960 + 31 + 8 × 6.75.

## Player sprites: nothing new [certain]

- `playerexport` gives `x-falcon` (member 87, sets of 228 and 93 sprites, 189 animations, 4 skipped).
  All 191 PNGs are byte-identical to `~/mmx-data/x6/sprites/players/x-falcon/`; `animations.json`
  differs only in `source` (the state name).
- Animation 108 is in the shared export: frames (212, 2 ticks), (212, 96 ticks, hold). The strip is
  complete and correct by eye (Falcon colours, arm and buster forward, no loose pieces).
  81, 82, 109, 110 and 139 are also present and look correct.
- The skipped animations 85, 180, 181, 182 stay skipped: this state has the same member (87), the same
  form (1) and the same set 1 (93 sprites) as `shark-1_ride`. A new state cannot fix them; they need
  the code that picks their frame set (`docs/TODO.md`, "Player animations that use sprites past
  their frame set").

## Other stage objects: nothing new [certain]

`spriteexport` gives 8 IDs. 7 are already in `sprites/enemies/` with byte-identical PNGs:
`ice-block`, `nightmare-virus`, `nightmare-virus-orb`, `nightmare-shadow`, `teleport-crystal`,
`enemy-78c8f556f5`, `enemy-cd98e86270`. Type 6 (no table found) and the types without texture (0, 54)
are skipped as before.

## The lemon: new ID, but not a correct export [certain]

`spriteexport` makes `enemy-54284bb54d` from the live weapon (class `weapon`, type 0): all 40
animations of `shot_anime_tbl` (`0x8007789C`), drawn with the lemon's sprite data and CLUT.

- The bank sheet `banks/wolfang/wolfang-4_x-crouch-shot/weapon-0-live-0.png` is pixel-identical to
  `players/x-shots/bank-80149b1c.png` (the probe sheet).
- Animations 21 (lemon) and 39 (lemon muzzle flash): every frame is pixel-identical to
  `players/x-shots/anim-21-lemon.png` and `anim-39-lemon-flash.png`. So these are already extracted.
- The other 38 animations are wrong or not checked. `shot_anime_tbl` is shared by objects that take
  other sprite data and other CLUTs. From the code:

  | user | animation | sprite data (directory `scratch+0x1C` entry) | CLUT |
  |---|---|---|---|
  | lemon, weapon id 0 (`LemonInit` `0x801ECE0C`) | 21 | 1 | `0x7802` (`0x7844` with part bits `0x180` or `0x40000`) |
  | muzzle flash, effect type 2 var 0 (`0x80049270`, table `0x80077C0C`) | 39 | 1 | `0x7802` (variants by `0x801EE434` and part `0x40000`) |
  | muzzle flash var 9 (half charge) | 22 | 3 | `0x7802` |
  | muzzle flash var 18 (full charge) | 23 | 2 | `0x7802` |
  | muzzle flash vars 19, 20 | 25 | 2 | `0x7802` |

  The muzzle table `0x80077C0C` has 3 bytes per entry: {sprite-data entry, animation, sound id}.
  Animations 22, 26 and 27 of the new export show lemon-bank sprites (blue rings, green bursts), not
  the half and full charge of `players/x-shots/`. The special-weapon animations (for example 0–20)
  likely use entry 1 sprites, but their CLUTs are not `0x7802` [likely: the green and blue sprites
  of the bank].

So the export is correct for 2 of 40 animations, and those 2 are already in `players/x-shots/`.

**Cause:** for a live weapon or effect object, `add_export` (`mmxdemake/spriteexport.py`) calls
`fitting`, which keeps every animation of the table whose sprites are below the source's sprite
count. The sprite data at entry 1 has 212 sprites, so all 40 animations fit.

**Fix (proposal):** for a live weapon or effect object, export only the animations that the object
can show with its sprite data and CLUT. Two ways:
1. A catalog rule per shared table: `{"table": "0x8007789C", "entry": 1, "clut": "0x7802",
   "animations": [21, 39]}` (entries 2 and 3 as in the table above). The export then has
   only those animations.
2. Without a catalog rule: keep only the animation that holds the object's current frame pointer
   `+0x34` (here animation 21). This is always correct but gives one animation per state.

Until then, do not put `enemy-54284bb54d` in `sprites/enemies/`.

## Crouch code (step 26) [certain for the code]

- `StartCrouch` (`0x8003B9AC`): animation 109 when the halfword at `mega+0x8E` is not 0
  (`lw +0x8C` & `0xFFFF0000`: `+0x8E` shot pose or `+0x8F` shot this update), else 81. It is not the
  dash flag: the dash flag is the halfword `+0x8C`, which the mask removes.
- Sub-step 1 (`0x80035F24`): `DashTerrainBox` (`0x8003CD00`). Then, when `+0x8F` ≠ 0 (a shot in
  this update) and the character is X (`+0x02` = 0): animation 139 when the fired id `+0x96` is 18,
  19, 20, 43 or 46 (the full charges and id 46), else 108. With no shot: while `+0x8E` = 0 and X,
  `PlayerSetAnimeAt(81, 2)` in each such update (the crouched frame of 81).
- Down released: animation 110 when the halfword `+0x8E` is not 0 and X, else 82; sub-step 2.
- The state matches: shot pose on, animation 108, lemon (id 0).
