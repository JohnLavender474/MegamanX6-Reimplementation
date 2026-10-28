# Doc changes for Gate stage 2, section 1 (stage 17)

New files (copy as is):

- `docs/stages/gate-2.md` = [`~/mmx-data/x6/re/gate-stage-2/area-page.md`](area-page.md)
- `docs/enemies/missile-launcher.md` = [`~/mmx-data/x6/re/gate-stage-2/missile-launcher.md`](missile-launcher.md)
- `docs/enemies/illumina-orb.md` = [`~/mmx-data/x6/re/gate-stage-2/illumina-orb.md`](illumina-orb.md)

The stage name `gate-2` is provisional (the coordinator decides the scheme for stages 17 and 18).

Catalog `learn` entries and the `texture_split`: `notes.md`, "Catalog entries".
Helper names: `helpers-proposed.json` (one entry, `0x800E9E24`, is an `alt` for the existing
`FlameJetPauseInit` line).

## `catalog/enemies.json`

Old:

```
  "stages": {"0": "intro", "1": "yammark", "2": "wolfang", "3": "heatnix", "4": "shark", "5": "scaravich", "6": "turtloid", "7": "sheldon", "8": "mijinion", "16": "gate-1", "21": "turtloid-alt", "21.0": "scaravich-alt", "22": "sheldon-alt", "22.1": "mijinion-alt", "20.1": "shark-alt"},
```

New:

```
  "stages": {"0": "intro", "1": "yammark", "2": "wolfang", "3": "heatnix", "4": "shark", "5": "scaravich", "6": "turtloid", "7": "sheldon", "8": "mijinion", "16": "gate-1", "17": "gate-2", "21": "turtloid-alt", "21.0": "scaravich-alt", "22": "sheldon-alt", "22.1": "mijinion-alt", "20.1": "shark-alt"},
```

Old:

```
    "enemy-e809e4309e": "section-goal",
```

New:

```
    "enemy-e809e4309e": "section-goal",
    "enemy-eb6483d4fb": "missile-launcher",
```

## `catalog/object-names.json`

Old:

```
            "main:29": "LavaController", "main:33": "Gate1Type33", "main:34": "Gate1Type34",
            "main:60": "NightmareMother", "logic:7": "IceSlide"},
```

New:

```
            "main:29": "LavaController", "main:33": "Gate1Type33", "main:34": "MissileLauncher",
            "main:60": "NightmareMother", "logic:7": "IceSlide"},
    "113": {"main:11": "IlluminaOrb", "main:31": "FallingRock", "main:34": "MissileLauncher",
            "main:55": "HighMax", "stage:3": "TotemPole", "stage:5": "RocketPlatform"},
```

## `catalog/helpers.json`

Old:

```
    "0x800F43A0": {"name": "Gate1Type34Init", "note": "overlay 22, main 34 state 0: HP 30, contact 4, dmg_tbl_normal", "overlay": 22},
```

New:

```
    "0x800F43A0": {"name": "MissileLauncherInitLab", "note": "overlay 22, main 34 state 0 (same code as overlay 23 0x800EA778): HP 30, contact 4, dmg_tbl_normal (docs/enemies/missile-launcher.md)", "overlay": 22},
```

Old:

```
    "0x800F4BA4": {"name": "Gate1Type34", "note": "overlay 22, main 34 entry (states 0x800FC87C): new type of Gate stage 1, not named; reads game.character; visuals 37, 38", "overlay": 22},
```

New:

```
    "0x800F4BA4": {"name": "MissileLauncherLab", "note": "overlay 22, main 34 entry (states 0x800FC87C): the missile launcher of Gate stage 1 (same 42 functions as overlay 23 0x800EAF7C); visuals 37 (shell), 38 (homing missile)", "overlay": 22},
```

## `docs/stages/gate-1.md`

Old:

```
(`docs/damage.md`, "Main type 59: Gate"); stage 17 is probably the High Max fight (boss pack 136 =
the High Max pack, `docs/enemies/high-max.md`) [likely]. The stage is a lab entrance in the snow, a
```

New:

```
(`docs/damage.md`, "Main type 59: Gate"); stage 17 ends with High Max `var` 2 (record 112;
[`gate-2.md`](gate-2.md)) and goes straight on to stage 18 [certain for the record and the code of
`StageExit`]. The stage is a lab entrance in the snow, a
```

Old:

```
| 99–101 | 0 | 34 | 0 | 3 main 34 (new, no sheet) at (4080, 600), (4336, 632), (5152, 632) |
```

New:

```
| 99–101 | 0 | 34 | 0 | 3 missile launchers ([`missile-launcher.md`](../enemies/missile-launcher.md), main 34) at (4080, 600), (4336, 632), (5152, 632) |
```

Old:

```
### Main 33 and main 34 (new types; behaviour not read in full)
```

New:

```
### Main 33 and main 34 (new types; main 33 not read in full)
```

Old:

```
- **Main 34** (entry `0x800F4BA4`, set-up `0x800F43A0`): HP 30, contact 4, `dmg_tbl_normal`;
  reads `game.character`; neighbours visual 37 and 38; uses `GetEffectObject`,
  `SpawnExplosionRand` and `DropItem`. Three records, all on the corridor floor.
```

New:

```
- **Main 34** (entry `0x800F4BA4`, set-up `0x800F43A0`): the missile launcher
  ([`missile-launcher.md`](../enemies/missile-launcher.md)); the same 42 functions as main 34 of
  overlay 23 (Gate stage 2) [certain for the code match]. Three records, all on the corridor floor.
```

Old:

```
(1440, 120), (1916, 0). A sixth pointer (`0x80072DB0`, player (152, 1904)) is past the map; no
marker sets checkpoint 5 [guess: data of the next entry].
```

New:

```
(1440, 120), (1916, 0). A sixth pointer (`0x80072DB0`, player (152, 1904)) is past the map; no
marker sets checkpoint 5. It is checkpoint 0 of stage 17 (`docs/stages/gate-2.md`) [certain].
```

Old:

```
own sprite source is in `docs/enemies/nightmare-mother.md`. A save state at checkpoint 0–3 is
needed for the enemy sprites.
```

New:

```
own sprite source is in `docs/enemies/nightmare-mother.md`. A save state at checkpoint 0–3 is
needed for the enemy sprites. `gate-3_stage-2` (stage 17) gives the main 34 sheet
(`missile-launcher`, `enemy-eb6483d4fb`) and the main 15 sheets; the other types are not in the
stage 17 type list.
```

Old:

```
1. The official names of main 33 and main 34, and what they do (only the set-up is read).
```

New:

```
1. The official name of main 33 and what it does (only the set-up is read); the official name of
   the missile launcher (main 34).
```

## `docs/enemies/high-max.md`

Old:

```
  136 (`boss_pack_tbl[17]`) has the same bytes [`var` 2 in stage 17 is a guess]. Member 124 (the
```

New:

```
  136 (`boss_pack_tbl[17]`) has the same bytes (the `var` 2 fight of stage 17,
  `docs/stages/gate-2.md`). Member 124 (the
```

Old:

```
3. `var` 2: where it is made (stage 12, or stage 17: `boss_pack_tbl[17]` = 136, the High Max pack),
   steps 13–17, objects `var` 9–11, and the maker of `var` 2 shots.
```

New:

```
3. `var` 2: record 112 of stage 17 (main 55 `id` 2 at (3312, 336); overlay 23 holds the same 158
   functions; `docs/stages/gate-2.md`) [certain]. Open: steps 13–17, objects `var` 9–11, and the
   maker of `var` 2 shots (a save state of the stage 17 fight is needed).
```

## `docs/engine.md`

Old:

```
stage 18 is Gate (`docs/damage.md`). Overlay 22 runs up to `0x800FD5A8`, over `0x800FA000`: Nightmare
```

New:

```
stage 18 is Gate (`docs/damage.md`). Stage 17 is section 1 of Gate stage 2 (`docs/stages/gate-2.md`):
overlay 23 holds High Max's code (main 55, the same functions as overlay 44) and runs up to
`0x800F9BEC`; `learn` finds the layout in overlay 23 in `gate-3_stage-2`. `StageExit` sends a
stage 17 clear straight to stage 18 (game mode 8), and a stage 16 or 18 clear to game mode 19
(scenes 8 and 9) [certain for the code]. Overlay 22 runs up to `0x800FD5A8`, over `0x800FA000`: Nightmare
```

## `docs/enemies/boss-gate.md`

Old:

```
| Gate stage 1 | id 39, x 7664: camera fixed at (7664, 0); lowers BG layer 1 (the lava) | id 26, x 7984: camera x 7984–8096, max y 160 |
```

New:

```
| Gate stage 1 | id 39, x 7664: camera fixed at (7664, 0); lowers BG layer 1 (the lava) | id 26, x 7984: camera x 7984–8096, max y 160 |
| Gate stage 2 (stage 17) | id 28, x 2896: camera fixed at x 2880 | id 29, x 3184: camera x 3184–3216, y 224 (High Max) |
```

## `docs/enemies/falling-rock.md`

Old:

```
--type 31`. Units, tags and shared rules: `docs/engine.md`. Records: `docs/stages/scaravich.md`.
```

New:

```
--type 31`. Units, tags and shared rules: `docs/engine.md`. Records: `docs/stages/scaravich.md`.
Overlay 23 (Gate stage 2, stage 17) has the same 10 functions after masking (entry `0x800EA3A0`,
set-up `0x800E9E24`), sprite type 133, and 46 records (`docs/stages/gate-2.md`); one rock is live
in `gate-3_stage-2` (`var` 2, step 0) [certain].
```

## `docs/enemies/totem-pole.md`

Old:

```
No record makes the base. The section goal (main 12) makes stage object 3 `var` 0 at the place of
```

New:

```
In stage 5 no record makes the base. (In Gate stage 2, overlay 23, entry `0x800EFA94`, the same 28
functions after masking, six records make the base directly: kind 4, `b` 3, `id` 0, `a` 48;
`docs/stages/gate-2.md`.) The section goal (main 12) makes stage object 3 `var` 0 at the place of
```

## `docs/enemies/rocket-platform.md`

Old:

```
Units, confidence tags and the shared engine rules: `docs/engine.md`. Records:
`docs/stages/turtloid.md`.
```

New:

```
Units, confidence tags and the shared engine rules: `docs/engine.md`. Records:
`docs/stages/turtloid.md`. Overlay 23 (Gate stage 2, stage 17) has the same 5 functions after
masking (entry `0x800E9B8C`, sprite type 131) and 5 records (`docs/stages/gate-2.md`).
```

## `docs/enemies/illumina.md`

Old:

```
3. Main 8 (jets) and main 11 (orbs) are not read beyond their spawns; their names are not known.
```

New:

```
3. Main 8 (jets) is not read beyond its spawn. Main 11 (orbs): `docs/enemies/illumina-orb.md`
   (overlay 23 has the same code). The names are not known.
```
