# X's special weapons — pseudo code (player RE step 4)

Source: `kit/code.txt` (X kit, `sheldon-1_lemons`) for overlay 0 and the shared main-program
functions; `scripts/dz.py` for the main-program weapon types (1, 4, 7, 10, 13, 16, 53, 54).
`m` = `mega` (`0x800970A0`). Speeds are px per update (16.16); a positive `speed_y` is up. `cam`
= the layer camera (view top left). "flip" ≠ 0 = facing right. The shot try, the charge and the
buster ids are in `../attacks/pseudocode.md`.

## Input (before the player update)

```
PlayerInput (0x8003CD44):
  m.7C = PadToButtons(pad_held)   ; 0x800C456C
  m.7E = PadToButtons(pad_old)    ; 0x800C456E
  m.80 = PadToButtons(pad_press)  ; 0x800C4570

PadToButtons (0x8003D7AC, raw):
  out = 0
  for i in 0..15: if key_cfg[i] (0x8006D600) & raw: out |= 1 << i
  if out & 3 == 3: out &= ~3          ; left + right: none
  if out & 0xC == 0xC: out &= ~0xC    ; up + down: none
  if m.var == 0: exchange bits 0x10 and 0x20   ; X only
  return out

key_cfg (default, same in every state):
  bit 0x001 right <- 0x2000   bit 0x002 left <- 0x8000   bit 0x004 up <- 0x1000
  bit 0x008 down  <- 0x4000   bit 0x010      <- 0x0080   bit 0x020    <- 0x0010
  bit 0x040       <- 0x0002   bit 0x080 jump <- 0x0040   bit 0x100 dash <- 0x0020
  bit 0x200       <- 0x0008   bit 0x400      <- 0x0004
```

With the PS1 pad word read with the two bytes exchanged (directions in the high byte: up `0x1000`,
right `0x2000`, down `0x4000`, left `0x8000`), the low byte is L2 `0x01`, R2 `0x02`, L1 `0x04`, R1
`0x08`, Triangle `0x10`, Circle `0x20`, Cross `0x40`, Square `0x80`. So: Square → `0x10`, Triangle
→ `0x20`, R2 → `0x40`, Cross → jump, Circle → dash, R1 → `0x200`, L1 → `0x400`; for X the exchange
makes **Square the buster (`0x20`) and Triangle the weapon button (`0x10`)**.

## Energy

```
WeaponCost (0x8003EB0C, m, id):
  if m.var (Zero):
      b = cost_z[id] (0x80073DF4)
      part 0x20: return 3b ; Black Zero (game+0x5F & 0x20): return 4b ; else return 6b
  c = 6 * cost_x[id] (0x80073D74); if c == 0: return 0
  if part 0x20: c = c / 2
  elif form in (1, 3, 4): c = 2c / 3
  return max(c, 1)

HasEnergy (0x8003FA00): e = m.energy[m.93]; return e != 0 and e >= WeaponCost(m, m.94)
SpendEnergy (0x8003F740, m, id):
  if m.144 == 3 and id not in (44, 47, 49): return
  m.energy[m.93] -= WeaponCost(m, id)

Gauge on a hurt (PlayerHitCheck 0x80039514, after PlayerHurt):
  X: m.A8 += m.14A / 8, at most m.14A
  Zero: m.energy[5] (+0xB2) += WeaponCost(m, 5) / 2, at most m.14A
  part 0x800: the slot of 0..8 with the lowest energy += 12 (at most m.14A)
```

## Timed item effect (+0x144)

```
PauseItemUse (0x80034288), item kind k = item+0x2D:
  4: life refill path ; 5: weapon refill path (slots 0..8 below max)
  else: PlaySound(0, 34); m.148 = (k == 6) ? 480 : 300; m.144 = k
PlayerUpdate (0x80034F64): if m.148: m.148 -= 1; at 0: m.144 = 0
users: 1 ContactMega skips visual objects (enemy shots); 2 ApplyWeaponHit damage + 1;
       3 SpendEnergy free; 6 form 2 buster ids = plain ids (18 at full charge)
```

## Effect type 27 (weapon graphics loader, 0x8004AF94)

```
block = layout_base+0x10 table entry by var:
  0 +8, 1 +64, 2 +12 (Zero) / +20 (X), 3 +68, 4 +32, 5 +8, 6 +12, 7 +12*, 8 +16*, 9 +60*,
  10 +52, 11 +28, 12 +44, 13 +36           (* = to y 64, tpage 0x540; others y 32, tpage 0x520)
PlayerFrameStream(e, 320, y); DeleteObject3(e)
makers: WeaponChanged (slots 1, 6, 8), Ice Burst 2 init (var 1), Metal Anchor 4 init (var 2),
        13 (var 3), beam-in form 2 (var 5), pose 38 (var 6), Giga 37 (var 7), Giga 36 and type 19
        (var 8), pose 39 (var 9), type 16 (var 11)
```

## Slot 1: Yammar Option (types 1, 10; shots 54 and 10 var 7; main program)

```
Entry 0x800434E0: end (state 2) on lock/gate, ride, HP 0; X: slot != 1; Zero: m.A6 != 1
  X, controller, m.8C == 3, both 0x600: slot 0, ChargeReset, sound 0/12, WeaponChanged
Init 0x80043638, controller (var 0):
  angle +0x94 = 4096 (not turning yet); +0x8C = 3; +0x8D = 1 (option 1)
  for n in 1..3: option var n (FindWeaponObject)
      n >= 2 and type 1: if energy >= cost: SpendEnergy, +0x8D |= bit   ; extra options cost
      type 10: all bits
  option: anime 0x800779C0, at X
Controller 0x80043890 (each update):
  if angle != 4096: angle = (angle + 34) & 0xFFF                       ; 1 turn = 120.5 updates
  +0x93 &= ~1
  if +0x93 & 2 (auto fire): +0x88 += 1; every 20: +0x93 |= 1; at 481: +0x88 = 0, +0x93 &= ~2
  else if weapon press (X 0x10): for each missing option: if energy >= cost: SpendEnergy, set bit
  if +0x8C/+0x8D (halfword) == 3: state 2, +0x8F = 1                    ; all options gone
  if type 10 and bits == 7: type = 1, +0x93 = 2                          ; charged: auto fire
  if X fires id 10 (+0x96 == 10) and energy: SpendEnergy, bits = 7, +0x93 = 2
  if weapon press: +0x93 = 1                                             ; one volley
Option (var 1..3), every update 0x80043DBC:
  angle_n = angle + (n - 1) * 1365 + 3584
  x = X.x + cos(angle_n) * r ; y = X.y + sin(angle_n) * r               ; r: +0x88, 0 -> 32 by 8
  sub-steps 0x80076124: grow r, start (box (-15,-8,30,16)), idle/fire, break, ...
  idle 0x80044118: on its own hit (+0x98): count; at 3 hits: animation 5, break (bit cleared)
  fire 0x80043B90 when controller +0x93 & 1 and the option's wait +0x8E == 0:
      sound 1/19; new object at the option, speed = (cos d, -sin d) d = 0 or 0x800 (facing)
      auto fire (+0x93 & 2): type 10 var 7, 8.0 px/update, box (-8,-8,16,16)
      else: type 54 var n+3, 6.0 px/update, box (-8,-8,16,16), option wait 6
```

## Slot 2: Ice Burst (types 2, 52 shard; 11)

```
Entry 0x801EECE8: end (state 3) on lock/gate, slot != 2, or X fires 11
  type 2 resting (+0x75): SolidObject for X (X can stand on it), then CollideCheck
Init 0x801EEE40: anime 0x800779D8, flip = X; type 2 var 0: at the buster, sound 1/20,
  state 1 step 0; type 52: animation var + 3, box (-4,-4,8,8), state 2
  PaletteSlotLoad(7); SpawnEffect(1, 27, 1)
Main 0x801EEFC0: off screen 16/16 -> end; AnimeTick; step:
  0 0x801EF9C8: follow the buster; at anime flag 1: animation 1, x += 8 forward,
      speed_x = 4.0 forward, gravity 0.15625, box (-6,-6,12,12), terrain (0,0,16,16), step 1
  1 0x801EFAB4: MoveObject2 (gravity); every 3 updates: shard (until 3 shards)
  2 0x801EFB04 (resting): +0x88 = 180 down; blink the last 30; at 0 end
  after the step (not in step 2): +0x98 -> end; walls (collide & 0xC or & 3 both) -> explosion,
      end; floor (8): animation 2, speeds 0, no attack box, +0x75 = 1, +0x88 = 180, step 2;
      other flags -> explosion, end
Shard 0x801EFB5C: type 52, var from a random table, speed = dir * 2.5 (dirs 0, 4, 26, 16, 12, 22)
Shard main 0x801EF818: off screen; move; +0x98 or wall -> hit spark, end

Type 11 entry 0x801EFDA8: end on lock/gate or slot != 2
  init 0x801EF284: var 0: sound 1/21, +0x8C = 360; var 1..4: anime 0x800779F4, hit tag 1,
      var 1/2: animation 0/1, speed_y 1.0; var 3/4: animation 2, speed_x 1.0 (4: flip right);
      PaletteSlotLoad(10); sound 1/22; step 2
  main 0x801EF944: var 0: +0x8C -= 1 while X is not in step 39; at 0 end
  step 0 0x801EFC70: wait for X in step 12, 13, 35 or 74 (dash, air dash, Falcon dash, Mach
      Dash) with sub-step 0; keep X's x, y and step
  step 1 0x801EF410: while X stays in that step: when X has moved 32 px (x; also y in step 74)
      from the kept point: the kept point moves 32 px; 2 parts there (vars 1, 2 for an x move,
      3, 4 for a y move)
  part 0x801EF618: off screen 32 -> end; at its flag 0x80 frame: box, move 16 px out (1 up,
      2 down, 3 left, 4 right); then move out with speed + 1.0 per update, at most 6.0
  end 0x801EFCF4 (var 0): normal palette, m.98 -= 1, m.99 -= 1, m.A6 = 0 (when 1)
Pose (0x8003FB48): animation 95, SpawnWeapon(1, 11, 0), SpawnEffect(1, 27, 9), m.A6 = 1, step 39
```

## Slot 3: Magma Blade (types 3, 12; the slash is type 50/51)

```
Pose for 3 (SabrePoseStart 0x8003FD6C): steps 45/46/47; the slash object 50/51 uses
  sound 1/23 and clut 30793 when slot 3 is selected (0x801EDDFC)
Type 3 entry 0x801F0664: end on lock/gate, slot != 3, or X fires 12
  init var 0 0x801EFE34: own frame stream (320, 32); keep X's step
  var 0 0x801EFFAC: end when X leaves that step or is in step 17
      at X's anime flag 0x02: fireball var 1 at X +-32 px (m.98 += 1, m.99 += 1); 8 updates
      later a second one
  fireball init: box (-16,-16,32,32), terrain (0,0,16,16), speed 5.0 forward, animation 0
  fireball 0x801F0870: off screen 48 -> end; move; wall -> animation 1, no boxes, then end
Type 12 entry 0x801F0700: end on lock/gate or slot != 3
  init var 0 0x801F01A8: own frame stream; keep X's step and facing
  var 0 0x801F0410: at X's anime flag 0x80: fireball at X; then every 20 updates a fireball at
      x = cam.x - 32 (facing right) or cam.x + 352, y = X.y - 32 + rand(64); 8 in all
  fireball init: box (-24,-24,48,48), direction rand(3) of (1, 0, 31) * rand(3, 4, 5) px,
      sound 1/24
  fireball 0x801F0918: off screen 64 -> end; MoveObject (no terrain test)
Pose (0x8003FBBC): animation 95, step 43
```

## Slot 4: Metal Anchor (types 4, 13; main program)

```
Type 4 entry 0x80044EA0: end on lock/gate; X: slot != 4 or X fires 13; +0x8F = collide flags
  init 0x800445A8: box (-6,-6,12,12), terrain (0,0,8,8); X: at the buster; Zero: X +-15, +14
      speed_x 1.5 forward; speed_y 6.0 (Zero 3.0), gravity 0.25 (Zero 0.125)
      sound 1/25; SpawnEffect(1, 27, 2)
  main 0x80045018: off screen 32 -> end; at anime flag 1: box (-10,-10,20,20)
      step 0x800447CC: MoveObject2; on contact: ceiling -> speed_y -3.0; floor -> speed_y 6.0
          (Zero 3.0); right wall -> speed_x -1.5; left wall -> 1.5; back to the old place; MoveObject2
          bounces +0x90 += 1 (Zero 2); a hit: +0x90 += 1 (Zero 9), the hit is cleared
      corner (flags 0xC or 3) or +0x90 >= 8 -> explosion, end
Type 13 entry 0x80044F78: end on lock/gate, slot != 4 or X not in step 40
  var 0 0x80044D04: every 8 updates an anchor (var 1); after 64 updates (8 anchors) end
  anchor init 0x80044914: x, y at random on the top edge (or the edge behind X), speed (4.24,
      -4.24) forward and down (6.0), box (-36,-56,52,88), SpawnEffect(1, 27, 3)
  anchor main: MoveObject (no terrain test); end 32 px outside the view
Pose (0x8003FAFC): animation 97, sound 1/26, step 40
```

## Slot 5: Ground Dash (types 5, 14)

```
TryShoot: no SpendEnergy for id 5 when the front side is blocked (m.89)
Pose (0x8003FBF8): step 44; front blocked: animation 96 at frame 2, sub-step 2 (no throw)
Type 5 entry 0x801F0B90: end on lock/gate, slot != 5, or X fires 14
  init 0x801F0A40: only when X is in step 44 sub-step 0, else end; own frame stream (320, 32),
      at the buster, offset to X kept; sound 1/32
  step 0 0x801F0CBC: follow X; at its animation end: m.act4 = 1 (X: animation 96, sub-step 1)
  step 1 0x801F0D58: follow X; when X reaches sub-step 2 (X's anime flag 0x80): sound 1/34,
      speed (1.0, 1.0) or with up held (0.5, 1.5), gravity 0.03125, box (-24,-24,48,48)
  step 2 0x801F0E6C: off screen 24 -> end; MoveObject2; terrain -> explosion, end
Type 14 entry 0x801F0C30: end on lock/gate or slot != 5
  init 0x801F0F98: own frame stream, at the buster, sound 1/33
  step 0: at the buster; at its animation end m.act4 = 1
  step 1: when X reaches sub-step 2: sound 1/35, speed 8.0 forward, 4.0 up, gravity 0.25,
      box (-32,-32,64,64)
  step 2: off screen 24 -> end; MoveObject2 (no terrain test)
Pose (0x8003FC88): step 75; game.stop 0x12..0x19 = 1, game+0x1C (input lock) = 1, m.E1 = 0
  step 75 sub 0: m.E1 += 1; on act4: sub 1; sub 1: m.E1 += 1; at 60: all stop flags and the
  lock 0, sub 2; sub 2: StandOrFall. Every sub-step: slot != 5 -> StandOrFall
```

## Slot 6: Meteor Rain (types 6, 15)

```
Type 6 entry 0x801F1A58: end (state 2) on lock/gate, slot != 6, or X fires 15
  init 0x801F129C: anime 0x80077A28, hit tag 1
      var 0: sound 1/27, at the buster, animation 0
      var 1: at the parent, y - 8, speed_y 4.0, box (-14,-14,28,28), terrain (0,0,4,13), anim 1
      var 2: at the parent, y + 17, speed_y -4.0, the parent's boxes, animation 3
  var 0 step 0x801F1B64: at anime flag 0x40: var 1 (SpawnWeaponFrom); animation end -> end
  var 1 step 0x801F1450: moves when the parent shows flag 0x80; ceiling or y < cam.y + 16 ->
      animation 2, next step; floor-and-ceiling -> animation 4, no boxes
  var 1 step 0x801F1C1C: at anime flag 0x40: var 2; animation end -> end
  var 2 step 0x801F1CD4: moves when the parent showed flag 0x80; floor -> y + 9, animation 4
      (splash), no boxes; then 0x801F1DD0: animation end -> end
  the hit result is not read: a hit or a deflect does not stop any part
Type 15 entry 0x801F1AE8: end on lock/gate or slot != 6
  init 0x801F15B0: var 0: sound 1/27, at the buster, m.A6 = 3
      var 1: at the parent, y - 8, speed_y 4.0
      var 16: sound 1/28, animation 5, x at random on the top edge (or 36 px outside the side
          behind X), speed (5.30, -5.30) forward-down (7.5)
  var 0 0x801F1EC8: at anime flag 0x40: var 1; animation end -> end
  var 1 0x801F1F4C: rises; at y < cam.y - 16: hidden, +0x8E = 30, next step
  var 1 0x801F2000: every 30, then 20 updates: var 16; after 10 -> end
  var 16 0x801F2094: MoveObject; end 32 px outside the view (56 px below)
  end 0x801F21AC: m.A6 = 0 (when 3)
```

## Slot 7: Guard Shell (types 7, 53 shard; 16; main program)

```
TryShoot: no SpendEnergy and no shot pose for id 7
Type 7 entry 0x80045D68: end (state 3) on lock/gate, HP 0, ride; X: slot != 7 or X fires 16;
  Zero: m.A6 != 2
  init 0x800451B8 var 0: sound 1/29; one part: var 1 (up held), 2 (down held), 3 (no up/down);
      Zero: var 3
  part init: anime 0x80077A40, SpawnEffect(1, 27, 4), PaletteSlotLoad(27)
      at X + ofs[var]: 1 (0, -32), 2 (0, +32), 3 (+-32, 0); animation 0 (1, 2) or 1 (3)
  part each update 0x800456D4: follow X; when the controller is ending: animation 4/5, end
  part sub 0 0x80045F78: energy < cost, or a new weapon press -> sound 1/29, end
  part sub 1 0x8004605C: at anime flag: box (-24,-12,48,24) for 1, 2; (-12,-24,24,48) for 3
  part sub 2 0x800460C8: on +0x9C bit 1 (a blocked shot): +0x8F = 1, animation 2/3, +0x8E = 20
  part sub 3 0x8004548C: after 20 updates: SpendEnergy; shard type 53 var 5: sound 1/31,
      speed 3.0 up (1), down (2) or forward (3), box (-8,-8,16,16); back to sub 2
  end 0x80045810: a pending block (+0x8F) is paid now
Shard (type 53 sub 4, 0x80046148): off screen 32; move; hit or wall -> spark, end
Type 16 entry 0x80045E58: end on lock/gate or slot != 7
  init 0x80045928 var 0: sound 1/30; vars 1..4; SpawnEffect(1, 27, 11); PaletteSlotLoad(30)
      vars 1..4: screen space (layer -1), start at X's screen point
      vars 5..8: sound 1/31 (var 8); from corner p[var-5] toward q[var-5] at 4.0 px/update
          p = (52,31) (268,31) (268,209) (52,209); q = (320,240) (0,240) (0,0) (320,0)
  var 0 sub 0 0x800462C0: X not in step 42 -> +0x91 = 1, end; 4 parts ready -> X sub-step 1
  var 0 sub 1 0x80046318: wait +0x90; on a weapon press: +0x8F = 1 (fire), +0x90 = 50,
      +0x8E += 1; at 8 -> end
  part 0x80045C14: 20 updates from X's point to its corner (linear), then the corner
  part 0x80046394: when the controller fires: var + 4 from here
  flying part 0x80046478/0x800464C4: box at flag 0x80; move; off screen 16 -> end
  end 0x80046550: m.A6 = 0 (when 2)
Pose (0x8003FCD4): animation 95, m.A6 = 2, step 42
```

## Slot 8: Ray Arrow (types 8, 17)

```
Type 8 entry 0x801F26B4: end on lock/gate, slot != 8, or X fires 17
  init 0x801F226C var 0: sound 1/36, speed_x 6.75, at the buster, animation 0
      vars 1, 2, 3: flashes at the parent (animation 4; 5 at y - 8; 6 at y + 8)
  main 0x801F28F4: while attached (step2 0): m.91 = 5 (X keeps the shot pose)
  step 0 0x801F27D4: follow the buster; at anime flag 0x80: box (-18,-6,36,12), terrain
      (0,0,12,6), speed_x by facing, step 1
  step 1 0x801F23C8: move; wall in front: flash var 1; box (-6,-18,12,36), terrain (0,0,6,12),
      speed_x 0, speed_y 6.75 up (-6.75 with down held), step 2; off view -> end
  step 2 0x801F259C: move; ceiling (up) or floor (down): flash var 2 or 3, end
  flash 0x801F2950: animation end or 10 updates -> end
  the hit result is not read: the arrow goes through enemies
Type 17 entry 0x801F2758: end on lock/gate or slot != 8
  made by pose step 41 (0x801ECAF4) at the end of X's animation: sound 1/37; 5 columns at
      X.x - 64, -32, 0, +32, +64; var 0 at cam.y + 368, var 1 at cam.y + 528; m.98, m.99 += 1 each
  init 0x801F2A38: speed_y 16.0; var 0 animation 11, box (-16,-128,32,255); var 1 animation 12,
      no box
  main 0x801F2AFC: MoveObject; end above the view (var 0: 128 px, var 1: 32 px)
Pose (0x8003FD18): animation 95, m.E0 = 2, no terrain box, no_hit, lock, step 41
```

## Other X weapon objects

```
20 0x801ED36C: on a hit: a type-21 part at the shot when fewer than (form 4 ? 3 : 1) live;
   the hit is cleared (the shot goes on); deflect, wall or off screen (42/34) -> end
21 0x801EEA8C: animation 31 (var 0) or a second set; short life (8), no move
22 0x801EE14C: lives while X shows animation 106 (Giga, form 4); at X; box (-52,-34,124,66);
   hit tag + 1 every 4 updates
23 0x801EE210: lives while X is in step 35 (Falcon dash); box (-25,-25,50,50); tag every 4
32 0x801F2C08: lives while X is in step 74 (Mach Dash); PaletteSlotLoad(84, 26); tag changes
43 0x801F3604: 10.0 (12.0 Speed Shot); box (-16,-6,16,12); PaletteSlotLoad(84, 26); sound
   1/10; tag + 1 every 4
   updates; a hit is cleared (pierce); deflect or wall -> end animation
45 0x801ED524: 8.0 (12.0 Speed Shot) in direction 1, 0 or 31 (5, 6, 5 of 16); box
   (-6,-6,12,12); in step 31 three kunai downward (24, 22, 26); ends at the first hit or wall
46 0x801ED898: slash at X +-32 (+-64 when X's act4); box from the frame flag; tag + 1 every 8
47 0x801EDA64: lives in step 37; two parts that turn around X at 48 px; tag + 1 every 8
49 0x801ED058: var 0 from cam.y - 32 down at 12.0, var 1 from cam.y + 272 up at 12.0;
   x = cam.x + 8 + 16 * rand(20); box (-8,-24,16,48); tag + 1 every 4; off screen 32/64
50/51 0x801EDCB0: the sabre slash; follows X; box by frame flag (0x801F3858); tag = frame
   flag; after a hit: no box for 4 updates; at most 4 hits; in step 77 at flag 0x40: type 57
56 0x801F3200 (step 76), 57 0x801F33F0 (Giga 77 wave, 6.0 px/update forward)
```
