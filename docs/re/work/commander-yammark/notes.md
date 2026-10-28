# Commander Yammark: working notes

## Method

- Kit: `python3 -m mmxdemake.rekit --name commander-yammark --out ~/mmx-data/x6/re/commander-yammark/kit`
  (export `enemy-c7cfecfbf7`, save state `yammark-2_boss.state.gz`, main type 49, entry
  `0x800F40C8`). 85 functions, 18 state tables, 1 neighbour (visual 15), 1 animation table.
- Read: `summary.md`, `calls.txt`, `code.txt` (all of it), `data.txt` (all of it).
- Extra reads with `--at` or by hand: `0x80025FD8` (boss level), `0x80071D80` (level table),
  `0x800179A4` (`SetAnime` writes `+0x17`), the save-state objects, the damage table bytes.
- The drone main (`0x800F1B54`–`0x800F3AD8`) is one function with six `jr` switch tables
  (`0x800E9910` mode, `0x800E9928` step 0 subs, `0x800E9940` entry x by slot, `0x800E9958` step 5
  subs, `0x800E9980` column y by slot, `0x800E99A0` step 6 subs). The kit shows it as one 2,000-line
  block (kit TODO: switch tables).
- Visual type 15 uses a seventh table `0x800E99B8` by `var`.

## Save-state check (`yammark-2_boss`)

- Boss slot `0x8008EF48`: `var` 0, state 1, step 5, step2 3, HP 32, (5395, 2011).
- Three drones `var` 1, state 1, step 3, step2 1, HP 8, around the boss.
- `game+0x8C..+0x8F` = 32, 7, 7, 0 (HP max 32, `+0x8D` = `+0x8E` = 7, level 0). Difficulty 1.
- Globals: `0x800F7E0C` = 6, `0x800F7E0E` = 2 (mode 2), `0x800F7E10` = 3 (alive), `0x800F7E20` =
  1, 1, 1 (slots 0–2 used), `0x800F7DE0` = 0x25 (fight timer), `0x800F7DEC` = boss.
- Room: target min x 5104, min y 1904. Points: x 5120/5440 (limits), 5152/5408 (side points), y
  2000/2054 (heights).
- Player step 2 at (5184, 2075).

So the save state is in step 5 sub 3 (summon at the side point), drones circling (mode 2, step 3).
That matches the code.

## Key facts and where they come from

| fact | where |
|---|---|
| HP 32 + {0,6,12,16}[level] | `0x800EF810` (stack copy of `0x800E990C`, index `game+0x8F`) |
| level = 3 in stage 12, else `0x80071D80[game+0x8D+char]` | `0x80025FD8` |
| pattern choice | `0x800EFD38` |
| patterns | `0x800F7BA0` |
| drone pairs (3,1) (4,2) (6,3); Hard −2 | `0x800F7BD0`, `0x800EFB20` |
| i-frames 30 / 2 (ids 24, 25) | `0x800EFC0C` |
| stun ids 8, 17, 44 | `0x800EFC3C` (results 9, 18, 45 = id + 1) |
| summon count = pair.drones − alive | each sub 3 (`0x800F0284` etc.) |
| drone circle 32 px | `sra 7` of `rcos`/`rsin` (4096 / 128) |
| 546 per update | `addiu 546` on `work.8A` (u16 angle) |
| lance every 75 updates | drone step 5 sub 4: boss `work.7E` ≥ 75, reset to 0; set to 60 when the column is ready |
| knocked drone → 3 HP | drone step 5 sub 8 sets `0x800F7E30` = 30; boss `0x800F07A8` takes 3 HP when it is 30 |
| soul 0x10 / 0x11 by fight timer < 5401 | `0x800F1904` |
| drone shot speed 3.75 / Hard aimed 3 | `0x800F3BE4` (×125/100; 48 × `rcos`) |

## Arithmetic used

- `x * 0x55555556 >> 32` = x / 3. `x * 0x88888889` = x / 30 (with the shift). `x * 0x51EB851F >> 37`
  = x / 100 (with ×125 before: ×1.25).
- Speeds: `rcos << 5` = 2 px/frame at full scale; `×45` (`(3v<<4)−3v`) = 2.81; `×48` = 3; `×96` = 6.

## Open questions

See the sheet. Also: the boss's `+0x8E` in step 6 (the swing direction of `act4`) is reused by the
drones as their own `+0x8E` (a target y), so the name `self+0x8e` means different things.
