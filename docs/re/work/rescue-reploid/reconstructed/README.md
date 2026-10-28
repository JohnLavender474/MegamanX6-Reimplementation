# Reconstructed source (plausible, not original)

My guess at how the original C for the rescue Reploid (stage object 4) and its shots (visual
object 24) could have looked (Psy-Q SDK, PS1). It is not compiled and it is not a matching
decompilation.

## Layout

| file | contents |
|---|---|
| `include/object.h` | copied from Meta Wheel F |
| `include/game.h` | Meta Wheel F's, plus the `GAME` fields `pause[10]` (`+0x10`), `lives` (`+0x39`), `max_hp[2]` (`+0x5B`), `armors` (`+0x5F`), `tank[2]` (`+0x63`), `armor_parts` (`+0x69`), `tanks` (`+0x6A`), `reploid[16]` (`+0xD8`); `frame_count`, `mega_sub`, the Reploid status helpers, the text and stream calls |
| `include/sound.h` | adds the bank 0 ids 13, 21, 24 and the bank 2 ids 46, 65 |
| `src/st03/m04_rescue_reploid.c` | the Reploid and its shots |

In the binary the code is in the main program: the Reploid at `0x8004E800`–`0x8004F2EC` and
`0x8004F834`–`0x800500C0`, the shots at `0x8004F2EC`–`0x8004F834` and `0x800500C0`. The folder name
`st03` only says where it was studied (overlay 03, Commander Yammark).

## What is certain (from the binary)

- Every constant: the bank 141, the boxes, HP 13, contact 6 / 4 / 3, the heal table {2, 4, 8, 16},
  the timers 4, 80, 130, 60, 20, the speeds 8, 4 (× 2 on Hard) and 3 px/frame, the gravity `0x1800`,
  the division by 70, the screen margins, the sounds, the animation numbers, the status values.
- The control flow of every function and the three dispatch tables.

## What is a guess

- All names.
- The `work[]` layout (the binary uses `+0x7C`…`+0x84` directly).
- The tex/clut set-up lines are a literal transcription, not a likely original form.
- `mega_sub` for the objects at `0x800CD340` / `0x800CD3F0`, and `BombTrail` for `0x8004A2B4`.
