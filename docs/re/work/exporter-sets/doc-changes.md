# Exporter sets, actor2, D-1000: proposed changes to files that the exporter agent must not edit

## 1. `catalog/enemies.json`: add `sets`

The exact content that the check run used (`catalog-sets/enemies.json` = the repo catalog plus this key):

```json
"sets": [
  {"stage": 0, "type": 48, "set": 1, "animations": [6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23], "texture": null, "palette": 0},
  {"stage": 4, "type": 56, "set": 1, "animations": [34, 35, 36, 37], "texture": "0x0540", "palette": 1},
  {"stage": 6, "type": 57, "set": 1, "animations": [28, 29, 30, 31, 32], "texture": "0x0140", "palette": 0},
  {"stage": 6, "type": 57, "set": 2, "animations": [33], "texture": "0x0540", "palette": 1},
  {"stage": 7, "type": 51, "set": 1, "animations": [0, 1, 2, 3, 35, 36], "texture": "0x0540", "palette": 1}
]
```

Sources: Turtloid `0x800F0434` / `0x800F14D4` (overlay 09); Sheldon `0x800F5F04` (overlay 10);
Shark `0x800F4144` / `0x800F6E78` (overlay 07); D-1000: every `SetAnime` call in overlay 02 loads
`bank_base[k]` (animations 0–5) or `bank_base[k + 1]` (6–12, 14–23) just before the call.
Animation 13 has no call; it uses sprite 16, which only set 1 has.

Important: until this key is in the repo catalog, a `spriteexport` run of `turtloid-7_boss`,
`sheldon-17_boss`, `shark-7_boss`, `shark-9_boss-revive` or `intro-3_boss` with the repo catalog
draws the wrong strips again into `~/mmx-data/x6/sprites/`. The exporter agent re-exported these
five states (and `shark-1_ride`) into the real root with `catalog-sets`.

## 2. `catalog/enemies.json`: names

| ID | proposed name | what |
|---|---|---|
| `enemy-e32b3dd423` | `ride-armor` | `actor2` type 0 in `shark-1_ride` (31 animations) |
| `enemy-f15c898594` | `d-1000-sets` | D-1000 type 48 with both sets (24 animations) |

`d-1000` cannot be reused for `enemy-f15c898594`: the folder `enemies/d-1000` holds
`enemy-5473b61b53`, and the exporter refuses a folder that holds another ID. Options for the
user: keep the old `d-1000` and `d-1000-parts` folders (their strips 6–9 and 0–5 are wrong), or
retire them and give `d-1000` to the new ID. The new ID is not the old one because animations
6–23 use sprite numbers above 15 (set 0 has 16 sprites); for Turtloid, Sheldon and Shark every
sprite number is in the main set, so the ID and the folder name did not change.

## 3. `helpers.json`

See `helpers-proposed.json` (`TurtloidSetAnime`, `TurtloidPartSetAnime`, the `actor2` note that
replaces "unknown"). D-1000 has no set routine of its own (inline loads before `SetAnime`).

## 4. `docs/engine.md`, "More than one sprite set"

Add after the Metal Shark sentence:

> D-1000 (overlay 02, type 48) has no set routine: each `SetAnime` call first sets `+0x3C` to
> `bank_base[k]` (animations 0–5) or `bank_base[k + 1]` (6–23). The core and the effects copy the
> table, texture and palette of the body and use set 1. `spriteexport` reads the sets from `sets`
> in `catalog/enemies.json` (README).

And in the `actor2` row or note: `spriteexport` exports a live `actor2` as `class` `actor2`.

## 5. `docs/TODO.md`

- Line "`spriteexport`: export `actor2` …": mark the first half done; keep the `rekit --name`
  `KeyError` (it is the same as the separate item below; one of the two can go).
- "`d-1000-extra` … decide whether `d-1000` or `d-1000-parts` should take it": done. Entry 16 is
  set 1 of type 48 (body, core and effects); one export `enemy-f15c898594` (`docs/enemies/d-1000.md`,
  "Sprites").
- "D-1000's sprite bank (boss area)": done. Set 0 (entry 15, 16 sprites) and set 1 (entry 16, 49
  sprites) are both in the exports of `intro-3_boss`; nothing is missing.
- The three "re-export from the next bank" items of Turtloid (`rainy-turtloid.md` open item 4),
  Sheldon (item 4) and Shark (item 5): done with the `sets` rules. Turtloid's stunned pose 33 is
  now drawn from `turtloid-7_boss` (entry 16, texture `0x540`); check it in play.
- "Exporter minors from the final review": done: the table is in the `sources` merge key; the
  folder messages say `--root`; a read past the end of RAM is exit 1. Still open: record a cut
  table ("kept k of n") in `animations.json`; a name removed from the names file leaves the folder
  under that name.
- "Free-standing effect objects (visual and misc tables) are not exported": still open. Visual and
  misc objects get only a sprite-only sheet today; an animated export needs their table field and
  a type rule for each class. Not small.
- New item: a set rule draws from video memory at its texture; the tool cannot see whether that
  texture holds the set in a state. A state where `bank_base[k + n]` holds other data would give
  wrong strips. A check (for example the entry's type in the bank list, where it has one) is open.

## 6. Process-log draft (next section number)

```
## NN. Sprite exporter: sprite sets, actor2, D-1000

Four bosses take some animations from a later entry of `sprite_bank_base`: Rainy Turtloid
(28–32 from bank + 1, 33 from bank + 2), Shield Sheldon (0–3, 35, 36), Metal Shark Player
(34–37) and D-1000 (6–23; found in this work: no set routine, an inline load before each
`SetAnime`). The exporter drew every animation from one bank, so these strips were wrong.

Design: a small table `sets` in `catalog/enemies.json` (stage, type, set, animations, texture,
palette). We chose a catalog table over reading the set call from the code: the four bosses use
three different code forms (a routine with an animation threshold, a routine with a set argument,
inline loads). Set n takes entry bank + n. The exporter draws each animation from its set; one
cell for the whole export. A live object of the type whose sprite data is a set entry is skipped
(same table). The ID is unchanged when every sprite number is in the main set (Turtloid, Sheldon,
Shark); D-1000 gets a new ID.

`actor2` (`0x800CD340`, the Ride Armor) is now a live source (`class: "actor2"`). Its export
(`enemy-e32b3dd423`, `shark-1_ride`) is byte-identical to the Ride Armor probe (31 strips).

Identity check: old code (`git archive HEAD`) and new code on all 44 dumps, one catalog copy,
two fresh roots, `diff -rq`: only the new actor2 export (folder, bank sheet, its ID in
`stages/shark.json`) differs. Then the new code with the proposed `sets`: only Turtloid (all
strips; 0–27 have the same pixels on a larger cell, 205 × 205 instead of 177 × 179), Sheldon
0–3, 35, 36, Shark 34–37, their `animations.json`, `stages/intro.json`, `stages/shark.json`, and
D-1000 (one export `enemy-f15c898594` in place of `d-1000` and `d-1000-parts`) differ.

Lesson: a wait loop with `pgrep -f <pattern>` matches its own shell when the pattern is in the
loop's command line; it never ends.
```
