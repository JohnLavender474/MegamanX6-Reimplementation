# Player code map (X and Zero)

Date: 2026-10-21. Step 1 of the player RE plan (process log section 76). Sources: the player kits
(`python3 -m mmxdemake.rekit --player`, in [`~/mmx-data/x6/re/x/kit/`](../../work/x/kit/) and `.../zero/kit/`), the
player exports (`players/x`, `players/zero`), and the scripts in [`~/mmx-data/x6/re/x/map/`](../../work/x/map/). The
state is `intro-1_start` (X) unless a line says otherwise.

## Code layout

| part | where | functions walked | note |
|---|---|---|---|
| `PlayerUpdate` and the 32 steps | main program, `0x80034FBC`–`0x800381F0` | 217 in the main program (X kit) | shared: X and Zero use the same 32 step addresses [certain] |
| X's own code | overlay 0 at `0x801EA000` (44 588 bytes) | 180 | [certain] |
| Zero's own code | overlay 1 at `0x801EA000` (22 964 bytes) | 64 | the rest of the area still holds old bytes of overlay 0 in a Zero state [certain] |

- **The shared code calls each character's overlay at fixed addresses**, under a test of `mega+0x02`
  (`var`: 0 = X). Example: the stand step calls `0x801EB61C` when `var` = 0 and `0x801EC9C4` when
  not. So in X's RAM the Zero targets land inside X functions (17 of them), and in Zero's RAM 5 X
  targets do. The kit lists them as "the other character's entry" and does not walk them
  [certain].
- **The stand step is a chain of "try" calls.** Each returns not 0 when it has started an action
  (and set the step), and the chain stops there. The order in step 2: requests
  (`PlayerRequests`), `0x8003A3DC`, `0x8003F13C`, Zero's three tries, `0x8003A870`,
  `0x80039C40`, Zero's three more, `0x80039C9C`, `0x80039CFC` → walk start (`0x8003AE8C`), and at
  last the character's idle (`0x801EB61C` X, `0x801EC9C4` Zero) [certain for the order].
- **Animations** go through the player's own `SetAnime` (`0x8003BA04`, 99 calls in the X kit, 90
  with a constant animation) and `SetAnimeAt` (`0x8003BB94`, 23 calls, 17 constant). Both pick the frame set first (`docs/engine.md`, "Frame
  sets") [certain].

## The 32 steps

"Entered by" is the function that stores the step number into `mega+0x05`; "from" lists the steps
that reach that function. The animations are the constants given to the player `SetAnime` in the
step and the functions only it uses; see `~/mmx-data/x6/sprites/players/x-falcon/anim-<n>.png`
(and `x-no-armor/`).

| step | routine | name | sheet | evidence |
|---|---|---|---|---|
| 0 | `0x80035144` | beam-in [certain] | ladder-hurt-events | set up by `PlayerInitBeam` (`0x8003D850`: animation 1, `speed_y` −8.0); falls at 8 px/update, lands with animation 2, loads the form, stands |
| 1 | `0x8003674C` | beam-out [certain] | ladder-hurt-events | entered by `BeamOut` (`0x8003B954`: `speed_y` +8.0) from `PlayerExit` and step 25; rises until off screen, then player state 3 |
| 2 | `0x80035274` | stand [certain] | movement | |
| 3 | `0x800353C0` | walk start [certain] | movement | |
| 4 | `0x800354E8` | walk [certain] | movement | |
| 5 | `0x80036800` | Zero's stop pose [certain] | movement, ladder-hurt-events | entered by `Stop` only when `var` ≠ 0 (animation 9) |
| 6 | `0x80035620` | jump [certain] | air | entered by `StartJump` (`0x8003AF1C`) |
| 7 | `0x800368FC` | fall [certain] | air | entered by `ToFall` (`0x8003AFF4`) and at the top of 6 and 10 |
| 8 | `0x80036A04` | land [certain] | movement | entered by `ToLand` (`0x8003B054`) |
| 9 | `0x80036B10` | wall grab [certain] | air | `StartWallGrab` (`0x8003B3B8`, animation 13); 8 updates, then 11 |
| 10 | `0x80036C28` | wall kick [certain] | air | `StartWallKick` (`0x8003B440`, animation 14); goes to 7 |
| 11 | `0x80036E70` | wall slide and push-off [certain] | air | `StartWallSlide` (`0x8003B548`, animation 15), `WallRelease` (`0x8003B5B8`) |
| 12 | `0x800370B0` | ground dash [certain] | movement | `StartDash` (`0x8003A9C4`); sub-steps `dash_steps` |
| 13 | `0x80037130` | air dash [certain] | air | `StartAirDash` (`0x8003B298`, animation 18); X only in form 4, Zero always |
| 14 | `0x8003744C` | ladder moves [certain] | ladder-hurt-events | sub-steps `ladder_steps`: from the ground (27), onto the top (28), down from the top (29), off at the bottom and let go (30) |
| 15 | `0x80037610` | ladder climb up [certain] | ladder-hurt-events | `ToLadderUp` (`0x8003B7B4`); 1.5 px/update; animation 31 |
| 16 | `0x80037704` | ladder climb down [certain] | ladder-hurt-events | `ToLadderDown` (`0x8003B810`); animation 32 |
| 17 | `0x80037838` | hit reaction [certain] | ladder-hurt-events | `PlayerHurt` (`0x8003963C`) from `PlayerHitCheck` (`0x80039228`) on HP bit 7; animations 33–37 |
| 18 | `0x80037BC4` | ride [certain]; the ride object is `actor2` (`docs/players/ride-armor.md`) | ladder-hurt-events, ride-armor | `PlayerRideCheck` (`0x8003A268`, `mega+0xD5` > 0; stage overlays 06, 28); jump out when `+0xD5` = 0. The first map called it "warp out" [wrong] |
| 19 | `0x80037C74` | boss gate walk [certain] | `docs/enemies/boss-gate.md` | |
| 20 | `0x80037DC4` | event hold [certain] | ladder-hurt-events | |
| 21 | `0x80037E88` | event walk [certain] | ladder-hurt-events | animation 38; `+0xD2` = 0: walk to camera x + 64, then 20 |
| 22 | `0x80037F70` | event warp out in place [certain] | ladder-hurt-events | animation 3, then hidden |
| 23 | `0x80037FD4` | event jump [certain] | ladder-hurt-events | animations 10, 11; on landing 20 |
| 24 | `0x8003809C` | Light capsule pose [certain] | ladder-hurt-events | animation 107 (X) / 5 (Zero), then 39 |
| 25 | `0x80038144` | victory and beam-out [certain] | ladder-hurt-events | `exit_req` `0x10`; waits for the music stream, animations 39, 40, then step 1 |
| 26 | `0x80035D38` | crouch [likely] | movement | `StartCrouch` (`0x8003B9AC`) |
| 27 | `0x8003605C` | rope hang [likely] | air | `TryHang` (`0x80039984`): up held, tile 46. The first map called it "wall slide" [wrong] |
| 28 | `0x800361D4` | bar hang [certain: live in `scaravich-12_boss-fight`, Zero on a flat bar (tile 45) of the boss room, sub-step 1, animation 163, `speed_x` 2.0] | air | `TryHang`: up held, tiles 37–45 |
| 29 | `0x80037F34` | event walk at 1 px/update [certain] | ladder-hurt-events | animation 38 |
| 30 | `0x80036364` | event leap [certain] | ladder-hurt-events | animations 10, 11, 81, 82 |
| 31 | `0x800381F0` | super jump [certain] | air | `TrySuperJump` (`0x8003A870`); sub-steps `super_jump_steps` (`0x80073C58`) |

Sheets: movement = `docs/players/movement.md`, air = `docs/players/air.md`, ladder-hurt-events =
`docs/players/ladder-hurt-events.md`.

**More than 32 steps.** `player_steps` has no bound test and 78 entries: X 32–47 and 74–77, Zero
48–63, 64–71 stand, and 72 and 73 have no writer [certain for the table]. See
`docs/players/steps-extra.md`. The player kit walks all 78 entries.

## Weapon objects

The weapon objects (32 × `0xA0` at `0x800950A0`) run from a fixed type table at `0x80071220` in the
main program (loop in the function at `0x8002012C`) [certain]. In the first 64 entries:

- types 0, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17–23, 32, 43, 45–47, 49–52, 56, 57, 59, 60, 63 start
  a function in X's overlay;
- types 24–31, 33–42, 44, 48, 58 start a function in Zero's overlay;
- types 1, 4, 7, 10, 13, 16, 53–55, 61, 62 are main-program functions (shared).

The type is the weapon id [certain]: the spawn copies the id into the type byte (`0x8003FA60`,
`0x80049898`). Ids 9 and 18–20 are made by the muzzle flash (effect type 2); see
`docs/players/attacks.md`.

Type 79 is not a player shot: it is the probe box of the laser in Shield Sheldon's stage
(`0x80044554`, main program; `docs/enemies/laser-reflector-puzzle.md`). It follows a visual type 11
laser and has no damage.

## Fields of `mega`

The X kit's field table counts 128 functions that use `+0x06` (sub-step) and 109 that use `+0x05`
(step). The counts include X's weapon objects in the overlay, which use the same field offsets for
their own data. Fields named so far are in `catalog/helpers.json` (`mega.*`); open fields with many
users: `+0x7C`/`+0x80`/`+0x84`/`+0x88`/`+0x8C` (the engine's `work` fields; 77, 35, 43, 14, 74
uses), `+0x89` (69), `+0xD3` (49), `+0xDC` (written by the player `SetAnime`). Named since:
`+0x8E` shot pose, `+0x93` selected weapon, `+0xE7` form, `+0xEC` part bits.

## Open items

- The table words after entry 31 are steps 32–77 (above); `dash_steps`, `air_dash_steps` and
  `ladder_steps` follow at `0x80073C04`, `0x80073C10`, `0x80073C1C` [certain].
- Zero's step bodies 48–63 in detail, and the official names of Zero's techniques.
- X animations 85, 180–182 and Zero animation 118 use sprites past their frame set.

## Step 2 (movement core): done

1. Ground: `docs/players/movement.md` (2–5, 8, 12, 26).
2. Air: `docs/players/air.md` (6, 7, 9–11, 13, 27, 28, 31).
3. Ladder, hit reaction, beams, ride and events: `docs/players/ladder-hurt-events.md` (0, 1, 5,
   14–18, 20–25, 29, 30).
4. Steps 32–77 and the forms: `docs/players/steps-extra.md`.

## Step 3 (attacks): done

X's buster, the charge, the shots and the weapon ids: `docs/players/attacks.md`.
