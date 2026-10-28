# The study of X6

This folder is a copy of the reverse-engineering (RE) work that the remake is built on. Do not edit
it here: `make docs` (`tools/import_docs.py`) deletes `findings/` and `work/` and copies them again.

| folder      | what                                                                                                                                                                                                         | copied from                       |
|-------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------|
| `findings/` | the results: one behaviour sheet per object (`enemies/`, `players/`), the stage docs (`stages/`), the engine (`engine.md`), damage and sound, the specs and plans of the tools, and the process log (`process/`) | `docs/` of `mmx-demake-tools`     |
| `work/`     | the work behind the results: one folder per topic with `notes.md`, `pseudocode.md`, `doc-changes.md` (the changes that the topic proposed for `findings/`), the C reconstructed from the disassembly (`reconstructed/`, `pseudo.c`), kit summaries and probe scripts | `re/` of the data root            |
| `work/notes/`, `work/stages/` | other notes and stage probe summaries                                                                                                                                                       | `notes/` and `stages/` of the data root |

Start with `findings/`. Each fact in it has a confidence tag (certain, likely or guess, see
`findings/re-guide.md`). Go to `work/<topic>/` to see how a fact was found.

## Why there are two sources

The tools repo `mmx-demake-tools` holds the tool code and the finished docs. The data root
(`~/mmx-data/x6`) is not in git: it holds the disc, the executable, the save states and the tool
output, which cannot go in a public repo. The tools write their work folders into the data root, so
the RE notes are there too.

## What the copy leaves out

- In `work/`: everything except `.md`, `.py`, `.sh`, `.c` and `.h` files. The disassembly
  (`.dis`), images, JSON and probe output are not copied, nor are the old copies of the tools docs
  in `re/`.
- The paths in the copies keep their text. A path in backticks to a copied file or folder (for
  example `~/mmx-data/x6/re/damage/`) becomes a relative link. Paths to files that are not copied
  (save states, sprites, the executable) and paths in code blocks stay as text.
- Some drafts in `work/*/doc-changes.md` link relative to `findings/` (for example `../engine.md`).
  These links do not work from `work/`.
