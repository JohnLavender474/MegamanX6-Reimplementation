# boss_choice (0x800CD30B = game+0x43B)

## Writers
- 0x8001BBB0 (main, in 0x8001BA1C LoadGame): game+0x43B = save[+0x3F]. 0x8001BD3x (SaveGame) reads it back (0x8001BE84). Overlay 18 0x800ECFEC is a copy (memory-card load).
- Overlay 16 (in-stage? game-mode table 0x800F1068, index game+0x01). Mode 2 (0x800ECB04), when 0x80097424 == 0:
  - game+0x66 == 1 -> mode 9 (0x800ED534/0x800ED508): game+0x66 = 2.
  - 6 <= game+0x158 < 8 -> mode 10 (game+0x158 = 0xFF): 0x800ED5DC writes game+0x66 = 3, boss_choice = 2 (if 0x8008EAFC == 0).
  - boss_choice == 0 and game+0x5F & 0x10 -> mode 11: 0x800ED6B4 writes boss_choice = 1 (if 0x8008EAFC == 0).
  - else mode 3.
- Overlay 20 0x800EE7E8 (after-stage, memory-card save overlay): game+0x26 = stage left (0x800E3141), game+0x27 = 1 if not a clear (0x800E3140); written at 0x800EEA54.
  if game+0x27 == 0 and game+0x26 >= 19 and boss_choice == 1: boss_choice = 2, flag "Gate lab" event.
  Same flag: stage 1..8 cleared and game+0x60 == 0xFF (all 8 Mavericks), or souls (game+0xD2/0xD4) >= 3000.
  If game+0x66 < 3 and flag: queue game+0x158[n] = 6 (X) / 7 (Zero, game+0x38 != 0).

## Stage-exit handler 0x8001E32C (main), stage >= 19 cleared
- boss_choice 0: game+0x00 = 19, game+0x1D = 2, game+0x5F |= 0x10 (Zero unlocked).
- boss_choice 1: game+0x00 = 19, game+0x1D = 4.
- else: game+0x00 = 11.

## Readers (stage >= 19)
- 0x800143B4: 0x8006E1A4[stage] = 193 / 197 / 195 (file id, loaded with 0x8001642C). Stage 0 (intro) and stage 17 also use 197.
- 0x80021B18: boss music (index 2) = 22 / 8 / 21.
- 0x80052998: 0x80079F20[stage] = 138 / 139 / 140.
- 0x80052E64, 0x80052EE0: BossSpawner (overlay 43/44/45, type 53/55/63).

## Hidden areas
stage 19..22, area 0/1 -> overlays 25..32 (stage 21 area 1 = overlay 30). Each has one stage-logic type 13 record:
19/0 (2352,336) ov25; 19/1 (2928,1104) ov26; 20/0 (752,336) ov27; 20/1 (3312,384) ov28;
21/0 (992,368) ov29; 21/1 (1360,864) ov30; 22/0 (2640,384) ov31; 22/1 (2000,384) ov32.

## Boss identity
- 0 Zero Nightmare (save state). Clear unlocks Zero (game+0x5F bit 0x10).
- 1 High Max: overlay 44 shares 5123 of 5437 opcode 10-grams with the intro overlay (around intro main type 55, 0x800F5170); file id 197 = intro and stage 17 (Gate lab 2, High Max). Clear opens Gate's lab (TASVideos: "beating High Max in a secondary area").
- 2 Dynamo: the choice after Gate's lab opens (by elimination and the wiki rule).

## game+0x5F (save +0x3B) bits: 0x01 X, 0x02 (armour set, ov20), 0x04 (armour set, ov20), 0x08 (Ultimate, with 0x5E=4), 0x10 Zero, 0x20 (Black Zero?).
