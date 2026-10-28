# TODO

## Collision legend (name each collision value)

Status: done for the code (2026-10-22, helper agent). Every value 0–63 has a meaning from the
collision jump tables, or has no handler. The rules are in `docs/engine.md`, "Tile collision"; the
list for tools is `catalog/collision.json` (value → name → meaning → Maverick target). The intro
counts and cell places are in `docs/stages/intro.md`, "Collision". The old intro counts were right;
only the old places and pictures (from `0x8009741E`) were wrong.

To do:

- [x] Name the open values (1–12 and 17–28 slopes, 16 under a slope, 34 one-way floor beside a ladder
  top, 35 water body, 37–47 bars and rope, 61 and 13–15, 29–31, 48–55 no handler).
- [x] Record the legend in `catalog/collision.json` for the TMX converter.
- [ ] Repeat a picture check on another stage (for example Commander Yammark or the Sheldon glass
  slopes, values 25–28 on 60) and confirm that each value means the same thing there. The code is
  the same for all stages, so only stage overlays that read the values themselves can differ.
- [ ] Make the per-value check pictures part of the `stages` command instead of a scratch script
  (the intro pictures of 2026-10-22 are in the session scratchpad, `agent-collision/`).
- [ ] Why the intro has 21 spike cells (63) inside solid ground, where the player cannot reach them.
- [ ] Maverick has no slope, bar or rope objects: decide the targets for the converter.

## Object legend (name each object kind and id)

Status: in progress. The legend, with every intro object and its status
(confirmed, plausible, unknown), is in `docs/objects-legend.md`.

To do:

- [x] Confirm on the intro `stage.ora` that the markers are on real objects
  (2026-10-19: enemies, blocks and gates match; ground enemies probably fall
  onto the floor below their marker). Note 2026-10-21: that `stage.ora` was drawn from the wrong
  layout address (art one screen row low); check the markers again on the new drawing.
- [x] Name the intro objects by number; from that, name each `kind` and `b`
  (29 confirmed, 19 plausible, 9 unknown; see the legend).
- [x] Why Junkroid 27 never turns (2026-10-19: `id` 0 = dormant; see the legend).
- [ ] Confirm the difficulty filter: play the intro on the hardest difficulty
  and look at the blue (kind 2) markers.
- [x] Object 24 (2026-10-20: a Hover Gunner with id 0; it flies in from the right
  at once, see `docs/enemies/hover-gunner.md`).
- [ ] Settle the rest of the open questions in the legend (the `id` byte for
  other types).
- [x] Repeat the pipeline on Commander Yammark (2026-10-19: layout, textures and
  palette start (0, 486) are the same as the intro; the object list pointer is
  per stage, see below).
- [ ] Name the Yammark section 1 objects (108) the same way.
- [x] Behaviour sheets for the intro enemies (2026-10-20): Junkroid, Hover Gunner, Meta Dridler,
  D-1000 (`docs/enemies/`). Still to do: Meta Wheel F.
- [ ] Check in play: Meta Dridlers 46–48 (do rock blocks appear under them?), the Hover Gunner
  contact box, the D-1000 weak-point weapons.
- [x] Intro, wave 1 (2026-10-22, helper agents): the camera zones and checkpoints, the collision
  values, the sabre block (`docs/enemies/sabre-block.md`), the kind 5 events (wrecks, the Hunters'
  scene), the talk zones, the BG modes and the stage start (`docs/stages/intro.md`,
  `docs/engine.md`).
- [x] Intro, wave 2 (2026-10-22, helper agents): the text format and the text banks on disc
  (`docs/engine.md`, "Text"; the intro texts in `docs/stages/intro.md`, "Texts"); the D-1000 entry
  and death (`docs/enemies/d-1000.md`); the boss entry and exit objects (stage logic 24, 4, 26);
  Zero and the High Max scene (`docs/stages/intro.md`, "Events"; `docs/enemies/high-max.md`, "The
  intro version"). `0x80021AF0` is `PortraitCharacter`, not `StageMusic`; `HpBarShow` /
  `HpBarBusy` are `BossMusicRequest` / `BossMusicWait`.
- [ ] Intro play checks: the sabre block's 8-px move up; zone 3 (record 5) acts; the Hunters' scene
  and checkpoint 2 order; the "!" icon and Select at x 1920; what is behind the transparent BG; a
  respawn at checkpoint 1 or 2 (texture entry); the READY total time; the D-1000 landing y; the text
  speed (5 updates per character) and the buttons that turn a page.
- [x] Intro save states while texts 2, 3 and 4 are open: `intro-4_hunters-text`,
  `intro-5_alia-text`, `intro-6_high-max-text`. The High Max pack and sprites are confirmed live.
- [ ] Intro save state (optional): a High Max big ball (palette `0x7948`).
- [ ] `0x80097428` (the talk zones and the HUD are off while it is set); `mega+0xE0` = 1 or 2 in
  the talk zones; whether anything clears `talk_seen` (`game+0x9E`); `game+0x1F` (cleared while a
  text box is open).
- [ ] Game mode 11 after the intro (`game+0x66` = 1): what the report screen shows. Known: the
  mission report (overlay 20) draws `game+0x98`, the HP lost, capped at 999 (`docs/damage.md`,
  "Counters").
- [ ] Ray types 3, 4, 6, 8 and 15 (the WARNING, the explosion light beams, the slash flash): what
  they draw.
- [x] Tool: a `text` command (`mmxdemake/text.py`) that writes the text of every bank (members
  203–242, 106, 110, 111, 149–161) as JSON and text, from the disc only.
- [x] Tool: `spriteexport` `sets` rule with `"palette": null` (= the main set's palette) and the
  High Max rule (set 1, animations 10–17 and 20, stages 21, 22 and 0); the export `high-max` is now
  `enemy-1b0293f642` with 23 animations. The old folder is `enemies/enemy-b15382edbf/` (not used).
- [ ] Tool: the overwrite check of `spriteexport` does not cover live stage objects. In
  `scaravich-9_high-max` the High Max texture covers banks 4–8, and the grey block (bank 7) was
  exported as a wrong sheet, `enemies/enemy-f386f3428b/` (delete it; the good export is
  `grey-block`). The same in `scaravich-11_boss`: the boss texture covers banks 7 and 9 and the grey
  block, and three wrong sheets were made (types 31, 37 and stage 12; deleted by hand and listed
  as skipped in `sprites/stages/scaravich.json`). A run again on that state makes them again. The
  same three again in `scaravich-13_boss-death`, also deleted.
  Cause [certain for the VRAM, likely for the owner]: the VRAM cells of banks 4–9 are the same in
  `scaravich-4` and `scaravich-10`, all change between `scaravich-10` and `scaravich-11` (the boss
  room load), and stay the same in 11, 12 and 13. The Scaravich ball (visual 14) draws with
  texture `0x240` (boss + `0x100`) over banks 7–9. The check sees an overwrite only through a live
  object that uses the cells: it does not count live visual objects whose sprite data is a
  main-type bank (state 12), and after the ball explodes at the boss death no object uses the
  cells but the pixels stay (state 13). Fix: count those visual objects, and also mark a bank as
  overwritten when the boss textures of the area (the type-50 banks with no texture entry) are
  loaded over it.
- [ ] Tool: export the Ground Scaravich ball (visual 14, bank 15 sprite data, texture boss +
  `0x100`, table `0x800F6234`, 21 animations). The probe export is in
  `sprites/enemies/scaravich-ball/` (`enemy-073aff2613`), drawn by
  [`~/mmx-data/x6/re/scaravich-boss/scripts/ball_export.py`](../work/scaravich-boss/scripts/ball_export.py) and copied in with the tool's content ID
  by `ball_publish.py` in the same folder, so that `spriteexport` accepts the folder.
- [x] Tool: `spriteexport` reads a main or stage object's palette without bit 15 (`CLUT_MASK` in
  `_source`, `animfind.py`). Before, the boss death flash (`0xF948` in `scaravich-13_boss-death`)
  was read as outside VRAM. In the game, bit 15 makes `DrawObject4` use the fixed CLUT `0x7840`.
- [ ] Tool (optional): draw the flash frames with CLUT `0x7840`.
- [x] Tool: `spriteexport` exports live effect objects (`class` `effect`). From
  `scaravich-13_boss-death`: `enemies/explosion/` (effect type 4, table `0x80077D54`, 19 frames).
  The sheet sprites 33–41 look broken (other effects whose textures are not loaded); the
  animation does not use them.
- [ ] Rays (`BossExplosion`, ray type 4, `0x8004C2F4`, states `0x80078288`) have no sprite data:
  4 corner points (`+0x16`…`+0x32`), a visibility test at `0x800328B0`. Read the draw pass that
  draws them (the primitive type and the colours) to rebuild them in the engine.
- [ ] rekit: the entry of stage 2 main type 6 (`0x800EB5A0`) loads its state table `0x800F6A84`
  with the `lui`/`addiu` interleaved with the `prev_x`/`prev_y` stores; the kit lists it under
  "Globals and data" and does not walk the 4 states and the 8 steps that follow it (`0x800F6A94`).
  It also merges the visual `var` 1 table (`0x800F6AD8`) and `var` 2 table (`0x800F6AE4`) of
  visual type 5 into one 6-entry table (the dispatcher `0x800EB670` loads the `var` 2 table with
  its own `addiu`, at `0x800EB724`). Work-around used: `--at` for each function
  (`~/mmx-data/x6/re/purple-wolf/at/`).
- [ ] Blizzard Wolfang ice: a play check of the avalanche (do the blocks hit a standing X?), of the
  rumble stop rule of stage logic 12 (5/3 each update with no falling ice) and of the `CopyTiles`
  at the end of the ice part; the reader of `game+0x27`; a stage page `docs/stages/wolfang.md`
  (records, the trigger table of `docs/enemies/ice-fall-controller.md`, the Wolfloid records of
  `docs/enemies/wolfloid.md`, the boss room and its way in from `docs/enemies/blizzard-wolfang.md`,
  "The boss room"). Save states during an avalanche (X at x 769–863) and near the
  bouncing blocks (x about 4720, y ≥ 448) would confirm the modes read only from the code.
- [ ] Tool: the overwrite check of `spriteexport` does not cover the extra sprite source of a live
  misc object. In `wolfang-6_boss-warning` and `wolfang-7_boss-fight` the boss texture (type 54, no
  texture entry, banks 4–8 marked "overwritten by type 54") also covers the cells of misc 23 (entry
  8, the snow scenery), and the tool makes a wrong sheet `enemy-6e86b66bc1` (the good one is
  `enemy-cd98e86270` from `wolfang-5_ice-pit`). Fix: apply the same cell test to extra sources.
  This is the same family as the Scaravich boss states: the boss and ball textures stay in VRAM,
  and the check sees an overwrite only through live objects.
- [ ] Blizzard Wolfang: save states at boss level 2 or 3 (ice spikes on the floor, walls and
  ceiling), of a stun (Magma Blade), of the ceiling moves (HP half or less) and on Hard (debris that
  hurts) would confirm the parts read only from the code (`docs/enemies/blizzard-wolfang.md`, open
  questions).
- [ ] Tool: `spriteexport` skips the Rekkoha beam (weapon id 44) and its top part (effect type
  26), both on the shared table `0x8007798C` with sprite data `scratch+0x1C` entry 15
  (`heatnix-3_rekkoha`): "animation 0 uses sprite 8, bank -1 has 6". `fitting` keeps only the
  leading animations that fit, and animation 0 of this table does not fit. Only animations 11
  (beam) and 12 (top part) are theirs. The same fix as the lemon: keep the animations of the
  object's own sprite data (a catalog rule per entry, or the animation of `+0x17`), not a prefix.
  Kit export until then: `sprites/enemies/rekkoha-beam/` (`enemy-56bb20f5e7`, made by
  [`~/mmx-data/x6/re/zero-rekkoha/scripts/rekkoha_export.py`](../work/zero-rekkoha/scripts/rekkoha_export.py)).
- [ ] Tool: live effect objects whose sprite data is in the `scratch+0x3C` directory (the Giga
  backdrop, effect type 17, entry 0, texture `0x1E00`, CLUT `0x7E50`, sprites 83/84 for Zero) are
  not exported: they have no animation table and the data is not a `sprite_bank_base` entry. Kit
  export: `sprites/enemies/rekkoha-backdrop/` (`enemy-c61736d6e0`).
- [ ] `spriteexport` on `heatnix-3_rekkoha` makes `enemy-2fbbe8998b` (main type 36, search, bank 3,
  table `0x800F3CBC`) with every sprite blank (cell 1 × 1). Do not copy it; find why bank 3 draws
  nothing in this state.
- [ ] Tool: `read_table` stops at `MAX_ANIMS` = 64 (`mmxdemake/anims.py`). Illumina's table has 84
  animations, so the `illumina` export lacks 64–83 (robot bank: 75–81). Its animation 1 draws empty
  sprites 100–104 (not read). The effect 11 live source of `mijinion-*` makes two duplicate exports
  with the wrong set (`enemy-b797c30985`, `enemy-cf8f009f32`; removed by hand and listed as
  skipped in `sprites/stages/mijinion.json`).
- [ ] Tool: in `mijinion-3_boss-fight` bank 3 (main type 8, the Illumina jets) is drawn from
  overwritten cells, but the overwrite check does not mark it; the wrong export
  `enemy-08bc9efa49` and `bank-3.png` were removed by hand. The same family as the other boss-state
  items above.
- [ ] Stage 8 page `docs/stages/mijinion.md`: Illumina is record 51 (main 24 `id` 0 at (160, 128));
  the fixed list holds stage logic 5 (`TunnelBgAnimator`) and main 19 `id` 1 (the Nightmare Effect
  3 spawner); camera zones 11/12 (x 2016–2047) and 13/14 (x 4192–4223) set min y at the two
  Illumina fights; boss gates `id` 0 (record 52, x 5488) and 27 (record 53, x 5808); Infinity
  Mijinion is record 54 at (5968, 1032) (`docs/enemies/illumina.md`,
  `docs/enemies/infinity-mijinion.md`).
- [ ] Nightmare Snake (`docs/enemies/nightmare-snake.md`): save states of the other fights (`var` 4,
  6, 7, 8 or 9, 10) to check the code-only rows of the sheet; a play check of the roll cycle (about
  576 updates) and of whether the ring hurts the Rescue Reploid.
- [x] Blaze Heatnix's map: the `texture_split` (block `0x16` from camera zone 1, x 1376) was set by
  eye from the disc data. `heatnix-5_snake-rising` (checkpoint 5) has `0x16` in VRAM; its drawing
  (entry "Blaze Heatnix, second texture", `tmx_skip`) agrees with the regions: the tower and the
  rooms left of x 1280 are correct only with block 1, the cave floors, the right building and the
  purple BG fields only with block 2. The TMX still takes the colours of `heatnix-1_start`.
- [ ] Tool: a live stage object 4 (Rescue Reploid) with other sprite data than the set-B rule
  expects gives wrong animations 26, 27 and 33 (`enemy-c1aea0bc4c`, stage 3,
  `heatnix-2_mini-boss`): the set-B animations are drawn with the live source's sprites.
- [x] Stage object 7 of stage 2 (overlay 04): the ice stack block
  (`docs/enemies/ice-stack-block.md`): the pit generators (`var` 0, records 58 and 59), the pit
  blocks (`var` 12, one in two with a Wolfloid), and the rows with one gap of the ice shaft
  (`var` 1–5, 10, 11) that stage logic 8 makes. Save state `wolfang-5_ice-pit`.
- [ ] Ice shaft of stage 2 (stage logic 8, x 5760–6000, y 312–1264): no save state has it. Check in
  play: the gap filler kills a player in the gap; a row on the player's head does 16 damage and
  breaks; the extra gap fillers at row x ± 144 (side niches?); the camera field `+0x2C`.
- [ ] Boss intro: the start x of BG layer 2 (from `LayerInit`) and so the time of sub-mode 6; what
  sets game mode 5 (likely overlay 16 after the pick); whether stages 19–22 show the intro; entries
  `0x20000` and `0xB` of members 141–148.
- [ ] Tool: `mmxdemake.bossintro`, to draw the 8 boss intro pictures from the disc files (layout and
  camera tables of overlay 17, members 141–148, palette members 52–59; `docs/engine.md`, "Boss
  intro"). A working draft is [`~/mmx-data/x6/re/boss-intro/scripts/render_intro.py`](../work/boss-intro/scripts/render_intro.py); its plan is in
  [`~/mmx-data/x6/re/boss-intro/doc-changes.md`](../work/boss-intro/doc-changes.md), part 5. `texture.raw_tile` draws these 8 bpp tiles
  correctly, so the README's "8 bpp pages are unverified" can change when the tool has a test.
- [x] Tool: `playerexport` draws Zero's sabre animations with their own palettes. The sabre
  objects draw with CLUT `0x7801`, which `PaletteSlotLoad` fills from the player palette list
  (scratchpad `+0x24`, `0x80103000`) with palette `sabre_tbl` byte 1 (3; 4/5 with the parts or
  Black Zero; 6 for id 48, 7 for id 35, 8 for id 26 in step 55). The tool output is the same,
  pixel for pixel, as the hand-made `sprites/players/zero-sabre/` (which also has the hit box of
  each frame).
- [ ] [`~/mmx-data/x6/stages/`](../work/stages/) holds files that `stages` did not write (`probes-*`,
  `*-doc-changes.md`, `*-helpers-proposed.json`), so `stages --out ~/mmx-data/x6/stages` refuses to
  run. Move them. The three Scaravich folders (members 100, 119) were drawn into another folder
  and copied in by hand.
- [ ] Tool: the `stages` map of `catalog/enemies.json` has no area, so stage 21 area 0 (Scaravich's
  hidden area) is filed as `turtloid-alt` in the sprite exports.
- [x] Tool: export misc-type banks and the text portraits (`zero-intro`, `hunters`,
  `portrait-x-falcon`, `portrait-alia`, `portrait-high-max`).
- [ ] Tool: the sabre block (stage object 2, type 129, bank 1) still has no export: no save state
  has a live block, and a stage object of a misc type is taken only while it is live.
- [ ] Tool: animation tables for misc objects (Zero `intro_zero_anims` `0x800FBC2C`, the portrait
  talk animations); the exports are sheets only. The intro scenes that need them: Zero's
  beam-in and slash (`zero-intro`), the hurt Hunters (`hunters`), and the talk portraits
  (`portrait-x-falcon`, `portrait-alia`, `portrait-high-max`).
- [ ] Intro play check: Meta Wheel F record 38 has `id` 0; the code reads 0 as "rolls right".
  Check in play that it rolls right.
- [ ] The HUD: the player's health bar, the weapon energy bar and the boss bar (layout, sprites,
  colours, fill rule). Only the "!" icon (HUD element 10, `0x80024528`) is read so far.
- [ ] Portraits of the other characters and of X in the other armours (save states with a text open).
- [x] The pointers next to `0x8007349c`: a fixed table with one entry per
  stage, 8 bytes apart (intro = entry 0, Yammark = entry 1). `learn` now
  tries every entry and records `table_entry`.
- [x] A second list follows the first one on the disc, with its own per-stage
  table at `0x80073554` (`fixed_set_tbl`). 2026-10-21, Turtloid: it holds the objects of fixed
  place (stage logic, the rain controller, scenery misc objects, the shadow spawners); see
  `docs/stages/turtloid.md`. Still open: the kind-6 records and the Yammark list (21 records).
- [x] Withdrawn 2026-10-21: the Yammark "vertical wrap", the layer-0 "row 0 header" and the
  converter items for them. All three came from a wrong layout address (`0x8009741E`). The game
  reads the layout through the scratchpad pointer `+0x04`, which is `0x80097438` in every save
  state, 26 bytes later. With that address every map learns with 0 differing cells, and X, the
  Junkroids and the Meta Wheel Fs stand a fixed distance above solid floor (process log section 68).
- [ ] The 26 bytes on the disc before each layout (`ff ff ff ff ff ff 80 …`): what they are.
- [ ] Check every map again by eye after the fix ([`~/mmx-data/x6/stages/`](../work/stages/); the old drawings are in
  `~/mmx-data/x6/stages.old/`).

## Enemy RE (behaviour sheet, pseudo code, reconstructed C)

Status: the RE kit (`mmxdemake.rekit`) and `docs/re-guide.md` are done; piloted on Meta Wheel F
(process log section 58).

- [x] Junkroid, Hover Gunner, Meta Dridler, D-1000 (before the kit); Meta Wheel F (kit pilot).
- [x] Batch 1, small enemies (2026-10-20, helper agents): Killer Moth, Batton Bone B81, Sea Attacker
  ST, Monbando, Worm Spider, Nightmare Insect (process log section 59).
- [x] Split the shared rules out of the sheets: `docs/engine.md`, `docs/stages/` (process log
  section 60).
- [x] Batch 2, stage objects and shared systems (helper agents): Caterpirade, boss gate, teleporter
  crystal, Weather Analyze, Nightmare Virus, Rescue Reploid, Light capsule; short sheet rain effect;
  Nightmare shadows (full set: not a pure effect) (process log section 61).
- [x] Turtloid stage map and records (`stages`), for `docs/stages/turtloid.md` (process log section 62).
- [x] Turtloid stage-object types 5 (rocket platform) and 13 (health capsule) (process log section 63).
- [ ] What draws the Turtloid sea where the art layer is empty.
- [ ] Not documented yet: the Rescue Reploid's shot (visual type 24, `0x8004F2EC`), the Nightmare
  Virus rays (`0x8009C9B0` class), the stage-logic objects (rain control `0x800ED510`, camera
  records), the object at `0x800EE154` (Nightmare Effect 8, overlay 03).
- [x] Find what writes `nightmare_effect` (`0x800CD30A`): the stage pick writes
  `game.stage_effect`, the stage start copies it; the stage select marks only the targets of the
  last pick (`docs/engine.md`, section 79).
- [ ] Nightmare Effect value 5 in stage 7 (state table `0x800FC6B8`, not read), and the values of the
  stages not read yet.
- [x] Shield Sheldon stage map and records (`learn`, `stages`): `docs/stages/sheldon.md` (helper
  agent; section 79).
- [x] Shield Sheldon: Nightmare Bug (`docs/enemies/nightmare-bug.md`), the infected Rescue Reploid,
  the laser puzzle (`docs/enemies/laser-reflector-puzzle.md`), the hidden floors (helper agents;
  section 79).
- [x] Shield Sheldon: the lower rooms (`sheldon-8`, `sheldon-9`, second texture block) and the
  hidden area behind the teleporter crystal (stage 22, `docs/stages/sheldon-alt.md`; `sheldon-10`
  to `sheldon-12`, High Max) (helper agent; section 80).
- [x] Shield Sheldon: Mettaur D2 (`docs/enemies/mettaur-d2.md`) and Guardian
  (`docs/enemies/guardian.md`) (helper agent; section 80).
- [x] Shield Sheldon: the texture swap (`StageTextureSwap`, camera zone record 3 at x 4352); the
  camera y limits 496 and 752 (the locks of doors `0x54` and `0x55`); the background change at
  checkpoint 1 (`SheldonBgSwitch`); why X cannot climb back up from the first lower room (the door
  `0x54` hole is closed with 56); the boss sheet `docs/enemies/shield-sheldon.md`; the Death
  Guardian (helper agents; section 81).
- [ ] Shield Sheldon: a state after a death and restart at checkpoint 1, 2 or 3 (does the restart
  load entry `0x10000` again?); the laser puzzle just after each of gates `0x51`–`0x53`; a hidden
  brick block while shown; a floor after a hit by weapon id 44.
- [ ] Shield Sheldon boss: re-export the set-1 animations (0–3, 35, 36) from bank 15 / texture
  `0x540` (a "bank + 1 by animation" rule, as for Rainy Turtloid); states in the bounce, with the
  plates or turrets out, in the mirror attack, just after a stun hit, at a higher boss level; which
  weapons ids 4, 13 and 35 are; whether the plates, image and turrets react to every weapon and hurt
  on contact.
- [ ] `sheldon-13` to `sheldon-17` have `game+0x5E` = 1 (Falcon), not 0.
- [x] Stage logic 14 (`0x8005335C`): it is `TalkZone`, Alia's message zone (forced or optional "!"
  with Select), not a background setting (2026-10-22; `docs/engine.md`, "Talk zones").
- [ ] Check the other stage members for a second texture entry (`0x16`); member 107 has entries
  `0x10014` and `0x10016`, member 137 `0x1040a` (not read).
- [ ] Which armour part is bit `0x04` of `game+0x69` (the Light capsule of stage 7).
- [ ] Hidden areas: where a hidden area leads after its boss (read from the code:
  `docs/enemies/high-max.md`, "Where the clear leads"; no state after a clear yet). A hidden area
  with `boss_choice` 2: done (`mijinion-4_dynamo`, `docs/stages/mijinion-alt.md`).
- [ ] Shield Sheldon: stage logic 7 (`0x800F0BB0`) and 27 (`0x800541D4`), stage object 15
  (`0x800F3B98`); who sets the Nightmare Bug's state 3.
- [ ] Play checks: Mettaur D2 guard timing (9 / 60 / 80 updates), the shot when the player is
  behind it, the "never shoots again" lock after a guard that interrupts a shot, and the pace on the
  hidden floors; Guardian cycle (297 updates, 4 shots), open window (updates 22–149), the back-half
  rule and the 188 px rule.
- [x] `spriteexport`: a type listed twice in the stage type list gets an export for its second bank
  too (bank + 1, same type, texture loaded): `death-guardian` (section 81).
- [x] Metal Shark Player, section 1 (stage 4 area 0, overlay 06, member 98):
  `docs/stages/shark.md`; the Nightmare Press (`docs/enemies/nightmare-press.md`) and the crush
  death; the Ride Armor (`docs/players/ride-armor.md`) (helper agents; section 82).
- [x] Metal Shark Player, section 2 (stage 4 area 1, overlay 07, member 99): `docs/stages/shark-2.md`;
  the conveyor belt (`docs/enemies/conveyor-belt.md`), the section 2 press, the mini boss Nightmare
  Pressure (`docs/enemies/nightmare-pressure.md`), the boss (`docs/enemies/metal-shark-player.md`),
  the two warps and the white fade (helper agents; section 83).
- [ ] Metal Shark Player: states of the stage start, the section 1 lower path (to `learn` the second
  texture entry from a real state), the small room and the bottom room (crystal destinations), the
  way from area 0 to area 1; X under the falling press on floor 320 or 352, or under a tooth (x
  928–991, floor 368); the Ride Armor holding the press; a junk block on screen; Nightmare Effects 2
  and 5; a restart after a death at checkpoint 1 and 2; during the white fade; the beam-in on the
  belt.
- [ ] Nightmare Pressure: states during the BEAM, the DRILL and the PRESS bottom, with Zero, and on
  Hard; play checks: the press gap (40 px; X 38 px tall), the drill-ram safe spot, pods hit only
  from their front; who reaches step 9 (END CRUSH).
- [ ] Metal Shark Player: the side-kick names (`var` 3 probably Blast Hornet; `var` 1 and 2 open);
  which weapons are ids 35 and 42; re-export strips 34–37 from the next bank; states with Zero
  (side kick `var` 1), on Hard (`var` 3), in BURROW, a charged slot-6 shot, the death and soul.
- [ ] `catalog/helpers.json` applies overlay addresses in every stage: `0x800F765C` is
  `SheldonTurret` in overlay 10 but the spark of visual 22 in overlay 07. Give overlay names a
  per-overlay section or note.
- [ ] Main type 35 of stage 4 area 1 (Nightmare Effect 2, blue ice-like blocks, export
  `enemy-4c32312ebb`): official name and sheet.
- [x] Two bad exports from `shark-7_boss` (`enemy-65ec66c9da`, `enemy-c5fe34fa82`: the boss texture
  covers their cells): keep them, unnamed (user decision, 2026-10-22). Do not delete them.
- [ ] Nightmare Press: read `0x80032B34`, `0x80032BE8`, `0x8003208C`, `0x80031F4C`; `mega+0xE0` = 2
  (freezes the press); the stage ≥ 19 user of `PressUpdate`.
- [ ] Ride Armor: bits `0x01`, `0x10` of `+0x97`; the `var` ≠ 0 model; `actor2` types 2–4; which
  stage overlay 28 is; states of the ride jumping, dashing, punching, at 0 HP, and empty; play
  checks (jump about 61 px, dash about 132 px, 2-punch limit, charged punch, up + jump to leave).
- [ ] `spriteexport`: export `actor2` (the Ride Armor, bank 3, 31 animations) as a live source;
  `rekit --name` on a sprite-only misc or visual export fails with a `KeyError`.
- [x] Laser puzzle: the laser sprites (all 250 of `sprite_bank_base` entry 16), sprite-only export
  `laser-reflector-puzzle-laser` (helper agent; section 80).
- [ ] Laser puzzle: whether doors stay open after a death; whether the laser probes count against
  X's shot limit; play checks (aiming cycle 304 updates, sabre wait 33, fixed beamer on/off, 1
  damage from the head only).
- [x] `spriteexport`: an object that stores its state table in `+0x30` (Sheldon main type 32) gets
  a sprite-only export of its bank (`hidden-glass-floor`).
- [ ] `rekit --name` on a sprite-only export of a misc or visual source fails with a `KeyError`
  (the source has no `table`); use `--type`/`--at`, or teach `rekit.resolve` the misc and visual
  classes.
- [ ] `d-1000-extra`: the intro's `sprite_bank_base` entry 16 (49 sprites: D-1000 body frames,
  shots, a blue beam, visual type 1 in `intro-3_boss`); decide whether `d-1000` or `d-1000-parts`
  should take it.
- [ ] An extra sprite source is found only while an object that uses it is live; the flip rule
  (pixel `rx` → `−rx − 1`) is likely, not certain.
- [ ] Re-learn the older catalog entries once, so that `edge.v` and `edge.v_joins` use the new
  vertical score (joins with a uniform edge row are not scored).
- [x] Commander Yammark: sheet, notes, pseudo code, reconstructed C, review and fix pass (process
  log sections 65–66).
- [ ] Commander Yammark: check the drone timings in play (the extra lap, the lance rhythm, the bob).
- [x] Rainy Turtloid: the boss-room state `turtloid-7_boss` and the export `rainy-turtloid`.
- [x] Rainy Turtloid: behaviour sheet `docs/enemies/rainy-turtloid.md` (helper agent; process log
  section 74).
- [ ] Rainy Turtloid: re-export animations 28–33 from the next two sprite banks (the exporter used one
  bank).
- [ ] Rainy Turtloid: play checks: the stun weapons (ids 2, 11, 33); collision 57 on the hit wall; the
  back hurt box from behind; the missile count per volley.
- [ ] Turtloid: stage logic type 3 (`0x80052898`) and the kind-6 records of the second list; the
  counters `game+0x98`, `+0x9C`, `+0xAA`; the respawn point of each checkpoint.
- [ ] What `game+0x8D` counts (the boss level), with a save state later in the game.
- [x] Kit: name an address as an offset from a known global (`mega+0x14`).
- [ ] Kit: show `jr reg` switch tables as code, not data, and walk their targets (boss gate
  `0x80011694`, shadow `0x800E9864`); follow a copy of self in another register; fields of the
  player read through a base register, also `a1` = `game` and `a2` = `mega` in stage-object steps
  (`0x80050574`).
- [ ] Kit, from batch 2: know the class tables at scratchpad `+0x88` and `+0x8C` (stage logic);
  find animation tables of shared objects in the main program (Rescue Reploid `0x80078BEC`, Light
  capsule `0x800791A0`); cut `data.txt` dumps at the next known table (`0x8007497C`, `0x800F7400`);
  for a spawner type, follow the object it makes (main type 7 → stage type 1); name `+0x82` as
  `work.82` like `+0x80`; walk shot code made through a table (`0x8004F2EC`, ray entry
  `0x80041B6C`); split `0x800760E8` (3 entries, not 6).
- [ ] Kit: a step table is read past its end into the next table (rocket platform `0x800F60A0`: 3
  entries, the kit walks 8 and adds the rain controller's code to the platform). Stop a table at the
  start of another known table, or at the highest step value that the code stores.
- [ ] Kit minors from the final review: `found_tables` takes about 21 s without `--name` and does not
  mark searched candidates; `self_fields` misses a store through `a0` in the delay slot of the first
  call; no test for an export with several tables; a type out of range gives exit 1 (the spec says
  2); the sheet's box heading does not say that terrain boxes use half sizes.

## Player RE (X and Zero)

- [x] Step 0: `rekit --player` (process log section 75).
- [x] Step 1: the code map `docs/players/map.md` (section 76): the 32 steps named from their
  animations and transitions, the weapon type table, the overlay calls.
- [x] Step 2a: ground movement (steps 2–4, 8, 12 dash, 26 crouch) — `docs/players/movement.md`;
  notes, pseudo code and C in [`~/mmx-data/x6/re/x/movement/`](../work/x/movement/) (process log section 77).
- [ ] Step 2a open items: `mega+0x87`; `0x8003D26C`, `0x8003F644`, the dash dust and afterimage; the
  part names (`mega+0xEC` bits); `game+0xC9`; play checks of the dash and walk start.
- [x] Step 2b: air movement (6, 7, 9–11, 13, 27, 28, 31) — `docs/players/air.md`; notes, pseudo code
  and C in [`~/mmx-data/x6/re/x/air/`](../work/x/air/) (helper agent; process log section 78).
- [x] Step 2c: ladder, hit reaction, death, beams, ride and events (0, 1, 5, 14–18, 20–25, 29, 30) —
  `docs/players/ladder-hurt-events.md`; [`~/mmx-data/x6/re/x/other/`](../work/x/other/) (helper agent; section 78).
- [ ] Step 2b open items: `mega+0x87`, `+0x92`, `+0xA4`; the overlay air steps 33, 35, 74 and Zero's
  air tries; `0x80038594`, `0x8003F700`; tiles 47 and 57; effects type 33 (var 0, 1, 3) and 65.
- [ ] Step 2c open items: (the ride object is `actor2`, `docs/players/ride-armor.md`); the users of
  hit classes 3 (grab, `PlayerGrabDamage` `0x8003D07C`) and 5 (push); who asks for event steps 21,
  23 and 30; part bits `0x400`, `0x800`; `+0xCF`, `+0x8D`, `0x800C4568`; the death orb (misc type
  17); `game+0x98`.
- [x] Player steps 32–77 and the forms (0 no armour, 1–4 armours, 5 Zero) —
  `docs/players/steps-extra.md`; the kit walks all 78 entries (helper agent; section 79).
- [x] Step 3: X's buster, charge, shots and weapon ids — `docs/players/attacks.md`;
  [`~/mmx-data/x6/re/x/attacks/`](../work/x/attacks/) (helper agent; section 79).
- [ ] Step 3 open items: X special weapons 1–8 and the charged forms 10–17; ids 20/21, 43, 45, 46,
  50, 51; the pose code (`0x8003F864`) and the Giga attack (`0x8003E344`); effect type 27;
  `+0xA6`, `+0x144`; which pad buttons are bits `0x10`, `0x20`, `0x40`, `0x200`, `0x400`.
- [ ] Zero's step bodies 48–63 in detail, and the official names of Zero's techniques.
- [ ] Save states in each armour (Shadow, Blade, Ultimate) to export members 88 and 89 and check
  the frame-set rule of `playerexport`; check whether the Ultimate Armor looks like no-armour X
  (forms 0 and 4 share member 86); a state of Zero in a technique step.
- [ ] Play checks, player: jump and tap height; wall delay 10, grab 8, push-off 4 updates; air dash
  hover and drop; super jump ceiling hold; ladder 1.5 px/update and the top climb; knock-back
  (light about 18 px, heavy hop about 16 px high); invulnerability 75/100 updates; beam-in landing
  87 updates; death sequence about 70 updates; spike death and form 2.
- [ ] Damage open items (`docs/damage.md`, "Open questions"): what reads `game+0x9C` (a reader
  that gets `game` as an argument is not found by the scan); the in-game names of part bits
  `0x200000`, `0x400000`, `0x1000000`, `0x2000000` (they are graphics: a save state of the parts
  menu or the stage-result screen); the fourth byte of `sabre_tbl` (`0x801EE68C`); the `var`s of
  Gate's visual type 26 (a stage 18 save state); whether explosions hurt the player.
- [ ] Play check: plain buster shots pass through enemy shots with `dmg_tbl_one_hit` (for example
  the Mettaur D2 shot); the Guard Shell, Metal Anchor and a sabre with part `0x1000000` destroy
  them.
- [ ] Decide: merge the Nightmare Cube and Nightmare Crate sheets, or keep them apart.
- [ ] The Falcon full charge (id 19): its look (its own frame set) and the charge palettes of X in
  each armour. No save state has the shot or the charge colours on screen; make one with a
  Falcon full charge in the air and one while charging.

## Front end (Capcom logo, opening, title, main menu)

Status: the state flow is read from the code and the two movies are decoded (2026-10-25,
`docs/engine.md`, "Front end"). Five save states from the logo to the menu (`title-1` to
`title-5`); the title and menu screens are rebuilt in `~/mmx-data/x6/re/front-end/images/`.

- [x] Capcom logo and opening: `python3 -m mmxdemake.movie` writes the frames, the sound and
  `movie.json` to `~/mmx-data/x6/movies`.
- [x] Save states of the title with PRESS START and the main menu (three rows: GAME START,
  CONTINUE, OPTION): `title-4_press-start`, `title-5_main-menu`.
- [ ] Save states: the CONTINUE screen (state 7), the OPTION screen (state 8), a demo in play, the
  picture after the demo (member 0). The loading screen is in `title-2_loading` (the user saw
  it; the state has no rendered frame). Not known which code draws it, or if it shows the
  "Now Loading..." text of the resident VRAM page at (832, 256).
- [x] The stage 14 layout (4 × 2, layer 0 screens 1–4) and the misc type 19 sprites are drawn by
  scripts in [`~/mmx-data/x6/re/front-end/`](../work/front-end/); the misc type table of overlay 18 is `0x800F15F8`.
- [ ] Tools: `learn` refuses stage 14 (no solid cells, so no joins), and `spriteexport` refuses it
  (no type list). Allow a stage without collision in `learn`, and a sprite source from a live misc
  object without a type list in `spriteexport`. Misc type 21 (state 3) and misc type 1 of overlay
  18 (`0x800EA02C`) are not read.
- [ ] Menu rows 1 and 2: row 1 is CONTINUE (state 7, `LoadGame`), row 2 is OPTION (state 8). Still
  open: the options rows, the memory card load screen, what shows screens 3 and 4 of member 107.
- [ ] Overlay 19 (stage 14 area 1) has a Left / Right X-or-Zero choice and a cheat counter that
  must reach 11 (`docs/players/steps-extra.md`); the main menu has its own two cheats (game mode
  0). Find when overlay 19 is loaded.
- [ ] What member 162 (the unused notice), member 0 (the picture after the demo) and stage 14
  area 1 (member 108, overlay 19) show; the format of the demo members 90–93.
- [ ] `movie_table` fields `+6` and `+8`, and what fills rows 8 and 9 (game mode 15).

## Audio

Status: sound effects, the XA decode and the track table done (2026-10-22, `docs/sound.md`).

- [x] Sound effects: `python3 -m mmxdemake.sound` finds the 116 VAB banks, parses the heads and
  decodes the SPU ADPCM samples to WAV.
- [x] Map `PlaySound(bank, id, obj)` (`0x80016C48`): the bank is the slot, the id is a word of
  the bank's list; `bank.json` gives the WAV file of each id.
- [ ] Sound ids with no file: 151 words name another VAB id, 42 a program or tone that is not
  in the bank (boss members 137, 140). Find which loaded bank they play from.
- [ ] Bank 4 (not loaded in any state read), the flag in byte 1 bit 7 and the mode in byte 3 of
  the sound word.
- [x] Music: `python3 -m mmxdemake.music` decodes `XA/BGM.XA` (30 records on 8 channels) and
  `XA/DEMO.XA` (540 records on 16 channels) to WAV (`docs/sound.md`, "Music (XA)").
- [x] The track table: `bgm_streams`, `voice_streams`, `stage_music` and `boss_music`;
  `songs.json` and `voices.json` (`docs/sound.md`, "The track table"). One record is one song.
- [ ] Track uses still open: the values that overlays 18, 19 and 21 pass to `StreamPlay`, and the
  voice numbers that overlay 12 passes to `VoicePlay` (which voice goes with which text).
- [ ] Whether tracks 0, 2, 8, 10 and 29 (0.96 s, looped) are silence; the second byte of
  `stage_music` and `boss_music` (112–127).
- [ ] Listen to some WAV files (sound effects and music) to confirm the decoders by ear.
- [ ] SEQ members (archive type 3) and the sequence sound words (kinds 0 and 1, `PlaySequence`
  `0x80017008`): what they hold; convert them only if some stage music is SEQ.
- [ ] Rough parts to improve later: the play rate per tone (the WAV files are at 44 100 Hz), ADSR
  envelopes, volume, pan and SPU reverb.

## Player sprites

- [x] Probe: are all player frames in RAM? Yes, and on disc too: X = member 87 (228 frames), Zero =
  member 85 (240 frames); process log section 72.
- [x] Player exporter (`mmxdemake.playerexport`, process log section 73). Fixed in section 76: each
  animation now uses its own frame set. X: sets of 228 and 93 sprites, 189 animations; Zero: 240,
  113 and 102 sprites, 168 animations.
- [ ] Player animations that use sprites past their frame set: X 85, 180–182; Zero 118.
- [ ] Player shots and effects: the smaller frame sets after the main set in each member, and the
  weapon objects (`0x800950A0`). The buster shots were drawn by probe scripts
  (`~/mmx-data/x6/sprites/players/x/shots/`, copied to `players/x-shots/` on 2026-10-22: a
  `playerexport` run replaces `x-falcon/`, so the copy is not inside it); a tool path needs a source per live weapon or effect
  object, the shared animation table `0x8007789C`, and a streamed frame set for the Falcon full
  charge (type 19), which no state has on screen.
- [ ] Tool: `spriteexport` exports a live lemon (weapon id 0, `wolfang-4_x-crouch-shot`) as
  `enemy-54284bb54d` with all 40 animations of `shot_anime_tbl` (`0x8007789C`). Only 21 (lemon) and
  39 (lemon muzzle flash) use the lemon's sprite data (directory `scratch+0x1C` entry 1) and CLUT
  `0x7802`; the half-charge flash (22) uses entry 3, the full-charge flashes (23, 25) entry 2
  (muzzle table `0x80077C0C`: {entry, animation, sound} per var). Fix: keep only the animations of
  the object's own sprite data (a catalog rule per entry, or the animation of `+0x34`). Do not
  copy that export into `sprites/enemies/` until then ([`~/mmx-data/x6/re/x-crouch-shot/notes.md`](../work/x-crouch-shot/notes.md)).
- [x] X's shot sprites are copied to `~/mmx-data/x6/sprites/players/x-shots/`. The old `x/` export
  (the same content as `x-falcon/`, with `x/shots/`) stays as it is: the user prefers duplicates to a
  move.
- [x] X's armour layer: `playerexport` draws the second piece list (sprite entry 5 / 19) on top of
  the body for X in forms 1–4, with the CLUTs of `0x80071E68`. `x-falcon` was exported again (before,
  it showed plain X) and `x-blade` is new. The old `x/` export (`intro-1_start`, form 1) still shows
  plain X.
- [ ] Armour layer of sprite 126: the game skips the layer in step 1 (the beam) at sprite 126 for
  forms 1–3; `playerexport` skips it for sprite 126 in every animation. Check if another step uses
  sprite 126.
- [ ] Player palettes: armour, weapon and charge colours (only the palette of the save state is used).
- [x] Identify members 86, 88 and 89: 86 forms 0 and 4, 88 Shadow, 89 Blade (`0x80013BF0`).
- [ ] The smaller frame sets after the main one in each member.
- [ ] Parse the member header (word 0 = `0x10`, word 1 = size) and map the parts that go to RAM.

## Enemy sprites

Status: solved (process log section 54). All intro-area banks draw from the start save state.

- [x] Save state with a Junkroid on screen; its 11 sprites draw correctly
  (`~/mmx-data/x6/probe/sprites/draw2.py`).
- [x] The engine's decompressor: word LZ at `0x80018BE0` (process log section 53). It streams the
  player's frames; the stage enemies use another path.
- [x] The cell mismatch: the live drawer `0x800232D4` remaps tiles by object byte 0 bits `0x60`.
- [ ] Disc source of the stage enemy textures (now optional: one start state per stage area works).
- [ ] D-1000's sprite bank (boss area).
- [ ] Flipped drawing (piece flags `0x40` and `0x80`, object facing `+0x15`) and semi-transparency
  (object flag `0x10`).
- [x] Animation sequences and the sprite exporter: `mmxdemake.spriteexport` (process log section 55).
- [x] Name the Yammark exports (named by the user; `rain-effect` is a guess, not an official name).
- [x] D-1000 from the intro boss-room state (`d-1000`, `d-1000-parts`; process log section 56).
- [x] Commander Yammark (type 49) from the Yammark boss-room state.
- [x] Zero Nightmare (stage 21 type 53) from `turtloid-5_zero.state.gz` (`zero-nightmare`). The exporter
  now reads code in the boss overlay at `0x800FA000` (process log section 69).
- [x] Zero Nightmare: behaviour sheet `docs/enemies/zero-nightmare.md` (helper agent; process log
  section 70). Its timings need a check in play.
- [x] What sets `boss_choice` (`0x800CD30B`): `LoadGame`, overlay 16 modes 10 and 11, overlay 20.
  Type 55 = High Max, type 63 = Dynamo (`mijinion-4_dynamo`).
- [x] High Max: `turtloid-8_high-max` (`boss_choice` 1, overlay 44) confirms type 55; export
  `high-max` (15 of 23 animations).
- [x] High Max: behaviour sheet `docs/enemies/high-max.md` (helper agent; process log section 74).
  `var` 0/1/2 share one code.
- [ ] High Max open items: the weapons of the guard-break ids (33–36, 42–44, 46, 56) and of the X stun
  ids; what `mega+0xE7` is; `var` 2 (where it is made, steps 13–17, objects `var` 9–11).
- [x] Sprite exporter: High Max's shot animations (10–17, 20) use the shots' own sprite data; done
  with the `sets` rule (see the `"palette": null` item under "Object legend").
- [x] A hidden-area save state with `boss_choice` 2, to export Dynamo: `mijinion-4_dynamo`; export
  `dynamo` (`enemy-f49a77c40e`, 15 of 15 animations).
- [x] Sprite exporter: `catalog/enemies.json` `stages` takes a `"<stage>.<area>"` key that wins
  over the stage key: `"21.0": "scaravich-alt"`, `"22.1": "mijinion-alt"`. The old outputs were
  moved by hand (not exported again, which would bring back the wrong sheets removed by hand);
  `sheldon-10_section-2` was exported again to `sheldon-alt`, and the old `stage-22` outputs were
  removed.
- [ ] Sprite exporter: in `mijinion-4_dynamo` bank 3 (main type 8) is drawn from cells that
  Dynamo's texture covers, but the overwrite check does not mark it (wrong search export
  `enemy-06ef7e2514`). The same fault as `enemy-08bc9efa49` in `mijinion-3_boss-fight`.
- [ ] Sprite exporter: in `gate-1_before-boss` (and `gate-2_nightmare-mother`) the boss pack
  (member 135, entry `0x10411`) is in VRAM pages 7–14 and covers the cells of banks 1–6 (main 15,
  6, 18, 13, 33, 34). No live object uses those cells, so the overwrite check does not see it, and
  the seven search exports are noise (`enemy-9b63092464`, `enemy-71ce84488a`, `enemy-6d90265515`,
  `enemy-51c2def92c`, `enemy-80ea7f5439`, `enemy-5e5c1e01f6`, `enemy-4c7523f990`; removed by hand
  and listed as skipped in `sprites/stages/gate-1.json`). Fix: when `boss_pack_loaded`
  (`0x800CC868`) is 1, mark the banks whose cells are in the pages of the boss pack. Gate stage 1
  needs a save state at checkpoint 0–3 for its enemy sprites and the colours of its start
  (`docs/stages/gate-1.md`).
- [ ] Sprite exporter: the live table of `nightmare-mother` (`enemy-c99ffb5c2b`, bank 14) runs on
  past its end: animations 46 and 47 belong to the floor-fire table `0x800FCF50` (bank 15), and
  48–51 are words of the state table. Animations 46–51 were removed by hand from the shared export.
  A new run of `gate-2_nightmare-mother` brings them back. The floor fire (bank 15, texture `0x4A0`)
  is not exported: its palette is copied only when the fire starts.
- [ ] Dynamo: a state of the other moves (pillars, centre volley, leaping slash), of the stun and
  of the death.
- [ ] What resets `boss_choice` for a new game; what `0x80079F20` is. (`0x8008EAFC` is `text_open`:
  `TextUpdate` runs the text box while it is set. `0x80097424` is `fade_dir`.)
- [x] Main loop open items (`docs/engine.md`, "Main loop"): the writer of `game+0x67` (an
  options-menu byte), the hit-stop weapon ids, `PaletteFxRun`, the CD upload queue, task state 4
  (no writer).
- [ ] Main loop follow-ups: the on-screen label of the half-speed option (overlay 18, sprite frames
  7 and 9); the modes of `ArcLoadStart` (switch `0x80010024`); the
  libcd/libsnd function names (match against Psy-Q signatures); whether a block clear of `mega`
  resets `+0xCC`, `+0xCD`, `+0xE4`.
- [ ] Measure the frame rate on a running game. Make two PCSXR save states 10–30 s apart in one
  play session (one pair in normal play, one pair in a busy boss fight). Updates per blank =
  Δ `frame_count` (`0x80097420`) / Δ word `0x8007C8E4` (likely the Psy-Q `VSync(-1)` blank counter;
  check this first). Fallback: Δ `psxRegs.cycle` in the save state / 565 000 cycles per blank
  (states less than 2 minutes apart). Expected: 1.0; below 1.0 = slowdown; about 0.67 = half speed.
- [ ] Zero Nightmare open items: which weapon is id 50 (it starts his stun warp); `game+0x5F` bit
  `0x20` (set at his death at boss level 3; probably Black Zero).
- [x] Caterpirade (Yammark type 7): table `0x800F73D4` in `catalog/enemies.json` `tables`, read from its
  live object in `yammark-3_caterpirade.state.gz`.
- [x] The stage-object array (`0x800C1220`, 80 slots of `0xA4`, update table at scratchpad `+0x94`)
  is in the live-object scan (process log section 57); the Caterpirade needs no catalog entry.
- [ ] The misc-object arrays (scratchpad `+0x64`: `0x800CD410` with table `+0x90`, and `0x800964A0`,
  32 slots of `0x60`). 2026-10-21: four live objects in `0x800CD410` (Turtloid scenery, misc type 20)
  use the main-object layout (flags, type, state, x, y, record `+0x10`, priority `+0x16`, sprite data
  `+0x3C`, texture `+0x40`, palette `+0x42`). `0x800964A0` has no live object yet. Exporting misc
  types is still open.
- [ ] Stage-object types are found only from live objects; a code search over the `+0x94` table
  would also need each type's sprite source.
- [ ] Visual and misc types (Dr. Light capsule 128, savable Reploid 141): their banks are drawn, but
  their animations are not exported (the exporter covers main types only).
- [ ] Yammark type 15 (Nightmare Virus, shared code in the main program): its tables use sprites up
  to 175, but its bank has 43; find where its other sprites come from. Its frames look speckled:
  probably its semi-transparent look (object flag `0x10`); also check its remap mode (byte 0).
- [ ] Exporter minors from the final review: record a cut table ("kept k of n") in the output; add
  the table to the `sources` merge key; say `--root` in the folder messages; turn a read past the
  end of RAM into exit 1; a name removed from the names file leaves the folder under that name.
- [ ] Free-standing effect objects (visual and misc tables) are not exported.

## Deferred minor findings (from reviews)

- `stage.png` draws colour 0 as black, so layer-0 tiles hide layers 1–2 in
  their whole 16 × 16 area; add a README note, or fix with palettes.
- `learn` reads and hashes all ~51 screen-set members; a bad unrelated member
  blocks learning.
- Layouts with fewer than 10 horizontal joins (small rooms such as boss rooms)
  are refused by `learn`.
- Palettes: done for the intro (task H). Still open: palettes that change
  during play (the colours are those at the save point), semi-transparent
  pixels (bit 15), and the check on a second stage (Yammark).
- Overlapping 4 bpp / 8 bpp texture cells: explained. The same VRAM bytes are
  read as 4 bpp and as 8 bpp (process log section 44).
- `learn` accepts an object list with 0 objects (pointer at an end record).
- `stages` `_validate` accepts a negative or `true` object offset or count; the
  run still fails, but with a confusing message.
- A missing object overlay file gives an `error:` line without the stage name.
- Running `learn` again without `--note` deletes the save state's note.
- `find_list` keeps searching after the second hit (no visible cost).
- `learn` writes nothing when the palette step fails, so a stage whose VRAM
  textures differ from the disc (for example animated tiles) cannot be learned
  at all; add a fallback entry without `palettes`.
- A hand-edited `palettes` field that points outside VRAM crashes `stages`
  with a traceback (`learn` never writes one).
- No `palettes` test has a layout value at or above the screen count (the code
  guards it; the real intro covers it).
- `stages` reads the save state twice (once for the SHA-1, once to decompress).
