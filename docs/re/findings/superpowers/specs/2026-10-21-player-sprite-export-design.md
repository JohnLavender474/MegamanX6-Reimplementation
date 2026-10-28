# Player sprites and animations (X and Zero) — design and plan

Date: 2026-10-21. Status: approved by the user's request ("Build the player sprite exporter").

## Purpose

Export every sprite and every animation of the player character in a save state (X or Zero), in
the same form as the enemy exports: one PNG strip per animation and an `animations.json`.

## Facts this rests on (process log section 72 and this task's probe)

- The player is `mega` (`0x800970A0`). Its fields: `+0x30` animation table, `+0x38` frame table,
  `+0x3C` sprite data, `+0x40` texture (`0x500`, page 5), `+0x42` palette (`0x7800`), byte 0 bits
  `0x60` the cell mode (0 for the player).
- Only one frame is in VRAM at a time. `PlayerFrameStream` unpacks frame `mega+0x47` (the sprite
  number of the current animation step) from the frame table and copies it to the texture position,
  16 tiles (a 64 × 16 halfword rectangle) at a time. So sprite n is drawn from frame n.
- Frame table entry n: bits 20–31 = tile count, bits 0–19 = offset from the table start. The data is
  word LZ (`0x80018BE0`) and unpacks to tile count × 128 bytes.
- All frames are in RAM: X 228 (sprite data count 228), Zero 240 (count 240). The same bytes are in
  `ROCK_X6.DAT` member 87 (X) and 85 (Zero).
- The player code and its animation table are in a player overlay at `0x801EA000`: overlay 0 for X
  (193 animations), overlay 1 for Zero (169 animations).

## Design

- **New module `mmxdemake/wordlz.py`**: `decode(data, pos)` returns the words and the end
  position. It is our own reading of the decompressor at `0x80018BE0`.
- **`animfind`**: `PLAYER_OVERLAY_BASE` and `player_overlay(ram, overlays)`, which needs exactly one
  overlay at `0x801EA000`.
- **`anims.read_table`**: a `limit` argument (default 64), because the player has up to 193
  animations.
- **New module `mmxdemake/playerexport.py`**, CLI `python3 -m mmxdemake.playerexport --state S`:
  1. read `mega`, the player overlay and the frame count (first table offset / 4). The frame count
     must be at least the sprite count;
  2. for each sprite n: unpack frame n, write it into a copy of VRAM at the texture position, and
     draw sprite n with `sprites.draw`;
  3. walk each animation of the table. An animation that fails to walk or that uses a sprite out of
     range is skipped and listed with its reason; the others keep their table index;
  4. find the `ROCK_X6.DAT` member that holds the frame set (the first 4 KB, then the full blob).
     The folder name comes from `catalog/players.json` (`{"members": {"85": "zero", "87": "x"}}`),
     else `player-m<member>`. No member → exit 1;
  5. write `players/<name>/` under the sprite root (default `~/mmx-data/x6/sprites`): `anim-<k>.png`,
     `sprites.png` (all sprites, numbered) and `animations.json` (name, member, fps, cell, origin,
     animations, skipped, source). A new run replaces the folder.
- **`spriteexport`**: `players` becomes an allowed top folder of the shared root, and the numbered
  grid of `sheet` becomes a function that both tools use.

Out of scope: shots, charge effects and the smaller frame sets after the main set; armour members
86, 88 and 89 (they need a state, or a disc-only mode later); the palettes of weapons and armours.

## Plan

1. `wordlz.decode` and its tests (literal, copy, zero run, long form, end).
2. `read_table(limit=…)`, `player_overlay`, and their tests.
3. `spriteexport`: the `grid` function and the `players` top folder; the existing tests still pass.
4. `playerexport` and its tests with a synthetic world (two frames of different texels, a skipped
   animation, a member name from the catalog and one without, no player overlay, root in the repo).
5. Run it on `intro-1_start` (X) and `turtloid-6_zero-player` (Zero); look at the output.
6. Review, fix pass, docs (engine, TODO, process log), commit.
