# Gaps and TODOs

Each line names what is guessed or missing, where it is in the code, and the source doc
(`~/IdeaProjects/mmx-demake-tools/docs/`). Code `TODO` comments point here.

## From the spec

- Shot sprites: `x-shots/` has no `animations.json`, and there is no export of the Falcon full
  charge (shot 19). Shot 19 uses the plain full charge (id 18) art and box. (`docs/players/attacks.md`)
- Sabre hit box: the docs give only "box from the frame flag", and the box table is not read. On
  each frame of the `x-sabre` animation with a flag that is not 0, the box is the bounding box of
  the drawn pixels of that frame; frames with flag 0 have no box. The floor slash has a box on
  frames 1–6 (15 updates). `game/player/SabreSlash.java`, `anim/SpriteBounds.java`
  (`docs/players/weapons.md`, "The other ids that X makes")
- Sabre slash animations: the code that picks the slash animation is not read. Floor (id 50) uses
  `x-sabre` animation 0, air (id 51) animation 1 and wall (id 51) animation 4, chosen by how they
  line up with X's sabre poses 183, 184 and 187. Animations 2, 3, 5 and 6–17 are not used. The
  slash starts at the pose frame with flag `0x80` and ends at the hold frame of its animation.
  `game/player/SabreSlash.java`
- D-1000 sprites: no combined export; animations 0–5 come from `d-1000`, 6–23 from `d-1000-parts`.
  (`docs/enemies/d-1000.md`, "Sprites")

## Guesses in the code

- Camera follow: the X6 follow rule is not read. The camera puts X at (160, 128) in the view,
  inside the limits, with no smoothing; only the limits ease (2 px/update).
  `game/CameraController.java` (`docs/engine.md`, "Camera")
- Buster point: `buster_ofs` (`0x8007673C`) gives a point per X sprite. Only two are used: stand
  (21 px forward, 9 px up) and crouch (31 px forward, 4 px down). Walk, jump, dash and wall shots
  start at the stand point; the table gives 20–42 px forward and 12 px up to 5 px down for them.
  `game/player/Player.java` (`docs/players/attacks.md`, "The shots")
- Camera zone 3: X6 deletes the zone when the camera x is more than 3840. With the centred follow,
  the camera x is 3840 or more for all trigger x (4000–4031), so the zone never acted and the
  camera followed X down in the upper ledge corridor. The check is not made; the zone acts like
  the others. `game/CameraZones.java` (`docs/stages/intro.md`, "Camera")
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
- Lemon muzzle flash (animation 39) is not drawn. (`docs/players/attacks.md`)
- Player hurt box: the Falcon hurt-box table is not read; the terrain box is used.

## Enemies

- Spawning: a record spawns when its point enters the 48 px band (not on every update while it is
  inside), so a killed enemy comes back only after its point leaves the band and enters it again.
  `game/Spawner.java` (`docs/engine.md`, "Spawning and despawning")
- Hover Gunner poses: which of animations 1–5 shows which aim is not documented; horizontal → 1,
  45° up → 2, up → 3, 45° down → 4, down → 5. `game/enemy/HoverGunner.java`
- Meta Wheel F: the stud overlay (misc type 2) is not drawn; the floor-and-ceiling squeeze death is
  not checked. `game/enemy/MetaWheelF.java` (`docs/enemies/meta-wheel-f.md`)
- Meta Dridler rock block: the tile copy and 4 explosions are in; the 60 updates of extra
  explosions, debris and screen shake are not. `game/enemy/MetaDridler.java`
- Enemy terrain uses the player's tile rules, not the object point tests (`docs/engine.md`, "Tile
  collision", object point tests).
- Sabre block art: cut from `banks/intro/intro-1_start/bank-1.png`, whose background colour
  (40, 40, 60) is made transparent. `render/Assets.java`

## Gates and stage start

- Boss gates: only the two intro ids are mapped (id 2: camera fixed at the gate x, y 224; id 1:
  x to x + 80). The trigger also needs X on the floor. During the pass X does not walk by himself;
  the gate moves him at 0.640625 px/update and he shows the walk animation.
  `game/enemy/BossGate.java` (`docs/enemies/boss-gate.md`, open question 2)
- Stage start: the READY banner is skipped; X beams in at once. (`docs/engine.md`, "Stage start")

## D-1000

- Floor height: the body lands at y 372 and stands at y 304. At 372 the bottom of the landed
  sprite (animation 8, 44 px below the origin) is on the floor at y 416. The sheet's crouched
  height (y 352) left the body 20 px above the floor. The terrain box is not used
  (`docs/enemies/d-1000.md`, open question 8). Bombs land at y 416.
- Animations: the sheet names only some of them. Used: 0 idle, 1 walk, 3 crouch shake, 4 charge,
  5 stun, 7 fall, 8 land, 9 stand up, 11 wreck; core 13 idle, 15 hit, 16 appear; core shot 19,
  bomb 14, fire pillar 20–22. The rest is a guess from the strips. `game/boss/D1000.java`
- WARNING: plain blinking text for 470 updates; the 22 rays are not drawn.
  `game/boss/BossWarning.java` (`docs/engine.md`, "Boss entry and exit")
- Not done: the rock fall and screen quakes at the landing, the core's energy particles, the RAM's
  screen shake, the bomb's landing rumble.
- RAM wind-up: the body moves toward a height 90 px above X, kept between y 256 and 352 (open
  question 3).
- Death: the blink and a simplified BossExplosion (explosions, white fade, the wreck, a fade back),
  then STAGE CLEAR. The second blink and explosion and Zero are cut. `game/boss/D1000.java`
  ("Death")
- Lemons and half charges hit the core and stop, with no effect (the sheet says only that they
  "do nothing to it").

## Skipped on purpose

- Item drops (health, weapon energy, extra lives): the drop weights exist, the drops are out of
  scope. (`docs/engine.md`, "Death, debris and drops")
