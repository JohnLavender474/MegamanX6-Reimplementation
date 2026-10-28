/*
 *	m49_yammark.c
 *
 *	ST03 (Commander Yammark)  main object 49 : Commander Yammark  (var 0 : boss, var 1 : option)
 *	                          visual object 15 : options' shots and the boss effects (var 0 - 6)
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
	MODE_IDLE,
	MODE_THINK,
	MODE_SWARM,
	MODE_LANCE,
	MODE_DASH,
	MODE_WALLDASH,
	MODE_CHASE,
	MODE_STUN
};

enum {
	CMD_NONE = 0,
	CMD_GATHER,
	CMD_RING,
	CMD_SWARM,
	CMD_LANCE,
	CMD_WALL,
	CMD_TRAIL
};

enum {
	OPT_ENTRY = 0,
	OPT_SUMMON,
	OPT_GATHER,
	OPT_RING,
	OPT_SWARM,
	OPT_LANCE,
	OPT_WALL,
	OPT_TRAIL,
	OPT_FREEZE = 99
};

enum {
	FX_SHOT = 0,
	FX_SHOT_RING,
	FX_BOMB,
	FX_BURST,
	FX_WING,
	FX_FLASH,
	FX_TRAIL
};

enum {
	ANM_HOVER = 0,
	ANM_POSE,
	ANM_FLY,
	ANM_FLY_UP,
	ANM_FLY_DOWN,
	ANM_SUMMON,
	ANM_UNUSED6,
	ANM_DEAD,
	ANM_STUN,
	ANM_OPT_APPEAR,
	ANM_OPT_ATTACK,
	ANM_OPT_FLY,
	ANM_OPT_BOMB,
	ANM_OPT_DASH,
	ANM_WING,
	ANM_FLASH,
	ANM_UNUSED16,
	ANM_SHOT,
	ANM_BOMB,
	ANM_TRAIL,
	ANM_BURST,
	ANM_JUNK0,
	ANM_JUNK1
};

#define STAGE_REMATCH		12

#define YMK_HP			32
#define YMK_ATK			4
#define YMK_PRI			5
#define YMK_TPAGE		480
#define YMK_CLUT_REMATCH	0x7948

#define OPT_HP			8
#define OPT_ATK			2
#define OPT_PRI			4
#define OPT_RADIUS_SHIFT	7
#define OPT_SPIN		546
#define OPT_TRAIL_LEN		210

#define FLY_SHIFT		5
#define MUTEKI_TIME		30
#define MUTEKI_SABER		2
#define STUN_TIME		90
#define KNOCK_TIME		30
#define KNOCK_DAMAGE		3
#define SWARM_TIME		500
#define LANCE_TIME		600
#define DASH_TRACK		120
#define WALL_WAIT		260
#define WALL_PAUSE		50
#define WALL_TURNS		3
#define CHASE_TIME		500
#define LANCE_GAP		75
#define SOUL_TIME		5401

#define DASH_SPD		0x20000
#define WALL_SPD		0x28000
#define DESCEND_SPD		0x10000

#define W_TIME(o)		((o)->timer)
#define W_COUNT(o)		(*(short *)&(o)->work[0x00])
#define W_SUB(o)		(*(OBJECT **)&(o)->work[0x02])
#define W_MUTEKI(o)		((o)->work[0x06])
#define W_PATTERN(o)		((o)->work[0x08])
#define W_PAT_I(o)		((o)->work[0x09])
#define W_SLOT(o)		(*(short *)&(o)->work[0x0a])
#define W_ANGLE(o)		(*(u_short *)&(o)->work[0x0c])
#define W_8C(o)			(*(short *)&(o)->work[0x0e])
#define W_8E(o)			(*(short *)&(o)->work[0x10])
#define W_SWING(o)		((o)->act4)

#define PARENT(o)		(*(OBJECT **)&(o)->timer)
#define FX_NOHIT(o)		(*(long *)&(o)->work[0x06])
#define FX_DELAY(o)		((o)->act4)
#define FX_COUNT(o)		((o)->act3)

#define IS_ZERO()		(mega.var == 1)

/* ---------------------------------------------------------------- */

typedef struct {
	short	count;
	short	stay;
} OPTPAIR;

typedef struct {
	short	x;
	short	y;
	short	flip;
	short	pad;
} TRAIL;

static short	pt_left, pt_right, pt_floor, pt_spawn_x, pt_spawn_y;
static short	pt_side_l, pt_side_r, pt_high, pt_low;
static short	pt_entry_x[3], pt_entry_y;
static short	pt_col_hi, pt_col_lo, pt_col_mid;

long	ymk_time;
long	ymk_dead;
long	ymk_stun;
OBJECT	*ymk_body;
OPTPAIR	*ymk_pair;
OPTPAIR	ymk_pairs[3];
short	opt_ready;
short	opt_cmd;
short	opt_alive;
short	opt_slot[6];
long	opt_knock;
TRAIL	ymk_trail[OPT_TRAIL_LEN];

static u_char	hp_bonus_unused[4] = { 0, 1, 1, 2 };
static u_char	hp_bonus[4]        = { 0, 6, 12, 16 };

static OPTPAIR	pair_init[3] = { { 3, 1 }, { 4, 2 }, { 6, 3 } };

static u_char	pattern_tbl[6][6] = {
	{ MODE_SWARM, MODE_DASH, MODE_THINK, MODE_THINK, MODE_THINK, MODE_THINK },
	{ MODE_LANCE, MODE_DASH, MODE_THINK, MODE_THINK, MODE_THINK, MODE_THINK },
	{ MODE_SWARM, MODE_DASH, MODE_DASH,  MODE_THINK, MODE_THINK, MODE_THINK },
	{ MODE_LANCE, MODE_DASH, MODE_DASH,  MODE_THINK, MODE_THINK, MODE_THINK },
	{ MODE_CHASE, MODE_WALLDASH, MODE_THINK, MODE_THINK, MODE_THINK, MODE_THINK },
	{ MODE_CHASE, MODE_DASH, MODE_DASH,  MODE_THINK, MODE_THINK, MODE_THINK },
};

static u_short	opt_angle[6][8] = {
	{ 0x0000, 0x5555, 0xaaaa },
	{ 0x0000, 0x4000, 0x8000, 0xc000 },
	{ 0x0000, 0x3333, 0x6666, 0x9999, 0xcccc },
	{ 0x0000, 0x2aaa, 0x5555, 0x8000, 0xaaaa, 0xd555 },
	{ 0x0000, 0x248d, 0x491a, 0x6da7, 0x9234, 0xb6c1, 0xdb4e },
	{ 0x0000, 0x2000, 0x4000, 0x6000, 0x8000, 0xa000, 0xc000, 0xe000 },
};

static short	opt_shot_rate[8] = { 30, 25, 45, 35, 25, 20, 35, 40 };
static short	opt_wall_dy[8]   = { -40, 40, -60, 60, -20, 20, -80, 80 };

static HITBOX	ymk_col  = {  0,   8, 16, 32 };
static HITBOX	ymk_atk  = { -8, -16, 24, 48 };
static HITBOX	ymk_dmg  = { -8, -16, 24, 48 };
static HITBOX	opt_col  = {  0,   0, 16,  8 };
static HITBOX	opt_atk  = { -16, -6, 32, 12 };
static HITBOX	opt_dmg  = { -16, -6, 32, 12 };
static HITBOX	shot_col = {  0,   0,  7,  7 };
static HITBOX	shot_atk = { -7,  -7, 14, 14 };
static HITBOX	shot_dmg = { -7,  -7, 14, 14 };
static HITBOX	bomb_col = {  0,   0,  2,  6 };
static HITBOX	bomb_atk = { -2,  -6,  4, 12 };
static HITBOX	burst_atk = { -8, -8, 16, 16 };

static u_char	opt_junk[2] = { ANM_JUNK0, ANM_JUNK1 };

extern ANIME	*ymk_anime[];		/* 0x800F7B44, 23 animations */

/* ---------------------------------------------------------------- */

static void	Body_Init(OBJECT *op);
static void	Body_Main(OBJECT *op);
static void	Body_Think(OBJECT *op);
static void	Swarm_GoSide(OBJECT *op);
static void	Swarm_Summon(OBJECT *op);
static void	Lance_GoSide(OBJECT *op);
static void	Lance_Summon(OBJECT *op);
static void	Lance_Wait(OBJECT *op);
static void	Dash_GoSide(OBJECT *op);
static void	Dash_Summon(OBJECT *op);
static void	Dash_Track(OBJECT *op);
static void	Wall_GoSide(OBJECT *op);
static void	Wall_Summon(OBJECT *op);
static void	Chase_GoSide(OBJECT *op);
static void	Chase_Summon(OBJECT *op);
static void	Chase_Run(OBJECT *op);
static void	Dead_End(OBJECT *op);
static void	Opt_Init(OBJECT *op);
static void	Opt_Main(OBJECT *op);
void		V15_YammarkFx(OBJECT *op);
static void	Shot_Init(OBJECT *op);
static void	Bomb_Main(OBJECT *op);
static void	Wing_Main(OBJECT *op);
void		M49_Yammark(OBJECT *op);
static void	Body_Nop(OBJECT *op);
static void	Body_Entry(OBJECT *op);
static void	Entry_WaitMega(OBJECT *op);
static void	Entry_Warning(OBJECT *op);
static void	Entry_Descend(OBJECT *op);
static void	Entry_Talk(OBJECT *op);
static void	Entry_Pose(OBJECT *op);
static void	Entry_Bar(OBJECT *op);
static void	Entry_FillHp(OBJECT *op);
static void	Body_Idle(OBJECT *op);
static void	Idle_Sub(OBJECT *op);
static void	Body_Swarm(OBJECT *op);
static void	Swarm_Phase(OBJECT *op);
static void	Swarm_Gather(OBJECT *op);
static void	Swarm_Ready(OBJECT *op);
static void	Swarm_Go(OBJECT *op);
static void	Swarm_Wait(OBJECT *op);
static void	Body_Lance(OBJECT *op);
static void	Lance_Phase(OBJECT *op);
static void	Lance_Gather(OBJECT *op);
static void	Lance_Ready(OBJECT *op);
static void	Lance_Go(OBJECT *op);
static void	Body_Dash(OBJECT *op);
static void	Dash_Phase(OBJECT *op);
static void	Dash_Gather(OBJECT *op);
static void	Dash_Ready(OBJECT *op);
static void	Dash_Wall(OBJECT *op);
static void	Body_WallDash(OBJECT *op);
static void	Wall_Phase(OBJECT *op);
static void	Wall_Gather(OBJECT *op);
static void	Wall_Ready(OBJECT *op);
static void	Wall_Wait(OBJECT *op);
static void	Wall_Run(OBJECT *op);
static void	Wall_Pause(OBJECT *op);
static void	Body_Chase(OBJECT *op);
static void	Chase_Phase(OBJECT *op);
static void	Chase_Gather(OBJECT *op);
static void	Chase_Ready(OBJECT *op);
static void	Body_Stun(OBJECT *op);
static void	Stun_Start(OBJECT *op);
static void	Stun_Wait(OBJECT *op);
static void	Body_Dead(OBJECT *op);
static void	Dead_Start(OBJECT *op);
static void	Dead_Blink(OBJECT *op);
static void	Option(OBJECT *op);
static void	Opt_Delete(OBJECT *op);
static void	Shot_Main(OBJECT *op);
static void	Fx_Delete(OBJECT *op);
static void	Bomb_Init(OBJECT *op);
static void	Burst_Init(OBJECT *op);
static void	Burst_Main(OBJECT *op);
static void	Wing_Init(OBJECT *op);
static void	Flash_Init(OBJECT *op);
static void	Flash_Main(OBJECT *op);
static void	Trail_Init(OBJECT *op);
static void	Trail_Main(OBJECT *op);

static void	(*body_act[])(OBJECT *) = { Body_Init, Body_Main, Body_Dead };

static void	(*body_mode[])(OBJECT *) = {
	ReactMode, Body_Nop, Body_Entry, Body_Idle, Body_Think, Body_Swarm,
	Body_Lance, Body_Dash, Body_WallDash, Body_Chase, Body_Stun
};

static void	(*entry_tbl[])(OBJECT *) = {
	Entry_WaitMega, Entry_Warning, Entry_Descend, Entry_Talk, Entry_Pose, Entry_Bar, Entry_FillHp
};
static void	(*idle_tbl[])(OBJECT *)  = { Idle_Sub };
static void	(*swarm_tbl[])(OBJECT *) = {
	Swarm_Phase, Swarm_Gather, Swarm_GoSide, Swarm_Summon, Swarm_Ready, Swarm_Go, Swarm_Wait
};
static void	(*lance_tbl[])(OBJECT *) = {
	Lance_Phase, Lance_Gather, Lance_GoSide, Lance_Summon, Lance_Ready, Lance_Go, Lance_Wait
};
static void	(*dash_tbl[])(OBJECT *)  = {
	Dash_Phase, Dash_Gather, Dash_GoSide, Dash_Summon, Dash_Ready, Dash_Track, Dash_Wall
};
static void	(*wall_tbl[])(OBJECT *)  = {
	Wall_Phase, Wall_Gather, Wall_GoSide, Wall_Summon, Wall_Ready, Wall_Wait, Wall_Run, Wall_Pause
};
static void	(*chase_tbl[])(OBJECT *) = {
	Chase_Phase, Chase_Gather, Chase_GoSide, Chase_Summon, Chase_Ready, Chase_Run
};
static void	(*stun_tbl[])(OBJECT *)  = { Stun_Start, Stun_Wait };
static void	(*dead_tbl[])(OBJECT *)  = { Dead_Start, Dead_Blink, Dead_End };

static void	(*opt_act[])(OBJECT *)   = { Opt_Init, Opt_Main, Opt_Delete };

static void	(*fx_shot[])(OBJECT *)   = { Shot_Init,  Shot_Main,  Fx_Delete };
static void	(*fx_bomb[])(OBJECT *)   = { Bomb_Init,  Bomb_Main,  Fx_Delete };
static void	(*fx_burst[])(OBJECT *)  = { Burst_Init, Burst_Main, Fx_Delete };
static void	(*fx_wing[])(OBJECT *)   = { Wing_Init,  Wing_Main,  Fx_Delete };
static void	(*fx_flash[])(OBJECT *)  = { Flash_Init, Flash_Main, Fx_Delete };
static void	(*fx_trail[])(OBJECT *)  = { Trail_Init, Trail_Main, Fx_Delete };

/* ---------------------------------------------------------------- */

static void
Ymk_Move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject2(op);
	DisplayCheck(op, 64, 64);
}

static void
Ymk_Phase(OBJECT *op)
{
	if (op->hp < game.boss_hp / 3)
		ymk_pair = &ymk_pairs[2];
	else if (op->hp < game.boss_hp * 2 / 3)
		ymk_pair = &ymk_pairs[1];
}

static void
Ymk_NextMode(OBJECT *op)
{
	op->act2 = pattern_tbl[W_PATTERN(op)][W_PAT_I(op)++];
	op->act3 = 0;
}

static void
Ymk_FlyTo(OBJECT *op, short tx, short ty)
{
	short	a;

	if (POS_X(op) == tx && POS_Y(op) == ty) {
		op->spd_x = 0;
		op->spd_y = 0;
	} else {
		a = ratan2(ty - POS_Y(op), tx - POS_X(op));
		op->spd_x = rcos(a) << FLY_SHIFT;
		op->spd_y = -rsin(a) << FLY_SHIFT;
	}
	if (op->spd_x != 0)
		SetAnime(op, ANM_FLY);
	else if (op->spd_y > 0)
		SetAnime(op, ANM_FLY_UP);
	else if (op->spd_y < 0)
		SetAnime(op, ANM_FLY_DOWN);
}

static short
Ymk_SideX(OBJECT *op)
{
	if (POS_X(op) < (pt_left + pt_right) / 2) {
		op->flip = OBJ_FLIP_R;
		return pt_side_l;
	}
	op->flip = OBJ_FLIP_L;
	return pt_side_r;
}

static int
Ymk_Arrive(OBJECT *op)
{
	short	tx, ty;
	int	i;
	OBJECT	*tp;

	tx = op->flip ? pt_side_l : pt_side_r;
	if (op->spd_x < 0 && POS_X(op) < tx) {
		POS_X(op) = tx;
		op->spd_x = 0;
	} else if (op->spd_x > 0 && tx < POS_X(op)) {
		POS_X(op) = tx;
		op->spd_x = 0;
	}
	ty = W_COUNT(op) ? pt_low : pt_high;
	if (op->spd_y > 0 && POS_Y(op) < ty) {
		POS_Y(op) = ty;
		op->spd_y = 0;
	} else if (op->spd_y < 0 && ty < POS_Y(op)) {
		POS_Y(op) = ty;
		op->spd_y = 0;
	}
	if (op->spd_x != 0 || op->spd_y != 0)
		return 0;

	opt_cmd   = CMD_NONE;
	opt_ready = ymk_pair->count - opt_alive;
	for (i = 0; i < opt_ready; i++) {
		if ((tp = GetMainObject()) != NULL) {
			tp->flag  = op->flag;
			tp->var   = 1;
			tp->act2  = OPT_SUMMON;
			W_SUB(tp) = op;
			tp->type  = op->type;
		}
	}
	if (opt_ready != 0) {
		SetAnime(op, ANM_SUMMON);
		PlaySound(SE_BANK_ENEMY, SE_YMK_SUMMON, op);
		if ((tp = GetVisualObject()) != NULL) {
			tp->type   = 15;
			tp->var    = FX_FLASH;
			PARENT(tp) = op;
			tp->flag   = op->flag;
		}
	}
	op->act3++;
	return 1;
}

static void
Ymk_Kill(OBJECT *op)
{
	op->act  = ACT_DEAD;
	op->act2 = 0;
	op->act3 = 0;
	op->clut &= ~CLUT_FLASH;
	ymk_dead = 1;
	((u_char *)&mega)[0x7a] = 1;
	DisplayCheck(op, 64, 64);
}

static void
Ymk_Boxes(OBJECT *op)
{
	op->atk_box = &ymk_atk;
	op->dmg_tbl = dmg_tbl_yammark;
}

/* ---------------------------------------------------------------- */

static void
Body_Init(OBJECT *op)
{
	u_char	unused[4];
	u_char	bonus[4];
	short	x0, y0;
	int	i;

	*(long *)unused = *(long *)hp_bonus_unused;
	*(long *)bonus  = *(long *)hp_bonus;

	x0 = bg_layer[0].dest_min_x;
	y0 = bg_layer[0].dest_min_y;
	pt_left     = x0 + 16;
	pt_right    = x0 + 336;
	pt_floor    = y0 + 192;
	pt_spawn_x  = x0 + 240;
	pt_spawn_y  = y0 - 32;
	pt_side_l   = x0 + 48;
	pt_side_r   = x0 + 304;
	pt_high     = y0 + 96;
	pt_low      = y0 + 150;
	pt_entry_x[0] = x0 + 32;
	pt_entry_x[1] = x0 + 176;
	pt_entry_x[2] = x0 + 320;
	pt_entry_y  = y0 - 32;
	pt_col_hi   = y0 + 48;
	pt_col_lo   = y0 + 144;
	pt_col_mid  = y0 + 96;

	if (game.stage == STAGE_REMATCH) {
		op->clut   = YMK_CLUT_REMATCH;
		op->sprt_p = (char *)stage_sprt_bank + stage_sprt_bank->ofs[0];
	} else {
		i = GetSpriteBank(op->type);
		op->sprt_p = (char *)sprt_bank + sprt_bank[i].ofs[0];
	}
	op->tpage     = YMK_TPAGE;
	op->anime_tbl = ymk_anime;
	op->layer     = mega.layer;
	op->pri       = YMK_PRI;
	op->atk_box   = &ymk_atk;
	op->flip      = OBJ_FLIP_L;
	op->fall_f    = 0;
	op->disp      = 0;
	op->dmg_box   = &ymk_dmg;
	op->pad62     = 0;
	op->muteki    = 0;
	op->col_box   = NULL;
	op->dmg_tbl   = dmg_tbl_yammark;
	op->atk       = YMK_ATK;
	op->spd_x     = 0;
	op->spd_y     = 0;
	op->acc_x     = 0;
	op->grav      = 0;
	op->hp        = 0;
	POS_X(op)     = pt_spawn_x;
	POS_Y(op)     = pt_spawn_y;

	game.boss_f   = 0;
	game.boss     = op;
	game.boss_hp  = YMK_HP;
	BossLevel();
	game.boss_hp += bonus[game.boss_level];

	W_PAT_I(op)   = 0;
	W_PATTERN(op) = 0;
	W_MUTEKI(op)  = 0;
	PlayerEvent(20, OBJ_FLIP_R);

	ymk_time  = 0;
	ymk_dead  = 0;
	ymk_stun  = 0;
	ymk_body  = op;
	opt_alive = 0;
	for (i = 5; i >= 0; i--)
		opt_slot[i] = 0;
	ymk_pair  = ymk_pairs;
	opt_ready = 0;
	opt_cmd   = CMD_NONE;
	opt_knock = 0;
	for (i = 0; i < 3; i++)
		ymk_pairs[i] = pair_init[i];
	if (game.level == LEVEL_HARD) {
		for (i = 0; i < 3; i++) {
			ymk_pairs[i].stay -= 2;
			if (ymk_pairs[i].stay < 0)
				ymk_pairs[i].stay = 0;
		}
	}

	op->act2 = MODE_ENTRY;
	op->act++;
}

static void
Body_Main(OBJECT *op)
{
	int	r;

	r = ContactWeapon(op);
	if (r < 0) {
		Ymk_Kill(op);
		return;
	}
	ContactMega(op);

	if (r > 0 && r != 127) {
		W_MUTEKI(op) = (r == 25 || r == 26) ? MUTEKI_SABER : MUTEKI_TIME;
		op->dmg_tbl  = dmg_tbl_guard;
	}
	if ((r == 9 || r == 18 || r == 45) && opt_knock == 0) {
		ymk_stun = 1;
		op->act2 = MODE_STUN;
		op->act3 = 0;
	}
	if (W_MUTEKI(op) != 0) {
		W_MUTEKI(op)--;
		if (W_MUTEKI(op) & 2)
			op->clut |= CLUT_FLASH;
		else
			op->clut &= ~CLUT_FLASH;
		if (W_MUTEKI(op) == 0) {
			op->dmg_tbl = dmg_tbl_yammark;
			op->clut &= ~CLUT_FLASH;
		}
	}

	body_mode[op->act2](op);

	if (op->act2 != MODE_ENTRY)
		ymk_time++;
}

static void
Body_Think(OBJECT *op)
{
	int	r;
	int	high;

	Ymk_Phase(op);
	r = GetRNG() & 15;
	W_PAT_I(op) = 0;
	high = (game.boss_hp / 2 < op->hp);

	if (mega.var == 0) {
		if (high)
			W_PATTERN(op) = (r < 8) ? 0 : 2;
		else if (r < 4)
			W_PATTERN(op) = 0;
		else if (r < 12)
			W_PATTERN(op) = 4;
		else
			W_PATTERN(op) = 5;
	} else {
		if (high)
			W_PATTERN(op) = (r < 8) ? 1 : 3;
		else if (r < 4)
			W_PATTERN(op) = 1;
		else if (r < 12)
			W_PATTERN(op) = 4;
		else
			W_PATTERN(op) = 5;
	}
	op->act3 = 0;
	op->act2 = pattern_tbl[W_PATTERN(op)][W_PAT_I(op)++];
	Ymk_Move(op);
}

static void
Swarm_GoSide(OBJECT *op)
{
	short	tx, ty;

	tx = Ymk_SideX(op);
	if (GetRNG() & 1) {
		ty = pt_high;
		W_COUNT(op) = 0;
	} else {
		ty = pt_low;
		W_COUNT(op) = 1;
	}
	Ymk_FlyTo(op, tx, ty);
	op->act3++;
}

static void
Swarm_Summon(OBJECT *op)
{
	Ymk_Arrive(op);
}

static void
Lance_GoSide(OBJECT *op)
{
	short	tx, ty;

	tx = Ymk_SideX(op);
	if (GetRNG() & 1) {
		ty = pt_high;
		W_COUNT(op) = 0;
	} else {
		ty = pt_low;
		W_COUNT(op) = 1;
	}
	Ymk_FlyTo(op, tx, ty);
	op->act3++;
}

static void
Lance_Summon(OBJECT *op)
{
	Ymk_Arrive(op);
}

static void
Lance_Wait(OBJECT *op)
{
	if (W_TIME(op) == 120)
		PlaySound(SE_BANK_ENEMY, SE_YMK_LANCE, op);

	if (W_8E(op) == 0) {
		W_SWING(op) += 2;
		if (W_SWING(op) >= 30)
			W_8E(op) = 1;
	} else {
		W_SWING(op) -= 2;
		if (W_SWING(op) < -29)
			W_8E(op) = 0;
	}

	if (opt_knock == KNOCK_TIME) {
		op->hp -= KNOCK_DAMAGE;
		if ((char)op->hp <= 0) {
			op->hp = 0;
			Ymk_Kill(op);
			return;
		}
		PlaySound(SE_BANK_ENEMY, SE_YMK_STUN, op);
		SetAnime(op, ANM_STUN);
		op->atk_box = NULL;
		op->dmg_tbl = dmg_tbl_guard;
	} else if (opt_knock == 1) {
		SetAnime(op, ANM_HOVER);
		Ymk_Boxes(op);
	}
	if (opt_knock != 0)
		opt_knock--;

	W_COUNT(op)++;
	if (++W_TIME(op) == LANCE_TIME) {
		SetAnime(op, ANM_HOVER);
		Ymk_Boxes(op);
		opt_knock = 0;
		Ymk_NextMode(op);
	}
}

static void
Dash_GoSide(OBJECT *op)
{
	short	tx;

	tx = Ymk_SideX(op);
	W_COUNT(op) = 0;
	Ymk_FlyTo(op, tx, pt_high);
	op->act3++;
}

static void
Dash_Summon(OBJECT *op)
{
	Ymk_Arrive(op);
}

static void
Dash_Track(OBJECT *op)
{
	short	my;

	op->flip = (POS_X(op) < POS_X(&mega)) ? OBJ_FLIP_R : OBJ_FLIP_L;
	if (W_TIME(op) == 90)
		PlaySound(SE_BANK_ENEMY, SE_YMK_DASH, op);

	my = POS_Y(&mega);
	if (POS_Y(op) < my) {
		POS_Y(op) += 2;
		if (my < POS_Y(op))
			POS_Y(op) = my;
	} else if (my < POS_Y(op)) {
		POS_Y(op) -= 2;
		if (POS_Y(op) < my)
			POS_Y(op) = my;
	}

	if (++W_TIME(op) == DASH_TRACK) {
		op->spd_x = (POS_X(op) < POS_X(&mega)) ? DASH_SPD : -DASH_SPD;
		op->act3++;
	}
}

static void
Wall_GoSide(OBJECT *op)
{
	short	tx;

	tx = Ymk_SideX(op);
	W_COUNT(op) = 0;
	Ymk_FlyTo(op, tx, pt_high);
	op->act3++;
}

static void
Wall_Summon(OBJECT *op)
{
	Ymk_Arrive(op);
}

static void
Chase_GoSide(OBJECT *op)
{
	short	tx;

	tx = Ymk_SideX(op);
	W_COUNT(op) = 1;
	Ymk_FlyTo(op, tx, pt_low);
	op->act3++;
}

static void
Chase_Summon(OBJECT *op)
{
	Ymk_Arrive(op);
}

static void
Chase_Run(OBJECT *op)
{
	short	a;
	TRAIL	*tp;

	if (W_TIME(op) == 90)
		PlaySound(SE_BANK_ENEMY, SE_YMK_CHASE, op);

	if ((W_TIME(op) & 7) == 0) {
		op->flip = (POS_X(op) < POS_X(&mega)) ? OBJ_FLIP_R : OBJ_FLIP_L;
		a = ratan2(POS_Y(&mega) - POS_Y(op), POS_X(&mega) - POS_X(op));
		op->spd_x = rcos(a) << FLY_SHIFT;
		op->spd_y = -rsin(a) << FLY_SHIFT;
	}

	tp = &ymk_trail[W_8E(op)];
	tp->x    = POS_X(op);
	tp->y    = POS_Y(op);
	tp->flip = op->flip;
	if (++W_8E(op) == OPT_TRAIL_LEN)
		W_8E(op) = 0;

	if (++W_TIME(op) == CHASE_TIME || opt_alive == 0) {
		SetAnime(op, ANM_HOVER);
		op->spd_x = 0;
		op->spd_y = 0;
		Ymk_NextMode(op);
	}
}

static void
Dead_End(OBJECT *op)
{
	OBJECT	*tp;

	tp = W_SUB(op);
	if (tp->flag != 0) {
		if (tp->act4 == 0) {
			if (--W_COUNT(op) == -1) {
				W_COUNT(op) = 5;
				op->clut ^= CLUT_FLASH;
			}
			DisplayCheck(op, 64, 64);
		}
		return;
	}

	if (game.stage == STAGE_REMATCH) {
		game.message = 0x80;
		((char *)&game)[0x2c + game.checkpoint] = 1;
		game.checkpoint += 8;
	} else if ((tp = GetMainObject()) != NULL) {
		tp->flag = 0x41;
		tp->type = 15;
		tp->var  = ((u_long)ymk_time < SOUL_TIME) ? 0x10 : 0x11;
	}
	DeleteObject3(op);
}

static void
Opt_Init(OBJECT *op)
{
	OBJECT	*bp;
	int	i;

	bp = ymk_body;
	op->tpage     = bp->tpage;
	op->anime_tbl = bp->anime_tbl;
	op->sprt_p    = bp->sprt_p;
	op->clut      = bp->clut & ~CLUT_FLASH;
	op->pri       = OPT_PRI;
	op->fall_f    = 0;
	op->disp      = 0;
	op->col_box   = NULL;
	op->atk_box   = NULL;
	op->dmg_box   = NULL;
	op->dmg_tbl   = dmg_tbl_yammark;
	op->pad62     = 0;
	op->muteki    = 0;
	op->layer     = mega.layer;

	for (i = 0; i < ymk_pair->count; i++) {
		if (opt_slot[i] == 0) {
			opt_slot[i] = 1;
			W_SLOT(op)  = i;
			break;
		}
	}
	opt_alive++;

	op->hp       = OPT_HP;
	op->spd_x    = 0;
	op->spd_y    = 0;
	op->acc_x    = 0;
	op->grav     = 0;
	op->atk      = OPT_ATK;
	W_MUTEKI(op) = 0;
	W_COUNT(op)  = 0;
	op->act3     = 0;
	op->act++;
}

/*
 *	the option on the ring around (cx, cy), 32 px out
 */
static void
Opt_Ring(OBJECT *op, short cx, short cy)
{
	short	a;

	a = (short)W_ANGLE(op) >> 4;
	POS_X(op) = cx + rcos(a) / 128;
	POS_Y(op) = cy + (-rsin(a)) / 128;
}

static void
Opt_Spin(OBJECT *op)
{
	if (ymk_body->flip == OBJ_FLIP_L)
		W_ANGLE(op) += OPT_SPIN;
	else
		W_ANGLE(op) -= OPT_SPIN;
	Opt_Ring(op, POS_X(ymk_body), POS_Y(ymk_body));
}

/*
 *	turn the ring angle toward a and count the stops in +0x8E
 */
static void
Opt_TurnTo(OBJECT *op, short a)
{
	short	d;

	if (ymk_body->flip == OBJ_FLIP_L) {
		d = a - (short)W_ANGLE(op);
		if (d > 0) {
			W_ANGLE(op) += OPT_SPIN;
			d = a - (short)W_ANGLE(op);
			if (d <= 0) {
				W_ANGLE(op) = a;
				W_8E(op)++;
			}
		} else {
			W_ANGLE(op) += OPT_SPIN;
		}
	} else {
		d = a - (short)W_ANGLE(op);
		if (d < 0) {
			W_ANGLE(op) -= OPT_SPIN;
			d = a - (short)W_ANGLE(op);
			if (d >= 0) {
				W_ANGLE(op) = a;
				W_8E(op)++;
			}
		} else {
			W_ANGLE(op) -= OPT_SPIN;
		}
	}
	Opt_Ring(op, POS_X(ymk_body), POS_Y(ymk_body));
}

static void
Opt_Arm(OBJECT *op, int col)
{
	op->col_box = col ? &opt_col : NULL;
	op->atk_box = &opt_atk;
	op->dmg_box = &opt_dmg;
	SetAnime(op, ANM_OPT_ATTACK);
}

static int
Opt_Step(short *p, short t, short d)
{
	if (*p < t) {
		*p += d;
		if (t < *p)
			*p = t;
	} else if (t < *p) {
		*p -= d;
		if (*p < t)
			*p = t;
	}
	return (*p == t);
}

static OBJECT *
Opt_Fx(OBJECT *op, int var)
{
	OBJECT	*tp;

	if ((tp = GetVisualObject()) != NULL) {
		tp->type   = 15;
		tp->var    = var;
		PARENT(tp) = op;
		tp->flag   = op->flag;
	}
	return tp;
}

static void
Opt_Main(OBJECT *op)
{
	OBJECT	*bp;
	OPTPAIR	*pp;
	int	r;
	short	x, y, tx, ty, a;
	TRAIL	*tp;

	bp = ymk_body;
	pp = ymk_pair;

	if (ymk_dead) {
		op->act = ACT_DEAD;
		return;
	}

	r = ContactWeapon(op);
	if (r < 0 && !(op->act2 == OPT_LANCE && op->act3 == 5 && IS_ZERO())) {
		op->clut &= ~CLUT_FLASH;
		DisplayCheck(op, 32, 16);
		SpawnExplosion(op);
		SpawnJunk(2, opt_junk, op);
		op->act++;
		return;
	}
	ContactMega(op);
	W_COUNT(op)++;

	if (ymk_stun) {
		if (op->act2 == OPT_SUMMON) {
			opt_slot[W_SLOT(op)] = 0;
			opt_alive--;
			DeleteObject3(op);
			return;
		}
		opt_cmd = CMD_NONE;
		if (W_SLOT(op) >= 0)
			op->act2 = OPT_FREEZE;
		goto freeze;
	}

	switch (opt_cmd) {
	case CMD_GATHER:
		if (op->act2 != OPT_GATHER && W_SLOT(op) >= 0) {
			op->spd_x = 0;
			op->spd_y = 0;
			op->act2  = OPT_GATHER;
			op->act3  = 0;
		}
		break;
	case CMD_RING:
		if (op->act2 != OPT_RING && W_SLOT(op) >= 0) {
			op->act2 = OPT_RING;
			op->act3 = 0;
		}
		break;
	case CMD_SWARM:
	case CMD_LANCE:
		a = (opt_cmd == CMD_SWARM) ? OPT_SWARM : OPT_LANCE;
		if (op->act2 != a && W_SLOT(op) >= 0 && W_SLOT(op) < pp->count - pp->stay) {
			op->act2 = a;
			op->act3 = 0;
		}
		break;
	case CMD_WALL:
		if (op->act2 != OPT_WALL && W_SLOT(op) >= 0) {
			op->act2 = OPT_WALL;
			op->act3 = 0;
		}
		break;
	case CMD_TRAIL:
		if (op->act2 != OPT_TRAIL && W_SLOT(op) >= 0) {
			op->flip  = bp->flip;
			W_8C(op)  = POS_X(bp);
			op->act2  = OPT_TRAIL;
			op->act3  = 0;
			W_8E(op)  = POS_Y(bp);
		}
		break;
	}

	switch (op->act2) {
	case OPT_ENTRY:
		switch (op->act3) {
		case 0:
			switch (W_SLOT(op)) {
			case 0: POS_X(op) = pt_entry_x[0]; break;
			case 1: POS_X(op) = pt_entry_x[1]; break;
			case 2: POS_X(op) = pt_entry_x[2]; break;
			case 3: POS_X(op) = (pt_entry_x[0] + pt_entry_x[1]) / 2; break;
			case 4: POS_X(op) = (pt_entry_x[1] + pt_entry_x[2]) / 2; break;
			}
			POS_Y(op)   = pt_entry_y;
			W_ANGLE(op) = opt_angle[pp->count - 3][W_SLOT(op)];
			a = ratan2(POS_Y(bp) - POS_Y(op), POS_X(bp) - POS_X(op));
			if (W_SLOT(op) == 0) {
				op->spd_x = rcos(a) * 45;
				op->spd_y = -rsin(a) * 45;
			} else {
				op->spd_x = rcos(a) << FLY_SHIFT;
				op->spd_y = -rsin(a) << FLY_SHIFT;
			}
			op->flip = (POS_X(op) < POS_X(bp)) ? OBJ_FLIP_R : OBJ_FLIP_L;
			SetAnime(op, ANM_OPT_FLY);
			op->act3++;
			break;
		case 1:
			x = POS_X(bp) - POS_X(op);
			y = POS_Y(bp) - POS_Y(op);
			if (x < 0) x = -x;
			if (y < 0) y = -y;
			if (x < 3 && y < 3) {
				op->flip  = bp->flip;
				op->spd_x = 0;
				op->spd_y = 0;
				opt_ready++;
				op->act3++;
			}
			goto normal;
		case 2:
			if (opt_ready != pp->count)
				goto normal;
			W_8C(op) = 0;
			op->act3++;
			break;
		case 3:
			a = (short)W_ANGLE(op) >> 4;
			POS_X(op) = POS_X(bp) + (rcos(a) * W_8C(op)) / 4096;
			POS_Y(op) = POS_Y(bp) + (-rsin(a) * W_8C(op)) / 4096;
			if (++W_8C(op) == 32) {
				opt_ready++;
				op->act3++;
			}
			break;
		case 4:
			Opt_Ring(op, POS_X(bp), POS_Y(bp));
			break;
		}
		goto normal;

	case OPT_SUMMON:
		op->flip = bp->flip;
		switch (op->act3) {
		case 0:
			if (W_COUNT(op) < 38)
				return;
			SetAnime(op, ANM_OPT_APPEAR);
			W_ANGLE(op) = opt_angle[pp->count - 3][W_SLOT(op)];
			Opt_Ring(op, POS_X(bp), POS_Y(bp));
			op->act3++;
			break;
		case 1:
			if (op->anime.next == 0) {
				SetAnime(op, ANM_OPT_FLY);
				W_TIME(op) = 0;
				op->act3++;
			}
			break;
		case 2:
			if (++W_TIME(op) == 60) {
				opt_ready--;
				op->act3++;
			}
			break;
		}
		if (op->act3 < 2)
			goto normal;
		goto blink;

	case OPT_GATHER:
		op->flip = bp->flip;
		switch (op->act3) {
		case 0:
			op->col_box = NULL;
			op->atk_box = NULL;
			SetAnime(op, ANM_OPT_FLY);
			op->dmg_box = NULL;
			op->spd_x   = 0;
			op->spd_y   = 0;
			op->act3++;
			W_ANGLE(op) = opt_angle[pp->count - 3][W_SLOT(op)];
			break;
		case 1:
			a  = (short)W_ANGLE(op) >> 4;
			tx = POS_X(bp) + rcos(a) / 128;
			ty = POS_Y(bp) + (-rsin(a)) / 128;
			Opt_Step(&POS_X(op), tx, 4);
			Opt_Step(&POS_Y(op), ty, 4);
			if (POS_X(op) == tx && POS_Y(op) == ty) {
				opt_ready++;
				op->act3++;
			}
			break;
		case 2:
			Opt_Ring(op, POS_X(bp), POS_Y(bp));
			break;
		}
		goto blink;

	case OPT_RING:
		op->flip = bp->flip;
		Opt_Spin(op);
		switch (op->act3) {
		case 0:
			SetAnime(op, ANM_OPT_FLY);
			op->act3++;
			break;
		case 1:
			if (bp->act2 == MODE_DASH && bp->act3 == 6
			 && W_COUNT(op) % opt_shot_rate[W_SLOT(op)] == 0)
				Opt_Fx(op, FX_SHOT_RING);
			break;
		}
		goto blink;

	case OPT_SWARM:
		op->flip = bp->flip;
		switch (op->act3) {
		case 0:
			Opt_Spin(op);
			W_8E(op) = 0;
			op->act3++;
			break;
		case 1:
			Opt_TurnTo(op, (W_SLOT(op) & 1) ? -0x4000 : 0x4000);
			if (W_8E(op) == 2) {
				Opt_Arm(op, 1);
				op->spd_y = (W_SLOT(op) & 1) ? -0x30000 : 0x30000;
				op->spd_y = op->spd_y * 125 / 100;
				W_TIME(op) = 0;
				op->act3++;
			}
			break;
		case 2:
			if (op->col_f & (COL_U | COL_D))
				op->spd_y = -op->spd_y;
			if (W_COUNT(op) % 30 == 0)
				Opt_Fx(op, FX_SHOT);
			break;
		}
		goto flicker;

	case OPT_LANCE:
		switch (op->act3) {
		case 0:
			op->flip = bp->flip;
			Opt_Spin(op);
			W_8E(op) = 0;
			op->act3++;
			break;
		case 1:
			Opt_TurnTo(op, (bp->flip == OBJ_FLIP_L) ? -0x8000 : 0);
			if (W_8E(op) == 2) {
				Opt_Arm(op, 1);
				op->act3++;
			}
			break;
		case 2:
			tx = (bp->flip == OBJ_FLIP_L) ? POS_X(bp) - 32 : POS_X(bp) + 32;
			if (Opt_Step(&POS_X(op), tx, 3)) {
				switch (W_SLOT(op)) {
				case 0: W_8E(op) = pt_col_hi;       break;
				case 1: W_8E(op) = pt_col_lo;       break;
				case 2: W_8E(op) = pt_col_mid;      break;
				case 3: W_8E(op) = pt_col_mid + 32; break;
				case 4: W_8E(op) = pt_col_mid - 32; break;
				case 5: W_8E(op) = pt_col_mid + 16; break;
				case 6: W_8E(op) = pt_col_mid - 16; break;
				}
				op->act3++;
			}
			break;
		case 3:
			if (Opt_Step(&POS_Y(op), W_8E(op) + bp->act4, 3)) {
				W_COUNT(bp) = 60;
				op->act3++;
			}
			break;
		case 4:
			POS_Y(op) = W_8E(op) + bp->act4;
			if (W_COUNT(bp) < LANCE_GAP)
				break;
			W_COUNT(bp) = 0;
			op->flip = (POS_X(op) < POS_X(&mega)) ? OBJ_FLIP_R : OBJ_FLIP_L;
			SetAnime(op, ANM_OPT_DASH);
			PlaySound(SE_BANK_ENEMY, SE_YMK_OPT_DASH, op);
			a = ratan2(POS_Y(&mega) - POS_Y(op), POS_X(&mega) - POS_X(op));
			op->spd_x = (rcos(a) * 48) * 2;
			op->spd_y = -rsin(a) * 96;
			W_ANGLE(op) = POS_X(&mega);
			W_8C(op)    = POS_Y(&mega);
			Opt_Fx(op, FX_TRAIL);
			op->act3++;
			break;
		case 5:
			if (r != 0 && IS_ZERO()) {
				op->spd_x   = op->flip ? -0x40000 : 0x40000;
				op->spd_y   = 0x40000;
				op->grav    = 0x2800;
				op->col_box = NULL;
				op->atk_box = NULL;
				op->dmg_box = NULL;
				opt_slot[W_SLOT(op)] = 0;
				opt_alive--;
				W_SLOT(op) = -1;
				op->act3   = 8;
				break;
			}
			x = POS_X(op) - (short)W_ANGLE(op);
			y = POS_Y(op) - W_8C(op);
			if (x < 0) x = -x;
			if (y < 0) y = -y;
			if ((x < 5 && y < 5) || op->col_f != 0) {
				SetAnime(op, ANM_OPT_ATTACK);
				op->spd_x  = 0;
				op->spd_y  = 0;
				W_TIME(op) = 0;
				op->act3++;
			}
			break;
		case 6:
			if (++W_TIME(op) == 60)
				op->act3++;
			break;
		case 7:
			tx = (bp->flip == OBJ_FLIP_L) ? POS_X(bp) - 32 : POS_X(bp) + 32;
			if (Opt_Step(&POS_X(op), tx, 3)) {
				op->act3 -= 4;
				op->flip  = bp->flip;
			}
			break;
		case 8:
			if (W_COUNT(op) & 1)
				op->clut &= ~CLUT_FLASH;
			else
				op->clut |= CLUT_FLASH;
			x = POS_X(bp) - POS_X(op);
			y = POS_Y(bp) - POS_Y(op);
			if ((x < 0 ? -x : x) < 17 && y < 17 && y >= -32
			 && opt_knock == 0 && bp->act2 == MODE_LANCE)
				opt_knock = KNOCK_TIME;
			if (POS_X(op) < pt_left - 32 || pt_right + 32 < POS_X(op)) {
				DeleteObject3(op);
				return;
			}
			break;
		}
		goto flicker;

	case OPT_WALL:
		op->flip  = bp->flip;
		POS_X(op) = POS_X(bp);
		switch (op->act3) {
		case 0:
			Opt_Spin(op);
			W_8E(op) = 0;
			op->act3++;
			break;
		case 1:
			Opt_TurnTo(op, (W_SLOT(op) & 1) ? -0x4000 : 0x4000);
			if (W_8E(op) == 2) {
				Opt_Arm(op, 0);
				op->act3++;
			}
			break;
		case 2:
			if (Opt_Step(&POS_Y(op), POS_Y(bp) + opt_wall_dy[W_SLOT(op)], 3))
				op->act3++;
			break;
		case 3:
			if (bp->spd_x != 0)
				op->act3++;
			break;
		case 4:
			if ((W_SLOT(op) & 1) == 0)
				POS_Y(op) += (W_COUNT(bp) & 0x20) ? 1 : -1;
			else
				POS_Y(op) += (W_COUNT(bp) & 0x20) ? -1 : 1;
			break;
		}
		goto flicker;

	case OPT_TRAIL:
		switch (op->act3) {
		case 0:
			if (bp->flip == OBJ_FLIP_L)
				W_ANGLE(op) += OPT_SPIN;
			else
				W_ANGLE(op) -= OPT_SPIN;
			Opt_Ring(op, W_8C(op), W_8E(op));
			if (W_TIME(bp) == (W_SLOT(op) + 1) * 30) {
				Opt_Arm(op, 1);
				W_8E(op) = 0;
				op->act4 = 0;
				op->act3++;
			}
			break;
		case 1:
			tp = &ymk_trail[W_8E(op)];
			op->flip = tp->flip;
			Opt_Step(&POS_X(op), tp->x, 3);
			Opt_Step(&POS_Y(op), tp->y, 3);
			if (op->act4 != 0)
				op->act4--;
			x = POS_X(&mega) - POS_X(op);
			if ((x < 0 ? -x : x) < 12 && POS_Y(op) + 48 < POS_Y(&mega) && op->act4 == 0) {
				op->act4 = 30;
				Opt_Fx(op, FX_BOMB);
				SetAnime(op, ANM_OPT_BOMB);
			}
			if (op->act4 == 15)
				SetAnime(op, ANM_OPT_ATTACK);
			if (++W_8E(op) == OPT_TRAIL_LEN)
				W_8E(op) = 0;
			break;
		}
		if (op->act3 > 0)
			goto normal;
		goto flicker;

	case OPT_FREEZE:
	freeze:
		DisplayCheck(op, 32, 16);
		AnimeTick(op);
		return;
	}
	goto move;

flicker:
	if (op->act3 >= 2)
		goto normal;
blink:
	if ((W_COUNT(op) & 1) == 0)
		goto move;
normal:
	DisplayCheck(op, 32, 16);
move:
	AnimeTick(op);
	MoveObject2(op);
}

/* ---------------------------------------------------------------- */

void
V15_YammarkFx(OBJECT *op)
{
	if (FX_NOHIT(op) == 0 && op->col_f == 0)
		CornerCheck(op);
	op->disp = 0;

	switch (op->var) {
	case FX_SHOT:
	case FX_SHOT_RING:
		fx_shot[op->act](op);
		break;
	case FX_BOMB:
		fx_bomb[op->act](op);
		break;
	case FX_BURST:
		fx_burst[op->act](op);
		break;
	case FX_WING:
		fx_wing[op->act](op);
		break;
	case FX_FLASH:
		fx_flash[op->act](op);
		break;
	case FX_TRAIL:
		fx_trail[op->act](op);
		break;
	}
}

static void
Fx_Copy(OBJECT *op)
{
	OBJECT	*pp;

	pp = PARENT(op);
	op->anime_tbl = pp->anime_tbl;
	op->tpage     = pp->tpage;
	op->sprt_p    = pp->sprt_p;
	op->clut      = pp->clut & ~CLUT_FLASH;
	op->layer     = pp->layer;
}

static int
Fx_Gone(void)
{
	return (ymk_dead || ymk_stun || opt_knock);
}

static void
Shot_Init(OBJECT *op)
{
	short	a;

	Fx_Copy(op);
	op->pri     = 3;
	op->col_box = &shot_col;
	op->atk_box = &shot_atk;
	op->fall_f  = 0;
	op->disp    = 0;
	op->dmg_box = &shot_dmg;
	op->dmg_tbl = dmg_tbl_yammark;
	SetAnime(op, ANM_SHOT);
	POS_X(op) = POS_X(PARENT(op));
	POS_Y(op) = POS_Y(PARENT(op));

	if (game.level != LEVEL_HARD) {
		if (POS_X(op) < POS_X(&mega)) {
			op->spd_x = 0x30000;
			op->flip  = OBJ_FLIP_R;
		} else {
			op->spd_x = -0x30000;
			op->flip  = OBJ_FLIP_L;
		}
		op->spd_x = op->spd_x * 125 / 100;
	} else {
		a = ratan2(POS_Y(&mega) - POS_Y(op), POS_X(&mega) - POS_X(op));
		op->spd_x = rcos(a) * 48;
		op->spd_y = -rsin(a) * 48;
		op->flip  = (op->spd_x > 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
	}
	op->acc_x = 0;
	op->grav  = 0;
	op->hp    = 1;
	op->atk   = 2;
	PlaySound(SE_BANK_ENEMY, SE_YMK_SHOT, op);
	op->act2 = 0;
	op->act++;
}

static void
Bomb_Main(OBJECT *op)
{
	OBJECT	*tp;
	static const short	bx[3] = { 10, -16, 4 };
	static const short	by[3] = { -8, -4, -12 };
	static const char	bd[3] = { 1, 8, 16 };
	int	i;

	if (Fx_Gone()) {
		op->act = 2;
		return;
	}
	if (op->col_f & COL_D) {
		for (i = 0; i < 3; i++) {
			if ((tp = GetVisualObject()) != NULL) {
				tp->type   = 15;
				tp->var    = FX_BURST;
				PARENT(tp) = op;
				tp->flag   = op->flag;
				POS_X(tp)  = POS_X(op) + bx[i];
				FX_DELAY(tp) = bd[i];
				POS_Y(tp)  = POS_Y(op) + by[i];
			}
		}
		op->act = 2;
		DisplayCheck(op, 4, 16);
		return;
	}
	ContactMega(op);
	op->spd_y -= 0x1000;
	if (op->spd_y < -0x30000)
		op->spd_y = -0x30000;
	AnimeTick(op);
	MoveObject2(op);
	DisplayCheck(op, 4, 16);
}

static void
Wing_Main(OBJECT *op)
{
	OBJECT	*bp;

	bp = ymk_body;
	op->flip = bp->flip;
	POS_X(op) = (bp->flip == OBJ_FLIP_L) ? POS_X(bp) + 3 : POS_X(bp) - 3;
	POS_Y(op) = POS_Y(bp) - 6;

	switch (bp->anime_no) {
	case ANM_HOVER:
	case ANM_FLY:
	case ANM_FLY_UP:
	case ANM_FLY_DOWN:
		if (op->act3 == 0) {
			PlaySound(SE_BANK_ENEMY, SE_YMK_WING, op);
			op->act3 = 1;
		}
		break;
	default:
		op->act3 = 0;
		PlaySound(SE_BANK_ENEMY, SE_YMK_WING_OFF, op);
		break;
	}
	AnimeTick(op);
	MoveObject2(op);
	if (bp->anime_no == ANM_HOVER)
		DisplayCheck(op, 64, 32);
}

/* ---------------------------------------------------------------- */

void
M49_Yammark(OBJECT *op)
{
	if (op->var == 1) {
		Option(op);
		return;
	}
	op->old_x = op->x;
	op->disp  = 0;
	op->old_y = op->y;
	body_act[op->act](op);
	CollideCheck(op);
}

static void
Body_Nop(OBJECT *op)
{
}

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

	bg_layer[0].limit_spd = 2;
	PlayerEvent(29, OBJ_FLIP_R);
	W_TIME(op) = 40;
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
Entry_Warning(OBJECT *op)
{
	OBJECT	*tp;

	if (game.message != 0)
		return;

	if (--W_TIME(op) == 0)
		PlayerEvent(20, OBJ_FLIP_R);

	if (W_SUB(op)->flag != 0)
		return;

	if ((tp = GetVisualObject()) != NULL) {
		tp->type   = 15;
		tp->var    = FX_WING;
		PARENT(tp) = op;
		tp->flag   = op->flag;
	}
	op->spd_y = -DESCEND_SPD;
	SetAnime(op, ANM_HOVER);
	op->act3++;
}

static void
Entry_Descend(OBJECT *op)
{
	OBJECT	*tp;
	int	i;

	if (op->col_box == NULL && POS_Y(op) >= pt_floor - 128)
		op->col_box = &ymk_col;

	if (op->col_f & COL_D) {
		opt_ready = 0;
		for (i = 0; i < ymk_pair->count; i++) {
			if ((tp = GetMainObject()) != NULL) {
				tp->flag  = op->flag;
				tp->var   = 1;
				tp->act2  = OPT_ENTRY;
				W_SUB(tp) = op;
				tp->type  = op->type;
			}
		}
		op->spd_y = 0;
		op->act3++;
	}
	Ymk_Move(op);
}

static void
Entry_Talk(OBJECT *op)
{
	if (opt_ready == ymk_pair->count * 2) {
		if (game.stage != STAGE_REMATCH) {
			TextOpen(0, 255, game.talk_f);
			game.talk_f = 1;
		}
		opt_cmd    = CMD_RING;
		W_TIME(op) = 0;
		op->act3++;
	}
	Ymk_Move(op);
}

static void
Entry_Pose(OBJECT *op)
{
	if (text_busy == 0) {
		SetAnime(op, ANM_POSE);
		op->act3++;
	}
	Ymk_Move(op);
}

static void
Entry_Bar(OBJECT *op)
{
	if (op->anime.next == 0) {
		SetAnime(op, ANM_HOVER);
		ShowBossBar(0);
		game.boss_bar = 1;
		W_TIME(op) = 3;
		op->act3++;
	}
	Ymk_Move(op);
}

static void
Entry_FillHp(OBJECT *op)
{
	if (BossBarBusy()) {
		DisplayCheck(op, 64, 64);
		return;
	}
	if (op->hp < game.boss_hp) {
		if (--W_TIME(op) == 0) {
			PlaySound(SE_BANK_SYS, SE_HP_FILL, NULL);
			W_TIME(op) = 3;
		}
		op->hp++;
		DisplayCheck(op, 64, 64);
		return;
	}
	PlayerEventEnd();
	Ymk_Move(op);
	W_TIME(op) = 0;
	op->act2   = MODE_THINK;
	op->act3   = 0;
}

static void
Body_Idle(OBJECT *op)
{
	idle_tbl[op->act3](op);
	Ymk_Move(op);
}

static void
Idle_Sub(OBJECT *op)
{
}

/*
 *	sub 0 of every attack: the HP phase, then call the options back
 */
static void
Ymk_AttackPhase(OBJECT *op)
{
	Ymk_Phase(op);
	SetAnime(op, ANM_HOVER);
	if (opt_cmd != CMD_RING) {
		opt_ready = 0;
		opt_cmd   = CMD_GATHER;
		op->act3 += 1;
	} else {
		op->act3 += 2;
	}
}

static void
Ymk_AttackGather(OBJECT *op)
{
	if (opt_ready == opt_alive)
		op->act3++;
}

static int
Ymk_AttackReady(OBJECT *op, int anm, int se, int cmd)
{
	if (opt_ready != 0)
		return 0;
	SetAnime(op, anm);
	if (se >= 0)
		PlaySound(SE_BANK_ENEMY, se, op);
	opt_cmd    = cmd;
	W_TIME(op) = 0;
	op->act3++;
	return 1;
}

static void
Body_Swarm(OBJECT *op)
{
	swarm_tbl[op->act3](op);
	Ymk_Move(op);
}

static void Swarm_Phase(OBJECT *op)  { Ymk_AttackPhase(op); }
static void Swarm_Gather(OBJECT *op) { Ymk_AttackGather(op); }
static void Swarm_Ready(OBJECT *op)  { Ymk_AttackReady(op, ANM_HOVER, -1, CMD_RING); }

static void
Swarm_Go(OBJECT *op)
{
	if (++W_TIME(op) == 2) {
		opt_cmd    = CMD_SWARM;
		W_TIME(op) = 0;
		op->act3++;
	}
}

static void
Swarm_Wait(OBJECT *op)
{
	if (W_TIME(op) == 120)
		PlaySound(SE_BANK_ENEMY, SE_YMK_SWARM, op);
	if (++W_TIME(op) == SWARM_TIME)
		Ymk_NextMode(op);
}

static void
Body_Lance(OBJECT *op)
{
	lance_tbl[op->act3](op);
	Ymk_Move(op);
}

static void Lance_Phase(OBJECT *op)  { Ymk_AttackPhase(op); }
static void Lance_Gather(OBJECT *op) { Ymk_AttackGather(op); }
static void Lance_Ready(OBJECT *op)  { Ymk_AttackReady(op, ANM_HOVER, -1, CMD_RING); }

static void
Lance_Go(OBJECT *op)
{
	if (++W_TIME(op) == 2) {
		opt_cmd     = CMD_LANCE;
		W_8E(op)    = 0;
		W_SWING(op) = 0;
		W_TIME(op)  = 0;
		W_COUNT(op) = 0;
		op->act3++;
	}
}

static void
Body_Dash(OBJECT *op)
{
	dash_tbl[op->act3](op);
	Ymk_Move(op);
}

static void Dash_Phase(OBJECT *op)  { Ymk_AttackPhase(op); }
static void Dash_Gather(OBJECT *op) { Ymk_AttackGather(op); }
static void Dash_Ready(OBJECT *op)  { Ymk_AttackReady(op, ANM_HOVER, SE_YMK_READY, CMD_RING); }

static void
Dash_Wall(OBJECT *op)
{
	if (op->col_f & (COL_R | COL_L)) {
		opt_cmd   = CMD_NONE;
		op->spd_x = 0;
		Ymk_NextMode(op);
	}
}

static void
Body_WallDash(OBJECT *op)
{
	wall_tbl[op->act3](op);
	Ymk_Move(op);
}

static void Wall_Phase(OBJECT *op)  { Ymk_AttackPhase(op); }
static void Wall_Gather(OBJECT *op) { Ymk_AttackGather(op); }
static void Wall_Ready(OBJECT *op)  { Ymk_AttackReady(op, ANM_HOVER, SE_YMK_READY, CMD_WALL); }

static void
Wall_Wait(OBJECT *op)
{
	if (W_TIME(op) == 90)
		PlaySound(SE_BANK_ENEMY, SE_YMK_WALLDASH, op);
	if (++W_TIME(op) == WALL_WAIT) {
		op->spd_x   = op->flip ? WALL_SPD : -WALL_SPD;
		W_COUNT(op) = 0;
		W_8E(op)    = 0;
		op->act3++;
	}
}

static void
Wall_Run(OBJECT *op)
{
	W_COUNT(op)++;
	if ((op->spd_x < 0 && (op->col_f & COL_L))
	 || (op->spd_x > 0 && (op->col_f & COL_R))) {
		op->spd_x = 0;
		if (++W_8E(op) == WALL_TURNS) {
			Ymk_NextMode(op);
			return;
		}
		W_TIME(op) = 0;
		op->act3++;
	}
}

static void
Wall_Pause(OBJECT *op)
{
	W_COUNT(op)++;
	if (++W_TIME(op) == WALL_PAUSE) {
		if (op->flip) {
			op->spd_x = -WALL_SPD;
			op->flip  = OBJ_FLIP_L;
		} else {
			op->spd_x = WALL_SPD;
			op->flip  = OBJ_FLIP_R;
		}
		op->act3--;
	}
}

static void
Body_Chase(OBJECT *op)
{
	chase_tbl[op->act3](op);
	Ymk_Move(op);
}

static void Chase_Phase(OBJECT *op)  { Ymk_AttackPhase(op); }
static void Chase_Gather(OBJECT *op) { Ymk_AttackGather(op); }

static void
Chase_Ready(OBJECT *op)
{
	if (Ymk_AttackReady(op, ANM_FLY, SE_YMK_READY, CMD_TRAIL))
		W_8E(op) = 0;
}

static void
Body_Stun(OBJECT *op)
{
	if (W_MUTEKI(op) < 15)
		W_MUTEKI(op) = 30;
	stun_tbl[op->act3](op);
	Ymk_Move(op);
}

static void
Stun_Start(OBJECT *op)
{
	op->spd_x = 0;
	SetAnime(op, ANM_STUN);
	op->spd_y = 0;
	PlaySound(SE_BANK_ENEMY, SE_YMK_STUN, op);
	op->atk_box = NULL;
	op->dmg_tbl = dmg_tbl_guard;
	W_TIME(op)  = 0;
	op->act3++;
}

static void
Stun_Wait(OBJECT *op)
{
	if (++W_TIME(op) == STUN_TIME) {
		ymk_stun = 0;
		SetAnime(op, ANM_HOVER);
		Ymk_Boxes(op);
		op->act2 = MODE_THINK;
		op->act3 = 0;
	}
}

static void
Body_Dead(OBJECT *op)
{
	dead_tbl[op->act2](op);
}

static void
Dead_Start(OBJECT *op)
{
	W_TIME(op)  = 127;
	W_COUNT(op) = 25;
	op->muteki  = 25;
	op->clut   &= ~CLUT_FLASH;
	SetAnime(op, ANM_DEAD);
	PlaySound(SE_BANK_ENEMY, SE_YMK_DEAD, op);
	op->act2++;
	PlayerEvent(20, mega.flip);
	DisplayCheck(op, 64, 64);
}

static void
Dead_Blink(OBJECT *op)
{
	OBJECT	*tp;

	if (--W_TIME(op) == 0) {
		if ((tp = GetSysObject()) != NULL) {
			tp->flag  = 1;
			tp->type  = 26;
			POS_X(tp) = POS_X(op);
			POS_Y(tp) = POS_Y(op);
			W_SUB(op) = tp;
		}
		op->act2++;
	}
	if (--W_COUNT(op) == -1) {
		op->clut ^= CLUT_FLASH;
		op->muteki -= 5;
		if (op->muteki >= 26)
			op->muteki = 0;
		W_COUNT(op) = (op->muteki < 6) ? 5 : op->muteki;
	}
	DisplayCheck(op, 64, 64);
}

/* ---------------------------------------------------------------- */

static void
Option(OBJECT *op)
{
	op->disp = 0;
	opt_act[op->act](op);
	CollideCheck(op);
}

static void
Opt_Delete(OBJECT *op)
{
	opt_slot[W_SLOT(op)] = 0;
	opt_alive--;
	DeleteObject3(op);
}

static void
Shot_Main(OBJECT *op)
{
	if (Fx_Gone()) {
		op->act = 2;
		return;
	}
	if (op->col_f != 0 || ContactWeapon(op) < 0 || ContactMega(op) != 0) {
		SpawnExplosion(op);
		op->act = 2;
		return;
	}
	AnimeTick(op);
	MoveObject2(op);
	DisplayCheck(op, 16, 16);
}

static void
Fx_Delete(OBJECT *op)
{
	DeleteObject3(op);
}

static void
Bomb_Init(OBJECT *op)
{
	Fx_Copy(op);
	op->pri     = 3;
	op->fall_f  = 0;
	op->disp    = 0;
	op->col_box = &bomb_col;
	op->atk_box = &bomb_atk;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	SetAnime(op, ANM_BOMB);
	op->flip  = PARENT(op)->flip;
	POS_X(op) = POS_X(PARENT(op));
	op->spd_y = -0x10000;
	op->spd_x = 0;
	op->acc_x = 0;
	op->grav  = 0;
	op->hp    = 0;
	op->atk   = 2;
	op->act2  = 0;
	op->act++;
	POS_Y(op) = POS_Y(PARENT(op));
}

static void
Burst_Init(OBJECT *op)
{
	Fx_Copy(op);
	op->pri     = 2;
	op->fall_f  = 0;
	op->disp    = 0;
	op->col_box = NULL;
	op->atk_box = &burst_atk;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav  = 0;
	SetAnime(op, ANM_BURST);
	op->hp   = 0;
	op->atk  = 4;
	op->act2 = 0;
	op->act++;
}

static void
Burst_Main(OBJECT *op)
{
	if (ymk_dead || ymk_stun || opt_knock) {
		op->act = 2;
		return;
	}
	switch (op->act2) {
	case 0:
		if (--FX_DELAY(op) == 0) {
			SpawnExplosion(op);
			op->act2++;
		}
		return;
	case 1:
		if (++FX_DELAY(op) == 30) {
			op->act = 2;
			return;
		}
		break;
	default:
		return;
	}
	ContactMega(op);
	DisplayCheck(op, 16, 16);
}

static void
Wing_Init(OBJECT *op)
{
	Fx_Copy(op);
	op->pri     = 6;
	op->fall_f  = 0;
	op->disp    = 0;
	op->col_box = NULL;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	SetAnime(op, ANM_WING);
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav  = 0;
	op->hp    = 0;
	op->atk   = 0;
	op->act3  = 0;
	op->act2  = 0;
	op->act++;
}

static void
Flash_Init(OBJECT *op)
{
	Fx_Copy(op);
	op->pri     = 3;
	op->fall_f  = 0;
	op->disp    = 0;
	op->col_box = NULL;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	SetAnime(op, ANM_FLASH);
	op->flip  = ymk_body->flip;
	POS_X(op) = POS_X(ymk_body);
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav  = 0;
	op->hp    = 0;
	op->atk   = 0;
	op->act3  = 0;
	op->act2  = 0;
	op->act++;
	POS_Y(op) = POS_Y(ymk_body);
}

static void
Flash_Main(OBJECT *op)
{
	if (op->anime.next == 0 || Fx_Gone()) {
		op->act = 2;
		return;
	}
	if (++FX_COUNT(op) == 22)
		PlaySound(SE_BANK_ENEMY, SE_YMK_FLASH, op);
	AnimeTick(op);
	MoveObject2(op);
	DisplayCheck(op, 96, 96);
}

static void
Trail_Init(OBJECT *op)
{
	Fx_Copy(op);
	op->pri     = 3;
	op->fall_f  = 0;
	op->disp    = 0;
	op->col_box = NULL;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	SetAnime(op, ANM_TRAIL);
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
Trail_Main(OBJECT *op)
{
	OBJECT	*pp;

	pp = PARENT(op);
	if (ymk_dead) {
		op->act = 2;
		return;
	}
	op->flip  = pp->flip;
	POS_X(op) = POS_X(pp);
	POS_Y(op) = POS_Y(pp);
	if (pp->act2 != OPT_LANCE || pp->act3 != 5) {
		op->act = 2;
		return;
	}
	AnimeTick(op);
	MoveObject2(op);
	DisplayCheck(op, 32, 32);
}
