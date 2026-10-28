# Dr. Light capsule (stage 03, stage object type 26): behaviour sheet

The Dr. Light capsule gives the player one armour part. When the player comes near, it opens, plays a
voice stream and shows a text message. When the player steps in, the player does the "get part"
animation and the part is added to `game.armorParts`. "Dr. Light capsule" is the common name of this
object in the series; it is not an enemy, so the Fandom enemy list has no entry. The sprites are
sprite bank 128 (`docs/objects-legend.md`).

Overlay `03` (Commander Yammark), but **all the code is in the main program** (`0x80051158`–
`0x80051FE8`), so every stage uses it [certain]. Save state `yammark-1_start`; kit `python3 -m
mmxdemake.rekit --name light-capsule --state yammark-1_start.state.gz --type 26 --class
stage`. Units, tags and shared rules: `docs/engine.md`. Record 106: `docs/stages/yammark.md`.

## Spawn and set-up [certain]

The entry (`0x800515B4`) has two kinds of object: `var` 16 is the capsule lid (made by the capsule);
any other `var` is the capsule.

**Capsule** (state 0, `0x80051158`):

1. **Already taken → `DeleteObject`** (it comes back only to test again). See "The `id` parameter".
2. Damage table `0x80075B9C`, flags bit 3 set, `display` = 1, `+0xA2` = 1, `+0x75` = 1.
3. Terrain box `0x80078DF8` = (0, 0, 16, 12): 32 × 24 px. Hurt box `0x80078E00` = (−16, −12, 32, 24).
   No attack box: it does not hurt.
4. Animation table `0x800791A0`, animation 0 (closed). Layer = `mega.layer`, draw priority 4,
   `flip` 0. `mega+0xE2` (the player's capsule phase) = 0.
5. **Drops onto the floor:** it moves down in 16 px steps until `CollideCheck` gives the floor flag.
6. Makes the **lid**: a stage object (`GetStageObject`, `0x8002C4C4`) with flags 9, type 26,
   `var` 16, the capsule at `+0x7C`.
7. Makes the **part picture**: an effect object of type 35 (flags `0x49`, parent at `+0x50`), with
   `var` = `0x80078E10[id]` (3, 4, 5, 6, 3, 4, 5, 6 for `id` 0–7); for `id` 8: 16 (X) or 17 (Zero).
8. State 1, step 0; visible 136.

**Lid** (`var` 16): terrain box `0x80078DFC` = (0, 0, 32, 12): 64 × 24 px; hurt box `0x80078E04` =
(−32, −12, 64, 24); the same damage table. It sits **78 px above** the capsule. It deletes itself when
the capsule is gone.

Each update both parts call the solid-object helpers (`0x80032B34`, `0x80032BE8`, `0x80031DA8`
unless `mega+0xD1` ≠ 0, `0x8003208C`, `0x80031F4C`): they make the capsule and the lid solid blocks
for the player, the shots and the other objects [likely: the helpers read the player, the weapon
objects, the visual and the main objects, and write the contact fields `+0x72`…`+0x78`].

## The `id` parameter [certain for the code]

| id | part | "already taken" test | given at the end |
|---|---|---|---|
| 0–3 | a Blade Armor part: bit `0x01`, `0x04`, `0x02`, `0x08` | `game.armors` bit 2 (full Blade Armor) or `game.armorParts` & bit | `game.armorParts` \|= bit |
| 4–7 | a Shadow Armor part: bit `0x10`, `0x40`, `0x20`, `0x80` | `game.armors` bit 1 (full Shadow Armor) or `game.armorParts` & bit | `game.armorParts` \|= bit |
| 8 | X: Ultimate Armor; Zero: Black Zero | X: `game.armors` bit 3 or `mega+0xE7` ≠ 0; Zero: `game.armors` bit 5 | Zero: `game.armors` \|= `0x20`, then `0x8003C4EC` (new palette [likely]); X: nothing in this code |
| 16 | the lid (internal) | – | – |

- The bits come from `0x80078E08` (`01 04 02 08 10 40 20 80`). `game.armors` is `game+0x5F`
  (`0x800CCF2F`); `game.armorParts` is `game+0x69` (`0x800CCF39`).
- Low nibble = Blade, high nibble = Shadow [likely: the same split as the New Game tweak values
  `0x0F` / `0xF0` in `mmx-reference`].
- The text message of the capsule is `0x80078E48[id]` (`0C 07 05 07 04 07 06 06`).
- **Record 106 has `id` 3:** the Blade Armor part with bit `0x08`, message 7, part picture 6: the
  **legs**.

### Which part each bit is [certain for the code path; the part names are from the pictures]

The part picture (effect type 35, entry `0x800516E0`) uses its `var` as the animation of the capsule
table `0x800791A0`: in `sheldon-9_lower-2` the picture of the stage 7 capsule (`id` 1) is effect
type 35, `var` 4, sprite 12, the first frame of animation 4. Animations 3–6 of the export
(`~/mmx-data/x6/sprites/enemies/light-capsule/anim-3.png` … `anim-6.png`) show a helmet, a chest
with shoulder plates, two arm parts and two boots. So:

| `id` | bit of `game+0x69` | armour | part | picture (`var`) | message | capsule seen in |
|---|---|---|---|---|---|---|
| 0 | `0x01` | Blade | head | 3 | `0x0C` | – |
| 1 | `0x04` | Blade | body | 4 | 7 | stage 7, record 42 (`docs/stages/sheldon.md`) |
| 2 | `0x02` | Blade | arms | 5 | 5 | stage 22 area 1 (Mijinion's hidden area), record 27 (`docs/stages/mijinion-alt.md`) |
| 3 | `0x08` | Blade | legs | 6 | 7 | stage 1, record 106 |
| 4 | `0x10` | Shadow | head | 3 | 4 | stage 4 (`docs/stages/shark.md`) |
| 5 | `0x40` | Shadow | body | 4 | 7 | – |
| 6 | `0x20` | Shadow | arms | 5 | 6 | – |
| 7 | `0x80` | Shadow | legs | 6 | 6 | – |

So in bit order: `0x01` Blade head, `0x02` Blade arms, `0x04` Blade body, `0x08` Blade legs,
`0x10` Shadow head, `0x20` Shadow arms, `0x40` Shadow body, `0x80` Shadow legs. The low nibble is the
Blade Armor and the high nibble the Shadow Armor [certain: the "already taken" tests use
`game.armors` bit 2 for ids 0–3 and bit 1 for ids 4–7, and the armour select gives form 3 (Blade)
for `game.armors` bit `0x04` and form 2 (Shadow) for bit `0x02`, `docs/players/steps-extra.md`].
The parts menu of overlay 16 uses the same bit order (table `0x800F0EEC` = `01 04 02 08 10 40 20
80`, one misc object type 40, `var` 24 + index, per part owned).

**Full armour from the parts** [certain for the code]. Overlay 20 (the screens after a stage clear
[likely]), function
`0x800F0668`: when the low nibble of `game+0x69` is `0x0F` and it was not `0x0F` at the stage start
(`game+0xC8`, the copy that `StageStartReset` makes), `game.armors` |= `0x04` (Blade Armor); when
the high nibble is `0xF0` and was not before, `game.armors` |= `0x02` (Shadow Armor). The same
overlay compares `game+0x69` with `game+0xC8` to show the part that was found in the stage
(`0x800EA65C`). So the armour can be worn from the next stage on, after all 4 parts.
- Save-state check: `game.armorParts` is 0 in `yammark-1_start` and `0x08` in `yammark-2_boss`
  (the player took this capsule on the way [likely]). `game.armors` is 1 in all states [certain for the
  values].

## State machine

States: 0 set-up, 1 main (`0x80051890`). The main routine: `AnimeTick`, the step (table
`0x80078E18`, 12 steps), the solid-object helpers, visible 136. In the steps, "pause on" means
`game+0x12`, `+0x13`, `+0x16`, `+0x17` = 1 (the other objects stop [likely]); "pause off" clears them.

| step | routine | waits for | then |
|---|---|---|---|
| 0 | `0x80051924` | the player within 64 px in x and from 104 px above to 16 px below the capsule | animation 10 (open), `PlayerEvent(20, 0 or 64)` (`0x8003D308`: the player goes to the event hold, step 20, facing the capsule), faces the player, pause on |
| 1 | `0x80051A44` | the open animation's flag; then the stream state `0x800CD40C` = 0 | stops the stream (`0x80018774(3)`), then plays stream 28 at volume 127 (`0x80018060`, Dr. Light's voice [likely]) |
| 2 | `0x80051AC8` | the stream state = 2 | opens the text box with message `0x80078E48[id]` (`0x80021760`) |
| 3 | `0x80051B28` | `0x8008EAFC` = 1 (text box shown [guess]) | animation 11 (the light talks) |
| 4 | `0x80051B74` | `0x8008EAFC` = 0 (text closed), then the stream state = 0 | stops the stream, restarts the stage music (`0x8001854C`), animation 12 (close) |
| 5 | `0x80051C08` | the close animation's flag | `PlayerEventEnd` (`0x8003D330`: the player takes input again), pause off |
| 6 | `0x80051C68` | `+0x72` bit 3: the player stands on the capsule [likely] | animation 1, `PlayerEvent(24, mega.flip)` (player event step 24 [likely: the "get part" sequence]), effect type 36 `var` 0, `mega+0xE2` = 1, pause on, the player's x = the capsule's x |
| 7 | `0x80051D28` | `mega+0xE2` = 2 (set by the player code) | 4 effects of type 37, `var` 0–3 (parent the capsule) |
| 8 | `0x80051E40` | `mega+0xE2` = 3 | animation 2, timer 40 |
| 9 | `0x80051E9C` | 40 updates | – |
| 10 | `0x80051ED4` | – | `mega+0xE2` = 4, animation 13, **gives the part** (see the table), pause off |
| 11 | `0x80051FA8` | more than 136 px off screen | `DeleteObjectKeep` (`0x8002C9D4`): removed, and the record gets flags bit 7, so the capsule does not come back in this visit |

From step 6 to step 10 the capsule writes its x into `mega.x` each update, so the player stays in the
centre.

## Animations (table `0x800791A0`) [certain for the frames]

| # | what | frames |
|---|---|---|
| 0 | closed | sprite 0, hold |
| 10 | open | 2-update flicker of sprites 1 / 0, then 0x15 / 0, then sprite 6 with flag 1; ends in a 2-frame loop |
| 11 | the light talks | 1-update flicker of sprites 7 / 6 with 0, loop |
| 12 | close | 2-update flicker of sprites 0x15 / 0 and 1 / 0, sprite 0 for 8, sprites 8, 9, 10 for 3 each, sprite 1 with flag 1 for 1 update, then sprite 1 (hold) |
| 1, 2 | the player inside | 2 sprites, 2 updates each, loop |
| 13 | after the part | sprite 1, hold |
| 3–6 | the part pictures (effect type 35 [likely]) | 8, 10, 12, 20 updates, loop |
| 7–9, 14, 15 | other effects (beam, rings [guess]) | not read |

The capsule tests only "flag ≠ 0" in steps 1 and 5; the open and close animations have flag 1.

## Damage

- Taken: none; the capsule has no HP. Damage table `0x80075B9C` (all damage bytes 0):
  - class 7: ids 0, 1, 5, 6, 8–22, 42, 43, 45–54, 58, 63 (the X-Buster shots among them);
  - class 2 (no effect): ids 2–4, 7, 23–41, 44, 55–57 (the sabre ids 24–31 among them);
  - class 0: ids 59–62; class `0xFF` (deflect): ids 64–79.
  Class 7 is listed in `docs/engine.md`; its effect is not read. Probably the shot stops on the
  capsule without damage [guess].
- Dealt: none.

## Sounds and music

Stream 28 (voice) through `0x80018060`. The stage music restarts through `0x8001854C`. (The calls
`0x8003D308` and `0x8003D330` were listed here as sounds before; they are the player event calls,
`docs/engine.md`, "The player".)

## Summary for a reimplementation

- A solid capsule (32 × 24) with a solid lid (64 × 24) 78 px above; it falls to the floor at spawn.
- Not shown when the part is already owned.
- Player within 64 px: open, pause the world, play the voice and the text, close, unpause.
- Player stands on it: pause, lock the player to the centre, run the 4-phase "get part" sequence with
  the player code (`mega+0xE2` = 1 → 2 → 3 → 4, 40 frames in phase 3), then set the part bit.
- Record 106 (`id` 3): Blade Armor legs, bit `0x08`. Stage 7 record 42 (`id` 1): Blade Armor
  body, bit `0x04`.
- Parts: `id` 0–3 = Blade head, body, arms, legs; `id` 4–7 = Shadow head, body, arms, legs. All 4
  parts of one armour give the full armour in overlay 20, after the stage [likely for when].

## Open questions

1. (answered: the table "Which part each bit is") Which part each bit is.
2. The text system: `0x80021760`, `0x8008EAFC` and the stream state `0x800CD40C`.
3. The stage export has no art at the record (x 1800, y 776); the collision map has a floor at
   y 896. So the capsule room is drawn by something that the export does not show.
4. The meaning of damage class 7, and what player event step 24 does.
