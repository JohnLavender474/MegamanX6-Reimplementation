# X's attacks — pseudo code (player RE step 3)

Source: `~/mmx-data/x6/re/x/kit/code.txt` (X kit, `intro-1_start`), printed with
`scripts/fn.sh <addr>` (lower case). Code outside the kit (effect type 2, `ContactWeapon`,
`ApplyWeaponHit`, `WeaponHitReaction`): `~/mmx-data/x6/re/player/disasm.py <state> <addr> <n>`.
Tables: `scripts/tables.py`, `scripts/anims.py`. `m` = `mega` (`0x800970A0`). `held` = `m+0x7C`
(16 bits), `old` = `m+0x7E` (held in the update before), `pressed` = `m+0x80`. The 32-bit word
at `m+0x7C` is `held | old << 16`, so `word & 0x200020 == 0x200000` means "0x20 held in the update
before and not now" = released this update. Speeds are 16.16 px/update.

## Order in one player update (`PlayerUpdate` `0x80034FBC`)

```
m.8F = 0; m.90 = 0; m.DD = 0                       ; 0x80034FE8
PlayerHitCheck                                      ; can call ChargeStop
if m.92: m.92 -= 1                                  ; shot_wait
if m.91: m.91 -= 1; if m.91 == 0: m.90 = 1; m.8E = 0    ; shot pose ends
... gate, ride, CheckWalkOff
WeaponUpdate (0x8003f048)                           ; weapon select, shot ids
step routine                                        ; TryShoot is called from the steps
ChargeUpdate (0x8003ec78)                           ; charge timers
ShotPalettes (0x8003ce38)                           ; muzzle palette, charge blink
PlayerHurtBox (0x8003c3c8)
```

## Weapon select and shot ids (before the step)

```
WeaponUpdate (0x8003f048):
  if m.var: ZeroWeaponUpdate (0x8003dbdc); return
  old = m.93
  WeaponSelect (0x8003dd6c)
  ShotIds (0x8003df58)
  if m.93 != old: WeaponChanged (0x8003f0bc)       ; palette; effect 27 for slots 1, 6, 8

WeaponSelect (0x8003dd6c):
  return if m.D3 or m.99 or m.D5 or m.E0 or game.player_lock or step in (45, 46, 47, 76)
  if held & 0x600 == 0x600 or pressed & 0x600 == 0x600:
      if m.93: m.93 = 0; ChargeReset; PlaySound(0, 12)          ; both: back to the buster
      return
  if pressed & 0x200: next owned slot (wrap after 8 to 0)       ; owned: bit_tbl 0x80073ED8[i] & m.C9
  if pressed & 0x400: previous owned slot (wrap 0 -> 8)
  on a change: m.93 = slot; ChargeReset; PlaySound(0, 12)

ShotIds (0x8003df58):
  if form == 2:
      if m.144 == 6 and step != 31: BusterChargedId(m, 1); return
      m.94 = 50; m.95 = 45 (46 when m.9C == 2 or part 0x80000); return
  m.94 = m.93                     ; 0 -> 50 below
  if m.93 == 0: m.94 = 50
  elif m.9B == 2: m.94 = m.93 + 9
  elif part 0x80000 and form in (1, 3, 4): m.94 = m.93 + 9
  m.95 = 0
  BusterChargedId(m, 1)           ; 0x8003f7bc

BusterChargedId (0x8003f7bc, k = 1 -> m.95):
  if m.144 != 6:
      if not part 0x80000:
          if m.9C == 0: return               ; stays 0 = lemon
          if m.9C == 1: m.95 = 9; return     ; half charge
      form 4: m.95 = 20; return
      form 1: m.95 = 19; return
      form 3: m.95 = 43; return
  m.95 = 18                                   ; other forms, or m.144 == 6
```

## The shot try (in the steps)

```
TryShoot (0x8003e614):
  if m.92: return                                 ; shot_wait
  m.96 = -1
  PickWeaponShot (0x8003e9e8)                     ; button 0x10
  PickBusterShot (0x8003f954)                     ; button 0x20, only when m.96 is still -1
  w = m.96; if w == -1: return
  if w == 46 and m.86 == 10: m.96 = -1; return
  if w == 43 and up held and m.86 != 9: w = m.96 = 56; m.86 = 9
  if pose_tbl[w] (0x800741B4: 46, 50, 51, 56):
      if step == 9: m.96 = -1; return
      m.DD = 1; m.8F = 1; return                  ; the step's pose code fires it
  if m.A6 == 1 and w in (2, 11) / m.A6 == 3 and w in (6, 15) / m.A6 == 2 and w in (7, 16): m.96 = -1; return
  max = (part 0x40000 ? max_rapid 0x800740B4 : max 0x80074074)[w]
  if not m.98 < max: return
  if single_tbl[w] (0x800740F4) and m.99: return
  spawn_tbl[w] (0x80073F74)(m)
  w in (1, 7, 10): m.8E = m.8F = 0; else m.8E = m.8F = 1
  m.DD = 1; m.97 = w; m.90 = 0
  m.91, m.92 = time_tbl[w] (0x80073EFC, 2 bytes)
  if part 0x80000 and w != 15 and m.144 != 6 and w not in 1..7: m.92 = time_tbl[w][1] * 4
  if count_tbl[w] (0x80074134): m.98 += 1
  if count2_tbl[w] (0x80074174): m.99 += 1
  m.9A = (m.9A + 1) mod 3
  unless w == 7 or (w == 5 and the side in front is blocked): SpendEnergy(m, w)   ; 0x8003f740
  if m.93 == 0 and m.A4 == 0: m.A2 = {0: 6, 9: 5, 18: 7, 45: 6}[w]                 ; muzzle palette time

PickWeaponShot (0x8003e9e8):
  if pressed & 0x10 or (held & 0x10 and game+0xC9 & 2):
      if m.93 and not HasEnergy: return
      if m.94 == 50 and step in (31, 26): return
      m.96 = m.94; return
  if word & 0x100010 == 0x100000:                 ; released 0x10
      if m.94 in (0, 50) or no_charge_tbl[m.94] (0x80073E40): return
      if m.93 and not HasEnergy: return
      m.96 = m.94

PickBusterShot (0x8003f954):
  if m.96 != -1: return
  if pressed & 0x20 or (held & 0x20 and game+0xC9 & 2): m.96 = m.95; return
  if word & 0x200020 == 0x200000:                 ; released 0x20
      if m.95 == 0: return
      if m.95 == 45 and m.9C == 0: return
      m.96 = m.95
```

Tables for the buster ids (`tables.py`, `sheldon-1_lemons`):

| id | spawn | max (Rapid 5) | `+0x98` | timer `+0x91` | wait `+0x92` | stand anim |
|---|---|---|---|---|---|---|
| 0 | `0x8003FA60` | 3 (5) | +1 | 20 | 6 | 88 |
| 9 | `0x8003FA8C` | 3 | (+1 by the flash) | 25 | 15 | 90 |
| 18 | `0x8003FA8C` | 3 | (flash) | 35 | 15 | 91 |
| 19 | `0x8003FA8C` | 2 | (flash) | 35 | 15 | 91 |
| 20 | `0x8003FA8C` | 4 | (flash) | 35 | 15 | 91 |
| 43 | `0x8003FA60` | 4 | 0 | 35 | 15 | 91 |
| 45 | `0x8003FA60` | 3 (5) | +1 | 20 | 6 | 88 |
| 46, 50, 51, 56 | pose | – | – | 35 / 1 / 1 / 1 | 15 / 1 / 1 / 1 | 91 / 92 / 92 / 92 |

## Spawn

```
SpawnShot (0x8003fa60):       SpawnWeapon(1, m.96, 0, m)                 ; 0x8003d3f8
SpawnChargeShot (0x8003fa8c): SpawnEffect(0x21, 2, m.96, 0)              ; 0x8003d510, the muzzle flash
                              if m.96 in (18, 19, 20) and GetRNG() & 1: PlayVoice(m, 6)
SpawnWeapon(flag, type, var, parent): o = GetWeaponObject (0x8002c32c: first free of 32, cleared)
                              o.flag, o.type, o.var; o.layer = mega.layer; o.7C = parent
SpawnEffect(flag, type, var, parent): GetEffectObject; same; o.50 = parent
```

## Charge (after the step)

```
ChargeUpdate (0x8003ec78):
  if m.var: return
  if m.E0 and form == 4: return
  if step in (0, 1, 25, 24) or m.D5: return
  if ChargeCancel (0x8003ed98):
      ChargeReset (0x8003f480): StopSound(1, 7); m.A1 = 0; m.9B = m.9C = m.9D = m.9E = 0
      palette: m.A2 ? 0x8003cf54(m, 50) : 0x8003c4ec(m)
      if m.A6 == 1: m.A1 = 41
      return
  if part 0x80000: return                         ; Ultimate Buster: no charge at all
  WeaponCharge (0x8003eee4)                        ; m.9D -> m.9B
  BusterCharge (0x8003fff4)                        ; m.9E -> m.9C
  if not ChargeGlow(m, 0): ChargeGlow(m, 1)        ; 0x8003ff90

ChargeCancel (0x8003ed98): 1 when
  step 31; m.A6 == 1; pressed & 0x10 or 0x20; held 0x10 or 0x20 with game+0xC9 bit 1;
  game+0xC9 bit 0 with the buster selected: released 0x10 or 0x20 (only those);
  else: released 0x10 or 0x20; m.9D and 0x10 not held; m.9E and 0x20 not held

BusterCharge (0x8003fff4):
  if not (held & 0x20) and not (game+0xC9 & 1 and m.93 == 0): return
  if m.9C == 2: return
  m.9E += 1
  half, full = (12, 45) with part 0x100000, else (25, 90)
  m.9E == half: PlaySound(1, 7, m); m.9C = 1
  m.9E == full: m.9C = 2

WeaponCharge (0x8003eee4):
  m.93 == 0: m.9B = 0; return
  need held & 0x10, m.9B != 2, form in (1, 3, 4), energy[m.93] >= cost(m.93 + 9)
  same count on m.9D, half 25 / 12, full 90 / 45, sound 1/7 at half

ChargeGlow (0x8003ff90, k):
  level = m.9B[k]; 0 -> return 0
  m.A1 = 38 at level 1; 41 at level 2; 47 at level 2 when the id m.94[k] is 20
ShotPalettes (0x8003ce38):
  skip steps 0, 1, m.A4 > 0
  m.A2: count down; at 0 the normal palette
  m.A4 < 0: m.A1 = 35
  m.A1: m.9F counts down; at 0: palette m.A1 or normal by m.A0, m.9F = 1, m.A0 ^= 1  ; blink, 2 updates each
  m.A1 = 0                                         ; set again each update

ChargeStop (0x8003f3dc), from PlayerHitCheck, PlayerRideCheck, TrySuperJump, 0x8003b870:
  X only: ChargeReset; ShotIds; normal palette; effect 27 for slots 1, 6, 8
```

## Weapon objects (32 × `0xA0`, `WeaponLoop` `0x8002012C`, types `0x80071220`)

```
type 0 lemon (0x801ee05c, states 0x801f37e4):
 0 LemonInit (0x801ece0c):
   flags 0x21; terrain box 0x801f37b0 {-3,0,5,5}; attack box 0x801f3790 {-10,-6,18,11}
   anime 0x8007789c, sprite bank = layout directory entry 1; clut 0x7802 (0x7844 with parts 0x180 or 0x40000)
   flip = X's flip; LemonFollow: at the buster (0x801ee34c) while X keeps the shot pose and facing
   speed_x = 6.75 (8.75 with part 0x100), sign by flip; no gravity
   SetAnime(21); state 1; DisplayCheck(12, 8)
   SpawnEffect(0x21, 2, 0, 0)                     ; the lemon muzzle flash
 1 LemonMain (0x801ee458):
   OffScreenCheck2(12, 8) -> ShotEnd (state 3)
   AnimeTick
   o.98 == 0: MoveObject; wall (collide or CornerCheck) -> ShotSpark (0x800383f8), ShotEnd
   o.98 > 0 (hit): ShotEnd
   o.98 < 0 (deflected): speed_x = -speed_x; speed_y = +4.8 or -4.8 (GetRNG & 1); flip ^= 0x40; state 2
 2 LemonBounce (0x801ee5d4): off screen -> end; AnimeTick; MoveObject (no boxes, no wall test)
 3 LemonDelete (0x801ee650): parent.98 -= 1; DeleteObject3

types 9, 18 (0x801ee098, states 0x801f37f4), made by the flash:
 0 (0x801ecf64): var = 0 (type 9) or 1 (type 18)
   speed_x: 9: 8.0 (10.0 with 0x100); 18: 10.0 (12.0)
   attack box 0x801f3794 + 4 var: 9 {-13,-7,29,13}; 18 {-34,-19,64,36}
   terrain box 0x801f37b4 + 4 var: 9 {0,0,8,8}; 18 {0,0,12,8}
   SetAnime(26 + var); state 1; DisplayCheck(32, 20)
 1 (0x801ee684): OffScreenCheck2(32, 20) or o.98 != 0 -> state 3
   AnimeTick; MoveObject; wall -> ShotSpark, state 3
 2, 3 (0x801ee72c): mega.98 -= 1 (not for type 21); DeleteObject3

type 19 Falcon full charge (0x801ee0d4, states 0x801f3804):
 0 (0x801ee774): flags 1; hit tag o.64 = 1; attack box 0x801f37c4; terrain 0x801f37c8
   its own frame table (o.38 = layout_base+0x10 entry 0x3C, as the player frames), anime 0x80077a80,
   tpage 0x540; speed_x = +-10.0; SetAnime(0); SpawnEffect(1, 27, 8, 0); state 1
 1 (0x801ee860): OffScreenCheck2(32, 32) -> state 2
   AnimeTick; MoveObject; every 4 updates (frame_count & 3 == 0) o.64 += 1
   wall or o.98 != 0 -> spark (only for the wall), state 2
 2 (0x801ee940): mega.98 -= 1; DeleteObject3

type 20 Ultimate full charge (0x801ee110, states 0x801f381c), init 0x801ed1b8:
   type 20: attack 0x801f37a4, terrain 0x801f37bc, hit tag 1, speed 11.0 (13.0 with 0x100), anime 29
   type 21 (the parts that 0x801ed36c makes): speed 0, anime 30 or a second set (var != 0)
   main 0x801eea10: OffScreenCheck2(42, 34); type 20 -> 0x801ed36c, type 21 -> 0x801eea8c  [not read]
```

## The muzzle flash (effect type 2, `0x80049478`, effect table `0x8006DDDC` + stage × 8 + area × 4)

```
MuzzleFlash: mega.step == 20 -> DeleteObject3; state 0 -> MuzzleInit; else MuzzleMain
MuzzleInit (0x80049270):
   kind = {9: 0, 18: 1, 19: 2, 20: 2, 0: 3, 45: 3}[var]
   bank, anime, sound = 0x80077c0c[kind]: 0 (3, 22, 9); 1 (2, 23, 10); 2 (2, 25, 10); 3 (1, 39, 8)
   lemon: clut from 0x801ee434 (0x7844 with part 0x40000)
   flip = X; MuzzleFollow; SetAnime(anime); PlaySound(1, sound); state 1; DisplayCheck(64, 32)
MuzzleMain (0x80049800): AnimeTick; MuzzleFollow (0x800499a0, like LemonFollow, offset by var)
   step 0 (0x80049854): when the frame has an event flag:
       var not 0 and not 45 and a free weapon slot: new weapon object
         flags 0x21, type = var, var 0, x, y, flip, sprite bank, anime table, tpage, clut, priority from the flash
         mega.98 += 1
       step 1
   step 1 (0x80049950): animation at its hold frame (next == 0) or detached -> DeleteObject3
```

Flash animations (table `0x8007789C`; ticks, flag): 22 (half) 2, 1, 1, **1 flag**, 2, hold 96;
25 (Falcon/Ultimate) and 23 (plain full) 2, 1, 2, 1, **1 flag**, 3, 3, hold 96; 39 (lemon) 2, 1,
1, hold with flag (var 0 makes no shot). So the half shot appears 4 updates after the release
and the full shot 6 updates after.

## Hit result in the shot (`ApplyWeaponHit` `0x800315CC` → `WeaponHitReaction` `0x800319EC`)

```
table 0x800737D0, 5 bytes per id: {spark, sound kind, end flag, keep on kill, sound id}
deflect (class 0xFF): end flag -> shot.98 = -1; PlaySound(0, 6)
hit (mode 0) or kill (mode 1, 2): end flag and (mode == 0 or keep-on-kill == 0) -> shot.98 = 1
ids: 0 {3,0,1,0,12}; 9 {3,0,1,1,12}; 18 {4,0,1,1,12}; 19 {3,0,1,0,12}; 20 {4,0,1,0,12}
```

So 9 and 18 go through an enemy that the hit kills; 0, 19, 20 end at the first hit.
