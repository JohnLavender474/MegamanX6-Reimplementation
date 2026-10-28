/*
 *	m10_sea_attacker.c
 *
 *	ST09 (Rainy Turtloid)  main object 10 : Sea Attacker ST
 *	                       visual object 09 : its bubble shot
 */
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>

#include "object.h"
#include "game.h"
#include "sound.h"

enum {
	ACT_INIT = 0,
	ACT_MAIN,
	ACT_DEAD,
	ACT_GONE
};

enum {
	MODE_REACT = 0,
	MODE_NONE,
	MODE_SHOOTER,
	MODE_DIVER
};

enum {
	SHOOT_WAIT = 0,
	SHOOT_FIRE,
	SHOOT_REST
};

enum {
	DIVE_CURL = 0,
	DIVE_RISE,
	DIVE_HOVER,
	DIVE_DASH,
	DIVE_DROP
};

enum {
	TAMA_RISE = 0,
	TAMA_WAIT,
	TAMA_FLY
};

#define SEA_HP		6
#define SEA_ATK		4
#define SEA_PRI		7
#define SEA_PRI_DIVE	18
#define SEA_RANGE	96
#define SEA_FACE_TM	30
#define SEA_REST	20
#define DIVE_SPD	0x3c000
#define DIVE_HOVER_TM	40
#define DIVE_HIT_X	0x18000

#define TAMA_HP		1
#define TAMA_ATK	4
#define TAMA_PRI	5
#define TAMA_SPD	0x40000
#define TAMA_OFS_X	6
#define TAMA_OFS_Y	20
#define TAMA_WAIT_N	40
#define TAMA_WAIT_H	8

#define tama_sub(op)	((op)->act4)
#define face_tm(op)	(*(u_short *)&(op)->work[2])
#define rise_ofs(op)	(*(short *)&(op)->work[4])
#define tama_tm(op)	(*(u_short *)&(op)->work[10])

extern ANIME	*sea_anime[];
extern HITBOX	sea_atk_box, sea_dmg_box;
extern HITBOX	ball_atk_box, ball_dmg_box;
extern HITBOX	tama_col_box, tama_atk_box, tama_dmg_box;

static u_char	junk_anime[] = { 4, 5, 6, 7, 8 };

static void	tama_init(OBJECT *op);
static void	tama_move(OBJECT *op);
static void	tama_delete(OBJECT *op);
static void	sea_init(OBJECT *op);
static void	sea_main(OBJECT *op);
static void	sea_delete(OBJECT *op);
static void	sea_delete2(OBJECT *op);
static void	sea_shooter(OBJECT *op);
static void	shooter_wait(OBJECT *op);
static void	shooter_fire(OBJECT *op);
static void	shooter_rest(OBJECT *op);
static void	sea_diver(OBJECT *op);
static void	diver_curl(OBJECT *op);
static void	diver_rise(OBJECT *op);
static void	diver_hover(OBJECT *op);
static void	diver_dash(OBJECT *op);
static void	diver_drop(OBJECT *op);
static void	sea_nop(OBJECT *op);
static void	sea_shot_spawn(OBJECT *op);
static void	sea_face_mega(OBJECT *op);

static void	(*sea_act[])(OBJECT *) = {
	sea_init, sea_main, sea_delete, sea_delete2
};

static void	(*sea_mode[])(OBJECT *) = {
	HitReactionDispatch, sea_nop, sea_shooter, sea_diver
};

static void	(*shooter_tbl[])(OBJECT *) = {
	shooter_wait, shooter_fire, shooter_rest
};

static void	(*diver_tbl[])(OBJECT *) = {
	diver_curl, diver_rise, diver_hover, diver_dash, diver_drop
};

static void	(*tama_act[])(OBJECT *) = {
	tama_init, tama_move, tama_delete
};

/* ---------------------------------------------------------------- */
/*	visual object 09 : bubble shot                                    */
/* ---------------------------------------------------------------- */

static void tama_init(OBJECT *op)
{
	op->atk = TAMA_ATK;
	op->hp = TAMA_HP;
	op->dmg_tbl = dmg_tbl_shot;
	op->pri = TAMA_PRI;
	SetAnime(op, 2);
	op->fall_f = 1;
	op->col_box = &tama_col_box;
	op->atk_box = &tama_atk_box;
	op->dmg_box = &tama_dmg_box;
	op->act = 1;
	op->act2 = 0;
	op->act3 = 0;
	tama_sub(op) = TAMA_RISE;
	if (op->flip == OBJ_FLIP_L)
		op->x.w.hi += TAMA_OFS_X;
	else
		op->x.w.hi -= TAMA_OFS_X;
	op->y.w.hi -= TAMA_OFS_Y;
	ratan2(POS_Y(op) - POS_Y(&mega), POS_X(&mega) - POS_X(op));
	op->acc_x = 0;
	op->grav = 0;
	op->spd_x = 0;
	op->spd_y = TAMA_SPD;
	if (OffScreenCheck2(op, 32, 32))
		op->act++;
	else
		DisplayCheck(op, 16, 16);
}

static void tama_move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);

	switch (tama_sub(op)) {
	case TAMA_RISE:
		if (POS_Y(&mega) < POS_Y(op))
			break;
		SetAnime(op, 9);
		tama_tm(op) = HARD_MODE() ? TAMA_WAIT_H : TAMA_WAIT_N;
		op->spd_x = 0;
		op->spd_y = 0;
		tama_sub(op)++;
		break;
	case TAMA_WAIT:
		if (--tama_tm(op) != 0)
			break;
		SetAnime(op, 2);
		op->spd_x = (POS_X(&mega) < POS_X(op)) ? -TAMA_SPD : TAMA_SPD;
		tama_sub(op)++;
		break;
	}

	if (ContactWeapon(op) < 0) {
		SpawnExplosion(op);
		op->act++;
		return;
	}
	op->clut &= ~CLUT_FLASH;
	ContactMega(op);
	if (op->col_f == 0) {
		CornerCheck(op);
		if (op->col_f == 0) {
			if (OffScreenCheck2(op, 32, 32))
				op->act++;
			else
				DisplayCheck(op, 16, 16);
			return;
		}
	}
	SpawnShotBurst(op);
	op->act++;
}

/* ---------------------------------------------------------------- */
/*	main object 10 : Sea Attacker ST                                  */
/* ---------------------------------------------------------------- */

void m10_sea_attacker(OBJECT *op)
{
	(*sea_act[op->act])(op);
}

void v09_sea_shot(OBJECT *op)
{
	(*tama_act[op->act])(op);
}

static void sea_init(OBJECT *op)
{
	op->atk = SEA_ATK;
	op->hp = SEA_HP;
	op->muteki = 0;
	op->disp = 0;
	op->anime_tbl = sea_anime;
	op->pri = SEA_PRI;
	op->dmg_tbl = dmg_tbl_mecha_s;
	op->atk_box = &sea_atk_box;
	op->flip = OBJ_FLIP_L;
	op->fall_f = 0;
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav = 0;
	op->disp = 0;
	op->col_box = NULL;
	op->dmg_box = &sea_dmg_box;
	op->layer = mega.layer;
	op->old_x = op->x;
	op->old_y = op->y;
	face_tm(current_main) = SEA_FACE_TM;
	sea_face_mega(op);
	if (op->var == 0) {
		SetAnime(op, 0);
		op->act2 = MODE_SHOOTER;
	} else {
		SetAnime(op, 3);
		op->act2 = MODE_DIVER;
	}
	sea_face_mega(op);
	op->act = ACT_MAIN;
	op->act3 = 0;
}

static void sea_main(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	AnimeTick(op);
	(*sea_mode[op->act2])(op);

	if (ContactWeapon(op) < 0) {
		SpawnJunk(5, junk_anime, op);
		SpawnExplosion(op);
		DropItem(op, 19);
		op->act = ACT_DEAD;
		return;
	}
	ContactMega(op);
	if (OffScreenCheck2(op, 64, 64))
		op->act = ACT_GONE;
	else
		DisplayCheck(op, 32, 32);
}

static void sea_delete(OBJECT *op)
{
	DeleteObject(op);
}

static void sea_delete2(OBJECT *op)
{
	DeleteObject2(op);
}

static void sea_shooter(OBJECT *op)
{
	(*shooter_tbl[op->act3])(op);
}

static void shooter_wait(OBJECT *op)
{
	short d;

	d = POS_X(&mega) - POS_X(op);
	if (d < 0)
		d = -d;
	if (d > SEA_RANGE)
		return;
	sea_face_mega(op);
	op->act3++;
	SetAnime(op, 1);
}

static void shooter_fire(OBJECT *op)
{
	if (op->anime.flag == ANM_FIRE) {
		PlaySound(SE_BANK_ENEMY, SE_SEA_SHOT, op);
		sea_shot_spawn(op);
	} else if (op->anime.flag == ANM_END) {
		SetAnime(op, 0);
		op->timer = SEA_REST;
		op->act3++;
	}
}

static void shooter_rest(OBJECT *op)
{
	if (--face_tm(current_main) == 0) {
		sea_face_mega(op);
		face_tm(current_main) = SEA_FACE_TM;
	}
	if (--op->timer == 0)
		op->act3 = SHOOT_WAIT;
}

static void sea_diver(OBJECT *op)
{
	(*diver_tbl[op->act3])(op);
	MoveObject(op);
}

static void diver_curl(OBJECT *op)
{
	if (op->anime.flag != ANM_FIRE)
		return;
	op->atk_box = &ball_atk_box;
	op->dmg_box = &ball_dmg_box;
	op->spd_y = DIVE_SPD;
	rise_ofs(current_main) = GetRNG() & 0x1f;
	op->act3++;
}

static void diver_rise(OBJECT *op)
{
	if (POS_Y(&mega) - POS_Y(op) < rise_ofs(current_main))
		return;
	op->pri = SEA_PRI_DIVE;
	sea_face_mega(op);
	op->timer = DIVE_HOVER_TM;
	op->spd_x = 0;
	op->spd_y = 0;
	op->act3++;
}

static void diver_hover(OBJECT *op)
{
	if (--face_tm(current_main) == 0) {
		sea_face_mega(op);
		face_tm(current_main) = SEA_FACE_TM;
	}
	if (--op->timer != 0)
		return;
	sea_face_mega(op);
	op->spd_x = op->flip ? DIVE_SPD : -DIVE_SPD;
	op->act3++;
}

static void diver_dash(OBJECT *op)
{
	long d;

	d = mega.x.l - op->x.l;
	if (d >= 0) {
		if (d > DIVE_HIT_X)
			return;
	} else {
		if (-d > DIVE_HIT_X)
			return;
	}
	op->spd_x = 0;
	op->spd_y = -DIVE_SPD;
	op->act3++;
}

static void diver_drop(OBJECT *op)
{
}

static void sea_nop(OBJECT *op)
{
}

static void sea_shot_spawn(OBJECT *op)
{
	OBJECT *tp;

	if ((tp = GetVisualObject()) == NULL)
		return;
	tp->flag = 0x41;
	tp->type = 9;
	tp->var = 0;
	tp->flip = op->flip;
	tp->x.w.lo = 0;
	tp->x.w.hi = op->x.w.hi;
	tp->y.w.lo = 0;
	tp->y.w.hi = op->y.w.hi;
	tp->clut = op->clut & ~CLUT_FLASH;
	tp->anime_tbl = op->anime_tbl;
	tp->sprt_p = op->sprt_p;
	tp->tpage = op->tpage;
	tp->layer = op->layer;
}

static void sea_face_mega(OBJECT *op)
{
	if (POS_X(op) < POS_X(&mega))
		op->flip = OBJ_FLIP_R;
	else
		op->flip = OBJ_FLIP_L;
}

static void tama_delete(OBJECT *op)
{
	DeleteObject3(op);
}
