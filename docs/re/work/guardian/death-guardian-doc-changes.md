# Death Guardian: proposed changes to files that the helper does not own

## `catalog/enemies.json`, `names`

Add:

```
"enemy-2dadc31ecb": "death-guardian",
```

The export `enemy-2dadc31ecb` (13 animations, table `0x800FC428`) has bank 8 of stage 7 (13 Sheldon
states) and bank 7 of stage 22 (`sheldon-10`, `sheldon-11`): the second bank of main type 30. The next
`spriteexport` run renames the folder.

## `catalog/helpers.json`

Replace the notes of `0x800EFE48` (`GuardianInit`), `0x80095098` (`bank_tpage`) and `0x800FCFF0`
(`guardian_box_closed`) with the notes in `death-guardian-helpers-proposed.json`. No new names.

## `docs/TODO.md`

- Line 133 ("the large Guardian (`id` 16, (3928, 664)) live"): mark done; state
  `sheldon-16_death-guardian`; official name Death Guardian.
- Line 147 ("an export only for its first bank; the large Guardian (`id` 16, bank 8) needs an
  export"): mark done. `spriteexport` now exports the bank directly after the first bank of a type
  when that bank has the same type and a loaded texture (export `death-guardian`).
- Line 171 (Rainy Turtloid, "the next two sprite banks"): not changed by this work. Type 57 is listed
  twice (banks 14 and 15) but both have no texture in the Turtloid states, so the new rule adds
  nothing there.

## `docs/engine.md`, "A type can have two sprite banks" (line 496)

Add at the end:

> `spriteexport` exports such a second bank (bank + 1, same type, texture loaded) with the same
> animation tables; the source has `bank` = the second bank. In `sheldon-16_death-guardian` a normal
> Guardian set-up had written the box globals after the Death Guardian set-up, so the Death Guardian
> used the normal boxes with contact 7 [certain for the values].

Change "the Guardian with `var` ≥ 16" to "the Death Guardian (Guardian `var` ≥ 16)".

## `docs/stages/sheldon.md`

Record 102 (`id` 16, (3928, 664)): name it Death Guardian, link
`docs/enemies/guardian.md#death-guardian-id--16`, state `sheldon-16_death-guardian`.

## `catalog/savestates.json`

If not there yet: `sheldon-16_death-guardian` (stage 7, area 0, X form 0, Death Guardian live in
main slot 12, ATTACK step2 2, one shot live).

## `docs/process/process-log.md`, a new section (draft)

> Death Guardian and second sprite banks. The user named the large Guardian (`id` 16) the Death
> Guardian and saved a state with it live. The helper read every `var` use of the Guardian code: 3
> places; only the set-up differs (contact 7, large boxes, bank + 1 for texture, palette and sprite
> data). HP, timing and shots are the same. The state confirmed bank 8 values and showed the shared
> box globals in play: the Death Guardian had the normal boxes. `spriteexport` now exports the
> second bank of a type listed twice (bank + 1, same type, texture loaded). The helper ran the old
> and the new code on all 33 dumps into two new roots with one catalog copy: `diff -rq` showed only
> the new folder `enemy-2dadc31ecb` and the new ID in the `enemies` lists of `stages/sheldon.json`
> (13 states) and `stages/sheldon-alt.json` (2 states); every other file is byte-identical.
