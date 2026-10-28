# X's special weapons: behaviour sheet

Step 4 of the player RE plan: the eight special weapons of X (slots 1–8, uncharged and charged),
the other weapon ids that X makes, the Giga attacks, the energy, and the pad buttons. The buster
(lemon and charge shots), the shot try and the charge timers are in `docs/players/attacks.md`; the
pose steps and the forms are in `docs/players/steps-extra.md`.

Source: the X kit (`python3 -m mmxdemake.rekit --player --state sheldon-1_lemons.state.gz`), the
main-program weapon code (types 1, 4, 7, 10, 13, 16), and the states of stages 0, 1, 4, 6 and 7.
Notes, pseudo code and reconstructed C are in [`~/mmx-data/x6/re/x/weapons/`](../../work/x/weapons/). Units and tags as in
`docs/engine.md` ("Conventions"): speeds in px per update (16.16), times in updates, a positive
`speed_y` is up.

**No save state has a special-weapon shot on screen** except one Meteor Rain drop in
`shark-9_boss-revive`. So almost all numbers below are from the code [certain for the code], and
the look of each weapon is a play check.

## Slots, weapons and ids

The weapon object's type is the weapon id. Slot n fires id n; the charged shot is id n + 9
(`ShotIds` `0x8003DF58`: `+0x94` = `+0x93`, + 9 at charge level 2) [certain]. Slot n is the weapon
of the Maverick of stage n: `mega+0xC9` (owned, bit `1 << (n − 1)`, table `0x80073ED8`) and
`game+0x60` (beaten) are both `0x21` after the stage 1 and stage 6 clears [likely].

| slot | Maverick (stage) | weapon | ids (uncharged, charged, parts) | proof of the name |
|---|---|---|---|---|
| 1 | Commander Yammark (1) | **Yammar Option** | 1, 10; shots 54 (and 10 var 7) | stage rule; Yammark's own table deflects 1 and 10; ids 1, 10, 54 are the only ones that hurt the Nightmare Bug [likely] |
| 2 | Blizzard Wolfang (2) | **Ice Burst** | 2, 11; shards 52 | 2, 11 stun Rainy Turtloid; Turtloid's weakness is Ice Burst (Fandom) [likely] |
| 3 | Blaze Heatnix (3) | **Magma Blade** | 3, 12 (the slash is 50/51) | the slot 3 shot is a sabre slash with a fire palette and fireballs; the only stage left for Heatnix; 3, 12 stun Blizzard Wolfang (5 and 8 damage), whose weakness is Magma Blade (Fandom), with Zero's id 34 [likely] |
| 4 | Metal Shark Player (4) | **Metal Anchor** | 4, 13 | 4, 13 stun Shield Sheldon (5 and 8 damage); Sheldon's weakness is Metal Anchor (Fandom); Shark's table deflects 4 and 13 [likely] |
| 5 | Ground Scaravich (5) | **Ground Dash** | 5, 14 | `scaravich-1_start` is stage 5; ids 5, 14 break the Nightmare crates (Sheldon agent) [likely] |
| 6 | Rainy Turtloid (6) | **Meteor Rain** | 6, 15 | 6, 15 stun Metal Shark Player (5 and 8 damage); slot 6 fired id 6 in `shark-8_boss-stun`; the user names slot 6 Turtloid's weapon [likely] |
| 7 | Shield Sheldon (7) | **Guard Shell** | 7, 16; shards 53 | stage rule; the shot costs energy only when it blocks (a shield) [likely] |
| 8 | Infinity Mijinion (8) | **Ray Arrow** | 8, 17 | 8, 17 stun Commander Yammark (5 and 8 damage); Yammark's main weakness is Ray Arrow, 5 and 8 units (Fandom) [likely] |

Weapon names: the official X6 names (Mega Man Knowledge Base, Fandom). The stage numbers 2, 3
and 8 (Wolfang, Heatnix, Mijinion) agree with Zero's skill bits: bit n is the technique of the
stage n + 1 Maverick, and the boss stun ids pair each X slot with that Maverick's Zero technique
(`docs/players/zero.md`) [likely].

## Buttons (key configuration)

`PlayerInput` (`0x8003CD44`) converts the three pad words (`0x800C456C` held, `+2` held before,
`+4` pressed) with `PadToButtons` (`0x8003D7AC`): internal bit i is set when the pad word has a
bit of `key_cfg[i]` (`0x8006D600`, 16 halfwords). Left with right, or up with down, gives none.
**For X only, bits `0x10` and `0x20` are then exchanged** [certain for the code].

The table is the same in the executable and in every state (no key change was made). With the
PS1 pad word in the usual byte order of games (directions in the high byte):

| internal bit | pad mask | button | use (X) |
|---|---|---|---|
| `0x01`, `0x02`, `0x04`, `0x08` | `0x2000`, `0x8000`, `0x1000`, `0x4000` | right, left, up, down | moves |
| `0x10` | `0x0010` (after the exchange) | **Triangle** | weapon button: the special weapon (the sabre with the buster selected) |
| `0x20` | `0x0080` (after the exchange) | **Square** | buster button |
| `0x40` | `0x0002` | **R2** | Giga attack |
| `0x80` | `0x0040` | Cross | jump |
| `0x100` | `0x0020` | Circle | dash |
| `0x200` | `0x0008` | **R1** | next weapon |
| `0x400` | `0x0004` | **L1** | previous weapon; L1 + R1: back to the buster |

[certain for the table and the bit uses; likely for the button names (the byte order of the pad
word was not followed to the pad driver)]. For Zero there is no exchange: Square = `0x10` (sabre),
Triangle = `0x20` (special). The key configuration screen can write the table [guess: no writer
was searched].

## Rules for all special weapons

**Select.** R1/L1 go to the next/previous owned slot (0 = buster); a change resets the charge,
plays sound (0, 12) and, for slots 1, 6 and 8, starts effect type 27 (below). No select while
locked, riding, during the special attacks (`+0xE0`), while a single-at-a-time shot is live
(`+0x99`), or in steps 45, 46, 47, 76 [certain].

**Fire.** A press of the weapon button fires id = slot. A release fires the charged id when the
charge (`+0x9B`) reached level 2; the uncharged ids 1–8 do not fire on a release (table
`0x80073E40`) [certain]. The charge on the weapon button: level 1 at 25 updates, level 2 at 90
(12, 45 with Quick Charge), **only in forms 1, 3 and 4** and only when the slot has the energy of
the charged shot. With the Ultimate Buster part (`0x80000`) in forms 1, 3, 4 the weapon button
always gives the charged id [certain].

**Energy** (`WeaponCost` `0x8003EB0C`). Each slot has a halfword at `mega+0xA8 + 2 × slot`
(slot 0 is the Giga gauge). The maximum is `mega+0x14A` = 6 × `game+0x61` for X and 6 ×
`game+0x62` for Zero (288 at the start, 324 with all upgrades seen; 6 units = one bar unit)
[certain for 80 save states: every X state agrees with `+0x61`, every Zero state with `+0x62`;
`gate-1_before-boss`: Zero, `+0x61` = 56, `+0x62` = 52, maximum 312; the writer is not read]. The cost of a shot is 6 × `cost_x[id]`
(`0x80073D74`, bar units), × 2/3 in forms 1, 3, 4 (at least 1), × 1/2 with Energy Saver (part
`0x20`, which wins over the form) [certain]. A shot needs energy ≠ 0 and ≥ the cost (`HasEnergy`
`0x8003FA00`). Checked: Meteor Rain in form 1 costs 8 (`shark-8`: 324 − 268 = 7 × 8; `shark-9`:
88 = 11 × 8) [certain].

| id | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| bar units | 2 | 8 | 2 | 4 | 4 | 2 | 2 | 3 | 6 | 8 | 4 | 8 | 8 | 8 | 6 | 8 |
| form 0, 2 | 12 | 48 | 12 | 24 | 24 | 12 | 12 | 18 | 36 | 48 | 24 | 48 | 48 | 48 | 36 | 48 |
| forms 1, 3, 4 | 8 | 32 | 8 | 16 | 16 | 8 | 8 | 12 | 24 | 32 | 16 | 32 | 32 | 32 | 24 | 32 |

Exceptions: id 5 costs nothing when the side in front is blocked (no shot); id 7 costs nothing
when fired and pays per blocked shot; the Yammar Option pays again for each extra option. `+0x144`
= 3 (item, below) makes all of them free [certain].

**Limits and times** (`TryShoot`, tables in `attacks.md`). A shot is refused while `+0x98` (shots
counted) is at the limit; ids marked "single" are refused while `+0x99` ≠ 0 (one at a time). The
parts that a weapon makes add to `+0x98` too (each Magma Blade fireball, each charged Ray Arrow
pillar) [certain].

| id | limit `+0x98` < | single | pose `+0x91` / wait `+0x92` | shooting pose | pose step |
|---|---|---|---|---|---|
| 1 | 3 | yes | 1 / 1 | no | – |
| 2 | 3 | yes | 20 / 6 | yes | – |
| 3 | 3 | yes | 1 / 1 | – | 45, 46, 47 (sabre) |
| 4 | 2 | – | 20 / 6 | yes | – |
| 5 | 3 | yes | 20 / 6 | – | 44 |
| 6 | 2 | – | 20 / 6 | yes | – |
| 7 | 3 | yes | 1 / 1 | no | – |
| 8 | 1 | yes | 20 / 6 | yes | – |
| 10 | 4 | yes | 20 / 6 | no | – |
| 11 | 4 | – | 1 / 1 | – | 39 |
| 12 | 4 | – | 1 / 1 | – | 43 |
| 13 | 3 | – | 1 / 1 | – | 40 |
| 14 | 3 | – | 1 / 1 | – | 75 |
| 15 | 4 | – | 20 / 6 | yes | – |
| 16 | 2 | – | 1 / 1 | – | 42 |
| 17 | 4 | – | 1 / 1 | – | 41 |

**End rules.** Each weapon object ends when X is locked (event, gate, `+0xD3`/`+0xD4`) or when the
selected slot is not its slot any more; most uncharged objects also end when X fires the charged
id of the same slot [certain].

**Hits.** `WeaponHitReaction` writes the shot's `+0x98` from the table `0x800737D0`, but **only
the objects that read `+0x98` stop**. The readers are the lemon (0), Ice Burst (2) and its shards
(52), the Guard Shell shards (53), the Yammar Option options (3 hits), Metal Anchor (counts the
hit), the sabre (50/51: up to 4 hits), the charge shots 9/18/19/20/43/45. All other special-weapon
objects go through enemies and do not stop on a deflect [certain for the code; the drop in
`shark-9` has `+0x98` = −1 and still falls].

**`mega+0xA6`** (the weapon lock): 1 while the charged Ice Burst is live, 2 for the charged Guard
Shell, 3 for the charged Meteor Rain. TryShoot then refuses that slot's ids (2/11, 7/16, 6/15);
value 1 also drops every charge and keeps the glow palette 41 on X. The weapon's end clears it
[certain].

**Effect type 27** (`0x8004AF94`): not a visible effect. It runs one update: it streams one frame
block of the weapon graphics (`layout_base+0x10`, entry by var) into VRAM (x 320, y 32 or y 64),
then deletes itself. The weapons and the forms start it to load their sprites [certain].

## Slot 1: Yammar Option (ids 1, 10)

| | uncharged (1) | charged (10) |
|---|---|---|
| objects | a controller (var 0) and up to 3 options (vars 1–3) | the same, all 3 options at once |
| cost | the shot, then each extra option again (2 and 3) when the energy is there | the charged cost only |
| options | turn around X at 32 px (the radius grows 8 px per update), 120° apart, one turn in 4096 / 34 ≈ 120 updates; box (−15, −8, 30 × 16) | the same |
| fire | each new weapon press: every option fires one shot (type 54), 6.0 px/update forward, box 16 × 16; 6 updates between the shots of one option; missing options come back (paid) | auto fire: one volley every 20 updates for 480 updates; shots are type 10 var 7 at 8.0 px/update |
| life | an option breaks after 3 hits; the controller ends when all options are gone | the same |
| damage (`dmg_tbl_normal`) | 54: 4; options (id 1): 5 | 10: 6 |

The controller also ends when X leaves slot 1, rides, dies or is locked; with 3 options and L1 +
R1 it goes back to the buster. The weapon makes no shooting pose [certain for the code].

## Slot 2: Ice Burst (ids 2, 11)

| | uncharged (2) | charged (11) |
|---|---|---|
| start | at the buster until the first animation flag; sound (1, 20) | pose step 39 (animation 95), effect 27 var 9, `+0xA6` = 1; sound (1, 21) |
| motion | **4.0 px/update** forward, gravity 0.15625 per update | X dashes: in steps 12, 13, 35, 74 every 32 px of X's move, 2 ice parts appear at the old place; each part moves 16 px out (up/down for an x move, left/right for a Mach Dash y move), then out with + 1.0 per update up to **6.0 px/update** |
| parts | 3 shards (id 52) in the first 9 updates of the flight, one every 3 updates, 2.5 px/update in random directions; box 8 × 8; each stops at a hit or a wall | – |
| floor | it stops and becomes a **solid block for 180 updates**: X can stand on it (`SolidObject`); it blinks the last 30 updates | – |
| life | a wall, the ceiling or a hit ends it (explosion; not after a hit) | 360 updates (the time in step 39 does not count) |
| boxes | (−6, −6, 12 × 12), terrain 16 × 16 | per part |
| damage | 2: 4; 52: 3 | 11: 6 |

## Slot 3: Magma Blade (ids 3, 12)

| | uncharged (3) | charged (12) |
|---|---|---|
| start | the sabre pose (steps 45 floor, 46 air, 47 wall); the slash object (id 50, or 51 in the air) has the fire palette and sound (1, 23) | pose step 43 (animation 95) until the weapon ends or the slot changes |
| fire | at X's animation flag `0x02`: a fireball 32 px in front of X, then a second one 8 updates later | at X's flag `0x80` frame: one fireball at X; then every 20 updates one from the screen edge behind X (32 px outside), at X's height ± 32 (random); **8 in all** (about 140 updates) |
| motion | **5.0 px/update** forward | 3, 4 or 5 px/update (random), straight or 11.25° up or down (random); through walls |
| life | a wall (burst animation) or 48 px off screen; the controller ends when X leaves the pose or is hit | 64 px off screen |
| boxes | fireball (−16, −16, 32 × 32) | fireball (−24, −24, 48 × 48) |
| hits | goes through | goes through |
| damage | 3: 6; the slash 50: 4 | 12: 6 |

## Slot 4: Metal Anchor (ids 4, 13)

| | uncharged (4) | charged (13) |
|---|---|---|
| start | at the buster; sound (1, 25); effect 27 var 2 | pose step 40 (animation 97, sound (1, 26)); effect 27 var 3 |
| motion | **1.5 px/update** forward, **6.0 up**, gravity 0.25: an arc. Floor: up at 6.0 again; ceiling: down at 3.0; wall: the x speed turns | **8 anchors, one every 8 updates** (64 updates); each starts at a random place on the top edge (or at the side edge behind X) and moves at **6.0 px/update** (4.24 forward, 4.24 down); through walls |
| life | 8 counts: each bounce and each hit is 1; at 8, or at a corner, an explosion | 32 px outside the view |
| boxes | (−6, −6, 12 × 12), then (−10, −10, 20 × 20) after the first frame | (−36, −56, 52 × 88) |
| hits | goes through (the hit counts) | goes through |
| damage | 4: 6 | 13: 6 |

At most 2 uncharged anchors on screen. Zero's version (the same object): 3.0 up, gravity 0.125,
a bounce counts 2 and a hit 9 [certain for the code].

## Slot 5: Ground Dash (ids 5, 14)

| | uncharged (5) | charged (14) |
|---|---|---|
| start | pose step 44 (animation 96). With the front blocked: no shot and no cost | pose step 75: **all enemy classes stop** (`game+0x12`…`+0x19`) and the input locks |
| object | a large object at the buster (own streamed frames); sound (1, 32) | the same; sound (1, 33) |
| launch | when X reaches sub-step 2 (X's flag `0x80` frame): sound (1, 34); **1.0 forward, 1.0 up**, gravity 0.03125; with up held 0.5 forward, 1.5 up | 60 updates after the start of step 75 the world moves again and the object goes: sound (1, 35); **8.0 forward, 4.0 up**, gravity 0.25; through walls |
| life | terrain (explosion) or 24 px off screen | 24 px off screen |
| box | (−24, −24, 48 × 48) | (−32, −32, 64 × 64) |
| hits | goes through | goes through |
| damage | 5: 6 | 14: 8 |

Ids 5 and 14 are the only X ids that break the brown and red Nightmare crates (Sheldon agent).

## Slot 6: Meteor Rain (ids 6, 15)

| | uncharged (6) | charged (15) |
|---|---|---|
| start | a launcher at the buster (var 0); sound (1, 27) | the same; `+0xA6` = 3 |
| motion | at the launcher's flag `0x40`: a water ball (var 1) **rises at 4.0 px/update** to the ceiling or to 16 px below the top of the view; there it makes a drop (var 2) 17 px lower that **falls at 4.0 px/update** to the floor (a splash) | the ball rises at 4.0 to 16 px above the view; 30 updates later **10 meteors, one every 20 updates** (about 210 updates); each starts on the top edge at a random x (or outside the side edge behind X) and moves at **7.5 px/update** diagonally forward and down (5.30, −5.30); sound (1, 28) each; through walls |
| life | the splash | 32 px outside the view |
| box | (−14, −14, 28 × 28) | the same |
| hits | goes through, also after a deflect [certain: `shark-9`] | goes through |
| damage | 6: 5 | 15: 6 |

At most 2 uncharged shots on screen.

## Slot 7: Guard Shell (ids 7, 16)

| | uncharged (7) | charged (16) |
|---|---|---|
| start | no shooting pose, no cost; sound (1, 29); effect 27 var 4 | pose step 42 (animation 95), `+0xA6` = 2; sound (1, 30); effect 27 var 11 |
| object | one shell part that stays with X: 32 px above (up held), 32 px below (down held) or 32 px in front; box 48 × 24 (above, below) or 24 × 48 (front) | 4 parts move in 20 updates from X to the screen points (52, 31), (268, 31), (268, 209), (52, 209) |
| block | when an enemy shot touches it (part `+0x9C` bit 1): 20 updates later X pays the cost and the part shoots a shard (id 53) back at **3.0 px/update** up, down or forward; sound (1, 31) | each weapon press (50 updates apart, **8 in all**): each corner part sends a part (vars 5–8) at **4.0 px/update** toward the opposite corner of the screen |
| life | a new weapon press, not enough energy, a slot change, a ride, death, or the charged shot | X leaves step 42, or after the 8th volley |
| damage | 7: none on most tables (class 2), 4 on the Nightmare shadow; 53: 6 | 16: 8 |

## Slot 8: Ray Arrow (ids 8, 17)

| | uncharged (8) | charged (17) |
|---|---|---|
| start | at the buster; sound (1, 36); X stays in the shooting pose while the arrow is at the buster (`+0x91` = 5) | pose step 41 (animation 95; X has no terrain box, `no_hit`, the lock); at the end of the animation: sound (1, 37) |
| motion | at its flag `0x80` frame: **6.75 px/update** forward; at a wall in front: a flash, then it goes **along the wall at 6.75 px/update up** (down with down held); at the ceiling or floor: a flash and the end | **10 pillars** in 5 columns at X − 64, − 32, 0, + 32, + 64 px, 128 and 288 px below the bottom of the view; they **rise at 16.0 px/update** through everything and end above the view |
| box | (−18, −6, 36 × 12), along a wall (−6, −18, 12 × 36) | the front pillar of each column: (−16, −128, 32 × 255); the second one has no box |
| hits | goes through | goes through |
| damage | 8: 4 | 17: 8 |

Only one uncharged arrow at a time.

## The other ids that X makes

| id | what | made by | behaviour [certain for the code] | damage normal |
|---|---|---|---|---|
| 20 | Ultimate full charge (form 4) | the muzzle flash | 11.0 (13.0 Speed Shot). **A hit does not stop it**: it makes a part (21) at the hit, up to 3 live parts in form 4 (1 else), and goes on. A deflect, a wall or 42/34 px off screen end it | 16 |
| 21 | the parts of 20 | type 20 | a short burst at the hit place, no motion | 1 |
| 22 | the Ultimate Giga (step 34) | step 34 | the hit part on X while X shows animation 106; box (−52, −34, 124 × 66); hit tag + 1 every 4 updates (a hit every 4 updates) | 32 |
| 23 | the Falcon dash (step 35) | step 35 | the hit part on X while X is in step 35; box 50 × 50; hit tag + 1 every 4 updates | 1 |
| 32 | the Mach Dash (step 74) | step 74 | the hit part on X while X is in step 74 | 3 |
| 43 | Blade full charge (form 3) | the shot try | 10.0 (12.0 Speed Shot); box (−16, −6, 16 × 12); hit tag + 1 every 4 updates; a hit does not stop it; a deflect or a wall ends it | 5 |
| 45 | Shadow buster (form 2), buster button | the shot try | 8.0 (12.0 Speed Shot), straight or 11.25° up or down (5 : 6 : 5 at random); box 12 × 12; ends at the first hit or wall. In the super jump (step 31): 3 at once, down and ± 22.5° | 4 |
| 46 | Shadow charged buster / Ultimate Buster in form 2 | pose step 38 | a slash 32 px (64 px) in front of X, box from the frame; hit tag + 1 every 8 updates | 15 |
| 47 | the Shadow Giga (step 37) | step 37 | 2 parts that turn around X at 48 px; hit tag + 1 every 8 updates | 8 |
| 49 | the Falcon Giga (step 36) | step 36 | every 3–5 updates for 120 updates: a part from 32 px above the view down at 12.0 px/update, or from below up at 12.0; x on a 16-px grid of 20 columns at random; box (−8, −24, 16 × 48); hit tag + 1 every 4 updates | 4 |
| 50, 51 | the sabre slash (floor, air/wall) | the sabre poses 45–47, 37, 38, 76, 77 | follows X; box from the frame flag; up to 4 hits, no box for 4 updates after a hit. **With the buster selected the weapon button gives id 50: X's sabre in every form** [likely: the slash object and the sabre pose] | 4, 4 |
| 52 | Ice Burst shard | type 2 | above | 3 |
| 53 | Guard Shell shard | type 7 | above (Zero's Guard Shell uses it too) | 6 |
| 54 | Yammar Option shot | the options | above | 4 |
| 56 | Blade up-slash (form 3, Blade full charge with up held) | pose step 76 | own streamed frames | 3 |
| 57 | the Blade Giga wave (step 77) | the slash at its flag `0x40` frame | 6.0 px/update forward | 8 |

**Ids 24–31, 33–42, 44, 48, 58 are Zero's** [certain for the code]: in X's RAM their
`weapon_types` entries (`0x801ED2A8`, `0x801ED928`, `0x801EDAF4`, `0x801EDFC8`, `0x801EDCC8`) are
in the middle of X functions (`0x801ED2A8` is inside the type 20 code); in Zero's RAM
(`turtloid-6_zero-player`) they are function starts. X's spawn table gives `ShotNone` for them.
So the third stun id of each boss is a Zero technique (`docs/players/zero.md`): 33 Hyoroga
(Turtloid), 35 Rakukojin (Sheldon; it also pops the buried Metal Shark Player), 42 Ensuizan
(Shark), 44 Rekkoha (Yammark, Zero's step 59); also 34 Shoenzan, 36 Sentsuizan, 48 the Hyoroga
attack. Ids 35 and 42 run Zero's sabre code `0x801ED2A8` (the same code as the sabre combo ids
24–31).

## Giga attacks (steps 34, 36, 37, 77)

`TryGigaAttack` (`0x8003E344`) runs in the pose code of the stand, crouch, hang, ladder and air
steps, before the shot try. Conditions: X, no shot wait, **the gauge `+0xA8` full (=
`+0x14A`)**, R2 (`0x40`) pressed, form 1–4; form 4 also needs `+0x86` = 0 and the side in front
free. Then: the weapon goes back to the buster (slot 0, charge reset), `no_hit` = 1, the player
lock, invulnerability timer 0, and the gauge goes to 0 (not in form 4) [certain].

**The gauge** fills only when X is hurt: each hurt adds `+0x14A` / 8, so **8 hurts fill it** (36
at the start, 288 / 8). With part `0x800` each hurt also gives 12 to the lowest gauge of slots 0–8
[certain for the code; the part name is open].

| step | form | what [certain for the code; details in `steps-extra.md`] | weapon ids |
|---|---|---|---|
| 36 | 1 Falcon | animation 174 / 177; the screen fades (51 updates); for 120 updates a part every 3–5 updates from the top and the bottom of the view at 12.0 px/update | 49 |
| 37 | 2 Shadow | animation 185 / 186; no terrain box; two parts turn around X at 48 px; for 148 updates a slash (50 / 51) at each flag frame; sound (1, 18) | 47, 50, 51 |
| 77 | 3 Blade | animation 185 / 186; one slash; at its flag `0x40` frame a wave at 6.0 px/update forward; sound (1, 18) | 50, 51, 57 |
| 34 | 4 Ultimate | animation 106; X rises at (± 2.5, 3.5) with the fall gravity, then moves 8.0 px/update forward for 24 updates (192 px) with the hit part on X (124 × 66, a hit every 4 updates) | 22 |

## The timed item effect (`mega+0x144`)

The pause-menu item code (`0x80034288`) writes the item kind (`+0x2D` of the item) into
`mega+0x144` and starts the timer `+0x148`: **300 updates (480 for kind 6)**; `PlayerUpdate`
clears `+0x144` at 0. Kinds 4 and 5 are refills (life; weapon energy of slots 0–8) and do not use
it [certain for the code]. The effects [certain for the code; which items they are is open]:

| `+0x144` | effect | where |
|---|---|---|
| 1 | no contact damage from enemy shots (the visual pool) | `ContactMega` `0x80030F30` |
| 2 | + 1 damage for every weapon hit | `ApplyWeaponHit` `0x800316F4` |
| 3 | special weapons cost nothing (not ids 44, 47, 49) | `SpendEnergy` `0x8003F740` |
| 6 | in form 2 the buster button gives the plain buster ids (lemon, 9, 18) | `ShotIds` `0x8003DF74`, `BusterChargedId` |

## What the save states showed

- `shark-8_boss-stun`: slot 6, `+0x94` = 6, last shot 6, energy[6] 268 of 324, no weapon object.
- `shark-9_boss-revive`: energy[6] 236 (11 shots of 8). **A live Meteor Rain drop** (type 6 var
  2, weapon slot 2): state 1, step 3, `speed_y` −4.0, animation 3, box 28 × 28, its parent (the
  ball) already deleted, `+0x98` = −1 (a deflected hit) and it still falls. This matches the code
  [certain].
- `+0xC9` = `0x21` (slots 1, 6) in the stage 4 and 7 states, `0x01` in `turtloid-1`, 0 in
  `intro-1_start` and `yammark-1_start`. `+0xA6` and `+0x144` are 0 in all states.
- The key table `0x8006D600` is the same in all states.

## Play checks

1. Buttons: Triangle fires the special weapon, Square the buster, R2 the Giga attack, R1/L1 the
   next/previous weapon, L1 + R1 the buster (default keys).
2. Ice Burst: the block falls in a short arc and stays on the floor for 3 seconds; X can stand on
   it; 3 small shards fly out at the start. Count the energy: it is the most costly uncharged
   weapon (32 in the Falcon Armor, 48 with no armour).
3. Charged Ice Burst: dash after the shot; ice parts appear every 32 px behind X and shoot out to
   the sides; no slot-2 shot is possible for 6 seconds.
4. Magma Blade: a fire slash and two fireballs 8 updates apart at 5 px/update that go through
   enemies. Charged: 8 fireballs across the screen from behind X.
5. Metal Anchor: an arc that bounces 8 times (or fewer when it hits enemies). Charged: 8 anchors
   fall diagonally across the screen.
6. Ground Dash with a wall in front: nothing happens and no energy is used. Charged: everything
   stops for 1 second, then the large object flies in a high arc.
7. Meteor Rain: a ball goes up to the ceiling (or near the top of the screen) and a drop falls
   down at the same x; it goes through enemies. Charged: 10 meteors, about one every 1/3 s.
8. Guard Shell: the shell sits above X with up held, below with down held, else in front; it
   uses energy only when it blocks a shot, and sends a shard back. Press the weapon button again:
   the shell goes away.
9. Charged Guard Shell: 4 parts go to the screen corners; each weapon press sends 4 parts across
   the screen; 8 presses in all.
10. Ray Arrow: the arrow hits a wall and goes up the wall (down with down held). Charged: 5
    columns of light rise from below the screen.
11. Yammar Option: 3 options turn around X; each weapon press makes them shoot; each option
    breaks after 3 hits; a press refills the options (it uses energy).
12. Giga attack: get hurt 8 times, then press R2. Each form's attack as in the table.
13. Ultimate full charge (cheat): it goes on through enemies and leaves small bursts.
14. The buster selected, the weapon button: a sabre slash in every armour.

## Open items

- The look of every special weapon (no state has one on screen except the Meteor Rain drop).
- The pause-menu item kinds 1, 2, 3 and 6 (`+0x144`): their names in the game.
- (answered: `ContactWeapon` sets bit 1 when a shot with `+0x9C` = 1 kills an object with
  `dmg_tbl_minor` or `dmg_tbl_one_hit`; `docs/damage.md`) Which code sets the Guard Shell part's
  `+0x9C` bit 1 (the block).
- Part bit `0x800` (the gauge bonus on a hurt): its name.
- Type 43's path with `act4` = 1 (it makes a type 21 and ends).
- The form 4 Giga: when the gauge goes to 0.
- Zero's ids 24–44, 48, 58 (the Zero agent).
