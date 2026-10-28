# Blaze Heatnix's stage, start (`heatnix-1_start`): working notes

Stage 3, area 0, overlay 05. User: "Zero spawned into Blaze Heatnix's stage". Second state used for
the map and the objects only: `heatnix-2_mini-boss` (the mini boss itself is in
[`~/mmx-data/x6/re/heatnix-mini-boss/`](../heatnix-mini-boss/), not here).

## Files

| file | what |
|---|---|
| `stage-page.md` | the proposed `docs/stages/heatnix.md` |
| `sheet-flame-jet.md` | the proposed `docs/enemies/flame-jet.md` (main type 36) |
| `pseudo.c` | reconstructed C of the flame jet (plausible, not original, not compiled), with hazard comments |
| `doc-changes.md` | exact text for the other docs and the catalogs |
| `helpers-proposed.json` | 25 functions and 4 globals for `catalog/helpers.json` (overlay 5) |
| `kit-main-36/` | `rekit --name flame-jet --state heatnix-1_start.state.gz --type 36 --sprites sprites-root --out kit-main-36` |
| `kit-stage-12/` | `rekit --state heatnix-1_start.state.gz --type 12 --class stage` (grey block) |
| `kit-main-22/` | `rekit --state heatnix-1_start.state.gz --type 22` (Nightmare Bug) |
| `kit-main-28/` | `rekit --state heatnix-1_start.state.gz --type 28` (teleporter crystal) |
| `sprites-root/` | `spriteexport` and `playerexport` of `heatnix-1_start` (private root) |
| `scripts/live.py` | game state, type list, banks and all live objects of every class |
| `scripts/tables.py` | the update-table entries that point into the overlays |
| `scripts/scanimm.py` | loads and stores with a given 16-bit offset (used for `nightmare_effect`, `0xD30A`) |
| `scripts/cmpranges.py` | compares two code ranges of two states with immediates masked |
| `scripts/stage_tables.py` | checkpoint respawn records and the camera zone table of the state's stage |
| `scripts/texpages.py` | each 4 bpp page of the two texture blocks of member 97 against VRAM |
| `scripts/split_preview.py` | composites a `texture_split` from two `stage.ora` drawings |
| `scripts/record_table.py` | the Markdown record table of the stage page |

Run the scripts from the repo root with `PYTHONPATH=.`, except `split_preview.py` and
`record_table.py` (plain Python with Pillow).

## What is live in `heatnix-1_start` [certain]

```
stage 3 area 0 checkpoint 0 exit 0x0 chara 1 form 5 difficulty 1 lives 5 time 664 kills 0 armor_parts 0xc
nightmare_effect 4 stage_effect [0, 6, 3, 4, 2, 4, 7, 5, 3, ...] effect_new [2, 8, 8]
player x 80 y 394 chara 1 state 1 step 2 hp 34
main[0]  type 25 var 5 st 0.0.0 (448, 512)  rec 0x800F36D8 (second list record 0)
main[1]  type 25 var 4 st 0.0.0 (1248, 2624) rec 0x800F36E0 (second list record 1)
stage[0] type 8 var 7 st 1.0.0 (944, 608) rec 0x800F36E8 (boss gate, second list record 2)
logic[0] type 14 var 129 (80, 256); logic[1] type 14 var 130 (304, 256)  (talk zones)
logic[2] type 27 (ReadyControl, main program)
```

No visual, misc, effect, weapon, ray, kind-7 or `actor2` object is live. The player HP is 34.

Type list (`0x800733C4` word 6): `128 15 141 36 22 142 25 28 0 0 0 0 0 0 52 52`. Banks 8–13 (type 0)
and 14–15 (type 52) have no texture; banks 8–14 share one sprite data pointer (`0x8014CD58`).
`sprite_bank_base` has 16 entries (no extra source).

## What is new

| class, type | object | status |
|---|---|---|
| main 36 | flame jet (18 records) | **new**: sheet written, export `enemy-7f9bbc9a47` |
| main 25 | mini boss (5 records + 2 second-list records) | new; other work folder |
| main 52 | Blaze Heatnix [likely] (1 record) | new; not loaded, not read |
| visual 29, visual 18 | main 25's shot; near the boss code | new; not read here |
| stage 12 | grey block (15 records) | known: same code as overlays 08/29 |
| main 22 | Nightmare Bug (19 records) | known: same code as overlay 10 |
| main 28 | teleporter crystal (1 record) | known: same code as overlay 03 |
| main 15, stage 4, 8, 26, main 47 | Nightmare Virus, Rescue Reploid, boss gate, Light capsule, item | shared main-program code |

So only one new enemy type of the stage, other than the mini boss and the boss: the flame jet.
The task allowed three sheets; there was no second or third candidate near the start.

Update-table entries into overlay 05 (`scripts/tables.py`): main 22, 25, 28, 36, 52; visual 18, 29;
effect 7, 13, 15; logic 1, 29; misc 1, 29, 56; stage 12. Effect 23, 25, 26 point into the player
overlay. Logic 1 (`0x800E9AE0`), logic 29 (`0x800EA2EC`), misc 1 (`0x800EA02C`), misc 29
(`0x800E9E98`), misc 56 (`0x800ECBAC`), effect 7 (`0x800E992C`), 13 (`0x800E9970`) and 15
(`0x800E9AD0`) are inside functions of main 36, 22 and 25 (from the kits' function ranges) [certain
for the addresses]. I did not search for code that makes these types; they are probably stale
entries [likely].

## Code comparisons [certain]

`scripts/cmpranges.py` (I-type immediates and jump targets masked):

| object | here | other | words | result |
|---|---|---|---|---|
| stage 12 | `0x800EA5C8` | `scaravich-9_high-max` `0x800EA834` (overlay 29) | 344 | all equal (333 raw) |
| main 22 | `0x800EA07C` | `sheldon-5_nightmare-hidden` `0x800F3F54` (overlay 10) | 339 | all equal (319 raw) |
| main 28 | `0x800EDB90` | `yammark-1_start` `0x800EE5CC` (overlay 03) | 846 | all equal (816 raw) |

Data tables of these objects were not compared byte for byte; the grey block boxes and animation
read from `kit-stage-12/data.txt` match `docs/enemies/grey-block.md`.

## Sprite export (private root) [certain for the files; by eye for the looks]

`spriteexport --state heatnix-1_start.state.gz --root sprites-root`: 8 banks drawn, 8 without
texture, 7 exports (2 live, 5 search), 2 skipped (types 0 and 52: no texture).

| export | status |
|---|---|
| `boss-gate`, `nightmare-bug`, `nightmare-virus`, `nightmare-virus-orb`, `teleport-crystal` | the same IDs as the shared exports; every PNG equal byte for byte |
| `enemy-7f9bbc9a47` (main 36) | **new, good**: 5 animations, flames grow, burn and die, up (0, 1) and down (2, 3), 4 off. Proposed name `flame-jet` |
| `enemy-6e15315f49` (main 25) | **new**: the mini boss; left to the other work folder (13 animations, live source). Not judged here |

`playerexport`: `players/zero` (member 85): 200 PNGs, all equal to the shared
`sprites/players/zero/`; the same skip (animation 118, sprite 222 > 113). Nothing new.

Bank sheets checked by eye: bank 3 (flames, 28 sprites), bank 4 (bugs, 6), bank 5 (grey block
heights, 8; byte-equal to Turtloid `bank-11.png`), bank 6 (main 25: green orbs, fire, sparks,
blue rings). The export wrote `banks/stage-3/` and `stages/stage-3.json`: at the time of my run
`catalog/enemies.json` had no `stages` name for 3; the repo now maps 3 → `heatnix`, so the shared
run will use `heatnix`.

No wrong sheet was found.

## The texture question [certain for VRAM; by eye for the regions]

- `scripts/texpages.py`: in both states VRAM (320, 256) holds all 8 pages of entry `0x10000`;
  entry `0x16` matches page 0 (256 rows), page 1 (16 rows) and page 7 (192 rows) only.
- `stage_texture_swap` (`0x8008EAB0`) = 0 in both states.
- Camera zone 1 (record 8, `id` 129) is the swap trigger: x 1376–1391, y 2720–2799.
- Drawing with block `0x16`: a scratch catalog entry with `"texture": 22` and
  `python3 -m mmxdemake.stages --out <scratch> --catalog <scratch>` (scratchpad, not kept).
- Layer 0, x ≥ 1280 from y 1536 down, and all of x ≥ 2816: clean with `0x16`, broken with
  `0x10000`. Pixel difference of layer 0 between the two drawings: tower (x 896–1280) 50 %; side
  corridor 11 % (it looks right only with `0x16`).
- The purple screens: layer 1 screens 111–138 and layer 2 screens 167–203. Garbage with
  `0x10000`, an even pattern with `0x16`. Not layer 0.
- `split_preview.py` with the proposed regions: layer 0 is clean everywhere.

## Nightmare Effect [certain for the values and the reads]

- `scripts/scanimm.py heatnix-1_start.state.gz d30a 800e9860 800fa000`: two reads in overlay 05,
  `0x800EA10C` (main 22 set-up, `== 1`) and `0x800EA5D8` (stage 12 set-up, `== 4`).
- Value 4 → the grey blocks exist; value 1 would make the bugs exist.
- The state bytes agree with the pick rule of `docs/engine.md`: Shark pick (`3 5 8`) wrote 4 to
  3, 5, 8; the later Heatnix pick (`2 8 8`) wrote 3 to 2 and 8.

## Flame jet: key facts and where they come from

- Set-up `0x800E9CF8`: `addiu v0,zero,12` → `hp`; `addiu a3,zero,16` → `contact_damage`;
  `sw zero,84(a0)` (hurt box 0); `current_main+0x80` = −8, `+0x82` = 16.
- Main `0x800E98C0`: `hurt_box` = 0 on both paths; `attack_box` = `current_main + 128` unless
  sprite 13 or 27; `ContactWeapon` returns 2 or 55 → `+0x84` = 1; < 0 → `+0x84` = 1.
- `WeaponBoxTest` (`0x8002D868`): `lw t3,84(t6)` / `beq t3,zero` → no hit with a null hurt box. So
  the `+0x84` path is dead (`rekit --at 0x8002D868`, `--at 0x80031474`).
- Heights `0x800F3CD0`: `10 28 38 48 58 58 58 58 48 38 18 08 08 01` twice.
- Timers: `addiu a0,zero,180` / `addiu v0,zero,14`; pause `+0x7E + 60` (`addiu a0,a0,60`),
  `+0x7E` = 200 (`addiu v0,zero,200`) on a put-out.
- Burst count: `andi v0,v0,0xf`; `slti v0,v0,8`; `GetRNG` `andi 3` + 1.
- Sound: `PlaySound(2, 22)` (`a0` 2, `a1` 22) in sub 0 and sub 3.

### Hazards

1. **Dead weapon path.** The object calls `ContactWeapon` and reacts to ids 1 and 54 and to a
   kill, but its hurt box is always null, so the engine never reports a hit. Safe because the
   box is never set. A reimplementation should leave the weapon test out, or decide on purpose to
   give the flame a hurt box (then ids 1 and 54 would put it out for 200 extra updates).
2. **Table indexed by the sprite.** `flame_jet_height_tbl[anime.sprite]` has 28 entries; the
   animation table uses sprites 0–27. Safe because of the animation data. A new animation with a
   higher sprite reads past the table.
3. **Unsigned burst count.** `+0x85` is decremented before the test for 0. It is always ≥ 1 when
   sub 3 runs (sub 0 and the put-out path write 1 or more). A 0 would wrap to 255 bursts.
4. **`current_main`.** The box and the flags are written through the scratchpad pointer of the
   object being updated, not through `self`. Safe because the main loop sets `current_main` =
   `self` before each update. A reimplementation should use `self`.

No recursion: no function calls itself, directly or through a helper (`kit-main-36/calls.txt`).

## Not done / could not find

- The mini boss (main 25, visual 29): other work folder.
- The boss (main 52, visual 18): no texture in the start state; not read.
- What sets `game+0x26` in this stage (6 gates need it).
- The destination of the teleporter crystal (`id` 1).
- Item kind 2 (record 110).
- The colours and the screen position of the BG screens after the texture swap.
- The official name of the flame jet.
