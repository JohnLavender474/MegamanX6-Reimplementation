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

- Camera follow: read from the code, not in the docs. The layer 0 mode 1 (`0x80029538`) keeps the
  player between screen x 160 and 160 (`cam+0x30`, `+0x32`) and screen y 96 and 171 (`cam+0x2C`,
  `+0x2E`), moves the camera at most `cam+0x48` = 8 px per update (`0x800295E8`, `0x800296A8`),
  then clamps it to the limits (`0x8002820C`). Mode 0 (`0x800294B4`) sets `cam+0x47` = 4. All 21
  checked save states agree. When `cam+0x44` (follow) is 0, the camera and its limits do not move.
  Before 2026-10-26 the camera went to the player at once, so a limit that opened made the camera
  jump. Not done: Nightmare Snake `var` 10 lowers the 171 line by 64. `game/CameraController.java`
- Camera limits: `CameraLimitEase` (`0x80028EBC`), read from the code. Max y is as
  `docs/engine.md` gives it. The other three differ from the docs' "same rule mirrored": a limit
  within `cam+0x47` of its target goes to it at once; min x and max x, when the target is farther,
  go to the target at once unless the eased value passes it (then they take the eased value); min y,
  when the target is above the camera and the player is below screen y 96, takes the value player
  screen y − 96 (a small number), as the code does. `game/CameraController.java`
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
- Rope hang: the hang test point is not read. The rope (tile 46, step 27) is tested at the head, as
  the bars are. X snaps to the tile centre, climbs at 1.5 px/update, dashes at 4.125 for up to 30
  updates and stops at the rope ends. The shooting pose (animation 144) is not used.
  `game/player/Player.java` (`docs/players/air.md`, "Hang: rope and bar")

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
  (`docs/enemies/d-1000.md`, open question 8). Bombs land at y 416. The user's custom change
  lowers the landing by 12 px more (`docs/custom-changes.md`).
- Animations: the sheet names only some of them. Used: 0 idle, 1 walk, 3 crouch shake, 4 charge,
  5 stun, 7 fall, 8 land, 9 stand up, 11 wreck; core 13 idle, 15 hit, 16 appear; core shot 19,
  bomb 14, fire pillar 20–22. The rest is a guess from the strips. `game/boss/D1000.java`
- WARNING: plain blinking text for 470 updates; the 22 rays are not drawn.
  `game/boss/BossWarning.java` (`docs/engine.md`, "Boss entry and exit")
- Not done: the rock fall and screen quakes at the landing, the core's energy particles, the RAM's
  screen shake, the bomb's landing rumble.
- RAM wind-up: the body moves its y 2 px per update toward X's y, with no offset and no limit, as
  `0x800EE58C` does. Before 2026-10-26 it moved toward 90 px above X (kept between y 256 and 352),
  which took the body half off the top of the screen.
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
  (3648, 224)) drew with the wrong texture block (for example both boss rooms). The maps in
  `assets/tiled/tmx/` are made again by `mmxdemake.tmx` from a scratch copy of the catalog with these
  splits (texture `0x16`, regions set by eye from both drawings): Yammark `[0, 592, 1100, 6656,
  2304]`, `[1, 592, 1100, 6656, 2304]`, `[2, 700, 700, 6656, 2304]`; Turtloid `[0, 3520, 0, 9472,
  1536]`, `[1, 3520, 0, 9472, 1536]`, `[2, 4400, 576, 9472, 1536]`. The tools catalog
  (`catalog/stages.json`) is not changed. A few BG cells keep a few wrong colours (the palettes come
  from the start state). `make assets` never replaces these maps. `tools/import_assets.py`
- Turtloid BG layer 2: the cells beside the pagoda (layer 2 x 140–260, y 495–545) and beside the
  gate roof over the sea wall (x 1600–1970, y 255–300) show garbage with both texture blocks;
  probably VRAM that other art covers in play. Marked in the TMX (`TODO` layer).
- TMX, Blizzard Wolfang: the boss room background above the floor (x 6320–6688, y 304–420) draws
  as garbage blocks with the tiles of the export. The cause is probably the texture block, as in the
  Yammark and Turtloid boss rooms; a split from a boss-room save state (`wolfang-6`, `wolfang-7`) is
  needed in the tools' TMX export. Marked in the TMX (`TODO` layer).
- Stage select, `Progress`: lives, souls, Reploid status and armour parts are kept for the session.
  Lives are shown and counted but a game over is not made; parts have no effect.
- Nightmare Effect: fixed per stage, from the save states of each stage (Yammark 0, Wolfang 7,
  Heatnix 4, Scaravich 4, Turtloid 0, Sheldon 1). So the Heatnix and Scaravich grey blocks, the
  Wolfang shadows and the Sheldon bugs exist; the rain of Effect 6, the crates of Effect 5, the fire
  ring of Effect 3 and the darkness of Effect 8 are not made. `stage/Stages.java`
- Items (main type 47), talk zones (Alia), palette cycles, scenery (Turtloid misc 20): not made.
- Screen shake: `Rumble` and `Quake` / `Quake2` are not done (Monbando's slam, the grey block, the
  rolls of Rainy Turtloid and Nightmare Snake, the ice block hops and rows, Blizzard Wolfang's
  landing).
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
  in front of layer 0 at 60 % / 75 % alpha. The first snake (`var` 5) hops in place of the X6
  roll: a custom change (`docs/custom-changes.md`). `game/enemy/heatnix/NightmareSnake.java`
- Nightmare Bug: not made (Nightmare Effect 4 deletes the records).

## Blizzard Wolfang's stage

- Ice slide: the slide (stage logic 7) starts when the player step changes to stand, crouch, land
  or dash on collision 60; the real arm state reads the step each update. The slope push and the
  foot-of-slope rule follow `docs/engine.md`, "Slippery floors and slopes (stage logic 7)".
  `game/enemy/IceSlide.java`
- Nightmare Shadow: the death draws two copies that move apart at 1 px/update; one flickering copy
  stands in. `game/enemy/NightmareShadow.java` (`docs/enemies/nightmare-shadow.md`)
- Wolfloid: X is the only player, so the slope wolves (`id` 0, 1) always use the cannon; Zero's
  breath steps are not made. `game/enemy/wolfang/Wolfloid.java`
- Ice stack block: the camera field `+0x2C` (+48 / −48 in the shaft) is not read, so the shaft camera
  uses only the limits. `game/enemy/wolfang/IceShaftController.java`
- Ice fall controller: the rumble loops (bank 5, ids 8 and 9) play once at the start of each fall;
  their stop rules and the avalanche camera shake are not done. `game/enemy/wolfang/IceBlock.java`
- Snow scenery (misc 23): drawn from the sprite-only export `enemy-cd98e86270` (sprite = `id`)
  behind X, as the other objects; its draw order against the tiles and X is not read. The export of
  `wolfang-6`/`-7` is wrong (banks 4–8 overwritten by the boss texture), so the `wolfang-5` export is
  used. `game/enemy/wolfang/SnowScenery.java`
- Blizzard Wolfang: boss level 0 only, so the floor ice spikes (`var` 16) and the wall spikes of the
  wall runners (visual 20, `var` 32, 33) are not made (they exist at level 2 and 3). The trail images
  (main 54 `var` 1) use flag `0x10` and the next bank's palette (semi-transparent [guess]); they are
  drawn as plain blinking copies. The room walls, floor and ceiling are fixed lines from the room
  origin, not terrain tests. The text box before the fight is not shown ("Maverick text").
  `game/enemy/wolfang/BlizzardWolfang.java` (`docs/enemies/blizzard-wolfang.md`)
- Nightmare Fire Ring (main 19): Nightmare Effect 3 only; the stage runs with effect 7, so it is
  not made.

## Ground Scaravich's stage

- Scaravich routing: the poles, the roll (`route_tbl`), the section and return tables and the random
  blocks follow `docs/stages/scaravich.md`; the block table (place and source of each block) is read
  from the code with `~/mmx-data/x6/re/scaravich-crystals/probes/sections.py`. Guesses: `FadeOut(8)`
  and `FadeIn(8)` take 32 updates each; the player faces the pole at a return point and right at a
  section entry; the player's shots are not cleared at the teleport. The roll uses `rng % 12`, not
  `GetRNG() % 12` (the tiny bias of the real roll is lost). The poles cleared, the row and the roll
  phase are kept over a restart, as the doc says. `game/enemy/scaravich/Sections.java`,
  `SectionTeleport.java`
- Section goal: its boxes are not read (the drawn statue, 28 × 40 px, stands in), and its 60
  updates of effects are an explosion every 8 updates and debris every 20. `SectionGoal.java`
- Totem pole: the period of the heads is 40 also on Hard; the game here is always Normal. The base
  is solid on all its height (the real one only while X is within 48 px; no other body comes near).
  `TotemPole.java` (`docs/enemies/totem-pole.md`)
- Falling rock and rock block: there is no sprite export of bank 5, so the sprites are cut from the
  numbered bank sheet (`sprites/banks/scaravich/scaravich-3_section/bank-5.png`; 68 × 107 px cells,
  the sprite at the top left under the label). The sheet loses the sprite origins: the rocks use the
  centre of the drawn pixels, and the blocks put the bottom of the drawn pixels on the bottom of
  their hurt box. `anim/BankSheetAnims.java`, `render/Assets.java`
- Ground Scaravich: his hurt and contact boxes are not in the docs; a 40 × 52 px box over the drawn
  body stands in. The walk-in sounds, the rolling sound timer, the text box, the stun (ids 1, 10,
  54, no X weapon), the Hard balls (gold, purple) and the extra falling balls of boss levels 1–3 are
  not made. `GroundScaravich.java`
- Not made: the shaft tile animators and the room dimmer (stage logic 29), the palette cycles, the
  talk zones, the items (main 47), stage object 26 and the hidden area (stage 21).

## Shield Sheldon's stage

- Laser puzzle: one class for all parts (main 13). The lasers are caught through their head box,
  not through a probe in the weapon pool; the parts and the lasers update in the entity order, so
  the on and off times can differ by 1–2 updates (the docs leave the order open). The draw pieces
  are made each update from the sprite-only export `enemy-e5920c31ed`; the body piece of length L
  that starts at P has its origin at P + 112 px along the laser, which the export's sprite bounds
  show. `game/enemy/sheldon/LaserPuzzle.java`, `Laser.java`
- Hidden glass floor: weapon id 44 (Zero's step 59) also shows a floor; X has no such weapon. When
  the timer ends while X still stands on it, the floor starts again in the same update (the real one
  in the next). `HiddenGlassFloor.java`
- Guardian: two hurt boxes; the shield is the object itself and the body is a second object after it
  in the list, so a shot meets the shield first, as in the game. The shot is not hittable (the real
  one breaks from weapons with `+0x9C` set, the charged shots). The shared box globals (a Death
  Guardian with normal boxes) are not copied. `Guardian.java`
- Mettaur D2: its shot is not hittable (as the Guardian shot). `MettaurD2.java`
- Shield Sheldon: the stun (ids 4, 13 and 35, no X weapon), the Hard speeds, the text box, the
  rumbles and the warp-guard step 2 (not used) are not made. The plates, the image and the turrets
  have no damage table in the game, so a shot that touches them stops and makes them react; here they
  use the normal table with HP set back each update, which gives the same result. Their contact
  damage is 0 (the docs say 1 after the scaling [likely]). The second shield of the high catch waits
  for the first one; at `act4` 2 it flies to the floor point as the table says. `ShieldSheldon.java`
- Not made: the start scenery (misc 14), the Nightmare crates (Nightmare Effect 5 only), the palette
  cycles (stage logic 3), the talk zones, the item, the hidden area (stage 22).

## Front end

Source: `docs/engine.md`, "Front end", and `~/mmx-data/x6/re/front-end/notes.md`. The order (logo,
load, opening, title, menu), the fades (32 updates: `FadeTask` moves a level of 0–255 by the speed
8 per update), the 600-update title, the menu rows, the menu sounds (bank 0, ids 12 and 34) and the
18 001-update menu idle are from the code. `frontend/FrontEnd.java`, `screen/FrontEndScreen.java`

- Logo: `BootTask` plays `CAPLOGO.STR` with the movie player directly; the 90 frames and the Start
  skip are taken from `movie_table` row 0, which has the same file. `FrontEnd.java`
- Load: the black load of stage 14 after the logo has no known length; 60 updates stand in. The
  text "Now Loading..." is in a resident VRAM page, but no code that draws it is known. It is shown
  16 px in from the bottom right, cut from `vram-512-256-clut7800.png` with the menu CLUT `0x7800`
  (its real CLUT is not known). `FrontEnd.java`, `FrontEndText.java`, `FrontEndScreen.java`
- Opening skip: the movie and its sound stop at the press and the title starts at once, as the
  movie player's skip buttons (`movie_table`) suggest. The sound 34 comes from the front-end skip
  rule; whether the movie player plays it is not known. `FrontEnd.java`
- Presses during a fade do nothing (one rule for all phases). X6 likely takes Start during the
  title fade-in: title sub-step 2 opens the skip lock together with `FadeIn(8)`, and the skip rule
  only skips its own `FadeOut` while a fade runs. Here a Start in the first 32 title updates is
  lost. `FrontEnd.java`
- Movies: the frames run on the update count (4 updates per frame, 15 fps). The sound
  (`audio.wav`) plays on its own clock, so the two can drift apart. `FrontEndScreen.java`
- Title: after the 600 updates, X6 runs a demo (state 2) and then the member 0 picture (state 3).
  Neither is made; the title goes back to the opening. A menu idle also goes to the opening.
  `FrontEnd.java`
- Blink: PRESS START BUTTON and the menu button line show 16 of each 32 updates; the phase (shown
  while bit `0x10` of the phase's update count is clear) is a guess. `FrontEnd.java`
- Menu: CONTINUE (state 7, memory card) has no screen; a pick on it does nothing. OPTION (state 8)
  opens a custom screen in place of X6's (`docs/custom-changes.md`). The two new-game cheats (Ultimate Armor, Black Zero) are not made. `FrontEnd.java`
- Music: no XA track plays on the title or the menu [likely: the stream play flag `0x800E2E5C` is 0
  in `title-4_press-start` and `title-5_main-menu`, and 2 in a stage]. The stage select screen keeps
  track 1.
- Fade: X6 draws a subtractive rectangle with the fade level; here it is black with alpha
  level / 255. `Fade.java`
- Text sprites: `misc19-sprites.png` loses the sprite origins; the sizes and origins in
  `FrontEndText` come from `re/front-end/scripts/draw_misc19.py` on `title-5_main-menu`.
- Display: the 512 × 240 title and menu are stretched to the 4:3 window (x × 1.875, y × 3), as a
  PS1 shows display mode 1.

## Skipped on purpose

- Item drops (health, weapon energy, extra lives): the drop weights exist, the drops are out of
  scope. (`docs/engine.md`, "Death, debris and drops")
