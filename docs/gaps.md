# Gaps and TODOs

Each line names what is guessed or missing, where it is in the code, and the source doc
(`~/IdeaProjects/mmx-demake-tools/docs/`). Code `TODO` comments point here.

## From the spec

- Shot sprites: `x-shots/` has no `animations.json`, and there is no export of the Falcon full
  charge (shot 19). Shot 19 uses the plain full charge (id 18) art and box. (`docs/players/attacks.md`)
- Sabre hit box: the docs give only "box from the frame flag". The box is a guess.
  (`docs/players/weapons.md`, "The other ids that X makes")
- D-1000 sprites: no combined export; animations 0–5 come from `d-1000`, 6–23 from `d-1000-parts`.
  (`docs/enemies/d-1000.md`, "Sprites")

## Guesses in the code

- Camera follow: the X6 follow rule is not read. The camera puts X at (160, 128) in the view,
  inside the limits, with no smoothing; only the limits ease (2 px/update).
  `game/CameraController.java` (`docs/engine.md`, "Camera")
- Bar hang snap: `0x800397F8` is not read; X's head is put 4 px into the bar cell.
  `game/player/Player.java` (`docs/players/air.md`, "Hang")
- Step down: the "16 px lower" floor test keeps X on a slope at any depth, but on a flat floor
  only for a drop of 8 px or less, so X falls off a 16 px ledge. `physics/TerrainMover.java`
  (`docs/engine.md`, "Tile collision")
- Ladder top: after the climb onto the top, X's feet are put on the ladder-top cell (the docs' 16 px
  move leaves them 5 px into it with this box). `game/player/Player.java`
  (`docs/players/ladder-hurt-events.md`, "Step 14")
- Falcon dash: always flies level (animation 171); the docs leave the rising and falling cases
  open. No hit part (weapon id 23). `game/player/Player.java` (`docs/players/air.md`, "Falcon dash")
- Dash end: X slides from 4.125 down by 0.1875 per update until the end animation stops; the
  timer rule of the docs is not used. Voices (bank 3) are not played.

## Drawn with shapes (no exported art)

- Explosions: circles that grow and fade for 16 updates. `game/fx/Explosion.java`
  (`docs/engine.md`, "Death, debris and drops")
- Death orbs: misc type 17 is not read; 3 waves of 8 circles at 2.5 px/update. `game/fx/Orb.java`
  (`docs/players/ladder-hurt-events.md`, "Death")
- Charge effect: the glow palettes (38, 41) are not drawn; 4 small blue circles orbit X while the
  buster charges, faster at level 2. `screen/StageScreen.java` (`docs/players/attacks.md`, "Charge")

## More guesses

- Respawn: the path from the death orbs back to the stage is not read; a 30-update black fade, then
  the restart at the last checkpoint. `game/StageFlow.java`
- Sabre sound: X's slash sound is not named; the Magma Blade slash sound (bank 1, id 23) is used.
  `game/player/Player.java` (`docs/players/weapons.md`, "Slot 3")
- Lemon muzzle flash (animation 39) is not drawn; the per-sprite buster offsets are not used (the
  standing offset, 21 px forward and 9 px up, is used for every pose). (`docs/players/attacks.md`)
- Player hurt box: the Falcon hurt-box table is not read; the terrain box is used.

## Skipped on purpose

- Item drops (health, weapon energy, extra lives): the drop weights exist, the drops are out of
  scope. (`docs/engine.md`, "Death, debris and drops")
