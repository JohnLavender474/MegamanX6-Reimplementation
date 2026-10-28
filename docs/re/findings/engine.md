# X6 engine: shared rules for objects

Game: Mega Man X6, PS1, USA v1.1 (`SLUS-01395`). This page has the rules that every enemy and object
uses. The behaviour sheets in `docs/enemies/` refer to this page and give only what is particular to
one object. The level data of each stage is in `docs/stages/`. The record format and the kinds are in
`docs/objects-legend.md`.

The addresses are main-program RAM addresses. The names are in `catalog/helpers.json`.

## Conventions

- **Confidence tags:** **[certain]** read directly from the code or data; **[likely]** strong
  inference; **[guess]** plausible, not checked.
- **Units:** 1 px = 1 game pixel; 1 block = 16 px. Positions and speeds are 16.16 fixed point:
  `0x10000` = 1 px/frame. Gravity and acceleration are in px/frame².
- **Axes:** screen y grows **downwards**. The field `speed_y` is positive **upwards**: the engine
  does `y -= speed_y`. Gravity is subtracted from `speed_y`.
- **Frame rate:** **one update per VSync, 60 per second on NTSC, while the work fits in one
  frame** [likely; see "Main loop"]. Object code changes timers by 1 and positions by the speed once
  per update, with no delta time. "Frames" and "updates" mean the same in the sheets.
- **Directions:** the engine's 32-way directions are 0..31: 0 right, 8 up, 16 left, 24 down
  (`AimAt`, `SetSpeedDir`, table `0x80073760`). The Psy-Q angles of `rcos`, `rsin` and `ratan2`
  are 0..4095: 4096 = one turn; 0 right, 1024 up, 2048 left, 3072 down; `rcos`/`rsin` give 4096 for 1.0.
- **Facing:** `flip` (`+0x15`) is 0 = left, `0x40` = right. Most objects use "face the player":
  `flip` = `0x40` when the object's x is less than the player's x, else 0. The rule for an equal x
  is not the same in all objects (Junkroid: right; Killer Moth: left).
- **Boxes** are 4 bytes `{s8 ox, s8 oy, u8 w, u8 h}`:
  - attack box (`+0x50`, contact with the player) and hurt box (`+0x54`, weapon hits): `(ox, oy)` is the
    top-left corner relative to the origin, `w × h` the size. Facing left, x is `X + ox … X + ox + w`.
    Facing right it is mirrored: `X − ox − w … X − ox` (`BoxOverlap`, `0x8002D654`) [likely]. y is
    `Y + oy … Y + oy + h`. `BoxOverlap(a, b)` tests `a`'s attack box against `b`'s hurt box, and
    returns 0 while `b+0x61` (invincibility) is not 0 [certain].
  - terrain box (`+0x68`): `(ox, oy)` is the centre, `w` and `h` are **half** sizes
    (`TerrainBoxSetup`, `0x800304D8`).
  - A null box means "no test": no contact, no weapon hits, or no terrain collision.
  - How the boxes and the contact state work, compared with the Maverick bodies and fixtures:
    `docs/contact-model.md`.

## Main loop

The program has two levels. The **main thread** runs the render loop `MainLoop` (`0x800120A0`,
called from `0x80054B78`). The game logic runs in **tasks**: BIOS threads that the main thread
starts once per pass. Each task does its work for one frame, then gives control back with
`TaskWait(1)`. So one pass of `MainLoop` is one game frame [certain for the code].

```text
SystemInit (0x80012658): GPU, pads, CD, sound; RNG = 3383; TaskStart(0, BootTask)
│
└─► MainLoop (0x800120A0), main thread, one pass per frame
    1  VSync(0)                                  wait for the next vertical blank
    2  PutDispEnv, PutDrawEnv, DrawOTag          the buffer that the tasks filled in the last pass
    3  draw_buffer ^= 1; ClearOTagR(OT, 32)      the other buffer, for this pass
    4  ShakeBorders; link the shake primitives   game+0xD0, +0xD1
    5  CdStreamPoll, MusicStreamUpdate           CD and XA music
    6  PadRead                                   pad 1 and pad 2 words; actuator set-up
    7  VramQueueReset
    8  frame_count += 1
    9  RunTasks ───────────────────────────────► task 0  BootTask, then FrontEndTask
   10  RunUploadQueue                            task 1  GameModeTask:
   11  VSync(1) → scratchpad +0x70                  loop { game_modes[game+0x00](game);
   12  DrawSync(0)                                         TaskWait(1) }
   13  VramQueueFlush, PaletteUpload,            task 2  FadeTask
       StageTextureSwap                          task 3  WhiteFadeTask
   14  DrawSync(0)
   15  RumbleUpdate, SoftReset
   └── back to 1

game mode 10 (stage play, 0x8001E9EC): play_submodes[game+0x01]
    0  PlayFrame (0x8001EA28): pause check; GetRNG(); GameFrame(); PaletteFxRun (mega+0xE4 ≠ 0); DrawStage()
    1  PlayExit  (0x8001EB48): GameFrame() and DrawStage() while the fade-out runs; then StageExit
    2  Pause     (0x8001EC40): PlayerInput(); the pause menu (0x80034830)

GameFrame (0x80020C40): SolidRectsBuild → PlayerInput → Actor2Update → PlayerTick → WeaponLoop
    → Kind7Loop → MainObjectLoop → VisualLoop → EffectLoop → StageObjectLoop → MiscLoop
    → RayLoop → ScrollSpawn → StageLogicLoop → TextUpdate → CollideCheck(mega)
    → the cameras of layers 0, 1, 2 → PaletteFade → PlayerPartsLoop → PlayerImageLoop
    → PlayerFrameStream(mega, 320, 0)
```

### The render loop [certain for the call order]

- **Double buffer.** `draw_buffers` (`0x800C4578`) holds two buffers of `0xF0` bytes: `DISPENV`
  at `+0x00`, `DRAWENV` at `+0x14`, and an ordering table of 32 entries at `+0x70`. Scratchpad word
  `+0x00` (`draw_buffer`) is the index (0 or 1). `draw_buf_ptr` (`0x8009B7A0`) points to the
  buffer for this pass. `DrawOTag` (step 2) starts at entry 31 (`+0xEC`), because `ClearOTagR`
  links the table in reverse. The GPU draws the buffer of the last pass while the tasks fill the
  other buffer [likely].
- **The Psy-Q and BIOS calls** are named from their debug strings and the BIOS call numbers
  [certain]: `VSync` `0x80060964`, `DrawSync` `0x80066150`, `PutDispEnv` `0x80066980`,
  `PutDrawEnv` `0x800667B4`, `DrawOTag` `0x80066744`, `ClearOTagR` `0x8006663C`, `SetDispMask`
  `0x800660B8`, `LoadImage` `0x800663FC`. The thread calls are BIOS B(`0x0E`) `OpenThread`
  `0x80069E74`, B(`0x0F`) `CloseThread` `0x80069E84`, B(`0x10`) `ChangeThread` `0x80069E94`, and
  syscalls 1 and 2 (`EnterCriticalSection` `0x80069ED4`, `ExitCriticalSection` `0x80069EE4`).
- **Uploads to VRAM happen after `DrawSync`** (step 13), so they do not change textures while the
  GPU draws. Code queues 64 × 16 uploads in `vram_queue` (`0x800C11C0`, 8 entries of 12 bytes,
  pointer `vram_queue_ptr`); `VramQueueFlush` (`0x80017A88`) sends each entry that has a data
  pointer with `LoadImage`. `PaletteUpload` (`0x80017C40`) sends the palette buffers that
  `bg_refresh` (`0x800C4560`) marks (bits `0x01`, `0x02`, `0x08`) to the rectangle at
  `0x8006E360`, then clears `bg_refresh`.
- **Screen offset.** `ShakeBorders` (`0x800123D0`) adds a black `POLY_F4` (code `0x28`) at the
  screen edge when `game+0xD0` (x) is not 0, and one more when `game+0xD1` (y) is not 0. Two
  12-byte primitives per buffer (`0x800CF908`, `0x800CF948`), built from the same two bytes with
  `0x80066F5C` and `0x80066EDC`, are linked at entry 29 [certain for the code; likely: the draw
  area and the draw offset of a screen shake].
- **Pads** (`PadRead`, `0x80012A34`): the raw buffers are `0x800CF920` (pad 1) and `0x800CF870`
  (pad 2). The pad type is the high nibble of byte 1 (`0x4` digital, `0x5`, `0x7` analog). The
  words go to `pad1` (`0x800C456C`) and `pad2` (`0x800C4758`): `+0` held, `+2` held in the last
  pass, `+4` pressed in this pass. The bits are the inverted pad bits, with a remap for type 5.
  `0x80013064` then sets up the DualShock actuators, and `RumbleUpdate` (`0x80012C54`) plays the
  vibration patterns that `Rumble` starts.
- **CD and music** [likely]: `CdStreamPoll` (`0x800185B4`) polls the CD state (`0x800E2E5C` = 2)
  and `MusicStreamUpdate` (`0x80018290`) runs the music state machine (`0x80070A28`, state
  `0x800E2E18`) and sets the XA volume.
- **The CD upload queue** [certain for the code; the libcd and libsnd names are likely]. The CD
  archive loader runs in the CD ready callback (`ArcReadyCallback`, `0x8001531C`, registered with
  `0x800648B4`, likely `CdReadyCallback`). It reads each 2048-byte sector into one slot of
  `cd_sector_ring` (`0x800CF998`, 32 × `0x800`), and the member handler of the file
  (`arc_member_handlers`, `0x80010038`: 0 RAM `0x8001668C`, 1 VRAM `0x8001574C`, 2 VAB
  `0x80015A0C`, 3 SEQ `0x80015630`) adds a 12-byte job to `cd_upload_ring` (`0x800E01E8`, 32
  entries: `+0` valid, `+1` type, `+4` VRAM y and `+6` x, or `+4` VAB slot; `+8` = `0xFFFF` for
  the first VAB part). The VRAM and VAB handlers set `cd_upload_pending` (`0x8008EC08`) = 1.
  - `RunUploadQueue` (`0x8001651C`), in the main thread after the tasks: when
    `cd_upload_pending` is set, `CdUploadQueueDrain` (`0x80015C5C`) runs all ready jobs from the
    read index (`0x800DF998`; write index `0x800DF999`) through `cd_upload_handlers`
    (`0x8006E348`, 3 entries): 0 nothing (`0x800166FC`), 1 `CdUploadVramSector` (`0x80016704`:
    `LoadImage` of the slot as 64 × 16 at x, y), 2 `CdUploadVabSector` (`0x80015DA0`: one part of a
    VAB body to SPU RAM, `0x80060604`, likely `SsVabTransBodyPartly`). Only types 1 and 2 are
    posted. When the done count (`0x800E01D4`) equals the posted count (`0x800E01D6`), it clears
    `cd_upload_pending`.
  - When 32 jobs wait (posted − done ≥ 32), the handler stops the read (`CdControlB(9)`, pause)
    and sets `arc_load_status` (`0x8009509C`) = `0x80`; the load is then tried again. Status: 1
    started, 2 done, `0x80` error, `0xC0` time-out after 600 frames.
  - `ArcLoadStart(file, mode, arg)` (`0x80014F70`) resets the ring and starts a read. Its callers
    include the stage load (`0x80013BF0`, `0x80013D10`, `0x80013DE8`) and `BossSpawner`
    (`0x800528F4`). The waits (`0x80015EC0`, `0x80016178`) call `TaskWait(1)` until the status is
    2 and the queue is empty, so the main loop drains the queue while a task waits [likely].
  - In all 44 dumps the queue is empty (`cd_upload_pending` = 0, status 2). The boss, Zero and
    High Max dumps show 90–148 finished VAB jobs from the last load.
- **CPU time.** `VSync(1)` returns the time since the last vertical blank; it goes to scratchpad
  `+0x70` after the tasks ran [certain; likely a performance meter].
- **Soft reset** (`SoftReset`, `0x80012EB4`): when `reset_enable` (`0x8008EC0C`) is not 0 and no
  fade runs, holding Start + Select (`pad1` held `& 0x900`) for 60 passes (`reset_hold`,
  `0x8009741A`) kills the four tasks and restarts the title (`0x8001D218`, `0x80016B3C`,
  `0x8002C1DC`, `0x8001D25C`) [certain for the code].

### Tasks [certain for the code]

- `tasks` (`0x801F8100`): 4 slots of `0x80` bytes. `+0x00` state (halfword), `+0x02` frames to
  wait, `+0x08` thread handle, `+0x10` stack pointer, `+0x44` `gp`. `task_current` (`0x801F8300`)
  points to the slot that runs.
- `TaskInit` (`0x80013128`, called by `SystemInit`) sets each slot to state 0, sp
  `0x801FEC00` + slot × `0x400` (stacks of `0x400` bytes), and `+0x0C` to a BIOS TCB pointer
  (`*0x110` + `0xC0` × (slot + 1)), whose `+0x94` (SR) it sets to `0x40000404`. No code writes
  `+0x44`, so each task starts with `gp` = 0. It also copies the task function table
  (`0x8006D9B0`, 9 entries: `TaskStart`, `TaskWait`, `TaskExit`, `TaskKill`, `TaskRestart`,
  `TaskSuspend`, `TaskResume`, two empty stubs) to `0x801F8000`. No reader of that copy was found.
- States: 0 free, 1 waiting (`+0x02` frames), 2 started, 127 running. `RunTasks` (`0x800131AC`)
  looks at the slots in order 0 to 3. State 1: it subtracts 1 from the wait, and when the wait
  reaches 0 the task runs in the same pass. States 2, 4 and 127 run in each pass; other values
  do not run. To run a task: state = 127, `ChangeThread(handle)`.
  - No code writes state 4 (scan of the main program, all 59 `rock_bin` overlays, all
    `rock_dat` files and the 44 dumps; access through a computed pointer is not excluded). It is
    a leftover.
  - `TaskSuspend(slot)` (`0x80013480`) sets bit `0x40` of the state, and `TaskResume(slot)`
    (`0x800134A4`) clears it. A suspended task does not run. Only the function table refers to
    them; no `jal` calls them.
- `TaskStart(i, pc)` (`0x80013300`): `OpenThread(pc, sp, gp)`, state 2. `TaskWait(n)`
  (`0x80013360`): state 1, wait `n`, `ChangeThread` to the main thread (`0xFF000000`).
  `TaskExit` (`0x800133A0`): state 0 and `CloseThread` of the own thread. `TaskKill(i)`
  (`0x800133F4`). `TaskRestart(pc)` (`0x8001344C`) sets `task_restart_req` (`0x8006D9AC`) and
  `task_restart_pc` (`0x800CF960`); `RunTasks` then closes the thread and opens it again at `pc`.
- The slots:

| slot | code | started by |
|---|---|---|
| 0 | `BootTask` (`0x8001CB3C`): stage 14, `0x80013DE8`, then `TaskRestart(FrontEndTask)`. `FrontEndTask` (`0x8001D0E4`) runs `front_end_states` (`0x800710E8`) by `front_end_state` (`0x800CD3F8`), with `TaskWait(1)` after each call | `SystemInit` |
| 1 | `GameModeTask` (`0x8001E6E4`): `game_modes[game+0x00](game)`, then `TaskWait(1)`, without end | the front end (`0x8001DB70`, `0x8001DDC0`, `0x8001DDF0`); each of them then calls `TaskExit`. `FrontEndTask` kills it when a skip button is pressed (`0x8001D1D0`): the demo skip (see "Front end") [certain for the code] |
| 2 | `FadeTask` (`0x80013530`) | `FadeIn`, `FadeOut` |
| 3 | `WhiteFadeTask` (`0x80013788`) | `WhiteFadeIn`, `WhiteFadeOut` |

- In all 44 save states, only slot 1 is live: state 1, wait 1, and `task_current` is past the last
  slot [certain]. So each dump was saved between two passes, with the game task at its `TaskWait(1)`.

### Frame rate [likely]

Each pass calls `VSync(0)` once, and the game task runs once per pass. So the game makes one update
per vertical blank: 60 per second on NTSC. When the tasks and the GPU draw of the last pass take
more than one frame, `VSync(0)` waits for the next blank: the pass takes two blanks and the game
slows down. `frame_count` counts passes, not blanks. This is from the code; it was not measured on
a running game.

**Half speed.** `PlayerTick` calls `VSync(0)` once more on odd `frame_count` values when
`game+0x67` is not 0 and the player's step has a flag in `0x80073A1C` (steps 2–19, 26–28, 31–63).
Then the odd passes take two blanks: 2 updates per 3 blanks [certain for the code].

- `game+0x67` is an **options-menu setting** [certain for the code]. Game mode 9 (stage start,
  `0x8001E1BC`) copies the byte `0x8006D5F9` into it (`0x8001E2C8`). `0x8006D5F9` is in an
  options block after the string `"CAPCOM"` (`0x8006D5F0`; `0x8006D5F8` is the difficulty). Only
  row 1 of the options menu in overlay 18 changes it (`0x800E9C74`: Left, Right or X switch 0 and
  1). The save code does not write it, so it is lost at power-off [likely]. It is 0 in the
  executable and in all 44 dumps.
- The label of the option on the screen is not known (sprite frame 7 when off, 9 when on) [guess:
  a game-speed setting].
- The same step flags (table `0x80073A6C`, the same bytes as `0x80073A1C`) gate two counters
  that `PlayerTick` adds 1 to in each update: `game+0x90` (the stage time) and `game+0xCC` (saved
  and loaded with the memory card; likely the total play time).

### Stage play (game mode 10) [certain for the code]

`StagePlay` (`0x8001E9EC`) calls `play_submodes` (`0x80071184`, 3 entries) by `game+0x01`.

- **0 `PlayFrame`** (`0x8001EA28`):
  - **Pause check.** The menu opens when `game+0x1C` (input lock) = 0, `fade_dir` = 0,
    `game+0x10` = 0, `game+0x0F` = 0, and (START is pressed in `pad1`, or `0x80097426` = 0 and pad 1
    is not connected (status `0xFF`)). Then `RumbleStop` (`0x80013008`), sub-mode 2, and
    `DrawStage` only: no update in that pass.
  - When `mega.state` = 3: sub-mode 1 and `FadeOut(8)`.
  - Then `GetRNG()` (one RNG step in each play frame), `GameFrame`, `PaletteFxRun` when
    `mega+0xE4` is not 0 (see "Palette effects" below), and `DrawStage`.
- **1 `PlayExit`** (`0x8001EB48`): while `fade_dir` is not 0, `GameFrame`, `PaletteFxRun` and
  `DrawStage`: the stage keeps running during the fade-out. Then `StageExit`, `StopSound(255, 0)`,
  `bg_refresh` = 1, `0x800186A8` (when `game+0x1B` = 0), `RumbleStop`, `reset_enable` = 0, and the
  stop flags `+0x10`, `+0x11`, `+0x14`, `+0x16`, `+0x17` = 0.
- **2 `Pause`** (`0x8001EC40`): `PlayerInput`, then the pause menu (`0x80034830`). The objects do
  not update and the stage is not drawn again; the menu draws over the last frames in VRAM
  [likely].

**`DrawStage`** (`0x80025FB0`) calls `DrawLayers` (`0x80026D44`: the three BG layers, camera work
`0x800971F8` + layer × `0x54`) and `DrawSprites` (`0x80022960`) [likely for the names]. They run
after `GameFrame`, so a frame draws the positions after its own update.

**Palette effects** [certain for the code]. The palettes are 512 CLUTs of 16 colours in VRAM
(0, 480)–(255, 511): CLUT p is at x = (p % 16) × 16, y = 480 + p / 16. Three channels share the
arrays `fade_mode` (`0x800CF850`), `fade_r` (`0x800CF854`), `fade_g` (`0x80097430`), `fade_b`
(`0x80090D5C`) and `fade_mask` (`0x80090C90`, 16 words per channel, one bit per CLUT, MSB first).

- Channel 0 is `PaletteFade` in `GameFrame` (output `0x80097798`, `bg_refresh` = 2).
- Channels 1 and 2 are `PaletteFxChannel(ch)` (`0x8002AF6C`), called by `PaletteFxRun`
  (`0x8002B29C`) for bits `0x01` and `0x02` of `mega+0xE4` (`palette_fx`). The source is
  `0x80097798` when `bg_refresh` = 2, `palette_fx_buf` (`0x8009E1B0`) when it is 8 (channel 2 on top
  of channel 1), else the raw palette (`*scratchpad+0x28`). The output is `palette_fx_buf`, then
  `bg_refresh` = 8, so `PaletteUpload` sends it. CLUTs whose mask bit is clear are copied.
  - Scale mode (`fade_mode[ch]` ≠ 0): each component c becomes c − c × k / 32, k = `fade_r[ch]`
    (up to 31). k = 31 makes the CLUT transparent black.
  - Add mode (`fade_mode[ch]` = 0): adds `fade_r/g/b[ch]`, clamped. This is the white flash.
- `PaletteFxReset` (`0x8002ABA8`) sets the channel 1 offsets to 0 and loads `fade_mask[1]` from
  `stage_bg_masks` (`0x80073680`, by stage × 2 + area; default CLUTs 96–479: the BG CLUTs, VRAM
  row 486 and lower), then `bg_refresh |= 1`.
- **Channel 0 in subtract mode.** `PaletteFade` reads three subtract flags: `fade_mode[0]`
  (`0x800CF850`) for red, `fade_sub_g` (`0x8009742C`) for green and `fade_sub_b` (`0x80090D56`) for
  blue. With all three set and `fade_r/g/b[0]` = 31, 31 × 32, 31 × 1024, the masked CLUTs are black.
  Blizzard Wolfang's entry uses it: the room (CLUTs 32–35 and 96–479) goes dark in 124 updates, the
  boss CLUT (88–95) joins the mask later, and all fades back (`docs/enemies/blizzard-wolfang.md`)
  [certain for the code]. The other writers of the two flags (`0x8002BBC0`, `0x800505D0`,
  `0x800523B8`, `0x80052440`, `0x800525A0`) were not read.
- **Who uses it.** Only bit `0x01` has writers: X's Falcon, Shadow and Blade Giga attacks (steps
  36, 37, 77; overlay 00) and Zero's Rekkoha (step 59; overlay 01). The Ultimate Giga (step 34) does
  not. Sequence (Falcon Giga): `PaletteFxReset`, scale mode, `+0xE4 |= 1`; `fade_r[1]` += 2 per
  update (the BG goes black in about 16 updates); at update 51 the mask is all CLUTs and add mode
  with 31 for two updates (white flash); then scale mode again until the attack ends; then
  `fade_r[1]` −= 8 per update, and at the end `+0xE4 &= ~1` and `bg_refresh |= 1`.
- The teleporter crystal's warp clears `mega+0xE4` (one copy in each stage overlay). Bit `0x02`
  and channel 2 have no writer: a leftover. `mega+0xE4` is 0 in the 44 earlier dumps.
  `heatnix-3_rekkoha` (Zero in step 59.4) has `+0xE4` = 1, `fade_mode[1]` = 1, `fade_r[1]` = 31,
  `fade_mask[1]` = the stage BG mask and `bg_refresh` = 8: the BG CLUTs are all 0 in
  `palette_fx_buf` and in VRAM [certain].
- The Rekkoha (step 59) is the same sequence with other times: the flash at update 16 for 2
  updates, then scale mode with 31 until the attack ends, then −8 per update
  (`docs/players/zero.md`). It also writes 1 to `fade_sub_g[1]` and `fade_sub_b[1]` and adds
  64 / 2048 per update to `fade_g[1]` / `fade_b[1]`. `PaletteFxChannel` reads only `fade_mode` and
  `fade_r` in scale mode, so these writes count only in add mode (the flash) [certain for the
  code].
- A Giga that a hit stops leaves bit `0x01` set with offsets 0 (no visible effect) until a
  teleporter clears it [likely].

### Game modes [certain for the table]

`game_modes` (`0x8007112C`, 22 entries) by `game+0x00`. `GameModeTask` calls the entry once per
pass. Entries in the overlay area (`0x800E9xxx` and up) are the code of the overlay that is loaded
for that screen.

| mode | code | what it does |
|---|---|---|
| 0 | `0x8001E088` | new game |
| 2 | `0x8001E908` | game mode 3 |
| 3 | `0x8001E8CC` | stage 13 area 0 (overlay 16, the stage select), `SetDisplayMode(1)`, game mode 4 |
| 5 | `0x8001E7C0` (`BossIntroStart`) | the boss intro, see "Boss intro" |
| 6 | `0x800E9A88` (overlay 17, `BossIntro`) | the boss intro, see "Boss intro" |
| 7 | `0x8001E880` (`StageLoadStart`) | `SetDisplayMode(0)`, `0x8002B838`, `StageLoadPlayer`, game mode 8 |
| 8 | `0x8001E914` (`StageStartReset`) | the stage load |
| 9 | `0x8001E1BC` (`StageStart`) | the stage start |
| 10 | `0x8001E9EC` (`StagePlay`) | stage play |
| 11 | `0x8001E81C` | stage 15 area 0 (overlay 20, the report), `SetDisplayMode(1)`, game mode 12 |
| 19 | `0x8001ED88` | scene start |
| 21 | `0x8001EDDC` | the next scene of the list |
| 1, 4, 12, 17, 20 | overlay addresses | the code of the loaded overlay |
| 13, 15, 16 | `0x8001FB3C`, `0x8001EC68`, `0x8001ED44` | not read |
| 14, 18 | 0 | no entry |

**Display width** [certain for the code]. `SetDisplayMode(mode)` (`0x80012890`) returns when
`mode` equals `display_mode` (`0x80090D6A`). Else it writes `display_mode` = `mode` and
`screen_width` (`0x8009B79C`) = 320 (mode 0) or 512 (other values), and sets up both draw buffers
with that width. Game modes 3 (the stage select), 5 (the boss intro) and 11 (the report) call
`SetDisplayMode(1)`; game mode 7 (before a stage) and the stage load at `0x8001466C` call
`SetDisplayMode(0)`. So the menus and the boss intro are 512 × 240, and stage play is 320 × 240.
The layer draw set-up (`0x800264DC`–`0x80026734`) has one copy for a texture at VRAM x 320 and one
for x 512, by `display_mode`.

### Front end [certain for the code unless marked]

The screens from power-on to the start of a game. Save states: `title-1_capcom-logo` (the logo
movie), `title-2_loading` (the loading screen: the load of stage 14 after the logo, `BootTask` →
`0x80013DE8`), `title-3_anime-intro` (the opening movie, frame 19 of 1527),
`title-4_press-start` (the title) and
`title-5_main-menu` (the menu). The emulator keeps no rendered frame in VRAM, so only the movie
frames are visible in a state; [`~/mmx-data/x6/re/front-end/`](../work/front-end/) rebuilds the title and menu screens
from the layout and the objects. In the logo and opening states, task slot 0 runs (state 127,
the movie player runs inside the task); slot 1 is free in all five [certain].

- **Capcom logo.** `BootTask` calls `0x8001CBE0`: `SetDisplayMode(0)` and the movie player
  `0x8001A9B0` with CD file 243 (`STR/CAPLOGO.STR`). Then it loads stage 14 area 0 and starts
  `FrontEndTask`.
- **Movies.** `PlayMovie(row)` (`0x8001A6CC`) plays a row of `movie_table` (`0x80070CA4`, 12
  bytes: u32 CD file, u16 frames to play, u16 `+6` [not known], u16 `+8` = 1, u16 skip buttons).
  Row 0: file 243, 90 frames, skip `0x0800` (Start). Row 1: file 244 (`STR/X6OP.STR`, the
  opening), 1518 frames, skip `0x08F0` (Start or a face button). The CD file numbers come after
  the 243 `ROCK_X6.DAT` members, in the order of the name table `0x8006E224` (22-byte records:
  `CAPLOGO.STR`, `X6OP.STR`, `BGM.XA`, `DEMO.XA`). The player uses the MDEC library
  (`0x80069000`–`0x80069800`) at 320 × 240, 24 bit, into both frame buffers. Game mode 15
  (`0x8001EC68`) calls `PlayMovie(8)` and `PlayMovie(9)`; rows 8 and 9 are 0 in the executable
  and in RAM in the five front-end states [not known what fills them].
  `python3 -m mmxdemake.movie` decodes both files: 93 and 1527 frames at 15 fps, with stereo XA
  sound (the opening music is in the movie).
- **`FrontEndTask`** (`0x8001D0E4`) calls `front_end_states[fe.state]` (`0x800710E8`) once per
  pass, with `fe` = `0x800CD3F8`: `+0x00` state, `+0x01` sub-step, `+0x02` menu row, `+0x04`
  timer (halfword), `+0x0C` demo number (0–3), `+0x0D` skip lock, `+0x0E` the state at the skip.
  Most sub-steps wait for `fade_dir` (`0x80097424`) = 0 before they go on.
- **Skip.** When `+0x0D` = 0 and a skip button is pressed (`0x0800` in state 1, `0x08F0` in the
  other states): sound 34, `TaskKill(1)` (the demo game task), `FadeOut(8)` when no fade runs, `+0x0E` =
  the state, state 5. State 5 goes to state 6 (the menu) after a skip on the title, else to state 1 (the
  title).

| state | code | what it does |
|---|---|---|
| 0 | `0x8001D338` | Sub-step table `0x8007108C`. When the byte `0x80071088` is not 0 (it is 1), sub-step 0 jumps to sub-step 6: `PlayMovie(1)`, the opening; then state 1. Sub-steps 0–5 are not used: they load member 162 and show it for 300 frames or until a button [likely: a notice screen of another version] |
| 1 | `0x8001E024` | **Title.** Sub-step table `0x80071118`; then `StageLogicLoop`, `MiscLoop`, `RayLoop`, `PaletteFade`, `DrawStage`. Sub-step 0 loads stage 14 area 0 again (member 107, overlay 18), `SetDisplayMode(1)` (512 wide), shows BG layer 0 only; sub-step 2 makes one misc object type 19 `var` 4 (`0x80071114`; PRESS START BUTTON, blinks), `FadeIn(8)`. 600 frames (`fe+0x04` counts down), then `FadeOut(8)` and state 2. Live: `title-4_press-start`, sub-step 3, timer 465 |
| 2 | `0x8001D424` | **Demo.** Sub-step table `0x800710AC`. `SetDisplayMode(0)`, loads demo member `0x800710BC[fe+0x0C]` (90–93: input recordings [likely], header word 2500) to `0x801F6000`, loads the stage and the player, runs it for 2400 frames |
| 3 | `0x8001D760` | Member 0 (one `0x10013` and one `0x3` entry) as two misc objects type 21 (x 128 and 384, y 120) for 300 frames [not known what the picture shows] |
| 4 | `0x8001D8EC` | `fe+0x0C` = next demo (0–3), state 0: the opening plays again |
| 5 | `0x8001D930` | After a skip, see above |
| 6 | `0x8001D9CC` | **Main menu.** Sub-step table `0x800710D0`. Six misc objects type 19, `var` 0, 1, 2, 5, 6, 7 (`0x800710E0`): the rows GAME START, CONTINUE, OPTION, the line "PRESS START BUTTON or ⊗ BUTTON" (`var` 5 and 7, blink) and the orange highlight (`var` 6). Up and Down (sound 12) move `fe+0x02` over rows 0–2; Start or Cross (`0x0840`, sound 34) picks the row. Row 0 (GAME START): `game+0x00` = 0 (new game), `TaskStart(1, GameModeTask)`. Row 1 (CONTINUE): state 7. Row 2 (OPTION): state 8. 18 000 frames with no button go back to state 3. On row 0, two input counters are the new-game cheats (see below). Live: `title-5_main-menu`, sub-step 1, row 0 |
| 7 | `0x800ED760` | **CONTINUE** (overlay 18): sub-step table `0x800F08FC`; sub-step 3 (`0x800EA8C0`) calls `LoadGame` (`0x8001BA1C`) [certain] |
| 8 | `0x800EA5C8` | **OPTION** (overlay 18): sub-step table `0x800F08E4` (6 entries); the half-speed option code `0x800E9C74` is in this part [likely] |

Stage 14 area 0 is member 107 with overlay 18 and palette member 26; area 1 is member 108 with
overlay 19 and palette member 27 (`stage_members`, `stage_overlays`, `palette_members_x`) [not
known what uses area 1]. The title picture is drawn with BG layer 0, as the boss intro [certain;
`title-4_press-start`, `title-5_main-menu`]: the layout is 4 × 2 with layer 0 row 0 = screens 1–4
and layers 1, 2 empty; all cameras at (0, 0); member 107 entry `0x10014` is at VRAM (512, 0);
the CLUTs are member 26 rows 0–7 at VRAM rows 480–487 (BG CLUT base (0, 486)). Screens 1, 2 are
the title picture (logo, lens, copyright lines; the full 512 × 240 view). Screens 3, 4 are a blue
hexagon pattern outside the view [guess: the CONTINUE or OPTION background]. Stage 14 has no
solid cells, so `learn` cannot learn it.

**Title and menu text** (misc type 19, overlay 18, `FrontEndText` `0x800EDB6C`; misc type table
`0x800F15F8`) [certain]. State table `0x800F0BF8`: init, update, delete. The init puts the object
in screen space (layer −1), texture `0x1800` (VRAM (512, 256)), sprite data = the first entry of
`sprite_bank_base`, and x, y, sprite and CLUT from `0x800F0B44` (6 bytes per `var`). `var` 0–3:
GAME START, CONTINUE, OPTION, TRAINING at x 258, y 142, 158, 174, 182; `var` 4: PRESS START BUTTON
(258, 160); `var` 5 and 7: "PRESS START BUTTON or ⊗" and "BUTTON" (220 and 476, y 192); `var` 6:
the highlight, CLUT `0x7801` (orange; the others `0x7800`, purple), priority 2, y = 142 +
row × 16, sprite of the row. `var` 4, 5 and 7 are shown 16 of each 32 frames. No code makes
`var` 3 (TRAINING) [likely a leftover of another version].

**New-game cheats** [certain for the code]. On menu row 0, two word counters (cleared by Up, Down,
another row and the menu set-up): `0x800E3130` counts Left (`0x8000`) to 3, then Right
(`0x2000`) sets it to 4; `0x800E3134` counts L1 (`0x0004`) to 3, then R2 (`0x0002`) sets it to 4
(sound 34 at 4; each clears the other). Game mode 0 (`0x8001E088`) gives X armour bits `0x01`
and form 1 (Falcon); with the first counter at 4, bits `0x09` and form 4 (Ultimate Armor); with
the second at 4, it adds bit `0x20` (Black Zero). Then game mode 19.

### Boss intro (game modes 5, 6, 7) [certain for the code]

The screen with the Maverick picture and name before a stage. It is a small stage: stage 13 area
1, overlay 17 at `0x800E9860`, drawn with the BG layers (`DrawStage`).

- **Game mode 5** (`BossIntroStart`, `0x8001E7C0`): `game+0x26` and `+0x27` = the picked stage and
  area; stage 13 area 1; `game+0x36` = the picked stage (1–8); the stage data load
  (`0x80013DE8`); `SetDisplayMode(1)`; game mode 6.
- **The data.** For stage 13 with area ≠ 0, the stage load writes
  `boss_intro_members[game+0x36 − 1]` (`0x8006DA5C`: 141–148) into `stage_members[27]`
  (`0x8006D9FC`, u16 [stage × 2 + area]) and `boss_intro_palette_members[game+0x36 − 1]`
  (`0x8006DB40`: 52–59) into `palette_members_x[27]` and `palette_members_zero[27]`. The picture
  member has entries `0x4`, `0x20000`, `0x10014` (the 256 KB texture: 4 pages of 8 bpp, to VRAM
  (512, 0)), `0x0` (8 screens), `0x1` (tile info) and `0xB`. Entries `0x4`, `0x20000` and `0xB`
  are the same in all 8 members. All tiles are 8 bpp and use palettes 0, 16 and 32 at the BG CLUT
  base (0, 486): rows 6–8 of the palette member.
- **The layout** (overlay 17, `0x800EA2C8`; 24 × 8, only row 0 is used): layer 0 = the name
  (screens 1, 2 at x 2, 3), layer 1 = the full body (screens 3, 4 at x 2, 3), layer 2 = a close-up
  of the face (screens 5, 6, 7 at x 0–2). The name is a graphic in the tiles, not text.
- **Game mode 6** (`BossIntro`, `0x800E9A88`): `boss_intro_steps` (`0x800EA528`) by `game+0x01`,
  with `game+0x04` as the timer; then `StageLogicLoop`, `EffectLoop`, `MiscLoop`, `RayLoop`,
  `PaletteFade`, `DrawStage`. The steps:
  1. Set-up (`CheckpointStart`, `LayerInit`), `FadeIn(8)`; when the fade ends, `StreamPlay(3)`
     (`c01-t02`, 9.4 s, no loop).
  2. When `stream_state` (`0x800CD40C`) = 2: ray type 12 `var` 0. Layer 1 (the body) slides in:
     camera x += target / 120 per update to `boss_intro_body_x[b − 1]` (`0x800EA508`: 672, 656,
     640, 624, 560, 656, 720, 624). Wait 30; 24 rays type 12 (`var` `0x80` + i).
  3. Layer 2 (the close-up) slides: camera x −= 5.12 px per update to 0. Wait 10.
  4. Layer 0 (the name) slides in: camera x += target / 30 per update to `boss_intro_name_x[b − 1]`
     (`0x800EA518`: 480, 512, 528, 496, 512, 528, 528, 528).
  5. Palette channel 0 in add mode with 31 for r, g and b for 4 updates, then
     `PaletteFxClear(0)` [likely: a white flash]. The rays are hidden during the flash.
  6. Wait 30 + 180 updates, `FadeOut(8)`. When the jingle and the fade end: stage = `game+0x26`,
     area = `+0x27`, game mode 7, and `game+0x26`…`+0x35` = 0.
- **Ray type 12** (`0x800EA23C`, overlay 17) draws flat shapes: `var` 0 a bar (colour `0x7FFF`)
  that grows down and then gets wider, `var` `0x80` + i 24 lines of 2 px (colour `0x2208`) that move
  up to y 164 + 2i [likely for what they look like].
- No `PlaySound` call: the screen has only the XA track.
- Boss `b` is the stage number: 1 Commander Yammark, 2 Blizzard Wolfang, 3 Blaze Heatnix, 4 Metal
  Shark Player, 5 Ground Scaravich, 6 Rainy Turtloid, 7 Shield Sheldon, 8 Infinity Mijinion
  (read from the pictures).
- Save state: `wolfang-0_boss-intro` (sub-mode 6: the body is in place, the close-up slides, the
  name is not yet on the screen). The picture is drawn from the disc files only; the save state is
  not necessary. The images of all 8 Mavericks (draft script, not yet a tool):
  `~/mmx-data/x6/re/boss-intro/images/` (`scripts/render_intro.py`).

### The game frame (`GameFrame`, `0x80020C40`) [certain for the call order]

| # | call | what it does |
|---|---|---|
| 1 | `SolidRectsBuild` (`0x8002E84C`) | clears `solid_rect_count` and adds one rectangle per live stage object with a solid box (`+0x68`, `+0xA2` ≠ 0): see "Solid objects" |
| 2 | `PlayerInput` (`0x8003CD44`) | `pad1` → `mega+0x7C` (held), `+0x7E` (held before), `+0x80` (pressed), through the key configuration (`0x8003D7AC`) |
| 3 | `Actor2Update` (`0x80020A08`) | the Ride Armor slot |
| 4 | `PlayerTick` (`0x80034DCC`) | the hit stop, then the player state (`player_states`); see below |
| 5 | `WeaponLoop` (`0x8002012C`) | 32 weapon objects |
| 6 | `Kind7Loop` (`0x800208EC`) | 4 kind-7 objects |
| 7 | `MainObjectLoop` (`0x8002000C`) | 48 main objects, `main_table` (scratchpad `+0x80`) |
| 8 | `VisualLoop` (`0x80020258`) | 32 visual objects, scratchpad `+0x84` |
| 9 | `EffectLoop` (`0x80020378`) | 96 effect objects, scratchpad `+0x88` |
| 10 | `StageObjectLoop` (`0x800205A0`) | 80 stage objects, scratchpad `+0x94` |
| 11 | `MiscLoop` (`0x800206C0`) | misc objects, scratchpad `+0x90` |
| 12 | `RayLoop` (`0x800207D4`) | 64 ray objects, scratchpad `+0x98` |
| 13 | `ScrollSpawn` (`0x80029D18`) | when the layer-0 camera x (`+0x0A`) or y (`+0x0E`) is not the same as in the last frame (`+0x16`, `+0x1A`), `SpawnInRect` for the strip that comes into view (side 1–4); see "Spawning and despawning" |
| 14 | `StageLogicLoop` (`0x8002048C`) | 96 stage-logic objects, scratchpad `+0x8C` |
| 15 | `TextUpdate` (`0x80020D18`) | while `text_open` (`0x8008EAFC`) is set: `0x80021BF4` (the text box); else, when the halfword `0x8008EB00` is not 0: `TextOpen(id & 0x7FFF, 255, 0)` |
| 16 | `CollideCheck(mega)` (`0x8002F288`) | the player's terrain collision |
| 17 | `0x8002927C`, `0x8002935C`, `0x800293CC` | the cameras of layers 0, 1, 2. Layer 0 runs its mode (`0x80071E80`, by `+0x04`) only when `+0x44` is not 0, `mega+0xCC` = 0, and not while Zero has `mega+0xE0` set; then `CameraLimitEase` |
| 18 | `PaletteFade` (`0x8002ACA8`) | when one of the colour offsets (`0x800CF854` r, `0x80097430` g, `0x80090D5C` b) changed or `bg_refresh` bit `0x04` is set: adds or subtracts them from all 512 colours into `0x80097798`, then `bg_refresh` = 2; channel 0 of the palette effects (see "Stage play") [likely for the name] |
| 19 | `PlayerPartsLoop` (`0x80020D74`) | 3 objects of `0x50` at `0x800911A8` that copy the player's position (2 always, the third when live) [guess for the name] |
| 20 | `PlayerImageLoop` (`0x80020DC8`) | 3 objects of `0x60` at `0x800972F8` that copy the player's frame, flip and sprite source [guess: after-images] |
| 21 | `PlayerFrameStream(mega, 320, 0)` (`0x80017B04`) | queues the player's new frame to VRAM |

- **Stop flags.** Each loop reads one byte of `game+0x10…+0x19`: `+0x10` player, `+0x11` weapon,
  `+0x12` main, `+0x13` visual, `+0x14` effect, `+0x15` stage logic, `+0x16` stage objects,
  `+0x17` misc, `+0x18` ray, `+0x19` kind-7. While the flag is set, only the objects with flag bit
  `0x08` (byte 0) update. The main, visual, weapon, effect, stage-object and misc loops call
  `Display` (`0x8002CD6C`) for the other live objects that are visible (`+0x03` ≠ 0), so the
  objects stay on the screen. The stage-logic, kind-7 and ray loops skip them [certain].
- **Hit stop.** When `mega+0xCD` is set, `PlayerTick` clears it and sets `mega+0xCC` = 5. While
  `mega+0xCC` is not 0, it counts down by 1 per frame, and the player, the main, visual, weapon
  and stage-object loops and the layer-0 camera do nothing (no update, no `Display`). The effect,
  misc, ray and stage-logic loops still run.
  - `ApplyWeaponHit` sets `mega+0xCD` = 1 (at `0x8003168C`) for weapon ids 26–31, 35, 46, 51, 57,
    61 and 62 (switch on id − 26, table `0x8001102C`; all other ids go to the shared code at
    `0x80031690`), when the hit class of this hit (`+0x63`) is not 7 [certain for the code].
  - By the player docs, these are Zero's sabre slashes (26 combo hit 3, 27 jump/fall, 28 landing,
    29 spin, 30 ladder, 31 wall) and Rakukojin (35), X's sabre slashes (46 Shadow Armor charged
    slash, 51 air/wall slash), the Blade Armor Giga wave (57), and the Ride Armor punches (61, 62)
    [likely for the names]. Zero's combo hits 1–2, Sentsuizan, Ensuizan, Rekkoha and all buster
    shots do not start it.
  - The test comes before the hit-class checks, so a blocked hit (class `0xFF`) of these ids also
    starts it. Early exits make no hit stop: a hit tag match (126) and class 2. Class 7 is only in
    `dmg_tbl_capsule`: a hit on the Light capsule never starts it [certain].
  - Only `ApplyWeaponHit` and `PlayerTick` write `mega+0xCC`/`+0xCD` [certain for single stores].
- **Free-move mode.** After the hit stop, `PlayerTick` copies the position to `+0x18`, `+0x1C`,
  `+0x13C` and `+0x140`. Then, when `solid_off` (`0x800C4568`) is not 0, it moves the player 8 px
  per frame with the D-pad bits of `mega+0x7C` and runs no state [certain for the code; likely a
  debug mode].
- **Order effects.** The player moves before all objects, so objects see the player position of
  this frame, and the player sees the objects of the last frame. The terrain collision of the
  player (`CollideCheck`) runs after all objects, so a solid object that moves in this frame
  pushes the player in the same frame. The cameras follow after the collision, and the sprites are
  drawn after the cameras [certain for the order].

## Object classes

| class | slots | size | base | update table | made by |
|---|---|---|---|---|---|
| main (enemies, bosses) | 48 | `0x9C` | `0x8008EF48` | scratchpad `+0x80`, 77 entries | the stage spawner, `GetMainObject` |
| stage objects (record kind 4) | 80 | `0xA4` | `0x800C1220` | scratchpad `+0x94`, 32 entries | the stage spawner |
| visual (enemy shots) | 32 | `0xA0` | `0x80093C98` | scratchpad `+0x84`, 58 entries | `GetVisualObject` (`0x8002C458`) |
| misc (debris, overlays, flames, scenery) | 96 + 32 | `0x60` | `0x800CD410` (96), `0x800964A0` (32) | scratchpad `+0x90`, 64 entries | `GetMiscObject` (`0x8002C530`), `GetMiscObject2` (`0x8002C590`) |
| effect (explosions, sparks, muzzle flashes) | 96 | `0x70` | `0x80091298` | scratchpad `+0x88` (from `0x8006DDDC` + stage × 8 + area × 4, set at `0x80013EDC`); loop `0x80020378` | `GetEffectObject` (`0x8002C398`) |
| weapon (player shots, stage laser probes) | 32 | `0xA0` | `0x800950A0` | `weapon_types` (`0x80071220`), loop `WeaponLoop` (`0x8002012C`) | `GetWeaponObject` (`0x8002C32C`), `SpawnWeapon` (`0x8003D3F8`) |
| stage logic (record kind 3: camera zones, checkpoints, talk zones, palette cycles, rain control) | 96 | `0x30` | `0x8009B7B0` | scratchpad `+0x8C`, 45 entries; loop `0x8002048C` | the stage spawner |
| ray objects (Nightmare Virus) | 64 | `0x60` | `0x8009C9B0` | scratchpad `+0x98`; loop `RayLoop` (`0x800207D4`) | `GetRayObject` (`0x8002C7F0`) |
| kind-7 objects (stage 7 background switch, stage 4 Nightmare Press, stage 4 area 1 forced scroll `AutoScroll`, stage 20 area 1 forced scroll `AutoScrollAlt` (type 4)) | 4 | `0x30` | `0x800C4778` (`kind7_objects`) | `kind7_types` (`0x8007136C`, 5 entries), loop `Kind7Loop` (`0x800208EC`, after `WeaponLoop`; while `game+0x19` is set only flag bit `0x08`) | record kind 7, `GetKind7Object` (`0x8002C950`) |
| `actor2` (second body: the Ride Armor) | 1 | `0xB0` | `0x800CD340` | `actor2_types` (`0x80071380`, 5 entries), `Actor2Update` (`0x80020A08`, first in the frame, before the player) | `RideSpawner` (stage logic 30 of stage 4) |

The weapon object's type byte is the weapon id: the spawn copies the id into it [certain]. Effect
type 2 is the muzzle flash (`0x80049478`); for the charged shots it makes the weapon object (see
`docs/players/attacks.md`). Effect type 3 is the shot spark. `weapon_types` has at most 83 entries: the kind-7 table starts at
`0x8007136C` and the `actor2` table at `0x80071380`.

**Frame order:** see "The game frame" in "Main loop". The kind table of the records
(`0x8007347C`, `0x8002A3AC`): 0–2 main, 3 stage logic, 4 stage objects, 5 misc, 6 ray, 7 kind-7
objects.

**`actor2`.** A fixed second-body slot, not in a pool. While `mega.ride` < 0, `ContactMega`, the
item and soul pickups (the refill still goes to X) and `SolidObject` act on `actor2`, and X has no
terrain box (`mega+0x68` = 0) and no hurt box. In stage 4 (overlay 06, type 0) and overlay 28
(type 1) it is the Ride Armor (`docs/players/ride-armor.md`).

**Stage overlays.** Stage 4 has two overlays: 06 (area 0) and 07 (area 1); the area 1 record
lists are past the end of overlay 06 and are valid in overlay 07; the area 1 states (`shark-2` to
`shark-9`) have overlay 07 at `0x800E9860`. So: intro 02, stage 1 = 03, stage 2 = 04, stage 3 = 05,
stage 4 = 06 and 07, stage 5 = 08, stage 6 = 09, stage 7 = 10, stage 8 = 11 [certain for 02, 03, 04,
05, 06, 07, 09, 10, 11; 04, 05 and 11: `learn` finds the stage layout in that overlay, and
`rock_bin/05.bin` matches RAM in `heatnix-2_mini-boss` (4 of 44084 bytes differ)]. Shield Sheldon's and Metal
Shark Player's code is in their stage overlays; neither stage loads a boss overlay. Gate's lab:
stage 16 = overlay 22 (member 112), stage 17 = 23 (113), stage 18 = 24 (114); none of the three
has an area 1 [certain for `stage_overlays` and `stage_members`; 16: `learn` finds the layout in
overlay 22 in `gate-1_before-boss`; `rock_bin/22.bin` matches RAM in `gate-2_nightmare-mother`, 11 of
81 224 bytes differ]. Stage 16 is Gate stage 1 (`docs/stages/gate-1.md`, boss the Nightmare Mother),
stage 18 is section 2 of Gate stage 2 and Gate's room (`docs/stages/gate-2b.md`; Gate's code, main
59, is in overlay 24, which ends at `0x800F780A`; no boss overlay; `docs/enemies/gate.md`). Stage 17
is section 1 of Gate stage 2 (`docs/stages/gate-2.md`):
overlay 23 holds High Max's code (main 55, the same functions as overlay 44) and runs up to
`0x800F9BEC`; `learn` finds the layout in overlay 23 in `gate-3_stage-2`. `StageExit` sends a
stage 17 clear straight to stage 18 (game mode 8), and a stage 16 or 18 clear to game mode 19
(scenes 8 and 9) [certain for the code]. Overlay 22 runs up to `0x800FD5A8`, over `0x800FA000`: Nightmare
Mother's code (main type 60) is in the stage overlay, and no boss overlay is loaded
(`docs/enemies/nightmare-mother.md`).

**Warps inside an area.** Overlay code can write the three layer cameras, the limits and the
player position at once, with `SpawnInRect` for the records of the new place. Stage 4 area 1 has
two: `LowerPathWarpJump` (stage logic 10, at camera x 4672) and Nightmare Pressure's death (while
the screen is white); see `docs/stages/shark-2.md` [certain]. The stage, the area and the exit
request do not change.

**Damage passed to a parent.** Nightmare Pressure's parts give their parent `old hp − new hp` after
each `ContactWeapon`. `ApplyWeaponHit` lets the HP go below 0, so the parent takes the full damage
of the killing hit, also past the part's HP [certain for the code]. Nightmare Snake does it
differently: the body sets its HP to 0 each update and each gem adds its own HP (a byte add; −128 is
changed to 127). So the bar shows the sum of the gem HP. The body dies when its four gems are dead
(a counter in `+0x07`), not by HP [certain for the code]. Nightmare Mother (overlay 22, main 60) uses
the same sum: the core (`var` 0, slot 0) sets its HP to 0 each update and the two eyes (`var` 3, 4)
add theirs; each eye adds its `var` to core `+0x8D` when it dies, and the core dies at 7 [certain for
the code].

**A boss on a BG layer.** Nightmare Pressure (overlay 07) is drawn on BG layer 1 (mode 5); its code
moves `cam1.y` to move the machine, and its objects follow the layer [certain]. Nightmare Snake
(overlay 05, main 25) is a ring on BG layer 1 (mode 5): each update the body writes layer 1 =
(512 − x, 512 − y) and the redraw request; the layer is drawn relative to the layer 0 camera, so
layer px (512, 512) stays on the body point. The layer is shown (`cam1+0x03`) only while the body
is within 96 px of the view [certain; `heatnix-2_mini-boss`: body (617, 584), layer 1 (−105, −72)].
Nightmare Snake `var` 8/9 also use BG layer 2 as a hazard: mode 9 with y fixed at 1712, so the
purple flame stays on the screen while the code moves the layer 0 y limits up 1 px every 4 updates;
the frames are four places in the layer 2 map that a visual object picks through `+0x40` (ox); two
invisible objects with contact 127 are the kill line [certain; `heatnix-5_snake-rising`]. Its cells
have bits 14 and 15 set and semi-transparent colours, and it is seen in front of an opaque layer 0:
cell bit 15 draws in front [likely], bit 14 is the semi-transparent draw [guess; the BG draw code
is not read].

**A boss in screen space.** Illumina (overlay 11, main 24) and all its pieces have `layer`
(`+0x14`) = −1. `DrawObject4` (`0x800232D4`) then draws `x`, `y` as screen coordinates;
`BoxOverlap` (`0x8002D654`) and `WeaponBoxTest` (`0x8002D868`) add the layer 0 camera x and y to
an object with `layer` < 0, so its boxes still meet world objects. Illumina's weak spot is a lens
on a cable drawn in BG layer 1 tiles; its screen box covers the lens only while it locks the camera
(`docs/enemies/illumina.md`) [certain for the code].

**Boss sequence.** Rainy Turtloid, Shield Sheldon, Commander Yammark and Metal Shark Player share
one frame: `PlayerEvent(29, 64)` and engine object 24 at the intro; at death 127 updates of flashing,
then engine object 26 (`BossExplosion`, stage logic 26, `0x80053CDC`), then a Nightmare Virus soul
(`var` `0x10`, 200 souls, when the fight took under 5401 updates, else `0x11`) [certain for the
four codes]. `BossExplosion` makes rays, explosions and `Rumble(10)`, then `WhiteFadeOut(25)`; when
the screen is white it sets its `+0x07` = 1 (a dead boss can wait for that signal); after 40
updates (200 with flag bit 7) `WhiteFadeIn(25)`; at the end `game.player_lock` = 1 and it deletes
itself. Blizzard Wolfang, Ground Scaravich and Infinity Mijinion (overlay 11, fight timer
`0x800FDA84`) use the same death and soul code.

- A type number names an object only inside one class and one stage. Types whose code is in the main
  program are shared by all stages.
- **Boss overlay.** A second overlay can be loaded at `0x800FA000`, above the stage overlay. The
  stage's main table can point into it before it is loaded. Two loaders are known [certain for the
  code]:
  - stage logic type 13 of stage 21 (`BossSpawner`, `0x80052E44`, the alternate Turtloid section):
    it loads overlay 43, 44 or 45 when `boss_choice` (`0x800CD30B`) is 0, 1 or another value
    (`LoadOverlay(n, address)`, `0x80016858` [likely]). When the load ends (`0x80014B78` returns not
    0), it makes main type 53, 55 (`var` 1) or 63, calls `0x8002B3D4(88, scratchpad[+0x30] + 0 /
    256 / 384, 8)`, and deletes itself;
  - `0x80013E24`: when `game.stage` = 11, the overlay `0x8006DC50`[`game+0x1D`] (by checkpoint).

  In `turtloid-5_zero`, overlay 43 is at `0x800FA000` (15 of 17052 bytes differ: its variables).

  Stage 22 (area 0, overlay 31, Shield Sheldon's hidden area) has the same `BossSpawner`.
  `sheldon-12_high-max`: `boss_choice` 1, overlay 44 at `0x800FA000` (1 of 33 304 bytes differs),
  main type 55 `var` 1 [certain]. Overlays of the hidden areas: stage 20 area 1 = 28
  (`shark-10_alt`), stage 21 area 0 = 29, stage 21 area 1 = 30, stage 22 area 0 = 31, stage 22
  area 1 = 32; all fit overlay = 25 + 2 × (stage − 19) + area [certain: `stage_overlays` (`0x8006DB78`)[38–45]
  = 25–32, and five save states agree]. Their stage members are 118 (stage 20 area 1) to
  122 (stage 22 area 1), in stage and area order, so 115–117 are likely stage 19 areas 0 and 1 and
  stage 20 area 0 [likely]. `mijinion-4_dynamo` (stage 22 area 1, `boss_choice` 2): overlay 45
  at `0x800FA000` (18 of 11 878 bytes differ, all variables), main type 63 = Dynamo [certain]
  (`docs/enemies/dynamo.md`).
- **`boss_choice`** is `game+0x43B` (saved at save `+0x3F`, read back by `LoadGame`, `0x8001BA1C`).
  The 8 hidden areas (stages 19–22, overlays 25–32) each have one `BossSpawner` record, so all of
  them have the same boss at a given time. The value goes 0 → 1 → 2 with game progress [certain for
  the code and for the names of 0, 1 and 2 (`turtloid-5_zero`, `turtloid-8_high-max`,
  `mijinion-4_dynamo`; the text of messages 2 and 3 of every hidden-area text bank has the speaker
  "Dynamo")]:
  - 0 at the start: Zero Nightmare. A hidden-area clear with 0 unlocks Zero (`StageExit`,
    `0x8001E32C`; `game+0x5F |= 0x10` at `0x8001E5A8`).
  - Overlay 16 mode 11 (`0x800ED6CC`) sets 1 when the value is 0 and Zero is unlocked: High Max.
    Overlay 44 has the same code as the intro's main type 55: every walked instruction matches
    after relocation. `var` 0 is the intro fight (made by intro code at `0x800F9A54`), `var` 1 the
    hidden areas, `var` 2 a third fight (probably stage 12) [certain for the code; the stage of
    `var` 2 is a guess].
  - Overlay 20 (`0x800EE7E8`) sets 2 after a clear with 1 and opens Gate's lab. Overlay 16 mode 10
    (`0x800ED5F8`) also sets 2 when Gate's lab opens in another way (all 8 Mavericks, or 3000
    Nightmare Souls), also from 0: Dynamo.
  - The overlay 16 writers act only when `text_open` (`0x8008EAFC`) = 0, that is, with no text box open. No random value
    is used. The value is also read for the portrait of text slot 2 (`PortraitCharacter`, `0x80021AF0`: 22 / 8 / 21), a
    file id (`0x800143B4`: 193 / 197 / 195) and `0x80079F20` (138 / 139 / 140, meaning open).
- **Progress bytes, seen across the Turtloid states** [observed; one clear only]: before and after
  the Zero Nightmare clear, `game+0x5F` goes `0x01` → `0x11` (the Zero unlock bit `0x10`), and
  `game+0x5E` (the form) goes 1 → 5 when the user picks Zero: form 5 is Zero. The character is
  `game+0x38` (0 X, 1 Zero); the armour select sets both.
  `game+0x60` goes `0x01` → `0x21` at the Rainy Turtloid clear (`report-1`), and `0x21` → `0x29`
  at the Metal Shark Player clear (`report-2_shark`). So the defeated-Maverick bit is
  `1 << (stage − 1)`: `0x01` Commander Yammark (stage 1), `0x08` Metal Shark Player (stage 4),
  `0x20` Rainy Turtloid (stage 6) [likely: three points]. X's owned special weapons (`mega+0xC9`)
  are `0x21` in the Shark states and the report: the same bits for Yammark and Turtloid; the Shark
  weapon bit is not set yet on the report screen. The souls go 464 → 664 (+ 200) across the boss
  fight and the report, and the lives 8.
- A boss whose bank has no texture-table entry (D-1000, Zero Nightmare, High Max, Rainy Turtloid)
  holds its own sprite source
  in `+0x3C`, `+0x40`, `+0x42`. Its texture can cover the cells of other banks while it is live.
- The `GetXObject` functions return the first free slot, cleared, or 0 when all slots are used. The
  caller then sets byte 0 (flags, `0x41` for spawned objects) and byte 1 (type).
- The player is at `0x800970A0` (`mega`). `mega+0x02` is the character: 0 = X, not 0 = Zero
  [likely]. The player's HP in the intro save state is 32.

## Spawning and despawning

- **Records.** Each stage area has a list of 8-byte records: flags, type, `id`, `kind`, x, y (see
  `docs/objects-legend.md`). The list of a stage and area is found through `enemy_set_tbl`
  (`0x8007349C`, stage × 8 + area × 4); a record of kind 15 ends it (`0x80042C48`). The spawner is
  `SpawnInRect(x0, x1, y0, y1, flag)` (`0x80029F38`): the record's type goes to the object byte 1 and
  its `id` to byte 2 (`var`) [certain]. The object keeps a pointer to its record (`+0x10`).
- **Second list.** `fixed_set_tbl` (`0x80073554`, same index) points to a second list per area. In
  Turtloid it holds the objects of fixed place: stage logic, the rain controller, scenery (kind 5,
  misc objects) and the Nightmare shadow spawners (kind 0, type 26); most are at (0, 0) [certain for
  Turtloid].
- **Checkpoints.** Stage-logic type 6 (`CheckpointMarker`, `0x80052D30`) sets `game.checkpoint`
  (`game+0x1D`) = `id & 0x7F` when the player's x reaches the record x (y when `id` bit 7 is set)
  [certain for the code].
- **Death.** After a death and respawn, no object behind the respawn point is live and every
  record's "spawned" bit and kind high digit are cleared, also for objects that never delete
  themselves (Turtloid: a Weather Analyze wreck, the rocket platforms). The Rescue Reploid status
  survives [certain from two Turtloid save states].
- **Spawners.** Some main types only make another object and delete themselves. Example: main type 7
  in Yammark's stage makes the Caterpirade in a stage-object slot (`GetStageObject`, `0x8002C4C4`).
- **Difficulty filter.** A record of kind 0–2 is spawned only when the difficulty
  (`game.difficulty`, `0x800CCF38`, 0 = Easy … 3) is at least the kind. Difficulty 3 skips the filter
  (`0x80029E7C`) [certain for the code]. The sheets call difficulty 0 Easy, 1 Normal, 2 Hard
  [likely].
- **Spawn band.** The spawner makes the object when the record point comes within 48 px (3 blocks)
  of the 320 × 240 view [certain]. Bit 0 of the record flags marks "spawned", so one record makes
  one object at a time; a record of kind 0 also gets kind `|= 0x10` [certain: Yammark record 59 is
  `01 07 00 10` in RAM and `00 07 00 00` on the disc].
- **Despawn.** `DeleteObject` (`0x8002CA14`) clears the record's "spawned" bit (`flags &= 0x70`) and
  the object. It skips the record when `+0x10` is 0 (an object that no record made).
  `DeleteObject2` (`0x8002CA54`) also changes the record kind high digit `0x10` to 0. Objects use
  `DeleteObject2` when they go off screen and `DeleteObject` when they are killed (Nightmare Virus
  states 3 and 2). `DeleteObject3` (`0x8002C9B0`) only clears
  the object (shots and effects). `DeleteObjectKeep` (`0x8002C9D4`) sets bit 7 of the record flags
  (`flags |= 0x80`) and then clears the object. The spawner skips a record when `flags & 0x81` is not
  0 (`0x8002A014`), so that record never spawns again while the area is loaded (single-use objects:
  a saved Rescue Reploid, a used Light capsule) [certain for the code].

## The standard main-object update

Most main objects follow this pattern [certain for every enemy read so far]. The sheets give only
the table addresses and what differs.

| state | usual routine |
|---|---|
| 0 | set-up: HP, contact damage, boxes, damage table, draw priority, layer = `mega.layer`, animation, first step; then state 1 |
| 1 | main: one update of behaviour |
| 2 | after death: `DeleteObject` |
| 3 | far off screen: `DeleteObject2` |

Each update, the update function:

1. saves the position in `prev_x`/`prev_y` (`+0x18`, `+0x1C`);
2. calls the routine for `state` (`+0x04`); the main routine calls the routine for `step` (`+0x05`),
   and a step can have sub-steps in `step2` (`+0x06`);
3. calls `CollideCheck` (`0x8002F288`) when the object has a terrain box. It uses the move since
   `prev_x`/`prev_y` and sets `collide_flags` (`+0x70`): 8 = floor, 4 = ceiling, 1 and 2 = the two
   walls [likely for the bit names].

**Step 0** of every step table is the engine routine `HitReactionDispatch` (`0x80040290`). It picks
a reaction by the hit class at `+0x63`. No enemy read so far sets step 0 itself, and all their
damage tables have only classes 0, 2 and `0xFF`. So it is probably not used by these enemies
[likely].

**Stage objects** (record kind 4) differ: step 0 is the object's own routine, not
`HitReactionDispatch`, and the step routines get `game` in `a1` and `mega` in `a2` [certain for the
boss gate].

**The main routine** usually does, in an order that the sheet gives:

- the step;
- the **fall check** (ground enemies): no floor flag and `latch` (`+0x67`) = 0 → the fall step,
  `speed_y` = 0, gravity set, `latch` = 1. On landing the step clears the latch;
- the **weapon test** (`ContactWeapon`) and death;
- the **squeeze test** (some ground enemies): floor and ceiling flags in the same update (or both
  wall flags) → death;
- the **contact test** (`ContactMega`, `0x80030ECC`): the attack box against the player; it applies
  the contact damage;
- the **screen tests**: `OffScreenCheck2(obj, rx, ry)` (`0x8002CBFC`) is 1 when the object is outside
  `[−rx, 320 + rx) × [−ry, 240 + ry)`; the caller then sets state 3 (the check only returns the
  value). Else `DisplayCheck(obj, rx, ry)` (`0x8002CCB0`)
  sets `display` (`+0x03`) when it is inside the view plus the margins. `OffScreenCheck`
  (`0x8002CAB4`) uses a fixed margin of 64 px; `DisplayCheck1` (`0x8002CB50`) a fixed 32 px. The
  sheets give the margins as "off screen 64, visible 32".

**Movement:**

- `MoveObject` (`0x8002D2B0`): `x += speed_x; y −= speed_y`.
- `MoveObject2` (`0x8002D230`): as `MoveObject`, then `speed_x` += `accel_x` in the facing direction,
  `speed_y` −= gravity, and `speed_y` is limited to `fall_max` (`0x8009719C`, −6.5 px/frame).
- `SetSpeedDir(obj, dir)`: 1 px/frame in a 32-way direction. Many objects then scale the speed.
- `GetRNG` (`0x8002D2D4`) returns 0–255 [certain].

## Solid objects

Some objects are solid for the player: the boss gate, the Caterpirade's back, the Light capsule.
They do not use tiles. Some of them (the junk block, the Light capsule, the intro's sabre block) also
set `+0xA2` = 1, so that `SolidRectsBuild` (`0x8002E84C`) adds their terrain box to `solid_rects`
(`0x800E41B0`) at the start of each frame. Only `TerrainPointCheck` and `CornerCheckRects`
(`0x8002E974`) read these rectangles (as value 56); the player's `CollideCheck` does not [certain for
the code]. The object calls
`SolidObject(obj)` (`0x80031DA8`, main program) each update [certain for the code]. It does nothing
when the object has no terrain box or when `solid_off` (`0x800C4568`) is not 0. Else, for the
player:

1. **Push out** (`SolidPush`, `0x80032208`): when the player's centre is inside the object's terrain
   box, `mega+0x71` = 15 and `mega+0xE6` = the object's contact damage [certain; the effect is not
   known]. Else, when the two terrain boxes overlap, it moves the player out along the shorter axis
   (the amounts from `0x8002DFA0` / `0x8002E0DC` go to `+0x6C` / `+0x6E`). So the body is solid on
   all four sides.
   `SolidPush` returns at once when the pushed body has no terrain box (`+0x68` = 0), so while the
   player rides it acts only on `actor2`. `SolidTouch` writes `mega+0xE6` = the contact damage for
   any contact, also with `actor2`; the touch flags go to the touched body's `+0x71`. After the
   `actor2` part, `SolidObject` copies the ride's x and y to the player while the player is in step
   18 [certain for the code].
2. **Carry** (`SolidCarry`, `0x800324C8`; only when `obj+0x76` > 0, that is, the player touched it
   in the update before), by the touch flags `obj+0x72`:
   - `0x08` (player on top): the player's x moves by the object's x move of this update, only when
     `mega+0x138` = 0, and the move is stored there (so only one object carries the player per
     update) [likely for the reason]. When the object moves down, the player's y moves with it; when
     it moves up, the player's feet are put on its top.
   - `0x04` (player under it): the same x carry, and the player is put under it, only in player step
     31 (with `mega+0x06` < 4) or 63.
   - `0x01`, `0x02` (player at a side): the player is put against the side, only in player steps 9,
     11, 47 or 52 [guess: wall slide and wall kick].
3. **Touch flags** (`SolidTouch`, `0x8002E1A0`): exact contact of the two terrain boxes: `0x08` the
   player is on top, `0x04` below, `0x01`/`0x02` at a side. They go to `obj+0x72` and are ORed into
   `mega+0x71`. `obj+0x76` = 1 for a contact on top or at a side, else 0. Any contact sets
   `mega+0xE6` = the contact damage.
4. When the player moved: `CollideCheck(mega)`. When the player is then between floor and ceiling,
   or between two walls, the player goes back to its old position (an object cannot squeeze the
   player into the terrain).
5. The same for a second body, `actor2` (`0x800CD340`; flags in `obj+0x73`, carry flag `obj+0x77`).

On the player side [certain for the code, likely for the effect]:

- The player update clears `mega+0x71`, `mega+0x72` and `mega+0x4A` after its own step
  (`0x800350F8`); the objects set them again in the same frame. Bit `0x08` of `mega+0x72` means
  "standing on an object".
- With that bit set, the player's `CollideCheck` skips the floor-tile test while the player moves
  down (`0x8002F328`): the player stands on the object, not on the tiles, so tiles under a carrying
  object (spikes) do not act.
- When both sides of the player are blocked and one of them is an object push (`mega+0x71`),
  `PlayerHitCheck` (`0x80039228`, branch `0x80039324`) applies the squeeze damage: `mega+0xE5` = 75
  (−1 per update at `0x80035020`, read by stage objects), HP −= `mega+0xE6` (the pushing object's
  contact damage, or 127), and `+0x72` and `+0x76` of all 80 stage objects are cleared. Terrain on
  both sides kills at once [certain for the code]. This is not the general hurt (below, "Damage to
  the player").
- Other callers of `SolidObject`: `0x8005026C` and `0x800510E4` (in the boss gate code),
  `0x800516B8` and `0x800518E4` (in the Light capsule code).
- `IceStackBlockMain` (`0x800EBF18`, overlay 04): only when `mega.squeeze_timer` = 0 and |dx| <
  48. It reads the touch flag `0x04` (player under the block) after the call and breaks the block
  when a copy of the player (`0x800F76EC`) stands on floor tiles (`docs/enemies/ice-stack-block.md`).
- `SolidMainObjects(obj)` (`0x80031F4C`): the same push, carry and squeeze undo for the 48 main
  objects with a terrain box (a touch bit per object in `obj+0x98`) [likely for the carry].
- `SolidVisualHit(obj)` (`0x80032BE8`): every live visual object whose box touches `obj` gets
  `collide_flags` |= `0x0F`, so a shot that checks its flags ends on the object [certain for the
  code].

## Game state

The game work is at `game` = `0x800CCED0`. The fields that objects use [certain for the addresses;
the names are likely unless marked]:

| field | address | meaning |
|---|---|---|
| `+0x0C`, `+0x0D` | `0x800CCEDC` | stage, area. Stage 13 area 0 is the stage select, stage 13 area 1 the boss intro, stage 15 area 0 the report |
| `+0x0F` | `0x800CCEDF` | exit request: `0x40` + n = area change (teleporter), `0x10` = player step 25 (after a boss soul), else beam-out. `0x80` = a boss death in stage 12: Rainy Turtloid, and High Max `var` 2, which also sets `game[0x2C + checkpoint]` = 1 and checkpoint += 9 [certain for the code]. Ground Scaravich's death in stage 12 does the same with checkpoint += 8 (`0x800F0774`, overlay 08; `docs/stages/scaravich.md`) [certain for the code] |
| `+0x10`…`+0x19` | `0x800CCEE0` | stop flags: not 0 = that class is frozen; `PauseObjects` (`0x8004FE58`), the boss gate and `SetStopFlags(1)` (`0x8004E78C`, the health capsule) write 1, the teleporter writes 2. For the player (`+0x10`): see "The player" |
| `+0x12`…`+0x1A` | `0x800CCEE2` | the player's death sets them all to 1 for 8 updates (everything freezes) |
| `+0x1C` | `0x800CCEEC` | player input lock |
| `+0x1D` | `0x800CCEED` | checkpoint |
| `+0x1E` | `0x800CCEEE` | `game.ready`: 1 when the READY text ends (`ReadyText`); the player set-up waits for it (see "Stage start") |
| `+0x20` | `0x800CCEF0` | the boss object (Yammark's set-up writes it) |
| `+0x24` | `0x800CCEF4` | set to 1 when the boss HP bar is shown [likely] |
| `+0x26` | `0x800CCEF6` | a per-stage flag. Stage 4: 1 while the Ride Armor touches a floor and a ceiling at once (`RideUpdate`); the Nightmare Press then stops and rises, and the teleporter crystal reads it. Stage 4 area 1: Nightmare Pressure's death writes 1 (`0x800EB60C`), which ends `AutoScroll`. Turtloid: rain on (Weather Analyze). Boss gates `id` 7, 8, 10, 12, 14, 15 open only when it is not 0, `id` 44 only when it is 0; the teleporter `id` 3 waits until it is 0. In the boss intro (game modes 5, 6): the stage to load after the intro; `+0x27` the area |
| `+0x27`, `+0x28` | `0x800CCEF7` | more per-stage flags. Stage 4: `+0x27` = 1 in each update of a Nightmare Press rise. Turtloid: `+0x27` holds the rain timers; `+0x28` = Weather Analyze machine 3 destroyed, and boss gate `id` 34 opens only when it is not 0 |
| `+0x2E` | `0x800CCEFE` | 1 after Yammark's pre-fight text [likely: "text seen"] |
| `+0x36` | `0x800CCF06` | the picked Maverick stage (1–8), written by game mode 5; the stage load uses it to pick the boss intro members (see "Boss intro") [certain for the code] |
| `+0x39` | `0x800CCF09` | lives (maximum 9) |
| `+0x5B`[2] | `0x800CCF2B` | maximum HP, per character |
| `+0x38` | `0x800CCF08` | character: 0 X, 1 Zero; picks the player member and overlay |
| `+0x5E` | `0x800CCF2E` | player form, copied to `mega+0xE7`: 0 no armour, 1 Falcon, 2 Shadow, 3 Blade, 4 Ultimate, 5 Zero |
| `+0x60` | `0x800CCF30` | Zero's skill bits, copied to `mega+0xC9` (0 in form 2) |
| `+0x63`[2], `+0x6A` | `0x800CCF33` | sub tank levels; tanks owned |
| `+0x68` | `0x800CCF38` | difficulty |
| `+0x69` | `0x800CCF39` | armour part bits (Light capsules) |
| `+0x8C` | `0x800CCF5C` | boss HP maximum (the HP bar) |
| `+0x8D`[2] | `0x800CCF5D` | a counter per character that sets the boss level: level = `0x80071D80`[it] = {3, 3, 2, 1, 0, 0, 0, 0} for 0–7; 7 in the Yammark save state [certain for the code; what it counts is open] |
| `+0x8F` | `0x800CCF5F` | boss level, written by `BossLevel` (`0x80025FD8`): 3 in stage 12 area 0 (it compares the halfword `+0x0C`), else from `+0x8D`. Yammark: HP + {0, 6, 12, 16}[level] |
| `+0x90` | `0x800CCF60` | stage time, 32-bit |
| `+0x94` | `0x800CCF64` | kill count |
| `+0x9E`[k] | `0x800CCF6E` | `talk_seen`: one halfword per talk index k, one bit per text id (see "Talk zones"). In `intro-3_boss` the intro word is 1. No code that clears it was found |
| `+0x98` | `0x800CCF68` | `game.damage_taken`: the HP that the player lost (32-bit; `ContactMega` after scaling, also the ride; the squeeze damage; all the HP that is left at a pit, crush or spike death) [certain for the writers]. The mission report (overlay 20) shows it, capped at 999 [likely] (`docs/damage.md`, "Counters") |
| `+0x9C` | `0x800CCF6C` | `game.max_hit`: the strongest weapon hit so far, after the bonuses (16-bit; `ApplyWeaponHit`) [certain for the writer] |
| `+0xD2`[2] | `0x800CCFA2` | Nightmare Souls, per character |
| `+0xD8` | `0x800CCFA8` | Rescue Reploid status, 4 bits per Reploid `id` (word `id >> 3`, nibble `id & 7`): 2 saved, 3 killed, 4 lost. Yammark uses `id` 0–15; the field length was not read |
| `+0x118` | `0x800CCFE8` | a copy of the 16 Reploid status words, made at the stage start [certain for the copy; the use is not known] |
| `+0x168`[stage] | `0x800CD038` | `game.stage_effect`: the Nightmare Effect value of each stage (see below) |
| `+0x178` | `0x800CD048` | collected-soul bits, one 32-byte row per stage area (row from `0x80075FE8`) |

- **Nightmare Effect** [certain for the code and for the values in the states]:
  - **The stage pick writes it.** When the player picks a Maverick stage (overlay 16, the stage
    select, near `0x800EB278`), the code takes 3 target bytes for that stage from `0x800F0F14`
    (stages 1–8: `3 7 7`, `4 4 4`, `2 8 8`, `3 5 8`, `4 7 7`, `1 5 5`, `2 6 6`, `1 6 6`). For each
    target t it writes `game.stage_effect`[t] = the picked stage number, and it writes the 3
    targets to `game.effect_new` (`game+0x465`). It writes at the pick, not at a clear:
    `yammark-1_start` already has the bytes of the Yammark pick. A new game (`0x8001E088`) clears
    the 16 bytes. Nothing else clears them, so a value stays until a later pick writes over it.
  - **The stage start reads it.** `StageStartReset` (`0x8001E914`, store at `0x8001E9D0`): when
    `game.stage` < 19 and `game.area` = 0, `nightmare_effect` (`0x800CD30A`, `game+0x43A`) =
    `game.stage_effect`[stage]. The same function clears the stage time, the kill count and the
    damage total, and copies the Reploid status words to `game+0x118`.
  - **The value is the stage that caused it.** The objects of each effect test the value: 1
    Nightmare Bug (stage 7, main type 22, `docs/enemies/nightmare-bug.md`), 2 main type 35 of stage 4 area 1
    (44 records, set-up `0x800F01BC`; blue ice-like blocks), 5 the falling crates of stage 4 (stage
    object 15, set-up `0x800ECBEC`), stage object 15 of stage 4 area 1 (set-up `0x800EEC50`) and a
    stage 7 object (state table `0x800FC6B8`, not
    read), 3 main type 19 of stage 2 (set-up `0x800EEEA0`) and of stage 8 (overlay 11, set-up
    `0x800ECF30`, entry `0x800ED624`), a fixed-list record, `id` 1: a spawner; `var` 0 is an orange
    fire ball, export `enemy-78c8f556f5`, HP 8, contact 4, damage table `0x8007465C`, 3 px/update
    down, sound (2, 80), drop table 18. The spawner (`0x800ED1F0` in overlay 11) works only in zones:
    stage 2 player x 4641–5039, y 225–1263 at checkpoint 2, and x 4993–5679, y 1281–1695 at
    checkpoint 3; stage 8 player x 2513–4223 (between the two Illumina fights). In a zone:
    `Rumble(8)` once, every 20, 30 or 40 updates 0–2 balls at the camera top (x + (0–3) × 80 +
    (0–2) × 32, y − (0–4) × 16), and a camera y shake of ±2 (±6 at random) every second update
    [certain for the code],
    4 the grey block, Nightmare Iron (stage object 12 of stages 3 and 5 and of the stage 21 hidden
    area; `docs/enemies/grey-block.md`), 6 wind and rain (`rain-effect`), 7 Nightmare shadows
    (stages 2 and 6, magenta Zero silhouettes for X; `docs/enemies/nightmare-shadow.md`), 8 the
    overlay 03 object at `0x800EE154`. Stage 3 (overlay 05) has both the grey block (value 4) and
    the Nightmare Bug (value 1, main type 22) [certain for the two reads].
  - **The stage select highlight uses only the last pick.** The portrait modes (overlay 16,
    `0x800E9B60`) are 3 for a cleared Maverick and 1 otherwise; then each non-zero byte of
    `game.effect_new` sets its portrait (map `0x800F0BBC`) to mode 5, the purple Nightmare mark.
    `game.stage_effect` is not read there. So a stage that got its effect from an earlier pick
    has the effect but no mark.
  - Values seen (stages 0–8; `game.effect_new`): `intro-1` all 0; `yammark-*` `0 0 0 1 0 0 0 1 0`
    (`3 7 7`); `turtloid-*`, `report-1` `0 6 0 1 0 6 0 1 0` (`1 5 5`); `sheldon-*`
    `0 6 7 1 0 6 7 1 0` (`2 6 6`). In the Sheldon states `nightmare_effect` is 1 (the Yammark
    pick); in the Yammark and Turtloid states it is 0. `shark-1_ride` `0 6 7 4 0 4 7 1 4` (`3 5 8`):
    the stage 4 pick after the Sheldon pick; stage 4's byte is 0, as the rule predicts. Only the
    picks of stage 2 (`4 4 4`) or stage 5 (`4 7 7`) can give stage 4 an effect (2 or 5).
    `wolfang-*` `0 6 7 4 2 4 7 5 4` (`4 4 4`), `nightmare_effect` 7: stage 2 has 7 from an earlier
    stage 7 pick, and the last pick (stage 2, targets `4 4 4`) did not write over it.
- **Wind.** `mega.wind` (`mega+0x150`, `0x800971F0`) = 1 makes `PlayerMove` (`0x8002D5D0`) move the
  player at 3/4 of the x speed when the player faces right (a head wind) [certain].
  `PlayerMoveAccel` (`0x8002CFB0`, the dash) uses 86/100 instead [certain for the code].
- **Player HP bit 7:** with HP > 0, a hit waits for its reaction; HP = `0x80` (−128) is dead
  (`PlayerHitCheck` then sets HP 0 and player state 2). `ContactMega` returns at once while it is
  set (`0x80031064`). The rain damage writes HP directly, so HP 1 − 1 becomes `0x80` [certain for
  the code].

## The player

The player object is `mega` (`0x800970A0`). In its state 1, `PlayerUpdate` (`0x80034FBC`) counts its
timers, runs the request checks, and then runs its step (`mega+0x05`) from `player_steps`
(`0x80073ACC`, 78 entries; 32–77: `docs/players/steps-extra.md`) [certain for the code].

- **Update order.** `WeaponUpdate` runs before the step; `ChargeUpdate` (`0x8003EC78`),
  `ShotPalettes` (`0x8003CE38`) and the hurt box (`0x8003C3C8`) run after it. `mega+0x7E` holds
  the buttons of the update before. For X, `mega+0xC9` holds the special weapons owned (Zero: the
  skill bits) [certain].
- **Part bits** of `mega+0xEC` [names likely; they match the code effects]: `0x4` Speedster, `0x8`
  Jumper, `0x10` Hyper Dash, `0x20` Energy Saver, `0x80` Buster Plus, `0x100` Speed Shot, `0x40000`
  Rapid 5, `0x80000` Ultimate Buster, `0x100000` Quick Charge.

- **Stop flag.** When `game+0x10` ≠ 0, the step does not run: the player does not move and takes no
  input. When it is 1, three more routines still run after the step (`0x8003EC78`, `0x8003CE38`,
  `0x8003C3C8`); when it is 2 (the teleporter), they do not [certain for the code; what the three do
  is not read].
- **Steps** [certain for the code; the names are likely]:

  | step | routine | what |
  |---|---|---|
  | 2 | `0x80035274` | stand. Every save state has the player standing in step 2. It tries the other actions in turn; a left or right button goes to step 3 |
  | 3 | `0x800353C0` | walk start: x speed `mega+0xF0`. At the end of its animation, step 4 |
  | 4 | `0x800354E8` | walk: x speed `mega+0xF4` |
  | 19 | `0x80037C74` | boss gate walk (`PlayerGateEnter`, `docs/enemies/boss-gate.md`) |
  | 20 | `0x80037DC4` | event hold: the player stands, only the animation runs. When `mega+0xD0` = 0, step 2 |

  Steps 3 and 4 go back to step 2 when the button is released or a wall is on that side
  (`mega+0x89` bits 0–1). The buttons are `mega+0x7C` (bits 0–1: the two directions). The other 27
  steps are not read yet.
- **Events.** An object asks for an event step with `PlayerEvent(step, facing)` (`0x8003D308`):
  `mega+0xD0` = 1, `mega+0xD1` = step, `mega+0xD2` = facing, `mega+0x7A` = 1 and the player lock
  (`game+0x1C`) = 1. At its next update the player takes the request (`PlayerRequests` `0x8003A628`,
  then `PlayerStartEvent` `0x8003AAD0`: `mega.step` = `mega+0xD1`). `PlayerEventEnd`
  (`0x8003D330`) clears `mega+0xD0`, `mega+0x7A` and the lock. Users: D-1000 (20, 29), the Light
  capsule (20, 24), the health capsule (20). The event steps start at 20; the set-up of each is a
  switch at `0x800113C4` [certain for the code].
- These two routines were named `MusicPlay` and `BossMusic` (guesses) before; those names were wrong.
- **Frames.** Only the current frame of the player is in VRAM, at (320, 0). `PlayerFrameStream`
  (`0x80017B04`) loads a new frame when `mega+0x47` (wanted) differs from `mega+0x48` (loaded). The
  frame table is at `mega+0x38`; entry n is a word: bits 20–31 = tile count, bits 0–19 = offset from
  the table start to the packed frame. The packed frame is word LZ (`0x80018BE0`); it unpacks to
  tile count × 128 bytes, into a buffer (`0x800C8868` when `mega+0x49` = 3, else `0x800C9868` +
  `mega+0x49` × 4096). Then it goes to VRAM in parts of 16 tiles (64 × 16 halfwords) [certain: the
  unpacked frame matches VRAM in 4 states]. `mega+0x3C` is the sprite data, as for other objects.
- **All frames are in RAM** while the stage runs: X has 228 frames (table `0x80103ACC`), Zero has 240
  (table `0x80103A78`). The table and its packed frames are the same bytes in every state of one
  character [certain].
- **Disc source.** The player packages are `ROCK_X6.DAT` members 85–89, all with the same structure:
  a header, then the frame table at offset about 151 600, then smaller frame sets (probably shots and
  effects). Member 87 is X (the frames at `+0x2504C`) and member 85 is Zero (`+0x25018`) [certain:
  byte match]. The stage load (`0x80013BF0`) picks the member: Zero (`game+0x38` ≠ 0) 85; X by the
  form: 1 → 87, 2 → 88, 3 → 89, 0 and 4 → 86 [certain for the code]. Byte match: 87 in the Falcon
  states, 86 in `sheldon-4_no-armor`, 89 in `yammark-4_blade-armor` [certain]. Each X member has
  one 228-frame set; the second
  set has 66 (86), 93 (87), 94 (88) and 76 (89) frames. Part of the
  member is loaded at a fixed offset: member 87 offset `+0x28000` is at `0x80106A80`, and the
  part near `+0x9C000` holds the sprite data at `mega+0x3C` [likely].
- **Sprite n is frame n** of the current frame set. `mega+0x47` is the sprite byte of the current
  animation step (`+0x44` is the step word: ticks, flag, step, sprite) [certain].
- **Frame sets.** The player's `SetAnime` (`0x8003BA04`) picks the frame table (`mega+0x38`) and the
  sprite data (`mega+0x3C`) for each animation, from a byte per animation in the player overlay and
  two directories: `scratch+0x14` (frame tables) and `scratch+0x1C` (sprite data), as offsets from
  the directory start [certain for the code]:

  | character | byte table | byte 0 | byte 1 | byte 2 |
  |---|---|---|---|---|
  | X (overlay 0) | `0x801F49F4` | frames 0, sprites 0 (228) | frames 10, sprites 18 (93 with member 87, 66 with 86) | (byte ≠ 0 is set 1) |
  | Zero (overlay 1) | `0x801EF620` | frames 0, sprites 0 (240) | frames 1, sprites 5 (113) | frames 2, sprites 7 (102) |

  X uses set 1 for 50 animations (83–85, 140, 144–192: wall and dash poses among them), Zero uses
  sets 1 and 2 for 96. It also writes `mega+0xDC` (5 or 19 for X): the sprite entry of X's armour
  layer (`docs/players/steps-extra.md`, "Sprites by form") [certain for the code]. Four X
  animations (85, 180–182) and one Zero animation (118) use sprites past their set [open].
  The player's cell mode (byte 0 bits `0x60`) is 0, so the tiles are not remapped.
- **Player overlay.** The player code and its animation table (`mega+0x30`) are in an overlay at
  `0x801EA000`: overlay 0 for X (44 588 bytes, table `0x801F4AD4`, 193 animations) and overlay 1 for
  Zero (22 964 bytes, table `0x801EF6D0`, 169 animations). In every state checked the overlay is
  unchanged from the disc file [certain]. The palette `0x7800` (VRAM row 480) is member offset
  `0x800` [certain for X and Zero].
- **The player specs** are in `docs/players/`: the code map (`map.md`), ground (`movement.md`), air
  (`air.md`), ladder, hit reaction, beams and events (`ladder-hurt-events.md`), attacks
  (`attacks.md`), steps 32–77 and the forms (`steps-extra.md`).
- **Player states** (`player_states`, `0x80073ABC`): 0 set-up (`PlayerInit` `0x8003CBE4`: wait for
  `game+0x1E`, then the beam-in), 1 `PlayerUpdate`, 2 death (`PlayerDeath` `0x8003CC78`), 3 nothing
  [certain].
- **More steps.** `player_steps` (`0x80073ACC`) has no bound test and 78 entries: 32–47 and 74–77
  are X's overlay steps, 48–63 Zero's, 64–71 stand, and 72 and 73 have no writer
  (`docs/players/steps-extra.md`) [certain for the table].
- **Air latch** `mega+0x67`: 1 rising, −1 in the air, 0 on the ground. `CheckWalkOff`
  (`0x80039DFC`) starts the fall each update when it is 0 and there is no floor [certain].
- **Wall tiles.** The side-wall test `0x80030C58` (types 56–63, table `0x80010F18`) sets `mega+0x4A`
  (wall slide and kick allowed: 56, 58) and `mega+0x79` for 62, 63 (spikes: death, except in form
  2) [certain for the code].
- **Form** `mega+0xE7` = `game+0x5E`: 0 no armour, 1 Falcon, 2 Shadow, 3 Blade, 4 Ultimate, 5
  Zero [certain for the values: the armour select lists form 0 always, and form n + 1 for armour
  bit n; `sheldon-4_no-armor` has 0]. The form changes the air action, the special attack, the
  special-weapon charge, the weapon cost, the hit class, spikes, the wall slide and the sprite
  member; walk, jump and dash physics are the same in all forms (`docs/players/steps-extra.md`).

## Layout

- The layout is 3 layers of width × height bytes, one screen number per cell, layer after layer, row
  by row. Width and height are at `0x800CD338` and `0x800CD339`; their product at `layout_size`
  (`0x8008EC10`). The set-up (`0x800142A4`) reads them from a table at `0x80076398` [certain].
- The layout reader (for example `0x80027F1C`) takes the address of the layout from the scratchpad
  word `+0x04` and reads `base + layer × size + y × width + x` [certain]. The word is `0x80097438`
  (`stage_layout`) in all ten save states.
- Our tools used `0x8009741E`, 26 bytes too early, until 2026-10-21. That gave the "row 0 header",
  the Yammark "vertical wrap" and the scrambled first Turtloid map. On the disc, 26 bytes
  (`ff ff ff ff ff ff 80 …`) come before each layout; what they are is not known.
- Only layer 0 holds the collision. Screens of the background layers do not have to join their
  neighbours (intro layer 1, rows 4 and 5).
- The last two bytes of each layer's camera record before `0xFF` are width − 1 and height − 1.
- **A stage member can have two texture blocks** [certain for member 102]. Entry `0x10000` (256
  KB) is stored page by page (8 pages of 64 × 256 VRAM halfwords) and is loaded at the stage start
  to VRAM x 320–831, y 256–511. Entry `0x16` (256 KB) holds the same area stored row by row (256
  rows of 1024 bytes). In Shield Sheldon's stage, entry `0x16` stays in RAM for the whole stage
  and is in VRAM in the lower rooms (`sheldon-8`, `sheldon-9`); it draws the lower rooms and the
  boss part correctly. `learn` tries entry `0x16` when entry `0x10000` is not in VRAM and writes
  `"texture": 22`; `stages` draws with that block.
- **The texture swap** [certain]. The stage load keeps entry `0x16` in RAM (the only pointer is
  scratchpad word `+0x44`); it does not keep entry `0x10000`. `StageTextureSwap` (`0x80027A50`,
  called each frame from the main loop at `0x8001226C`) uploads it with `LoadImage` (`0x800663FC`)
  to the rectangle (320, 256, 512, 256) when `stage_texture_swap` (`0x8008EAB0`) is 1, then sets it
  to 2. A camera zone with `id` bit 7 writes 1 (stage 7: record 3, `id` 128, trigger x 4352–4383,
  in laser room 3; a teleporter crystal B2 with `id` 2 also writes 1; stage 4 area 1:
  `LowerPathWarpJump` at `0x800EDBEC`; intro: record 3, `id` 129, trigger x 2128–2159, seen in
  `intro-3_boss`). The layer set-up of the
  stage start writes 0. Entry `0x16` is row by row because one `LoadImage` of 512 × 256 needs that
  order. Whether a restart after a death loads entry `0x10000` again is not known.
- Member 98 (Metal Shark Player, stage 4) also has entry `0x16`: it draws the right shaft, the
  lower path and the bottom rooms [observed; no state with it in VRAM yet]. Member 99 (stage 4 area
  1) has it too: entry `0x10000` is in VRAM on the upper path (`shark-2`, `shark-3`), entry `0x16`
  after the warp to the mini-boss room (`shark-4` to `shark-7`) [certain].
- **Animated tiles.** The belt of stage 4 area 1 (`BeltUpdate`, stage logic 9) turns the 4-byte tile
  definitions (scratchpad `+0x0C`) of 8 tile blocks by one place every 5 updates, so byte 2 (the
  texture cell) of 32 tile info words in RAM differs from the disc; `learn` accepts that. It also
  copies cells with `CopyTiles` from a frame store in the map. Byte 0 of a definition is the
  collision value [certain].

## Tile collision

The collision value of a tile is byte 0 of its tile definition (layer 0 only).
`TileCollisionAt` (`0x80030234`) and its copy `TileCollisionAtSub` (`0x80030304`, which also writes
`obj+0x6C` = x & 15 and `obj+0x6E` = y & 15) read the whole byte. They return 0 outside the map
and 56 for 59 [certain]. The value list for tools is `catalog/collision.json`.

**`CollideCheck(obj)`** (`0x8002F288`) is the terrain test of the player and of every object with a
terrain box [certain for the code]:

1. `TerrainBoxSetup`: the box goes to `0x800E4192`…`0x800E41A0` (half sizes, offsets, position,
   centre).
2. The x move: `CollideRight` (`0x8002F71C`, cell x = cx + w − 1) or `CollideLeft` (`0x8002F5FC`,
   cell x = cx − w), 3 points each (the centre y and two edge points).
3. The y move: `CollideFloor` (`0x8002F840`) when y − old y ≥ 0, else `CollideCeiling`
   (`0x800306F4`). The player skips the floor test while `mega+0x72` bit `0x08` is set (it stands
   on an object).
4. The corner fix-up (`CornerResolveX` `0x8002FBEC`, `CornerResolveY` `0x8003091C`, and
   `0x8003056C`, `0x800305B0`); `ApplyTerrainPush` (`0x800305F4`) adds the push [likely for the
   names].
5. `TerrainTouchFlags` (`0x8002F418`): `+0x70` = 2 left wall, 1 right wall, 8 floor, 4 ceiling.

When the floor test places the object on a slope (`SlopeSnap`, `0x80030834`), it sets `slope_hit`
(`0x800E4190`) = 1 and `+0x70` = 8, and `CollideCheck` ends before steps 4 and 5.

**The tests by value** [certain for the code]:

| test | function, table | values |
|---|---|---|
| floor, centre point | `FloorValueTest` `0x8002F9DC`, `0x80010C70` | 1–12 and 17–28 slopes; 33, 34, 56–58, 60, 62, 63 flat floor at the cell top; 16: the test uses the cell 16 px higher; all other values: no floor |
| floor, box corners | `FloorCornerTest` `0x800308B8`, `0x80010DF8` | 33, 34, 56–58, 60, 62, 63 |
| walls | `RightWallValue` `0x80030690` (`0x80010DB8`), `LeftWallValue` `0x80030628` (`0x80010D98`) | 56–58, 60, 62, 63 |
| ceiling | `CeilingValue` `0x800307CC`, `0x80010DD8` | 56–58, 60, 62, 63 |
| touch flags | `FloorCeilingValueTest` `0x80030AE8` (`0x80010E98` floor, `0x80010E78` ceiling); `TouchWall` `0x80030B80` → `SideWallTest` `0x80030C58` (`0x80010F18`) | 62 and 63 on any side set `obj+0x79` (death) unless `mega.form` = 2. The form of the player is tested for every object |
| object point tests | `CornerCheck` `0x8002EE10`, `CornerCheckRects` `0x8002E974`, `0x8002F06C`, `IsSolidValue` `0x80030D80` (13 identical tables, `0x800100D0`…`0x80010B78`, `0x80010F38`) | 3–6, 10, 11, 16, 56–58, 60, 62, 63 (and 59 through the reader) |

So:

- **Slopes are floor only.** They act through the centre point of the box, never as a wall or a
  ceiling. On the ground the snap is always made; in the air only when the foot is at or below the
  surface. `0x8002F900` keeps a grounded object on a slope that goes down (it tests the cell 16 px
  lower).
- **Slope profiles.** h is the surface in px below the cell top, at the left and the right edge of
  the cell:

  | values | slope | h |
  |---|---|---|
  | 1, 2, 3, 4 | 4 cells, rises to the right, 4 px per 16 px | 15→12, 11→8, 7→4, 3→0 |
  | 5, 6, 7, 8 | 4 cells, falls to the right | 0→3, 4→7, 8→11, 12→15 |
  | 9, 10 | 2 cells, rises to the right, 8 px per 16 px | 15→8, 7→0 |
  | 11, 12 | 2 cells, falls to the right | 0→7, 8→15 |
  | 17–28 | the same profiles as value − 16; never solid for object point tests | |

  Values 17–28 are in the maps only on slippery floors (60: the ice of Blizzard Wolfang's stage,
  member 96, and member 116) and belt (58, member 112) floors [certain for the maps]. Stage logic 7
  makes them slippery slopes ("Slippery floors and slopes (stage logic 7)" below). Member 96 has
  25, 26 and 27, 28 only (2-cell slopes, 8 px per 16 px).
- **16** is the cell under a slope piece. In the intro, all 52 cells with 16 are directly under a
  slope cell.
- **33** (ladder top) and **34** are floors from above only. 34 has no ladder entry. In the intro
  and in stage 4 it is on the two sides of each ladder top, where the ladder art is 3 cells wide.
- Objects see the lower, almost full slope cells (3–6, 10, 11) and 16 as solid in their point
  tests, and never 17–28.
- 57 and 60 are solid with no wall slide or kick (`SideWallTest`); a ceiling hit on 57 in the super
  jump makes the player fall (`0x800364F0`). 58 is the conveyor surface: `BeltUpdate` for the
  player, `0x8004DC80` for objects (1 px left per update).
- 32 ladder, 36 water surface (`0x8003CA98`, `0x8003D358`), 37–47 bars and rope (`BarAt`
  `0x8003040C`, table `0x80010D70`; `BarSnap` `0x800397F8`; `docs/players/air.md`). 35 is the water
  body of the Turtloid map (with 36); no code compares with 35 [likely]. Infinity Mijinion's bar
  emitters write collision 45 cells at run time (`CopyTiles` from the hidden cell (5632, 1200) of
  member 103), so a temporary hang bar appears in the boss room
  (`docs/enemies/infinity-mijinion.md`).
- No handler: 13–15, 29–31, 48–55, 61.

### Slippery floors and slopes (stage logic 7) [certain for the code]

Stage logic 7 makes collision 60 slippery and pushes the player down the slopes 17–28. The same
code is in overlay 04 (Blizzard Wolfang, `0x800ED3A4`), 07 (stage 4 area 1, `0x800F0530`) and 10
(Shield Sheldon, `0x800F0BB0`): every differing word is an address part, and the two jump tables
have the same cases. One record per area, in the fixed list. It acts only while `mega.state` = 1.
It reads the collision value under the middle of the player's feet each update.

- **Flat 60 (and 0 on an object):** the slide. When the player stops, lands or dashes on 60, the
  object keeps a slide speed and adds it to the player's own x move until it falls to 0, the
  player jumps or hits a wall, or the floor is not 60 (or 20, 24, 26, 28). Start speeds and the
  slow-down: `docs/stages/sheldon.md`, "The glass slide (stage logic 7)".
- **Slopes** (value under the feet; 16 reads the cell 16 px higher). `dx` = the player's x move of
  this update (`mega.x` − `mega+0x13C`), `s` = `mega+0x104` (the dash start speed, 6.5; 8.125
  with the dash part):

  | value | slope | the player's x move becomes | y |
  |---|---|---|---|
  | 17–20, 25, 26 | rises to the right | max(`dx` − 3.5, −`s`) | + abs(move) (down; the player's `CollideCheck` puts the player back on the slope) |
  | 21–24, 27, 28 | falls to the right | min(`dx` + 3.5, `s`) | + abs(move) |

  So with no input the player goes down the slope at 3.5 px/update. A walk uphill (2.0) still goes
  down at 1.5. A dash uphill goes up at the dash speed − 3.5.
- **The foot of a slope:** when the value of the update before was 60 and the player faces up the
  slope (right for 17–20, 25, 26; left for 21–24, 27, 28) and the step is not 12 (dash), the x
  move of this update is undone (`mega.x` = `mega+0x13C`). So from flat ice the player cannot walk
  onto an up-slope; only a dash goes on.
- After the slope move the object reads the value again. When the player is now on 60, the slide
  start (state 1) runs in the same update, with the slope move as the "last x move": a player that
  slides off a slope at 3.5 px/update keeps about 4.7 px/update on the flat ice and slows down at
  0.09375 per update, about 115 px [likely: from the numbers, not seen in play].
- Every slope value puts the slide back to state 1 (armed): the slide and the slope push do not
  add up.

The object also keeps `+0x1E` = `0x40` when the player moved right in this update, 0 when left.

For the conversion: collision 60 = ice floor, 25–28 on ice = ice slope; push the player down an
ice slope at 3.5 px/frame against its own move (cap: the dash speed); stop a walk at the foot of an
up-slope; keep the run-out on the flat ice.

## Camera

Each BG layer has a camera record of `0x54` bytes; layer 0 is at `0x800971F8`, layer 1 at
`0x8009724C` [certain]. Fields (layer 0 addresses):

| offset | address | meaning |
|---|---|---|
| `+0x0A`, `+0x0E` | `0x80097202` | camera x, y in px |
| `+0x1C`, `+0x1E`, `+0x20`, `+0x22` | `0x80097214` | current limits: max x, min x, max y, min y |
| `+0x24`…`+0x2A` | `0x8009721C` | target limits, same order (`cam.target_max_x`, `cam.target_min_x`, `cam.target_max_y`, `cam.target_min_y`) |
| `+0x2E` | `0x80097226` | the screen y where the camera keeps the player (read at `0x800291E4`). 171 in every other save state with a player; Nightmare Snake `var` 10 lowers it by 64 (107 in `heatnix-6_snake-room`, where Zero is at screen y 107) [likely] |
| `+0x34` | `0x8009722C` | event flags (skipped while the player passes a gate) |
| `+0x44` | `0x8009723C` | follow the player |
| `+0x47` | `0x8009723F` | the speed at which the limits move to their targets, px/frame (`CameraLimitEase`, `0x80028EBC`) |
| `+0x4C` | `0x80097244` | redraw request |

`CheckpointStart` (`0x80029B8C`; the store at `0x80029C20`) writes each current limit and its target
from the same value (see "Checkpoints"). The boss gate and the Weather Analyze set new target
limits; the camera then eases to them. The teleporter sets the cameras of all three layers
directly. The limits are the top-left position of the 320 × 240 view.

`CameraLimitEase` moves a limit relative to the camera, not on its own. Max y (`0x80029180`), when
the target is below the camera: if (player y − camera y − `cam+0x2E` + 2) ≥ 4 (the player is more
than 1 px below the follow line `cam+0x2E`), max y = the target at once; else max y = camera y +
max(0, player y − `mega+0x1E`) + `cam+0x47`, not past the target. When the target is above the
camera: max y = max(camera y − `cam+0x47`, target). The other limits use the same pattern [likely;
min x and min y read in part]. In `heatnix-4_cave` (`cam+0x2E` = 171, Zero standing on the line)
max y = 1727 + 0 + 2 = 1729 while the target is 2624 [certain].

- **Camera zones** (stage logic 0, `CameraZone`, `0x80052258`; set-up `0x80052294`, update
  `CameraZoneUpdate` `0x8005211C`) [certain for the code]. The set-up takes a 16-byte entry from
  `camera_zone_tbl` (`0x80079D70`, word index stage × 2 + area) at `(id & 0x7F) × 16`: `u16 x0,
  x1, y0, y1` (a rectangle that the player position must be in, both ends included), then `u16
  min_x, max_x, min_y, max_y` (`0xFFFF` = keep). While the player is in the rectangle, the zone
  writes the target limits and sets `cam+0x47` = 2. With `id` bit 7 it sets `stage_texture_swap`
  (`0x8008EAB0`) = 1 and deletes itself with `DeleteObject3` (the record stays "spawned"). Off
  screen (64 px) it deletes itself. The rectangle does not have to contain the record point (intro
  record 5: record x 3776, trigger x 4000–4031). The list of a stage area ends where the next
  area's list starts. The intro zones are in `docs/stages/intro.md`.
- In Shield Sheldon's stage the laser doors (main 13, `id` `0x50`–`0x55`, `LaserGateOpen`
  `0x800EA650`) also set target limits when they open.
- `TileCollisionAt(obj, x, y)` (`0x80030234`) returns the collision value of the tile at (x, y) on
  the object's layer (59 reads as 56). See "Tile collision".
- **BG layer modes.** Each BG layer runs a mode function by `cam+0x04` (layer 1 table `layer1_modes`
  `0x80071E88`, 10 entries; layer 2 table `layer2_modes` `0x80071EB0`, 12 entries; updates
  `Layer1Update` `0x8002935C`, `Layer2Update` `0x800293CC`) [certain]. The update first copies the
  position (`+0x0A`, `+0x0E`, the integer halves of the 16.16 words `+0x08`, `+0x0C`) to the old
  position (`+0x16`, `+0x1A`). No mode function changes the mode; only mode 0 writes `+0x04`. Mode 0
  (`Layer1ModeInit` `0x80029718`, `Layer2ModeInit` `0x800298F4`) takes the start mode from
  `bg_mode_tbl` (`0x800720AC`, byte stage × 4 + area × 2 + layer − 1) and does not move the layer in
  that update. The offsets `+0x40`, `+0x42` come from the checkpoint record (see "Checkpoints").
  In the table below, x0 and y0 are the layer 0 position, ox = `+0x40`, oy = `+0x42`, and "/" is a
  floor division (`sra`, signed 16-bit values).

  | mode | layer 1 | layer 2 | x | y | cap |
  |---|---|---|---|---|---|
  | 1 | `Layer1ModeHalf` `0x8002974C` | `Layer2ModeHalf` `0x8002992C` | x0 / 2 + ox | y0 / 2 + oy | layer 1 only |
  | 2 | `Layer1ModeFull` `0x80029780` | `Layer2ModeFull` `0x80029958` | x0 + ox | y0 + oy | no |
  | 3 | `Layer1ModeQuarter` `0x80028338` | `Layer2ModeQuarter` `0x80028900` | x0 / 4 + ox | y0 / 4 + oy | yes |
  | 4 | `Layer1ModeHalfX` `0x8002844C` | `Layer2ModeHalfX` `0x8002998C` | x0 / 2 + ox | y0 + oy | layer 1 only |
  | 5 | `Layer1ModeNone` `0x800297B4` | `Layer2ModeNone` `0x800299C4` | not changed (another object moves the layer) | not changed | no |
  | 6 | `0x8002855C` | `0x80028A14` | ¾ x0 + ox | ¾ y0 + oy | yes |
  | 7 | `0x800286A0` | `0x80028B4C` | x0 + x0 / 4 + ox | y0 + y0 / 4 + oy | yes |
  | 8 | `0x800287D0` | `0x80028C7C` | x0 + x0 / 2 + ox | y0 + y0 / 2 + oy | yes |
  | 9 | `LayerModeFollowX` `0x800297BC` | `Layer2ModeFollowX` `0x800299CC` | x0 + ox | not changed | no |
  | 10 | — | `Layer2ModeQuarterX` `0x80028DAC` | x0 / 4 + ox | y0 + oy | yes |
  | 11 | — | `Layer2ModeThreeHalfY` `0x800299E8` | x0 (no ox) | y0 + y0 / 2 (toward 0) + oy | no |

  Mode 6 rounds differently on the two layers: layer 1 uses (3 × x0) / 4 rounded toward 0, layer 2
  uses x0 / 2 + x0 / 4. `bg_mode_tbl` uses only layer 1 modes 1, 2, 5, 9 and layer 2 modes 1–5;
  modes 3, 4, 6, 7, 8 (layer 1) and 6, 7, 8, 10, 11 (layer 2) have no known user [likely]. The checks
  against all save states agree (`turtloid-*`, `shark-*`: layer 2 mode 4; `sheldon-*`: mode 3 and
  the mode 5 rule below; `intro-*`, `scaravich-*`, `yammark-*`: modes 1–3) [certain]. `shark-3` is
  2 px off in layer 1 y: the press used layer 0 y 224 before a quake moved layer 0 to 226.
  - The cap (`LayerFollowCap`, `0x80029818`; inline in the other capped modes) compares the target
    with the old position: a change of less than 16 px goes to the target at once, 16–327 px moves
    16 px per frame, 328 px or more puts the layer 320 px short of the target. At a respawn the old
    position is set to the target, so the cap does not act.
  - Mode 9 y: no mode code writes it. In stage 4 (and stage 20 area 1, stage 18) the Nightmare Press
    (`PressUpdate`, `0x80046E14`) sets layer 1 y = y0 + `var` − offset each update
    (`docs/enemies/nightmare-press.md`); at the top rest (offset 16) that is y0 + 16 in area 0
    (`var` 32) and y0 in area 1 (`var` 16). In stage 20 area 1 the top rest is offset 32 (`var`
    32), so y0 (`shark-10_alt`: layer 1 y 224 = 224 + 32 − 32) [certain]. After the press deletes itself, y keeps its last value
    (`shark-8`, `shark-9`: 240 = 224 + 16 − 0, from one press update after a respawn [likely]).
  - Mode 5 users. Turtloid (stage 6, stage 21 area 1), layer 1: the acid-rain controller (stage
    logic 11; overlay 09 `0x800ED510`, overlay 30 `0x800ECE00`) sets (0, 0) when the rain starts
    and then y = frame × 256 (frames 0–4, one per 4 updates); the checkpoint records put the layer at
    (320, 0) or (1280, 0) while it is dry (`docs/enemies/rain-effect.md`). Stage 7 at checkpoint ≥ 1:
    `SheldonBgSwitch` (a kind-7 object, `0x800E9E4C`; state 0 `0x800E9E88`, state 1 `0x800E9EF0`) sets layer 1 `+0x42`
    = 320 and layer 2 mode 5, y 448, x = layer 1 x / 4 (toward 0) each update, so layer 2 x ≈ x0 / 8.
    Stage 4 area 1: warp 1 (`0x800EDBB8`) and `AutoScroll` (at checkpoint 1) set layer 1 mode 5;
    Nightmare Pressure moves layer 1 y (776–960); warp 2 (`0x800EA2E0`) puts layer 1 at (5456, 224),
    where it stays. Stage 3: Nightmare Snake moves layer 1 with its body
    (`docs/enemies/nightmare-snake.md`). Stage 8 (overlay 11): stage logic 5 (`TunnelBgAnimator`,
    `0x800FB13C`, a fixed-list record) sets layer 2 mode 5 and layer 2 y = frame × 256 (frames 0–2,
    one per 16 updates): an animated tunnel picture fixed on the screen [certain].
  - Code at `0x800503D8`–`0x80050470` (main program) lowers layer 1 `+0x42` for an object whose
    `+0x02` is 25 or 39 [certain for the code; the user is not known].
  - Intro: layer 1 mode 1 with offsets (0, 560), layer 2 mode 3 with offsets (0, 640), for the
    whole stage.
- **The side-wall test** `0x80030C58` (jump table `0x80010F18`, values 56–63): `obj+0x4A` = 1 (wall
  slide and kick) only for 56 and 58 (and 62, 63 in form 2); 57 and 60 are solid with `+0x4A` = 0;
  59 and 61 return 0 here [certain].
- `CameraClamp` (`0x8002820C`) clamps the player only in x (to min x + 8 … max x + 312); in y it
  sets y = 0 above the map top and starts the fall death 8 px below camera max y + 256 [certain].

## Checkpoints [certain for the code]

- **Markers** (stage logic 6, `CheckpointMarker` `0x80052D30`, states `0x8007A11C`). State 0
  (`0x80052D6C`): when `id & 0x0F` = `game.checkpoint` (`0x800CCEED`), the marker deletes itself.
  State 1 (`0x80052DB8`): off screen, `DeleteObject` (it can spawn again); else, when `mega.x` ≥ x
  (`mega.y` ≥ y with `id` bit 7), `game.checkpoint` = `id & 0x7F` and the marker deletes itself.
- **Respawn records.** `CheckpointStart` (`0x80029B8C`; called from `StageStart` at `0x8001E244`
  and twice from `FrontEndTask`) reads `checkpoint_start_tbl` (`0x80073300`), one word per stage ×
  2 + area, which points to a list of pointers by checkpoint. Each record is 36 bytes:

  | offset | field |
  |---|---|
  | `+0x00`, `+0x02` | player x, y |
  | `+0x04`, `+0x06` | layer 0 camera x, y |
  | `+0x08`…`+0x0E` | layer 1 and layer 2 camera x, y, before the offsets |
  | `+0x10`…`+0x16` | min x, max x, min y, max y (current and target limits) |
  | `+0x18` | facing, to `mega+0x15` (`0x40` = right) |
  | `+0x1A`, `+0x1C` | layer 1 `+0x40`, `+0x42` |
  | `+0x1E`, `+0x20` | layer 2 `+0x40`, `+0x42` |
  | `+0x22` (byte) | `mega.beam_thru` (`mega+0xCE`) |

  The player starts at the record point, not at a stage record. The intro records are in
  `docs/stages/intro.md`.
- **What a restart resets.** `StageStart` (`0x8001E1BC`) calls `RecordsReset` (`0x8002A33C`):
  `flags &= 0x70` and `kind &= 0x0F` for every record of the area, so destroyed objects come back.
  Then `SpawnView` (`0x8002A3E8`) spawns the records in the view. `0x8001D584` also calls
  `RecordsReset`. `talk_seen` is not reset by it.

## Talk zones (stage logic 14) [certain for the code]

`TalkZone` (`0x8005335C`; states `0x8007ACD4`, steps `0x8007ACE0`) is a navigator (Alia) message
zone. The old name `BackgroundSetting` was wrong: it does not change the background.

- The record point is not used. k = `talk_stage_idx` (`0x8007A124`)[stage × 2 + area]. The box is
  `talk_boxes` (`0x8007A154`) + k × 128 + (`id` & 0x0F) × 8: `s16 x0, x1, y0, y1`; the player
  position must be strictly inside it. The seen bits are `talk_seen` (`game+0x9E` + 2k), bit `id &
  0x0F` (`TextSeen` `0x800533F4`, `SetTextSeen` `0x80053440`; boss texts use them too).
- **State 0** (`TalkZoneWait`, `0x80052F64`), while the player HP is not 0:
  1. When the seen bit is set, or `0x80097428` is not 0: state 2 (deleted; the record stays
     "spawned").
  2. When `mega+0xE0` = 2, or `mega+0xE0` = 1 and `id` bit 7: clear `mega.hint` and wait.
  3. **Forced zone** (`id & 0xF0` = 0), player inside: `PlayerEvent(20, mega+0x15)`, `+0x14` = 1,
     set the seen bit, state 1.
  4. **Optional zone** (`id & 0xF0` ≠ 0), player inside, player step ≥ 2, HP not 0 or `0x80`,
     `mega+0xD4` = 0: `mega.hint` (`mega+0x14E`) = `id & 0x0F` (15 for 0), and `PlaySound(0, 42)`
     once per entry (latch `+0x16`). When `pad1` pressed bit `0x100` (Select [likely]) is set:
     `game+0x10` = 2, set the seen bit, state 1. Outside: clear the latch, and clear `mega.hint`
     when it holds this zone.
  5. The HUD (`0x80024528`) draws HUD element 10 (the "!" icon [likely]) while `mega.hint` is not 0.
- **State 1** (`TalkZoneTalk` `0x80053398`). Step 0 (`TalkZoneOpen`, `0x80053274`): a forced zone
  waits for player step 20; then `player_lock` = 1; a forced zone also calls `ClearPlayerShots` and
  sets `mega+0x71 |= 8`; flag `0x08` on itself; stop flags `game+0x11`…`+0x19` = 1 (all classes
  but the player freeze); `TextOpen(id & 0x0F, 255, 0)`. Step 1 (`TalkZoneClose`, `0x8005348C`):
  when `text_open` = 0, clear `game+0x10` (optional zone only) and the stop flags,
  `PlayerEventEnd`, state 2 (`DeleteObjectKeep`).
- An optional zone that the player never uses stays live (no off-screen delete): in `intro-3_boss`
  the record 16 zone is in stage logic slot 0, in state 0, with X at x 6145.
- Stages with b 14 records: intro (k 0, `id` 0 and 129), Yammark (k 1, 129–134), stage 4 (k 4 for
  both areas, so they share the seen bits), Turtloid (k 6, 129, 2, 131–135; box 7 is empty, so `id`
  135 never acts), Sheldon (k 7, 129–134), the alternate areas of members 120 and 121 (k 13–20,
  132) [certain for the box match].

## Text [certain for the code unless marked]

**Banks on disc.** Each text bank is the one entry (type `0x15`, at offset `0x800`) of a
`ROCK_X6.DAT` member. Stages 0–18: member = `text_members` (`0x8006DBD4`, u16)[stage × 2 +
character] (character `game+0x38`: 0 X, 1 Zero; the area does not matter): 0 → 203/204, 1 →
205/206, …, 8 → 219/220, 12 → 221/222, 16 → 223/224, 17 and 18 → 225/226; stages 9–11 and 13–15
have none. Stages 19–22: the stage load copies `text_members_alt` (`0x8006DC30`)[((stage − 19) × 2
+ area) × 2 + character] into the table (members 227–242). The stage load reads the text member
after the palette member (`0x8001417C`, mode 1). At the load, `ArcEntryToSlot` (`0x800154E4`)
writes the address of each entry of a type below `0x10000` to `*arc_entry_slots[type]`
(`0x8006E27C`): type 5 → scratchpad `+0x28` (palettes), `0x15` → `+0x48` (text), `0x1B` → `+0xA0`,
`0x1D` → `+0xA8` (portraits). In all 9 stage save states checked, the bank in RAM is a byte match
to the disc member. Other banks: member 106 (3 messages), members 110 and 111 (stage 15, 22
messages), members 149–161 (541 wide lines with no portraits [guess: the cutscene text]).

**Bank.** u32 offsets from the bank start (message count = first offset / 4). A message is a u16
header, then one u16 per character up to the first word with bit 15 set.

**Header** (to `text_header`, `0x800E33C6`): bit 15 portraits; bit 14 one portrait at x 160 (else
two, at x 80 and 240); bits 0–1 the left (or single) portrait slot; bits 4–5 the right slot. Values
in use: `0x0000`, `0x8001`, `0x8020`, `0xC010`, `0xC011`.

**Character word** (`TextPrint` `0x8002234C`, `TextBox` `0x80021BF4`):

| bits | meaning |
|---|---|
| 0–8 | glyph code; `0x1FF` = no glyph and no sound |
| 9–10 | speaker slot (0–3), read only on the first line of a page |
| 11 | box at the bottom (y + 120); set in all stage banks |
| 12 | wait after this character (typing); without it the next character prints in the same update, so the name line appears at once |
| 13 | end of the page: the arrow, then a wait for a press |
| 14 | line break after this character |
| 15 | end of the message: a wait for a press, then the close |

**Characters** (font page in VRAM x 960–1023, y 256–511, 4 bpp, 16 × 16 cells, code = row × 16 +
column; on disc entry type `0x10001` of members 85–89 and others): A–G `0x00`–`06`, H–N
`0x10`–`16`, O–U `0x20`–`26`, V–Z `0x30`–`34`, space `0x35`, a–g `0x70`–`76`, h–n `0x80`–`86`, o–u
`0x90`–`96`, v–z `0xA0`–`A4`, 0–3 `0xB8`–`BB`, 4–9 `0x98`–`9D`, `&` `0x52`, `!` `0x53`, `+` `0x54`,
`-` `0x55`, `?` `0x5E`, `.` `0x60`/`0x61`, `…` `0x62`, `:` `0x63`, `'` `0x64`, `,` `0x65`, `%`
`0x6C`, `"` `0x6D`. All 40 stage banks decode with no unknown code.

**The box** (`TextBox`, states `text_state+0x0D`, table `0x800100B4`):

- `TextOpen(id, unused, mode)` (`0x80021760`): when a box is open, it closes it and queues the id
  (`+0x10` = id | `0x8000`). Mode 0: save and clear `game+0x24` (boss bar) and `game+0x1F`; back up
  palette slots 21–22 to `0x800E33D8`; `PaletteCopy(21, text_palettes 0x8006E408, 2)`; stream
  pointer and header; the name palette (`TextNamePalette` `0x80022838`); the window (ray type 2,
  `TextWindow` `0x8004B984`: x 16–304, y 22–96, or y 142–216 for the bottom box; none in stages 11
  and 15). Mode not 0: a 60-update wait with no text.
- State 0: wait 50 updates (1 in stage 11), make the portraits, `text_open` = 1. State 1: print.
  State 2: the delay between characters: one character every 5 updates [likely: `TextUpdate` runs
  once per frame]. State 3: wait for a press (`text_open` = 255): any of L2, R2, L1, R1, Triangle,
  Circle, Cross, Square (the low byte of `pad1` pressed) [likely]; a page end clears the glyphs and
  prints the next page; a message end removes the portraits and glyphs, timer 50, `text_open` =
  `0x80`, state 4. State 4: at the end of the timer, restore the palettes, `game+0x24` and
  `game+0x1F`, `text_open` = 0, state 5 (idle).
- A press while typing prints the rest of the page at once (`text_state+0x0F` = 0); a page end sets
  it to 2, so the press that turns the page does not also skip the next page.
- Sound `PlaySound(0, 14)` on every update that prints (not for `0x1FF`, not in stage 11).
- Glyphs: misc type 22 `var` 0 (`TextSprite` `0x80048628`) at x 160, y 0 or 120, font texture page
  31, CLUT slot 21 (the name line slot 22). Each character adds `{flags, code, x / 2, y}` to the list
  at `0x800E3148`. x starts at −128 (relative), +12 for codes below `0x40`, else +11; a line is 16 px.
  A page has 2 or 3 lines (the name line, then 1 or 2 body lines). The page arrow is misc 22 `var` 1
  at x 152, y 81 (or 197), blinking every 32 updates.
- **Portraits** (misc 22 `var` 5) at x 80 / 240 (or 160), y 72. `PortraitCharacter` (`0x80021AF0`,
  once named `StageMusic`) maps a slot to a character id through the overlay row
  `portrait_rows[stage × 8 + area × 4]` (`0x80071CB8`). −1 = the player (Zero 1, or 6 with `game+0x5F`
  bit `0x20`; X by `game+0x5E`: 0 → 0, 1 → 3, 2 → 4, 3 → 5, 4 → 2). In stages ≥ 19 slot 2 follows
  `boss_choice` (0 → 22, 1 → 8 High Max, 2 → 21 [likely for the order]). Every stage row is [player, 7 Alia, the boss, 20]: intro 8 High Max
  and 20 "Hunters", Yammark 9, Shark 12, Scaravich 13, Turtloid 14, Sheldon 15. The name is the
  first line of each page ("Alia:"); its colour is `0x8006E484` + 32 × `name_pal`
  (`0x8006E468`)[character].
- **Live portrait objects** (misc slots, 96 of `0x60` at `0x800CD410`) [certain for the values]:

  | save state | portrait | x | `+0x3C` data | count | `+0x40` texture | `+0x42` CLUT |
  |---|---|---|---|---|---|---|
  | `intro-4_hunters-text` | X | 160 | `0x801B6118` | 6 | `0x0000` | `0x7F80` |
  | `intro-5_alia-text` | Alia | 160 | `0x801B6840` | 6 | `0x0040` | `0x7F80` |
  | `intro-6_high-max-text` | X | 80 | `0x801B6118` | 6 | `0x0000` | `0x7F80` |
  | `intro-6_high-max-text` | High Max | 240 | `0x801B6A60` | 4 | `0x0570` | `0x7FC0` |

  Scratchpad `+0xA0` is `0x801B5A94` and `+0xA8` is `0x801B7E44` in all four intro states. The
  portrait data is not one of the `sprite_bank_base` entries. The portraits have display byte `+3`
  = 2, so they are 8 bpp sprites (below). Each frame is 20 cells (5 × 4), 80 × 64 px with its
  backdrop; X (Falcon) and Alia have 6 frames, High Max 4. Drawn with the 8 bpp rule, all three are
  correct pictures [certain]. Exports: `portrait-x-falcon`, `portrait-alia`, `portrait-high-max`
  (`mmxdemake/sprites.py` mode `DEPTH8`).

**Object draw** (`DrawSprites` `0x80022960`) [certain for the code]. For each object with display
byte `+3` = 1, `DrawObject4` (`0x800232D4`); for `+3` = 2 (misc objects), `DrawObject8`
(`0x800239CC`). Both read the frame's sprite index at `+0x47`, the sprite data at `+0x3C`, the
texture base at `+0x40`, the CLUT at `+0x42`, and the cell mode from the object flags (`+0 & 0x60`,
the `remap` modes of `mmxdemake/sprites.py`). The difference:

| | `DrawObject4` | `DrawObject8` |
|---|---|---|
| cell number | `(tile \| (flags & 3) << 8) + texture` | `((tile \| (flags & 3) << 8) << 1) + texture` |
| colour depth | 4 bpp (draw mode `0xE1000020 \| page`) | 8 bpp (`0xE10000A0 \| page`) |
| u, v in the page | `(t & 15) × 16`, `(t >> 4 & 15) × 16` (texels) | `(t & 0xE) × 8`, `(t >> 4 & 15) × 16` (texels) |
| CLUT | `+0x42 + (flags >> 2 & 3)`; `+0x42` bit 15 → `0x7840` | `+0x42` (256 colours), no cell offset |

The wide-cell flag (`0x20`) and the `0x80` path (a `POLY_FT4`) are in `DrawObject4` only.

For X in forms 1–4, `DrawSprites` also calls `PlayerArmorLayer` (`0x8002619C`) after the player's
`DrawObject4`. It runs `DrawObjectLayer(mega, mega+0xDC)` (`0x80025984`): the `DrawObject4` path
with the pieces of sprite-directory entry `mega+0xDC` and the CLUT `player_layer_cluts[flags >> 2
& 3]` (`0x80071E68`: `0x7843`, `0x7800`, `0x7844`, `0x7801`) [certain for the code]. See
`docs/players/steps-extra.md`, "Sprites by form".

**Palette slots** [certain for the code]. `PaletteUpload` sends the palette buffer to the rectangle
(0, 480, 256, 32): buffer slot k is the CLUT `0x7800 + (k / 16) × 64 + k % 16` (16 CLUTs of 16
colours per VRAM row). So slots 0–15 are `0x7800`–`0x780F`, slot 19 is `0x7843` and slot 20 is
`0x7844`. `PaletteSlotLoad` (`0x8003CF24`) is `(palette, slot, n)`: list palette `palette`
(scratchpad `+0x24`) into buffer slot `slot`.

The decoded intro texts are in `docs/stages/intro.md`, "Texts".

## Boss entry and exit (stage logic 24, 4 and 26) [certain for the code]

- **`BossWarning`** (stage logic 24, `0x8005353C`, states `0x8007ACFC`, steps `0x8007ACE8`; the
  boss makes it at its entry). State 0: in stages 1–8, when the stage's bit of `game+0x60` is set,
  `game.exit_req` = 1 (a rematch skip); else it makes stage logic 4. State 1: sound (0, 19) at update
  6, then every 61 updates. Steps: 0 makes 22 ray objects type 3 (`var` 0–21, pointers at
  `0x800E4498`), one per timer of 10; 1 waits 180 updates; 2 waits until the loader is gone and all
  22 rays are in their step 1; 3 moves each ray to its step 2, one per update; 4 waits 60 updates.
  Then `DeleteObject3`. About 470–495 updates in all, more with a slow CD load [likely]. "WARNING"
  is the probable display [likely].
  Blizzard Wolfang's entry keeps the player walking (event step 29) for 20 updates while the WARNING
  runs, then waits for the warning object to end.
- **`BossPackLoad`** (stage logic 4, `0x80052C84`, states `0x8007A110`; run `0x800528F4`): state 0
  `StreamStop(3)` (the stage music stops); state 1 waits for `stream_state` = 0, then
  `ArcLoadStart(boss_pack_tbl[stage], 4)` (`0x8007A0E0`: 123, 125–132, 0, 0, 0, 133, 0, 0, 0,
  135–138, 138, 138, 138; stage 0 uses 124 when `game+0x26` is not 0; stages ≥ 19 138/139/140 by
  `boss_choice`), unless `boss_pack_loaded` (`0x800CC868`) is set; state 2 `reset_enable` = 1 and
  delete. The pack holds the boss texture, palette and sound. Member 123 holds the D-1000 art and
  Zero's art (texture `0x4E0`); member 124 (High Max) writes over the same VRAM area.
- **Boss music.** `BossMusicRequest` (`0x80040488`, once named `HpBarShow`) sets `game+0x0E` = 1.
  `BossMusicWait` (`0x80040498`, once named `HpBarBusy`): when `stream_state` = 0, `BossMusicStart`
  (`0x80017F6C`) and `game+0x0E` = 0; it returns `game+0x0E`. `BossMusicStart` plays XA track
  `0x800709F4`[stage × 2]: stage 0 track 21, or 23 when `game+0x26` is set; stage 12 area ≠ 0: 27 or
  20 by `game+0x31`; stages ≥ 19: 25 / 23 / 24 by `boss_choice` [certain for the code].
- **`BossExplosion`** (stage logic 26, `0x80053CDC`, states `0x8007AD08`). E = the update it is made:

  | state | time | what |
  |---|---|---|
  | 0 | E | a random angle a (`GetRNG2` `0x8002D30C`); 3 rays type 4 at a, a + 1365, a + 2730 |
  | 1 | E + 1 … E + 9 | each update: the 3 rays again, `DeleteSysObjects(2 \| 0x80)` (`0x8002C0DC`), `SpawnExplosion` (8 in all). At the end: `var` ≠ 1 → `StreamStopNow` (`0x800186A8`), `game+0x1B` = 0; `Rumble(10)` |
  | 2 | 100 updates | every 4 updates `SpawnExplosionNear(1, 96, 96, 2)` (`0x80049F98`); one random ray type 4 in about half of the updates. At the end: ray type 8 |
  | 3 | 10 updates | as state 2. At the end: 16 rays type 6, `WhiteFadeOut(25)` |
  | 4 | about 165 updates | as state 2, and ray type 6 in 1 of 4 updates. At full white: `+0x07` = 1, `game+0x24` = 0, sound (0, 5), `bg_refresh` = 1. Timer 200 (flag bit 7) or 40 |
  | 5 | 201 or 41 updates | the white hold, then `WhiteFadeIn(25)` |
  | 6 | about 165 updates | waits for the fade |
  | 7 | | `game.player_lock` = 1, `DeleteObject3` |

  The white fade adds 25 per update to a level of 0–4095 (drawn as level >> 4, additive).

## Stage start [certain for the code]

1. `CheckpointStart` places the player and the cameras (see "Checkpoints").
2. `ReadyStart` (`0x8001E730`, from `StageStart` and `0x8001D59C`): when `game.ready` = 0, it makes
   stage logic 27 (`ReadyControl`, `0x800541D4`, states `0x8007AD28`); with no free slot it sets
   `game.ready` = 1.
3. `ReadyControl` waits for `fade_dir` = 0, then makes ray type 7 (`ReadyRay`, `0x8004CF68`) `var` 0
   (a banner: x −320 → +320 at 32 px/update, y 112, 20 updates), then 10 × `var` 1, then 8 × `var`
   2 (strip effects), each group after the one before ends. Then `+0x16` = 1. It stays live.
4. At update 10 of the banner, misc type 18 (`ReadyText`, `0x800481D0`) `var` 0 and 1: the two READY
   halves at (160, 120). A palette ramp of 14 steps (`0x80077B44`); when `ReadyControl` `+0x16` = 1:
   sound (0, 10), the halves move apart for 8 updates, wait 30, move back for 8 updates, then fly
   off at 16 px/update and set `game.ready` = 1 (`0x8004847C`).
5. `PlayerInit` waits for `game.ready`, then the beam-in (`docs/players/ladder-hurt-events.md`).

The total time from the fade-in to the beam is not known (the lifetime of the `var` 2 strips was
not read).

## Animations

- An object has an animation table (`+0x30`): a list of pointers, one per animation. Each frame is
  4 bytes: {duration, event flag, next, sprite}. Duration 0 means 256. `next` is the relative index
  of the next frame; 0 = hold the frame for ever.
- `SetAnime(obj, n)` (`0x800179A4`) loads frame 0 of animation `n` into `+0x44`…`+0x47`.
  `AnimeTick` (`0x80017A04`) counts the duration down by 1; at 0 it moves to the next frame and loads
  it. So a frame with duration d is shown for d updates.
- The **event flag** (`+0x45`) of the current frame is how the code finds "fire now" or "animation
  done". The sheets list the flags that the code tests.
- The current sprite is `+0x47`. Some objects test the sprite instead of a flag (Monbando's guard).
- **More than one sprite set.** A boss can change its sprite data by animation. Rainy Turtloid
  (`0x800F0434`, overlay 09) takes animations 0–27 from his bank, 28–32 from the next bank and 33
  from the bank after that (texture `0x540`). High Max's shots (visual type 21) use their own sprite
  data, not the boss's. An export that draws every animation from one sprite set is wrong for these
  animations [certain].
- **An extra sprite source.** `sprite_bank_base` (scratchpad `+0x20`) can have one entry more than
  the banks of the type list: 17 entries for 16 banks in stages 0, 6, 7 and 22; none in stages 1 and
  21. In stages 7 and 22 entry 16 is the laser of the puzzle (250 sprites); in the intro it holds 49
  extra D-1000 sprites (visual type 1). An object that uses it takes the sprite data from the entry
  and the texture and palette from another object (the laser: its parent) [certain for the code of
  stage 7, `0x800EAC78`; the counts observed in 26 states]. `spriteexport` makes a sprite-only
  export for such a source.
- **Flip.** An object with `flip` `0x40` is drawn mirrored about its x (pixel offset `rx` goes to
  `−rx − 1`) [likely: the laser pieces join only so in 3 states]. The frame-buffer area of VRAM
  (x 0–319) is black in the save states, so a drawing cannot be compared with the screen.
- **A type can have two sprite banks.** The stage type list (`0x800733C4`, pointer per stage × 8 +
  area × 4; byte list, `0xFF` ends) can hold a type twice. `GetSpriteBank` returns the first.
  Stages 7 and 22 list main type 30 twice; the Death Guardian (Guardian `var` ≥ 16) takes the
  texture (`bank_tpage[bank + 1] >> 7`, table pointer at `0x80095098`), the palette and the sprite
  data of bank + 1 [certain for the code]. `spriteexport` exports such a second bank (bank + 1,
  same type, texture loaded) with the same animation tables.
- Shield Sheldon (`0x800F5F04(obj, set)`): set 1 = bank + 1, texture `0x540`, for boss animations
  0, 1, the turret animations 2, 35, 36 and the shield animation 3 [certain for the code]. Metal
  Shark Player (`0x800F4144` / `0x800F6E78`, overlay 07): set 1 = bank + 1, texture `0x540`, for
  animations 34–37 (the side-kick silhouettes and the stun).
- `SetAnimeFrame(obj, n, frame)` (`0x800179D0`) starts animation n at a given frame. Some objects use
  frames after the first loop of an animation (Metal Shark Player animations 27 and 33), so the
  kit's tables, which stop at the first loop, can miss frames.

## Damage to an object

The full step order, the bonus list, the shot reactions and the call map are in
`docs/damage.md`. The short rules:

- **Damage table** (`+0x58`): 80 weapon ids, 2 bytes each: {class, damage}. Class 0 = normal damage;
  class 2 = no effect; class `0xFF` = deflected (the shot bounces off with a "tink", bank 0 id 6);
  class 7 = absorbed (the shot ends as on a wall, no sound); classes 8 and 9 also exist in the code
  (`docs/damage.md`) [certain for the code].
- `ContactWeapon` (`0x80031474`) tests the hurt box against the 32 player weapon objects
  (`0x800950A0`). Result: 0 no hit (also when this shot already hit); weapon id + 1 for a hit;
  127 blocked; < 0 killed (`−(weapon id + 1)`) [certain]. With `dmg_tbl_one_hit` it skips every
  shot whose `+0x9C` is 0, so the plain buster shots pass through such objects
  (`docs/damage.md`, "Shots that can hit enemy shots") [certain for the code].
- **Hit tag.** `ApplyWeaponHit` returns 126 when the object's `+0x65` equals the shot's tag
  (`+0x64`), and else stores the tag. `ContactWeapon` clears `+0x65` when no shot touches the hurt
  box. So an object that calls `ContactWeapon` twice in one update with two boxes (the Guardian:
  shield with `dmg_tbl_deflect`, then body with `dmg_tbl_normal`) cannot be hit twice by one shot
  in that update [certain for the code].
- `ApplyWeaponHit` (`0x800315CC`) reads the table, stores the class in `+0x63`, then [certain]:
  - +1 damage for some parts and items, by weapon id: the Buster Plus part (`0x80`) for ids 0, 9,
    18, 19, 20, 45, 46, 58; else the sabre bonus (part `0x400000`, or Zero with armours bit `0x20`)
    for ids 24–31 and 37–41; parts `0x200000` and `0x2000000` for special weapon ids; the timed
    item 2 for every id;
  - on Easy (difficulty 0): damage × 1.25, rounded down (3 stays 3), cap 127;
  - damage 0 and 127 skip the bonuses and the Easy step;
  - then `hp −= damage`; HP ≤ 0 is a kill;
  - class 2 returns 0 (no hit; the shot is not stopped); class `0xFF` calls
    `WeaponHitReaction(shot, −1)` and returns 127; classes 7 and 8 also return 127.
- **The shot side.** `WeaponHitReaction` writes the shot's `+0x98` from `shot_hit_tbl`
  (`0x800737D0`, 5 bytes per id): −1 on a deflect, 1 on a hit, and on a kill only when byte 3 is 0.
  So ids 9, 18, 33, 49 and 58 go on through what they kill [certain].
- **No general invincibility.** A shot cannot hit the same object twice in a row: each weapon has a
  hit tag, stored in the object at `+0x65`. `no_hit` (`+0x7A`) ≠ 0 turns all `ContactWeapon` hits
  off. The weapon box test (`0x8002D868`) gives no hit while the object's `+0x61` is not 0; the
  object's own code sets and counts it (D-1000: 30 frames) [certain for the code].
- **Hit flash:** palette bit 15 (`+0x42`) is set on the hit update and cleared at the next hit test,
  so the object flashes for 1 update. There is no knock-back unless the object's own code makes one.
- `game.kill_count` (`0x800CCF64`) gets +1 per kill through `ContactWeapon`, except for objects
  with `dmg_tbl_minor` or `dmg_tbl_one_hit` [certain for the code].

### Weapon ids

Known meanings. Most are inferred from the part bonus lists and from which ids the bosses accept
[likely unless marked]:

| id | weapon |
|---|---|
| 0 | X-Buster lemon (small shot) [certain] |
| 9 | X-Buster half charge [certain] |
| 18 | X-Buster full charge, forms 0 and 2 [certain for the code] |
| 19 | X-Buster full charge, Falcon (form 1) [certain] |
| 20, 21 | X-Buster full charge, Ultimate (form 4), and its parts [certain for the code] |
| 43 | X-Buster full charge, Blade (form 3) [certain for the code] |
| 45, 46 | the Shadow buster button [certain for the code] |
| 50, 51 | the weapon button with the buster: a pose attack [guess: a sabre] |
| 10–17 | charged special weapons (slot + 9) [certain] |
| 44 | the only id that shows a hidden floor of Shield Sheldon's stage [certain for the id] |
| 1–8 | the special weapon of slot n, uncharged; 10–17 = slot n + 9, charged [certain for the code: `0x8003DF58` sets `mega+0x94` = `mega+0x93`, + 9 for the charged shot] |
| 2, 11, 33 / 4, 13, 35 / 6, 15, 42 / 8, 17, 44 | the stun ids of Rainy Turtloid / Shield Sheldon / Metal Shark Player / Commander Yammark: slot n uncharged, slot n charged, and one id of 33–44 (ids 35 and 42 run the same weapon code, `0x801ED2A8`; which weapon makes them is open). In `shark-8_boss-stun` Metal Shark Player was stunned by id 6 with slot 6 selected; the user names slot 6 Rainy Turtloid's weapon [certain for the ids]. Blizzard Wolfang: 3, 12, 34 (slot 3 stuns him). Illumina (stage 8): 6, 15, 42 cancel its attack and clear its parts (step 10). |
| (slots) | slot n is the weapon of the stage n Maverick [likely]: `mega+0xC9` (owned) and `game+0x60` (beaten) are both `0x21` after the Yammark (stage 1) and Turtloid (stage 6) clears. So slot 4 (Metal Shark Player) stuns Shield Sheldon, slot 6 (Rainy Turtloid) stuns Metal Shark Player, slot 2 stuns Rainy Turtloid, slot 8 stuns Commander Yammark |
| 32 | X Blade Armor Mach Dash hit part (main `0x8003F3C0`, step 74); its bonus is `+0x06` = `mega+0xD9` / 30, so 0–4 [certain for the code] |
| 33 | Zero's icicles: 3 objects from step 63 (`0x801ECB64`) [certain for the code] |
| 34 | Zero's rising fire wave (`TryShoenzan` `0x801EC86C`, step 55) [certain for the code] |
| 47 | X Shadow Armor Giga parts (`weapon_types[47]` = `0x801EDA64` in X's RAM) [certain for the code] |
| 49 | X Falcon Armor Giga part (`0x801EA340`, step 36) [certain for the code] |
| 56 | X Blade Armor up-slash: id 43 with up held (`TryShoot` `0x8003E6A4`; made at `0x801EB098`, step 76) [certain for the code] |
| 58 | Zero's buster shot (`0x801EAD10`, `0x801EADD8`, step 62) [certain for the code] |
| 61, 62 | the Ride Armor punches (5 and 15 damage) [certain for the code] |
| 79 | the laser probe of Shield Sheldon's stage (`0x80044554`); class 2 in the standard tables, so it hurts nothing [certain for the tables read] |
| 18–21, 50, 51 | the ids that stun D-1000 through its core |
| 24–31 | Z-sabre and its combo hits (the sabre bonus ids) |
| 1, 10, 54 | the only ids that kill Nightmare Insect for good, and the only ids that hurt the Nightmare Bug (4 damage) [certain for the ids; the weapons are not known]; also the stun ids of Ground Scaravich (5, 8, 5 damage; `docs/stages/scaravich.md`) |
| 3, 12, 34 | the stun ids of Blizzard Wolfang (5, 8, 8 damage; `docs/enemies/blizzard-wolfang.md`): slot 3 (Magma Blade) uncharged and charged, and id 34, Zero's rising fire wave [certain for the ids] |
| 16, 53 | the stun ids of Infinity Mijinion (2 and 8 damage; `docs/enemies/infinity-mijinion.md`): Guard Shell (slot 7) charged and its shards. Id 7 (Guard Shell uncharged) is class 2 in his table, so his id 7 stun case is never reached [certain for the tables] |
| 7, 16 | the only ids that hurt a Nightmare shadow; id 7 has no effect on every other table read so far [certain for the ids] |
| 64–78 | not met; deflected by every table except `dmg_tbl_one_hit` |

### The damage tables

Each row is 16 ids. `D` = deflected (class `0xFF`); `–` = no effect (class 2); a number = damage
(class 0) [certain, read from the executable].

`dmg_tbl_normal` (`0x800743DC`): the standard enemy table (Junkroid, Hover Gunner, Killer Moth, Worm
Spider, Batton Bone B81, Sea Attacker ST, open Monbando, Wolfloid and its cannon ball).

| ids | damage |
|---|---|
| 0–15 | 3 5 4 6 6 6 5 – 4 7 6 6 6 6 8 6 |
| 16–31 | 8 8 15 15 16 1 32 1 4 6 4 4 4 4 4 4 |
| 32–47 | 3 3 6 8 8 6 4 4 4 4 4 5 8 4 15 8 |
| 48–63 | 4 4 4 4 3 6 4 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then – (id 79) |

Hits to kill with `dmg_tbl_normal`, before the modifiers:

| HP | lemon (3) | half charge (7) | full charge (15) | sabre (4) |
|---|---|---|---|---|
| 3 | 1 | 1 | 1 | 1 |
| 5 | 2 | 1 | 1 | 2 |
| 6 | 2 | 1 | 1 | 2 |
| 8 | 3 | 2 | 1 | 2 |
| 12 | 4 | 2 | 1 | 3 |
| 16 | 6 | 3 | 2 | 4 |
| 32 | 11 | 5 | 3 | 8 |

`dmg_tbl_nightmare_insect` (`0x8007479C`): the same as `dmg_tbl_normal` except id 34 = 8.

`dmg_tbl_illumina` (`0x8007483C`): Illumina, its drones and its wall cannons (stage 8).

| ids | damage |
|---|---|
| 0–15 | 1 2 2 3 3 3 3 – D 1 6 6 6 6 8 6 |
| 16–31 | 8 D 5 5 5 D 8 D 2 2 5 4 4 4 2 2 |
| 32–47 | 2 2 4 4 4 2 2 2 2 2 4 2 D 1 4 8 |
| 48–63 | 2 D 4 4 1 8 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_nightmare_bug` (`0x800746FC`): ids 1, 10, 54 = 4; ids 55, 59, 60, 63 and 64–79 no effect
(–); every other id deflected (D).

`dmg_tbl_reflector` (`0x8007451C`, the laser puzzle parts of Shield Sheldon's stage; a counted hit
turns a turning reflector, see `docs/enemies/laser-reflector-puzzle.md`):

| ids | damage |
|---|---|
| 0–15 | 3 3 3 4 6 5 7 – 3 7 6 6 6 8 8 8 |
| 16–31 | 8 8 15 10 16 1 32 – 4 6 4 4 4 4 4 4 |
| 32–47 | 3 3 6 8 8 6 4 4 4 4 4 5 8 4 7 8 |
| 48–63 | 4 4 2 3 6 4 4 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then – (id 79) |

`dmg_tbl_reflector_wait` (`0x800745BC`, for 33 updates after a reflector turn): – for every id,
except id 0 (3), id 9 (7) and id 18 (15).

`dmg_tbl_sheldon` (`0x80074E7C`, Shield Sheldon while his guard is open,
`docs/enemies/shield-sheldon.md`):

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 5 2 2 – 2 1 2 2 2 8 2 2 |
| 16–31 | D 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 8 2 2 2 2 2 2 2 2 D 1 4 8 |
| 48–63 | 2 1 2 2 1 D 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_sheldon_guard` (`0x80074F1C`): D for every id except id 1 (2), id 54 (1), id 55 (127),
ids 7 and 23 (–).

`dmg_tbl_shark` (`0x8007555C`, Metal Shark Player and his side kicks,
`docs/enemies/metal-shark-player.md`):

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 D 2 5 – 2 1 2 2 2 D 2 8 |
| 16–31 | 2 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 4 2 2 2 2 2 2 6 2 D 1 4 8 |
| 48–63 | 2 1 2 2 1 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_wolfang` (`0x8007519C`, Blizzard Wolfang, `docs/enemies/blizzard-wolfang.md`):

| ids | damage |
|---|---|
| 0–15 | 1 2 D 5 2 2 2 – 2 1 2 D 8 2 2 2 |
| 16–31 | 2 8 4 4 4 D 8 D 1 1 2 2 2 2 2 2 |
| 32–47 | 2 2 8 2 2 2 2 2 2 2 2 2 D 1 4 8 |
| 48–63 | 2 1 2 2 D 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_mijinion` (`0x8007569C`, Infinity Mijinion, his clones and bubbles,
`docs/enemies/infinity-mijinion.md`):

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 2 2 2 – D 1 2 2 2 2 2 2 |
| 16–31 | 2 D 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 2 2 2 2 2 2 2 2 2 D 1 4 8 |
| 48–63 | 2 D 2 2 1 8 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_gate` (`0x8007573C`, Gate and all his orbs, fragments and shots,
`docs/enemies/gate.md`; a weapon hit never costs Gate HP, `docs/damage.md`, section 7):

| ids | damage |
|---|---|
| 0–15 | 1 2 2 2 2 2 2 – 2 1 2 2 2 2 2 2 |
| 16–31 | 2 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 2 2 2 2 2 2 2 2 2 8 1 4 8 |
| 48–63 | 2 1 2 2 1 2 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_nightmare_mother` (`0x800757DC`, the eyes of Nightmare Mother,
`docs/enemies/nightmare-mother.md`):

| ids | damage |
|---|---|
| 0–15 | 2 5 3 D 6 3 D – D 2 3 3 D 8 3 D |
| 16–31 | 3 D 5 5 5 D 8 D 3 3 6 3 3 3 3 3 |
| 32–47 | 3 3 D 8 3 3 3 3 3 3 D 3 D 2 4 8 |
| 48–63 | 3 2 3 3 2 3 4 127 4 8 4 4 15 5 15 4 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_nightmare_mother_shell` (`0x8007587C`): D for every id, except ids 24–29, 31, 50 and 51
(damage 0), id 55 (127) and ids 7, 23, 79 (–). The shells' set-up writes it, but their main state
writes `dmg_tbl_deflect` over it each update, so it has no effect after the first update [certain
for the code].

**An object with no damage table** (`+0x58` = 0): `ApplyWeaponHit` then reads the table at RAM
address 0. Shield Sheldon's plates, image and turrets never set `+0x58`. In the save state most ids
there are class 0 with damage 0, so a touching shot is a hit, the HP (0) stays 0, the result is a
kill, the shot stops and the kill count gets + 1; damage 0 goes the same path as 127
(`0x800318D8`) [certain for the code; the low RAM bytes depend on the BIOS].

`dmg_tbl_minor` (`0x800741FC`): enemy shots and small spawned enemies that can be shot down.

| ids | damage |
|---|---|
| 0–15 | 3 4 4 4 4 4 4 4 4 5 4 4 7 7 7 7 |
| 16–31 | 7 7 15 4 16 2 32 4 4 6 4 4 4 4 4 4 |
| 32–47 | 3 3 6 8 8 4 4 4 4 4 4 4 4 4 4 4 |
| 48–63 | 4 4 4 4 4 4 4 – 1 1 4 127 127 127 127 3 |
| 64–79 | D × 16 |

`dmg_tbl_one_hit` (`0x8007429C`): 127 for all 80 ids. Any hit destroys.

`dmg_tbl_deflect` (`0x8007433C`): D for every id, except ids 7, 23 and 79 (–). Nothing can hurt the
object.

`dmg_tbl_sabre_only` (`0x8007447C`, Meta Wheel F):

| ids | damage |
|---|---|
| 0–15 | D D D D D D D – D D D D D D D D |
| 16–31 | D D D D D D D D 4 6 4 4 4 4 4 4 |
| 32–47 | 3 3 8 8 8 6 4 4 4 4 4 D 8 D 4 6 |
| 48–63 | D D 4 6 D D D 127 1 1 D D D D D D |
| 64–79 | D × 16 |

`dmg_tbl_d1000` (`0x80074B5C`, D-1000 body):

| ids | damage |
|---|---|
| 0–15 | D 2 3 1 1 1 1 – 1 D 1 3 1 1 1 1 |
| 16–31 | 1 1 5 1 4 D 8 1 2 2 3 2 2 2 1 2 |
| 32–47 | 1 1 1 1 1 3 1 1 1 1 1 3 2 1 1 2 |
| 48–63 | 1 1 1 1 1 1 1 – 1 0 1 D D D D 0 |
| 64–79 | D × 16 |

`dmg_tbl_yammark` (`0x80074BFC`, Commander Yammark, his drones and their shots):

| ids | damage |
|---|---|
| 0–15 | 1 D 2 2 2 2 2 – 5 1 D 2 2 2 2 2 |
| 16–31 | 2 8 4 4 4 D 8 D 2 2 5 2 2 2 2 2 |
| 32–47 | 2 2 2 2 2 2 2 2 2 2 2 2 8 1 4 8 |
| 48–63 | 2 1 2 2 1 2 D 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_nightmare_shadow` (`0x8007497C`): – for every id, except id 7 (4) and id 16 (6).

`dmg_tbl_nightmare_snake` (`0x800748DC`, the gems of Nightmare Snake,
`docs/enemies/nightmare-snake.md`):

| ids | damage |
|---|---|
| 0–15 | 1 2 5 D 5 5 4 – 2 1 4 8 D 8 8 8 |
| 16–31 | 2 8 4 4 4 D 8 D 3 3 5 2 2 2 2 2 |
| 32–47 | 2 5 D 5 6 3 2 2 2 2 4 2 8 1 4 8 |
| 48–63 | 4 4 3 3 1 5 1 127 3 8 4 4 15 5 15 3 |
| 64–79 | D × 15, then 127 (id 79) |

`dmg_tbl_capsule` (`0x80075B9C`, Light capsule), `7` = class 7 (damage 0):

| ids | class |
|---|---|
| 0–15 | 7 7 – – – 7 7 – 7 7 7 7 7 7 7 7 |
| 16–31 | 7 7 7 7 7 7 7 – – – – – – – – – |
| 32–47 | – – – – – – – – – – 7 7 – 7 7 7 |
| 48–63 | 7 7 7 7 7 7 7 – – – 7 0 0 0 0 7 |
| 64–79 | D × 16 |

`dmg_tbl_ice_block` (`0x80075CDC`, the ice blocks of Blizzard Wolfang's stage): D for every id
except ids 4 and 13 (3), id 55 (127), ids 7 and 23 (–). Only the Metal Anchor (slot 4) hurts it.
`dmg_tbl_grey_block` (`0x80075E1C`) is the same pattern with 4 damage and id 35.

## Damage to the player

The full step order is in `docs/damage.md`.

- `ContactMega` applies the object's contact damage (`+0x60`) when its attack box touches the player.
  It does nothing while the player's own invincibility (`mega+0x61`) is not 0.
- `PlayerDamageScale` (`0x80030DC0`) [certain]: 0 stays 0 and 127 stays 127 (no scaling). Else
  Easy × 0.75, Normal × 1, Hard and difficulty 3 × 1.25 (rounded toward 0, cap 127); halved for X
  in every armour (forms 1–4), for Zero with `game.armors` bit `0x20` and for the Shock Buffer part
  (bit `0x200`); minimum 1. So a contact damage of 0 is a hit with no HP loss.
- While `mega.ride` < 0, `ContactMega` tests `actor2`'s hurt box: hit class 1 (damage < 5) or 2, no
  `PlayerDamageScale`, ride HP −= damage, `game+0x98` += damage; HP ≤ 0 → `0x80` and `game+0x1C` =
  1 (`docs/players/ride-armor.md`).
- **Squeeze** (`PlayerHitCheck`, `0x800392CC`): `+0x89` = `+0x70` | `+0x71` (terrain flags and
  object flags, set by `PlayerUpdate` at `0x80035008`). When `+0x89` & `0xC` = `0xC` (floor and
  ceiling) or & `0x3` = `0x3` (both sides), HP −= `mega+0xE6`. So one side can be terrain and the
  other an object. It does not test the invulnerability or the difficulty: with `+0xE6` = 127 it is
  an instant death (the Nightmare Press, `docs/enemies/nightmare-press.md`) [certain].

  | base | Easy | Normal | Hard |
  |---|---|---|---|
  | 1 | 1 | 1 | 1 |
  | 3 | 2 | 3 | 3 |
  | 4 | 3 | 4 | 5 |
  | 5 | 3 | 5 | 6 |
  | 6 | 4 | 6 | 7 |
  | 8 | 6 | 8 | 10 |

- A hit does not start a step. `ContactMega` sets HP bit 7 (or HP = `0x80` and the player lock when
  HP reaches 0), the hit class `mega+0x63` and the facing `mega+0xA5`; `PlayerHitCheck` runs the
  reaction (step 17, `PlayerHurt` `0x8003963C`) at the player's next update. Hit class = the
  attacker's `+0x62`; class 0 picks 4 for player forms 1–4, else 1 (damage < 5) or 2 (heavy)
  [certain for the code]. Details: `docs/players/ladder-hurt-events.md`.
- `ContactMega` does nothing while `mega+0xA4` is not 0 (from the hit to the end of the blink) or
  `0x800C4568` is not 0.

## Death, debris and drops

- **Explosions:** `SpawnExplosion` (`0x80049E50`, at the object, random variant), `SpawnExplosion2`
  (`0x80049E70`), `SpawnExplosionRand(obj, type, mx, my)` (`0x80049EBC`, at a random offset within
  ±mx, ±my). Whether an explosion can hurt the player or plays a sound is not read [open].
- **Debris:** `SpawnJunk(n, anims, obj)` (`0x800477E4`) makes n misc objects (type 3), one per
  animation number in the list, with a random facing and a start offset of 0–3 px. Their motion is
  in the engine and was not read.
- **Drops:** `DropItem(obj, table)` (`0x8004E654`) does nothing on Hard (difficulty ≥ 2). Else it
  rolls one random byte against the cumulative weights of the drop table (out of 256) and makes an
  item object (main type 47) with the item kind as its `var` [certain]. Weights at `0x80078860`
  (6 bytes per table), items at `0x800788F0`. There are 24 tables.
- **No drop for weapon id 55.** Mettaur D2 and the Guardian skip `DropItem` when the kill result of
  `ContactWeapon` is −56 (weapon id 55). `dmg_tbl_normal` gives id 55 127 damage. Which weapon id
  55 is, is not known [certain for the code].
- **Nightmare Souls** are not items and not drop rolls. A killed Nightmare Virus (and an infected
  Rescue Reploid) becomes a soul object that adds to `game.soul`; one soul per record, kept in
  `game.soul_done` (see `docs/enemies/nightmare-virus.md`).

| table | none | `0x20` | `0x21` | `0x23` | `0x24` | `0x26` | user |
|---|---|---|---|---|---|---|---|
| 0–5 | | | | | | | 255 for one slot: a fixed item |
| 6 | 64 | 160 | 32 | 0 | 0 | 0 | |
| 7 | 192 | 48 | 16 | 0 | 0 | 0 | |
| 8 | 192 | 32 | 32 | 0 | 0 | 0 | |
| 9 | 192 | 16 | 48 | 0 | 0 | 0 | |
| 10 | 64 | 32 | 160 | 0 | 0 | 0 | |
| 11 | 160 | 0 | 0 | 64 | 32 | 0 | |
| 12 | 192 | 0 | 0 | 48 | 16 | 0 | |
| 13 | 128 | 0 | 0 | 64 | 64 | 0 | |
| 14 | 192 | 0 | 0 | 16 | 48 | 0 | Junkroid (second roll) |
| 15 | 128 | 0 | 0 | 48 | 80 | 0 | |
| 16 | 128 | 56 | 36 | 24 | 8 | 4 | Caterpirade |
| 17 | 192 | 28 | 10 | 6 | 16 | 4 | Wolfloid |
| 18 | 192 | 30 | 7 | 16 | 10 | 1 | Hover Gunner, Killer Moth, Worm Spider, Mettaur D2, Nightmare Bug |
| 19 | 128 | 8 | 52 | 40 | 24 | 4 | Sea Attacker ST |
| 20 | 128 | 40 | 56 | 24 | 8 | 0 | Meta Wheel F, Batton Bone B81, Guardian |
| 21 | 192 | 10 | 32 | 6 | 16 | 0 | Junkroid (first roll), Monbando, Nightmare Insect |
| 22 | 192 | 16 | 10 | 32 | 6 | 0 | |
| 23 | 128 | 24 | 40 | 56 | 8 | 0 | |

Item kinds [guess]: `0x20` / `0x21` small / large life energy; `0x23` / `0x24` small / large weapon
energy; `0x26` a rare item (maybe an extra life). Not checked in play.

## Shots and attached objects

- `GetSysObject` (`0x8002C3F8`) returns a free stage-logic slot (`0x8009B7B0`). The boss engine
  objects 24 (intro) and 26 (death explosion) are stage-logic types [certain for the class].

- A shot or overlay made by an enemy usually copies from its parent: the texture page, the palette
  **without** the hit-flash bit, the animation table, the sprite data, the layer, `flip` and the
  position. It keeps a pointer to the parent: visual objects at `+0x7C`, misc objects at `+0x50`.
  The sheets say "copies the parent" for this.
- A shot usually ends in one of these ways, which the sheets list: killed by a weapon (explosion);
  touches the player; touches terrain (`collide_flags` set, or `CornerCheck`, `0x8002EE10`, finds a
  corner of its terrain box in a solid tile); more than 32 px off screen (no effect). Visible within
  16 px.
- `SpawnHitEffect` (`0x80049D90`): an effect object (flags `0x21`, type 3, var 1) at the object,
  mirrored when `speed_x` > 0. Probably the small burst of an enemy shot on a wall [likely].

## Sounds

`PlaySound(bank, id, obj)` (`0x80016C48`). Bank 0 holds common sounds (id 6 = deflect "tink").
Bank 2 is the stage's enemy bank [likely]: the same id can be a different sound in another stage.

## Screen effects

- `Quake(a, b, c)` (`0x80029478`) and `Quake2` (`0x8002943C`): screen shake; the arguments go to the
  layer-0 camera work (`0x800971F8`). Their meaning (strength, time) is a guess.
- `Rumble(n)` (`0x80012D9C`): controller vibration [likely].
- `WhiteFadeOut(speed)` (`0x80013AA4`) and `WhiteFadeIn(speed)` (`0x80013A48`): the white fade, as
  `FadeOut`/`FadeIn` with white. `white_fade_dir` (`0x80097425`): −1 to white, +1 back, 0 done. The
  task `WhiteFadeTask` (`0x80013788`) draws a GPU rectangle `0x62` (monochrome, semi-transparent)
  over the view with draw mode `0xE1000025` (additive), r = g = b = `white_fade_level`
  (`0x80090C8A`) >> 4 [certain for the command words].

## Open questions

1. (answered from the code: one update per `VSync(0)`; see "Main loop". Not measured on a running
   game) The real frame rate (VSyncs per game update).
2. The meaning of most weapon ids (only 0, 9, 18–20 and 24–31 are likely).
3. The item kinds `0x20`–`0x26`.
4. (answered) Step 0 is the beam-in; the hit reaction is step 17, started by `PlayerHurt`
   (`0x8003963C`).
5. The draw priority order: whether a higher number draws in front.
6. Whether explosions hurt the player.
7. (answered: the stage pick writes `game.stage_effect`, the stage start copies it) What each
   Nightmare Effect value does in the stages not read yet.
8. (answered: `RayLoop`, table scratchpad `+0x98`; the `+0x88` table is the effect class) The
   update of the ray objects.
9. (answered: absorb, the shot ends as on a wall; `docs/damage.md`) Damage class 7.
10. (answered: `actor2` is a fixed second-body slot; stage 4 uses it for the Ride Armor) What
    `actor2` types 2–4 are.
11. Which weapon is id 55 (no drop for Mettaur D2 and the Guardian; 127 damage in
    `dmg_tbl_normal`).
12. (answered: an options-menu byte, `0x8006D5F9`) What sets `game+0x67`. Still open: the label
    of the option on the screen.
13. (answered: 26–31, 35, 46, 51, 57, 61, 62) Which weapon ids start the hit stop.
14. (answered: palette effects; the CD upload queue) What `0x8002B29C` does with `mega+0xE4`, and
    what the entries of `RunUploadQueue` are.
15. (answered: none; state 4 is a leftover) The writer of task state 4.
16. The five modes of `ArcLoadStart` (switch `0x80010024`). (Answered: stage 14 area 0 loads
    overlay 18, the title, the menu and the options; see "Front end".)
