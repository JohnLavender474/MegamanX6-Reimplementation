/*
 * Blizzard Wolfang (stage 2, overlay 04, main type 54): pseudo code.
 * A plausible reading of the disassembly in kit/code.txt, not original code, not compiled.
 * Fixed point: 16.16 (0x10000 = 1 px/update). Angles: 4096 = 360 degrees, rcos/rsin 4096 = 1.0.
 * "room" = (cam.target_min_x, cam.target_min_y) = camera_layer0 +0x26, +0x2A.
 */

#define ROOM_X  (camera_layer0.target_min_x)   /* 6320 in the states */
#define ROOM_Y  (camera_layer0.target_min_y)   /* 304 */
#define FLIP_R  0x40                           /* flip 0x40 = facing right */

static u32 wolfang_fight_time;                 /* 0x800F7840, last word of overlay 04 */
static const u8 wolfang_hp_bonus[4] = {0, 2, 4, 6};          /* 0x800E9B2C */
static const u8 wolfang_moves[16][8];          /* 0x800F6FB0, HP above half */
static const u8 wolfang_moves_low_hp[16][8];   /* 0x800F7030, HP half or less */
static const u8 wolfang_ball_counts[2][4] = {{3,4,5,5},{3,4,5,5}};  /* 0x800F75F0 [char][level] */
static const u8 wolfang_ball_waits[2][2] = {{40,60},{70,60}};       /* 0x800F75F8 [char]{next, end} */
static const u8 wolfang_volleys[4] = {1, 2, 3, 4};                  /* 0x800F7620 [level] */

/* 0x800F2E8C */
void BlizzardWolfang(Obj *o) {
    o->prev_x = o->x; o->prev_y = o->y;
    wolfang_states[o->state](o);       /* 0x800F7500: Init, Main, Death, Trail, Ice, Delete */
    CollideCheck(o);
}

/* 0x800F14B0, state 0 */
void WolfangInit(Obj *o) {
    if (game.stage == 12) { o->sprite_data = ray_table4_entry(0); o->clut = 0x7948; }
    o->contact_damage = 4; o->damage_table = dmg_tbl_wolfang; o->hp = 0; o->iframes = 0;
    o->display = 0; o->layer = mega.layer; o->speed_x = o->speed_y = o->accel_x = o->gravity = 0;
    o->latch = 0; o->anime_table = wolfang_anims; o->priority = 5;
    o->attack_box = o->hurt_box = o->terrain_box = NULL;
    if (o->var == 0) {                               /* the boss */
        o->tex = 0x140; o->flip = 0; SetAnime(o, 0);
        o->move_table = -1; o->move_row = 0; o->move_pos = 0;   /* +0x8A, +0x8B, +0x8C */
        wolfang_fight_time = 0;
        o->attack_box = box_70B4; o->hurt_box = box_70B8; o->terrain_box = box_70B0;
        o->state = 1; o->step = 2;
    } else if (o->var == 1) {                        /* trail image */
        o->priority = 6; o->timer = 20; o->display = 0; o->flags |= 0x10;
        o->state = 3; o->step = 0;
    } else if ((o->var & 0xF0) == 0x10) {            /* ice spikes */
        if (game.boss_level < 2) { o->state = 5; o->step = 0; return; }
        o->damage_table = dmg_tbl_minor; o->contact_damage = 6; o->hp = 2;
        switch (o->var) {
        case 16: SetAnime(o, 31); o->y = ROOM_Y + 192; break;          /* floor */
        case 17: SetAnime(o, 32);                                        /* wall */
                 if (ROOM_X + 192 < o->x) { o->flip = FLIP_R; o->x = ROOM_X + 352; }
                 else                      { o->flip = 0;      o->x = ROOM_X + 32;  }
                 break;
        case 18: SetAnime(o, 33); o->y = ROOM_Y + 32; break;           /* ceiling */
        }
        o->priority = 4; o->state = 4; o->step = 0;
    }
    o->step2 = 0; o->act4 = 0;
}

/* 0x800F1738, state 1 */
void WolfangMain(Obj *o) {
    AnimeTick(o);
    wolfang_steps[o->step](o);                       /* 0x800F7518, 14 entries */
    WolfangBoxes(o);
    if (o->step != 2 && o->var == 0) wolfang_fight_time++;
    o->blocked = 0;                                  /* +0x8D */
    int r = ContactWeapon(o);
    if (r < 0) { mega.no_hit = 1; o->state = 2; o->step = 0; return; }
    if (r > 0 && r != 127) {
        o->damage_table = dmg_tbl_deflect;
        o->hit_timer = (r == 25 || r == 26) ? 2 : 30;               /* +0x88 */
        if (r == 4 || r == 13 || r == 35) {          /* weapon ids 3, 12, 34 */
            o->damage_table = dmg_tbl_deflect; o->hit_timer = 10000;
            o->step = 3; o->step2 = 0;
        }
    } else if (r == 127) o->blocked = 1;
    if (o->hit_timer) {
        u16 old = o->hit_timer--;
        o->clut = (old & 2) ? (o->clut | 0x8000) : (o->clut & 0x7FFF);
        if (o->hit_timer == 0) { o->damage_table = dmg_tbl_wolfang; o->clut &= 0x7FFF; }
    }
    ContactMega(o);                                  /* also during the hit timer */
}

/* 0x800F4FD8: boxes by sprite */
void WolfangBoxes(Obj *o) {
    u8 s = o->sprite;
    if ((s >= 7 && s < 13) || (s >= 26 && s < 30) || s == 91 || s == 96 || s == 97)
        { o->attack_box = box_70BC; o->hurt_box = box_70C0; }   /* (-16,-26,32,42) */
    else if (s == 25)
        { o->attack_box = box_70C4; o->hurt_box = box_70C8; }   /* (-64,-16,96,32) */
    else
        { o->attack_box = box_70B4; o->hurt_box = box_70B8; }   /* (-16,-16,32,42) */
}

/* 0x800F25A0 */
void WolfangNextMove(Obj *o) {
    o->step2 = 0; o->contact_damage = 4;
    if (o->move_table == 0 && o->hp <= (s8)boss_hp_max / 2) {
        o->move_row = GetRNG() & 15; o->move_pos = 0; o->move_table = 1;
        o->step = wolfang_moves_low_hp[o->move_row][0]; o->move_pos = 1; return;
    }
    if (o->move_table == -1) {
        o->move_row = GetRNG() & 15; o->move_pos = 0;
        o->step = wolfang_moves[o->move_row][0]; o->move_pos = 1; o->move_table = 0; return;
    }
    const u8 (*t)[8] = (o->move_table == 0) ? wolfang_moves : wolfang_moves_low_hp;
    u8 s = t[o->move_row][o->move_pos];
    if (s == 0xFF) { o->move_row = GetRNG() & 15; o->move_pos = 0; s = t[o->move_row][0]; }
    o->step = s; o->move_pos++;
}

/* 0x800F5488 */
void WolfangFacePlayer(Obj *o) { o->flip = (o->x < mega.x) ? FLIP_R : 0; }

/* ---- step 2: entry (sub-steps 0x800F763C) ---- */
void Entry0(Obj *o) { PlayerEvent(20, FLIP_R); o->step2++; }                    /* 0x800F4910 */
void Entry1(Obj *o) {                                                           /* 0x800F4948 */
    if (game.exit_req || mega.step != 20) return;
    if (game.stage != 12) PlayerEvent(29, FLIP_R);
    Obj *w = GetSysObject(); if (w) { w->flags = 1; w->type = 24; o->child = w; } /* BossWarning */
    o->timer = 20; o->step2++;
}
void Entry2(Obj *o) {                                                           /* 0x800F232C */
    if (--o->timer == 0) PlayerEvent(20, FLIP_R);
    if (o->child->flags) return;
    game.boss_active = 1; game.byte25 = 0; game.boss = o;                       /* +0x24 +0x25 +0x20 */
    BossLevel();
    boss_hp_max = 48 + wolfang_hp_bonus[game.boss_level];
    PaletteFxClear(0);
    fade_sub_b = 1; fade_sub_g = 1; fade_mode[0] = 1;                           /* subtract r, g, b */
    fade_mask[0][1] = 0xF0000000; for (int i = 3; i <= 14; i++) fade_mask[0][i] = 0xFFFFFFFF;
    bg_refresh |= 1;
    o->timer = 3; o->act4 = 0; o->step2++;
}
void Entry3(Obj *o) {                                                           /* 0x800F49E4 */
    if (--o->timer != -1) return;
    o->timer = 3; fade_r[0] += 1; fade_g[0] += 32; fade_b[0] += 1024; bg_refresh |= 1;
    if (fade_r[0] < 31) return;
    o->x = ROOM_X + 240; o->y = ROOM_Y + 144; o->gravity = 3000;
    o->speed_x = o->accel_x = o->speed_y = 0; o->prev_x = o->x; o->prev_y = o->y; o->step2++;
}
void Entry4(Obj *o) {                                                           /* 0x800F4AB4 */
    if (!(o->collide & 8)) { MoveObject2(o); return; }
    o->x += 6; o->y -= 24; SetAnime(o, 7); o->display = 1; o->timer = 180; o->step2++;
}
void Entry5(Obj *o) {                                                           /* 0x800F4B2C */
    if (--o->timer) return;
    if (game.stage != 12) { TextOpen(0, 255, game.byte2E); game.byte2E = 1; }
    o->step2++;
}
void Entry6(Obj *o) { if (!text_open) { BossMusicRequest(0); o->step2++; } }   /* 0x800F4BA8 */
void Entry7(Obj *o) {                                                           /* 0x800F4BEC */
    if (BossMusicWait()) return;
    fade_mask[0][2] = 0xFF; bg_refresh |= 1;                 /* CLUTs 88-95: the boss palette */
    o->x -= 6; o->y += 24; SetAnime(o, 0); o->timer = 3; o->step2++;
}
void Entry8(Obj *o) {                                                           /* 0x800F4C68 */
    if (--o->timer != -1) return;
    o->timer = 3; fade_r[0] -= 1; fade_g[0] -= 32; fade_b[0] -= 1024; bg_refresh |= 1;
    if (fade_r[0]) return;
    PlaySound(2, 16, o); SetAnime(o, 8); o->step2++;
}
void Entry9(Obj *o) { if (o->anime_flag == 1) { o->timer2 = 3; o->step2++; } }  /* 0x800F4D20 */
void Entry10(Obj *o) {                                                          /* 0x800F4D48 */
    if (--o->timer2 == 0) { PlaySound(0, 14, NULL); o->timer2 = 3; }
    if (++o->hp == boss_hp_max) o->step2++;
}
void Entry11(Obj *o) { PlayerEventEnd(); WolfangNextMove(o); }                  /* 0x800F4DCC */

/* ---- step 3: stun (0x800F766C) ---- */
void Stun0(Obj *o) {                                                            /* 0x800F4E34 */
    PlaySound(2, 33, o); SetAnime(o, 25); o->contact_damage = 4;
    if (o->collide & 8) { o->step2 += 2; return; }
    o->speed_x = o->accel_x = o->speed_y = 0; o->gravity = 0x4000; o->step2++;
}
void Stun1(Obj *o) { MoveObject2(o); if (o->collide & 8) { PlaySound(2,18,o); Rumble(8); o->step2++; } }
void Stun2(Obj *o) { o->timer = 50; o->step2++; }
void Stun3(Obj *o) {                                                            /* 0x800F4F24 */
    if (--o->timer) return;
    o->hit_timer = 40; o->step2 = 0;
    o->move_row = GetRNG() & 15; o->move_pos = 0;
    if (o->move_table >= -1) {
        if (o->move_table <= 0) o->step = wolfang_moves[o->move_row][0];
        else if (o->move_table == 1) o->step = wolfang_moves_low_hp[o->move_row][0];
    }
    o->move_pos++;
}

/* ---- step 4: stand (no writer found) ---- */
void Wait0(Obj *o) { SetAnime(o, 0); o->timer = 120; o->step2++; }
void Wait1(Obj *o) { if (--o->timer == 0) WolfangNextMove(o); }

/* ---- step 5: dash (0x800F7564) ---- */
void Dash0(Obj *o) { WolfangFacePlayer(o); SetAnime(o, 2); o->step2++; }
void Dash1(Obj *o) {
    WolfangFacePlayer(o);
    if (o->anime_flag != 1) return;
    PlaySound(2, 18, o); o->speed_x = o->flip ? 0x40000 : -0x40000; o->step2++;
}
void Dash2(Obj *o) {                                                            /* 0x800F351C */
    MoveObject(o);
    bool ahead = o->flip ? (o->x < mega.x && !(o->collide & 1)) : (mega.x < o->x && !(o->collide & 2));
    if (!ahead) { SetAnime(o, 23); o->speed_x = o->flip ? 0x20000 : -0x20000; o->timer = 16; o->step2++; }
    else if (o->anime_flag == 2) PlaySound(2, 18, o);
}
void Dash3(Obj *o) { MoveObject(o); if (--o->timer == 0) WolfangNextMove(o); }

/* ---- step 6: jump (0x800F7574) ---- */
void Jump0(Obj *o) { WolfangFacePlayer(o); SetAnime(o, 4); o->timer = 60; o->step2++; }
void Jump1(Obj *o) {                                                            /* 0x800F19EC */
    if (--o->timer) return;
    PlaySound(2, 17, o); PlayVoice34_36(o);          /* RNG % 3: (2,34) (2,35) (2,36) */
    SetAnime(o, 5);
    o->gravity = 0x500000 / 162; o->speed_y = 0x8E386; o->accel_x = 0;  /* 0.494, 8.889 */
    o->speed_x = o->flip ? 0xA00000 / 36 : -0x471C7;                     /* +-4.444 */
    o->timer = 12; o->step2++;
}
void Jump2(Obj *o) { if (--o->timer == 0) { SetAnime(o, 6); o->timer = 11; o->step2++; } else MoveObject2(o); }
void Jump3(Obj *o) { if (--o->timer == 0) { SetAnime(o, 5); o->step2++; } else MoveObject2(o); }
void Jump4(Obj *o) {
    if (!(o->collide & 8)) { MoveObject2(o); return; }
    PlaySound(2, 18, o); Rumble(8); SetAnime(o, 4); o->timer = 13; o->step2++;
}
void Jump5(Obj *o) { if (--o->timer == 0) WolfangNextMove(o); }

/* ---- step 7: leap to a wall, then dive (0x800F758C) ---- */
void Dive0(Obj *o) { SetAnime(o, 9); o->timer = 60; o->step2++; }
void Dive1(Obj *o) {                                                            /* 0x800F38B4 */
    if (--o->timer) return;
    PlaySound(2, 17, o); PlayVoice34_36(o); SetAnime(o, 10);
    o->tx = (o->x < ROOM_X + 192) ? 368 : 16;        /* +0x84, room offsets */
    o->ty = 128;                                     /* +0x86 */
    o->step2++;
}
void Dive2(Obj *o) {                                                            /* 0x800F1B50 */
    int a = ratan2(o->y - (ROOM_Y + o->ty), (ROOM_X + o->tx) - o->x);
    o->speed_x = rcos(a) * 96; o->speed_y = rsin(a) * 96;                      /* 6 px/update */
    o->flip = (o->speed_x >= 0) ? FLIP_R : 0;
    MoveObject(o);
    bool there = abs(o->tx - (o->x - ROOM_X)) < 6 && abs(o->ty - (o->y - ROOM_Y)) < 6;
    bool wall = o->flip ? (o->collide & 1) : (o->collide & 2);
    if (there || wall) { PlaySound(2, 18, o); Rumble(8); o->step2++; }
}
void Dive3(Obj *o) { SetAnime(o, 11); o->timer = 90; o->tx = mega.x; o->ty = mega.y; o->step2++; }
void Dive4(Obj *o) {                                                            /* 0x800F3A2C */
    if (--o->timer) return;
    PlaySound(2, 19, o); SetAnime(o, 10);
    int a = ratan2(o->y - o->ty, o->tx - o->x);
    o->speed_x = rcos(a) * 96; o->speed_y = rsin(a) * 96;
    o->flip = (o->speed_x >= 0) ? FLIP_R : 0; o->step2++;
}
void Dive5(Obj *o) {
    MoveObject(o);
    if ((o->collide & 8) || (o->collide & 4) || (o->flip ? (o->collide & 1) : (o->collide & 2))) o->step2++;
}
void Dive6(Obj *o) { o->speed_x = o->accel_x = o->speed_y = 0; o->gravity = 6000; o->step2++; }
void Dive7(Obj *o) { if (o->collide & 8) { PlaySound(2,18,o); Rumble(8); o->step2++; } else MoveObject2(o); }
void Dive8(Obj *o) { SetAnime(o, 9); o->timer = 20; o->step2++; }
void Dive9(Obj *o) { if (--o->timer == 0) WolfangNextMove(o); }

/* ---- step 8: ice slide (0x800F75B4) ---- */
void Slide0(Obj *o) { WolfangFacePlayer(o); SetAnime(o, 16); o->speed_x = o->accel_x = o->speed_y = o->gravity = 0; o->step2++; }
void Slide1(Obj *o) {                                                           /* 0x800F3D44 */
    PlaySound(2, 19, o);
    if (o->anime_flag != 1) return;
    PlaySound(2, (GetRNG() & 1) ? 37 : 38, o);
    o->speed_x = o->flip ? 0x60000 : -0x60000;
    o->tx = o->flip ? o->x + (0x20 | (GetRNG() & 0x1F)) : o->x - (0x20 | (GetRNG() & 0x1F));
    o->contact_damage = 6;
    WolfangSpawnIce(o, 16);
    o->timer = 8; o->step2++;
}
void Slide2(Obj *o) {                                                           /* 0x800F1CF0 */
    MoveObject(o);
    bool passed = o->flip ? (mega.x < o->x || (o->collide & 1)) : (o->x <= mega.x || (o->collide & 2));
    if (passed) { SetAnime(o, 23); o->speed_x = o->flip ? 0x60000 : -0x60000; o->timer = 8; o->step2++; return; }
    if (o->flip ? (o->tx <= o->x) : (o->x <= o->tx)) {
        o->tx = o->flip ? o->x + (0x20 | (GetRNG() & 0x1F)) : o->x - (0x20 | (GetRNG() & 0x1F));
        WolfangSpawnIce(o, 16);
    }
    if (mega.step == 9 || mega.step == 11 || mega.step == 52) { o->step = 9; o->step2 = 0; return; }
    if (--o->timer == 0) { o->timer = 8; WolfangSpawnTrail(o, 1); }
}
void Slide3(Obj *o) { MoveObject(o); if (--o->timer == 0) WolfangNextMove(o); }

/* ---- step 9: leap to the player's wall, bounce down (0x800F75C4) ---- */
void Bounce0(Obj *o) { WolfangFacePlayer(o); SetAnime(o, 18); o->step2++; }
void Bounce1(Obj *o) {                                                          /* 0x800F1EB0 */
    if (!o->anime_flag) return;
    PlaySound(2, 17, o); PlayVoice34_36(o);
    int a = o->flip ? 227 : 1821;                   /* 20 degrees up */
    o->speed_x = rcos(a) * 96; o->speed_y = rsin(a) * 96; o->step2++;
}
void Bounce2(Obj *o) {                                                          /* 0x800F3EEC */
    if (o->flip ? (o->collide & 1) : (o->collide & 2)) {
        PlaySound(2, 18, o); Rumble(8); SetAnime(o, 11);
        WolfangSpawnVisual(o, 32); WolfangSpawnVisual(o, 33);   /* wall runners */
        o->timer = 30; o->step2++;
    } else MoveObject(o);
}
void Bounce3(Obj *o) {                                                          /* 0x800F3F94 */
    if (--o->timer) return;
    PlaySound(2, 19, o); SetAnime(o, 10);
    int a;
    if (o->flip == 0) { o->flip = FLIP_R; a = 3869; } else { o->flip = 0; a = 2275; }  /* 20 degrees down */
    o->speed_x = rcos(a) * 96; o->speed_y = rsin(a) * 96; o->step2++;
}
void Bounce4(Obj *o) {                                                          /* 0x800F1FE0 */
    if (o->collide & 8) { PlaySound(2,18,o); Rumble(8); SetAnime(o, 9); o->timer = 30; o->step2++; return; }
    if (o->flip ? (o->collide & 1) : (o->collide & 2)) {
        PlaySound(2, 18, o); Rumble(8); SetAnime(o, 11);
        WolfangSpawnVisual(o, 32); WolfangSpawnVisual(o, 33);
        o->timer = 30; o->step2--; return;
    }
    MoveObject(o);
}
void Bounce5(Obj *o) { if (--o->timer == 0) WolfangNextMove(o); }

/* ---- step 10: ice balls (0x800F75DC) ---- */
void Shoot0(Obj *o) { o->timer2 = wolfang_ball_counts[mega.var][game.boss_level]; o->step2++; }
void Shoot1(Obj *o) { WolfangFacePlayer(o); SetAnime(o, 13 + mega.var); o->step2++; }
void Shoot2(Obj *o) {                                                           /* 0x800F20C0 */
    switch (o->anime_flag) {
    case 1: PlaySound(2, 21, o); WolfangSpawnMuzzle(o, mega.var); break;
    case 2: PlaySound(2, 24, o); WolfangSpawnVisual(o, mega.var); break;   /* the ball */
    case 3:
        if (--o->timer2) { o->timer = wolfang_ball_waits[mega.var][0]; o->step2++; }
        else { SetAnime(o, 0); o->timer = wolfang_ball_waits[mega.var][1]; o->step2 += 2; }
    }
}
void Shoot3(Obj *o) { if (--o->timer == 0) o->step2 -= 2; }
void Shoot4(Obj *o) { if (--o->timer == 0) WolfangNextMove(o); }

/* ---- step 11: jump to the ceiling (0x800F75FC) ---- */
void Up0(Obj *o) { WolfangFacePlayer(o); SetAnime(o, 19); o->step2++; }
void Up1(Obj *o) {
    if (o->anime_flag != 1) return;
    PlaySound(2, 17, o); PlayVoice34_36(o);
    o->speed_x = o->accel_x = o->gravity = 0; o->speed_y = 0x60000; o->step2++;
}
void Up2(Obj *o) {
    MoveObject(o);
    if (o->collide & 4) { PlaySound(2,18,o); Rumble(8); WolfangFacePlayer(o); SetAnime(o, 20); o->step2++; }
}
void Up3(Obj *o) { if (o->anime_flag == 1) WolfangNextMove(o); }

/* ---- step 12: ice balls from the ceiling (0x800F760C) ---- */
void Volley0(Obj *o) { o->timer2 = wolfang_volleys[game.boss_level]; o->step2++; }
void Volley1(Obj *o) { PlaySound(2, 39, o); WolfangFacePlayer(o); SetAnime(o, 15); o->step2++; }
void Volley2(Obj *o) {                                                          /* 0x800F447C */
    switch (o->anime_flag) {
    case 1: PlaySound(2, 21, o); WolfangSpawnMuzzle(o, 2); break;
    case 2: WolfangSpawnVisual(o, 2); break;                   /* 4 times per animation */
    case 3: o->timer = 40; if (--o->timer2) o->step2++; else o->step2 += 2;
    }
}
void Volley3(Obj *o) { if (--o->timer == 0) o->step2 -= 2; }
void Volley4(Obj *o) { if (--o->timer == 0) WolfangNextMove(o); }

/* ---- step 13: ceiling slide, bounce down (0x800F7624) ---- */
void Roof0(Obj *o) {
    o->flip = (o->x < ROOM_X + 192) ? FLIP_R : 0;
    SetAnime(o, 17); o->speed_x = o->accel_x = o->speed_y = o->gravity = 0; o->step2++;
}
void Roof1(Obj *o) {                                                            /* 0x800F4664 */
    if (o->anime_flag != 1) return;
    PlaySound(2, 19, o);
    o->speed_x = o->flip ? 0x60000 : -0x60000;
    o->tx = o->flip ? o->x + (0x20 | (GetRNG() & 0x1F)) : o->x - (0x20 | (GetRNG() & 0x1F));
    WolfangSpawnIce(o, 18); o->timer = 8; o->step2++;
}
void Roof2(Obj *o) {                                                            /* 0x800F21E8 */
    MoveObject(o);
    if (o->flip ? (o->collide & 1) : (o->collide & 2)) {
        PlaySound(2, 18, o); Rumble(8); SetAnime(o, 11); o->step2++; return;  /* timer kept */
    }
    if (o->flip ? (o->tx <= o->x) : (o->x <= o->tx)) {
        o->tx = o->flip ? o->x + (0x20 | (GetRNG() & 0x1F)) : o->x - (0x20 | (GetRNG() & 0x1F));
        WolfangSpawnIce(o, 18);
    }
    if (--o->timer == 0) { o->timer = 8; WolfangSpawnTrail(o, 1); }
}
/* Roof3 = Bounce3 (0x800F470C), Roof4 = Bounce4 without the wall runners (0x800F47D0), Roof5 = Bounce5 */

/* ---- state 2: death (0x800F7550) ---- */
void Death0(Obj *o) {                                                           /* 0x800F2F24 */
    PlaySound(2, 32, o); mega.byteCA = 0; PlayerEvent(20, mega.flip);
    SetAnime(o, 25); o->step = 1; o->timer = 127; o->timer2 = o->iframes = 25;
    o->clut &= 0x7FFF; DisplayCheck(o, 96, 96);
}
void Death1(Obj *o) {                                                           /* 0x800F2FB0 */
    if (--o->timer == 0) {
        o->step = 2;
        Obj *e = GetSysObject();
        if (e) { e->flags = 1; e->type = 26; e->x = o->x; e->y = o->y; o->child = e; }  /* BossExplosion */
    }
    DisplayCheck(o, 96, 96);
    if (--o->timer2 == -1) {
        o->clut ^= 0x8000; o->iframes -= 5; if (o->iframes >= 26) o->iframes = 0;
        o->timer2 = (o->iframes < 6) ? 5 : o->iframes;
    }
}
void Death2(Obj *o) {                                                           /* 0x800F18C4 */
    o->display = 0;
    if (o->child->flags) {
        if (o->child->act4 == 0) {
            if (--o->timer2 == -1) { o->timer2 = 5; o->clut ^= 0x8000; }
            DisplayCheck(o, 96, 96);
        }
        return;
    }
    memset(&o->work80, 0, 0x18);
    game.boss_active = 0; game.boss = NULL;
    if (game.stage == 12) { game.exit_req = 0x80; game.cleared[game.checkpoint] = 1; game.checkpoint += 8; }
    else WolfangSpawnSoul(wolfang_fight_time < 5401 ? 0x10 : 0x11);   /* main 15 */
    DeleteObject3(o);
}

/* ---- state 3: trail image (main 54 var 1) ---- */
void WolfangTrail(Obj *o) {                                                     /* 0x800F30A4 */
    if (o->parent->state != 1) { o->state = 5; o->display = 0; return; }
    if (o->step == 0) { if (--o->timer == 0) { o->timer = 4; o->attack_box = NULL; o->step++; } }
    else {
        o->sprite = o->parent->sprite; o->display = 1;
        if (--o->timer == 0) { o->display = 0; o->state = 5; }
    }
    DisplayCheck(o, 64, 64);
}

/* ---- state 4: ice spike (main 54 var 16-18) ---- */
void WolfangIce(Obj *o) {                                                       /* 0x800F31A4 */
    if (o->parent->state != 1) { o->state = 5; o->display = 0; return; }
    AnimeTick(o);
    switch (o->step) {
    case 0:                                                                     /* 0x800F326C */
        if (o->anime_flag == 1) {
            o->timer = 300;
            if (o->var == 17) { o->hurt_box = box_70D4; o->attack_box = box_70D8; }
            else              { o->hurt_box = box_70CC; o->attack_box = box_70D0; }
            o->step++;
        }
        break;
    case 1:                                                                     /* 0x800F2464 */
        if (o->parent->step != o->last_parent_step && o->parent->step == 3 && (GetRNG() & 1))
            goto end_of_life;                                 /* the boss was just stunned */
        if (o->var == 18 && abs(o->x - mega.x) < 33) goto fall;
        if (--o->timer) break;
    end_of_life:
        if (o->var == 18) {
    fall:   o->terrain_box = box_70DC; o->speed_x = o->accel_x = o->speed_y = 0;
            o->gravity = 19660; o->step++; break;
        }
        SpawnHitEffect(o); WolfangSpawnDebris(o, 16); WolfangSpawnDebris(o, 17); WolfangSpawnDebris(o, 18);
        o->state = 5; break;
    case 2:                                                                     /* 0x800F32F4 */
        if (o->collide & 8) {
            SpawnHitEffect(o); WolfangSpawnDebris(o, 16); WolfangSpawnDebris(o, 17); WolfangSpawnDebris(o, 18);
            o->state = 5;
        } else MoveObject2(o);
    }
    if (ContactWeapon(o) < 0) { SpawnExplosion(o); o->display = 0; o->state = 5; o->step = 0; return; }
    ContactMega(o);
    DisplayCheck(o, 32, 32);
    o->last_parent_step = o->parent->step;                                     /* +0x84 */
}

/* ---- visual 20 (0x800F54F8): ice ball var 0-2, debris var 16-18, wall runner var 32/33 ---- */
void WolfangShotInit(Vis *v) {                                                  /* 0x800F2750 */
    if (v->parent->state != 1) { v->state = 2; v->display = 0; return; }
    v->contact_damage = 0; v->hp = 1; v->priority = 4; v->latch = 1; v->damage_table = dmg_tbl_deflect;
    v->terrain_box = v->attack_box = v->hurt_box = NULL; v->display = 0;
    v->state = 1; v->step = v->step2 = v->act4 = 0; v->speed_x = v->speed_y = v->accel_x = v->gravity = 0;
    switch (v->var & 0xF0) {
    case 0x00:                                       /* ice ball */
        SetAnime(v, 27); v->damage_table = dmg_tbl_minor; v->contact_damage = 4; v->hp = 2;
        v->x += (v->var == 1 ? 30 : 24) * (v->flip ? 1 : -1);
        v->y += (v->var == 0) ? -8 : 8;
        v->step = 2; break;
    case 0x10:                                       /* debris */
        v->damage_table = dmg_tbl_minor; v->contact_damage = 2; v->hp = 1; v->flip = 0; v->step = 0; break;
    case 0x20:                                       /* wall runner */
        v->terrain_box = box_769C; v->flip = 0; v->step = 1; break;
    }
}
void WolfangShotMain(Vis *v) {                                                  /* 0x800F2954 */
    if (v->parent->state != 1) { v->state = 2; v->display = 0; return; }
    v->prev_x = v->x; v->prev_y = v->y;
    AnimeTick(v);
    shot_steps[v->step](v);                          /* 0: debris, 1: runner, 2: ball */
    if (v->state != 1) { v->display = 0; return; }
    int r = ContactWeapon(v);
    if (r < 0) {
        if ((v->var & 0xF0) == 0) { PlaySound(2, 23, v); Rumble(8); }
        goto boom;
    }
    if (ContactMega(v)) {
        if ((v->var & 0xF0) == 0) { PlaySound(2, 23, v); Rumble(8); goto boom; }
        if ((v->var & 0xF0) == 0x10) goto boom;
    }
    if (OffScreenCheck2(v, 32, 80)) v->state = 2; else DisplayCheck(v, 16, 16);
    if ((v->var & 0xF0) == 0x20) v->display = 0;     /* runners are never drawn */
    return;
boom:
    SpawnExplosion(v); v->display = 0; v->state = 2;
}
void WolfangDebrisInit(Vis *v) {                                                /* 0x800F2AD4 */
    SetAnime(v, 28 + (frame_count + v->var) % 3);
    if (game.difficulty >= 2) { v->hurt_box = box_7694; v->attack_box = box_7698; }
    v->terrain_box = NULL;
    int dx = GetRNG() % 40 + 10, h = GetRNG() % 30 + 30, t = GetRNG() % 10 + 15;
    v->gravity = (h << 16) / (t * t / 2); v->speed_y = v->gravity * t; v->accel_x = 0;
    if (v->var == 17 || (v->var == 18 && !(GetRNG() & 1))) v->speed_x = (dx << 16) / t;
    else v->speed_x = (-dx << 16) / t;
    v->step2++;                                      /* then MoveObject2 each update */
}
void WolfangShotLaunch(Vis *v) {                                                /* 0x800F2CC4 */
    if (v->parent->step == 3) { v->state = 2; v->step = 0; v->display = 0; return; }
    if (!v->anime_flag) return;
    PlaySound(2, 22, v);
    v->hurt_box = box_76A0; v->attack_box = box_76A4; v->terrain_box = box_76A8;
    if (v->var == 0) {
        v->gravity = 0x600000 / 264; v->speed_y = 0x85D11; v->accel_x = 0;
        v->speed_x = ((mega.x - v->x) << 16) / 46;
    } else {
        int a = ratan2(v->y - mega.y, mega.x - v->x);
        int k = (v->var == 1) ? 64 : 112;            /* 4.0 or 7.0 px/update */
        v->speed_x = rcos(a) * k; v->speed_y = rsin(a) * k; v->accel_x = v->gravity = 0;
    }
    v->step2++;
}
void WolfangShotFly(Vis *v) {                                                   /* 0x800F55FC */
    MoveObject2(v);
    if (v->collide || (CornerCheck(v), v->collide)) {
        PlaySound(2, 23, v); Rumble(8); SpawnHitEffect(v);
        WolfangShotDebris(v, 16); WolfangShotDebris(v, 17); WolfangShotDebris(v, 18);
        v->state = 2;
    }
}
void WolfangWallRunner(Vis *v) {                                                /* 0x800F56CC, 0x800F570C */
    if (v->step2 == 0) {
        v->speed_x = v->accel_x = v->gravity = 0;
        v->speed_y = (v->var == 32) ? 0x100000 : -0x100000;    /* 16 px up / down per move */
        v->timer = 8; v->step2++; return;
    }
    if (--v->timer == 0) { v->timer = 8; WolfangRunnerSpawnIce(v, 17); MoveObject(v); }
    if (v->collide || (CornerCheck(v), v->collide)) v->state = 2;
}

/* ---- effect 8: mouth ice (0x800F599C) ---- */
void WolfangMuzzleInit(Eff *e) {                                                /* 0x800F5788 */
    e->priority = 4; SetAnime(e, 26);
    e->x += (e->var == 1 ? 30 : 24) * (e->flip ? 1 : -1);
    e->y += (e->var == 0) ? -8 : 8;
    e->display = 0; e->state = 1; e->step = e->step2 = 0;
}
void WolfangMuzzleMain(Eff *e) {                                                /* 0x800F5888 */
    Obj *b = e->parent;                              /* +0x50 */
    if (b->state != 1 || b->step == 3) { e->display = 0; e->state++; return; }
    AnimeTick(e);
    if (e->var == 2 ? (b->anime_flag == 4) : (e->anime_flag == 1)) e->state++;   /* 0x800F5940 */
    DisplayCheck(e, 32, 32);
}
