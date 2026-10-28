# Meta Dridler and its two effects: pseudo code

Addresses: RAM, `SLUS-01395` with overlay `02` at `0x800E9860`. Field names as in
`../junkroid/pseudocode.md`. `self.wNN` = work byte at offset `0xNN`. The scratchpad word
`0x1F80004C` is the current main object (here: `self`); `0x1F800064` is the current misc object (set
at `0x800206E4`, `0x80021108`, `0x80021170`).

## Tables

| address | contents |
|---|---|
| `0x800FA424` + 0 | main table entry 0 = `dridler_update` `0x800EA1D8` |
| `0x800FA814` + 0 | misc table entry 0 = `dridler_fx_update` `0x800EA224` |
| `0x800FAA64` | `dridler_state[3]` = init `0x800EA260`, main `0x800EA370`, delete `0x800EA40C` |
| `0x800FAA70` | `dridler_step[6]` = engine `0x80040290`, nop `0x800EA8D8`, WAIT `0x800EA42C`, DROP `0x800EA52C`, GROUND `0x800EA6B8`, RISE `0x800EA798` |
| `0x800FAA88` | WAIT sub-steps `0x800EA468`, `0x800EA4BC` |
| `0x800FAA90` | DROP sub-steps `0x800EA568`, `0x800EA5D0` |
| `0x800FAAB0` | GROUND sub-steps `0x800EA6F4`, `0x800EA738`, `0x800E9F1C` (rock block), `0x800EA09C` (rock loop) |
| `0x800FAAC0` | RISE sub-steps `0x800EA7D4`, `0x800EA840` |
| `0x800FAAC8` | `fx_state[3]` = init `0x800EA990`, main `0x800EAA40`, delete `0x800EAAD8` |
| `0x800FAAD4` | `fx_step[2]` = `0x800EAAF8` (effect 0), `0x800EABB0` (effect 1) |
| `0x800FAA34` | `dridler_animations[10]` |
| `0x800FA99C` | terrain box `{0, 6, 11, 32}` (centre, half sizes) |
| `0x800FA9A0` | contact box `{-14, -40, 28, 80}` |
| `0x800FA9A4` | hurt box `{-16, -40, 32, 80}` |
| `0x800FAA5C` | debris animations `{7, 7, 7, 8, 8, 9}` |
| `0x800FAA98`, `0x800FAAA0`, `0x800FAAA8` | rock template blocks for id 1, 2, 3: `{x, y, w, h}` = `{4640, 1136, 4, 3}`, `{4864, 1136, 4, 3}`, `{4928, 1136, 4, 3}` |

## Work bytes

| off | name | meaning |
|---|---|---|
| `+0x7C` | `timer` (s16) | countdown |
| `+0x80` | `home_y` (s16) | spawn y, top of the path |
| `+0x82` | `motion` | 0 = on the ground, 1 = waiting or rising, 2 = dropping (read by effect 0) |
| `+0x83` | `warn` | 0 = none, 1 = rise warning, 2 = drop warning (read by effect 1) |

## Engine helpers used (new here)

| address | name | what it does |
|---|---|---|
| `0x80027BA0` | `CopyTiles(layer, dx, dy, src)` | for each of `src.h` rows and `src.w` columns: `0x800262EC(layer, dx + 16i, dy + 16j, src.x + 16i, src.y + 16j)` |
| `0x800262EC` | `CopyCell(layer, dx, dy, sx, sy)` | finds the screen at (sx, sy) and at (dx, dy) in the layout (`*0x1F800004` + layer × `0x8008EC10` + row × width + column), then copies the 16-bit cell word of the source screen to the destination screen (screens are 512 bytes at `*0x1F800008`). It changes the screen itself, so every place that uses that screen changes |
| `0x80029478` | `Quake(a, b, c)` | stores `a`, `b`, `c` in the layer-0 camera work (`0x800971F8` + `0x37`, `0x46`/`0x3F`, `0x3D`/`0x3B`) and sets bit 0 of `+0x34` |
| `0x80049EBC` | `SpawnExplosionRand(obj, type, mx, my)` | explosion effect at `obj.x ± (rand & mx)`, `obj.y ± (rand & my)` |

## Meta Dridler

```
dridler_update(self):                               // 0x800EA1D8
    dridler_state[self.state](self)
    CollideCheck(self)

dridler_init(self):                                 // 0x800EA260
    self.contact_damage = 8 ; self.timer = 0 ; self.hp = 1 ; self.iframes = 0
    self.layer = player.layer ; self.prev = self.pos
    self.home_y = self.y.hi ; self.motion = 1 ; self.warn = 0
    self.anime_table = dridler_animations ; self.priority = 33
    self.terrain_box = {0, 6, 11, 32}
    self.attack_box = {-14, -40, 28, 80} ; self.hurt_box = {-16, -40, 32, 80}
    self.damage_table = 0x8007433C                  // guard table: shots are deflected
    self.flip = LEFT ; self.lock(+0x67) = 1
    SetAnime(self, 0)
    all speeds = 0
    self.state = MAIN ; self.step = WAIT ; self.step2 = 0
    dridler_make_fx(self, 0)
    dridler_make_fx(self, 1)

dridler_main(self):                                 // 0x800EA370
    self.prev = self.pos
    AnimeTick(self)
    dridler_step[self.step](self)
    ContactWeapon(self)                             // result not used: it cannot die
    ContactMega(self)
    if OffScreenCheck2(self, 80, 160): self.state = 2
    else: DisplayCheck(self, 48, 64)

dridler_delete(self):                               // 0x800EA40C
    DeleteObject2(self)
```

### WAIT

```
wait_start(self):           // 0x800EA468
    SetAnime(self, 0)
    self.timer = (difficulty < 2) ? 90 : 45
    self.step2 += 1

wait_count(self):           // 0x800EA4BC
    if self.timer == ((difficulty < 2) ? 30 : 15): self.warn = 2
    if --self.timer == 0: self.step = DROP ; self.step2 = 0
```

### DROP

```
drop_start(self):           // 0x800EA568
    PlaySound(2, 1, self)
    self.speed_y = 0x1000 ; self.gravity = 0x2000
    SetAnime(self, 3)
    self.motion = 2 ; self.step2 += 1

drop_move(self):            // 0x800EA5D0
    MoveObject2(self)
    if self.collide_flags & GROUND:
        self.motion = 0 ; self.warn = 0 ; self.step = GROUND
        self.step2 = (self.var == 0) ? 0 : 2
    elif self.y.hi >= self.home_y + 128:
        self.y.hi = self.home_y + 128
        self.motion = 0 ; self.warn = 0 ; self.step = GROUND ; self.step2 = 0
```

### GROUND

```
drill_start(self):          // 0x800EA6F4, sub 0
    PlaySound(2, 2, self)
    self.timer = 60 ; self.step2 += 1

drill_count(self):          // 0x800EA738, sub 1
    if self.timer == 30: self.warn = 1
    if --self.timer == 0: self.motion = 1 ; self.step = RISE ; self.step2 = 0

rock_start(self):           // 0x800E9F1C, sub 2: put a rock block (template at y 1136) under the drill
    switch self.var:
        1: CopyTiles(0, 4640, 880, {4640, 1136, 4, 3})
        2: CopyTiles(0, 4864, 880, {4864, 1136, 4, 3})
        3: CopyTiles(0, 4928, 880, {4928, 1136, 4, 3})
    layer0.w4C = 1                                  // 0x800971F8 + 0x4C; probably "redraw the map"
    self.y.hi += 32
    4 × SpawnExplosionRand(self, 1, 64, 32)
    SpawnJunk(6, {7, 7, 7, 8, 8, 9}, self)
    self.y.hi -= 32
    if layer0.w20 == layer0.w0E: Quake(8, 4, 2)
    self.timer = 60 ; self.step2 += 1

rock_loop(self):            // 0x800EA09C, sub 3
    if (self.timer & 7) == 0:
        self.y.hi += 32 ; 3 × SpawnExplosionRand(self, 1, 64, 32) ; self.y.hi -= 32
    if (self.timer & 15) == 0:
        self.y.hi += 32 ; SpawnJunk(6, {7, 7, 7, 8, 8, 9}, self) ; self.y.hi -= 32
        if layer0.w20 == layer0.w0E: Quake(12, 1, 2)
    if self.timer == 30: self.warn = 1
    if --self.timer == 0: self.motion = 1 ; self.step = RISE ; self.step2 = 0
```

### RISE

```
rise_start(self):           // 0x800EA7D4
    PlaySound(2, 3, self)
    SetAnime(self, 6)
    self.speed_y = 0x8000 ; self.gravity = 0          // 0.5 px/frame up
    self.motion = 1 ; self.timer = 60 ; self.step2 += 1

rise_move(self):            // 0x800EA840
    MoveObject2(self)
    if self.y.hi <= self.home_y:
        self.y.hi = self.home_y ; self.warn = 0
        self.step = WAIT ; self.step2 = 0
    elif --self.timer == 0:
        self.timer = 60 ; PlaySound(2, 3, self)
```

## Effects (misc object type 0)

```
dridler_make_fx(self, n):   // 0x800EA8E0
    fx = GetMiscObject()
    if fx:
        fx.flags = 0x41 ; fx.type = 0 ; fx.var = n
        copy flip, tpage, clut (no flash bit), anime_table, sprite data, layer from self
        fx.x = self.x.hi (fraction 0) ; fx.y = self.y.hi (fraction 0)
        fx.parent(+0x50) = self

dridler_fx_update(fx):      // 0x800EA224
    fx_state[fx.state](fx)

fx_init(fx):                // 0x800EA990
    fx.flip = LEFT ; fx.y_ofs(+0x56) = 0
    if fx.var == 0: fx.priority = 32 ; SetAnime(fx, 1) ; fx.display = 1 ; fx.step = 0
    if fx.var == 1: fx.priority = 32 ; SetAnime(fx, 2) ; fx.display = 0 ; fx.step = 1
    all speeds = 0 ; fx.state = 1

fx_main(fx):                // 0x800EAA40
    if fx.parent.state == 2:                        // the drill is being deleted
        fx.display = 0 ; fx.state += 1 ; return
    AnimeTick(fx)
    fx_step[fx.step](fx)
    fx.y.hi = fx.parent.y.hi + fx.y_ofs

fx0_motion(fx):             // 0x800EAAF8, keyed on parent.motion; acts only when it changes (+0x54 = last value)
    motion 0: fx.display = 0
    motion 1: fx.priority = 34 ; SetAnime(fx, 1) ; fx.display = 1
    motion 2: fx.priority = 32 ; SetAnime(fx, 4) ; fx.display = 1

fx1_warn(fx):               // 0x800EABB0
    fx.y_ofs = (fx.parent.anime.sprite == 23) ? -1 : 0
    keyed on parent.warn, acts only when it changes:
    warn 0: fx.display = 0
    warn 1: fx.display = 1 ; SetAnime(fx, 5)
    warn 2: fx.display = 1 ; SetAnime(fx, 2)

fx_delete(fx):              // 0x800EAAD8
    DeleteObject3(fx)
```

Note: `fx0_motion` and `fx1_warn` store the last value in `+0x54` of the effect object, which on a
main object would be the hurt box pointer. Misc objects have no hurt box, so the field is free.

## Animations (`0x800FAA34`)

| no | use | frames `(duration, flag, next, sprite)` |
|---|---|---|
| 0 | drill: wait | (2,0,+1,0) (2,1,−1,23) |
| 1 | effect 0: wait / rise | (1,0,+1,4) (2,0,+1,5) (1,0,+1,6) (1,0,+1,7) (1,0,+1,8) (2,0,−5,14) |
| 2 | effect 1: drop warning | (4,0,+1,19) (3,0,+1,20) (2,0,+1,21) (1,0,+1,22) (2,0,+1,21) (3,0,−5,20) |
| 3 | drill: drop / drill | (2,0,+1,0) (2,0,+1,1) (2,0,+1,2) (2,0,−3,3) |
| 4 | effect 0: drop | (1,0,+1,9) (2,0,+1,10) (1,0,+1,11) (1,0,+1,12) (1,0,+1,13) (2,0,−5,14) |
| 5 | effect 1: rise warning | (4,0,+1,15) (3,0,+1,16) (2,0,+1,17) (1,0,+1,18) (2,0,+1,17) (3,0,−5,16) |
| 6 | drill: rise | (2,0,+1,0) (2,0,−1,23) |
| 7–9 | debris | sprites 24, 25, 26 |
