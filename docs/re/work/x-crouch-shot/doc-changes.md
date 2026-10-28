# Doc changes from `wolfang-4_x-crouch-shot`

Sources: the code in the state (`scripts/dis.py`: `0x8003B9AC`, `0x80035D38`…`0x80036048`,
`0x80049270`, `0x801ECE0C` in the X kit) and the live player and weapon objects (`notes.md`).

## `docs/players/movement.md`

Replace the paragraph under "### Crouch (step 26)":

````
Entered by `0x8003B9AC`: animation 81 (or 109 with the dash flag), step 26. Each update: `flip` from
the direction pressed, the requests, `0x8003F644` [open], then dash and jump. A dash from the crouch
starts at the dash's main phase (animation 128 in the shooting pose, else 16, at frame 5; the dust,
the dash sound, `+0x8C` = 1, sub-step 1). Else the Zero checks; with down released and a direction
held: walk start. Sub-steps: 0 crouch down (to sub-step 1 at the last frame), 1 crouched (shooting
versions 108, 139 for some weapons, the return to 81), and when down is released: animation 82 (110
for X with the dash flag), sub-step 2; stand at the last frame [certain for the code].
````

with:

````
Entered by `0x8003B9AC`: animation 81, or 109 when X is in the shooting pose (the halfword
`+0x8E` is not 0: `+0x8E` shot pose or `+0x8F` shot in this update), step 26. Each update: `flip`
from the direction pressed, the requests, `0x8003F644` [open], then dash and jump. A dash from the
crouch starts at the dash's main phase (animation 128 in the shooting pose, else 16, at frame 5; the
dust, the dash sound, `+0x8C` = 1, sub-step 1). Else the Zero checks; with down released and a
direction held: walk start. Sub-steps [certain for the code]:

- 0: crouch down; sub-step 1 at the last frame.
- 1: crouched. `DashTerrainBox` (`0x8003CD00`). In the update of a shot (`+0x8F`), X only:
  animation 139 when the fired id (`+0x96`) is 18, 19, 20, 43 or 46 (the full charges), else 108.
  Else, while `+0x8E` = 0 (no shooting pose), X only: animation 81 from frame 2
  (`PlayerSetAnimeAt`, each such update).
- Down released: animation 110 when X is in the shooting pose (the halfword `+0x8E` is not 0), else
  82; sub-step 2; stand at the last frame.

Seen in `wolfang-4_x-crouch-shot`: X (Falcon) in sub-step 1, animation 108 frame 1 (sprite 212),
`+0x8E` = 1, `+0x91` = 12, one lemon 8 updates after the shot [certain].
````

Replace the table row:

````
| 81, 82 | crouch down, stand up (108–110, 139: crouch with shot or dash flag) |
````

with:

````
| 81, 82 | crouch down, stand up (109, 110: the same in the shooting pose; 108: crouch shot; 139: crouch full-charge shot) |
````

## `docs/players/attacks.md`

Replace:

````
charges. Standing, X's buster is 21 px in front and 9 px above X's origin [certain].
````

with:

````
charges. Standing, X's buster is 21 px in front and 9 px above X's origin [certain]. Crouching
(sprites 209, 211, 212, 213: offset (−31, 4)), it is 31 px in front and 4 px below the origin
[certain: in `wolfang-4_x-crouch-shot` the lemon is at x = X + 31 + 8 × 6.75, y = X + 4].
````

## `docs/TODO.md` ("Player sprites")

Replace:

````
  `playerexport` run replaces `x-falcon/`, so the copy is not inside it); a tool path needs a source per live weapon or effect
  object, the shared animation table `0x8007789C`, and a streamed frame set for the Falcon full
  charge (type 19), which no state has on screen.
````

with:

````
  `playerexport` run replaces `x-falcon/`, so the copy is not inside it); a tool path needs a source per live weapon or effect
  object, the shared animation table `0x8007789C`, and a streamed frame set for the Falcon full
  charge (type 19), which no state has on screen.
- [ ] Tool: `spriteexport` exports a live lemon (weapon id 0, `wolfang-4_x-crouch-shot`) as
  `enemy-54284bb54d` with all 40 animations of `shot_anime_tbl` (`0x8007789C`). Only 21 (lemon) and
  39 (lemon muzzle flash) use the lemon's sprite data (directory `scratch+0x1C` entry 1) and CLUT
  `0x7802`; the half-charge flash (22) uses entry 3, the full-charge flashes (23, 25) entry 2
  (muzzle table `0x80077C0C`: {entry, animation, sound} per var). Fix: keep only the animations of
  the object's own sprite data (a catalog rule per entry, or the animation of `+0x34`). Do not
  copy that export into `sprites/enemies/` until then (`~/mmx-data/x6/re/x-crouch-shot/notes.md`).
````

## `catalog/helpers.json`

Replace:

````
    "0x8003B9AC": {"name": "StartCrouch", "note": "animation 81 (109 with the dash flag), step 26"},
````

with:

````
    "0x8003B9AC": {"name": "StartCrouch", "note": "animation 81 (109 in the shooting pose: halfword mega+0x8E not 0), step 26"},
````
