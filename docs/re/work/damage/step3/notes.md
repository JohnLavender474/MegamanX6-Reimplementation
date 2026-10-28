# Damage step 3: weapon ids, main type 59, part bits, sabre +0x9C (2026-10-22)

Sources: main program, X overlay (00.bin, RAM of `sheldon-1_lemons`), Zero overlay (01.bin, RAM of
`turtloid-6_zero-player`), rock_bin overlays 16, 20, 24. Scripts in this folder:

- `ram.py dis|store|imm <state> ...`: disassemble RAM, find stores to an offset, find immediates.
- `tables.py <state> <addr> <n> [size]`: dump a table.
- `spawns.py <state> lo hi`: SpawnWeapon/SpawnWeaponFrom calls and their type argument.
- `fstart.py <state> addr...`: function start and references (jal or pointer).
- `ovl.py <nn> <base> dis|ref|words`: read an overlay file at its load address.
- `partread.py`: reads of part bits 0x200000/0x400000/0x800000/0x1000000/0x2000000 near `lw 0xEC`.
- `gamefield*.py`, `rmw.py`, `scanfiles.py`: writers of game+0x70/0x74/0x78.
- `type59.py`: main type 59 entry in each state. `glyphfind.py`: glyph-coded text search.
- `text/`: `python3 -m mmxdemake.text` output (all text banks).
- disassembly: `x_*.txt` (X), `z_*.txt` (Zero), `m_*.txt` (main), `ovl20_reward_tbl.txt`.

## 1. Weapon ids

`weapon_types` 0x80071220 (main) holds one address per id for both player overlays (both load at
0x801EA000). So the same address means different code in X's and Zero's RAM.

| id | object | made at | how |
|---|---|---|---|
| 32 | X Blade Armor Mach Dash hit part | main 0x8003F3C0 (SpawnWeapon flag 0x21, var 0) | 0x8003F32C (step 74 start, anim 189, +0x86 = 7, +0xD9 = 0) from the air-dash test 0x800390D8 when form +0xE7 = 3. Update 0x801F2C08: deleted when X is not in step 74; set-up at X's flag-0x80 frame while no_hit; `+0x06` = mega+0xD9 / 30 (0x801F2CA8). +0xD9 counts +1 per update while jump/dash held, to 120 (0x801EAF30, step 74 sub-step 0x801EAECC). So the bonus is 0..4 |
| 33 | Zero Hyoroga icicle (x3, var 0..2) | Zero 0x801ECB64 loop (GetWeaponObject, sb type 33) | step 63 (0x801ECAF0) at frame flag 0x40; update 0x801ED928 |
| 34 | Zero Shoenzan fire wave | Zero 0x801EAF70 (in step 48, 0x801EAE7C) and 0x801EC8E8 (TryShoenzan 0x801EC86C, called from main steps) | skill bit 0x04 (+0xC9), up held (+0x7C 0x04), attack pressed (+0x80 0x10); SpawnSabre(26) then SpawnSabre(34); step 55; update 0x801EDAF4 |
| 49 | X Falcon Armor Giga part | X 0x801EA340 | step 36: every 3-5 updates (mega+0xE1 = rand%3 + 3), var = toggle byte 0x801F4DD8 (top/bottom); update 0x801EE310 |
| 56 | X Blade Armor up-slash | X 0x801EB098 in pose step 76 at frame flag 0x80 | TryShoot 0x8003E6A4: id 43 (Blade full charge) with up held (+0x7C 0x04) and +0x86 != 9 becomes 56, +0x86 = 9 (once until +0x86 is cleared); update 0x801F3200 (+0x9C = 1 at 0x801F32D4) |
| 58 | Zero Z-Buster shot | Zero 0x801EAD10 and 0x801EADD8 (GetWeaponObject, sb type 58) | step 62 (0x801EACAC); update 0x801EDCC8 |

The +0x9C = 1 writers, corrected:
- X 0x801ED964 is in 0x801ED898 = weapon_types[46] (Shadow charged slash), not id 33.
- X 0x801EDB04 is in 0x801EDA64 = weapon_types[47] (Shadow Giga parts), not id 34.
- X 0x801F32D4 is in 0x801F3200 = weapon_types[56] (Blade up-slash).
- main 0x800446A4 is in 0x800445A8 = state 0 of the id-4 table 0x80076170 (Metal Anchor, all vars).
- main 0x80045390 is in 0x800451B8 = state 0 of the id-7/53 table 0x800761BC (Guard Shell), the
  var != 0 branch (the child that var 0 makes with var 1/2/3 by up/down held; Zero always 3).
- Zero 0x801ED600 (sabre set-up). The Zero RAM also shows 0x801F32D4: that is stale X code past the
  end of the Zero overlay (01.bin is 22964 bytes, ends 0x801EF9B4).
- Ids 33, 34, 49, 58 have no +0x9C writer: they cannot hit `dmg_tbl_one_hit` objects.

## 2. Main type 59

Main type table pointer: scratchpad +0x80 (per stage overlay). In all 56 save states type 59 is the
stub 0x80054428. In the overlay files only overlay 24 has a real entry: 0x800F49F0 (table at
0x800F5F88). Stage/area to overlay table: 0x8006DB78 (halfword, stage*4 + area*2): stage 18 area 0
= overlay 24. So type 59 exists only in stage 18.

Type 59 = Gate [likely]: set-up 0x800EF400 places it at (3456, 912), damage table 0x8007573C,
game.boss (game+0x20) = self, boss_hp_max = 48 + table[game+0x8F], PlayerEvent(20, 64). Its step
0x800F4B2C opens TextOpen(1, ...): message 1 of the stage 17/18 text bank (members 225/226) is
Gate's talk ("Most impressive. All of my Reploids have already been destroyed...").

Hits: main state 0x800EF590 saves HP, calls ContactWeapon; ApplyWeaponHit lowers HP, deflects the
shot (WeaponHitReaction -1: tink, spark) and returns 127; then Gate writes back the old HP and
stores the damage (old - new) in byte 0x800F7806. So Gate itself never loses HP from a weapon.
Readers of 0x800F7806: visual type 26 of overlay 24 (0x800F0A24, table 0x800F60BC; switch on var,
35 cases) in five state functions (0x800F0DFC, 0x800F1584, 0x800F1D2C, 0x800F2678, 0x800F2E04):
they add it to their +0x98 and change step at >= 16. Gate's HP drains 4 per update while word
0x800F77DC != 0 (set at 0x800F3778 by visual 26 code when Gate's step != 7); at HP <= 0 Gate goes
to its death set-up (anim 7, state 2).

ApplyWeaponHit tests only obj+0x01 = 59, not the class (0x80031900).

## 3. Part bits

- game+0x70: parts owned. game+0x74 + 4*char: parts equipped (X, Zero). PlayerLoadForm 0x8003D740
  copies game+0x74+4*var to mega+0xEC.
- Parts menu (overlay 16, base 0x800E9860): bit table 0x800F0E74, 24 words, by menu index:
  0x4 0x8 0x10 0x20 0x40 0x80 0x100 0x200 0x400 0x800 0x40000 0x80000 0x400000 0x800000 0x100000
  0x200000 0x1000000 0x2000000 | 0x1000 0x2000 0x4000 0x8000 0x10000 0x20000 (index >= 18: the
  second loop, likely the Limited Parts). Equip/remove 0x800EC35C: confirm (pad 0x840), part index
  game+0x2A, char game+0x29; owned test; toggles the bit; write 0x800EC4A8.
- Owned bits are set only at the stage-result screen (overlay 20, 0x800EA580..0x800EA590):
  game+0x70 |= reward, for each saved Reploid n (status 2 in game+0xD8) with
  `reward_tbl[n]` (main 0x8006D674, 128 words; 0 none, 1 and 2 other rewards, else a part bit).
  Overlay 20 part list 0x800F370C (26 records of 12 bytes: bit, two misc-type-45 var bytes).
  LoadGame 0x8001BA1C and overlay 18 0x800EC... (file 0x3644) copy the save.
- Reploid giving each bit (reward_tbl): 0x200000 -> 110; 0x400000 -> 94; 0x1000000 -> 18;
  0x2000000 -> 122. Reploid ids: stage 7 alt 104-111 (Sheldon), Turtloid 80-95, so 110 = Shield
  Sheldon's stage (alt area), 94 = Rainy Turtloid's stage (area 1). 18 and 122 by the 16-per-stage
  pattern = stages 2 and 8 [likely].
- Save-state check: turtloid-5 (mid-stage, Reploids 80-95 saved): owned 0x00040040.
  sheldon-1 (after the Turtloid clear): owned 0x00450050 = + 0x10 (Reploid 93), 0x10000 (91),
  0x400000 (94). Matches reward_tbl. Equipped = 0 in all states.
- Readers (all code, pattern lui + lw +0xEC): only ApplyWeaponHit (0x8003177C, 0x800317EC,
  0x80031840) and the Zero sabre set-up (0x801ED57C, 0x801ED590, 0x801ED5B8). No X overlay reader.
- Effects:
  - 0x200000: +1 for ids 1-8, 10-17, 52-54 (X special weapons and their parts).
  - 0x400000: +1 for ids 24-31, 37-41 (Zero's sabre hits), only when part 0x80 is off; Zero sabre
    palette 4.
  - 0x1000000: Zero sabre palette 5 (wins over 0x400000), and sabre +0x9C (cuts enemy shots).
  - 0x2000000: +1 for ids 1, 4, 33, 34, 35, 36, 42, 44, 52, 53, 54 (Zero's techniques and the
    shared weapon objects that Zero also makes).
- Names: no part names in any glyph text bank (search of all rock_dat and rock_bin). The names are
  drawn as misc object type 45 (overlay 20, var = 106 + n and a second byte) and misc type 40
  (overlay 16, var 24 + index): graphics. Need a save state of the parts menu or of the stage
  result screen to export those frames.

## 4. Sabre +0x9C

0x801ED5F8 is not a table: it is `lbu a0,5(v1)` with v1 = sabre_tbl 0x801EE68C + 4*(id-24).
sabre_tbl entry = {anim, palette, after-image flag (part 0x800000), b3}. The read is byte 5 = the
palette byte of the NEXT entry (id-23). Set-up 0x801ED3DC (first update, +0x04 = 0), only for ids
whose own palette byte is 3 (ids 24-31, 36-42); needs mega+0x02 != 0 (Zero) and (parts 0x1000000 or
game+0x5F bit 0x20 Black Zero). Values: 3 for ids 24-30 and 36-41; 0 for 31 and 42.
ContactWeapon tests only +0x9C != 0, so 3 works as "can hit dmg_tbl_one_hit"; the +0x9C |= 2 after
a kill needs exactly 1, so it never happens for the sabre.
