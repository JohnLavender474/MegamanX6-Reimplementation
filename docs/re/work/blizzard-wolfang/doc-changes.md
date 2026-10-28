# Doc changes for Blizzard Wolfang (main 54, stage 2)

Check the claims that these texts depend on before you apply them (`notes.md` has the evidence).

## 1. New file `docs/enemies/blizzard-wolfang.md`

Copy [`~/mmx-data/x6/re/blizzard-wolfang/sheet.md`](sheet.md) (remove the HTML comment at the top). The section
"The boss room" can move to a future `docs/stages/wolfang.md` (docs/TODO.md asks for that page);
then leave one line in the sheet: "The room: `docs/stages/wolfang.md`, "The boss room"."

## 2. `docs/objects-legend.md`, the type table: add after the `Wolfang | 134 | 4` row

```
| Wolfang | 54 | 14 | Blizzard Wolfang, the boss; the same type with `var` 1 (trail image) and 16–18 (ice spikes), and his ice balls, debris and wall runners (visual 20) and mouth ice (effect 8) use the same table and sprites (`docs/enemies/blizzard-wolfang.md`) | `blizzard-wolfang` (`enemy-6757bcd2a2`, from `wolfang-6_boss-warning` and `wolfang-7_boss-fight`) | code reading; boss of stage 2 |
```

In the misc 23 row (`Wolfang | 144 | 8`), add to the export column: "; wrong in the boss states
(the boss texture covers its cells; `docs/TODO.md`)".

## 3. `docs/engine.md`

### 3a. "Weapon ids": add a row after the row `| 1, 10, 54 | …`

```
| 3, 12, 34 | the stun ids of Blizzard Wolfang (5, 8, 8 damage; `docs/enemies/blizzard-wolfang.md`): slot 3 (Magma Blade) uncharged and charged, and id 34, Zero's rising fire wave [certain for the ids] |
```

In the row `| 2, 11, 33 / 4, 13, 35 / 6, 15, 42 / 8, 17, 44 | the stun ids of Rainy Turtloid / …`,
the pattern "slot n, slot n + 9, one id of 33–44" now also has 3, 12, 34 (Blizzard Wolfang). Add
at the end of the text of that row: "Blizzard Wolfang: 3, 12, 34 (slot 3 stuns him)."

### 3b. "The damage tables": add after `dmg_tbl_shark`

```
`dmg_tbl_wolfang` (`0x8007519C`, Blizzard Wolfang, `docs/enemies/blizzard-wolfang.md`):

| ids | damage |
|---|---|
| 0–15 | 1 2 D 5 2 2 2 – 2 1 2 D 8 2 2 2 |
| 16–31 | 2 8 4 4 4 D 8 D 1 1 2 2 2 2 2 2 |
| 32–47 | 2 2 8 2 2 2 2 2 2 2 2 2 D 1 4 8 |
| 48–63 | 2 1 2 2 D 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |
```

### 3c. "Palette effects": add a bullet after the `PaletteFxReset` bullet

```
- **Channel 0 in subtract mode.** `PaletteFade` reads three subtract flags: `fade_mode[0]`
  (`0x800CF850`) for red, `fade_sub_g` (`0x8009742C`) for green and `fade_sub_b` (`0x80090D56`) for
  blue. With all three set and `fade_r/g/b[0]` = 31, 31 × 32, 31 × 1024, the masked CLUTs are black.
  Blizzard Wolfang's entry uses it: the room (CLUTs 32–35 and 96–479) goes dark in 124 updates, the
  boss CLUT (88–95) joins the mask later, and all fades back (`docs/enemies/blizzard-wolfang.md`)
  [certain for the code]. The other writers of the two flags (`0x8002BBC0`, `0x800505D0`,
  `0x800523B8`, `0x80052440`, `0x800525A0`) were not read.
```

In "Who uses it", the text "Only bit `0x01` has writers" is about channels 1 and 2; no change.

### 3d. "Boss entry and exit": add at the end of the `BossWarning` bullet

```
  Blizzard Wolfang's entry keeps the player walking (event step 29) for 20 updates while the WARNING
  runs, then waits for the warning object to end.
```

## 4. `docs/players/weapons.md`, slot 3 row, column "proof of the name"

Replace

```
the slot 3 shot is a sabre slash with a fire palette and fireballs; the only stage left for Heatnix [likely]
```

with

```
the slot 3 shot is a sabre slash with a fire palette and fireballs; the only stage left for Heatnix; 3, 12 stun Blizzard Wolfang (5 and 8 damage), whose weakness is Magma Blade (Fandom), with Zero's id 34 [likely]
```

## 5. `catalog/enemies.json`, `names`

```
"enemy-6757bcd2a2": "blizzard-wolfang",
```

## 6. `catalog/object-names.json`, `members` → `"96"`

```
"main:54": "BlizzardWolfang",
```

## 7. `catalog/helpers.json`

Add the entries of `helpers-proposed.json` (30 functions, 14 globals). No name and no address
clashes with the current file (checked). The kit names `0x8009742C` as `white_fade_dir+0x7` and
`0x80090D56` as `sound_bank_vab+0x6` today; the new names `fade_sub_g` and `fade_sub_b` fix that.

## 8. Sprites (main session, after the catalog name in 5)

Run `spriteexport` on a copy of `sprites/` for `wolfang-6_boss-warning` and `wolfang-7_boss-fight`
and check by eye. Then:

- keep `enemies/blizzard-wolfang/` (`enemy-6757bcd2a2`, 34 animations): good;
- delete `enemies/enemy-6e86b66bc1/` (misc 23, wrong: the boss texture covers its cells; the good
  export is `enemy-cd98e86270`) and list it in `sprites/stages/wolfang.json` as skipped for both
  states, reason "misc 23 entry 8: cells overwritten by type 54 (boss texture)";
- the bank sheets `banks/wolfang/wolfang-6_boss-warning/entry-8-live-0.png` and
  `banks/wolfang/wolfang-7_boss-fight/entry-8-live-0.png` are wrong for the same reason (delete);
- `boss-gate`, `nightmare-virus`, `nightmare-virus-orb`, `teleport-crystal`: same IDs, nothing new.

## 9. `docs/TODO.md`: add

```
- [ ] Tool: the overwrite check of `spriteexport` does not cover the extra sprite source of a live
  misc object. In `wolfang-6_boss-warning` and `wolfang-7_boss-fight` the boss texture (type 54, no
  texture entry, banks 4–8 marked "overwritten by type 54") also covers the cells of misc 23 (entry
  8, the snow scenery), and the tool makes a wrong sheet `enemy-6e86b66bc1` (the good one is
  `enemy-cd98e86270` from `wolfang-5_ice-pit`). Fix: apply the same cell test to extra sources.
- [ ] Blizzard Wolfang: save states at boss level 2 or 3 (ice spikes on the floor, walls and
  ceiling), of a stun (Magma Blade), of the ceiling moves (HP half or less) and on Hard (debris that
  hurts) would confirm the parts read only from the code (`docs/enemies/blizzard-wolfang.md`, open
  questions).
```

In the existing item "Blizzard Wolfang ice: … a stage page `docs/stages/wolfang.md` (…)", add "the
boss room and its way in (`docs/enemies/blizzard-wolfang.md`, "The boss room")" to the list of what
the page gets.

## 10. `docs/stages/scaravich.md`, "Death"

No change. The Wolfang sheet points to its death text ("The same as Ground Scaravich").
