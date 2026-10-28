# Object legend

Status: started 2026-10-19 on the intro stage (DAT member 94, 57 objects).
Sources: the user's play-through of the intro, a gameplay video, the object
records (`objects.json`, written by `stages`), and a disassembly of Junkroid's
code. Statuses:

- **confirmed**: seen in the game at that marker;
- **plausible**: fits the data and the game, not yet seen directly;
- **unknown**: no good reading yet.

## The record

8 bytes per object: `a`, `b`, `id`, `kind` (one byte each), then `x` and `y`
(2 bytes each, little-endian, stage pixels from the top-left).

| Field | Meaning | Status |
|---|---|---|
| `kind` | low digit: category; for 0–2 also the **minimum difficulty** (0, 1, 2 all make normal objects; one is spawned only when the game's difficulty ≥ kind, and difficulty 3 skips the filter). High digit (0x10–0x60): a respawn counter that the game changes at run time; a killed object comes back on the third entry into the spawn band (`docs/engine.md`, "The kind high digit") | category confirmed for 0, 4; difficulty filter plausible (from the game code, not yet seen in play) |
| `b` | type inside the category; kinds 0, 1 and 2 share one type list (0 Meta Dridler, 1 Junkroid, 2 Meta Wheel F, 5 Hover Gunner, 48 D-1000) | confirmed for kinds 0, 4 |
| `id` | per-object parameter; its meaning depends on the type (Junkroid: 0 = dormant, ≠ 0 = active; Hover Gunner: 0 = enters from the right at once, 1 = waits until X has passed, then comes from the left) | Junkroid, Hover Gunner: confirmed (game code); others plausible |
| `a` | 0 on the disc; the game writes flags here at run time (`0x81`, `0x01` seen); bit 0 = already spawned | confirmed (runtime flags) |
| `x`, `y` | spawn point in stage pixels | certain for the intro ground enemies (checked against the corrected layout, 2026-10-21): every Junkroid record is 24 px and every Meta Wheel F record 32 px above solid floor, the same as X's own position is 24 px above the floor it stands on. So the record point is the object's centre, not its feet |

## Kinds and types

The colour is the marker colour in the `objects` layer of `stage.ora`.

| kind | Colour | Category | b → type | Status |
|---|---|---|---|---|
| 0 | red | object on every difficulty | 0 Meta Dridler; 1 Junkroid (Junkroid); 2 Meta Wheel F (Meta Wheel F); 5 Hover Gunner; 48 D-1000 (boss) | 0, 1, 2 confirmed; 5, 48 plausible |
| 1 | green | object from difficulty 1 up | same type list: 5 Hover Gunner (id 1 = enters from the left, id 0 = from the right); 0 Meta Dridler | jets seen; difficulty filter plausible |
| 2 | blue | object from difficulty 2 up (not met in the play-through) | same type list: 1 Junkroid, 2 Meta Wheel F, 5 Hover Gunner | plausible; test on a harder difficulty |
| 3 | yellow | stage logic: camera zones (b 0, `id & 0x7F` = entry of `camera_zone_tbl`, bit 7 = texture swap); palette cycles (b 3, second list, `PaletteCycle`); checkpoint markers (b 6, `id` = checkpoint); the Turtloid acid-rain controller (b 11, second list); talk zones (b 14, `TalkZone`: Alia messages; `id & 0x0F` = box and text, bit 7 or any of `0xF0` = optional "!" call with Select, else forced; not a background setting) | camera ids count up from 0; they sit in pairs at one place | b 0, 3, 6, 11, 14 certain from the code (intro, Yammark, Turtloid) |
| 6 | – | ray objects (`0x8009C9B0`, the class of the Nightmare Virus rays), second list only | 5 the darkness effect of Nightmare Effect 8 (`id` = zone 0–7) | certain from the code (Yammark, Turtloid) |
| 4 | purple | stage object | 1 Caterpirade (made by main type 7, no record); 2 sabre block (intro, overlay type; `docs/enemies/sabre-block.md`); 4 Rescue Reploid; 5 rocket platform (Turtloid); 8 boss gate; 13 health capsule (Turtloid); 26 Dr. Light capsule (Yammark) | confirmed |
| 5 | teal | event; the records make misc objects (`0x800CD410`) | the types are overlay types. Intro: 8 the Hunters' scene (talk and beam-out), 10 and 15 wreck scenery (not the stage start and the hint); Turtloid: 20 scenery | certain from the code (intro, Turtloid type 20) |

## Object types by sprite bank

From the sprite exporter (`mmxdemake.spriteexport`). A type number names a type only inside one
stage, except types whose code is in the main program (shared). Types ≥ `0x80` are visual or misc
objects. "stage n" is type n of the stage-object class (placement records of kind 4; its own
type list), not a main type. The export folders are in `~/mmx-data/x6/sprites/enemies/`.

| Stage | Type | Bank | Object | Export | Source of the name |
|---|---|---|---|---|---|
| intro | 0 | 2 | Meta Dridler | `meta-dridler` | code reading; confirmed |
| intro | 1 | 3 | Junkroid | `junkroid` | code reading; confirmed |
| intro | 2 | 4 | Meta Wheel F | `meta-wheel-f` | code reading; confirmed |
| intro | 5 | 5 | Hover Gunner | `hover-gunner` | code reading; confirmed |
| intro | 48 | 15 | D-1000 (body and core, from live objects) | `d-1000`, `d-1000-parts` | code reading; confirmed |
| intro | 129 | 1 | sabre block (stage 2): the block and 3 debris sprites | none (no live object; `banks/intro/intro-1_start/bank-1.png`) | code reading |
| intro | 132 | 6 | the two Hunters and their beams (misc type 8) | none (misc type) | code reading; by eye |
| intro | 145, 146 | 7, 8 | wreck scenery (misc types 10 and 15) | none (misc type) | code reading; by eye |
| intro | 128 | 0 | Dr. Light capsule and boss gate (shared bank of 84 sprites; same as Yammark and Turtloid bank 0) | none (visual type); the gate via `boss-gate` | user (Turtloid); plausible here |
| Yammark | 128 | 0 | Dr. Light capsule (stage-object type 26, `docs/enemies/light-capsule.md`) and boss gate | none (visual type); the gate via `boss-gate` | user; code reading |
| Yammark | 15 | 1 | Nightmare Virus (shared: code in the main program) | `nightmare-virus` (`nightmare-virus-orb` is a false match: a table the virus never uses) | user; code reading |
| Yammark | 141 | 2 | Rescue Reploid: stage-object type 4, code in the main program (`docs/enemies/rescue-reploid.md`) | none (drawn with sprite type 141) | user; code reading |
| Yammark | 7 | 3 | Caterpirade: main type 7 is its spawner; the enemy is stage-object type 1 | `caterpirade` (from its live object: stage-object class, type 1, table `0x800F73D4`) | user; code reading |
| Yammark | 3 | 4 | Worm Spider | `worm-spider` | user |
| Yammark | 4 | 5 | Killer Moth | `killer-moth` | user |
| Yammark | 23 | 6 | Nightmare Insect | `nightmare-insect` | user |
| Yammark | 28 | 7 | teleporter crystal (X jumps in to move to another part of the stage) | `teleport-crystal` | user |
| Yammark | 37 | 8 | wind-driven rain (Nightmare Effect 6) | `rain-effect` | code reading |
| Yammark | 49 | 15 | Commander Yammark (from his live object) | `commander-yammark` | user |
| Wolfang | 6 | 3 | Wolfloid: purple robot wolf with ice breath or a back cannon (`docs/enemies/wolfloid.md`) | `wolfloid` (was `purple-wolf`) | user ("purple wolf"); name from the Mega Man Knowledge Base [likely]; code reading |
| Wolfang | 17 | 5 | ice block: avalanche blocks, falling ice and bouncing blocks (modes by `var`), made by spawner modes of the same type; the spawners come from stage logic 12 (`docs/enemies/ice-block.md`) | `ice-block` | code reading; user (falling ice) |
| Wolfang | 19 | 6 | fire ring of Nightmare Effect 3 (only when `nightmare_effect` = 3) | `enemy-78c8f556f5` | code reading |
| Wolfang | 26 | 7 | Nightmare shadows (Nightmare Effect 7): a magenta Zero silhouette when the player is X (`docs/enemies/nightmare-shadow.md`) | `nightmare-shadow` | user ("purple Zero"); code reading |
| Wolfang | 144 | 8 | scenery: snow mounds and ice wall edges, misc type 23 (7 fixed-list records, x 5120–5888; sprite = `var`; `var` 1 is always drawn) | `enemy-cd98e86270` (sprite-only, misc type); wrong in the boss states (the boss texture covers its cells; `docs/TODO.md`) | code reading; by eye |
| Wolfang | 134 | 4 | ice stack block, stage-object type 7: pit generators, falling blocks that become map tiles (one in two carries a Wolfloid), the rows with one gap of the ice shaft (stage logic 8), and the sparkle (misc type 9) (`docs/enemies/ice-stack-block.md`) | `ice-stack-block` (was `enemy-583d456ecb`; from its live object: stage-object class, type 7, table `0x800F6B6C`) | user (falling ice blocks in a pit); code reading |
| Wolfang | 54 | 14 | Blizzard Wolfang, the boss; the same type with `var` 1 (trail image) and 16–18 (ice spikes), and his ice balls, debris and wall runners (visual 20) and mouth ice (effect 8) use the same table and sprites (`docs/enemies/blizzard-wolfang.md`) | `blizzard-wolfang` (`enemy-6757bcd2a2`, from `wolfang-6_boss-warning` and `wolfang-7_boss-fight`) | code reading; boss of stage 2 |
| Heatnix | 128 | 0 | Light capsule and boss gate (shared bank) | none (visual type); the gate via `boss-gate` | code reading |
| Heatnix | 15 | 1 | Nightmare Virus | `nightmare-virus` (the same export) | code reading |
| Heatnix | 141 | 2 | Rescue Reploid (stage-object type 4) | none (the live export of `heatnix-2_mini-boss` was wrong: `docs/TODO.md`) | code reading |
| Heatnix | 36 | 3 | flame jet: a fire nozzle in the first tower, up or down (`docs/enemies/flame-jet.md`) | `flame-jet` (`enemy-7f9bbc9a47`) | code reading; descriptive name |
| Heatnix | 22 | 4 | Nightmare Bug (only when `nightmare_effect` = 1; not in the states) | `nightmare-bug` (the same export) | code reading |
| Heatnix | 142 | 5 | grey block, stage-object type 12 (Nightmare Iron, only when `nightmare_effect` = 4; the same bank as Turtloid bank 11) | `grey-block` | code reading |
| Heatnix | 25 | 6 | Nightmare Snake, the mid-boss: the four gems, its energy balls and jet flames (visual 29); the ring itself is BG layer 1 (`docs/enemies/nightmare-snake.md`) | `nightmare-snake` (`enemy-6e15315f49`) | user ("red dragon donut"); name from a web search [likely]; code reading |
| Heatnix | 28 | 7 | teleporter crystal | `teleport-crystal` (the same export) | code reading |
| Heatnix | 0 | 8–13 | no texture; the sprite data of bank 14 | none | code reading |
| Heatnix | 52 | 14–15 | not loaded at the start (probably Blaze Heatnix) | none yet | plausible |
| turtloid | 128 | 0 | Dr. Light capsule and boss gate | none (visual type); the gate via `boss-gate` | user |
| turtloid | 15 | 1 | Nightmare Virus | `nightmare-virus` | user |
| turtloid | 141 | 2 | savable Reploid (pink body, green hair) | none (misc type) | user |
| turtloid | 9 | 3 | Monbando | `monbando` | user |
| turtloid | 10 | 4 | Sea Attacker ST | `sea-attacker-st` | user |
| turtloid | 14 | 5 | Weather Analyze (controls the stage's weather; destroying it stops the acid rain) | `weather-analyze` | user |
| turtloid | 131 | 6 | rocket platform (floats; moves left–right or up–down; stage-object type 5) | `rocket-platform` | user; code reading |
| turtloid | 28 | 7 | teleporter crystal (blue/green) | `teleport-crystal` | user |
| turtloid | 18 | 8 | Batton Bone B81 (bat that drops a bomb on X) | `batton-bone-b81` | user |
| turtloid | 139 | 9 | water splash effect | none (visual type) | user (likely) |
| turtloid | 140 | 10 | one piece of scenery, maybe a pillar: misc type 20, 4 records in the second list, gone from checkpoint 4 | none (misc type) | user (unsure); code reading |
| turtloid | 142 | 11 | Nightmare Iron (only under the Nightmare Effect) | none (misc type) | user |
| turtloid | 143 | 12 | health capsule (refills the HP; stage-object type 13) | `health-capsule` | user; code reading |
| turtloid | 26 | 13 | Nightmare shadows: flat silhouettes of the other character (a magenta Zero for X, a cyan shadow for Zero); spawn only under Nightmare Effect 7 and fly at the player (descriptive name; `docs/enemies/nightmare-shadow.md`) | `nightmare-shadow` | user |
| turtloid | 57 | 14–15 | not loaded at the save point (probably Rainy Turtloid) | none yet | plausible |
| both | stage 8 | (own source, 84 sprites) | boss gate (stage-object class, record kind 4, b 8; shared code in the main program) | `boss-gate` | legend kind 4 / b 8; by eye |

## Objects of each stage

The record lists of each stage are in `docs/stages/` (`intro.md`, `yammark.md`, `turtloid.md`,
`heatnix.md`, and the others there).

## Open questions

- **Do the meanings hold in other stages?** Yes for Commander Yammark section 1 (2026-10-22): all
  108 records are named in `docs/stages/yammark.md` with the same kinds and types as the intro and
  Turtloid; kinds 0–2 use the main type list, kind 3 the stage-logic table, kind 4 the stage-object
  table. Its second list (21 records) adds kind 6 and a main type 37 record.
- Kind 3 type 14 (`TalkZone`): the text of each message in each stage (the text decode is open).
- Kind 6 has no marker colour in `stage.ora` (no first-list record has kind 6 so far).

## For the Maverick conversion

- Ground enemies: place them on the first floor below the marker.
- Kind 3 b 0 camera zones: X6 has no rooms; the converter will need to turn the zone limits
  (`camera_zone_tbl`, decoded for the intro in `docs/stages/intro.md`) into Maverick rooms or camera
  limits. Kind 3 b 6 checkpoint markers and `checkpoint_start_tbl` give the player spawns.
