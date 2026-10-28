# Ride Armor: pseudo code

Units: px per update (16.16); a positive `speed_y` is up. `held` = `+0x88`, `pressed` = `+0x8A`.
`attack` = `pressed & 0x30` (buster or weapon button). `leave` = (`held & 7` == up only and jump
pressed, sound (2, 37)) or `mega.hp` == −128.

```
Actor2Update():                                   # 0x80020A08, before PlayerUpdate
    if game.stop[player] == 0:  if actor2.flags: actor2_types[actor2.type](&actor2)
    elif actor2.flags & 8:      actor2_types[actor2.type](&actor2)
    elif actor2.display:        Display(&actor2)

RideEntry(r):                                     # 0x800F2414
    if mega.event == 0: r.held = mega.held; r.pressed = mega.pressed
    else:               r.held = r.pressed = 0
    if mega.stopped (+0xCC): Display(r); return
    r.prev = r.pos
    r.coll = r.collide_flags | r.push; r.push = 0
    [RideInit, RideMain, RideRemove][r.state](r)
    r.hp &= 0x7F
    game.stage_flag = (r.coll & 0x0C) == 0x0C

RideMain(r):                                      # 0x800F1EC4
    r.display = 0
    if OffScreenCheck(r): return                  # 64 px margin: frozen, not drawn
    if mega.ride and (r.coll & 1) and r.x - cam.x < 32:      # crush at the left screen edge
        mega.hp = r.hp = 0x80; 8 x SpawnExplosionRand(32, 32); DeleteObject3(r)
        game.stage_flag = 0; return
    if r.hp < 0:                                  # a hit is waiting (ContactMega set bit 7)
        if r.hp == 0x80:
            if not r.f97 & 0x80: r.f97 |= 0x80; r.f92 = r.f91 = 30; r.blink = 0
            r.hp = 32                             # refilled; the countdown runs
        r.kb_speed = 4.0; r.blink = 121; effect 12 var 8
        r.kb = 1; r.lock = 1; r.f97 |= 2          # knockback
    if r.blink: r.blink -= 1 (at 0 with f97 & 1: mega.display = 1)
    if (r.f97 & 0x41) == 0x41 or (game.exit_req and r.f97 & 0x40):
        r.f97 ^= 1; step 11
    DoubleTapDash(r); DashButton(r); Charge(r)
    if r.destroy == 0:
        if r.var == 0: steps_a[r.step](r)        # 0x800F3A74, 14 entries
        else: r.cool -= 1 if r.cool; steps_b[r.step](r)   # 0x800F3AAC, not reached
        Knockback(r); CollideCheck(r)
        if r.f97 & 0x40:                          # X inside: X follows the ride
            mega.x = r.x; mega.y = r.y; mega.flip = r.flip
            if r.blink: clut flash on when (blink & 3) == 0
        if r.f97 & 0x80:                          # 0 HP countdown, 494 updates
            r.f92 -= 1; if 0: flash 1 update; r.f92 = r.f91; r.f91 -= 1; if 0: r.destroy = 1
        Display(r)
    if r.destroy == 1:                            # eject X
        r.destroy = 2; r.blink = 255; mega.ride = 0; r.f97 ^= 0x40
        PlayerSetTerrainBox(mega); r.f90 = 30; r.f97 |= 4; SpawnJunk(7, anims 24..30, r)
    elif r.destroy == 2:
        r.f90 -= 1; if 0: state 2, SpawnExplosion, StopSound(2, 8)
        else SpawnExplosionRand(31, 31)
    elif r.blink == 0: r.lock = 0; flash off
    Bounds(r)

Bounds(r):                                        # 0x800F1DA8, only with X inside
    x <= cam.max_x + 288;  x >= cam.min_x + 32 unless blocked right
    if y - 32 >= cam.max_y + 240: state 2 (removed); if X inside: mega.hp = 0x80

Knockback(r):                                     # 0x800F1C80
    phase 1: save speeds; speed_x = -4.0 facing right (+4.0 facing left); accel_x = gravity = 0.34375
             speed_y = 0; phase 2
    phase 2: MoveObject2; when |speed_x| < 0.28125: restore speeds, f97 ^= 2, phase 0
             on the floor: speed_y = 0
    # 11 updates, about 25 px back

DoubleTapDash(r):  first tap of left or right arms a 12-update window; the same direction again in
                   steps 1, 2 or 5 -> step 7 (on the ground), f97 ^= 8
DashButton(r):     dash pressed in steps 1, 2 or 5 -> step 7 (on the ground), f97 |= 8
Charge(r):         (not in knockback)
    phase 0: buster or weapon held -> phase 1, timer 60, remember the buttons
    phase 1: still held: timer -1; at 0 phase 2.  released early: phase 0
    phase 2: released -> phase 0 (StopSound(2, 8))
```

## Steps (var 0)

```
0  empty idle   anim 17; not on floor -> 12. Board when: no event, no exit, Zero: +0x8E = 0,
                mega+0x7A = 0, mega+0xE0 = 0, r+0x96 & 0x8200 = 0, X's terrain box overlaps the
                boarding box (centre (0, 22), half 15 x 10), X HP > 0, mega+0xA4 = 0, mega+0x79 = 0
                -> step 10, anim 18, f97 |= 0x40, mega.ride = 1 (2 if this were not actor2)
10 board        anim 18 (X 57), sound (2, 36); flag frame: effect 12 var 7, sound (2, 38)
                end -> step 1, hurt box (-8, -14, 16, 46), +0x61 = 0
1  stand        anim 0 (X 41), speed 1.375. charge ready -> 13; leave -> 11; no floor -> 12
                knockback: tick only. left/right -> 2; jump -> 4; attack -> 3
2  walk         anim 1 (X 42); as 1; x += +-1.375; footstep sound (2, 42) on the flag frames
3  punch        +0x87 = 1: type 62 var 2, anim 10 (X 50), sound (2, 43)
                else: type 61 var 0, anim 8 (X 49), sound (2, 44)
                frame flag 1 + attack + +0x87 < 2: queue (+0x87 += 1)
                end: queued -> step 3 again, else step 1
4  jump         speed_y 5.5, gravity 0.2578, dust effect 12 var 6, anim 2 (X 43), sound (2, 39)
                attack -> 6; leave -> 11; left/right: speed_x = +-(+0x98 / 256)
                ceiling, speed_y < 0 or jump released -> 12; else MoveObject2
5  land         anim 4 (X 45), sound (2, 40); leave -> 11; end -> 1; no floor -> 12;
                attack -> 3; left/right: x += +-1.375
6  air punch    type 62 var 3, anim 11 (X 51); floor -> 5; left/right: +-(+0x98/256);
                rising and (ceiling or jump released): speed_y = 0; end -> 4 (rising) or 12
                speed_y >= -5.875; MoveObject2
7  dash         speed_x +-4.125, +0x98 = 1056, timer 30, anim 5 (X 47), sound (2, 41),
                effect 12 var 5. leave -> 11; jump -> 4; attack -> 9; no floor -> 12;
                wall -> 8; forward not held (and, with f97 & 8, dash not held) -> 8
                x += speed_x; timer -1; below 0 -> 8   (32 updates, about 132 px)
8  dash end     anim 7 (X 48), +0x98 = 352; leave -> 11; end -> 1; jump -> 4;
                attack -> 3; left/right held -> 3 (sic)
9  dash punch   type 62 var 1, anim 9 (X 55), sound (2, 44); no floor -> 12; the dash goes on
                until the dash timer is below 0, then 8
11 leave        +0x61 = 1, terrain box 0x800F3A44; with X inside: mega.ride = 0, f97 ^= 0x40,
                PlayerSetTerrainBox(mega); no floor -> 12 else anim 19; end -> step 0
12 fall         speed 0, gravity 0.2578, latch 1, anim 3 (X 44). X inside: leave -> 11
                floor: X inside -> 5, empty -> 11. attack -> 6; left/right +-(+0x98/256)
                speed_y >= -5.875; MoveObject2
13 charge       sub 0: anim 12 (X 52); sub 1: wait until the buttons are released, face by
                left/right; then sub 2: speed_x +-4.125, timer 30, type 62 var 4, anim 15 (X 54),
                sound (2, 45); x += speed_x; stop at 0 or when the opposite direction is held:
                sub 3: anim 16 (X 41), end -> step 1
```
