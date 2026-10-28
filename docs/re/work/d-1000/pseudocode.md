# D-1000 (body and core) and its projectiles: pseudo code

Addresses: RAM, `SLUS-01395` with overlay `02` at `0x800E9860`. Field names as in
`../junkroid/pseudocode.md`; `self.wNN` = byte at `+0xNN`. `GAME` = `0x800CCED0`. `mega` = the
player object at `0x800970A0`; `mega.var` (`+0x02`) is 0 for X and non-zero for Zero.

## Tables

| address | contents |
|---|---|
| `0x800FA424` + 48·4 | main table entry 48 = `d1000_update` `0x800EFED0` |
| `0x800FA558` + 1·4 | visual table entry 1 = `d1000_fx_update` `0x800EF2EC` |
| `0x800FB198` | body states: init `0x800EFF4C`, main `0x800EDD60`, dead `0x800F0F10` |
| `0x800FB1A4` | body steps 0–10: engine `0x80040290`, nop `0x800F0058`, ENTRY `0x800F0060`, nop-step `0x800F0380`, THINK `0x800EE188`, WALK `0x800F03EC`, CROUCH `0x800F04F4`, BOMB `0x800F0730`, SUMMON `0x800F0820`, RAM `0x800F0934`, STUN `0x800F0C0C` |
| `0x800FB1D0` | ENTRY sub-steps: `0x800F009C`, `0x800F0134`, `0x800EDED4`, `0x800EDFD4`, `0x800F01E0`, `0x800F0294` |
| `0x800FB1E8` | step 3 sub-steps: `0x800F03E4` (empty) |
| `0x800FB1EC` | WALK: `0x800F0450`, `0x800EE2C4` |
| `0x800FB1F4` | CROUCH: `0x800F0558`, `0x800F05C0`, `0x800F065C`, `0x800EE390`, `0x800F06B0` |
| `0x800FB208` | BOMB: `0x800F0794`, `0x800EE468` |
| `0x800FB210` | SUMMON: `0x800F0884`, `0x800F08E8` |
| `0x800FB218` | wall templates `{x, y, w, h}`: `{6512, 208, 1, 13}`, `{6528, …}`, `{6560, …}`, `{6576, …}` |
| `0x800FB238` | RAM: `0x800F0998`, `0x800EE58C`, `0x800F09FC`, `0x800F0B14`, `0x800F0BDC` |
| `0x800FB24C` | STUN: `0x800F0C88`, `0x800F0D58`, `0x800F0DCC`, `0x800EE6D0`, `0x800F0E78` |
| `0x800FB260` | DEAD steps: `0x800EE7F4`, `0x800F0F4C`, `0x800EE904`, `0x800F104C`, `0x800F117C`, `0x800F11AC`, `0x800F11FC`, `0x800F12FC` |
| `0x800FB280` | core states: init `0x800EEB48`, main `0x800EECAC`, delete `0x800F13C8` |
| `0x800FB2A0` … `0x800FB2E4` | visual `var` 0–5 state tables, 3 entries each (init, main, delete) |
| `0x800E9A30` | jump table: visual `var` → its state table |
| `0x800FAF08` | body terrain box `{0, -24, 64, 48}` |
| `0x800FAF0C`, `0x800FAF10` | body contact and hurt box, both `{-56, -64, 112, 92}` |
| `0x800FAF14` | core hurt box `{-10, -10, 20, 20}` |
| `0x800FB28C` | bomb terrain box `{0, 0, 1, 16}` |
| `0x800FB290` | fire pillar contact box `{-16, -128, 32, 255}` |
| `0x800FB294` | core shot terrain box `{0, 0, 8, 8}` |
| `0x800FB298`, `0x800FB29C` | core shot contact and hurt box `{-8, -8, 16, 16}` |
| `0x80074B5C` | body damage table (lemon and half charge deflected; see the sheet) |
| `0x8007429C` | core shot damage table (127 for every weapon) |

Globals: `0x800FBC70` = boss dead; `0x800FBC74` = cleared at the stun landing (the visual objects
remove themselves while it is set); `0x800FBC78` = pointer to the body; `GAME + 0x20` = boss
pointer, `GAME + 0x8C` (`0x800CCF5C`) = HP bar maximum (32).

## Work bytes

Body: `+0x07` = signal from the core; `+0x7C` = timer (s16); `+0x80` = pointer to a helper object;
`+0x84` = invincibility counter; `+0x88` = "touched the player" (OR of `ContactMega` results).
Core: `+0x20`/`+0x24` = target point (the body's head, not a speed); `+0x7C` = timer;
`+0x7E` = particle counter; `+0x80` = pointer to the body.

## Engine helpers used (new here)

| address | name | what it does |
|---|---|---|
| `0x8002A47C` | `GetSpriteBank(type)` | index of the sprite set for an object type; the code then sets `sprite_data = bank_base + bank[i]` (`*0x1F800020`) |
| `0x8002BBF4` | `GetMainObject` | free main object slot |
| `0x8002C3F8` | `GetSysObject` | free slot for engine objects 24 and 26 [guess for the name] |
| `0x80012D9C` | `Rumble(n)` | controller vibration [likely] |
| `0x8003D308`, `0x8003D330` | player event | `PlayerEvent(step, facing)`; `PlayerEventEnd()` (see `docs/engine.md`, "The player") |
| `0x80040488`, `0x80040498` | HP bar | show the bar; "bar busy" [likely] |
| `0x8004A768` | rock fall | falling rock effects in an x range [guess] |
| `0x80065B74`, `0x80065A14`, `0x80065944` | `ratan2`, `rcos`, `rsin` | Psy-Q GTE library (4096 = 1.0) |

## Body

```
d1000_update(self):                     // 0x800EFED0
    if self.var == 1: core_update(self) ; return        // 0x800F1388
    self.prev = self.pos ; self.display = 0
    body_state[self.state](self)
    CollideCheck(self)

body_init(self):                        // 0x800EFF4C
    self.tpage = 480 ; self.anime_table = 0x800FB138 ; self.priority = 5
    self.attack_box = 0x800FAF0C ; self.hurt_box = 0x800FAF10
    self.damage_table = 0x80074B5C
    self.x.hi = 6288 ; self.y.hi = 160 ; self.contact_damage = 6
    self.flip = LEFT ; terrain box = NULL ; all speeds 0 ; self.hp = 0
    self.layer = player.layer
    GAME.w25 = 0 ; GAME.boss = self ; GAME.hp_max = 32
    0x80025FD8() ; music(20, 64) ; self.w84 = 0
    boss_dead = 0 ; flag_BC74 = 0 ; boss_ptr = self
    self.step = ENTRY ; self.state = MAIN

body_main(self):                        // 0x800EDD60
    r = ContactWeapon(self)
    if r < 0:                                           // killed by a direct hit
        self.state = DEAD ; self.step = self.step2 = 0 ; clear flash
        boss_dead = 1 ; mega.no_hit = 1
        DisplayCheck(self, 100, 100) ; return
    self.w88 |= ContactMega(self)
    if r > 0 and r != 127:                              // hit
        self.w84 = (r in (25, 26)) ? 2 : 30             // weapon ids 24, 25: 2 frames
        self.damage_table = 0x8007433C                  // guard
    if self.step == 99:                                 // set by the core
        self.w84 = 0 ; self.attack_box = NULL ; self.damage_table = 0x8007433C
        self.step = STUN ; self.step2 = 0
    if self.w84:
        self.w84 -= 1
        flash bit = (self.w84 & 8) ? on : off
        if self.w84 == 0: self.damage_table = 0x80074B5C ; flash off
    body_step[self.step](self)
```

Most steps end with `AnimeTick`, `MoveObject2` and `DisplayCheck(self, 160, 160)`.

### ENTRY (step 2)

```
entry_wait_player(self):                // 0x800F009C
    if mega.step != 20: return
    mega.w19F = 2 ; music(29, 64) ; self.timer = 40
    o = GetSysObject() ; if o: o.flags = 1 ; o.type = 24 ; o.pos = self.pos ; self.w80 = o
    self.step2 += 1

entry_wait_warning(self):               // 0x800F013C
    if GAME.w0F != 0: return
    if --self.timer == 0: music(20, 64)
    if self.w80.flags == 0:                             // the type-24 object is gone
        self.gravity = 0x6000 ; SetAnime(self, 7) ; self.step2 += 1

entry_fall(self):                       // 0x800EDED4
    if self.terrain_box == NULL and self.y.hi >= 288: self.terrain_box = 0x800FAF08
    if self.collide_flags & GROUND:
        PlaySound(2, 5, self) ; Rumble(3) ; speed_y = gravity = 0
        SetAnime(self, 8) ; Quake(20, 4, 2) ; self.timer = 0 ; self.w07 = 0 ; self.step2 += 1
    self.speed_y -= self.gravity ; AnimeTick(self) ; MoveObject(self)

entry_land(self):                       // 0x800EDFD4
    self.timer += 1
    if self.timer == 20: Quake(20, 2, 2)
    if self.timer < 30 and (self.timer & 3) == 0: RockFall(32, x - 100, 400, x + 100, 416, 8)
    if self.timer == 120:
        core = GetMainObject() ; if core: core.flags = self.flags ; core.type = 48 ; core.var = 1 ; core.w80 = self
    if self.w07 == 1: SetAnime(self, 9) ; self.w07 = 2         // stand-up pose
    elif self.w07 >= 2 and self.anime.next == 0:
        fx = GetVisualObject() ; if fx: fx.type = 1 ; fx.var = 4 ; fx.parent = self
        PlaySound(2, 4, self) ; self.speed_y = +0x10000 ; self.step2 += 1

entry_rise(self):                       // 0x800F01E0
    if self.y.hi <= 304:
        self.y.hi = 304 ; speed_y = 0 ; SetAnime(self, 0)
        ShowHpBar(0) ; GAME.w24 = 1 ; self.timer = 3 ; self.step2 += 1

entry_fill_hp(self):                    // 0x800F0294
    if HpBarBusy(): return
    if self.hp < GAME.hp_max:
        if --self.timer == 0: PlaySound(0, 14, 0) ; self.timer = 3
        self.hp += 1
    else:
        BossMusic() ; self.timer = 0 ; self.step = THINK ; self.step2 = 0
```

Note: in `entry_fill_hp` the `hp += 1` runs every frame and the sound every 3 frames, so the bar
fills in 32 frames.

### THINK (step 4)

```
think(self):                            // 0x800EE188
    r = GetRNG() & 15
    face the player
    self.timer += 1
    if self.timer == 60:
        healthy = self.hp > GAME.hp_max / 2
        if self.step2 == 0:  next = healthy ? (r < 8 ? RAM : SUMMON) : (r < 6 ? RAM : BOMB)
        else:                next = healthy ? (r < 8 ? WALK : SUMMON) : (r < 6 ? SUMMON : BOMB)
        self.step = next ; self.step2 = 0
    AnimeTick ; MoveObject2 ; DisplayCheck(160, 160)
```

`step2` is 1 only when THINK is entered from CROUCH.

### WALK (step 5)

```
walk_start(self):                       // 0x800F0450
    face the player ; self.speed_x = self.flip ? +0x10000 : -0x10000
    SetAnime(self, 1) ; self.timer = 0 ; self.step2 += 1

walk(self):                             // 0x800EE2C4
    if abs(player.x.hi - self.x.hi) <= 60:
        self.speed_x = 0 ; self.step = CROUCH ; self.step2 = 0 ; return
    if ++self.timer == 120:
        SetAnime(self, 0) ; self.speed_x = 0 ; self.timer = 0
        self.step = THINK ; self.step2 = 0
```

### CROUCH (step 6)

```
crouch_start(self):                     // 0x800F0558
    self.gravity = 0x8000 ; SetAnime(self, 2) ; self.w88 = 0 ; self.step2 += 1

crouch_drop(self):                      // 0x800F05C0
    if self.y.hi >= 352:
        self.y.hi = 352 ; PlaySound(2, 1, self) ; Rumble(3)
        self.speed_y = +0x20000 ; self.gravity = 0 ; SetAnime(self, 3)
        self.timer = 0 ; self.step2 += 1

crouch_shake(self):                     // 0x800F065C
    if self.anime.next == 0: self.step2 += 1
    self.timer += 1
    if self.timer & 1 == 0: self.speed_y = -self.speed_y

crouch_hold(self):                      // 0x800EE390
    self.timer += 1
    if self.timer == 90 or self.w88:
        SetAnime(self, 1) ; self.speed_y = +0x10000 ; self.step2 += 1
    elif self.timer == 60: self.speed_y = 0
    elif self.timer < 60 and self.timer & 1 == 0: self.speed_y = -self.speed_y

crouch_rise(self):                      // 0x800F06B0
    if self.y.hi <= 304:
        self.y.hi = 304 ; speed_y = 0 ; SetAnime(self, 0)
        self.timer = 0 ; self.step = THINK ; self.step2 = 1
```

### BOMB (step 7)

```
bomb_start(self):                       // 0x800F0794
    face the player ; SetAnime(self, 5) ; self.timer = 0 ; self.step2 += 1

bomb(self):                             // 0x800EE468
    face the player ; self.timer += 1
    if difficulty == 2:
        if self.timer in (24, 84): PlaySound(2, 6, self) ; spawn visual type 1 var 1
        end = 220
    else:
        if self.timer == 24: PlaySound(2, 6, self) ; spawn visual type 1 var 1
        end = 160
    if self.timer == end: self.step = WALK ; self.step2 = 0
```

### SUMMON (step 8)

```
summon_start(self):                     // 0x800F0884
    SetAnime(self, 0) ; self.w07 = 0 ; self.step2 += 1

summon_wait(self):                      // 0x800F08E8
    face the player
    if self.w07: self.step = WALK ; self.step2 = 0
```

### RAM (step 9)

```
ram_start(self):                        // 0x800F0998
    SetAnime(self, 1) ; self.timer = 0 ; self.step2 += 1

ram_windup(self):                       // 0x800EE58C
    face the player
    if ++self.timer == 180:
        SetAnime(self, 4) ; PlaySound(2, 4, self) ; self.accel_x = 0x10000 ; self.step2 += 1
    move self.y.hi 2 px toward player.y.hi (no overshoot)

ram_charge(self):                       // 0x800F09FC
    if self.speed_x > 4 px or self.speed_x < -4 px: self.speed_x = ±4 px ; self.accel_x = 0
    hit = self.flip ? (collide_flags & 1) : (collide_flags & 2)
    if hit:
        PlaySound(2, 5, self) ; Rumble(3)
        if collide_flags & 2: CopyTiles(0, 6064, 208, wall_dented_left)    // 0x800FB220
        else:                 CopyTiles(0, 6448, 208, wall_dented_right)   // 0x800FB230
        layer0.redraw = 1 ; speed_x = accel_x = 0
        Quake(-106, 4, 2) ; self.timer = 0 ; self.step2 += 1

ram_stuck(self):                        // 0x800F0B14
    if ++self.timer == 180:
        CopyTiles(0, 6064, 208, wall_left) ; CopyTiles(0, 6448, 208, wall_right)
        layer0.redraw = 1 ; SetAnime(self, 1) ; self.speed_y = +0x10000 ; self.step2 += 1

ram_rise(self):                         // 0x800F0BDC
    if self.y.hi <= 304: self.y.hi = 304 ; speed_y = 0 ; self.step = WALK ; self.step2 = 0
```

### STUN (step 10)

```
stun(self):                             // 0x800F0C0C
    if self.w84 < 50: self.w84 = 100        // keeps the guard table and the flash running
    stun_sub[self.step2](self)
    AnimeTick ; MoveObject2 ; DisplayCheck(160, 160)

stun_start(self):                       // 0x800F0C88
    CopyTiles(0, 6064, 208, wall_left) ; CopyTiles(0, 6448, 208, wall_right) ; layer0.redraw = 1
    all speeds 0 ; SetAnime(self, 6) ; PlaySound(2, 2, self) ; self.step2 += 1

stun_stagger(self):                     // 0x800F0D58
    if self.anime.next == 0: self.gravity = 0x6000 ; SetAnime(self, 7) ; self.step2 += 1

stun_fall(self):                        // 0x800F0DCC
    if self.collide_flags & GROUND:
        PlaySound(2, 5, self) ; Rumble(3) ; flag_BC74 = 0
        speed_y = gravity = 0 ; SetAnime(self, 8) ; Quake(60, 4, 2)
        self.timer = 0 ; self.w07 = 0 ; self.step2 += 1

stun_down(self):                        // 0x800EE6D0
    if ++self.timer == 65:
        self.hp -= 8
        if self.hp <= 0:
            self.state = DEAD ; self.hp = 0 ; self.step = self.step2 = 0
            boss_dead = 1 ; mega.no_hit = 1 ; DisplayCheck(160, 160) ; return
    if self.w07 == 1: SetAnime(self, 9) ; self.w07 = 2
    elif self.w07 >= 2 and self.anime.next == 0:
        PlaySound(2, 4, self) ; self.speed_y = +0x10000 ; self.step2 += 1

stun_rise(self):                        // 0x800F0E78
    if self.y.hi <= 304:
        self.y.hi = 304 ; self.attack_box = 0x800FAF0C ; self.damage_table = 0x80074B5C
        self.w84 = 0 ; speed_y = 0 ; SetAnime(self, 0)
        self.timer = 0 ; self.step = THINK ; self.step2 = 0
```

### DEAD (state 2, outline)

```
dead(self):                             // 0x800F0F10: dead_step[self.step](self)
dead_0:  // 0x800EE7F4: restore both walls; layer0.redraw = 1; timer = 127; blink = 25; w61 = 25;
         // flash off; SetAnime 0; speed_y = -0x4000 (sinks 0.25 px/frame); music fade by side; step++
dead_1:  // 0x800F0F4C: at timer 0 make engine object 26 (flags 0x81) at the body; step++.
         // Every frame: blink: when the blink counter passes -1, toggle the flash bit and shorten
         // the period by 5 (w61 -= 5, at < 26 it goes to 0; period = max(w61, 5))
dead_2:  // 0x800EE904: count while object 26 is in state >= 4; sink after 30 frames; when object
         // 26 sets its +0x07: SetAnime 11, step++, move the player 160 px to the side, adjust the
         // camera limits, move the body to camera y + 160, clear GAME.boss
dead_3:  // 0x800F104C: when object 26 is gone: misc object 16 (debris) with a shifted palette; step++
dead_4:  // 0x800F117C: idle
dead_5:  // 0x800F11AC: timer = 127; blink = 25; w61 = 25; step++
dead_6:  // 0x800F11FC: as dead_1, with engine object 26 flags 1
dead_7:  // 0x800F12FC: blink while object 26 runs; DeleteObject3 when it is gone
```

## Core (`var` 1)

```
core_update(self):                      // 0x800F1388
    self.display = 0 ; core_state[self.state](self)

core_init(self):                        // 0x800EEB48
    copy tpage, anime_table, clut (no flash) from the body (self.w80)
    self.priority = 3 ; self.hurt_box = 0x800FAF14 ; self.damage_table = 0x80074B5C
    self.attack_box = NULL ; terrain box = NULL
    self.x.hi = 6368 ; self.y.hi = 256 ; self.layer = player.layer
    self.flip = (player.x.hi > 6368) ? RIGHT : LEFT
    all speeds 0 ; self.hp = 0 ; self.contact_damage = 0 ; self.w84 = 0
    SetAnime(self, 15) ; PlaySound(2, 9, self)
    fx = GetVisualObject() ; if fx: fx.type = 1 ; fx.var = 5 ; fx.parent = self
    self.w7E = 0 ; self.step = self.step2 = 0 ; self.state += 1

core_main(self):                        // 0x800EECAC
    if boss_dead: PlaySound(2, 20, self) ; self.state = 2 ; return
    body = self.w80
    self.target_x(+0x20) = body.x.hi + (body.flip ? +62 : -62)
    self.target_y(+0x24) = body.y.hi - 60
    self.flip = (self.x.hi < self.target_x) ? RIGHT : LEFT
    self.w7E += 1
    particle = (self.w7E % 6 == 0)

    step 0 (appear):
      sub 0: if particle: visual var 0
             if self.anime.next == 0: SetAnime(self, 12) ; PlaySound(2, 7, self) ; self.timer = 0 ; sub 1
      sub 1: if particle: visual var 0
             if ++self.timer == 100: body.w07 = 1 ; sub 2
      sub 2: if particle: visual var 0
             if body.step == THINK: self.step = 1 ; self.step2 = 0
      AnimeTick ; DisplayCheck(32, 32)

    step 1 (active):
      if particle: visual var 0
      sub 0: if body.step == SUMMON: self.timer = 0 ; sub 1
      sub 1: if mega.var == 0 (X): shots at timer 0, 8, 16, 24 ; signal at 124
             else (Zero):          shots at timer 0, 60, 120 ; signal at 220
             shot = visual var 2 + PlaySound(2, 8, self)
             signal = body.w07 = 1 ; sub 0
             self.timer += 1
      self.hp = 127
      r = ContactWeapon(self) ; flash off
      weak = (mega.var == 0) ? (r - 19 in 0..3 or r in (51, 52)) : (r > 0)
      if weak:
          body.step = 99
          SetAnime(self, 16) ; PlaySound(2, 9, self) ; PlaySound(2, 20, self)
          self.step = 2 ; self.step2 = 0
      AnimeTick ; DisplayCheck(32, 32)

    step 2 (hit, then move):
      sub 0: if self.anime.next == 0: self.timer = 0 ; sub 1
      sub 1: if ++self.timer == 60:
                 self.x.hi = (player.x.hi < 6264) ? 6368 : 6160 ; self.y.hi = 256
                 face the player ; SetAnime(self, 15) ; PlaySound(2, 9, self)
                 self.step = self.step2 = 0

core_delete(self):                      // 0x800F13C8
    DeleteObject3(self)
```

## Visual object type 1

```
d1000_fx_update(self):                  // 0x800EF2EC
    if self.w84 == 0 and self.collide_flags == 0: CornerCheck(self)
    self.display = 0
    fx_state[self.var][self.state](self)                // jump table 0x800E9A30
```

All variants copy `tpage`, `anime_table` and `clut` from their parent (`+0x7C`) or from the body
(`0x800FBC78`), and remove themselves when `boss_dead` or `flag_BC74` is set (except as noted).

| var | made by | init | main |
|---|---|---|---|
| 0 | core, every 6 frames | `0x800EF3F8`: at core ±10 px, anim 18, start pos saved in `+0x28/+0x2C`, speed 0 | `0x800EF528`: speed += 8 each frame; moves from the start toward the core's target point along `ratan2`; removed when it reaches the target or after 100 frames. No boxes. |
| 1 | body, BOMB | `0x800EF6B8`: at body ±56, −24; anim 19; terrain box `0x800FB28C`; gravity `0x4000`; contact 6; `speed_x = (player.x.hi − x.hi) << 11` | `0x800EF804`: `ContactMega`. Sub 0: on the ground: PlaySound(2, 0), Rumble(5), stop, y −= 78, anim 20, visual var 3. Sub 1: at animation end: anim 21, contact box `0x800FB290`, counter 0. Sub 2: after 50 frames: anim 22, contact box NULL. Sub 3: at animation end: delete. |
| 2 | core, SUMMON | `0x800EFA5C`: at core ±10; anim 14; boxes `0x800FB294/298/29C`; damage table `0x8007429C`; hp 1, contact 4, priority 2; `a = ratan2(player.y − y, player.x − x)`, `speed_x = rcos(a) << 5`, `speed_y = −rsin(a) << 5` (2 px/frame) | `0x800F1428`: `ContactWeapon` < 0 or `collide_flags` ≠ 0: explosion, delete. `ContactMega`, AnimeTick, MoveObject2, DisplayCheck(16, 16). |
| 3 | bomb | `0x800EFBFC`: at the bomb, y + 92; anim 23; priority 3 | `0x800F14F0`: removed when its parent bomb is deleted |
| 4 | body, end of ENTRY | `0x800EFD00`: anim 10, priority 6 | `0x800F1594`: follows the body at ±8, −8; hidden during STUN sub-steps 0–3 |
| 5 | core, init | `0x800EFDDC`: anim 17, priority 2 | `0x800F167C`: follows the core; shown only while the core shows animation 12 |
