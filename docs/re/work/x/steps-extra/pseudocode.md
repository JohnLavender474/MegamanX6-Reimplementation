# Pseudo code: player steps 32–77 and the forms

Units: px per update (16.16), times in updates, `speed_y` > 0 is up. Names as in `notes.md`.

## Form and member

```
new game (ovl 19):     X: armors = 0x01, form = 1      Zero: armors = 0x10, form = 5
  cheat (11 inputs):   X: armors = 0x08, form = 4      Zero: armors = 0x31
armour select (ovl 16): list = [0] + [1 if 0x01] + [5 if 0x10] + [2 if 0x02] + [3 if 0x04] + [4 if 0x08]
  confirm: form = list[cursor]; character = (form == 5)
stage load (0x80013BF0): member = 85 if character else {1: 87, 2: 88, 3: 89}.get(form, 86)
beam-in landing (PlayerLoadForm): mega.form = form; mega.parts = game.parts[var]
  form 2: skills = 0; effect (1, 27, 5); palette slot 2 <- 25, 24
  else:   skills = game+0x60
```

## Air tries by form (TryAirDash, air.md)

```
form 1: jump or dash pressed  -> Falcon dash, step 35
form 3: jump or dash pressed  -> Mach Dash, step 74
form 4: jump pressed          -> hover, step 33
        dash pressed          -> air dash, step 13
Zero:   dash pressed          -> air dash, step 13
```

## Step 33: hover (form 4)

```
entry: speeds 0, gravity 0, air_lock = 4, timer = 180, bob = 0, anim 21
each update:
  locked -> fall
  air weapon pose -> (that)
  sub 0: wait frame flag 0x80 -> effect (33, 33), sub 1
  sub 1..6:
    timer -= 1; timer == 0 or jump pressed -> fall
    wall test -> wall grab
    sub 1 only: dash pressed and no shot wait -> air dash (step 13)
    y += bob[i]; i = (i + 1) & 15; speed_x = 0
    held side free: speed_x = +-2.0 (dir: +1 to the facing side, -1 back)
    sub 1 with a direction: timer = 40
    moves: forward 23, back 25, stop 24 / 26, idle 22 (PlayerMove only in the move sub-steps)
```

## Special attack (0x8003E344, X, forms 1-4)

```
needs: X, no shot wait, gauge (+0xA8) full (= +0x14A), pressed 0x40
form 4 also: air_lock == 0 and facing side free
all: clear action flags, slot 0, shot set-up, latch 1, iframes 0, +0xA4 0
     gauge = 0 (not form 4); no_hit = 1; player lock = 1
form 1: anim 174 (floor) / 177 (air), effect (1, 27, 8), step 36
form 2: anim 185 / 186, air_lock 5, no terrain box, +0xE0 2, effect (1, 27, 7), step 37
form 3: anim 185 / 186, air_lock 5, no terrain box, +0xE0 2, step 77
form 4: anim 106, air_lock 5, speed (+-2.5, 3.5), fall gravity, +0xE1 24, +0xE0 1, step 34
```

Steps 36, 37, 77 share a frame: sub 0 fades the screen (51 updates); sub 1 runs the attack
(36: weapon objects type 49 every 3-5 updates for 120 updates; 37: two weapon objects type 47
and a slash object 50/51 at each flag 0x80 frame for 148 updates; 77: one slash object 50/51 at
the flag 0x80 frame); then the fade goes back; sub 2 at the animation end: no_hit and lock off,
terrain box back; sub 3: palette back, stand or fall.

Step 34: rise with gravity until flag 0x80; at flag 0x20: weapon object type 22, palette 73,
sound (1, 18), speed_x = +-8.0, no gravity; then 24 moves (192 px) or until the facing side
is blocked; then no_hit and lock off, palette back, stand or fall.

## Step 74: Mach Dash (form 3)

```
entry: anim 189, speeds 0, dash_air 1, timer = 15, air_lock = 7, weapon object (33, 32, player)
sub 0: face the held side; wait the animation end;
       hold jump or dash: wait up to 120 updates more
       sound (1, 5); up: anim 190, speed_y = +8.25; down: 191, speed_y = -8.25;
       else 192, speed_x = +-8.25 forward; no_hit = 1; effect (33, 25)
sub 1: move; timer -= 1; timer < 8: no_hit = 0
       timer 0 or blocked in the move direction: sound (1, 6);
       down dash on the floor with down held: crouch; else stand or fall
```

## Weapon poses (0x8003F864, by the shot type +0x96)

```
3, 50 -> 45 ground slash / 46 air slash / 47 wall slash (183/184/187; form 2: 185/186)
5 -> 44      11 -> 39      12 -> 43      13 -> 40      14 -> 75 (all classes frozen)
16 -> 42     17 -> 41 (no terrain box, no_hit, lock)   46 -> 38      56 -> 76
others: nothing
```

Steps 40, 42, 43, 44, 75 hold while the weapon object lives (`+0x99` != 0) and the slot `+0x93`
is 4, 7, 3, 5, 5. Step 41 holds while `+0x99` != 0. Steps 38, 39, 45, 46, 47, 76 end at their
animation end.

## Steps 64-73

```
64..71: stand (0x80035274)
72: points into the stage overlay area; no writer
73: AnimeTick until exit_req != 0, then state 3; no writer
```
