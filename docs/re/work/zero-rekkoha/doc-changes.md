# Doc changes from `heatnix-3_rekkoha` (Zero's Rekkoha)

Facts and checks: `notes.md` in this folder. Each change gives the old text and the new text.

## `docs/players/zero.md`

### Sources (line 9)

Old:

```
`turtloid-6_zero-player`, `turtloid-7_boss`, `turtloid-8_high-max`, `scaravich-1_start`,
`scaravich-2_totem-section`. Notes, scripts, pseudo code and reconstructed C are in
`~/mmx-data/x6/re/zero/attacks/`. Units and tags as in `docs/engine.md` ("Conventions"): speeds
```

New:

```
`turtloid-6_zero-player`, `turtloid-7_boss`, `turtloid-8_high-max`, `scaravich-1_start`,
`scaravich-2_totem-section`, `heatnix-3_rekkoha` (Rekkoha in progress). Notes, scripts, pseudo
code and reconstructed C are in `~/mmx-data/x6/re/zero/attacks/` and, for the Rekkoha,
`~/mmx-data/x6/re/zero-rekkoha/`. Units and tags as in `docs/engine.md` ("Conventions"): speeds
```

### Rekkoha section (replace the whole section from `### Rekkoha (step 59, bit `0x80`)` to `Total about 180 updates [likely, from the timers].`)

New:

````
### Rekkoha (step 59, bit `0x80`)

Giga button on the ground with 72 energy [certain]. Zero is locked (`game.player_lock`), cannot
be hit (`no_hit`), and has no terrain box for the whole attack. Voice 7 at the start. Sub-steps
(`rekkoha_steps`, `0x801EE39C`) [certain for the code; update numbers ±1]:

| update | what |
|---|---|
| 0 | animation 104; `PaletteFxReset`, channel 1 scale mode, `mega+0xE4` \|= 1 |
| 1–15 | `fade_r[1]` + 2 per update: the BG goes dark |
| 16, 17 | **white flash**: `fade_r[1]` = 31, `fade_mask[1]` saved (`0x801EF974`) and set to all 512 CLUTs, add mode (+31): the whole screen, sprites too, is white |
| 18 | mask written back, scale mode with 31: the BG CLUTs are transparent black; 10 backdrop objects (effect type 17) |
| ≈23 | animation 104 frame flag `0x80`: sound (1, 27) |
| ≈27 | frame flag `0x40`: sound (1, 28); the waves start |
| every 10 | a wave (below) until 11 beams are counted |
| +71 | animation 111; `fade_r[1]` − 8 per update (4 updates); at 0 `+0xE4` &= ~1 |
| hold frame of 111 (31 updates) | `+0x93` = 0 (the gauge selection goes back to none), busy = 0; unlock and `no_hit` = 0 (when `+0xD3` = 0); then stand or fall |

Total about 181 updates with Zero in the middle of the view [likely, from the timers].

- **Waves** (`RekkohaWave`, `0x801EAA24`): wave k makes one beam at Zero's x (k = 0) or two at
  x ∓ 32k. A beam outside the view (x < cam x − 16 or > cam x + 336) deletes itself and is not
  counted. With Zero in the middle: 6 waves (1 + 2 × 5 = 11 beams, the last pair at the view
  edges); near an edge: up to 10 waves [certain for the code].
- **Beam** (weapon id 44, `RekkohaBeam` `0x801EDFC8`): starts at cam y − 128, falls 8 px per
  update, deleted at cam y + 368 (62 updates). Box (−16, −128, 32 × 255) (`0x801EE3DC`). Hit tag
  + 1 every 8 frames (the global frame counter): one hit per 8 updates on a target. Each hit **or
  deflect** counts; after 4 the box is removed, but the beam goes on falling. Damage 8 in
  `dmg_tbl_normal` (other tables below). Hit sound (1, 9), kill (1, 10); spark variant 7.
- **Top part** (effect type 26, `RekkohaBeamTop` `0x801EE254`): made with each beam at the same x,
  160 px higher (cam y − 288), the same speed, no box; deleted at cam y + 400.
- **Backdrop** (effect type 17, `GigaBackdrop` `0x8004AE1C`): 5 rows of 2 sprites (83: 256 × 64
  at screen x 128; 84: 64 × 64 at screen x 384, which fills x 256–319), a red and yellow streak
  curtain over the full view. It scrolls up 8 px per update (wraps at the view top to 320). It
  deletes itself when `fade_r[1]` ≠ 31 or Zero leaves step 59. Priority 55.

Id 44 in the other damage tables: 8 for `nightmare_insect`, `nightmare_snake`, `yammark` (his
stun id), `sabre_only`, `reflector`; 4 `minor`; 2 `d1000`; 127 `one_hit`; deflected by
`deflect`, `illumina`, `nightmare_bug`, `sheldon`, `sheldon_guard`, `shark`, `wolfang`,
`mijinion`, `ice_block`, `grey_block`; no effect on `reflector_wait`, `nightmare_shadow`,
`capsule` [certain, read from the executable]. A deflect uses one of the beam's 4 hits.

Live check (`heatnix-3_rekkoha`, Zero in the middle of the view, step 59.4, `+0xE1` = 20)
[certain]: `act4` = 3 beams after the waves at `+0xE1` = 1 and 11; beam y = cam y − 128 + 8 ×
moves (504 after 19 updates, 424 after 9: the set-up of a weapon object runs in the update of its
spawn); hit tags 3, 2, 2; Nightmare Snake's gem took 2 × 8, its body deflected beam 0 three times
(`+0x8C` = 3); energy slot 5 = 216 (288 − 72); `mega+0xE4` = 1; channel 1 in scale mode at 31.

Sprites: Zero's animations 104 and 111 are in `players/zero/`. The beam (animation 11 of the
shared table `0x8007798C`: sprites 0–2, orange, blue and cyan, one update each) and the top part
(animation 12: sprites 3–5) use sprite data `scratch+0x1C` entry 15 and CLUT `0x7802`. The
backdrop uses `scratch+0x3C` entry 0, texture `0x1E00`, CLUT `0x7E50`. `spriteexport` does not
export them (`docs/TODO.md`); kit exports are in `~/mmx-data/x6/re/zero-rekkoha/sprites-root/`.
````

### Play check 11

Old:

```
11. Rekkoha: 11 beams 32 px apart, one wave every 10 updates; Zero cannot be hit.
```

New:

```
11. Rekkoha: white flash, then the BG goes black behind a red curtain that scrolls up; 11 beams
    32 px apart, one wave every 10 updates (more waves near a view edge); Zero cannot be hit; at
    the end the gauge selection is back to none.
```

### Open items

Old:

```
- Whether a weapon object made in the player step runs its first box test in the same update
  or the next one (the "box live" times above count from its first own update).
```

New:

```
- Whether a weapon object made in the player step runs its first box test in the same update
  or the next one (the "box live" times above count from its first own update). (partly
  answered: its set-up runs in the same update, `heatnix-3_rekkoha`; its first move in the next.)
```

Old:

```
- A state in a technique (steps 48, 55–59) to test the timings.
```

New:

```
- A state in a technique (steps 48, 55–58) to test the timings. (step 59: tested with
  `heatnix-3_rekkoha`.)
- Rekkoha: what `mega+0xE0` (2 during step 59) and `mega+0xD3` do; who uploads the backdrop
  CLUT 416, which the dark mode does not clear.
```

Note for the stage numbers (no text change proposed): the state has `game+0x60` = `0xFB` (all bits
except `0x04`) while the user plays Blaze Heatnix's stage (stage 3) after he beat Infinity
Mijinion and gained the Rekkoha. This agrees with `0x04` = Heatnix (3) and `0x80` = Mijinion (8)
[likely]; it is not a proof from a stage overlay.

## `docs/engine.md` ("Palette effects")

Old:

```
- The teleporter crystal's warp clears `mega+0xE4` (one copy in each stage overlay). Bit `0x02`
  and channel 2 have no writer: a leftover. `mega+0xE4` is 0 in all 44 dumps.
```

New:

```
- The teleporter crystal's warp clears `mega+0xE4` (one copy in each stage overlay). Bit `0x02`
  and channel 2 have no writer: a leftover. `mega+0xE4` is 0 in the 44 earlier dumps.
  `heatnix-3_rekkoha` (Zero in step 59.4) has `+0xE4` = 1, `fade_mode[1]` = 1, `fade_r[1]` = 31,
  `fade_mask[1]` = the stage BG mask and `bg_refresh` = 8: the BG CLUTs are all 0 in
  `palette_fx_buf` and in VRAM [certain].
- The Rekkoha (step 59) is the same sequence with other times: the flash at update 16 for 2
  updates, then scale mode with 31 until the attack ends, then −8 per update
  (`docs/players/zero.md`). It also writes 1 to `fade_sub_g[1]` and `fade_sub_b[1]` and adds
  64 / 2048 per update to `fade_g[1]` / `fade_b[1]`. `PaletteFxChannel` reads only `fade_mode` and
  `fade_r` in scale mode, so these writes count only in add mode (the flash) [certain for the
  code].
```

## `docs/TODO.md` ("Player sprites", after the lemon item)

New item:

```
- [ ] Tool: `spriteexport` skips the Rekkoha beam (weapon id 44) and its top part (effect type
  26), both on the shared table `0x8007798C` with sprite data `scratch+0x1C` entry 15
  (`heatnix-3_rekkoha`): "animation 0 uses sprite 8, bank -1 has 6". `fitting` keeps only the
  leading animations that fit, and animation 0 of this table does not fit. Only animations 11
  (beam) and 12 (top part) are theirs. The same fix as the lemon: keep the animations of the
  object's own sprite data (a catalog rule per entry, or the animation of `+0x17`), not a prefix.
  Kit export until then: `~/mmx-data/x6/re/zero-rekkoha/sprites-root/enemies/rekkoha-beam/`.
- [ ] Tool: live effect objects whose sprite data is in the `scratch+0x3C` directory (the Giga
  backdrop, effect type 17, entry 0, texture `0x1E00`, CLUT `0x7E50`, sprites 83/84 for Zero) are
  not exported: they have no animation table and the data is not a `sprite_bank_base` entry. Kit
  export: `~/mmx-data/x6/re/zero-rekkoha/sprites-root/enemies/rekkoha-backdrop/`.
- [ ] `spriteexport` on `heatnix-3_rekkoha` makes `enemy-2fbbe8998b` (main type 36, search, bank 3,
  table `0x800F3CBC`) with every sprite blank (cell 1 × 1). Do not copy it; find why bank 3 draws
  nothing in this state.
```

## [`~/mmx-data/x6/re/zero/attacks/notes.md`](../zero/attacks/notes.md)

### Live check, last bullet

Old:

```
- No state has a live sabre object or a technique in progress, so the timings below are from the
  code only. A state in step 48, 55 or 59 would test them.
```

New:

```
- No state has a live sabre object or a technique in progress, so the timings below are from the
  code only. A state in step 48, 55 or 59 would test them. (step 59: `heatnix-3_rekkoha` agrees
  with the code: waves, positions, hit tags, hit count, energy; `../../zero-rekkoha/notes.md`.)
```

### Open (add)

```
- Rekkoha (`../../zero-rekkoha/notes.md`): `mega+0xE0` = 2 during step 59 (no reader read);
  `mega+0xD3`; who uploads the backdrop CLUT 416. `pseudocode.md` calls effect type 26 a "light":
  it is the top part of the beam; the type 17 objects are a scrolling backdrop (sprites 83/84),
  not animations.
```

### Helper names

`helpers-proposed.json` of `zero/attacks` already has `TryRekkoha`, `RekkohaFadeIn`,
`RekkohaLights`, `RekkohaWave`, `RekkohaFadeOut`, `ZeroStep59`, `RekkohaBeam`, `rekkoha_steps`
(none of them is in `catalog/helpers.json` yet). Change the note of `RekkohaLights`
(`0x801EA888`) to: "step 59.2: at the 2nd update write the mask back; 10 effect objects type 17
(the backdrop: sprites 83/84 for Zero, 85/86 for X) at screen x 128/384, y 64…320; sub 3". The
other names are in `../zero-rekkoha/helpers-proposed.json` (no clash with `catalog/helpers.json`
or with the `zero/attacks` proposal).
