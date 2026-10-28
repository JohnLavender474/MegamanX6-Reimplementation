# Commander Yammark: pseudo code

Overlay 03. `boss` = the `var` 0 object; `G` = the overlay globals:

| global | name here |
|---|---|
| `0x800F7DC0`…`0x800F7DDE` | `P.*` room points (absolute px) |
| `0x800F7DE0` | `fight_time` |
| `0x800F7DE4` | `boss_dead` |
| `0x800F7DE8` | `boss_stunned` |
| `0x800F7DEC` | `boss` |
| `0x800F7DF0` | `pair` (pointer to {drones, stay}) |
| `0x800F7E00`, `04`, `08` | the three pairs |
| `0x800F7E0C` | `ready` (a counter) |
| `0x800F7E0E` | `mode` |
| `0x800F7E10` | `alive` |
| `0x800F7E20`[6] | `slot_used` |
| `0x800F7E30` | `knock_hit` |
| `0x800F7E40`[210] | `trail` {x, y, flip} |

## Dispatch tables

| table | entries |
|---|---|
| `0x800F7BDC` boss states | 0 `Setup`, 1 `Main`, 2 `Death` |
| `0x800F7BE8` boss steps | 0 `HitReactionDispatch`, 1 nop, 2 `Intro`, 3 `Idle`, 4 `Think`, 5 `Swarm`, 6 `Lance`, 7 `Dash`, 8 `WallDash`, 9 `Chase`, 10 `Stunned` |
| `0x800F7C14` Intro | `I0_Wait` `I1_Warning` `I2_Descend` `I3_Talk` `I4_Pose` `I5_Bar` `I6_Fill` |
| `0x800F7C34` Swarm (5) | `A0_Phase` `A1_Gather` `A2_GoSide(random y)` `A3_Summon` `S4_Ready(mode 2)` `S5_Go(mode 3)` `S6_Wait500` |
| `0x800F7C50` Lance (6) | `A0` `A1` `A2_GoSide(random y)` `A3` `L4_Ready` `L5_Go(mode 4)` `L6_Wait600` |
| `0x800F7C6C` Dash (7) | `A0` `A1` `A2_GoSide(96)` `A3` `D4_Ready(mode 2)` `D5_Track` `D6_DashToWall` |
| `0x800F7C88` WallDash (8) | `A0` `A1` `A2_GoSide(96)` `A3` `W4_Ready(mode 5)` `W5_Wait260` `W6_ToWall` `W7_Pause50` |
| `0x800F7CA8` Chase (9) | `A0` `A1` `A2_GoSide(150)` `A3` `C4_Ready(mode 6)` `C5_Chase` |
| `0x800F7CC0` Stunned (10) | `T0` `T1` |
| `0x800F7CC8` Death | `X0` `X1` `X2` |
| `0x800F7D54` drone states | `DroneSetup` `DroneMain` `DroneDelete` |
| `0x800E99B8` visual 15 by `var` | 0,1 → `0x800F7D78` shot; 2 → `0x800F7D84` bomb; 3 → `0x800F7D90` burst; 4 → `0x800F7D9C` wings; 5 → `0x800F7DA8` flash; 6 → `0x800F7DB4` trail |

## Boss

### Entry — 0x800F40C8

```
if self.var == 1: DroneEntry(self); return
self.prev = self.pos; self.display = 0
boss_states[self.state](self)
CollideCheck(self)
```

### Setup — 0x800EF810 (state 0)

```
X0 = camera_layer0.target_min_x; Y0 = camera_layer0.target_min_y
P.left = X0+16; P.right = X0+336; P.floor = Y0+192; P.spawn = (X0+240, Y0-32)
P.side = (X0+48, X0+304); P.high = Y0+96; P.low = Y0+150
P.entry_x = (X0+32, X0+176, X0+320); P.entry_y = Y0-32; P.col = (Y0+48, Y0+144, Y0+96)
graphics: stage 12 → stage_object_table[2] and CLUT 0x7948; else GetSpriteBank(type)
anime_table = 0x800F7B44; priority 5; attack = hurt = (-8,-16,24,48); terrain = none
damage_table = dmg_tbl_yammark; contact 4; speeds 0; hp = 0
self.pos = P.spawn
game.boss_flag25 = 0; game.boss = self
game.boss_hp_max = 32; BossLevel(); game.boss_hp_max += {0,6,12,16}[game.boss_level]
self.pattern = self.pattern_i = self.84 = 0
PlayerEvent(20, 64)
fight_time = boss_dead = boss_stunned = 0; boss_ptr = self; alive = 0; slot_used[0..5] = 0
pairs = {(3,1),(4,2),(6,3)}; pair = &pairs[0]; ready = mode = 0; knock_hit = 0
if game.difficulty == 2: for each pair: stay = max(stay - 2, 0)
self.step = 2; self.state = 1
```

### Main — 0x800EFB9C (state 1)

```
r = ContactWeapon(self)
if r < 0:
    self.state = 2; self.step = self.step2 = 0; clear flash
    boss_dead = 1; mega[0x7A] = 1; DisplayCheck(self, 64, 64); return
ContactMega(self)
if r > 0 and r != 127:
    self.84 = (r == 25 or r == 26) ? 2 : 30
    self.damage_table = dmg_tbl_deflect
if r in (9, 18, 45) and knock_hit == 0:
    boss_stunned = 1; self.step = 10; self.step2 = 0
if self.84:
    self.84 -= 1; flash = (self.84 & 2)
    if self.84 == 0: self.damage_table = dmg_tbl_yammark; flash off
boss_steps[self.step](self)
if self.step != 2: fight_time += 1
```

### Phase — inline in Think and every A0

```
if hp < hp_max/3: pair = &pairs[2]
elif hp < 2*hp_max/3: pair = &pairs[1]
```

### Think — 0x800EFD38 (step 4)

```
Phase()
r = GetRNG() & 15; self.pattern_i = 0
if mega.var == 0:  pattern = hp > hp_max/2 ? (r < 8 ? 0 : 2) : (r < 4 ? 0 : r < 12 ? 4 : 5)
else:              pattern = hp > hp_max/2 ? (r < 8 ? 1 : 3) : (r < 4 ? 1 : r < 12 ? 4 : 5)
NextStep()          ; step = patterns[pattern][pattern_i++]; step2 = 0
AnimeTick; MoveObject2; DisplayCheck(64, 64)
```

`patterns` (`0x800F7BA0`): {5,7,4}, {6,7,4}, {5,7,7,4}, {6,7,7,4}, {9,8,4}, {9,7,7,4}.

Each attack step N is `step_N: subs[step2](self); AnimeTick; MoveObject2; DisplayCheck(64, 64)`.

### A0_Phase — 0x800F4744 / 0x800F49E4 / 0x800F4BFC / 0x800F4E44 / 0x800F51E0

```
Phase(); SetAnime(0)
if mode != 2: ready = 0; mode = 1; step2 += 1   ; call the drones back
else: step2 += 2
```

### A1_Gather — 0x800F4824 / 0x800F4AC4 / 0x800F4CDC / 0x800F4F24 / 0x800F52C0

```
if ready == alive: step2 += 1
```

### A2_GoSide — 0x800EFF18 (5), 0x800F0360 (6), 0x800F09C8 (7), 0x800F0F1C (8), 0x800F1334 (9)

```
if x < (P.left + P.right)/2: tx = P.side[0]; flip = right   else: tx = P.side[1]; flip = left
ty: step 5, 6: random(P.high: 7E=0, P.low: 7E=1); step 7, 8: P.high (7E=0); step 9: P.low (7E=1)
if at (tx, ty): speed = 0
else: a = ratan2(ty - y, tx - x); speed = (rcos(a), -rsin(a)) << 5     ; 2 px/frame
SetAnime(speed_x ? 2 : speed_y > 0 ? 3 : 4)
step2 += 1
```

### A3_Summon — 0x800F008C (5), 0x800F04D4 (6), 0x800F0B0C (7), 0x800F1060 (8), 0x800F1478 (9)

```
clamp x at tx and y at ty in the direction of motion (then that speed = 0)
if speed == 0:
    mode = 0; ready = pair.drones - alive
    repeat ready times: d = GetMainObject(); d.flags = self.flags; d.type = 49; d.var = 1; d.step = 1; d.80 = self
    if ready > 0: SetAnime(5); PlaySound(2, 34, self); visual 15 var 5 (parent self)
    step2 += 1
```

### S4 / L4 / D4 / W4 / C4 — the "ready" sub

```
if ready == 0:
    SetAnime(0)            ; C4: SetAnime(2)
    D4, W4, C4: PlaySound(2, 39, self)
    mode = 2 (S4, L4, D4), 5 (W4), 6 (C4)
    work.7C = 0; step2 += 1     ; C4 also self.8E = 0
```

### S5_Go — 0x800F48A8; L5_Go — 0x800F4B48

```
work.7C += 1
if work.7C == 2: mode = 3 (S5) or 4 (L5); work.7C = 0; step2 += 1
L5 also: self.8E = 0; act4 = 0; work.7E = 0
```

### S6_Wait500 — 0x800F48EC

```
if work.7C == 120: PlaySound(2, 38)
work.7C += 1; if work.7C == 500: NextStep()
```

### L6_Wait600 — 0x800F07A8

```
if work.7C == 120: PlaySound(2, 35)
act4 swings: 8E == 0 → act4 += 2, at >= 30 → 8E = 1;  8E == 1 → act4 -= 2, at < -29 → 8E = 0
if knock_hit == 30:
    hp -= 3
    if hp <= 0: die as in Main
    else: PlaySound(2, 33); SetAnime(8); attack_box = none; damage_table = dmg_tbl_deflect
elif knock_hit == 1: SetAnime(0); attack_box = boss box; damage_table = dmg_tbl_yammark
if knock_hit: knock_hit -= 1
work.7E += 1; work.7C += 1
if work.7C == 600: SetAnime(0); boxes/table back; knock_hit = 0; NextStep()
```

### D5_Track — 0x800F0DE0

```
face the player
if work.7C == 90: PlaySound(2, 40)
y moves 2 px toward mega.y (no overshoot)
work.7C += 1
if work.7C == 120: speed_x = (x < mega.x) ? +2 : -2; step2 += 1
```

### D6_DashToWall — 0x800F4D70

```
if collide_flags & 3: mode = 0; speed_x = 0; NextStep()
```

### W5_Wait260 — 0x800F4FB8

```
if work.7C == 90: PlaySound(2, 41)
work.7C += 1
if work.7C == 260: speed_x = flip ? +2.5 : -2.5; work.7E = 0; self.8E = 0; step2 += 1
```

### W6_ToWall — 0x800F5048; W7_Pause50 — 0x800F5108

```
W6: work.7E += 1
    if wall in the direction of motion:
        speed_x = 0; self.8E += 1
        if self.8E == 3: NextStep() else: work.7C = 0; step2 += 1
W7: work.7C += 1; work.7E += 1
    if work.7C == 50: flip = !flip; speed_x = ±2.5 in the new facing; step2 -= 1
```

### C5_Chase — 0x800F174C

```
if work.7C == 90: PlaySound(2, 42)
if (work.7C & 7) == 0: face the player; a = ratan2 to the player; speed = 2 px/frame along a
trail[self.8E] = (x, y, flip); self.8E = (self.8E + 1) % 210
work.7C += 1
if work.7C == 500 or alive == 0: SetAnime(0); speed = 0; NextStep()
```

### Stunned — 0x800F5358 (step 10)

```
if self.84 < 15: self.84 = 30
T0 (0x800F53D4): speed = 0; SetAnime(8); PlaySound(2, 33); attack_box = none; damage_table = deflect; work.7C = 0; step2 = 1
T1 (0x800F5434): work.7C += 1
    if work.7C == 90: SetAnime(0); boss_stunned = 0; boxes/table back; step = 4; step2 = 0
```

### Intro — 0x800F414C (step 2)

```
I0 (0x800F4188): if mega.step == 20:
    cam.limit_speed = 2; PlayerEvent(29, 64); work.7C = 40
    work.80 = GetSysObject() as type 24 at self.pos; step2 += 1
I1 (0x800F4220): if game.exit_req == 0:
    work.7C -= 1; if 0: PlayerEvent(20, 64)
    if work.80.flags == 0: visual 15 var 4 (wings); SetAnime(0); speed_y = -1; step2 += 1
I2 (0x800F42D8): if no terrain box and y >= P.floor - 128: terrain_box = (0, 8, 16, 32)
    if on the floor: ready = 0
        repeat pair.drones times: drone with var 1, step 0, 80 = self
        speed_y = 0; step2 += 1
    AnimeTick; MoveObject2; DisplayCheck
I3 (0x800F43F4): if ready == 2 * pair.drones:
    if game.stage != 12: TextOpen(0, 255, game[0x2E]); game[0x2E] = 1
    mode = 2; work.7C = 0; step2 += 1
I4 (0x800F44A4): if text_flag (0x8008EAFC) == 0: SetAnime(1); step2 += 1
I5 (0x800F4508): if anime.next == 0: SetAnime(0); HpBarShow(0); game[0x24] = 1; work.7C = 3; step2 += 1
I6 (0x800F4588): if HpBarBusy(): return
    if hp < hp_max: work.7C -= 1; if 0: PlaySound(0, 14); work.7C = 3
                    hp += 1
    else: PlayerEventEnd(); step = 4; step2 = 0; work.7C = 0
```

### Death — 0x800F54A4 (state 2)

```
X0 (0x800F54E0): work.7C = 127; work.7E = 25; iframes = 25; flash off; SetAnime(7)
    PlaySound(2, 32); step += 1; PlayerEvent(20, mega.flip)
X1 (0x800F5564): work.7C -= 1; if 0: work.80 = GetSysObject() as type 26 at self.pos; step += 1
    work.7E -= 1; if -1: toggle flash; iframes -= 5 (min 5 used); work.7E = max(iframes, 5)
        (a reload of n gives n + 1 updates: intervals 26, 21, 16, 11, then 6)
X2 (0x800F1904): if work.80 alive:
        if work.80[7] == 0: blink every 6 updates (counter reloaded with 5)
        else: not drawn (no DisplayCheck)
    else:
        if game.stage == 12: game.exit_req = 0x80; game[0x2C + game[0x1D]] = 1; game[0x1D] += 8
        else: v = GetMainObject(); v.flags = 0x41; v.type = 15; v.var = fight_time < 5401 ? 0x10 : 0x11
        DeleteObject3(self)
```

## Drone

### DroneEntry — 0x800F5664

```
display = 0; drone_states[self.state](self); CollideCheck(self)
```

### DroneSetup — 0x800F1A3C

```
copy boss tex, anime_table, sprite_data, CLUT (flash off); priority 4; no boxes
damage_table = dmg_tbl_yammark; layer = mega.layer
for i in 0 .. pair.drones - 1: if !slot_used[i]: slot_used[i] = 1; self.88 = i; break
alive += 1; hp = 8; contact 2; speeds 0; state = 1
```

### DroneMain — 0x800F1B54

```
if boss_dead: state = 2; return
r = ContactWeapon(self)
if r < 0 and not (step == 5 and step2 == 5 and mega.var == 1):
    DisplayCheck(32, 16); SpawnExplosion; SpawnJunk(2, {21, 22}); state = 2; return
ContactMega(self); work.7E += 1
if boss_stunned:
    if step == 1: slot_used[88] = 0; alive -= 1; DeleteObject3(self); return
    if 88 >= 0: mode = 0; step = 99
else switch mode:
    1: if step != 2 and 88 >= 0: speed = 0; step = 2
    2: if step != 3 and 88 >= 0: step = 3
    3: if step != 4 and 0 <= 88 < pair.drones - pair.stay: step = 4
    4: if step != 5 and 0 <= 88 < pair.drones - pair.stay: step = 5
    5: if step != 6 and 88 >= 0: step = 6
    6: if step != 7 and 88 >= 0: flip = boss.flip; 8C = boss.x; 8E = boss.y; step = 7
   (each change: step2 = 0)
switch step: (below)
tail (per step; "blink" = DisplayCheck only on odd work.7E):
      step 0: always DisplayCheck(32, 16); step 1: sub < 2 DisplayCheck, else blink;
      steps 2, 3: always blink; steps 4-6: sub < 2 blink, else DisplayCheck; step 7: sub 0 blink, else DisplayCheck
      step 1 sub 0 before work.7E = 38: return (no display, no move)
      AnimeTick; MoveObject2 (step 99: AnimeTick only)
```

`Ring(a)` = position boss.pos + (rcos(a) / 128, -rsin(a) / 128): 32 px from the boss. `Spin(a)` =
a ± 546 per update (+ when the boss faces left). `TurnTo(t)`: Spin; when the signed difference
t − a crosses from > 0 to ≤ 0 (boss left) or from < 0 to ≥ 0 (boss right): a = t, 8E += 1. At
difference 0 it keeps spinning, so the second arrival (8E == 2) comes one full lap (120 updates)
after the first.

```
step 0 (intro): sub 0: x = entry_x[slot] (slot 3, 4: midpoints); y = P.entry_y
                       a = angles[count][slot]; aim at boss at 2 px/frame (slot 0: 2.81); face boss; SetAnime(11)
                sub 1: when |dx| < 3 and |dy| < 3: flip = boss.flip; speed = 0; ready += 1
                sub 2: when ready == pair.drones: r = 0
                sub 3: pos = boss + r·(cos a, -sin a); r += 1; at r == 32: ready += 1
                sub 4: pos = Ring(a)
step 1 (summoned): flip = boss.flip
                sub 0: after work.7E >= 38: SetAnime(9); a = angles[count][slot]; pos = Ring(a)
                sub 1: at the end of the animation: SetAnime(11); work.7C = 0
                sub 2: work.7C += 1; at 60: ready -= 1
step 2 (back):  sub 0: no boxes; SetAnime(11); a = angles; speed = 0
                sub 1: move 4 px/axis per update to Ring(a); there: ready += 1
                sub 2: pos = Ring(a)
step 3 (circle): a = Spin(a); pos = Ring(a)
                sub 0: SetAnime(11)
                sub 1: if boss.(step, step2) == (7, 6) and work.7E % rate[slot] == 0: shot (visual 15 var 1)
step 4 (swarm):  sub 0: a = Spin(a); pos = Ring(a); 8E = 0
                 sub 1: TurnTo(+90° even slot / -90° odd slot); pos = Ring(a)
                        when 8E reaches 2: boxes on; SetAnime(10); speed_y = (odd ? -3 : +3) × 1.25
                 sub 2: if collide_flags & 0xC: speed_y = -speed_y
                        if work.7E % 30 == 0: shot (visual 15 var 0)
step 5 (lance):  sub 0: spin; 8E = 0
                 sub 1: TurnTo(180° boss faces left / 0°); when 8E == 2: boxes on; SetAnime(10)
                 sub 2: x moves 3 px toward boss.x ∓ 32 (in front); there: 8E = col_y[slot]
                 sub 3: y moves 3 px toward 8E + boss.act4; there: boss.work.7E = 60
                        (each arrival, also after sub 7 → sub 3: the next dash is 15 updates later)
                 sub 4: y = 8E + boss.act4; when boss.work.7E >= 75: boss.work.7E = 0
                        face the player; SetAnime(13); PlaySound(2, 51)
                        speed = toward the player at 6 px/frame; 8A, 8C = player pos; trail (visual 15 var 6)
                 sub 5: if r != 0 and mega.var == 1:   ; hit by Zero
                            speed_x = flip ? -4 : +4; speed_y = +4; gravity = 0x2800; no boxes
                            slot_used[88] = 0; alive -= 1; 88 = -1; step2 = 8
                        elif within 5 px of (8A, 8C) or any collide flag: SetAnime(10); speed = 0; work.7C = 0; step2 = 6
                 sub 6: work.7C += 1; at 60: step2 = 7
                 sub 7: x moves 3 px toward boss.x ∓ 32; there: flip = boss.flip; step2 = 3
                 sub 8: flicker; if |boss.x - x| <= 16 and -32 <= boss.y - y <= 16 and knock_hit == 0
                                    and boss.step == 6: knock_hit = 30
                        if x < P.left - 32 or x > P.right + 32: DeleteObject3(self)
step 6 (wall):   x = boss.x
                 sub 0: spin; 8E = 0
                 sub 1: TurnTo(±90°); when 8E == 2: attack + hurt boxes (no terrain); SetAnime(10)
                 sub 2: y moves 3 px toward boss.y + {-40,40,-60,60,-20,20,-80,80}[slot]
                 sub 3: wait until boss.speed_x != 0
                 sub 4: y ±1 per update by boss.work.7E bit 5: 32 up, 32 down (64-update period; odd slots opposite)
step 7 (trail):  sub 0: a = Spin(a); pos = (8C, 8E) + 32·(cos a, -sin a)
                        when boss.work.7C == (slot + 1) * 30: boxes on; SetAnime(10); 8E = 0; act4 = 0
                 sub 1: (tx, ty, flip) = trail[8E]; move 3 px/axis toward (tx, ty)
                        act4 (cooldown) -= 1 if > 0
                        if |mega.x - x| < 12 and mega.y > y + 48 and act4 == 0:
                            act4 = 30; bomb (visual 15 var 2); SetAnime(12)
                        if act4 == 15: SetAnime(10)
                        8E = (8E + 1) % 210
step 99:         nothing (AnimeTick only)
```

### DroneDelete — 0x800F56B4

```
slot_used[88] = 0; alive -= 1; DeleteObject3(self)
```

## Visual type 15

```
Entry 0x800F3AD8: if +0x84 == 0 and collide_flags == 0: CornerCheck(self)
                  display = 0; table[var][state](self)
```

```
var 0/1 set-up 0x800F3BE4: copy parent graphics; priority 3; terrain (0,0,7,7); attack, hurt (-7,-7,14,14)
    damage_table = dmg_tbl_yammark; SetAnime(17); pos = parent.pos
    difficulty != 2: speed_x = ±3 toward the player × 1.25; flip by side
    difficulty == 2: speed = 3 px/frame toward the player
    hp = 1; contact 2; PlaySound(2, 50); state 1
var 0/1 run 0x800F56F8: if boss_dead or boss_stunned or knock_hit: state 2
    if collide_flags or ContactWeapon < 0 or ContactMega: SpawnExplosion; state 2
    else AnimeTick; MoveObject2; DisplayCheck(16, 16)
var 2 set-up 0x800F57DC: priority 3; terrain (0,0,2,6); attack (-2,-6,4,12); no hurt, no table
    SetAnime(18); flip, pos = parent; speed_y = -1; hp 0; contact 2
var 2 run 0x800F3DE4: same stop tests (state 2)
    on the floor (collide & 8): 3 × visual 15 var 3 at (+10, -8, delay 1), (-16, -4, delay 8),
        (+4, -12, delay 16); state 2; DisplayCheck(4, 16)
    else ContactMega; speed_y -= 0x1000, min -3; AnimeTick; MoveObject2; DisplayCheck(4, 16)
var 3 set-up 0x800F58E8: priority 2; attack (-8,-8,16,16); SetAnime(20); contact 4
var 3 run 0x800F59A8: stop tests; step 0: delay -= 1, at 0: SpawnExplosion; step 1
    step 1: count += 1; at 30: state 2 (so 29 updates of contact); else ContactMega; DisplayCheck(16, 16)
var 4 set-up 0x800F5AB8: priority 6; no boxes; SetAnime(14)
var 4 run 0x800F3FB0: flip = boss.flip; pos = boss + (flip ? -3 : +3, -6)
    if boss.anime in (0, 2, 3, 4): if !played: PlaySound(2, 48); played = 1
    else: played = 0; PlaySound(2, 20)
    AnimeTick; MoveObject2; if boss.anime == 0: DisplayCheck(64, 32)
var 5 set-up 0x800F5B90: priority 3; SetAnime(15); flip, pos = boss
var 5 run 0x800F5C78: if anime.next == 0 or boss_dead or boss_stunned or knock_hit: state 2
    count += 1; at 22: PlaySound(2, 49); AnimeTick; MoveObject2; DisplayCheck(96, 96)
var 6 set-up 0x800F5D58: priority 3; SetAnime(19)
var 6 run 0x800F5E0C: if boss_dead or parent.(step, step2) != (5, 5): state 2
    flip, pos = parent; AnimeTick; MoveObject2; DisplayCheck(32, 32)
state 2 of each: DeleteObject3
```

## Engine helpers called

| address | name |
|---|---|
| `0x80016C48` | `PlaySound` |
| `0x800179A4` | `SetAnime` |
| `0x80017A04` | `AnimeTick` |
| `0x80021760` | `TextOpen` |
| `0x80025FD8` | `BossLevel` (new) |
| `0x8002A47C` | `GetSpriteBank` |
| `0x8002BBF4` | `GetMainObject` |
| `0x8002C3F8` | `GetSysObject` |
| `0x8002C458` | `GetVisualObject` |
| `0x8002C9B0` | `DeleteObject3` |
| `0x8002CCB0` | `DisplayCheck` |
| `0x8002D230` | `MoveObject2` |
| `0x8002D2D4` | `GetRNG` |
| `0x8002EE10` | `CornerCheck` |
| `0x8002F288` | `CollideCheck` |
| `0x80030ECC` | `ContactMega` |
| `0x80031474` | `ContactWeapon` |
| `0x8003D308` | `PlayerEvent` |
| `0x8003D330` | `PlayerEventEnd` |
| `0x80040290` | `HitReactionDispatch` |
| `0x80040488` | `HpBarShow` |
| `0x80040498` | `HpBarBusy` |
| `0x800477E4` | `SpawnJunk` |
| `0x80049E50` | `SpawnExplosion` |
| `0x80065944`, `0x80065A14`, `0x80065B74` | `rsin`, `rcos`, `ratan2` |

## Object struct (the fields used here)

| offset | boss | drone | visual 15 |
|---|---|---|---|
| `+0x07` | `act4`: swing (step 6) | `act4`: bomb cooldown | delay (var 3) |
| `+0x17` | current animation | | |
| `+0x7C` | `work.7C`: timer | `work.7C`: timer | parent pointer |
| `+0x7E` | `work.7E`: counter; lance clock | `work.7E`: age counter | |
| `+0x80` | engine object 24/26 | parent (boss) | |
| `+0x84` | invulnerability timer | | terrain test off when ≠ 0 |
| `+0x86`, `+0x87` | pattern, pattern index | | |
| `+0x88` | | slot | |
| `+0x8A` | | angle (u16) / target x | |
| `+0x8C` | | radius / saved x / target y | |
| `+0x8E` | swing direction; wall count; trail index | target y / phase count / trail index | |
