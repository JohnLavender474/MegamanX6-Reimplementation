# Proposed doc changes (Illumina, stage 8)

## 1. New file `docs/enemies/illumina.md`

The text of [`~/mmx-data/x6/re/mijinion-bot/sheet.md`](sheet.md), as it is.

## 2. `docs/engine.md`, "Object classes", after the paragraph "**A boss on a BG layer.** …"

Add:

```
**A boss in screen space.** Illumina (overlay 11, main 24) and all its pieces have `layer`
(`+0x14`) = −1. `DrawObject4` (`0x800232D4`) then draws `x`, `y` as screen coordinates;
`BoxOverlap` (`0x8002D654`) and `WeaponBoxTest` (`0x8002D868`) add the layer 0 camera x and y to
an object with `layer` < 0, so its boxes still meet world objects. Illumina's weak spot is a lens
on a cable drawn in BG layer 1 tiles; its screen box covers the lens only while it locks the camera
(`docs/enemies/illumina.md`) [certain for the code].
```

## 3. `docs/engine.md`, "Nightmare Effect", the value 3 item

Replace

```
3 main type 19 of stage 2 (set-up `0x800EEEA0`, a fixed-list record, `id` 1: a spawner;
    `var` 0 is an orange fire ring, export `enemy-78c8f556f5`, HP 8, contact 4, damage table
    `0x8007465C`, 3 px/update down, sound (2, 80); the spawner works only in zones by stage and
    checkpoint; not read further),
```

with

```
3 main type 19 of stage 2 (set-up `0x800EEEA0`) and of stage 8 (overlay 11, set-up `0x800ECF30`,
    entry `0x800ED624`), a fixed-list record, `id` 1: a spawner; `var` 0 is an orange fire ball,
    export `enemy-78c8f556f5`, HP 8, contact 4, damage table `0x8007465C`, 3 px/update down,
    sound (2, 80), drop table 18. The spawner (`0x800ED1F0` in overlay 11) works only in zones:
    stage 2 player x 4641–5039, y 225–1263 at checkpoint 2, and x 4993–5679, y 1281–1695 at
    checkpoint 3; stage 8 player x 2513–4223 (between the two Illumina fights). In a zone:
    `Rumble(8)` once, every 20, 30 or 40 updates 0–2 balls at the camera top (x + (0–3) × 80 +
    (0–2) × 32, y − (0–4) × 16), and a camera y shake of ±2 (±6 at random) every second update
    [certain for the code],
```

## 4. `docs/engine.md`, "The damage tables", after `dmg_tbl_nightmare_insect`

Add:

```
`dmg_tbl_illumina` (`0x8007483C`): Illumina, its drones and its wall cannons (stage 8).

| ids | damage |
|---|---|
| 0–15 | 1 2 2 3 3 3 3 – D 1 6 6 6 6 8 6 |
| 16–31 | 8 D 5 5 5 D 8 D 2 2 5 4 4 4 2 2 |
| 32–47 | 2 2 4 4 4 2 2 2 2 2 4 2 D 1 4 8 |
| 48–63 | 2 D 4 4 1 8 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |
```

## 5. `docs/engine.md`, "Weapon ids", the stun ids row

After "Blizzard Wolfang: 3, 12, 34 (slot 3 stuns him)." add:

```
Illumina (stage 8): 6, 15, 42 cancel its attack and clear its parts (step 10).
```

## 6. `docs/engine.md`, "Camera", the mode 5 users list

Add:

```
    Stage 8 (overlay 11): stage logic 5 (`TunnelBgAnimator`, `0x800FB13C`, a fixed-list record)
    sets layer 2 mode 5 and layer 2 y = frame × 256 (frames 0–2, one per 16 updates): an
    animated tunnel picture fixed on the screen [certain].
```

## 7. `catalog/enemies.json`

- `sets`: add (after the stage 7 rule):

```
    {"stage": 8, "type": 24, "set": 1, "animations": [32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63], "texture": "0x0530", "palette": 1},
```

- `names`: add `"enemy-ef4d46bae1": "illumina"` (the ID with the rule above; tested in
  `sprites-root`).

## 8. `docs/stages/` (no page for stage 8 yet)

When a stage 8 page is made: record 51 (main 24 `id` 0 at (160, 128)) is Illumina; the fixed list
holds stage logic 5 (`TunnelBgAnimator`) and main 19 `id` 1 (the Nightmare Effect 3 spawner).
Camera zones 11/12 (x 2016–2047) and 13/14 (x 4192–4223) set min y at the two fight places; the
fights lock the camera at (2048, 384) and (4224, 768) themselves.

## 9. `docs/process/process-log.md`

```
- Illumina (stage 8, overlay 11, main 24): read with the RE kit from `mijinion-1_start` and
  `mijinion-2_illumina-helpers`. A screen-space robot (layer −1) with a lens on two BG layer 1
  cables; two fights of 64 HP; drones with a scanning head; walls with laser cannons. New
  `dmg_tbl_illumina`. Main 19 (Nightmare Effect 3) zones read for stages 2 and 8.
```
