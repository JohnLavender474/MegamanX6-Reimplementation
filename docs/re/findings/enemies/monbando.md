# Monbando (stage 09, main object type 9): behaviour sheet

Monbando is a large purple mechaniloid that stays in one place. Most of the time it is closed in a
guard pose, and all weapons bounce off it. About once per second it opens and attacks in one of three
ways: a ground slam that sends three blue waves along the floor, a homing missile, or a short punch.
It always turns to face the player when it is not attacking.

Overlay `09` (Rainy Turtloid), save state `turtloid-1.state.gz` (stage 6, area 0). Written with the
RE kit (`python3 -m mmxdemake.rekit --name monbando`) and `docs/re-guide.md`. Units, confidence tags
and the shared engine rules: `docs/engine.md`. Records: `docs/stages/turtloid.md`.

## Spawn and set-up [certain]

- **HP 32**, contact damage **6**, `iframes` = 0.
- Boxes:
  - terrain box `0x800F5A48` = (0, 0, 24, 24): a 48 × 48 px body;
  - contact box `0x800F5A4C` = (−16, −24, 32, 48): 32 × 48 px;
  - hurt box `0x800F5A50` = (−24, −24, 48, 48): 48 × 48 px.
- Draw priority 4. Faces the player.
- It starts in the FALL step with gravity 0 and animation 0. `CollideCheck` runs at the end of the
  set-up update. When it is placed on the floor, it stays there; when it is in the air, the fall check
  gives it gravity (below) and it drops onto the floor [certain for the code].
- The record `id` (`+0x02`) is **not read** by Monbando's code. All Monbandos act the same.

## State machine

States (table `0x800F5B44`): 0 `0x800EA41C`, 1 `0x800E9D64`, 2 and 3 `0x800EA514` (`DeleteObject` and
`DeleteObject2`). `CollideCheck` after the state.

### Main (state 1) [certain]

Each update, in this order:

1. **Guard:** the damage table is `dmg_tbl_deflect` when the current sprite is 0 (the closed pose),
   else `dmg_tbl_normal`. The test uses the sprite at the start of the update.
2. The previous position is saved.
3. Runs its step (table `0x800F5B54`): 0 engine hit reaction, 1 WAIT, 2 FIRE, 3 RECOVER, 4 FALL.
4. **Turn:** in steps 0, 1 and 4 it faces the player. In steps 2 and 3 (an attack) it does not turn.
5. **Fall check** (standard): gravity `0x4200` (0.258 px/frame²).
6. **Weapon test:** a weapon that kills it → death (below). A blocked hit (result 127, a shot that
   bounces off the guard) **resets the wait timer to 0**.
7. **Squeeze** (floor and ceiling) → death.
8. Contact test; off screen 64 → state 3; visible 32.

### WAIT (step 1) [certain]

- The animation runs (animation 1 closes the guard in 8 updates, then holds sprite 0).
- A timer (`+0x7C`) counts up by 1 per update. While its old value is below 61 nothing happens, so the
  wait is **62 updates (about 1 s)** after the last reset.
- A blocked hit resets the timer. So **a player who keeps shooting the guard delays the attack**
  [likely for the effect in play].
- Then it picks an attack with `GetRNG() & 1`:

| roll | player character (`mega+0x02`) | attack | animation | kind (`step2`) |
|---|---|---|---|---|
| 0 | any | ground slam, 3 waves | 4 | 0 |
| 1 | 0 (X) [likely] | missile | 3 | 1 |
| 1 | ≠ 0 (Zero) [likely] | punch | 2 | 2 |

So against X it is half slam, half missile; against Zero it is half slam, half punch.

### FIRE (step 2) [certain]

- The attack animation runs. On the frame with event flag 1 it makes the attack objects (visual object
  type 7, below), then goes to RECOVER.
- Slam: **3 waves**, with start delays 0, 6 and 12 updates. Missile or punch: 1 object, no delay.
- Time from the start of the animation to the flag frame [likely, ±1 update]: slam 27 updates, missile
  49 updates, punch 2 updates.

### RECOVER (step 3) [certain]

- The animation runs to its last frame (sprite 0). Then: animation 1, timer 0, back to WAIT.
- Whole animation lengths [likely, ±1]: slam 62 updates, missile 94 updates, punch 31 updates.

### FALL (step 4) [certain]

- Moves with `MoveObject2`.
- Ends when it has floor contact **and** the current frame has event flag 2: speeds, acceleration and
  gravity become 0, `latch` 0, animation 1, step 1. Only animation 0 has a flag-2 frame (sprite 37,
  after 46 updates), so after the spawn it stands for about 46 updates before it closes.
- A later fall (floor removed under it) keeps the current animation. Animations 1–4 have no flag-2
  frame, so it would stay in FALL and never attack again [certain for the code; it is not clear if
  this can happen in the stage].

## Animations (table `0x800F5B14`)

| # | what | frames |
|---|---|---|
| 0 | spawn / landing: open, arms down | sprites 34 36 37, 23 updates each, flag 2 on 37, loops |
| 1 | close into the guard pose | sprites 34 35 0 (4, 4, 1 updates), hold sprite 0 |
| 2 | punch: arm out (sprite 2, flag 1), back (flag 3 on sprite 1), close | sprites 1 2 3 1 0 (2 6 16 7 1) |
| 3 | missile: arm cannon opens, fires (flag 1 on sprite 7), closes | sprites 1 4 5 6 7 6 1 0 (6 11 8 24 2 36 7 1) |
| 4 | ground slam: arm up, fist down onto the floor (flag 1 on sprite 13, blue splash), closes | sprites 8 10 9 10 11 12 13 14 27 28 29 1 0 (6 2 3 10 3 3 4 4 5 6 9 7 1) |
| 5 | blue ground wave (the slam shot) | sprites 15–22, 1 update each, loops |
| 6 | missile with exhaust flame | sprites 23–26, 1 update each, loops |
| 7–10 | debris pieces | sprites 30–33, 1 frame each |

Sprite 0 is the closed guard pose. Every attack animation ends on sprite 0.

## Shots and effects: visual object type 7 (`0x800EA834`) [certain]

Monbando makes them with `GetVisualObject`; each copies the parent (`docs/engine.md`). Its `id`
(`+0x02`) is the attack kind (0, 1, 2) and `+0x06` is its start delay. States (table `0x800F5B8C`):
0 set-up, 1 move, 2 delete (`DeleteObject3`). In state 1 it waits out its delay first (it is not drawn
and does not move during the delay).

### Kind 0: ground wave (slam)

- Set-up: sound (bank 2, id 29), `Rumble(5)`, `Quake2(8, 2, 2)` (screen shake). Each of the 3 waves
  does this in its own set-up, so all three play in the same update.
- Starts **36 px** in front and **8 px** below Monbando's origin. Speed **2.5 px/frame forward and
  2.5 px/frame down** (diagonal), no gravity.
- When it is on the floor for a second update, `speed_y` and gravity become 0: it **runs along the
  floor at 2.5 px/frame**.
- Deleted (no explosion) when: it had floor contact and loses it (a ledge); it touches a wall; it is
  more than 32 px off screen.
- It is not deleted when it hits the player. No hurt box: weapons cannot destroy it.
- Contact damage **4**. Terrain box `0x800F5B70` = (0, 0, 10, 14) (20 × 28 px); contact box
  `0x800F5B68` = (−8, −4, 16, 16). Priority 4.

### Kind 1: missile (homing)

- Set-up: sound (bank 2, id 28). Starts **32 px** in front and **16 px** above the origin. Speed
  **3 px/frame forward**, no vertical speed.
- A timer starts at 50 and counts down each update. During state-1 updates 12–50 (`0x800EA28C`), the
  missile **steers to the player's height**: 2 px per update up or down when the player's `y` is more
  than 8 px away. After that it flies straight.
- Standard shot ends (`docs/engine.md`): it explodes on the player, on a weapon kill and on terrain;
  32 px off screen it is deleted with no explosion.
- **HP 6.** `dmg_tbl_minor` on difficulty 0 and 1: lemon 3 (2 hits), half charge 5, full charge 15
  (id 18; id 19 does 4), sabre 4 (id 25: 6). `dmg_tbl_deflect` on difficulty 2 and up: on Hard it
  cannot be shot down.
- Contact damage **4**. Terrain box `0x800F5B7C` = (0, 0, 16, 8); contact box and hurt box
  `0x800F5B74` / `0x800F5B78` = (−16, −4, 32, 8). Priority 6.

### Kind 2: punch box

- Set-up: sound (bank 2, id 27). Placed **48 px** in front and **16 px** above the origin. It does not
  move and it is **not drawn** (display 0): it is the hit area of the extended arm.
- Contact damage **8**. Contact box and hurt box `0x800F5B80` / `0x800F5B84` = (−16, −8, 32, 16);
  terrain box `0x800F5B88` = (0, 0, 16, 8). HP 0, `dmg_tbl_deflect`: shots that hit the arm bounce off
  [likely].
- Deleted when Monbando's animation reaches the flag-3 frame (the arm comes back), when Monbando dies
  or leaves (state 2 or 3), or when it is more than 32 px off screen. It lives for about 22 updates
  (sprite 2 for 6, sprite 3 for 16) [likely].

The animation set-up gives kinds 1 and 2 animation 6; kind 2 is never drawn, so only the missile shows
it.

## Damage

- Open (sprite ≠ 0): `dmg_tbl_normal`, 32 HP: 11 lemons, 5 half charges, 3 full charges, 8 sabre hits
  (id 25: 6 hits); id 22 (32) and id 55 (127) kill at once.
- Closed (sprite 0): `dmg_tbl_deflect`: nothing hurts it.
- So it can be hurt whenever its sprite is not 0: during each attack animation, during the 8 updates
  of animation 1 after every attack, and during the spawn animation.
- Dealt: body 6, ground wave 4, missile 4, punch 8.

## Death [certain]

One explosion (`SpawnExplosion`), 4 debris pieces (animations 7, 8, 9, 10, list `0x800F5B40`), drop
table 21, then state 2. The same happens when it is squeezed. Shots already in flight are not removed,
except the punch box.

## Sounds

| bank, id | when |
|---|---|
| 2, 27 | punch |
| 2, 28 | missile launch |
| 2, 29 | ground slam (once per wave, 3 in the same update) |

The slam also makes `Rumble(5)` and a screen shake `Quake2(8, 2, 2)`.

## Summary for a reimplementation

- Stationary; 32 HP; contact damage 6; 48 × 48 px body. Faces the player except during an attack.
- Closed (guard pose) most of the time: all shots bounce off, and a bounced shot restarts its
  62-update wait.
- After 62 updates: 50 % ground slam (3 floor waves, 6 updates apart, 2.5 px/frame, 4 damage);
  50 % missile against X (3 px/frame, steers ±2 px/update for 39 updates, 6 HP, 4 damage) or punch
  against Zero (invisible 32 × 16 box 48 px in front for about 22 updates, 8 damage).
- It can be hurt whenever its sprite is not the guard pose (sprite 0), because the damage table is
  chosen by the sprite each update: during each attack animation (about 31 to 94 updates), during the
  8 updates of animation 1 (sprites 34, 35) after every attack, and during the spawn animation
  (sprites 34, 36, 37, about 46 updates).
- Death: 1 explosion, 4 debris, drop table 21.

## Open questions

1. `mega+0x02` as the character number (0 = X) is inferred from engine code, not read from a Zero save
   state. A save state with Zero would confirm the punch.
2. The second box at `0x800F5B6C` (−8, −8, 16, 16) next to the wave's contact box has no user here.
3. `Quake2` (`0x8002943C`) writes the same kind of values as `Quake`, to other bytes of the layer-0
   camera work. The exact difference (axis or channel) is not read.
4. The exact update counts of the animation timings (±1, from how `SetAnime` and `AnimeTick` count).
5. Whether Monbando can ever lose its floor in the stage (the FALL lock above).
