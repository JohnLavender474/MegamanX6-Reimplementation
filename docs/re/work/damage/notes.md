# Damage research notes (2026-10-22)

Method: `python3 -m mmxdemake.rekit --state sheldon-1_lemons.state.gz --at <addr>` for
0x80030DC0, 0x80030ECC, 0x80031474, 0x800315CC, 0x800319EC, 0x80032B34, 0x800312DC, 0x8003D07C,
0x8002D868 (disassembly in `disasm/`). `callers.py <extracted>` counts the jal callers in the
executable and the 59 overlays.

Evidence from the save states:
- All weapon objects in the states (ids 0, 9, 6, 79) have +0x9C = 0.
- game+0x9C = 15 in X states, 6 in Zero states (strongest hit).
- intro-2_junkroid: HP 31, game+0x98 = 1, hit class 4 (Falcon).

+0x9C = 1 writers (corrected in step3/notes.md: 0x801ED964 is id 46, 0x801EDB04 is id 47): X overlay 0x801ED964, 0x801EDB04, 0x801F32D4 (id 56); main
0x800446A4, 0x80045390 (bit 1 cleared at 0x800456B0); Zero overlay 0x801ED600 (sabre, needs part
0x1000000 or armors 0x20).

Result in the repo: `docs/damage.md`.
