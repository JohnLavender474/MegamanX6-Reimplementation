# Stage: intro (stage 0, overlay 02)

The opening stage. DAT member 94, 57 object records in one area. Overlay `02` at RAM `0x800E9860`.
Save states: `intro-1_start`, `intro-2_junkroid`, `intro-3_boss`, `intro-4_hunters-text` (text 2),
`intro-5_alia-text` (text 3, after the D-1000), `intro-6_high-max-text` (text 4) (in
`~/mmx-data/x6/dumps/`).

The record format, the kinds and the marker colours are in `docs/objects-legend.md`. The shared
object rules are in `docs/engine.md`. The enemies and objects have their own sheets in
`docs/enemies/`: `junkroid`, `hover-gunner`, `meta-wheel-f`, `meta-dridler`, `d-1000`,
`sabre-block`, `boss-gate`.

## Objects (left to right)

| No. | Colour | kind | b | id | a | x | y | Screen | Reading | Status |
|---|---|---|---|---|---|---|---|---|---|---|
| 54 | teal | 5 | 10 | 0 | 0 | 40 | 1105 | 0,4 | scenery: a large wrecked claw (misc type 10, sprite type 145); removed at checkpoint ≥ 2 (see "Events") | confirmed (game code) |
| 15 | yellow | 3 | 14 | 0 | 0 | 144 | 1104 | 0,4 | talk zone, forced: Alia's opening talk (text 0); box x 64–192, y 944–1136 (see "Talk zones") | confirmed (game code) |
| 25 | red | 0 | 1 | 1 | 0 | 416 | 1080 | 1,4 | Junkroid; turns to face X | confirmed |
| 20 | green | 1 | 5 | 1 | 0 | 560 | 992 | 2,3 | Hover Gunner (type 5), from difficulty 1 up; enters from the left (id 1) | plausible |
| 26 | red | 0 | 1 | 1 | 0 | 608 | 1080 | 2,4 | Junkroid; turns to face X | confirmed |
| 27 | red | 0 | 1 | 0 | 0 | 800 | 1048 | 3,4 | Junkroid, dormant (id 0): keeps its spawn facing and never shoots; forced active from difficulty 2 | confirmed |
| 28 | red | 0 | 1 | 1 | 0 | 1024 | 1048 | 4,4 | Junkroid; turns to face X | confirmed |
| 29 | red | 0 | 1 | 1 | 0 | 1696 | 984 | 6,3 | Junkroid; turns to face X | confirmed |
| 55 | teal | 5 | 15 | 0 | 0 | 1880 | 1026 | 7,4 | scenery: a small purple wreck (misc type 15, sprite type 146); removed at checkpoint ≥ 2. It is not the "!" hint | confirmed (game code) |
| 30 | red | 0 | 1 | 1 | 0 | 1888 | 984 | 7,3 | Junkroid; turns to face X | confirmed |
| 16 | yellow | 3 | 14 | 129 | 0 | 1920 | 992 | 7,3 | talk zone, optional: the "!" icon; Select opens Alia's sabre hint (text 1); box x 1920–1984, y 944–1008 | confirmed (game code) |
| 40 | purple | 4 | 2 | 0 | 0 | 2016 | 984 | 7,3 | sabre block, on the ladder top | confirmed |
| 21 | blue | 2 | 5 | 1 | 0 | 2112 | 1056 | 8,4 | Hover Gunner (type 5), difficulty 2 and up only | plausible |
| 0 | yellow | 3 | 6 | 1 | 0 | 2112 | 1088 | 8,4 | checkpoint marker: checkpoint 1 when X reaches x 2112 | confirmed (game code) |
| 3 | yellow | 3 | 0 | 129 | 0 | 2112 | 1088 | 8,4 | camera zone 1, one use: min x 1968 and the texture swap (see "Camera") | confirmed (game code) |
| 9 | yellow | 3 | 0 | 7 | 0 | 2128 | 1088 | 8,4 | camera zone 7: min y 896 | confirmed (game code) |
| 2 | yellow | 3 | 0 | 0 | 0 | 2288 | 1088 | 8,4 | camera zone 0: min y 944 | confirmed (game code) |
| 31 | red | 0 | 2 | 1 | 0 | 2416 | 1104 | 9,4 | Meta Wheel F; rolls left slowly; Z-sabre only | confirmed |
| 32 | red | 0 | 2 | 1 | 0 | 2496 | 1104 | 9,4 | Meta Wheel F; rolls left slowly; Z-sabre only | confirmed |
| 41 | purple | 4 | 2 | 0 | 0 | 2848 | 1080 | 11,4 | sabre block | confirmed |
| 33 | blue | 2 | 2 | 1 | 0 | 3040 | 1104 | 11,4 | Meta Wheel F (type 2), difficulty 2 and up only | plausible |
| 14 | yellow | 3 | 0 | 12 | 0 | 3072 | 1072 | 12,4 | camera zone 12: min y 944 | confirmed (game code) |
| 42 | purple | 4 | 2 | 0 | 0 | 3104 | 1112 | 12,4 | sabre block | confirmed |
| 34 | red | 0 | 2 | 1 | 0 | 3280 | 1104 | 12,4 | Meta Wheel F; rolls left slowly; Z-sabre only | confirmed |
| 4 | yellow | 3 | 0 | 2 | 0 | 3632 | 1088 | 14,4 | camera zone 2: max x 5120, min y 704 | confirmed (game code) |
| 5 | yellow | 3 | 0 | 3 | 0 | 3776 | 808 | 14,3 | camera zone 3: max y 704; the trigger is at x 4000–4031, 224 px right of the record | confirmed (game code) |
| 10 | yellow | 3 | 0 | 8 | 0 | 3776 | 824 | 14,3 | camera zone 8: max y 944 (trigger x 3968–3999) | confirmed (game code) |
| 35 | red | 0 | 2 | 1 | 0 | 3872 | 784 | 15,3 | Meta Wheel F; rolls left slowly; Z-sabre only | confirmed |
| 36 | blue | 2 | 2 | 1 | 0 | 3936 | 784 | 15,3 | Meta Wheel F (type 2), difficulty 2 and up only | plausible |
| 43 | purple | 4 | 2 | 0 | 0 | 4000 | 792 | 15,3 | sabre block | confirmed |
| 37 | blue | 2 | 2 | 1 | 0 | 4064 | 784 | 15,3 | Meta Wheel F (type 2), difficulty 2 and up only | plausible |
| 45 | red | 0 | 0 | 0 | 0 | 4384 | 736 | 17,2 | Meta Dridler (indestructible); drops from the upper cavity and rises again | confirmed |
| 46 | red | 0 | 0 | 1 | 0 | 4672 | 736 | 18,2 | Meta Dridler (indestructible); drops from the upper cavity and rises again | confirmed |
| 22 | green | 1 | 5 | 0 | 0 | 4832 | 808 | 18,3 | Hover Gunner (type 5), from difficulty 1 up; enters from the right (id 0) | plausible |
| 23 | blue | 2 | 5 | 0 | 0 | 4832 | 832 | 18,3 | Hover Gunner (type 5), difficulty 2 and up only | plausible |
| 47 | green | 1 | 0 | 2 | 0 | 4896 | 736 | 19,2 | Meta Dridler (type 0), from difficulty 1 up; not noticed in play | plausible |
| 48 | red | 0 | 0 | 3 | 0 | 4960 | 736 | 19,2 | Meta Dridler (indestructible); drops from the upper cavity and rises again | confirmed |
| 50 | red | 0 | 0 | 0 | 0 | 5088 | 368 | 19,1 | Meta Dridler (indestructible); drops from the upper cavity and rises again | confirmed |
| 38 | blue | 2 | 2 | 0 | 0 | 5088 | 624 | 19,2 | Meta Wheel F (type 2), difficulty 2 and up only | plausible |
| 6 | yellow | 3 | 0 | 4 | 0 | 5184 | 816 | 20,3 | camera zone 4: min x 5008, min y 256 (trigger x 5200–5231) | confirmed (game code) |
| 11 | yellow | 3 | 0 | 9 | 0 | 5184 | 816 | 20,3 | camera zone 9: min x 3136, y 704 (trigger x 5152–5168; on the way back) | confirmed (game code) |
| 51 | red | 0 | 0 | 0 | 0 | 5200 | 304 | 20,1 | Meta Dridler (indestructible); drops from the upper cavity and rises again | confirmed |
| 8 | yellow | 3 | 0 | 6 | 0 | 5232 | 400 | 20,1 | camera zone 6: max x 5120 (on the way back) | confirmed (game code) |
| 24 | red | 0 | 5 | 0 | 0 | 5248 | 400 | 20,1 | Hover Gunner (type 5) on every difficulty; with id 0 it flies left at 4 px/frame as soon as it spawns, so it is never seen at its marker | confirmed (game code) |
| 7 | yellow | 3 | 0 | 5 | 0 | 5264 | 400 | 20,1 | camera zone 5: max x 5424 | confirmed (game code) |
| 52 | red | 0 | 0 | 0 | 0 | 5312 | 304 | 20,1 | Meta Dridler (indestructible); drops from the upper cavity and rises again | confirmed |
| 49 | red | 0 | 0 | 0 | 0 | 5344 | 576 | 20,2 | Meta Dridler (indestructible); drops from the upper cavity and rises again | confirmed |
| 13 | yellow | 3 | 0 | 11 | 0 | 5352 | 400 | 20,1 | camera zone 11: max y 704 (on the way back) | confirmed (game code) |
| 12 | yellow | 3 | 0 | 10 | 0 | 5368 | 400 | 20,1 | camera zone 10: max y 256 | confirmed (game code) |
| 39 | blue | 2 | 2 | 1 | 0 | 5408 | 416 | 21,1 | Meta Wheel F (type 2), difficulty 2 and up only | plausible |
| 53 | red | 0 | 0 | 0 | 0 | 5424 | 304 | 21,1 | Meta Dridler (indestructible); drops from the upper cavity and rises again | confirmed |
| 44 | purple | 4 | 2 | 0 | 0 | 5488 | 424 | 21,1 | sabre block | confirmed |
| 18 | purple | 4 | 8 | 2 | 0 | 5744 | 384 | 22,1 | boss gate, first | confirmed |
| 1 | yellow | 3 | 6 | 2 | 0 | 5760 | 376 | 22,1 | checkpoint marker: checkpoint 2 when X reaches x 5760 | confirmed (game code) |
| 17 | teal | 5 | 8 | 0 | 0 | 6000 | 416 | 23,1 | the Hunters' scene: two injured Hunters talk with X and teleport out (misc type 8, text 2; see "Events") | confirmed (game code) |
| 19 | purple | 4 | 8 | 1 | 0 | 6064 | 384 | 23,1 | boss gate, second (before the boss) | confirmed |
| 56 | red | 0 | 48 | 0 | 0 | 6162 | 346 | 24,1 | D-1000 (boss); the body ignores this point and starts at (6288, 160), above the screen | confirmed (game code) |

The overlay tables of the intro [certain]: stage logic `0x800FA6E0` (scratchpad `+0x8C`; 0
`CameraZone`, 3 `PaletteCycle`, 6 `CheckpointMarker`, 14 `TalkZone`), stage objects `0x800FA794`
(`+0x94`; 2 sabre block), misc `0x800FA814` (`+0x90`; 8 Hunters, 10 and 15 wrecks).

## Camera [certain for the data]

`camera_zone_tbl` word 0 (stage 0 area 0) = `0x800791F0`: 13 entries (ids 0–12; the stage 1 list
starts at `0x800792C0`). Each entry is used by exactly one record. The table in RAM is the same as
in the EXE. The rules are in `docs/engine.md`, "Camera". Limits are the camera top-left position;
the view is 320 × 240.

| entry | `id` | record (x, y) | trigger x | trigger y | min x | max x | min y | max y |
|---|---|---|---|---|---|---|---|---|
| `0x800791F0` | 0 | 2 (2288, 1088) | 2272–2303 | 1040–1135 | keep | keep | 944 | keep |
| `0x80079200` | 1 | 3 (2112, 1088), `id` 129 | 2128–2159 | 1040–1135 | 1968 | keep | keep | keep |
| `0x80079210` | 2 | 4 (3632, 1088) | 3616–3647 | 1040–1135 | keep | 5120 | 704 | keep |
| `0x80079220` | 3 | 5 (3776, 808) | 4000–4031 | 752–815 | keep | keep | keep | 704 |
| `0x80079230` | 4 | 6 (5184, 816) | 5200–5231 | 752–895 | 5008 | keep | 256 | keep |
| `0x80079240` | 5 | 7 (5264, 400) | 5248–5279 | 352–479 | keep | 5424 | keep | keep |
| `0x80079250` | 6 | 8 (5232, 400) | 5216–5247 | 352–479 | keep | 5120 | keep | keep |
| `0x80079260` | 7 | 9 (2128, 1088) | 2112–2143 | 1040–1135 | keep | keep | 896 | keep |
| `0x80079270` | 8 | 10 (3776, 824) | 3968–3999 | 752–815 | keep | keep | keep | 944 |
| `0x80079280` | 9 | 11 (5184, 816) | 5152–5168 | 752–895 | 3136 | keep | 704 | 704 |
| `0x80079290` | 10 | 12 (5368, 400) | 5360–5375 | 352–447 | keep | keep | keep | 256 |
| `0x800792A0` | 11 | 13 (5352, 400) | 5344–5359 | 352–447 | keep | keep | keep | 704 |
| `0x800792B0` | 12 | 14 (3072, 1072) | 3072–3103 | 1072–1135 | keep | keep | 944 | keep |

- **Zone 1** (record 3, `id` 129) sets min x 1968 and `stage_texture_swap` = 1, then deletes itself
  (one use). Member 94 entry `0x16` then replaces entry `0x10000` at VRAM (320, 256): in `intro-1`
  and `intro-2` entry `0x10000` is in VRAM and the swap flag is 0; in `intro-3_boss` entry `0x16`
  is in VRAM and the flag is 2.
- **Zone 3** (record 5): the record is at x 3776, the trigger at x 4000–4031. The zone deletes
  itself when the camera x is more than 3840, so it acts only while the camera is at most 3840 when
  X is at x 4000 [likely that it works in play; not seen].
- The live limits agree: `intro-1` and `intro-2` have x 0–5120, y 896–944 (the checkpoint 0
  values); `intro-3_boss` has x 6064–6144, y 224 (boss gate 19).

The camera along the stage:

| part | player x | camera x | camera y | set by |
|---|---|---|---|---|
| start corridor | 144 → 2128 | 0–5120 | 896–944 | checkpoint 0 |
| after the swap | 2128 → 2272 | 1968–5120 | 896–944 | zone 1; zone 7 sets min y 896 again on the way back |
| lower corridor | 2272 → 3616 | 1968–5120 | 944 | zone 0; zone 12 at x 3072 sets min y 944 again |
| rise to the upper ledge | 3616 → 4000 | 1968–5120 | 704–944 | zone 2 |
| upper ledge corridor | 4000 → 5152 | 1968–5120 | 704 | zone 3; zone 8 sets max y 944 again on the way back |
| shaft up | 5200 → (y 352–479) | 5008–5120 | 256–704 | zone 4; zone 9 on the way back: min x 3136, y 704 |
| upper path | 5248 → 5744 | 5008–5424 | 256 | zone 5 (5424 + 320 = 5744 = gate 18); zone 10 sets max y 256; zones 6 and 11 restore 5120 and 704 on the way back |
| gate 1 hallway | 5744 → 6064 | 5744 | 224 | boss gate 18; checkpoint 2 |
| boss room | 6064 → | 6064–6144 | 224 | boss gate 19 |

When X goes back left from the shaft, zone 9 lets the camera go left only to x 3136, not to 1968
[certain for the value; the effect in play is a guess].

## Checkpoints [certain]

Records 0 and 1 are `CheckpointMarker`s (`docs/engine.md`, "Checkpoints"). `checkpoint_start_tbl`
word 0 = `0x80073134`, 3 records:

| cp | record | player x, y | camera 0 | camera 1 | camera 2 | limits x | limits y | facing | `beam_thru` |
|---|---|---|---|---|---|---|---|---|---|
| 0 | `0x80072108` | 144, 976 | 0, 944 | 0, 1032 | 0, 876 | 0–5120 | 896–944 | right | 1 |
| 1 | `0x8007212C` | 2112, 1072 | 1968, 944 | 984, 1032 | 492, 876 | 1968–5120 | 944 | right | 6 |
| 2 | `0x80072150` | 5904, 364 | 5744, 224 | 2872, 672 | 1436, 696 | 5744 | 224 | right | 1 |

Camera 1 and 2 are the values after the offsets: layer 1 (0, 560) and layer 2 (0, 640) in all three
records. The live layer cameras of `intro-1` and `intro-3` agree. X starts at x 144, not at the x 40
of record 54.

## Talk zones [certain for the code]

Stage logic 14 (`TalkZone`, `docs/engine.md`, "Talk zones"), talk index k 0:

| record | `id` | type | box (x, y) | text |
|---|---|---|---|---|
| 15 | 0 | forced | 64–192, 944–1136 | 0: Alia and X. Mechaniloids have become Mavericks; the Falcon Armor dash is not complete; "Go to the entrance of the ruins ahead!" [likely for the text] |
| 16 | 129 | optional ("!", Select) | 1920–1984, 944–1008 | 1: Alia's hint: if the buster fails, use the Z-Saber (sabre block 40 is at x 2016) [likely for the text] |

- The player starts at (144, 976), inside box 0. The zone's `PlayerEvent(20)` acts when X stands, so
  text 0 opens just after the beam-in [likely]. In all three states the seen word is 1 and record 15
  is gone.
- Record 16 is still live in `intro-3_boss` (not used: bit 1 is clear).
- The other intro texts: 2 the Hunters (record 17), 3 Alia "Z...Zero!?" (opened by Zero, misc 16,
  at `0x800F9F14`), 4 and 5 X and High Max (`0x800F23DC`, `0x800F301C`), 6 X alone after High Max
  (`0x800F657C`). The full text is in "Texts".

## Backgrounds [certain unless marked]

Kind 3 b 14 is not a background setting (see "Talk zones"). The BG scroll never changes in the
intro:

| layer | mode | x | y | `intro-1` (layer 0 at 0, 944) | `intro-3` (layer 0 at 6064, 224) |
|---|---|---|---|---|---|
| 1 | 1 | layer 0 x / 2 + 0 | layer 0 y / 2 + 560 | 0, 1032 | 3032, 672 |
| 2 | 3 | layer 0 x / 4 + 0 | layer 0 y / 4 + 640 | 0, 876 | 1516, 696 |

`intro-2` (layer 0 at 183, 944) gives layer 1 (91, 1032) and layer 2 (45, 876). The modes come from
`bg_mode_tbl` (intro 1, 3) and the offsets from the checkpoint records. Both modes have the 16
px/frame follow cap (`docs/engine.md`, "Camera").

**The art sets.** The visible change near x 2000 is the texture swap of zone 1, not a scroll change.
What each layer shows (layer px, from `stage.ora` and a render with entry `0x16`):

| layer | x, y | art | texture | seen at layer 0 x |
|---|---|---|---|---|
| 0 | to x ≈ 1600 | outdoors | `0x10000` | |
| 0 | from x ≈ 1600 | the base inside | `0x16` | |
| 1 | 0–960, 1104–1248 | desert rocks | `0x10000` | 0–1920 |
| 1 | 1088–1698, 994–1280 | dark blue machines | `0x16` | about 1536–3396 |
| 1 | 1713–2976, 688–1260 | dark red reactor pillars; at x 2560–2976, y 688–930 a purple night sky with a wreck | `0x16` | about 2786–5952 |
| 2 | 0–656, 848–1088 | sunset sky with the colony wreck | `0x10000` | ≤ 2624 |
| 2 | 1504–1968, 656–896 | purple night sky with a wreck (the boss room background) | `0x16` | ≥ about 4736 |

Columns 12–25 of layers 1 and 2 are empty. After zone 1 the camera min x is 1968, so the outdoor art
cannot come back into view [likely]. The limits of each art area are bounding boxes of the opaque
pixels, not checked in play.

**Palette cycles.** The second list (`0x800FA3E4`) has 7 `PaletteCycle` objects (stage logic 3,
`id` 0–6), all live in `intro-1`:

| `id` | target palette | script (source palette: updates) | loop |
|---|---|---|---|
| 0 | 99 | 0, 1, 2, 3, 2, 1: 8 each | 48 updates |
| 1 | 96 | 4, 5: 1 each | 2 |
| 2 | 98 | 6, 7, 8, 7: 8 each | 32 |
| 3 | 97 | 9, 10, 11, 10: 15 each | 60 |
| 4 | 100 | 12–16–13: 15 each | 120 |
| 5 | 101 | 17, 18: 1 each | 2 |
| 6 | 183 | 18, 17: 1 each | 2 |

Which tiles use these palettes was not checked.

**Unused BG switch.** `IntroBgSwitch` (`0x800E9B90`, kind-7 type 0): when `mega.x` > 2048, it sets
layers 1 and 2 to mode 2 (1:1 with layer 0, offsets 0) and requests a redraw; else it restores modes
1 and 3. No intro record has kind 7, the kind-7 slots are empty in all three states, and `intro-3`
still has modes 1 and 3. It is probably left over from development [likely].

## Events [certain for the code]

- **Stage start.** No record drives it. `CheckpointStart` places X and the cameras, then the READY
  sequence runs and X beams in (`docs/engine.md`, "Stage start"). Then talk zone 0 opens text 0.
- **Wrecks** (records 54 and 55, misc types 10 and 15; `IntroWreckA` `0x800F9570`, `IntroWreckB`
  `0x800F9718`). State 0: when `game.checkpoint` ≥ 2, `DeleteObject3`; else the sprite of type 145
  (bank 7, a large brown wrecked claw, about 96 × 64) or 146 (bank 8, a small purple wreck, about 40
  × 32), sprite 0, priority 19. State 1: `DisplayCheck` (48 × 32 or 32 × 32) each frame; deleted
  at checkpoint ≥ 2. No box, no trigger; the record `id` is not read. In `intro-1` and `intro-2`
  record 54 is live; in `intro-3_boss` (checkpoint 2) it is gone.
- **The Hunters' scene** (record 17, misc type 8, `IntroHunters` `0x800F9274`; set-up
  `IntroHuntersInit` `0x800F90FC`; states `0x800FBB3C`, steps `0x800FBB48`, animations
  `0x800FBB2C`; sprite type 132 = bank 6: sprites 0–5 two Hunters, 6–10 a purple beam, 11–15 a
  green beam).
  - Set-up: with `var` 0 and checkpoint 2 it stays in state 0 for ever (invisible: the scene is
    skipped after a respawn at checkpoint 2). Else x 5968, y 404 (the record point is not used),
    priority 3, facing left, `speed_y` 8.0. `var` 0: animation 0, step 0. `var` 1: x + 20,
    animation 1, step 5.
  - Steps of `var` 0: 0 `PlayerEvent(20, 0x40)` (hold, face right); 1 when player step 20:
    `PlayerEvent(29, 0x40)` (walk right at 1 px/update); 2 when `mega.x` ≥ 5824:
    `PlayerEvent(20, 0x40)` and `TextOpen(2, 255, 0)`; 3 when `text_open` = 0: spawn misc 8 `var` 1
    (the green Hunter's beam at x 5988), `SetAnime(3)` (one Hunter left), wait 20 updates; 4
    `SetAnime(2)` (the purple beam); 5 frame flag 1 (after 8 updates): `PlaySound(5, 0)`; flag 2
    (after 10): rise at 8 px/update. When y < 192: `PlayerEventEnd` (`var` 0 only), then
    `ClearAllEnemies` (all 48 main and 32 visual objects) and `DeleteObject3`.
  - Only the player is locked (`PlayerEvent`); no stop flags are set.
  - From the end of the text to `PlayerEventEnd`: about 57 updates.
  - The scene starts when the record enters the spawn band (camera x ≥ 5632), during the pass
    through gate 1. In `intro-3_boss` record 17 has flags `01` and no misc 8 is live, so the scene
    ran [likely].

- **The D-1000 fight and death** (`docs/enemies/d-1000.md`, "Entry" and "Death"). After gate 19 the
  WARNING (stage logic 24) runs and loads the D-1000 pack (member 123); the boss falls in, the core
  appears, the HP bar fills. After the kill: blink, the first big explosion (stage logic 26), the
  white screen (X is put 160 px from the wreck), then Zero.
- **Zero** (misc type 16, `IntroZero` `0x800F9B70`; states `0x800FBC3C`, steps `0x800FBC48`,
  animations `0x800FBC2C`). Made by the D-1000 death step 3 when the first explosion ends. Texture
  = the D-1000 texture `0x1E0` + `0x300` = `0x4E0` (member 123), palette and sprites of sprite type
  137 (bank 12, palette `0x7940`); a purple "Zero Nightmare" palette [likely]. Animations: 0 beam
  (sprites 0, 1), 1 forming (2–14, 40 updates), 2 slash (14–26, 22 updates; flag 2 at sprites 18,
  19, 20, 22, 24), 3 un-forming (14 → 2).

  | step | routine | what |
  |---|---|---|
  | set-up | `0x800F9BAC` | facing = the player's; x = D-1000 x − 96 (player faces right) or + 96, so 64 px in front of X; y = camera y − 32; priority 3; animation 0; timer 60 |
  | 0 | `0x800F9CC4` | after 60 updates: sound (2, 10); falls 8 px/frame |
  | 1 | `0x800F9D24` | stops at camera y + 170 (about 26 updates); animation 1 |
  | 2 | `0x800F9D88` | at its flag 1 (about 41 updates): animation 2, sound (2, 102) |
  | 3 | `0x800F98C0` | each flag 2: the wreck flashes; at sprites 18, 20, 24 also a slash flash (ray type 15) at x ± 56, sound (2, 104). At flag 1: the D-1000 goes to death step 5 (the second blink and explosion); timer 120 |
  | 4 | `0x800F9DE0` | after 120 updates: sound (2, 11), animation 3 |
  | 5 | `0x800F9E44` | at its flag 1: animation 0, rises 8 px/frame |
  | 6 | `0x800F9E94` | until it is above the view (about 28 updates) |
  | 7 | `0x800F9EE8` | waits until the D-1000 is deleted (the end of explosion 2); `TextOpen(3)` |
  | 8 | `0x800F9F3C` | waits for `text_open` = 0 |
  | 9 | `0x800F9A54` | `IntroHighMaxSpawn`: player HP at least 2; main type 55 `var` 0 at (6162, 346), texture `0x1E0`, palette and sprites of bank 55; `game+0x26` = 1; Zero is deleted |

- **High Max** (`var` 0, `docs/enemies/high-max.md`, "The intro version"). A second WARNING loads
  his pack (member 124, over the D-1000 and Zero art). He comes down, text 4, the HP bar fills, and
  the fight runs for 900 updates. He cannot be hurt, and the player's HP cannot fall below 1. Then
  INTRO END: text 5, he flies off, text 6.
- **The end of the stage.** `game.exit_req` = `0x40`. `PlayerExit` sets `mega.state` = 3 (no
  beam-out); the fade-out; `StageExit` (stage 0 branch, `0x8001E38C`): `game+0x1E` = 0, `game+0x66`
  = 1, game mode 11 (the report screen: stage 15, overlay 20). What the report screen does with
  `game+0x66` = 1 was not read.

## Texts

Bank: DAT member 203 (X) or 204 (Zero; the same messages 0–5, no message 6), the format in
`docs/engine.md`, "Text". All messages use the bottom box, and every page ends with the arrow. The
first line of each page is the speaker's name.

| msg | header | portraits | used by |
|---|---|---|---|
| 0 | `0x8001` | Alia left, X right | talk zone record 15 |
| 1 | `0x8001` | Alia left, X right | talk zone record 16 |
| 2 | `0xC010` | X alone | the Hunters (record 17) |
| 3 | `0xC011` | Alia alone | Zero, after the D-1000 |
| 4 | `0x8020` | X left, High Max right | High Max INTRO |
| 5 | `0x8020` | X left, High Max right | High Max INTRO END |
| 6 | `0xC010` | X alone | High Max INTRO END |

```
msg 0
  Alia: Most of the / Mechaniloids became
  Alia: Mavericks... It's / been a long time...
  Alia: Stay alert X!
  X: I'll be fine since / I have the Armor
  X: and Saber.
  Alia: Since I fixed the / Falcon Armor very
  Alia: quickly, the dash / function is
  Alia: incomplete.
  X: All right.
  X: Anyway, I will stop / the Mechaniloids.
  Alia: OK.
  Alia: Go to the entrance / of the ruins ahead!
msg 1
  Alia: If the buster fails, / use the Z Saber.
  X: Yes. It is the time / to ask for
  X: Zero's power.
msg 2
  X: There you are! / Are you all right?
  Hunters: Beyond here is / a giant
  Hunters: Mechaniloid...
  Hunters: It suddenly / activated due to a
  Hunters: radio wave problem.
  Hunters: We are afraid and / we got hurt...
  X: I'll take care of / this.
  X: Return to the Hunter / Base now.
  Hunters: All right.
  Hunters: It acts like it's / possessed.
  Hunters: Be careful...
msg 3
  Alia: Z...Zero!?
msg 4
  ??: I knew Zero was / doing this.
  X: Who are you!?
  ??: High Max... / I am called...
  High Max: I'm investigating / the Zero Nightmare.
  X: What! Did you say / the Zero Nightmare?
  High Max: What are you trying / to do?
  High Max: What is your / connection to the
  High Max: Zero Nightmare?
  X: What about Zero? / You are behind this!
  High Max: You are dangerous... / I'll dispose of you.
msg 5
  High Max: Maverick Hunter... / Not amazing.
  High Max: We will destroy the / Zero Nightmare.
  High Max: You are not wanted / here. Keep out.
msg 6
  X: I could not inflict / any damage on him...
  X: High Max. / The Zero...
  X: Night...mare...
  X: Hmm, did I dream / about Zero?
  X: What is happening?
```

One line per page; " / " separates the lines of a page.

## Collision

Rules: `docs/engine.md`, "Tile collision"; values: `catalog/collision.json`. Intro counts (layer 0,
cells with a tile): 0: 2845, 1–4: 6 each, 5–8: 7 each, 16: 52, 32: 7, 33: 1, 34: 2, 45: 17, 56:
5200, 57: 961, 59: 20, 63: 21 [certain].

- **Slopes 1–4** (rise to the right): two slopes in the east, rows at y 448–480, x 5168–5359, and
  rows at y 848–880, x 5120–5311. **Slopes 5–8** (fall to the right): east at y 656–672, x
  5120–5247; west at y 1024–1088, x 1120–1439 (five pieces). Value 16 is under each slope piece.
- **The ladder**: 32 at x 2000, y 1024–1135; the top 33 at (2000, 1008), with 34 at (1984, 1008)
  and (2016, 1008).
- **45**: a flat hanging bar, one row at y 1008, x 2368–2639, 32 px under a ceiling.
- **59**: the gate columns at x 5728–5759 and 6048–6079 (2 × 4 cells each) and x 6512 (1 × 4), y
  352–415 (boss gates 18 and 19).
- **63** (spikes): all 21 cells are inside solid ground (20 two rows under the east slopes, 1 at
  (1872, 1168)). The player cannot reach them [certain for the map; why they are there is not
  known].

Check pictures (one per value, red over the dimmed art):
`/tmp/claude-1000/-home-johnlavender-IdeaProjects-mmx-demake-tools/fcc5057d-eac8-4847-9af6-1eb26006b9cc/scratchpad/agent-collision/m094-value-NN.png`
(scratch; make again with the `stages` tool when it can draw them, `docs/TODO.md`).

## How Junkroid 27 works (from the game code)

Read from the intro's code (overlay 02 at RAM `0x800E9860`) by disassembly on
2026-10-19; the addresses agree with the X4 decompilation's spawn code.

- **Spawn** (`0x80029f38`): the record's `b` and `id` are copied into the live
  object (bytes 1 and 2); the object also keeps a pointer to its record.
  Objects of kind 0–2 are skipped when the difficulty (`0x800ccf38`) is below
  the kind, except at difficulty 3 (`0x80029e7c`).
- **Junkroid set-up** (`0x800eaffc`): Junkroid faces X once (`0x800eb064`). At
  difficulty 2 or higher, its `id` is forced to 1 (`0x800eb0e4`).
- **Every frame** (`0x800eac8c`): only when `id` ≠ 0 does it turn to face X.
- **Idle** (`0x800eb14c`): with `id` = 0 it returns at once; otherwise it starts
  its shooting loop when X is within 112 px.

So Junkroid 27 is dormant by design on lower difficulties: it faces X's side at
the moment it spawns (left, because X comes from the left) and never acts.

## Meta Dridler rock blocks

Records 46, 47 and 48 (Meta Dridler `id` 1–3) can copy a rock block from a template 256 px lower in
the stage to the floor under the drill. The cells are in `docs/enemies/meta-dridler.md`. It is not
known whether this happens in play.

## The D-1000 boss room [certain for the numbers]

- The body ignores the record position (6162, 346): it always starts at **x 6288, y 160**, above the
  screen.
- Standing height: body origin at y 304. Crouched (after landing, during the crouch attack and the
  stun): about y 352, on the floor.
- The room walls are two map columns at x 6064 (left) and x 6448 (right), from y 208, 13 cells high.
  The game changes them by copying 1 × 13-cell columns from templates outside the room:

  | template | x | used for |
  |---|---|---|
  | normal wall, left | 6512 | after a wall ram, at the stun, at death |
  | dented wall, left | 6528 | when the ram hits the left wall |
  | normal wall, right | 6560 | after a wall ram, at the stun, at death |
  | dented wall, right | 6576 | when the ram hits the right wall |

- The core appears at **x 6368 or x 6160, y 256**: on the side away from X (X left of x 6264 → core
  on the right, at 6368; else on the left, at 6160). The first time, it is at 6368.

## Boss gates

Record 18 (`id` 2, x 5744) fixes the camera at x 5744, y 224. Record 19 (`id` 1, x 6064) is the gate
before the boss: camera x 6064–6144. Both are checked in the save state
(`docs/enemies/boss-gate.md`).

## Open questions

- **The difficulty filter** (from the game code, not yet seen in play): play
  the intro on the hardest difficulty and look at the blue markers (21, 23, 33,
  36–39). Extra Meta Wheel Fs and Hover Gunners there would confirm it. Also: which
  difficulty name goes with each value (the save state has 1, probably Normal).
- **The `id` byte** of Meta Wheel F: all records have 1 except record 38 (0). The code reads it
  (0 = rolls right, else left); check record 38 in play.
- **The Hunters' scene and checkpoint 2.** Checkpoint marker 1 (x 5760) sets checkpoint 2, and the
  scene is skipped at checkpoint 2. If the gate pass moves X past x 5760 before the camera reaches x
  5632, the scene would be skipped on the first visit; `intro-3_boss` shows that it was not.
  Something in the gate pass may spawn the record first.
- **Respawn at checkpoint 1 or 2:** is texture entry `0x16` loaded again (the swap flag is reset at
  the stage start)? Needs a save state after a death past x 2112.
- **Behind the BG:** what is drawn where layers 1 and 2 are transparent (the GPU clear colour or a
  backdrop).
- **Zone 3** (record 5): check in play that it acts.
- **`0x80097428`**: while it is not 0, the talk zones delete themselves and the HUD is skipped
  [guess: a demo flag].
- **Scene sprites** (exports in `~/mmx-data/x6/sprites/enemies/`) [certain]:
  - `zero-intro` (from `intro-5_alia-text`): Zero, misc 16, bank 12 (type 137), mode `0x40`, 27
    sprites: the beam, the forming, the slash. His colours there are pink and purple (CLUT
    `0x7940`), so the palette at that moment may be a beam-in palette [guess].
  - `hunters` (from `intro-4_hunters-text`): the hurt Hunters, misc 8, bank 6 (type 132), 16 sprites.
  - `portrait-x-falcon`, `portrait-alia`, `portrait-high-max`: the text portraits (8 bpp; values in
    `docs/engine.md`, "Text").
  - High Max: the shared export `high-max` (`docs/enemies/high-max.md`).
  - These are sheets with no animations: the animation tables of misc objects are not exported.
- **Live checks in `intro-4` to `intro-6`** [certain]: in `intro-6_high-max-text` the member 124
  texture is in VRAM at (576, 16) (0 of 1024 rows differ) and High Max is main slot 0 (type 55,
  `var` 0, flags `0x41`). In `intro-5_alia-text` the member 123 (D-1000) texture is still in VRAM and
  Zero (misc 16) is still live at (6333, 170) at the last page of text 3. In `intro-4_hunters-text`
  neither boss pack is in VRAM.
- **Game mode 11 after the intro** (`game+0x66` = 1): what the report screen shows next.
