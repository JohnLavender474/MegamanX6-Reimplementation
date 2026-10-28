# Process log: how we started demaking Mega Man X6 into Maverick

Status: working draft for a later presentation. Started 2026-10-18.

This log records each step in the order we did it. Each step gives what we did, why, the result and the
evidence.

---

## 1. Review the PDF plan

**What we did.** We read the user's research document, "Demaking a Mega Man Game on the Maverick Engine"
(12 pages, dated 2026-10-17). Then we compared its claims with the Megaman-Maverick repo.

**What the plan says.**
- Use two stages with a neutral middle. Stage 1 extracts the game into engine-agnostic files: PNG tile
  sheets, JSON room grids, JSON object placements and a written behavior spec for each enemy. Stage 2
  converts these files into Maverick content: 8-bit art, TMX maps made with the `level-builder` tooling,
  and Kotlin subclasses of `AbstractEnemy` / `AbstractBoss`.
- Get the logic by reading the decomp and writing a behavior spec, not by transpiling. A literal
  translation would import X6 physics and frame timing, and these do not agree with Maverick's.
- The route to the X6 maps: X4, X5 and X6 share an engine. The `TeheManX4_Editor` fully decodes the X4
  stage format. The plan was to dump X6 RAM after a stage loads, then decode it with X4's rules. The
  on-disc `ROCK_X6.DAT` was called undocumented.
- The pilot is Commander Yammark's stage.
- Legal rules: no game data in any repo, keep raw extracted data private, and write your own code
  instead of copying the AGPL or PolyForm projects.

**Where the repo check disagreed with the doc.** The doc calls `X6_CommanderYammark_Remake_v1.tmx` a
"hand-made answer key" for the pipeline. In the repo, this map is only 50 × 25 tiles. It has one room and
two blocks, and it is not registered in `LevelDefinition`. It is a tileset test, not a full-stage answer
key.

**Other repo facts that we confirmed.**
- `LevelType.EXTRA_LEVEL` already exists (`GAUNTLET_1`, `GAUNTLET_2`, `RUMBLECADE_FACTORY`). These levels
  go back to `EXTRAS_SCREEN` when they end.
- `MMX6_YAMMARK_MUSIC` and the burailaplace 8-bit Yammark tileset are already in the repo.
- The PS1 screen is 20 × 15 tiles. The Maverick view is 16 × 14 tiles. So each room needs a design pass
  by a person.

---

## 2. Divide the work into sub-projects

**What we did.** We classified the work as architectural and too large for one spec. Then we divided it
into six sub-projects. Each one gets its own spec, plan and build cycle.

1. Map extraction (X4 → X6): RAM dump, decoder, neutral JSON.
2. Map conversion: neutral JSON → `level-designer` grid → TMX → `level-analyzer` diff.
3. Tileset conversion: quantize, dedupe, cleanup by a person.
4. Enemy port pipeline: behavior spec → Kotlin, plus a table from source IDs to Maverick entities.
5. Boss port: Yammark on `AbstractBoss`.
6. Pipeline skills: `demake-extract`, `demake-spec`, `demake-entity`, written after the pilot.

---

## 3. Decide the repo layout

**What we did.** The user asked if the sub-projects need new repos. We recommended one new repo, not
one for each sub-project, with the split at the neutral-file boundary.

| Location | Contents |
|---|---|
| New repo `mmx-demake-tools` | Extraction: emulator and dump scripts, decoders, neutral JSON schema |
| Megaman-Maverick (existing) | Conversion to TMX and tilesets, Kotlin ports, `LevelDefinition` entries, `demake-*` skills |
| `~/mmx-data/` (outside all repos) | Disc image, dumps, extracted files |
| `~/IdeaProjects/mmx-reference/` | Read-only clones of the reverse-engineering projects |

**Why a separate tools repo.**
- Legal isolation: extraction code works directly with copyrighted game data. A separate repo prevents
  an accidental commit of a dump into an MIT game repo.
- Clean-room boundary: we read AGPL and PolyForm code while we write decoders. A separate repo makes it
  clear that the code is original.
- Different toolchain: Python and emulator scripting, with no connection to Gradle or Kotlin.
- Reuse: format findings can go back to the `mmx6` project.

**Why not more repos.** The user works alone. Each extra repo adds setup and version work, and no other
sub-project needs to stand alone. The only contract between the two repos is the versioned neutral JSON
schema.

**The user's decision.** Keep all repos local with git. Do not push to GitHub yet.

---

## 4. Do the local setup

**What we did.** We checked the five reference repos with `git ls-remote`. Then we cloned them with
`--depth 1` into `~/IdeaProjects/mmx-reference/`, and created the tools repo and the data folder.

**New tools repo.** `~/IdeaProjects/mmx-demake-tools/`, branch `master`, first commit `9ad4ebf`
("Initial commit: README and data-blocking .gitignore").
- `README.md` gives the rules: no game data, and reference repos are for facts only.
- `.gitignore` blocks `data/`, `dumps/`, `out/` and disc or dump formats (`*.BIN`, `*.DAT`, `*.ARC`,
  `*.iso`, `*.cue`, `*.img`, `*.chd`, `*.sav`, `*.mcd`).

**Reference clones.**

| Repo | Size | License |
|---|---|---|
| `anzaldoivan/mmx6` | 35M | AGPL-3.0 |
| `sozud/mmx4` | 14M | AGPL-3.0 |
| `mstan/MegaManX6Recomp` | 44M | PolyForm Noncommercial 1.0.0 |
| `Kuumba123/TeheManX4_Editor` | 2.9M | none (all rights reserved) |
| `Kuumba123/MegaManX6_PS1_Modding` | 304K | none (all rights reserved) |

- The PDF only gave the AGPL license for `mmx4`. `mmx6` is AGPL too.
- Two repos have no license. That is more restrictive than AGPL. So we only read all five for facts.
- We did not get the git submodules of `MegaManX6Recomp` and `mmx4`. They are only needed to build those
  projects.

**Data folder.** We created `~/mmx-data/` outside all repos.

---

## 5. Choose the goal: the pipeline first

**What we did.** We asked the user what to get first: a playable Yammark level soon, the automated
pipeline first, or both at the same time.

**The user's decision.** The automated extraction pipeline first. The user's aim is to recreate X6 in
Maverick as closely as possible, so we must extract as much data from X6 as we can. The user accepts that
the result cannot be 1:1, because X6 is a PS1 game and Maverick copies the NES style.

**Reading the reference repos for this decision.**
- `mmx6` has fully decoded `ROCK_X6.BIN`: 59 uncompressed code overlays with a `{sector, size}` TOC.
  `ROCK_X6.DAT` (50,913,280 B) is "Unknown" in its `docs/formats.md`.
- `MegaManX6Recomp/tools/dbg.py` talks to a TCP debug server on port 4490, with commands such as
  `read_ram` and `screenshot`.
- `TeheManX4_Editor/TeheManX4/Redux.cs` reads and writes PS1 RAM through the PCSX-Redux web API
  (`/api/v1/cpu/ram/raw`).
- `MegaManX6_PS1_Modding/symbols/layer.txt` gives `bgLayers = 0x800971f8`, `layoutWidth = 0x800cd338`,
  `layoutHeight = 0x800cd339`, `layoutSize = 0x8008ec10`.
- All tools need the same disc: US release SLUS-01395 v1.1, as a Redump-matching BIN/CUE.
- This machine had no X6 image, no PCSX-Redux and no DuckStation.

---

## 6. Identify and verify the disc

**What we did.** The user looked up Redump entry 32516 (Mega Man X6, USA, SLUS-01395, "Rev 1"). We
compared it with the hashes that the tools expect.

| | Redump | `mmx6` `config/medium.sha1` | `MegaManX6Recomp` `DISC.md` |
|---|---|---|---|
| Version | Rev 1 | v1.1 | v1.1 |
| SHA-1 | `d4f7e08371027a87a3bf13311db5a4c56733f4ea` | same | same |
| MD5 | `237b6feddd1a88e86ab1cddc8822f03f` | — | same |
| CRC-32 | `8e6d014d` | — | `8E6D014D` |

Result: Redump "Rev 1" is the v1.1 that both tools expect. Redump also lists a v1.0 (SHA-1
`80F856D4…`), which the tools do not accept.

**Verification of the user's own image.** The user's dump is in
`~/Downloads/Mega Man X6 (USA) (Rev 1)/`.

| Check | Expected | Result |
|---|---|---|
| Size | 599,985,792 B | match |
| SHA-1 (`sha1sum`) | `d4f7e083…6733f4ea` | match |
| MD5 (`md5sum`) | `237b6fed…822f03f` | match |
| Cue sheet | 1 track, `MODE2/2352` | match |

**Copy and rename.** `mmx6` (`Makefile:10`) and `MegaManX6Recomp` (`game.toml:7`) both expect the name
`Mega Man X6 (USA) (v1.1)`. We copied both files to `~/mmx-data/disc/` with that name, and we did not
change the originals in `~/Downloads`. Then we changed the `FILE` line in the cue sheet from
`(Rev 1).bin` to `(v1.1).bin`. A second `sha1sum -c` on the copy gave `OK`.

---

## 7. Inspect the disc and the repos

**What we did.** The user asked us to inspect the files and the repos before we took any action. We
wrote throwaway Python probes in the session scratchpad (not in a repo) and ran them with `python3 -I`.

**7.1 Our own ISO9660 reader.**
- The reader found all 9 files on the disc.
- 7 of 9 files have the same SHA-1 as in `mmx6/manifest/retail.jsonl`: `SLUS_013.95`, `ROCK_X6.BIN`,
  `ROCK_X6.DAT`, `SYSTEM.CNF`, `XA/BGM.XA`, `XA/DEMO.XA`, `ZNULL.DAT`.
- The 2 `.STR` video files did not match. The cause is in our probe, not in the data: the probe read the
  Form 1 / Form 2 type from the first sector's subheader only. `mmx6/docs/formats.md` says to use the
  XA attribute in the directory record (bit `0x1000` = Form 2) and to cross-check it with the subheader.
- The manifest also has the SHA-1 of all 59 `ROCK_X6.BIN` members. So a static extraction can be checked
  at each step.

**7.2 `ROCK_X6.DAT` has a simple outer layer.** The file starts with the same `{u32 sector, u32 size}`
TOC as `ROCK_X6.BIN`.
- 243 entries, then a zero pair at offset 1944. The rest of the header is zero.
- The first member starts at sector 2. There are no gaps between members.
- The last member ends at sector 24,860, which is the end of the file.
- Member sizes go from 512 B to 1,030,144 B. The most common size is `0x4800` (72 members).

**7.3 Sub-headers in the members.**
- 197 of the 243 members start with `{u32 count, u32 total_size}`, where `total_size` equals the member
  size. Then `count` × `{u32 type, u32 size}` sub-entries follow.
- There are 32 different sequences of type codes.
- The sub-entry sizes do not add up exactly to the member size (only 47 members fit our first rule). So
  the padding or alignment rule is still unknown.
- The other 46 members are in the range 83..202.
- The byte entropy is mostly 2–6 bits/byte. So most members are probably not compressed.

**7.4 A palette lead.** Members 168–202 are 512 B each. Their words look like 15-bit colours, and 512 B
is 256 colours × 2 B. They may be CLUTs (palettes). This is a lead only.

**7.5 The PDF's RAM addresses are weaker than stated.** `mmx6/docs/prior-art.md` (row L4) rates the 20
Kuumba123 RAM addresses, including `layoutWidth` and `bgLayers`, as **lead**, not proven.

**7.6 Runtime tool status.**
- `mmx6` dumps RAM with PCSX-Redux in headless mode and Lua scripts (`make redux-smoke`). Its targets
  are written for macOS, but Redux also runs on Linux.
- The recomp has a TCP `read_ram` server and Linux packaging.
- Neither is installed. Both need a build, a BIOS (OpenBIOS is free) and scripted input to reach a stage.

**Conclusion.** Static extraction can reach more than the PDF expected. The RAM dump can be a
cross-check, not the only route.

---

## 8. Choose the first sample task

**The user's concern.** The user said that going deep first could cause errors that are hard to
diagnose. They asked for a small sample task to prove that we can extract data at all.

**The options.**
- **A. Static container extraction.** Disc → 9 ISO files → 59 `ROCK_X6.BIN` members → 243
  `ROCK_X6.DAT` members, plus a JSON index. Each output is checked with `mmx6`'s hashes, or with size and
  coverage checks for the DAT. No emulator, and the same result on each run.
- **B. Runtime proof.** PCSX-Redux, boot to a stage, dump RAM, read `layoutWidth` / `layoutHeight`.
  It has many failure points and no hash to check against.
- **C. Visual proof.** Decode one 512 B member as a palette to PNG. The only check is a person's eyes.

**The decision.** A first, then C on top of A's output, then B.

**Why.** Each step adds only one new unknown. If a step fails, there is only one place to look. A
hash-verified first step gives a known-good base for all later work.

**Next action.** Write a short spec for task A in `mmx-demake-tools`, then a plan, then the code.

---

## 9. Write the spec for task A

**What we did.** We wrote a design spec:
`docs/superpowers/specs/2026-10-18-container-extraction-design.md` (commit `3011eb8`).

**What it says.** One small Python module for each layer of the disc: CUE and sectors, the ISO9660
file system, the `{sector, size}` archives, the index file, the reference check, and the command
that runs them in order. Every output gets a check that can fail. A fact that we have only seen once
(for example the DAT sub-headers) is recorded as an "observation", not as a rule.

**Why.** If the extractor is wrong, every later step builds on bad data. Small layers with their own
checks show exactly which layer failed.

---

## 10. Understand the disc image (the user's questions)

**What we did.** The user opened the BIN and CUE in a text editor and asked what they are. We wrote
`docs/process/disc-image-explained.md`: the CUE line by line, the layout of one sector, sector
addresses (LBA and MSF), the file system, the 9 files, and the archive lists.

**A surprise.** While we looked at real sector bytes for the explainer, we saw that the first sector
of `STR/CAPLOGO.STR` does not carry the "Form 2" flag (submode `0x48`). The `mmx6` doc says that it
does. This explains why our first probe got the wrong hash for both video files. The spec said that a
disagreement here is an error, so the extractor would have failed on a good disc. We changed the spec:
the folder entry decides the form, and the sector flag is only recorded (commit `3b4df81`).

**A shell trap.** The user ran `xxd -s $((211,998*2352))`. In shell arithmetic the comma is an
operator, so this read LBA 998 (an XA audio sector, channel 5), not LBA 211,998. We decoded both
sectors and added the trap and a header check (`(MM*60 + SS)*75 + FF − 150 = LBA`) to the explainer.

**Lesson.** Looking at real bytes found a spec error before we wrote any code.

---

## 11. Write the implementation plan

**What we did.** We wrote a 6-task plan:
`docs/superpowers/plans/2026-10-18-container-extraction.md` (commit `19e75cc`). Each task has its tests
first, then the code, then a commit. Tasks 1–5 build and test the modules on small fake discs made in
memory (no game data in the repo). Task 6 runs on the real disc and compares against the `mmx6` hashes.

**Corrections found while planning** (spec commit `3b4aac2`):
- The spec's commands used `python3 -I`. Isolated mode cannot find a package in the current folder,
  so the commands would fail. The `-I` rule protects against code planted in a folder of downloads;
  here the code is our own repo, so the commands now run without `-I`.
- A path written as `"~/…"` in quotes is not expanded by the shell, so the tool expands `~` itself.
- The tool refuses an output folder inside the repo, so game data cannot be committed by accident.

---

## 12. Build the container extractor (task A)

**What we built.** A small Python package, `mmxdemake`, with one module for each layer of the disc:

| Module | Layer | Commit |
|---|---|---|
| `cdimage` | CUE → BIN → 2352 B sectors, identity hash | `2796139` |
| `iso9660` | sectors → the 9 files (form taken from the folder entry) | `ef1fbca` |
| `rockarc` | `{sector, size}` archives → members, list checks, header observations | `a963a91` |
| `index`, `verify` | `index.json`, comparison with the `mmx6` hashes | `9945681` |
| `extract` | the command that runs all layers and handles failures | `e3c2def` |

**How we built it.** Test first for every module. Each test was run and seen to fail before the code
existed, then seen to pass after. The tests use small fake discs built in memory, with known faults
added on purpose (a gap in a list, a missing end marker, a video file without the Form 2 flag, a
non-zero padding byte). Each fault must give an exact error message.

**Result on the real disc.**

```
identity: ok
note: STR/CAPLOGO.STR: directory says Form 2, first sector submode 0x48
note: STR/X6OP.STR: directory says Form 2, first sector submode 0x48
ISO: 9 files
ROCK_X6.BIN: 59 members, TOC ok, 59/59 tail-zero
ROCK_X6.DAT: 243 members, TOC ok, 243/243 tail-zero (observation)
verify: 68 reference hashes match; 243 outputs have no reference
done: /home/johnlavender/mmx-data/extracted/retail
```

- All 68 reference hashes match, including the two `.STR` files that the first probe got wrong.
- Both video files start with a sector that does not carry the Form 2 flag. The folder-entry rule was
  correct.
- New fact: all 243 `ROCK_X6.DAT` members have zero padding after their size, like `ROCK_X6.BIN`.
  This is now observed on the whole file, so a later task can make it a hard check.
- Output: `iso/` 556 MB, `rock_bin/` 1.7 MB, `rock_dat/` 49 MB, `index.json` 144 KB. All of it is in
  `~/mmx-data/`. `git status` showed no game data in the repo.

**Evidence.** Unit suite: 51 tests, `OK (skipped=1)` (the skip is the real-disc test). Real-disc test:
1 test, `OK`, in 4 s for two full extractions; the second run gave a byte-equal `index.json`.

**Why it matters.** We now have every piece of the disc as a separate, verified file, and a list that
says where each piece came from. Task C (decode a palette) can start from `rock_dat/` with confidence
that the input bytes are correct.

---

## 13. Independent review, and a data-loss bug

**What we did.** A fresh reviewer agent (on the most capable model, with no memory of how the code
was written) read the whole change and tried to break it with test discs of its own.

**What it found.**
- **Critical:** a successful run deleted any folder that `--out` named. `--out ~/mmx-data` would have
  deleted the disc image; `--out ~` would have deleted the home folder. All our tests passed, because
  none of them tried a wrong `--out`.
- **Important:** a bad `--reference` file, an unwritable folder, or an output path that is a file or a
  symlink gave a Python traceback, some only after all the work. A reference with no usable records
  said "0 hashes match" and still reported success.
- **Minor (deferred):** three cases that only a damaged disc can reach, because the identity check
  stops any other disc first, and noisy test output.

**The fix** (commit `4cd1b03`). The tool now replaces `--out` and `<out>.tmp` only when they are empty
or hold nothing but its own output (`iso`, `rock_bin`, `rock_dat`, `index.json`). It loads and checks
the reference before any work. Write errors become `error:` lines. Each fix started as a test that
failed; the suite went from 51 to 60 tests, all passing. On the real disc, `--out ~/mmx-data` is now
refused with exit 2, and the normal run still matches all 68 hashes.

**Lesson.** Tests check what their author thought of. A reviewer with fresh eyes found the one case
that could destroy data. For any tool that deletes or overwrites, ask "what if the user points it at
the wrong place?"

---

## 14. Task C: test the palette guess on the verified output

**What we did.** Before the design, a throwaway probe read the verified `index.json` and rendered
two candidate pieces to PNG (outside the repo, in `~/mmx-data/probe/`).

**A correction.** The 512-byte pieces are not 168–202. They are the even members 164–202 (20 pieces),
and each one is followed by an 8,192-byte piece (165–201). On the PS1, this pattern usually means
"palette, then the image that uses it".

**Result (pair 180/181).**
- The palette's 256 colours run smoothly from dark to light, with a few bright accents. Colour 0 is
  `0x0000`, the PS1 value for "transparent". Random bytes do not look like this.
- The 8,192-byte piece, read as 8 bits per pixel at 128 × 64, shows real hand-drawn artwork
  (anime-style shading, armour, two small UI icons). Read as 4 bits per pixel, it is noise. So these
  images are 8 bpp.
- The artwork is cut into rectangular blocks: a texture sheet that the game puts together at run time.

**Why it matters.** It confirms the PS1 colour format and the image format for this data, and it is
a second, independent confirmation that task A cut the DAT at the correct places.

**Decision.** Scope A: build a reusable palette + pixel → PNG decoder and render all 20 pairs.
Rebuilding the whole portraits from the blocks is a later task.

**Side note.** The user opened the PNG by typing its path, which made the shell try to run it as a
script. The fix is an opener (`xdg-open`, or `gimp` via a new alias for the Flatpak).

---

## 15. Write the spec for task C

**What we did.** We wrote `docs/superpowers/specs/2026-10-18-clut-render-design.md`: a `render`
command with one module each for PS1 colours, pixel unpacking, PNG writing, and output-folder safety.

**How correctness is shown without reference hashes.**
- Exact unit tests on made-up colours and pixels (for example `0x7C00` must be pure blue).
- The PNG writer is checked by our own test reader and, separately, by Pillow.
- Each input piece is checked against its SHA-1 from task A before it is decoded.
- A person looks at all 20 images. Noise in any of them is reported as a finding.

**A design improvement from task A's review.** The output-folder guard that fixed the data-loss bug
moves into a shared module, so the new command gets the same protection from the start.

---

## 16. Write the plan for task C

**What we did.** We wrote a 5-task plan, `docs/superpowers/plans/2026-10-18-clut-render.md`
(commit `de74022`): colour and pixel decoding, the PNG writer, the shared output-folder guard, the
`render` command, then the real run.

**Defects found while planning.**
- The spec said the pixel module imports no other module, but it needs the colour module. We
  corrected the spec.
- In the planned code, `c & 0x7FFF == 0` would have counted no black colours at all. Python reads it
  as `c & (0x7FFF == 0)`, so the result is always 0. The fix is `(c & 0x7FFF) == 0`. Small mistakes
  like this are why every count in the summary also has an exact unit test.

---

## 17. Build the render command (task C)

**What we built.** Five modules, test first: `psxcolor` (PS1 colours → RGBA), `indexed` (4 and 8 bpp
pixels), `png` (PNG writer, standard library only), `outdir` (the data-loss guard, now shared with
`extract`), and the `render` command. Commits `1486eca`, `f75b574`, `b54aa59`, `d9f57f4`.

**How it was tested.** 97 tests (2 skipped real-data tests). The PNG writer is checked by our own
reader and by Pillow, a separate PNG library. After the guard moved, all 9 task A guard tests passed
without any change.

**A check caught a spec error.** The first real run stopped with
`error: found 19 palette/image pairs, expected 20`. The cause was a miscount in the spec, not a
code bug: there are 20 palettes, but the last one (member 202) is followed by member 203, which is
6,144 bytes, not 8,192. Member 203 starts with a `{count, size, type, size}` sub-header
(`{1, 6144, 0x15, 2724}`), so it is a small container, not raw pixels. We corrected the expected count
to 19 and kept the check as strict as before. Rendering 202/203 four ways (8 and 4 bpp, widths 128
and 64) gave noise each time, which agrees.

**Result on the real data.** 19 pairs rendered to `~/mmx-data/renders/`. Every palette has colour 0 =
transparent. A contact sheet of the 19 images shows:
- 17 pairs: clear hand-drawn artwork (faces, eyes, armour, hair), cut into blocks, each with small
  square UI icons.
- 198/199: mostly outlines and transparency (193 of its 256 colours are black). Structured, not noise.
- 200/201: a grainy, blurred texture with sharp UI icons on top. Possibly a "static" effect. Unclear.

**Still to do.** The user looks at the 19 images (`~/mmx-data/renders/`) and confirms.

**Lesson.** A strict count check turned a silent wrong assumption ("20 pairs") into a clear stop
with the exact number. We did not loosen the check; we found the cause and corrected the fact.

---

## 18. Independent review of task C

**What we did.** A fresh reviewer agent read the whole change, ran both real-data tests, opened a
render, and tried to break the tool with its own inputs.

**Result.** No critical or important issues. It confirmed that the colour and nibble rules match
the PS1 format, that the PNGs are valid for normal viewers, and that the moved guard still refuses
every dangerous `--out` it tried (the extracted folder, a parent of it, a parent of the repo, a
symlink, a file).

**Six minor findings, deferred for the user to decide.** Tracebacks on a hand-edited `index.json` or
on `--out .`; an invalid 0-height PNG from an empty member in manual mode; `--clut-row` silently
ignored at 8 bpp; the guard checks only top-level names; one stale "40 PNGs" line in the spec.
None of them loses data.

**Compared with task A.** Task A's review found a data-loss bug. This review found none, partly
because task A's lesson (a shared, tested output guard) was built into this design from the start.

---

## 19. The user's check of the renders, and where the map data is

**The user's check.** The 19 renders are a mix of anime-style artwork. Some have clean colours;
others have colours that do not look right. The shapes are clean in all of them, so the pixel decode
is right. The likely cause is the pairing rule: "palette `i` belongs to image `i+1`" seems to hold
for some pairs only. The palettes also fall into two groups (STP on 255 of 256 colours, or on about
100–130). Open finding; it can be tested by rendering each image with each palette.

**The question.** Can map data (tiles, collision) come from the disc, or does it need a RAM dump?

**What we found.** The X4 editor (`TeheManX4_Editor`, `Level.cs`, read for facts only) loads each X4
stage archive by type code: `0` = screens (16 × 16 tile numbers, `0x200` bytes per screen), `1` =
tile info (4 bytes per tile; low 6 bits = collision type). X6 uses the same type codes in the
`ROCK_X6.DAT` sub-headers. 23 members (94–105, 112–122) hold both. In all 23, type `0` is an exact
multiple of `0x200` (30–256 screens) and type `1` an exact multiple of 4 (700–3,800 tiles).

**What it means.** Screens and collision are very probably on the disc and can be read statically.
The layout (which screen goes where) is not in these entries; X4 keeps it in the program file. A RAM
dump becomes the answer key for checking, not the only way in.

**Proposed next step.** Sample task D: decode one stage member's screens + tile info into collision
pictures, one per screen. It needs no graphics or palettes.

---

## 20. Task D exploration: the inside of a DAT member, and stage structure checks

**The user's constraint.** Use only the data we already have. Change to a RAM dump only if needed.

**Entry positions inside a member.** The X4 editor (`ARC.cs`, read for facts only) puts the first
entry at `0x800` and pads every entry to a `0x800` boundary. On X6, `0x800 + sum(padded sizes) ==
member size` for **197 of 197** members with a sub-header. This explains why the sub-sizes "did not
add up" in task A's inspection, and it lets us cut every member into its typed entries.

**Stage members.** For all 23 members with screens (type `0`) and tile info (type `1`):
- every screen tile number (low 14 bits, as X4 masks with `& 0x3FFF`) is below the tile count;
- the largest tile number used is exactly `tile count − 1`, so the tile table fits the screens
  exactly;
- 36–73% of cells are tile 0 (X4: "empty");
- 37 collision values occur (low 6 bits of the tile info); `0`, `56`, `57`, `60`, `63` are the most
  common. Their meanings are not known yet.

**Why it matters.** Without a RAM dump, the screens and collision of 23 stage sections are readable
and pass structure checks that random data would fail. The layout (which screen goes where) is not
in these entries.

---

## 21. Spec and plan for task D

**Spec** (`docs/superpowers/specs/2026-10-18-collision-map-design.md`, commit `b886490`): an
`arcentries` module that opens any DAT member by the proven `0x800` rule, a `stagemap` module that
decodes screens and tile info, and a `collision` command that writes `screens.json` (the first
neutral-format file of the pipeline), a collision contact sheet and a summary per stage member.

**Plan** (`docs/superpowers/plans/2026-10-18-collision-map.md`): 4 tasks, 29 new tests. One spec
check ("the header list runs past `0x800`") cannot trigger when the entry count is at most 63, so
the plan leaves it out and says why.

---

## 22. Build the collision command (task D)

**What we built.** `arcentries` (open any DAT member into typed entries), `stagemap` (screens and
tile info → tile, flags, collision, with a range check), and the `collision` command. Commits
`2de27fb`, `3950988`, `1375c52`, then the fix below. Test first throughout; 127 tests (3 skipped
real-data tests), and the real-data collision test passes.

**A count check caught a spec defect again.** The first real run stopped with
`error: found 51 stage members, expected 23`. The probe had chosen members with types `0`, `1` and
`0x10102`; the spec's rule asked only for `0` and `1`. Of the 28 extra members, 27 pass the same
strict check (largest tile = tile count − 1, 3–43 screens each): probably smaller screen sets such
as boss rooms, menus or backgrounds. Member 73 fails it (tile numbers up to 13,600 with 128 tiles),
so its types `0`/`1` mean something else. Ruling: the default run uses `0` + `1` + `0x10102` (the 23
stage members, as the spec's facts say); `--member N` accepts any member with `0` + `1`, so the 27
other screen sets can be looked at now. A new test went red, then green.

**Result on the real data.** 23 members decoded in 2.6 s into `~/mmx-data/collision/` (31 MB):

| Member | Screens | Tiles | Empty cells |
|---|---|---|---|
| 94 | 137 | 3098 | 62% |
| 95 | 217 | 3839 | 59% |
| 96 | 202 | 3353 | 55% |
| 97 | 204 | 3058 | 38% |
| 98 | 170 | 2000 | 59% |
| 99 | 165 | 2157 | 66% |
| 100 | 256 | 3386 | 44% |
| 101 | 154 | 2946 | 54% |
| 102 | 95 | 3273 | 62% |
| 103 | 135 | 2672 | 73% |
| 104 | 162 | 1009 | 66% |
| 105 | 67 | 1059 | 71% |
| 112 | 140 | 2431 | 49% |
| 113 | 67 | 773 | 49% |
| 114 | 180 | 1859 | 57% |
| 115 | 47 | 1314 | 73% |
| 116 | 74 | 1705 | 58% |
| 117 | 64 | 1178 | 63% |
| 118 | 60 | 1128 | 67% |
| 119 | 30 | 708 | 58% |
| 120 | 39 | 1389 | 36% |
| 121 | 47 | 1403 | 70% |
| 122 | 31 | 1777 | 64% |

**What the sheets look like** (members 94 and 105, checked before reporting): stage geometry, not
static. Large solid areas with distinct edge values along tops and sides, step patterns like stairs
or slopes, thin outlines (possibly ladders, spikes or special surfaces), and shapes that continue
across neighbouring screens, so screens are stored roughly in stage order. Later screens in each
member hold only tiles with art and no collision: probably the background-layer screens.

**Still to do.** The user looks at the sheets. Naming the collision values, the layout, and which
member is which stage are later tasks.

---

## 23. Independent review of task D

**Result.** No critical or important issues. The reviewer ran both test sets and the full command,
tried `--member` on 73 (refused: tiles out of range), 106 and 141 (screen sets: decoded), and 999
(refused), and viewed the sheets.

**Two useful confirmations.**
- In all 23 members, empty cells (tile 0) have collision 0 and no flags, so `screens.json` and the
  sheets agree.
- `screens.json` stores each grid as `[y][x]`, row by row. TMX uses the same order, which makes the
  later converter simpler.

**Six minor findings, deferred.** Tracebacks on a damaged index; an old error wording for
`--member`; large JSON files (one number per line); two dark colours that are close; a leftover
`.tmp` folder after a failed run; a naming limit at 1,000 members.

---

## 24. The user's check of the sheets, and a measured review

**The user's check.** The sheets look like a jumble of patterns. The user could not see any real
stage in them.

**A correction.** Earlier we said the sheets "clearly" show stage geometry. That came from looking
at two pictures and was too strong. We measured instead (throwaway scripts on `screens.json`):

| Claim | Measurement | Confidence |
|---|---|---|
| Screens hold real level structure | Neighbouring cells agree on solid/not-solid 91–95%; with the same cells shuffled, 63–78% (all 23 members) | High |
| Screens are drawn the right way round | Storage-order neighbours: right→left edges match 0.895, bottom→top 0.696 (1,538 pairs) | High |
| Storage order partly follows the stage | Right→left edge match 0.75–0.97 vs 0.52–0.84 for random pairs (member 117: no better than random) | Medium |
| The values are collision in the game's sense | X4 format; background-only screens have none | Medium |
| A person can recognise X6 stages in the sheets | No | — |

**Why it looks like a jumble.** No layout (a box of puzzle pieces, wrapped 16 to a row); no art
(collision is only an outline); arbitrary colours for unnamed values; foreground and background
screens mixed. A gap-free strip of member 94's screens in three colours was still hard to read.

**A new lead.** The tile info's graphics fields match the X4 format: texture page 0–11 (X4 draws
nothing above 11), a 16 × 16 position grid in the page (all 256 used), 96 palette numbers. Member 94
has two entries of exactly 262,144 bytes (types `0x10000`, `0x16`), which is 8 PS1 texture pages at
4 bits per pixel.

**Proposed next step.** Task E: draw the screens with their real tile graphics, from the disc data
only, so that a person can recognise the stages.

---

## 25. Task E exploration: textures solved, palettes not found

**X4 facts used** (`TeheManX4_Editor` `Level.cs`, read for facts only): tile textures are entry
`0x10000`, one 262,144-byte block. Pages 0–7 are 4 bpp, `0x8000` bytes each, 128 bytes per row.
Pages 8–11 are 8 bpp, built from two halves (`(page & 3) × 0x10000`, then `+0x8000`), and the high
nibble of an 8 bpp pixel selects one of 16 palettes after the tile's own. A tile is 16 × 16 at grid
position (x, y) in its page. Palettes are 16 colours (32 bytes); in X4 they come from a separate
colour archive, and tile palette `n` uses slot `n + 64`.

**Textures: solved.** Member 94's `0x10000` entry, drawn as greyscale pages: pages 0–3 at 4 bpp are
clean tile art (pipes, rocks, cracked slabs, sand dunes, machinery); the second half at 8 bpp is
blocky tile art. Screens drawn with these textures in greyscale show real game art in place: two
rounded doors with lights, pipes, wall machinery, panels, floor edges
(`~/mmx-data/probe/m094_grey_screens.png`).

**Palettes: not found.** First guess (`0x1060d`, 256 palettes): wrong, garish colours. Then an
automatic search: score every DAT entry that could hold palettes (size a multiple of 32) by how
smooth it makes 366 sampled 4 bpp tiles (neighbour colour difference ÷ random-pair difference; lower
is smoother). Plain greyscale scores 0.37; the best entry scores 0.44. No entry, read as a plain list
with offset 0 or 64, gives the real colours. (A first version of the score ranked empty palettes
best at 0.00; dividing by the random-pair difference fixed that.)

**Why it matters.** The art is decodable from the disc alone. The palette source is the one unknown
left for drawing stages in colour, and it is the point where a VRAM dump would give a quick, exact
answer.

---

## 26. Task E decision and the 8 bpp pages

**Decision.** The user chose scope A: build the tile renderer in greyscale now, and search for the
palettes as a separate step afterwards. A VRAM dump stays the fallback.

**8 bpp check (member 94).** Texture cells used per page: page 9 uses 113 cells, all in the right
half (x 8–15); pages 10 and 11 use all 256. Rebuilt from only the cells the screens use:
- page 9 (whole byte as grey) gives smooth, coherent art, so the X4 8 bpp rule holds there;
- pages 10 and 11 give blocky tiles that do not join. This may be the missing palette (in 8 bpp the
  high nibble selects one of 16 palettes, so the raw value need not be smooth), or a different
  layout. They stay "unverified" until a palette confirms them.
- Entry `0x16` (also 256 KB) gives striped noise in every reading, so it is not plain texture data.
- Flags are set on only 280 of 13,410 non-empty cells, so flipping does not explain the noise.

---

## 27. Spec and plan for task E

**Spec** (`docs/superpowers/specs/2026-10-18-tile-render-design.md`, commit `93cd5dd`): a `texture`
module with the exact byte rules for 4 bpp and 8 bpp tiles, a `shade` step that is greyscale now and
can take a palette later without changes to the renderer, and a `tiles` command that writes, per
stage member, every screen at game size, an atlas of the texture cells in use, and a summary. Every
8 bpp page is labelled "unverified" until a palette confirms it.

**Plan** (`docs/superpowers/plans/2026-10-18-tile-render.md`): 3 tasks, 23 new tests. The test
fixture includes an 8 bpp tile and a 4 bpp tile that read the same bytes, so the overlap rule is
tested with exact pixel values from both readings.

---

## 28. Build the tiles command (task E), and the first stage we can name

**What we built.** `texture` (the byte rules for one tile), `shade` (greyscale now, a palette
later), and the `tiles` command. Commits `dd21154`, `d753873`, then the real-data test. 150 tests
(4 skipped real-data tests); the real-data test passes. All 23 members are drawn in about 25 s
(24 MB of PNGs in `~/mmx-data/tiles/`).

**Overlap counts.** 15 of the 23 members have no 4 bpp cell that an 8 bpp tile also reads, so every
cell in them has one reading. The other 8: member 94: 162, 95: 10, 96: 20, 97: 299, 98: 362,
100: 1054, 101: 133, 103: 157. Members 121 and 122 use only 4 bpp pages, so they are a clean test of
the drawing with no unverified pages.

**What the screens show** (checked before reporting):
- **Member 100 shows readable text in the art: "WELCOME TO CENTRAL MUSEUM", and room signs "A-1"
  and "A-2".** Central Museum is one of X6's Maverick stages (we believe Ground Scaravich's; the user
  should confirm). It also shows many flat square blocks; they fit that stage's block rooms, but some
  may be tiles from its 1,054 overlapping cells drawn the wrong way.
- Member 122 (4 bpp only): clean industrial art: girder walkways, wall panels, a machine doorway,
  and a repeated spiral tunnel.

**Why it matters.** This is the first time the decoded data names a real stage. It confirms the whole
chain on real data: disc → archive → member entries → screens → tile info → textures.

**Still to do.** The user looks at the screens and confirms the stage. Palettes, the layout, and the
overlapping cells remain open.

---

## 29. Independent review of task E

**Result.** No critical or important issues. The reviewer confirmed the byte rules and the overlap
mapping by hand, and measured the full real run: 27 s and 229 MB peak memory, including the
256-screen member (a 2084 × 8324 image).

**A design limit found.** A real palette will come from each member's own data. The current shade
step cannot get that data, so the per-member setup (`_process`, `run`) will need a small change in
the palette task. The drawing functions will not.

**Data notes.** On member 94, the overlapping cells come from 8 bpp page 9 and 4 bpp pages 2–3, and
both readings look like art, so one of them is probably wrong for those cells. Greyscale of 8 bpp
values shows palette indices, not brightness, so noise on an 8 bpp page does not prove a wrong
layout.

**Four minor findings, deferred.** The palette-time change above; a README note on 8 bpp greyscale;
a clearer message for a `--member` number that does not exist; tests for 8 bpp pages 8, 10, 11.

---

## 30. The user identifies two stages

**The user's check of the greyscale screens.**
- Member 100: pieces of **Ground Scaravich's** stage (Central Museum). The renders are very clear,
  but the screens are "snapshots" in an unordered jumble that does not follow the stage design.
- Member 122: a few pieces of **Infinity Mijinion's** stage, mostly background and a few platforms.
  A thin line runs across several screens; its meaning is not known (perhaps a laser beam or a
  cable, drawn with many small tiles).

**What it means.** We can now tie disc data to named stages. The "jumble" is the missing layout: each
member stores its unique screens, and a separate table (in X4, a `width × height × 3` grid of screen
numbers in the program file) says where each screen goes.

**Proposed next step.** The layout, searched statically first (X6 program and overlay files), checked
with the edge-match measure from task D, and tried on members 100 and 122 so the user can confirm by
eye. A RAM dump stays the fallback.

---

## 31. Static layout search: negative, so change to a RAM dump

**Method.** Read a byte run as a grid of screen numbers at many widths, then score how well
neighbouring screens' collision edges join (task D measure; random pairs join at about 0.63).

**Results.**
- `layoutSize` (`0x8008ec10`) lies inside the program file, but holds zeros on disc: it is filled in
  at run time.
- All 16 entries of member 94, and member 100's entries: no grid joins clearly better than random.
  Entry `0x18` scored 0.81 horizontally on member 100 at almost any width, but it is shared data
  (byte-identical in members 94 and 122, starting with a table of 32-bit offsets).
- Program file + 59 overlays, runs of bytes below 137 (member 94's screen count): the top candidates
  are sparse 16-wide grids whose numbers rise strictly in reading order (1, 2, 3 … 252). A layout
  would use real screen numbers, out of order and with repeats. These look like slot tables (for
  example sprite cell maps). They scored well only because consecutive numbers mimic storage order.
- The width search found its best fit at 16 or multiples of 8, not at the header byte.

**Conclusion.** The layout is not a plain grid in the places a static search can reach. It may be
compressed, built by code, or in an unknown format.

**Decision proposed.** Use the fallback the user allowed: a RAM dump with a stage loaded. The
reference symbols give `layoutWidth = 0x800cd338`, `layoutHeight = 0x800cd339` and the layer
structures at `bgLayers = 0x800971f8`. A dump gives the real layout of one stage, an oracle for
finding the storage format on disc, and the palettes from video memory.

**Lesson.** A good score is not proof. Each candidate needs a check of what else could cause the
score (here, counting tables that look like stage order).

---

## 32. The RAM-dump route with PCSXR save states

**The user's setup.** PCSXR (PCSX-Reloaded 1.9.94, `/usr/games/pcsxr`) with the built-in HLE BIOS.
An old X6 save state already exists (`~/.pcsxr/sstates/MEGAMAN_X6-SLUS01395.010`, from 2024).

**Check of the method (read-only, on the old save state).** A PCSXR save state is gzip-compressed.
Decompressed, it starts with the header `STv4 PCSXR v1.9.94`. The X6 program text was found at state
offset `0x1A025`, so main RAM (2 MB) starts at offset `0x9025`: after the 32-byte header, a 4-byte
version, a 1-byte HLE flag and a 128 × 96 × 3 screenshot. `layoutWidth`/`layoutHeight` in that RAM
read 4 × 2, so the old state was taken on a small screen, not in a full stage.

**Why it matters.** A save state is a full RAM (and video-memory) dump, made with one menu click and
no extra tools.

**Next action.** The user plays to the start of Commander Yammark's stage and saves states to new
slots. The copies go to `~/mmx-data/dumps/`.

---

## 33. First RAM dump: the intro stage's layout, and a correction

**The dump.** The user loaded the CUE in PCSXR, played to the start of the intro stage, pressed Esc
and saved to slot 1 (`MEGAMAN_X6-SLUS01395.000`, copied to `~/mmx-data/dumps/intro_start.state.gz`).
Note: Esc also wrote PCSXR's automatic state to slot `.010`, which overwrote the user's 2024 state
there. Using F1 in the game window avoids this.

**What RAM showed.**
- `layoutWidth` = 26, `layoutHeight` = 6, `layoutSize` = 156 (= 26 × 6). Consistent.
- Member 94's tile info is in RAM (`0x801b26cc`), and no other member's: **the intro stage is
  member 94**.
- A RAM search for 26 × 6 grids of member 94 screen numbers found two equal copies whose neighbours
  join at 0.998 (horizontal) and 1.000 (vertical); random pairs join at about 0.63. One copy is at
  `0x800fa15e` (the overlay load area), one at `0x80097556` (next to `bgLayers`).

**On disc.** The same bytes occur once: `rock_bin/02.bin` at `0x108FE`. The full layout is 3 layers
of 26 × 6 bytes (`width × height × 3`, as in X4), from `0x107c6`. Layer 0 = main screens (1–72),
layer 1 = more foreground screens (73–110), layer 2 = background (111–136, with 111 repeated). Row 0
of layer 0 holds header-like bytes on disc and almost nothing in RAM; open detail, outside the drawn
stage area.

**Result.** Drawn with member 94's textures in greyscale, the layers join into one continuous intro
stage: wreckage at the bottom left, a long corridor right through the facility, then a climb to the
top right (`~/mmx-data/probe/intro_composite.png`).

**A correction to section 31.** The static search had found this layout (`bin02 @0x107dd`) and we
dismissed it as a "counting table". That reasoning was wrong: screens are stored in the order the
stage first uses them, so a real first layer does rise in reading order. The real cause of the miss
was the search filter, which needed at least 15 vertical joins; a 6-screen-tall map has fewer. The
other dismissed grids (overlays 05, 08, 22, 24, 30) are probably other stages' layouts.

**Lesson.** An oracle (the RAM dump) settles in minutes what blind search argues about for hours. And
a filter that seems harmless can hide the answer.

---

## 34. Layout tool exploration: why each stage needs a save state

**Overlay 02 facts.** `0x800fa026 − 0x107c6 = 0x800E9860`, an overlay load address that `mmx6` lists as
proven, so overlay 02 is loaded whole and the layout is part of it. There is no 32-bit pointer to the
layout in the overlay or the program, and no `(26, 6)` byte pair: the size probably comes from code.
Before the layout: a pointer table, a few values, then 0xff padding; the layout starts 6 bytes before
the first non-0xff byte. After it: repeating 8-byte records (for example `00 03 f0 08 40 04 00 00`)
whose 16-bit fields fall inside the stage's pixel area: probably object placements.

**A blind static search fails.** Search all overlays for grids after 0xff padding, trying every stage
member and every width and height, and keep near-perfect joins. The known intro layout (overlay 02,
member 94, 26 × 6) is not picked. Near-perfect scores come instead from members 99 and 118 at 2–5
rows, because those members have many plain screens whose edges match almost anything, and few joins
are counted. About 25 overlays (02–11, 14–15, 22–32) do show the padding-then-grid pattern.

**Design proposed.** Every save state becomes one verified stage entry: read the size from RAM, find
the loaded member by its tile info, find the layout in RAM with member and size fixed, then find
those bytes on disc. A committed `stages.json` holds the entries (positions and evidence only), and a
`stages` command draws whole stages from the disc data.

---

## 35. The intro stage, section 1, built by hand; save states stay out of the repo (proposed)

**The user's requests.** Build the intro stage's first section now, before the tool. Keep the save
states in named files in the repo.

**Save states and the repo.** A save state holds all 2 MB of RAM (the X6 program and overlays) and
the video memory (textures): copyrighted game data. Proposed instead: named files in
`~/mmx-data/dumps/` (the first: `intro-1_start.state.gz`) and a manifest in the repo (name, SHA-1,
stage, notes). The decision is the user's.

**The build** (a scratch script using the tested `mmxdemake` modules), in
`~/mmx-data/preview/intro-1/`: `layout.json` (neutral: 26 × 6, 3 layers as `[y][x]` grids, source,
evidence), `stage.png` (6656 × 1536, greyscale textures, layers 2 → 1 → 0), and a collision map per
foreground layer.

**Findings.**
- Disc and RAM layouts differ in exactly 19 cells, all in row 0 of layer 0 (the header-like row).
  Every other cell of all 3 layers matches. The 19 cells are set to empty and listed.
- Layer 0 holds the collision: one continuous map with floors, solid areas, steps and the climb at
  the top right. Layer 1 is almost all decoration (art, no collision). For Maverick's `blocks` layer,
  layer 0 is the one that matters.

---

## 36. Spec for the stage layout tool (task F)

**Decision.** The user agreed: save states stay as named files in `~/mmx-data/dumps/`, and the repo
keeps a manifest. `intro-1_start.state.gz` is the first entry and the real-data test.

**Spec** (`docs/superpowers/specs/2026-10-18-stage-layout-design.md`, commit `d87bc1c`):
- `savestate`: open a PCSXR state, check the X6 program text in RAM, return 2 MB of RAM.
- `ramlayout`: read the size, find the loaded member by its tile info, read the layout from the
  working copy at `0x8009741e` and check it with the edge measure, find it in exactly one overlay,
  list the cells where disc and RAM differ.
- `learn`: one save state → one entry in `catalog/stages.json` and `catalog/savestates.json` (names,
  hashes, numbers and positions only).
- `stages`: each entry → `layout.json`, `stage.png` (greyscale textures) and `collision.png`
  (layer 0).

**A simplification from the data.** The working layout in RAM sits right after the layer structures,
so it is probably a fixed array. The tool reads that address and checks the result, instead of
searching all of RAM. The next save state (Yammark) tests the assumption; if it fails, the check
stops the run with a message saying so.

---

## 37. Build the stage layout tool (task F)

**A spec fix before planning** (commit `8f1c567`). Run on the real intro state, the spec's edge check
(layers 0 and 1, row 0 of layer 0 skipped) gave 0.931 / 0.942, below the planned 0.95 threshold: the
tool would have refused the correct layout. Shuffled layouts score at most 0.715. The threshold is now
0.85. Member detection uses the whole tile info entry (only member 94 matches).

**What we built.** `savestate`, `ramlayout`, `learn` and `stages`. Commits `350679f`, `89a886f`,
`e4ebef7`, `7011813`. Test first; 180 tests (5 skipped real-data tests); the real-data test passes on
`intro-1_start.state.gz`.

**The first catalog entry.**

```
Intro, section 1: member 94, 26x6, overlay 2 @0x107c6, edges 0.931/0.942, 19 cells differ (layer 0 row 0)
```

`catalog/savestates.json` and `catalog/stages.json` hold only names, a hash and numbers.

**Check against the hand-built preview.** `stages` wrote `~/mmx-data/stages/m094-intro-section-1/`.
Its `stage.png` is pixel-for-pixel identical to `~/mmx-data/preview/intro-1/stage.png`.

---

## 38. Independent review of task F, and the fix pass

**Result.** No critical issues. The reviewer confirmed that the catalog holds no game data and that
two independent checks protect the fixed-address assumption (the edge check, and a unique exact match
on disc). It found three important issues; two minor ones were raised to important by effect:

| Finding | Effect | Fix |
|---|---|---|
| A corrupt compressed stream raised `zlib.error` | traceback instead of an `error:` line | catch it |
| A hand-edited catalog crashed `learn` | traceback | validate the catalog, exit 2 |
| A hand-edited catalog made `stages` clear the wrong cell | **wrong map, exit 0** | validate every cell, exit 2 |
| `--state` given as a full path | **home path in the committed catalog**; duplicate entries | require a file name |
| A name with no letters or digits, or two names with one folder | the next `stages` run fails | refuse such names |

Each fix started as a failing test (8 new tests; suite 188, 5 skipped). The real-data test and the
real commands gave the same entry and output after the fixes (commit `de90e3d`).

**Deferred (minor).** A README note that `stage.png` draws colour 0 as black; `learn` hashing all ~51
members; small rooms (fewer than 10 joins) being refused.

**Lesson.** The silent wrong map was the serious one: a run that says "done" with wrong output is
worse than a crash.

---

## 39. The user reads the intro collision map; naming collision values

**The user's check of `collision.png`.** A clean rendering of the intro stage. The intro has only one
section, so the catalog entry is renamed "Intro" (folder `m094-intro`). The user's readings: blue =
collision, grey = graphics (background), teal = gates, purple = ladders and ropes, light green =
unknown (perhaps camera control).

**Matched to values** (intro, layer 0, counted per tile cell):

| Colour | Value | Cells | Reading |
|---|---|---|---|
| Blue | 56 | 5,200 | solid (user) |
| Grey | 0 | 2,845 | art, no collision (user) |
| Teal | 59 | 20 | gates (user) |
| Purple | 45 and 32 | 17 and 7 | two values with similar colours: probably ladders and ropes are separate |
| Light green | 57 | 961 | thin lines along floor tops and ceiling undersides: the surface row of solid ground, not camera data |

Smaller groups: 16 and 63 (stair-like, probably slopes), 1–8 (6–7 cells each, open space above),
33 and 34 (1–2 cells).

**A check picture per value.** `~/mmx-data/preview/intro-collision-values/` holds one image per value:
the dimmed stage art with that value's cells in red. Value 57 shows as one-tile lines on surfaces;
value 45 is one short horizontal line in the middle corridor (a bar, rope or thin platform), not a
vertical ladder.

**Next action.** The user names the values from the pictures; the names go into a collision legend
in the catalog for the TMX converter (blocks, ladders, gates).

## 40. Where are the enemies? Finding the object list (task G design)

**The user's question.** The collision map holds only collision. Where do enemy positions come from,
and should that come before the Commander Yammark save state?

**Why collision has no enemies.** X games keep the map and the objects in different lists. The map
says what each tile is; a separate list says "put object K at pixel (x, y)".

**What we found.**
- The bytes after the intro layout in overlay 02 are 8-byte records. Their x and y values are always
  inside the stage (6656 × 1536 pixels).
- Proof that the game uses them: RAM address `0x8007349c` holds the pointer `0x800fa214`. That is
  overlay 02 offset `0x109b4`, the start of these records.
- Record shape: `a`, `b`, `id`, `kind` (one byte each), then `x` and `y` (2 bytes each). The list
  ends with a record whose kind is `0x0f`. The intro has 65 objects.

**Why the disc and not RAM.** The game creates an enemy only when the camera comes near it, so RAM
shows a few live enemies. The disc list has every placement. We use RAM only to find the list.

**Order.** Intro objects first (the data is already here, and the user knows the stage). The user
plays to Commander Yammark at the same time; that save state then tests both the layout and the
object pointer on a second stage.

**Design (task G).** `learn` follows the pointer, finds the same bytes on the disc, and stores the
location in the catalog. `stages` writes `objects.json` and `objects.png`: the dimmed stage with a
coloured square (by kind) and a number for each object. The user names the numbers, like the
collision check. Spec: `docs/superpowers/specs/2026-10-19-object-placement-design.md`.

## 41. Task G: objects on the intro map

**Two surprises from the real data** (the small fake test stage could not show them):

1. **The game writes into its own list.** RAM and disc differ in two records, only in the first
   byte (`a`): `00` became `81` and `01`. This is the game marking objects (for example "already
   spawned"), like the changed row 0 of the layout. The disc search now ignores that byte in each
   record; it still needs exactly one hit.
2. **57 objects, not 65.** The design's first scan stopped when values left the stage, not at the
   end record. The real list ends after 57 records. After it comes a second, separate list of 7
   records at (0, 0) with its own end record and its own RAM pointer (`0x80073554`). That second
   list goes to `docs/TODO.md`.

**Result.**

```
Intro: member 94, 26x6, overlay 2 @0x107c6, edges 0.931/0.942, 19 cells differ (layer 0 row 0), 57 objects @0x109b4
```

`~/mmx-data/stages/m094-intro/objects.png` shows the dimmed intro with 57 numbered squares;
`summary.txt` lists each number with its kind, id, position and screen. Most markers are along the
stage path; some are in the black area above the art (possibly triggers or camera points).

**Independent review.** One important finding: with only one save state we cannot tell whether
`0x8007349c` is the start of the list or a read position that moves as the camera scrolls. If it
moves, a save state from later in a stage would point into the middle of the list, and the tool
would learn a list with the first objects missing. Fix: the disc bytes just before a hit must be
8 zero bytes or an end record; anything else is refused ("the pointer may not be the list start").
Five minor findings went to `docs/TODO.md`.

**Next action.** The user opens `objects.png` and says what the numbered objects are. That names
the kinds and ids, and checks that `x` and `y` are stage pixels.

## 42. One layered file: stage art, collision and objects

**The user's feedback.** `objects.png` drew the markers over `stage.png`, and `stage.png` has
"gobbledygook": random bits of background around ladders and background art. (Causes, all known
open items: no palettes yet, transparent pixels drawn black, and overlapping texture cells.) The
collision view is cleaner, so the user wanted the objects over collision too, and asked whether the
objects could be one layer over both pictures.

**The size problem.** The two pictures were different sizes: `stage.png` is 1 px per game pixel
(6656 × 1536 for the intro); `collision.png` was 4 px per tile, a quarter of that. A shared layer
needs the same size, so `collision.png` is now 16 px per tile. It is still a small file (63 KB),
because it is flat colour.

**The choice: OpenRaster (`.ora`).** An open, simple image format with real layers: a zip that
holds one PNG per layer, a short `stack.xml` that lists the layers in order, a combined picture
and a thumbnail. GIMP (3.2.6, Flatpak) opens it with its own plug-in. We tested a tiny two-layer
file first; the user confirmed that the layers open and switch on and off. Python's standard
library (`zipfile`) can write it, so the tools still need no outside packages.

**Result.** Each stage folder now has `stage.ora` with three layers: `objects` (transparent except
the markers), `collision` (50 % opacity) and `stage`. `objects.png` is gone. Intro: 1.6 MB, drawn
in under 4 seconds.

**Next action.** The user opens `~/mmx-data/stages/m094-intro/stage.ora` in GIMP and names the
numbered objects.

## 43. The art in three layers

**The user's check.** "It looks awesome": stage art, collision and object positions in one view.
Some "goop" remains in two places: near the start, and in one spot that looks like parallax art.

**Why.** The layout has three layers: 0 main (with collision), 1 decoration, 2 background. In the
game the background scrolls more slowly than the camera (parallax), so the game never shows it
screen-aligned with the other layers. Our flat `stage.png` pins all three to one grid, so where the
background shows through it looks cut up.

**Change.** `stage.ora` now has one art layer for each layout layer (`art 0 main`, `art 1
decoration`, `art 2 background`), transparent where that layer has no tile. `stage.png` and the
combined picture in the `.ora` are unchanged (checked byte for byte). A tile's colour-0 pixels are
still drawn black: whether colour 0 is transparent is not proven yet.

**Confirmation.** The intro's background layer has art in exactly two places: around the start and
one block about a quarter of the way along. These are the user's two goop spots. Hiding that layer
removes them.

## 44. Palette probe: video memory in the save state

A probe (throwaway scripts, nothing in the repo) to see whether palettes can come from the save
state's copy of video memory (VRAM: 1024 × 512 pixels of 16 bits).

**Found.**
- **VRAM location.** Decompressed save state offset `0x29b749`, right after the graphics chip's
  state header (version 1, then status and 256 control words). The only candidate that fits.
- **The intro textures are in VRAM.** All 8 texture pages of member 94 match VRAM byte for byte,
  starting at VRAM x = 320, y = 256, with rows exactly one VRAM row (2048 bytes) apart.
- **The "overlapping 4 bpp / 8 bpp cells" are not an error.** The 8 bpp pages are the same VRAM
  bytes as the 4 bpp pages, read 8 bits per pixel. The game uses one VRAM area both ways.
- **Palettes.** VRAM rows y = 480–511 (x = 0–319) hold rows of 16-colour palettes. The first colour
  of each is `0x0000`, which the PS1 draws transparent. So colour 0 really is transparent.
- **The screen image area is black.** The emulator draws with the PC graphics card, so the picture
  on screen is not copied back to VRAM. Textures and palettes are there anyway.

**Not solved: which palette a tile uses.** A first rule (palette n at x = (n mod 16) × 16,
y = 480 + n ÷ 16) gives crisp, partly correct colour: the start area and the floors look right,
but large areas come out flat green or yellow. Drawing only the 4 bpp tiles in colour shows the same
flat areas, so the rule is wrong for many 4 bpp palettes too, not only for 8 bpp.

**Recommendation.** Get the rule from the game's own drawing commands. At the moment of the save
state, RAM holds the list of commands the game sent to the graphics chip; each tile command names
its texture page and its palette position. Matching those against the tile info gives the real
rule.

## 45. Palette probe 2: the game's own drawing commands

**Finding the commands.** RAM holds about 1,300 "draw a 16 × 16 sprite" commands (`0x7C`–`0x7F`)
whose palette field points into the palette area. Example: `7dff1010 00a50086 78453050` = draw at
screen (134, 165), texture cell u = 0x50, v = 0x30, palette at VRAM (80, 481). The commands do not
name a texture page, so a direct match to a tile is not possible.

**Finding the camera by voting.** Each sprite is a stage tile seen through the camera. For every
sprite and every stage tile with the same texture cell, the difference of positions is a candidate
camera position. The real one wins clearly:

| Layout layer | Camera (x, y) | Votes | Next best |
|---|---|---|---|
| 0 main | (0, 1200) | 294 | 48 |
| 1 decoration | (0, 1288) | 192 | 108 |
| 2 background | (0, 1132) | 588 | 60 |

The three layers have different cameras: this measures the parallax the user saw.

**The rule.** With the camera known, 1,074 of 1,264 sprites land on a stage tile with the same
texture cell (the rest are probably X, enemies and the HUD). All 20 palette numbers seen on screen
give one rule, with no exceptions: palette n is at VRAM **x = (n mod 16) × 16, y = 486 + n ÷ 16**.
The first guess (section 44) had x right and the start row wrong (480). 8 bpp tiles use the same
rule; they read 256 colours from that position.

**Result.** The intro in colour: the pink desert sunset with wrecked ships, sand, machinery blocks,
the rocky outcrop, the blue-grey steel interior. A few patches are still wrong (for example a red
striped area); probably palettes that the game loads or changes later in the stage. Pictures:
`~/mmx-data/preview/palette-probe/`.

**Open for the design.** Whether 486 is fixed or differs per stage (the vote can find it again for
every save state); palettes that change during play; where `learn` keeps the palettes (game data:
`~/mmx-data`, not the repo).

## 46. Task H: the stages in colour

**One surprise from the real data.** The first real run refused: "no camera for layout layer 0".
The tool reads every RAM word that looks like a 16 × 16 sprite command, and the intro RAM holds
1,007 copies of one empty record (screen (0, 0), cell (0, 0), palette (0, 0)), probably unused
slots in a command table. Every copy voted for every tile that uses cell (0, 0), and this buried the
real camera. (The probe had not seen this, because it kept only commands whose palette was in the
palette rows; the design dropped that filter so that it would not assume where palettes live.)
Fix: the camera vote counts each distinct command once. A test with 200 empty copies reproduces the
problem and now passes.

**Result.**

```
Intro: member 94, 26x6, overlay 2 @0x107c6, edges 0.931/0.942, 19 cells differ (layer 0 row 0), 57 objects @0x109b4, palettes @(0, 486) from 537 tiles
```

The catalog stores the palette start (0, 486), the 537 matched tiles (agreement 1.0) and the three
cameras: (0, 1200) main, (0, 1288) decoration, (0, 1132) background. `stages` now draws
`stage.png` and the `stage.ora` art layers in colour, with colour 0 transparent; the picture
matches the probe.

**Independent review.** One important finding: the rule step still counted every command, so the
empty records could, on an unlucky stage, match a real tile and push a wrong rule past the 95 %
agreement check. A test with 4,000 such records reproduced it: the tool accepted a wrong rule,
(0, 0). Fix: the rule step also counts each distinct command once. This also corrects the evidence
count: the game draws every tile twice (one copy per screen buffer), so the intro has 537 distinct
matched tiles, not 1,074. The rule is unchanged. Minor findings went to `docs/TODO.md`.

**Next action.** The user opens `~/mmx-data/stages/m094-intro/stage.ora` and checks the colours
against the game.

## 47. Naming the intro objects by playing the stage

**Setup.** PCSXR ignored every controller: its input plugin had no device selected
(`DevNum=-1`, keyboard only), and the device list showed the controller as "1: null" (the old
plugin cannot read names from the newer SDL2; cosmetic). The user then got a PS3 controller
working and played the intro slowly, with `stage.ora` and a left-to-right walkthrough list
(`~/mmx-data/preview/intro-objects-walkthrough.txt`) open.

**What the play-through showed.** The second record byte (`kind`) is a category, and the first
non-flag byte (`b`) is the type inside it:

- kind 0 = enemies: b 1 legless green robot (6 of 6), b 2 wheel (4 of 4), b 0 drill (9 of 9).
- kind 4 = stage objects: b 2 Z-sabre block (5 of 5), b 8 boss gate (2 of 2).
- kind 1 = spawners: two propeller-flyer spawners; their `id` matches the side the flyer
  enters from (1 = left, 0 = right).
- kind 3 = stage logic: the user saw the camera behaviour change exactly at objects 3/9 and 5/10.
  X6 has no rooms; camera limits change at points instead.
- kind 5 = events: an Alia hint ("!") and an NPC dialogue, by position.

**`id` is a per-object parameter.** Robot 27 is the only robot with id 0, and the user (from a
gameplay video) saw that it is the only one that never turns to face X. A short search of the
reference repos found no notes on it; reading the robot's code is left as a curiosity.

**Marker heights.** The gap between a ground enemy's marker and the floor varies (0 to 88 px), so
it is not an offset: the game most likely drops ground enemies onto the floor.

**Recorded** in `docs/objects-legend.md`: every intro object with its colour, fields, reading and
status (29 confirmed, 19 plausible, 9 unknown), plus the open questions.

## 48. Robot 27 explained, and the second stage

**Robot 27, from the game's own code.** An agent disassembled the robot's code in overlay 02 (and
compared the spawn code with the X4 decompilation). `id` 0 makes this robot dormant: it faces X
once when it spawns, then skips both "turn to face X" and "start shooting". At difficulty 2 or
higher the game forces `id` to 1, so the robot acts normally there. The same code showed that the
low digit of `kind` (0–2) is a **minimum difficulty**: kind 1 objects appear from difficulty 1 up
and kind 2 only from difficulty 2 up, and kinds 0–2 share one type list. That turns most of the
"unknown" blue (kind 2) markers into extra wheels and flyers for harder play, which explains why
nothing happened at them. Status: plausible until a play-through on a harder difficulty.

**Commander Yammark.** The user took `yammark-1_start.state.gz` right after X's spawn-in. The
first `learn` run refused, correctly: "object list found 0 times". The RAM word `0x8007349c`
still held the intro's address, and that address now held Yammark's program code. The word is
entry 0 of a fixed table with one entry per stage, 8 bytes apart; the intro worked only because
it is stage 0. Yammark is entry 1 (`0x800734a4`): 108 objects in overlay 03 at `0xcb08`. Fix:
`learn` tries every entry and needs exactly one list in the same overlay as the layout; the
catalog records `table_entry`.

Everything else held on the second stage without change:

```
Commander Yammark, section 1: member 95, 26x9, overlay 3 @0xc82e, edges 0.923/0.956, 18 cells differ (layer 0 row 0), 108 objects @0xcb08, palettes @(0, 486) from 264 tiles
```

The fixed RAM layout address, the textures at VRAM (320, 256) and the palette start (0, 486) are
the same as in the intro. The stage draws in colour (purple jungle, teal underwater caves). Open:
a thin strip along the top looks like the bottom-right of the stage.

**The wrap.** The user confirmed in GIMP that the stage wraps vertically. Only layers 1 and 2 use
it: their row 0 holds the screens one row below the main layer's bottom row. Row 0 of layer 0 is a
header, not map data (`ff ff ff ff ff ff 80 …` on disc in both stages, overwritten at run time),
which also explains the "differing cells". Plausible reason for the wrap: a 256-cell limit per
layer (26 × 10 = 260).

## 49. The first enemy, reverse-engineered

**The user's wish.** Three artifacts for the intro's legless robot: a behaviour sheet and pseudo
code (for Maverick), and a plausible reconstruction of the original C, with file layout and
includes (for curiosity).

**Where they live.** The pseudo code and the reconstruction are derived from the game's code, so
they stay in `~/mmx-data/re/junkroid/` with the disassembly listings. Only the behaviour
sheet, our own description with numbers, goes into the repo: `docs/enemies/junkroid.md`.
(Each enemy gets a specific folder name, so that "robot" is not confused with other enemies.)

**What the robot does** (read from the code by an agent, with addresses):
- 12 HP, 4 contact damage; it falls with gravity if spawned in the air.
- Active robots (`id` 1) face X whenever they are in the idle pose, and fire one straight shot
  every 153 frames (≈ 2.55 s) while X is within 112 px (7 blocks) horizontally, at any height.
- The shot: 2 px/frame (7.5 blocks/s), 8 × 8 px, 1 damage, bursts on walls; it can be shot down,
  except on Hard.
- No invincibility frames: a hit flashes the robot for one frame. Death: an explosion, 4 debris
  pieces, and up to two 25 % item-drop rolls (none on Hard).
- Difficulty scales damage both ways (Easy: X takes ×0.75, enemies take ×1.25; Hard: X takes
  ×1.25).

**The reconstruction** (`reconstructed/`): `src/st02/m01_mecha_s.c` with shared headers
(`object.h`, `game.h`, `sound.h`) and Psy-Q includes. The constants and control flow come from
the binary; names, comments and file layout are invented, and a `README.md` says which is which.

**A find in the reference repos.** `mmx-reference/mmx6` holds a decompilation with C for these
overlay routines (for example `func_800EAC8C.c`). It was used only to check facts. It is
AGPL-3.0, so the same rule applies: read it, never copy it. Its state (snapshot of 2026-10-16):
about 3,480 machine-generated C drafts (m2c output: raw offsets, no names), while the build still
uses about 2,450 assembly placeholders; its docs cover disc formats but not stage data, objects
or palettes. So it complements this project (exact code) rather than replacing it (data, meaning,
behaviour).

---

## 50. Three more intro enemies, and their official names

**The user's wish.** The same three artifacts (behaviour sheet, pseudo code, reconstructed C) for
the other intro enemies: the small jet, the drills that fall and rise, and the intro boss.

**Names.** During the work the user gave the official names, from the Mega Man Knowledge Base list
of X6 enemies: Junkroid (the green half-robot), Hover Gunner (the small jet), Meta Wheel F (the
pink wheel), Meta Dridler (the drill) and D-1000 (the boss). Files, folders and the legend now use
them, and `docs/prior-work.md` credits the wiki for the names.

**Hover Gunner** (`docs/enemies/hover-gunner.md`): 5 HP, ignores terrain. It flies in at
4 px/frame, stops when X is 4–7 blocks away (random per spawn), fires two bursts of 3 aimed shots
(16 directions, 2 px/frame), then a fan of 9, then flies up and away. `id` 0 enters from the right
at once; `id` 1 waits until X has passed it by 11 blocks. This also answered the legend's "object
24": it is a jet with `id` 0, so it is never seen at its marker. One false lead on the way: a
pointer in scratchpad RAM looked like a shared "squadron" block, but it is only "the current
object", set by the object loop.

**Meta Dridler** (`docs/enemies/meta-dridler.md`): indestructible, 8 contact damage. Waits 1.5 s,
drops up to 8 blocks with gravity, drills 1 s, rises at 0.5 px/frame. Drills with `id` 1–3 copy a
4 × 3-cell rock block from a template area under them when they land on a floor. The first reading
("they break a hole") was wrong; checking the collision of the source and target cells in our own
layout data corrected it. It is still open whether they ever land in play.

**D-1000** (`docs/enemies/d-1000.md`): 32 HP. Lemons and half charges bounce off; full charges do
5, the saber 2–3. A second object, the core, sits at the far side of the room and shoots at X; a
full charge into the core stuns the boss and takes 8 HP, so 4 core hits win. Moves: walk into a
crouching body slam, a wall ram that dents the wall (map cells copied from templates), a summon
(the core fires 4 aimed shots), and below half HP a lobbed bomb that becomes a 50-frame fire
pillar.

**Where things live.** As before: the behaviour sheets in `docs/enemies/`; pseudo code,
reconstructions and disassembly listings in `~/mmx-data/re/<enemy>/`; the small disassembly tools,
now shared, in `~/mmx-data/re/tools/`.

---

## 51. One data folder per game

The user asked to make the data folder game-specific, so that the tools can later serve X4 and X5
too. All X6 data moved from `~/mmx-data/` to `~/mmx-data/x6/` (a rename on the same disk; nothing
inside the data stores its own path). Changed: the default paths in `mmxdemake/learn.py` (also used
by `stages`), the scratch folder default of the real-data tests, the disassembly tool in
[`~/mmx-data/x6/re/tools/`](../../work/tools/), the README, the TODO and the notes. Older entries in this log and in the
specs and plans keep the old path, because they record what was true then. Checked: the normal
suite (256 tests, 6 skipped as before), the 6 real-data tests with the paths set to the new folder
(all pass, including the full extraction), and `stages` with its default paths.

---

## 52. Probe: enemy sprites (Junkroid)

**Question.** Can we draw Junkroid's sprites from the save state we have?

**What the code and data say** (all confirmed by reading the code and matching bytes):
- A sprite number points into the object's sprite data (`+0x3C`): a list of `{u16 count,
  u16 offset}`, then `count` pieces of 4 bytes `{flags, tile, s8 dx, s8 dy}`. Each piece is one
  16 × 16 tile at 4 bpp, drawn with the 16 × 16 sprite command (the same commands the palette step
  reads). The flag bits give 2 more tile bits and a palette offset (0–3).
- Tile number + the object's texture field (`+0x40`) = a cell in VRAM: `>> 8` is the texture page,
  the low byte is the 16 × 16 cell in it.
- For stage enemies the spawner sets these from `i`, the type's place in the stage's list of loaded
  types: sprite data = bank base + `bank[i]`; texture = `table[i] >> 7`; palette = VRAM row
  `482 + i // 4`, column `64 × (i % 4)`, 4 palettes per type.
- On disc (intro, DAT member 94): entry 2 (type `0xA`) is the whole sprite bank (loaded to
  `0x80154DFC`; Junkroid at `+0x1438`, 11 sprites, the body has 22 tiles); entry 1 (type `0xC`) is
  the texture table (byte offsets, 128 bytes per cell).

**Result.** The sprite records decode cleanly, but the picture is empty: VRAM pages 0–4, where the
enemy tiles go, are empty in both start save states. The textures are probably unpacked into VRAM
later. Entries 4 and 6 of member 94 (types `0x20505`, `0x20202`) look compressed and are the likely
source.

**Next.** A save state with a Junkroid on screen should make the existing probe code draw it.
Reading the textures from disc needs the decompressor first.

---

## 53. Enemy textures from disc: the decompressor, and an open mismatch

**Goal.** Read enemy textures from the disc instead of from save states (the user's choice: one
save state per enemy does not scale).

**Found.**
- The game's decompressor (`0x80018BE0`) is a word LZ: a 16-bit flag word, then 16 items; a clear
  bit is a literal word; a set bit is a copy (top 5 bits = length and low 11 bits = distance in
  words, or a long form with the length in the next word; distance 0 = a run of zero words;
  length and distance 0 = end). We re-implemented it from the binary
  (`~/mmx-data/x6/probe/sprites/wordlz.py`). It matches the X4 editor's texture decoder, so the
  format is shared across the engine family.
- Its only caller (`0x80017B04`) streams one sprite frame at a time to VRAM (320, 0) for the
  player (X and Zero), from a per-object table of `(tile count << 20) | offset`. This is why VRAM
  page 5 holds X's current frame.
- With the format from section 52, X's current sprite draws correctly from the save state. So the
  sprite format and the assembler are right.
- VRAM pages 5–7 hold the art of the intro enemies (Junkroid, Meta Wheel F, Meta Dridler, Hover
  Gunner), each block where `texture table >> 7` + `0x500` points.

**Open.**
- The enemy sprite definitions do not line up with those VRAM cells: the right art, in the wrong
  arrangement, for every enemy we tried. Tile offsets, piece order and narrower layouts did not fix
  it. The spawner sets the texture field without `+0x500`, which points at pages 1–4, empty at the
  stage start. So the cells the enemies really use are probably loaded later, in another
  arrangement, by a loader we have not found.
- The disc source of the enemy textures: the cells are not stored raw anywhere, and member entries
  4 and 6 (types `0x20505`, `0x20202`) do not decode with this LZ from any start offset.

## 54. Enemy sprites solved: the drawer remaps the tiles

**Input.** One calibration save state from the user: the intro, with the first Junkroid on screen
(`~/mmx-data/x6/dumps/intro-2_junkroid.state.gz`).

**Found.**
- VRAM pages 1–4 were still empty with the Junkroid on screen. They are inside the screen buffers
  (x 0–319), so the enemy could not draw from there. So the texture field is not a direct page.
- The drawer in sections 52 and 53 (`0x80022F1C`) has no callers. The real object drawer is
  `0x800232D4`. Before it builds the GPU packet, it remaps the tile number `t` (tile + texture
  field) by bits `0x60` of the object's first byte:

  | byte 0 & `0x60` | remap | VRAM area |
  |---|---|---|
  | `0x60` | `((t >> 4) << 8) + (t & 15) + 0x600` | row 0 of pages 6, 7, … |
  | `0x40` | `q = t / 160`; `(q << 8) + (t − 160q) + 0x610` | rows 1–10 of pages 6, 7, … |
  | `0x20` | `q = t / 80`; `(q << 8) + (t − 80q) + 0x5B0` | rows 11–15 of pages 5, 6, … |
  | `0` | none | linear (the player, at page 5) |

- The stage spawner writes `0x41` to byte 0 of every main object that it makes from a stage record
  (`0x8002A150`). So all placed enemies use the `0x40` layout. Many effect objects are made with
  `0x21` (the `0x20` layout).
- With the remap, the Junkroid's 11 sprites draw correctly. The same numbers, applied to the old
  start state with no Junkroid alive, give the same result. So no per-enemy save state is needed.
- All 9 sprite banks of the intro area draw cleanly from the start state alone (bank 2 Meta
  Dridler, 3 Junkroid, 4 Meta Wheel F, 5 Hover Gunner; also an item, a wall block, rocks and others).
  D-1000 is not in these banks; it is probably loaded with the boss area.
- Probe: `~/mmx-data/x6/probe/sprites/draw2.py`; sheet `banks_all.png`.

**Correction to section 53.** The art in pages 5–7 was in the right place. The mismatch came from
the missing remap, not from a second loader.

**Still open.** The disc source of those textures. For now, one start save state per stage area is
enough to draw every enemy in it.

## 55. Sprite and animation exporter (task I)

**Goal.** Export every animated object type of a stage area as Maverick-style animation strips,
without knowing the names in advance (spec and plan of 2026-10-20; native execution).

**How it works.**
- Animation format: each animation is a list of 4-byte frames (ticks, event flag, step, sprite). A
  step of 0 holds the frame; a negative step loops back.
- The animation table of each type comes from three sources: a live object in the save state
  (certain), a search of the code for the instruction that stores the table (stage code and main
  program), or an address in `catalog/enemies.json` (last resort).
- Each export is named by a fingerprint of the raw game data (pieces, texture cells, colours,
  timings), so the same object in two stages gets one folder. Names are added later.
- Each run replaces only what its save state contains; the rest of the output stays.

**Results.**

| state | banks drawn | exports | skipped types |
|---|---|---|---|
| intro start | 9 | 4: Meta Dridler, Junkroid, Meta Wheel F, Hover Gunner | 55 and 48 (D-1000): no texture loaded |
| intro, Junkroid on screen | 9 | the same 4 IDs; the Junkroid also from its live object | the same |
| Yammark start | 9 | 7 (types 3, 4, 23, 28, 37, and 2 tables of the shared type 15) | 7: no table found; 0 and 49: no texture loaded |

- The live object and the code search gave the same Junkroid table and the same ID.
- Ruling during execution: on Yammark, three tables ran on into pointers to program code, so their
  later "frames" were code bytes. A found table now ends at its first animation that does not fit
  the bank; the earlier animations are kept and a line names the cut.
- Yammark type 15 has its code in the main program, so other stages probably share it. Its art
  draws correctly, but its tables use sprites up to 175 while its bank has 43. Open.
- The four intro IDs got their official names in `catalog/enemies.json`.

## 56. The intro boss room: D-1000, draw order, overwritten banks

**Input.** A save state in the intro boss room (`intro-3_boss.state.gz`), with D-1000 falling in.

**Found.**
- The room is still area 0, and bank 15 (type 48) still has no texture in the table. D-1000 sets
  its own sprite source: its two live objects (body and core) hold sprite data, texture field
  `0x1E0` and CLUT `0x794C` in their slots. Their animation table is `0x800FB138`, as the code
  reading said.
- The boss's art is loaded over the cells of banks 5–8 (Hover Gunner and three small banks). A run
  that trusted those banks gave a damaged Hover Gunner with a new ID.
- **Correction:** the last piece of a sprite is on top, not the first. D-1000's overlapping pieces
  showed flat blocks over its head in the old order; the drawer code chains the pieces in list order.
  The Junkroid, used for the first check, barely overlaps, so both orders looked plausible there.
  The content IDs do not change (they hash the raw data); every PNG was drawn again.

**Changes (test-first).**
- Draw order: last piece on top.
- A type whose bank has no texture uses the sprite source of each live object of that type.
- A bank whose cells a live object with another texture field also uses is skipped as
  `cells overwritten by type <t>`.

**Result.** The boss-room run gives `d-1000` (the body set, 16 sprites, 10 animations) and
`d-1000-parts` (49 sprites: body poses, the core, the bomb and the explosions; 24 animations), and
skips banks 5–8. The damaged Hover Gunner folder from the first run was deleted.

**Yammark boss room** (`yammark-2_boss.state.gz`). The same pattern, with no code change: type 49
has no bank texture, and its live object holds a 113-sprite source. All 23 animations of its table
fit (body poses, wings, shots and effects), exported as `commander-yammark`. Its art overwrites
banks 4–8, which were skipped. Type 7 still has no table (its bank, 24 sprites, is drawn).

**Caterpirade** (`yammark-3_caterpirade.state.gz`, saved with Esc, which writes slot `.010`). The user
named type 7 as the Caterpirade. The main slots were empty; an object at `0x800C1220`, outside the
main array, points at bank 3's sprite data and holds table `0x800F73D4`. The search had found that
store but could not link it to a type. The table went into the catalog (`tables`, the last resort),
and the export (`caterpirade`, 6 animations: crawl, turn, curl, roll, uncurl) is correct.

## 57. The stage-object array

**Question.** Why was the Caterpirade outside the main object slots, and where is that array?

**Found** (from the update loop at `0x800205A0`).
- The array starts at `0x800C1220`: 80 slots of `0xA4` bytes. The loop keeps the current object at
  scratchpad `+0x60` and calls the function at entry (byte 1) of the table at scratchpad `+0x94`
  (intro `0x800FA794`, Yammark `0x800F6AF0`, 32 entries each).
- These are the stage objects (placement records of kind 4). In the intro boss room its only live
  objects are two of type 8, the boss gates of the legend. The Caterpirade is its type 1.
- The slot layout matches the main objects: table `+0x30`, sprite data `+0x3C`, texture `+0x40`,
  CLUT `+0x42`.
- A third pair of loops (current object at scratchpad `+0x64`) walks the misc objects; no state has a
  live one yet, so they are not added.

**Changes (test-first).** The live-object scan reads both classes. A stage object is exported from its
own sprite source, and its sources say `"class": "stage"`. The source merge key includes the class.
Also fixed: a re-run of a save state now replaces that state's old sources (it kept stale ones).

**Result.** The Caterpirade comes out with no catalog entry and the same ID as before. A new export,
`boss-gate` (shared code; the same ID in both boss rooms), shows the lock and the doors opening.

## 58. The RE kit, and its pilot on Meta Wheel F

**Goal.** Make the three RE documents per object faster, and doable by a person without an AI.

**What was built.**
- `mmxdemake.mips`: the disassembler, moved into the repo with tests.
- `mmxdemake.codewalk`: function ends, state tables (cut apart where the next one starts), the
  functions an object can reach, neighbour entries of the visual, misc and stage-object tables
  (after the object's code), constants, and the object fields each function reads and writes.
- `mmxdemake.rekit`: one command per object; four text files outside the repo.
- `catalog/helpers.json`: names and notes for 50 engine functions, 25 globals and 45 fields.
- `docs/re-guide.md`: the procedure, the templates, the checks, the short sheet for effects.

**Pilot.** Meta Wheel F (`docs/enemies/meta-wheel-f.md`; notes, pseudo code and C in
[`~/mmx-data/x6/re/meta-wheel-f/`](../../work/meta-wheel-f/)). The kit found its 18 functions, 3 state tables, its stud overlay
(misc type 2) and 17 engine helpers, all named. Findings: it rolls at up to 0.75 px/frame, stops at
walls or other wheels for about 111 updates, then rolls back; only the sabre hurts it; it dies when
squeezed between floor and ceiling, which another wheel landing on it can cause.

**What the kit saved, what stayed manual.** Finding code, tables, helper names and field names was
automatic. Reading the logic, the switch tables inside a function, and functions whose first argument
is not the object stayed manual. Two kit fixes came from the real data (test first): adjacent state
tables and a neighbour from the previous object; and a `lui` up to 12 instructions before its use.

## 59. Batch 1: six enemies by helper agents

**How.** Six helper agents ran in parallel, one per enemy, each with the same brief: follow
`docs/re-guide.md`, use Meta Wheel F as the quality reference, write only that enemy's files, and
report new names and kit problems instead of editing shared files.

**Results.** Behaviour sheets in `docs/enemies/`, notes, pseudo code and reconstructed C in
`~/mmx-data/x6/re/<name>/`:

| enemy | stage | in short |
|---|---|---|
| Killer Moth | Yammark | hovers; homes to 48 px above X, circles and drops 8 powder shots |
| Worm Spider | Yammark | floor or ceiling nest; lunges and fires aimed energy balls; a water-jumper variant |
| Nightmare Insect | Yammark | lays hatchlings, throws a blade wave (and a boomerang as Zero); revives as a husk |
| Batton Bone B81 | Turtloid | flies to 80 px above X and drops a bomb; five flight variants by `id` |
| Sea Attacker ST | Turtloid | shooter (bubbles) or diver (curls up and dashes) by `id` |
| Monbando | Turtloid | still turret; guards against shots; ground waves, a missile (X) or a punch (Zero) |

**Merged afterwards.**
- 5 functions, 17 globals and 7 fields into `catalog/helpers.json`.
- The damage tables, settled from the data. Two agents found that my earlier note on `0x8007429C`
  ("deflects everything") was wrong: it is the one-hit table. The deflect-all table is `0x8007433C`.
- The old Junkroid sheet had weapon id 23 as "no effect"; the table gives 1 damage. Corrected.

**Kit fixes from the reports (test first).** Neighbour entries must be real function starts outside
the walked functions (three agents met false ones); fields written through the current-object pointer
(`0x1F80004C`, `0x60`, `0x64`) are now counted; an address next to a known global is named as an
offset (`mega+0x14`).

## 60. One engine page, one page per stage, shorter sheets

**Why.** About half of each behaviour sheet repeated engine rules: units, the spawn band, the
damage modifiers, the player's damage scaling, the drop roll. The Junkroid sheet had become the
engine reference by accident ("weapon ids as in the Junkroid sheet"). A stale copy of the Junkroid
sheet (with the old generic name) was also left in [`~/mmx-data/x6/re/junkroid/`](../../work/junkroid/); it is deleted.

**What.**
- `docs/engine.md` (new): conventions and confidence tags, the object classes, spawning and
  despawning, the standard main-object update, animations, damage to objects (the weapon ids and all
  seven damage tables in full, read from the executable), damage to the player, death and the 24
  drop tables, shots, sounds, screen effects.
- `docs/stages/` (new): `intro.md` (the object list and the D-1000 boss room, moved from
  `objects-legend.md` and `d-1000.md`), `yammark.md` (the section 1 records from `objects.json`),
  `turtloid.md` (the Sea Attacker ST records; the stage map is not extracted yet).
- The 11 sheets keep only what is particular to the object: 2,494 lines before, 1,920 after.
- `docs/re-guide.md`: the sheet template now says what goes where.

**Found on the way.**
- "In every table, ids 64–78 are deflected and id 79 has no effect" was false. `dmg_tbl_one_hit` does
  127 for all 80 ids, and `dmg_tbl_minor`, `dmg_tbl_sabre_only` and the D-1000 table deflect id 79.
- The Meta Wheel F sheet said that all intro wheels have `id` 1. Record 38 (Hard only) has `id` 0, so
  it rolls right.
- The facing rule for an equal x is not the same in all objects (Junkroid right, Killer Moth left).
- The Yammark records show main type 7 (the Caterpirade sprite bank) 10 times and two item objects
  (main type 47). The live Caterpirade is stage-object type 1, but no kind-4 record has type 1.

## 61. Batch 2: stage objects and shared systems

**How.** Before the start I ran the kit on all nine objects. One run failed: `--name` with
`--state` and `--type` still looked for a sprite export. Fixed with a test first (`577fb3d`). Then
seven helper agents, one brief file for all, the new short-sheet template, and the rule "write only
your own files; report shared facts".

**Results.**

| object | what it is |
|---|---|
| Caterpirade | main type 7 only spawns it into a stage-object slot (type 1). Crawls, curls into a ball when hit, its back is a solid platform |
| boss gate | a solid block; on the trigger it clears the enemies, locks the player, opens, walks the player through, sets new camera limits (51 cases by `id`), closes |
| teleporter crystal | touch trigger; freeze, flicker, fade; then an area change (`id` 0/1) or a fixed destination (`id` 2–4) |
| Weather Analyze | a machine with 4 sensors, a display and a barrier; while it lives, a stage flag makes the acid rain (1 HP per 180 frames, no scaling) |
| Nightmare Virus | floating enemy in the main program; `id` selects normal, snatcher, soul; a killed virus becomes a Nightmare Soul and comes back if it is not collected |
| Rescue Reploid | stage-object type 4 (the unknown Yammark objects): life, HP and sub tank on touch; can be infected |
| Light capsule | stage-object type 26: gives an armour part bit |
| rain effect | wind and rain in one zone; a head wind slows the player to 3/4 speed |
| Nightmare shadows | not a pure effect: they dash at the player for 6 damage; only weapon ids 7 and 16 hurt them |

**Merged.** 31 functions and 60 globals into `catalog/helpers.json` (one name where two agents
differed, for example `SolidObject` for `0x80031DA8`); `docs/engine.md` gained the object classes,
solid objects, the game state, the camera, two damage tables; the stage pages gained the record
meanings. The two Yammark objects were renamed `rescue-reploid` and `light-capsule`, because their
code is shared by all stages.

**Checked myself before the merge.**
- The "spawned" mark of a record is bit 0 of the flags, not bit 7 as `engine.md` said: Yammark
  record 59 is `01 07 00 10` in RAM.
- `GetMiscObject` has 96 slots, not 32 (`0x8002C530` scans 9216 bytes in steps of 96).
- The shadow and capsule damage tables, read from the executable.
- Three agents read `game+0x26` in three objects; their readings agree: a per-stage flag.

**Two agents found the same fact alone.** The Nightmare Virus agent and the stage-object agent both
found that stage-object type 4 is the Rescue Reploid. An independent second reading is the best
check that we have without play.

**Review.** A fresh reviewer checked about 110 claims against the code and the save states. Most
were correct. Two Important errors, both about one gate: the open-condition table of the boss gate
was shifted by one row. Read again in the `bne` chain at `0x8005027C`–`0x80050358`: gate `id` 34
waits for `game+0x28` (Weather Analyze machine 3 destroyed), not for the rain flag; `id` 44 waits for
`game+0x26` = 0; `id` 20 has no condition. So in Rainy Turtloid's stage the player must destroy
machine 3 before the boss corridor opens. Fixed in the sheet, its pseudo code and C, the Turtloid page
and `engine.md`. Also fixed: `DeleteObjectKeep` sets bit 7 of the record flags (the spawner skips
`flags & 0x81`), two Turtloid records added (a Light capsule with the Shadow Armor bit `0x40`, a
teleporter crystal), the shared solid-object rules moved from two sheets into `engine.md`, and
open questions that another sheet had already answered.

Lesson: a table in a sheet that was built from a branch chain is easy to shift by one row. Check
each row against its branch, not the table as a whole.

## 62. The Rainy Turtloid stage map

The user asked if the tile, collision and object layers of Rainy Turtloid's stage can be extracted
by a script, or if they need manual work. The stage tools from sections 36–48 (`learn`, then
`stages`) already do it. They had only been used on the intro and Yammark stages, so we first ran
them on a copy of the catalog in a scratch folder. Nothing was changed by hand.

`learn` found stage member 101 (37 × 6 screens) in overlay 9, the 137 object records (the same count
as the stage list in RAM), and the palettes. `stages` drew the art, the collision, the objects and
`stage.ora` in 13 seconds. As in the intro, the cells of row 0 of layer 0 differ between disc and RAM;
the tool clears them.

Checks: the collision is complete. The art has large empty parts (the sea part). A greyscale draw
has the same gaps, so the layout has no tiles there; the palettes are not the cause. The record
counts by type found two stage-object types that we do not know (5 and 13), and showed that all
Batton Bone B81 records use `id` 0.

Lesson: before writing new tools for a new stage, run the old ones on it in a scratch copy.

## 63. Two unknown Turtloid stage objects: rocket platform and health capsule

The stage map showed two stage-object types that we did not know: 5 (5 records) and 13 (5 records).
We ran the RE kit on each, read the code, and found each sprite bank through the stage's sprite type
list (`GetSpriteBank`: type 131 → bank 6, type 143 → bank 12).

- **Type 5, rocket platform.** A solid 28 × 56 px block on a rocket flame. `id` 0 moves horizontally
  ±96 px, `id` ≠ 0 vertically ±80 px, at 1 px/frame, with a 40-frame stop at each end. It is not
  solid while the player flashes after a hit.
- **Type 13, health capsule.** A solid pad. When the player stands in its middle, it holds the player,
  sets `game+0x27` and refills the HP by 1 every 2 updates. `game+0x27` holds the rain drain, so this
  explains the "guess: an event" in the Weather Analyze sheet. `id` 0 can be used again; `id` 1
  breaks after one use. Four of the five are in rain zones.

Both readings agree with the names that the user gave the sprite banks by eye (the bank table in
`docs/objects-legend.md`), so we kept those names.

A kit fault: the platform's step table has 3 entries, but the kit read 8. The next 5 entries belong
to the rain controller, which the Weather Analyze sheet already describes. Their code mentions the
rain flag, so it looked at first as if the platform tested the rain. Added to the TODO.

Lesson: when a short object's walk has many more functions than its code needs, check where each
state table ends.

Follow-up: the user asked what it takes to close the three open points. One was closed at once: the
unnamed call `0x8004E78C(n)` writes `n` to the stop flags, as `PauseObjects` does. It is now
`SetStopFlags` in the catalog. The player states need a reading of the player's step table; the
one-use capsule after a death needs a play test or a reading of the restart code.

## 64. The player's step table, and two wrong names

The user asked for the player's step table, to name the steps that the health capsule tests (2, 3,
4, 20). The known gate routine `0x8003A1C8` is called from one place, `PlayerUpdate`
(`0x80034FBC`), which runs the step from a table of 32 routines at `0x80073ACC`. When `game+0x10` (a
stop flag) is not 0, the step does not run.

- **Step 2 is stand.** All seven save states have the player standing, and all have step 2.
- **Steps 3 and 4 are walk start and walk.** Step 2 goes to 3 when a direction button is held and
  no wall is on that side; 3 goes to 4 at the end of its animation; both go back to 2 when the
  button is released. Both call `PlayerMove`.
- **Step 20 is the event hold.** Nothing in the player code reads `game+0x27`, so something else
  moves the player to step 20. It was the call that the catalog named `MusicPlay` (a guess). Its code
  shows that it is a player event request: `mega+0xD0` = 1, `mega+0xD1` = step, the player lock on.
  The player code then sets its step to `mega+0xD1`. Step 20 only animates until `mega+0xD0` is
  cleared, by the call that the catalog named `BossMusic` (also a guess).

So two catalog names were wrong. They are now `PlayerEvent` and `PlayerEventEnd`. Three sheets used
them: the health capsule ("music track 20"), the Light capsule ("a sound") and D-1000 ("the boss
music starts"; "X's step 20, probably landed"). All three are fixed, and `docs/engine.md` has a new
section "The player".

Lesson: a "(guess)" name in the catalog spreads into every sheet that meets the call. Before a sheet
leans on a guessed name, read the routine; these two were 5 and 9 instructions long.

## 65. Batch 3 starts: Commander Yammark

The user asked to extract the sprites, the animations and the behaviour logic of Commander Yammark.
The sprites and the 23 animations were already exported (section 55). The kit ran on the boss-room
save state: 85 functions, 18 state tables, one neighbour (visual type 15). The disassembly is about
6,900 lines. We read all of it by hand in one pass, in this order: set-up, main, THINK, the step
dispatchers, the attack sub-steps, the drones, the shots, the intro, the death.

What we found:

- **He fights through drones.** Main type 49 with `var` 1 is a dragonfly drone (8 HP). The boss sets
  a "mode" global, and every drone reads it to pick its step. The drone count follows the HP: 3, 4,
  then 6.
- **Six fixed attack patterns**, picked by HP and a random number. X and Zero get different
  patterns. Against Zero the drones dash at the player one by one, and a drone that Zero hits flies
  back and takes 3 HP from the boss.
- **Weapon ids 8, 17 and 44 stun him** for 90 updates and freeze the drones.
- **Boss level.** His HP is 32 plus 0, 6, 12 or 16 by a boss level. The level comes from a per-
  character counter at `game+0x8D` (7 in the save state, level 0). What the counter counts is open.
- **Soul.** A kill in under 90 s gives the 200-soul boss soul, else the 100-soul one.

The drone main routine is one function of 2,000 lines with six `switch` tables. The kit shows it as
one block (the kit TODO already has this item). Reading the switch tables in `data.txt` by hand
worked.

The sheet, the notes and the pseudo code are written. The reconstructed C is not written yet. New
catalog names: `BossLevel`, `boss_level_tbl`, `dmg_tbl_yammark`, `game.boss`, `game.boss_bar`,
`game.boss_count`, `game.boss_level`. `docs/engine.md` has the new damage table and the boss fields.

Lesson: for a boss, read the step dispatchers before the sub-steps. The dispatchers give the order,
and five of the sub-step routines were the same code with different numbers.

## 66. Yammark: fresh review, reconstructed C, fix pass

The user asked for a fresh review by a helper agent, and for the reconstructed C. The reviewer
checked the sheet against the disassembly while we wrote the C.

**Reconstructed C** ([`~/mmx-data/x6/re/commander-yammark/reconstructed/`](../../work/commander-yammark/reconstructed/)): the headers from D-1000
with the corrected `PlayerEvent` names and the new boss fields, and one C function per game function
in the binary's order. A syntax-only compile with stub PS1 headers passes; the logic is not compiled
or run.

**Review.** All the risky tables were correct: the pattern choice, the pattern lists, the mode →
step map, the `var` → table map, the column heights, the wall offsets, the shot rates. Four Important
errors, all in the drones. We checked each against the code before the fix:

1. The flicker rule. Each drone step has its own display exit. Circling drones flicker all the time;
   they are solid only when they enter, appear, attack or are frozen.
2. An extra lap. The turn-to-angle code arrives only when the signed difference crosses zero, and the
   attack waits for the second arrival. So the drones turn one more full lap (120 updates) first.
   The C had the same logic wrong in one branch.
3. The lance rhythm. Each drone that reaches its column sets the boss counter to 60, and a dash comes
   at 75. So a dash comes 15 updates after the latest arrival, not every 75 updates.
4. The wall bob. Bit 5 of a counter gives a 64-update period, not 32.

Also 17 Minor findings: off-by-one counts (death blink 26/21/16/11/6, burst contact 29), sound
timing, the deflected id range (64–78; id 79 is 127), the knock-back condition (any hit while the
player is Zero), the stage 12 area 0 test, and small things the sheet left out. All fixed in the
sheet, the pseudo code, the C and `docs/engine.md`.

Also renamed `0x8008EAFC` from `no_rain_drain` to `text_open`: Yammark and the Light capsule both wait
on it after `TextOpen`, so the rain drain skips HP loss while a text box is open.

Lesson: a counter that is reloaded with n and tested at −1 runs n + 1 updates. Count each reload
loop by hand.

---

## 67. Two mid-stage Turtloid save states

The user saved two states in the middle of Rainy Turtloid's stage (PCSXR slots 1 and 2) and asked
what new information they hold. We copied them to `turtloid-2_mid.state.gz` (x 5736) and
`turtloid-3_mid.state.gz` (x 8132), and listed every live slot of every object class, the game work
bytes that changed, and the record flags, against `turtloid-1`.

**New facts.**

- **The second record list** (`0x80073554`, open since the intro) holds the objects of fixed place:
  stage logic, the rain controller, the Nightmare shadow spawners and four scenery objects. So the
  TODO question "what places the rain generator and the shadow spawners" has its answer.
- **The first live misc objects.** The scenery records (kind 5, type 20) make misc objects in
  `0x800CD410`. Their slots use the main-object layout. The code draws sprite 0 of the one-sprite
  bank 140 at a fixed point until checkpoint 4.
- **Checkpoints.** The `b` 6 stage-logic records are checkpoint markers. The checkpoint values in
  the three states (0, 3, 4) agree with the marker x positions.
- **Two new sprite exports** from live objects: `rocket-platform` (2 animations) and `health-capsule`
  (10 animations). We added the names to `catalog/enemies.json`.
- The live Weather Analyze machines, sensors and rocket platforms agree with their sheets.

**A wrong guess, corrected by the user.** In `turtloid-3_mid` every record behind X was reset and
the machine 2 wreck, which never deletes itself, was gone. The lives were 8 in all three states, so
we concluded that X did not die, and started to look for another routine that clears the objects.
The user told us that X died and got lives back by saving Reploids. So a death clears all objects
and resets the whole record list, and the Reploid status survives it.

Lesson: a counter that both goes up and down (lives) does not prove that an event did not happen.
Ask the player when the save state cannot tell.

---

## 68. The alternate Turtloid section, and the wrong layout address

The user saved a state (slot 3, `turtloid-4_alt.state.gz`) after the blue teleporter crystal
before Turtloid's boss room, and asked us to read it. X was in a new stage number: stage 21, area 1,
overlay 30.

**Three tool limits on the way.** `learn` stopped three times, and each stop was a real limit:

1. The tile info of two members (101 and 120) was in RAM; the old one stays loaded. `learn` now
   keeps the member whose screens join the layout.
2. The object list pointer of area 1 was not tried; `learn` tried only area 0 of each stage. It now
   tries both, takes the current area when both fit, and records `table_area`.
3. The disc search of the list compared the kind byte exactly, but the game adds a respawn digit
   (`0x10`) to the kind of a spawned kind-0 record. The search now ignores that digit.

**The big find.** The first map of the section was clearly wrong: X stood in an empty screen, but a
health capsule stood next to him. The layout reader in the engine (`0x80027F1C`) takes the layout
address from the scratchpad word `+0x04`, and that word is `0x80097438` in all ten save states. Our
tools read from `0x8009741E`, 26 bytes too early, since sections 33–35. The test: with the new address,
X stands 24 px above solid floor in all six standing states, and every intro Junkroid record is 24
px and every Meta Wheel F record 32 px above solid floor. With the old address, none of this holds.

What the wrong address had caused:

- For the 26-wide stages (intro, Yammark), the art was one screen row too low. The "row 0 header",
  the bottom row that seemed to be missing, and the Yammark "vertical wrap" (section 48) were all this offset.
- For Turtloid (37 wide), every row was shifted by 11 screens, so the first map was scrambled.
- The edge test did not catch it: a shift keeps most joins, because each row is still a run of
  neighbours.

After the fix, all four maps learn with 0 differing cells. We redrew them (old drawings in
`~/mmx-data/x6/stages.old/`). The edge test now scores only layer 0: the background layers scroll
on their own, and intro layer 1 does not join between rows 4 and 5. Its vertical threshold is
0.70; the four real maps score 0.74–0.96, the wrong member 0.31.

**The section.** One tall room of platforms with 12 Monbandos, 15 Batton Bone B81s, Weather
Analyze machine 4 (the table row that no record used before), 4 Rescue Reploids, 2 health capsules
and boss gates 44 and 45. Banks for types 53, 55 and 63 are loaded without texture, so what is behind
gate 45 is not known yet.

Lessons:

- A score that passes is not proof. Check the result against a fact from outside the tool, here
  "the player stands on the floor".
- When a value comes "from the game", find the code that reads it. A fixed address that worked on
  one state was never confirmed by code.
- Withdraw old conclusions openly. Three TODO items and one confirmed note were withdrawn.

---

## 69. Zero Nightmare and the boss overlay

The user saved a state (slot 1, `turtloid-5_zero.state.gz`) in the boss room behind gate 45 of the
alternate section, during the fight against Zero. They said that the fight is almost the same as the
Zero fight in X5, with a new palette and one or two new attacks.

**What `learn` said.** The same map as section 68 (member 120, 0 cells differ). So the boss room is
part of the section, not a new stage.

**Why the sprites first failed.** The exporter skipped types 53, 55 and 63: "update function is in
neither the stage overlay nor the main program". Their update functions are at `0x800FB214` to
`0x800FDA98`, above the end of the stage overlay (`0x800F003C`). A search of all 59 overlays in RAM
found overlay 43 at `0x800FA000`. Two pieces of code load overlays to that address. One is the
record of stage logic 13 at the boss room (x 1360). It picks overlay 43, 44 or 45 from the byte
`0x800CD30B`, waits for the load, makes the boss (type 53, 55 or 63) and deletes itself. So this
section can hold three different bosses; in this state it was Zero Nightmare.

**The fix.** `animfind.boss_overlay` finds an overlay at `0x800FA000` (or none), and
`code_regions` gives the exporter and `rekit` all code areas. Two tests. After the fix, Zero
Nightmare exports from its live object: 124 sprites, 17 animations (`zero-nightmare`). Its texture
covers the cells of banks 4–13 in this state, so the exporter skips those banks here, as it should.

Lessons:

- A "not found" from a tool can mean a missing search area. Ask where the address really is before
  you mark the object as unknown.
- A section can choose its boss at run time. One save state shows one choice only.

---

## 70. Two helper agents: the Zero Nightmare sheet and `boss_choice`

The user asked for two agents at the same time. To keep them apart, the sheet agent could write only
its new sheet, and the research agent could write no repo file. Both reported their proposed changes
to the shared files, and we merged them. We checked the key claims ourselves first: the three
`boss_choice` writers disassemble as `sb …,1083(base)`, and `0x800CCED0` + 1083 = `0x800CD30B`.

**The sheet** (`docs/enemies/zero-nightmare.md`). HP 48 + {0, 2, 4, 8} by boss level. He stays on
the floor. THINK picks a chain of steps from a pattern table, by distance, height, HP and Hard.
Attacks: dash, buster with a saber wave, rapid fire and ground pillars (low HP only), slash, a warp to
the player, big crescents (Hard only), and a stun warp that one weapon (id 50) starts. The agent
checked THINK against the save state: the boss was in the dash of the pattern that THINK gives for
his position and HP.

**`boss_choice`.** Our search in section 69 found no writer because each writer uses the `game`
pointer with offset `0x43B`, not the absolute address. The byte is in the memory-card save. It goes
0 (Zero Nightmare) → 1 (High Max) → 2 (Dynamo) with game progress, and all 8 hidden areas share it.
This confirms the user's guess. High Max rests on code that overlay 44 shares with the intro's type
55; Dynamo is the remaining choice [both likely].

Lessons:

- A search for an absolute address misses stores through a base pointer. Also search for the offset
  from each known base (`game+0x43B`).
- Give parallel agents separate files, and merge their shared facts yourself.

---

## 71. Four new states: High Max and Rainy Turtloid

The user beat Zero Nightmare, played on as Zero, beat Rainy Turtloid and went back to the hidden
area. They saved four states: Zero in the main stage (`turtloid-6_zero-player`), the main boss room
(`turtloid-7_boss`), the report screen (`report-1`) and the hidden boss room again
(`turtloid-8_high-max`). We copied them with plain `cp`, because PCSXR slots are already gzipped.

**The prediction held.** Section 70 said that `boss_choice` 1 gives High Max through overlay 44. In
`turtloid-8_high-max` the byte is 1, overlay 44 is at `0x800FA000`, and the boss is main type 55
`var` 1. The sprites show High Max in a dark palette. So value 1 is now certain; only Dynamo (value
2) is still a guess.

**Exports.** Both bosses exported from their live objects on the first run, with no tool change:
`high-max` (49 sprites, 15 of 23 animations) and `rainy-turtloid` (119 sprites, 34 animations).
Rainy Turtloid's code is in the stage overlay, so no boss overlay was needed. Four other exports in
these states had garbled sprites: "search" exports and objects whose bank cells the boss texture
covered. We did not name them.

**`learn`.** Only the hidden-area state fit a catalog entry. The two main-stage states failed the
texture-block search (the boss texture changes VRAM), and the report screen is not a stage. The
earlier mid-stage states are not in the catalog either, so nothing is lost.

**Side facts.** Across the states, `game+0x5F` got the Zero unlock bit, `game+0x5E` changed from 1
to 5 when Zero was picked, and `game+0x60` got bit `0x20` at the Turtloid clear.

Lessons:

- A prediction from the code is worth a save state to test it. One state turned "likely" into
  "certain".
- Compare the same bytes across a sequence of states. The progress bytes changed one step at a time.

---

## 72. Probe: where the player frames come from

**Question.** The players keep only one frame in VRAM. Can we get all their frames from one save
state, or do we need the disc? The answer sets the size of the player exporter.

**What we did.** We disassembled the only caller of the decompressor (`0x80017B04`, now
`PlayerFrameStream`). It reads the frame number from `mega+0x47`, takes the word for that frame from
the table at `mega+0x38`, and unpacks the data at "table + low 20 bits". The high 12 bits are the
tile count. So the frames are next to their table. A script then unpacked every entry in 7 states.

**Results.**

- X: 228 frames, Zero: 240 frames. Every frame unpacks to exactly its tile count × 128 bytes, and the
  blob is the same in every state of one character.
- The frame on screen in 4 states, unpacked by our code, matches VRAM (320, 0) row for row.
- The same blobs are in `ROCK_X6.DAT`: member 87 (X) and member 85 (Zero), byte for byte. Members 86,
  88 and 89 have the same structure with a 228-frame set each, so they are probably X's armours.

**What it means.** The player exporter can read the disc directly, with no new save states for the
frames. Save states are still useful to check the palettes and to identify the armour members.

Lessons:

- Read the one routine that does the job before you plan a search. Its pointer arithmetic told us
  where the data is.
- When the data is in RAM, look for it on the disc too. A byte match turns a state-based tool into
  a disc-based one.

---

## 73. The player sprite exporter

**Design** (`docs/superpowers/specs/2026-10-21-player-sprite-export-design.md`). Three more facts
made the tool simple:

- The frame that `PlayerFrameStream` loads is the sprite number of the current animation step. The
  sprite count equals the frame count. So sprite n is drawn from frame n.
- The player's animation table is in a player overlay at `0x801EA000` (overlay 0 for X, 1 for Zero),
  unchanged from the disc.
- The existing `sprites.draw` draws a player sprite with no change, when the frame is put in VRAM
  first.

**Build.** `mmxdemake/wordlz.py` (the decompressor, from our reading of `0x80018BE0`),
`animfind.player_overlay`, a `limit` on `read_table` (the player has up to 193 animations, the old
limit was 64), a `grid` function split out of `spriteexport.sheet`, and
`mmxdemake/playerexport.py`. For each sprite the tool unpacks its frame, puts it into a copy of VRAM
at the texture position (rows of 16 tiles, as the game does), and draws it. It finds the disc member
that holds the frames, and names the folder from `catalog/players.json`.

**Tests.** 21 new tests (402 in all, 10 skipped). The key test gives sprites 0 and 1 the same tile
and frames 0 and 1 different colours there; a wrong frame choice would give the wrong colour.

**Result.** X: 228 sprites, 193 animations; Zero: 240 sprites, 169 animations; none skipped, about
2 seconds each. We looked at the strips: X's beam-in (animation 2) runs from the light column to the
landing pose, and Zero's saber trails are green, as in the game. A state with no live player (the
report screen) stops with a clear error.

**Review.** One fix: a state without a live player first gave "0 frames"; now it says that no player
object is live.

Lessons:

- A prototype on real data before the module found the facts that the design needed (sprite =
  frame, the overlay) in minutes.
- Name scripts with care: our own `dis.py` in the notes folder shadowed the standard library again.

---

## 74. Two helper agents: the Rainy Turtloid and High Max sheets

The user asked for two agents, as in section 70. Each could write only its own sheet, and each
reported its proposed changes to the shared files. While they ran we built the player exporter
(section 73).

**Our checks before the merge.** The main type table gives `0x800F2E20` for type 57 and
`0x800FDA98` for type 55, as the agents said. Rainy Turtloid's sprite routine (`0x800F0434`) compares
the animation number with 28. High Max's four shots in the save state use sprite data `0x801521BC`
(67 sprites), not the boss's `0x8014FCE8` (49): so they have their own sprite set.

**Rainy Turtloid** (`docs/enemies/rainy-turtloid.md`). HP 56 + {0, 0, 4, 8}. The shell deflects
frontal shots; only a back strip takes damage, while the shell is open. Two crystals (16 HP) in the
shell fire the missiles; with both lost, the whole body takes damage for 660 updates. THINK picks
the missile volley, then a spin, bubble or rain roll, by HP and Hard. Weapon ids 2, 11 and 33 stun
him.

**High Max** (`docs/enemies/high-max.md`). HP {48, 52, 56, 64}. A guard deflects all but a few
weapon ids; those ids break the guard and give one damaging hit. Attacks: a 12-ball volley, a big
ball that bursts into homing pieces, a barrier rush, a shield ram, and orbs below 1/3 HP. The intro
High Max (`var` 0) is the same code with a fight that cannot be won or lost.

**What the merge changed.** Main-program names went to `catalog/helpers.json` (the damage tables,
`mega.armor`, `PaletteBlend`, `BossKillSound`). The agents' overlay-local names stay in the sheets,
because one overlay address names different code in each stage.

**A problem the agents found in our exports.** Both bosses use more than one sprite set: Rainy
Turtloid takes animations 28–33 from the next two banks, and High Max's shots have their own set.
The enemy exporter draws every animation from one set, so those strips are wrong. This is now a
TODO item.

Lessons:

- A behaviour sheet is also a check of the exports: each agent compared the code's sprite choice
  with the PNGs.
- Keep overlay-local names out of a global catalog, or give the catalog a per-overlay section.

---

## 75. Player RE, step 0: a player mode for `rekit`

The user chose the plan's steps 0 and 1 for X, to run unattended.

**What the code looks like.** All 32 step routines are in the main program, and X and Zero use the
same 32 addresses. The character's own code is in the player overlay: overlay 0 for X (about 171
functions with a prologue), overlay 1 for Zero (about 61). A walk from the steps alone reached only
73 of X's overlay functions. The rest are reached through pointer tables: one in the main program at
`0x80071220` (it holds X overlay addresses even in Zero's state, so it is a fixed table, probably
the weapon-object types) and tables inside the overlay (`0x801F37E4`, `0x801F397C`, …).

**The tool.** `rekit --player --state S`:

- `codewalk.walk` now takes a list of areas to expand into (before: one).
- The roots are `PlayerUpdate`, the 32 steps, and every overlay function start: a prologue, or a
  pointer-table target that looks like a function start. A start inside an earlier function is
  skipped.
- The summary gets a "Roots" table (function, end, role: "step 4", "overlay 0 function") and the
  function count per area. The animation table may have up to 256 entries.

For X: 414 functions (197 in overlay 0, 217 in the main program), 44 state tables, no truncation.
Short step routines are correct: step 10, for example, only picks a sub-step and calls one of two
routines.

**The retail tests, run for the first time in a while.** With the paths set, two stage tests
failed. The cause was commit `7b1e46d` (the layout read moved 26 bytes to its real address): the
tests still expected the old offsets, 19 differing cells and the old palette camera, and they had
no `table_area`. We updated the expectations to the current catalog values. New retail tests cover
the player exporter and the player kit for X and Zero. With `MMX6_CUE` set too, all 408 tests pass,
none skipped.

Lessons:

- Tests that are skipped by default go stale without notice. Run the retail tests after a change to
  a tool that they cover.

---

## 76. Player RE, step 1: the code map, and two fixes found on the way

**Fix 1: the other character's entry points.** The shared step code calls fixed addresses in the
player overlay, and it chooses with `var`: X's targets when `var` = 0, Zero's otherwise. In X's RAM,
17 Zero targets fall inside X functions, and the first kit walked them as functions. Now a call from
the main program into the overlay must reach a function start; calls inside the overlay are not
checked. The kit lists the others as "the other character's entry". (A first version checked every
call; its test caught that it also dropped real leaf functions.)

**Fix 2: frame sets.** To find each step's animations we first looked for `SetAnime` calls and found
almost none. The player has its own wrapper, `PlayerSetAnime` (`0x8003BA04`). Before `SetAnime`, it
picks the frame table and the sprite data by a byte per animation in the player overlay. X has two
frame sets, Zero three. The player exporter (section 73) drew every animation from the main set, so
50 X and 96 Zero strips were wrong without any error, because the sprite numbers happened to fit.
The exporter now picks the set per animation (its synthetic tests had a 32-bit overflow on a
backward directory offset, also fixed), and 4 X and 1 Zero animations that do not fit are skipped
and listed.

**The map** (`docs/players/map.md`). For each function we collected the step it stores into
`mega+0x05`, its sub-steps, its animations (through the wrapper) and its sounds. Then for each step:
which "try" function enters it, from which steps, and with which animations. With an index sheet of
all 193 X animations, most steps got a name: stand, walk, jump, fall, land, dash, air dash, wall,
ladder, beam-in and beam-out, gate and events. Seven steps stay open. Other findings: the weapon
objects run from a fixed type table (`0x80071220`) whose entries point into X's overlay, Zero's
overlay or the main program, and the type numbers match the weapon ids in the boss damage tables.

Lessons:

- A wrong export can pass every check that only tests ranges. Look for the code that chooses the
  data, not only the data.
- A walk over code shared by two characters must know which character's overlay is in RAM.

---

## 77. Player RE, step 2a: ground movement

The user asked for 2a unattended, then 2b and 2c in parallel if all went well.

**What we read.** `PlayerUpdate`, the ground steps (stand, walk start, walk, land), every "try"
function they call, the physics set-up, the dash (step 12 with its three sub-steps) and the crouch
(step 26), plus `PlayerMove` and the dash's move function. The sheet is `docs/players/movement.md`;
notes, pseudo code and C are in [`~/mmx-data/x6/re/x/movement/`](../../work/x/movement/).

**Main results.**

- The buttons: held at `mega+0x7C` (right, left, up, down, dash), pressed at `+0x80` (jump, dash).
- All speeds come from one table of 16.16 constants (`0x8003D598`): walk start 0.5, walk 2.0, jump
  5.8125, fall −6.5, dash 6.5 slowing by 0.1875 to 4.125. Three part bits add bonuses.
- The dash does not move during its first 5 updates; it ends at a wall, at the opposite direction, on
  release, or after 30 updates, then slides to 0.
- While X shoots, every animation n becomes n + 112, switched at the same frame.

**Corrections to the map.** Step 12 is the dash (the map said hurt), step 26 is the crouch (the map
said dash), step 5 is only Zero's stop pose, and step 31 is the super jump. The three "unused"
addresses after `player_steps` are the sub-step tables of steps 12–14. `0x8001750C` is `StopSound`
(the High Max sheet called it a guess "boss kill sound"; fixed there).

**Checks.** The constants in RAM equal the set-up values in three states (X twice, Zero), with no
parts; all three are in stand with the animation the HP rule gives.

Lessons:

- Names from animations and transitions are a first guess. Two of seven ground names in the map
  were wrong; the code that starts each step corrected them.
- Read the movement call. The dash's move is inside its end test, so "no move call" in a sub-step
  was a real fact (the start frames do not move), not a gap.

**Afterwards: a kit fix.** Naming the player's functions in `helpers.json` made the player kit stop
at them (397 → 363 functions), which would have hidden `StartJump` from part 2b. The player kit now
stops only at named functions outside the player's main-program code (`0x80034FBC`–`0x80040000`,
the cluster the walk found).

---

## 78. Player RE, steps 2b and 2c: two helper agents

As planned with the user, 2b (air) and 2c (ladder, hit reaction, beams, events) ran in parallel
after 2a. Each wrote only its sheet: `docs/players/air.md` and `docs/players/ladder-hurt-events.md`.

**Our checks before the merge.** `player_steps` holds overlay addresses at 32–63 and 74–77 and stand
at 64–71; `TryHang` (`0x80039984`) tests up held and tiles 46 / 37–45; the jump step zeroes
`speed_y` when jump is released; `ToLadderUp` uses `+0x11C` (1.5); `PlayerInitBeam` sets `speed_y`
−8.0 and step 0; `BeamOut` sets +8.0 and step 1; `PlayerHitCheck` calls `PlayerHurt` (`0x8003963C`).

**Main results.**

- Air: a held jump rises 68.5 px in 23 updates; releasing jump stops the rise at once. Walls: only
  side tiles 56 and 58 allow a slide or a kick; grab 8 updates, slide 1.4375 px/update, kick 5.625 up
  and 2.0 away. The air dash is only for X in form 4 and for Zero. X's form 1 (the Falcon Armor,
  X's start in X6 [likely]) has its own air dash.
- The hit reaction is step 17, started by HP bit 7 one update after the hit; six hit classes with
  tables for the animation, the motion and the invulnerability.
- Step 0 is the beam-in, step 1 the beam-out, step 18 a ride. Death: spikes, a crush between terrain,
  a fall 256 px below the camera's lowest limit, or HP 0.

**Corrections to the map.** Steps 27/28 are rope and bar hangs, not wall steps (the wall steps are 9–11);
step 18 is a ride, not a warp out; steps 0 and 1 were swapped; the step table has more than 32
entries, so the stores of 33, 35 and 74 are real player steps. The engine note "when the player is
hurt (`0x80039324`)" was only the squeeze damage.

**Merge choices.** Three functions had a name from each agent: `0x8003CDBC` → `ClearShotFlags`,
`0x8003D394` → `WaterSplashCheck`, `0x8003B114` → `StandOrFall` (it ends both an air move and a hit).
The `mega` fields at `+0xD5`, `+0xE5`, `+0xE6` and `+0xE7` got new names (`ride`, `squeeze_timer`,
`squeeze_dmg`, `form`).

Lessons:

- A first map from animations and transitions named 25 of 32 steps; 7 of those names were wrong
  (12, 26, 0, 1, 18, 27, 28). The map was still worth it: it set the split that let two agents work
  without overlap.
- Agents that report shared changes instead of writing them gave three name conflicts, not three
  broken files.

---

## 79. Shield Sheldon: seven states, five agents

The user saved seven states in Shield Sheldon's stage (stage 7): X shooting lemons, a half charge
and a full charge (Falcon Armor), X with no armour, the Nightmare Effect with hidden floor tiles
(hidden, then shown under X), and the laser puzzle. We copied them with plain `cp` as
`sheldon-1_lemons` … `sheldon-7_lasers`. A first probe of each state (stage, form, live objects
per pool) gave two facts at once: stage 7 is Shield Sheldon, and the no-armour form is 0, not 1. So
the docs' "X uses forms 1–4" was wrong.

Five helper agents worked at the same time. Each owned its own files and wrote shared changes
(names, `engine.md`, `TODO.md`) to a proposal file:

- **X's attacks** (`docs/players/attacks.md`). Lemon 6.75 px/update, at most 3 counted shots; the
  charge levels at 25 and 90 updates (12 and 45 with Quick Charge); the shot ids per form. The
  full-charge state had no shot object: the charged shots come from a muzzle flash (effect type 2)
  that makes the shot at a flagged frame, and the state was one update before it.
- **Steps 32–77 and the forms** (`docs/players/steps-extra.md`). `rekit --player` now walks all 78
  step entries (one more rule: an overlay entry inside a known function is the other character's
  entry). Forms: 0 no armour, 1 Falcon, 2 Shadow, 3 Blade, 4 Ultimate, 5 Zero. The stage load picks
  the sprite member by form: 1 → 87, 2 → 88, 3 → 89, 0 and 4 → 86. `catalog/players.json` now names
  86 `x-no-armor` and 87 `x-falcon`.
- **The stage rip** (`docs/stages/sheldon.md`). `learn` refused the map (vertical edge score 0.282).
  X stood on solid floor, so the map was right and the test was wrong: many floors here end exactly
  at a screen edge. A vertical join with a uniform edge row is no longer scored. The hidden floors:
  the collision (new value 60) is always in the map; main type 32 only draws the floor for 150
  updates after a touch. A search for the new camera limit (3488) found the laser door routine.
- **The Nightmare Bug and the infected Reploid** (`docs/enemies/nightmare-bug.md`,
  `rescue-reploid.md`). The wasps are a new type (22), not the Nightmare Insect. They follow the
  player, deflect almost every weapon and leave after 6 deflects.
- **The laser puzzle** (`docs/enemies/laser-reflector-puzzle.md`). One main type (13) for beamers,
  reflectors and doors; the weapon-pool type 79 is the laser's probe box, not a player shot.

**The user's question: why was the stage not purple on the stage select?** We searched all the
code for the effect array and found its writer in the stage select (overlay 16). A stage pick
writes the picked stage number into two target stages, and writes the targets to a separate 3-byte
list. The stage start copies a stage's byte to `nightmare_effect`. The portrait code marks only the
stages in the 3-byte list, so only the last pick's targets are purple. Shield Sheldon's effect came
from the Yammark pick; the Turtloid pick replaced the list. The states agree at each step. One agent
had guessed that a clear writes the bytes; `yammark-1_start` already holds the Yammark bytes before
the fight, so the write is at the pick.

**Merge.** The two player agents gave nine addresses different names; we kept the attack names for
the shot code and `WeaponPoseStart` for the pose step. We renamed `cam.target` to four
`cam.target_*` fields. Spot checks in the disassembly: the member pick at `0x80013BF0`, the charge
constants at `0x80040048`, the floor timer 150 in overlay 10.

Lessons:

- Probe a new state before you hand it out. Two lines per pool gave the stage, the form and the
  object types, and every agent prompt started with facts instead of guesses.
- A question about what the player sees can have a plain answer in the data. Two reads of one array
  and one list across the states explained the missing purple mark.
- An agent's "seems" in a proposal is a lead. One state that predates the event was enough to test
  it.

---

## 80. Shield Sheldon: the lower rooms, the hidden area, two enemies and the laser sprites

The user named the two enemies that the agents could not look up (Mettaur D2 and Guardian) and
saved six more states: two in the lower rooms of section 1 (`sheldon-8_lower-1`,
`sheldon-9_lower-2`), and four in the hidden area behind the teleporter crystal
(`sheldon-10_section-2`, `sheldon-11_laser-shot`, `sheldon-12_high-max`). A probe gave the stage of
the new section at once: stage 22, a hidden area, with `boss_choice` 1. Three helper agents worked
at the same time, with the same file ownership rules as in section 79.

**The stage rip and the `learn` fit test.** The hidden-area states fit at once: overlay 31, member
121, 26 × 3 screens, 42 records. The two lower-room states failed: "texture block found at 0 VRAM
positions". The closest position was the right one, with 1519 rows wrong; a page count showed pages
2–6 all different. A search of member 102 for short pieces of those VRAM rows found them 1024 bytes
apart: entry `0x16`, a second texture block of the same size, stored row by row. So the first
drawing of the stage had garbled art in the lower rooms and the boss part. We had not noticed it,
because only the start and the laser rooms were checked. `learn` and `stages` now handle the second
block, and a second catalog entry draws the lower rooms. The hidden area has bars over spikes, the
first rope (collision 46) in our maps, nine ceiling beamers, and High Max from the same
`BossSpawner` as stage 21.

**Did the boss-room state need a new rip?** No. Its stage, area and layout bytes were the same as
the section start, so the map was already complete; the state only confirmed the boss and the boss
room camera.

**The enemies.** Mettaur D2 paces on one hidden floor each, shoots forward after 61 updates in
range, and guards only against a lemon from the front. The Guardian does not move; its shield
covers the front of its body, and it opens it for about 128 updates to fire 4 aimed shots. Both
matched the live objects in three states.

**The laser sprites.** The laser takes its sprite data from `sprite_bank_base` entry 16, one entry
after the type-list banks, and its texture and palette from its parent. `spriteexport` now makes a
sprite-only export for such an extra source, and for an object whose table holds only function
starts (the hidden glass floor). The agent ran the old and the new code on all 26 dumps into two
new roots: every old file stayed byte-identical; only the new folders differ.

Lessons:

- "0 positions found" with a closest match of 1519 bad rows means a different block, not noise.
  Count the bad rows per page before you change a threshold.
- When a piece of VRAM is found on disc at a regular stride, the stride tells the storage order.
- Check every part of a drawing, not only the part that the first state shows.
- Before a new rip, compare the layout bytes with a state you already have. Equal bytes answer
  "is this state needed?" in one line.
- For a tool change, run the old and the new code on all the data and diff the output. That
  proves "nothing else changed" better than the tests alone.

---

## 81. Shield Sheldon: the texture swap, the closed hole, the boss and the Death Guardian

The user saved five more states in Shield Sheldon's stage: a door just opened (`sheldon-13`), the
corridor before the lower rooms (`sheldon-14`), the first lower room (`sheldon-15`, where they saw
a new background and could not climb back up), the Death Guardian (`sheldon-16`) and the boss room
(`sheldon-17`). They asked what logic stops X from climbing the wall. Three helper agents worked
at the same time.

**The wall question.** We answered from the code and the disc map at once: the walls at the
opening are collision 57, and the side-wall test allows a slide and a kick only on 56 and 58; the
opening is 224 px above the floor and a jump is 68 px; `CameraClamp` clamps only x. The stage agent
then read the live collision: in state 15 there is no opening. Door `0x54` copies solid cells over
the hole when the camera passes. Our parts held, but they were not the main cause.

**The texture swap.** A page count showed that `sheldon-14`, "before", already had the second
texture. The only pointer to it was in the scratchpad (`+0x44`). A search for the rectangle (320,
256, 512, 256) found a 10-line function that uploads it when a byte is 1; a camera zone with `id`
bit 7 writes that byte. That explained the row-by-row storage too. The background change is a
separate kind-7 object that changes the BG layer scroll at checkpoint 1.

**The boss.** Shield Sheldon's code is in the stage overlay. His guard deflects every weapon; he is
open only in a few windows of each attack. The "reflect" is his plates and turrets, which fire back
when a shot touches them. Weapon ids 4, 13 and 35 stun him. The plates, image and turrets have no
damage table, so `ApplyWeaponHit` reads one from RAM address 0.

**The Death Guardian.** The same code as the Guardian; only the set-up differs (contact 7, larger
boxes, bank + 1). `spriteexport` now exports the second bank of a type that is listed twice. The
agent checked the old and the new exporter on all 33 dumps: only the new folder and two stage
index lines differ.

Lessons:

- Check the "before" state first. One page count showed that the change was earlier than the user
  thought.
- A pointer that is not in RAM can be in the scratchpad.
- A rectangle in the data is a short way to the upload code.
- Read the live collision before you explain a "cannot reach" question. The disc map showed a hole
  that the game had closed. Our first answer was right in its parts and wrong in its main cause.

---

## 82. Metal Shark Player: section 1, the Nightmare Press and the Ride Armor

The user moved on to Metal Shark Player's stage and saved one state: X on the Ride Armor on the
upper path (`shark-1_ride`, stage 4). They asked two questions: what exactly kills X when the
ceiling crusher meets his head on the ground, and whether the mech suit is a "fixture" attached to
X. A two-line probe showed the player in step 18 and `actor2` live at X's position, so the mech is a
separate body. Three helper agents worked at the same time: the stage rip, the crusher, and the Ride
Armor.

**The stage.** `learn` passed at once: member 98, 22 × 8 screens, overlay 06, 118 records. The
lower part needs the second texture block again (member 98 has entry `0x16` too). The camera zone
table of area 0 has 11 entries and the next 4 belong to area 1, whose record lists are valid only in
overlay 07. So stage 4 has two overlays, and that explains the overlay numbers of stages 5–7. A
search for `nightmare_effect` reads found the stage's effect object at once: falling crates that
delete themselves unless the value is 5. The stage-select rule from section 79 predicted the
effect bytes of this save exactly.

**The Nightmare Press.** It has no sprites: its art is BG layer 1, which a kind-7 controller moves
up and down, and 15 invisible solid blocks follow the layer. The crush is not a special test. The
press pushes the player down and writes the squeeze damage 127; the floor sets the other flag; the
next update `PlayerHitCheck` sees floor and ceiling and takes 127 HP. Nothing in that path tests the
invulnerability or the difficulty.

**The Ride Armor.** `actor2` is a fixed second-body slot with its own type table. The ride reads
the pad, moves, collides and takes the hits, then writes its position into X. X has no terrain box
and no hurt box while riding, so the press skips X; the ride instead sets a flag that makes the
press go back up. In engine terms the mech is a second entity that the player is parented to, not
a fixture on the player.

**Merge.** Two agents named the kind-7 class differently ("BG effect pool" and "sys objects") and
gave its loop two addresses 4 bytes apart. The disassembly showed `0x800208EC` as the prologue. One
class holds both the Sheldon background switch and the press, so we gave it a neutral name.

Lessons:

- Probe the state before you split the work. `actor2` at X's position decided the split between
  the crusher and the ride agents.
- A "crusher" can be a background layer. When no sprite exists, look at the layer cameras.
- When two agents name one thing from two uses, the name should come from what the thing is, not
  from either use.

---

## 83. Metal Shark Player: section 2, the belt, the mini boss, the boss and the weapon slots

The user saved nine states in section 2 of Metal Shark Player's stage (`shark-2` to `shark-9`): the
spawn on a conveyor belt, the press at its lowest over X's head, the mini boss, the white screen and
the jump to the boss gate, the rooms before and with the boss, the boss stunned by Rainy Turtloid's
weapon, and the boss with a "resurrection" side kick. Then the report screen (`report-2_shark`).
Four helper agents worked at the same time: the stage rip, the belt and the press, the mini boss,
and the boss.

**A first guess was wrong.** A probe showed a kind-7 object (type 3) that was gone after the mini
boss, and we said it was probably the belt. It is the forced scroll of the mini-boss room. The belt
is stage logic 9: it moves X 1 px left on collision value 58, waits while X is in the beam-in step,
and deletes itself at checkpoint 2.

**The stage.** The `learn` fit test failed twice for new reasons: the belt animation changes byte 2
of 32 tile info words in RAM, and the press shake made two draw buffers with cameras 2 px apart tie
the vote. Both fixes change only results that failed before. X's two jumps between the states are
warps inside the area, not area changes; the second one happens while the boss explosion makes the
screen white.

**The press check.** In `shark-3` X stood alive under the press at its lowest point: his box top was
10 px below the kill line. The rule from section 82 held; the agent corrected one offset in the
press sheet.

**The mini boss and the boss.** Nightmare Pressure is a machine on BG layer 1; its pods pass their
damage to the head. Metal Shark Player has five attacks and calls one of three side kicks at low HP.

**The weapon slots.** The stun state gave the link that the docs had guessed at: slot 6 was selected
and the stun id was 6. The player code copies the slot to the weapon id and adds 9 for the charged
shot. So the stun ids 2/11, 4/13, 6/15 and 8/17 are slots 2, 4, 6 and 8, and the owned-weapon and
beaten-Maverick bits suggest that slot n is the weapon of the stage n Maverick. The report screen
added a third point to the beaten-Maverick bit rule (`1 << (stage − 1)`).

**Merge.** Spot checks: the boss HP table {0, 0, 4, 8}, the slot copy and the + 9 at `0x8003DF58`.
Two agents gave three overlay 07 functions different names; we kept the stage agent's warp names. A
boss agent noticed that `catalog/helpers.json` applies overlay names in every stage (`0x800F765C`
is a Sheldon turret in overlay 10 and a spark in overlay 07); this is now a TODO item.

Lessons:

- A user's "weak state" save is worth more than it looks: the selected weapon in it tied slot
  numbers to weapon ids for every boss.
- When an exact RAM search fails, diff the closest copy byte by byte. Here one byte per word was an
  animation.
- A tie in a vote can be two copies of one answer. Look at how far apart the tied candidates are.
- Say "probably" for a guess from a probe, and correct it plainly when the code says otherwise.

---

## 84. The damage map

The user asked how damage to and from objects is calculated. Most rules were already in
`docs/engine.md`, collected one object at a time. We read the nine damage functions of the main
program in full and wrote one map, `docs/damage.md`: the call order, the formulas, the classes, the
bonus list, the shot reactions and the player side.

**Four rules in the docs were wrong or short.** A contact damage of 0 does no damage (the docs said
the minimum 1 applies). "Already hit" comes back as 0, not 126. The armour halving is every X armour,
not "some armour states". And the sabre bonus also covers ids 37–41.

**A rule that the enemy sheets missed.** `ContactWeapon` skips every shot with `+0x9C` = 0 when the
object has `dmg_tbl_one_hit`. All the buster shots in the save states have 0 there. So the "any
hit destroys it" in seven enemy sheets is true only for the few weapons that set `+0x9C`. The
sheets are not changed yet: each shot must first be checked for the call it uses
(`ContactWeapon` or `SolidWeaponHit`).

Lessons:

- A rule that each sheet copies from the first sheet is worth one full read of the shared
  function. Here one branch at the top of `ContactWeapon` changed seven sheets.
- Count the callers. "Only player weapon objects damage objects" came from a scan of all 59
  overlays, not from the functions alone.

---

## 85. The contact model

The user asked how X6 knows what an object touches, and how this compares with the bodies and
fixtures of the Maverick engine. No new code was read: the X6 facts were already in
`docs/engine.md` and `docs/damage.md`, one rule at a time. We put them on one page,
`docs/contact-model.md`, next to the Maverick `WorldSystem`, `Body`, `Fixture` and contact
classes.

**X6 has no contact list.** Each object tests its own three boxes in its own update, and the
contact memory is a set of state bytes (`collide_flags`, the hit tag, `+0x76`). Maverick collects
all contacts in one pass and sends begin, continue and end events.

Lessons:

- A comparison with the target engine shows the port risks early: here the hits during one long
  overlap, the one-frame delays and the tile terrain.

---

## 86. The damage open questions

We worked on the open questions of `docs/damage.md`. A helper agent did the slow part (the weapon
ids, main type 59, the part bits and the sabre `+0x9C`) in the background, while the main session
did the two scans.

- **The seven sheets.** A call-graph walk from the entry of each enemy shot finds `ContactWeapon`
  in all of them and `SolidWeaponHit` in none. So plain buster shots pass through all of them,
  and the sheets now say so.
- **The counters.** `game+0x98` is the HP lost, also at deaths and squeezes; the mission report
  draws it. No reader of `game+0x9C` was found.
- **The `+0x9C` writers were given the wrong ids.** `weapon_types` holds one address per id for
  both player overlays. So an address in X's RAM must be matched against X's table: the two
  writers are ids 46 and 47, not 33 and 34.
- Weapon ids 32, 33, 34, 49, 56 and 58 now have names. Main type 59 is Gate (stage 18): a hit
  deflects and Gate gets its HP back; its own visual objects take the damage. The four part bits
  have names by effect; the in-game names are graphics and stay open.

Lessons:

- An address in the player overlay area means two things. Always say which RAM (X or Zero) it
  was read in.
- Background work is worth it only for a separate part with little shared context. Here the scans
  stayed in the main session, and the agent took the slow reading.

---

## 87. The sound effects

The user asked what it takes to extract and map the audio, and chose the order: sound effects
first, then the music. The sound effects are done: `python3 -m mmxdemake.sound` and
`docs/sound.md`.

- **Where they are.** The high half of an archive entry type is the loader handler, so a
  `0x2xxxx` entry is a VAB body. Its head is in a RAM entry before it, after a short prefix: the
  head offset, the bank slot, the body size and the bank's sound words. 116 heads, and in each one
  the sample sizes add up to the body size.
- **Three data surprises, each caught on the real data, each turned into a test first:** the
  program numbers have gaps (the tone blocks follow the programs that are used); a body is not
  always the next entry; and every sample ends with a silent block that loops, so play must stop
  at the first end block.
- **The map.** `PlaySound(bank, id)` reads word `id` of the bank's list, and the word gives the
  VAB, program and tone. The bank is the slot in the head prefix: a match of the word lists in
  five save states gave bank 0 common, 1 and 3 the player, 2 and 5 the stage.

Lessons:

- A format that has a known public layout (VAB, SPU ADPCM) is fast to read, but the game's own
  wrapper (the prefix, the handler in the type) must still come from the data.
- A check that the data agrees with itself (the sample sizes against the body size) is a strong
  test of a parser when you cannot listen to the output.

---

## 88. The XA music

`python3 -m mmxdemake.music` decodes the two XA files. `extract` had kept them as raw 2336-byte
sectors, so the subheaders (channel, coding, end flag) were already there.

- A survey of the subheaders first: `BGM.XA` is 8 stereo channels, `DEMO.XA` 16 mono channels,
  all 37 800 Hz 4-bit, and the end-of-file flag ends each record. The rest is the standard CD-XA
  ADPCM, written test first from small built sectors.
- Pure Python decodes about 1.9 million samples per second, and `BGM.XA` has about 626 million.
  One process per record on 16 CPUs takes the run from about 6 minutes to under 1 minute.
- The records of `BGM.XA` are 6–7 minutes long and have no mark between songs. So the songs can be
  cut only with the game's track table, the next step.

Lessons:

- Survey the container (here the subheaders) before you write a decoder: it gave the channel
  count, the coding and the record ends in one short script.
- Time a plain loop on the real data before you design the tool. The speed set the design (the
  process pool and `--channel`).

---

## 89. The track table

`StreamPlay(track)` reads a 16-byte row (start, end, channel, loop) and adds the file LBA from the
file table; a twin, `VoicePlay`, does the same for `DEMO.XA`. Two byte tables pick the track: one
per stage, area and character for the stage music, one per stage for the boss. `music` now writes
`songs.json` and `voices.json` from them.

- **The guess of section 88 was wrong.** The 6–7 minute records are not several songs: every one
  of the 30 rows starts and ends exactly at one record. One record is one song.
- The intro plays track 9; its boss plays track 21 (23 with `game+0x26`).

Lessons:

- A guess from the data alone ("no marks, so several songs") can be wrong in the other direction.
  Mark it as a guess, and let the code decide.

---

## Lessons so far

- Check the plan against the repo. The doc's "answer key" map was only a one-room test.
- Check licenses yourself. `mmx6` is AGPL too, and two repos have no license at all.
- Use hashes as an oracle. A Redump hash plus `mmx6`'s manifest let us prove each extraction step.
- Look at the data before you trust "unknown". `ROCK_X6.DAT` had a simple TOC that a short probe found.
- Read the confidence ratings. `mmx6` marks the PDF's RAM addresses as leads, not facts.
- Add one unknown at a time. Small, verifiable steps keep failures easy to diagnose.
- Keep data and code apart from the first commit: separate folder, a `.gitignore` that blocks dumps, and
  a separate repo for extraction.
- Check a reading of the code against the data. The drill's "floor break" turned out to be a rock
  block once we looked at the collision of the cells it copies.
- Make sure that the code you read is the code that runs. The first sprite drawer had no callers;
  the live one remaps the tiles.
- Give each search a fact to stop on. The table reader stopped only at a word outside the code
  area, and it read code as frames; checking each animation against its bank fixed it.
- Check a visual rule on the hardest case. The draw order looked right on a sprite whose pieces
  hardly overlap; the boss, with many overlaps, showed it was wrong.
- Pilot a tool on real data before trusting its output. Each of the three kit fixes came from one
  look at a real enemy, not from the synthetic tests.
- Let parallel workers report shared facts instead of writing them. Two agents named the same
  table differently; one merge from the data settled it.

---

## Suggested slides

1. Title: Demaking Mega Man X6 into Maverick: the first day.
2. The goal: X6 stages as Maverick extra levels, as close as NES style allows.
3. The research plan: two stages with a neutral middle.
4. Reality check: the plan versus the repo (the 50 × 25 "answer key").
5. Six sub-projects.
6. Repo layout: tools repo, Maverick, data folder, read-only references.
7. Licenses: AGPL, PolyForm and "no license", and why we only read them.
8. Disc identity: Redump, two tools and one SHA-1.
9. Our own ISO reader: 7 of 9 hashes on the first try, and the `.STR` miss explained.
10. Discovery: `ROCK_X6.DAT` has a TOC (243 members, no gaps).
11. Sub-headers and a palette lead.
12. Leads versus facts: the RAM addresses.
13. The user's concern and the choice: A (static), then C (visual), then B (runtime).
14. Lessons so far.
