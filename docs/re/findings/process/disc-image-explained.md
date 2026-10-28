# What is inside the X6 disc image

This note explains the two files in `~/mmx-data/disc/` from the outside in. Each
layer is a box inside the box before it. The extractor (sample task A) opens
the boxes in the same order.

```
CUE  ──points to──▶  BIN (raw copy of the whole CD)
                      └─ 255,096 sectors of 2352 B
                          └─ ISO9660 file system (the "folders")
                              └─ 9 files
                                  ├─ SLUS_013.95   the program
                                  ├─ ROCK_X6.BIN   59 code pieces
                                  ├─ ROCK_X6.DAT   243 data pieces (graphics, stages, ...)
                                  └─ XA / STR      music and video
```

## 1. The CUE file: the label on the box

The CUE is a short text file. It tells a program how to read the BIN. An editor
shows 4 lines: 3 lines of content, and an empty last line after the final line
break.

```
FILE "Mega Man X6 (USA) (v1.1).bin" BINARY
  TRACK 01 MODE2/2352
    INDEX 01 00:00:00
```

| Line | Meaning |
|---|---|
| `FILE "….bin" BINARY` | The disc data is in this file, next to the CUE. We changed this name when we renamed the BIN. |
| `TRACK 01 MODE2/2352` | There is 1 track. It is a data track in CD "Mode 2". Each sector in the BIN is 2352 bytes. |
| `INDEX 01 00:00:00` | The track starts at the very start of the BIN (minute 0, second 0, frame 0). |

A music CD has many `TRACK` lines, one for each song. X6 has only one track,
because its music is stored as files inside the data track (see `XA` below).

## 2. The BIN file: a photocopy of the disc

A CD is one long spiral of small blocks called **sectors**. The BIN is every
sector, copied in order, with nothing added or removed.

- Sector size: 2352 bytes.
- Sector count: 599,985,792 B ÷ 2352 = **255,096 sectors**.
- A CD reads 75 sectors per second, so 255,096 sectors = 56 min 41 s 21 frames.
  This is the `56:41:21` length on the Redump page.

This is why the BIN looks like noise in a text editor. It is a raw disc copy,
not a document.

## 3. One sector, opened up

Every sector starts with the same 12-byte **sync** pattern (`00`, ten `FF`,
`00`). The drive uses it to find where a sector starts. After that, the layout
depends on the sector's **form**:

```
Form 1 (programs, data: must be exact)
| sync 12 | header 4 | subheader 8 | user data 2048 | error check 280 |

Form 2 (audio, video: speed over safety)
| sync 12 | header 4 | subheader 8 | user data 2324 |  check 4 |
```

- **Header (4 B):** the sector's address as minutes, seconds, frames, then the
  mode (`02`).
- **Subheader (8 B):** what kind of sector this is. Byte 2 is the **submode**.
  Its bit `0x20` means Form 2; `0x08` means data; `0x04` means audio; `0x40`
  means real-time (stream).
- **Form 1** keeps 280 B of error-correction, so data reads back exactly. Only
  2048 B of each sector carry file data.
- **Form 2** keeps only a 4-byte check and uses the space for more data
  (2324 B). A small error in audio or video is acceptable.

### Seen in our BIN

```
$ xxd -s 0 -l 32 "Mega Man X6 (USA) (v1.1).bin"
00000000: 00ff ffff ffff ffff ffff ff00 0002 0002  ................
00000010: 0000 0800 0000 0800 0000 0000 0000 0000  ................
```

- `00ff…ff00`: sync.
- `00 02 00 02`: header. Address 00:02:00, mode 2.
- `00 00 08 00` (twice): subheader. Submode `0x08` = data, Form 1.

### Two addresses for one sector

- **LBA** (logical block address) is the plain sector number: 0, 1, 2, …
  The byte offset in the BIN is `LBA × 2352`.
- **MSF** (minute:second:frame) is the address in the header. It is 2 seconds
  (150 sectors) ahead of the LBA, because a CD has a 2-second lead-in before
  sector 0. So LBA 0 = `00:02:00`.

Example: `XA/BGM.XA` starts at LBA 25,217. Its header says `00:05:38:17`.
(5 × 60 + 38) × 75 + 17 − 150 = 25,217.

## 4. The file system: the folders

On top of the sectors is an **ISO9660** file system, the same standard that
data CDs on a PC use. Its main record (the Primary Volume Descriptor) is
always at LBA 16:

```
$ xxd -s $((16*2352+24)) -l 16 "Mega Man X6 (USA) (v1.1).bin"
00009318: 0143 4430 3031 0100 504c 4159 5354 4154  .CD001..PLAYSTAT
```

`CD001` is the ISO9660 signature. `PLAYSTATION` is the system name. This record
points to the root folder. Each folder entry gives a file's name, first LBA and
size. PlayStation discs add an "XA" attribute to each entry that says whether
the file is Form 1 or Form 2.

## 5. The 9 files

| File | LBA | Size | Form | What it is |
|---|---|---|---|---|
| `SYSTEM.CNF` | 195,657 | 68 B | 1 | Boot settings. Tells the console which program to start. |
| `SLUS_013.95` | 210,930 | 522,240 B | 1 | The main program (the "exe"). The name is the game's serial. |
| `ROCK_X6.BIN` | 211,185 | 1.6 MB | 1 | 59 extra code pieces ("overlays"). The game loads one when it needs it, because the PS1 has only 2 MB of RAM. |
| `ROCK_X6.DAT` | 211,998 | 50.9 MB | 1 | 243 data pieces. Graphics, palettes, stage data and more are probably here. |
| `ZNULL.DAT` | 236,858 | 37 MB | 1 | Not known yet. |
| `XA/BGM.XA` | 25,217 | 349 MB | 2 | Background music, as CD-XA audio streams. |
| `XA/DEMO.XA` | 961 | 49.7 MB | 2 | Probably voice and cutscene audio (from the name; not checked). |
| `STR/CAPLOGO.STR` | 25 | 1.9 MB | 2 | Capcom logo video. |
| `STR/X6OP.STR` | 195,658 | 31.3 MB | 2 | Opening movie. |

Music and video take about 430 MB of the 600 MB disc. The data we need for
stages (`ROCK_X6.DAT`) is about 51 MB.

### A trap we found

At LBA 25, the first sector of `STR/CAPLOGO.STR` has submode `0x48` (data +
real-time). The Form 2 bit (`0x20`) is not set. `XA/BGM.XA` has submode `0x64`
(audio + Form 2 + real-time). So the first sector of a video file does not
always say "Form 2". The folder entry's XA attribute is the reliable source.
Our first probe trusted the sector and got the wrong hash for both `.STR` files.

## 6. Inside ROCK_X6.BIN and ROCK_X6.DAT: boxes with a list on the lid

Both files are simple **archives**, like a ZIP file with no compression:

```
| list (2 sectors) | piece 0 | piece 1 | piece 2 | … | last piece |
```

- The list is pairs of numbers: `{where the piece starts (in 2048 B sectors),
  how big it is (in bytes)}`. A pair of zeros ends the list.
- The pieces sit one after another with no gaps. Each piece starts on a new
  sector.
- `ROCK_X6.BIN`: 59 pieces, documented by the `mmx6` project.
- `ROCK_X6.DAT`: 243 pieces. We found this list ourselves on 2026-10-18.
  `mmx6` still lists the file as unknown.

Many `ROCK_X6.DAT` pieces start with a smaller list of their own (a count, the
piece size, then `{type, size}` pairs). So a piece is a smaller box with more
items inside it. Decoding those items is the next task (C), not this one.

## 7. What the extractor does, layer by layer

| Layer | Extractor step | How we know it is correct |
|---|---|---|
| CUE → BIN | Read the CUE, find the BIN, check its SHA-1 | Must equal the Redump hash |
| BIN → sectors | Cut the BIN into 2352 B sectors, keep the user data | Sector math (LBA × 2352) |
| Sectors → files | Read ISO9660 at LBA 16, follow the folders | Each file's SHA-1 equals the `mmx6` manifest |
| Files → pieces | Read the list in `ROCK_X6.BIN` and `ROCK_X6.DAT` | BIN pieces: `mmx6` hashes. DAT pieces: no gaps, list ends exactly at the end of the file |

## Looking at it yourself

Sublime Text shows a binary file as hex, but it cannot jump to a byte offset.
`xxd` can do this:

```
cd ~/mmx-data/disc
B="Mega Man X6 (USA) (v1.1).bin"
xxd -s $((LBA*2352)) -l 64 "$B"
```

Replace `LBA` with a sector number from the table above. The first 24 bytes are
the sync, header and subheader. The file data starts at byte 24 of the sector.

Do not put commas in the number. In shell arithmetic, `$((211,998*2352))` is the
comma operator: it drops `211` and calculates `998*2352`. You then read LBA 998
(inside `XA/DEMO.XA`), not LBA 211,998. To check that you are in the correct
sector, decode the header: `MM SS FF` → `(MM*60 + SS)*75 + FF − 150` must equal
the LBA. The bytes are BCD, so header `47 08 48` means 47:08:48, not hex.

### Example: the first sector of `ROCK_X6.DAT` (LBA 211,998)

```
1db853a0: 00ff ffff ffff ffff ffff ff00 4708 4802  ............G.H.
1db853b0: 0000 0800 0000 0800 0200 0000 0050 0000  .............P..
1db853c0: 0c00 0000 0048 0000 1500 0000 0048 0000  .....H.......H..
```

- Header `47 08 48 02`: (47 × 60 + 8) × 75 + 48 − 150 = 211,998. Mode 2.
- Subheader submode `0x08`: data, Form 1.
- The data at byte 24 is the archive list, as little-endian 32-bit numbers:
  `02 00 00 00` = 2, `00 50 00 00` = `0x5000`, then `0x0c`, `0x4800`, `0x15`,
  `0x4800`. So piece 0 starts at sector 2 and is `0x5000` B (10 sectors), and
  piece 1 starts at sector 2 + 10 = 12 (`0x0c`).

### Example: an XA audio sector (LBA 998)

```
0023d120: 00ff ffff ffff ffff ffff ff00 0015 2302  ..............#.
0023d130: 0105 6400 0105 6400 1414 1515 1414 1515  ..d...d.........
```

- Header `00 15 23`: (0 × 60 + 15) × 75 + 23 − 150 = 998, inside `XA/DEMO.XA`.
- Subheader `01 05 64 00`: file 1, **channel 5**, submode `0x64` = audio +
  Form 2 + real-time. An XA file interleaves several audio streams (channels)
  sector by sector, and the drive plays only the selected channel.
- The data is ADPCM sound. Each 128-byte sound group starts with 16 bytes of
  decoder settings (`14 14 15 15 …`, each value written twice), then the
  compressed samples.
