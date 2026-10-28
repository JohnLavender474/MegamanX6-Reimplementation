# Notes: stage 07 laser puzzle

Sheet: [`~/IdeaProjects/mmx-demake-tools/docs/enemies/laser-reflector-puzzle.md`](../../findings/enemies/laser-reflector-puzzle.md).

## Method

1. `probes/probe_objs.py sheldon-7_lasers.state.gz`: live objects (weapon pool added to the probe).
2. Kit: `python3 -m mmxdemake.rekit --name sheldon-lasers --state sheldon-7_lasers.state.gz --type 13
   --out ~/mmx-data/x6/re/sheldon-lasers/kit` (46 functions; neighbours visual 11, misc 7, misc 14).
3. `--at` for `0x80044554` (weapon type 79 entry; state table `0x8007614C`: `0x800444B0`, `0x800444EC`,
   `0x80044534`), `0x8002C32C` (GetWeaponObject), `0x80030234` (tile collision value),
   `0x8002C5F0` (GetMiscObject2-like walk of the 96 misc slots), `0x80031474` (ContactWeapon),
   `0x800315CC` (ApplyWeaponHit: weapon id = weapon object byte 1; class 2 returns 0; 126 returns 0 in
   ContactWeapon).
4. `probes/tables.py`: boxes, reflection table, turn lists, door rectangles, tile copies, live fields.
5. `probes/dmgtables.py`: the two reflector damage tables from the executable.
6. `probes/roles.py` on `probes/records-sheldon-7.txt` (from `nightmare-sheldon/probe_recs.py`): records
   by role.
7. Sprite export: `python3 -m mmxdemake.spriteexport --state sheldon-7_lasers.state.gz` -> type 13 is
   `enemy-07d032a495` (34 animations, live table `0x800FBA58`). Laser sprites (210-245) are in another
   sprite source (sprite_bank_base word `+0x40`) and are not exported.
8. Names: Fandom list returned HTTP 402, megamanwiki 403; no names in the reference projects. Descriptive
   names used.

## Checks against the save state (all agree)

- Beamer slot 0 (`0x15`, at 3080,304): `work.88` = 6 (start 5, one turn toward the player at 3034,427:
  AimAt -> 32-way about 22 -> 8-way 6). Step 4 sub 2, timer 214 -> 88 updates since the shot.
- Its laser (visual 0): age 182 - 94 = 88; start (3080, 320) = beamer + (0, 16); head caught at
  (3080, 394) = reflector (3080, 408) + (0, -14) = table entry for r 15, in 6.
- Reflector (`0x3F`): `+0x86` = 6, `+0x89` = 1, `work.84` = 15 = id of its laser (visual 1). Visual 1:
  dir 1, start (3080, 394), head at (3144, 330) = door (3152, 360) + (-8, -30). Age 76.
- Door (`0x50`): step 4 (open), sprite 41; camera x 2874 < 3168 so not passed yet.
- Beamers slots 3, 4 (`0x16` at x 3360, 3600): spawned by door 0's SpawnInRect (3168-3791 x 256-447), off
  screen. Timer 237 -> 65 updates; their lasers: age 182 - 117 = 65, start y 304, head y 448 (a wall):
  144 px = 9 moves = 27 updates, then stopped.
- Weapon slots 0-3: type 79, box `0x80076148` (-8,-8,16,16), at the heads, `+0x8C` = laser ids 14-17.
- Shot counter `0x800FCFEC` = 18.

## Key facts and where they come from

- Head move: `0x800EB3E0`..`0x800EB43C` (wall test, `+0x88` = 0, `+0x98` != 2); `+0x88` mod 3 and `+0x96`
  mod 5 at `0x800EB6B8`.
- Life 182: `0x800EAF58`; parent `+0x89` = 2 at `0x800EB148`.
- End flicker: `0x800EB734` (13 + 1 updates, odd frames).
- Catch: `0x800EAA00`. Reflection: `0x800FBCC0` / `0x800FBD40`; rule out = (r - in) mod 8 checked on all
  128 entries; out 255 for the 8 head-on cases (entries 48-55).
- Turn: `0x800EA534`..`0x800EA5E8`; wait table 33 updates at `0x800EA618`.
- Aim: `0x800ECC74`, `0x800ED0AC` (|dx| < 145; table `0x800E987C`).
- Door: `0x800ECA5C`, `0x800EA650`, `0x800EA7C8`, `0x800ED1CC`; spawn in set-up `0x800EA358`..
  (jump table `0x800E9864`, door 3 = no spawn).

## Open

See the sheet. Also: misc type 14 (`0x800ED324`) is a one-sprite scenery object (type 136 bank, priority
19, deleted at checkpoint >= 2); it only shares the code region.
