# Ground Scaravich routing: work notes

- Kits: `kit/` (main 28 crystal), `kit-main-12` (section goal), `kit-main-37` (not read),
  `kit-stage-3` (totem pole), `kit-stage-12` (grey block).
- `stageexit.txt`: disassembly of StageExit 0x8001E32C.
- `records-s5.txt`: both record lists of stage 5.
- `pseudo.c`: pseudo code of the routing.
- Probes (run from `probes/`, not from the scratchpad):
  - `ramdis.py STATE ADDR N`: disassemble RAM.
  - `records.py STATE STAGE [TYPE]`: record lists.
  - `rngsim.py STATE1 STATE2`: GetRNG step, calls between two states, period.
  - `sections.py`: block places of each section (coin flips).
  - `tables.py`: route, section, return and place tables.
  - `blocks.py`: reads the 6 block places of section 2 in both states and matches the RNG
    sequence (matches at calls 35 and 921 of 1176).
  - `xref.py`, `st2f.py`: call sites of GetRNG, writers of the RNG state and game+0x2F.
- Results: see `docs/stages/scaravich.md` in the repo.
