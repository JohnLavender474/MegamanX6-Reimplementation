# Doc changes: boss intro (game modes 5–7, overlay 17)

Source: `notes.md` in this folder. Check before you apply: the addresses in the tables of
`notes.md` come from the disassembly of `rock_bin/17.bin` at `0x800E9860` and of the main program,
and the member numbers from `0x8006DA5C` and `0x8006DB40` in the executable.

## 1. `docs/engine.md`: new section after "Stage play (game mode 10)"

Insert before the line `### The game frame (`GameFrame`, `0x80020C40`) [certain for the call order]`:

````markdown
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
  not necessary.
````

## 2. `docs/engine.md`, "Game state" table

Replace the row of `+0x0C`, `+0x0D`:

```markdown
| `+0x0C`, `+0x0D` | `0x800CCEDC` | stage, area |
```

with:

```markdown
| `+0x0C`, `+0x0D` | `0x800CCEDC` | stage, area. Stage 13 area 0 is the stage select, stage 13 area 1 the boss intro, stage 15 area 0 the report |
```

Add a row after the `+0x2E` row:

```markdown
| `+0x36` | `0x800CCF06` | the picked Maverick stage (1–8), written by game mode 5; the stage load uses it to pick the boss intro members (see "Boss intro") [certain for the code] |
```

Note: `game+0x26` and `+0x27` are the per-stage flags in stage play. In game modes 5 and 6 they
hold the stage and area to load after the intro. The table row of `+0x26` can get one more
sentence:

```markdown
In the boss intro (game modes 5, 6): the stage to load after the intro; `+0x27` the area.
```

## 3. `docs/sound.md`

In "Which song plays where", replace:

```markdown
  overlay 16 (the stage select) track 1; overlay 17 track 3; overlay 20 (the stage result) track 6;
```

with:

```markdown
  overlay 16 (the stage select) track 1; overlay 17 (the boss intro, game mode 6) track 3, and the
  intro ends when it stops; overlay 20 (the stage result) track 6;
```

and in the table replace `| 3 | `c01-t02` | 9.4 s | no | stage 13; overlay 17 |` with:

```markdown
| 3 | `c01-t02` | 9.4 s | no | stage 13 area 1; the boss intro (overlay 17) |
```

## 4. `docs/TODO.md`

Add (open items):

```markdown
- [ ] Boss intro: the start x of BG layer 2 (from `LayerInit`) and so the time of sub-mode 6; what
  sets game mode 5 (likely overlay 16 after the pick); whether stages 19–22 show the intro; entries
  `0x20000` and `0xB` of members 141–148.
- [ ] Tool: `mmxdemake.bossintro` (see below), or a member mode in `tiles` with colour.
```

## 5. `README.md`: proposed tool

When the tool exists, add after the `mmxdemake.render` part:

````markdown
Draw the boss intro pictures (the Maverick picture and name before a stage):

```
python3 -m mmxdemake.bossintro \
  --extracted ~/mmx-data/x6/extracted/retail \
  --out ~/mmx-data/x6/bossintro
```

For each Maverick `b` (1–8), the tool reads the layout and the two camera tables of overlay 17,
member 140 + `b` (screens, tile info, texture entry `0x10014`) and palette member 51 + `b`, and
writes `b<b>-<name>/layer0-name.png`, `layer1-body.png`, `layer2-closeup.png` (transparent,
cropped) and `final.png` (the 512 × 240 screen at the end of the intro, without the ray shapes).
No save state is needed.
````

What the tool needs (`scripts/render_intro.py` is a working draft):

- Read the layout from `rock_bin/17.bin` offset `0xA68` (3 × 24 × 8 bytes) instead of a fixed
  list, and the tables at offsets `0xCA8` (`0x800EA508`) and `0xCB8` (`0x800EA518`).
- Read the member tables from the executable (`0x8006DA5C`, `0x8006DB40`) instead of 140 + `b`
  and 51 + `b`.
- Reuse `stagemap.decode_screens`, `decode_tile_info`, `texture.raw_tile` (the 8 bpp path is
  correct for these members) and `psxcolor`. The CLUT of a tile is palette entry row
  `6 + palette / 16` (base (0, 486), palette member row n = VRAM row 480 + n), 256 colours, colour
  `0x0000` transparent.
- A test: member 142 + palette member 53 must match the VRAM of `wolfang-0_boss-intro` (texture
  at (512, 0) page by page, CLUT rows 486–488).
