# Notes: method, evidence, open questions

## Method

1. Built a RAM image: `SLUS_013.95` body at `0x80010000` (file offset + 0x800) and overlay
   `rock_bin/02.bin` at `0x800E9860`. Data reads used the RAM of the save state
   `~/mmx-data/x6/dumps/intro-1_start.state.gz` (offset `0x9025`, 2 MB).
2. Disassembled with a small MIPS R3000 decoder (`disasm/tools/mips.py`) and a function dumper that
   stops at the last `jr ra` (`disasm/tools/dfn.py`, usage: `python3 dfn.py <hex addr> ...`).
3. Read every robot and shot routine, then every helper whose result changes the behaviour (movement,
   gravity, collision, hit and damage, drops, spawn, delete, screen checks).
4. Read the overlay data (animation, boxes, tables) and the main-program damage and drop tables.
5. Cross-checked the names of a few engine functions and struct fields with the reference repos
   (X6 modding headers: `SetAnime`, `DeleteObject3`, `PlaySound`, `mega` at `0x800970A0`, object
   array bases, the `Object` field names). I used them for facts and names only.

## Evidence files (`disasm/`)

| file | contents |
|---|---|
| `robot_overlay02_800eac8c-800eb44c.txt` | all robot and shot code |
| `robot_data_800faadc-800fab7c.txt` | boxes, animation frames, tables; drop tables 14 and 21 |
| `h_small_helpers.txt` | `MoveObject2`, `OffScreenCheck2`, `DisplayCheck`, `DeleteObject`, `DeleteObject2`, `GetVisualObject`, `0x80040290`, `SetAnime` |
| `h_anim_drop_explode.txt` | `AnimeTick`, `DropItem` (`0x8004E680`), explosion (`0x80049E70`) |
| `h_800315cc_apply_weapon_hit.txt` | weapon damage application |
| `h_shotreact_wallcheck.txt` | weapon hit reaction (`0x800319EC`), `CornerCheck` (`0x8002EE10`) |
| `h_80030ecc_contact_mega.txt` | contact damage to the player |
| `h_80030dc0_player_damage_scale.txt` | difficulty and armour scaling of damage to the player |
| `h_800304d8_terrain_box_8002d654_box_overlap.txt` | terrain box set-up, hit box overlap |
| `h_800477e4_spawn_junk.txt` | debris spawn |
| `h_80029d80_stage_spawner.txt` | stage list spawner and its difficulty filter |
| `savestate_objects.txt` | live objects and the robot records in the save state |

## Key facts and where they come from

- Robot update `0x800EAFB0` is entry 1 of the intro main table `0x800FA424` (pointer at `0x8006DC6C`).
- Shot update `0x800EB414` is entry 0 of the intro visual table `0x800FA558` (pointer at `0x8006DD24`).
  The shot is allocated with `0x8002C458`, which scans the visual objects at `0x80093C98`, so type 0
  is looked up in that table. This confirms that the code at `0x800EB23C`–`0x800EB44C` is the shot.
- Weapon ids: the Buster Plus bonus jump table at `0x800110C4` gives +1 to ids 0, 9, 18, 19, 20, 45, 46,
  58. The part bit `0x80` is "Buster Plus" according to the part codes in the X6 recomp project. From
  this I take id 0 = lemon, 9 = half charge, 18/19/20 = full charge shots. This is an inference.
- The difficulty byte is `0x800CCF38` (`GAME + 0x68`). Value 0 makes enemies take × 1.25 and the player
  take × 0.75, so 0 = Easy; ≥ 2 = Hard. The save state has 1 (Normal).

## Verified against the save state

- The save state RAM is identical to the previous run's `ram.bin`.
- Difficulty = 1. Player at (144, 1115), HP 32. Camera of layer 0 at (0, 944). View width 320.
- No enemy object and no visual object is live at this point. Record 25 (x 416) is outside the spawn
  band (camera X + 368 = 368), so it is not spawned yet. Thus I could not check live robot fields.
- The six robot records (25–30) and their ids, including the one dormant robot (record 27, x 800).
- The terminal fall speed global `0x8009719C` = −0x68000.
- All the data tables quoted in `behaviour.md` were read from this RAM.

Not verified dynamically: the timing (153-frame interval, first shot after 11 frames) is a hand
simulation of the code order. A second save state with a live robot, or a trace, would confirm it.

## Open questions

1. Frame rate: I assume 60 object updates per second (NTSC). Check the main loop's VSync count.
2. Item kinds `0x20`, `0x21`, `0x23`, `0x24`, `0x26` (item object type 47): which pickups are they?
   Why does a weapon kill roll two tables (21 and 14) and a crush only one? Table 14 could be Nightmare
   Souls; not checked.
3. Sound names for bank 2 ids 26 and 33.
4. Sub-state 0 (`0x80040290`, a dispatcher by hit-reaction class): can any outside code put the robot
   into it? No robot code does.
5. Weapon ids 7 and 23 have class 2 (no effect) on the robot; id 55 does 127. Which weapons are they?
6. The debris motion (misc object type 3) and the explosion effect (`0x8004A0C0`) are not reversed.
7. The record y values (1080, 1048, 984) with a feet offset of +24 put the feet at y = 1104 etc. I did
   not check the tile map, so I do not know if the robots start on the floor or fall a little at spawn.
8. `CollideCheck` (`0x8002F288`) is only outlined; the exact bit meaning of `collide_flags` (which bit is
   left and which is right) is from the corner order in `CornerCheck`, not from a test.
