/* Nightmare Bug, stage 7 (overlay 10) main type 22. Reconstructed from the disassembly. */
#include "x6.h"

static const u8 BOX_BUG[4] = { (u8)-6, (u8)-6, 11, 11 };
static const s16 XOFS[8] = { -96, -80, -64, -48, 48, 64, 80, 96 };
static const s16 YOFS[3] = { 0, 16, 32 };

static void bug_follow(Obj *o);
static void bug_leave(Obj *o);
static void (*const bug_steps[])(Obj *) = { HitReactionDispatch, bug_follow, bug_leave };

static void bug_setup(Obj *o)
{
    s8 ne = nightmare_effect;
    if (ne != 1) {
        DeleteObject3(o);
        return;
    }
    o->hp = 8;
    o->contact_damage = 2;
    o->damage_table = dmg_tbl_nightmare_bug;
    o->iframes = 0;
    o->anime_table = bug_anims;
    o->priority = 5;
    o->layer = mega.layer;
    o->speed_x = 0;
    o->speed_y = 0;
    o->latch = 0;
    o->collide_flags = 0;
    o->flip = 0;
    o->terrain_box = NULL;
    o->hurt_box = BOX_BUG;
    o->attack_box = BOX_BUG;
    o->w80 = XOFS[GetRNG() & 7];
    o->w82 = YOFS[GetRNG() % 3];
    o->w85 = 0;
    SetAnime(o, 0);
    o->state = ne;
    o->step = ne;
    o->step2 = 0;
}

static void bug_follow(Obj *o)
{
    s16 ty, tx, dx, dy;
    int a;

    o->flip = (o->x.px < mega.x.px) ? 0x40 : 0;
    ty = mega.y.px + o->w82;
    tx = mega.x.px + o->w80;
    a = ratan2(ty - o->y.px, tx - o->x.px);
    o->speed_x = rcos(a) * 80;
    o->speed_y = -(rsin(a) * 80);

    dx = o->x.px - tx;
    dy = o->y.px - ty;
    if ((dx < 0 ? -dx : dx) < 16 && (dy < 0 ? -dy : dy) < 32) {
        o->speed_x = 0;
        o->speed_y = 0;
        if ((frame_count & 1) == 0) {
            o->w84 = (o->w84 + 1) & 0x3F;
            o->y.px += (s16)(((s64)rsin(o->w84 << 7) * 0x3E88CB3D >> 32) >> 9)
                     - (rsin(o->w84 << 7) >> 31);
        }
    }
    AnimeTick(o);
    MoveObject(o);
}

static void bug_leave(Obj *o)
{
    o->speed_x = 0;
    o->speed_y = 0x50000;
    AnimeTick(o);
    MoveObject(o);
}

static void bug_main(Obj *o)
{
    int r;

    bug_steps[o->step](o);
    r = ContactWeapon(o);
    if (r < 0) {
        DropItem(o, 18);
        SpawnExplosion(o);
        o->state = 2;
        return;
    }
    if (mega.gate_state == 1) {
        o->state = 2;
        return;
    }
    if (r == 127) {
        if ((s8)o->w85++ >= 5)
            o->step = 2;
    }
    ContactMega(o);
    if (OffScreenCheck2(o, 64, 64))
        DeleteObject2(o);
    else
        DisplayCheck(o, 32, 32);
}

static void bug_gone(Obj *o)
{
    DeleteObject3(o);
}

static void bug_blink(Obj *o)
{
    if ((s16)o->w7c++ < 60)
        o->display ^= 1;
    else
        DeleteObject3(o);
}

static void (*const bug_states[])(Obj *) = { bug_setup, bug_main, bug_gone, bug_blink };

void NightmareBug(Obj *o)
{
    o->prev_x = o->x.raw;
    o->prev_y = o->y.raw;
    bug_states[o->state](o);
}
