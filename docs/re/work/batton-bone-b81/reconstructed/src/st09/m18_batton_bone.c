/*
 *	m18_batton_bone.c
 *
 *	ST09 (Rainy Turtloid)  main object 18 : Batton Bone B81
 *	                      visual object 17 : its bomb
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
	MODE_RESUME,
	MODE_CHASE,
	MODE_DROP,
	MODE_FLYAWAY,
	MODE_STRAIGHT,
	MODE_SCATTER,
	MODE_HOMING,
	MODE_RISE
};

#define BAT_HP		3
#define BAT_ATK		3
#define CHASE_TIME	200
#define CHASE_HEIGHT	0x500000
#define CHASE_NEAR	0x20000
#define DROP_WAIT1	5
#define DROP_WAIT2	13
#define FLYAWAY_SPD_Y	0x5000
#define SCATTER_SPD_Y	0x18000
#define SCATTER_ACC_X	(-0x1000)
#define SCATTER_LIFT	0x100
#define HOMING_TIME	300
#define RISE_SPD	0x20000

#define BOMB_HP		1
#define BOMB_ATK	5
#define BOMB_SPD_Y	(-0x10000)
#define BOMB_GRAV	0x3000

#define drop_timer(op)	(*(long *)&(op)->work[2])
#define no_bomb(op)	(*(long *)&(op)->work[10])
#define aim_count(op)	(*(long *)&(op)->work[18])
#define resume(op)	(*(long *)&(op)->work[22])
#define bomb_parent(op)	(*(OBJECT **)&(op)->timer)

#define FLIP_IDX(op)	((((op)->flip >> 4) & 4) >> 2)

extern ANIME	*bat_anime[];
extern HITBOX	bat_body_box, bat_bomb_box, bat_atk_box;
extern HITBOX	bomb_col_box, bomb_box;

static long	bat_fly_spd[2] = { -0x30000, 0x30000 };
static long	bat_scatter_spd[2] = { -0x38000, 0x38000 };
static u_char	junk_anime[] = { 4, 5, 6, 7 };
static HITBOX	*bat_dmg_box_tbl[] = { &bat_body_box, &bat_bomb_box };

static void	bat_init(OBJECT *op);
static void	bat_main(OBJECT *op);
static void	bat_delete(OBJECT *op);
static void	bat_resume(OBJECT *op);
static void	bat_chase(OBJECT *op);
static void	chase_start(OBJECT *op);
static void	chase_move(OBJECT *op);
static void	bat_drop(OBJECT *op);
static void	drop_start(OBJECT *op);
static void	drop_release(OBJECT *op);
static void	drop_end(OBJECT *op);
static void	bat_flyaway(OBJECT *op);
static void	flyaway_start(OBJECT *op);
static void	flyaway_move(OBJECT *op);
static void	bat_straight(OBJECT *op);
static void	straight_start(OBJECT *op);
static void	straight_move(OBJECT *op);
static void	bat_scatter(OBJECT *op);
static void	scatter_start(OBJECT *op);
static void	scatter_wait(OBJECT *op);
static void	scatter_move(OBJECT *op);
static void	bat_rise(OBJECT *op);
static void	bat_homing(OBJECT *op);
static void	homing_start(OBJECT *op);
static void	homing_move(OBJECT *op);
static void	bat_face(OBJECT *op);
static void	bomb_spawn(OBJECT *op);

static void	(*bat_act[])(OBJECT *) = {
	bat_init, bat_main, bat_delete, bat_delete
};

static void	(*bat_mode[])(OBJECT *) = {
	HitReactionDispatch, bat_resume, bat_chase, bat_drop, bat_flyaway,
	bat_straight, bat_scatter, bat_homing, bat_rise
};

static void	(*bat_chase_tbl[])(OBJECT *) = { chase_start, chase_move };
static void	(*bat_drop_tbl[])(OBJECT *) = { drop_start, drop_release, drop_end };
static void	(*bat_flyaway_tbl[])(OBJECT *) = { flyaway_start, flyaway_move };
static void	(*bat_straight_tbl[])(OBJECT *) = { straight_start, straight_move };
static void	(*bat_scatter_tbl[])(OBJECT *) = { scatter_start, scatter_wait, scatter_move };
static void	(*bat_homing_tbl[])(OBJECT *) = { homing_start, homing_move };

static void bat_init(OBJECT *op)
{
	op->hp = BAT_HP;
	op->atk = BAT_ATK;
	op->flag = 0x41;
	op->muteki = 0;
	op->dmg_tbl = dmg_tbl_mecha_s;
	op->anime_tbl = bat_anime;
	op->pri = 6;
	op->dmg_box = &bat_body_box;
	op->atk_box = &bat_atk_box;
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav = 0;
	op->fall_f = 0;
	op->col_box = NULL;
	drop_timer(op) = 0;
	*(long *)&op->work[6] = 0;
	no_bomb(op) = 0;
	*(long *)&op->work[14] = 0;
	aim_count(op) = 0;
	resume(op) = 0;
	op->layer = mega.layer;
	op->old_x = op->x;
	op->old_y = op->y;
	op->act++;

	switch (op->var) {
	case 0:
		SetAnime(op, 0);
		op->act2 = MODE_CHASE;
		break;
	case 1:
		SetAnime(op, 3);
		op->act2 = MODE_STRAIGHT;
		no_bomb(op) = 1;
		break;
	case 4:
		SetAnime(op, 3);
		op->act2 = MODE_HOMING;
		no_bomb(op) = 1;
		break;
	default:
		SetAnime(op, 3);
		op->act2 = MODE_SCATTER;
		no_bomb(op) = 1;
		break;
	}
	op->act3 = 0;
}

static void bat_main(OBJECT *op)
{
	u_char i;
	int r;

	op->old_x = op->x;
	op->old_y = op->y;
	(*bat_mode[op->act2])(op);

	r = (u_char)ContactMega(op);
	if (op->var == 4 && r == 1) {
		op->act2 = MODE_RISE;
		op->act3 = 0;
	}
	if (op->act2 != MODE_REACT)
		resume(op) = op->act2;

	for (i = 0; i < 2 - no_bomb(op); i++) {
		op->dmg_box = bat_dmg_box_tbl[i];
		r = ContactWeapon(op);
		if (r < 0) {
			if (i == 0 && no_bomb(op) == 0)
				bomb_spawn(op);
			SpawnExplosion(op);
			SpawnJunk(4, junk_anime, op);
			DropItem(op, 20);
			op->act++;
			return;
		}
		if (r != 0)
			break;
	}

	if (OffScreenCheck2(op, 32, 32))
		op->act = ACT_GONE;
	else
		DisplayCheck(op, 32, 32);
}

void m18_batton_bone(OBJECT *op)
{
	(*bat_act[op->act])(op);
}

static void bat_delete(OBJECT *op)
{
	if (STAGE_AREA() == 2 || op->act == ACT_GONE)
		DeleteObject2(op);
	else
		DeleteObject(op);
}

static void bat_resume(OBJECT *op)
{
	op->act2 = resume(op);
}

static void bat_chase(OBJECT *op)
{
	bat_face(op);
	(*bat_chase_tbl[op->act3])(op);
}

static void chase_start(OBJECT *op)
{
	AnimeTick(op);
	op->timer = CHASE_TIME;
	op->act3++;
}

static void chase_move(OBJECT *op)
{
	long sx, dx;

	AnimeTick(op);
	MoveObject(op);
	SetSpeedDir(op, AimAtPoint(op, mega.x.l, mega.y.l - CHASE_HEIGHT));
	op->spd_x += op->spd_x / 2;

	sx = op->spd_x;
	if (sx < 0)
		sx = -sx;
	dx = mega.x.l - op->x.l;
	if (dx < 0)
		dx = -dx;
	if (sx + CHASE_NEAR < dx) {
		if (--op->timer != 0)
			return;
	}
	op->act2 = MODE_DROP;
	op->act3 = 0;
}

static void bat_drop(OBJECT *op)
{
	(*bat_drop_tbl[op->act3])(op);
}

static void drop_start(OBJECT *op)
{
	SetAnime(op, 1);
	drop_timer(op) = DROP_WAIT1;
	op->spd_x = 0;
	op->dmg_box = &bat_body_box;
	op->atk_box = &bat_body_box;
	op->act3++;
}

static void drop_release(OBJECT *op)
{
	AnimeTick(op);
	if (--drop_timer(op) == 0) {
		no_bomb(op) = 1;
		bomb_spawn(op);
		drop_timer(op) = DROP_WAIT2;
		op->act3++;
	}
}

static void drop_end(OBJECT *op)
{
	AnimeTick(op);
	if (--drop_timer(op) == 0) {
		SetAnime(op, 3);
		op->act2 = MODE_FLYAWAY;
		op->act3 = 0;
	}
}

static void bat_flyaway(OBJECT *op)
{
	(*bat_flyaway_tbl[op->act3])(op);
}

static void flyaway_start(OBJECT *op)
{
	SetAnime(op, 3);
	op->spd_y = FLYAWAY_SPD_Y;
	op->act3++;
	op->spd_x = bat_fly_spd[FLIP_IDX(op)];
}

static void flyaway_move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
}

static void bat_straight(OBJECT *op)
{
	(*bat_straight_tbl[op->act3])(op);
}

static void straight_start(OBJECT *op)
{
	SetAnime(op, 3);
	op->atk_box = &bat_body_box;
	op->spd_y = 0;
	no_bomb(op) = 1;
	op->act3++;
	op->spd_x = bat_fly_spd[FLIP_IDX(op)];
}

static void straight_move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
}

static void bat_scatter(OBJECT *op)
{
	(*bat_scatter_tbl[op->act3])(op);
}

static void scatter_start(OBJECT *op)
{
	u_short r;

	SetAnime(op, 3);
	r = GetRNG();
	r = (r << 8) + GetRNG();
	op->spd_x = bat_scatter_spd[FLIP_IDX(op)];
	if (GetRNG() & 1)
		op->spd_x += r;
	else
		op->spd_x -= r;
	op->acc_x = SCATTER_ACC_X;
	op->spd_y = SCATTER_SPD_Y;
	op->atk_box = &bat_body_box;
	no_bomb(op) = 1;
	if (op->var == 2) {
		op->timer = GetRNG() & 0x3f;
		op->act3++;
	} else {
		op->act3 += 2;
	}
}

static void scatter_wait(OBJECT *op)
{
	if (op->timer == 0)
		op->act3++;
	else
		op->timer--;
}

static void scatter_move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
	op->spd_y += SCATTER_LIFT;
}

static void bat_rise(OBJECT *op)
{
	op->spd_x = 0;
	op->spd_y = RISE_SPD;
	MoveObject(op);
	AnimeTick(op);
}

static void bat_homing(OBJECT *op)
{
	bat_face(op);
	(*bat_homing_tbl[op->act3])(op);
}

static void homing_start(OBJECT *op)
{
	op->atk_box = &bat_body_box;
	SetAnime(op, 3);
	AnimeTick(op);
	no_bomb(op) = 1;
	op->timer = HOMING_TIME;
	op->act3++;
}

static void homing_move(OBJECT *op)
{
	AnimeTick(op);
	if (--op->timer == 0) {
		op->act2 = MODE_RISE;
		op->act3 = 0;
	}
	aim_count(op) = (u_char)(aim_count(op) + 1) & 3;
	if ((u_char)aim_count(op) == 0)
		SetSpeedDir(op, AimAtPoint(op, mega.x.l, mega.y.l));
	MoveObject(op);
}

static void bat_face(OBJECT *op)
{
	if (mega.x.l < op->x.l)
		op->flip = OBJ_FLIP_L;
	else
		op->flip = OBJ_FLIP_R;
}

static void bomb_spawn(OBJECT *op)
{
	OBJECT *bp;

	if ((bp = GetVisualObject()) == NULL)
		return;
	bp->flag = 0x41;
	bp->type = 17;
	bp->var = 0;
	bp->tpage = op->tpage;
	bp->clut = op->clut;
	bp->anime_tbl = op->anime_tbl;
	bp->sprt_p = op->sprt_p;
	bp->layer = op->layer;
	bp->flip = op->flip;
	bp->x = op->x;
	bomb_parent(bp) = op;
	bp->pri = 6;
	bp->y = op->y;
}

/* ---------------------------------------------------------------- */
/*	visual object 17 : the bomb                                       */
/* ---------------------------------------------------------------- */

static void	bomb_init(OBJECT *op);
static void	bomb_main(OBJECT *op);
static void	bomb_delete(OBJECT *op);
static void	bomb_idle(OBJECT *op);

static void	(*bomb_act[])(OBJECT *) = {
	bomb_init, bomb_main, bomb_delete, bomb_idle
};

void v17_batton_bomb(OBJECT *op)
{
	(*bomb_act[op->act])(op);
}

static void bomb_init(OBJECT *op)
{
	op->spd_y = BOMB_SPD_Y;
	op->grav = BOMB_GRAV;
	op->col_box = &bomb_col_box;
	op->dmg_tbl = dmg_tbl_shot;
	op->dmg_box = &bomb_box;
	op->atk_box = &bomb_box;
	op->atk = BOMB_ATK;
	op->disp = 1;
	op->hp = BOMB_HP;
	op->spd_x = 0;
	op->acc_x = 0;
	op->act++;
	op->clut &= ~CLUT_FLASH;
	SetAnime(op, 2);
}

static void bomb_main(OBJECT *op)
{
	AnimeTick(op);
	MoveObject2(op);
	if (ContactWeapon(op) < 0) {
		SpawnExplosion(op);
		op->act++;
		return;
	}
	if (ContactMega(op)) {
		SpawnExplosion(op);
		op->act = 2;
		return;
	}
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
	op->y.w.hi += 32;
	SpawnExplosion(op);
	op->act++;
}

static void bomb_delete(OBJECT *op)
{
	DeleteObject3(op);
}

static void bomb_idle(OBJECT *op)
{
}
