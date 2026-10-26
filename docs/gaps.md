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

- Camera follow: only the y line is read (`cam+0x2E` = 171, the screen y where the camera keeps the
  player). The x line (160) is a guess, and the follow has no dead zone or smoothing.
  `game/CameraController.java` (`docs/engine.md`, "Camera")
- Camera limits: `CameraLimitEase` (`0x80028EBC`) as `docs/engine.md` gives it for max y
  (`0x80029180`): a limit moves relative to the camera; when the player is more than 1 px past the
  follow line toward a farther target, the limit goes to the target at once; else it keeps up with
  the player's move plus `cam+0x47`. Min y, min x and max x use the same rule mirrored (the docs mark
  them [likely]). Before 2026-10-25 every limit moved 2 px per update, which made the Yammark rope
  pan slow and killed X in the pit below it. `game/CameraController.java`
- Air dash start: the `x-falcon` export of animation 169 (frame set 1) shows other sprites (a few
  loose pixels), so X looked invisible for its 8 updates. The game still plays 169 for the timing,
  but the first frame of animation 171 is drawn. `render/Assets.java` (`docs/players/air.md`,
  "Animations used here")
- Hit flash: the game draws a hit object for 1 update with the fixed flash CLUT `0x7840` (a ramp
  from (140, 222, 255) to white, read from VRAM in three save states). The exports are RGBA, not
  palette indexes, so a shader maps the brightness of each pixel onto that ramp. The D-1000 death
  blink uses the same flash (before, the body was hidden). `render/FlashShader.java`
  (`docs/engine.md`, "Hit flash")
- Buster point: the points of `buster_ofs` (`0x8007673C`, frame set 1: `0x8007693C`) are copied
  into `game/player/BusterPoints.java` from the executable. The point comes from X's sprite after
  the shot pose starts (the walk sprites have no point; their shooting versions have one). A sprite
  with no point uses the stand point (21 px forward, 9 px up). The frame set of an animation
  (set 1: 83–85, 140, 144–192) is from `docs/engine.md`, "Frame sets".
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

- Explosions: `enemies/explosion/` (effect type 4) animation 0, 46 updates up to its hold frame.
  Frame 0 (1 update) is not drawn: it is a flash frame for the fixed flash CLUT `0x7840`, which
  the export does not draw. All explosions use this one variant ("random variant" is not read). The
  shot spark (effect type 3) is not exported and is still drawn as circles.
  `game/fx/Explosion.java` (`docs/engine.md`, "Death, debris and drops")
- Death orbs: misc type 17 is not read; 3 waves of 8 circles at 2.5 px/update. `game/fx/Orb.java`
  (`docs/players/ladder-hurt-events.md`, "Death")
- Charge effect: the glow palettes (38, 41) are not drawn; 4 small blue circles orbit X while the
  buster charges, faster at level 2. `screen/StageScreen.java` (`docs/players/attacks.md`, "Charge")

## More guesses

- Draw order: the docs do not say whether a higher draw priority is in front (`docs/engine.md`,
  open question 5). Objects with priority 0 or more are drawn behind X (higher first), and objects
  with a negative priority (the sabre slash, shots, the charge flash, explosions, debris) in front of
  X. `screen/StageScreen.java`
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

- Boss gates: all 51 cases of the limits table, the push speeds and the open conditions (`game+0x26`,
  `game+0x28`) are in; the time stamp, the special parts of ids 18, 25, 31 and 39 (stages not done)
  are not. The trigger also needs X on the floor. During the pass X does not walk by himself; the gate
  moves him and he shows the walk animation. `game/enemy/BossGate.java` (`docs/enemies/boss-gate.md`,
  open question 2)
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
- Death: the blink, then `BossExplosion` with flags `0x81` (below), the sink of 0.25 px/update
  and the wreck at the white screen, then STAGE CLEAR when the explosion object is gone. X is not
  moved at the white screen. The second blink, the second explosion and Zero are cut.
  `game/boss/D1000.java` ("Death")
- Boss explosion: the states, times, explosions and white fade follow `docs/engine.md`
  ("`BossExplosion`"). Not done: the rays (types 4, 6 and 8 have no sprite data), `Rumble(10)`,
  `DeleteSysObjects`, `game.player_lock`. The 8 burst explosions come in the first 8 of the 9
  burst updates. `game/fx/BossExplosion.java`
- Lemons and half charges hit the core and stop, with no effect (the sheet says only that they
  "do nothing to it").

## Stages

- TMX: the Yammark and Turtloid exports of `~/mmx-data/x6/tmx` have no texture split, so the parts
  after their texture swap (Yammark camera zone `id` 132 at (592, 1568); Turtloid zone `id` 136 at
  (3648, 224)) drew with the wrong texture block (for example both boss rooms). The copies in
  `assets/tmx/` are made again by `mmxdemake.tmx` from a scratch copy of the catalog with these
  splits (texture `0x16`, regions set by eye from both drawings): Yammark `[0, 592, 1100, 6656,
  2304]`, `[1, 592, 1100, 6656, 2304]`, `[2, 700, 700, 6656, 2304]`; Turtloid `[0, 3520, 0, 9472,
  1536]`, `[1, 3520, 0, 9472, 1536]`, `[2, 4400, 576, 9472, 1536]`. The tools catalog
  (`catalog/stages.json`) is not changed. A few BG cells keep a few wrong colours (the palettes come
  from the start state). `DataPaths.stageTmx`
- Turtloid BG layer 2: the cells beside the pagoda (layer 2 x 140–260, y 495–545) and beside the
  gate roof over the sea wall (x 1600–1970, y 255–300) show garbage with both texture blocks;
  probably VRAM that other art covers in play. Marked in the TMX (`TODO` layer).
- Stage select, `Progress`: lives, souls, Reploid status and armour parts are kept for the session.
  Lives are shown and counted but a game over is not made; parts have no effect.
- Nightmare Effect: fixed per stage (Yammark 0, Turtloid 0, Heatnix 4, the value of the Heatnix
  save states). So the Heatnix grey blocks exist and the Nightmare Bugs do not; the rain of Effect 6,
  the shadows of Effect 7 and the darkness of Effect 8 are not made. `stage/Stages.java`
- Items (main type 47), talk zones (Alia), palette cycles, scenery (Turtloid misc 20): not made.
- Screen shake: `Rumble` and `Quake` / `Quake2` are not done (Monbando's slam, the grey block, the
  rolls of Rainy Turtloid and Nightmare Snake).
- Shots that hit shots: the Worm Spider ball and the Sea Attacker bubble have 1 HP for weapons with
  `+0x9C` ≠ 0 (`docs/damage.md`); the buster passes through them, and they are made unhittable.
- Solid objects: `World.solidObject` follows `SolidObject` (`docs/engine.md`, "Solid objects") after
  each solid moves: no push when X's centre is inside, else a push along the shorter overlap; when
  that puts X into terrain, X goes back, takes the squeeze damage (the object's contact damage, else
  127) and gets the 75-update squeeze timer (`mega+0xE5`), during which the stage objects skip the
  push (the boss gate does not). The player's own moves stop at a solid only when X moves into it
  from outside (`physics/TerrainMover.java`). Not done: the touch flags, `actor2`, and the carry under
  a solid (`0x04`) and at its sides. Moving solids carry X on top by their move (`World.carry`).
  The tools docs disagree on `mega+0xE5`: `caterpirade.md` and `rocket-platform.md` say it starts
  after any hit, `engine.md` and `ladder-hurt-events.md` say only after a squeeze; this code follows
  the second.
- Maverick text: the text box of each Maverick before the fight is not shown.
- Maverick end: after the boss soul (`docs/enemies/nightmare-virus.md`, BOSS SOUL) the player exit
  (step 25, the victory music) is not read; STAGE CLEAR shows. `game/boss/Maverick.java`
- Sound banks during a Maverick fight: bank 2 is the boss member's bank (Yammark `m075-e02` and
  Turtloid `m080-e02`, read from bank 2's word list in `yammark-2_boss` and `turtloid-7_boss`;
  Heatnix `m077-e02` is a guess by the member order 75–82). `stage/Stages.java`
- Sound rate: each tone plays at 44100 × 2^((note_min − centre + fine/128)/12) Hz (`PlayTone` keys
  `SsUtKeyOnV` `0x8005A9D4` at byte 6 of the tone, not at the centre note, so `docs/sound.md` is
  wrong there); ADSR, volume and pan are not applied. `audio/SoundBanks.java`, `audio/WavRate.java`

## Shared stage objects

- Rescue Reploid: the 0–3 sub tank units of a rescue are not given (no sub tanks). The heal pauses
  the other objects and locks X. `game/enemy/RescueReploid.java` (`docs/enemies/rescue-reploid.md`)
- Nightmare Virus: the two after-images of the revival and the warp, the boss soul's light rays and
  the Hard WARP strike are not made. `game/enemy/NightmareVirus.java`
- Light capsule: Dr. Light's voice (stream 28) and the text box are a 150-update message; the
  player's "get part" sequence (event step 24) is 60 + 40 updates of the capsule animations; the part
  pictures (effect type 35) are not drawn. `game/enemy/LightCapsule.java`
- Teleporter crystal: drawn only; the area change (`game.exit_req` `0x41`) is not made, and the
  Turtloid alternate section (Zero Nightmare, High Max) is out of scope.
  `game/enemy/TeleporterCrystal.java`

## Commander Yammark's stage

- Worm Spider: the water jumper (`id` 2) is not made (no record uses it here).
- Killer Moth: the ENTER variant (`id` ≠ 0) is not made (no record uses it).
- Nightmare Insect: its boxes are not centred; they are mirrored with the facing. Only weapon ids 1,
  10 and 54 destroy it, so with X's buster and sabre it only makes the husk (as in the game).
- Commander Yammark: the Zero patterns (drone lance) and the stun (ids 8, 17, 44) are not made; his
  box is mirrored with the facing. Drones turn to their attack point on the circle and attack at the
  second arrival; the drone circle and bob timings are hand-read. `game/enemy/yammark/`

## Rainy Turtloid's stage

- Acid rain: the curtain (BG layer 1) is drawn at half alpha in front of layer 0 and the objects
  (the docs do not give the layer order; the user saw that the rain must show in the caves too), and
  the palette fade to the rain colours (`PaletteDarken`, `0x800ED18C`) is not done.
  `game/enemy/turtloid/AcidRain.java`
- Batton Bone B81: the set-up does not set the facing, so the bats face left.
- Monbando: X is the player, so the attack roll picks the slam or the missile (no punch).
- Health capsule: a 48 × 16 px block on the floor; X jumps onto it (the push-out of the solid body
  stops a walk onto it).
- Rainy Turtloid: Easy and Normal patterns only (no Hard jumps, no RAIN ROLL); the stun (ids 2, 11,
  33) is not made (special weapons); the bubble start points use 16 even points on the 24 px circle
  in place of the tables `0x800F6AD8` / `0x800F6B18`. His shell box is a separate object that
  deflects. `game/enemy/turtloid/RainyTurtloid.java`

## Blaze Heatnix's stage

- Blaze Heatnix: no behaviour sheet and no sprite export (no save state reached his room). A
  stand-in (shapes, 32 HP, it only hovers) ends the stage. `game/enemy/heatnix/BlazeHeatnix.java`
- Nightmare Snake: `var` 4 (wall run), 6 (hops) and 7 (roll with gravity) follow the short notes
  in `~/mmx-data/x6/re/heatnix-mini-boss/pseudo.c`; their sub-steps are not read. The snake waits
  until a gate pass ends before it holds X. The rising-room flame and the lava are BG layer 2 drawn
  in front of layer 0 at 60 % / 75 % alpha. `game/enemy/heatnix/NightmareSnake.java`
- Nightmare Bug: not made (Nightmare Effect 4 deletes the records).

## Skipped on purpose

- Item drops (health, weapon energy, extra lives): the drop weights exist, the drops are out of
  scope. (`docs/engine.md`, "Death, debris and drops")
