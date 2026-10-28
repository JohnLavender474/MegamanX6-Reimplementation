# Rainy Turtloid (main type 57, stage overlay 09): notes

## Method

- Kit: `python3 -m mmxdemake.rekit --name rainy-turtloid --state turtloid-7_boss.state.gz --type 57`
  (89 functions, 16 state tables, neighbour visual type 23). Output in `kit/`.
- Read `kit/code.txt` in full (4,960 lines), `kit/data.txt`; `--at 0x8002c3f8` (GetSysObject) and
  `--at 0x80027ba0` (CopyTiles).
- Save-state check: `scripts/peek.py` (run with `PYTHONPATH=<repo>:scripts`).
- Sprite sheets: `scripts/sheet.py` -> `img/a0-9.png`, `img/a10-21.png`, `img/a22-33.png`.
- Room: `img/room-stage.png`, `img/room-collision.png` (crop of the m101 stage map, x 8900, y 600).

## Save-state values (turtloid-7_boss)

- difficulty 1, boss_level 0, game+0x8D = [5, 7], boss_hp_max 56, game+0x24 = 1, game+0x25 = 0.
- player Zero (char 1), step 2, (9032, 858), flip 0x40, HP 28.
- boss slot 0: state 1, step 5, sub 2, anim 19, flip 0, (9184, 816), HP 56, contact 6, 7C = 3, 7E = 9,
  act4 = 1, +0x86 = 0 (pattern 0), +0x87 = 1, hurt box 0x800F652C, attack box 0x800F6520,
  terrain 0x800F651C, collide 8 (floor). work.80 = 0x8009B7B0 (the old WARNING object slot).
- fight timer 0x800F6C0C = 78. 6BF8 = 6BFC = 0, 6C04 = 0 (no crystal lost).
- room points 6BE4.. = 8960, 9344, 9152, 880, 9184, 592, 656, 9280, 9024 (camera min 8944, 640).
- visual type 23: slot 0 var 0 and slot 1 var 1 (crystals, step 2, anim 20/21, HP 16, frame
  counter +0x90 = 1576); 8 missiles (var 2 x3, var 3 x5), 3 in step 0 (counters 3, 11, 19) and 5 in
  step 1 (counters 3, 11, 19, 27, 35). Speeds (-0.766, +0.643), (-0.174, -0.985): 40 and 80 deg.
- Pattern 0 = Normal, HP > half. 7E = 9 and 8 missiles: fires at the resets 1->2 .. 8->9. So
  9 missiles per volley at level 0 (table 0x800E98C4 {10,14,18,22} minus 1).

## Tables

- states 0x800F69A4: 0 setup 06B4, 1 main 0910, 2 death 46F0.
- steps 0x800F69B0: 0 HitReactionDispatch, 1 nop, 2 intro, 3 idle (nop), 4 THINK 0DB4, 5 missiles
  33EC, 6 spin/roll 35DC, 7 roll + bubbles 3CB0, 8 roll + water balls 40B8, 9 stun 44C0.
- patterns 0x800F6924 (6 bytes): 0 [5,6,4], 1 [5,7,4], 2 [5,8,4].
- HP bonus 0x800E98C0 {0,0,4,8}; volley count 0x800E98C4 {10,14,18,22}; water-ball rate
  0x800E98F8 {60,50,40,30}.
- boxes: terrain 651C (0,0,64,64); attack 6520 (-40,-40,104,104), spin attack 6524 (-60,-60,120,120);
  hurt shell 6528 (16,-48,32,96) [deflect], back 652C (48,-48,24,96), whole 6530 (-48,-48,96,96).
- crystal hurt 6AB4 (-16,-30,32,16) var 0, 6AB8 (-16,10,32,16) var 1.
- missile terrain 6ABC (0,0,14,5), attack/hurt 6AC0/6AC4 (-14,-5,28,10).
- bubble 6AC8/6ACC (-14,-14,28,28); water ball 6AD0/6AD4 (-14,-14,28,28).
- spin circle 6AD8 (16 x {dx,dy}, r 24) and 6B18 (the same, 8 entries on = opposite point).
- CopyTiles sources (1 x 14 cells): 6984 (8944,976) left wall "hit", 698C (8960,976) left wall normal,
  6994 (9344,976) right "hit", 699C (9360,976) right normal. Hit columns are collision 57; normal
  are 59 x4 + 56 x10 (left, gate) and 56 x14 (right).
- dmg table 0x800755FC (main program; matches the EXE): 
  0 1 2 5 2 2 2 D - 2 1 2 8 2 2 2 D / 2 8 4 4 4 D 8 D 1 1 2 2 2 2 2 2 /
  2 5 2 2 2 2 2 2 2 2 D 2 D 1 4 8 / 2 1 2 2 3 2 1 127 3 8 4 4 15 5 15 3 / D x15, 127.

## Pseudo code (short)

```
main: 
  if step==9: hurt=WHOLE, tbl=deflect
  elif crystals_lost==3: hurt=WHOLE, tbl = inv ? (kept) : boss
  elif anim in {13,16,28,29,30}: hurt=WHOLE, tbl=deflect
  else: hurt=SHELL, tbl=deflect; r=ContactWeapon; if r: inv=1
        else: tbl = inv?deflect:boss; hurt=BACK
  r=ContactWeapon
  if r<0: sounds 20,32; state 2; 6BF8=1; mega+7A=1
  if step!=9: attack = anim==30 ? SPIN : BODY; ContactMega
  if 0<r<127: inv = (r in 25,26) ? 2 : 30; tbl=deflect
  if r in {3,12,34}: sounds 20,33,120; 6BFC=1; step 9
  inv countdown, flash bit 1 of inv; at 0: tbl=boss
  run step; if step!=2: fight_timer++
THINK: r=rng&15; idx=0
  Hard: pat = r<8 ? 0 : (hp>max/2 ? 1 : 2)
  else: pat = hp>max/2 ? 0 : (r<8 ? 0 : 1)
step 5: face; [sub2] every 8: if n==tbl[lvl]: anim10, sub3 else {n==0: snd34, anim19}; act4=rng&1; n++
        [sub3] 300 -> next
step 6: anim13; anim28 snd61; spin anim30 60 -> Hard? 12 : 3
  3: n>=4 -> 9; |dx|<64 -> vy=+3 (sub6) else vx=+-3 to player (sub4)
  4: wall -> snd63, stop, quake, n++, sub5(60) -> 3
  6: ceiling -> n++, sub7: 60 then vx = +-4 to far half (or 0 if near), g=1/16; sub8: floor -> n++, sub5
  12: n==2 -> 9; vy=5.5, g=0.171875, vx=dx/64 ; 13: floor -> 14: 60, vx=+-4 to player ; 15: wall -> n++, 12
  9: wait sprite 0; snd20,64; anim29; anim16; anim10; 270 -> next
step 7/8: snd36/37 anim13; anim28; spin 60; 3: n>=4 -> 6; face away from middle; 4: vx+=1 to 5,
  wall -> CopyTiles hit column, snd63, quake 90 (right) / 30 (left), n++; 5: 50 -> restore walls, 3
  6..8: open, 270 -> next
step 9: restore walls; anim33; stop; 50; g=1; floor; 50 -> 6BFC=0, anim10; 40 -> next
death: anim32, PlayerEvent(20, mega flip); 127 blink -> sys 26; hidden while sys+7 != 0,
  else blink 6; gone -> soul (type 15 var 0x10 if timer<5401 else 0x11) or stage 12 exit 0x80.
```

## Visual type 23 (var -> table 0x800E98C8, pairs)

- 0/1 crystals (6B58): HP 16, boss table, no attack box. Follow boss x/y/flip. Steps: 0 idle
  (hits on), 1 intro, 2 volley (hits on; fire var+2 when boss 7C==0 and act4==var), 3 shell
  (fire bubbles every 14 / water balls every tbl[lvl] while boss rolls in step 7 / 8), 4 stun wait.
  Lost: bit in 6C04; regrow 16 HP after 660 when both lost (or on Hard); blink last 60.
  A lost crystal still fires (no HP test on the fire code).
- 2/3 missile: 24 updates at 1 px/frame (0/40/80 deg, var 3 mirrored down), 40 still, snd60, face
  player, 4 px/frame horizontal (3 if boss hp > max: never), y tracks player 0.5 px/frame.
- 4/5 bubble: vx +2 / -2, vy 5..8 up, g 0.1875; delete y > floor+64.
- 6/7 splash, 10/11 flash: anim 24 / 26, end with anim.
- 8/9 water ball (Hard step 8 only): up 8 px/frame, no boxes; y < miny-24 -> stop; 50 -> anim27,
  snd59, boxes, vx +-2 (boss facing at spawn), vy -2 (down).

## Open

- weapon ids 2, 11, 33 (stun), sound 20, sys objects 24/26 (stage-logic class 0x8009B7B0).
- collision 57 on the hit wall column: effect for the player (no wall cling? guess).
- anims 28-33 use the next two sprite banks (bank+1, bank+2 with tex 0x540): the export drew them
  from bank 0, so those strips are wrong.
- step 5 sub 1 (0x800F0ED8, walk 3 px/frame to x 9280 / 9024) and step 3 are never entered.
