# Doc changes for Gate stage 2, section 2 (stage 18) and Gate

New files (copy as is):

- `docs/stages/gate-2b.md` = [`~/mmx-data/x6/re/gate-stage-3/area-page.md`](area-page.md)
- `docs/enemies/gate.md` = [`~/mmx-data/x6/re/gate-stage-3/gate-sheet.md`](gate-sheet.md)

Catalog entries that `learn` adds, the `texture_split`, and the commands: `notes.md`, "Catalog
entries". Helper names: `helpers-proposed.json`.

## `catalog/enemies.json`

Old:

```
  "stages": {"0": "intro", "1": "yammark", "2": "wolfang", "3": "heatnix", "4": "shark", "5": "scaravich", "6": "turtloid", "7": "sheldon", "8": "mijinion", "16": "gate-1", "17": "gate-2", "21": "turtloid-alt", "21.0": "scaravich-alt", "22": "sheldon-alt", "22.1": "mijinion-alt", "20.1": "shark-alt"},
```

New:

```
  "stages": {"0": "intro", "1": "yammark", "2": "wolfang", "3": "heatnix", "4": "shark", "5": "scaravich", "6": "turtloid", "7": "sheldon", "8": "mijinion", "16": "gate-1", "17": "gate-2", "18": "gate-2b", "21": "turtloid-alt", "21.0": "scaravich-alt", "22": "sheldon-alt", "22.1": "mijinion-alt", "20.1": "shark-alt"},
```

Old:

```
    "enemy-d69cdcfc32": "portrait-alia",
```

New:

```
    "enemy-d69cdcfc32": "portrait-alia",
    "enemy-db764876a4": "gate",
```

Old:

```
    {"stage": 4, "type": 56, "set": 1, "animations": [34, 35, 36, 37], "texture": "0x0540", "palette": 1},
```

New:

```
    {"stage": 4, "type": 56, "set": 1, "animations": [34, 35, 36, 37], "texture": "0x0540", "palette": 1},
    {"stage": 18, "type": 59, "set": 1, "animations": [31, 32, 33], "texture": "0x0540", "palette": 1},
```

## `catalog/object-names.json`, `members`

Old:

```
            "main:55": "HighMax", "stage:3": "TotemPole", "stage:5": "RocketPlatform"},
```

New:

```
            "main:55": "HighMax", "stage:3": "TotemPole", "stage:5": "RocketPlatform"},
    "114": {"main:2": "MetaWheelF", "main:8": "IlluminaJet", "main:9": "Monbando", "main:14": "WeatherAnalyze",
            "main:28": "TeleporterCrystal", "main:59": "Gate", "stage:9": "PressBlock", "stage:13": "HealthCapsule",
            "logic:17": "AcidRain", "kind7:2": "PressUpdate"},
```

## `docs/engine.md`

Old:

```
stage 18 is Gate (`docs/damage.md`). Stage 17 is section 1 of Gate stage 2 (`docs/stages/gate-2.md`):
```

New:

```
stage 18 is section 2 of Gate stage 2 and Gate's room (`docs/stages/gate-2b.md`; Gate's code, main
59, is in overlay 24, which ends at `0x800F780A`; no boss overlay; `docs/enemies/gate.md`). Stage 17
is section 1 of Gate stage 2 (`docs/stages/gate-2.md`):
```

Old:

```
`dmg_tbl_nightmare_mother` (`0x800757DC`, the eyes of Nightmare Mother,
```

New:

```
`dmg_tbl_gate` (`0x8007573C`, Gate and all his orbs, fragments and shots,
`docs/enemies/gate.md`; a weapon hit never costs Gate HP, `docs/damage.md`, section 7):

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 2 2 2 – 2 1 2 2 2 2 2 2 |
| 16–31 | 2 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 2 2 2 2 2 2 2 2 2 8 1 4 8 |
| 48–63 | 2 1 2 2 1 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_nightmare_mother` (`0x800757DC`, the eyes of Nightmare Mother,
```

## `docs/damage.md`

Old:

```
- It is Gate [likely]: the set-up `0x800EF400` puts it at (3456, 912), with damage table
  `0x8007573C`, `game.boss` = itself, `boss_hp_max` = 48 + a table byte, and `PlayerEvent(20,
  64)`. Its step `0x800F4B2C` opens message 1 of the stage 17/18 text bank (members 225, 226):
  Gate's talk.
- A weapon hit does not cost Gate HP [certain for the code]. `ApplyWeaponHit` lowers the HP and
  deflects the shot. Then Gate's main state `0x800EF590` writes the old HP back and puts the
  damage (old − new) in byte `0x800F7806`.
- Visual type 26 of overlay 24 (`0x800F0A24`, 35 kinds by `var`) reads that byte (`0x800F0DFC`,
  `0x800F1584`, `0x800F1D2C`, `0x800F2678`, `0x800F2E04`), adds it to its own `+0x98`, and
  changes step at 16 or more. Visual 26 code sets `0x800F77DC` (`0x800F3778`). While it is not 0,
  Gate's HP goes down by 4 per update; at HP ≤ 0 Gate goes to its death. What each `var` of
  visual 26 is needs a stage 18 save state [open].
```

New:

```
- It is Gate [certain: `gate-5_gate-fight` has main 59 live in Gate's room after his talk]: the
  set-up `0x800EF400` puts it at (3456, 912), with damage table `dmg_tbl_gate` (`0x8007573C`),
  `game.boss` = itself, `boss_hp_max` = 48 + {4, 6, 8, 10}[boss level], and `PlayerEvent(20,
  64)`. Its step `0x800F4B2C` opens message 1 of the stage 17/18 text bank (members 225, 226):
  Gate's talk. The full sheet is `docs/enemies/gate.md`.
- A weapon hit does not cost Gate HP [certain for the code]. `ApplyWeaponHit` lowers the HP and
  deflects the shot. Then Gate's main state `0x800EF590` writes the old HP back and puts the
  damage (old − new) in byte `0x800F7806` (`gate_damage`).
- Visual type 26 of overlay 24 (`0x800F0A24`) holds Gate's parts. The five orbs (`var` 0–4) add
  `gate_damage` to their `+0x98`; at 16 or more the orb vanishes without fragments. A killed orb
  makes six fragments (`var` 5–10); a fragment within 24 px (x) and 32 px (y) of Gate, while Gate
  is not in step 7, sets `gate_hit` (`0x800F77DC`, at `0x800F3778`). Gate's main state then
  takes 4 HP once and goes to step 8 (hurt) for 90 updates, which clears the flag. So Gate dies
  after 13, 14, 14 or 15 fragment hits (boss level 0–3) [certain for the code].
```

Old:

```
2. (answered: Gate, stage 18; section 7) Main type 59. Still open: the `var`s of visual type 26.
```

New:

```
2. (answered: Gate, stage 18; section 7 and `docs/enemies/gate.md`) Main type 59 and the `var`s
   of visual type 26.
```

## `docs/enemies/boss-gate.md`

Old:

```
   3.84375 px/frame. Id 30 writes Y−48 to the max x; this is what the code does (maybe an error in
   the game) [certain for the code].
```

New:

```
   3.84375 px/frame. Id 30 writes Y−48 to the max x; this is what the code does (maybe an error in
   the game) [certain for the code]. Id 30 is the first gate of Gate's corridor (stage 18, x 2928,
   y 1072): in `gate-4_before-gate` the target max x is 1024 and the current max x 2878, below the
   min x 2880, and the camera stays at x 2880 (`docs/stages/gate-2b.md`) [certain for the values].
```

Old:

```
   answered: Nightmare Snake); in which stages ids 7,
   18 and 31 are used. Ids 25 and 39 are gates of Gate stage 1 (stage 16, records 103 and 104):
```

New:

```
   answered: Nightmare Snake); in which stages ids 7 and
   18 are used. Id 31 is the second gate of Gate's room (stage 18, record 63, x 3184); Gate's
   intro undoes its FADE with the reverse steps (`docs/enemies/gate.md`, intro sub-step 5), so
   the FADE darkens the room before Gate's talk [likely]. Ids 25 and 39 are gates of Gate stage 1 (stage 16, records 103 and 104):
```

## `docs/enemies/teleport-crystal.md`

Old:

```
2. Which stage uses `id` 2–4 and the destination table; it is the same in overlays 03 and 09. Both
   known records (Yammark 34, Turtloid 133) have `id` 1 (area change).
```

New:

```
2. (answered) `id` 2–4 are used in stage 18 (section 2 of Gate stage 2, overlay 24, records
   57–59; `docs/stages/gate-2b.md`). Overlay 24 has the same destination table at `0x800F6E80`,
   and its four entries are the places of that stage: `id` 2 at the end of the top corridor sends
   X to checkpoint 1 and Zero to checkpoint 2, `id` 3 and `id` 4 end those paths at checkpoint 3
   [certain for the table and the records].
```

## `docs/enemies/weather-analyze.md`

Old:

```
records use 0–3. All machines share the one global, so only one machine can work at a time
```

New:

```
records use 0–3. Stage 18 (section 2 of Gate stage 2, overlay 24, the same tables) uses rows 5 and
6 (records `id` `0x15`, `0x16`; `docs/stages/gate-2b.md`). All machines share the one global, so only one machine can work at a time
```

## `docs/stages/gate-2.md`

Old:

```
  `game+0x66` = 5. So stages 17 and 18 play as one stage with two sections, and only stages 16 and
```

New:

```
  `game+0x66` = 5. So stages 17 and 18 play as one stage with two sections (section 2:
  [`gate-2b.md`](gate-2b.md)), and only stages 16 and
```
