/* Ground Scaravich's stage (stage 5, overlay 08): section routing of the teleporter totem poles.
 * Pseudo code from the disassembly of scaravich-1_start (overlay 08 at 0x800E9860).
 * Not compilable. game = 0x800CCED0. */

/* ---- main program ---------------------------------------------------- */

u16 rng_state;                       /* 0x80090E70; = 3383 once at boot (0x80012838) */

u8 GetRNG(void)                      /* 0x8002D2D4 */
{
    u16 s  = rng_state;
    u16 hi = ((3 * s) & 0xFFFF) >> 8;
    u8  lo = (s + hi) & 0xFF;
    rng_state = lo | (hi << 8);
    return lo;                       /* period 43534 from 0x88FA */
}

u8 GetRNG1(void)                     /* 0x8002D30C: the same step; returns 1 instead of 0 */
{
    u8 v = GetRNG();
    return v ? v : 1;
}

/* Demo / replay recorder (likely): 0x80021694 saves frame_count, rng_state and 0x460 bytes of game
 * to 0x801F6000; 0x8002170C stores mega+0x7C (buttons) per frame, max 2500 frames, at 0x801F68DC.
 * 0x80021400.. restores frame_count, rng_state and game from that buffer. */

/* game+0x26 .. game+0x35 are cleared only by game mode 12 (0x8001E81C, the move to stage 15). */

/* ---- stage object 3: the totem pole (overlay 08, entry 0x800EC5AC) ---- */
/* var 0      : the pole base (made by main 12, not by a record); HP 5, solid; makes 5 heads
 * var 1..4   : the teleporter poles (records 18..21; blue); 32 x 160 trigger box
 * var 10..14 : the five heads of the pole (children of var 0) */

void TotemSetup(Obj *o)              /* 0x800EA820 */
{
    if (o->var != 0 && game[0x2F] >= o->var) {   /* this pole was already cleared */
        o->display = 0; o->state = 3;            /* delete */
        return;
    }
    /* ... sprite type 130, HP 16, contact damage 4, boxes ... */
    switch (o->var) {
    case 0:  /* base: terrain box (0,0,14,80); 5 children var 14-i at y + {64,32,0,-32,-64}; HP = 5 */
    case 1: case 4:  /* teleporter, flip 0     */
    case 2: case 3:  /* teleporter, flip 0x40  */
        o->attack_box = {-16, -80, 32, 160}; o->hurt_box = 0; o->step2 = 2;
    case 10..14:     /* heads */
    }
}

/* teleport, step2 2, sub-steps in act4 (+0x07), table 0x800F5854 */
void TotemTouch(Obj *o)              /* act4 0, 0x800EB298 */
{
    if (!BoxOverlap(o, mega)) return;            /* fails while mega+0x61 != 0 */
    /* sound 1/1, camera follow off, stop flags = 2, player terrain box off, mega.step = 2,
       clear charge and shots; start the split at the player's x */
    o->act4 = 1;
}

void TotemFadeOut(Obj *o)            /* act4 1, 0x800EB398 */
{
    split_images(o);
    if (++o->w80 < 29) return;

    if (o->var == 1 && game[0x31] == 0) {        /* first and second pole share one roll */
        game[0x31] = 1;
        game[0x30] = GetRNG() % 12;
    } else if (o->var == 3 && game[0x31] == 1) { /* third and fourth pole share a new roll */
        game[0x31] = 2;
        game[0x30] = GetRNG() % 12;
    }
    FadeOut(8);
    o->act4++;

    int section = route_tbl[game[0x30] * 4 + o->var - 1];   /* 0x800F5644, 12 x 4 bytes */
    switch (section) {                                        /* jump table 0x800E98CC */
    case 0: /* 7 places */ case 1: /* 10 */ case 2: /* 6 */ case 3: /* 3 */
    case 4: /* 6 */        case 5: /* 8 */  case 6: /* 11 */ case 7: /* 7 */
        for (each place p of the section)
            if ((GetRNG() & 1) == 0)
                CopyTiles(0, p.x, p.y, p.src);  /* a block of the map store, copied into layer 0 */
    }
}

void TotemArrive(Obj *o)             /* act4 2, 0x800EBEA0, when the fade-out ends */
{
    int section = route_tbl[game[0x30] * 4 + o->var - 1];
    SectionEntry *e = &section_tbl[section];                  /* 0x800F5754, 32 bytes each */
    mega.x = e->player_x; mega.y = e->player_y - (mega.char != 0);   /* Zero: y - 1 */
    /* three camera layers from e, layer-0 limits (current and target) from e+0x10 */
    /* clear no_hit, latch, invincibility, input lock; redraw; ClearPlayerShots */
    SpawnInRect(cam.x - 48, cam.x + 368, cam.y - 48, cam.y + 288, 0);
    FadeIn(8); PlaySound(1, 1, o);
    o->act4++;
}
/* act4 3 (0x800ECD3C): images converge until the fade-in ends.
 * act4 4 (0x800EC2F0): 29 frames, then mega.x = e->player_x, terrain box back, ToStand, stop
 * flags 0, camera follow on. act4 5 (0x800ECDA4): DeleteObject3 (the record stays "spawned"). */

/* fight: base var 0, step2 0 (0x800EAEF8) */
void TotemBaseDeath(Obj *o)
{
    if (o->hp > 0) return;           /* each head death does parent->hp -= 1 (0x800EAD30) */
    /* 120 frames: effects and debris */
    DropItem(o, ...); o->state = 3;
    if (game.stage == 5 && o->y in {336, 576, 1056, 1296})
        game[0x2F]++;                /* poles cleared */
}

/* ---- main 12: the section goal (overlay 08, entry 0x800ED5C4) --------- */
/* 8 records, id 0..7, one at the end of each section; id is not read.
 * HP 32, contact damage 4, takes weapon hits (dmg_tbl_normal). */
void GoalMain(Obj *o)                /* 0x800ED704 */
{
    step_tbl[o->step](o);            /* 0x800F5900 */
    if (ContactWeapon(o) < 0) { o->attack_box = 0; o->w7C = 60; o->step = 2; }  /* killed */
    ContactMega(o); ...
}
/* step 2 (0x800ECF44): 60 frames of effects and debris, then step 3.
 * step 3: the same teleport as the totem; destination = return_tbl[game[0x2F]] (0x800F5910,
 * 4 x 32 bytes); at the end (0x800ED950): stage object 3, var 0 (the pole base) at
 * pole_place[game[0x2F]] (0x800F5990: (1136,336) (784,576) (784,1056) (1200,1296)); DeleteObject3. */

/* ---- tables ------------------------------------------------------------ */
u8 route_tbl[12][4] = {              /* 0x800F5644: section for pole var 1, 2, 3, 4 */
    {0,1,4,5}, {1,0,5,4}, {2,0,6,4}, {3,0,7,4},
    {0,2,4,6}, {1,2,5,6}, {2,1,6,5}, {3,1,7,5},
    {0,3,4,7}, {1,3,5,7}, {2,3,6,7}, {3,2,7,6},
};
