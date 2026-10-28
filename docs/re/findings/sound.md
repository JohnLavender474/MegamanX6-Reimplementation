# Sound: VAB banks, `PlaySound` and the XA music

This page tells where the sound effects are on the disc, how the tool decodes them, and how a
`PlaySound(bank, id)` call in the game code finds its sample. The last section, "Music (XA)", covers
`XA/BGM.XA` and `XA/DEMO.XA`.

Tool: `python3 -m mmxdemake.sound` (`mmxdemake/vab.py`, `mmxdemake/sound.py`). Output:
`~/mmx-data/x6/sounds/`. Facts are [certain for the code and the data] unless marked.

## On the disc

- The banks are in `ROCK_X6.DAT` members. The high half of an archive entry type is the loader
  handler (`arc_member_handlers`, `docs/engine.md`): 0 RAM, 1 VRAM, 2 VAB body, 3 SEQ.
- A bank has two entries:
  1. A RAM entry: word 0 = the offset of the VAB head (bits 0–23) and the **slot** (bits 24–31);
     word 1 = the body size; then the **sound words** (one per sound id); then the VAB head
     (magic `pBAV`).
  2. The next entry with handler 2 (type `0x2xxxx`) after it: the VAB body (the samples). It is
     not always the next entry (member 108 has a VRAM entry between them).
- The VAB head is the standard Sony format: a 32-byte header (version 7, program, tone and sample
  counts), 128 program records of 16 bytes, 16 tone records of 32 bytes for each program that
  has tones (in program order; the program numbers can have gaps), and 256 sample sizes (in units
  of 8 bytes). In every bank the sample sizes add up to the body size.
- The samples are SPU ADPCM: 16-byte blocks of 28 samples, with a shift, a filter (5 filter
  pairs) and flags (1 end, 2 repeat, 4 loop start). Play stops at the first block with the end
  flag. The X6 samples all end with one more silent block with flags 7, which play never reaches.
- 116 banks in 80 members, 55 of them different; 716 samples, 60 of them loop.

## The bank slots

`PlaySound(bank, id, obj)` (`0x80016C48`) takes a bank number 0–5. `sound_bank_vab`
(`0x80090D50`) gives the VAB id of each bank (−1 = not loaded), and `sound_words`
(`0x80097778`) / `sound_words_end` (`0x80097738`) give the bank's word list in RAM. The bank
number is the slot in word 0 of the head entry. A match of the word lists in the save states:

| bank | members (entry) | use [likely for the use] |
|---|---|---|
| 0 | 85–89 (4) | common sounds, the same in every state |
| 1 | 86, 87 (11) | the player |
| 2 | the stage member (intro 94 entry 5; Yammark 95; Sheldon 102; D-1000's room 123) | the stage and its enemies |
| 3 | 86–89 (13) | the player's second bank |
| 4 | not loaded in any state read | |
| 5 | the stage member's second bank (intro 94 entry 3) | |

So a sheet's "bank 2, id 79" (the Guardian shot, stage Sheldon) is word 79 of `m102-e05`.

## The sound word

`PlaySound` returns −1 when the bank is not loaded, −2 when `id` is past the list, −5 for a zero
word. Else byte 0 >> 6 indexes `sound_kind_handlers` (`0x8006E354`: 1, 2, 0, 0):

| bits | meaning |
|---|---|
| byte 0 bits 6–7 | kind: 0, 1 a sequence (`PlaySequence` `0x80017008`); 2, 3 one tone (`PlayTone` `0x80016D34`) |
| byte 0 bits 0–5 | the VAB id |
| byte 1 bits 0–6 | the program (for a sequence: the sequence number) |
| byte 1 bit 7 | a flag [open] |
| byte 2 bits 4–7 | the tone in the program |
| byte 2 bits 0–3 | the priority |
| byte 3 bits 0–4 | the SPU voice |
| byte 3 bits 5–6 | a mode [open] |

`PlayTone` gets the tone record (`0x8005A794`), scales the volume by the distance of `obj`, and
keys on the voice at the tone's centre note (`0x8005A9D4`). So a tone plays its sample at the
base pitch.

## The output

Each bank is a folder `mNNN-eKK` (member, entry). It holds `vag-NN.wav` (16-bit mono, numbered
as in the VAB) and `bank.json`: the slot, the sound words, `ids` (each word decoded, with the
sample and the WAV file it plays), the programs and tones (centre note, fine tune, ADSR, sample)
and the samples (length, loop start). A copy of an earlier bank has only `bank.json`, with
`same_as`. `summary.txt` lists the banks.

Of the 772 sound ids of the different banks, 695 name a WAV file and 20 are sequences. The rest
name a VAB id that is not the bank's own (151 over all banks), or a program or tone that is not
in the bank (42 over all banks, for example in the boss members 137 and 140) [open: probably the
stage bank of the same slot].

## Rough parts

- The WAV files are written at 44 100 Hz. The real play rate depends on the tone's centre note
  and fine tune (in `bank.json`) [guess: most effects are near the base rate].
- The ADSR envelopes, the volume, the pan and the SPU reverb are not applied.
- The sequence sounds (kinds 0 and 1) and the SEQ members are not decoded.

## Music (XA)

Tool: `python3 -m mmxdemake.music` (`mmxdemake/xa.py`, `mmxdemake/music.py`). Output:
`~/mmx-data/x6/music/` (about 1.4 GB of WAV files; about 1 minute with 16 processes).

- `extract` keeps the two XA files as raw Form 2 sectors of 2336 bytes: an 8-byte subheader (file,
  channel, submode, coding, written two times), 2324 data bytes, 4 EDC bytes.
- The audio sectors have submode bit `0x04`. The other sectors (file 1, channel 0, coding 0) are
  filler, and the tool skips them.
- A sector holds 18 sound groups of 128 bytes. A group has 16 parameter bytes (the 8 unit
  parameters are bytes 4–11: shift in bits 0–3, filter in bits 4–5) and 28 × 4 data bytes; unit
  `u`, sample `j` is the nibble `u & 1` of byte `16 + 4j + u / 2`. In stereo the even units are
  left and the odd units are right, and each side keeps its own filter history. This is the
  standard CD-XA ADPCM.
- The end-of-file flag (submode `0xE4`) ends a record on its channel. The tool writes one WAV
  file per record: `cNN-tMM.wav` (channel, record number) and `tracks.json` (first and last sector,
  sector count, LBA, coding, length).

| file | channels | coding | records | length |
|---|---|---|---|---|
| `BGM.XA` | 8 | `0x01`: stereo, 37 800 Hz, 4-bit | 30 | 8279 s |
| `DEMO.XA` | 16 | `0x00`: mono, 37 800 Hz, 4-bit | 540 (0.3–9.1 s) | 2001 s |

- `BGM.XA`: 21 records of 360–416 s, one of 249 s, and 8 short records (1–9 s) at the ends of
  channels 0 and 1. The track table shows that each record is one song (see "The track table").
- `DEMO.XA`: short mono records [likely: the voice lines of the scenes and the talk].
- The stereo channels correlate (0.5–0.64 on the first four records) and the zero-crossing rate is
  low (0.06–0.10), as for music; the WAV files were not listened to.

## The track table

`python3 -m mmxdemake.music` also reads the stream tables from the executable and writes
`bgm/songs.json` and `demo/voices.json` [certain for the code and the tables].

- **`StreamPlay(track)`** (`0x80018060`) reads row `track` of `bgm_streams` (`0x8006E58C`, 30
  rows of 16 bytes): start sector, end sector (both from the start of the file), channel, loop
  flag. It adds the LBA of file 245 (`0x80016548`: the file table at `0x800E0398`, 8 bytes each;
  245 = `BGM.XA` at LBA 25217). **`VoicePlay(n)`** (`0x80018178`) does the same with
  `voice_streams` (`0x8006E76C`, 541 rows) and file 246 (`DEMO.XA`, LBA 961).
- The stream poll (`0x80018644`) stops at the end sector. With the loop flag it starts the stream
  again from the start sector; without it, it pauses the CD.
- Every one of the 30 `BGM.XA` rows starts on the first sector of one record and ends at its last
  sector, so **one record is one song** (the 6–7 minute records are long songs, probably with the
  loop played more than once on the disc [guess]). The 541 voice rows all fall inside the 540
  `DEMO.XA` records.

**Which song plays where:**

- **Stage music** (`StageMusicRestart` `0x8001854C`, from `ReadyStart`): byte
  `stage_music[stage × 8 + area × 4 + character × 2]` (`0x8007093C`, 23 stages); the next byte is
  112–127 [likely: a volume]. Stage 15 area 1 plays nothing new.
- **Boss music** (`BossMusicStart` `0x80017F6C`): `boss_music[stage × 2]` (`0x800709F4`), with
  the cases of the catalog note (stage 0: 21, or 23 with `game+0x26`; stage 12 area ≠ 0: 27 or 20;
  stages ≥ 19 by `boss_choice`).
- **Fixed calls:** `PlayerExit` (`0x8003B8CC`) track 4; `BossGate` (`0x80051AA0`) track 28;
  overlay 16 (the stage select) track 1; overlay 17 (the boss intro, game mode 6) track 3, and the
  intro ends when it stops; overlay 20 (the stage result) track 6;
  overlays 18, 19 and 21 pass a value from a register or a table [open].

| track | record | length | loop | used by |
|---|---|---|---|---|
| 0, 2, 8, 10, 29 | short records of channels 0 and 1 | 0.96 s | yes | 0: the stage and boss tables where there is no music (stages 14, 15; many bosses) [likely: silence] |
| 1 | `c06-t02` | 364 s | yes | stages 13, 14; the stage select |
| 3 | `c01-t02` | 9.4 s | no | stage 13 area 1; the boss intro (overlay 17) |
| 4 | `c00-t03` | 5.5 s | no | `PlayerExit` [likely: the stage clear] |
| 5 | `c00-t02` | 6.4 s | no | – |
| 6 | `c04-t01` | 374 s | yes | stage 15; the stage result |
| 7 | `c02-t02` | 249 s | no | – |
| 9 | `c07-t02` | 360 s | yes | stage 0 (the intro) |
| 11–18 | | 370–416 s | yes | the stage music of stages 1–8, and 19–22 (the second visits) |
| 19 | `c04-t00` | 393 s | yes | stages 16–18 |
| 20 | `c03-t00` | 398 s | yes | stages 9–12; stage 12 boss (area ≠ 0) |
| 21 | `c05-t02` | 363 s | yes | boss: stages 0, 16 (the intro boss) |
| 22 | `c04-t02` | 366 s | yes | boss: stages 1–8, 12 (the Mavericks) |
| 23 | `c07-t01` | 386 s | yes | boss: stage 17; stage 0 with `game+0x26` |
| 24, 25 | | 370, 392 s | yes | stages ≥ 19 by `boss_choice` |
| 26 | `c00-t01` | 367 s | yes | boss: stage 18 (Gate) |
| 27 | `c05-t01` | 377 s | yes | stage 12 boss (area ≠ 0) |
| 28 | `c03-t02` | 360 s | yes | `BossGate` |

`songs.json` has one row per track: the channel, the sectors, the loop flag, the record WAV, the
start and end in seconds in that WAV, and the stage and boss uses. `voices.json` has the same for
the 541 voices (with no uses: `VoicePlay` is called only from overlay 12, with a value from a
table [open]).
