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
| Rocket platforms are full blocks (Rainy Turtloid) | Not given by the docs. An earlier version of this game had a one-way top [guess] | The platform is solid on all sides | `game/enemy/turtloid/RocketPlatform.java` |
