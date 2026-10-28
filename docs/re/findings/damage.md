# Damage: the full map

This page maps how the game calculates damage: to the objects (enemies, bosses, blocks, enemy
shots) and to the player. It gives the order of the steps, the formulas and the functions. The
damage tables, the weapon ids and the drop tables are in `docs/engine.md` ("Damage to an object",
"Weapon ids", "The damage tables"). The player's hit reaction (step 17, knock-back, blink) is in
`docs/players/ladder-hurt-events.md`.

Source: the main program (`SLUS_013.95`) and the X and Zero overlays in the save states, read
with `python3 -m mmxdemake.rekit --state <state> --at <address>`. Notes and the disassembly are in
[`~/mmx-data/x6/re/damage/`](../work/damage/). All facts are [certain for the code] unless marked.

## The two directions

```
player weapon object (32 slots, 0x800950A0)        enemy object (any pool)
        |                                                  |
        |  the object's own update calls                   |  the object's own update calls
        v                                                  v
ContactWeapon(obj)          SolidWeaponHit(obj)     ContactMega(obj)
  hurt box vs weapon box      terrain box vs          attack box vs player hurt box
  (WeaponBoxTest)             terrain box             (BoxOverlap)
        \                    /                             |
         v                  v                              v
        ApplyWeaponHit(obj, shot)                    PlayerDamageScale(mega, d)
          damage table, bonuses, Easy,                 difficulty, armour, minimum 1
          obj.hp -= d                                  mega.hp -= d, HP bit 7
                 |                                         |
                 v                                         v
        WeaponHitReaction(shot, kind)                PlayerHitCheck (next player update)
          shot ends or goes on, spark, sound           PlayerHurt -> step 17, invulnerability
```

No engine loop does a hit test. Each object calls these functions from its own update, so an
object that does not call `ContactMega` cannot hurt the player, and an object that does not call
`ContactWeapon` or `SolidWeaponHit` cannot be hurt. The main program calls `ContactMega` from 5
places and `ContactWeapon` from 3; every stage and boss overlay calls both (for example overlay 22:
20 and 12 calls). `ApplyWeaponHit` has only the two callers above: an object can only be damaged by
a player weapon object. There is no enemy-to-enemy damage.

## Functions

| address | name | what it does |
|---|---|---|
| `0x80031474` | `ContactWeapon(obj)` | tests the object's hurt box against all 32 weapon objects; the first hit goes to `ApplyWeaponHit` |
| `0x80032B34` | `SolidWeaponHit(obj)` | the same with the terrain boxes (`TerrainBoxOverlap`), for solid objects. No `no_hit` test, no one-hit test, no kill count |
| `0x8002D868` | `WeaponBoxTest(shot, obj, &x, &y)` | the shot's attack box (`+0x50`) against the object's hurt box (`+0x54`). No hit while `obj+0x61` ≠ 0 or a box is null. Writes the hit point to the shot's `+0x80`, `+0x82` [likely for the hit point] |
| `0x800312DC` | `HitPoint(obj, shot, &x, &y)` | the hit point for `SolidWeaponHit`: halfway between the two terrain box centres (weapon id 58: the shot's box edge) |
| `0x800315CC` | `ApplyWeaponHit(obj, shot)` | the damage calculation, `hp −= d`, hit flash, result code |
| `0x800319EC` | `WeaponHitReaction(shot, kind)` | the shot side: end or go on, sound, spark |
| `0x80030ECC` | `ContactMega(obj)` | the object's attack box against the player (or the Ride Armor); hit class, damage, HP bit 7 |
| `0x80030DC0` | `PlayerDamageScale(mega, d)` | difficulty and armour scaling of the damage to the player |
| `0x8003D07C` | `PlayerGrabDamage(d)` | damage while a grabber holds the player (hit class 3) |
| `0x80039228` | `PlayerHitCheck` | each player update: crush, squeeze, spikes, death, invulnerability count, the hit reaction |

## Damage to an object

### 1. `ContactWeapon(obj)`

1. `obj.no_hit` (`+0x7A`) ≠ 0: clear the hit tag (`+0x65`), return 0.
2. Clear the hit flash (palette bit 15, `+0x42`).
3. For each of the 32 weapon objects (`0x800950A0`, `0xA0` bytes each) that is active:
   - The object has `dmg_tbl_one_hit` and the shot's `+0x9C` is 0: skip this shot (see "Shots that
     can hit enemy shots" below).
   - `WeaponBoxTest(shot, obj)` fails: skip.
   - `r = ApplyWeaponHit(obj, shot)`. `r` = 126 (this shot already hit): return 0, keep the tag.
   - `r` < 0 (a kill): with `dmg_tbl_minor` or `dmg_tbl_one_hit`, a shot with `+0x9C` = 1 gets
     `+0x9C` bit 1 (a block, read by the Guard Shell part); with any other table,
     `game.kill_count` (`game+0x94`) += 1.
   - `r` ≠ 0: return `r`.
4. No shot hit: clear the hit tag, return 0.

Result: 0 no hit (also "already hit by this shot"); weapon id + 1 a hit; 127 blocked (deflect,
absorb, stop); < 0 a kill, `−(weapon id + 1)`.

### 2. `ApplyWeaponHit(obj, shot)`

`id` = the shot's type (the weapon id, `shot+0x01`). `entry` = `obj.damage_table[id]` = {class,
damage}.

1. **Hit tag.** The shot's tag (`+0x64`) ≠ 0: when it equals `obj+0x65`, return 126; else store
   it. Tag 0: `obj+0x65` = 0, so a shot with tag 0 can hit in every update.
2. `obj+0x63` = the class. The object code can read it after the call.
3. **Class 2:** return 0 (no hit; the shot is not stopped).
4. **Hit stop.** Ids 26–31, 35, 46, 51, 57, 61 and 62, when the class is not 7:
   `mega.hit_stop_req` (`mega+0xCD`) = 1 (`PlayerTick` makes a 5-update hit stop).
5. **Class 8:** the shot's `+0x98` = 1 (it ends), return 127. No sound, no spark.
   **Class `0xFF`:** `WeaponHitReaction(shot, −1)` (deflect), return 127.
   **Class 7:** the shot's `+0x70` (its terrain flags) |= `0xF`: the shot ends as on a wall, with
   no sound and no spark. Return 127.
6. **Damage** `d` = the table value. When `d` is 0 or 127, skip the bonuses and the Easy step.
   Else, in this order:

   | condition | change |
   |---|---|
   | `mega+0x144` = 2 (a timed item, `docs/players/weapons.md`) | + 1 |
   | weapon id 32 (the Blade Armor Mach Dash) | + the shot's `+0x06` = `mega+0xD9` / 30 (`0x801F2CA8`): `mega+0xD9` counts up to 120 while jump or dash is held in the dash wait, so + 0–4 |
   | part bit `0x80` (Buster Plus) | + 1 for ids 0, 9, 18, 19, 20, 45, 46, 58 |
   | else: part bit `0x400000`, or Zero with `game.armors` bit `0x20` | + 1 for ids 24–31 and 37–41 (the sabre hits) |
   | part bit `0x2000000` | + 1 for ids 1, 4, 33, 34, 35, 36, 42, 44, 52, 53, 54 |
   | part bit `0x200000` | + 1 for ids 1–8, 10–17, 52–54 (all special weapons) |
   | Easy (`game.difficulty` = 0) | `d += d / 4` (rounded down), cap 127 |

   The part bits are `mega.parts` (`mega+0xEC`): see "Part bits" below.
7. `game+0x9C` (16 bits) = the largest `d` so far: the strongest hit in the stage [likely: it is 15
   in the X states (a full charge) and 6 in the Zero states].
8. `obj.hp` (`+0x5C`, a byte) −= `d`.
9. **Main type 59:** `WeaponHitReaction(shot, −1)`, return 127. The HP still went down, but the
   shot bounces off and there is no flash. The test is `obj+0x01` = 59 only, not the object class
   (`0x80031900`). Type 59 is Gate (see "Main type 59: Gate").
10. Hit flash: palette bit 15 on.
11. **HP ≤ 0 (kill):** HP = 0. Class 9: the shot's `+0x70` |= `0xF` and reaction 2; else reaction 1.
    Return `−(id + 1)`.
12. **HP > 0:** main type 32 (in Shield Sheldon's stage, the hidden floor): return `id + 1` with
    no reaction, so the shot goes on. Weapon id 43 (Blade full charge) with HP ≥ 16 left: the shot's `+0x07` = 1.
    Class 9: `+0x70` |= `0xF` and reaction 2; else reaction 0. Return `id + 1`.

Notes:

- Damage 0 with class 0 is a hit with no HP loss. An object at HP 0 dies from it (the objects
  with no damage table, `docs/engine.md`).
- HP is a signed byte. The test is on the byte after the subtraction, so `d` = 127 kills any
  object with HP ≤ 127.
- `ApplyWeaponHit` does not test `obj+0x61`. The box test (`WeaponBoxTest`) does, so an object
  that sets `+0x61` gets no weapon hits until its own code counts it to 0 (D-1000: 30 updates).
  `SolidWeaponHit` uses the terrain box test, which does not read `+0x61`.

### 3. The classes

| class | effect on the object | the shot |
|---|---|---|
| 0 | damage | reaction 0 (hit) or 1 (kill) |
| 2 | nothing | goes on, no sound |
| 7 | nothing | ends as on a wall, no sound, no spark (the Light capsule absorbs most ids) |
| 8 | nothing | ends (`+0x98` = 1), no sound, no spark |
| 9 | damage | ends as on a wall; reaction 2 (sound, no spark) |
| `0xFF` | nothing | deflect: "tink" (bank 0 sound 6), spark, bounces off |

Classes 8 and 9 are not in any table read so far.

### 4. The shot side: `WeaponHitReaction(shot, kind)` and `shot_hit_tbl`

`kind`: −1 deflect, 0 hit, 1 kill, 2 class-9 hit. `shot_hit_tbl` (`0x800737D0`) has 5 bytes per
row: spark variant, sound kind, end on hit, go on after a kill, sound id. Zero uses other rows for
5 ids: id 4 → row 80, ids 1 and 54 → row 81, ids 7 and 53 → row 82.

1. Sound kind 4 (id 55 only): nothing at all.
2. Deflect: id 23 does nothing. Else, with "end on hit" set: `+0x98` = −1 (the shot code then flies
   it back). Sound (0, 6). Spark variant 0.
3. Hit or kill: with "end on hit" set, the shot ends (`+0x98` = 1), except on a kill when "go on
   after a kill" is set. Sound: kind 0 → (1, sound id); kind 1 → (1, 9) on a hit, (1, 10) on a
   kill; kinds 2 and 3 → (5, sound id).
4. Kind 2 stops here. Else an effect object (type 3, var = spark variant) at the hit point, with
   the facing turned. X shows sparks 6 and 7 as variant 3. Sparks 6 and 7 also make a ray object
   (type 15) in a random direction.

What the table gives:

| rule | weapon ids |
|---|---|
| a hit never ends the shot ("end on hit" 0); the shot code decides | 3, 5, 11–15, 22, 24–31, 37–42, 48, 55, 61, 62 and 63–77 |
| goes on through the objects it kills | 9 (half charge), 18 (full charge, forms 0 and 2), 33, 49, 58 |
| ends at the first hit or kill | all the others |

The shot code reads `+0x98`. A weapon can ignore it: the Ultimate full charge (id 20) has "end on
hit" set, but `docs/players/weapons.md` says a hit does not stop it.

### 5. Shots that can hit enemy shots

`ContactWeapon` skips every shot with `+0x9C` = 0 when the object has `dmg_tbl_one_hit`. The
lemon, half charge, Meteor Rain and laser probe objects in the save states all have `+0x9C` = 0.
The code that writes `+0x9C` ≠ 0:

- X's overlay: id 46, the Shadow charged slash (`0x801ED964` in `0x801ED898`); id 47, the Shadow
  Giga parts (`0x801EDB04` in `0x801EDA64`); id 56, the Blade up-slash (`0x801F32D4` in
  `0x801F3200`) [certain for the code]. `weapon_types` (`0x80071220`) holds one address per id
  for both player overlays, so the same address is other code in X's RAM and in Zero's RAM.
  (An earlier version of this page gave ids 33 and 34 here: that was wrong.)
- The main program: state 0 of Metal Anchor (id 4, state table `0x80076170`, `0x800446A4`, all
  `var`s) [certain]; state 0 of Guard Shell (ids 7 and 53, table `0x800761BC`, `0x80045390`), in
  the `var` ≠ 0 branch: the child object that `var` 0 makes [likely]. `0x800456B0` clears bit 1
  again after a block.
- Zero's overlay: the sabre set-up `0x801ED3DC` calls `0x801ED600` once. For an id whose palette
  byte in `sabre_tbl` (`0x801EE68C`, 4 bytes per id from 24: animation, palette, after-image flag,
  one byte not read) is 3 (ids 24–31, 36–42), and with Zero, and part `0x1000000` or Black Zero
  (`game+0x5F` bit `0x20`): `+0x9C` = byte 5 of the entry, that is the palette byte of the next
  id [guess: an off-by-one in the game]. Result: ids 24–30 and 36–41 get 3; ids 31 (wall slash)
  and 42 get 0; ids 35 and 48 do not get to the write. 3 is not 1, so the `|= 2` after a kill
  never happens for the sabre.
- Ids 33, 34, 49 and 58 have no writer: they cannot hit `dmg_tbl_one_hit` objects [certain].
- In Zero's RAM `0x801F32D4` also exists: it is old X code past the end of the Zero overlay
  (`01.bin` ends at `0x801EF9B4`).

So the plain buster shots cannot destroy an enemy shot that has `dmg_tbl_one_hit`: they pass
through it. Only the weapons above can. Enemy shots and small enemies with `dmg_tbl_minor` are
not skipped: every shot can hit them. `SolidWeaponHit` has no such test either.

### 6. Part bits

`game+0x70` holds the parts that the player has. `game+0x74 + 4 × character` holds the parts
that are equipped; `PlayerLoadForm` (`0x8003D740`) copies them to `mega+0xEC` [certain].

- The parts menu is in overlay 16. Its bit table `0x800F0E74` has 24 entries in menu order; equip
  and remove are at `0x800EC35C` (the write at `0x800EC4A8`).
- The stage-result screen (overlay 20, `0x800EA590`) sets the owned bits: `game+0x70 |=
  reward_tbl[n]` for each saved Reploid `n` (`reward_tbl` `0x8006D674`, 128 words).
- The save states agree: `turtloid-5` has Reploids 80–95 saved but not rewarded, owned =
  `0x00040040`; `sheldon-1`, after the Turtloid clear, owned = `0x00450050`. Only
  `yammark-4_blade-armor` (a later save game) has parts equipped: owned `0x02F52D50`, X `0x50`
  (`game+0x74`, copied to `mega+0xEC`), Zero `0x00C00000` (`game+0x78`).

Only `ApplyWeaponHit` and the Zero sabre set-up read the four bits below (a scan of all code). The
in-game names are graphics, not text (misc type 45 of overlay 20, misc type 40 of overlay 16), so
they are open; the names here are by effect.

| bit | menu index | Reploid | effect |
|---|---|---|---|
| `0x200000` | 15 | 110, Shield Sheldon's other area [certain for the table] | + 1 damage for ids 1–8, 10–17, 52–54: all of X's special weapons |
| `0x400000` | 12 | 94, Rainy Turtloid's stage [certain for the table] | + 1 damage for Zero's sabre hits (ids 24–31, 37–41) when part `0x80` is off; sabre palette 4. No effect for X |
| `0x1000000` | 16 | 18, stage 2 [likely: 16 Reploids per stage] | sabre palette 5 (it wins over `0x400000`); the sabre gets `+0x9C`, so it can cut enemy shots. Zero only |
| `0x2000000` | 17 | 122, stage 8 [likely: the same pattern] | + 1 damage for ids 1, 4, 33–36, 42, 44, 52–54: Zero's techniques and the weapon objects that Zero also makes |

### 7. Main type 59: Gate

- Only overlay 24 has a real entry for main type 59 (`0x800F49F0`, main table `0x800F5F88`); in
  all other overlays and in all save states it is the stub `0x80054428`. The table `0x8006DB78`
  gives stage 18 area 0 = overlay 24 [certain].
- It is Gate [certain: `gate-5_gate-fight` has main 59 live in Gate's room after his talk]: the
  set-up `0x800EF400` puts it at (3456, 912), with damage table `dmg_tbl_gate` (`0x8007573C`),
  `game.boss` = itself, `boss_hp_max` = 48 + {4, 6, 8, 10}[boss level], and `PlayerEvent(20,
  64)`. Its step `0x800F4B2C` opens message 1 of the stage 17/18 text bank (members 225, 226):
  Gate's talk. The full sheet is `docs/enemies/gate.md`.
- A weapon hit does not cost Gate HP [certain for the code]. `ApplyWeaponHit` lowers the HP and
  deflects the shot. Then Gate's main state `0x800EF590` writes the old HP back and puts the
  damage (old − new) in byte `0x800F7806` (`gate_damage`).
- Visual type 26 of overlay 24 (`0x800F0A24`) holds Gate's parts. The five orbs (`var` 0–4) add
  `gate_damage` to their `+0x98`; at 16 or more the orb vanishes without fragments. A killed orb
  makes six fragments (`var` 5–10); a fragment within 24 px (x) and 32 px (y) of Gate, while Gate
  is not in step 7, sets `gate_hit` (`0x800F77DC`, at `0x800F3778`). Gate's main state then
  takes 4 HP once and goes to step 8 (hurt) for 90 updates, which clears the flag. So Gate dies
  after 13, 14, 14 or 15 fragment hits (boss level 0–3) [certain for the code].

## Damage to the player

### 1. `ContactMega(obj)`

1. `solid_off` (`0x800C4568`) ≠ 0: no hit.
2. `mega.no_hit` (`+0x7A`) = 1: no hit. `no_hit` = 2: only the visual objects (the enemy shots,
   `0x80093C98` … `+0x1400`) can hit, and the first such hit sets `no_hit` to 0.
3. `mega+0x144` = 1 (a timed item): the visual objects cannot hit.
4. **Riding** (`mega+0xD5` < 0): the target is `actor2` (the Ride Armor). No hit while its HP bit 7
   is set, while `actor2+0x85` ≠ 0, or when the contact damage is < 0. Hit class 1 (`d` < 5) or 2.
   Ride HP −= `d` (no scaling), `game+0x98` += `d`. HP > 0: HP bit 7. Else HP = `0x80`,
   `game+0x1C` = 1. The facing for the push: `0x40` when the ride is left of the object.
5. **On foot:** no hit while HP bit 7 is set (a hit waits, or dead), or `mega.hit_lock`
   (`+0xA4`) ≠ 0. `BoxOverlap(obj, mega)`, which fails while `mega.iframes` (`+0x61`) ≠ 0.
6. Hit class (contact damage `d` ≥ 0 only): `mega+0xCA` = 0, class = `obj+0x62`. Class 0 becomes
   4 for forms 1–4, form 5 with part `0x200`, or Zero with `game.armors` bit `0x20`; else 1
   (`d` < 5) or 2 (`d` ≥ 5). Class 3 (grab) sets `mega+0xCA` = 1. Other classes stay.
7. `s = PlayerDamageScale(mega, d)`. `game+0x98` += `s` (for `s` = 127: += the HP before the hit).
8. HP −= `s`. HP > 0: HP bit 7 (the hit waits for `PlayerHitCheck`). Else HP = `0x80` (dead) and
   `game.player_lock` = 1.
9. The facing for the reaction (`+0xA5`): class 3 none; class 0 with part `0x200` or Zero with
   armours bit `0x20`: the player's own facing (no turn); else `0x40` when the player is left of
   the object, else 0.

A contact damage < 0 on foot skips step 6 and still goes through the scaling, so it heals
(except −1, which the minimum makes a 1-damage hit). No object with a negative contact damage is
known.

### 2. `PlayerDamageScale(mega, d)`

1. `d` = 0: return 0. No minimum: a 0-damage contact is a hit with no HP loss (the reaction and
   the blink still happen).
2. `d` = 127: return 127, no scaling (instant death).
3. Easy (difficulty 0): `d = 3d / 4`. Normal (1): no change. Hard and difficulty 3: `d = d + d / 4`,
   cap 127. All divisions round toward 0.
4. Halve (`d / 2`) for X in forms 1–4 (every armour), for form 5 with `game.armors` bit `0x20`,
   and for any form with part bit `0x200`.
5. `d` = 0: `d` = 1.

| base | Easy | Normal | Hard | Easy, halved | Normal, halved | Hard, halved |
|---|---|---|---|---|---|---|
| 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| 1 | 1 | 1 | 1 | 1 | 1 | 1 |
| 2 | 1 | 2 | 2 | 1 | 1 | 1 |
| 3 | 2 | 3 | 3 | 1 | 1 | 1 |
| 4 | 3 | 4 | 5 | 1 | 2 | 2 |
| 5 | 3 | 5 | 6 | 1 | 2 | 3 |
| 6 | 4 | 6 | 7 | 2 | 3 | 3 |
| 8 | 6 | 8 | 10 | 3 | 4 | 5 |
| 10 | 7 | 10 | 12 | 3 | 5 | 6 |
| 16 | 12 | 16 | 20 | 6 | 8 | 10 |
| 127 | 127 | 127 | 127 | 127 | 127 | 127 |

Check in a save state: `intro-2_junkroid` (X, Falcon, Normal) has HP 31, `game+0x98` = 1 and hit
class 4. That matches a Junkroid shot (contact 1). The Junkroid body (contact 4) would give 2.

### 3. Other HP losses

| source | where | damage | scaling | invulnerability |
|---|---|---|---|---|
| crush (terrain on two opposite sides) | `PlayerHitCheck` | death | – | ignored |
| squeeze (two opposite sides, one an object) | `PlayerHitCheck` | `mega+0xE6`: the pushing object's contact damage, or 127 | none | ignored |
| spikes (tiles 62, 63; not form 2) | `PlayerHitCheck` | death | – | needs `+0x61` = 0 and `no_hit` ≠ 1 |
| pit (camera at its lowest limit) | camera code `0x8002820C` | death | – | ignored |
| grab (hit class 3, held) | `PlayerGrabDamage(d)` | `d` (0: none) | `PlayerDamageScale` | not tested; not added to `game+0x98` |
| acid rain (Turtloid) | the rain controller | 1 every 180 updates, written to HP directly | none | – |
| Ride Armor contact | `ContactMega` | contact damage, to the ride | none | ride `+0x85` |

Details: `docs/players/ladder-hurt-events.md`, `docs/players/ride-armor.md`,
`docs/stages/turtloid.md`.

Crush, spike and pit deaths add the HP that was left to `game+0x98`. A squeeze adds `mega+0xE6`,
or the HP that was left when `mega+0xE6` ≥ HP (`0x80039324`…`0x800393EC`). Grab and acid rain do
not add [certain for the scan, see "Counters"].

### 4. After the hit

`ContactMega` only changes the HP. At the next player update `PlayerHitCheck` finds HP bit 7 and
calls `PlayerHurt`: the reaction by hit class (`hurt_steps`, `hurt_anime`, `hurt_motion`,
`hurt_iframes`), then the invulnerability count `mega+0x61`. `mega.hit_lock` stops new contact
hits from the hit to the end of the blink. See `docs/players/ladder-hurt-events.md`.

## Counters

| field | address | rule |
|---|---|---|
| `game.kill_count` | `game+0x94` | + 1 per kill through `ContactWeapon`, except objects with `dmg_tbl_minor` or `dmg_tbl_one_hit` |
| `game+0x98` | `game+0x98` (32 bits) | the HP that the player lost: the scaled contact damage (`ContactMega` `0x80031000`, the ride path `0x800311C4`), and all the HP that was left at a death by a pit (`CameraClamp` `0x80028304`) or by crush or spikes, and the squeeze damage (`PlayerHitCheck`, `0x80039270`…`0x80039444`) |
| `game+0x9C` | `game+0x9C` (16 bits) | the strongest weapon hit so far, after the bonuses (`ApplyWeaponHit` `0x800318DC`) |

`StageStartReset` clears the stage counters.

**Readers** [certain for the scan]: a scan of the main program and the 59 overlays for the
absolute address and for the `game` base (`0x800CCED0`) plus the offset finds:

- `game+0x98`: one draw function in overlay 20 (`0x800F1D28` in `report-1`) and its copy in overlay
  18. It loads the value, draws a fixed sprite when it is 999 or more, and else draws the digits
  (a divide by 100 at `0x800F1DF4`). Overlay 20 is loaded in the two report save states
  (`report-1`: 177, `report-2_shark`: 198). So the mission report shows the damage taken, with a
  cap of 999 [likely for the screen].
- `game+0x9C`: no reader other than the max test in `ApplyWeaponHit`. A reader that gets `game` as
  an argument (as the stage objects do) is not found by this scan [open].

Scripts: [`~/mmx-data/x6/re/damage/counters.py`](../work/damage/counters.py) and `counters2.py`.

## Examples

- X, no armour, Normal, a lemon (id 0) on Junkroid (HP 12, `dmg_tbl_normal`): 3 per hit, 4 hits.
  With Buster Plus: 4, 3 hits. On Easy: 3 (3 + 0), and 5 with Buster Plus.
- X in the Falcon Armor on Normal, Junkroid body (contact 4): 4 → halved 2. On Hard: 5 → 2.
- Zero on Normal, the same contact: 4. With `game.armors` bit `0x20`: 2, hit class 4.
- A lemon against Shield Sheldon's plate shot (`dmg_tbl_one_hit`): no hit; the lemon passes
  through.

## Corrections to the earlier docs

- `PlayerDamageScale` returns 0 for a contact damage of 0; it was given as "minimum 1".
- `ContactWeapon` returns 0, not 126, when the shot already hit the object.
- The armour halving is not "some armour states": it is every X armour (forms 1–4), Zero with
  armours bit `0x20`, and part bit `0x200`.
- The sabre bonus covers ids 37–41 as well as 24–31, and it is not tested when part bit `0x80` is
  set. Parts `0x200000`, `0x2000000` and the timed item 2 also add 1.
- Class 7 is "absorb" (the shot ends as on a wall); classes 8 and 9 also exist.
- Pierce after a kill is ids 9, 18, 33, 49 and 58, not only 9 and 18.
- "Any hit destroys it" for `dmg_tbl_one_hit` is true only for the weapons that set `+0x9C`. The
  enemy sheets that said so (Sea Attacker ST, Mettaur D2, Guardian, High Max, Shield Sheldon, Worm
  Spider, Zero Nightmare) are corrected. A call-graph walk from the entry of each of these shot
  types ([`~/mmx-data/x6/re/damage/shot_hits.py`](../work/damage/shot_hits.py)) finds `ContactWeapon` in all of them and
  `SolidWeaponHit` in none [certain for the code].
- `+0x61` on an object is an engine rule: `WeaponBoxTest` gives no hit while it is not 0.

## Open questions

1. (answered: `docs/engine.md`, "Weapon ids"; `0x800446A4` is Metal Anchor) The weapons of ids
   32, 33, 34, 49, 56 and 58.
2. (answered: Gate, stage 18; section 7 and `docs/enemies/gate.md`) Main type 59 and the `var`s
   of visual type 26.
3. (answered by effect, section 6) Part bits `0x200000`, `0x400000`, `0x1000000`, `0x2000000`.
   Still open: the in-game names (a save state of the parts menu or the stage-result screen).
4. (answered for `game+0x98`: the mission report, see "Counters") What reads `game+0x9C`.
5. (answered: `0x801ED5F8` is an instruction, not a table; section 5) The sabre `+0x9C` value.
   Still open: the fourth byte of `sabre_tbl`.
6. Whether explosions hurt the player (`docs/engine.md`).
