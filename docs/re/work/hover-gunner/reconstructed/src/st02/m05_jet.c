/*
 *	m05_jet.c
 *
 *	ST02 (opening stage)  main object 05 : small jet mechaniloid
 *	              visual object 04 : its shot
 *	              misc   object 06 : muzzle flash
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
	ACT_WAIT_L,
	ACT_OUT
};

enum {
	MODE_REACT = 0,
	MODE_NOP,
	MODE_DRIFT,
	MODE_FLY,
	MODE_BURST,
	MODE_FAN,
	MODE_LEAVE,
	MODE_BOB
};

enum {
	ANM_FLY = 0,
	ANM_AIM_H,
	ANM_AIM_U,
	ANM_AIM_D,
	ANM_AIM_UH,
	ANM_AIM_DH,
	ANM_TAMA,
	ANM_FIRE_FX,
	ANM_JUNK0,
	ANM_JUNK1,
	ANM_JUNK2,
	ANM_JUNK3,
	ANM_JUNK4
};

#define VAR_CHILD		0x80
#define VAR_TYPE(o)		((o)->var & 0x0f)
#define VAR_RIGHT		1
#define VAR_HOVER		2
#define VAR_TURRET		3

#define JET_HP			5
#define JET_ATK			3
#define JET_PRI			4

#define JET_SPD			0x40000
#define DRIFT_SPD		0x8000
#define LEAVE_SPD		0x20000

#define DRIFT_TIME		48
#define FLY_TIME		60
#define AIM_WAIT		20
#define FIRE_GAP		8
#define BURST_WAIT		20
#define BURST_SHOTS		3
#define BURST_LOOP		2
#define FAN_SHOTS		9
#define WAKE_X			176
#define TURRET_WAIT		90
#define TURRET_FREEZE		60

#define BOB_SPD			0x20000
#define BOB_WAIT		40
#define BOB_TIME		32
#define BOB_GRAV		0x1000

#define DROP_JET		18

#define TAMA_HP			1
#define TAMA_ATK		3
#define TAMA_PRI		1
#define TAMA_OUT		32
#define TAMA_DISP		16

#define FX_DISP_X		48
#define FX_DISP_Y		16

/* work area */
#define BOB_TIME_W(o)		(*(short *)&(o)->work[0x00])
#define WAIT_W(o)		((o)->work[0x02])
#define RANGE_W(o)		((o)->work[0x03])
#define SHOTS_W(o)		((o)->work[0x04])
#define LOOP_W(o)		((o)->work[0x05])
#define DIR_W(o)		(*(char *)&(o)->work[0x07])
#define DIR_A_W(o)		((o)->work[0x08])
#define DIR_B_W(o)		((o)->work[0x09])
#define DIR_C_W(o)		((o)->work[0x0a])
#define DELAY_W(o)		((o)->work[0x0b])

#define SAVE_ACT3(o)		((o)->work[0x0c])
#define SAVE_TIMER(o)		((o)->work[0x0d])
#define SAVE_Y(o)		(*(short *)&(o)->work[0x10])
#define SAVE_SPD_Y(o)		(*(long *)&(o)->work[0x12])
#define SAVE_GRAV(o)		(*(long *)&(o)->work[0x16])

#define PARENT(o)		(*(OBJECT **)&(o)->timer)
#define FX_PARENT(o)		((OBJECT *)(o)->atk_box)

/* ---------------------------------------------------------------- */

extern void	ReactMode(OBJECT *op);

static void	Jet_Init(OBJECT *op);
static void	Jet_Main(OBJECT *op);
static void	Jet_Delete(OBJECT *op);
static void	Jet_WaitLeft(OBJECT *op);
static void	Jet_Nop(OBJECT *op);
static void	Jet_Drift(OBJECT *op);
static void	Jet_Fly(OBJECT *op);
static void	Jet_Burst(OBJECT *op);
static void	Jet_Fan(OBJECT *op);
static void	Jet_Leave(OBJECT *op);
static void	Jet_Bob(OBJECT *op);

static void	Drift_Start(OBJECT *op);
static void	Drift_Move(OBJECT *op);
static void	Fly_Start(OBJECT *op);
static void	Fly_Move(OBJECT *op);
static void	Burst_Stop(OBJECT *op);
static void	Burst_Aim(OBJECT *op);
static void	Burst_Wait(OBJECT *op);
static void	Burst_Fire(OBJECT *op);
static void	Burst_Gap(OBJECT *op);
static void	Burst_End(OBJECT *op);
static void	Fan_Start(OBJECT *op);
static void	Fan_Aim(OBJECT *op);
static void	Fan_Fire(OBJECT *op);
static void	Fan_Next(OBJECT *op);
static void	Fan_Pass(OBJECT *op);
static void	Fan_End(OBJECT *op);
static void	Leave_Start(OBJECT *op);
static void	Leave_Move(OBJECT *op);
static void	Bob_Start(OBJECT *op);
static void	Bob_Move(OBJECT *op);
static void	Bob_Stop(OBJECT *op);
static void	Bob_Back(OBJECT *op);

static void	Tama_Init(OBJECT *op);
static void	Tama_Main(OBJECT *op);
static void	Tama_Delete(OBJECT *op);
static void	Tama_Nop(OBJECT *op);

static void	Fx_Init(OBJECT *op);
static void	Fx_Main(OBJECT *op);
static void	Fx_Delete(OBJECT *op);

/* ---------------------------------------------------------------- */

static HITBOX	jet_dmg = { -9, -9, 23, 18 };
static HITBOX	jet_atk = {  0,  0, 11, 11 };

static short	jet_range[] = { 64, 80, 96, 112 };

static u_char	jet_junk[] = { ANM_JUNK0, ANM_JUNK1, ANM_JUNK2, ANM_JUNK3, ANM_JUNK4 };

static u_char	turret_dir[32] = {
	 0,  1,  2,  2,  2,  2,  2,  2,
	15, 15, 15, 15, 15, 15, 14, 15,
	16, 17, 18, 18, 18, 18, 18, 18,
	18, 30, 30, 30, 30, 30, 30, 31,
};

static ANIME	anm_fly[]   = { { 1, 0,  1,  0 }, { 1, 0,        -1,  1 } };
static ANIME	anm_aim_h[] = { { 1, 0,  1,  2 }, { 1, ANM_FIRE, -1,  3 } };
static ANIME	anm_aim_u[] = { { 1, 0,  1, 10 }, { 1, ANM_FIRE, -1, 11 } };
static ANIME	anm_aim_d[] = { { 1, 0,  1, 14 }, { 1, ANM_FIRE, -1, 15 } };
static ANIME	anm_aim_uh[] = { { 1, 0, 1,  4 }, { 1, ANM_FIRE, -1,  5 } };
static ANIME	anm_aim_dh[] = { { 1, 0, 1, 12 }, { 1, ANM_FIRE, -1, 13 } };
static ANIME	anm_tama[] = {
	{ 2, 0,  1, 16 },
	{ 2, 0,  1, 17 },
	{ 2, 0,  1, 18 },
	{ 2, 0, -3, 19 },
};
static ANIME	anm_fire_fx[] = {
	{ 1, 0, 1, 20 },
	{ 1, 0, 1, 21 },
	{ 1, 0, 1, 22 },
	{ 2, 0, 1, 23 },
	{ 3, 0, 1, 24 },
	{ 4, 0, 1, 25 },
	{ 5, 0, 0, 26 },
};
static ANIME	anm_junk0[] = { { 1, 0, 0, 27 } };
static ANIME	anm_junk1[] = { { 1, 0, 0, 28 } };
static ANIME	anm_junk2[] = { { 1, 0, 0, 29 } };
static ANIME	anm_junk3[] = { { 1, 0, 0, 30 } };
static ANIME	anm_junk4[] = { { 1, 0, 0, 31 } };

static ANIME	*jet_anime[] = {
	anm_fly,
	anm_aim_h,
	anm_aim_u,
	anm_aim_d,
	anm_aim_uh,
	anm_aim_dh,
	anm_tama,
	anm_fire_fx,
	anm_junk0,
	anm_junk1,
	anm_junk2,
	anm_junk3,
	anm_junk4,
};

static void	(*jet_act[])(OBJECT *) = {
	Jet_Init,
	Jet_Main,
	Jet_Delete,
	Jet_WaitLeft,
	Jet_Delete,
};

static void	(*jet_mode[])(OBJECT *) = {
	ReactMode,
	Jet_Nop,
	Jet_Drift,
	Jet_Fly,
	Jet_Burst,
	Jet_Fan,
	Jet_Leave,
	Jet_Bob,
};

static void	(*drift_tbl[])(OBJECT *) = { Drift_Start, Drift_Move };
static void	(*fly_tbl[])(OBJECT *)   = { Fly_Start, Fly_Move };
static void	(*burst_tbl[])(OBJECT *) = {
	Burst_Stop, Burst_Aim, Burst_Wait, Burst_Fire, Burst_Gap, Burst_End
};
static void	(*fan_tbl[])(OBJECT *) = {
	Fan_Start, Fan_Aim, Fan_Fire, Fan_Next, Fan_Pass, Fan_End
};
static void	(*leave_tbl[])(OBJECT *) = { Leave_Start, Leave_Move };
static void	(*bob_tbl[])(OBJECT *)   = { Bob_Start, Bob_Move, Bob_Stop, Bob_Back };

static HITBOX	tama_box = { -3, -6, 8, 8 };
static HITBOX	tama_col = {  0,  0, 4, 4 };

/* muzzle offsets : L, R, UL, UR, U, D, DL, DR */
static char	tama_ofs[8][2] = {
	{ -13,   0 }, {  13,   0 },
	{ -10, -12 }, {  10, -12 },
	{   0, -16 }, {   0,  16 },
	{ -10,  12 }, {  10,  12 },
};
static char	fx_ofs[8][2] = {
	{ -13,   0 }, {  13,   0 },
	{ -10, -12 }, {  10, -12 },
	{   0, -16 }, {   0,  16 },
	{ -10,  12 }, {  10,  12 },
};

static void	(*tama_act[])(OBJECT *) = {
	Tama_Init,
	Tama_Main,
	Tama_Delete,
	Tama_Nop,
};

static void	(*fx_act[])(OBJECT *) = {
	Fx_Init,
	Fx_Main,
	Fx_Delete,
};

/* ---------------------------------------------------------------- */

static void
Jet_SetPose(OBJECT *op)
{
	switch (DIR_W(op) & 0xfe) {
	case 0:
		SetAnime(op, ANM_AIM_H);
		op->flip = OBJ_FLIP_R;
		break;
	case 2: case 4: case 6:
		op->flip = OBJ_FLIP_R;
		/* fall through */
	case 10: case 12: case 14:
		if (DIR_W(op) >= 10)
			op->flip = OBJ_FLIP_L;
		SetAnime(op, ANM_AIM_UH);
		break;
	case 8:
		SetAnime(op, ANM_AIM_U);
		break;
	case 16:
		SetAnime(op, ANM_AIM_H);
		op->flip = OBJ_FLIP_L;
		break;
	case 18: case 20: case 22:
		op->flip = OBJ_FLIP_L;
		/* fall through */
	case 26: case 28: case 30:
		if (DIR_W(op) >= 26)
			op->flip = OBJ_FLIP_R;
		SetAnime(op, ANM_AIM_DH);
		break;
	case 24:
		SetAnime(op, ANM_AIM_D);
		break;
	}
}

static int
Jet_Ofs(OBJECT *op)
{
	switch (op->var & 0xfe) {
	case 2: case 4: case 6:		return 3;
	case 8:				return 4;
	case 10: case 12: case 14:	return 2;
	case 18: case 20: case 22:	return 6;
	case 24:			return 5;
	case 26: case 28: case 30:	return 7;
	}
	return (op->flip & OBJ_FLIP_R) ? 1 : 0;
}

static void
Jet_TurnMega(OBJECT *op)
{
	if (POS_X(&mega) - POS_X(op) > 0)
		op->flip = OBJ_FLIP_R;
}

static void
Jet_Shot(OBJECT *op)
{
	OBJECT	*tp;

	if ((tp = GetVisualObject()) != NULL) {
		PlaySound(SE_BANK_ENEMY, SE_JET_SHOT, op);
		tp->flag = 0x41;
		tp->type = 4;
		tp->x    = op->x;
		tp->y    = op->y;
		PARENT(tp) = op;
		*(long *)&tp->work[0x06] = 0;
		tp->var  = DIR_W(op);
		SetSpeedDir(tp, DIR_W(op) & 0xfe);

		WAIT_W(op) = FIRE_GAP;
		op->act3++;
		SHOTS_W(op)++;
	}

	if ((tp = GetMiscObject()) != NULL) {
		tp->flag    = 0x41;
		tp->type    = 6;
		tp->x       = op->x;
		tp->y       = op->y;
		tp->atk_box = (HITBOX *)op;
		tp->var     = DIR_W(op);
	}
}

static void
Jet_Search(OBJECT *op)
{
	short	range;
	short	dx;

	if (op->fall_f)
		return;
	if (op->act2 == MODE_BURST || op->act2 == MODE_FAN)
		return;

	range = jet_range[RANGE_W(op)];

	if (op->var == VAR_TURRET) {
		if (DELAY_W(op) == 0) {
			op->act2 = MODE_BURST;
			op->act3 = 0;
		} else {
			DELAY_W(op)--;
		}
		return;
	}

	dx = POS_X(&mega) - POS_X(op);
	if (dx < 0)
		dx = -dx;
	if (dx <= range) {
		op->act2 = MODE_BURST;
		op->act3 = 0;
	}
}

static void
Jet_Save(OBJECT *op)
{
	SAVE_ACT3(op)  = op->act3;
	SAVE_TIMER(op) = op->timer;
	SAVE_Y(op)     = POS_Y(op);
	SAVE_SPD_Y(op) = op->spd_y;
	SAVE_GRAV(op)  = op->grav;
}

static void
Jet_Load(OBJECT *op)
{
	op->act3  = SAVE_ACT3(op);
	op->timer = (char)SAVE_TIMER(op);
	POS_Y(op) = SAVE_Y(op);
	op->spd_y = SAVE_SPD_Y(op);
	op->grav  = SAVE_GRAV(op);
}

/* ---------------------------------------------------------------- */

void
M05_Jet(OBJECT *op)
{
	jet_act[op->act](op);
}

static void
Jet_Init(OBJECT *op)
{
	op->layer     = mega.layer;
	op->anime_tbl = jet_anime;
	op->pri       = JET_PRI;
	op->atk_box   = &jet_atk;
	op->dmg_box   = &jet_dmg;
	op->dmg_tbl   = dmg_tbl_mecha_s;
	op->spd_x     = 0;
	op->spd_y     = 0;
	op->acc_x     = 0;
	op->grav      = 0;
	op->fall_f    = 0;
	op->col_box   = NULL;
	op->muteki    = 0;
	op->hp        = JET_HP;
	op->atk       = JET_ATK;
	op->act       = ACT_MAIN;
	op->act3      = 0;
	RANGE_W(op)   = GetRNG() & 3;
	DELAY_W(op)   = 0;

	if (op->var & VAR_CHILD) {
		SetAnime(op, ANM_FLY);
		op->act2   = MODE_DRIFT;
		op->fall_f = 1;
	} else {
		op->flip   = OBJ_FLIP_L;
		SetAnime(op, ANM_FLY);
		op->fall_f = 0;
		op->act2   = MODE_FLY;
	}

	if (VAR_TYPE(op) == VAR_RIGHT)
		op->act = ACT_WAIT_L;

	if (op->var == VAR_TURRET) {
		SetAnime(op, ANM_AIM_H);
		Jet_Save(op);
		op->act2 = MODE_BOB;
	}
}

static void
Jet_Main(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;

	if (ContactWeapon(op) < 0) {
		SpawnExplosion(op);
		DropItem(op, DROP_JET);
		SpawnJunk(5, jet_junk, op);
		op->act = ACT_DEAD;
		return;
	}

	if (op->var == VAR_TURRET) {
		if (mega.x.l < op->x.l)
			op->flip = OBJ_FLIP_L;
		else
			op->flip = OBJ_FLIP_R;
	}

	Jet_Search(op);

	if (DELAY_W(op) < TURRET_FREEZE)
		jet_mode[op->act2](op);
	else
		AnimeTick(op);

	ContactMega(op);

	if (OffScreenCheck(op))
		op->act = ACT_OUT;
	else
		DisplayCheck1(op);
}

static void
Jet_Delete(OBJECT *op)
{
	int	i;

	for (i = 0x02; i <= 0x0a; i++)
		op->work[i] = 0;

	if (op->var & VAR_CHILD)
		DeleteObject3(op);
	else if (op->act == ACT_DEAD)
		DeleteObject(op);
	else
		DeleteObject2(op);
}

static void
Jet_WaitLeft(OBJECT *op)
{
	if (POS_X(&mega) - POS_X(op) >= WAKE_X) {
		op->flip = OBJ_FLIP_R;
		op->act  = ACT_MAIN;
		op->act2 = MODE_FLY;
	}
}

/* ---------------------------------------------------------------- */

static void
Jet_Nop(OBJECT *op)
{
}

static void
Jet_Drift(OBJECT *op)
{
	drift_tbl[op->act3](op);
}

static void
Drift_Start(OBJECT *op)
{
	op->fall_f = 1;
	op->act3++;
	op->spd_x = op->flip ? DRIFT_SPD : -DRIFT_SPD;
	op->acc_x = 0;
	op->spd_y = 0;
	op->grav  = 0;
	WAIT_W(op) = DRIFT_TIME;
	SetAnime(op, ANM_FLY);
}

static void
Drift_Move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
	if (--WAIT_W(op) == 0) {
		op->act2 = MODE_FLY;
		op->act3 = 0;
	}
}

static void
Jet_Fly(OBJECT *op)
{
	fly_tbl[op->act3](op);
}

static void
Fly_Start(OBJECT *op)
{
	op->fall_f = 0;
	op->act3++;
	if (VAR_TYPE(op) == VAR_HOVER)
		op->spd_x = 0;
	else
		op->spd_x = op->flip ? JET_SPD : -JET_SPD;
	op->acc_x = 0;
	op->spd_y = 0;
	op->grav  = 0;
	SetAnime(op, ANM_FLY);
	WAIT_W(op) = FLY_TIME;
}

static void
Fly_Move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
	if (--WAIT_W(op) == 0) {
		op->fall_f = -1;
		op->act2   = MODE_LEAVE;
		op->act3   = 0;
	}
}

/* ---------------------------------------------------------------- */

static void
Jet_Burst(OBJECT *op)
{
	burst_tbl[op->act3](op);
	AnimeTick(op);
}

static void
Burst_Stop(OBJECT *op)
{
	op->spd_x = 0;
	op->acc_x = 0;
	op->spd_y = 0;
	op->grav  = 0;
	op->act3++;
	if (op->var == VAR_TURRET)
		WAIT_W(op) = 1;
	else
		WAIT_W(op) = AIM_WAIT;
}

static void
Burst_Aim(OBJECT *op)
{
	if (--WAIT_W(op))
		return;

	Jet_TurnMega(op);
	DIR_W(op) = AimAt(op, &mega);
	if (op->var == VAR_TURRET)
		DIR_W(op) = turret_dir[DIR_W(op)];
	Jet_SetPose(op);
	op->act3++;
}

static void
Burst_Wait(OBJECT *op)
{
	if (op->anime.flag)
		op->act3++;
}

static void
Burst_Fire(OBJECT *op)
{
	Jet_Shot(op);

	if (op->var == VAR_TURRET) {
		DELAY_W(op) = TURRET_WAIT;
		op->act2    = MODE_BOB;
		Jet_Load(op);
	}
}

static void
Burst_Gap(OBJECT *op)
{
	if (SHOTS_W(op) >= BURST_SHOTS) {
		op->act3++;
		SHOTS_W(op) = 0;
		WAIT_W(op)  = BURST_WAIT;
		return;
	}
	if (--WAIT_W(op) == 0)
		op->act3--;
}

static void
Burst_End(OBJECT *op)
{
	if (WAIT_W(op)) {
		WAIT_W(op)--;
		return;
	}

	if (++LOOP_W(op) >= BURST_LOOP) {
		op->act2    = MODE_FAN;
		op->act3    = 0;
		LOOP_W(op)  = 0;
	} else {
		op->act2 = MODE_FLY;
		op->act3 = 0;
	}
}

/* ---------------------------------------------------------------- */

static void
Jet_Fan(OBJECT *op)
{
	fan_tbl[op->act3](op);
	AnimeTick(op);
}

static void
Fan_Side(OBJECT *op)
{
	DIR_C_W(op) = DIR_W(op) & 0xfe;

	switch (DIR_W(op) & 0xfe) {
	case 0:				DIR_A_W(op) = 30; DIR_B_W(op) =  2; break;
	case 2: case 4: case 6:		DIR_A_W(op) =  0; DIR_B_W(op) =  8; break;
	case 8:				DIR_A_W(op) =  6; DIR_B_W(op) = 10; break;
	case 10: case 12: case 14:	DIR_A_W(op) =  8; DIR_B_W(op) = 16; break;
	case 16:			DIR_A_W(op) = 14; DIR_B_W(op) = 18; break;
	case 18: case 20: case 22:	DIR_A_W(op) = 16; DIR_B_W(op) = 24; break;
	case 24:			DIR_A_W(op) = 22; DIR_B_W(op) = 26; break;
	case 26: case 28: case 30:	DIR_A_W(op) = 24; DIR_B_W(op) =  0; break;
	}
}

static void
Fan_Start(OBJECT *op)
{
	op->spd_x = 0;
	op->acc_x = 0;
	op->spd_y = 0;
	op->grav  = 0;
	op->act3++;
	SetAnime(op, ANM_AIM_H);
	WAIT_W(op)  = AIM_WAIT;
	SHOTS_W(op) = 0;
	Jet_TurnMega(op);
	DIR_W(op) = AimAt(op, &mega);
	Fan_Side(op);
}

static void
Fan_Aim(OBJECT *op)
{
	if (--WAIT_W(op))
		return;

	Jet_SetPose(op);
	op->act3++;
}

static void
Fan_Fire(OBJECT *op)
{
	if (op->anime.flag)
		Jet_Shot(op);
}

static void
Fan_Next(OBJECT *op)
{
	if (SHOTS_W(op) < FAN_SHOTS) {
		switch (SHOTS_W(op) - 1) {
		case 0: case 4:
			DIR_W(op) = DIR_A_W(op);
			break;
		case 2: case 6:
			DIR_W(op) = DIR_B_W(op);
			break;
		case 1: case 3: case 5: case 7:
			DIR_W(op) = DIR_C_W(op);
			break;
		}
		op->act3 -= 2;
	} else {
		op->act3++;
	}
}

static void
Fan_Pass(OBJECT *op)
{
	op->act3++;
}

static void
Fan_End(OBJECT *op)
{
	op->fall_f = -1;
	op->act2   = MODE_LEAVE;
	op->act3   = 0;
}

/* ---------------------------------------------------------------- */

static void
Jet_Leave(OBJECT *op)
{
	leave_tbl[op->act3](op);
}

static void
Leave_Start(OBJECT *op)
{
	op->spd_x = 0;
	op->acc_x = 0;
	op->spd_y = LEAVE_SPD;
	op->grav  = 0;
	op->act3++;
	SetAnime(op, ANM_AIM_H);
}

static void
Leave_Move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
}

/* ---------------------------------------------------------------- */
/*	turret (var 3) : bob up and down between shots                  */
/* ---------------------------------------------------------------- */

static void
Jet_Bob(OBJECT *op)
{
	bob_tbl[op->act3](op);
	Jet_Save(op);
}

static void
Bob_Start(OBJECT *op)
{
	if (POS_X(&mega) - POS_X(op) > 0)
		op->flip = OBJ_FLIP_R;
	SetAnime(op, ANM_AIM_H);
	AnimeTick(op);
	BOB_TIME_W(op) = BOB_TIME;
	op->timer   = BOB_WAIT;
	op->spd_x = 0;
	op->acc_x = 0;
	op->spd_y = -BOB_SPD;
	op->grav  = 0;
	op->act3++;
}

static void
Bob_Move(OBJECT *op)
{
	AnimeTick(op);
	if (op->timer) {
		op->timer--;
		return;
	}
	MoveObject(op);
	if (--BOB_TIME_W(op) == 0) {
		op->timer   = BOB_WAIT;
		BOB_TIME_W(op) = BOB_TIME;
		op->spd_y      = -op->spd_y;
	}
}

static void
Bob_Stop(OBJECT *op)
{
	AnimeTick(op);
	MoveObject2(op);
	if (op->spd_y == 0) {
		op->timer = BOB_WAIT;
		op->spd_y    = 0;
		op->grav     = -BOB_GRAV;
		op->act3++;
	}
}

static void
Bob_Back(OBJECT *op)
{
	AnimeTick(op);
	if (op->timer) {
		op->timer--;
		return;
	}
	MoveObject2(op);
	if (op->spd_y > 0x1ffff)
		op->grav = BOB_GRAV;
	if (op->spd_y == 0)
		op->act3 = 0;
}

/* ---------------------------------------------------------------- */
/*	shot  (visual object)                                           */
/* ---------------------------------------------------------------- */

void
V04_JetTama(OBJECT *op)
{
	if (*(long *)&op->work[0x06] == 0 && op->col_f == 0)
		CornerCheck(op);
	tama_act[op->act](op);
}

static void
Tama_Init(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);
	int	n;

	op->col_box   = &tama_col;
	op->dmg_box   = &tama_box;
	op->atk_box   = &tama_box;
	op->disp      = 1;
	op->acc_x     = 0;
	op->grav      = 0;
	op->fall_f    = 0;
	op->pri       = TAMA_PRI;
	op->dmg_tbl   = dmg_tbl_shot;
	op->act++;
	op->tpage     = pp->tpage;
	op->clut      = pp->clut & ~CLUT_FLASH;
	op->anime_tbl = pp->anime_tbl;
	op->sprt_p    = pp->sprt_p;
	op->flip      = pp->flip;
	op->layer     = pp->layer;
	op->atk       = TAMA_ATK;
	op->hp        = TAMA_HP;
	op->spd_x   <<= 1;
	op->spd_y   <<= 1;

	n = Jet_Ofs(op);
	POS_X(op) += tama_ofs[n][0];
	POS_Y(op) += tama_ofs[n][1];

	SetAnime(op, ANM_TAMA);
}

static void
Tama_Main(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);

	if (ContactMega(op))
		goto end;

	if (ContactWeapon(op) < 0) {
		SpawnExplosion(op);
		goto end;
	}

	if (BoxOverlap(op, &mega))
		goto end;

	if (op->col_f) {
		SpawnExplosion(op);
		goto end;
	}

	if (OffScreenCheck2(op, TAMA_OUT, TAMA_OUT))
		goto end;

	DisplayCheck(op, TAMA_DISP, TAMA_DISP);
	return;
end:
	op->act++;
}

static void
Tama_Delete(OBJECT *op)
{
	DeleteObject3(op);
}

static void
Tama_Nop(OBJECT *op)
{
}

/* ---------------------------------------------------------------- */
/*	muzzle flash  (misc object)                                     */
/* ---------------------------------------------------------------- */

void
X06_JetFire(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	fx_act[op->act](op);
}

static void
Fx_Init(OBJECT *op)
{
	OBJECT	*pp = FX_PARENT(op);
	int	n;

	op->disp      = 1;
	op->spd_x     = 0;
	op->acc_x     = 0;
	op->spd_y     = 0;
	op->grav      = 0;
	op->pri       = 0;
	op->tpage     = pp->tpage;
	op->clut      = pp->clut & ~CLUT_FLASH;
	op->anime_tbl = pp->anime_tbl;
	op->sprt_p    = pp->sprt_p;
	op->flip      = pp->flip;
	op->layer     = pp->layer;
	SetAnime(op, ANM_FIRE_FX);

	n = Jet_Ofs(op);
	POS_X(op) += fx_ofs[n][0];
	POS_Y(op) += fx_ofs[n][1];

	op->act++;
}

static void
Fx_Main(OBJECT *op)
{
	AnimeTick(op);

	if (OffScreenCheck(op)) {
		op->act++;
		return;
	}

	DisplayCheck(op, FX_DISP_X, FX_DISP_Y);
	if (op->anime.next == 0)
		op->act++;
}

static void
Fx_Delete(OBJECT *op)
{
	DeleteObject3(op);
}
