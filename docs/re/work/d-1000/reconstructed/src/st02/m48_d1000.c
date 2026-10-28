/*
 *	m48_d1000.c
 *
 *	ST02 (opening stage)  main object 48 : D-1000  (var 0 : body, var 1 : core)
 *	              visual object 01 : D-1000 effects and shots (var 0 - 5)
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
	ACT_DEAD
};

enum {
	MODE_REACT = 0,
	MODE_NOP,
	MODE_ENTRY,
	MODE_NOP2,
	MODE_THINK,
	MODE_WALK,
	MODE_CROUCH,
	MODE_BOMB,
	MODE_SUMMON,
	MODE_RAM,
	MODE_STUN,
	MODE_DAMAGE = 99
};

enum {
	CORE_APPEAR = 0,
	CORE_ACTIVE,
	CORE_HIT
};

enum {
	FX_SPARK = 0,
	FX_BOMB,
	FX_TAMA,
	FX_PILLAR,
	FX_GLOW,
	FX_EYE
};

enum {
	ANM_STAND = 0,
	ANM_WALK,
	ANM_CROUCH,
	ANM_SHAKE,
	ANM_RAM,
	ANM_THROW,
	ANM_STAGGER,
	ANM_FALL,
	ANM_LAND,
	ANM_GETUP,
	ANM_GLOW,
	ANM_DEAD,
	ANM_CORE_ON,
	ANM_UNUSED13,
	ANM_TAMA,
	ANM_CORE_APPEAR,
	ANM_CORE_HIT,
	ANM_EYE,
	ANM_SPARK,
	ANM_BOMB,
	ANM_BOMB_LAND,
	ANM_PILLAR,
	ANM_PILLAR_END,
	ANM_PILLAR_FX
};

#define D1000_HP		32
#define D1000_ATK		6
#define D1000_PRI		5
#define D1000_TPAGE		480

#define START_X			6288
#define START_Y			160
#define STAND_Y			304
#define CROUCH_Y		352
#define TBOX_Y			288

#define CORE_X_R		6368
#define CORE_X_L		6160
#define CORE_Y			256
#define CORE_SIDE_X		6264
#define CORE_PRI		3
#define CORE_HP			127

#define WALL_L_X		6064
#define WALL_R_X		6448
#define WALL_Y			208

#define FALL_GRAV		0x6000
#define CROUCH_GRAV		0x8000
#define RISE_SPD		0x10000
#define SHAKE_SPD		0x20000
#define WALK_SPD		0x10000
#define RAM_ACC			0x10000
#define RAM_MAX			0x40000
#define SINK_SPD		0x4000

#define THINK_TIME		60
#define WALK_TIME		120
#define NEAR_X			60
#define CROUCH_HOLD		90
#define CROUCH_STOP		60
#define RAM_WAIT		180
#define RAM_STUCK		180
#define STUN_DAMAGE_T		65
#define STUN_DAMAGE		8
#define LAND_TIME		120
#define MUTEKI_TIME		30
#define MUTEKI_SABER		2

#define BOMB_T1			24
#define BOMB_T2_H		84
#define BOMB_END		160
#define BOMB_END_H		220

#define CORE_SIGNAL		100
#define CORE_MOVE_T		60
#define CORE_SPARK		6
#define TAMA_GAP_X		8
#define TAMA_END_X		124
#define TAMA_GAP_Z		60
#define TAMA_END_Z		220

#define PILLAR_TIME		50

#define W_SIGNAL(o)		((o)->act4)
#define W_SUB(o)		(*(OBJECT **)&(o)->work[0x02])
#define W_MUTEKI(o)		((o)->work[0x06])
#define W_TOUCH(o)		((o)->work[0x0a])
#define W_COUNT(o)		(*(short *)&(o)->work[0x00])
#define W_COUNT2(o)		(*(long *)&(o)->work[0x0e])

#define PARENT(o)		(*(OBJECT **)&(o)->timer)
#define TARGET_X(o)		((o)->spd_x)
#define TARGET_Y(o)		((o)->spd_y)

#define IS_X()			(mega.var == 0)

/* ---------------------------------------------------------------- */

extern void	ReactMode(OBJECT *op);

OBJECT	*d1000_body;
long	d1000_dead;
long	d1000_clear;

static void	Body_Init(OBJECT *op);
static void	Body_Main(OBJECT *op);
static void	Body_Dead(OBJECT *op);

static void	Body_Nop(OBJECT *op);
static void	Body_Entry(OBJECT *op);
static void	Body_Nop2(OBJECT *op);
static void	Body_Think(OBJECT *op);
static void	Body_Walk(OBJECT *op);
static void	Body_Crouch(OBJECT *op);
static void	Body_Bomb(OBJECT *op);
static void	Body_Summon(OBJECT *op);
static void	Body_Ram(OBJECT *op);
static void	Body_Stun(OBJECT *op);

static void	Entry_WaitMega(OBJECT *op);
static void	Entry_WaitWarning(OBJECT *op);
static void	Entry_Fall(OBJECT *op);
static void	Entry_Land(OBJECT *op);
static void	Entry_Rise(OBJECT *op);
static void	Entry_FillHp(OBJECT *op);
static void	Nop2_Sub(OBJECT *op);
static void	Walk_Start(OBJECT *op);
static void	Walk_Move(OBJECT *op);
static void	Crouch_Start(OBJECT *op);
static void	Crouch_Drop(OBJECT *op);
static void	Crouch_Shake(OBJECT *op);
static void	Crouch_Hold(OBJECT *op);
static void	Crouch_Rise(OBJECT *op);
static void	Bomb_Start(OBJECT *op);
static void	Bomb_Throw(OBJECT *op);
static void	Summon_Start(OBJECT *op);
static void	Summon_Wait(OBJECT *op);
static void	Ram_Start(OBJECT *op);
static void	Ram_Windup(OBJECT *op);
static void	Ram_Charge(OBJECT *op);
static void	Ram_Stuck(OBJECT *op);
static void	Ram_Rise(OBJECT *op);
static void	Stun_Start(OBJECT *op);
static void	Stun_Stagger(OBJECT *op);
static void	Stun_Fall(OBJECT *op);
static void	Stun_Down(OBJECT *op);
static void	Stun_Rise(OBJECT *op);

static void	Dead_Start(OBJECT *op);
static void	Dead_Blink(OBJECT *op);
static void	Dead_Sink(OBJECT *op);
static void	Dead_Debris(OBJECT *op);
static void	Dead_Idle(OBJECT *op);
static void	Dead_Start2(OBJECT *op);
static void	Dead_Blink2(OBJECT *op);
static void	Dead_End(OBJECT *op);

static void	Core(OBJECT *op);
static void	Core_Init(OBJECT *op);
static void	Core_Main(OBJECT *op);
static void	Core_Delete(OBJECT *op);

static void	Spark_Init(OBJECT *op);
static void	Spark_Main(OBJECT *op);
static void	Bomb_Init(OBJECT *op);
static void	Bomb_Main(OBJECT *op);
static void	Tama_Init(OBJECT *op);
static void	Tama_Main(OBJECT *op);
static void	Pillar_Init(OBJECT *op);
static void	Pillar_Main(OBJECT *op);
static void	Glow_Init(OBJECT *op);
static void	Glow_Main(OBJECT *op);
static void	Eye_Init(OBJECT *op);
static void	Eye_Main(OBJECT *op);
static void	Fx_Delete(OBJECT *op);

/* ---------------------------------------------------------------- */

static HITBOX	body_col  = {   0, -24,  64,  48 };
static HITBOX	body_atk  = { -56, -64, 112,  92 };
static HITBOX	body_dmg  = { -56, -64, 112,  92 };
static HITBOX	core_dmg  = { -10, -10,  20,  20 };
static HITBOX	bomb_col  = {   0,   0,   1,  16 };
static HITBOX	pillar_atk = { -16, -128, 32, 255 };
static HITBOX	tama_col  = {   0,   0,   8,   8 };
static HITBOX	tama_atk  = {  -8,  -8,  16,  16 };
static HITBOX	tama_dmg  = {  -8,  -8,  16,  16 };

static BLOCKCOPY	wall_l      = { 6512, WALL_Y, 1, 13 };
static BLOCKCOPY	wall_l_hit  = { 6528, WALL_Y, 1, 13 };
static BLOCKCOPY	wall_r      = { 6560, WALL_Y, 1, 13 };
static BLOCKCOPY	wall_r_hit  = { 6576, WALL_Y, 1, 13 };

extern ANIME	*d1000_anime[];		/* 0x800FB138, 24 animations */

static void	(*body_act[])(OBJECT *) = {
	Body_Init,
	Body_Main,
	Body_Dead,
};

static void	(*body_mode[])(OBJECT *) = {
	ReactMode,
	Body_Nop,
	Body_Entry,
	Body_Nop2,
	Body_Think,
	Body_Walk,
	Body_Crouch,
	Body_Bomb,
	Body_Summon,
	Body_Ram,
	Body_Stun,
};

static void	(*entry_tbl[])(OBJECT *) = {
	Entry_WaitMega, Entry_WaitWarning, Entry_Fall, Entry_Land, Entry_Rise, Entry_FillHp
};
static void	(*nop2_tbl[])(OBJECT *)   = { Nop2_Sub };
static void	(*walk_tbl[])(OBJECT *)   = { Walk_Start, Walk_Move };
static void	(*crouch_tbl[])(OBJECT *) = {
	Crouch_Start, Crouch_Drop, Crouch_Shake, Crouch_Hold, Crouch_Rise
};
static void	(*bomb_tbl[])(OBJECT *)   = { Bomb_Start, Bomb_Throw };
static void	(*summon_tbl[])(OBJECT *) = { Summon_Start, Summon_Wait };
static void	(*ram_tbl[])(OBJECT *)    = {
	Ram_Start, Ram_Windup, Ram_Charge, Ram_Stuck, Ram_Rise
};
static void	(*stun_tbl[])(OBJECT *)   = {
	Stun_Start, Stun_Stagger, Stun_Fall, Stun_Down, Stun_Rise
};
static void	(*dead_tbl[])(OBJECT *)   = {
	Dead_Start, Dead_Blink, Dead_Sink, Dead_Debris,
	Dead_Idle, Dead_Start2, Dead_Blink2, Dead_End
};

static void	(*core_act[])(OBJECT *) = {
	Core_Init,
	Core_Main,
	Core_Delete,
};

static void	(*fx_spark[])(OBJECT *)  = { Spark_Init,  Spark_Main,  Fx_Delete };
static void	(*fx_bomb[])(OBJECT *)   = { Bomb_Init,   Bomb_Main,   Fx_Delete };
static void	(*fx_tama[])(OBJECT *)   = { Tama_Init,   Tama_Main,   Fx_Delete };
static void	(*fx_pillar[])(OBJECT *) = { Pillar_Init, Pillar_Main, Fx_Delete };
static void	(*fx_glow[])(OBJECT *)   = { Glow_Init,   Glow_Main,   Fx_Delete };
static void	(*fx_eye[])(OBJECT *)    = { Eye_Init,    Eye_Main,    Fx_Delete };

/* ---------------------------------------------------------------- */

static void
SetAnimeBank(OBJECT *op, int bank, int no)
{
	int	n;

	n = GetSpriteBank(d1000_body->type);
	op->sprt_p = (char *)sprt_bank + sprt_bank[n].ofs[bank];
	SetAnime(op, no);
}

static void
D1000_TurnMega(OBJECT *op)
{
	if (POS_X(op) < POS_X(&mega))
		op->flip = OBJ_FLIP_R;
	else
		op->flip = OBJ_FLIP_L;
}

static void
D1000_Move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject2(op);
	DisplayCheck(op, 160, 160);
}

static OBJECT *
D1000_SetFx(OBJECT *op, char var)
{
	OBJECT	*tp;

	if ((tp = GetVisualObject()) != NULL) {
		tp->type = 1;
		tp->var  = var;
		PARENT(tp) = op;
		tp->flag = op->flag;
	}
	return tp;
}

static void
D1000_Walls(void)
{
	CopyBlock(0, WALL_L_X, WALL_Y, &wall_l);
	CopyBlock(0, WALL_R_X, WALL_Y, &wall_r);
	bg_layer[0].redraw = 1;
}

static void
D1000_Kill(OBJECT *op)
{
	op->act  = ACT_DEAD;
	op->act2 = 0;
	op->act3 = 0;
	d1000_dead   = 1;
	mega.nohit_f = 1;
}

/* ---------------------------------------------------------------- */

void
M48_D1000(OBJECT *op)
{
	if (op->var == 1) {
		Core(op);
		return;
	}

	op->old_x = op->x;
	op->old_y = op->y;
	op->disp  = 0;
	body_act[op->act](op);
	CollideCheck(op);
}

static void
Body_Init(OBJECT *op)
{
	op->tpage     = D1000_TPAGE;
	op->anime_tbl = d1000_anime;
	op->pri       = D1000_PRI;
	op->atk_box   = &body_atk;
	op->dmg_box   = &body_dmg;
	op->dmg_tbl   = dmg_tbl_d1000;
	POS_X(op)     = START_X;
	POS_Y(op)     = START_Y;
	op->atk       = D1000_ATK;
	op->flip      = OBJ_FLIP_L;
	op->fall_f    = 0;
	op->disp      = 0;
	op->col_box   = NULL;
	op->pad62     = 0;
	op->muteki    = 0;
	op->spd_x     = 0;
	op->spd_y     = 0;
	op->acc_x     = 0;
	op->grav      = 0;
	op->hp        = 0;
	op->layer     = mega.layer;

	game.boss_f   = 0;
	game.boss     = op;
	game.boss_hp  = D1000_HP;
	BossInit();
	PlayMusic(20, 64);
	W_MUTEKI(op)  = 0;

	d1000_dead  = 0;
	d1000_clear = 0;
	d1000_body  = op;

	op->act2 = MODE_ENTRY;
	op->act++;
}

static void
Body_Main(OBJECT *op)
{
	int	hit;

	hit = ContactWeapon(op);
	if (hit < 0) {
		op->clut &= ~CLUT_FLASH;
		D1000_Kill(op);
		DisplayCheck(op, 100, 100);
		return;
	}

	W_TOUCH(op) |= ContactMega(op);

	if (hit > 0 && hit != 127) {
		if (hit == 25 || hit == 26)
			W_MUTEKI(op) = MUTEKI_SABER;
		else
			W_MUTEKI(op) = MUTEKI_TIME;
		op->dmg_tbl = dmg_tbl_guard;
	}

	if (op->act2 == MODE_DAMAGE) {
		W_MUTEKI(op) = 0;
		op->atk_box  = NULL;
		op->dmg_tbl  = dmg_tbl_guard;
		op->act2     = MODE_STUN;
		op->act3     = 0;
	}

	if (W_MUTEKI(op)) {
		W_MUTEKI(op)--;
		if (W_MUTEKI(op) & 8)
			op->clut |= CLUT_FLASH;
		else
			op->clut &= ~CLUT_FLASH;
		if (W_MUTEKI(op) == 0) {
			op->dmg_tbl = dmg_tbl_d1000;
			op->clut   &= ~CLUT_FLASH;
		}
	}

	body_mode[op->act2](op);
}

static void
Body_Nop(OBJECT *op)
{
}

/* ---------------------------------------------------------------- */
/*	entry                                                           */
/* ---------------------------------------------------------------- */

static void
Body_Entry(OBJECT *op)
{
	entry_tbl[op->act3](op);
}

static void
Entry_WaitMega(OBJECT *op)
{
	OBJECT	*tp;

	if (mega.act2 != 20)
		return;

	((u_char *)&mega)[0x19f] = 2;
	PlayMusic(29, 64);
	op->timer = 40;

	if ((tp = GetSysObject()) != NULL) {
		tp->flag  = 1;
		tp->type  = 24;
		POS_X(tp) = POS_X(op);
		POS_Y(tp) = POS_Y(op);
		W_SUB(op) = tp;
	}
	op->act3++;
}

static void
Entry_WaitWarning(OBJECT *op)
{
	if (game.message)
		return;

	if (--op->timer == 0)
		PlayMusic(20, 64);

	if (W_SUB(op)->flag == 0) {
		op->grav = FALL_GRAV;
		SetAnimeBank(op, 1, ANM_FALL);
		op->act3++;
	}
}

static void
Entry_Fall(OBJECT *op)
{
	if (op->col_box == NULL && POS_Y(op) >= TBOX_Y)
		op->col_box = &body_col;

	if (op->col_f & COL_D) {
		PlaySound(SE_BANK_ENEMY, SE_D1000_LAND, op);
		Rumble(3);
		op->spd_y = 0;
		op->grav  = 0;
		SetAnimeBank(op, 1, ANM_LAND);
		Quake(20, 4, 2);
		op->timer = 0;
		W_SIGNAL(op) = 0;
		op->act3++;
	}

	op->spd_y -= op->grav;
	AnimeTick(op);
	MoveObject(op);
	DisplayCheck(op, 160, 160);
}

static void
Entry_Land(OBJECT *op)
{
	OBJECT	*tp;

	if (++op->timer == 20)
		Quake(20, 2, 2);

	if (op->timer < 30 && (op->timer & 3) == 0)
		RockFall(32, POS_X(op) - 100, 400, POS_X(op) + 100, 416, 8);

	if (op->timer == LAND_TIME) {
		if ((tp = GetMainObject()) != NULL) {
			tp->flag  = op->flag;
			tp->var   = 1;
			W_SUB(tp) = op;
			tp->type  = op->type;
		}
	}

	if (W_SIGNAL(op) == 1) {
		SetAnimeBank(op, 1, ANM_GETUP);
		W_SIGNAL(op)++;
	} else if (W_SIGNAL(op) && op->anime.next == 0) {
		D1000_SetFx(op, FX_GLOW);
		PlaySound(SE_BANK_ENEMY, SE_D1000_RISE, op);
		op->spd_y = RISE_SPD;
		op->act3++;
	}

	D1000_Move(op);
}

static void
Entry_Rise(OBJECT *op)
{
	if (POS_Y(op) <= STAND_Y) {
		POS_Y(op) = STAND_Y;
		op->spd_y = 0;
		SetAnimeBank(op, 0, ANM_STAND);
		ShowBossBar(0);
		game.boss_bar = 1;
		op->timer = 3;
		op->act3++;
	}
	D1000_Move(op);
}

static void
Entry_FillHp(OBJECT *op)
{
	if (BossBarBusy()) {
		DisplayCheck(op, 160, 160);
		return;
	}

	if (op->hp < game.boss_hp) {
		if (--op->timer == 0) {
			PlaySound(SE_BANK_SYS, SE_HP_FILL, NULL);
			op->timer = 3;
		}
		op->hp++;
		DisplayCheck(op, 160, 160);
		return;
	}

	StartBossMusic();
	D1000_Move(op);
	op->timer = 0;
	op->act2  = MODE_THINK;
	op->act3  = 0;
}

static void
Body_Nop2(OBJECT *op)
{
	nop2_tbl[op->act3](op);
	D1000_Move(op);
}

static void
Nop2_Sub(OBJECT *op)
{
}

/* ---------------------------------------------------------------- */
/*	think                                                           */
/* ---------------------------------------------------------------- */

static void
Body_Think(OBJECT *op)
{
	int	r;
	int	healthy;

	r = GetRNG() & 15;
	D1000_TurnMega(op);

	if (++op->timer == THINK_TIME) {
		healthy = (game.boss_hp / 2 < op->hp);

		if (op->act3 == 0) {
			if (healthy)
				op->act2 = (r < 8) ? MODE_RAM : MODE_SUMMON;
			else
				op->act2 = (r < 6) ? MODE_RAM : MODE_BOMB;
		} else {
			if (healthy)
				op->act2 = (r < 8) ? MODE_WALK : MODE_SUMMON;
			else
				op->act2 = (r < 6) ? MODE_SUMMON : MODE_BOMB;
		}
		op->act3 = 0;
	}

	D1000_Move(op);
}

/* ---------------------------------------------------------------- */
/*	walk                                                            */
/* ---------------------------------------------------------------- */

static void
Body_Walk(OBJECT *op)
{
	walk_tbl[op->act3](op);
	D1000_Move(op);
}

static void
Walk_Start(OBJECT *op)
{
	D1000_TurnMega(op);
	op->spd_x = op->flip ? WALK_SPD : -WALK_SPD;
	SetAnimeBank(op, 0, ANM_WALK);
	op->timer = 0;
	op->act3++;
}

static void
Walk_Move(OBJECT *op)
{
	int	dx;

	dx = POS_X(&mega) - POS_X(op);
	if (dx < 0)
		dx = -dx;

	if (dx <= NEAR_X) {
		op->spd_x = 0;
		op->act2  = MODE_CROUCH;
		op->act3  = 0;
		return;
	}

	if (++op->timer == WALK_TIME) {
		SetAnimeBank(op, 0, ANM_STAND);
		op->spd_x = 0;
		op->timer = 0;
		op->act2  = MODE_THINK;
		op->act3  = 0;
	}
}

/* ---------------------------------------------------------------- */
/*	crouch                                                          */
/* ---------------------------------------------------------------- */

static void
Body_Crouch(OBJECT *op)
{
	crouch_tbl[op->act3](op);
	D1000_Move(op);
}

static void
Crouch_Start(OBJECT *op)
{
	op->grav = CROUCH_GRAV;
	SetAnimeBank(op, 0, ANM_CROUCH);
	W_TOUCH(op) = 0;
	op->act3++;
}

static void
Crouch_Drop(OBJECT *op)
{
	if (POS_Y(op) < CROUCH_Y)
		return;

	POS_Y(op) = CROUCH_Y;
	PlaySound(SE_BANK_ENEMY, SE_D1000_STOMP, op);
	Rumble(3);
	op->spd_y = SHAKE_SPD;
	op->grav  = 0;
	SetAnimeBank(op, 0, ANM_SHAKE);
	op->timer = 0;
	op->act3++;
}

static void
Crouch_Shake(OBJECT *op)
{
	if (op->anime.next == 0)
		op->act3++;

	if ((++op->timer & 1) == 0)
		op->spd_y = -op->spd_y;
}

static void
Crouch_Hold(OBJECT *op)
{
	short	t = ++op->timer;

	if (t == CROUCH_HOLD || W_TOUCH(op)) {
		SetAnimeBank(op, 0, ANM_WALK);
		op->spd_y = RISE_SPD;
		op->act3++;
	} else if (t == CROUCH_STOP) {
		op->spd_y = 0;
	} else if (t < CROUCH_STOP && (op->timer & 1) == 0) {
		op->spd_y = -op->spd_y;
	}
}

static void
Crouch_Rise(OBJECT *op)
{
	if (POS_Y(op) <= STAND_Y) {
		POS_Y(op) = STAND_Y;
		op->spd_y = 0;
		SetAnimeBank(op, 0, ANM_STAND);
		op->timer = 0;
		op->act2  = MODE_THINK;
		op->act3  = 1;
	}
}

/* ---------------------------------------------------------------- */
/*	bomb                                                            */
/* ---------------------------------------------------------------- */

static void
Body_Bomb(OBJECT *op)
{
	bomb_tbl[op->act3](op);
	D1000_Move(op);
}

static void
Bomb_Start(OBJECT *op)
{
	D1000_TurnMega(op);
	SetAnimeBank(op, 0, ANM_THROW);
	op->timer = 0;
	op->act3++;
}

static void
Bomb_Throw(OBJECT *op)
{
	short	end;

	D1000_TurnMega(op);
	++op->timer;

	if (game.level == LEVEL_HARD) {
		if (op->timer == BOMB_T1 || op->timer == BOMB_T2_H) {
			PlaySound(SE_BANK_ENEMY, SE_D1000_THROW, op);
			D1000_SetFx(op, FX_BOMB);
		}
		end = BOMB_END_H;
	} else {
		if (op->timer == BOMB_T1) {
			PlaySound(SE_BANK_ENEMY, SE_D1000_THROW, op);
			D1000_SetFx(op, FX_BOMB);
		}
		end = BOMB_END;
	}

	if (op->timer == end) {
		op->act2 = MODE_WALK;
		op->act3 = 0;
	}
}

/* ---------------------------------------------------------------- */
/*	summon : wait for the core to shoot                             */
/* ---------------------------------------------------------------- */

static void
Body_Summon(OBJECT *op)
{
	summon_tbl[op->act3](op);
	D1000_Move(op);
}

static void
Summon_Start(OBJECT *op)
{
	SetAnimeBank(op, 0, ANM_STAND);
	W_SIGNAL(op) = 0;
	op->act3++;
}

static void
Summon_Wait(OBJECT *op)
{
	D1000_TurnMega(op);
	if (W_SIGNAL(op)) {
		op->act2 = MODE_WALK;
		op->act3 = 0;
	}
}

/* ---------------------------------------------------------------- */
/*	ram                                                             */
/* ---------------------------------------------------------------- */

static void
Body_Ram(OBJECT *op)
{
	ram_tbl[op->act3](op);
	D1000_Move(op);
}

static void
Ram_Start(OBJECT *op)
{
	SetAnimeBank(op, 0, ANM_WALK);
	op->timer = 0;
	op->act3++;
}

static void
Ram_Windup(OBJECT *op)
{
	D1000_TurnMega(op);

	if (++op->timer == RAM_WAIT) {
		SetAnimeBank(op, 0, ANM_RAM);
		PlaySound(SE_BANK_ENEMY, SE_D1000_RISE, op);
		op->acc_x = RAM_ACC;
		op->act3++;
	}

	if (POS_Y(op) < POS_Y(&mega)) {
		POS_Y(op) += 2;
		if (POS_Y(op) > POS_Y(&mega))
			POS_Y(op) = POS_Y(&mega);
	} else if (POS_Y(op) > POS_Y(&mega)) {
		POS_Y(op) -= 2;
		if (POS_Y(op) < POS_Y(&mega))
			POS_Y(op) = POS_Y(&mega);
	}
}

static void
Ram_Charge(OBJECT *op)
{
	int	hit;

	if (op->spd_x > RAM_MAX || op->spd_x < -RAM_MAX) {
		op->spd_x = (op->spd_x > 0) ? RAM_MAX : -RAM_MAX;
		op->acc_x = 0;
	}

	if (op->flip)
		hit = op->col_f & COL_R;
	else
		hit = op->col_f & COL_L;
	if (!hit)
		return;

	PlaySound(SE_BANK_ENEMY, SE_D1000_LAND, op);
	Rumble(3);

	if (op->col_f & COL_L)
		CopyBlock(0, WALL_L_X, WALL_Y, &wall_l_hit);
	else
		CopyBlock(0, WALL_R_X, WALL_Y, &wall_r_hit);
	bg_layer[0].redraw = 1;

	op->spd_x = 0;
	op->acc_x = 0;
	Quake(-106, 4, 2);
	op->timer = 0;
	op->act3++;
}

static void
Ram_Stuck(OBJECT *op)
{
	if (++op->timer != RAM_STUCK)
		return;

	D1000_Walls();
	SetAnimeBank(op, 0, ANM_WALK);
	op->spd_y = RISE_SPD;
	op->act3++;
}

static void
Ram_Rise(OBJECT *op)
{
	if (POS_Y(op) <= STAND_Y) {
		POS_Y(op) = STAND_Y;
		op->spd_y = 0;
		op->act2  = MODE_WALK;
		op->act3  = 0;
	}
}

/* ---------------------------------------------------------------- */
/*	stun : the core was hit                                         */
/* ---------------------------------------------------------------- */

static void
Body_Stun(OBJECT *op)
{
	if (W_MUTEKI(op) < 50)
		W_MUTEKI(op) = 100;

	stun_tbl[op->act3](op);
	D1000_Move(op);
}

static void
Stun_Start(OBJECT *op)
{
	D1000_Walls();
	op->spd_x = 0;
	op->acc_x = 0;
	op->spd_y = 0;
	op->grav  = 0;
	SetAnimeBank(op, 1, ANM_STAGGER);
	PlaySound(SE_BANK_ENEMY, SE_D1000_STAGGER, op);
	op->act3++;
}

static void
Stun_Stagger(OBJECT *op)
{
	if (op->anime.next == 0) {
		op->grav = FALL_GRAV;
		SetAnimeBank(op, 1, ANM_FALL);
		op->act3++;
	}
}

static void
Stun_Fall(OBJECT *op)
{
	if ((op->col_f & COL_D) == 0)
		return;

	PlaySound(SE_BANK_ENEMY, SE_D1000_LAND, op);
	Rumble(3);
	d1000_clear = 0;
	op->spd_y = 0;
	op->grav  = 0;
	SetAnimeBank(op, 1, ANM_LAND);
	Quake(60, 4, 2);
	op->timer = 0;
	W_SIGNAL(op) = 0;
	op->act3++;
}

static void
Stun_Down(OBJECT *op)
{
	if (++op->timer == STUN_DAMAGE_T) {
		op->hp -= STUN_DAMAGE;
		if ((char)op->hp <= 0) {
			op->hp = 0;
			D1000_Kill(op);
			DisplayCheck(op, 160, 160);
			return;
		}
	}

	if (W_SIGNAL(op) == 1) {
		SetAnimeBank(op, 1, ANM_GETUP);
		W_SIGNAL(op)++;
	} else if (W_SIGNAL(op) && op->anime.next == 0) {
		PlaySound(SE_BANK_ENEMY, SE_D1000_RISE, op);
		op->spd_y = RISE_SPD;
		op->act3++;
	}
}

static void
Stun_Rise(OBJECT *op)
{
	if (POS_Y(op) > STAND_Y)
		return;

	POS_Y(op)    = STAND_Y;
	op->atk_box  = &body_atk;
	op->dmg_tbl  = dmg_tbl_d1000;
	W_MUTEKI(op) = 0;
	op->spd_y    = 0;
	SetAnimeBank(op, 0, ANM_STAND);
	op->timer    = 0;
	op->act2     = MODE_THINK;
	op->act3     = 0;
}

/* ---------------------------------------------------------------- */
/*	dead                                                            */
/* ---------------------------------------------------------------- */

static void
Body_Dead(OBJECT *op)
{
	dead_tbl[op->act2](op);
}

static void
Dead_Blinking(OBJECT *op)
{
	if (--W_COUNT(op) == -1) {
		op->clut   ^= CLUT_FLASH;
		op->muteki -= 5;
		if (op->muteki >= 26)
			op->muteki = 0;
		W_COUNT(op) = (op->muteki < 6) ? 5 : op->muteki;
	}
}

static void
Dead_Start(OBJECT *op)
{
	D1000_Walls();
	op->timer   = 127;
	W_COUNT(op) = 25;
	op->muteki  = 25;
	op->clut   &= ~CLUT_FLASH;
	SetAnimeBank(op, 0, ANM_STAND);
	op->spd_y   = -SINK_SPD;
	op->spd_x   = 0;
	op->act2++;

	if (op->x.l < mega.x.l)
		PlayMusic(20, 0);
	else
		PlayMusic(20, 64);

	DisplayCheck(op, 160, 160);
}

static void
Dead_Blink(OBJECT *op)
{
	OBJECT	*tp;

	if (--op->timer == 0) {
		if ((tp = GetSysObject()) != NULL) {
			tp->flag  = 0x81;
			tp->type  = 26;
			POS_X(tp) = POS_X(op);
			POS_Y(tp) = POS_Y(op);
			W_SUB(op) = tp;
		}
		op->act2++;
	}

	Dead_Blinking(op);
	DisplayCheck(op, 160, 160);
}

static void
Dead_Sink(OBJECT *op)
{
	OBJECT	*tp = W_SUB(op);

	if (tp->act >= 4)
		op->timer++;
	if (op->timer >= 31)
		MoveObject(op);

	if (tp->act4 == 0) {
		if (--W_COUNT(op) == -1) {
			W_COUNT(op) = 5;
			op->clut   ^= CLUT_FLASH;
		}
		DisplayCheck(op, 64, 64);
		return;
	}

	op->act2++;
	SetAnimeBank(op, 1, ANM_DEAD);

	if (bg_layer[0].x + 200 < POS_X(op)) {
		PlayMusic(20, 64);
		POS_X(&mega) = POS_X(op) - 160;
		op->flip     = OBJ_FLIP_L;
		CameraClampLeft(POS_X(&mega) - 48);
	} else {
		PlayMusic(20, 0);
		POS_X(&mega) = POS_X(op) + 160;
		op->flip     = OBJ_FLIP_R;
		CameraClampRight(POS_X(&mega) - 256);
	}

	bg_layer[0].redraw = 1;
	POS_Y(op)       = bg_layer[0].y + 160;
	game.boss_bar   = 0;
	game.boss       = NULL;
	DisplayCheck(op, 160, 160);
}

static void
Dead_Debris(OBJECT *op)
{
	OBJECT	*tp;
	int	n;
	int	m;

	if (W_SUB(op)->flag == 0) {
		if ((tp = GetMiscObject()) != NULL) {
			tp->flag  = 0x41;
			tp->type  = 16;
			tp->x     = op->x;
			tp->var   = 0;
			tp->y     = op->y;
			tp->tpage = op->tpage + 768;
			n = GetSpriteBank(137);
			m = GetSpriteBank(137);
			tp->clut  = (((n * 4 + 32) % 16)) | (((m + 8) / 4 + 480) << 6);
			n = GetSpriteBank(137);
			tp->sprt_p = (char *)sprt_bank + sprt_bank[n].ofs[0];
		}
		op->act2++;
	}
	DisplayCheck(op, 160, 160);
}

static void
Dead_Idle(OBJECT *op)
{
	op->clut &= ~CLUT_FLASH;
	DisplayCheck(op, 160, 160);
}

static void
Dead_Start2(OBJECT *op)
{
	op->timer   = 127;
	W_COUNT(op) = 25;
	op->muteki  = 25;
	op->clut   &= ~CLUT_FLASH;
	op->act2++;
	DisplayCheck(op, 160, 160);
}

static void
Dead_Blink2(OBJECT *op)
{
	OBJECT	*tp;

	if (--op->timer == 0) {
		if ((tp = GetSysObject()) != NULL) {
			tp->flag  = 1;
			tp->type  = 26;
			POS_X(tp) = POS_X(op);
			POS_Y(tp) = POS_Y(op);
			W_SUB(op) = tp;
		}
		op->act2++;
	}

	Dead_Blinking(op);
	DisplayCheck(op, 160, 160);
}

static void
Dead_End(OBJECT *op)
{
	OBJECT	*tp = W_SUB(op);

	if (tp->flag == 0) {
		DeleteObject3(op);
		return;
	}
	if (tp->act4)
		return;

	if (--W_COUNT(op) == -1) {
		W_COUNT(op) = 5;
		op->clut   ^= CLUT_FLASH;
	}
	DisplayCheck(op, 160, 160);
}

/* ---------------------------------------------------------------- */
/*	core  (var 1)                                                   */
/* ---------------------------------------------------------------- */

static void
Core(OBJECT *op)
{
	op->disp = 0;
	core_act[op->act](op);
}

static void
Core_Init(OBJECT *op)
{
	OBJECT	*pp = W_SUB(op);

	op->tpage     = pp->tpage;
	op->anime_tbl = pp->anime_tbl;
	op->clut      = pp->clut & ~CLUT_FLASH;
	op->pri       = CORE_PRI;
	op->dmg_box   = &core_dmg;
	op->dmg_tbl   = dmg_tbl_d1000;
	op->fall_f    = 0;
	op->disp      = 0;
	op->col_box   = NULL;
	op->atk_box   = NULL;
	op->pad62     = 0;
	op->muteki    = 0;
	POS_X(op)     = CORE_X_R;
	POS_Y(op)     = CORE_Y;
	op->layer     = mega.layer;

	if (POS_X(&mega) > CORE_X_R)
		op->flip = OBJ_FLIP_R;
	else
		op->flip = OBJ_FLIP_L;

	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav  = 0;
	op->hp    = 0;
	op->atk   = 0;
	W_MUTEKI(op) = 0;

	SetAnimeBank(op, 1, ANM_CORE_APPEAR);
	PlaySound(SE_BANK_ENEMY, SE_D1000_CORE, op);
	D1000_SetFx(op, FX_EYE);

	W_COUNT(op) = 0;
	op->act2 = CORE_APPEAR;
	op->act3 = 0;
	op->act++;
}

static int
Core_WeakHit(int hit)
{
	if (IS_X())
		return ((unsigned)(hit - 19) < 4 || hit == 51 || hit == 52);
	return (hit > 0);
}

static void
Core_Main(OBJECT *op)
{
	OBJECT	*pp = W_SUB(op);
	int	spark;
	int	hit;

	if (d1000_dead) {
		PlaySound(SE_BANK_ENEMY, SE_D1000_CORE_OFF, op);
		op->act = ACT_DEAD;
		return;
	}

	TARGET_X(op) = POS_X(pp) + (pp->flip ? 62 : -62);
	TARGET_Y(op) = POS_Y(pp) - 60;
	if (POS_X(op) < TARGET_X(op))
		op->flip = OBJ_FLIP_R;
	else
		op->flip = OBJ_FLIP_L;

	spark = ((++W_COUNT(op) % CORE_SPARK) == 0);

	switch (op->act2) {
	case CORE_APPEAR:
		if (spark)
			D1000_SetFx(op, FX_SPARK);
		switch (op->act3) {
		case 0:
			if (op->anime.next == 0) {
				SetAnimeBank(op, 1, ANM_CORE_ON);
				PlaySound(SE_BANK_ENEMY, SE_D1000_CORE_ON, op);
				op->timer = 0;
				op->act3++;
			}
			break;
		case 1:
			if (++op->timer == CORE_SIGNAL) {
				W_SIGNAL(pp) = 1;
				op->act3++;
			}
			break;
		case 2:
			if (pp->act2 == MODE_THINK) {
				op->act2++;
				op->act3 = 0;
			}
			break;
		}
		AnimeTick(op);
		DisplayCheck(op, 32, 32);
		break;

	case CORE_ACTIVE:
		if (spark)
			D1000_SetFx(op, FX_SPARK);
		switch (op->act3) {
		case 0:
			if (pp->act2 == MODE_SUMMON) {
				op->act3++;
				op->timer = 0;
			}
			break;
		case 1:
			if (IS_X()) {
				if (op->timer == 0 || op->timer == TAMA_GAP_X ||
				    op->timer == TAMA_GAP_X * 2 || op->timer == TAMA_GAP_X * 3) {
					D1000_SetFx(op, FX_TAMA);
					PlaySound(SE_BANK_ENEMY, SE_D1000_TAMA, op);
				} else if (op->timer == TAMA_END_X) {
					W_SIGNAL(pp) = 1;
					op->act3--;
				}
			} else {
				if (op->timer == 0 || op->timer == TAMA_GAP_Z ||
				    op->timer == TAMA_GAP_Z * 2) {
					D1000_SetFx(op, FX_TAMA);
					PlaySound(SE_BANK_ENEMY, SE_D1000_TAMA, op);
				} else if (op->timer == TAMA_END_Z) {
					W_SIGNAL(pp) = 1;
					op->act3--;
				}
			}
			op->timer++;
			break;
		}

		op->hp = CORE_HP;
		hit = ContactWeapon(op);
		op->clut &= ~CLUT_FLASH;

		if (Core_WeakHit(hit)) {
			pp->act2 = MODE_DAMAGE;
			SetAnimeBank(op, 1, ANM_CORE_HIT);
			PlaySound(SE_BANK_ENEMY, SE_D1000_CORE, op);
			PlaySound(SE_BANK_ENEMY, SE_D1000_CORE_OFF, op);
			op->act2++;
			op->act3 = 0;
		}
		AnimeTick(op);
		DisplayCheck(op, 32, 32);
		break;

	case CORE_HIT:
		switch (op->act3) {
		case 0:
			if (op->anime.next == 0) {
				op->timer = 0;
				op->act3++;
			}
			AnimeTick(op);
			DisplayCheck(op, 32, 32);
			break;
		case 1:
			if (++op->timer == CORE_MOVE_T) {
				if (POS_X(&mega) < CORE_SIDE_X)
					POS_X(op) = CORE_X_R;
				else
					POS_X(op) = CORE_X_L;
				POS_Y(op) = CORE_Y;
				if (POS_X(op) < POS_X(&mega))
					op->flip = OBJ_FLIP_R;
				else
					op->flip = OBJ_FLIP_L;
				SetAnimeBank(op, 1, ANM_CORE_APPEAR);
				PlaySound(SE_BANK_ENEMY, SE_D1000_CORE, op);
				op->act2 = CORE_APPEAR;
				op->act3 = 0;
			}
			break;
		}
		break;
	}
}

static void
Core_Delete(OBJECT *op)
{
	DeleteObject3(op);
}

/* ---------------------------------------------------------------- */
/*	visual object 01                                                */
/* ---------------------------------------------------------------- */

void
V01_D1000Fx(OBJECT *op)
{
	if (*(long *)&op->work[0x06] == 0 && op->col_f == 0)
		CornerCheck(op);

	op->disp = 0;

	switch (op->var) {
	case FX_SPARK:	fx_spark[op->act](op);	break;
	case FX_BOMB:	fx_bomb[op->act](op);	break;
	case FX_TAMA:	fx_tama[op->act](op);	break;
	case FX_PILLAR:	fx_pillar[op->act](op);	break;
	case FX_GLOW:	fx_glow[op->act](op);	break;
	case FX_EYE:	fx_eye[op->act](op);	break;
	}
}

static int
Fx_Gone(void)
{
	return (d1000_dead || d1000_clear);
}

static void
Fx_Copy(OBJECT *op, OBJECT *pp, char pri)
{
	op->tpage     = pp->tpage;
	op->anime_tbl = pp->anime_tbl;
	op->layer     = 0;
	op->pri       = pri;
	op->clut      = pp->clut & ~CLUT_FLASH;
	op->fall_f    = 0;
	op->disp      = 0;
	op->flip      = pp->flip;
}

static void
Spark_Init(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);

	Fx_Copy(op, pp, 4);
	op->col_box = NULL;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	SetAnimeBank(op, 1, ANM_SPARK);

	POS_X(op) = POS_X(pp) + (op->flip ? 10 : -10);
	POS_Y(op) = POS_Y(pp);

	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = op->x.l;
	op->grav  = op->y.l;
	op->hp    = 0;
	op->atk   = 0;
	op->act2  = 0;
	op->act3  = 0;
	op->act++;
}

static void
Spark_Main(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);
	long	dx, dy;
	long	d2;
	short	a;

	if (Fx_Gone()) {
		op->act = 2;
		return;
	}

	op->spd_x += 8;
	a = ratan2(TARGET_Y(pp) - (op->grav >> 16), TARGET_X(pp) - (op->acc_x >> 16));

	dx = (op->acc_x >> 16) - TARGET_X(pp);
	if (dx < 0)
		dx = -dx;
	dy = (op->grav >> 16) - TARGET_Y(pp);
	if (dy < 0)
		dy = -dy;
	d2 = dx * dx + dy * dy;

	op->x.l = op->acc_x + ((rcos(a) * op->spd_x) << 4);
	op->y.l = op->grav  + ((rsin(a) * op->spd_x) << 4);

	if ((unsigned long)(op->spd_x * op->spd_x) >= (unsigned long)(d2 - 64)) {
		op->act = 2;
		return;
	}

	if (++op->act3 == 100)
		op->act = 2;

	DisplayCheck(op, 16, 16);
	AnimeTick(op);
}

static void
Bomb_Init(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);

	Fx_Copy(op, pp, 4);
	op->col_box = &bomb_col;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	SetAnimeBank(op, 1, ANM_BOMB);

	POS_X(op) = POS_X(pp) + (op->flip ? 56 : -56);
	POS_Y(op) = POS_Y(pp) - 24;

	op->grav  = 0x4000;
	op->atk   = D1000_ATK;
	op->spd_y = 0;
	op->acc_x = 0;
	op->hp    = 0;
	op->act2  = 0;
	op->spd_x = (POS_X(&mega) - POS_X(op)) << 11;
	op->act++;
}

static void
Bomb_Main(OBJECT *op)
{
	if (Fx_Gone()) {
		op->act = 2;
		return;
	}

	ContactMega(op);

	switch (op->act2) {
	case 0:
		if (op->col_f & COL_D) {
			PlaySound(SE_BANK_ENEMY, SE_D1000_BOMB, op);
			Rumble(5);
			op->spd_x = 0;
			op->spd_y = 0;
			op->grav  = 0;
			POS_Y(op) -= 78;
			SetAnimeBank(op, 1, ANM_BOMB_LAND);
			D1000_SetFx(op, FX_PILLAR);
			op->act2++;
		}
		break;
	case 1:
		if (op->anime.next == 0) {
			SetAnimeBank(op, 1, ANM_PILLAR);
			op->atk_box = &pillar_atk;
			W_COUNT2(op) = 0;
			op->act2++;
		}
		break;
	case 2:
		if (++W_COUNT2(op) == PILLAR_TIME) {
			SetAnimeBank(op, 1, ANM_PILLAR_END);
			op->atk_box = NULL;
			op->act2++;
		}
		break;
	case 3:
		if (op->anime.next == 0)
			op->act = 2;
		break;
	}

	AnimeTick(op);
	MoveObject2(op);
	DisplayCheck(op, 32, 32);
}

static void
Tama_Init(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);
	short	a;

	Fx_Copy(op, pp, 2);
	op->col_box = &tama_col;
	op->atk_box = &tama_atk;
	op->dmg_box = &tama_dmg;
	op->dmg_tbl = dmg_tbl_d1000_tama;
	SetAnimeBank(op, 1, ANM_TAMA);

	POS_X(op) = POS_X(pp) + (op->flip ? 10 : -10);
	POS_Y(op) = POS_Y(pp);

	a = ratan2(POS_Y(&mega) - POS_Y(op), POS_X(&mega) - POS_X(op));
	op->spd_x = rcos(a) << 5;
	op->spd_y = -(rsin(a) << 5);

	op->hp    = 1;
	op->acc_x = 0;
	op->grav  = 0;
	op->atk   = 4;
	op->act2  = 0;
	op->act++;
}

static void
Tama_Main(OBJECT *op)
{
	if (Fx_Gone()) {
		op->act = 2;
		return;
	}

	if (ContactWeapon(op) < 0 || op->col_f) {
		SpawnExplosion(op);
		op->act = 2;
		return;
	}

	ContactMega(op);
	AnimeTick(op);
	MoveObject2(op);
	DisplayCheck(op, 16, 16);
}

static void
Pillar_Init(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);

	Fx_Copy(op, pp, 3);
	op->col_box = NULL;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	SetAnimeBank(op, 1, ANM_PILLAR_FX);

	POS_X(op) = POS_X(pp);
	POS_Y(op) = POS_Y(pp) + 92;
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav  = 0;
	op->hp    = 0;
	op->atk   = 0;
	op->act2  = 0;
	op->act++;
}

static void
Pillar_Main(OBJECT *op)
{
	if (Fx_Gone() || PARENT(op)->act == 2) {
		op->act = 2;
		return;
	}

	AnimeTick(op);
	MoveObject2(op);
	DisplayCheck(op, 48, 32);
}

static void
Glow_Init(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);

	Fx_Copy(op, pp, 6);
	op->col_box = NULL;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	SetAnimeBank(op, 1, ANM_GLOW);

	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav  = 0;
	op->hp    = 0;
	op->atk   = 0;
	op->act2  = 0;
	op->act++;
}

static void
Glow_Main(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);

	if (d1000_dead) {
		op->act = 2;
		return;
	}

	op->flip  = pp->flip;
	POS_X(op) = POS_X(pp) + (op->flip ? -8 : 8);
	POS_Y(op) = POS_Y(pp) - 8;

	if (pp->act2 != MODE_STUN || pp->act3 >= 4)
		DisplayCheck(op, 64, 64);

	AnimeTick(op);
	MoveObject2(op);
}

static void
Eye_Init(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);

	Fx_Copy(op, pp, 2);
	op->col_box = NULL;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	SetAnimeBank(op, 1, ANM_EYE);

	POS_X(op) = POS_X(pp);
	POS_Y(op) = POS_Y(pp);
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav  = 0;
	op->hp    = 0;
	op->atk   = 0;
	op->act2  = 0;
	op->act++;
}

static void
Eye_Main(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);

	op->flip  = pp->flip;
	POS_X(op) = POS_X(pp);
	POS_Y(op) = POS_Y(pp);

	if (d1000_dead) {
		op->act = 2;
		return;
	}

	if (pp->anime_no == ANM_CORE_ON)
		DisplayCheck(op, 64, 64);

	AnimeTick(op);
	MoveObject2(op);
}

static void
Fx_Delete(OBJECT *op)
{
	DeleteObject3(op);
}
