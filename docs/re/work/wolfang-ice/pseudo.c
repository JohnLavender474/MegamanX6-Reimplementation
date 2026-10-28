/*
 * Blizzard Wolfang's stage (stage 2, area 0, overlay 04): the ice objects.
 * Readable pseudo code, from the kits in this folder:
 *   kit-main17/   main type 17   (ice block, 8 modes by var)
 *   kit-logic12/  stage logic 12 (ice fall controller: makes the type 17 spawners)
 *   kit-logic7/   stage logic 7  (the slide on ice floors and ice slopes)
 * Not original code, not compiled. Units: 16.16 fixed point, 0x10000 = 1 px or 1 px/frame.
 * speed_y is positive upwards (y -= speed_y). Addresses are RAM addresses of overlay 04.
 */

/* ===================================================================== */
/* Main type 17: ice block                                               */
/* entry 0x800EED40, states 0x800F6CE8, steps 0x800F6CF4                */
/* ===================================================================== */

static const Box ice_terrain[2] = { {0,0,30,30}, {0,0,22,22} };        /* 0x800F6C48: centre, half sizes */
static const Box ice_attack[2]  = { {-28,-28,56,56}, {-22,-22,44,44} }; /* 0x800F6C50 */
static const Box ice_hurt[2]    = { {-32,-32,64,64}, {-24,-24,48,48} }; /* 0x800F6C58 */
static const u8  ice_debris[]   = { 4, 5, 6, 7, 8 };                    /* 0x800F6CE0, SpawnJunk(3, ...) */

void IceBlock(Obj *o)                                   /* 0x800EED40 */
{
    if (mega.x >= 4673 && mega.y >= 1265)               /* the end of the ice part of the stage */
        DeleteObject3(o);                               /* HAZARD: the state function below still runs on the cleared slot */
    ice_states[o->state](o);                            /* 0 IceBlockInit, 1 IceBlockMain, 2 IceBlockDelete */
}

void IceBlockInit(Obj *o)                               /* 0x800EE334, state 0 */
{
    o->hp = 8;  o->contact_damage = 8;
    o->damage_table = dmg_tbl_ice_block;                /* 0x80075CDC */
    o->iframes = 0;
    o->anime_table = 0x800F6CBC;
    o->speed_x = o->speed_y = o->accel_x = o->gravity = 0;
    o->latch = 0; o->collide_flags = 0; o->flip = 0;
    o->priority = 6; o->layer = mega.layer;
    o->prev_x = o->x; o->prev_y = o->y;
    o->size = GetRNG() & 1;                             /* +0x80: 0 large (64 px), 1 small (48 px) */
    o->terrain_box = &ice_terrain[o->size];
    o->hurt_box    = &ice_hurt[o->size];
    o->attack_box  = &ice_attack[o->size];
    o->count81 = 0;

    switch (o->var) {                                   /* switch 0x800E9AB0; var >= 8: no case, step stays 0 */
    case 0: o->timer = 120; goto spawner;               /* avalanche spawner, left  */
    case 1: o->timer = 12;                              /* avalanche spawner, right (no entry of the controller uses it) */
    spawner:
        o->hurt_box = o->attack_box = 0; o->display = 0; o->step = 3; break;
    case 2: o->hurt_box = o->attack_box = 0; o->timer = 0; o->step = 3; break;               /* falling ice spawner */
    case 3: o->hurt_box = o->attack_box = 0; o->timer = 0; o->count = 0; o->step = 3; break; /* bouncing block spawner */
    case 4:                                             /* avalanche block, down-left along a 1:2 slope */
        o->display = 1; o->speed_x = -8.0; o->speed_y = -4.0;
        o->hurt_box = o->attack_box = 0;
        goto avalanche;
    case 5:                                             /* avalanche block, down-right (boxes kept: they are off in step 4 anyway) */
        o->display = 1; o->speed_x = +8.0; o->speed_y = -4.0;
    avalanche:
        SetAnime(o, o->size);                           /* 0 or 1: the turning block */
        o->step = 4; o->terrain_box = 0; o->timer = 50;
        break;
    case 6:                                             /* falling ice piece */
        o->display = 1; o->speed_y = -9.0;              /* 9 px/frame down; terrain box kept, but never used */
        SetAnime(o, o->size + 2);                       /* 2 or 3: one still frame */
        o->step = 1;
        break;
    case 7:                                             /* bouncing block */
        o->display = 1;
        if (mega.x < o->x) { o->speed_x = -1.125; o->flip = 0;    }
        else               { o->speed_x = +1.125; o->flip = 0x40; }
        o->speed_y = 2.0625; o->gravity = 0x1080;       /* 0.064453125 px/frame^2 */
        SetAnime(o, o->size);
        o->step = 2; o->timer = 20;
        break;
    }
    o->state = 1; o->step2 = 0; o->act4 = 0;
}

void IceBlockMain(Obj *o)                               /* 0x800EE5C4, state 1 */
{
    o->prev_x = o->x; o->prev_y = o->y;
    ice_steps[o->step](o);                              /* 0 HitReactionDispatch, 1 Move, 2 Bounce, 3 Spawner, 4 Appear */

    int r = ContactWeapon(o);
    if (r < 0) {                                        /* killed: only weapon ids 4 and 13 (3 each) and 55 hurt it */
        if (game.b27 == 0) game.b27 = 120;              /* game+0x27: reader not found */
        if (o->var == 4 || o->var == 5 || o->var == 7)
            DropItem(o, 21);
        SpawnJunk(3, ice_debris, o);
        o->state = 2;
        return;
    }
    ContactMega(o);

    if (o->var == 4) {
        if (o->x < mega.x - 240 || game.checkpoint > 0) o->state = 2;
    } else if (o->var == 5) {
        if (o->x > mega.x + 240 || (mega.y >= 529 && mega.x >= 5025)) o->state = 2;
    } else if (o->var >= 6) {
        if (OffScreenCheck2(o, 240, 64) == 1) o->state = 2;
        o->latch = (o->collide_flags & 0x08) ? 0 : (o->speed_y < 0 ? -1 : 1);
    }                                                   /* var 0-3: nothing */
}

void IceBlockDelete(Obj *o) { DeleteObject3(o); }       /* 0x800EEDB8, state 2 (the record pointer is 0: no record) */

void IceBlockMove(Obj *o)                               /* 0x800EEDD8, step 1: var 4, 5 after the blink; var 6 */
{
    o->act4++;                                          /* age in frames; the controller reads it */
    AnimeTick(o);
    MoveObject(o);                                      /* straight line, no gravity, no terrain test */
}

void IceBlockAppear(Obj *o)                             /* 0x800EEE14, step 4: var 4, 5 */
{
    AnimeTick(o);
    if (o->timer != 0) {                                /* 50 frames: blink in place, harmless, not hittable */
        o->hurt_box = o->attack_box = 0;
        o->timer--;
        o->display ^= 1;
        return;
    }
    o->step = 1; o->display = 1;
    o->hurt_box = &ice_hurt[o->size]; o->attack_box = &ice_attack[o->size];
}

void IceBlockBounce(Obj *o)                             /* 0x800EE78C, step 2: var 7 */
{
    if (o->timer != 0) {                                /* 20 frames: blink in place */
        o->hurt_box = o->attack_box = 0;
        o->timer--;
        o->display ^= 1;
        return;
    }
    if (((o->collide_flags & 1) && o->speed_x > 0) ||   /* a wall in the move direction: break */
        ((o->collide_flags & 2) && o->speed_x < 0)) {
        PlaySound(2, 17, o);
        SpawnJunk(3, ice_debris, o);
        o->state = 2;                                   /* no item roll; the rest of this update still runs */
    }
    o->display = 1;
    o->hurt_box = &ice_hurt[o->size]; o->attack_box = &ice_attack[o->size];
    AnimeTick(o);
    MoveObject2(o);
    if (o->collide_flags & 0x08) {                      /* floor of the last update: hop again */
        o->speed_y = 2.0625; o->gravity = 0x1080;
        Quake2(8, 2, 2);
        PlaySound(2, 17, o);
    }
    CollideCheck(o);
}

/* step 3: the spawners (var 0-3). New blocks copy the sprite source of the spawner. */
static void IceChild(Obj *o, Obj *n, int var)   /* fills the slot that GetMainObject gave */
{
    {
        n->flags = 0x41; n->type = 17; n->var = var;
        n->clut_cord = o->clut_cord & 0x7FFF;
        n->anime_table = o->anime_table; n->sprite_data = o->sprite_data; n->tex_cord = o->tex_cord;
    }
}

void IceBlockSpawner(Obj *o)                            /* 0x800EE8D8, step 3 */
{
    Obj *n;
    if (o->timer != 0) { o->timer--; return; }          /* +0x7C */

    switch (o->var) {
    case 0:                                             /* avalanche: one block every 7 frames while count > 0 */
    case 1:
        if (o->var == 0 && game.checkpoint > 0) { o->state = 2; return; }
        if (o->count == 0) { o->state = 2; return; }    /* +0x7E, set to 360 by the controller */
        n = GetMainObject();                            /* the free slot is only filled on count % 7 == 0 */
        if (n && o->count % 7 == 0) {
            IceChild(o, n, o->var == 0 ? 4 : 5);
            n->x = o->x;
            n->y = o->y + (GetRNG() & 31) - 16;
        }
        o->count--;
        return;

    case 2:                                             /* falling ice: 18 pieces in 180 frames, then a 120-frame pause */
        if (o->count == 0) { o->timer = 120; o->count = 180; return; }
        n = GetMainObject();
        if (n && o->count % 10 == 0) {
            IceChild(o, n, 6);
            n->x = o->x + (GetRNG() & 15) - 8;
            n->y = o->y;
        }
        o->count--;
        return;

    case 3:                                             /* bouncing blocks: only while X is 48 px or more away in x */
        if (abs(mega.x - o->x) < 48) return;
        if (o->count != 0) { o->count--; return; }
        n = GetMainObject();
        if (n && o->count % 31 == 0) {                  /* count is 0 here, so always true */
            IceChild(o, n, 7);
            n->x = o->x + (GetRNG() & 15) - 8;
            n->y = o->y;
        }
        if (o->count81++ >= 3) { o->count81 = 0; o->timer = 140; }
        o->count = 100;
        return;
    }
}

/* ===================================================================== */
/* Stage logic 12: ice fall controller                                   */
/* entry 0x800F049C, states 0x800F6DFC; one record in the fixed list    */
/* ===================================================================== */

typedef struct { s16 x0; s16 x; s16 y; u8 var; u8 pad; } IceTrigger;
extern const IceTrigger ice_triggers[15];              /* 0x800F6E08; a 16th row is read by no code */
/* the logic object: +0x05 sound flag A (5/9 playing), +0x06 sound flag B (5/8 playing),
   +0x07 sound timer, +0x14 shake counter (s16), +0x16..+0x24 one "done" byte per trigger */

void IceFallController(Logic *o)                       /* 0x800F049C */
{
    if (mega.x >= 4833 && mega.y >= 1217) {             /* the end of the ice part */
        CopyTiles(0, 4672, 1056, &tiles_0x800F6DF4);    /* src {4672, 912, 3, 1} */
        PlaySound(5, 3, 0);
        DeleteObject3(o);
        return;
    }
    if (o->shake != 0) {                                /* avalanche: camera y +3, -3, +3, ... */
        camera_layer0.y += (o->shake & 1) ? -3 : +3;
        o->shake--;
    }
    ctrl_states[o->state](o);                           /* 0 init, 1 main, 2 DeleteObject3 */
}

void IceFallControllerInit(Logic *o)                    /* 0x800F058C */
{
    memset(&o->done[0], 0, 15);                         /* +0x16 .. +0x24 */
    game.b27 = 0;
    o->shake = 0; o->step = o->step2 = o->act4 = 0;
    o->state++;
}

void IceFallControllerMain(Logic *o)                    /* 0x800EFFD8 */
{
    if (game.checkpoint >= 3) return;

    /* 1. Count the live pieces and start the rumble sounds. */
    int falling = 0, avalanche = 0;
    for (int i = 47; i >= 0; i--) {
        Obj *m = &main_objects[i];
        if (!m->flags || m->type != 17) continue;
        if (m->var == 6 && m->act4 >= 7) {
            falling++;
            if (o->step == 0) { PlaySound(5, 9, 0); Rumble(5); o->step = 1; }
        } else if (m->var == 4 || m->var == 5) {
            avalanche++;
            if (o->step2 == 0 && o->act4 == 0) { PlaySound(5, 8, 0); o->step2 = 1; o->act4 = -16; }
        }
    }

    /* 2. Stop the rumble (5/3: a 56-sample tone on the same SPU voice 14). As read; see notes.md. */
    if (game.checkpoint > 0 && mega.y >= 625) PlaySound(5, 3, 0);
    if (falling == 0 && o->step == 0) { PlaySound(5, 3, 0); o->step = 0; }
    if (o->act4 != 0) o->act4--;
    if (falling == 0 && o->step2 == 0) {
        PlaySound(5, 3, 0); o->step = 0;
    } else if (avalanche == 0 && o->step == 0) {
        if (o->act4 != 0) o->act4--;
        else { PlaySound(5, 3, 0); o->step2 = 0; }
        goto triggers;                                  /* (the act4 test below is skipped on this path when act4 was not 0) */
    }
    if (o->act4 != 0) o->act4--;

triggers:
    /* 3. One spawner per trigger, once per controller life. */
    for (int i = 0; i < 15; i++) {
        const IceTrigger *t = &ice_triggers[i];
        if (o->done[i]) continue;
        if (!(t->x0 < mega.x && mega.x < t->x0 + 96)) continue;
        if (i == 12 && mega.y < 448) return;            /* HAZARD: return, not continue (12-14 are the last rows) */
        if (i == 13 && mega.y < 688) return;
        if (i == 14 && mega.y < 848) return;
        if (i == 15 && mega.y < 976) return;            /* dead: the loop ends at 14 */

        Obj *n = GetMainObject();
        if (n) {
            n->flags = 0x41; n->type = 17; n->var = t->var;
            /* texture, palette and sprite data of the type 17 bank (GetSpriteBank(17)) */
            n->x = t->x;
            if (t->var == 2) {
                PlaySound(5, 9, 0); Rumble(5);
                n->count = 180;
                n->y = t->y + (GetRNG() & 31) - 16;
            } else if (t->var == 3) {
                n->y = t->y;
            } else {                                    /* var 0, 1: avalanche */
                Rumble(5); PlaySound(5, 8, 0);
                n->count = 360;
                if (o->shake < 480) o->shake = 480;
                n->y = t->y + (GetRNG() & 31) - 16;
            }
        }
        o->done[i] = 1;                                 /* also when GetMainObject failed */
    }
}

/* ===================================================================== */
/* Stage logic 7: the ice slide (same code in overlays 04, 07 and 10)    */
/* entry 0x800ED3A4 (overlay 10: 0x800F0BB0, overlay 07: 0x800F0530)    */
/* Only the slope part is new here; the flat part is in                  */
/* docs/stages/sheldon.md, "The glass slide (stage logic 7)".            */
/* ===================================================================== */

void IceSlide(Logic *o)                                 /* 0x800ED3A4 */
{
    o->prev_value = o->value;                           /* +0x21 */
    o->value = FootValue(0);                            /* TileCollisionAt(mega, x, y + box.oy + box.h), +0x20 */
    s32 dx = mega.x - mega.x_start;                     /* mega+0x13C: x at the start of the player update */
    if (mega.state != 1) goto direction;

again:
    switch (o->value) {                                 /* table 0x800E98F4, 61 cases */
    case 17: case 18: case 19: case 20:                 /* slope that rises to the right */
    case 25: case 26:
        o->state = 1;
        if (o->prev_value == 60 && mega.flip != 0 && mega.step != 12) {
            mega.x = mega.x_start;                      /* from ice onto an up-slope, facing uphill, no dash: blocked */
        } else {
            s32 d = max(dx - 3.5, -mega.dash_speed);    /* mega+0x104 */
            mega.x = mega.x_start + d;
            mega.y += abs(d);                           /* pressed down; the player's CollideCheck puts it back on the slope */
        }
        goto reread;
    case 21: case 22: case 23: case 24:                 /* slope that falls to the right */
    case 27: case 28:
        o->state = 1;
        if (o->prev_value == 60 && mega.flip == 0 && mega.step != 12) {
            mega.x = mega.x_start;
        } else {
            s32 d = min(dx + 3.5, mega.dash_speed);
            mega.x = mega.x_start + d;
            mega.y += abs(d);
        }
        goto reread;
    case 16:                                            /* under a slope: use the cell 16 px higher */
        o->value = FootValue(-16);
        goto again;
    case 0:
        if ((mega.b89 & 0x08) == 0) { if (o->state == 2) slide_states[2](o); o->step_seen = mega.step; goto direction; }
        /* on an object: */ slide_states[o->state](o); o->step_seen = mega.step; goto direction;
    case 60:
        slide_states[o->state](o); o->step_seen = mega.step; goto direction;
    default:                                            /* 1-15, 29-59 */
        o->state = 1; o->step_seen = mega.step; goto direction;
    }

reread:                                                 /* 0x800ED6E0 */
    o->value = FootValue(0);
    if ((o->value >= 16 && o->value <= 28) || o->value == 56) goto direction;
    if (o->value != 0 || o->state != 2)
        slide_states[o->state](o);                      /* off the slope onto ice in the same update: arm (state 1) */
    o->step_seen = mega.step;

direction:
    if (mega.x != mega.x_start) o->dir = (mega.x < mega.x_start) ? 0 : 0x40;   /* +0x1E */
}
