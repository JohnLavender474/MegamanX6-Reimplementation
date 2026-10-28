/*
 * Ice stack block: stage 2 (overlay 04) stage object type 7, and the ice shaft controller
 * (stage logic type 8). A plausible reconstruction from the disassembly in kit/code.txt and
 * at/*.txt. It is not original code and it was not compiled. Field names follow
 * docs/engine.md; the include files are those of ~/mmx-data/x6/re/purple-wolf/include/.
 * Comments mark hazards only (docs/re-guide.md, "Hazards").
 */
#include "object.h"
#include "game.h"
#include "sound.h"

#define SPRITE_TYPE_ICE_STACK 134

extern StateFn ice_stack_states[5];      /* 0x800F6B9C */
extern StateFn ice_stack_steps[10];      /* 0x800F6BD8 */
extern AnimeTable ice_stack_anims;       /* 0x800F6B6C */
extern u8 ice_stack_junk[3];             /* 0x800F6B8C: 5, 6, 7 */
extern Box ice_stack_terrain_box;        /* 0x800F6B90: (0, 0, 24, 24) */
extern Box ice_stack_hurt_box;           /* 0x800F6B94: (-24, -24, 48, 48) */
extern Box ice_row_terrain_box;          /* 0x800F6B98: (0, 0, 120, 24) */
extern s16 ice_row_offsets[5][4];        /* 0x800F6BB0 */
extern TileRect ice_stack_tiles_edge;    /* 0x800F6C00: {6016, 1568, 3, 4} */
extern TileRect ice_stack_tiles;         /* 0x800F6C08: {6016, 1584, 3, 3} */
extern s16 ice_row_gap_x[5];             /* 0x800F6C10: 96, 48, 0, -48, -96 */
extern StateFn ice_sparkle_states[3];    /* 0x800F6C1C */
extern StateFn ice_shaft_states[3];      /* 0x800F6C34 */
extern TileRect ice_shaft_door_tiles;    /* 0x800F6C40: {6144, 1264, 3, 3} */
extern Object ice_stack_player_copy;     /* 0x800F76EC, 0x154 bytes */

#define GEN_COUNT(o)   ((o)->act4)
#define COL_COUNT(o, c) (((u8 *)(o))[0x86 + (c)])
#define PARENT(o)      ((Object *)(o)->work7C)
#define TIMER(o)       (((u8 *)(o))[0x84])

/* 0x800EB96C: 56 when (x, y) is inside the terrain box of another live solid stage object. */
static u8 StageBlockAt(Object *self, s16 x, s16 y)
{
    int i;
    Object *o = stage_objects;

    for (i = 0; i < 80; i++, o++) {
        Box *b;
        s16 cx, cy;

        if (o == self || o->flags == 0 || o->solid_rect == 0 || o->terrain_box == NULL)
            continue;
        b = o->terrain_box;
        cx = (o->flip == 0x40) ? o->x.px - b->ox : o->x.px + b->ox;
        cy = o->y.px + b->oy;
        if (x < cx - b->w || x > cx + b->w - 1)
            continue;
        if (y < cy - b->h || y > cy + b->h - 1)
            continue;
        return 56;
    }
    return 0;
}

static void CopySpriteSource(Object *dst, Object *src)
{
    dst->tex_cord = src->tex_cord;
    dst->anime_table = src->anime_table;
    dst->sprite_data = src->sprite_data;
    dst->clut_cord = src->clut_cord;
}

/* 0x800EBAA0, state 0 */
void IceStackBlockInit(Object *self)
{
    int i;

    self->step = 2;
    self->state = 1;
    self->flags = 0x51;
    self->flip = 0;
    self->layer = mega.layer;
    LoadSpriteSource(self, SPRITE_TYPE_ICE_STACK);
    self->hp = 32;
    self->damage_table = dmg_tbl_capsule;
    self->hurt_box = &ice_stack_hurt_box;
    self->priority = 6;
    self->iframes = 0;
    self->anime_table = &ice_stack_anims;
    self->attack_box = NULL;
    self->terrain_box = (Box *)&self->work80;
    self->work80 = ice_stack_terrain_box;
    self->field75 = 0;
    self->contact_damage = 6;
    self->last_hit_tag = 0;
    SetAnime(self, 0);
    self->solid_rect = 1;
    self->speed_x = 0;
    self->speed_y = -0x10000;
    self->field85 = 0;
    TIMER(self) = 0;
    for (i = 5; i >= 0; i--)
        COL_COUNT(self, i) = 0;

    switch (self->var) {
    case 0:
        self->act4 = 0;
        self->step2 = 8;
        self->terrain_box = NULL;
        break;
    case 1: case 2: case 3: case 4: case 5:
        for (i = 0; i < 4; i++) {
            Object *b = GetStageObjectAfter(self, 0);

            if (b == NULL)
                continue;
            b->flags = 0x41;
            b->type = 7;
            b->var = 10;
            CopySpriteSource(b, self);
            b->x.px = self->x.px - ice_row_offsets[self->var - 1][i];
            b->work7C = (u32)self;
            b->y.px = self->y.px;
        }
        self->speed_y = -0x10000;
        self->act4 = 0;
        self->terrain_box = (Box *)&self->work80;
        self->work80 = ice_row_terrain_box;
        self->hurt_box = NULL;
        self->state = 3;
        self->display = 0;
        break;
    case 10:
        self->contact_damage = 16;
        self->act4 = 0;
        self->step2 = 1;
        break;
    case 11:
        self->contact_damage = 127;
        self->step2 = 4;
        self->terrain_box = NULL;
        self->hurt_box = NULL;
        self->display = 0;
        self->act4 = 0;
        SetAnime(self, TerrainPointCheck(self, self->x.px, self->y.px - 26) ? 1 : 0);
        break;
    case 12: {
        Object *w = GetMainObject();

        if (w != NULL && (GetRNG() & 1) == 0) {
            w->flags = 0x41;
            w->type = 6;
            w->var = 4 + (GetRNG() & 1);
            w->x.px = self->x.px;
            w->y.px = self->y.px;
            LoadSpriteSource(w, w->type);
            w->work80 = (u32)self;
        }
        self->step2 = 6;
        self->speed_y <<= 1;
        break;
    }
    default:
        break;
    }
}

static int PlayerOnFloor(void)
{
    ice_stack_player_copy = mega;
    CollideCheck(&ice_stack_player_copy);
    return ice_stack_player_copy.collide_flags & 0x08;
}

/* 0x800EBF18, state 1 */
void IceStackBlockMain(Object *self)
{
    Object *parent;
    s16 dx;

    parent = PARENT(self);
    ice_stack_steps[self->step2](self);

    if (self->step2 == 4 || self->step2 >= 8) {
        self->display = 0;
        return;
    }
    if (self->step2 == 5) {
        self->display ^= 1;
        return;
    }

    self->clut_cord &= 0x7FFF;
    SolidWeaponHit(self);
    if (!OffScreenCheck2(self, 128, 128))
        DisplayCheck1(self);
    if (self->var == 12)
        SolidMainObjects(self);
    SolidVisualHit(self);
    SolidPushShots(self);

    if (mega.squeeze_timer == 0) {
        dx = mega.x.px - self->x.px;
        if (dx < 0)
            dx = -dx;
        if (dx < 48)
            SolidObject(self);
    }

    if (self->var == 10) {
        /* HAZARD: parent (the row) is a raw pointer. Safe in the game: the row sets act4 = 1 and
         * deletes itself only after all its blocks have left step2 1, and the blocks do not
         * read the parent in steps 2 and 3 except here; a deleted row slot reads act4 = 0. */
        if (parent->act4 == 4) {
            SpawnJunk(2, ice_stack_junk, self);
            DeleteObject3(self);
            return;
        }
        if ((self->touch72 & 0x04) && PlayerOnFloor())
            parent->act4 = 4;
        return;
    }

    if ((self->touch72 & 0x04) && self->step2 == 6 && PlayerOnFloor()) {
        SpawnJunk(3, ice_stack_junk, self);
        /* HAZARD: parent (the generator) may already be deleted (state 2 after the player left
         * the pit). Then these two decrements write into a free or reused stage object slot.
         * In the game a block falls for at most about 180 updates, and the generator ends only
         * when the player is out of the pit, so a crush after the end is rare [likely]. */
        parent->act4--;
        COL_COUNT(parent, self->act4)--;
        self->state = 4;
    }
}

/* 0x800EC1DC, step2 0 (var 6-9; no user found) */
void IceStackBlockSlide(Object *self)
{
    u8 below_r, below_l, side_r_up, side_l_up, side_r, side_l;
    Box *b = (Box *)&self->work80;

    MoveObject(self);
    CollideCheck(self);
    below_r = StageBlockAt(self, self->x.px + b->ox + b->w - 1, self->y.px + b->oy + b->h);
    below_l = StageBlockAt(self, self->x.px + b->ox - b->w + 1, self->y.px + b->oy + b->h);
    side_r_up = StageBlockAt(self, self->x.px + b->ox + b->w, self->y.px + 16);
    side_l_up = StageBlockAt(self, self->x.px + b->ox - b->w - 1, self->y.px + 16);
    side_r = StageBlockAt(self, self->x.px + b->ox + b->w, self->y.px);
    side_l = StageBlockAt(self, self->x.px + b->ox - b->w - 1, self->y.px);

    if ((self->speed_x > 0 && (side_r_up || (self->collide_flags & 1))) ||
        (self->speed_x < 0 && (side_l_up || (self->collide_flags & 2))) ||
        (self->speed_x > 0 && (side_r || (self->collide_flags & 1))) ||
        (self->speed_x < 0 && (side_l || (self->collide_flags & 2)))) {
        self->speed_x = 0;
        self->step2 = 7;
        self->x.px = (self->x.px & 0xFFF0) | 8;
        return;
    }
    if (!below_r && !below_l && !(self->collide_flags & 0x08)) {
        self->speed_y = -0x10000;
        self->x.sub = 0;
        self->speed_x = 0;
        self->step2 = 6;
        self->x.px = (self->x.px & 0xFFF0) | 8;
    }
}

/* 0x800EC4DC, step2 3: freeze into map tiles */
void IceStackBlockFreeze(Object *self)
{
    int edge_above;

    AnimeTick(self);
    if (++TIMER(self) == 32) {
        if (self->var == 11)
            SetAnime(self, TerrainPointCheck(self, self->x.px, self->y.px - 26) ? 3 : 2);
        else
            SetAnime(self, 2);
    }
    /* HAZARD: the end depends on the event flag of the last frame of animations 2 and 3
     * (0x800F6B6C). Animations 0 and 1 (shown before timer 32) have flag 0. If a frame of
     * animation 0 or 1 had flag 1, the block would turn into tiles at once. */
    if (self->anime.flag != 1)
        return;
    edge_above = 0;
    if (self->var == 11)
        edge_above = TerrainPointCheck(self, self->x.px, self->y.px - 26);
    if (edge_above)
        CopyTiles(0, self->x.px - 24, self->y.px - 24, &ice_stack_tiles);
    else
        CopyTiles(0, self->x.px - 24, self->y.px - 32, &ice_stack_tiles_edge);
    camera_layer0.redraw = 1;
    self->state = 2;
}

/* 0x800EC628, step2 6: fall */
void IceStackBlockFall(Object *self)
{
    Box *b = (Box *)&self->work80;
    u8 below_r, below_l;

    MoveObject(self);
    if (self->y.px < 1296)
        return;
    CollideCheck(self);
    below_r = StageBlockAt(self, self->x.px + b->ox + b->w - 1, self->y.px + b->oy + b->h - 1);
    below_l = StageBlockAt(self, self->x.px + b->ox - b->w, self->y.px + b->oy + b->h - 1);
    if (TileCollisionAt(self, self->x.px - 32, self->y.px) &&
        TileCollisionAt(self, self->x.px + 32, self->y.px))
        self->contact_damage = 127;
    if (below_r || below_l) {
        self->y = self->prev_y;
        return;
    }
    if (self->collide_flags & 0x08) {
        self->step2 = 7;
        self->y.sub = 0;
        self->x.sub = 0;
        self->speed_x = 0;
        self->speed_y = 0;
        self->y.px = (self->y.px & 0xFFF0) | 8;
        if (self->display)
            PlaySound(2, 17, self);
    }
}

/* 0x800EC7DC, step2 9: pit generator */
void IceStackGenerator(Object *self)
{
    int period = (game.difficulty < 2) ? 128 : 64;
    u8 c = GetRNG() % 6;
    int first = self->x.px < 5312;
    int max_col = first ? 3 : 4;
    int max_all = first ? 18 : 24;
    int base_x = first ? 5016 : 5368;
    s16 x;
    int k;

    if (TIMER(self) % period == 0 && GEN_COUNT(self) < max_all) {
        x = base_x + 48 * c;
        if (COL_COUNT(self, c) == max_col) {
            c = (c + 1) % 6;
            for (k = 0; k < 5; k++) {
                x = base_x + 48 * c;
                if (COL_COUNT(self, c) != max_col)
                    break;
                c = (c + 1) % 6;
            }
        }
        {
            Object *b = GetStageObjectAfter(self, 0);

            if (b != NULL) {
                b->flags = 0x41;
                b->type = 7;
                b->var = 12;
                CopySpriteSource(b, self);
                b->x.px = x;
                b->y.px = 1248;
                b->act4 = c;
                b->work7C = (u32)self;
            }
        }
        /* HAZARD: the counts go up also when GetStageObjectAfter found no free slot. Then a
         * column counts a block that does not exist and can never be filled. Safe in the game
         * while the 80 stage object slots are not all used [likely]. */
        COL_COUNT(self, c)++;
        GEN_COUNT(self)++;
    }

    if (first) {
        if (mega.x.px >= 5281 && camera_layer0.target_max_x == 4976) {
            camera_layer0.target_max_x = 5328;
            camera_layer0.target_min_x = 5328;
            self->state = 2;
        }
    } else {
        if (mega.x.px >= 5633 && camera_layer0.target_max_x == 5328) {
            camera_layer0.target_max_x = 5712;
            camera_layer0.target_min_x = 5712;
            self->state = 2;
        }
    }
    TIMER(self)++;
}

/* 0x800ECC10, state 3: row (var 1-5) */
void IceRowLand(Object *self)
{
    Object *b;
    s16 y;

    if (self->act4 == 4) {
        /* HAZARD: work7C is the shaft controller; no test. Safe: only stage logic 8 makes rows. */
        PARENT(self)->step--;
        self->state = 2;
        return;
    }
    if (self->y.px < 369)
        return;
    CollideCheck(self);
    if (!(self->collide_flags & 0x08))
        return;
    if (self->display)
        PlaySound(2, 17, self);
    self->act4 = 1;
    self->terrain_box = NULL;

    b = GetStageObject();
    if (b != NULL) {
        b->flags = 0x41;
        b->type = 7;
        b->var = 11;
        b->x.px = self->x.px + ice_row_gap_x[self->var - 1];
        b->y.px = self->y.px;
    }
    y = self->y.px;
    if ((u16)(y - 977) < 46 || (u16)(y - 737) < 46) {
        b = GetStageObject();
        if (b != NULL) {
            b->flags = 0x41;
            b->type = 7;
            b->var = 11;
            b->x.px = self->x.px - 144;
            b->y.px = self->y.px;
        }
        y = self->y.px;
    }
    if ((u16)(y - 833) < 46 || (u16)(y - 689) < 46) {
        b = GetStageObject();
        if (b != NULL) {
            b->flags = 0x41;
            b->type = 7;
            b->var = 11;
            b->x.px = self->x.px + 144;
            b->y.px = self->y.px;
        }
    }
    self->state = 2;
}

/* 0x800ECDE8, entry */
void IceStackBlock(Object *self)
{
    self->prev_x = self->x;
    self->prev_y = self->y;
    if (mega.special == 2)
        return;
    ice_stack_states[self->state](self);
}

/* 0x800ECE48 (state 2) and 0x800ECE68 (state 4) */
void IceStackBlockDelete(Object *self)
{
    DeleteObject3(self);
}

/* 0x800ECE88, step2 1: a row block falls with its row */
void IceRowBlockFall(Object *self)
{
    Object *row = PARENT(self);

    if (row->y.px != self->y.px)
        row->y.px = self->y.px;
    MoveObject(self);
    if (row->act4 == 1) {
        self->step2 = 2;
        self->y.sub = 0;
        self->x.sub = 0;
        self->speed_x = 0;
        self->speed_y = 0;
        self->y.px = (self->y.px & 0xFFF0) | 8;
        if (self->display)
            Quake2(16, 3, 1);
    }
}

static void SpawnSparkle(Object *self)
{
    Object *m = GetMiscObject();

    if (m == NULL)
        return;
    m->flags = 0x41;
    m->type = 9;
    m->flip = self->flip;
    m->tex_cord = self->tex_cord;
    m->clut_cord = self->clut_cord & 0x7FFF;
    m->anime_table = self->anime_table;
    m->sprite_data = self->sprite_data;
    m->layer = self->layer;
    m->x.sub = 0;
    m->x.px = self->x.px;
    m->y.sub = 0;
    m->work50 = (u32)self;
    m->y.px = self->y.px;
}

/* 0x800ECF20, step2 2: row block or gap filler has landed */
void IceStackBlockSettleFast(Object *self)
{
    SpawnSparkle(self);
    self->damage_table = dmg_tbl_capsule;
    self->priority = 7;
    TIMER(self) = 31;
    self->step2 = 3;
}

/* 0x800ECFE8, step2 4: gap filler, test the place */
void IceGapFillerCheck(Object *self)
{
    self->display = 0;
    if (TerrainPointCheck(self, self->x.px, self->y.px)) {
        self->state = 2;
        return;
    }
    self->display = 1;
    self->hp = 32;
    self->step2 = 5;
}

/* 0x800ED044, step2 5: gap filler, boxes on */
void IceGapFillerArm(Object *self)
{
    self->damage_table = dmg_tbl_capsule;
    self->hurt_box = &ice_stack_hurt_box;
    self->terrain_box = (Box *)&self->work80;
    self->work80 = ice_stack_terrain_box;
    self->step2 = 2;
}

/* 0x800ED08C, step2 7: pit block has landed */
void IceStackBlockSettle(Object *self)
{
    SpawnSparkle(self);
    self->damage_table = dmg_tbl_capsule;
    self->priority = 7;
    TIMER(self) = 0;
    self->step2 = 3;
}

/* 0x800ED150, step2 8: pit generator waits for the player */
void IceStackGeneratorWait(Object *self)
{
    s16 lock;

    if (self->x.px < 5312) {
        if (mega.y.px < 1473 || mega.x.px < 5137)
            return;
        lock = 4976;
    } else {
        if (mega.y.px < 1489 || mega.x.px < 5345)
            return;
        lock = 5328;
    }
    camera_layer0.target_min_x = lock;
    camera_layer0.target_max_x = lock;
    self->step2 = 9;
}

/* 0x800ED1EC: sparkle, misc type 9 entry */
void IceSparkle(Object *self)
{
    self->prev_x = self->x;
    self->prev_y = self->y;
    ice_sparkle_states[self->state](self);
}

/* 0x800ED238, sparkle state 0 */
void IceSparkleInit(Object *self)
{
    Object *block = (Object *)self->work50;

    self->display = 1;
    self->priority = 0;
    self->tex_cord = block->tex_cord;
    self->clut_cord = block->clut_cord & 0x7FFF;
    self->anime_table = block->anime_table;
    self->sprite_data = block->sprite_data;
    self->flip = block->flip;
    self->layer = block->layer;
    DisplayCheck(self, 48, 16);
    if (self->display)
        PlaySound(2, 21, self);
    SetAnime(self, 4);
    self->step2 = 60;
    self->state++;
}

/* 0x800ED2F4, sparkle state 1 */
void IceSparkleMain(Object *self)
{
    AnimeTick(self);
    if (self->anime.flag == 1) {
        if (self->step2 != 0)
            self->step2--;
        else
            self->state++;
    }
    if (OffScreenCheck(self))
        self->state++;
    else
        DisplayCheck(self, 48, 16);
}

/* 0x800ED384, sparkle state 2 */
void IceSparkleDelete(Object *self)
{
    DeleteObject3(self);
}

/* ---- Stage logic type 8: ice shaft controller ---- */

#define ROWS(o)      ((o)->step)
#define SHAFT_T(o)   (*(s16 *)((u8 *)(o) + 0x18))
#define CAM_STEP(o)  (((s8 *)(o))[0x1A])

/* 0x800EE208, entry */
void IceShaftController(Object *self)
{
    if (mega.special == 2)
        return;
    ice_shaft_states[self->state](self);
}

/* 0x800EDF3C, state 0 */
void IceShaftWait(Object *self)
{
    int i;
    Object *o;

    if (mega.x.px < 5953 || mega.y.px >= 1244)
        return;
    ROWS(self) = 0;
    SHAFT_T(self) = 0;
    CAM_STEP(self) = 0;
    self->state++;
    CopyTiles(0, 5952, 1264, &ice_shaft_door_tiles);
    camera_layer0.redraw = 1;
    camera_layer0.target_max_y = 1072;
    camera_layer0.target_min_y = 320;
    /* HAZARD: this clears every stage object of type 7 with no further test, also a pit block
     * that a Wolfloid still rides. The wolf keeps its pointer (+0x80) to the cleared slot and
     * reads state 0 there (neither 2 nor 4), so it stays a carried wolf and sets its y to the
     * slot's y (0) - 39: it jumps off the top of the map [likely]. Safe in the game because the
     * player needs much more than the ~48 updates of a landed block to reach the shaft; only a
     * block that is still falling in the second pit can cause it. */
    for (i = 0, o = stage_objects; i < 80; i++, o++)
        if (o->type == 7)
            memset(o, 0, 0xA4);
}

static void MakeRow(Object *self, s16 y)
{
    Object *r = GetStageObject();

    if (r == NULL)
        return;
    r->flags = 0x41;
    r->type = 7;
    r->x.px = 5880;
    r->var = GetRNG() % 5 + 1;
    r->work7C = (u32)self;
    r->y.px = y;
}

/* 0x800EE258, state 1 */
void IceShaftStart(Object *self)
{
    for (ROWS(self) = 0; ROWS(self) < 5; ROWS(self)++)
        MakeRow(self, 1032 - 144 * ROWS(self));
    SHAFT_T(self) = 0;
    self->state++;
}

/* 0x800EE038, state 2 */
void IceShaftRun(Object *self)
{
    if (mega.special != 2) {
        if (ROWS(self) < 16) {
            if (SHAFT_T(self) % 144 == 0) {
                MakeRow(self, 312);
                ROWS(self)++;
            }
        }
        SHAFT_T(self)++;
    }
    if (CAM_STEP(self) == 0 && camera_layer0.max_y < 1073) {
        camera_layer0.field2C += 48;
        CAM_STEP(self)++;
    }
    if (mega.x.px >= 5761 && mega.y.px < 448 && CAM_STEP(self) == 1) {
        camera_layer0.field2C -= 48;
        CAM_STEP(self)++;
    }
}
