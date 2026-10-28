# Nightmare Effect: stage select highlight vs effect

- Overlay 16 (stage select, base 0x800E9860), pick code near offset 0x1A18:
  for k in 0..2: t = tbl_0x800F0F14[(stage-1)*3 + k]; game+0x168[t] = stage; game+0x465[k] = t.
  Table (stage 1..8): [3,7,7] [4,4,4] [2,8,8] [3,5,8] [4,7,7] [1,5,5] [2,6,6] [1,6,6].
- Stage start (0x8001E914): nightmare_effect (game+0x43A) = game+0x168[stage].
- New game (0x8001E088): game+0x168[0..15] = 0.
- Portrait modes (overlay 16, 0x800E9B60..): mode = 3 if Maverick cleared (game+0x60 bit) else 1;
  then for each non-zero byte v of game+0x465..467: mode[map_0x800F0BBC[v-1]] = 5 (Nightmare highlight).
  The highlight reads only the 3-byte list of the LAST pick, not game+0x168.
- States: yammark-1 array [0,0,0,1,0,0,0,1,0] list [3,7,7]; turtloid-1 array [0,6,0,1,0,6,0,1,0]
  list [1,5,5]; sheldon-* array [0,6,7,1,0,6,7,1,0] list [2,6,6], effect now 1.
- So the write is at the pick (yammark-1 has the array set before any clear), not at a clear.
- Sheldon (7) got value 1 when Yammark (1) was picked; Turtloid's pick replaced the list with 1,5,
  so the stage select before Sheldon showed stages 1 and 5 purple, not 7.
