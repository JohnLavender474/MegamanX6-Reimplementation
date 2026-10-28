/*
 *	m00_dridler.c
 *
 *	ST02 (opening stage)  main object 00 : drill mechaniloid (drops from the ceiling)
 *	              misc   object 00 : its effects (0 : jet / spark, 1 : warning)
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
	ACT_OUT
};

enum {
	MODE_REACT = 0,
	MODE_NOP,
	MODE_WAIT,
	MODE_DROP,
	MODE_DRILL,
	MODE_RISE
};

enum {
	ANM_WAIT = 0,
	ANM_FX_UP,
	ANM_FX_WARN_D,
	ANM_DRILL,
	ANM_FX_DOWN,
	ANM_FX_WARN_U,
	ANM_RISE,
	ANM_JUNK0,
	ANM_JUNK1,
	ANM_JUNK2
};

enum {
	MOVE_STOP = 0,
	MOVE_UP,
	MOVE_DOWN
};

enum {
	WARN_NONE = 0,
	WARN_UP,
	WARN_DOWN
};

#define DRL_HP			1
#define DRL_ATK			8
#define DRL_PRI			33
#define FX_PRI			32
#define FX_PRI_UP		34

#define DROP_SPD		0x1000
#define DROP_GRAV		0x2000
#define RISE_SPD		0x8000
#define DROP_MAX		128

#define WAIT_TIME		90
#define WAIT_TIME_H		45
#define WARN_TIME		30
#define WARN_TIME_H		15
#define DRILL_TIME		60
#define RISE_SE_TIME		60

#define ROCK_OFS_Y		32
#define ROCK_Y		880

#define OUT_X			80
#define OUT_Y			160
#define DISP_X			48
#define DISP_Y			64

#define HOME_Y(o)		(*(short *)&(o)->work[0x02])
#define MOVE_W(o)		((o)->work[0x04])
#define WARN_W(o)		((o)->work[0x05])

#define FX_PARENT(o)		((OBJECT *)(o)->atk_box)
#define FX_LAST(o)		(*(u_char *)&(o)->dmg_box)
#define FX_OFS_Y(o)		(*((short *)&(o)->dmg_box + 1))

typedef struct {
	short	x;
	short	y;
	u_short	w;
	u_short	h;
} BLOCKCOPY;

/* ---------------------------------------------------------------- */

extern void	ReactMode(OBJECT *op);

static void	Drl_Init(OBJECT *op);
static void	Drl_Main(OBJECT *op);
static void	Drl_Delete(OBJECT *op);
static void	Drl_Nop(OBJECT *op);
static void	Drl_Wait(OBJECT *op);
static void	Drl_Drop(OBJECT *op);
static void	Drl_Drill(OBJECT *op);
static void	Drl_Rise(OBJECT *op);

static void	Wait_Start(OBJECT *op);
static void	Wait_Count(OBJECT *op);
static void	Drop_Start(OBJECT *op);
static void	Drop_Move(OBJECT *op);
static void	Drill_Start(OBJECT *op);
static void	Drill_Count(OBJECT *op);
static void	Rock_Start(OBJECT *op);
static void	Rock_Loop(OBJECT *op);
static void	Rise_Start(OBJECT *op);
static void	Rise_Move(OBJECT *op);

static void	Fx_Init(OBJECT *op);
static void	Fx_Main(OBJECT *op);
static void	Fx_Delete(OBJECT *op);
static void	Fx_Move(OBJECT *op);
static void	Fx_Warn(OBJECT *op);

/* ---------------------------------------------------------------- */

static HITBOX	drl_col = {   0,   6, 11, 32 };
static HITBOX	drl_atk = { -14, -40, 28, 80 };
static HITBOX	drl_dmg = { -16, -40, 32, 80 };

static ANIME	anm_wait[] = {
	{ 2, 0,        1,  0 },
	{ 2, ANM_FIRE, -1, 23 },
};
static ANIME	anm_fx_up[] = {
	{ 1, 0,  1,  4 },
	{ 2, 0,  1,  5 },
	{ 1, 0,  1,  6 },
	{ 1, 0,  1,  7 },
	{ 1, 0,  1,  8 },
	{ 2, 0, -5, 14 },
};
static ANIME	anm_fx_warn_d[] = {
	{ 4, 0,  1, 19 },
	{ 3, 0,  1, 20 },
	{ 2, 0,  1, 21 },
	{ 1, 0,  1, 22 },
	{ 2, 0,  1, 21 },
	{ 3, 0, -5, 20 },
};
static ANIME	anm_drill[] = {
	{ 2, 0,  1, 0 },
	{ 2, 0,  1, 1 },
	{ 2, 0,  1, 2 },
	{ 2, 0, -3, 3 },
};
static ANIME	anm_fx_down[] = {
	{ 1, 0,  1,  9 },
	{ 2, 0,  1, 10 },
	{ 1, 0,  1, 11 },
	{ 1, 0,  1, 12 },
	{ 1, 0,  1, 13 },
	{ 2, 0, -5, 14 },
};
static ANIME	anm_fx_warn_u[] = {
	{ 4, 0,  1, 15 },
	{ 3, 0,  1, 16 },
	{ 2, 0,  1, 17 },
	{ 1, 0,  1, 18 },
	{ 2, 0,  1, 17 },
	{ 3, 0, -5, 16 },
};
static ANIME	anm_rise[] = {
	{ 2, 0,  1,  0 },
	{ 2, 0, -1, 23 },
};
static ANIME	anm_junk0[] = { { 1, 0, 0, 24 } };
static ANIME	anm_junk1[] = { { 1, 0, 0, 25 } };
static ANIME	anm_junk2[] = { { 1, 0, 0, 26 } };

static ANIME	*drl_anime[] = {
	anm_wait,
	anm_fx_up,
	anm_fx_warn_d,
	anm_drill,
	anm_fx_down,
	anm_fx_warn_u,
	anm_rise,
	anm_junk0,
	anm_junk1,
	anm_junk2,
};

static u_char	drl_junk[] = {
	ANM_JUNK0, ANM_JUNK0, ANM_JUNK0, ANM_JUNK1, ANM_JUNK1, ANM_JUNK2
};

static void	(*drl_act[])(OBJECT *) = {
	Drl_Init,
	Drl_Main,
	Drl_Delete,
};

static void	(*drl_mode[])(OBJECT *) = {
	ReactMode,
	Drl_Nop,
	Drl_Wait,
	Drl_Drop,
	Drl_Drill,
	Drl_Rise,
};

static void	(*wait_tbl[])(OBJECT *)  = { Wait_Start, Wait_Count };
static void	(*drop_tbl[])(OBJECT *)  = { Drop_Start, Drop_Move };
static void	(*drill_tbl[])(OBJECT *) = { Drill_Start, Drill_Count, Rock_Start, Rock_Loop };
static void	(*rise_tbl[])(OBJECT *)  = { Rise_Start, Rise_Move };

/* rock block : template area one screen below */
static BLOCKCOPY	rock_blk1 = { 0x1220, 0x470, 4, 3 };
static BLOCKCOPY	rock_blk2 = { 0x1300, 0x470, 4, 3 };
static BLOCKCOPY	rock_blk3 = { 0x1340, 0x470, 4, 3 };

static void	(*fx_act[])(OBJECT *) = {
	Fx_Init,
	Fx_Main,
	Fx_Delete,
};

static void	(*fx_mode[])(OBJECT *) = {
	Fx_Move,
	Fx_Warn,
};

/* ---------------------------------------------------------------- */

static void
Drl_SetFx(OBJECT *op, u_char no)
{
	OBJECT	*tp;

	if ((tp = GetMiscObject()) == NULL)
		return;

	tp->flag      = 0x41;
	tp->type      = 0;
	tp->var       = no;
	tp->flip      = op->flip;
	tp->tpage     = op->tpage;
	tp->clut      = op->clut & ~CLUT_FLASH;
	tp->anime_tbl = op->anime_tbl;
	tp->sprt_p    = op->sprt_p;
	tp->layer     = op->layer;
	tp->x.w.lo    = 0;
	tp->x.w.hi    = POS_X(op);
	tp->y.w.lo    = 0;
	tp->atk_box   = (HITBOX *)op;
	tp->y.w.hi    = POS_Y(op);
}

static void
Drl_Explode(OBJECT *op, int n)
{
	POS_Y(op) += ROCK_OFS_Y;
	while (n--)
		SpawnExplosionRand(op, 1, 64, 32);
}

static int
Drl_CameraStill(void)
{
	return (bg_layer[0].dest_y == bg_layer[0].y);
}

/* ---------------------------------------------------------------- */

void
M00_Dridler(OBJECT *op)
{
	drl_act[op->act](op);
	CollideCheck(op);
}

static void
Drl_Init(OBJECT *op)
{
	op->atk       = DRL_ATK;
	op->timer     = 0;
	op->hp        = DRL_HP;
	op->muteki    = 0;
	op->layer     = mega.layer;
	op->old_x     = op->x;
	op->old_y     = op->y;
	HOME_Y(op)    = POS_Y(op);
	MOVE_W(op)    = MOVE_UP;
	WARN_W(op)    = WARN_NONE;
	op->anime_tbl = drl_anime;
	op->pri       = DRL_PRI;
	op->col_box   = &drl_col;
	op->atk_box   = &drl_atk;
	op->flip      = OBJ_FLIP_L;
	op->dmg_box   = &drl_dmg;
	op->fall_f    = 1;
	op->dmg_tbl   = dmg_tbl_guard;
	SetAnime(op, ANM_WAIT);

	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav  = 0;
	op->act   = ACT_MAIN;
	op->act2  = MODE_WAIT;
	op->act3  = 0;

	Drl_SetFx(op, 0);
	Drl_SetFx(op, 1);
}

static void
Drl_Main(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;

	AnimeTick(op);
	drl_mode[op->act2](op);

	ContactWeapon(op);
	ContactMega(op);

	if (OffScreenCheck2(op, OUT_X, OUT_Y))
		op->act = ACT_OUT;
	else
		DisplayCheck(op, DISP_X, DISP_Y);
}

static void
Drl_Delete(OBJECT *op)
{
	DeleteObject2(op);
}

static void
Drl_Nop(OBJECT *op)
{
}

/* ---------------------------------------------------------------- */

static void
Drl_Wait(OBJECT *op)
{
	wait_tbl[op->act3](op);
}

static void
Wait_Start(OBJECT *op)
{
	SetAnime(op, ANM_WAIT);
	op->timer = HARD_MODE() ? WAIT_TIME_H : WAIT_TIME;
	op->act3++;
}

static void
Wait_Count(OBJECT *op)
{
	if (op->timer == (HARD_MODE() ? WARN_TIME_H : WARN_TIME))
		WARN_W(op) = WARN_DOWN;

	if (--op->timer == 0) {
		op->act2 = MODE_DROP;
		op->act3 = 0;
	}
}

static void
Drl_Drop(OBJECT *op)
{
	drop_tbl[op->act3](op);
}

static void
Drop_Start(OBJECT *op)
{
	PlaySound(SE_BANK_ENEMY, SE_DRL_DROP, op);
	op->spd_y = DROP_SPD;
	op->grav  = DROP_GRAV;
	SetAnime(op, ANM_DRILL);
	MOVE_W(op) = MOVE_DOWN;
	op->act3++;
}

static void
Drop_Move(OBJECT *op)
{
	MoveObject2(op);

	if (op->col_f & COL_D) {
		MOVE_W(op) = MOVE_STOP;
		WARN_W(op) = WARN_NONE;
		op->act2   = MODE_DRILL;
		op->act3   = op->var ? 2 : 0;
		return;
	}

	if (POS_Y(op) >= HOME_Y(op) + DROP_MAX) {
		POS_Y(op)  = HOME_Y(op) + DROP_MAX;
		MOVE_W(op) = MOVE_STOP;
		WARN_W(op) = WARN_NONE;
		op->act2   = MODE_DRILL;
		op->act3   = 0;
	}
}

/* ---------------------------------------------------------------- */

static void
Drl_Drill(OBJECT *op)
{
	drill_tbl[op->act3](op);
}

static void
Drill_Start(OBJECT *op)
{
	PlaySound(SE_BANK_ENEMY, SE_DRL_DRILL, op);
	op->timer = DRILL_TIME;
	op->act3++;
}

static void
Drill_Count(OBJECT *op)
{
	if (op->timer == WARN_TIME)
		WARN_W(op) = WARN_UP;

	if (--op->timer == 0) {
		MOVE_W(op) = MOVE_UP;
		op->act2   = MODE_RISE;
		op->act3   = 0;
	}
}

static void
Rock_Start(OBJECT *op)
{
	switch (op->var) {
	case 1:
		CopyBlock(0, 0x1220, ROCK_Y, &rock_blk1);
		break;
	case 2:
		CopyBlock(0, 0x1300, ROCK_Y, &rock_blk2);
		break;
	case 3:
		CopyBlock(0, 0x1340, ROCK_Y, &rock_blk3);
		break;
	}

	bg_layer[0].redraw = 1;

	Drl_Explode(op, 4);
	SpawnJunk(6, drl_junk, op);
	POS_Y(op) -= ROCK_OFS_Y;

	if (Drl_CameraStill())
		Quake(8, 4, 2);

	op->timer = DRILL_TIME;
	op->act3++;
}

static void
Rock_Loop(OBJECT *op)
{
	if ((op->timer & 7) == 0) {
		Drl_Explode(op, 3);
		POS_Y(op) -= ROCK_OFS_Y;
	}

	if ((op->timer & 15) == 0) {
		POS_Y(op) += ROCK_OFS_Y;
		SpawnJunk(6, drl_junk, op);
		POS_Y(op) -= ROCK_OFS_Y;
		if (Drl_CameraStill())
			Quake(12, 1, 2);
	}

	if (op->timer == WARN_TIME)
		WARN_W(op) = WARN_UP;

	if (--op->timer == 0) {
		MOVE_W(op) = MOVE_UP;
		op->act2   = MODE_RISE;
		op->act3   = 0;
	}
}

/* ---------------------------------------------------------------- */

static void
Drl_Rise(OBJECT *op)
{
	rise_tbl[op->act3](op);
}

static void
Rise_Start(OBJECT *op)
{
	PlaySound(SE_BANK_ENEMY, SE_DRL_RISE, op);
	SetAnime(op, ANM_RISE);
	op->spd_y  = RISE_SPD;
	op->grav   = 0;
	MOVE_W(op) = MOVE_UP;
	op->timer  = RISE_SE_TIME;
	op->act3++;
}

static void
Rise_Move(OBJECT *op)
{
	MoveObject2(op);

	if (POS_Y(op) <= HOME_Y(op)) {
		POS_Y(op)  = HOME_Y(op);
		WARN_W(op) = WARN_NONE;
		op->act2   = MODE_WAIT;
		op->act3   = 0;
		return;
	}

	if (--op->timer == 0) {
		op->timer = RISE_SE_TIME;
		PlaySound(SE_BANK_ENEMY, SE_DRL_RISE, op);
	}
}

/* ---------------------------------------------------------------- */
/*	effects  (misc object)                                          */
/* ---------------------------------------------------------------- */

void
X00_DridlerFx(OBJECT *op)
{
	fx_act[op->act](op);
}

static void
Fx_Init(OBJECT *op)
{
	op->flip     = OBJ_FLIP_L;
	FX_OFS_Y(op) = 0;

	switch (op->var) {
	case 0:
		op->pri = FX_PRI;
		SetAnime(op, ANM_FX_UP);
		op->disp = 1;
		op->act2 = 0;
		break;
	case 1:
		op->pri = FX_PRI;
		SetAnime(op, ANM_FX_WARN_D);
		op->disp = 0;
		op->act2 = 1;
		break;
	}

	op->spd_x = 0;
	op->acc_x = 0;
	op->spd_y = 0;
	op->grav  = 0;
	op->act   = ACT_MAIN;
}

static void
Fx_Main(OBJECT *op)
{
	OBJECT	*pp = FX_PARENT(op);

	if (pp->act == ACT_OUT) {
		op->disp = 0;
		op->act++;
		return;
	}

	AnimeTick(op);
	fx_mode[op->act2](op);
	POS_Y(op) = POS_Y(pp) + FX_OFS_Y(op);
}

static void
Fx_Delete(OBJECT *op)
{
	DeleteObject3(op);
}

static void
Fx_Move(OBJECT *op)
{
	u_char	move = MOVE_W(FX_PARENT(op));

	if (FX_LAST(op) != move) {
		switch (move) {
		case MOVE_STOP:
			op->disp = 0;
			break;
		case MOVE_UP:
			op->pri  = FX_PRI_UP;
			op->disp = move;
			SetAnime(op, ANM_FX_UP);
			break;
		case MOVE_DOWN:
			op->pri  = FX_PRI;
			op->disp = 1;
			SetAnime(op, ANM_FX_DOWN);
			break;
		}
	}
	FX_LAST(op) = move;
}

static void
Fx_Warn(OBJECT *op)
{
	OBJECT	*pp = FX_PARENT(op);
	u_char	warn = WARN_W(pp);

	if (pp->anime.sprt == 23)
		FX_OFS_Y(op) = -1;
	else
		FX_OFS_Y(op) = 0;

	if (FX_LAST(op) != warn) {
		switch (warn) {
		case WARN_NONE:
			op->disp = 0;
			break;
		case WARN_UP:
			op->disp = warn;
			SetAnime(op, ANM_FX_WARN_U);
			break;
		case WARN_DOWN:
			op->disp = 1;
			SetAnime(op, ANM_FX_WARN_D);
			break;
		}
	}
	FX_LAST(op) = warn;
}
