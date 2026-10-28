/* Conveyor belt and forced scroll, stage 4 area 1 (overlay 07). Reconstructed C.
 * Original code; written from the disassembly of the save states. Not compiled. */

#include <stdint.h>

typedef struct { int8_t ox, oy; uint8_t w, h; } Box;

typedef struct {
    uint8_t flags, type, var, display;
    uint8_t state, step, step2, pad7;
    int32_t x, y;                              /* 16.16; px at +0x0A, +0x0E */
    uint8_t pad10[4];
    uint8_t sound_on;                          /* +0x14 */
} LogicObj;

typedef struct {
    uint8_t flags, type, var, pad3;
    uint8_t state, step, pad6[14];
    int16_t target_x;                          /* +0x14 */
} Kind7Obj;

typedef struct {
    uint8_t flags, type, var, display;
    uint8_t state, step, step2, pad7;
    int32_t x, y;
    uint8_t pad[0x5C - 0x10];
    uint8_t hp;                                /* +0x5C */
    uint8_t pad2[0x68 - 0x5D];
    Box *terrain_box;                          /* +0x68 */
    uint8_t pad3[0xCC - 0x6C];
    int8_t cc;                                 /* +0xCC */
    uint8_t pad4[0xE0 - 0xCD];
    int8_t e0;                                 /* +0xE0 */
    uint8_t pad5[0xF4 - 0xE1];
    int32_t walk_speed;                        /* +0xF4, 16.16 */
    uint8_t pad6[0x14C - 0xF8];
    int16_t sound_reset;                       /* +0x14C */
} Player;

typedef struct {
    uint8_t pad[4];
    uint8_t mode;                              /* +0x04 */
    uint8_t pad5[3];
    int32_t x;                                 /* +0x08, 16.16 */
    int16_t y_frac, y;                         /* +0x0C, +0x0E */
    uint8_t pad10[0x1C - 0x10];
    int16_t max_x, min_x, max_y, min_y;        /* +0x1C */
    int16_t tmax_x, tmin_x, tmax_y, tmin_y;    /* +0x24 */
    uint8_t pad2c[0x4C - 0x2C];
    uint8_t redraw;                            /* +0x4C */
} Layer;

extern Player   mega;                          /* 0x800970A0 */
extern Layer    layers[3];                     /* 0x800971F8, 0x54 each */
extern uint8_t  game[];                        /* 0x800CCED0 */
extern uint32_t frame_count;                   /* 0x80097420 */
extern uint16_t belt_turns;                    /* 0x800E3142 */
extern uint16_t belt_frame;                    /* 0x800F985C */
extern uint32_t *tile_defs;                    /* scratchpad +0x0C */

extern const uint16_t belt_places[8][2];       /* 0x800F8C08 */
extern uint16_t belt_tiles[8][4];              /* 0x800F981C */
extern const int16_t roller_dst[20][4];        /* 0x800F88E8: dx, dy, w, h */
extern const uint16_t roller_src[20][8][2];    /* 0x800F8988 */

#define GAME_CHECKPOINT game[0x1D]
#define GAME_FLAG26     game[0x26]
#define PX(v)           ((int16_t)((v) >> 16))

int  TileCollisionAt(void *obj, int x, int y);
int  TileIdAt(int layer, int x, int y);
void CopyTiles(int layer, int dx, int dy, const int16_t src[4]);
void PlaySound(int bank, int id, void *obj);
void DeleteObject3(void *obj);

static void BeltSaveFrames(void)
{
    for (int i = 0; i < 8; i++) {
        int x = belt_places[i][0], y = belt_places[i][1];
        belt_tiles[i][0] = TileIdAt(0, x, y);
        belt_tiles[i][1] = TileIdAt(0, x + 16, y);
        belt_tiles[i][2] = TileIdAt(0, x, y + 16);
        belt_tiles[i][3] = TileIdAt(0, x + 16, y + 16);
    }
}

static void BeltRotateTiles(void)
{
    uint32_t tmp[4];
    if (frame_count % 5 != 0)
        return;
    for (int k = 0; k < 4; k++)
        tmp[k] = tile_defs[belt_tiles[0][k]];
    for (int i = 1; i < 8; i++)
        for (int k = 0; k < 4; k++)
            tile_defs[belt_tiles[i - 1][k]] = tile_defs[belt_tiles[i][k]];
    for (int k = 0; k < 4; k++)
        tile_defs[belt_tiles[7][k]] = tmp[k];
    belt_turns++;
}

static void BeltDrawRollers(void)
{
    belt_frame &= 7;
    for (int i = 0; i < 20; i++) {
        int16_t src[4] = {
            (int16_t)roller_src[i][belt_frame][0], (int16_t)roller_src[i][belt_frame][1],
            roller_dst[i][2], roller_dst[i][3] };
        CopyTiles(0, roller_dst[i][0], roller_dst[i][1], src);
    }
    layers[0].redraw = 1;
}

void BeltUpdate(LogicObj *obj)                 /* 0x800ED998, stage logic 9 */
{
    if (obj->state == 0) {
        belt_frame = belt_turns;
        BeltSaveFrames();
        BeltDrawRollers();
        obj->state++;
    }
    if (mega.e0 == 2 || mega.state != 1 || mega.hp == 0)
        return;
    if (mega.step == 20) {
        if (obj->sound_on) {
            PlaySound(5, 3, 0);
            obj->sound_on = 0;
        }
        return;
    }
    if (mega.step == 0)
        return;
    if (GAME_CHECKPOINT == 0)
        layers[0].tmin_x = layers[0].min_x = PX(layers[0].x);
    if (mega.terrain_box == 0)
        return;
    if (GAME_CHECKPOINT >= 2) {
        PlaySound(5, 3, 0);
        DeleteObject3(obj);
        return;
    }
    if (obj->sound_on == 0 || mega.sound_reset != 0) {
        PlaySound(5, 6, 0);
        obj->sound_on = 1;
    }
    BeltRotateTiles();
    if (frame_count % 5 == 0) {
        belt_frame++;
        BeltDrawRollers();
    }
    {
        Box *b = mega.terrain_box;
        int foot = PX(mega.y) + b->oy + b->h;
        if (TileCollisionAt(&mega, PX(mega.x), foot) == 58)
            mega.x -= 0x10000;                 /* only the pixel half is written */
    }
}

void AutoScroll(Kind7Obj *obj)                 /* 0x800E9C98, kind-7 type 3 */
{
    Layer *l0 = &layers[0];
    if (mega.cc != 0 || mega.e0 == 2)
        return;
    switch (obj->state) {
    case 0:
        if (GAME_CHECKPOINT == 1) {
            layers[1].mode = 5;
            obj->target_x = l0->max_x;
            obj->state++;
        }
        break;
    case 1:
        if (mega.step == 2) {
            mega.walk_speed += 0x8000;
            obj->state++;
        }
        break;
    case 2:
        if (GAME_FLAG26) {
            mega.walk_speed -= 0x8000;
            DeleteObject3(obj);
        } else if (PX(l0->x) < obj->target_x) {
            l0->x += 0x8000;
            l0->max_x = l0->min_x = l0->tmax_x = l0->tmin_x = PX(l0->x);
        }
        break;
    }
}
