# Rescue Reploid in stage 7 (Shield Sheldon): notes

Probes: ~/mmx-data/x6/re/nightmare-sheldon/probe_objs.py, probe_recs.py.

- Records 10-17 of enemy_set 0x800FAEE0: stage type 4, id 96..103 (kind 4).
- Status word game+0xD8 + 48 (0x800CCFD8): byte 0 = ids 96 (low nibble), 97 (high nibble).
  sheldon-4 0x02, sheldon-5/6 0x42, sheldon-7 0x32; byte 1 (ids 98, 99): sheldon-7 0x02.
- Live stage[0] in sheldon-5/6: 41 04 61 01, state 6 step 5, hp 13, contact 6, dmg 0x800743DC,
  attack 0x80078908, hurt 0x8007890C, +0x7E 0 (standing), +0x82 0 (faces left), anim tbl 0x80078BEC.
- Visual[2]: 41 18 01 01 = type 24 var 1 (bullet), x 1570 -> 1594 between the states? (no: the
  states are separate shots; speed_x -4.0 px/frame), y 430.
- Muzzle table 0x80011674 group 6: f6 fe 08 fe = (-10,-2) / (+8,-2).
- Snatcher record [18] type 15 id 34 (0x22) at (1768,352): flags 0, kind 0x10 in sheldon-5/6,
  kind 0x20 in sheldon-7. No virus live in sheldon-5/6.
- X souls (game+0xD2): 312, 312, 312, 316 (sheldon-4..7). Kill count 0, 1, 1, 6.
