# Nightmare Bug (stage 7, overlay 10, main type 22): notes

Kit: `python3 -m mmxdemake.rekit --name nightmare-bug` (state `sheldon-6_tile-shown`).
Probe scripts: [`~/mmx-data/x6/re/nightmare-sheldon/`](../nightmare-sheldon/) (`probe_objs.py`, `probe_recs.py`, `arr.py`,
`scan_ne.py`, `scan_imm.py`, `dumpat.py`).

## Code map (overlay 10, loaded at 0x800E9860)

| address | what |
|---|---|
| 0x800F42AC | entry: save prev x/y, call state[0x800FC718] |
| 0x800F3F54 | state 0, set-up |
| 0x800F42F8 | state 1, main: step[0x800FC728], weapon, contact, screen |
| 0x800F43F4 | state 2: DeleteObject3 |
| 0x800F4414 | state 3: blink display for 60 updates (+0x7C counter), then DeleteObject3 |
| 0x800F40FC | step 1 FOLLOW |
| 0x800F4468 | step 2 LEAVE |
| 0x80040290 | step 0 HitReactionDispatch (engine) |

Main table of stage 7: 0x800FB428 (types in the overlay: 5, 13, 22, 27, 28, 30, 32, 51).

## Set-up (0x800F3F54)

- `lb s1, nightmare_effect`; if != 1 -> DeleteObject3 (record keeps the "spawned" bit).
- hp 8 (+0x5C), contact 2 (+0x60), iframes 0, damage table 0x800746FC, anim table 0x800FC714,
  priority 5, layer = mega.layer, speed 0, latch 0, collide 0, flip 0, terrain box 0.
- attack box 0x800FC6F4 = fa fa 0b 0b = (-6,-6,11,11); hurt box 0x800FC6F8 = same.
- +0x80 (s16) = xofs[rand & 7], table 0x800E9DF0 = -96 -80 -64 -48 48 64 80 96.
- +0x82 (s16) = yofs[rand % 3], table 0x800E9E00 = 0 16 32. (Two GetRNG calls.)
- +0x85 = 0 (block counter). SetAnime 0. state = step = 1, step2 = 0.

## FOLLOW (0x800F40FC)

```
flip = (x < mega.x) ? 0x40 : 0
ty = mega.y + yofs ; tx = mega.x + xofs
a = ratan2(ty - y, tx - x)
speed_x = rcos(a) * 80          ; 4096*80 = 0x50000 = 5 px/frame
speed_y = -rsin(a) * 80
if |x - tx| < 16 and |y - ty| < 32:
    speed = 0
    if (frame_count & 1) == 0:
        p = (p + 1) & 63         ; +0x84
        y += trunc(rsin(p*128) * 0x3E88CB3D >> 32 >> 9)   ; -1, 0 or +1
AnimeTick; MoveObject
```

Bob simulated (`python3` with sin): y offset goes 0 -> +11 -> 0 over 32 steps (64 updates), only
down from the stop point (y grows down). 0x3E88CB3D/2^41 = 1/2098: |rsin| >= 2098 gives 1.

## LEAVE (0x800F4468)

speed_x 0, speed_y 0x50000 (5 px/frame up), AnimeTick, MoveObject.

## Main (0x800F42F8)

```
step()
r = ContactWeapon(self)
if r < 0: DropItem(self, 18); SpawnExplosion(self); state = 2; return
if mega+0xD4 == 1: state = 2; return            ; boss gate pass (byte 0x80097174)
if r == 127: if (blocks++ >= 5) step = 2         ; +0x85; 6th deflect -> LEAVE
ContactMega(self)
if OffScreenCheck2(self, 64, 64): DeleteObject2(self)
else DisplayCheck(self, 32, 32)
```

ApplyWeaponHit (0x800315CC): class 0xFF (-1) -> WeaponHitReaction(shot, -1), return 127 (the
shot is deflected). Class 2 -> return 0 (no hit, the shot is not stopped). Class 7, 8 -> 127 too.

## Damage table 0x800746FC (new; not in helpers.json)

- class 0, damage 4: ids 1, 10, 54 only.
- class 2 (no effect): ids 55, 59, 60, 63, 64-79.
- class 0xFF (deflect): every other id (0, 2-9, 11-53, 56-58, 61, 62).
- Same three ids as the only real kills of Nightmare Insect (overlay 03).

## Records, stage 7 area 0 (enemy_set 0x800FAEE0)

11 records of type 22, id 0: [23] (464, 320) kind 0, [24]-[32] x 3856-4632, y 304-640, all kind 0;
[33] at (0, 0). In sheldon-5/6 record [23] has flags 01 and kind 0x10 (spawned); in sheldon-7 its
flags are 00 again (the bug was removed by DeleteObject2: it left or went off screen; not killed,
because a kill uses DeleteObject3 and keeps the bit).

## Observed

| state | bug x, y | player x, y | note |
|---|---|---|---|
| sheldon-5 | 1649, 422 | 1574, 427 | flying (x not within 16 of target) |
| sheldon-6 | 1713, 443 | 1661, 427 | at target: xofs +48 (within 16), yofs +16 |

One bug only, from record [23] 1100+ px behind: it followed the player from the stage start.

## nightmare_effect

- Written once, at the stage start, by 0x8001E914 (0x8001E9D0: `sb v0, 0x43A(s0)`), when
  stage < 19 and area = 0: nightmare_effect = game[0x168 + stage] (0x800CD038 + stage).
  The same function clears time, kill count, damage, copies 16 words game+0xD8 (Reploid status)
  to game+0x118, and more.
- No writer of 0x800CD038.. found in the loaded code (main program, overlay 10). Probably in the
  stage-select or result code.
- Values by state (stages 0..8): yammark-1/2/3 `0 0 0 1 0 0 0 1 0`; turtloid-*, report-1
  `0 6 0 1 0 6 0 1 0`; sheldon-* `0 6 7 1 0 6 7 1 0`.
- Readers in stage 7: 0x800F3FE4 (bug, value 1) and 0x800F2748 (another object, value 5; state
  table 0x800FC6B8, class-table entry 0x800FB7D4; uses sprite bank 147; skips while
  mega+0xE0 = 2).

## Open

1. Who sets state 3 (60-update blink, then delete)? No code in overlay 10 writes it.
2. What writes game+0x168[stage].
3. Which weapons are ids 1, 10, 54.
