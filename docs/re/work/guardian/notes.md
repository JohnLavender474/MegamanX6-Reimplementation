# Guardian (stage 7, overlay 10, main type 30): notes

## Method

- Kit: `python3 -m mmxdemake.rekit --name guardian` (export `enemy-6575747cb9`, now in the folder
  `guardian`). The first kit came from `--state sheldon-8_lower-1.state.gz --type 30`; the walk is
  the same (36 functions, 7 state tables).
- A second kit from the stage 22 state: `kit-st22/` (`rekit --name guardian-st22 --state
  sheldon-11_laser-shot.state.gz --type 30 --out ~/mmx-data/x6/re/guardian/kit-st22`).
  `compare_st22.py kit/code.txt kit-st22/code.txt` compares the two function by function. All
  differences are relocated addresses; the boxes (`0x800F3578`…) have the same bytes. So stage 22
  (a hidden area, other overlay) has the same Guardian code.
- Read: `kit/summary.md`, `kit/calls.txt`, `kit/code.txt` (the Guardian and its shot,
  `0x800EFE48`–`0x800F0BB0`), the boxes in `kit/data.txt`. Engine rules from `docs/engine.md`.
- Not the Guardian: the shot state table `0x800FC4DC` has 3 entries; the next words
  (`0x800F1728`, `0x800F1014`, `0x800F1374`) belong to the next table, another object (they read
  `mega` state and floor tiles). The kit lists them under the shot by mistake.
- Live check: `../mettaur-d2/probe_live.py 27,30 35 <states>`.

## Code map

| address | what |
|---|---|
| `0x800F03F8` | entry: state table `0x800FC460` |
| `0x800EFE48` | state 0, set-up |
| `0x800F006C` | state 1, main |
| `0x800F0434` | state 2, death explosions (32 updates) |
| `0x800F04A0` | states 3 and 4: `DeleteObject` (3) / `DeleteObject2` (4) |
| step table `0x800FC474` | 0 `HitReactionDispatch`, 1 `0x800F0994` (step = `+0x94`; nothing sets it), 2 and 6 empty, 3 WAIT `0x800F04F8`, 4 TURN `0x800F0648`, 5 ATTACK `0x800F073C`, 7 `0x800F0938` (four empty sub-steps) |
| WAIT step2 `0x800FC494` | 0 `0x800F0534`, 1 `0x800F0570`, 2 `0x800F05EC` |
| TURN step2 `0x800FC4A0` | 0 `0x800F0684`, 1 `0x800F06B8`, 2 `0x800F0710` |
| ATTACK step2 `0x800FC4AC` | 0 `0x800F0778`, 1 `0x800F07AC`, 2 `0x800F07F4` (aim), 3 `0x800F0258` (fire), 4 `0x800F08A8`, 5 `0x800F08FC` |
| `0x800F09A0` | "player behind": `(flip XOR ((x < mega.x) << 6)) != 0` (16-bit pixels) |
| `0x800F09C4` | visual type 35 (the shot): init `0x800F0A00`, move `0x800F0AB4`, delete `0x800F0B90` |

## Key facts

- Set-up `0x800EFE48`: HP 6, `iframes` 0, animation table `0x800FC428`, priority 6, no attack or hurt
  box yet, all speeds 0, `+0x99` = 1, `+0x8B` = 0 (closed), layer = `mega.layer`. `flip` = `0x40`
  when `var & 0x0F` ≠ 0, else 0. Animation 0, state 1, step 3 (WAIT), step2 0.
- The four box pointers are **overlay globals**, `0x800FCFF0`…`0x800FCFFC`, written by every set-up:
  - `var & 0xF0` = 0: terrain `0x800FC2AC`, contact 6, closed attack `0x800FC29C`, open attack
    `0x800FC2A0`, body hurt `0x800FC2A8`, second attack `0x800FC2A4`.
  - `var & 0xF0` ≠ 0: the sprite set of the **next** bank: `GetSpriteBank(30)` + 1 picks the texture
    (`bank_tpage[bank + 1] >> 7`), the palette (x = ((bank + 1) × 4 + 32) mod 16 × 16 px, y = 480 +
    (bank + 9) / 4) and the sprite data (`sprite_bank_base[bank + 1]`); contact 7, terrain
    `0x800FC2C0`, closed attack `0x800FC2B0`, open attack `0x800FC2B4`, body hurt `0x800FC2BC`,
    second attack `0x800FC2B8`.
  - The stage type list of stage 7 area 0 (`0x800FAC8C`) holds type 30 twice: bank 7 and bank 8
    (stage 22 too: banks 6 and 7). `GetSpriteBank` returns the first. Bank 7 is the normal Guardian
    (front shield, two thrusters); bank 8 is a larger Guardian with a wide spiked shield
    (`~/mmx-data/x6/sprites/banks/stage-7/*/bank-8.png`). The export `guardian` has bank 7 only.
  - Because the box pointers are shared, a set-up of one kind changes the boxes of every live
    Guardian until the next set-up [certain for the code].
- The formula checks: the live normal Guardian (bank 7) has palette (192, 483); with bank + 0 the
  formula gives (60 mod 16 = 12 → 192, 480 + 15 / 4 = 483). The Mettaur D2 (bank 9) has (64, 484):
  (68 mod 16 = 4 → 64, 480 + 17 / 4 = 484). Texture 0x2D0 = `0x16800 >> 7` = bank word 7.
- Main `0x800F006C`, each update:
  1. prev pos, `AnimeTick`, the step.
  2. Unless state = 2: attack box = closed or open box (by `+0x8B`), `ContactMega`; attack box =
     second box, `ContactMega`. So two contact tests per update.
  3. Shield test: `dmg_tbl_deflect`, hurt box = `0x800FC2C4` (closed) or `0x800FC2C8` (open),
     terrain box = `0x800FC2AC` or `0x800FC2CC` (both (0, 0, 16, 16)); `ContactWeapon`.
  4. When the result is ≥ 0 (0, 126 or 127): `dmg_tbl_normal`, hurt box = body box,
     `ContactWeapon` again.
  5. Killed: `DropItem(20)` unless −56 (weapon id 55); `SpawnExplosion`, terrain box 0,
     `SpawnJunk(4, {8, 9, 10, 11})` (`0x800FC45C`), `+0x7C` = 32, `+0x7E` = 6, `display` 0, state 2.
  6. Else off screen 96 (`OffScreenCheck2(96, 96)`) → state 4; else `DisplayCheck(64, 64)`; the
     squeeze test as in the Mettaur (explosion and debris, no drop); else `CollideCheck`.
- `ContactWeapon` (`0x80031474`) and `ApplyWeaponHit` (`0x800315CC`): the object keeps the hit tag
  of the last shot in `+0x65`; a shot with the same tag returns 126 (no damage). `ContactWeapon`
  clears `+0x65` when no shot touches the box. So a shot deflected by the shield cannot also hurt
  the body in the same update [certain]. In a later update, when that shot touches only the body,
  the shield call clears the tag and the body call can count it [likely; deflected shots fly back,
  so this is rare].
- No move: the Guardian code calls no `MoveObject`, and all speeds stay 0. It hovers at its record
  point [certain; both live Guardians are at their record x, y].
- WAIT (step 3): step2 0: animation 0, `+0x7C` = 120. step2 1: when `+0x7C & 31` = 0 and the player
  is behind: step 4 (TURN). Else `+0x7C` − 1; at 0: step2 2. step2 2: when
  `mega.x − x < 188` (signed): step 4 when the player is behind, else step 5 (ATTACK).
- TURN (step 4): animation 2; on its flag (frame 6, after 11 updates): `flip ^= 0x40`, animation 1,
  `+0x7C` = 20; after 20 updates: step 5 (ATTACK). A turn always goes on to an attack.
- ATTACK (step 5):
  - step2 0: animation 3.
  - step2 1: flag 2 (frame 7, after 22 updates): `+0x8B` = 1 (open). Flag 1 (frame 11, after 37):
    `+0x7C` = 4 (shots left), `+0x7E` = 30, step2 2.
  - step2 2 (aim): `+0x7E` − 1; at 0: x ± 6 (to the front), `+0x81` = `AimAt(self, mega)`, x back,
    step2 3.
  - step2 3 (fire): when the direction is in the back half (facing left: 25…31 or 0…7; facing
    right: 9…23): `+0x7C` = 10, step2 4, no shot. Else animation 4 and a shot: visual object flags
    `0x41`, type 35, `var` = the parent's `var`, texture, palette, animation table, sprite data,
    `flip`, layer, x, y; `SetSpeedDir(shot, dir)`, shot state 0, `speed_x` and `speed_y` × 3. Then
    `+0x7C` − 1: not 0 → `+0x7E` = 20, step2 2; 0 → `+0x7C` = 10, step2 4.
  - step2 4: after 10 updates: animation 12, step2 5.
  - step2 5: flag 2 (frame 2, after 8 updates): `+0x8B` = 0 (closed); flag 1 (frame 9, after 35):
    step 3 (WAIT), step2 0.
- Death (state 2): `+0x7C` − 1; at 0 → state 3 (`DeleteObject`). Else `+0x7E` − 1; at 0: `+0x7E`
  = 6, `SpawnExplosionRand(self, 1, 16, 16)`. So 5 more explosions (updates 6, 12, 18, 24, 30)
  within ±16 px, then the delete at update 32.
- States 3 and 4 also clear `+0x80`…`+0x8F` and `+0x94`.
- Shot init `0x800F0A00`: `dmg_tbl_one_hit`, palette without the flash bit, state 1, `display` 1,
  priority 0, x ± 6 (by `flip`), `PlaySound(2, 79, shot)`, terrain `0x800FC4D8` (0, 0, 5, 5), hurt
  = attack = `0x800FC4D4` (−3, −2, 5, 4), HP 1, contact 3, animation 5.
- Shot move `0x800F0AB4`: `AnimeTick`, `MoveObject`; `ContactMega` ≠ 0 → state 2; killed →
  `SpawnExplosion`, state 2; else clear the flash bit; `+0x70` ≠ 0 or `CornerCheck` →
  `SpawnHitEffect`, state 2; off screen 32 → state 2; else `DisplayCheck(16, 16)`.

## Timing (hand simulation, ±1 update; update 0 = the step2 0 update)

| phase | updates | events |
|---|---|---|
| WAIT | 0–120 | player-behind checks at 25, 57, 89; distance test from 121 |
| TURN | 0–31 | turn at 11, ATTACK at 31 |
| ATTACK | 0–176 | open at 22; aims at 67, 88, 109, 130; shots at 68, 89, 110, 131; close animation at 141; closed at 149; WAIT at 176 |

- So with the player in front and near, one cycle is about 121 + 176 = 297 updates (5 s) and has 4
  shots, 21 updates apart. The shield is open (body hit from the front) for 127 updates (2.1 s).

## Records (list entry 7, area 0; from `sheldon-8` RAM)

| record | `id` | x, y | kind (low digit) |
|---|---|---|---|
| 92–95 | 0 | (3480, 424), (3720, 424), (4016, 424), (4080, 328) | 0 |
| 96 | 1 | (4384, 424) | 0 |
| 97 | 0 | (4512, 424) | 0 |
| 98 | 0 | (4624, 304) | 2 |
| 99, 100 | 0 | (4720, 424), (4752, 376) | 0 |
| 101 | 1 | (4504, 616) | 0 |
| 102 | 16 | (3928, 664) | 0 |
| 103–105 | 1 | (4336, 656), (4240, 576), (4024, 560) | 2 |
| 106, 107 | 0 | (4120, 904), (4168, 888) | 2 |
| 108, 109 | 0 | (4216, 872), (4264, 856) | 0 |

- 12 kind 0 and 6 kind 2 (Hard and above). `id` 0: 12, `id` 1 (faces right): 5, `id` 16 (the large
  Guardian, faces left): 1. The high digits of the kinds in RAM (`0x10`, `0x20`, `0x30`, `0x40`) were
  written during play; they are not read here.
- Stage 22 (`sheldon-11`): records 26–28 are type 30, `id` 0, at (632, 360), (1256, 328) (kind 1),
  (1624, 392).

## Checked against the save states

- `sheldon-8_lower-1`, main slot 10: type 30 `var` 0, record 108 at (4216, 872), the object at
  (4216, 872): it did not move. State 1, step 5, step2 5 (close), animation frame sprite 23 with 1
  tick left (frame 1 of animation 12), `+0x8B` = 1 (flag 2 not yet), `+0x7C` = 0, `+0x7E` = 0, last
  aim `+0x81` = 15 (left, a little up: the player is at (3931, 843), 285 px left and 29 px up), HP 6,
  contact 6, priority 6, attack box `0x800FC2A4` (the second box, set last), hurt box `0x800FC2A8`
  (body, set last), terrain `0x800FC2CC` (open), table `dmg_tbl_normal` (set last), `+0x99` = 1.
- `sheldon-11_laser-shot` (stage 22), main slot 3: the same code in another overlay. Record 26 at
  (632, 360), the object at (632, 360). Step 5, step2 2 (aim wait), `+0x7C` = 4 (no shot yet),
  `+0x7E` = 11 (19 updates after the open-end flag), `+0x8B` = 1, animation 3 held on its last frame
  (sprite 26).
- No Guardian shot (visual type 35) is live in any state, so the shot speed was not seen.

## Open questions

1. Which weapon id 55 is (no drop when it kills).
2. The large Guardian (`id` 16): not live in any state. Its sprites are bank 8; the exporter makes no
   export for the second bank of a type.
3. Step 1 (`step = +0x94`) and step 7 have no use that we found.
4. Animations 6 and 7 are not started by this code (6: a close that ends on sprites 38, 39; 7: 18
   frames of strong thrust, sprites 38–55).
5. Not checked in play: the cycle timing, the open window, the back-half rule, the 188 px rule
   (the player to the left counts at any distance).

## Death Guardian (`id` 16): `sheldon-16_death-guardian`

- Official name (from the user): Death Guardian. Sheet: `docs/enemies/guardian.md`, section
  "Death Guardian". Export `enemy-2dadc31ecb` (proposed name `death-guardian`).
- `var` is read 3 times: `0x800EFEB8` (`& 0xF0`, the kind), `0x800F002C` (`& 0x0F`, the facing),
  `0x800F0308` (copied into the shot). HP 6 is written before the branch. So HP, timing and shots
  are the same as the Guardian.
- Main slot 12: `var` 16, (3928, 664), HP 6, contact 7, data `0x801583D8` (bank 8), texture `0x340`,
  palette `0x7900`, `flip` `0x40`, step 5, step2 2, `+0x7C` 2, `+0x7E` 3, `+0x81` 31.
- The box globals hold the normal boxes (`0x800FC29C`, `0x800FC2A0`, `0x800FC2A8`, `0x800FC2A4`).
  No other Guardian is live: a normal Guardian was set up later and then removed.
- Shot (visual slot 1): `var` 16, bank 8 sprites, speed (2.942, −0.585) = 3.00 px/update, direction
  31, at (3984.02, 673.95) = 17 updates from (3934, 664).
- Probes: `probe_death.py`, `probe_twice.py` (types listed twice in every dump). Identity check of
  the exporter change: `identity/` (old and new code, roots, logs).
