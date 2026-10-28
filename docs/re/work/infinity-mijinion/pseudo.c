/* Infinity Mijinion (stage 8, overlay 11, main type 58): pseudo code.
 * A plausible reading of the code, not original source, not compiled.
 * Addresses are RAM addresses of overlay 11. Fields: docs/engine.md and catalog/helpers.json.
 * room_x/room_y = cam.target_min_x/min_y (5808, 832 in the fight). */

#define DMG_MIJINION   0x8007569C
#define BOSS           0   /* var */

s32  *bar_holder   = 0x800FDA74; /* BossWarning, then BossExplosion */
u8    bar_used[3]  ;             /* 0x800FDA78 */
s16   clone_count  ;             /* 0x800FDA7C */
s16   bubble_count ;             /* 0x800FDA7E */
s16   entry_angle  ;             /* 0x800FDA80 */
u32   fight_timer  ;             /* 0x800FDA84 */

const u8  hp_bonus[4]     = {0, 4, 6, 8};        /* 0x800E9B90 */
const s16 arrow_time[4]   = {180, 270, 360, 360}; /* 0x800E9B80 */
const u8  bubbles_n[4]    = {3, 4, 6, 8};        /* 0x800E9B88 */
const u8  clone_skip[4]   = {0, 1, 3, 7};        /* 0x800E9B8C */
const u8  clone_cap[4]    = {1, 3, 5, 7};        /* 0x800FD2EC */
/* move rows, 8 bytes, 0xFF ends: A 0x800FCD58, B 0x800FCDD8, C 0x800FCE58 */

void MijinionUpdate(Obj *o)                               /* 0x800F7E38 */
{
    o->prev_x = o->x; o->prev_y = o->y;
    state_tbl_800FD2F0[o->state](o);
    if ((o->var & 0xF0) == 0) CollideCheck(o);
}

void MijinionInit(Obj *o)                                 /* 0x800F5C34, state 0 */
{
    if (game.stage == 12) { o->sprite_data = ray_table4 + *ray_table4; o->clut = 0x7948; }
    o->damage_table = DMG_MIJINION; o->hp = 0; o->contact = 6; o->iframes = 0; o->display = 0;
    o->layer = mega.layer; o->speed_x = o->speed_y = o->accel_x = o->gravity = 0; o->latch = 0;
    o->anime_table = 0x800FD278; o->priority = 5; o->attack_box = o->hurt_box = o->terrain_box = 0;
    switch (o->var & 0xF0) {
    case 0x00:
        SetAnime(o, 0); o->tex = 0x140; o->tbl = -1; /* +0x86 */
        o->row = o->pos = 0; o->f8A = o->lemons = o->count = o->f8D = o->quarter = 0;
        o->terrain_box = 0x800FCED8;
        if ((o->var & 0xF) == 0) {
            o->flip = 0; bar_used[0..2] = 0; clone_count = bubble_count = 0; fight_timer = 0;
            o->state = 1; o->step = 2;
        } else { o->hp = 16; o->state = 2; o->step = 3; }                 /* clone */
        break;
    case 0x10: o->contact = 4; o->hp = 5; o->priority = 5; o->state = 4; o->step = 0; o->flags |= 0x10; break;
    case 0x20: SetAnime(o, 21); o->f8A = 0; o->len_target = o->len = o->f84 = 0;
               o->hp = 50; o->priority = 6; o->state = 5; o->step = 0; break;
    }
    o->sub = 0; o->act4 = 0;
}

static void FlashTimer(Obj *o)                            /* shared tail of states 1 and 2 */
{
    if (o->f84) {
        u16 old = o->f84--;
        o->clut = (old & 2) ? (o->clut | 0x8000) : (o->clut & 0x7FFF);
        if (o->f84 == 0) { o->damage_table = DMG_MIJINION; o->clut &= 0x7FFF; }
    }
}

void MijinionMain(Obj *o)                                 /* 0x800F5E20, state 1 */
{
    AnimeTick(o);
    boss_steps_800FD30C[o->step](o);
    MijinionBoxes(o);                                     /* 0x800F9DD0: boxes off while f8A */
    if (o->step != 2 && o->var == 0) fight_timer++;
    int r = ContactWeapon(o);
    if (r < 0) { mega.no_hit_7A = 1; o->state = 3; o->step = 0; return; }
    if (r > 0 && r != 127) {
        o->damage_table = dmg_tbl_deflect; o->f84 = 30;
        int id = r - 1;
        if (id == 7 || id == 16 || id == 53) { o->f84 = 10000; o->step = 3; o->sub = 0; return; }
        if (is_sabre(id) /* 24-29,31,37,39,50,51 */) { o->f84 = 10000; o->step = 12; o->sub = 0; return; }
        if (id == 0) {
            PlaySound(2, 36, o);
            o->lemons += (game.difficulty >= 2) ? 2 : 1;
            if (o->lemons > 8) o->lemons = 8;
            if (o->lemons >= 6) MijinionSpawnEffect(o, 0x11, 0);
            else if (o->lemons >= 3) MijinionSpawnEffect(o, 0x10, 0);
        }
        int lim = o->quarter == 0 ? boss_hp_max * 3 / 4 : o->quarter == 1 ? boss_hp_max / 2
                : o->quarter == 2 ? boss_hp_max / 4 : -128;
        if (o->quarter <= 2 && o->hp <= lim) {
            o->f84 = 10000; o->step = 12; o->sub = 3; o->quarter++; return;
        }
    }
    FlashTimer(o);
    ContactMega(o);
}

void MijinionNextMove(Obj *o)                             /* 0x800F6E4C */
{
    o->sub = 0;
    const u8 *A = 0x800FCD58, *B = 0x800FCDD8, *C = 0x800FCE58, *t;
    if (o->var != 0) {                                    /* clone: table C */
        if (o->tbl == -1) { o->tbl = 0; new_row(o, C); return; }
        t = C;
    } else if (o->tbl == -1) { o->tbl = 0; new_row(o, A); return; }
    else if (o->tbl == 0 && o->hp <= boss_hp_max / 2) { o->tbl = 1; new_row(o, B); return; }
    else if (o->tbl == 1 && o->hp >  boss_hp_max / 2) { o->tbl = 0; new_row(o, A); return; }
    else t = o->tbl ? B : A;
    u8 s = t[o->row * 8 + o->pos];
    if (s == 0xFF) { new_row(o, t); return; }
    o->step = s; o->pos++;
}
static void new_row(Obj *o, const u8 *t)
{ o->row = GetRNG() & 15; o->step = t[o->row * 8]; o->pos = 1; }

/* step 2: entry (sub-steps 0x800FD404) */
void Entry0(Obj *o) { PlayerEvent(20, 0x40); o->sub++; }                  /* 0x800F99A8 */
void Entry1(Obj *o)                                                      /* 0x800F99E0 */
{
    if (game.exit_req || mega.step != 20) return;
    if (game.stage != 12) PlayerEvent(29, 0x40);
    Obj *w = GetSysObject(); if (w) { w->flags = 1; w->type = 24; bar_holder = w; }
    o->f7C = 20; o->sub++;
}
void Entry2(Obj *o)                                                      /* 0x800F66D8 */
{
    if (--o->f7C == 0) PlayerEvent(20, 0x40);
    if (bar_holder->flags) return;
    game.f24 = 1; game.f25 = 0; game.boss = o; BossLevel();
    boss_hp_max = hp_bonus[game.boss_level] + 48;
    o->x = room_x + 208; o->y = room_y + 208; o->f7C = 2; o->f7E = 16; entry_angle = 0;
    PlaySound(2, 66, o); o->sub++;
}
void Entry3(Obj *o)                                                      /* 0x800F9A80 */
{
    if (--o->f7C != -1) return;
    o->f7C = 2;
    if (--o->f7E == 0) { o->sub++; return; }
    entry_angle += GetRNG() + 255;
    MijinionSpawnEffect(o, 0, entry_angle);              /* act4++ */
}
void Entry4(Obj *o)                                                      /* 0x800F9B24 */
{ if (o->act4) return; PlaySound(2, 67, o); SetAnime(o, 23); o->display = 1; o->sub++; }
void Entry5(Obj *o)                                                      /* 0x800F67E8 */
{
    if (o->anime_flag == 1) { PlaySound(2, 68, o); for (k = 0; k < 8; k++) MijinionSpawnEffect(o, 0x30, k * 512); }
    else if (o->anime_flag == 2) { for (k = 0; k < 8; k++) MijinionSpawnEffect(o, 0x30, 256 + k * 512); }
    else if (o->anime_flag == 3) {
        SetAnime(o, 0);
        if (game.stage != 12) { TextOpen(0, 255, game.text_seen_2E); game.text_seen_2E = 1; }
        o->sub++;
    }
}
void Entry6(Obj *o) { if (!text_open) { BossMusicRequest(0); o->sub++; } }   /* 0x800F9B80 */
void Entry7(Obj *o) { if (!BossMusicWait()) { o->f7E = 3; o->sub++; } }     /* 0x800F9BC4 */
void Entry8(Obj *o)                                                      /* 0x800F9C00 */
{
    if (--o->f7E == 0) { PlaySound(0, 14, 0); o->f7E = 3; }
    if (++o->hp == boss_hp_max) o->sub++;
}
void Entry9(Obj *o) { PlayerEventEnd(); MijinionNextMove(o); }             /* 0x800F9C84 */

/* step 3: stun knock-back (0x800FD42C) */
void Stun0(Obj *o)                                                       /* 0x800F6998 */
{
    PlaySound(2, 33, o); SetAnime(o, 8); o->contact = 6; o->lemons = 0;
    if (!(bar_used[0] && bar_used[1] && bar_used[2])) {
        int s = GetRNG() % 3;
        for (k = 0; k < 3; k++) if (bar_used[s] == 1) s = (s + 1 < 3) ? s + 1 : 0;
        MijinionSpawnBar(o, 0x20 + s);                    /* 0x800FA234: bar_used[s] = 1 */
    }
    if (clone_count < clone_cap[game.boss_level]) MijinionSpawnClone(o, 1);  /* 0x800FA150 */
    PushAway(o);                                          /* flip to player, speed_x -/+5, accel -0.25 */
    o->speed_y = o->gravity = 0; o->sub++;
}
void Stun1(Obj *o)                                                       /* 0x800F9CEC */
{
    MoveObject2(o);
    if ((o->flip == 0 && (o->coll & 1)) || (o->flip == 0x40 && (o->coll & 2))) {
        PlaySound(2, 78, o); Rumble(5); Quake2(20, 6, 2); o->f7C = 90; o->sub++;
    }
}
void Stun2(Obj *o) { if (--o->f7C == 0) { o->f84 = 40; MijinionNextMove(o); } } /* 0x800F9D8C */

/* step 4: float (0x800FD364) */
void Float0(Obj *o)                                                      /* 0x800F86D8 */
{
    SetAnime(o, 0);
    if (o->x < room_x + 168) { o->flip = 0x40; o->speed_x = 2.0; } else { o->flip = 0; o->speed_x = -2.0; }
    o->speed_y = 0; o->f7C = 60; o->act4 = GetRNG() % 3; o->sub++;
}
void Float1(Obj *o)                                                      /* 0x800F63CC */
{
    if (--o->f7C == 0) { o->sub++; return; }
    if (o->coll & 2) { o->flip = 0x40; o->speed_x = 2.0; } else if (o->coll & 1) { o->flip = 0; o->speed_x = -2.0; }
    MoveObject(o);
    ApproachY(&o->y, room_y + (o->act4 == 0 ? 112 : o->act4 == 1 ? 144 : 176), 1.0);  /* 0x800FA4F8 */
}
void Float2(Obj *o) { o->act4 = 0; MijinionNextMove(o); }                 /* 0x800F8778 */

/* step 5: arrows (0x800FD370) */
void Arrow0(Obj *o)                                                      /* 0x800F87D4 */
{ FacePlayer(o); SetAnime(o, 1); o->f7E = 0; o->f7C = arrow_time[game.boss_level]; o->window = GetRNG() % 24 + 8; o->sub++; }
void Arrow1(Obj *o)                                                      /* 0x800F64F0 */
{
    FacePlayer(o);
    if (--o->f7C == -1) { o->sub += 2; return; }          /* -> Arrow3 */
    if (o->f7E) { o->f7E--; ApproachY(&o->y, mega.y, 1.5); return; }
    if (abs(o->y - mega.y) <= o->window) { SetAnime(o, 2); o->sub++; }
    else ApproachY(&o->y, mega.y, 1.5);
}
void Arrow2(Obj *o)                                                      /* 0x800F8894 */
{
    if (o->anime_flag == 1) {
        PlaySound(2, (GetRNG() & 1) ? 34 : 35, o);
        MijinionSpawnShot(o, o->hp <= boss_hp_max / 2 ? 1 : 0);   /* visual 25, 0x800F9E00 */
    } else if (o->anime_flag == 2) {
        SetAnime(o, 1); o->f7E = 90; o->sub--; o->window = GetRNG() % 24 + 8;
    }
}
void Arrow3(Obj *o) { o->step = 4; o->sub = 0; }                          /* 0x800F899C */

/* step 6: teleport, dive, runners (0x800FD380) */
void Dive0(Obj *o) { FacePlayer(o); o->f7C = 30; o->f8A = 1; o->sub++; }   /* 0x800F89E8 */
void Dive1(Obj *o)                                                       /* 0x800F8A28 */
{ o->display = !(o->f7C & 1); if (--o->f7C == 0) { o->display = 1; o->sub++; } }
void Dive2(Obj *o)                                                       /* 0x800F8A7C */
{ PlaySound(2, 72, o); SetAnime(o, 5); o->x = room_x + 168; o->y = room_y + 144; FacePlayer(o); o->sub++; }
void Dive3(Obj *o)                                                       /* 0x800F8AF0 */
{ if (o->anime_flag == 1) { SetAnime(o, 3); o->f8A = 0; o->speed_y = -9.0; o->speed_x = 0; o->sub++; } }
void Dive4(Obj *o)                                                       /* 0x800F8B44 */
{
    FacePlayer(o);
    if (o->coll & 8) { PlaySound(2, 73, o); Quake2(8, 3, 1); SetAnime(o, 4); o->act4 = 0; o->sub++; }
    else MoveObject(o);
}
void Dive5(Obj *o)                                                       /* 0x800F8BC0 */
{
    if (o->anime_flag == 1) {
        PlaySound(2, (GetRNG() & 1) ? 34 : 35, o); PlaySound(2, 75, o);
        MijinionSpawnShot(o, 0x10); MijinionSpawnShot(o, 0x11);
    } else if (o->anime_flag == 2) o->sub++;
    else if (o->anime_flag == 3) PlaySound(2, 74, o);
}
void Dive6(Obj *o) { if (o->act4 == 0) { o->step = 4; o->sub = 0; } }       /* 0x800F8C84 */

void PickSpecial(Obj *o)                                  /* 0x800F8CA4, step 7 */
{ o->step = o->lemons >= 6 ? 10 : o->lemons >= 3 ? 9 : 8; o->sub = 0; }

void Charge0(Obj *o)                                      /* 0x800F8D14, step 8 */
{ MijinionSpawnEffect(o, 0x10, 0); o->lemons += (game.difficulty >= 2) ? 2 : 1; o->sub++; }
void Charge1(Obj *o) { o->step = 4; o->sub = 0; }         /* 0x800F8D80 */

/* step 9 (0x800FD3A4) */
void Fly0(Obj *o) { SetAnime(o, 1); o->count = o->lemons; o->lemons = 0; o->f7C = 30; o->sub++; } /* 0x800F8DCC */
void Fly1(Obj *o)                                                        /* 0x800F65D8 */
{
    int a = ratan2(o->y - mega.y, mega.x - o->x);
    o->speed_x = rcos(a) * 32; o->speed_y = rsin(a) * 32;   /* 2.0 px/update */
    MoveObject(o);
    if (abs(mega.x - o->x) >= 4) o->flip = o->speed_x < 0 ? 0 : 0x40;
    if (--o->f7C == 0) o->sub++;
}
void Fly2(Obj *o) { FacePlayer(o); SetAnime(o, 2); o->sub++; }             /* 0x800F8E14 */
void Fly3(Obj *o)                                                        /* 0x800F8E50 */
{
    if (o->anime_flag == 1) { PlaySound(2, 37, o); PlaySound(2, 71, o); Star8(o); }  /* 0x800F9EB4 x 8 */
    else if (o->anime_flag == 2) {
        if (--o->count == 0) { o->step = 4; o->sub = 0; }
        else { SetAnime(o, 1); o->f7C = 30; o->sub = 1; }
    }
}

/* step 10 (0x800FD3B4) */
void Heal0(Obj *o)                                                       /* 0x800F8FB4 */
{ FacePlayer(o); o->damage_table = dmg_tbl_deflect; o->f84 = 10000; o->f7C = 60; o->count = o->lemons; o->lemons = 0; o->sub++; }
void Heal1(Obj *o)                                                       /* 0x800F900C */
{ if (--o->f7C == 0) { PlaySound(2, 37, o); o->f7C = 8; o->f7E = boss_hp_max / 3; o->sub++; } }
void Heal2(Obj *o)                                                       /* 0x800F90A0 */
{
    if (--o->f7C) return;
    FacePlayer(o); o->f7C = 8;
    if (o->f7E == 0) { o->sub++; return; }
    o->f7E--;
    if (++o->hp >= boss_hp_max) { o->hp = boss_hp_max; o->sub++; }
}
void Heal3(Obj *o) { FacePlayer(o); SetAnime(o, 2); o->sub++; }            /* 0x800F9148 */
void Heal4(Obj *o)                                                       /* 0x800F9184 */
{
    if (o->anime_flag == 1) { PlaySound(2, 37, o); PlaySound(2, 71, o); Star8(o); }
    else if (o->anime_flag == 2) {
        if (--o->count == 0) { o->f84 = 1; o->step = 4; o->sub = 0; }
        else o->sub--;
    }
}

/* step 11 / clone step 2: bubbles (0x800FD3C8) */
void Bub0(Obj *o)                                                        /* 0x800F92DC */
{ if (bubble_count >= 10) { o->step = 4; o->sub = 0; return; } FacePlayer(o); SetAnime(o, 7); o->sub++; }
void Bub1(Obj *o)                                                        /* 0x800F9340 */
{
    if (o->anime_flag == 1) {
        PlaySound(2, (GetRNG() & 1) ? 34 : 35, o); PlaySound(2, 77, o);
        for (k = 0; k < bubbles_n[game.boss_level] && bubble_count < 10; k++)
            MijinionSpawnBubble(o, 0x10);                 /* 0x800FA06C: bubble_count++ */
    } else if (o->anime_flag == 2) o->sub++;
}
void Bub2(Obj *o) { o->step = 4; o->sub = 0; }                            /* 0x800F948C */

/* step 12: knock-back (0x800FD3D4) */
void Knock0(Obj *o)                                                      /* 0x800F95C4 */
{
    SetAnime(o, 8);
    if (clone_count < clone_cap[game.boss_level] && (GetRNG() & 15) >= clone_skip[game.boss_level])
        MijinionSpawnClone(o, 1);
    PushAway(o); o->speed_y = o->gravity = 0; o->sub++;
}
void Knock1(Obj *o) { /* as Stun1, timer 30 */ }                          /* 0x800F96E0 */
void Knock2(Obj *o) { if (--o->f7C == 0) { o->f84 = 40; MijinionNextMove(o); } } /* 0x800F9780 */
void Knock3(Obj *o) { SetAnime(o, 8); MijinionSpawnClone(o, 1); PushAway(o); o->sub = 1; } /* 0x800F97BC */

/* step 13 / clone step 3: appear (0x800FD3E4) */
void Appear0(Obj *o) { SetAnime(o, 5); o->f8A = 1; o->display = 1; o->sub++; }  /* 0x800F98A0 */
void Appear1(Obj *o) { if (o->anime_flag == 1) { o->f8A = 0; MijinionNextMove(o); } } /* 0x800F98E0 */

/* state 2: clone (steps 0x800FD344: hit, empty, Bub, Appear, Idle) */
void MijinionClone(Obj *o)                                /* 0x800F610C */
{
    if (o->parent->state != 1) { o->state = 6; o->display = 0; return; }
    AnimeTick(o); clone_steps[o->step](o); MijinionBoxes(o);
    int r = ContactWeapon(o);
    if (r < 0) goto die;
    if (r > 0 && r != 127) {
        o->damage_table = dmg_tbl_deflect; o->f84 = 20;
        if (is_clone_sabre(r - 1) /* 24-28,37,39,50,51 */ && clone_count >= 2 && ++o->f8D == 4) goto die;
    }
    FlashTimer(o); ContactMega(o); return;
die:
    SpawnExplosion(o); o->state = 6; o->display = 0;
}
void Idle0(Obj *o) { SetAnime(o, 0); o->f7C = (GetRNG() & 0x7F) + 120; o->sub++; }   /* 0x800F94DC */
void Idle1(Obj *o)                                                       /* 0x800F9524 */
{ if ((o->f7C & 15) == 0) FacePlayer(o); if (--o->f7C == 0) { o->act4 = 0; MijinionNextMove(o); } }

/* state 3: death (0x800FD358) -- the shared boss death, see the sheet */
void MijinionDeath0(Obj *o);  /* 0x800F7EE4 */
void MijinionDeath1(Obj *o);  /* 0x800F7F70: BossExplosion at 127 */
void MijinionDeath2(Obj *o);  /* 0x800F6298: soul var 0x10 if fight_timer < 5401 else 0x11 */

/* state 4: bubble (0x800FD438) */
void BubbleUpdate(Obj *o)                                 /* 0x800F8064 */
{
    if (o->parent->state != 1) { o->state = 6; o->display = 0; return; }
    AnimeTick(o); bubble_steps[o->step](o);
    if (ContactWeapon(o) < 0) { SpawnExplosion(o); o->display = 0; o->state = 6; o->step = 0; return; }
    ContactMega(o);
    if (OffScreenCheck2(o, 32, 32) == 1) { o->display = 0; o->state = 6; }
}
/* 0x800F8130 place; 0x800F81F8 boxes, wait 20..89; 0x800F6B5C aim 1.5 (3.0 Hard), fly 50..89;
   0x800F82AC fly, 9 dashes then animation 20; 0x800F8370 delete at the flag */

/* state 5: bar emitter (0x800FD44C) */
void BarUpdate(Obj *o)                                    /* 0x800F8390 */
{
    if (o->parent->state != 1) { o->state = 6; o->display = 0; return; }   /* no tile restore */
    AnimeTick(o); bar_steps[o->step](o);
    int r = ContactWeapon(o);
    if (r > 0) { if (mega.var != 0 || r == 1) o->f8A = 1; o->hp = 50; }
    if (ArrowTouchesBar(o)) o->f8A = 1;                   /* 0x800FA664: arrow +0x84 = 1 */
}
void Bar2(Obj *o)                                         /* 0x800F6CB0 */
{
    if (o->f8A) { o->len_target += 32; o->f8A = 0; }
    clamp len_target to the wall face (room x 16 or 320);
    Approach16(&o->len, o->len_target, 4, 1);            /* 0x800FA584 */
    MijinionSpawnBeam(o);                                 /* 0x800FA31C: effect 10 var 0x20 */
    if ((o->f84 & 15) && !(o->len & 15))                  /* a 16 px step was passed */
        CopyTiles(0, tip_x & ~15 (-16 facing right), o->y & ~15, &src_800FD45C /* (5632,1200,1,1): coll 45 */);
    if (--o->f7C == 0) { o->attack_box = o->hurt_box = 0; o->f7C = 60; o->step++; }
    o->f84 = o->len;
}
void Bar3(Obj *o)                                         /* 0x800F856C */
{
    if (--o->f7C == 0) {
        for (k = 1; k <= 19; k++) CopyTiles(0, room_x + 16 * k, o->y & ~15, &src_800FD464 /* (5632,1216): 0 */);
        bar_used[o->var & 15] = 0; o->state = 6;
    } else if (o->f7C & 1) MijinionSpawnBeam(o);
}
