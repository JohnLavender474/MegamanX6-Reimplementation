# Zero Nightmare (main type 53, boss overlay 43): notes

## Method

- Kit: `PYTHONPATH=. python3 -m mmxdemake.rekit --name zero-nightmare --state turtloid-5_zero.state.gz --type 53`
  (76 functions, 21 state tables, neighbour visual type 19). Output in `kit/`.
- Read `kit/code.txt` in full, `kit/data.txt`, `--at 0x8003c6a4`.
- Save-state check: `peek.py` (scratch) read game, mega and the boss object.
- Sprite strips viewed: `~/mmx-data/x6/sprites/enemies/zero-nightmare/anim-*.png`
  (stacked copy: `~/mmx-data/x6/notes/zero-nightmare/anims-misc.png`).
- Reference (facts only): MegaManX6Recomp damage-rules manifest lists "Nightmare Zero" HP
  48 / 50 / 52 / 56 for Lv.1-4. Matches 48 + {0,2,4,8}[boss_level].

## Save-state values (turtloid-5_zero)

- difficulty 1, boss_level 0, game+0x8D = [6, 7], boss_hp_max 48, stage 21 area 1, game+0x5F = 0x01.
- player X (char 0), step 2, (1344, 875); hurt box (-10,-17,19,38); terrain (0,2,11,19).
- boss: state 1, step 5, sub 1, HP 48, contact 10, (1446, 872), pattern 4 index 1, damage table 0x800750FC.
  Pattern 4 = far + same height + HP > half + r >= 12 + not Hard. Consistent.
- Room points: fe284 = 1280 (minx+16), fe286 = 1568 (minx+304), fe28a = 1488 (minx+224),
  fe288 = 896 (miny+192 = floor), fe28c = 864 (miny+160), fe28e = 672 (miny-32). Camera min (1264, 704).

## Tables

- step table 0x800FE140: 0 HitReactionDispatch, 1 nop, 2 intro, 3 idle (nop sub), 4 THINK 0x800FA650,
  5 dash 0x800FC508, 6 warp-to-player 0x800FC800, 7 buster x2 + wave 0x800FC8BC, 8 rapid spread 0x800FCC18,
  9 ground punch 0x800FCEE8, 10 slash 0x800FD0BC, 11 warp-far + crescents 0x800FD264, 12 stun-warp 0x800FD320.
- patterns 0x800FE0B4 (6 bytes): 0 [7,7,5] 1 [7,7,6,10] 2 [7,8,6,10] 3 [7,8,5,10] 4 [5,10] 5 [5,11]
  6 [6,9] 7 [10,5] 8 [9] 9 [5,10] 10 [5,11] 11 [5,9]; each ends with 4.
- jitter table 0x800FE114: -80,80,-72,72,-64,64,-56,56,-48,48,-40,40,-32,32,-24,24.
- HP bonus 0x800FA004 {0,2,4,8}; crescent count 0x800FA008 {1,2,3,4}.
- Boxes 0x800FDE30.. : terrain (0,0,24,24); normal (-20,-14,32,38); slash attack (-80,-30,60,54);
  slash hurt / recover attack (-26,-10,28,34); punch (-30,-10,24,34).
- Shot boxes 0x800FE1F4..: shot early (-6,-9,18,18); shot (-24,-9,36,18) / hurt (-24,-11,32,22);
  wave (0,-16,18,32) / hurt (-6,-12,24,24); pillar (-12,-24,24,36) / hurt (-12,-28,24,40);
  crescent (24,-66,16,132) attack = hurt.

## THINK (0x800FA650) pseudo code

```
r = rng & 15; idx = 0
if |px - x| <= 64: pat = hp > max/2 ? 7 : 8
elif |py - y| < 48:
    if hp > max/2: pat = r<6 ? 0 : r<12 ? 1 : hard ? 5 : 4
    else:          pat = r<4 ? 2 : r<8 ? 3 : r<12 ? (hard ? 5 : 4) : 6
else:
    if hp > max/2: pat = hard ? 10 : 9
    else:          pat = r<8 ? (hard ? 10 : 9) : 11
step = patterns[pat][idx++]
```

## Visual type 19 vars

0 dash jet (follows boss, waits boss anim flag 9, anim 9); 1 buster shot straight; 2 buster shot random
angle (-128/0/+128 of 4096); 3 muzzle flash anim 11; 4 aimed saber wave; 5 straight saber wave;
6 ground pillar (8, by boss act4 0..7); 7 ground burst anim 14; 8 big crescent anim 15; 9 spark anim 16.
Effect type 6 x3 (var 0,1,2, chained by +0x5C): after-images while boss+0x9A = 1 (step 5) [likely].

## Open

- weapon id 50 (stun trigger), game+0x5F bit 0x20 at boss level 3, sys objects 24 / 26,
  crescent box vs a dashing player, after-image routine table (kit grouped 0x800FDC6C.. into 0x800FE26C).
