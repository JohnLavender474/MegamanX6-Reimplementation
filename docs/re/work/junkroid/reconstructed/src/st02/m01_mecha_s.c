/*
 *	m01_mecha_s.c
 *
 *	ST02 (opening stage)  main object 01 : broken mechaniloid (sitting, shoots)
 *	              visual object 00 : its shot
 */
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>

#include "object.h"
#include "game.h"
#include "sound.h"

/* ---------------------------------------------------------------- */

enum {
	ACT_INIT = 0,
	ACT_MAIN,
	ACT_DEAD,
	ACT_OUT
};

enum {
	MODE_REACT = 0,
	MODE_WAIT,
	MODE_SHOT,
	MODE_FALL
};

enum {
	ANM_STAND = 0,
	ANM_FIRE_POSE,
	ANM_JUNK0,
	ANM_JUNK1,
	ANM_JUNK2,
	ANM_JUNK3,
	ANM_TAMA,
	ANM_TAMA_H
};

#define MECHA_HP		12
#define MECHA_ATK		4
#define MECHA_PRI		6

#define SEARCH_X		112
#define SHOT_WAIT		120

#define MECHA_GRAV		0x4200

#define DROP_NORMAL		21
#define DROP_BONUS		14

#define TAMA_SPD		0x20000
#define TAMA_OFS_X		16
#define TAMA_OFS_Y		12
#define TAMA_HP			1
#define TAMA_ATK		1
#define TAMA_PRI		4

#define OUT_RANGE		64
#define DISP_RANGE		32
#define TAMA_OUT		32
#define TAMA_DISP		16

/* ---------------------------------------------------------------- */

extern void	ReactMode(OBJECT *op);

static void	Mecha_Init(OBJECT *op);
static void	Mecha_Main(OBJECT *op);
static void	Mecha_Delete(OBJECT *op);
static void	Mecha_Wait(OBJECT *op);
static void	Mecha_Shot(OBJECT *op);
static void	Mecha_Fall(OBJECT *op);
static void	Mecha_FallCheck(OBJECT *op);

static void	Tama_Init(OBJECT *op);
static void	Tama_Main(OBJECT *op);
static void	Tama_Delete(OBJECT *op);

/* ---------------------------------------------------------------- */

static HITBOX	mecha_col = {   0,  16, 16,  8 };
static HITBOX	mecha_atk = { -12, -20, 24, 40 };
static HITBOX	mecha_dmg = { -12, -20, 24, 40 };

static ANIME	anm_stand[] = {
	{  1, 0,  0,  0 },
};
static ANIME	anm_fire[] = {
	{  6, 0,        1, 1 },
	{  4, 0,        1, 2 },
	{  1, ANM_FIRE, 1, 3 },
	{ 15, 0,        1, 3 },
	{  6, 0,        1, 2 },
	{  6, ANM_END,  0, 1 },
};
static ANIME	anm_junk0[] = { { 1, 0, 0,  8 } };
static ANIME	anm_junk1[] = { { 1, 0, 0,  9 } };
static ANIME	anm_junk2[] = { { 1, 0, 0, 10 } };
static ANIME	anm_junk3[] = { { 1, 0, 0,  7 } };
static ANIME	anm_tama[]  = { { 1, 0, 0,  4 } };
static ANIME	anm_tama_h[] = {
	{ 2, 0,  1, 5 },
	{ 2, 0,  1, 6 },
	{ 2, 0, -2, 7 },
};

static ANIME	*mecha_anime[] = {
	anm_stand,
	anm_fire,
	anm_junk0,
	anm_junk1,
	anm_junk2,
	anm_junk3,
	anm_tama,
	anm_tama_h,
};

static u_char	mecha_junk[] = { ANM_JUNK0, ANM_JUNK1, ANM_JUNK2, ANM_JUNK3 };

static void	(*mecha_act[])(OBJECT *) = {
	Mecha_Init,
	Mecha_Main,
	Mecha_Delete,
	Mecha_Delete,
};

static void	(*mecha_mode[])(OBJECT *) = {
	ReactMode,
	Mecha_Wait,
	Mecha_Shot,
	Mecha_Fall,
};

static HITBOX	tama_box = { -4, -4, 8, 8 };
static HITBOX	tama_col = {  0,  0, 6, 6 };

static void	(*tama_act[])(OBJECT *) = {
	Tama_Init,
	Tama_Main,
	Tama_Delete,
};

/* ---------------------------------------------------------------- */

static void
Mecha_TurnMega(OBJECT *op)
{
	if (mega.x.l < op->x.l)
		op->flip = OBJ_FLIP_L;
	else
		op->flip = OBJ_FLIP_R;
}

static int
Mecha_InRange(OBJECT *op, int range)
{
	int	dx;

	dx = POS_X(op) - POS_X(&mega);
	if (dx < 0)
		dx = -dx;
	return (dx < range);
}

/* ---------------------------------------------------------------- */

void
M01_Mecha(OBJECT *op)
{
	mecha_act[op->act](op);
	CollideCheck(op);
}

static void
Mecha_Init(OBJECT *op)
{
	op->hp      = MECHA_HP;
	op->dmg_tbl = dmg_tbl_mecha_s;
	op->atk     = MECHA_ATK;
	op->muteki  = 0;
	op->layer   = mega.layer;
	op->old_x   = op->x;
	op->old_y   = op->y;
	op->spd_x   = 0;
	op->spd_y   = 0;
	op->acc_x   = 0;
	op->grav    = 0;
	op->fall_f  = 0;
	op->col_f   = 0;

	Mecha_TurnMega(op);

	op->anime_tbl = mecha_anime;
	op->pri       = MECHA_PRI;
	op->col_box   = &mecha_col;
	op->dmg_box   = &mecha_dmg;
	op->atk_box   = &mecha_atk;
	SetAnime(op, ANM_STAND);

	op->act2  = MODE_WAIT;
	op->timer = 1;
	op->act   = ACT_MAIN;
	op->act3  = 0;

	/* HARD : all of them wake up */
	if (HARD_MODE())
		op->var = 1;
}

static void
Mecha_Main(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;

	mecha_mode[op->act2](op);

	if (op->var && op->anime.sprt == 0)
		Mecha_TurnMega(op);

	Mecha_FallCheck(op);

	if (ContactWeapon(op) < 0) {
		DropItem(op, DROP_NORMAL);
		SpawnExplosion(op);
		SpawnJunk(4, mecha_junk, op);
		DropItem(op, DROP_BONUS);
		op->act = ACT_DEAD;
		return;
	}

	/* crushed */
	if ((op->col_f & (COL_U | COL_D)) == (COL_U | COL_D) ||
	    (op->col_f & (COL_L | COL_R)) == (COL_L | COL_R)) {
		DropItem(op, DROP_NORMAL);
		SpawnExplosion(op);
		SpawnJunk(4, mecha_junk, op);
		op->act = ACT_DEAD;
		return;
	}

	ContactMega(op);

	if (OffScreenCheck2(op, OUT_RANGE, OUT_RANGE))
		op->act = ACT_OUT;
	else
		DisplayCheck(op, DISP_RANGE, DISP_RANGE);
}

static void
Mecha_Delete(OBJECT *op)
{
	if (op->act == ACT_DEAD)
		DeleteObject(op);
	else
		DeleteObject2(op);
}

/* ---------------------------------------------------------------- */

static void
Mecha_Wait(OBJECT *op)
{
	if (op->var == 0)
		return;

	if (Mecha_InRange(op, SEARCH_X))
		op->act2 = MODE_SHOT;
}

static void
Mecha_Shot(OBJECT *op)
{
	OBJECT	*tp;

	if (!Mecha_InRange(op, SEARCH_X + 1)) {
		op->timer = SHOT_WAIT;
		op->act2  = MODE_WAIT;
		SetAnime(op, ANM_STAND);
		return;
	}

	if (op->timer) {
		if (--op->timer == 0)
			SetAnime(op, ANM_FIRE_POSE);
	}

	if (op->anime.flag == ANM_FIRE) {
		if (HARD_MODE())
			PlaySound(SE_BANK_ENEMY, SE_E_SHOT_H, op);
		else
			PlaySound(SE_BANK_ENEMY, SE_E_SHOT, op);

		if ((tp = GetVisualObject()) != NULL) {
			tp->flag      = 0x41;
			tp->type      = 0;
			tp->var       = 0;
			tp->tpage     = op->tpage;
			tp->clut      = op->clut & ~CLUT_FLASH;
			tp->anime_tbl = op->anime_tbl;
			tp->sprt_p    = op->sprt_p;
			tp->flip      = op->flip;
			tp->x         = op->x;
			tp->act       = 0;
			tp->y         = op->y;
			tp->flip      = op->flip;
		}
	}

	if (op->anime.flag == ANM_END) {
		SetAnime(op, ANM_STAND);
		op->timer = SHOT_WAIT;
		op->act2  = MODE_WAIT;
		op->act3  = 0;
	}

	AnimeTick(op);
}

static void
Mecha_Fall(OBJECT *op)
{
	if (op->col_f & COL_D) {
		op->act2   = MODE_WAIT;
		op->act3   = 0;
		op->spd_x  = 0;
		op->acc_x  = 0;
		op->spd_y  = 0;
		op->grav   = 0;
		op->fall_f = 0;
	} else {
		MoveObject2(op);
	}
}

static void
Mecha_FallCheck(OBJECT *op)
{
	if (op->col_f & COL_D)
		return;
	if (op->fall_f)
		return;

	op->act2   = MODE_FALL;
	op->grav   = MECHA_GRAV;
	op->spd_y  = 0;
	op->acc_x  = 0;
	op->fall_f = 1;
}

/* ---------------------------------------------------------------- */
/*	shot  (visual object)                                           */
/* ---------------------------------------------------------------- */

void
V00_MechaTama(OBJECT *op)
{
	tama_act[op->act](op);
}

static void
Tama_Init(OBJECT *op)
{
	op->disp = 1;

	if (op->flip) {
		op->spd_x = TAMA_SPD;
		op->acc_x = 0;
		op->spd_y = 0;
		op->grav  = 0;
		POS_X(op) += TAMA_OFS_X;
	} else {
		op->spd_x = -TAMA_SPD;
		op->acc_x = 0;
		op->spd_y = 0;
		op->grav  = 0;
		POS_X(op) -= TAMA_OFS_X;
	}

	op->pri     = TAMA_PRI;
	op->col_box = &tama_col;
	op->dmg_box = &tama_box;
	op->atk_box = &tama_box;
	op->hp      = TAMA_HP;
	op->atk     = TAMA_ATK;
	op->muteki  = 0;
	POS_Y(op)  -= TAMA_OFS_Y;
	op->clut   &= ~CLUT_FLASH;

	if (HARD_MODE()) {
		/* HARD : can not be shot down */
		op->dmg_tbl = dmg_tbl_guard;
		SetAnime(op, ANM_TAMA_H);
	} else {
		op->dmg_tbl = dmg_tbl_shot;
		SetAnime(op, ANM_TAMA);
	}
	op->act++;
}

static void
Tama_Main(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);

	if (ContactMega(op) || ContactWeapon(op) < 0 || op->col_f)
		goto bomb;

	CornerCheck(op);
	if (op->col_f)
		goto bomb;

	if (OffScreenCheck2(op, TAMA_OUT, TAMA_OUT)) {
		op->act = 2;
		return;
	}
	DisplayCheck(op, TAMA_DISP, TAMA_DISP);
	return;

bomb:
	SpawnExplosion(op);
	op->act = 2;
}

static void
Tama_Delete(OBJECT *op)
{
	DeleteObject3(op);
}
