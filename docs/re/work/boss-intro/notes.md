# Boss intro screen (game modes 5, 6, 7; overlay 17)

State: `wolfang-0_boss-intro.state.gz`. User: "Screen before loading into Blizzard Wolf's level where
the 'boss intro' plays (screen with boss anime and name)".

## Result in short

- The boss intro is a small "stage": stage 13 area 1, overlay 17, game mode 6. It draws the picture
  with the normal BG layer code (`DrawStage`). It is not a sprite or a linear image. [certain]
- Each Maverick has one `ROCK_X6.DAT` member with the picture (141 + boss − 1) and one palette
  member (52 + boss − 1). The layout and the scroll targets are in overlay 17. So everything comes
  from the disc; a save state is not necessary. [certain]
- All 8 pictures are decoded and checked by eye: `images/all-bosses-final-views.png`.
- The state is good. It is in sub-mode 6 of 15 (after the fade-in; the body is in place; the
  close-up slides in; the name is not on the screen yet). No better state is needed.

## The state [certain unless marked]

| item | value |
|---|---|
| `game+0x00` (game mode) | 6 |
| `game+0x01` (sub-mode) | 6 |
| `game+0x04` (sub-mode timer) | 0 |
| `game+0x0C`, `+0x0D` (stage, area) | 13, 1 (not a leftover: game mode 5 writes them) |
| `game+0x26`, `+0x27` | 2, 0: the stage and area to load after the intro (game mode 5 copies `+0x0C`, `+0x0D`) |
| `game+0x36` | 2: the picked Maverick, used by the stage load to pick the intro members |
| `game+0x90` (stage time) | 38169: not cleared; `StageStartReset` clears it later |
| overlay at `0x800E9860` | `rock_bin/17.bin` (4717 bytes; 1 word differs from the disc, at file offset `0x126C`, a variable) |
| player overlay at `0x801EA000` | `01.bin` (Zero's overlay; no live player object) |
| stage member | 142 (`stage_members[13 × 2 + 1]`) |
| palette member | 53 (`palette_members_x[27]` and `palette_members_zero[27]`) |
| texture | member 142 entry `0x10014` is in VRAM at (512, 0)–(1023, 255), page by page (exact match) |
| CLUTs | VRAM rows 480–488 = member 53 entry 5 rows 0–8 (exact match); the picture uses rows 486, 487, 488 |
| display | `display_mode_d6a` = 1, `screen_width` (`0x8009B79C`) = 512; DISPENV 512 × 240 |
| layer cameras (`0x800971F8` + layer × `0x54`, x at `+0x0A`) | layer 0 x 0, layer 1 x 656 (`0x0290773F`), layer 2 x 230 (`0x00E66678`, going down) |
| `stream_state` (`0x800CD40C`) | 2 (the XA stream plays) |
| `fade_dir` | 0 |

- The frame buffer in VRAM (x 0–511) is black. It is also black in the stage-play states
  (`wolfang-1_x-sabre`), so this is the GPU plugin of the emulator, not the game. [certain]
- The preview picture of the state is black too (as for `intro-1_start`). The cause is not known.
  [open]

## Game modes around the intro [certain for the code]

`game_modes` (`0x8007112C`), the entries that this screen uses:

| mode | code | what it does |
|---|---|---|
| 5 | `0x8001E7C0` (`BossIntroStart`, proposed) | game mode 6; `game+0x26` = stage, `+0x27` = area (the picked stage); stage 13 area 1; `game+0x36` = `+0x26`; `0x80013DE8` (the stage data load); `SetDisplayMode(1)` (512 × 240) |
| 6 | `0x800E9A88` (overlay 17, `BossIntro`) | copies the layer 0 camera position (`0x800971F8` `+0x08`, `+0x0C`) to `+0x14`, `+0x18`; `boss_intro_steps[game+0x01](game)`; then `StageLogicLoop`, `EffectLoop`, `MiscLoop`, `RayLoop`, `PaletteFade`, `DrawStage` |
| 7 | `0x8001E880` (`StageLoadStart`, proposed) | `SetDisplayMode(0)` (320 × 240); `0x8002B838`; `StageLoadPlayer`; game mode 8 |

Other entries: 0 `0x8001E088` (new game), 2 `0x8001E908` (sets mode 3), 3 `0x8001E8CC` (stage 13
area 0, stage select, `SetDisplayMode(1)`, mode 4), 8 `0x8001E914`, 9 `0x8001E1BC`, 10
`0x8001E9EC`, 11 `0x8001E81C` (report, `SetDisplayMode(1)`), 13 `0x8001FB3C`, 14 and 18 null, 15
`0x8001EC68`, 16 `0x8001ED44`, 19 `0x8001ED88`, 21 `0x8001EDDC`. Modes 1, 4, 6, 12, 17, 20 point
into the overlay area (`0x800E9xxx`–`0x800EExxx`): their code is in the overlay of that screen.

So the stage select (overlay 16) and the report (overlay 20) also run at 512 × 240 [certain for
the calls of `SetDisplayMode(1)`]. Who sets game mode 5 was not read [likely: the stage select
after the pick].

### `SetDisplayMode(mode)` (`0x80012890`) [certain for the code]

- Returns at once when `mode` = `display_mode_d6a` (`0x80090D6A`).
- Writes `display_mode_d6a` = `mode`, `screen_width` = 320 (mode 0) or 512 (other), then makes
  both draw buffers with that width (`0x80068CD4`, `0x80068D94`: likely `SetDefDrawEnv`,
  `SetDefDispEnv`).
- The layer draw set-up (`LayerSetupDraw` area, `0x800264DC`–`0x80026734`) has a copy for the
  texture at VRAM x 320 and one for x 512. This is why the intro texture is at x 512.

## Overlay 17 [certain for the code]

Loaded at `0x800E9860` (`*0x80010000`; `stage_overlays[13 × 2 + 1]` = 17).

- `boss_intro_steps` (`0x800EA528`, 15 entries) by `game+0x01`. `game+0x04` is the timer. Boss
  `b` = `game+0x26` (1–8).

| step | code | what it does |
|---|---|---|
| 0 | `0x800E9B0C` | `0x8002B838`, `0x8002C2B8`, `CheckpointStart`, `LayerInit`, `0x80025F34`; `boss_intro_rays_on` (`0x800EAACC`) = 1 |
| 1 | `0x800E9B68` | `FadeIn(8)` |
| 2 | `0x800E9B9C` | waits for `fade_dir` = 0; `StreamPlay(3)` (the intro jingle) |
| 3 | `0x800E9BDC` | waits for `stream_state` = 2; ray type 12 `var` 0 |
| 4 | `0x800E986C` | layer 1 (the body) x += target / 120 per update, until x ≥ `boss_intro_body_x[b − 1]` (`0x800EA508`); then timer 30. Layer 1 `+0x4C` = 1 |
| 5 | `0x800E9C28` | waits 30; 24 rays type 12, `var` `0x80` + i (i = 0–23) |
| 6 | `0x800E9CB0` | layer 2 (the close-up) x −= 5.12 px per update (`0xFFFAE148`) until x ≤ 0; then x = 0, timer 10. Layer 2 `+0x4C` = 1. **The state is here.** |
| 7 | `0x800E9D04` | waits 10 |
| 8 | `0x800E9900` | layer 0 (the name) x += target / 30 per update, until x ≥ `boss_intro_name_x[b − 1]` (`0x800EA518`) |
| 9 | `0x800E9D38` | `ResetLayerCache(0)`; palette channel 0 in add mode with r, g, b = 31 (`fade_r` 31, `fade_g` `0x3E0`, `fade_b` `0x7C00`); `bg_refresh` \|= 1; `boss_intro_rays_on` = 0; timer 4 [likely: a 4-update white flash when the name stops] |
| 10 | `0x800E9DC8` | waits 4; `PaletteFxClear(0)` (`0x8002B2F8`); `boss_intro_rays_on` = 1; timer 30 |
| 11 | `0x800E9E2C` | waits 30; timer 180 |
| 12 | `0x800E9E88` | waits 180 (3 s: the full picture with the name) |
| 13 | `0x800E9EBC` | `FadeOut(8)` |
| 14 | `0x800E9EF0` | waits for `stream_state` = 0 (the jingle ended) and `fade_dir` = 0; `0x8001D218` (stops the stream and the sounds); stage = `game+0x26`, area = `+0x27`; game mode 7; sub-mode 0; clears `game+0x26`…`+0x35` |

- Steps 4 and 8 compute the step as `(target << 16) × 8 / 15 >> 6` = target / 120 (layer 1) and
  `>> 4` = target / 30 (layer 0): the body slides in for 120 updates (2 s), the name for 30
  updates.
- The start x of layer 2 comes from `LayerInit`; it was not read. So the length of step 6 is not
  known. [open]
- No `PlaySound` call: the screen has only the XA track. [certain]

### The layout (`boss_intro_layout`, `0x800EA2C8`, file offset `0xA68`) [certain]

24 × 8 screens, only row 0 is used. The same layout for all bosses; the member gives the screens.

| layer | screens at x 0, 1, 2, 3 | content | final camera x |
|---|---|---|---|
| 0 (front) | –, –, 1, 2 | the boss name (a graphic, not text) | `boss_intro_name_x[b − 1]` |
| 1 | –, –, 3, 4 | the full body | `boss_intro_body_x[b − 1]` |
| 2 (back) | 5, 6, 7, – | the close-up of the face (608 px wide) | 0 |

`boss_intro_body_x` (`0x800EA508`): 672, 656, 640, 624, 560, 656, 720, 624.
`boss_intro_name_x` (`0x800EA518`): 480, 512, 528, 496, 512, 528, 528, 528.

The view is 512 × 240. The body starts at x about 530 in the layout, so its left edge goes past
the left side of the screen (seen in the state: the drawn layer 1 tiles start at screen x 0).

### Ray type 12 (`0x800EA23C`, overlay 17) [likely for what it draws]

Ray class table of stage 13 area 1 = `0x800EAA80` (scratchpad `+0x98`, from `0x8006E0BC`); entry
12 = `0x800EA23C` [certain]. States by `+0x04`: init `0x800E9994`, run `0x800E9F7C` (by `+0x05`
and `+0x06`; `+0x03` = `boss_intro_rays_on`), delete `0x800E9FCC`.

- `var` 0: colour `0x7FFF`, x 96, half width 8. It grows down 2 px per update to y 240, waits for
  step 5, then gets wider (+0.32 px per update) to half width 40. [likely: a white vertical bar
  behind the body]
- `var` `0x80` + i: colour `0x2208`, full width (x 256 ± 256), 2 px high, starts at y 240 + 6i and
  goes up 1.48 px per update to y 164 + 2 × (i adjusted). [likely: the horizontal lines of the
  band under the name]
- These are flat polygons, not texture data. Our images do not show them.

## The data on the disc [certain]

| boss `b` (stage) | Maverick | picture member (`0x8006DA5C`) | palette member (`0x8006DB40`) |
|---|---|---|---|
| 1 | Commander Yammark | 141 | 52 |
| 2 | Blizzard Wolfang | 142 | 53 |
| 3 | Blaze Heatnix | 143 | 54 |
| 4 | Metal Shark Player | 144 | 55 |
| 5 | Ground Scaravich | 145 | 56 |
| 6 | Rainy Turtloid | 146 | 57 |
| 7 | Shield Sheldon | 147 | 58 |
| 8 | Infinity Mijinion | 148 | 59 |

The names are read from the pictures. The stage load (`0x80013DE8`, stage 13 with area ≠ 0) writes
`boss_intro_members[game+0x36 − 1]` into `stage_members[27]` (`0x8006DA32`), and
`boss_intro_palette_members[game+0x36 − 1]` into `palette_members_x[27]` (`0x8006DABE`) and
`palette_members_zero[27]` (`0x8006DB1A`).

Picture member (507 904 bytes, 6 entries):

| entry | size | content |
|---|---|---|
| `0x4` | 3144 | the same in all 8; in RAM at `0x80103000` [likely: sprite data of the common sprites] |
| `0x20000` | 228 080 | the same in all 8; not found as is in RAM or VRAM [guess: packed sprite textures] |
| `0x10014` | 262 144 | the picture texture: 4 pages of 8 bpp (128 × 256 px each), page by page, to VRAM (512, 0) |
| `0x0` | 4096 | 8 screens of 16 × 16 cells (screen 0 is empty) |
| `0x1` | 3156–3900 | tile info (789–975 tiles) |
| `0xB` | 256 | the words 0, 1, 2, … 63 (the same in all 8; meaning open) |

- All tiles are 8 bpp (texture pages 8–11). They use palettes 0, 16 and 32 of the BG CLUT base
  (0, 486): three 256-colour CLUTs on VRAM rows 486, 487, 488. [certain]
- Palette member: one entry of type 5 (15 360 bytes = 30 rows of 256 colours, row n → VRAM row
  480 + n). Rows 6–8 are the picture palettes. Colour `0x0000` is transparent.
- The 8 bpp decode of `texture.raw_tile` is correct for these members (clean pictures).

## Outputs

- `images/boss<b>-m<member>-layer0-name.png`, `-layer1-body.png`, `-layer2-closeup.png`: each layer,
  cropped to its used part, transparent background.
- `images/boss<b>-m<member>-final-view.png`: the 512 × 240 screen at the end (after step 8), layers
  2, 1, 0 at their final camera x, black background. The rays are not drawn.
- `images/all-bosses-final-views.png`: the 8 final views.
- `scripts/render_intro.py <out> [boss ...]`: draws them from the disc files only.
  `scripts/probe_member.py <member>`: tile pages and palettes, and the palette rule from the save
  state. `scripts/disasm.py ov17` or `<start> <end>`: disassembly with the helper names.

## Can the tools do it?

- `mmxdemake.render --palette N --pixels M`: no. It draws one linear image from a member pair. The
  intro picture is a tile map (screens + tile info) over a texture block.
- `mmxdemake.learn` / `stages`: no. `learn` stops with "3 stage members have their tile info in RAM
  ([100, 106, 142]) and 0 fit the layout": the intro has no collision, so `check_layout` rejects it.
  `stages` would also draw only one boss per save state.
- `mmxdemake.tiles`: draws stage members in greyscale; members 141–148 are not in its list.
- A small tool change is enough (see `doc-changes.md`, "Proposed tool"): a module that reads
  overlay 17 (layout, the two x tables), members 141–148 and 52–59, and writes the three layers
  and the final view per boss. `scripts/render_intro.py` is a working draft of it.

## Open

- The start x of layer 2 and the time of step 6.
- What sets game mode 5 (likely overlay 16 after the stage pick), and whether a second visit
  (stages 19–22) also shows the intro.
- Entries `0x20000` and `0xB` of the picture members.
- Why the preview picture of the state is black.
