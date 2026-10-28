# Doc changes from `yammark-4_blade-armor`

Base: the repo files as of this run. Source of each claim: `notes.md` in this folder.

## docs/players/steps-extra.md

Change 1: form 3 now has a state.

Old:

```
| X form 3 | 89 | code only (no state) |
```

New:

```
| X form 3 | 89 | byte match in `yammark-4_blade-armor` (form 3, `game.armorParts` = `0x0F`) |
```

Change 2: the armour layer; forms 0 and 4 do not look the same.

Old:

```
The member loads to the same address for all forms, so the player fields point to the same
places: `mega+0x30` (animation table, in overlay 0), `mega+0x38` (frame table) and the frame and
sprite directories hold the same addresses in the no-armour and Falcon states; only the bytes
differ [certain]. `mega+0x3C` differs because the sprite data is at another offset in each
member. The player `SetAnime` and the palette code do not test the form [certain for the code].
So **forms 0 and 4 have the same frames and palettes** in the paths read [certain for the code;
how the Ultimate Armor looks in play is a play check].
```

New:

```
The member loads to the same address for all forms, so the player fields point to the same
places: `mega+0x30` (animation table, in overlay 0) and the frame and sprite directories hold the
same addresses in the no-armour and Falcon states; only the bytes differ [certain]. With member 89
the frame directory has 20 entries (not 19), so the frame table `mega+0x38` is `0x80103B30`, not
`0x80103ACC` [certain]. `mega+0x3C` differs because the sprite data is at another offset in each
member. The player `SetAnime` and the palette code do not test the form [certain for the code].

**The armour is a second layer** [certain for the code]. Each packed frame unpacks to the tiles of
two sprites: the body (X with no armour) and the armour. The sprite directory has two piece lists
per frame set: entries 0 and 5 (set 0), 18 and 19 (set 1). `PlayerSetAnime` puts the body list in
`mega+0x3C` and the armour entry (5 or 19) in `mega+0xDC`. `DrawSprites` draws the body with
`DrawObject4`, then, for X in forms 1–4 only, `PlayerArmorLayer` (`0x8002619C`) draws the armour
list with `DrawObjectLayer` (`0x80025984`). That routine takes the CLUT of each piece from the
table `0x80071E68` (`0x7843`, `0x7800`, `0x7844`, `0x7801`, by piece flags >> 2 & 3), not from
`mega+0x42`. Forms 1–3 skip the layer for the beam (step 1, sprite 126); form 4 always draws it.
The armour layer is on top of the body [likely: by the look of the composites].

- CLUT `0x7843` is palette buffer slot 19, which `PlayerPaletteSet` fills with list palette n + 1
  (member offset `0x820` for n = 0; palette 4w + 1 with weapon w). CLUT `0x7844` is slot 20, which
  `PlayerPaletteRestore` fills for some part bits [certain for the code].
- Form 0 draws no armour layer, so it shows the body only. Member 86 holds armour pieces too:
  drawn with its palette 1 they show a black armour with red and white, the Ultimate Armor, which
  form 4 draws [likely]. So **forms 0 and 4 share the frames and the body palette, but only form 4
  has the armour layer**.
- The exports: `x-blade` (member 89), `x-falcon` (87) with the layer are in
  `~/mmx-data/x6/re/blade-armor/sprites-root/` until `playerexport` draws the layer.
```

Change 3: the beam-in palettes of form 2 (the arguments were read in the wrong order).

Old:

```
- Form 2 at the beam-in: Zero's skill bits are cleared (no effect for X), an effect object (type
  27, var 5) starts and two palettes (25, 24) load into palette slot 2 [certain for the code;
  what the effect shows is open].
```

New:

```
- Form 2 at the beam-in: Zero's skill bits are cleared (no effect for X), an effect object (type
  27, var 5) starts and palette 2 of the player palette list loads into palette slots 25 and 24
  (CLUTs `0x7849`, `0x7848`; `PaletteSlotLoad(palette, slot, n)`) [certain for the code;
  what the effect shows is open].
```

Change 4: the catalog name.

Old:

```
`catalog/players.json` has `86` → `x-no-armor`, `87` → `x-falcon`, `85` → `zero`.
```

New:

```
`catalog/players.json` has `86` → `x-no-armor`, `87` → `x-falcon`, `89` → `x-blade`, `85` →
`zero`.
```

Change 5: play check 3.

Old:

```
3. X in the Ultimate Armor (cheat): check that X looks like X with no armour (member 86 for both).
```

New:

```
3. X in the Ultimate Armor (cheat): check that X is black with red and white (the armour layer
   of member 86), not like X with no armour.
```

Change 6: open items.

Old:

```
- How the Ultimate Armor looks in play (forms 0 and 4 share member 86 in the paths read).
- A state in forms 2, 3 and 4, to check members 88 and 89 and the second frame set (the
  exporter rule "frames 10, sprites 18" is tested with members 86 and 87 only).
- The effect of form 2 at the beam-in (effect type 27 var 5, palettes 24 and 25).
```

New:

```
- (answered: the armour layer of member 86, black; play check 3) How the Ultimate Armor looks.
- A state in forms 2 and 4, to check member 88 and the Ultimate layer in play. Member 89 is
  checked (`yammark-4_blade-armor`): the rule "frames 10, sprites 18" holds, set 1 has 76 frames.
- The effect of form 2 at the beam-in (effect type 27 var 5, palette 2 in slots 24 and 25).
- The draw order of the two layers in the order table (the look says armour on top).
```

## docs/engine.md

Change 7: `mega+0xDC` is answered.

Old:

```
  sets 1 and 2 for 96. It also writes `mega+0xDC` (5 or 19 for X) [meaning open]. Four X
```

New:

```
  sets 1 and 2 for 96. It also writes `mega+0xDC` (5 or 19 for X): the sprite entry of X's armour
  layer (`docs/players/steps-extra.md`, "Sprites by form") [certain for the code]. Four X
```

Change 8: the member byte match.

Old:

```
  form: 1 → 87, 2 → 88, 3 → 89, 0 and 4 → 86 [certain for the code]. Byte match: 87 in the Falcon
  states, 86 in `sheldon-4_no-armor` [certain]. Each X member has one 228-frame set; the second
```

New:

```
  form: 1 → 87, 2 → 88, 3 → 89, 0 and 4 → 86 [certain for the code]. Byte match: 87 in the Falcon
  states, 86 in `sheldon-4_no-armor`, 89 in `yammark-4_blade-armor` [certain]. Each X member has
  one 228-frame set; the second
```

Change 9: the armour layer in the object draw, and the palette slot to CLUT rule.

Old:

```
The wide-cell flag (`0x20`) and the `0x80` path (a `POLY_FT4`) are in `DrawObject4` only.
```

New:

```
The wide-cell flag (`0x20`) and the `0x80` path (a `POLY_FT4`) are in `DrawObject4` only.

For X in forms 1–4, `DrawSprites` also calls `PlayerArmorLayer` (`0x8002619C`) after the player's
`DrawObject4`. It runs `DrawObjectLayer(mega, mega+0xDC)` (`0x80025984`): the `DrawObject4` path
with the pieces of sprite-directory entry `mega+0xDC` and the CLUT `player_layer_cluts[flags >> 2
& 3]` (`0x80071E68`: `0x7843`, `0x7800`, `0x7844`, `0x7801`) [certain for the code]. See
`docs/players/steps-extra.md`, "Sprites by form".

**Palette slots** [certain for the code]. `PaletteUpload` sends the palette buffer to the rectangle
(0, 480, 256, 32): buffer slot k is the CLUT `0x7800 + (k / 16) × 64 + k % 16` (16 CLUTs of 16
colours per VRAM row). So slots 0–15 are `0x7800`–`0x780F`, slot 19 is `0x7843` and slot 20 is
`0x7844`. `PaletteSlotLoad` (`0x8003CF24`) is `(palette, slot, n)`: list palette `palette`
(scratchpad `+0x24`) into buffer slot `slot`.
```

## docs/damage.md

Change 10: a state with parts equipped.

Old:

```
  `0x00040040`; `sheldon-1`, after the Turtloid clear, owned = `0x00450050`. No state has a part
  equipped.
```

New:

```
  `0x00040040`; `sheldon-1`, after the Turtloid clear, owned = `0x00450050`. Only
  `yammark-4_blade-armor` (a later save game) has parts equipped: owned `0x02F52D50`, X `0x50`
  (`game+0x74`, copied to `mega+0xEC`), Zero `0x00C00000` (`game+0x78`).
```

## README.md (apply only with the `playerexport` change in `notes.md`, section 6)

Change 11: the armour layer in the player export.

Old:

```
example `x-no-armor` (member 86) or `x-falcon` (member 87). A run replaces that
```

New:

```
example `x-no-armor` (member 86), `x-falcon` (member 87) or `x-blade` (member 89).
For X in forms 1–4 each sprite is the body with the armour layer on top, as the
game draws it (the second piece list of the frame set, CLUTs from `0x80071E68`;
`docs/players/steps-extra.md`). A run replaces that
```

## catalog/players.json

Change 12: the name of member 89.

Old:

```
  "members": {"85": "zero", "86": "x-no-armor", "87": "x-falcon"}
```

New:

```
  "members": {"85": "zero", "86": "x-no-armor", "87": "x-falcon", "89": "x-blade"}
```
