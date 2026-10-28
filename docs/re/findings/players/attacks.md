# X's attacks (X-Buster, charge, shot objects): behaviour sheet

Step 3 of the player RE plan: the lemon, the charge, the half-charged and fully-charged shots,
the weapon objects that carry them, and the map from weapon ids to shots. The code is in the main
program (the shot try, the charge, the muzzle flash) and in X's overlay 0 (the shot objects). The
step numbers are in `docs/players/map.md`.

Source: the X kit (`python3 -m mmxdemake.rekit --player --state intro-1_start.state.gz`), checked
against three save states in Shield Sheldon's stage (stage 7, X in form 1, no parts):
`sheldon-1_lemons`, `sheldon-2_half-charge`, `sheldon-3_full-charge`. Notes, pseudo code and
reconstructed C are in [`~/mmx-data/x6/re/x/attacks/`](../../work/x/attacks/). Units and tags as in `docs/engine.md`
("Conventions"): speeds in px per update, times in updates.

## Buttons and fields

| field | meaning |
|---|---|
| `mega+0x7C` bit `0x20` | **buster button**: a press fires the lemon (or the charged shot that is ready); a release fires the charged shot [certain for the code] |
| `mega+0x7C` bit `0x10` | **weapon button**: fires the selected special weapon (id `+0x94`). With the buster selected it gives id 50 (see "Weapon ids") [certain for the code] |
| `mega+0x7C` bit `0x40` | Giga attack, when the gauge `+0xA8` is full (`+0x14A`) [certain for the test; not read further] |
| `mega+0x7C` bits `0x200`, `0x400` | weapon select, next and previous; both together: back to the buster [certain for the code; which pad button is which is open] |
| `mega+0x7E` | the buttons held in the update before. So "released" = held before and not now [certain] |
| `+0x8E` `shot_pose` | X is in the shooting pose (animations + 112, `movement.md`) |
| `+0x8F` | 1 in the update of a shot only (cleared at the start of each update) |
| `+0x91` `shot_timer` | updates left in the shooting pose; at 0: `+0x8E` = 0, `+0x90` = 1 |
| `+0x92` `shot_wait` | updates before the next shot; the shot try does nothing while it is not 0 |
| `+0x93` | selected weapon: 0 buster, 1–8 special weapons |
| `+0x94`, `+0x95` | the id for the weapon button and for the buster button, set each update |
| `+0x96`, `+0x97` | the id to fire in this update (−1 none); the id of the last shot |
| `+0x98` | live shots that count toward the limit |
| `+0x9C`, `+0x9E` | buster charge level (0, 1, 2) and charge timer |
| `+0x9B`, `+0x9D` | special-weapon charge level and timer |
| `+0xA1`, `+0x9F`, `+0xA0` | charge glow palette and its blink timer and phase |
| `+0xA2` | muzzle palette time after a buster shot (lemon 6, half 5, plain full 7) |
| `+0xC9` (X) | special weapons owned, one bit per slot [likely] |

All [certain] unless the line says otherwise. `game+0xC9` bit 1 makes a held button fire as a
press (auto fire), bit 0 makes the buster charge with no button held (auto charge); both were 0 in
every state read [certain for the code; that they are options is a guess].

## Order in one update

1. `PlayerUpdate` counts down `+0x92` and `+0x91` (and clears `+0x8F`).
2. Weapon update (`0x8003F048`): weapon select, then the ids `+0x94`, `+0x95` from the selected
   weapon, the form and the charge levels.
3. The step. Steps that can shoot call the shot try `TryShoot` (`0x8003E614`).
4. Charge update (`0x8003EC78`): count the charge, or drop it.
5. Palettes (`0x8003CE38`): the muzzle palette and the charge blink.

So the charge level that a release fires is the level reached at the end of the update before
[certain].

## The shot try (`0x8003E614`)

Nothing while `+0x92` ≠ 0. Else the id comes from the weapon button first, then the buster button:

- Buster button pressed: fire `+0x95`. With no charge ready this is 0, the **lemon**.
- Buster button released: fire `+0x95` when it is not 0 (a charge level was reached).
- The shot is refused when the counted shots `+0x98` are at the limit of that id, below.
- On a shot: `+0x8E` = 1 (except ids 1, 7, 10), `+0x91` and `+0x92` from the time table, X's
  stand animation by id (88 lemon, 90 half, 91 full), the muzzle palette.

The steps that call it: stand, walk start, walk, the three dash sub-steps and land (through
`WeaponPose`); jump, fall, wall grab, wall kick, wall slide, air dash and X's armour air steps
(through `AirWeaponPose`); the ladder (15, 16, 32); the rope and bar hangs; the crouch; one
super-jump sub-step. A wall grab (step 9) refuses only the pose weapons [certain for the callers].
The hit reaction (17) and the event steps do not shoot [certain].

| id | shot | limit (`+0x98` <) | Rapid 5 | `+0x91` pose | `+0x92` wait |
|---|---|---|---|---|---|
| 0 | lemon | 3 | 5 | 20 | 6 |
| 9 | half charge | 3 | 3 | 25 | 15 |
| 18 | full charge (other forms) | 3 | 3 | 35 | 15 |
| 19 | full charge, form 1 (Falcon) | 2 | 2 | 35 | 15 |
| 20 | full charge, form 4 (Ultimate) | 4 | 4 | 35 | 15 |
| 43 | full charge, form 3 (Blade) | 4 | 4 | 35 | 15 |

All shots of ids 0–20 count in `+0x98`, also the charged ones (the muzzle flash adds them, below).
So a half charge cannot fire while 3 lemons are on screen, and the Falcon full charge not while 2
are [certain for the tables].

**Fire rate** [certain]: a lemon sets `+0x92` = 6, so fast tapping can fire one lemon every
6 updates (10 per second), at most 3 on screen. After a charged shot, 15 updates.

## Charge (`0x8003EC78`)

- While the buster button is held, `+0x9E` += 1 per update. At 25: level 1 (half), the charge
  sound (bank 1, id 7). At 90: level 2 (full), no new sound. With part bit `0x100000`
  (Quick Charge): 12 and 45 [certain for the code; the part name, likely].
- The update of the press resets the charge. So: **hold 25 updates after the press for a half
  charge, 90 for a full charge**; a release in a later update fires it [certain].
- The charge is dropped (timers and levels 0, charge sound stopped) by: any new press of the
  buster or weapon button, any release of them, the super jump (step 31), a hit (`ChargeStop`
  from `PlayerHitCheck`), the ride check, `+0xA6` = 1. The charge does not count in steps 0, 1,
  24, 25 and while riding (`+0xD5` ≠ 0) [certain].
- A release in a step that does not call the shot try (for example the hit reaction) loses the
  charge [certain for the order: the try is in the step, the drop is after it].
- The glow: palette 38 at level 1, 41 at level 2 (47 for the Ultimate full charge), blinking:
  2 updates on, 2 updates off [likely for the look].
- Special weapons charge the same way on the weapon button (`+0x9D`, `+0x9B`), only in forms 1, 3
  and 4 and with enough energy; the charged version is id `slot + 9` [certain for the code].
- **Ultimate Buster** (part bit `0x80000`): no charge at all; the buster button gives the full
  charge id at each press; the wait `+0x92` is × 4 (60 updates after a full charge) [certain for
  the code; the name, likely].

## The shots

Each shot starts at X's buster: the point is X's position plus an offset per sprite of X
(`0x8007673C`, or `0x8007693C` for frame set 1), mirrored by the facing, plus an offset per id
(`0x801F3894`): 0 for the lemon, 17 px further forward for the half charge, 14 px for the full
charges. Standing, X's buster is 21 px in front and 9 px above X's origin [certain]. Crouching
(sprites 209, 211, 212, 213: offset (−31, 4)), it is 31 px in front and 4 px below the origin
[certain: in `wolfang-4_x-crouch-shot` the lemon is at x = X + 31 + 8 × 6.75, y = X + 4].

### Lemon (id 0, weapon type 0)

- Made at once by the shot try, as a weapon object, with a muzzle flash (effect type 2 var 0,
  animation 39, sound bank 1 id 8) [certain].
- Speed **6.75 px/update** (8.75 with part bit `0x100`, Speed Shot), straight, no gravity
  [certain; state: 221.5 = 181 + 6 × 6.75].
- Attack box (−10, −6, 18 × 11); terrain box half size 5 × 5 [certain].
- In its first update it follows the buster while X keeps the pose and the facing [certain].
- Ends at a wall (with a spark, effect type 3, and sound bank 1 id 12), at its first hit, or 12 px
  (x) / 8 px (y) outside the view [certain].
- **Deflected** (class `0xFF`): the x speed is reversed, the y speed becomes +4.8 or −4.8 at
  random, the boxes are removed; it flies off the screen [certain].
- Animation 21: 6 frames, 1 + 2 + 2 + 2 + 2 + 2 ticks, then it loops from the second frame [certain].

### Half and full charge: the muzzle flash first

The charged ids 9, 18, 19, 20 are not made by the shot try. It makes a **muzzle flash** (effect
type 2, var = id) and, for 18–20, a voice (id 6) 1 time in 2. The flash follows the buster and
makes the weapon object at the first frame of its animation that has an event flag [certain]:

| id | flash animation | sound (bank 1) | shot appears after |
|---|---|---|---|
| 9 | 22 | 9 | 4 updates |
| 18 | 23 | 10 | 6 updates |
| 19, 20 | 25 | 10 | 6 updates |

The flash is deleted when X enters step 20 (event hold); then no shot is made [certain]. When X
turns or leaves the pose, the flash stays where it is, and the shot starts there [certain].

### Half charge (id 9) and plain full charge (id 18): weapon types 9, 18

| | 9 | 18 |
|---|---|---|
| speed | **8.0** (10.0 Speed Shot) | **10.0** (12.0) |
| attack box | (−13, −7, 29 × 13) | (−34, −19, 64 × 36) |
| animation | 26 (4 frames × 2 ticks, loops) | 27 (3 frames × 3 ticks, loops) |
| off screen | 32 / 20 px | 32 / 20 px |

Both end at a wall or when a hit stops them. **A kill does not stop them**: they go on through an
enemy that the hit destroys; a hit that does not kill stops them [certain for the code:
`WeaponHitReaction` table `0x800737D0`].

### Falcon full charge (id 19, weapon type 19)

- Speed **10.0 px/update**, no Speed Shot bonus [certain].
- Its own frame table (streamed like the player frames), animation table `0x80077A80`; it makes
  effect type 27 var 8 when it starts [certain for the code; the look is not exported].
- Its hit tag changes every 4 updates, but any hit (also a kill) ends it [certain for the code;
  why the tag changes is open].
- Off screen: 32 / 32 px.

### Ultimate full charge (id 20)

Speed 11.0 (13.0 Speed Shot), animation 29; it makes parts of type 21. Not read further [certain
for the speed].

## Weapon ids and shots

The weapon object's type byte is the weapon id: the spawn copies the id into it, so the type
table `0x80071220` is indexed by weapon id [certain].

| id | what | made by | damage `dmg_tbl_normal` |
|---|---|---|---|
| 0 | lemon | shot try | 3 |
| 1–8 | special weapons, slot 1–8 | shot try | (`engine.md`) |
| 9 | half charge | flash | 7 |
| 10–17 | charged special weapons (slot + 9) | shot try | |
| 18 | full charge, forms other than 1, 3, 4 | flash | 15 |
| 19 | full charge, form 1 (Falcon) | flash | 15 |
| 20 | full charge, form 4 (Ultimate); 21 its parts | flash | 16 (21: 1) |
| 43 | full charge, form 3 (Blade) | shot try | 5 |
| 45, 46 | form 2 (Shadow) buster button: plain, and charged / Ultimate Buster | shot try / pose | 4, 15 |
| 50, 51 | weapon button with the buster selected; a pose attack in X's overlay steps 37, 38, 76, 77 [guess: X's saber] | pose code | 4, 4 |

Buster Plus (part bit `0x80`) adds 1 damage to ids 0, 9, 18, 19, 20, 45, 46, 58 (`engine.md`).

## What the save states showed

- **Lemons**: 2 weapon objects of type 0. One at 6.75 px/update, 6 updates after its shot
  (`+0x91` 20 → 14, position 181 + 6 × 6.75). The other has just hit a wall at x ≈ 282 and is in
  its delete state, so `+0x98` = 2 [certain].
- **Half charge**: one type-9 object at 8.0 px/update, 16 updates after the release; it moved 11
  times, so it appeared 4–5 updates after the release, as the flash animation says. It has just
  hit the same wall [certain].
- **Full charge** (the state with no weapon object): the release was 5 updates before
  (`+0x91` 35 → 30, `+0x92` 15 → 10, `+0x96` = 19). The only object is the muzzle flash, effect
  type 2 var 19, one frame before its flagged frame. **The Falcon shot is not spawned yet**: in
  the next update the flash makes weapon type 19. It is not a different pool and the shot is not
  gone [certain for the code and the frame count].

## Play checks

- Tap the buster as fast as possible: at most 3 lemons on screen, one every 6 updates at best.
- Hold the button and count: the charge sound at about 0.4 s (25 updates), the full charge at
  1.5 s (90 updates). Release between them: a half charge.
- Release a full charge: the shot appears about 6 updates (0.1 s) after the release, from a
  flash in front of the buster. A half charge: about 4 updates.
- With 2 lemons still on screen, a Falcon full charge does not fire (and the charge is lost).
- Fire a half charge into a row of weak enemies: it goes on through the ones it kills; a lemon
  stops at the first.
- Fire a lemon at a deflecting enemy: it flies back at a slant, up or down at random.
- Get hit while charging: the charge is lost.
- Turn around in the first updates of a full charge: the shot leaves from the old place and
  direction.

## Summary for a reimplementation

1. Two buttons: weapon (special weapons) and buster. Each update: count down the wait and the
   pose; set the ids from the charge level; run the step (which can shoot); then count the charge.
2. Buster press: lemon at 6.75 px/update from the buster, max 3 counted shots, then 6 updates
   wait and a 20-update shooting pose.
3. Hold: level 1 at 25 updates, level 2 at 90; release fires 9 (8.0 px/update) or the form's full
   charge (Falcon 10.0); 15 updates wait, 25 / 35 updates of pose. Any press, release, hit or
   super jump drops the charge.
4. Charged shots come out of a flash 4 (half) or 6 (full) updates after the release; they count
   toward the shot limit.
5. Lemon: ends at the first hit or wall, bounces off at ±4.8 when deflected. Half and plain full
   charge pierce enemies they kill.

## Open questions

- Which pad buttons are bits `0x10`, `0x20`, `0x40`, `0x200`, `0x400` (the key configuration).
- `+0xA6` and `+0x144`: what sets them (they block some weapons and change the Shadow buster).
- Ids 20/21, 43, 45, 46, 50, 51 in detail; effect type 27 (made by the Falcon shot and by the
  weapon change).
- The Falcon shot's changing hit tag.
- The look of the Falcon full charge (its own frame set; no state has it on screen).
