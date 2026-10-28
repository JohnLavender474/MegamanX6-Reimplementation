/*
 * Nightmare Snake: overlay 05 (stage 3 area 0), main type 25 and visual type 29.
 * Plausible pseudo C from the disassembly in kit/code.txt. Not original code, not compiled.
 * Full detail for var 5 (the first fight) and the gems; the moves of var 4 and 6-10 in short.
 * Names: helpers-proposed.json. Units: px, px/frame (16.16), updates.
 */

typedef struct { s8 ox, oy; u8 w, h; } Box;

extern Object mega;              /* 0x800970A0 */
extern Game game;                /* 0x800CCED0 */
extern Camera cam[3];            /* 0x800971F8, 0x54 bytes each */
extern u32 frame_count;          /* 0x80097420 */

static const Box  snake_terrain_box  = { 0, -8, 64, 64 };      /* 0x800F3DD8, centre + half size */
static const Box  snake_attack_box   = { -51, -67, 106, 118 }; /* 0x800F3DDC */
static const Box  snake_hurt_box     = { -42, -62, 100, 100 }; /* 0x800F3DE0 */
static const Box  snake_gem_hurt[4]  = { {-12,-14,16,16}, {-12,-14,16,16}, {-14,-13,16,16}, {-12,-13,16,16} }; /* 0x800F3DE4 */
static const s16  snake_gem_ofs[4][2] = { {-44,-57}, {58,-54}, {-28,50}, {66,36} };   /* 0x800E987C */
static const s16  snake_jet_ofs[4][2] = { {-84,-11}, {82,-18}, {26,70}, {-4,-83} };   /* 0x800E988C */
static const Box  snake_ball_box     = { -6, -6, 12, 12 };     /* 0x800F3FF4, 0x800F3FF8 */
static const Box  snake_ball_terrain = { 0, 0, 6, 6 };         /* 0x800F3FFC */
static const Box  snake_lava_box6    = { -80, -56, 160, 112 }; /* 0x800F4000 */
static const Box  snake_lava_box7    = { -80, -64, 160, 128 }; /* 0x800F4004 */
extern const u8   dmg_tbl_nightmare_snake[80][2];              /* 0x800748DC */

/* ---------------------------------------------------------------- main type 25 */

void Snake(Object *o)                                    /* 0x800EC534, states 0x800F3F1C */
{
    o->prev_x = o->x; o->prev_y = o->y;
    switch (o->state) {
    case 0: SnakeInit(o); break;                         /* 0x800EAB28 */
    case 1: SnakeMain(o); break;                         /* 0x800EB304 */
    case 2: case 3: SnakeEnd(o); break;                  /* 0x800EC580 */
    }
}

static void SnakeCopyGfx(Object *dst, Object *src)
{
    dst->tex_cord = src->tex_cord;
    dst->clut_cord = src->clut_cord & 0x7FFF;
    dst->sprite_data = src->sprite_data;
    dst->anime_table = src->anime_table;
    dst->layer = src->layer;
}

void SnakeInit(Object *o)                                /* 0x800EAB28 */
{
    if (o->var == 4 && mega.y < 2384) return;            /* wait */
    if (o->var == 5 && game.checkpoint != 0) return;     /* waits forever */

    o->contact_damage = 8; o->hp = 32; o->iframes = 0;
    o->anime_table = snake_anims;                        /* 0x800F3ED8 */
    o->speed_x = o->speed_y = 0; o->flip = 0; o->collide_flags = 0;
    o->priority = 5; o->b8D = 0; o->b8E = 0;
    o->layer = mega.layer;

    if (o->var < 4) {                                    /* a gem, made by the body below */
        o->terrain_box = 0; o->attack_box = 0;
        o->hurt_box = &snake_gem_hurt[o->var];
        o->damage_table = dmg_tbl_nightmare_snake;
        SetAnime(o, o->var);
        o->step = 1;
        goto done;
    }

    switch (o->var) {
    case 4:
        if (game.checkpoint >= 3) { DeleteObject3(o); return; }
        SnakeBankGfx(o); o->b8D = 1; o->x = 1072; o->y = 2752;
        PlayerEvent(20, 0);
        break;
    case 5:
        if (mega.x < 363 && mega.y < 464) return;        /* not yet in the room */
        SnakeBankGfx(o);                                 /* tex, clut row 480 + bank / 4, sprite data */
        o->b8D = 1; o->x = 576; o->y = 584;
        PlayerEvent(20, 64);                             /* held, facing right */
        break;
    /* case 6, 7: x, y fixed (2640, 2304) / (2640, 2944), PlayerEvent(20, 64), +0x8C = 0.
       case 8: delete at checkpoint 5; camera target y 2816, mega.y >= 2800; wait for the player
               near (2944..3008, < 2880); x 3024, y = cam0.y + 352; layer 2 offsets, PlayerEvent.
       case 9: delete at checkpoint 4; camera y 2160; x 3024, y = cam0.y + 352; PlayerEvent.
       case 10: once: 4 visual 29 var 7 at (3296 + 160 i, 1304); layer 2 mode 2, offsets
               (-2272, 608); x 3616; wait for mega.x >= 3344; PlayerEvent(20, 64). */
    }

    cam[1].x = 512 - o->x; cam[1].y = 512 - o->y; cam[1].redraw = 1;

    for (int i = 0; i < 4; i++) {
        Object *g = GetMainObjectAfter(o, 0);
        if (g) {
            g->flags = 0x41; g->type = o->type; g->var = i; g->w10 = 0;
            SnakeCopyGfx(g, o); g->x.px = o->x.px; g->y.px = o->y.px;
            g->parent80 = o;
        }
        Object *v = GetVisualObject();
        if (v) {
            v->flags = 0x41; v->type = 29; v->var = i;
            SnakeCopyGfx(v, o); v->x = o->x; v->y = o->y;
            v->parent7C = o;
        }
    }
    cam[1].visible = 1;
    cam[1].b52 = 0;                                       /* meaning open */

    if (o->var < 8)        { o->terrain_box = &snake_terrain_box; o->step = 9; }
    else if (o->var == 10) { o->step = 9; }
    else                   { o->terrain_box = 0; o->step = 10; }
    if (o->var == 4) cam[1].visible = 0;                  /* shown later by the intro */

    o->hurt_box = &snake_hurt_box;
    o->damage_table = dmg_tbl_deflect;
    o->attack_box = &snake_attack_box;
    o->display = 0;
    game.stage_flag = 0;                                  /* game+0x26: boss gate id 7 locked */
done:
    o->t7C = 0; o->state = 1; o->t7E = 0; o->act4 = 0;
}

void SnakeMain(Object *o)                                /* 0x800EB304, steps 0x800F3F2C */
{
    Object *body = o->parent80;
    snake_steps[o->step](o);
    s32 r = ContactWeapon(o);

    if (o->var >= 4) {                                   /* body */
        if (o->step != 9 && o->step != 10) o->hp = 0;    /* the gems add their HP after this */
        if (o->act4 >= 4) {                              /* all gems dead */
            o->t7E = 0; o->step = 8; o->step2 = 0; o->act4 = 0;
            o->hurt_box = 0; o->attack_box = 0;
        }
        ContactMega(o);
        CollideCheck(o);
        if (!OffScreenCheck2(o, 128, 128)) DisplayCheck(o, 96, 96);
        cam[1].visible = o->display;                      /* the ring is BG layer 1 */
        o->display = 0;
        cam[1].x = 512 - o->x.px; cam[1].y = 512 - o->y.px; cam[1].redraw = 1;
        return;
    }

    /* gem */
    if (body->step != 9 && body->step != 10) {
        body->hp += o->hp;                               /* u8 add */
        if ((s8)body->hp == -128) body->hp = 127;        /* 4 x 32 = 128 */
    }
    if (r < 0) {                                         /* killed */
        SpawnExplosion(o);
        o->state = 2;                                    /* SnakeEnd deletes a gem */
        body->act4++;
        return;
    }
    if (!OffScreenCheck2(o, 64, 64)) DisplayCheck(o, 32, 32);
    o->x.px = body->x.px + snake_gem_ofs[o->var][0];
    o->y.px = body->y.px + snake_gem_ofs[o->var][1];
}

void SnakeGemShoot(Object *g)                            /* 0x800EB538, gem step 1 */
{
    Object *body = g->parent80;
    if (game.difficulty != 0 && body->step < 8 && body->b8D != 0 && frame_count % 120 == 0) {
        Object *v = GetVisualObject();
        if (v) {
            v->flags = 0x41; v->type = 29;
            if (game.difficulty == 1 || mega.var != 0) v->var = 4;     /* Normal, or Zero */
            else v->var = 4 + (GetRNG() & 1);                          /* Hard+, X */
            v->tex_cord = g->tex_cord; v->clut_cord = g->clut_cord & 0x7FFF;
            v->sprite_data = g->sprite_data; v->layer = g->layer;
            v->x = g->x; v->y = g->y; v->parent7C = g; v->anime_table = g->anime_table;
            PlaySound(2, 31, g);
        }
    }
    AnimeTick(g);
}

/* step 3: var 5, roll on the floor. Sub-steps 0x800F3F6C. */
void SnakeRollFloor(Object *o)                           /* 0x800EC66C */
{
    if (mega.y < 432) mega.y = 432;                      /* no exit over the left wall */
    switch (o->step2) {
    case 0:                                              /* 0x800ECC24 */
        o->speed_x = 0x10000; o->accel_x = -0x1000; o->speed_y = 0; o->gravity = 0;
        o->step2 = 1; break;
    case 1:                                              /* 0x800ECC4C: roll right */
        if (o->t7C) { o->t7C--; break; }
        if (o->speed_x > 0x1FFFF) o->speed_x = 0x20000;
        MoveObject2(o);                                  /* speed_x grows by 1/16 each update */
        if (o->collide_flags & 1) {
            PlaySound(2, 24, o); Rumble(3);
            o->speed_x = -0x10000; o->accel_x = 0x1000; o->speed_y = 0; o->gravity = 0;
            Quake2(60, 2, 1); o->t7C = 60;
            o->step2 = 2;
        }
        break;
    case 2: o->step2 = 3; break;                         /* 0x800ECD10 */
    case 3:                                              /* 0x800ECD24: roll left */
        if (o->t7C) { o->t7C--; break; }
        if (o->speed_x <= -0x20000) o->speed_x = -0x20000;
        MoveObject2(o);
        if (o->collide_flags & 2) {
            PlaySound(2, 24, o); Rumble(3); Quake2(60, 2, 1);
            o->t7C = 60; o->step2 = 0;
        }
        break;
    }
}

/* Other moves, short form:
 * step 2 (var 4, 0x800EC630, subs 0x800F3F58): right until wall bit 1, then up until ceiling bit 4,
 *        left until wall bit 2, down until floor bit 8; each turn: sound 24, Rumble(3),
 *        Quake2(60, 2, 1), wait 60. Speeds 1 -> 2 px/frame.
 * step 4 (var 6, 0x800EC6C8, subs 0x800F3F7C): diagonal roll with hops (speed_y 2.8125,
 *        gravity 0x0D00) at x 1953 and 2241 going right, 2400 and 2112 going left; turns at the wall
 *        at x >= 2657 / x < 1664; landing shakes (Quake2(30, 2, 1)).
 * step 5 (var 7, 0x800EC704, subs 0x800F3F8C): rolls between walls with gravity; landing shakes.
 * step 6 (var 8/9, 0x800EBAD0, subs 0x800F3F9C): picks x 2944 or 3104 at random (forced the other
 *        way after 3 picks), rises from below with gravity to cam.target_min_y + 144, falls to +416,
 *        waits 90 (b8D = 1 only while waiting: the gems shoot only then). The room scrolls up 1 px
 *        every 4 updates while the camera y >= 945 (sound bank 5, 3 when not); the player y is kept
 *        >= camera y - 16; the two lava kill boxes (visual 29 var 6) get y = camera y + 192.
 * step 7 (var 10, 0x800EC740, subs 0x800F3FB0): rolls toward the player; within 48 px in x (or at
 *        x >= 3729 / < 3296) it stops 50 updates and leaps to cam + 192, falls to cam + 352.
 * step 10 (var 8/9 intro, 0x800EC8DC, subs 0x800F3FE0): when the player is in step 20: two lava
 *        boxes (visual 29 var 6) at x 2944 and 3104; layer 2 mode 9, offsets (960, 1600); sound
 *        bank 5 id 1, Rumble(4); the lava (layer 2 y) rises 0.5 px/update to 1712 with Quake(3, 2, 1)
 *        every 4 updates; the body rises 1 px/update to cam + 192; HP bar, BossLevel; HP fill as
 *        step 9; then step 6 sub 4.
 */

/* step 9: intro. Sub-steps 0x800F3FD8. */
void SnakeIntroStart(Object *o)                          /* 0x800ED610 */
{
    if (o->var == 4 && mega.y >= 2577) cam[1].visible = 1;
    if (mega.step != 20) return;                         /* wait until the player is held */
    game.player_lock = 1;
    if (o->var == 4) CopyTiles(0, 1216, 2352, snake_gate_tiles);   /* 0x800F3F14: {1152, 2352, 4, 2} */
    game.boss_bar = 1;
    boss_hp_max = 127;
    BossLevel();                                         /* result not read here */
    game.b25 = 0;
    game.boss = o;
    o->t7E = 3;
    o->step2++;
}

void SnakeIntroFill(Object *o)                           /* 0x800ED6E8 */
{
    if (--o->t7E == 0) { PlaySound(0, 14, 0); o->t7E = 3; }
    if (frame_count & 1) return;
    if ((s8)++o->hp != 127) return;
    PlayerEventEnd();
    switch (o->var) {
    case 4: o->step = 2; break;
    case 5: o->step = 3; break;
    case 6: o->step = 4; break;
    case 7: o->step = 5; break;
    case 10: o->step = 7; break;
    }
    o->step2 = 0;
}

/* step 8: death. Sub-steps 0x800F3FC4. */
void SnakeDeath(Object *o)                               /* 0x800EC77C */
{
    switch (o->step2) {
    case 0:                                              /* 0x800ED4A4 */
        if (o->var == 8 || o->var == 9) { cam[0].target_max_y = cam[0].target_min_y; PlaySound(5, 1, o); Rumble(4); }
        o->t7C = 120; o->step2++; break;
    case 1:                                              /* 0x800EBBF4 */
        if (--o->t7C == 0) {
            ResetLayerCache(0);
            fade_sub_b = fade_mode = fade_sub_g = 0; fade_g = fade_b = fade_r = 0;
            bg_refresh |= 1;
            o->step2++;
            break;
        }
        if ((o->t7C & 7) == 0) {
            Object *e = GetEffectObject();
            if (e) {
                e->flags = 0x21; e->type = 4; e->var = 0; e->state = e->step = e->step2 = 0;
                e->x.px = o->x.px + (GetRNG2() % 64 - 32);     /* sign by o->var: same range */
                e->y.px = o->y.px + (GetRNG2() % 64 - 32);
                e->priority = 19; e->layer = o->layer;
            }
        }
        cam[1].visible ^= 1;                             /* flicker */
        break;
    case 2:                                              /* 0x800ED514: white flash */
        fade_r = 31; fade_g = 31 << 5; fade_b = 31 << 10; bg_refresh |= 1;
        o->step2++; break;
    case 3:                                              /* 0x800ED55C */
        PaletteFxClear(0);
        fade_b = fade_g = fade_r = 0;
        game.boss_bar = 0; game.boss = 0;
        cam[1].visible = 0;
        for (int i = 0; i < 12; i++) SpawnExplosionRand(o, 1, 64, 64);
        o->step2++; break;
    case 4:                                              /* 0x800ED5EC */
        if (o->var != 8 && o->var != 9) o->state = 3;
        break;
    }
    /* var 8/9 only: the lava sinks (layer 2 y - 1 per update, the lava boxes +1) for 112 updates,
       sound (5, 3), then at step2 4 and layer 1 hidden: camera target (2816, 944), state 3 for the
       body and both lava boxes. */
}

void SnakeEnd(Object *o)                                 /* 0x800EC580, states 2 and 3 */
{
    o->parent80 = 0; o->w84 = 0; o->w88 = 0; o->w8C = 0; o->w90 = 0; o->w94 = 0;
    if (o->var < 4) { DeleteObject3(o); return; }        /* a dead gem */
    cam[1].visible = 0;
    game.stage_flag = 1;                                 /* game+0x26: boss gate id 7 opens */
    if (o->var == 5 && mega.x < 944) {                   /* stay until the player leaves */
        if (mega.y < 432) mega.y = 432;
        return;
    }
    DeleteObject3(o);
}

/* ---------------------------------------------------------------- visual type 29 */

void SnakeVisual(Object *v)                              /* 0x800EDA5C, states 0x800F4008 */
{
    v->prev_x = v->x; v->prev_y = v->y;
    switch (v->state) {
    case 0: SnakeVisualInit(v); break;                   /* 0x800EBF1C */
    case 1: SnakeVisualMain(v); break;                   /* 0x800EC2D0 */
    case 2: DeleteObject3(v); break;                     /* 0x800EDA3C */
    }
    CollideCheck(v);
}

void SnakeVisualInit(Object *v)                          /* 0x800EBF1C */
{
    v->priority = 4; v->state++; v->step2 = 0; v->h8C = 0;
    v->terrain_box = v->hurt_box = v->attack_box = 0;
    switch (v->var) {
    case 0: SetAnime(v, 10); v->step = 0; v->flip = 0x40; break;   /* jet, left of the ring */
    case 1: SetAnime(v, 10); v->step = 0; v->flip = 0; break;      /* jet, right */
    case 2: SetAnime(v, 11); v->step = 0; v->flip = 0; break;      /* jet, below */
    case 3: SetAnime(v, 12); v->step = 0; v->flip = 0; break;      /* jet, above */
    case 4: case 5: {                                              /* energy ball */
        v->contact_damage = 4; v->hp = 2;
        s32 a = ratan2(mega.y.px - v->y.px, mega.x.px - v->x.px);
        v->speed_x = rcos(a) << 6;                       /* 4 px/frame */
        v->speed_y = -(rsin(a) << 6);
        v->attack_box = &snake_ball_box; v->hurt_box = &snake_ball_box;
        v->terrain_box = &snake_ball_terrain;
        v->damage_table = v->var == 4 ? dmg_tbl_normal : dmg_tbl_deflect;
        SetAnime(v, 9); v->step = 1; break;
    }
    case 6: v->contact_damage = 127; v->attack_box = &snake_lava_box6; v->step = 2; break;
    case 7: v->contact_damage = 127; v->attack_box = &snake_lava_box7; v->step = 2; break;
    }
}

void SnakeVisualMain(Object *v)                          /* 0x800EC2D0, steps 0x800F4014 */
{
    Object *p = v->parent7C;
    s32 r = ContactWeapon(v);
    switch (v->step) {
    case 0: SnakeJetFlame(v); break;                     /* 0x800EC148 */
    case 1: AnimeTick(v); MoveObject(v); break;          /* 0x800EDAB8 */
    case 2: break;                                       /* 0x800EDAE4 */
    }
    if (v->step == 1) {
        if (ContactMega(v) || v->collide_flags || r < 0) { SpawnExplosion(v); v->state = 2; return; }
        if (OffScreenCheck2(v, 96, 96)) { v->state = 2; return; }
        DisplayCheck(v, 64, 64);
        return;
    }
    if (v->step == 0) {                                  /* a jet ends with its body */
        if (p->state == 3) v->state = 2;
        return;
    }
    /* step 2, lava kill boxes (var 6, 7): y from +0x90 (var 6) and a wave of the layer 2
       offset every 10 updates (0x800F3F0C: 960, 1568, 2176, 2784 minus 2848 or 3232); when
       mega.x >= 3825: layer 0 min y + 64 and state 2; then ContactMega(v). */
}

void SnakeJetFlame(Object *v)                            /* 0x800EC148 */
{
    Object *body = v->parent7C;
    AnimeTick(v);
    v->x.px = body->x.px + snake_jet_ofs[v->var][0];
    v->y.px = body->y.px + snake_jet_ofs[v->var][1];
    static const u8 dir_of_var[4] = { 1, 3, 2, 4 };      /* body step2 that shows this jet */
    v->display = body->step2 == dir_of_var[v->var];
    if (body->step >= 8 || body->t7C != 0) v->display = 0;
    if ((frame_count & 31) == 0 && v->display) PlaySound(2, 3, v);
}
