# Three more Maverick stages: plan

**Goal:** add the stages of Blizzard Wolfang, Ground Scaravich and Shield Sheldon, each playable from
the start to the Maverick fight. Each gap gets a `TODO` in the code, an entry in `docs/gaps.md`,
and a point object in a `TODO` layer of the stage's TMX copy in `assets/tmx/` (the user's request;
all new maps do the same).

**Sources:** `docs/stages/scaravich.md` (with the boss), `docs/stages/sheldon.md`; no stage doc for
Wolfang (its values come from the executable, the save states and `~/mmx-data/x6/re/wolfang-*`,
`re/blizzard-wolfang`). Sheets in `docs/enemies/`. TMX: `m096-blizzard-wolfang-section-1`,
`m100-ground-scaravich`, `m102-shield-sheldon-section-1` (texture splits are already in the tools
catalog).

## Facts read for the plumbing (2026-10-25)

- `checkpoint_start_tbl` (`0x80073300`): Wolfang 6 records, Scaravich 3, Sheldon 4. Read with a
  scratch script and checked against the known Turtloid list.
- `stage_music`: Wolfang 13, Scaravich 15, Sheldon 17; boss 22.
- Bank 2 / 5 from the save states: Wolfang `m096-e05` / `m096-e03`, boss `m076-e02`; Scaravich
  `m100-e05`, boss `m079-e02`; Sheldon `m102-e05`, boss `m081-e02`.
- Nightmare Effect in the save states: Wolfang 7, Scaravich 4, Sheldon 1. Used as the stage value.

## Tasks

1. Plumbing: `StageDef`s, TMX copies with `TODO` layers, factories, camera layer settings (Sheldon's
   background switch at checkpoint 1).
2. Wolfang: Wolfloid, ice blocks, ice stack block, ice fall controller, ice slide (stage logic 7,
   slopes), Nightmare Shadow, Blizzard Wolfang.
3. Scaravich: teleporter poles with the section roll and the random blocks, section goals, totem pole
   fight, falling rocks, rock blocks, Ground Scaravich and his ball.
4. Sheldon: laser reflector puzzle with the laser gates, hidden glass floors and the glass slide,
   Guardian, Mettaur D2, Nightmare Bug, Shield Sheldon.
5. Tests per stage (boss intro and death, key objects), smoke run, screenshots, `docs/gaps.md`,
   README, `CLAUDE.md`.

## Progress

- [x] 1 plumbing
- [x] 2 Wolfang
- [x] 3 Scaravich
- [x] 4 Sheldon
- [x] 5 checks and docs
