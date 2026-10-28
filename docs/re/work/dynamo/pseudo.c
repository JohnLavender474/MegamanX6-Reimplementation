/*
 * Dynamo (boss overlay 45, main type 63; shots visual type 34): pseudo code.
 * A plausible reconstruction from the kit output (kit/kit-main/code.txt), not original code and
 * not compiled. Addresses are those of overlay 45 at 0x800FA000. Fixed point: 16.16 for speeds.
 * Room points are relative to the camera target minimum at the set-up; the set-up writes them
 * back as world values.
 */

#define ROOM_SPAWN_X   (*(s16 *)0x800FCE48)   /* 256 -> world */
#define ROOM_SPAWN_Y   (*(s16 *)0x800FCE4A)   /* -16 -> world */
#define ROOM_LEFT_X    (*(s16 *)0x800FCE4C)   /* 48 */
#define ROOM_RIGHT_X   (*(s16 *)0x800FCE4E)   /* 304 */
#define ROOM_CENTRE_X  (*(s16 *)0x800FCE50)   /* 176 */
#define ROOM_TOP_Y     (*(s16 *)0x800FCE52)   /* 16: shot box top */
#define ROOM_BOTTOM_Y  (*(s16 *)0x800FCE54)   /* 176: shot box bottom */
extern s32 dynamo_dead;      /* 0x800FCE58 */
extern s32 dynamo_stunned;   /* 0x800FCE5C */
extern Object *dynamo_obj;   /* 0x800FCE60 */
extern s32 dynamo_floor_y;   /* 0x800FCE64 */

static const s8 hp_bonus[4]     = { -8, 0, 4, 8 };   /* 0x800FA004, every second byte */
static const u8 throw_count[4]  = { 3, 3, 4, 5 };    /* 0x800FA00C */
static const u8 volley_count[4] = { 6, 7, 8, 8 };    /* 0x800FA010 */
/* patterns 0x800FCD10, 6 bytes each, ended by THINK */
static const u8 patterns[5][6] = {
    { IDLE, BOOMERANG, THINK },
    { IDLE, PILLARS, THINK },
    { IDLE, PILLARS, THINK },
    { IDLE, CENTRE_VOLLEY, LEAPING_SLASH, THINK },
    { IDLE, THINK },                                 /* no THINK result picks it */
};

enum { STEP_HIT = 0, STEP_NONE, INTRO, IDLE, THINK, BOOMERANG, CENTRE_VOLLEY, LEAPING_SLASH,
       PILLARS, STUN };

/* 0x800FB214: entry */
void Dynamo(Object *o)
{
    o->prev_x = o->x; o->prev_y = o->y;
    o->display = 0;
    dynamo_states[o->state](o);          /* 0x800FCD70: init, main, death */
    CollideCheck(o);
}

/* 0x800FA028: state 0 */
void DynamoInit(Object *o)
{
    BossLevel();
    boss_hp_max = (u8)(58 + hp_bonus[game.boss_level]);   /* 50, 58, 62, 66 */
    o->hp = 0;
    o->contact_damage = 4;
    o->damage_table = dmg_tbl_dynamo;                     /* 0x80075A5C */
    o->attack_box = box_body; o->hurt_box = box_body;     /* (-6, -22, 12, 50) */
    o->anime_tbl = dynamo_anims;                          /* 0x800FCCD4, 15 animations */
    o->priority = 5; o->layer = mega.layer;
    /* own texture 0x140, palette 0x7948, sprite data of his bank (GetSpriteBank(63)) */
    s16 cx = camera_layer0.target_min_x, cy = camera_layer0.target_min_y;
    ROOM_SPAWN_X += cx; ROOM_SPAWN_Y += cy; ROOM_LEFT_X += cx; ROOM_RIGHT_X += cx;
    ROOM_CENTRE_X += cx; ROOM_TOP_Y += cy; ROOM_BOTTOM_Y += cy;
    o->x = ROOM_SPAWN_X; o->y = ROOM_SPAWN_Y; o->flip = 0;
    game.boss = o; game.boss_bar_flag25 = 0;
    PlayerEvent(20, 64);
    o->cam48_old = camera_layer0.f48; camera_layer0.f48 = 1;   /* meaning open */
    dynamo_dead = 0; dynamo_stunned = 0; dynamo_obj = o;
    o->state = 1; o->step = INTRO; o->step2 = 0;
}

/* 0x800FA23C: state 1 */
void DynamoMain(Object *o)
{
    o->hurt_box = BoxBySprite(o->sprite, HURT);   /* 45-55: small; else body */
    int r = ContactWeapon(o);
    if (r < 0) {                                  /* killed */
        dynamo_dead = 1; o->state = 2; o->step = 0; o->clut &= ~0x8000; mega.no_hit = 1;
        return;
    }
    o->attack_box = BoxBySprite(o->sprite, ATTACK); /* sprite 52: slash box (-100, -4, 140, 68) */
    ContactMega(o);
    if (r > 0 && r != 127) {
        o->hit_timer = (r == 25 || r == 26) ? 2 : 30;  /* weapon ids 24, 25 */
        o->damage_table = dmg_tbl_deflect;
        if (o->step == STUN)
            o->soul_hit = 1;                      /* +0x84: ends the shake, drops a soul */
        else if (r == 7 || r == 16 || r == 43 || r == 47) {   /* ids 6, 15, 42, 46 */
            o->soul_hit = 0; dynamo_stunned = 1;
            o->step = STUN; o->step2 = 0; o->hit_timer = 20;
        }
    }
    if (o->hit_timer > 0) {
        o->hit_timer--;
        PaletteFlashToggle(o, 2);
        if (o->hit_timer == 0) { o->damage_table = dmg_tbl_dynamo; FlashOff(o); }
    }
    dynamo_steps[o->step](o);                    /* 0x800FCD7C */
}

/* 0x800FB27C: INTRO, sub-steps 0x800FCDA4 */
void DynamoIntro(Object *o)
{
    switch (o->step2) {
    case 0:                                       /* 0x800FB2B8 */
        if (mega.step != 20) break;
        camera_layer0.limit_speed = 2;
        PlayerEvent(29, 64);
        o->timer = 40;
        o->warning = SpawnSysObject(24, o->x, o->y);   /* the WARNING */
        o->step2++;
        break;
    case 1:                                       /* 0x800FB350 */
        if (game.exit_req) break;
        if (o->timer == 40) {
            o->cam26_old = camera_layer0.target_min_x;
            camera_layer0.target_min_x = camera_layer0.target_max_x - 16;
        }
        if (--o->timer == 0) PlayerEvent(20, 64);
        if (o->warning->flags == 0) {
            PlaySound(2, 71, o);
            SetAnime(o, 0);                       /* beam-in */
            o->speed_y = -8 << 16;                /* down */
            o->step2++;
        }
        break;
    case 2:                                       /* 0x800FB414 */
        if (!o->terrain_box && o->y >= 300)       /* world y: a fixed number */
            o->terrain_box = box_terrain;         /* (0, 0, 12, 32) */
        if (o->collide_flags & 8) o->speed_y = 0;
        if (o->anime.next == 0) {                 /* animation 0 ended (108 updates) */
            if (!TextSeen(2)) { TextOpen(2, 255, 0); SetTextSeen(2); }
            SetAnime(o, 1);
            o->timer = 0; o->step2++;
        }
        AnimeTick(o); MoveObject(o); DisplayCheck(o, 64, 64);
        break;
    case 3:                                       /* 0x800FB4E8 */
        if (text_open) break;
        BossMusicRequest(); game.boss_bar = 1; o->step2++;
        break;
    case 4:                                       /* 0x800FB54C */
        if (BossMusicWait()) break;
        camera_layer0.target_min_x = o->cam26_old;
        if (o->hp < boss_hp_max) {
            o->hp++;
            if (++o->timer % 3 == 0) PlaySound(0, 14, o);
            break;
        }
        camera_layer0.f48 = 8;
        PlayerEventEnd();
        dynamo_floor_y = o->y;
        o->step = THINK; o->step2 = 0;
        break;
    }
}

/* 0x800FB74C: THINK */
void DynamoThink(Object *o)
{
    int r = GetRNG() & 15, p;
    if (o->hp > boss_hp_max / 2) p = (r < 10) ? 0 : 1;
    else                         p = (r < 6) ? 2 : 3;
    o->pattern = p; o->pattern_pos = 0;           /* +0x8A, +0x8B */
    DynamoNextStep(o);
}

void DynamoNextStep(Object *o)                    /* inline at the end of each step */
{
    o->step = patterns[o->pattern][o->pattern_pos++];
    o->step2 = 0;
}

/* 0x800FB650: IDLE: animation 1 for 60 updates */

/* 0x800FB824: BOOMERANG, sub-steps 0x800FCDC0 */
void DynamoBoomerang(Object *o)
{
    switch (o->step2) {
    case 0: SetAnime(o, 2); FacePlayer(o); o->throws = 0; o->blades = 0; o->step2++; break;
    case 1:                                       /* 0x800FB8EC */
        if (o->anime.flag == 1) SpawnShot(o, 34, o->throws == 0 ? 0 : 1);
        if (o->anime.next == 0) {
            FacePlayer(o);
            if (++o->throws == throw_count[game.boss_level]) { SetAnime(o, 1); o->step2++; }
            else SetAnime(o, 2);
        }
        break;
    case 2:                                       /* 0x800FB9F8 */
        if (o->blades == 0) DynamoNextStep(o);    /* +0x07: live blades */
        break;
    }
    AnimeTick(o); MoveObject2(o); DisplayCheck(o, 64, 64);
}

/* 0x800FC038: PILLARS, sub-steps 0x800FCDF0 */
void DynamoPillars(Object *o)
{
    switch (o->step2) {
    case 0: {                                     /* 0x800FA5A0 */
        SetAnime(o, 5);
        s16 tx;
        if (mega.x < ROOM_CENTRE_X) { tx = ROOM_RIGHT_X; o->flip = 0; }
        else                         { tx = ROOM_LEFT_X;  o->flip = 0x40; }
        if (abs(tx - o->x) < 32) {
            SetAnime(o, 6); PlaySound(2, 68, o); Rumble(6); o->timer = 80; o->step2 = 2;
        } else {
            PlaySound(2, 53, o);
            o->speed_x = ((tx - o->x) << 16) / 42;
            o->speed_y = 3 << 16; o->gravity = 0x2666;    /* 0.15 */
            o->step2 = 1;
        }
        break;
    }
    case 1:                                       /* 0x800FC09C: landing, then the charge */
        if ((o->collide_flags & 8) && o->was_falling) {
            PlaySound(2, 66, o); o->speed_x = o->speed_y = o->gravity = 0;
        }
        if (o->anime.next == 0) {
            SetAnime(o, 6); PlaySound(2, 68, o); Rumble(6); o->timer = 80; o->step2 = 2;
        }
        break;
    case 2:                                       /* 0x800FA6EC */
        if (o->timer && --o->timer == 0) { SetAnime(o, 7); PlaySound(2, 59, o); }
        if (o->anime.flag == 2) { PlaySound(2, 69, o); Rumble(5); }
        if (o->anime.flag == 1) {
            PlaySound(2, 70, o);
            for (int k = 1; k <= 8; k++) SpawnPillar(o, 36 * k);   /* visual 34 var 4 */
        }
        if (o->anime_id == 7 && o->anime.next == 0) { SetAnime(o, 1); DynamoNextStep(o); }
        break;
    }
    AnimeTick(o); MoveObject2(o); DisplayCheck(o, 64, 64);
}

/* 0x800FBA6C: CENTRE VOLLEY, sub-steps 0x800FCDCC: jump to ROOM_CENTRE_X as in PILLARS;
 * then 0x800FA454: every 50 updates FacePlayer and SpawnShot(o, 34, mega.character ? 3 : 2),
 * volley_count[level] blades; wait for blades == 0; animation 9; at its end DynamoNextStep. */

/* 0x800FBCF8: LEAPING SLASH, driven by the flags of animation 4:
 * flag 1: sound 53, speed_x = (mega.x - o->x) / 40, speed_y = 3 up, gravity 0x1999;
 * flag 2: sounds 56 and 92; sprite 52 gives the slash box through BoxBySprite;
 * flag 3: speed_y = 4 up, gravity 0x2222, speed_x = 1 in the same direction, turn round;
 * landing: sound 66, speeds 0; end of the animation: animation 1, DynamoNextStep (THINK). */

/* 0x800FC13C: STUN, sub-steps 0x800FCDFC */
void DynamoStun(Object *o)
{
    switch (o->step2) {
    case 0:                                       /* 0x800FC1A0 */
        PlaySound(2, 62, o); SetAnime(o, 10);
        o->speed_x = o->speed_y = o->accel_x = o->gravity = 0;
        o->timer = 80; o->step2++;
        break;
    case 1:                                       /* 0x800FA864 */
        ShakeY(o, 2, 4);                          /* y +-2, direction every 4 updates */
        if (o->soul_hit && game.dynamo_souls < 3) {
            game.dynamo_souls++;                  /* game+0x33 */
            Object *s = GetMainObject();
            s->flags = 0x41; s->type = 15; s->var = 0x32;   /* 200-soul Nightmare Soul */
            s->x = o->x; s->y = o->y;
            o->timer = 0;
        }
        if (o->timer == 0 || --o->timer == 0) {
            if (o->hit_timer < 15) o->hit_timer = 30;
            o->damage_table = dmg_tbl_deflect;
            if (o->flip == 0) o->speed_x = 8 << 16;   /* only when facing left: see open question */
            o->gravity = 1 << 16;
            o->step2++;
        }
        break;
    case 2:                                       /* 0x800FC1FC: jump to the other side */
    case 3:                                       /* 0x800FC2F0: landing, dynamo_stunned = 0, THINK */
        break;
    }
}

/* 0x800FC3C0: death, steps 0x800FCE0C:
 * 0x800FC3FC: animation 10, flash off, PlayerEvent(20, player facing), 127 updates of flashing;
 * 0x800FC470: engine object 26 (boss explosion); at its white screen: stand (animation 11),
 *             y = dynamo_floor_y, no terrain box;
 * 0x800FC570 / 0x800FC67C: when object 26 is gone, TextOpen(3, 255, 0);
 * 0x800FC6E4: when the text is closed, sound 64, animation 12 (beam-out);
 * 0x800FC74C: at flag 1 sound 72, speed_y = 8 up (MoveObject); at the end
 *             game.exit_req = 0x10, DeleteObject3. No soul, no part, no progress byte. */

/* 0x800FC7D8: visual type 34 entry (blades var 0-3, pillars var 4) */
void DynamoShot(Object *s)
{
    if (s->soul_hit == 0 && s->collide_flags == 0) CornerCheck(s);
    if (s->var < 4) blade_states[s->state](s);   /* 0x800FCE30 */
    else            pillar_states[s->state](s);  /* 0x800FCE3C */
}

/* 0x800FA984: blade set-up */
void DynamoBladeInit(Object *s)
{
    Object *d = dynamo_obj;
    CopyGraphics(s, d); s->priority = 1;
    SetAnime(s, 13);
    s->attack_box = box_blade_atk;   /* (-16, -15, 32, 22) */
    s->hurt_box = box_blade_hurt;    /* (-16, -7, 32, 14) */
    s->damage_table = dmg_tbl_dynamo_blade;   /* 0x80075AFC */
    s->contact_damage = 6; s->hp = 100;
    s->x = d->x + (d->flip ? 16 : -16); s->y = d->y;
    if (s->var >= 2) s->y -= 8;
    s32 v = 3 * 0x18000;                          /* 4.5 px/update */
    if (s->var == 0) s->speed_x = d->flip ? v : -v;
    else { int a = ratan2(mega.y - s->y, mega.x - s->x); s->speed_x = v * rcos(a) >> 12; s->speed_y = v * rsin(a) >> 12; }
    PlaySound(2, 67, s);
    d->blades++;                                  /* d+0x07 */
    s->state++;
}

/* 0x800FABC0: blade main, switch 0x800FA014 (5 cases): fly out; wait (180 updates for var 0, 1;
 * 120 for var 3); fly back to Dynamo (var 0, 1) or at the player (var 3); end when far outside the
 * shot box or, for var 0/1, 10 updates after it comes within 64 px of Dynamo (he plays animation
 * 3); case 4: knocked away by a hit. HP is set to 100 every update. All end when dynamo_dead or
 * dynamo_stunned. The end state (0x800FC898) does d->blades-- and DeleteObject3. */

/* 0x800FC8CC / 0x800FC9AC / 0x800FCA64: pillar: animation 14, contact 8, 54 px above his origin;
 * after 20 updates attack box (-11, -96, 22, 192); end after 60 updates. */
