# High Max (main type 55, boss overlay 44): notes

## Method

- Kit (boss): `python3 -m mmxdemake.rekit --name high-max --state turtloid-8_high-max.state.gz --type 55
  --out ~/mmx-data/x6/re/high-max/kit` (124 functions, 18 state tables, 0 neighbours).
- Kit (shots, visual type 21, entry `0x80100C60`): `scripts/viskit.py turtloid-8_high-max.state.gz 21 kit-vis21`
  (patches rekit CLASSES with the visual class, scratchpad +0x84). 34 functions. The big-ball sub-steps
  (`0x8010217C` table, `0x80100D30..0x80100FAC`) were not walked; `--at` listings in `kit-vis21/bigball-subs.txt`.
- Intro copy: `rekit --name high-max-intro --state intro-3_boss.state.gz --type 55 --out kit-intro`.
  `scripts/cmpcode.py` (opcodes with jump/imm fields masked): every instruction of `kit` and `kit-vis21`
  is in `kit-intro` in the same order (only inserts). So the intro stage overlay has the same code.
- `scripts/state.py`, `scripts/anims.py`, `scripts/dmg.py`: save-state reads.
- Views: `view/sheet1.png` (anims 0,1,3,5,7,8,9), `view/anim6.png`.

## Save-state values (turtloid-8_high-max)

- stage 21/1, difficulty 1, boss level 0, game+0x8D = [5, 6], boss_hp_max 48, boss_choice 1, +0x5F 0x11,
  +0x5E 5, game+0x2E 1, game+0x24 1, game+0x20 = slot 0.
- Camera: x 1264, y 704; limits x 1264..1296 (current and target), y 704. cam+0x48 = 8 (restored).
- Player Zero (char 1), (1299, 860), HP 26, mega+0xE7 = 5.
- Boss slot 0: state 1, step 5 (VOLLEY), sub 1, (1552, 766), flip 0, HP 48, contact 8, damage table
  0x8007523C (guard), boxes attack 0x80101A34 / hurt 0x80101A2C, +0x84 = 6 (pattern 6 = [5, 11]),
  +0x85 = 0, +0x89 = 0, +0x8B = 0, +0x8C = 15 (bob index), frame pointer 0x80101B58 = anim 6 frame 22
  (22:13 right after the 4th shot, 11 left -> 2 updates after it). speed (2.0, 2.25) left over from the intro
  flight (2.25 = -1 + 52/16: 52 updates of flight). x 1552 = cam max x + 256 (the clamp).
- 4 visual type 21 var 0 (energy balls), speed about 5 px/frame (e.g. -4.27, -2.60), 74 px apart
  (15 updates at 5 px/frame). Newest at (1482, 765): spawn 1552-56 = 1496, y 758, 2-3 updates of flight.
- THINK check: HP 48 >= 2*16 -> r<8: pattern 4 [5,10], else 6 [5,11]. State has 6. Consistent.

## Tables

- State table 0x80101F80: 0 set-up 0x800FA04C, 1 main 0x800FA2A8, 2 death 0x800FDAF4 (subs 0x80101FD4).
- Step table 0x80101F8C: 0 HitReactionDispatch, 1 nop, 2 INTRO 0x800FDCA8, 3 DIVE SWEEP 0x800FE15C,
  4 HOVER 0x800FE4E8 (unused), 5 VOLLEY 0x800FE5EC, 6 BIG BALL 0x800FE6FC, 7 THINK 0x800FAEAC,
  8 KNOCKDOWN 0x800FEA18, 9 INTRO END (var 0) 0x800FEDE8, 10 BARRIER RUSH 0x800FEF04, 11 SHIELD RAM
  0x800FF508, 12 LOW-HP ORBS 0x800FF99C, 13-17 var 2 only (0x800FFB64, 0x800FFE5C, 0x801000A4,
  0x80100418, 0x801007E8).
- Patterns 0x80101F4C (4 bytes, 7 = THINK): 0 [3] 1 [5] 2 [3,5] 3 [6] 4 [5,10] 5 [6] 6 [5,11] 7 [13]
  8 [13,6] 9 [15,13] 10 [6,16] 11 [6,17] 12 [6,15].
- Resume limits 0x800FA004 (s32 x 18): 0 0 11 7 1 2 4 11 6 5 10 9 4 6 4 5 5 5.
- Bob table 0x8010216C: 0 1 0 1 1 0 1 0 0 -1 0 -1 -1 0 -1 0 (applied on even frame_count).
- Boxes: 0x80101A24 terrain (0,16,32,40); 0x80101A2C/34 body (-20,-28,40,72); 0x80101A3C/48 small
  (-8,-8,16,16); 0x80101A40/4C big (-40,-40,80,80); 0x80101A44/50 tall (-24,-56,48,112);
  0x80101A54 terrain (0,0,8,8); 0x80101A58 terrain (0,0,48,48).
- Damage tables: 0x8007523C guard (class 0 dmg 0 for ids 18-20, 33-36, 42-44, 46, 56; else D, 7 –);
  0x800752DC open (normal); 0x8007537C open (mega+0xE7 == 2: ids 46:6, 50:3, 51:4);
  0x800754BC big ball; 0x8007541C var 9/10 (same as dmg_tbl_yammark-like 1 2 2 ...).

## Pseudo code (boss)

```
setup:                                      # 0x800FA04C
  hp = 0; contact = 8; +0x62 = 0
  var 0: dmg = deflect; game+0x26 = 1; tex 0x1E0; +0x90 = 0
  var 1: tex 0x140 clut 0x7948; sprite = bank[0]; dmg = guard
  var 2: as var 1; +0x8D = +0x8E = {2,3,4,5}[boss_level]; +0x8A = 0
  anim table 0x80101EF0; prio 5; hurt/attack body; terrain 0; +0x61 = 0; layer = mega
  PlayerEvent(20, mega.flip); speeds 0; +0x84 +0x88 +0x86 +0x87 +0x8B = 0; +0x8C = 8
  SetAnime(2); display 0; state 1 step 2 sub 0

main:                                       # 0x800FA2A8
  r = ContactWeapon()
  if r < 0: 0x8001750C(255,0); snd 20,61,33; mega+0x7A = 1; state 2 step 0; return
  if step == 12:
     if +0x8B: +0x8B--; if 0: dmg = open(mega+0xE7); else flash every 4
     elif r not in (0,127): dmg = deflect; flash on; +0x8B = (r in 25,26) ? 2 : 60
  elif r == 127: pass
  elif step == 8:
     if sub == 0:
        if mega.char != 0 and r: snd 61; anim 4; dmg = deflect; sub 1; +0x8B = 30
        elif mega+0xE7 == 2 and r-1 in {50,51,46}: same, +0x8B = 100
        elif r-1 in {1,3..8,10..17,53,45,52}: same, +0x8B = 100
  elif r-1 in {18,19,20,46,56,43,33,34,35,36,42,44}:
     snd 61, 20; +0x86 = step; +0x87 = sub; +0x8B = 0; dmg = open(mega+0xE7); step 8 sub 0
  run step; ContactMega()
  if var == 0:
     if step != 2: +0x90++
     if mega.hp == 0x80: mega.hp = 1
     if step != 9 and ((mega.hp & 0x7F) < 2 or +0x90 > 900): PlayerEvent(20); (hp=1); step 9 sub 0
  if +0x89: dmg = deflect; +0x89--; if 0: dmg = guard, flash off; else flash every 4
  if step != 2: DisplayCheck(128,128)

THINK (var 1):                              # 0x800FAEAC
  r = rng & 15; t = boss_hp_max / 3
  if hp >= 2t: pat = r<8 ? 4 : 6
  elif hp >= t: pat = r<5 ? 4 : r<11 ? 5 : 6
  else: dmg = deflect; +0x8B = 60; step 12 sub 0; return
  +0x85 = 0; step = patterns[pat][0]; sub 0
THINK (var 0): pat = r<2 ? 0 : r<7 ? 2 : 3
THINK (var 2): hp>=2t: r<5 7, r<10 8, else 9; t<=hp<2t: r<3 8, r<7 10, r<12 11, else 12; hp<t: keep +0x84
next():  +0x85++; step = patterns[+0x84][+0x85 & 3]; sub 0

KNOCKDOWN step 8:
  0: flash every 8; +0x8B++; when old >= 121: dmg = guard; step,sub = +0x86,+0x87;
     if step >= 18 or limits[step] < sub: step 7; +0x8B = 0
  1: vx = flip ? -1.5 : +1.5; vy = -1; grav = 1/8; ax 0
  2: terrain box on (y > tminy+32); MoveObject2; on floor: Rumble 3, snd 27, 33, Quake(90,2,2), +0x7C 90, +0x8B 120
  3: 90 updates; anim 5; vy = +5; +0x8B 120
  4: flash; at anim flag: Move, terrain off; at y <= miny-64: re-place; +0x7C 50
  5: 50; vy = -4
  6: Move; at y >= miny+64: y = miny+64; +0x89 = 60; next()
re-place: if flip: flip 0, x = cam.max_x + 256 else flip 64, x = cam.min_x + 64; y = miny - 64

VOLLEY step 5: anim 6, snd 34; at flag 8: ball var 0 at x -/+ (sprite 25 ? 56 : 44), y-8, snd 28;
  at flag 1: sub 2; 41 updates bob; next()
BIG BALL step 6: snd 35, anim 7; at 10: Rumble 7, snd 29; at 41: anim 8; at flag 1: snd 30, ball var 1;
  wait (bob; snd 36 at 121) until act4 == 1 (ball's state 2 sets it); anim 9; 40 updates; next()
BARRIER RUSH step 10: anim 7, loops = {1,2,3,3}[level]; at 10 Rumble 7 snd 29; at 61 anim 8;
  at flag 1: snd 123, aura var 4+5, dmg = deflect; 91 bob; anim 9; at flag 1 vy -4;
  at y >= tminy+112: vx = flip?+6:-6, ax 1/256, +0x7C 50; 50 bob (no move);
  MoveObject2 until x < cam.min_x+48 (left) / x > cam.max_x+272 (right): vy +2, grav -0.375, vx -/+1;
  until y <= miny-64: re-place, 50 wait; dmg = guard; vy -6; at y >= miny+64: loops--; again or next()
SHIELD RAM step 11 (+0x89 kept at 120 in subs 0-5):
  0: mid = cam.min_x + (cam.max_x - cam.min_x + 320)/2; if player and boss on the same half: vx -/+6
     toward the other edge (sub 1) else snd 35, anim 18 (sub 2)
  1: Move; turn inward at room 112/240; stop at cam.min_x+64 / cam.max_x+256: snd 35, anim 18
  2: at anim flag 1: shield var 6
  3: 60 bob; vy -4
  4: at y >= tminy+116: vx flip?+6:-6, ax 1/16
  5: MoveObject2; at 6 updates anim 21; at cam.min_x+64 / cam.max_x+256: Quake2(40,2,2), snd 27
  6: 41 bob; +0x89 = 0; dmg = guard
  7: 31 bob; anim 19
  8: at flag 1: vy +2; face the centre
  9: at y <= miny+64: next()
LOW-HP step 12: anim 2; vx toward the centre 4; at the centre: snd 35, anim 7;
  10 updates: Rumble 7, snd 29; loop { 61 updates: (var 1) snd 61, 35, orb var 7 | (var 2) once var 11;
  66 updates (var 1): snd 29 } -- never ends.
DIVE SWEEP step 3 (var 0 patterns): vy -4 to tminy+112; vx -/+4, ax 1/256; 50 bob; sweep to edge;
  rise; re-place; 50 wait; vy -1 down to miny+64; next()
INTRO step 2: see sheet. INTRO END step 9 (var 0): see sheet.
DEATH state 2: anim 4, PlayerEvent(20, mega.flip), 127 updates (flash 25,20,15,10,5,5..), sys obj 26;
  flash every 6 while it lives; then stage 12: exit 0x80, game[0x2C+cp] = 1, cp += 9; else exit 0x10;
  game+0x24 = 0, game+0x20 = 0; cam target min y = cam y; delete.
```

## Visual type 21 (entry 0x80100C60, states 0x80102208: set-up 0x800FC21C, main 0x800FD86C, end 0x801019D8)

- main: clears flash; parent state 2 -> end (snd 20). parent step 8 sub > 0 and var 6/9/10 -> end
  (9/10 also clear their bit in parent +0x8A). Step table 0x801021E0 by own step. Steps 4/5: no contact.
  Else ContactMega; parent var 0: player HP floor 1, at HP < 2 PlayerEvent(20) and delete.
  OffScreenCheck2(96,64): vars 1/6/7 snd 20; end (not 9/10).
- end: var 1 -> parent act4 = 1; var 11 -> parent act4 = 0; delete.
- vars: 0 ball (step 0), 1 big ball (step 1, subs 0x8010217C), 2 (step 2/3, maker not found),
  3 floor effect (step 4), 4/5 aura (step 5), 6 shield (step 6), 7 orb (step 7 sub 0), 8 piece (step 7 sub 2),
  9/10 satellites (step 8, var 2), 11 (step 9, var 2).
- burst (var 1 kill; var 7 at 61): n = 4 (var 7 on difficulty >= 2: 6); angle_k = 90k + (rng & 0x2D) deg;
  speed 3 px/frame; piece +0x8D = 24 updates out, then act4 = 40(k+1) hold, then aim 5 px/frame, snd 125.
- After the burst / at its end the object does a 31-update palette blend (0x8002B54C) with bg_refresh |= 1.

## Intro maker

- intro-3_boss: code at 0x800F9A54 (an entry of a table at 0x800FBC40 in the intro overlay, owner not read)
  makes main type 55 var 0 at (6162, 346), tex 0x1E0, its own CLUT; before that it raises some object's
  HP (+0x5C) to at least 2 [owner not read].

## Open

- weapon ids of the guard-break list and the stun lists; mega+0xE7 meaning (armour index?; Zero = 5).
- sys objects 24 / 26; 0x8001750C(255, 0) at the kill.
- var 2: steps 13-17 and vars 9-11 not read in full; where var 2 is made (stage 12?).
- dmg_tbl_capsule class 7 on the orb (var 7) hurt box.
- anims 0, 1, 3, 11: not set by the walked code.
