# Contact model: X6 and Maverick

This page tells how X6 makes an object's boxes and how an object knows what it touches. It then
compares this with the body and fixture model of the Megaman Maverick engine, for the port.

Sources:

- X6: `docs/engine.md` ("Boxes", "The standard main-object update", "Solid objects", "Tile
  collision") and `docs/damage.md`. The X6 facts have the confidence tags of those pages.
- Maverick: `engine/src/main/kotlin/com/maverick/game/engine/world/` (`WorldSystem.kt`,
  `body/Body.kt`, `body/Fixture.kt`, `contacts/Contact.kt`,
  `collisions/StandardCollisionHandler.kt`) and
  `core/src/main/kotlin/com/megaman/maverick/game/world/` (`body/BodyComponentCreator.kt`,
  `body/BodySense.kt`, `contacts/MegaContactFilter.kt`, `contacts/MegaContactListener.kt`).

## X6

### Boxes, not bodies

X6 has no body or fixture objects. Each object is one record of `0xA0` bytes. Contact uses three
box pointers in that record:

| field | box | format | used by |
|---|---|---|---|
| `+0x50` | attack box (touches the player) | `{s8 ox, s8 oy, u8 w, u8 h}`, top-left corner | `ContactMega` `0x80030ECC`, `BoxOverlap` `0x8002D654` |
| `+0x54` | hurt box (weapon hits) | the same | `ContactWeapon` `0x80031474`, `WeaponBoxTest` `0x8002D868` |
| `+0x68` | terrain box | centre and half sizes | `CollideCheck` `0x8002F288`, `SolidObject` `0x80031DA8` |

- **Building the boxes:** step 0 of the state routine sets the pointers to boxes in static tables.
  To change a box, the object writes a different pointer. To turn a box off, it writes a null
  pointer ("no test").
- **Facing:** the attack box and the hurt box are mirrored on x when the object faces right
  (`flip` = `0x40`). Nothing rotates.
- **One box of each kind:** an object that needs two hurt boxes swaps the pointer and calls
  `ContactWeapon` two times in one update (the Guardian).
- **Solid objects:** an object with `+0xA2` = 1 also puts its terrain box in `solid_rects`
  (`0x800E41B0`) at the start of each frame (`SolidRectsBuild` `0x8002E84C`). Only the object
  point tests read these rectangles.

### Each object runs its own tests

There is no world pass and no contact list. The update of each object calls the tests that it
needs, in a fixed order (`docs/engine.md`, "The standard main-object update"):

1. the step (the movement);
2. `CollideCheck`, when the object has a terrain box;
3. `ContactWeapon` (the weapon test) and the death test;
4. `ContactMega` (the contact test);
5. `SolidObject`, for solid objects.

The test and the reaction are in the same call. So each pair is resolved when the update of the
object gets to it:

- The objects see the player of the frame before.
- The player's own `CollideCheck` runs after all objects. So a solid object that moves in this
  frame pushes the player in the same frame.

### Contact memory

X6 keeps no list of contacts. The code sets and clears state bytes:

| what it does | X6 field |
|---|---|
| terrain contact: floor 8, ceiling 4, walls 1 and 2 | `+0x70` `collide_flags`, made again by each `CollideCheck` |
| the player touches a solid object | `mega+0x71` and `mega+0x72`: the player update clears them, then the objects OR them in |
| "the player touched me in the update before" | `obj+0x76` (player), `obj+0x77` (`actor2`); `SolidCarry` `0x800324C8` acts only when it is set |
| a shot hits only once while it overlaps | hit tag: shot `+0x64` against obj `+0x65`; `ContactWeapon` clears `+0x65` when no shot touches |
| no hits for a time after a hit | obj `+0x61` (`BoxOverlap` and `WeaponBoxTest` give no hit), `mega+0xA4` `hit_lock` |

### Terrain

Terrain is not made of bodies. `CollideCheck` tests points of the terrain box against the tile
collision values of layer 0:

1. the x move: 3 points on the side that moves;
2. the y move: the floor or the ceiling;
3. the corner fix-ups;
4. the touch flags.

A slope is a floor only. It acts through the centre point of the box (`SlopeSnap` `0x80030834`).
Some values have their own rules: 57 and 60 are solid with no wall slide or kick, and 62 and 63
kill unless `mega.form` = 2 (`docs/engine.md`, "Tile collision").

A solid object pushes the player out of its terrain box along the shorter axis
(`SolidPush` `0x80032208`). When the player is then between a floor and a ceiling, or between two
walls, the player goes back to the old position.

### Filtering

The filtering is in the code. An object touches only what it tests. Only player weapon objects call
`ApplyWeaponHit` (2 callers in the main program and the 59 overlays), so enemies cannot damage
other enemies (`docs/damage.md`).

## Maverick

### Bodies and fixtures

- A `Body` is a bounds rectangle with a `BodyType` (`STATIC`, `DYNAMIC`, `ABSTRACT`) and
  `PhysicsData` (gravity, velocity, velocity clamp, friction).
- The fixtures are stored by type: `fixtures: OrderedMap<Any, OrderedSet<IFixture>>`. A body can
  have any number of each type (`FixtureType`: `FEET`, `HEAD`, `SIDE`, `BODY`, `DAMAGER`,
  `DAMAGEABLE`, `SHIELD`, `BLOCK`, `DEATH`, and others).
- `Fixture.getShape()` puts the raw shape at `bodyAttachmentPosition` plus
  `offsetFromBodyAttachment`, and rotates it by `body.direction`. A fixture with
  `attachedToBody` = false keeps its own position.
- `setActive(false)` turns a fixture off. The Met does this with `SHIELD` and `DAMAGEABLE`.
- `BodyComponentCreator.create` makes the fixtures from `BodyFixtureDef`s. It links the body and
  each fixture to the entity, saves the position in a pre-process hook (`ConstKeys.PRIOR`), and
  resets the body senses in `onReset`.

### The world system

`WorldSystem` runs at a fixed step, one or more cycles in each frame. Each cycle:

1. `preProcess` of all bodies;
2. `process` of all bodies: friction, gravity, the velocity clamp, then the move;
3. a new broad-phase container with all bodies and all active fixtures;
4. `collectContacts`: only the fixture types that are keys of the filter map do a query
   (`shouldProceedFiltering`), and `filter` and `overlaps` decide each pair;
5. `processContacts`: in the current set but not in the prior set → `beginContact`; in both →
   `continueContact`; in the prior set only → `endContact`;
6. `resolveCollisions`: `StandardCollisionHandler` pushes each dynamic body out of each static
   body along the shorter axis, and adds the static body's friction;
7. `postProcess` of all bodies.

### Filtering and contact handling

- `MegaContactFilter` holds a map from a fixture type to the types that it can touch. It also
  blocks enemy-to-enemy contacts, has rules for `GATE` and `CART` (only with Megaman), and lets a
  fixture add its own filter (`setFilter`, `CONSUMER`).
- `MegaContactListener` handles each pair of types. It calls the functions that are stored on
  the fixture (`hitByBody`, `hitByFeet`, `hitByBlock`, the consumer). It sets the body senses
  (`BodySense.FEET_ON_GROUND`, `SIDE_TOUCHING_BLOCK_LEFT`, and others).
- Damage: on `beginContact` of `DAMAGER` and `DAMAGEABLE`, when `canBeDamagedBy` and `canDamage`
  are true, `takeDamageFrom` and `onDamageInflictedTo`.

## Compare and contrast

| topic | X6 | Maverick |
|---|---|---|
| shape model | up to 3 typed box pointers in a fixed record | any number of typed fixtures on a body |
| placement | offset from the origin, mirrored on x by facing | attachment point plus offset, rotated by direction |
| turn a shape off | write a null pointer | `setActive(false)` |
| who runs the tests | each object, in its own update | `WorldSystem`, for all bodies |
| when the reaction happens | at once, in the test call | after all bodies moved, through listener events |
| contact memory | state bytes that the code sets and clears | prior and current contact sets: begin, continue, end |
| which pairs can touch | the test functions that the object calls | the filter map and the custom filters |
| terrain | tile collision values, point probes, slopes | static `Block` bodies, box against box |
| solid object push | `SolidPush`: out along the shorter axis | `StandardCollisionHandler`: the same rule |
| order effects | the update order matters: objects see the player of the frame before, the player's `CollideCheck` runs last | the same phases for all bodies, so the order does not matter |
| damage gate | tested each frame, gated by the hit tag, `+0x61` and `hit_lock` | only on `beginContact`, gated by `canBeDamagedBy` and `canDamage` |
| no enemy-to-enemy damage | no enemy calls the weapon test | a rule in `MegaContactFilter.filter` |

## For the port

The X6 boxes map to Maverick fixtures:

| X6 | Maverick |
|---|---|
| attack box `+0x50` | `DAMAGER` |
| hurt box `+0x54` | `DAMAGEABLE` |
| terrain box `+0x68` | the body bounds, with `FEET`, `HEAD` and `SIDE` |
| `+0xA2` = 1, `SolidObject` | a `BLOCK` (static body) |
| `collide_flags` `+0x70` | the body senses |

Three behaviours need more work:

- **Hits during one long overlap.** In X6 a hit can come again during one overlap, when the hit
  tag is cleared or `+0x61` ends. Maverick damages only on `beginContact`. A long overlap needs
  `continueContact` and an invincibility timer.
- **The one-frame delays.** X6 objects see the player of the frame before, and the carry uses the
  contact of the update before (`+0x76`). Maverick resolves all in the same cycle. So moving
  platforms and squeeze cases can act a little differently.
- **Terrain.** The X6 slopes, the special values (57, 60, 62, 63) and the 3-point probes have no
  match in the box-against-box resolution. They need tile-aware fixtures or a custom
  `ICollisionHandler`.
