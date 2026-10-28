# RE kit: sheldon-main-32

- Object: sheldon-main-32
- State: `sheldon-6_tile-shown.state.gz` (SHA-1 `30a84a4ee7ae3b8cb97d2351487bac96270b9ef3`), stage 7 area 0
- Class: main, type 32; entry function 0x800f3e2c
- Functions walked: 4; state tables: 1

## Engine helpers called

| address | name | note | called from |
|---|---|---|---|
| 0x8002ca54 | DeleteObject2 | as DeleteObject; also clears the record kind high nibble when 0x10 | 0x800f3f34 |
| 0x8002cbfc | OffScreenCheck2 | OffScreenCheck2(obj, rx, ry): 1 if outside [-rx, 320+rx) x [-ry, 240+ry) | 0x800f3ca0 |
| 0x8002dcf4 | TerrainBoxOverlap | TerrainBoxOverlap(a, b): the terrain boxes of a and b overlap | 0x800f3ca0 |
| 0x8002f288 | CollideCheck | terrain collision from the move since prev_x/prev_y; sets collide_flags | 0x800f3e2c |
| 0x80031474 | ContactWeapon | weapon hits: 0, weapon id + 1, 126 already hit, 127 blocked, < 0 killed | 0x800f3ca0 |

## Calls to unnamed code outside the walk

| address | called from |
|---|---|

## State tables

| table | used by | targets |
|---|---|---|
| 0x800fc6e4 | 0x800f3e78 | 0x800f3e78, 0x800f3ca0, 0x800f3f34 |

## Neighbour entries (likely shots and effects of this object)

| class | type | function |
|---|---|---|

## Animation tables

### Table 0x800fc6e4 (0 animations)

| # | sprites | ticks | flags | loop to |
|---|---|---|---|---|

Note: cut: animation 0 uses sprite 164; the sprite set has 2

## Object fields used

| offset | name | reads | writes | functions |
|---|---|---|---|---|
| 0x4 | state | 1 | 0 | 0x800f3e2c |

## Globals and data

| address | name | used by |
|---|---|---|
| 0x80074a1c | dmg_tbl_nightmare_shadow+0xa0 | 0x800f3e78 |
| 0x800970a0 | mega | 0x800f3ca0 |
| 0x800970b4 | mega.layer | 0x800f3e78 |
| 0x800cd340 | actor2 | 0x800f3ca0 |
| 0x800fc6d4 |  | 0x800f3e78 |
| 0x800fc6dc |  | 0x800f3e78 |
