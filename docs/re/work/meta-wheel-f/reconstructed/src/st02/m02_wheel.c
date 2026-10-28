/*
 *	m02_wheel.c
 *
 *	ST02 (opening stage)  main object 02 : Meta Wheel F
 *	              misc object 02 : the stud lights that follow the wheel
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
	MODE_ROLL,
	MODE_FALL,
	MODE_TURN
};

#define DIR_R		0x00
#define DIR_L		0x40

#define WHEEL_HP	32
#define WHEEL_ATK	4
#define WHEEL_ACC	0x0c00
#define WHEEL_SPD	0xc000
#define WHEEL_GRAV	0x2000
#define TURN_WAIT	90
#define TURN_FLASH	20

#define dir(op)		((op)->act4)
#define last_spr(op)	((op)->work[2])
#define parent(op)	(*(OBJECT **)&(op)->atk_box)

extern ANIME	*wheel_anime[];
extern HITBOX	wheel_col_box, wheel_atk_box, wheel_dmg_box;
extern u_char	dmg_tbl_wheel[];
extern u_char	studs_spr[];

static u_char	junk_anime[] = { 8, 9, 10 };

static void	wheel_init(OBJECT *op);
static void	wheel_main(OBJECT *op);
static void	wheel_delete(OBJECT *op);
static void	wheel_delete2(OBJECT *op);
static void	wheel_roll(OBJECT *op);
static void	wheel_fall(OBJECT *op);
static void	wheel_turn(OBJECT *op);
static void	turn_start(OBJECT *op);
static void	turn_wait(OBJECT *op);
static void	turn_flip(OBJECT *op);

static void	(*wheel_act[])(OBJECT *) = {
	wheel_init, wheel_main, wheel_delete, wheel_delete2
};

static void	(*wheel_mode[])(OBJECT *) = {
	HitReactionDispatch, wheel_roll, wheel_fall, wheel_turn
};

static void	(*wheel_turn_tbl[])(OBJECT *) = {
	turn_start, turn_wait, turn_flip
};

static void wheel_move(OBJECT *op)
{
	op->x.l += op->spd_x;
	op->y.l -= op->spd_y;
	if (dir(op))
		op->spd_x += op->acc_x;
	else
		op->spd_x -= op->acc_x;
	op->spd_y -= op->grav;
	if (op->spd_y < fall_max)
		op->spd_y = fall_max;
}

static void wheel_init(OBJECT *op)
{
	OBJECT *sp;

	op->hp = WHEEL_HP;
	op->timer = 0;
	op->muteki = 0;
	op->anime_tbl = wheel_anime;
	op->pri = 6;
	op->col_box = &wheel_col_box;
	op->atk_box = &wheel_atk_box;
	op->dmg_box = &wheel_dmg_box;
	op->atk = WHEEL_ATK;
	op->dmg_tbl = dmg_tbl_wheel;
	op->act2 = MODE_FALL;
	op->acc_x = -WHEEL_ACC;
	op->grav = WHEEL_GRAV;
	op->fall_f = 0;
	op->flip = OBJ_FLIP_L;
	op->spd_x = 0;
	op->spd_y = 0;
	op->act = ACT_MAIN;
	op->act3 = 0;
	op->layer = mega.layer;
	op->old_x = op->x;
	op->old_y = op->y;
	if (op->var == 0) {
		dir(op) = DIR_R;
		SetAnime(op, 2);
	} else {
		dir(op) = DIR_L;
		SetAnime(op, 0);
	}
	last_spr(op) = op->anime.sprt;

	if ((sp = GetMiscObject()) != NULL) {
		sp->flag = 0x41;
		sp->type = 2;
		sp->var = op->anime.sprt;
		sp->tpage = op->tpage;
		sp->clut = op->clut & ~CLUT_FLASH;
		sp->anime_tbl = op->anime_tbl;
		sp->sprt_p = op->sprt_p;
		sp->layer = op->layer;
		sp->x = op->x;
		sp->y.w.hi = op->y.w.hi;
		parent(sp) = op;
		sp->flip = op->flip;
	}
}

static void wheel_fall_check(OBJECT *op)
{
	if (op->fall_f == 0 && !(op->col_f & COL_D)) {
		op->act2 = MODE_FALL;
		op->act3 = 0;
		op->spd_y = 0;
		op->fall_f = -1;
	}
}

static void wheel_main(OBJECT *op)
{
	int i;

	last_spr(op) = op->anime.sprt;
	wheel_fall_check(op);
	(*wheel_mode[op->act2])(op);

	if (dir(op) == DIR_R) {
		if (op->spd_x > WHEEL_SPD)
			op->spd_x = WHEEL_SPD;
	} else {
		if (op->spd_x < -WHEEL_SPD)
			op->spd_x = -WHEEL_SPD;
	}

	if (ContactWeapon(op) < 0 || (op->col_f & (COL_U | COL_D)) == (COL_U | COL_D)) {
		for (i = 4; i >= 0; i--)
			SpawnExplosionRand(op, 1, 16, 16);
		SpawnJunk(3, junk_anime, op);
		DropItem(op, 20);
		op->act = ACT_DEAD;
		return;
	}

	ContactMega(op);
	if (OffScreenCheck2(op, 128, 128))
		op->act = ACT_GONE;
	else
		DisplayCheck(op, 96, 96);

	if (op->col_f & COL_D)
		op->fall_f = 0;
	else
		op->fall_f = (op->spd_y < 0) ? -1 : 1;
}

static void wheel_delete(OBJECT *op)
{
	DeleteObject(op);
}

static void wheel_delete2(OBJECT *op)
{
	DeleteObject2(op);
}

static void wheel_roll(OBJECT *op)
{
	u_char wall;

	wheel_move(op);
	AnimeTick(op);
	wall = (dir(op) == DIR_R) ? (op->col_f & COL_R) : (op->col_f & COL_L);
	if (wall) {
		PlaySound(SE_BANK_ENEMY, SE_WHEEL_HIT, op);
		op->x.w.lo = 0;
		op->spd_x = 0;
		op->acc_x = 0;
		op->act2 = MODE_TURN;
		op->act3 = 0;
	}
}

static void wheel_fall(OBJECT *op)
{
	AnimeTick(op);
	if (op->col_f & COL_D) {
		PlaySound(SE_BANK_ENEMY, SE_WHEEL_HIT, op);
		op->act2 = MODE_ROLL;
		op->act3 = 0;
		op->acc_x = -WHEEL_ACC;
		op->spd_y = 0;
		op->fall_f = 0;
	} else {
		wheel_move(op);
	}
}

static void wheel_turn(OBJECT *op)
{
	(*wheel_turn_tbl[op->act3])(op);
}

static void turn_start(OBJECT *op)
{
	op->timer = TURN_WAIT;
	op->act3++;
}

static void turn_wait(OBJECT *op)
{
	if (--op->timer == 0) {
		op->timer = TURN_FLASH;
		op->act3++;
	}
}

static void turn_flip(OBJECT *op)
{
	int n;

	if (--op->timer != 0)
		return;
	op->act2 = MODE_FALL;
	op->act3 = 0;
	if (dir(op) == DIR_R) {
		op->acc_x = WHEEL_ACC;
		SetAnime(op, 0);
		dir(op) = DIR_L;
		switch (last_spr(op)) {
		case 0: n = 4; break;
		case 1: n = 8; break;
		case 2: n = 12; break;
		case 3: n = 16; break;
		case 4: n = 20; break;
		case 5: n = 24; break;
		case 6: n = 28; break;
		default: n = 0; break;
		}
	} else {
		op->acc_x = -WHEEL_ACC;
		SetAnime(op, 2);
		dir(op) = DIR_R;
		switch (last_spr(op)) {
		case 1: n = 28; break;
		case 2: n = 24; break;
		case 3: n = 20; break;
		case 4: n = 16; break;
		case 5: n = 12; break;
		case 6: n = 8; break;
		case 7: n = 4; break;
		default: n = 0; break;
		}
	}
	while (n-- > 0)
		AnimeTick(op);
}

static void wheel_touch(OBJECT *ap, OBJECT *bp)
{
	short d;

	if (ap->col_box == NULL || bp->col_box == NULL)
		return;
	d = POS_X(ap) - POS_X(bp);
	if (d < 0)
		d = -d;
	if (d >= ap->col_box->w + bp->col_box->w)
		return;
	d = POS_Y(ap) - POS_Y(bp);
	if (d < 0)
		d = -d;
	if (d >= ap->col_box->h + bp->col_box->h)
		return;

	if (bp->fall_f) {
		if (ap->fall_f == 0)
			ap->col_f |= COL_U;
	} else if (ap->fall_f) {
		bp->col_f |= COL_U;
	}
	if (POS_X(bp) < POS_X(ap)) {
		bp->col_f |= COL_R;
		ap->col_f |= COL_L;
		return;
	}
	bp->col_f |= COL_L;
	ap->col_f |= COL_R;
}

static void wheel_pair_test(OBJECT *op)
{
	OBJECT *tp;

	for (tp = op + 1; tp < &main_object[MAIN_OBJECT_MAX]; tp++) {
		if (tp->flag && tp->type == op->type)
			wheel_touch(tp, op);
	}
}

void m02_wheel(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	if (op->col_f & COL_D)
		op->spd_y = 0;
	(*wheel_act[op->act])(op);
	CollideCheck(op);
	wheel_pair_test(op);
}

/* ---------------------------------------------------------------- */
/*	misc object 02 : stud lights                                      */
/* ---------------------------------------------------------------- */

static void studs_init(OBJECT *op)
{
	op->disp = 1;
	op->pri = 5;
	op->spd_y = 0;
	op->grav = 0;
	op->act++;
	DisplayCheck1(op);
}

static void studs_follow(OBJECT *op)
{
	OBJECT *wp = parent(op);

	op->anime.sprt = studs_spr[wp->anime.sprt * 4 - 1 + wp->anime.time];
	op->x = wp->x;
	op->y = wp->y;
	if (wp->act2 == MODE_TURN) {
		op->anime.sprt = studs_spr[wp->anime.sprt * 4 + 3];
		if (wp->act3 == 2) {
			switch (wp->anime.sprt) {
			case 0: case 4: SetAnime(op, 4); break;
			case 1: case 5: SetAnime(op, 5); break;
			case 2: case 6: SetAnime(op, 6); break;
			case 3: case 7: SetAnime(op, 7); break;
			}
			op->act++;
		}
	}
	if (OffScreenCheck2(op, 128, 128))
		DeleteObject3(op);
	else
		DisplayCheck(op, 96, 96);
	if (wp->act == ACT_DEAD)
		DeleteObject3(op);
}

static void studs_flash(OBJECT *op)
{
	OBJECT *wp = parent(op);

	if (wp->act2 != MODE_TURN)
		op->act = 1;
	AnimeTick(op);
	if (OffScreenCheck2(op, 128, 128))
		DeleteObject3(op);
	else
		DisplayCheck(op, 96, 96);
	if (wp->act == ACT_DEAD)
		DeleteObject3(op);
}

void e02_wheel_studs(OBJECT *op)
{
	if (op->act == 0)
		studs_init(op);
	else if (op->act == 1)
		studs_follow(op);
	else
		studs_flash(op);
}
