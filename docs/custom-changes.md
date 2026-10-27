# Custom changes

This file lists the changes that make the game different from X6 on purpose. They are design
choices of the user, not guesses. A guess or a gap goes in `docs/gaps.md`. Do not change these
back to the X6 behaviour without asking the user.

The values are in `game/CustomChanges.java` unless the entry names another place.

| Change | X6 behaviour | This game | Where |
|---|---|---|---|
| No ceiling over the rocket platforms (Rainy Turtloid) | Collision 57 on the row at y 208 (one cell above the screen top) from x 3328 to 6031, so X cannot jump past the top of the screen | The row is cleared from x 4128 to 4991 (the open water with the rocket platforms) when the stage loads. The rest of the row stays | `CustomChanges.apply`, called by the `World` constructor before the pristine copy of the grid, so a restart keeps it |
| Faster sabre recovery | X's sabre poses 183, 184 and 187 run their frames at the normal speed | The frames with flag `0x40` (after the cut, where the cancel is allowed) run at 2 ticks per update (`SABRE_RECOVERY_SPEED`). The floor sabre takes about 24 updates in place of 40 | `Player.updateSabre` |
| More sabre damage | The damage table value of weapon 50 / 51 | The table value × 1.5 (`SABRE_DAMAGE_SCALE`), rounded to the nearest whole number (half up). Deflect and no-effect entries do not change | `SabreSlash.damageScale`, `Damage.hitObject` |
| D-1000 lands lower | The body lands at y 372 (the bottom of the landing sprite on the floor at y 416; `docs/gaps.md`, "D-1000") | The landing height is 12 px lower, y 384 (`D1000_LANDING_DROP`), so the hull does not look as if it floats. It applies to every landing: the entry, the crouch slam and the stun | `D1000.CROUCH_Y` |
| The first Nightmare Snake hops (Blaze Heatnix, `var` 5) | The ring rolls from wall to wall at 1 to 2 px/update, waits 60 updates at each wall, and the gems fire every 120 updates while it rolls (`docs/enemies/nightmare-snake.md`, ROLL) | The ring hops from one raised floor part to a random other one (x 416, 560, 720, 864; `SNAKE_BLOCKS`). A hop takes 64 updates (`SNAKE_HOP_TIME`) and goes 32 px high (`SNAKE_HOP_HEIGHT`, so the top gems stay on screen); after it lands it waits 30 updates (`SNAKE_LAND_WAIT`). When the gems fire, the ring stops where it is, also in the air, for 40 updates (`SNAKE_SHOT_PAUSE`) | `NightmareSnake.moveHop`, `NightmareSnake.move` |
| The first Nightmare Snake's gems take more damage | The `dmg_tbl_nightmare_snake` value (a lemon does 1) | The value × 3 (`SNAKE_DAMAGE_SCALE`), with the sabre scale on top, rounded once. Deflect entries do not change. Only the gems of `var` 5 | `NightmareSnake.Gem.damageTakenScale`, `World.resolveWeapons` |
| X falls straight down when an event holds him in the air | `PlayerEvent` is taken only by a ground step, so X keeps his air control until he lands (`docs/players/movement.md`, "The ground steps") | The hold starts at once, and X's sideways speed is set to 0, so he drops straight down and does not fly into the boss | `Player.startEvent` |
| Rocket platforms are full blocks (Rainy Turtloid) | Not given by the docs. An earlier version of this game had a one-way top [guess] | The platform is solid on all sides | `game/enemy/turtloid/RocketPlatform.java` |
