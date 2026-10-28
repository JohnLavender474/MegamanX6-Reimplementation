/*
 *	m07_caterpirade.c
 *
 *	ST03 (Commander Yammark)  main object 07  : Caterpirade spawner
 *	                          stage object 01 : Caterpirade
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
	MODE_WALK = 0,
	MODE_CURL,
	MODE_FALL,
	MODE_TURN,
	MODE_BALL
};

#define DIR_L		0x00
#define DIR_R		0x40

#define CATER_HP	12
#define CATER_ATK	4
#define CATER_PRI	7
#define CATER_GRAV	0x2000
#define CATER_WALK	0x8000
#define CATER_ROLL	0x10000
#define CATER_BRAKE	(-0x200)
#define TURN_WAIT	40
#define CURL_WAIT	40
#define ROLL_TIME	128
#define BRAKE_TIME	90
#define PROBE_DOWN	8

#define mode(op)	((op)->act3)
#define sub(op)		((op)->act4)
#define timer(op)	((op)->work[2])

extern ANIME	*cater_anime[];
extern HITBOX	cater_col_box, cater_ball_col_box;
extern HITBOX	cater_atk_box, cater_ball_atk_box;
extern HITBOX	cater_dmg_box, cater_ball_dmg_box;

static void	cater_init(OBJECT *op);
static void	cater_main(OBJECT *op);
static void	cater_delete(OBJECT *op);
static void	cater_delete2(OBJECT *op);
static void	cater_walk(OBJECT *op);
static void	cater_curl(OBJECT *op);
static void	cater_fall(OBJECT *op);
static void	cater_turn(OBJECT *op);
static void	cater_ball(OBJECT *op);
static void	curl_start(OBJECT *op);
static void	curl_anim(OBJECT *op);
static void	curl_wait(OBJECT *op);
static void	ball_roll(OBJECT *op);
static void	ball_slow(OBJECT *op);
static void	ball_open(OBJECT *op);
static void	ball_uncurl(OBJECT *op);

static void	(*cater_act[])(OBJECT *) = {
	cater_init, cater_main, cater_delete, cater_delete2
};

static void	(*cater_mode[])(OBJECT *) = {
	cater_walk, cater_curl, cater_fall, cater_turn, cater_ball
};

static void	(*cater_curl_tbl[])(OBJECT *) = {
	curl_start, curl_anim, curl_wait
};

static void	(*cater_ball_tbl[])(OBJECT *) = {
	ball_roll, ball_slow, ball_open, ball_uncurl
};

static void cater_platform(OBJECT *op)
{
	u_char	old, was;

	old = MEGA_ON_OBJECT();
	MEGA_ON_OBJECT() = ON_OBJECT;
	was = op->touch[0];
	SolidObjectMega(op);
	old |= op->touch[0] & ON_OBJECT;
	MEGA_ON_OBJECT() = old;
	if ((was & ON_OBJECT) && !(op->touch[0] & ON_OBJECT))
		MEGA_ON_OBJECT() = old | ON_OBJECT;
}

static void cater_main(OBJECT *op)
{
	char	hp;

	op->clut &= ~CLUT_FLASH;
	hp = op->hp;
	if (ContactWeapon(op) < 0) {
		SpawnExplosion(op);
		DropItem(op, 16);
		op->act = ACT_DEAD;
		return;
	}
	if ((char)op->hp != hp) {
		PlaySound(SE_BANK_ENEMY, SE_CATER_HIT, op);
		op->spd_x = 0;
		op->acc_x = 0;
		op->anime.sprt = 0;
		mode(op) = MODE_CURL;
		sub(op) = 0;
		op->clut |= CLUT_FLASH;
	}

	cater_mode[mode(op)](op);

	if (OffScreenCheck2(op, 64, 64))
		op->act = ACT_GONE;
	else
		DisplayCheck(op, 32, 32);
	DisplayCheck(op, 32, 32);

	CollideCheck(op);

	if (MEGA_HIT_TIMER() == 0 && mode(op) != MODE_BALL && mode(op) != MODE_CURL)
		cater_platform(op);

	ContactMega(op);

	if (op->col_f & COL_D) {
		op->fall_f = 0;
		return;
	}
	op->fall_f = (op->spd_y < 0) ? -1 : 1;
}

static void cater_walk(OBJECT *op)
{
	HITBOX	*cb;
	short	x;

	AnimeTick(op);
	MoveObject2(op);

	cb = op->col_box;
	if (op->flip)
		x = POS_X(op) + cb->ox + cb->w;
	else
		x = POS_X(op) - cb->ox - cb->w;

	if (!TerrainPointCheck(op, x, POS_Y(op) + cb->oy + cb->h + PROBE_DOWN))
		goto turn;
	if (op->flip == DIR_L) {
		if (op->col_f & COL_L)
			goto turn;
	} else {
		if (op->col_f & COL_R)
			goto turn;
	}
	return;

turn:
	mode(op) = MODE_TURN;
	timer(op) = TURN_WAIT;
}

void cater_spawner(OBJECT *op)
{
	OBJECT	*sp;

	sp = GetStageObject();
	if (sp == NULL) {
		DeleteObject2(op);
		return;
	}
	sp->type = 1;
	sp->flag = op->flag;
	sp->var = op->var;
	POS_X(sp) = POS_X(op);
	POS_Y(sp) = POS_Y(op);
	sp->set = op->set;
	sp->tpage = op->tpage;
	sp->clut = op->clut;
	sp->sprt_p = op->sprt_p;
	DeleteObject3(op);
}

void cater_update(OBJECT *op)
{
	if (op->col_f & COL_D)
		op->spd_y = 0;
	op->old_x = op->x;
	op->old_y = op->y;
	cater_act[op->act](op);
}

static void cater_init(OBJECT *op)
{
	op->act2 = MODE_FALL;
	mode(op) = MODE_FALL;
	sub(op) = 0;
	op->act++;
	op->pri = CATER_PRI;
	op->col_box = &cater_col_box;
	op->dmg_box = &cater_dmg_box;
	op->atk_box = &cater_atk_box;
	op->anime_tbl = cater_anime;
	op->dmg_tbl = dmg_tbl_normal;
	op->grav = CATER_GRAV;
	op->hp = CATER_HP;
	op->atk = CATER_ATK;
	op->muteki = 0;
	op->side_kind = 0;
	op->flip = DIR_L;
	op->hit_tag = 0;
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->fall_f = 0;
	timer(op) = TURN_WAIT;
	op->layer = mega.layer;
	SetAnime(op, 0);
}

static void cater_delete2(OBJECT *op)
{
	DeleteObject2(op);
}

static void cater_delete(OBJECT *op)
{
	DeleteObject(op);
}

static void cater_curl(OBJECT *op)
{
	MoveObject2(op);
	cater_curl_tbl[sub(op)](op);
}

static void curl_start(OBJECT *op)
{
	SetAnime(op, 2);
	op->col_box = &cater_ball_col_box;
	op->dmg_box = &cater_ball_dmg_box;
	op->atk_box = &cater_ball_atk_box;
	op->dmg_tbl = dmg_tbl_deflect;
	sub(op)++;
}

static void curl_anim(OBJECT *op)
{
	AnimeTick(op);
	if (op->anime.flag) {
		timer(op) = CURL_WAIT;
		sub(op)++;
	}
}

static void curl_wait(OBJECT *op)
{
	if (--timer(op))
		return;

	SetAnime(op, 3);
	if (mega.x.l < op->x.l) {
		op->flip = DIR_L;
		op->spd_x = -CATER_ROLL;
	} else {
		op->flip = DIR_R;
		op->spd_x = CATER_ROLL;
	}
	timer(op) = ROLL_TIME;
	mode(op) = MODE_BALL;
	sub(op) = 0;
}

static void cater_fall(OBJECT *op)
{
	if (!(op->col_f & COL_D)) {
		MoveObject2(op);
		return;
	}
	mode(op) = MODE_WALK;
	if (mega.x.l < op->x.l) {
		op->spd_x = -CATER_WALK;
		op->flip = DIR_L;
	} else {
		op->flip = DIR_R;
		op->spd_x = CATER_WALK;
	}
	op->fall_f = 0;
}

static void cater_turn(OBJECT *op)
{
	if (timer(op)) {
		if (--timer(op) == 0)
			SetAnime(op, 1);
		return;
	}
	AnimeTick(op);
	if (op->anime.flag) {
		SetAnime(op, 0);
		mode(op) = MODE_WALK;
		op->flip = (op->flip == 0) ? DIR_R : DIR_L;
		op->spd_x = -op->spd_x;
	}
}

static void cater_ball(OBJECT *op)
{
	cater_ball_tbl[sub(op)](op);
}

static void ball_roll(OBJECT *op)
{
	AnimeTick(op);
	MoveObject2(op);
	if (--timer(op))
		return;
	op->acc_x = CATER_BRAKE;
	SetAnime(op, 5);
	timer(op) = BRAKE_TIME;
	sub(op)++;
}

static void ball_slow(OBJECT *op)
{
	AnimeTick(op);
	MoveObject2(op);
	if (--timer(op))
		return;
	op->spd_x = 0;
	op->acc_x = 0;
	sub(op)++;
}

static void ball_open(OBJECT *op)
{
	MoveObject2(op);
	AnimeTick(op);
	if (op->anime.sprt != 23 && op->anime.sprt != 16)
		return;
	PlaySound(SE_BANK_ENEMY, SE_CATER_OPEN, op);
	op->col_box = &cater_col_box;
	op->dmg_box = &cater_dmg_box;
	op->atk_box = &cater_atk_box;
	op->dmg_tbl = dmg_tbl_normal;
	SetAnime(op, 4);
	sub(op)++;
}

static void ball_uncurl(OBJECT *op)
{
	MoveObject2(op);
	AnimeTick(op);
	if (op->anime.flag) {
		SetAnime(op, 0);
		mode(op) = MODE_FALL;
		sub(op) = 0;
	}
}
