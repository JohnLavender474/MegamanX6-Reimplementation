# Nightmare Press (Metal Shark Player, stage 4 area 0, overlay 06): notes

## Method

- Save state `shark-1_ride.state.gz` (stage 4 area 0, X form 1 in a Ride Armor, camera (1885, 224)).
- Overlay 06 matches RAM at `0x800E9860` (16384 of the first 16384 bytes).
- Kit: `python3 -m mmxdemake.rekit --state shark-1_ride.state.gz --type 9 --class stage --name shark-crusher`.
  The kit walked only the entry of stage type 9 (`0x800E9E98`); the state table `0x800F35FC` was read
  with `--at`.
- Probes (`probes/`): `pool.py` (stage-object pool), `live.py` (layer cameras, player, actor2),
  `tables.py` (class update tables from the scratchpad), `records.py` / `fixed.py` (record lists),
  `refs.py` / `refs2.py` (code that addresses a RAM range, with base registers tracked),
  `lst.py` (linear disassembly from the state RAM), `mainpool.py` (other pools), `floor.py`
  (collision under the press, from the stage agent's `layout.json` and `collision/m098`).
- Disassembly listings kept: `probes/main-all.txt` (main program), `probes/ovl6.txt` (overlay 06),
  `probes/states.txt` (type 9 states), `probes/crusher-states.txt`, `probes/main-46d80.txt`,
  `probes/hitcheck.txt`, `probes/playerupdate.txt`, `probes/solidobject.txt`.

## How the press was found

1. The type 9 stage objects of `var` 0 sit every 240 px at y 256. Their sub-step (`0x800E9D5C`) sets
   `y = y0 + layer0.y − layer1.y` (`camera+0x0E`, `camera+0x62`). In the state, 192 + 224 − 160 = 256.
   So the ceiling is BG layer 1 and the type 9 objects are its solid body.
2. Layer-1 mode 9 (`0x800297BC`, from `0x80071E88`) sets only layer-1 x. A scan for accesses to
   `0x8009725A` (layer-1 y) found the writer `0x80046F10`, in `0x80046E14` (main program).
3. `0x80046E14` is entry 2 of the table `0x8007136C`, used by the loop `0x800208EC` over 4 slots of
   `0x30` at `0x800C4778` (record kind 7, allocator `0x8002C950`). Slot 0 is live: type 2, `var` 32.
   Its record is `fixed_set_tbl` stage 4 area 0, record 59 (`0x800F300C`: `00 02 20 07 00 00 00 00`).
4. Live values: state 1, sub-step 1, timer 114, count 0, speed −1.0, offset 96.0, saved 96.0.
   Layer-1 y = 224 + 32 − 96 = 160, as in RAM [certain].

## Key facts and their source

| fact | where |
|---|---|
| press update, layer-1 y, carry, crush test | `0x80046E14`–`0x800472CC` |
| press states 0, 1; sub-steps 0–5 | `0x800762C0` (2 states), `0x800762C8` (6 sub-steps) |
| travel 80 px = `0x500000 / |speed|` updates | `0x800474A0`, `0x8004750C`, `0x8004759C` |
| speeds: intro 2.0 × 8; 1.0 (difficulty < 2) or 1.25 | `0x800472E4`, `0x80047428`, `0x800476AC` |
| waits: 180 first top, 50 top, 40 + 120 (cp 0) / 40 + 50 bottom, 50 hold | sub-steps |
| crush push: `mega+0xE6` = 127, `mega.y += d`, `mega+0x71 |= 4` | `0x80047238`–`0x80047288` |
| `mega+0x89` = `mega+0x70` | `mega+0x71` | `PlayerUpdate` `0x80034FFC`–`0x8003500C` |
| squeeze kill (no `+0x61` test) | `PlayerHitCheck` `0x800392CC`–`0x800393F4` |
| frame order | `0x80020C40`: ride (`0x80020A08`), player (`0x80034DCC`), `WeaponLoop`, press (`0x800208EC`), …, stage objects (`0x800205A0`), …, `StageLogicLoop`, `CollideCheck(mega)`, layer cameras |
| Ride Armor sets `game+0x26` on floor + ceiling | `0x800F2414` (actor2 type 0, table `0x80071380`), `0x800F24C8`–`0x800F24F8` |
| press holds and rises on `game+0x26` | sub-steps 1 and 2 (`0x80047468`, `0x80047564`) |
| `game+0x27` = 1 while the press rises; type 9 then clears `mega+0x71` | `0x80047684`; `0x800E9B54`–`0x800E9B8C` |
| X box stand (0, 2, 11, 19), dash (0, 6, 11, 15); ride box (0, 3, 15, 29) | `0x80073D50`, `0x80073D58`, `0x800F3A44` |
| type 9 boxes | `0x800F35A8` (hurt), `0x800F35D4` (terrain) |

## Checks against the save state

- Layer-1 y = layer-0 y + `var` − offset: 224 + 32 − 96 = 160 [certain].
- Type 9 `var` 0 y = 192 + (224 − 160) = 256 [certain].
- Ride box bottom: y 384 + 3 + 29 = 416 = the floor top under it. So "box bottom = floor top"
  [certain for this case].
- `mega+0x68` = 0 while X rides [certain].
- `game+0x26` = 0, `game+0x27` = 0, difficulty 1 (Normal), checkpoint 0.

## Geometry (checkpoint 0, area 0)

- Kill line (world y) = `layer0.y − layer1.y + 256` = offset − 32 + 256 = **offset + 224**.
- Offset: 0 at the start, 16 after the intro (top), 96 at the bottom. Kill line 240 → 320.
- Press body (type 9 `var` 0): centre y = 160 + offset − 1, half height 64, so bottom = offset + 223:
  the kill line is 1 px under the body [likely: edge rounding not checked].
- Art (layer 1, from `stage.ora`): x 1600–5056, y 128–256 at offset 32 (the layout position).
  The type 9 bodies cover x 1456–5056 (15 × 240 px).
- Floor tops under the press (collision value 56): 320, 352, 368, 384, 416. Row of value 57 at y 208
  (the ceiling above the press; value 57 stops the super-jump ceiling hold, `docs/players/air.md`).
- X standing on floor F: head = F − 38; dashing: F − 30. Killed when offset + 224 ≥ head:
  - F 320: stand at offset ≥ 58, dash at offset ≥ 66 (dies while the press falls);
  - F 352: stand at offset ≥ 90 (the last 6 px), dash: never (needs 98);
  - F 368, 384, 416: never. These are the safe gaps.
- Ride Armor on floor F: top = F − 58. It touches the press at offset ≥ F − 281 (about):
  F 320 → 39, 352 → 71, 368 → 87; F 384, 416: never.

## Open questions

- `0x80032B34`, `0x80032BE8`, `0x8003208C`, `0x80031F4C`: called by type 9 around `SolidObject`;
  not read (probably the solid tests for other bodies: enemies, shots).
- Press: `layer0+0x34` = 0 when `layer0+0x20` ≠ `layer0+0x22` (camera flags; effect not read).
- `mega+0xE0` = 2 stops the press and the type 9 bodies (meaning of the value not known).
- The other stages that use the press code: stage ≥ 19 (checkpoint 0, kill line +240, x < 2896)
  and a stage < 19 at checkpoint 2 (x bands 1857–1919, 2304–3776, 4240–6016; kill line +2001,
  +2065, +2097). Which stages: not checked (no save state).
- Area 1 of stage 4 (kill line +240, x < 4257) and type 9 `var` 2 / `var` 112: no save state.
- Ride Armor HP when it props the press: the ride code does not read `mega+0xE6` in `0x800F2414`;
  the ride states were not read in full (Ride Armor agent).
