# RE kit: logic type 17

- Object: logic type 17
- State: `gate-4_before-gate.state.gz` (SHA-1 `f51fcc1d7dd4d15662dc4a3ecd18beb124cd7419`), stage 18 area 0
- Class: logic, type 17; entry function 0x800ee300
- Functions walked: 6; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x80016c48 | PlaySound | PlaySound(bank, id, obj): -1 when sound_bank_vab[bank] = -1; -2 when id is past the bank's word list (sound_words); -5 for a zero word; word byte 0 >> 6 indexes sound_kind_handlers: 1, 2 -> PlaySequence, 0 -> PlayTone (docs/sound.md) | 0x800ee1d4, 0x800ee478, 0x800ee604 |
| 0x8002b3d4 | PaletteCopy | PaletteCopy(slot, src, n): n palettes of 32 bytes into the palette buffer (*scratchpad+0x28) at slot; bg_refresh |= 1 | 0x800ee478, 0x800ee538, 0x800ee604 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|
| 0x8002b410 | 0x800ee478, 0x800ee538, 0x800ee604 |

## State tables

| table | used by | targets |
|---|---|---|
| 0x800f6df0 | 0x800ee300 | 0x800ee3a0, 0x800ee478, 0x800ee538, 0x800ee1d4, 0x800ee604 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

No animation table: the kit finds tables only for the main and stage classes.

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x0 | flags | 1 | 1 | 0x800ee3a0 |
| 0x2 | var | 1 | 0 | 0x800ee3a0 |
| 0x4 | state | 8 | 6 | 0x800ee1d4, 0x800ee300, 0x800ee3a0, 0x800ee478, 0x800ee538, 0x800ee604 |
| 0x14 | layer | 2 | 3 | 0x800ee300, 0x800ee478 |
| 0x16 | priority | 1 | 3 | 0x800ee1d4, 0x800ee478 |
| 0x18 | prev_x | 1 | 3 | 0x800ee1d4, 0x800ee478 |
| 0x1a | self+0x1a | 6 | 4 | 0x800ee478, 0x800ee538, 0x800ee604 |
| 0x1e | self+0x1e | 1 | 1 | 0x800ee300, 0x800ee478 |
| 0x20 | speed_x | 3 | 2 | 0x800ee3a0, 0x800ee478, 0x800ee538, 0x800ee604 |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x1f800030 | extra_palettes | 0x800ee3a0 |
| 0x8008eafc | text_open | 0x800ee1d4 |
| 0x800970a0 | mega | 0x800ee1d4 |
| 0x800971f8 | camera_layer0 | 0x800ee300, 0x800ee478 |
| 0x800ccef6 | game.stage_flag | 0x800ee1d4, 0x800ee478, 0x800ee538, 0x800ee604 |
| 0x800f7798 |  | 0x800ee3a0, 0x800ee478, 0x800ee538, 0x800ee604 |
| 0x800f77b8 |  | 0x800ee478, 0x800ee538, 0x800ee604 |
