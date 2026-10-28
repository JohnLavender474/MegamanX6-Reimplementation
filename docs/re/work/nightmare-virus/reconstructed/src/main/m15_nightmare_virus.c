/*
 *	m15_nightmare_virus.c  --  Nightmare Virus (main object 15), its orb (visual 12),
 *	its after-images (misc 11) and the light rays of the boss soul (ray object 13).
 *
 *	Plausible reconstruction, not original code, not compiled.
 *	Main program, 0x800404D8 - 0x800434BC.
 */
#include "object.h"
#include "game.h"
#include "sound.h"

#define NV_CNT(o)	(*(short *)&(o)->work[0x7e - 0x7e])
#define NV_TGT(o)	(*(OBJECT **)&(o)->work[0x80 - 0x7e])
#define NV_PX(o)	(*(short *)&(o)->work[0x84 - 0x7e])
#define NV_PY(o)	(*(short *)&(o)->work[0x86 - 0x7e])
#define NV_INDEX(o)	(*(u_short *)&(o)->work[0x88 - 0x7e])
#define NV_SOUL(o)	(*(u_short *)&(o)->work[0x8a - 0x7e])
#define NV_8C(o)	((o)->work[0x8c - 0x7e])
#define NV_KEEP(o)	((o)->work[0x8d - 0x7e])
#define NV_SOULBOX(o)	((o)->work[0x8e - 0x7e])
#define NV_PAT(o)	((o)->work[0x8f - 0x7e])

#define NV_KIND(o)	((u_char)(o)->var & 0xf0)
#define NV_ROW(o)	((u_char)(o)->var & 0x0f)

enum { NV_NORMAL = 0x00, NV_BOSS_SOUL = 0x10, NV_SNATCH = 0x20, NV_SOUL_ONLY = 0x30 };
enum { ST_HIT = 0, ST_NONE, ST_IDLE, ST_DASH, ST_SHOOT, ST_SOUL, ST_SNATCH, ST_BOSS_SOUL };

extern ANIME	*nv_anime_tbl[];			/* 0x80075FB0 */
extern HITBOX	nv_atk_box;				/* 0x80075EDC (-12,-12,24,24) */
extern HITBOX	nv_soul_box;				/* 0x80075EE0 (0,0,5,5) */
extern HITBOX	nv_dmg_box;				/* 0x80075EE4 (-10,-10,20,20) */
extern HITBOX	orb_atk_box;				/* 0x800760C4 (-3,-3,6,6) */
extern u_char	dmg_tbl_normal[];			/* 0x800743DC */

static u_char nv_pattern[4][8] = {			/* 0x80075EBC */
	{ ST_IDLE, ST_SHOOT, 8 },
	{ ST_IDLE, ST_DASH, 8 },
	{ ST_IDLE, ST_DASH, ST_SHOOT, ST_DASH, 8 },
	{ ST_IDLE, ST_IDLE, 8 },
};

static void nv_next_step(OBJECT *op);
static void nv_spawn_image(OBJECT *op, u_char var);
static void nv_spawn_orb(OBJECT *op, u_char var);
static void nv_face_player(OBJECT *op);
static int  nv_record_index(OBJECT *op);
static int  nv_soul_not_taken(OBJECT *op);
static void nv_mark_taken(OBJECT *op);
static void nv_find_reploid(OBJECT *op);
static int  nv_reploid_valid(OBJECT *rp);
static void nv_spawn_ray(OBJECT *op, u_char var, short angle);

static void set_tex(OBJECT *op)
{
	int bank = GetSpriteBank(op->type);
	/* tex_cord, sprite_data and clut from the sprite bank of the type (same code as other enemies) */
	(void)bank;
}

static void nv_set_hp(OBJECT *op)
{
	op->atk = 4;
	op->hp = 10;
	if (game.level == LEVEL_NORMAL)
		return;
	if (game.level == LEVEL_EASY)
		op->hp = 2;
	else
		op->hp += 8;
}

/* 0x800404D8 (state 0) */
static void nv_init(OBJECT *op)
{
	nv_set_hp(op);
	op->anime_tbl = nv_anime_tbl;
	op->dmg_box = &nv_dmg_box;
	op->muteki = 0;
	op->disp = 0;
	op->col_box = NULL;
	op->atk_box = &nv_atk_box;
	op->pri = 3;
	op->flip = 0;
	op->fall_f = 0;
	op->dmg_tbl = dmg_tbl_normal;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
	op->layer = mega.layer;
	op->old_x = op->x;
	op->old_y = op->y;
	nv_face_player(op);
	NV_KEEP(op) = 0;
	NV_SOUL(op) = 0;
	NV_PAT(op) = 0;
	NV_SOULBOX(op) = 0;
	op->act = 1;
	op->act3 = 0;

	switch (NV_KIND(op)) {
	case NV_NORMAL:
		NV_INDEX(op) = nv_record_index(op);
		nv_next_step(op);
		break;
	case NV_BOSS_SOUL:
		op->pri = 1;
		set_tex(op);
		op->atk = 0;
		NV_SOULBOX(op) = 1;
		op->atk_box = NULL;
		NV_KEEP(op) = 1;
		if (NV_ROW(op) == 0) {
			SetAnime(op, 8);
			NV_SOUL(op) = 200;
		} else if (NV_ROW(op) == 1) {
			SetAnime(op, 9);
			NV_SOUL(op) = 100;
		} else
			break;
		op->act2 = ST_BOSS_SOUL;
		break;
	case NV_SNATCH:
		NV_INDEX(op) = nv_record_index(op);
		NV_8C(op) = 1;
		op->act2 = ST_SNATCH;
		break;
	case NV_SOUL_ONLY:
		set_tex(op);
		op->atk = 0;
		NV_SOULBOX(op) = 1;
		op->atk_box = NULL;
		op->act2 = ST_SOUL;
		break;
	}
}

static void (*nv_step_tbl[])(OBJECT *);	/* 0x80076018 */

/* 0x80040800 (state 1) */
static void nv_main(OBJECT *op)
{
	int r;
	OBJECT *body;

	op->old_x = op->x;
	op->old_y = op->y;
	AnimeTick(op);
	if (op->act2 != ST_BOSS_SOUL)
		DisplayCheck(op, 48, 48);
	nv_step_tbl[op->act2](op);

	if (op->act2 == ST_SOUL) {
		body = &mega;
		if (((char *)&mega)[0xd5]) {
			body = &alt_body[0];
			if (alt_body[0].act2 == 0)
				body = &alt_body[1];
		}
		op->col_box = NV_SOULBOX(op) ? &nv_soul_box : NULL;
		if (TerrainBoxOverlap(op, body)) {
			game.soul[(int)mega.var] += NV_SOUL(op);
			PlaySound(SE_BANK_SYS, SE_SOUL_GET, op);
			op->col_box = NULL;
			op->act = 2;
			return;
		}
		op->col_box = NULL;
	} else if (op->act2 != ST_BOSS_SOUL) {
		if (op->dmg_box != NULL && (r = ContactWeapon(op)) < 0) {
			if (!nv_soul_not_taken(op)) {
				SpawnExplosion(op);
				op->act = 2;
				return;
			}
			SpawnExplosion(op);
			nv_mark_taken(op);
			op->atk = 0;
			NV_SOULBOX(op) = 1;
			op->atk_box = NULL;
			op->dmg_box = NULL;
			op->act2 = ST_SOUL;
			op->act3 = 0;
			op->clut &= ~CLUT_FLASH;
			return;
		}
		ContactMega(op);
	}
	if (OffScreenCheck2(op, 64, 64) == 1 && NV_KEEP(op) == 0)
		op->act = 3;
}

/* 0x80041BA8, 0x80041BC8 (states 2, 3) */
static void nv_delete(OBJECT *op)  { DeleteObject(op); }
static void nv_delete2(OBJECT *op) { DeleteObject2(op); }

static void (*nv_state_tbl[])(OBJECT *) = { nv_init, nv_main, nv_delete, nv_delete2 };	/* 0x80075FD8 */

/* 0x80041AB8 (main table entry 15) */
void m15_nightmare_virus(OBJECT *op)
{
	nv_state_tbl[(int)op->act](op);
}

/* ---- step 1: empty (0x800429CC) ---- */
static void nv_none(OBJECT *op) { }

/* ---- step 2: IDLE (0x80041F30, table 0x80076038) ---- */
static void idle_start(OBJECT *op)	/* 0x80041F6C */
{
	nv_face_player(op);
	SetAnime(op, 0);
	op->timer = 60;
	op->act3++;
}
static void idle_wait(OBJECT *op)	/* 0x80041FB0 */
{
	if (--op->timer == 0)
		op->act3++;
}
static void idle_anim_end(OBJECT *op)	/* 0x80041FE4 */
{
	if (op->anime.flag == 1)
		op->act3++;
}
static void idle_next(OBJECT *op) { nv_next_step(op); }	/* 0x8004200C */

static void (*idle_tbl[])(OBJECT *) = { idle_start, idle_wait, idle_anim_end, idle_next };
static void nv_idle(OBJECT *op) { idle_tbl[(int)op->act3](op); }

/* ---- step 3: DASH, and the Hard-only warp (0x80041BE8, table 0x80076048) ---- */
static void dash_start(OBJECT *op)	/* 0x80041C24 */
{
	nv_face_player(op);
	SetAnime(op, 1);
	op->act3++;
	NV_CNT(op) = GetRNG() % 7 + 6;
}
static void dash_aim(OBJECT *op)	/* 0x80040A04 */
{
	int a, r;

	op->timer = GetRNG() % 13 + 4;
	a = ratan2(POS_Y(op) - POS_Y(&mega), POS_X(&mega) - POS_X(op));
	op->acc_x = 0;
	op->grav = 0;
	op->spd_x = rcos(a) << 4;
	op->spd_y = rsin(a) << 4;
	op->flip = op->spd_x > 0 ? OBJ_FLIP_R : OBJ_FLIP_L;
	r = GetRNG() % 3;
	if (r == 1) {
		op->spd_x = op->spd_x * 3 / 2;
		op->spd_y = op->spd_y * 3 / 2;
	} else if (r == 2) {
		op->spd_x <<= 1;
		op->spd_y <<= 1;
	}
	op->act3++;
}
static void dash_move(OBJECT *op)	/* 0x80041C9C */
{
	MoveObject(op);
	if (--op->timer != 0)
		return;
	if (--NV_CNT(op) != 0) {
		op->act3 = 1;
		return;
	}
	if (game.level >= LEVEL_HARD && (GetRNG() & 1)) {
		op->act3 = 3;
		return;
	}
	nv_next_step(op);
}
static void warp_hide(OBJECT *op)	/* 0x80041D38 */
{
	op->disp = 0;
	op->dmg_box = NULL;
	op->atk_box = NULL;
	op->timer = 60;
	op->act3++;
}
static void warp_place(OBJECT *op)	/* 0x80041D5C */
{
	short a;

	op->disp = 0;
	if (--op->timer != 0)
		return;
	if (OffScreenCheck2(op, 64, 64) == 1) {
		op->act = 3;
		return;
	}
	nv_face_player(op);
	a = (short)(GetRNG() << 20 >> 16);
	NV_PX(op) = POS_X(&mega);
	NV_PY(op) = POS_Y(&mega);
	POS_X(op) = NV_PX(op) + (rcos(a) >> 6);
	POS_Y(op) = NV_PY(op) + (rsin(a) >> 6);
	op->act4 = 0;
	nv_spawn_image(op, 0);
	nv_spawn_image(op, 1);
	op->act3++;
}
static void warp_wait_images(OBJECT *op)	/* 0x80041E4C */
{
	op->disp = 0;
	if (op->act4 != 0)
		return;
	SetAnime(op, 1);
	op->dmg_box = &nv_dmg_box;
	op->atk_box = &nv_atk_box;
	DisplayCheck(op, 48, 48);
	op->act3++;
}
static void warp_strike(OBJECT *op)	/* 0x80040B84 */
{
	int a, dx, dy;

	a = ratan2(POS_Y(op) - NV_PY(op), NV_PX(op) - POS_X(op));
	op->acc_x = 0;
	op->grav = 0;
	op->spd_x = rcos(a) * 96;
	op->spd_y = rsin(a) * 96;
	op->flip = op->spd_x > 0 ? OBJ_FLIP_R : OBJ_FLIP_L;
	MoveObject(op);
	dx = NV_PX(op) - POS_X(op);
	dy = NV_PY(op) - POS_Y(op);
	if ((dx < 0 ? -dx : dx) < 7 && (dy < 0 ? -dy : dy) < 7) {
		op->timer = 8;
		op->act3++;
	}
}
static void warp_coast(OBJECT *op)	/* 0x80041EBC */
{
	if (--op->timer != 0)
		MoveObject(op);
	else
		op->act3++;
}
static void dash_next(OBJECT *op) { nv_next_step(op); }	/* 0x80041F10 */

static void (*dash_tbl[])(OBJECT *) = {
	dash_start, dash_aim, dash_move, warp_hide, warp_place,
	warp_wait_images, warp_strike, warp_coast, dash_next
};
static void nv_dash(OBJECT *op) { dash_tbl[(int)op->act3](op); }

/* ---- step 4: SHOOT (0x8004202C, table 0x8007606C) ---- */
static short shot_delay(void)
{
	if (game.level == LEVEL_NORMAL) return 60;
	if (game.level == LEVEL_EASY)   return 90;
	return 30;
}
static void shoot_start(OBJECT *op)	/* 0x800420CC */
{
	nv_face_player(op);
	SetAnime(op, 4);
	op->timer = shot_delay();
	NV_CNT(op) = game.level == LEVEL_NORMAL ? 4 : game.level == LEVEL_EASY ? 3 : 5;
	op->act3++;
}
static void shoot_fire(OBJECT *op)	/* 0x80042150 */
{
	if (--op->timer != 0)
		return;
	nv_face_player(op);
	PlaySound(SE_BANK_ENEMY, SE_NV_SHOT, op);
	nv_spawn_orb(op, 0);
	if (--NV_CNT(op) == 0) {
		op->timer = 40;
		op->act3++;
	} else
		op->timer = shot_delay();
}
static void shoot_end(OBJECT *op)	/* 0x80042214 */
{
	if (--op->timer == 0)
		nv_next_step(op);
}

static void (*shoot_tbl[])(OBJECT *) = { shoot_start, shoot_fire, shoot_end };
static void nv_shoot(OBJECT *op)
{
	BGLAYER *cam = &bg_layer[0];

	if (POS_X(op) < cam->x + 16 || cam->x + 304 < POS_X(op) ||
	    POS_Y(op) < cam->y + 16 || cam->y + 224 < POS_Y(op)) {
		op->act2 = ST_IDLE;
		op->act3 = 0;
		return;
	}
	shoot_tbl[(int)op->act3](op);
}

/* ---- step 5: SOUL and revival (0x80042250, table 0x80076078) ---- */
static void soul_start(OBJECT *op)	/* 0x8004228C */
{
	if ((u_char)op->var == 0x31) {
		SetAnime(op, 7);
		NV_SOUL(op) = 4;
	} else if ((u_char)op->var == 0x32) {
		SetAnime(op, 8);
		NV_SOUL(op) = 200;
	} else {
		SetAnime(op, 6);
		NV_SOUL(op) = 8;
	}
	op->spd_y = 0x80000;
	op->acc_x = 0;
	op->spd_x = 0;
	op->grav = 0x8000;
	NV_PY(op) = POS_Y(op);
	op->act3++;
}
static void soul_hop(OBJECT *op)	/* 0x80042320 */
{
	MoveObject2(op);
	if (POS_Y(op) < NV_PY(op))
		return;
	POS_Y(op) = NV_PY(op);
	if ((u_char)op->var == 0x31 || (u_char)op->var == 0x32) {
		op->timer = 180;
		op->act3 = 3;
	} else {
		op->timer = 60;
		op->act3++;
	}
}
static void soul_bob(OBJECT *op)
{
	POS_Y(op) = NV_PY(op) + (rsin((frame_count & 0x7f) << 5) >> 9);
}
static void soul_bob1(OBJECT *op)	/* 0x800423A0 */
{
	soul_bob(op);
	if (--op->timer != 0)
		return;
	SetAnime(op, 7);
	NV_SOUL(op) = 4;
	op->timer = 180;
	op->act3++;
}
static void soul_bob2(OBJECT *op)	/* 0x80042420 */
{
	soul_bob(op);
	if (--op->timer != 0)
		return;
	if (NV_KIND(op) == NV_SOUL_ONLY) {
		op->timer = 30;
		op->act3 = 6;
		return;
	}
	NV_SOULBOX(op) = 0;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->disp = 0;
	nv_face_player(op);
	op->act4 = 0;
	nv_spawn_image(op, 0);
	nv_spawn_image(op, 1);
	op->act3++;
}
static void revive_wait_images(OBJECT *op)	/* 0x800424E0 */
{
	op->disp = 0;
	if (op->act4 != 0)
		return;
	nv_face_player(op);
	SetAnime(op, 0);
	op->timer = 30;
	op->act3++;
}
static void revive_blink(OBJECT *op)	/* 0x80042538 */
{
	op->disp = (op->timer & 1) ? 0 : 1;
	if (--op->timer != 0)
		return;
	NV_PAT(op) = 0;
	op->var = NV_KIND(op) + 2;
	nv_next_step(op);
	nv_set_hp(op);
	op->atk_box = &nv_atk_box;
	op->dmg_box = &nv_dmg_box;
	op->col_box = NULL;
}
static void soul_fade(OBJECT *op)	/* 0x8004260C */
{
	op->disp = (op->timer & 1) ? 0 : 1;
	if (--op->timer == 0) {
		op->disp = 0;
		op->act = 2;
	}
}

static void (*soul_tbl[])(OBJECT *) = {
	soul_start, soul_hop, soul_bob1, soul_bob2, revive_wait_images, revive_blink, soul_fade
};
static void nv_soul(OBJECT *op) { soul_tbl[(int)op->act3](op); }

/* ---- step 6: SNATCH a Reploid (0x80042658, table 0x80076094) ---- */
static void snatch_start(OBJECT *op)	/* 0x80042694 */
{
	nv_face_player(op);
	SetAnime(op, 0);
	op->timer = 60;
	op->act3++;
}
static void snatch_wait(OBJECT *op)	/* 0x800426D8 */
{
	if (--op->timer == 0)
		op->act3++;
}
static void snatch_search(OBJECT *op)	/* 0x8004270C */
{
	if (op->anime.flag != 1)
		return;
	NV_KEEP(op) = 0;
	nv_find_reploid(op);
	if (NV_TGT(op) == NULL) {
		NV_PAT(op) = 0;
		nv_next_step(op);
		return;
	}
	NV_KEEP(op) = 1;
	op->act3++;
}
static int snatch_move(OBJECT *op)
{
	OBJECT *rp;
	int a, dx, dy;

	NV_KEEP(op) = 0;
	if (!nv_reploid_valid(NV_TGT(op))) {
		nv_next_step(op);
		return -1;
	}
	rp = NV_TGT(op);
	NV_KEEP(op) = 1;
	a = ratan2(POS_Y(op) + 28 - POS_Y(rp), POS_X(rp) - POS_X(op));
	op->acc_x = 0;
	op->grav = 0;
	op->spd_x = (rcos(a) << 4) >> 1;
	op->spd_y = (rsin(a) << 4) >> 1;
	op->flip = op->spd_x > 0 ? OBJ_FLIP_R : OBJ_FLIP_L;
	MoveObject(op);
	dx = POS_X(rp) - POS_X(op);
	dy = POS_Y(rp) - (POS_Y(op) + 28);
	if ((dx < 0 ? -dx : dx) >= 2 || (dy < 0 ? -dy : dy) >= 2)
		return 0;
	SetAnime(op, 2);
	nv_mark_taken(op);
	rp->act4 = 2;
	op->dmg_box = NULL;
	op->clut &= ~CLUT_FLASH;
	return 1;
}
static void snatch_approach_first(OBJECT *op)	/* 0x80040CB0 */
{
	int r;

	SetAnime(op, 1);
	r = snatch_move(op);
	if (r == 1)
		op->act3 = 5;
	else if (r == 0)
		op->act3++;
}
static void snatch_approach(OBJECT *op)	/* 0x80040E3C */
{
	if (snatch_move(op) == 1)
		op->act3++;
}
static void snatch_grab(OBJECT *op)	/* 0x80042780 */
{
	if (op->anime.flag != 1)
		return;
	op->act4 = 1;
	PlaySound(SE_BANK_ENEMY, SE_NV_GRAB, op);
	nv_spawn_image(op, 0x10);
	op->timer = 60;
	op->act3++;
}
static void snatch_hold(OBJECT *op)	/* 0x800427E0 */
{
	if (--op->timer != 0)
		return;
	PlaySound(SE_BANK_ENEMY, SE_NV_GRAB_END, op);
	op->timer = 90;
	op->act3++;
}
static void snatch_flash(OBJECT *op)	/* 0x8004283C */
{
	if (op->timer & 1)
		op->clut &= ~CLUT_FLASH;
	else
		op->clut |= CLUT_FLASH;
	if (--op->timer == 0) {
		op->act4 = 0;
		op->disp = 0;
		op->act = 2;
	}
}

static void (*snatch_tbl[])(OBJECT *) = {
	snatch_start, snatch_wait, snatch_search, snatch_approach_first,
	snatch_approach, snatch_grab, snatch_hold, snatch_flash
};
static void nv_snatch(OBJECT *op) { snatch_tbl[(int)op->act3](op); }

/* ---- step 7: BOSS SOUL (0x8004289C, table 0x800760B4) ---- */
static void bsoul_start(OBJECT *op)	/* 0x800428D8 */
{
	op->disp = 1;
	POS_Y(op) = bg_layer[0].y - 32;
	NV_CNT(op) = 90;
	op->timer = 1;
	op->spd_x = 0;
	op->spd_y = -0x10000;
	op->act4 = 0;
	op->act3++;
	POS_X(op) = POS_X(&mega);
}
static short ray_sets[8][3] = {
	{    0, 1536, 2560 }, {  512, 2048, 3072 }, { 1024, 2560, 3584 }, { 1536, 3072,    0 },
	{ 2048, 3584,  512 }, { 2560,    0, 1024 }, { 3072,  512, 1536 }, { 3584, 1024, 2048 },
};
static void bsoul_descend(OBJECT *op)	/* 0x80040FB4 */
{
	int i, k;

	if (op->act4 == 0 && POS_Y(op) >= bg_layer[0].y - 8) {
		PlaySound(SE_BANK_SYS, SE_SOUL_APPEAR, op);
		op->act4++;
	}
	if (NV_CNT(op) != 0 && --NV_CNT(op) == 0)
		op->spd_y = -0x10000;
	if (POS_Y(op) >= POS_Y(&mega) - 48) {
		MoveObject2(op);
		op->act3++;
		return;
	}
	MoveObject2(op);
	if (--op->timer != 0)
		return;
	k = GetRNG() & 7;				/* jump table 0x80011590 */
	for (i = 0; i < 3; i++)
		nv_spawn_ray(op, 0, ray_sets[k][i]);
	op->timer = 40;
}
static void bsoul_land(OBJECT *op)	/* 0x8004292C */
{
	MoveObject2(op);
	if (POS_Y(op) < POS_Y(&mega))
		return;
	game.soul[(int)mega.var] += NV_SOUL(op);
	PlaySound(SE_BANK_SYS, SE_SOUL_GET, op);
	game.message = 16;
	op->disp = 0;
	op->act3++;
}
static void bsoul_done(OBJECT *op) { }	/* 0x800429C4 */

static void (*bsoul_tbl[])(OBJECT *) = { bsoul_start, bsoul_descend, bsoul_land, bsoul_done };
static void nv_boss_soul(OBJECT *op) { bsoul_tbl[(int)op->act3](op); }

static void (*nv_step_tbl[])(OBJECT *) = {
	HitReactionDispatch, nv_none, nv_idle, nv_dash, nv_shoot, nv_soul, nv_snatch, nv_boss_soul
};

/* ---- helpers ---- */

/* 0x800429D4 */
static void nv_next_step(OBJECT *op)
{
	u_char *row;

	op->act3 = 0;
	if (NV_KIND(op) == NV_SNATCH) {
		nv_find_reploid(op);
		if (NV_TGT(op) != NULL) {
			SetAnime(op, 1);
			op->act2 = ST_SNATCH;
			op->act3 = 3;
			return;
		}
	}
	row = nv_pattern[NV_ROW(op)];
	if (row[NV_PAT(op)] == 8)
		NV_PAT(op) = 0;
	op->act2 = row[NV_PAT(op)++];
}

/* 0x80042A9C: misc object 11 (after-image var 0/1, grab overlay var 0x10) */
static void nv_spawn_image(OBJECT *op, u_char var)
{
	OBJECT *mp = GetMiscObject();

	if (mp == NULL)
		return;
	mp->flag = 0x41;
	mp->type = 11;
	mp->var = var;
	mp->flip = op->flip;
	mp->tpage = op->tpage;
	mp->clut = op->clut & ~CLUT_FLASH;
	mp->anime_tbl = op->anime_tbl;
	mp->sprt_p = op->sprt_p;
	mp->layer = op->layer;
	mp->x.l = (long)POS_X(op) << 16;
	mp->y.l = (long)POS_Y(op) << 16;
	mp->atk_box = (HITBOX *)op;			/* parent pointer at +0x50 */
	if ((var & 0xf0) == 0)
		op->act4++;
}

/* 0x80042B68: visual object 12 (orb) */
static void nv_spawn_orb(OBJECT *op, u_char var)
{
	OBJECT *vp = GetVisualObject();

	if (vp == NULL)
		return;
	vp->flag = 0x41;
	vp->type = 12;
	*(OBJECT **)&vp->timer = op;			/* parent pointer at +0x7C */
	vp->var = var;
	vp->flip = op->flip;
	vp->x.l = (long)POS_X(op) << 16;
	vp->y.l = (long)POS_Y(op) << 16;
	vp->clut = op->clut & ~CLUT_FLASH;
	vp->anime_tbl = op->anime_tbl;
	vp->sprt_p = op->sprt_p;
	vp->tpage = op->tpage;
	vp->layer = op->layer;
}

/* 0x80042C18 */
static void nv_face_player(OBJECT *op)
{
	op->flip = POS_X(op) < POS_X(&mega) ? OBJ_FLIP_R : OBJ_FLIP_L;
}

/* 0x80042C48: ordinal of this record among the type-15 records of the area */
static int nv_record_index(OBJECT *op)
{
	ENEMYSET *sp = enemy_set_tbl[(int)game.stage][(int)game.area];
	u_short n = 0;

	for (; sp->kind != 15; sp++) {
		if (sp->type != 15)
			continue;
		if (op->set == sp)
			return n;
		if (++n >= 256)
			return 0;
	}
	return 0;
}

/* 0x80042CE8 */
static int nv_soul_not_taken(OBJECT *op)
{
	u_char *row = game.soul_done[soul_done_row[game.stage * 2 + game.area]];

	return ((row[NV_INDEX(op) >> 3] >> (NV_INDEX(op) & 7)) & 1) ^ 1;
}

/* 0x80042D3C */
static void nv_mark_taken(OBJECT *op)
{
	u_char *row = game.soul_done[soul_done_row[game.stage * 2 + game.area]];

	row[NV_INDEX(op) >> 3] |= 1 << (NV_INDEX(op) & 7);
}

/* 0x80042D98: nearest waiting Reploid (stage object 4) */
static void nv_find_reploid(OBJECT *op)
{
	OBJECT *rp;
	long best = 1000000, d;
	int i, dx, dy;

	NV_TGT(op) = NULL;
	for (i = 0; i < STAGE_OBJECT_MAX; i++) {
		rp = (OBJECT *)stage_object[i];
		if (rp->flag == 0 || rp->type != 4 || rp->disp == 0 || rp->act4 != 1)
			continue;
		dx = POS_X(op) - POS_X(rp);
		dy = POS_Y(op) - POS_Y(rp);
		d = dx * dx + dy * dy;
		if (d < best) {
			best = d;
			NV_TGT(op) = rp;
		}
	}
}

/* 0x80042E68 */
static int nv_reploid_valid(OBJECT *rp)
{
	return rp->flag != 0 && rp->type == 4 && rp->disp != 0 && rp->act4 == 1;
}

/* 0x80042EB4: ray object 13 in the class at 0x8009C9B0 */
static void nv_spawn_ray(OBJECT *op, u_char var, short angle)
{
	u_char *rp = (u_char *)GetRayObject();

	if (rp == NULL)
		return;
	rp[0] = 1;
	rp[1] = 13;
	rp[2] = var;
	*(OBJECT **)(rp + 0x5c) = op;
	*(short *)(rp + 0x46) = angle;
	rp[7] = op->act3;
}

/* ================= orb: visual object 12 (0x80041AF4, table 0x800760C8) ================= */

static void orb_init(OBJECT *op)	/* 0x80041290 */
{
	int a;

	op->atk = 4;
	op->hp = 1;
	op->dmg_tbl = NULL;
	op->pri = 3;
	op->fall_f = 1;
	SetAnime(op, 5);
	op->col_box = NULL;
	op->atk_box = &orb_atk_box;
	op->dmg_box = NULL;
	op->act = 1;
	op->act2 = op->act3 = op->act4 = 0;
	POS_X(op) += op->flip == OBJ_FLIP_L ? -4 : 4;
	POS_Y(op) += 11;
	a = ratan2(POS_Y(op) - POS_Y(&mega), POS_X(&mega) - POS_X(op));
	op->acc_x = 0;
	op->grav = 0;
	op->spd_x = rcos(a) * 80;
	op->spd_y = rsin(a) * 80;
	if (OffScreenCheck2(op, 32, 32))
		op->act++;
	else
		DisplayCheck(op, 16, 16);
}
static void orb_main(OBJECT *op)	/* 0x80042FA8 */
{
	AnimeTick(op);
	MoveObject(op);
	ContactMega(op);
	if (OffScreenCheck2(op, 32, 32))
		op->act++;
	else
		DisplayCheck(op, 16, 16);
}
static void orb_delete(OBJECT *op) { DeleteObject3(op); }	/* 0x80043018 */

static void (*orb_tbl[])(OBJECT *) = { orb_init, orb_main, orb_delete };
void v12_nightmare_orb(OBJECT *op) { orb_tbl[(int)op->act](op); }

/* ================= after-image: misc object 11 (0x80041B30, table 0x800760D4) ================= */

#define IMG_PARENT(o)	((OBJECT *)(o)->atk_box)
#define IMG_DEST(o)	(*(short *)&(o)->dmg_box)

static void img_init(OBJECT *op)	/* 0x80043038 */
{
	op->disp = 0;
	op->pri = 3;
	if ((op->var & 0xf0) == 0) {
		SetAnime(op, 0);
		op->act2 = 0;
	} else if ((op->var & 0xf0) == 0x10) {
		op->pri = 1;
		SetAnime(op, 3);
		op->act2 = 1;
	}
	op->spd_x = op->acc_x = op->spd_y = op->grav = 0;
	op->act = 1;
	op->act3 = 0;
}
static void img_cv_start(OBJECT *op)	/* 0x800431C8 */
{
	IMG_DEST(op) = POS_X(op);
	if ((op->var & 0x0f) == 0) {
		op->spd_x = 0x40000;
		POS_X(op) -= 32;
	} else {
		op->spd_x = -0x40000;
		POS_X(op) += 32;
	}
	op->act3++;
}
static void img_cv_move(OBJECT *op)	/* 0x8004322C */
{
	MoveObject(op);
	if ((op->var & 0x0f) == 0 ? POS_X(op) >= IMG_DEST(op) : POS_X(op) <= IMG_DEST(op)) {
		POS_X(op) = IMG_DEST(op);
		op->act3++;
	}
}
static void img_cv_done(OBJECT *op)	/* 0x800432C8 */
{
	IMG_PARENT(op)->act4--;
	op->disp = 0;
	op->act++;
}
static void (*img_cv_tbl[])(OBJECT *) = { img_cv_start, img_cv_move, img_cv_done };
static void img_converge(OBJECT *op)	/* 0x80043174 */
{
	op->disp = (frame_count + op->var) & 1;
	img_cv_tbl[(int)op->act3](op);
}
static void img_follow(OBJECT *op)	/* 0x800432F4 */
{
	OBJECT *pp = IMG_PARENT(op);

	if (pp->act4 == 0) {
		op->disp = 0;
		op->act++;
		return;
	}
	op->x.w.hi = POS_X(pp);
	op->y.w.hi = POS_Y(pp);
	op->disp = pp->disp;
	op->clut = pp->clut;
}
static void (*img_sub_tbl[])(OBJECT *) = { img_converge, img_follow };	/* 0x800760E0 */
static void img_main(OBJECT *op)	/* 0x800430D0 */
{
	if (IMG_PARENT(op)->act != 1) {
		op->disp = 0;
		op->act++;
		return;
	}
	DisplayCheck(op, 16, 16);
	img_sub_tbl[(int)op->act2](op);
	AnimeTick(op);
}
static void img_delete(OBJECT *op) { DeleteObject3(op); }	/* 0x80043154 */

static void (*img_tbl[])(OBJECT *) = { img_init, img_main, img_delete };
void x11_nightmare_image(OBJECT *op) { img_tbl[(int)op->act](op); }

/*
 * ================= ray: object 13 of the class at 0x8009C9B0 (0x80041B6C, table 0x800760F4) =========
 * ray_init 0x80043350, ray_main 0x80043460 -> { ray_sweep 0x800413D0, ray_shrink 0x80041770 },
 * ray_delete 0x8004349C. The two drawing routines build a 4-point polygon from the parent's position
 * along the angle at +0x46 (16 px steps, up to 50, until the camera rectangle
 * [cam.x-16, cam.x+336] x [cam.y-48, cam.y+256] is left); +0x46 -= +0x4A each update until it is
 * at or below +0x48 or the parent's act3 changes, then the ray shrinks and is deleted.
 * Only outlined here; see ../../pseudocode.md.
 */
