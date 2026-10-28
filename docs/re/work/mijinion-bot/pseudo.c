/* Illumina (stage 8, overlay 11, main type 24) - pseudo code.
 * A plausible reading of the game code, not original code, not compiled.
 * Addresses are RAM addresses of overlay 11. Names: helpers-proposed.json.
 */

/* ---- shared fight state (0x800FD510..) ---- */
Object *illumina_body;          /* 0x800FD510 */
s8  illumina_phase;             /* 0x800FD514: 0 walk 1, 1 fight 1, 2 walk 2, 3 fight 2, 4 dead */
s16 illumina_target_x, illumina_target_y; /* 0x800FD516/518, screen coordinates */
s8  illumina_probe_shown;       /* 0x800FD51A */
s8  illumina_laser;             /* 0x800FD51B: 0 scan, 1 locked, 2 fire, -1 fired, -3 head home */
s8  illumina_walls_done;        /* 0x800FD51C */
s8  illumina_clear;             /* 0x800FD51D */
s8  illumina_drones_done;       /* 0x800FD51E */
s8  illumina_wall_rows;         /* 0x800FD51F */
s8  illumina_walls_up;          /* 0x800FD520 */
s8  illumina_cannons;           /* 0x800FD521 */
s8  illumina_drones;            /* 0x800FD522 */
u16 illumina_cannon_mask;       /* 0x800FD524 */
u16 illumina_time;              /* 0x800FD526 */
u16 illumina_old_max_y;         /* 0x800FD52A */

/* ---- entry 0x800F20D4 ---- */
void Illumina(Object *o) {
    o->prev_x = o->x; o->prev_y = o->y;
    illumina_states[o->state](o);  /* 0 init, 1 body, 2 parts, 3 DeleteObject3, 4 DeleteObject2 */
}

/* ---- 0x800ED7C8 ---- */
void IlluminaInit(Object *o) {
    clear(o); o->anime_table = illumina_anims; o->priority = 5;
    o->damage_table = dmg_tbl_illumina; o->layer = mega.layer;
    switch (o->var & 0xF0) {
    case 0x00:                                   /* body */
        illumina_body = o; clear_fight_state();
        o->pattern = GetRNG() & 15; o->jet_timer = 120; o->contact = 4;
        o->layer = -1; o->state = 1; o->step = 2;            /* INTRO */
        cam0.target_max_x = 2048;
        break;
    case 0x10:                                   /* head (16) and drones (17-19) */
        if (illumina_clear) { o->state = 3; return; }
        o->layer = -1; o->display = 1; o->x = 160; o->y = 72; o->state = 2;
        o->phase = GetRNG() << 4; o->hurt_box = BOX(-8, -8, 16, 16); o->hp = 16;
        if (o->var & 15) use_bank_plus_1(o);
        switch (o->var & 15) {
        case 0: SetAnime(o, 80); o->priority = 48; o->attack_box = o->hurt_box = 0;
                o->hp = 40; o->step = 2; break;           /* head */
        case 1: SetAnime(o, 33); o->step = 3; break;
        case 2: SetAnime(o, 37); o->step = 3; break;
        case 3: SetAnime(o, 37); o->step = 3; break;
        }
        break;
    case 0x20:                                   /* probe */
        o->priority = 2; o->layer = -1; o->display = 0; o->state = 2;
        use_bank_plus_1(o); SetAnime(o, 53); o->step = 5; break;
    case 0x30:                                   /* wall cannons 49-56, world space */
        o->display = 1; o->layer = 0; o->state = 2; use_bank_plus_1(o); SetAnime(o, 55);
        o->flip = ((o->var & 15) < 5) ? 0x40 : 0; o->hp = 20; o->step = 6; break;
    case 0x40:                                   /* wall controller */
        PlaySound(2, 56, o); illumina_walls_up = 1; o->layer = -1; o->state = 2;
        o->hp = 1; o->step = 7; break;
    }
    o->step2 = 0;
}

/* ---- body, state 1: 0x800F2208 ---- */
void IlluminaBodyMain(Object *o) {
    o->x = 160; o->y = 136;                       /* screen coordinates (layer -1) */
    AnimeTick(o);
    body_steps[o->step](o);                       /* 2 INTRO .. 10 SHOCK */
    IlluminaHpFill(o);
    int r = ContactWeapon(o);
    if (r < 0) o->hp = 0;
    else if (r > 0 && r != 127 && (r == 7 || r == 16 || r == 43) && o->step != 10) {
        illumina_clear = 1; o->step = 10; o->step2 = 0;    /* ids 6, 15, 42 */
    }
    ContactMega(o);
    IlluminaPhase(o);
}

/* ---- 0x800F1BE8 ---- */
void IlluminaPhase(Object *o) {
    switch (illumina_phase) {
    case 0:
        if (cam0.x >= 2048) {
            illumina_phase = 1;
            cam0.target_min_x = 2048; illumina_old_max_y = cam0.target_max_y;
            cam0.target_max_y = cam0.target_min_y = 384;
            o->fill = 1; o->hp = 1; o->attack_box = o->hurt_box = 0;
        } else if (o->step != 2) IlluminaJetSpawner(o, 0);
        break;
    case 1:
        if (illumina_body->hp == 0) {
            illumina_phase = 2; illumina_clear = 1;
            o->attack_box = o->hurt_box = 0; o->step = 8; o->step2 = 0;
        }
        IlluminaJetSpawner(o, 1);
        break;
    case 2:
        if (cam0.x >= 4224) {
            illumina_phase = 3; illumina_time = 0;
            cam0.target_min_x = 4224; illumina_old_max_y = cam0.target_max_y;
            cam0.target_max_y = cam0.target_min_y = 768;
            o->fill = 1; o->hp = 1; o->attack_box = o->hurt_box = 0;
        }
        IlluminaJetSpawner(o, 0);
        break;
    case 3:
        illumina_time++;
        if (illumina_body->hp == 0) {
            game.checkpoint = 2; illumina_clear = 1; illumina_phase = 4;
            o->attack_box = o->hurt_box = 0; o->step = 9; o->step2 = 0;
        }
        break;
    }
}

/* ---- 0x800F3D24 ---- */
void IlluminaHpFill(Object *o) {
    if (o->fill == 1) { game.boss_bar = 1; o->hp = 1; tick = 3; o->attack_box = o->hurt_box = 0; o->fill = 2; }
    else if (o->fill == 2) {
        if (--tick == 0) { PlaySound(0, 14, 0); tick = 3; }
        if (++o->hp == boss_hp_max) {            /* 64 */
            o->fill = 0;
            o->attack_box = o->hurt_box = BOX(104, -46, 24, 26);   /* the lens on the cable */
        }
    }
}

/* ---- 0x800F1DC4 ---- */
void IlluminaJetSpawner(Object *o, int random_side) {
    static const s16 places[2][2] = { { -16, 64 }, { 336, 64 } };   /* 0x800E9A1C */
    int side = random_side ? (GetRNG() & 1) : 1;
    if (o->jet_timer) {
        if (--o->jet_timer == 0) {
            Object *j = GetMainObject();
            if (j) { j->type = 8; j->var = 0; j->x = cam0.x + places[side][0];
                     j->y = cam0.y + places[side][1]; set_bank_graphics(j); }
        }
    } else if (!any_main_object_of_type(8)) o->jet_timer = 120;
}

/* ---- 0x800F0050 ---- */
void IlluminaThink(Object *o) {
    o->step2 = 0;
    if (o->pattern_phase != illumina_phase ||
        illumina_patterns[illumina_phase][o->pattern][o->pattern_i] == 0xFF) {
        o->pattern_phase = illumina_phase; o->pattern = GetRNG() & 15; o->pattern_i = 0;
    }
    o->step = illumina_patterns[illumina_phase][o->pattern][o->pattern_i++];
}

/* ---- ZOOM SHOTS, step 4 (0x800F279C, 0x800EE034, 0x800F2840) ---- */
void IlluminaZoomShots(Object *o) {
    switch (o->step2) {
    case 0: o->act4 = 6; spawn_pieces(o, 32, 36); o->timer = 180;
            o->shots = GetRNG() % 3 + 3; o->step2 = 1; break;
    case 1:
        o->act4 = 0;
        if (--o->timer) {
            if (o->timer == (illumina_phase == 1 ? 22 : 30)) IlluminaSpawnShot(o, 0);
            break;
        }
        if (illumina_phase == 0) { o->timer = 90; IlluminaSpawnPiece(o, 41); }
        else if (illumina_phase == 1) { o->timer = 45; IlluminaSpawnPiece(o, 42); }
        else if (--o->shots == -1) { o->act4 = 6; spawn_pieces(o, 37, 40); o->timer = 60; o->step2 = 2; }
        else { o->timer = 90; IlluminaSpawnPiece(o, 41); }
        break;
    case 2: o->act4 = 0; if (--o->timer == 0) IlluminaThink(o); break;
    }
}

/* ---- DRONES, step 5 (0x800F28C0..0x800F2B08) ---- */
void IlluminaDrones(Object *o) {
    switch (o->step2) {
    case 0: o->act4 = 6; IlluminaSpawnPiece(o, 2); spawn_pieces(o, 16, 19); o->timer = 40; o->step2++; break;
    case 1: o->act4 = 0; if (--o->timer == 0) { o->act4 = 6; spawn_pieces(o, 48, 53); o->timer = 180; o->step2++; } break;
    case 2:
        o->act4 = 0;
        if (o->timer && --o->timer == 18) {
            illumina_laser = 0; illumina_drones_done = 0; illumina_probe_shown = 0; illumina_drones = 0;
            IlluminaSpawnPart(o, 16); IlluminaSpawnPart(o, 17);
            IlluminaSpawnPart(o, 18); IlluminaSpawnPart(o, 19);
            o->step2++;
        }
        break;
    case 3: o->act4 = 0; if (illumina_laser == -3) { o->act4 = 6; spawn_pieces(o, 54, 59); o->timer = 180; o->step2++; } break;
    case 4: o->act4 = 0; if (--o->timer == 0) IlluminaThink(o); break;
    }
}

/* ---- head, var 16, step 2 (0x800EF0EC, 0x800EF204, 0x800F3610, 0x800EF304) ---- */
void IlluminaHead(Object *o) {
    switch (o->step2) {
    case 0: move_to(o, 160, 32, 2.0);           /* ratan2 + rcos/rsin << 5 */
            if (near(o, 160, 32, 3)) { IlluminaSpawnPart(o, 32); IlluminaSpawnRay(o, 0);
                o->x = 160; o->y = 32; o->head_timer = 360; o->step2 = 1; }
            break;
    case 1: o->y = 32 + (-rsin(o->phase) * 6 >> 12); o->phase += 20;
            if (illumina_phase == 3) {
                if (--o->head_timer == 0 || illumina_time < 11) {
                    illumina_laser = 0; illumina_drones_done = 1; o->step2 = 3; }
            } else if (illumina_drones == 0) {
                illumina_laser = 0; illumina_drones_done = 1; o->step2 = 3;
            } else if (illumina_laser == -1) { o->timer = 60; o->step2 = 2; }
            break;
    case 2: if (--o->timer == 0) { illumina_drones_done = 0; illumina_laser = 0;
                IlluminaSpawnPart(o, 32); IlluminaSpawnRay(o, 0); o->step2 = 1; }
            break;
    case 3: move_to(o, 160, 72, 2.0);
            if (near(o, 160, 72, 3)) { illumina_drones_done = 0; illumina_laser = -3; o->state = 3; }
            break;
    }
}

/* ---- probe, var 32, step 5 (0x800F384C, 0x800F38C4, 0x800EFCC0, 0x800F3908) ---- */
void IlluminaProbe(Object *o) {
    probe_steps[o->step2](o);
    if (BoxOverlap(o, &mega) == 1 && illumina_laser == 0) illumina_laser = 1;
}
/* sub 0: box (-16,-16,32,32); angle 2560; origin = head (x, y - 14).
 * sub 1: march from the origin in 8 px steps (cos/sin << 7) up to 70 steps, in world
 *        coordinates, until a solid tile (1-36, >= 48) or the screen edge; the end point
 *        becomes illumina_target (screen); the angle goes 2560 <-> 3584 by 20 per update.
 *        illumina_laser 0: show the marker (piece var 4).
 *        illumina_laser 1: the point = the player's place (screen); marker; timer 60; sub 2.
 * sub 2: while locked, show the marker; after 60 updates illumina_laser = 2 and delete. */

/* ---- drones, var 17-19, step 3 (0x800EF408, 0x800EF5B8, 0x800F36B8, 0x800F36F8) ---- */
/* sub 0: fly at 3 px/update to (288, 96) / (32, 32) / (80, 176).
 * sub 1: a = ratan2 to illumina_target; aim pose illumina_drone_aim_frames[a >> 8];
 *        bob +-8 px around the base y (+30 per update);
 *        if illumina_laser == 2: fire animation, IlluminaSpawnShot(o, 16) (hit point,
 *        contact 6, 3 updates), rays 16-19, timer 50, sub 2.
 * sub 2: after 50 updates illumina_laser = -1; sub 1.
 * sub 3: fly off the screen (not entered by the code read). */

/* ---- parts, state 2: 0x800EED14 ---- */
void IlluminaPartMain(Object *o) {
    if (illumina_clear) {
        if ((o->var & 0xF0) == 0x40) { remove_walls_with_explosions(); /* CopyTiles restore */ }
        o->state = 3; o->display = 0; return;
    }
    if (o->var >= 17 && o->var <= 19 && illumina_drones_done) { explode(o); o->state = 3; return; }
    if (o->var == 32 && illumina_drones_done) { o->state = 3; return; }
    AnimeTick(o);
    part_steps[o->step](o);
    if (ContactWeapon(o) < 0) {
        if ((o->var & 0xF0) == 0x10) { illumina_drones--; explode(o); SpawnJunk(4, debris_49_52, o); o->state = 3; }
        else if ((o->var & 0xF0) == 0x30) { SetAnime(o, 65); explode(o); SpawnJunk(4, debris_66_69, o);
                                           o->step2 = 4; illumina_cannons--; }
    }
    if ((o->var & 0xF0) != 0x20) ContactMega(o);
}
