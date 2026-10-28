# Dynamo (boss overlay 45, main object type 63): behaviour sheet

Dynamo is the boss of the 8 hidden areas when `boss_choice` (`0x800CD30B`) is 2 or more. The game
text names him: messages 2 and 3 of every hidden-area text bank (members 227–242) have the speaker
"Dynamo" [certain]. He is an optional fight: the player reaches him only through a hidden area.

He is a floor boss with gravity. He beams in from above. He jumps between the sides and the centre
of the room. He throws spinning blades that stop at the room edge and come back to him, strikes the
floor to make a row of tall energy pillars, fires aimed blades from the room centre, and does a
leaping slash with a large crescent. A few weapon ids stun him. A hit while he is stunned makes
him drop a 200-soul Nightmare Soul (at most 3 per visit). He does not explode for good: at the end
he stands up, speaks and beams away. He gives no armour part and sets no progress flag.

Save state: `mijinion-4_dynamo.state.gz` (stage 22, area 1: Infinity Mijinion's hidden area,
[`mijinion-alt.md`](../stages/mijinion-alt.md); Zero; boss level 2), the first state with him
live. Kit: `python3 -m mmxdemake.rekit --state mijinion-4_dynamo.state.gz --type 63 --name dynamo`
(52 functions; it also walks his shots, visual type 34, entry `0x800FC7D8`). The overlay at
`0x800FA000` in this state is `rock_bin/45.bin` (11 878 bytes): only 18 bytes differ, all of them
variables (the room points `0x800FCE48`–`0x800FCE55`, `dynamo_obj` `0x800FCE60` and the floor y
`0x800FCE64`) [certain]. The first version of this sheet was read from the disc overlay mapped
over `turtloid-5_zero` ([`~/mmx-data/x6/re/hidden-areas/scripts/kit45.py`](../../work/hidden-areas/scripts/kit45.py)); the live code is the
same. `BossSpawner` (`0x80052E44`) loads overlay 45 and makes main type 63 when `boss_choice` is
not 0 or 1 (`docs/engine.md`, "Boss overlay"). Export: `dynamo` (`enemy-f49a77c40e`, 15 of 15
animations, live, bank 15). Units, confidence tags and the shared engine rules:
`docs/engine.md`. "The room" below has its origin at `camera_layer0` `+0x26`, `+0x2A` at the
set-up (Mijinion's hidden area: (1936, 224)), as in `docs/enemies/zero-nightmare.md` and
`high-max.md`.

## Spawn and set-up (`0x800FA028`) [certain]

- **HP: {50, 58, 62, 66}[boss level].** `boss_hp_max` (`game+0x8C`) = 58 + the byte
  {`0xF8`, 0, 4, 8}[level] (table `0x800FA004`, every second byte), stored as a byte: 58 + 248 =
  306 = 50 (mod 256). Level from `BossLevel` (`game+0x8F`). The HP starts at 0 and fills in the
  intro. `mijinion-4`: level 2, `boss_hp_max` 62, HP 62 [certain].
- **Contact damage 4** (`+0x60`), the same in all poses.
- Boxes (attack, hurt), chosen from the current sprite (`+0x47`) on every update:

  | sprites | attack box | hurt box |
  |---|---|---|
  | most | `0x800FCA88` (−6, −22, 12, 50): 12 × 50 px | `0x800FCA94` the same |
  | 45–55, not 52 (leaping slash) | `0x800FCA8C` (−8, −14, 16, 34) | `0x800FCA98` (−8, −14, 16, 34) |
  | 52 (the slash) | `0x800FCA90` (−100, −4, 140, 68): 140 × 68 px, in front and below | (−8, −14, 16, 34) |

  Terrain box `0x800FCA84` (0, 0, 12, 32): a 24 × 64 px body, so his origin is 32 px above the
  floor. It is set in the intro (below) and removed at death.
- Damage table `0x80075A5C` (named `dmg_tbl_dynamo` here; below). Draw priority 5. Animation table
  `0x800FCCD4`. Own sprite source (texture `0x140`, palette `0x7948`, sprite data from his bank).
- Position: room (256, −16), above the view. Facing left. Speeds and gravity 0.
- `game+0x20` = this object, `game+0x25` = 0. `PlayerEvent(20, 64)`: the player is held.
  `camera_layer0+0x48` = 1 (the old value is kept in `+0x90`; the intro end writes 8) [meaning
  open].
- Room points (halfwords at `0x800FCE48`–`0x800FCE54`): spawn (256, −16); x 48 and 304 (the two
  sides); x 176 (the centre); y 16 and 176 (the top and bottom of the shot box, below). The
  set-up writes them back as world values. `mijinion-4` (room origin (1936, 224)): spawn (2192,
  208), sides x 1984 and 2240, centre x 2112, shot box y 240–400 [certain].
- He reads neither `nightmare_effect` (`0x800CD30A`) nor the difficulty (`game+0x68`): no walked
  instruction reads them [certain]. Only the boss level changes the fight.
- Shared flags: `0x800FCE58` = 0 (set to 1 at his death), `0x800FCE5C` = 0 (set to 1 while he is
  stunned), `0x800FCE60` = this object. All his shots end when either flag is 1.
- Then state 1, step 2 (INTRO).

## The `var` parameter

He has no record: `BossSpawner` makes him. The code does not read `var`.

## State machine

States (table `0x800FCD70`): 0 set-up, 1 main (`0x800FA23C`), 2 death (`0x800FC3C0`). The entry
(`0x800FB214`) saves the position, clears `display`, runs the state and then `CollideCheck`.

### Main (state 1) [certain]

1. Hurt box from the sprite (table above). `ContactWeapon`. A kill: `0x800FCE58` = 1, state 2,
   palette flash off, `mega+0x7A` = 1.
2. Attack box from the sprite. `ContactMega`.
3. A hit (result > 0, not 127): **30 updates of invulnerability** (`+0x88`; 2 updates after weapon
   ids 24 and 25, the first sabre hits): `dmg_tbl_deflect`, and the palette flash toggles every 2
   updates. At the end: his own table again.
4. A hit while in step 9 (STUN) sets `+0x84` = 1 (the soul drop, below).
5. A hit by **weapon id 6, 15, 42 or 46** (results 7, 16, 43, 47) when the step is not 9: `+0x84`
   = 0, `0x800FCE5C` = 1, step 9 (STUN), sub 0, and `+0x88` = 20. The weapons: X's Meteor Rain
   (6, 15), Zero's Ensuizan (42), X's Shadow Armor charged buster (46) [likely for the names; see
   open question 2]. During the invulnerability these ids are deflected, so they do not stun.
6. The step (table `0x800FCD7C`): 0 engine hit reaction, 1 nothing, 2 INTRO, 3 IDLE, 4 THINK,
   5 BOOMERANG, 6 CENTRE VOLLEY, 7 LEAPING SLASH, 8 PILLARS, 9 STUN.

Each step runs `AnimeTick`, `MoveObject2` (x += x speed; y −= y speed; y speed −= gravity) and
`DisplayCheck(64, 64)`.

### INTRO (step 2, sub-steps `0x800FCDA4`) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800FB2B8` | Waits until the player is in step 20 (held). Camera limit speed 2, `PlayerEvent(29, 64)`, a 40-update timer, engine object 24 (as for the other bosses) at his position. |
| 1 | `0x800FB350` | Waits while `game.exit_req` ≠ 0. First update: the camera target min x (`camera_layer0+0x26`) is kept in `+0x94` and set to the target max x (`+0x24`) − 16. The boss-room limits are min x … min x + 32 (gate cases 35–51, `boss-gate.md`), so the camera moves 16 px right during the intro [certain for the code; the reason is not known]. At the end of the timer: `PlayerEvent(20, 64)`. When object 24 is gone: sound 71, animation 0 (the **beam-in**), y speed **8 px/frame down**. |
| 2 | `0x800FB414` | He comes down as a beam (`MoveObject`, no gravity). When his **world** y is 300 or more: the terrain box (the test uses the world y, not the room y: in a room lower than y 316 he gets the box at once) [certain for the code]. On the floor: y speed 0. At the end of animation 0 (108 updates): his text, only the first time in this stage area (`TextSeen(2)` `0x800533F4`; `TextOpen(2, 255, 0)`, then `SetTextSeen(2)` `0x80053440`). Animation 1. `mijinion-4`: `talk_seen` k 20 (stage 22 area 1) = `0x0004`, bit 2: the text was shown [certain]. |
| 3 | `0x800FB4E8` | Waits until the text is closed (`text_open` = 0). `BossMusicRequest`, `game+0x24` = 1. |
| 4 | `0x800FB54C` | Waits while `BossMusicWait` (the boss music wait). The camera target min x gets its old value. **+1 HP per update**, a tick (bank 0, id 14) every 3 updates, up to `boss_hp_max`. Then camera `+0x48` = 8, `PlayerEventEnd`, his y is kept in `0x800FCE64` (the floor y), THINK. |

His texts (member 242 for Zero, 241 for X, stage 22 area 1; the other hidden areas have the same
messages 2 and 3) [certain for the decode]:

- Message 2, Zero: "Dynamo: You are very impressive..." / "Zero: Again..." / "Dynamo: You know, it
  takes hours for me to come this far. I've been gathering Nightmare Souls at the risk of my life.
  I've been dying to become stronger... Do you want to challenge me?"
- Message 2, X: "Dynamo: Hard work and no play makes X go crazy! Hee Hee Hee!" / "X: Dynamo! It's
  you, again." / "Dynamo: It takes hours for me to get here. I won't go back empty-handed. I've
  been gathering Nightmare Souls ..." (the rest as for Zero).
- Message 3 (after the fight), both: "Dynamo: You are way too strong..."

### Live check (`mijinion-4_dynamo`) [certain]

| what | value | agrees with |
|---|---|---|
| Dynamo | main slot 0, type 63, `var` 0, state 1, step 5 (BOOMERANG), sub 1, `+0x07` = 1, at (2192, 384), animation 2, sprite 23, `flip` 0 (left), HP 62, contact 4, boxes `0x800FCA88` / `0x800FCA94`, texture `0x140`, palette `0x7948`, animation table `0x800FCCD4` | the set-up; still at the spawn x; the floor y 384 (`0x800FCE64`) is 32 px above the floor at y 416 |
| throw count `+0x7E` | 1 | one blade thrown; the second throw has started (animation 2 again). Level 2 needs 4 throws |
| move pattern `+0x8A`, `+0x8B` | 0, 2 | pattern 0 (IDLE, BOOMERANG; HP above half, r < 10); the next entry is THINK |
| blade | visual slot 0, type 34, `var` 0, state 1, at (2126, 384), x speed −4.5 px/frame, HP 100, contact 6, boxes `0x800FCE24` / `0x800FCE28`, animation 13, `+0x7C` = Dynamo | the first throw: straight, 16 px + about 11 updates × 4.5 px in front of him |
| `game+0x20`, `game+0x24` | Dynamo, 1 | the HP bar is on |
| camera | target x 1936–1968, y 224; `+0x48` = 8 | the intro end restored min x and wrote 8 |
| shared flags | `0x800FCE58` = 0, `0x800FCE5C` = 0 | not dead, not stunned |
| `game+0x33` | 0 | no soul dropped yet |
| player | Zero, HP 26, step 2, the event ended (`mega+0xD0` = 0) | `PlayerEventEnd` at the end of the intro |

### THINK (step 4, `0x800FB74C`) [certain]

THINK has no wait. `r` = `GetRNG() & 15`. "High HP" means HP > `boss_hp_max` / 2 (25 at level 0).

| HP | pattern |
|---|---|
| high | r < 10: 0; else 1 |
| low (≤ half) | r < 6: 2; else 3 |

A pattern is a list of steps (`0x800FCD10`, 6 bytes each). Each step ends with the next entry
(`+0x8B`); the last entry is THINK.

| pattern | steps |
|---|---|
| 0 | IDLE, BOOMERANG |
| 1, 2 | IDLE, PILLARS |
| 3 | IDLE, CENTRE VOLLEY, LEAPING SLASH |
| 4 | IDLE (no THINK result gives it) |

So above half HP: boomerang blades (10/16) or pillars (6/16). At half HP or below: pillars (6/16),
or the centre volley and then the leaping slash (10/16). The difficulty is not tested. The
player's character changes only the centre-volley shot.

### IDLE (step 3, `0x800FB650`) [certain]

Animation 1 (stand, loops) for **60 updates**. Then the next step.

### BOOMERANG (step 5, `0x800FB824`) [certain]

1. Animation 2, face the player.
2. At the animation's flag 1 (update 25 of each 31-update throw): a **blade** (visual 34; `var` 0
   for the first throw, `var` 1 after it). At the end of the animation: face the player; the
   throw count + 1. When it is {3, 3, 4, 5}[boss level] (table `0x800FA00C`): animation 1, sub 2;
   else animation 2 again.
3. Sub 2: he waits until **all his blades are gone** (`+0x07` counts the live blades: +1 at each
   set-up, −1 at each end). Then the next step.

A returning blade sets his animation 3 (the catch pose) when it is near him.

### PILLARS (step 8, sub-steps `0x800FCDF0`) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800FA5A0` | Animation 5. The target is the side **away from the player**: room x 304 (and he faces left) when the player's x is left of the room centre, else room x 48 (facing right). Within 32 px of it: no jump, sub 2 directly (animation 6, sound 68, `Rumble(6)`). Else sound 53 and a **jump**: x speed = distance / 42, y speed 3 px/frame up, gravity 0.15 px/frame² (`0x2666`): a 40-update arc that lands on the target. |
| 1 | `0x800FC09C` | On the floor: sound 66 (when he was moving down), all speeds 0. At the end of animation 5 (65 updates): animation 6 (a charge loop), sound 68, `Rumble(6)`. |
| 2 | `0x800FA6EC` | **80 updates** of animation 6. Then animation 7 and sound 59. At its flag 2 (7 updates): sound 69, `Rumble(5)`. At its flag 1 (23 updates): sound 70 and **8 pillars** (visual 34 `var` 4) at 36, 72, … 288 px in front of him. At the end of animation 7 (55 updates): animation 1, the next step. |

So the pillars cover the room in front of him, from 25 px to about 300 px away. See `var` 4 below.

### CENTRE VOLLEY (step 6, sub-steps `0x800FCDCC`, low HP only) [certain]

| sub | routine | what |
|---|---|---|
| 0 | `0x800FBAD0` | Animation 5, face the player. Within 32 px of the room centre (x 176): animation 8, sub 2. Else sound 53 and the same 40-update jump as PILLARS, to the centre. |
| 1 | `0x800FBBD0` | Landing (sound 66). At the end of animation 5: face the player, animation 8 (a firing pose, loops from frame 2). |
| 2 | `0x800FA454` | Every **50 updates**: face the player, one **blade**: `var` 2 when the player is X (`mega+0x02` = 0), `var` 3 when the player is Zero. {6, 7, 8, 8}[boss level] blades (table `0x800FA010`). After the last one, he waits until all blades are gone; then animation 9. |
| 3 | `0x800FBC84` | At the end of animation 9 (7 updates): animation 1, the next step. |

### LEAPING SLASH (step 7, sub-steps `0x800FCDDC`, low HP only, after CENTRE VOLLEY) [certain]

Animation 4 (171 updates) drives the step:

| update | what |
|---|---|
| 0 | Face the player. |
| 12 (flag 1) | Sound 53. A **jump at the player**: x speed = (player x − his x) / 40, y speed 3 px/frame up, gravity 0.1 px/frame² (`0x1999`). He faces his direction of travel. |
| 18–76 | Sprites 45–55: the small boxes (−8, −14, 16, 34). |
| 48 (flag 2) | Sounds 56 and 92. |
| 56–59 | Sprite 52: the **slash box** (−100, −4, 140, 68), 140 × 68 px, in front of him and down to 64 px below his origin. He is coming down at this time. |
| 76 (flag 3) | A second hop: y speed 4 px/frame up, gravity 0.133 px/frame² (`0x2222`), x speed 1 px/frame in the same direction; he turns to face back. |
| landing | Sound 66, all speeds 0. |
| 171 | Animation 1, the next step (THINK). |

The first jump is 60 updates long on flat ground. With x speed = distance / 40 it would carry him
1.5 × the distance, but the slash at update 56 and the second hop at 76 come first [likely: he
lands near the player's old position; not checked].

### STUN (step 9, sub-steps `0x800FCDFC`) [certain]

Entered on a hit by weapon id 6, 15, 42 or 46 (the damage of the hit is applied first; ids 6 and
15 do 5 and 8). `0x800FCE5C` = 1 at once: **all his blades and pillars end**. He is invulnerable
for the first 20 updates.

| sub | routine | what |
|---|---|---|
| 0 | `0x800FC1A0` | Sound 62, animation 10 (hurt), all speeds 0. |
| 1 | `0x800FA864` | He shakes: y ± 2 px, the direction changes every 4 updates. After **80 updates**: the escape (next row). A **hit in this sub** ends it at once when `game+0x33` < 3: `game+0x33` + 1 and a **Nightmare Soul of 200** (main type 15 `var` `0x32`, `docs/enemies/nightmare-virus.md`) at his position. With 3 souls dropped, a hit does not end the stun. |
| (escape) | | `+0x88` = 30 when it is below 15, `dmg_tbl_deflect`. When he faces left: x speed 8 px/frame to the right (when he faces right the x speed stays 0) [certain for the code; it looks like an oversight]. Gravity 1 px/frame². |
| 2 | `0x800FC1FC` | He stays invulnerable (`+0x88` is kept at 15 or more). On the floor: animation 5, sound 53, and the 40-update jump to the other side of the room: to room x 304 (facing left) when he is left of the centre, else to room x 48 (facing right). |
| 3 | `0x800FC2F0` | Landing (sound 66). At the end of animation 5: `0x800FCE5C` = 0, 30 more updates of invulnerability, face the player, animation 1, THINK. |

`game+0x33` is in the per-stage bytes `game+0x26`…`+0x35`. Game mode 11 (`0x8001E81C`, the move
to the report screen) and the scene ends clear them; the hidden-area stage start does not
[certain for the clears]. So it allows **3 soul drops per visit** [likely].

### Death (state 2, steps in `0x800FCE0C`) [certain]

He does not explode for good.

1. At the kill: `0x800FCE58` = 1 (all his blades and pillars end), `mega+0x7A` = 1.
2. Animation 10 (hurt), palette flash off, `PlayerEvent(20, <the player's facing>)`.
3. **127 updates.** The flash toggles after 25, 20, 15, 10, then every 5 updates. Then engine
   object 26 (the boss explosion, as for the other bosses) at his position.
4. While object 26 runs he flashes every 6 updates. When its `+0x07` is not 0 (the screen is
   white): the player is turned to face him, he faces the player, flash off, animation 11, speeds
   0, no terrain box, and his y = the floor y kept at the end of the intro.
5. When object 26 is gone: his text (`TextOpen(3, 255, 0)`).
6. When the text is closed: sound 64, animation 12 (the **beam-out**: the beam-in frames in the
   other order).
7. At its flag 1 (update 59): sound 72 and y speed **8 px/frame up** (`MoveObject`, no gravity):
   the beam leaves upwards. At the end of animation 12 (update 115): `game.exit_req` = **`0x10`**
   and he is deleted.

He writes no `boss_choice`, `game+0x5F` or other progress byte, and he does not clear `game+0x20`
or `game+0x24`. He leaves no soul at death. Where the clear leads: `docs/enemies/high-max.md`,
"Where the clear leads"; with `boss_choice` 2 the hidden-area clear goes to the report screen.

## Animations (table `0x800FCCD4`, 15 animations) [certain for the frames]

Export `dynamo` (`enemy-f49a77c40e`, from `mijinion-4_dynamo`, bank 15, 137 sprites 0–136; all
15 animations clean, checked by eye). The pictures [certain, by eye]:

| # | sprites | updates | picture | used by |
|---|---|---|---|---|
| 0 | 0–10 | 108, holds | **beam-in**: a blue beam line (0, 1), a ball of light (2, 3), a blue sphere with his outline (4–6), then his body flickers between blue, white and his colours (7–10) | INTRO |
| 1 | 15 11 13 11 | 56, loops | stand (a dark armoured reploid with a long white-grey hair tail and orange boots), slight bob | IDLE and between steps |
| 2 | 15, 23–32, 134, 135 | 31; flag 1 at 25 | throw: he winds up and throws; a pink trail (30–32) at the flag | BOOMERANG |
| 3 | 39–42, 15 | 54, holds | catch: he holds an arm out | a returning blade |
| 4 | 15, 43–55, 18, 16 | 171; flags 1, 2, 3 at 12, 48, 76 | leaping slash: a crouch, a forward dive with a pink sabre, a large pink **crescent** (52), then the landing crouch | LEAPING SLASH |
| 5 | 18, 16, 15 | 65 | crouch, rise: a jump | PILLARS, CENTRE VOLLEY, STUN escape |
| 6 | 73–75 | 3, loops | charge: a green ring of energy above his raised hand | PILLARS |
| 7 | 75, 78–86, 88 | 55; flags 2, 1 at 7, 23 | pillar strike: he punches the floor; a green-cyan wave spreads along the floor (82–86) | PILLARS |
| 8 | 15, 89–93 | loops from frame 2 | firing pose: red-pink energy at his hand | CENTRE VOLLEY |
| 9 | 89, 15 | 7 | end of the volley | CENTRE VOLLEY |
| 10 | 136 | 1 | hurt (bent back) | STUN, death |
| 11 | 125 | 1 | stands after the explosion | death |
| 12 | 10 … 0 | 115; flag 1 at 59 | **beam-out** (animation 0 in the other order) | death |
| 13 | 98 101 100 99 | 4, loops | spinning blade: a small red-pink double blade, 4 angles | `var` 0–3 |
| 14 | 118–124 | 12, loops | pillar: a tall blue (118) or purple (119) beam of light, then thinner lines (120–124) as it fades | `var` 4 |

Sprites that no animation uses: 12, 14, 17, 19–22 (stand and jump poses), 33–38, 56–72 (more
poses; 56–72 hold a green energy ring), 76, 77, 87, 94–97 (purple energy at his hand), 102–117
(small blue shots, drops and a blue splash), 126–133 (blue beams with a blade at the base)
[certain for the list; what 102–117 and 126–133 were for is not known].

## Shots (visual object type 34)

Entry `0x800FC7D8`: `CornerCheck` (when `+0x84` and `+0x70` are 0), then `var` 0–3 use the state
table `0x800FCE30` (set-up `0x800FA984`, main `0x800FABC0`, end `0x800FC898`) and `var` 4 uses
`0x800FCE3C` (set-up `0x800FC8CC`, main `0x800FC9AC`, end `0x800FCA64`). Each copies his texture,
palette, animation table and sprite data; priority 1. Every shot ends at once when he is stunned
(`0x800FCE5C`) or dead (`0x800FCE58`) [certain].

### Blades (`var` 0–3) [certain]

- Set-up: animation 13; attack box `0x800FCE24` (−16, −15, 32, 22), hurt box `0x800FCE28` (−16,
  −7, 32, 14); damage table `0x80075AFC` (below); contact **6**; HP 100, written again on every
  update, so a blade cannot be destroyed. Start: 16 px in front of him, at his height (`var` 2 and
  3: 8 px higher). Speed **4.5 px/frame** (3 × 1.5): `var` 0 straight forward; `var` 1–3 aimed at
  the player once (`ratan2`, `rcos`, `rsin`). Sound 67. His blade count `+0x07` + 1.
- The **shot box** of the room: x 48–304, y 16–176 (the room points).

| `var` | made by | flight |
|---|---|---|
| 0, 1 | BOOMERANG (0 first, then 1) | Flies until it is more than 16 px left or right of the shot box, or above or below it; then it **stops for 180 updates**. Then sound 67 and it flies back, aimed once at **his** position, at 4.5 px/frame. Within 64 px of him (\|dx\| + \|dy\|): he faces it and plays animation 3; 10 updates later it ends. |
| 2 | CENTRE VOLLEY, player X | Aimed at the player; ends 80 px (x) or 40 px (y) outside the shot box. |
| 3 | CENTRE VOLLEY, player Zero | Flies until it leaves the shot box, **stops for 120 updates**, then sound 67 and it flies at the player (aimed once), and ends 80 / 40 px outside the shot box. |

- **A hit knocks it away.** The blade table deflects every id except 24–31, 33, 37–42, 47, 56, 57
  (1 damage, class 0). Such a hit (result > 0, not 127) turns the blade into step 4: no boxes, x
  speed 5 px/frame in the other x direction (random when it was 0), y speed 5 px/frame up or down
  at random. It blinks and ends 80 / 40 px outside the shot box. So Zero's sabre (ids 24–31) and
  Zero's ids 33 and 37–42 (`docs/enemies/high-max.md`, "Weapon ids") and X's ids 47, 56, 57 can bat
  a blade away; the X-Buster cannot.

### Pillars (`var` 4) [certain]

- Set-up: animation 14, no boxes, no damage table, HP 1, contact **8**; 54 px above his origin
  (86 px above the floor), at 36 × k px in front of him (k = 1 … 8).
- Main: `ContactMega`. After **20 updates**: attack box `0x800FCE2C` (−11, −96, 22, 192). After **60
  updates** in all: the end. No hurt box: a pillar cannot be hit.
- So each pillar is harmless for 20 updates and then hurts for 40 updates. Its box is 22 px wide,
  from 182 px above the floor to 10 px below it: a jump does not clear it. Between two pillars
  there is a gap of 14 px, and 25 px between him and the first pillar [likely: the safe places are
  the gaps, the space next to him and behind him; not checked].

## Damage

`D` = deflected, `–` = no effect, a number = damage (`docs/engine.md`, "The damage tables").

`0x80075A5C` (`dmg_tbl_dynamo`):

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 2 2 5 – 2 1 2 2 2 2 2 8 |
| 16–31 | 2 8 4 4 4 D 8 D 1 1 2 2 2 2 2 2 |
| 32–47 | 2 2 2 2 2 2 2 2 2 2 8 2 D 1 4 8 |
| 48–63 | 2 1 2 2 1 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

The ids that matter:

| ids | damage |
|---|---|
| 0 (lemon), 9 (half charge), 45 | 1 |
| 18, 19, 20 (full charge parts) | 4 each |
| 24, 25 / 26–31 (sabre combo) | 1 each / 2 |
| 6, 15, 42, 46 (they also stun him) | 5, 8, 8, 4 |
| 17, 22, 47, 57 | 8 |
| 60, 62 | 15 |
| 21, 23, 44, 64–78 | deflected |
| 7 | no effect |

With 50 HP (before the part bonuses and the Easy × 1.25): 50 lemons, about 13 full charges, or
about 7 sabre combos at 7 damage each (1 + 1 + 2 + …; the 30-update invulnerability lets only some
hits of a combo count [likely]).

Blades `0x80075AFC`: all D (ids 7, 23, 79 –) except 24–31, 33, 37–42, 47, 56, 57: 1.

Dealt (before the difficulty scaling): body 4; blade 6; pillar 8. The slash box uses the body's
contact damage (4).

## Sounds (bank 2 unless marked)

| id | when |
|---|---|
| 71 | intro, the fall starts |
| 53 | each jump; the leaping slash |
| 66 | landing |
| 67 | a blade is thrown or turns back |
| 68 | pillar charge start |
| 59 | pillar strike animation |
| 69 | pillar strike (with `Rumble(5)`) |
| 70 | the pillars appear |
| 56, 92 | the slash |
| 62 | stun |
| 64 | after his last text |
| 72 | he leaps away |
| bank 0, 14 | HP bar tick |

## Summary for a reimplementation

- A floor boss, 12 × 50 px (16 × 34 in the slash), contact 4, HP 50 (58 / 62 / 66 at higher boss
  levels). After a hit: 30 updates of invulnerability (2 for the first sabre hits). Weapon ids 6,
  15, 42 and 46 stun him.
- Between moves he stands 60 updates. Above half HP: 3–5 boomerang blades (the first straight, the
  rest aimed; 4.5 px/frame; they wait 180 updates at the room edge and fly back to him), or he
  jumps to the side away from the player and makes 8 pillars in a row in front of him (36 px apart,
  harmful for 40 updates after a 20-update warning). At half HP or below: pillars, or he jumps to
  the room centre, fires 6–8 aimed blades 50 updates apart (against Zero they wait 120 updates at
  the edge and then home once), and then does a leaping slash at the player.
- Blades: contact 6, cannot be destroyed; a sabre hit bats them away.
- Stun: 80 updates of shaking (a hit ends it early and drops a 200-soul soul, at most 3), then a
  jump to the other side.
- Death: 127 updates of flashing, the explosion, then he stands, speaks and beams away upwards; exit
  request `0x10`.

## Open questions

1. One state only, in the BOOMERANG step. The jump arcs, the pillar gaps, the slash reach, the stun
   and the death are read from the code, not seen.
2. Check the stun weapons in play: ids 6 and 15 are X's Meteor Rain (slot 6, uncharged and
   charged; `docs/players/weapons.md`), 42 Zero's Ensuizan (`docs/players/zero.md`), 46 X's
   Shadow Armor charged buster (`docs/players/attacks.md`) [likely for the names].
3. `ROCK_X6.DAT` member 140 is probably his data (members 138 and 139 are in RAM in the Zero
   Nightmare and High Max states; see `docs/enemies/high-max.md`) [not checked].
4. Why the intro moves the camera target min x to max x − 16, and what camera `+0x48` (1 in the
   intro, 8 after it) does. High Max writes the same field.
5. The x speed of the stun escape only when he faces left.
6. Sprites 102–117 and 126–133 (blue shots and beams) are in his bank but in no animation.
