# Blade Armor state: notes

State: `~/mmx-data/x6/dumps/yammark-4_blade-armor.state.gz`. User: "slot 1, X is equipped with the
newly acquired Blade Armor".

## 0. The dump file is gzipped two times

`yammark-4_blade-armor.state.gz` is a gzip of a gzip. After one `gunzip` the header is `1f 8b`, not
`STv4 PCSXR`, so `mmxdemake.savestate.read_state` refuses it ("is not a PCSXR save state"), and so
do `playerexport`, `spriteexport` and `where.py` [certain]. The inner file has the same SHA-1 as
`~/.pcsxr/sstates/MEGAMAN_X6-SLUS01395.000` (`325fa485a9e136dcf6e94c841044966bee8a8a89`), so the
data is correct. Fix in the main session: replace the dump with the inner file:

```
zcat ~/mmx-data/x6/dumps/yammark-4_blade-armor.state.gz > /tmp/x.gz && mv /tmp/x.gz ~/mmx-data/x6/dumps/yammark-4_blade-armor.state.gz
```

I did not change the dump. I used a single-gzip copy in my scratch folder for all work.

## 1. What the state holds [certain for the values]

- Stage 1 area 0 checkpoint 0, stage time 65. `mega` step 2, (160, 395), facing `0x40`. The
  screen is still black (the frame buffers are black in VRAM), so the preview is blank.
- `game+0x5E` = 3, `mega.form` (`mega+0xE7`) = 3: **form 3 is the Blade Armor** [certain now: the
  user's words, `game.armorParts` and the member below agree].
- `game.armors` (`game+0x5F`) = `0x15`: Falcon (`0x01`), Blade (`0x04`), Zero (`0x10`).
- `game.armorParts` (`game+0x69`) = `0x0F`: all 4 Blade parts (head, arms, body, legs). The stage-
  clear code (`0x800F0668`, overlay 20) sets `game.armors` bit `0x04` from this nibble
  (`docs/enemies/light-capsule.md`).
- `game+0x60` (Zero skills) = `0xFF`. This is a later save game than the other states.
- Parts: owned `game+0x70` = `0x02F52D50`; equipped X `game+0x74` = `0x00000050`, Zero `game+0x78`
  = `0x00C00000`; `mega.parts` (`mega+0xEC`) = `0x50`. **This is the first state with parts
  equipped** (`docs/damage.md` says "No state has a part equipped"). X bits: `0x10` (Hyper Dash,
  `docs/players/movement.md`) and `0x40` (see 5).
- The player member in RAM is `ROCK_X6.DAT` member **89** (byte match by `playerexport`, offset
  151632) [certain]. This confirms the code rule "form 3 → member 89".
- `mega+0x38` = `0x80103B30` (not `0x80103ACC` as with members 86 and 87): the frame table address
  changes with member 89 because its header is longer (the frame directory at `0x80103AE0` has 20
  entries, not 19) [certain]. `mega+0x30` (`0x801F4AD4`) and the palette `0x7800` are the same.
- Stage objects: the type list is the same as in `yammark-1_start` (`[128, 15, 141, 7, 3, 4, 23,
  28, 37, 0, 0, 0, 0, 0, 0, 49]`), same place. **Nothing new for the Yammark exports.**

## 2. How X's armour is drawn: two layers [certain for the code]

The armour is **not** in the frames that `playerexport` draws. Every X frame holds two images.

- Each packed frame of a player frame set unpacks to the tiles of **two sprites**: the body (no
  armour) and the armour. Example, member 89 frame 0: 14 tiles; sprite entry 0 uses tiles 0–6,
  sprite entry 5 uses tiles 7–13 [certain].
- The sprite directory (scratchpad `+0x1C`) has two piece lists for each frame set: entries 0 and 5
  for set 0 (228 sprites each), entries 18 and 19 for set 1 (76 each with member 89) [certain, in
  all X states].
- `PlayerSetAnime` (`0x8003BA04`) and `PlayerSetAnimeAt` (`0x8003BB94`) set `mega+0x38`
  (frame table) and `mega+0x3C` (body pieces, entry 0 or 18), and **`mega+0xDC` = 5 or 19**: the
  sprite entry of the armour layer. This closes the open item "`mega+0xDC` [meaning open]" in
  `docs/engine.md`.
- `DrawSprites` (`0x80022960`), at `0x80022AC8`: `DrawObject4(mega)` draws the body. Then, at
  `0x80022AE0`, for X (`var` 0) in **forms 1, 2, 3, 4** only, it calls `0x8002619C`:
  - form 4: always `0x80025984(mega, mega+0xDC)`;
  - forms 1–3: the same, except when `mega.step` = 1 and the sprite `mega+0x47` = 126 (the beam).
- `0x80025984(obj, entry)` is a copy of `DrawObject4` with two changes: the pieces come from
  sprite-directory entry `entry` (not `+0x3C`), and the CLUT of each piece is
  `table[(flags >> 2) & 3]` with **`table` = `0x80071E68` = `0x7843, 0x7800, 0x7844, 0x7801`**
  (not `+0x42` + bits) [certain; the same table is in the EXE file and in all states].
- So form 0 (no armour) shows only the body. Forms 1–4 show the body and the armour layer.
- Which layer is on top: the armour layer. [likely: by the look of the composites. A
  body-on-top composite shows plain X with a few armour bits; armour-on-top shows a complete
  armoured X. I did not read the order-table insertion of both draws to the end.]
- Member 86 also has armour pieces in entry 5. Drawn with its palette, they show a **black armour
  with red and white: the Ultimate Armor** (form 4 uses member 86, and `0x8002619C` draws the layer
  for form 4) [likely]. So forms 0 and 4 do **not** look the same: form 4 has the armour layer.
  This answers the open item "How the Ultimate Armor looks in play" in
  `docs/players/steps-extra.md`, and corrects its sentence "forms 0 and 4 have the same frames and
  palettes" (same frames and body palette, but form 0 draws no armour layer).
- Some animations of member 89 have no armour pieces: these are the animations of other forms
  (the hover 22–26, the Ultimate attack 106, 133–138). They draw as plain X [certain for the data].

### Palettes of the armour layer [certain for the code and values]

- The palette buffer (scratchpad `+0x28`) is uploaded to the rectangle `0x8006E360` = (0, 480, 256,
  32): **buffer slot k is CLUT `0x7800 + (k / 16) × 64 + k % 16`** (16 CLUTs per row, 32 rows). So
  `0x7843` is slot 19 and `0x7844` is slot 20.
- `PlayerPaletteSet(mega, n)` (`0x8003CF54`): slot 0 = palette n of the player palette list
  (scratchpad `+0x24`, `0x80103000`, member offset `0x800`); X also slot 19 = palette n + 1. So the
  **armour palette is palette 1 of the member** (member offset `0x820`). In this state CLUT
  `0x7843` = list palette 1 (byte match in member 89 at `0x820`; for 87 and 86 the same rule in their
  states).
- `PlayerPaletteRestore` (`0x8003C4EC`): with a weapon selected (`+0x93` = w), slot 0 = list
  palette 4w, slot 19 = 4w + 1 (and slot 1 = 4w − 1 for some w). So the armour colours change with
  the weapon, as the body colours do. Slot 20 (CLUT `0x7844`) gets list palette 79 (part `0x80`),
  78 (part `0x100`) or another for part `0x40000`; else it keeps its stage value.
- Armour palette 1 of member 89 (BGR555): `0000 cbbf a23d a195 94dd 8cac ff9b deb4 d22f b969 9063
  83e0 8269 ede7 e124 c4a3`. Entry 11 is pure green (`0x83E0`); the same in Falcon. [guess: a
  colour that the charge or a palette effect changes; it shows as green gems in the export.]
- Correction to `docs/players/steps-extra.md`: `PaletteSlotLoad` (`0x8003CF24`) is
  `(palette, slot, n)`: it copies list palette `a0` into buffer slot `a1`. So the form 2 beam-in
  loads **palette 2 into slots 25 and 24** (CLUTs `0x7849`, `0x7848`), not "palettes 25, 24 into
  slot 2" [certain for the code].

## 3. What the Blade Armor changes in play

The code side is already in `docs/players/steps-extra.md`, `air.md`, `attacks.md` and
`docs/damage.md` (form 3: Mach Dash step 74, special attack step 77, full-charge shot type 43,
weapon id 32 bonus, energy 2/3, charge of special weapons, class 4 hits, damage / 2). I listed all
30 reads of `mega+0xE7` in RAM (list in the scratch file
`/tmp/claude-1000/-home-johnlavender-mmx-data/2ad6db8a-32e9-4897-bc53-bda0c68a0213/scratchpad/blade-armor/form-tests.txt`); none is a form-3
path that the docs do not have. The only new form-gated code is the armour layer draw above.
This state is at the stage start, so it shows no Blade action.

## 4. Exports

All under `~/mmx-data/x6/re/blade-armor/sprites-root/players/`:

- `player-m89/`: plain `playerexport` (no catalog name for 89). 181 animations, 12 skipped (85,
  172–182: sprites past the 76 of set 1; the Falcon dash and ceiling-hold animations that member 89
  does not hold). **The sheets show plain X**: the body layer only, pixel-identical to set 0 of the
  shared `x-falcon` export. Correct as a body layer, wrong as the Blade Armor.
- `x-blade/`: the same export with the armour layer, made by `scripts/layered_export.py` (it runs
  `playerexport.run` with a replaced `render`, and the name `x-blade` for member 89). 181
  animations, the same 12 skipped. I checked every sheet by eye (all 181 `anim-*.png` in contact
  images, and both `sprites-*.png`): the Blade Armor (blue, gold crest, red hands, buster) on all
  Blade actions; Mach Dash 189–192 and the sabre poses 183–188 correct; animations of other forms
  plain (as in the data).
- `x-falcon/`: the same layered export of `shark-1_ride` (member 87), as a check of the method:
  it shows the Falcon Armor (white and blue, orange wings). **The shared
  `~/mmx-data/x6/sprites/players/x-falcon/` lacks this layer** and shows plain X.

Scripts (`scripts/`): `alt_entry.py` (draw any frame entry with any sprite entry),
`armor_layer.py` (body, armour, composite sheets of one set), `layered_export.py` (above).

## 5. Part bit `0x40` (side result)

`0x8004DDD4(obj, kind)`: kind 5 refills 4 units (**6 with part `0x40`**), kind 6 refills 16
(**22 with part `0x40`**), kind 7 the full gauge (`mega+0x14A`); the amount × 6 goes into
the weapon gauges from `mega+0xAA` [certain for the amounts; likely: the weapon energy pickups;
which gauge gets it is not read to the end]. `0x8004E3D4`/`0x8004E408`
also test part `0x40` and call `0x8004E0A8` with (5 or 4, 1) and (20, 3) [not read]. The docs
have no pickup sheet, so I made no doc change for this; helper names are proposed.

## 6. What `playerexport` lacks, and the proposed code change

The tool draws only `mega+0x3C` (sprite entry 0 / 18) with CLUT `mega+0x42`. For X in forms 1–4 the
game also draws the armour layer. The tool lacks:

1. the second sprite entry per set (5 for set 0, 19 for set 1: what `PlayerSetAnime` writes to
   `mega+0xDC`);
2. the layer CLUT table `0x80071E68` (by piece flags), in place of `+0x42` + bits;
3. the form test (character 0 and `mega+0xE7` in 1–4) and the beam exception (set 0 sprite 126 has
   no layer unless form 4);
4. a catalog name for member 89 (and 88).

Proposed change (not applied):

- `mmxdemake/sprites.py`, `draw`: add `cluts=None`; per piece, `clut = cluts[(flags >> 2) & 3] if
  cluts else bank.clut + ((flags >> 2) & 3)`. (Default behaviour unchanged.)
- `mmxdemake/playerexport.py`:
  - `LAYER_RULES = {0: (0x80071E68, {0: 5, 18: 19})}` (overlay 0 only), `FORM = 0xE7`,
    `LAYER_FORMS = range(1, 5)`, `BEAM_SPRITE = 126`.
  - `Player` gets `form` (`mega+0xE7`).
  - `frame_sets`: for each set, also read the armour piece list (sprite entry from the rule) into a
    second `Bank` (same texture; CLUT unused).
  - `render`: when `p.character == 0` and `p.form` in 1–4, after the body image of sprite n,
    unpack the same frame once, draw the armour bank with `cluts` = the 4 halfwords at
    `0x80071E68`, and merge it **over** the body (skip set 0 sprite 126 when form ≠ 4). The
    unpacked VRAM copy can be shared by both draws.
  - `animations.json` `source`: add `"layer": {"sprite_entries": {"0": 5, "1": 19}, "cluts":
    ["0x7843", "0x7800", "0x7844", "0x7801"]}` when the layer is drawn.
- `catalog/players.json`: `"89": "x-blade"` (and `"88": "x-shadow"` when a Shadow state exists).
- Tests: a synthetic frame with two piece lists, checking that form 0 gives the body only and form
  3 gives the body with the layer on top in the table CLUT.
- After the change, run `playerexport` again on a Falcon state (the shared `x-falcon` export is
  plain X now), on `sheldon-4_no-armor` (no change expected), and on this state (`x-blade`).

`scripts/layered_export.py` gives the expected output of this change.

## 7. Play checks

1. Blade Armor X in play: a gold crest, a blue body with gold trim, and green gems (check the
   green colour; entry 11 of the armour palette).
2. Ultimate Armor (cheat): X is black with red and white, not like X with no armour.
3. With a special weapon selected, the armour colours change (palette 4w + 1 in slot 19).
4. With part `0x40`: a small weapon energy pickup gives 6 units, a large one 22.
