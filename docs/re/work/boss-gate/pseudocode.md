# Boss gate (stage object type 8): pseudo code

Source: kit output in `kit/` (`code.txt`, `data.txt`), plus code read from the executable for the
jump table `0x80011694`, the player routine `0x8003A1C8`, the camera routines `0x80029280` and
`0x80028EBC`. Addresses are main-program RAM addresses. The code is shared by all stages.

Names in `snake_case` are mine. `X`, `Y` = the gate position in px. `cam` = `camera_layer0`
(`0x800971F8`). `game` = `0x800CCED0`.

## Dispatch tables (main-program data)

| table | index | routine |
|---|---|---|
| `0x80078DD4` state | 0 | `gate_init` `0x8005011C` |
| | 1 | `gate_main` `0x80050E58` |
| | 2 | `gate_closed` `0x800510C4` |
| `0x80078DE0` step | 0 | `gate_wait` `0x80050250` |
| | 1 | `gate_start` `0x80050540` |
| | 2 | `gate_open` `0x80050670` |
| | 3 | `gate_pass` `0x80050EA0` |
| | 4 | `gate_close` `0x80050F58` |
| | 5 | `gate_fade` `0x80051018` (only id 31) |
| `0x80011694` | var − 1 (0..50) | the 40 per-id camera cases of `gate_open` |

Step 0 is **not** `HitReactionDispatch` here: stage objects have their own step 0.

## `gate_update` — `0x80050DFC` (stage-object table entry 8)

```
gate_update(self):
    self.prev_x = self.x ; self.prev_y = self.y
    STATE_TABLE[self.state](self)
    DisplayCheck1(self)                       // no off-screen delete
```

## `gate_init` — `0x8005011C` (state 0)

```
gate_init(self):
    self.flags = 0x49 ; self.priority = 6
    tex_cord, sprite_data, clut_cord from GetSpriteBank(0x80)   // sprite bank 0x80 (shared bank)
    self.anime_table = 0x80078DCC
    self.self75 = 1 ; self.state = 1
    self.flip = 0 ; self.latch = 0 ; self.work7C = 0 ; self.work88 = 0
    self.terrain_box = 0x80078DC8            // {0, 0, 16, 32}: 32 x 64 px, centred on (X, Y)
    self.step = 0
    self.layer = mega.layer
    self.var = self.var & 0x3F               // bits 6-7 of the record id are dropped
    SetAnime(self, 0)                        // frame 0 = sprite 58, closed; not ticked yet
```

## `gate_main` — `0x80050E58` (state 1)

```
gate_main(self):
    STEP_TABLE[self.step](self, &game, &mega)
```

## `gate_wait` — `0x80050250` (step 0): closed, wait for the player

```
gate_wait(self, g, m):
    SolidObjectCollide(self)                 // 0x80031DA8: the gate is a solid box
    gate_drop_flags(self)                    // 0x80051104
    switch self.var:                         // the gate stays shut while the test fails
        7, 8, 10, 12, 14, 15: if byte[0x800CCEF6] == 0: return   // bne chain 0x8005027C-0x80050358
        34:                   if byte[0x800CCEF8] == 0: return
        44:                   if byte[0x800CCEF6] != 0: return
    if not gate_touch_test(self): return     // 0x80050C88
    if g.lock(+0x1C) == 0 and self.var in (25, 39):
        PlaySound(5, 1, NULL) ; Rumble(4)
    g.lock(+0x1C) = 1                        // 0x800CCEEC
    m.gate(+0xD4) = 1                        // request: the player enters its gate step
    if self.var == 25:                       // special: lower layer1.s42 to -80 first
        if layer1.s42 >= -79:          // 0x8009728E
            if (word[0x80097420] & 1) == 0: layer1.s42 -= 1      // 1 px every 2 frames
            if self.work88 != 0: self.work88 -= 1 ; return
            Quake2(10, 2, 2) ; self.work88 = 10 ; return     // a quake every 11 frames
        PlaySound(5, 3, ?) ; layer1.b03 = 0            // 0x8009724F
    elif self.var == 39:                     // special: lower layer1.s42 for 112 frames
        if self.work84++ < 112:
            layer1.s42 -= 1
            if self.work88 != 0: self.work88 -= 1 ; return
            Quake2(10, 2, 2) ; self.work88 = 10 ; return
        layer1.b03 = 0 ; PlaySound(5, 3, ?)
    if self.work7C == 0: ClearEnemies()      // 0x8002B79C; work7C is always 0
    if self.var == 7:
        o = GetMainObject()                  // no null test
        o.flags = 0x41 ; o.type = 25 ; o.var = 4
    cam.flags34 &= 0x000F
    self.step = 1
```

## `gate_touch_test` — `0x80050C88`

```
gate_touch_test(self):                       // 1 = the player pushes into the gate from the left
    if mega+0xA4 != 0: return 0
    if mega+0x5C & 0x80: return 0
    if mega.step(+0x05) == 3: return 0
    if mega.var == 0 (X): if mega+0x7A != 0: return 0
    else (Zero):          if mega+0x8E != 0: return 0
    if X < mega.x: return 0                  // the player must be at or left of the gate centre
    gb = self.terrain_box ; pb = mega.terrain_box
    if |(X + gb.ox) - (mega.x + pb.ox)| > gb.w + pb.w: return 0      // not touching
    if mega.y + pb.oy - pb.h < Y + gb.oy - gb.h: return 0            // player top above gate top
    return mega.y + pb.oy + pb.h <= Y + gb.oy + gb.h                // player bottom not below gate bottom
```

## `gate_drop_flags` — `0x80051104`

```
gate_drop_flags(self):
    if self.touch72 & 4: mega+0x71 &= 0x0B   // clear the "ceiling" bit the gate gave the player
    if self.touch72 & 8: mega+0x71 &= 0x07   // clear the "floor" bit the gate gave the player
```

## `gate_start` — `0x80050540` (step 1): wait for the player, then start

```
gate_start(self, g, m):
    if m.gate(+0xD4) >= 0: goto end          // the player routine 0x8003A1C8 sets -1
    m.gate = -2                              // "passing"; the camera skips its event handlers
    g+0x10 = 1 ; g+0x12 = 1                  // 0x800CCEE0, 0x800CCEE2
    m.terrain_box = NULL                     // the player has no terrain collision
    self.work80 = cam.speed47 ; cam.speed47 = 2      // limit easing 2 px/frame
    if self.var == 31:
        f_8002B2F8(0)
        byte[0x80090D56] = 1 ; byte[0x8009742C] = 1 ; byte[0x800CF850] = 1
        word[0x80090C98] = 15 ; word[0x80090C9C .. 0x80090CC8] = -1 (12 words)
        byte[0x800C4560] |= 1
        self.sA0 = 2 ; cam.flags34 &= 0xF ; self.step = 5 ; return
    self.step = 2
    PlaySound(0, 16, self)
end:
    cam.flags34 &= 0x000F
```

## `gate_open` — `0x80050670` (step 2): opening animation, then the new camera limits

```
gate_open(self, g, m):
    if self.anime.flag < 2:
        AnimeTick(self)
        if self.anime.flag == 1:             // frame 27 (sprite 68), after 67 updates
            PlaySound(0, 17, self) ; self.anime.flag = 0
        cam.flags34 &= 0xF ; return
    // frame 29 (sprite 70, open) reached, after 75 updates
    g+0x10 = 0 ; g+0x12 = 0
    cam.min_x(+0x26) = X ; cam.max_x(+0x24) = X
    self.work84 = 0xA400                     // push speed 0.640625 px/frame
    switch self.var (table 0x80011694): see the per-id table in the sheet
        // writes cam.max_x (+0x24), cam.min_x (+0x26), cam.max_y (+0x28), cam.min_y (+0x2A),
        // self.work84, and for some ids:  if word[game+0x43C] == 0: word[game+0x43C] = word[game+0x90]   // 32-bit
    self.step = 3
    cam.flags34 &= 0x000F
```

## `gate_pass` — `0x80050EA0` (step 3): move the player in until the camera arrives

```
gate_pass(self, g, m):
    m.x += self.work84                       // the gate moves the player to the right
    if cam.x(+0x0A) != cam.min_x(+0x26): return
    m.gate(+0xD4) = 0
    PlayerSetTerrainBox(m)                   // 0x8003CCBC: X 0x80073D50, Zero 0x80073D54
    self.step = 4
    SetAnime(self, 1)                        // close
    PlaySound(0, 17, self)
    if self.var == 18: byte[0x8008EAB0] = 1 ; cam.min_x = 3440
    cam.speed47 = self.work80
    cam.flags34 &= 0x000F
```

## `gate_close` — `0x80050F58` (step 4)

```
gate_close(self, g, m):
    if self.anime.flag < 2:
        AnimeTick(self)
        if self.anime.flag == 1:             // frame 2 (sprite 68), after 8 updates
            PlaySound(0, 16, self) ; self.anime.flag = 0
    else:                                    // frame 29 (sprite 58), after 76 updates
        self.state = 2 ; self.step = 0
        if mega+0xD0 == 0 and mega+0xD4 == 0 and mega+0xE0 == 0:
            g.lock(+0x1C) = 0                // 0x800CCEEC
    cam.flags34 &= 0x000F
```

## `gate_fade` — `0x80051018` (step 5, id 31 only)

```
gate_fade(self):
    self.sA0 -= 1
    if self.sA0 != -1: return                // one tick every 3 frames
    self.sA0 = 2
    hword[0x800CF854] += 1 ; hword[0x80097430] += 32 ; hword[0x80090D5C] += 0x400
    byte[0x800C4560] |= 1
    if hword[0x800CF854] >= 31:
        PlaySound(0, 16, self) ; self.step = 2           // then the normal opening
```

## `gate_closed` — `0x800510C4` (state 2): shut for good

```
gate_closed(self):
    if self.var == 20: return                // id 20: no collision after the pass
    SolidObjectCollide(self) ; gate_drop_flags(self)
```

## Player side: `PlayerGateEnter` — `0x8003A1C8` (in the player code)

```
if mega.gate(+0xD4) > 0 and mega+0xA4 <= 0:
    f_8003CDBC(mega)
    if mega+0x17 in (5, 6, 12): f_8003BA04(mega, 38)
    mega.flip = 0x40                         // face right
    mega.gate = -1 ; mega+0x8C = 0
    mega.step = 19 ; mega.step2 = 0          // the player's gate step (not read)
```

## Camera side

- `0x80029280`… (camera update): after the camera state routine, **when `mega+0xD4` ≠ −2**, it runs
  the handlers for `cam+0x34` bit `0x10` (`0x80029A6C`) and bit `0x01` (`0x80029AFC`). Then always
  `CameraLimitEase` (`0x80028EBC`).
- `CameraLimitEase`: moves each current limit (`+0x1C` max x, `+0x1E` min x, `+0x20` max y,
  `+0x22` min y) toward its target (`+0x24`, `+0x26`, `+0x28`, `+0x2A`) by at most `cam+0x47`
  px/frame; for the min x it uses `cam.x + speed`, so the view is pushed right at that speed.

## Engine helpers called

| address | name | use here |
|---|---|---|
| `0x80031DA8` | `SolidObjectCollide` (new) | the gate box against the player (and the object at `0x800CD340`): push out, touch flags, carry |
| `0x80032208` | `SolidPushOut` (new) | pushes the target out of the object's terrain box on the shorter axis |
| `0x8002E1A0` | `SolidTouchFlags` (new) | sets the touch bits (1, 2 walls; 4, 8 top/bottom) in `obj+0x72..0x74` and `target+0x71` |
| `0x800324C8` | `SolidCarry` (new, guess) | moves a target that stands on the object by the object's move |
| `0x8002DCF4`, `0x8002DE54` | terrain-box overlap tests (new) | used by `SolidPushOut` |
| `0x8002DFA0`, `0x8002E0DC` | push distance x / y (new) | write `target+0x6C` / `+0x6E` |
| `0x8002B79C` | `ClearEnemies` (new) | clears every main object except type 47 (items) and all 32 visual objects |
| `0x8003CCBC` | `PlayerSetTerrainBox` (new) | the player's terrain box by character, or none when `+0xD5` < 0 |
| `0x8002B2F8` | (id 31 only, not named) | clears 16 words `0x80090C90`..`0x80090CCC` and the halfwords `0x80090D5C`, `0x80097430`, `0x800CF854`; sets `0x800C4560` bit 0 |
| `0x8002943C` | `Quake2` | ids 25, 39 |
| `0x80012D9C` | `Rumble` | ids 25, 39 |
| `0x8002BBF4` | `GetMainObject` | id 7 |
| `0x80016C48` | `PlaySound` | bank 0 ids 16, 17; bank 5 ids 1, 3 |
| `0x800179A4`, `0x80017A04` | `SetAnime`, `AnimeTick` | |
| `0x8002A47C` | `GetSpriteBank` | bank `0x80` |
| `0x8002CB50` | `DisplayCheck1` | every update |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x00` | flags | `0x49` |
| `+0x02` | var | record id & `0x3F`: selects the camera case |
| `+0x04`, `+0x05` | state, step | |
| `+0x08`, `+0x0C` | x, y | |
| `+0x14` | layer | `mega.layer` |
| `+0x15` | flip | 0 |
| `+0x16` | priority | 6 |
| `+0x18`, `+0x1C` | prev_x, prev_y | |
| `+0x30` | anime_table | `0x80078DCC` |
| `+0x3C`, `+0x40`, `+0x42` | sprite_data, tex_cord, clut_cord | from bank `0x80` |
| `+0x45` | anime.flag | 1 = sound, 2 = done |
| `+0x67` | latch | 0 |
| `+0x68` | terrain_box | `0x80078DC8` |
| `+0x72`..`+0x78` | touch bits / ride counters | written by `SolidTouchFlags` |
| `+0x75` | (wall kind) | 1; copied to `mega+0x4A` on a side touch [guess] |
| `+0x7C` | work.7C | 0; `ClearEnemies` runs when it is 0 |
| `+0x80` | work.80 | saved `cam+0x47` |
| `+0x84` | work.84 | push speed (16.16); frame counter for id 39 |
| `+0x88` | work.88 | quake timer (ids 25, 39) |
| `+0xA0` | (s16) | fade tick timer (id 31) |

## Globals and player fields

| address | name (proposed) | read / write |
|---|---|---|
| `0x800971F8+0x0A` | `cam.x` | read (step 3) |
| `0x800971F8+0x24` / `+0x26` | `cam.max_x_dst` / `cam.min_x_dst` | write |
| `0x800971F8+0x28` / `+0x2A` | `cam.max_y_dst` / `cam.min_y_dst` | write |
| `0x800971F8+0x34` | `cam.event_flags` | `&= 0xF` every gate update |
| `0x800971F8+0x47` | `cam.limit_speed` | 2 during the pass, then restored |
| `0x8009724F` (layer 1 `+0x03`) | (byte) | 0 (ids 25, 39) |
| `0x8009728E` (layer 1 `+0x42`) | (s16) | decremented (ids 25, 39) |
| `0x800CCEE0` / `0x800CCEE2` | `game+0x10` / `game+0x12` | 1 from the start to the fully open gate |
| `0x800CCEEC` | `game.player_lock` | 1 from the trigger to the closed gate |
| `0x800CCEF6` / `0x800CCEF8` | `game+0x26` / `game+0x28` | read: open conditions of some ids |
| `0x800CCF60` | `game.stage_time` | read (copied to the stamp) |
| `0x800CD30C` | `game.boss_gate_time` | write once (some ids) |
| `0x8008EAB0` | (byte) | 1 (id 18) |
| `mega+0x68` | terrain box | NULL during the pass, then restored |
| `mega+0x08` | x | `+= work84` every frame of step 3 |
| `mega+0xD4` | `mega.gate` | 1 request, −1 ready (player), −2 passing, 0 done |
| `mega+0x71` | touch bits | bits 4 / 8 cleared |
| `mega+0x05`, `+0x5C`, `+0x7A`, `+0x8E`, `+0xA4` | | read by the trigger test |
| `mega+0xD0`, `+0xE0` | | read before `player_lock` is cleared |
