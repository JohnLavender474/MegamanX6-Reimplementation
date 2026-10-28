# Pseudo code: Rescue Reploid (stage object type 4) and its shot (visual type 24)

## Dispatch tables

- States `0x80078C7C`: 0 `reploid_init` 0x8004E800, 1 `reploid_wait` 0x8004EA78, 2 `reploid_heal`
  0x8004F880, 3 `tele_start` 0x8004F8C0, 4 `tele_wait` 0x8004F9A0, 5 `beam_up` 0x8004FAD0,
  6 `infected` 0x8004FB60, 7 `reploid_out` 0x8004FB40, 8 `reploid_gone` 0x8004F8A0.
- Infected steps `0x80078CA0`: 0 HitReactionDispatch, 1 nop 0x8004FE50, 2 `inf_change` 0x8004EFD0,
  3 `inf_arm` 0x8004FC40, 4 `inf_wait` 0x8004FD58, 5 `inf_attack` 0x8004F144.
- Shot states `0x80078CCC`: 0 `shot_init` 0x8004F2EC, 1 `shot_main` 0x8004F624, 2 delete 0x800500C0.

Notation: `side` = `+0x82` (0 = player on the left, 0x40 = right), `hover` = `+0x7E`, `setB` = `+0x84`.
`anim4(a, b, c, d)` = SetAnime by (hover, side): standing L a, standing R b, hovering L c, hovering R d;
set B uses its own pair (L, R).

### entry — 0x8004F834
```
prev = pos; state_tbl[state](self)
```

### reploid_init — 0x8004E800 (state 0)
```
if var != 0x80 and GetReploidStatus(var) in 2..4: state = 8; return
setB = (var & 0xF0) == 0x10
flags = 0x41; dmg_tbl = dmg_tbl_normal; speeds = 0; pri = 2; anime_tbl = 0x80078BEC
layer = mega.layer; tex/sprite/clut from sprite bank 141; flip = 0; prev = pos
face_player(self)
if !setB:
    terrain_box = 0x80078910; CollideCheck(self)
    if collide_flags & FLOOR: hover = 0; anim side ? 11 : 0
    else: hover = 1; base_y = y.px; anim side ? 14 : 5
else:
    terrain_box = 0x8007891C; CollideCheck(self); anim side ? 30 : 23
state = 1; act4 = 1; step = 0
if var == 0x80: act4 = 0
```

### face_player — 0x8004FE8C (returns 1 when the side changed)
```
want = (mega.x >= x) ? 0x40 : 0
changed = (side != want); side = want; return changed
```

### reploid_wait — 0x8004EA78 (state 1)
```
AnimeTick
if face_player(): restart idle anim for the side (0/11, 5/14, set B 23/30)
if rescue_test(): act4 = 0; return
if act4 == 2: SetReploidStatus(var, 4); state = 6; step = 2; step2 = 0; act4 = 0; return
if hover: y.px = base_y + (rsin((frame & 0x7F) << 5) >> 10)
if OffScreenCheck2(48, 48):
    if var != 0x80: display = 0; state = 7; act4 = 0
else: DisplayCheck(setB ? 32 : 16, 32)
```

### rescue_test — 0x8004EECC
```
p = &mega
if mega[0xD5] != 0: p = 0x800CD340; if p[5] == 0: p += 0xB0
if (p.hp & 0x7F) == 0 or !TerrainOverlap(self, p): return 0
if var != 0x80: SetReploidStatus(var, 2)
step = 0
game.lives += 1
if game.lives < 10: PlaySound(0, 21, 0) else game.lives = 9
r = GetRNG() & 3
give_reward(self, heal_tbl[r] /* 2,4,8,16 */, r)
return 1
```

### give_reward — 0x8004ED4C
```
max = game.maxHP[mega.char]
if (mega.hp & 0x7F) < max:
    PauseObjects(1); heal_timer = 4; heal_left = amount; state = 2; step = step2 = 0; flags |= 8
else: state = 3
if !((mega.hp & 0x7F) < max): mega.hp = (mega.hp & 0x80) | max
for i in 0..1:
    if game.tanks & (0x1000 << i):
        lv = game.tank[i] & 0x7F
        if lv < 32:
            lv += r; if lv >= 32: lv = 32; PlaySound(0, 24, 0)
            game.tank[i] = lv | 0x80; break
```

### PauseObjects — 0x8004FE58
```
game[0x10..0x17] = n; game[0x19] = n
```

### reploid_heal — 0x8004F880 → 0x8004EC30 (state 2)
```
AnimeTick
if step2 == 0:
    if --heal_timer != 0: return
    heal_timer = 4
    if (mega.hp & 0x7F) < max: mega.hp += 1 (keep bit 7)
    else: step2 = 1; mega.hp = (mega.hp & 0x80) | max
    PlaySound(0, 13, 0)
    if --heal_left == 0: step2 = 1
else:
    PauseObjects(0); state = 3; step = 0; flags &= ~8
```

### tele_start — 0x8004F8C0 (state 3)
```
face_player(); anim4(9, 17, 10, 18) / set B (25, 32)
terrain_box = 0; state = 4; timer81 = 80
```

### tele_wait — 0x8004F9A0 (state 4)
```
AnimeTick; if face_player(): anim4(9, 17, 10, 18) / (25, 32)
if timer81: timer81--; return
speed_y = 8.0; speed_x = 0; state = 5; SetAnime(setB ? 29 : 8)
```

### beam_up — 0x8004FAD0 (state 5)
```
AnimeTick; if anime.flag == 1: MoveObject
if OffScreenCheck2(64, 64): DeleteObject3 else DisplayCheck(32, 32)
```

### infected — 0x8004FB60 (state 6)
```
AnimeTick; step_tbl[step](self)
r = ContactWeapon(self)
if r < 0:
    SpawnMain(type 15, var 0x31 at self); SetReploidStatus(var, 3)
    display = 0; SpawnExplosion(self); state = 8; return
ContactMega(self)
if OffScreenCheck(self): SetReploidStatus(var, 4); state = 8; return
DisplayCheck(32, 32)
if glow: clut |= 0x8000
```

### inf_change — 0x8004EFD0 (step 2)
```
step2 0: t = 130; anim4(1, 12, 6, 15) / (24, 31); step2 = 1
step2 1: if --t == 0: PlaySound(2, 65, self); glow = 1; t = 60; step2 = 2
step2 2: if --t == 0: glow = 0; step2 = 0; step = 3
```

### inf_arm — 0x8004FC40 (step 3)
```
hp = 13; atk = 6
set A: timer81 = 60; hurt = 0x8007890C; atk_box = 0x80078908; anim4(19, 20, 21, 22)
set B: timer81 = 20; hurt = 0x80078918; atk_box = 0x80078914; anim (28, 35)
step = 4
```

### inf_wait — 0x8004FD58 (step 4)
```
if --timer81 != 0: return
anim4(2, 13, 7, 16) / (26, 33); step = 5
```

### inf_attack — 0x8004F144 (step 5)
```
if anime.flag == 1:
    if mega[0xD4] == 0:
        PlaySound(2, 46, self)
        if !setB: SpawnShot(self, 0); SpawnShot(self, 1) else SpawnShot(self, 16)
elif anime.flag == 2:
    side = (mega.x >= x) ? 0x40 : 0      ; written without the "changed" result
    set A: anim4(19, 20, 21, 22); timer81 = 60
    set B: anim (28, 35); timer81 = 20
    step -= 1
```

### SpawnShot — 0x8004FFF8
```
s = GetVisualObject(); if !s: return
s.flags = 0x41; s.type = 24; s.var = v; s[0x88] = (s8)self.var
s.clut = self.clut & 0x7FFF; s.anime_tbl, sprite, tpage = self's; s.flip = side; s.layer = layer
s.pos = self.pos (whole px); s[0x7C] = self
```

### SpawnMain — 0x8004FF7C
```
m = GetMainObject(); if !m: return
m.flags = 0x41; m.type = 15; m.var = v; m.layer = layer
if (v & 0xF0) == 0x30: m.pos = self.pos (whole px)
```

### shot_init — 0x8004F2EC (visual state 0)
```
ofs = muzzle_tbl[s[0x88] >> 4]          ; {lx, ly, rx, ry}, copied from 0x80011674
atk = 4; hp = 1; dmg_tbl = 0; fall_f = 1; display = 0; boxes = 0; speed_y = 0
var 0: pri = 1; SetAnime(3); pos += flip ? (rx, ry) : (lx, ly)
var 1: pri = 1; SetAnime(4)
       left: speed_x = -4.0; x += lx - 20; y += ly
       right: speed_x = +4.0; x += rx + 20; y += ry
       if game.difficulty >= 2: speed_x *= 2
       s[0x88] = 6
other (16): SetAnime(27); terrain = 0x80078CC0; atk_box = 0x80078CC4; hurt = 0x80078CC8
       x += flip ? 28 : -28; y -= 24
       speed_y = 3.0; accel_x = 0; hp = 2; gravity = 0x1800; atk = 3
       speed_x = ((mega.x - x) << 16) / 70
state = 1; step = step2 = act4 = 0
if OffScreenCheck2(32, 32) == 1: state++
```

### shot_main — 0x8004F624 (visual state 1)
```
AnimeTick; DisplayCheck(16, 16)
var 0: if anime.flag == 1: display = 0; state++ ; return
       goto tail
var 1: step 0: display = 0; if --s[0x88] == 0: atk_box = 0x80078CB8; terrain = 0x80078CBC; display = 1; step = 1
       step 1: MoveObject; if collide_flags or CornerCheck: SpawnHitEffect; display = 0; state++; return
       goto tail
var 16: if ++step2 == 8: 0x8004A2B4(0, 0, self, 1, 0); step2 = 0
       if collide_flags or CornerCheck or ContactWeapon < 0 or ContactMega:
           SpawnExplosion; state = 2; return
       AnimeTick; MoveObject2
       if OffScreenCheck2(16, 16): state++ else DisplayCheck(16, 16)
       return
tail:  ContactMega; if OffScreenCheck2(32, 32) == 1: state++
```

## Engine helpers called

| address | name | use here |
|---|---|---|
| `0x80016C48` | PlaySound | rescue and shot sounds |
| `0x800179A4` / `0x80017A04` | SetAnime / AnimeTick | |
| `0x8002A47C` | GetSpriteBank | bank 141 |
| `0x8002BBF4` | GetMainObject | type 15 on the kill |
| `0x8002C458` | GetVisualObject | the shots |
| `0x8002C9B0` / `0x8002CA54` | DeleteObject3 / DeleteObject2 | states 8 / 7, beam end |
| `0x8002CAB4` / `0x8002CBFC` / `0x8002CCB0` | OffScreenCheck / OffScreenCheck2 / DisplayCheck | |
| `0x8002D2B0` / `0x8002D230` | MoveObject / MoveObject2 | beam, bullet / bomb |
| `0x8002D2D4` | GetRNG | heal index |
| `0x8002DCF4` | TerrainOverlap (new name) | rescue touch |
| `0x8002EE10` | CornerCheck | shots |
| `0x8002F288` | CollideCheck | floor test at spawn |
| `0x80030ECC` / `0x80031474` | ContactMega / ContactWeapon | infected, shots |
| `0x80040290` | HitReactionDispatch | infected step 0 |
| `0x80049D90` / `0x80049E50` | SpawnHitEffect / SpawnExplosion | |
| `0x8004A2B4` | ? | bomb trail [guess] |
| `0x80065944` | rsin | hover |
| `0x8004FF00` / `0x8004FF4C` | SetReploidStatus / GetReploidStatus | |
| `0x8004FE58` | PauseObjects (new name) | heal pause |
| `0x80042D98` | FindReploid (new name; virus code) | |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x00` | flags | `0x41`; bit 3 during the heal |
| `+0x02` | var | Reploid id |
| `+0x04` / `+0x05` / `+0x06` | state / step / step2 | |
| `+0x07` | act4 | 1 catchable, 2 caught, 0 not |
| `+0x50` / `+0x54` / `+0x58` / `+0x5C` / `+0x60` | boxes, dmg table, HP, contact | infected only |
| `+0x68` / `+0x70` | terrain box, collide flags | |
| `+0x7C` (s16) | base y of the hover | |
| `+0x7E` | hover flag | |
| `+0x7F` | heal amount left / change timer | |
| `+0x80` | heal tick timer (4) | |
| `+0x81` | timer (80, 60, 20) | |
| `+0x82` | side (0 / `0x40`) | |
| `+0x83` | white glow on | |
| `+0x84` | set B (id 16–31) | |
| shot `+0x88` (s16) | parent id, then the bullet's 6-update delay | |
| shot `+0x7C` | parent | |
