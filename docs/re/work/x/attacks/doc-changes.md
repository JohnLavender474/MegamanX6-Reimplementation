# Proposed doc changes from step 3 (X's attacks)

The main session decides; this file does not change the repo. The new sheet is
`docs/players/attacks.md`. New names: `helpers-proposed.json` (53 functions, 33 globals, 1 field).

## `catalog/helpers.json` (corrections to existing entries)

- `mega.shot_wait` (`0x80097132`): note → "+0x92 updates before the next shot: TryShoot does
  nothing while not 0 (lemon 6, charged 15); also no ladder grab from the ground".
- `mega.shot_timer` (`0x80097131`): note → "+0x91 shot pose time (lemon 20, half 25, full 35); at 0
  shot_pose = 0, +0x90 = 1".
- `ChargeStop` (`0x8003F3DC`): remove "(likely)"; note → "X: drop the charge (ChargeReset), the
  shot ids again, normal palette; from PlayerHitCheck, PlayerRideCheck, TrySuperJump, 0x8003B870".
- `weapon_types` (`0x80071220`): "indexed by the weapon id [certain]: the spawn copies the id to
  the type byte".
- `WeaponPose` (`0x8003F13C`): note → "X only: TryGigaAttack, then TryShoot; pose weapons (+0x8F)
  run WeaponPoseStep (animation 88 for 5, 14)".

## `docs/engine.md`

- Object classes table: the effect class "update table" is **scratchpad `+0x88`** (set from
  `0x8006DDDC` + stage × 8 + area × 4 at `0x80013EDC`; loop `0x80020378`). Remove the line
  "Scratchpad `+0x88` is one more class table; its class was not read". Effect type 2 = muzzle
  flash (`0x80049478`), type 3 = shot spark (`0x800383F8` uses it).
- Weapon class: add a row or a line: 32 × `0xA0` at `0x800950A0`, `GetWeaponObject`
  (`0x8002C32C`), `SpawnWeapon` (`0x8003D3F8`), loop `WeaponLoop`, type = weapon id.
- "Weapon ids" table: 0 lemon [certain], 9 half charge [certain], 18 full charge for forms other
  than 1/3/4 [certain for the code], 19 Falcon full charge [certain], 20 Ultimate full charge
  [certain], 21 its parts [certain for the code], 43 Blade full charge [certain for the code], 45
  and 46 the Shadow buster button [certain for the code], 50/51 weapon button with the buster
  (pose attack, guess: saber), 10–17 = charged special weapons (slot + 9) [certain]. Replace "18,
  19 full charge (two parts)" and "20 charge shot (another part)".
- "Damage to an object": add the shot side: `WeaponHitReaction` writes the shot's `+0x98` from
  table `0x800737D0` (5 bytes per id): −1 on a deflect, 1 on a hit, and on a kill only when byte 3
  is 0. So ids 9 and 18 pierce what they kill.
- "The player": `mega+0xC9` is X's owned special weapons (Zero: skill bits). `mega+0x7E` = the
  buttons held in the update before. Player update order: `WeaponUpdate` before the step,
  `ChargeUpdate` (`0x8003EC78`) and `ShotPalettes` (`0x8003CE38`) after it (the three "routines
  after the step" of the stop-flag line are `0x8003EC78`, `0x8003CE38`, `0x8003C3C8` =
  charge, palettes, hurt box).
- Part bits of `mega+0xEC` (names likely, they match a public mod tool and the code):
  `0x20` Energy Saver, `0x80` Buster Plus, `0x100` Speed Shot, `0x40000` Rapid 5, `0x80000`
  Ultimate Buster, `0x100000` Quick Charge; from `movement.md`: `0x4` Speedster, `0x8` Jumper,
  `0x10` Hyper Dash.
- Add `docs/players/attacks.md` to the "player specs" line.

## `docs/players/map.md`

- "Weapon objects": "so the type is the weapon id [likely]" → [certain] (the spawn copies the
  id into the type byte, `0x8003FA60`, `0x80049898`); add "ids 9, 18–20 are made by the muzzle
  flash (effect type 2), see `attacks.md`".
- Fields: `+0x89` … `+0x8E` line: `+0x8E` shot pose, `+0x93` selected weapon (the "open fields
  with many users" list).
- Add a "Step 3 (attacks): done" section with `docs/players/attacks.md`.

## `docs/players/movement.md`

- Animations table: 88 is the stand shot of the lemon (and the special-weapon pose of 5, 14);
  add 90 (half), 91 (full), 92 (pose weapons).
- "Stand (step 2)": "in a weapon pose (`+0x8F`), the pose code" → "in the update of a shot
  (`+0x8F`): the stand shot animation by id (`0x801F3698`)".
- Physics constants: part bits named (see above).

## `docs/TODO.md`

- Done: X's buster (lemon, charge, half and full charge, shot objects, ids).
- New items: X special weapons 1–8 and their charged forms 10–17; ids 20/21, 43, 45, 46, 50, 51;
  the pose code (`0x8003F864`) and the Giga attack (`0x8003E344`); effect type 27; `+0xA6` and
  `+0x144`; the key configuration (which pad buttons are `0x10`, `0x20`, `0x40`, `0x200`,
  `0x400`); a sprite tool path for weapon objects (below).

## Sprite tools (no change made)

`mmxdemake.spriteexport` exports main and stage objects from the type list and the bank table;
`mmxdemake.playerexport` exports the player's streamed frame sets. Neither looks at weapon or
effect objects. A tool change for shots would take:

1. A source per live weapon / effect object: the bank is the object's own `+0x3C` (from the
   layout directory at scratchpad `+0x1C`, entries 1, 2, 3 for the buster shots), the texture
   `+0x40`, the clut `+0x42`, the cell mode `flags & 0x60` (`0x20` here). `sprites.Bank` with
   these values draws them correctly (the scripts here do that).
2. The animation table `0x8007789C` (main program, shared) with the animation numbers of this
   sheet, so the strips can be named without a live object.
3. For the Falcon full charge (type 19): a streamed frame set (`+0x38` from scratchpad `+0x14`
   entry `0x3C`, tpage `0x540`), as `playerexport` does for the player, and a state with the shot
   live.
4. Output under `sprites/players/x/shots/` and a `catalog/players.json` entry.
