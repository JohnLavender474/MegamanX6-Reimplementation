# Three Maverick stages: plan

**Goal:** add the stages of Commander Yammark, Rainy Turtloid and Blaze Heatnix, each playable from
the start to the Maverick fight, next to the intro. The user asked for the whole stage through the
boss, with `TODO`s (code, TMX, `docs/gaps.md`) for each missing piece, and for autonomous work.

**Sources:** `~/IdeaProjects/mmx-demake-tools/docs/stages/{yammark,turtloid,heatnix}.md` and the
sheets in `docs/enemies/`. TMX exports: `~/mmx-data/x6/tmx/m095-…`, `m101-…`, `m097-…` (already
made by the tools; copied to `assets/tmx/` for notes, as the intro).

## Decisions (my calls)

- A stage is a `StageDef`: the TMX folder and file, the checkpoints, the music, the sound banks
  (bank 2 and 5 folders), the Nightmare Effect, and the entity factory. `Stages` lists the four.
- A stage select screen at start (W/S, J or Enter). `-Dmmx.stage=<key>` and `make run STAGE=<key>`
  skip it. STAGE CLEAR goes back to the select screen.
- BG layer scroll comes from the TMX `x6_mode` and `x6_offset_*` (engine.md "BG layer modes").
  Mode 5 layers are moved by objects (the rain curtain, the snake ring) and are hidden otherwise.
- Nightmare Effect per stage: Yammark 0, Turtloid 0, Heatnix 4 (the value in all Heatnix save
  states; it makes the grey blocks and removes the Nightmare Bugs).
- Shared progress for the session (`Progress`): lives, souls, Reploid status, armour parts.
  Lives and parts are kept and shown, but have no further effect yet.
- Moving solids carry the player (rocket platforms, Caterpirade): `Solid` reports its move.
- Items (main type 47), talk zones, palette cycles, Nightmare Effects 6/7/8 (rain, shadows,
  darkness): out of scope, listed in `docs/gaps.md`.
- Teleporter crystals: drawn, no area change (Turtloid's alternate section and its random boss are
  out of scope; the Yammark and Heatnix destinations are not known).

## Missing pieces (call out to the user)

- Blaze Heatnix (main type 52): no sheet, no sprite export (not loaded in any save state). His room
  gets a stand-in boss with a `TODO`.
- Heatnix texture swap at camera zone 1: the TMX tiles are already split by region.

## Tasks

1. Stage plumbing: `StageDef`, `Stages`, per-stage TMX, tiles, sound banks, music, checkpoints,
   layer modes, stage select, `Progress`, generic boss gate table and open conditions, solid
   carry. Tests: loader for each TMX, gate table, layer modes.
2. Shared objects: Rescue Reploid (+ infected form and shot), Nightmare Virus (+ orb, soul),
   Light capsule, Teleporter crystal (static), Health capsule.
3. Yammark: Worm Spider (+ shot), Killer Moth (+ powder), Nightmare Insect (+ blade, hatchling),
   Caterpirade, Commander Yammark (+ drones, shots, bombs). Checkpoints, camera zones.
4. Turtloid: Monbando, Sea Attacker ST, Batton Bone B81, Weather Analyze (+ sensors, acid rain,
   rain curtain), Rocket platform, Rainy Turtloid. Checkpoints.
5. Heatnix: Flame jet, Grey block, Nightmare Snake (all fights), Blaze Heatnix stand-in. Gates
   with the snake flag. Checkpoints.
6. Play each stage with scripted runs and screenshots; update `docs/gaps.md` and the README.

## Progress

- [x] 1 plumbing
- [x] 2 shared objects (health capsule moves to Turtloid)
- [x] 3 Yammark (TMX regenerated with a texture split into assets/tmx)
- [x] 4 Turtloid (TMX regenerated with a texture split into assets/tmx)
- [x] 5 Heatnix (Blaze Heatnix is a stand-in: no data)
- [x] 6 play check and docs
