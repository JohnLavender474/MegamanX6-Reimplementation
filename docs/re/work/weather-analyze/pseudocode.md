# Weather Analyze: pseudo code

## Dispatch tables

| table | entries |
|---|---|
| `0x800F5DE4` machine states | 0 `wa_init` `0x800EB6C0`, 1 `wa_main` `0x800EB2D4`, 2 `wa_dead` `0x800EB89C`, 3 `wa_gone` `0x800EB8F0` |
| `0x800F5EBC` machine steps | 0 `HitReactionDispatch`, 1 `wa_none` `0x800EB7D8`, 2 `wa_wait` `0x800EB7E0` |
| `0x800F5EC8` sensor states | `0x800EB4D4`, `0x800EB974`, `0x800EBA50`, `0x800EBA50` |
| `0x800F5ED8` sensor steps | `HitReactionDispatch`, `0x800EBA40`, `0x800EBA48` (both empty) |
| `0x800F5EE4` display states | `0x800EBAF0`, `0x800EBBD4`, `0x800EBCD4`, `0x800EBCD4` |
| `0x800F5EF4` barrier states | `0x800EBD74`, `0x800EBE60`, `0x800EBF64`, `0x800EBF64` |
| `0x800F60AC` rain controller (type 11) | `0x800ED5C4`, `0x800ED69C`, `0x800ED774`, `0x800ED3E4`, `0x800ED850` |

### entry — 0x800EB5F8
```
switch (s8)obj.var: 1..4 → sensor(obj); 5 → display(obj); 6 → barrier(obj); return
prev = pos; display = 0; machine_states[state](obj); CollideCheck(obj)
```

### wa_init — 0x800EB6C0 (state 0)
```
wa_index = var & 0xF; var = 0
anim table 0x800F5D9C; priority 6; terrain box 0x800F5CB0; dmg_tbl_normal
no attack box, no hurt box; speeds 0; gravity 0x40000; hp 32; contact 0; layer = mega.layer
SetAnime(0); pos = wa_pos[wa_index]
game+0x26 = 1
step = 2; step2 = 0; act4 = 5; state = 1
```

### wa_main — 0x800EB2D4 (state 1)
```
r = ContactWeapon(obj)
game+0x26 = (zone[i].lo <= mega.x.px <= zone[i].hi)
if r < 0:
    if i == 5: cam+0x26 = cam+0x24
    if cam_right[i] != 0: cam+0x24 = cam_right[i]
    if i == 1: cam+0x2A = 224; CopyTiles(0, 2592, 96, rect0); CopyTiles(0, 2832, 0, rect1)
    game+0x26 = 0
    SpawnExplosion; SpawnJunk(4, {14,15,16,17}); SetAnime(13)
    state = 2; clut &= 0x7FFF; DisplayCheck(40, 64); return
ContactMega
if !OffScreenCheck2(40, 64): DisplayCheck(40, 64)
steps[step](obj); MoveObject2; AnimeTick
```

### wa_wait — 0x800EB7E0 (step 2)
```
if floor and act4 == 5:
    for v in 1..6: o = GetMainObject(); if o: o.flags = flags; o.var = v; o+0x80 = obj; o.type = type
    act4 = 4
if act4 == 0 and hurt_box == 0: hurt_box = 0x800F5CB4
```

### wa_dead — 0x800EB89C (state 2)
```
if wa_index == 3: game+0x28 = 1
state = 3; DisplayCheck(40, 64)
```

### wa_gone — 0x800EB8F0 (state 3)
```
DisplayCheck(40, 64)            ; never deleted
```

### sensor — 0x800EB914 / init 0x800EB4D4 / main 0x800EB974 / delete 0x800EBA50
```
init: copy parent tex/anim/sprite/clut; priority 6; attack 0x800F5CB8; hurt 0x800F5CBC
      dmg_tbl_normal; no terrain box; pos = sensor_pos[wa_index][var-1]; speeds 0; gravity 0
      hp 16; contact 4; SetAnime(9); step 2; state 1
main: if ContactWeapon < 0: parent.act4 -= 1; SpawnExplosion; SpawnJunk(3, {10,11,12}); state 2; return
      ContactMega; if !OffScreenCheck2(16,16): DisplayCheck(16,16)
      steps[step]() (empty); MoveObject2; AnimeTick
delete: state 2 → DeleteObject, else DeleteObject2
```

### display — 0x800EBA90 / init 0x800EBAF0 / main 0x800EBBD4
```
init: copy parent graphics; priority 4; no boxes; pos += parent pos; hp 0; SetAnime(1); state 1
main: n = parent.act4: 0 → state 2 (delete); 1 → anim 4; 2 → anim 3; 3 → anim 2; else keep
      visible 48/64; MoveObject2; AnimeTick
```

### barrier — 0x800EBD14 / init 0x800EBD74 / main 0x800EBE60
```
init: as display, priority 5, attack box 0x800F5CC0, contact 0, SetAnime(5)
main: n = parent.act4: 0 → state 2; 1 → anim 8; 2 → anim 7; 3 → anim 6
      ContactMega; visible 48/64; MoveObject2; AnimeTick
```

### rain controller (stage-logic type 11) — 0x800ED510
```
if game+0x27: t16 = t18 = 0
states[state](obj)
if +0x1E: every 4 updates: cnt = (cnt+1) mod 5; cam+0xA0 = 1; cam+0x62 = cnt << 8
```
- 0x800ED69C (1): if game+0x26: clear timers, cam+0x5E/+0x62 = 0, cam+0xA0 = 1, +0x1E = 1, fade = 0,
  palette setup; PlaySound(5, 4, 0); state 2.
- 0x800ED774 (2): if !game+0x26: state 4; else fade = min(fade+1, 30); blend; fade == 30 → state 3.
- 0x800ED3E4 (3): if !game+0x26: state 4. if mega+0x14C: PlaySound(5, 4, 0).
  if mega.hp != 0 and mega+0xE0 == 0:
    if ++t16 >= 180: t16 = 0; hp &= 0x7F; if !0x8008EAFC: hp = hp < 2 ? 0x80 : hp-1; restore bit 7
    if ++t18 >= 51: t18 = 0; mega+0xE8 = 1
- 0x800ED850 (4): if game+0x26: state 2; else fade = max(fade-1, 0); blend; fade == 0 →
  PlaySound(5, 3, 0); state 1.

## Engine helpers called

| address | name | use |
|---|---|---|
| `0x800179A4` | SetAnime | animations |
| `0x80017A04` | AnimeTick | |
| `0x80027BA0` | CopyTiles | map change on death (index 1) |
| `0x8002BBF4` | GetMainObject | the 6 children |
| `0x8002CA14` / `0x8002CA54` | DeleteObject / 2 | children |
| `0x8002CBFC` / `0x8002CCB0` | OffScreenCheck2 / DisplayCheck | visibility only |
| `0x8002D230` | MoveObject2 | |
| `0x8002F288` | CollideCheck | |
| `0x80030ECC` | ContactMega | |
| `0x80031474` | ContactWeapon | |
| `0x80040290` | HitReactionDispatch | step 0 |
| `0x800477E4` | SpawnJunk | |
| `0x80049E50` | SpawnExplosion | |

## Object struct (the fields used here)

| offset | name | use |
|---|---|---|
| `+0x00` | flags | copied to children |
| `+0x01` | type | copied |
| `+0x02` | var | kind / machine index (cleared) |
| `+0x03` | display | |
| `+0x04` | state | |
| `+0x05` | step | |
| `+0x06` | step2 | |
| `+0x07` | act4 | machine: 5 before landing, then live sensor count |
| `+0x08`/`+0x0C` | x / y | |
| `+0x14` | layer | |
| `+0x15` | flip | 0 |
| `+0x16` | priority | |
| `+0x17` | anime | display/barrier compare |
| `+0x18`/`+0x1C` | prev_x / prev_y | |
| `+0x20`..`+0x2C` | speeds, accel, gravity | |
| `+0x30` | anime_table | |
| `+0x3C` | sprite_data | |
| `+0x40` | tex_cord | |
| `+0x42` | clut_cord | |
| `+0x50` | attack_box | |
| `+0x54` | hurt_box | |
| `+0x58` | damage_table | |
| `+0x5C` | hp | |
| `+0x60` | contact_damage | |
| `+0x61` | iframes | 0 |
| `+0x67` | latch | 0 |
| `+0x68` | terrain_box | |
| `+0x70` | collide_flags | floor bit |
| `+0x80` | parent | children only |
