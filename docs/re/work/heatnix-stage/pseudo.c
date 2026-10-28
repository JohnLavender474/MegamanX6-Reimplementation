/*
 * Flame jet: stage 3 (overlay 05) main object type 36.
 * Plausible reconstruction from heatnix-1_start (kit: kit-main-36/). Not original code, not compiled.
 * Names: object fields as in catalog/helpers.json; the proposed function names are in
 * helpers-proposed.json.
 */
#include "object.h"
#include "game.h"
#include "sound.h"

extern Object *current_main;                         /* scratchpad 0x1F80004C */
extern const s16 dmg_tbl_normal[];                   /* 0x800743DC */

static const AnimeFrame *const flame_jet_anims[5];   /* 0x800F3CBC */
static const u8 flame_jet_height_tbl[28] = {         /* 0x800F3CD0, by sprite */
    16, 40, 56, 72, 88, 88, 88, 88, 72, 56, 24, 8, 8, 1,
    16, 40, 56, 72, 88, 88, 88, 88, 72, 56, 24, 8, 8, 1,
};

#define FLAME_DOWN(o)   ((o)->var & 0xF0)
#define FLAME_COUNT(o)  ((o)->var & 0x0F)
#define BOX(o)          ((s8 *)&(o)->work[0x80 - 0x7C])  /* self+0x80: {ox, oy, w, h} */
#define PUT_OUT(o)      ((o)->work8[0x84])               /* self+0x84 */
#define BURSTS(o)       ((o)->work8[0x85])               /* self+0x85 */
#define TIMER(o)        ((o)->work16[0x7C])              /* self+0x7C */
#define EXTRA_PAUSE(o)  ((o)->work16[0x7E])              /* self+0x7E */

static void FlameJetInit(Object *self);
static void FlameJetMain(Object *self);
static void FlameJetDelete(Object *self);
static void FlameJetFire(Object *self);
static void FlameJetNop(Object *self);
static void FlameJetPause(Object *self);
static void FlameJetIgnite(Object *self);
static void FlameJetGrow(Object *self);
static void FlameJetBurn(Object *self);
static void FlameJetDie(Object *self);
static void FlameJetPauseInit(Object *self);
static void FlameJetPauseWait(Object *self);

static void (*const flame_jet_states[])(Object *) = {          /* 0x800F3CEC */
    FlameJetInit, FlameJetMain, FlameJetDelete,
};
static void (*const flame_jet_steps[])(Object *) = {           /* 0x800F3CF8 */
    HitReactionDispatch, FlameJetNop, FlameJetFire, FlameJetPause,
};
static void (*const flame_jet_fire_subs[])(Object *) = {       /* 0x800F3D08 */
    FlameJetIgnite, FlameJetGrow, FlameJetBurn, FlameJetDie,
};
static void (*const flame_jet_pause_subs[])(Object *) = {      /* 0x800F3D18 */
    FlameJetPauseInit, FlameJetPauseWait,
};

/* 0x800E9CBC: entry */
void FlameJet(Object *self)
{
    flame_jet_states[self->state](self);
}

/* 0x800E9CF8: state 0 */
static void FlameJetInit(Object *self)
{
    self->hp = 12;
    self->anime_table = flame_jet_anims;
    self->contact_damage = 16;
    self->iframes = 0;
    self->display = 0;
    self->flags |= 0x10;
    self->prev_x = self->x;
    self->prev_y = self->y;
    self->priority = 7;
    self->latch = 0;
    self->flip = 0;
    self->damage_table = dmg_tbl_normal;
    self->hurt_box = NULL;
    self->attack_box = NULL;
    self->terrain_box = NULL;
    self->speed_x = self->accel_x = self->speed_y = self->gravity = 0;
    self->layer = mega.layer;
    /* the box and flags are written through current_main, which is self in this update */
    PUT_OUT(current_main) = 0;
    BOX(current_main)[0] = -8;
    BOX(current_main)[2] = 16;
    self->state = 1;
    self->step = 2;
    self->step2 = 0;
}

/* 0x800E98C0: state 1 */
static void FlameJetMain(Object *self)
{
    int hit;

    self->prev_x = self->x;
    self->prev_y = self->y;
    AnimeTick(self);
    flame_jet_steps[self->step](self);

    self->hurt_box = NULL;
    if (self->anime.sprite == 13 || self->anime.sprite == 27)
        self->attack_box = NULL;
    else
        self->attack_box = BOX(current_main);

    /* HAZARD: hurt_box is always NULL, so WeaponBoxTest returns 0 and these branches never run.
     * The code is kept as the game has it; see notes.md, "Hazards". */
    hit = ContactWeapon(self);
    if (hit > 0 && (hit == 2 || hit == 55))
        PUT_OUT(current_main) = 1;
    if (hit < 0)
        PUT_OUT(current_main) = 1;

    ContactMega(self);

    if (OffScreenCheck2(self, 160, 160)) {
        self->state++;
        return;
    }
    if (self->anime.sprite == 13 || self->anime.sprite == 27)
        self->display = 0;
    else
        DisplayCheck(self, 48, 48);
}

/* 0x800E9DC0: state 2 */
static void FlameJetDelete(Object *self)
{
    DeleteObject2(self);
}

/* 0x800E9DE0: step 1 */
static void FlameJetNop(Object *self)
{
}

/* HAZARD: the table is indexed by the sprite number; it has 28 entries and the animation
 * table uses sprites 0-27 only. A new animation with a higher sprite would read past it. */
static void FlameJetSetBox(Object *self)
{
    u8 h = flame_jet_height_tbl[self->anime.sprite];

    if (!FLAME_DOWN(self)) {
        BOX(current_main)[1] = -h;
        BOX(current_main)[3] = h;
    } else {
        BOX(current_main)[1] = 0;
        BOX(current_main)[3] = h;
    }
}

/* 0x800E9EAC: step 2 */
static void FlameJetFire(Object *self)
{
    flame_jet_fire_subs[self->step2](self);
}

/* 0x800E9EE8: step 2 sub 0 */
static void FlameJetIgnite(Object *self)
{
    int n;

    PlaySound(2, 22, self);
    SetAnime(self, FLAME_DOWN(self) ? 2 : 0);
    n = FLAME_COUNT(self);
    EXTRA_PAUSE(self) = 0;
    if (n == 0) {
        BURSTS(current_main) = 1;
        TIMER(self) = 180;
    } else if (n < 8) {
        BURSTS(current_main) = n;
        TIMER(self) = 14;
    } else {
        BURSTS(current_main) = (GetRNG() & 3) + 1;
        TIMER(self) = 14;
    }
    self->step2++;
}

/* 0x800E9FD4: step 2 sub 1 */
static void FlameJetGrow(Object *self)
{
    if (self->anime.flag == 1) {
        self->step2++;
        return;
    }
    FlameJetSetBox(self);
}

/* 0x800E9A04: step 2 sub 2 */
static void FlameJetBurn(Object *self)
{
    if (PUT_OUT(current_main)) {
        SetAnime(self, FLAME_DOWN(self) ? 3 : 1);
        BURSTS(current_main) = 1;
        EXTRA_PAUSE(self) = 200;
        self->step2++;
    } else if (--TIMER(self) == 0) {
        SetAnime(self, FLAME_DOWN(self) ? 3 : 1);
        self->step2++;
    }
    FlameJetSetBox(self);
}

/* 0x800E9B6C: step 2 sub 3 */
static void FlameJetDie(Object *self)
{
    if (self->anime.flag == 1) {
        /* HAZARD: an unsigned byte count; it is at least 1 here because FlameJetIgnite and
         * FlameJetBurn always write 1 or more. A 0 would wrap to 255 bursts. */
        if (--BURSTS(current_main) == 0) {
            self->step = 3;
            self->step2 = 0;
            return;
        }
        PlaySound(2, 22, self);
        SetAnime(self, FLAME_DOWN(self) ? 2 : 0);
        TIMER(self) = FLAME_COUNT(self) == 0 ? 180 : 14;
        self->step2 = 1;
        return;
    }
    FlameJetSetBox(self);
}

/* 0x800E9DE8: step 3 */
static void FlameJetPause(Object *self)
{
    flame_jet_pause_subs[self->step2](self);
}

/* 0x800E9E24: step 3 sub 0 */
static void FlameJetPauseInit(Object *self)
{
    SetAnime(self, 4);
    TIMER(self) = EXTRA_PAUSE(self) + 60;
    EXTRA_PAUSE(self) = 0;
    self->hp = 12;
    PUT_OUT(current_main) = 0;
    self->step2++;
}

/* 0x800E9E80: step 3 sub 1 */
static void FlameJetPauseWait(Object *self)
{
    if (--TIMER(self) == 0) {
        self->step = 2;
        self->step2 = 0;
    }
}
