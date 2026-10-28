# Notes: Dr. Light capsule (stage object type 26, code in the main program)

Sheet: [`~/IdeaProjects/mmx-demake-tools/docs/enemies/light-capsule.md`](../../findings/enemies/light-capsule.md).

## Method

- Kit: `python3 -m mmxdemake.rekit --name light-capsule --state yammark-1_start.state.gz
  --type 26 --class stage` (copied to `kit/` here; the original stays in
  `~/mmx-data/x6/re/yammark-stage-object-26/kit/`). The kit walked 55 functions; 16 are the object
  (`0x80051158`–`0x80051FE8`), the rest are engine code (solid-object collision, text, CD stream).
- Read in full: the object functions, `0x8002C4C4` (GetStageObject), `0x8002C9D4`
  (DeleteObjectKeep), `0x80018060`, `0x80018774`, `0x800187A0`. Read only the head of
  `0x80021760`, `0x80031DA8`, `0x8003208C`, `0x80031F4C`, `0x80032B34`, `0x80032BE8`.
- Decoded the animation table `0x800791A0` (first 16 entries) from the executable.

## Identification

- `GetSpriteBank(128)`: bank 128 is "Dr. Light capsule and boss gate" (`docs/objects-legend.md`).
- It tests and sets `game+0x69` with the bits of `0x80078E08` and `game+0x5F` bits 1, 2, 3, 5. The
  `mmx-reference` New Game tweak values give Blade = armors `0x05` / parts `0x0F`, Shadow = armors
  `0x03` / parts `0xF0`, and `include/common.h` names `+0x5F` `armors` and `+0x69` `armorParts`.
- It plays stream 28 and opens a text box: the Dr. Light talk.

## Save-state checks

- No live object of type 26 in `yammark-1_start`, `yammark-2_boss`, `yammark-3_caterpirade` or
  `turtloid-1`.
- `game.armors` (`+0x5F`) = 1 in all four states (bit 0, probably the Falcon Armor X starts with).
- `game.armorParts` (`+0x69`) = 0 in `yammark-1_start` and `0x08` in `yammark-2_boss`,
  `yammark-3_caterpirade` and `turtloid-1`. So the player took this capsule (`id` 3 → bit `0x08`) in
  that run [likely]: consistent with the `id` → bit table.

## Key facts

- Approach box: |player x − x| ≤ 64 and −104 ≤ player y − y ≤ 16.
- Lid offset: y − 78; lid terrain box (0, 0, 32, 12).
- Effect objects (class effect, flags `0x49`, parent at `+0x50`): type 35 (`var` 3–6, 16, 17) at the
  set-up; type 36 `var` 0 at step 6; type 37 `var` 0–3 at step 7.
- `0x80018060(n, vol)`: copies a 16-byte entry of `0x8006E58C` (stream table), seeks (`0x800646E4`),
  starts (`0x80058734`), sets the volume (`0x800187A0`), stream state `0x800CD40C` = 1.
- `0x80018774(m)`: when a stream plays, `0x800E2E1C` = m and `0x80097418` = 1 (stop request).
- `0x8001854C`: picks the stage music from `0x8007093C` by stage, area and character, then
  `0x80018060`; it does nothing for stage/area `0x010F`.

## Position

Record 106 at (1800, 776). `stage.png` is black there (no art exported). `collision.png` has the first
solid pixel below at y 896, so the capsule drops about 104–120 px at spawn.

## Open questions

See the sheet.
