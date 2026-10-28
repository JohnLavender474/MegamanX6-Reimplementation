# Nightmare Bug: pseudo code

```
entry(o):                                   ; 0x800F42AC
    o.prev = o.pos
    STATES[o.state](o)                      ; 0x800FC718

state0_setup(o):                            ; 0x800F3F54
    if nightmare_effect != 1: DeleteObject3(o); return
    o.hp = 8; o.contact = 2; o.iframes = 0
    o.dmg = DMG_BUG (0x800746FC)
    o.anim_tbl = 0x800FC714; o.priority = 5; o.layer = mega.layer
    o.speed = 0; o.latch = 0; o.collide = 0; o.flip = 0; o.terrain_box = null
    o.attack_box = o.hurt_box = (-6, -6, 11, 11)
    o.xofs = {-96,-80,-64,-48,48,64,80,96}[rng() & 7]
    o.yofs = {0,16,32}[rng() % 3]
    o.blocks = 0
    SetAnime(o, 0)
    o.state = 1; o.step = 1; o.step2 = 0

state1_main(o):                             ; 0x800F42F8
    STEPS[o.step](o)                        ; 0 hit reaction, 1 follow, 2 leave
    r = ContactWeapon(o)
    if r < 0:
        DropItem(o, 18); SpawnExplosion(o); o.state = 2; return
    if mega.gate_state (mega+0xD4) == 1: o.state = 2; return
    if r == 127:                            ; deflected
        if o.blocks++ >= 5: o.step = 2
    ContactMega(o)
    if OffScreenCheck2(o, 64, 64): DeleteObject2(o)
    else: DisplayCheck(o, 32, 32)

step1_follow(o):                            ; 0x800F40FC
    o.flip = (o.x < mega.x) ? RIGHT : LEFT
    tx = mega.x + o.xofs; ty = mega.y + o.yofs
    a = ratan2(ty - o.y, tx - o.x)
    o.vx = 5 * cos(a); o.vy = -5 * sin(a)   ; vy positive is up
    if |o.x - tx| < 16 and |o.y - ty| < 32:
        o.vx = o.vy = 0
        if frame_count is even:
            o.phase = (o.phase + 1) & 63
            o.y += trunc(rsin(o.phase * 128) / 2098)   ; -1 / 0 / +1
    AnimeTick(o); MoveObject(o)

step2_leave(o):                             ; 0x800F4468
    o.vx = 0; o.vy = 5                      ; up
    AnimeTick(o); MoveObject(o)

state2_gone(o): DeleteObject3(o)            ; 0x800F43F4
state3_blink(o):                            ; 0x800F4414
    if o.t++ < 60: o.display ^= 1
    else: DeleteObject3(o)
```
