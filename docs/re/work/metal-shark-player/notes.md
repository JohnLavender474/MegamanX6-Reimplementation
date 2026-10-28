# Metal Shark Player: notes

## Method

- Overlay check: `scripts/probe_overlay.py` (overlay 07 at `0x800E9860`, 65 564 bytes, to
  `0x800F987C`; 156 bytes differ in `shark-7_boss`, 153 in `shark-6_before-boss`; nothing matches at
  `0x800FA000`, so no boss overlay).
- Kit: `python3 -m mmxdemake.rekit --state shark-7_boss.state.gz --type 56 --name
  metal-shark-player` → `kit/` (146 functions, 23 state tables, 1 neighbour: visual 22).
- `scripts/code-compact.txt`: `kit/code.txt` without the hex words and nops (for reading).
- Read with `--at`: `0x800F765C` (the kit names it `SheldonTurret`, a stage 7 overlay name; here it
  is the spark start, visual 22 `var` 64) and `0x800179D0` (main program: `SetAnimeFrame(obj, anim,
  frame)`).
- `scripts/live.py <state>`: game, camera, player and live objects. `scripts/tables.py`: all 40
  animations with every frame up to the next animation, and the box-by-sprite table
  `0x800E9B1C`. `scripts/mega.py`: the player's weapon fields.
- Sprites: `python3 -m mmxdemake.spriteexport --state` for `shark-7_boss`, `shark-8_boss-stun`,
  `shark-9_boss-revive`. `img/bank14-part*.png` (crops of the bank sheet) and `img/sidekicks.png`.

## Key facts and where they come from

- HP: `0x800F45A0` stores `48 + 0x800E9A6C[game+0x8F]` to `game+0x8C` (`addiu v0,v0,48`).
- Damage table `0x8007555C` (`0x800F1FCC`, `0x800F231C`, `0x800F2FEC`), read in `kit/data.txt`.
- Stun ids: `0x800F21EC`–`0x800F22C8`: the result (id + 1) is compared with 7, 16, 43 (ids 6, 15,
  42) and 36 (id 35). Step 8 and 7: 120 (`+0x88`); other steps: step 3 with 10 000; step 7 with
  `+0x8D` = 1 and id 35: step 9 with 10 000.
- THINK `0x800F33F8`; tables `0x800F8F7C` (high), `0x800F8FFC` (low).
- Side kick pick `0x800F5AA8`: `game.difficulty` ≥ 2 and `GetRNG() & 1` → `var` 3; else `mega+0x02`
  = 0 → `var` 2, else `var` 1.
- Speeds: `rcos(a) << 7` = 8 px/update; `× 96` (`(x<<4 + x<<5) << 1`) = 6; `<< 6` = 4; `× 48` = 3;
  homing `0x800F3F94`: `× 8, 16, 24, 32` = 0.5, 1, 1.5, 2 px/update.
- Jumps: `gravity = (H << 16) / (t² / 2)`, `speed_y = gravity × t` (H 70 FIN, 60–79 ANCHOR; t
  10–24).
- Weapon ids of a slot: `0x8003DF58` (player): `+0x94` = `+0x93`; + 9 when `+0x9B` = 2 or (part
  `0x80000` and form 1, 3, 4). Weapon type entries: id 35 and id 42 both `0x801ED2A8`.

## Checked against the states

| state | check |
|---|---|
| `shark-7_boss` | boss state 1, step 5, sub 3, anim 8 (sprite 13), HP 48 = `boss_hp_max`, level 0, contact 4, table `0x8007555C`, boxes `0x800F90B0`/`0x800F90B4` (sprite 13), terrain `0x800F907C`, gravity 0.355 (a 71 px jump with t = 20), `+0x8A` 0, `+0x8B` 3 (high pattern 3: ANCHOR), `+0x8C` 1; fight timer 63. Anchor: visual 22 `var` 0, step 1 sub 1, speed (−3.37, −2.16) = 4.00 px/update, 21 px from its start (about 6 updates), boxes `0x800F9780`/`0x800F977C`, terrain `0x800F9784`, deflect, HP 1, contact 4, priority 4. Matches the throw at flag 1 of anim 8. |
| `shark-8_boss-stun` | step 3 sub 1, anim 37, texture `0x540` (set 1), flash bit set, `+0x88` = 9953 (47 updates after the hit), `+0x7C` = 45 (45 of 90 left), speeds and gravity 0, table deflect, HP 23. Player: `+0x93` 6, `+0x94` 6, `+0x97` 6 (last shot id 6), slot 6 energy `0x10C` (others `0x144`). No weapon object live. |
| `shark-9_boss-revive` | boss step 8 sub 4, `act4` 1, at room (302, 81) (target `+0x84` 6368 = room 304, `+0x86` 304 = room 80), HP 8, `+0x8A` 1 (low), pattern 3 (ANCHOR, FIN, RESURRECTION), `+0x8C` 3. Side kick main 56 `var` 2 (player X): state 3, step 3, sub 3 (waiting for anim 34), at room (80, 16), facing right, texture `0x540`, palette `0x794C`, `+0x7E` 2 (level 0), HP 24, `+0x80` = the boss. Piece counter 0, no visual object: all pieces are in. |

## The side kicks: identification

From the sprites only (bank 14 of `shark-7_boss`; silhouettes in `bank-14-live-1.png`):

- `var` 1: an upright body with a crest and a long curled tail with fins; its attack is a long
  straight diagonal line with a ring at the end (a tongue or a wire). Candidates: Sting Chameleon
  (X1, tongue), Toxic Seahorse (X3, the seahorse shape). Not confirmed.
- `var` 2: a heavy body with two big arms up, at the ceiling; it fires blue four-point sparks.
  Candidate: Spark Mandrill (X1, hangs from the ceiling, electric). Not confirmed.
- `var` 3: a hornet with wings, a drill stinger, and small bee shots that home in and explode:
  Blast Hornet (X3) and its Parasitic Bomb [likely].

## Open questions

See the sheet. In addition: the boss y in the stun state (375) is 16 px under the standing origin
on X's floor (359); the floor shape at that x was not read.
