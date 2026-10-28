# Nightmare shadow (stages 02 and 09, main object type 26): behaviour sheet

Nightmare shadows are flat silhouettes of the other character: magenta **Zero** shadows when the
player is X, cyan shadows (X [guess]) when the player is Zero. They appear only under Nightmare
Effect 7, in Blizzard Wolfang's stage (stage 2) and Rainy Turtloid's stage (stage 6). An invisible spawner
makes one shadow 32 px in front of the player every 300, 400 or 500 frames while the player is in its
zone. The shadow blinks in, stands still for 50 frames, then dashes in a straight line at the point
where the player was, at 4 px/frame. It is **not a pure effect**: its contact damage is 6. Most
weapons have no effect on it. There is no official name. "Nightmare shadow" is a descriptive name.

Overlay `09` (Rainy Turtloid), code at `0x800EE9E4`–`0x800EF284`, save state `turtloid-1.state.gz`
(stage 6, area 0). Overlay `04` (Blizzard Wolfang) has a copy with the same code and data at other
addresses: set-up `0x800EF738`, entry `0x800EFD58`, states `0x800F6DC8`, steps `0x800F6DD8`, appear
sub-steps `0x800F6DE8`, boxes `0x800F6D90`, animations `0x800F6DB8`. A live shadow (`var` 10, the
dash) is in `wolfang-2_ice-slope.state.gz`. Written with the RE kit (`python3 -m mmxdemake.rekit --state
~/mmx-data/x6/dumps/turtloid-1.state.gz --type 26`) and `docs/re-guide.md`. Units, confidence tags and
the shared engine rules: `docs/engine.md`. Stage: `docs/stages/turtloid.md`.

## Spawn condition [certain]

- The set-up (state 0, `0x800EE9E4`) reads the byte `0x800CD30A`. When it is not **7**, the object
  deletes itself at once (`DeleteObject3`). The byte is `nightmare_effect` (see "Nightmare Effect"
  in `docs/engine.md`). The value 7 comes from the pick of stage 7 (Shield Sheldon): its targets
  `2 6 6` give stages 2 and 6 the value 7 [certain for the rule and the bytes]. It is 7 in the three
  `wolfang-*` states and 0 in the `turtloid-*` states.
- The type 26 objects have two roles by `var`:

| `var` | role |
|---|---|
| 0–5 | spawner (invisible), one zone each (below) |
| 6–9, 12+ | nothing: the set-up does nothing and the object stays in state 0 [certain for the code] |
| 10 | shadow made when the player is X: a magenta **Zero** silhouette (animations 0, 1) |
| 11 | shadow made when the player is Zero: a cyan silhouette, X [guess] (animations 2, 3) |

- The spawners are records of the **fixed record list** (`fixed_set_tbl`, `0x80073554`): kind 0,
  type 26, x = y = 0, `id` = `var` [certain]. Stage 2 area 0 (list `0x800F6124`): records 11 and 12,
  `var` 0 and 1. Stage 6 area 0 (`0x800F5378`): records 10 and 11, `var` 2 and 3. Stage 21 area 1
  (`0x800EF1DC`, `turtloid-4_alt`): record 2, `var` 4. No dumped list has `var` 5. The normal stage
  list has no main record of type 26.

## Set-up (state 0) [certain]

- Flags `0x41`. Graphics from the sprite bank (bank 13).
- Spawner, `var` 0–5: it waits in state 0 until the player x (and y for `var` 5) is in its zone. Then:
  step 1, `display` 0 (never drawn), timer (`+0x7C`) = 300, 400 or 500 (`(GetRNG() % 3 + 1) × 100 +
  200`).
- Shadow, `var` 10 / 11: animation 0 / 2, timer 40, step 3, no attack box, no hurt box, `display` 1.
- Then, for both: **HP 4**, `iframes` 0, **contact damage 6**, damage table `0x8007497C` (below), draw
  priority 4, layer = `mega.layer`, no terrain box, `latch` 0, state 1, `step2` 0, faces the player
  (`flip` = `0x40` when the player x ≥ its x).

### Spawner zones (`var`, player position in px) [certain]

| `var` | zone | stage of the record |
|---|---|---|
| 0 | 321 ≤ x ≤ 2559 | 2 (Blizzard Wolfang) |
| 1 | 3201 ≤ x ≤ 4159 | 2 |
| 2 | 1249 ≤ x ≤ 1631 | 6 (Rainy Turtloid) |
| 3 | 4961 ≤ x ≤ 7311 | 6 |
| 4 | 1 ≤ x ≤ 1055 | 21, area 1 |
| 5 | 321 ≤ x ≤ 2143 and y < 1072 | not found |

The same tests are in the set-up (`0x800EEAF4`) and in the spawn step (`0x800EED10`); each picks
the test by `var` through a jump table (`0x800E9864`, `0x800E9894`), and the zone limits are immediate
values in the code.

Each overlay has the full table of six zones; each stage uses only the `var` values of its own
records.

## State machine

States (table `0x800F6360`): 0 set-up `0x800EE9E4`, 1 main `0x800EF060`, 2 death `0x800EF124`,
3 `0x800EF19C` (`DeleteObject3`). Steps (table `0x800F6370`): 0 engine hit reaction, 1 SPAWN
`0x800EECCC`, 2 DASH `0x800EF1BC`, 3 APPEAR `0x800EF1DC`. The entry (`0x800EF004`) calls
`CollideCheck` after the state routine; with no terrain box it does nothing [likely].

### Main (state 1)

1. Runs the step.
2. Weapon test (`ContactWeapon`). When a hit kills it: state 2, timer 60, `display` 1, `+0x80` and
   `+0x82` = x, hit-flash bit cleared.
3. Else: contact test (`ContactMega`). Then, only for `var` 10 and 11: when `OffScreenCheck2(128, 128)`
   is 1, state 3 (deleted). There is no `DisplayCheck`: the code sets `display` itself.

The spawner runs the same tests, but it has no boxes, so it cannot touch or be hit [likely]. It is
never deleted by this code.

### SPAWN (step 1, spawner) [certain]

- When the player is outside the zone, nothing happens. The timer stops.
- In the zone: the timer counts down by 1 each frame. At 0 it makes one shadow (`GetMainObject`,
  flags `0x41`, type 26), sets a new random timer (300, 400 or 500), and goes on. So the first
  shadow comes 300–500 frames after the player enters the zone, then one every 301–501 frames.
- The shadow: `var` 10 when `mega+0x02` = 0 (X), else 11 (Zero). x = player x + 32 when the player
  faces right (`mega.flip` ≠ 0), else player x − 32; y = player y. So it appears **32 px in front of
  the player, at the same height**. It copies the spawner's palette (without the hit-flash bit),
  animation table, sprite data, texture page and layer.
- With no free main slot, no shadow is made, but the timer is still reset.

### APPEAR (step 3, shadow) [certain]

| `step2` | time | what |
|---|---|---|
| 0 | 40 frames | `display` toggles each frame: the shadow blinks. No boxes. At the end: attack box and hurt box `0x800F6328` / `0x800F6334` |
| 1 | 1 frame | `display` 1, timer 50 |
| 2 | 50 frames | it stands still, with boxes; at the end it starts the dash |

At the end of `step2` 2 (`0x800EEF04`):

- animation 1 (X) or 3 (Zero), the dash pose;
- angle = `ratan2(player y − y, player x − x)`; `speed_x` = `rcos(angle) × 64`; `speed_y` =
  `−rsin(angle) × 64`. That is **4 px/frame toward the player's position at that frame**;
- attack box `0x800F632C`, hurt box `0x800F6338`; faces the player; step 2.

### DASH (step 2, shadow) [certain]

- `MoveObject` only: a straight line at 4 px/frame. It does not aim again, has no gravity and no
  terrain box: it goes through walls.
- It is deleted when it is more than 128 px outside the view (`OffScreenCheck2(128, 128)`).

### Death (state 2) [certain]

- For 60 frames: `+0x80` gets +1 and `+0x82` gets −1 each frame. The x position is `+0x82` on frames
  where bit 0 of the word `0x80097420` (a frame counter [likely]) is 1, else `+0x80`. So the shadow is
  drawn as two flickering copies that move apart at 1 px/frame each, to ±60 px.
- Then `DeleteObject3`. No explosion, no debris, no item drop, no sound.
- State 2 does not call `ContactMega`: a dying shadow is harmless.

## Boxes [certain]

| box | address | value | size |
|---|---|---|---|
| attack and hurt, standing | `0x800F6328`, `0x800F6334` | (−10, −17, 19, 38) | 19 × 38 px |
| attack and hurt, dash | `0x800F632C`, `0x800F6338` | (−11, −14, 18, 34) | 18 × 34 px |

The standing box is set at the end of the blink, so it is also the box for the 50 frames of standing.

## Animations (table `0x800F6350`)

| # | what | frames |
|---|---|---|
| 0 | `var` 10 (player X): Zero silhouette, standing (helmet fins, long hair) | sprite 1, hold |
| 1 | `var` 10: Zero silhouette, dash (hair behind) | sprite 0, hold |
| 2 | `var` 11 (player Zero): standing, X [guess] | sprite 3, hold |
| 3 | `var` 11: dash, X [guess] | sprite 2, hold |

`SetAnime` is called with 0, 1, 2 and 3. No event flag is tested. Sprites 0–1 are magenta and
sprites 2–3 cyan. The code gives all shadows one CLUT (the spawner's), but every piece of sprites 2
and 3 has piece flag `0x04`, so the draw uses that CLUT + 1 [certain]. The palettes are flat: colour
0 transparent, colours 1–15 all `0xFC1F` (magenta, r 248 g 0 b 248) or all `0xEFE9` (cyan, r 72 g 248
b 216), the same in stages 2 and 6. Both words have bit 15 set (the texel semi-transparency bit);
whether the draw is semi-transparent is not read. The silhouettes are about 64 px high, larger than
the player (about 40 px). The shape names come from the sprites and from the game (X sees a purple
Zero, `wolfang-2_ice-slope`) [likely].

## Damage

- **Dealt:** contact damage **6** (Easy 4, Normal 6, Hard 7 after `PlayerDamageScale`; a heavy hit).
  Only while the boxes are on: the 50 frames of standing and the dash. Not during the 40 frames of
  blinking, not while it dies.
- **Taken:** damage table `0x8007497C` (new; read from the executable) [certain]:

| ids | value |
|---|---|
| 7 | class 0, damage 4 |
| 16 | class 0, damage 6 |
| all other ids (0–79) | class 2 (no effect) |

  With HP 4, one hit of weapon id 7 or 16 kills it. No other weapon (also not the X-Buster ids 0, 9,
  18–20 or the sabre ids 24–31) has an effect. Ids 7 and 16 are Guard Shell, uncharged and charged
  (slot 7, Shield Sheldon, `docs/players/weapons.md`) [likely]: the weapon of the Maverick whose
  pick causes the effect.

## Shared state

| address | name | use |
|---|---|---|
| `0x800CD30A` | `nightmare_effect` | read: must be 7 |
| `0x800970A0` | `mega` | x, y (`+0x0A`, `+0x0E`), `+0x02` character, `+0x15` facing, layer |
| `0x80097420` | frame counter [likely] | bit 0 in the death flicker |

## Summary for a reimplementation

- Only under Nightmare Effect 7 (stages 2 and 6, after the stage 7 pick). Invisible spawners from the
  fixed record list, with a player-x zone each.
- In the zone, every 300/400/500 frames (random), one shadow appears 32 px in front of the player: a
  magenta Zero when the player is X, a cyan shadow when the player is Zero.
- Shadow: blinks 40 frames (harmless, cannot be hit), stands 50 frames, then dashes at 4 px/frame at
  the player's position, straight, through walls. Removed 128 px off screen.
- 6 contact damage. HP 4; only weapon ids 7 and 16 (Guard Shell [likely]) hurt it (one hit kills).
  Every other weapon has no effect.
- Death: two copies split apart for 60 frames, then gone. No drop.

## Open questions

1. (answered: the fixed record list, `var` = record `id`) What places the spawners. Still open:
   which list uses `var` 5.
2. (answered [likely]: Guard Shell) Which weapons are ids 7 and 16.
3. (answered: the pick of stage 7, `docs/engine.md`) What writes `0x800CD30A`, and which pick gives 7.
4. (answered: piece flag `0x04` selects CLUT + 1) The cyan palette of sprites 2–3. Still open: the
   shape of sprites 2–3 (X is a guess; a state with Zero as the player under effect 7 shows it), and
   whether the shadows are drawn semi-transparent (bit 15 in the colour words).
5. Does a class 2 hit stop the player's shot, or does the shot go through?
