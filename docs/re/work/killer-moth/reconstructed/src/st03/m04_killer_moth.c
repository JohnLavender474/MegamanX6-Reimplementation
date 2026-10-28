/*
 *	m04_killer_moth.c
 *
 *	ST03 (Commander Yammark)  main object 04   : Killer Moth
 *	                          visual object 02 : its powder shot
 *	                          misc object 04   : its exhaust flame
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
	MODE_NOP,
	MODE_HOVER,
	MODE_SWOOP,
	MODE_CIRCLE,
	MODE_ENTER
};

enum {
	FLAME_OFF = 0,
	FLAME_DOWN,
	FLAME_DIAG,
	FLAME_SIDE
};

#define MOTH_HP		8
#define MOTH_ATK	4
#define HOVER_TIME	180
#define NEAR_X		48
#define SWOOP_H_X	48
#define SWOOP_H_ZERO	64
#define ARRIVE		3
#define CIRCLE_R	16
#define CIRCLE_SPIN	32
#define SHOT_TIME	20
#define SHOT_COUNT	8
#define POWDER_ATK	4
#define POWDER_FALL	0x3fff

#define shots(op)	(*(short *)&(op)->work[0x00])
#define angle(op)	(*(long *)&(op)->work[0x02])
#define spin(op)	(*(long *)&(op)->work[0x06])
#define flame_mode(op)	((op)->work[0x0a])
#define centre_x(op)	(*(short *)&(op)->work[0x0c])
#define centre_y(op)	(*(short *)&(op)->work[0x0e])

#define flame_parent(op)	(*(OBJECT **)&(op)->atk_box)
#define last_mode(op)		(*(u_char *)&(op)->dmg_box)
#define last_mode_w(op)		(*((short *)&(op)->dmg_box + 1))
#define powder_parent(op)	(*(OBJECT **)&(op)->timer)

#define ABS(a)		((a) < 0 ? -(a) : (a))

typedef struct {
	short	x;
	short	y;
} POINT16;

extern ANIME	*moth_anime[];
extern HITBOX	moth_atk_box, moth_dmg_box;
extern HITBOX	powder_atk_box, powder_col_box;
extern u_short	moth_delay[8];
extern POINT16	moth_point[16];

static void	moth_init(OBJECT *op);
static void	moth_main(OBJECT *op);
static void	moth_delete(OBJECT *op);
static void	moth_delete2(OBJECT *op);
static void	moth_nop(OBJECT *op);
static void	moth_hover(OBJECT *op);
static void	moth_swoop(OBJECT *op);
static void	moth_circle(OBJECT *op);
static void	moth_enter(OBJECT *op);
static void	hover_start(OBJECT *op);
static void	hover_wait(OBJECT *op);
static void	hover_windup(OBJECT *op);
static void	hover_windup_wait(OBJECT *op);
static void	swoop_start(OBJECT *op);
static void	swoop_move(OBJECT *op);
static void	circle_start(OBJECT *op);
static void	circle_move(OBJECT *op);
static void	enter_wait_view(OBJECT *op);
static void	enter_delay(OBJECT *op);
static void	enter_fly_home(OBJECT *op);
static void	moth_make_flame(OBJECT *op, u_char var);
static void	moth_make_powder(OBJECT *op, u_char var);
static void	moth_face_player(OBJECT *op);
static void	powder_init(OBJECT *op);
static void	powder_move(OBJECT *op);
static void	powder_delete(OBJECT *op);
static void	flame_init(OBJECT *op);
static void	flame_follow(OBJECT *op);
static void	flame_delete(OBJECT *op);

static void	(*moth_act[])(OBJECT *) = {
	moth_init, moth_main, moth_delete, moth_delete2
};

static void	(*moth_mode[])(OBJECT *) = {
	HitReactionDispatch, moth_nop, moth_hover, moth_swoop, moth_circle, moth_enter
};

static void	(*moth_hover_tbl[])(OBJECT *) = {
	hover_start, hover_wait, hover_windup, hover_windup_wait
};

static void	(*moth_swoop_tbl[])(OBJECT *) = {
	swoop_start, swoop_move
};

static void	(*moth_circle_tbl[])(OBJECT *) = {
	circle_start, circle_move
};

static void	(*moth_enter_tbl[])(OBJECT *) = {
	enter_wait_view, enter_delay, enter_fly_home
};

static void	(*powder_act[])(OBJECT *) = {
	powder_init, powder_move, powder_delete
};

static void	(*flame_act[])(OBJECT *) = {
	flame_init, flame_follow, flame_delete
};

static void swoop_move(OBJECT *op)
{
	short dx, dy;
	long a;

	dx = POS_X(&mega) - POS_X(op);
	if (mega.var == 0)
		dy = POS_Y(&mega) - (POS_Y(op) + SWOOP_H_X);
	else
		dy = POS_Y(&mega) - (POS_Y(op) + SWOOP_H_ZERO);
	a = ratan2(-dy, dx);
	op->acc_x = 0;
	op->grav = 0;
	op->spd_x = rcos(a) * 160 / 4;
	op->spd_y = rsin(a) * 160 / 8;
	MoveObject(op);

	if (mega.var == 0) {
		if (ABS(POS_X(&mega) - POS_X(op)) >= ARRIVE)
			return;
		if (ABS(POS_Y(&mega) - (POS_Y(op) + SWOOP_H_X)) >= ARRIVE)
			return;
		POS_X(op) = POS_X(&mega);
		POS_Y(op) = POS_Y(&mega) - SWOOP_H_X;
	} else {
		if (ABS(POS_X(&mega) - POS_X(op)) >= ARRIVE)
			return;
		if (ABS(POS_Y(&mega) - (POS_Y(op) + SWOOP_H_ZERO)) >= ARRIVE)
			return;
		POS_X(op) = POS_X(&mega);
		POS_Y(op) = POS_Y(&mega) - SWOOP_H_ZERO;
	}
	if (op->flip == OBJ_FLIP_L)
		spin(CUR_MAIN) = -CIRCLE_SPIN;
	else
		spin(CUR_MAIN) = CIRCLE_SPIN;
	op->act2 = MODE_CIRCLE;
	op->act3 = 0;
}

static void enter_wait_view(OBJECT *op)
{
	short d;

	d = bg_layer[0].scr_x - (POS_X(op) - 160);
	if (ABS(d) >= 160)
		return;
	d = bg_layer[0].y - (POS_Y(op) - 120);
	if (ABS(d) >= 120)
		return;
	op->timer = moth_delay[GetRNG() & 7];
	op->act3++;
}

static void enter_fly_home(OBJECT *op)
{
	long a;

	a = ratan2(POS_Y(op) - centre_y(CUR_MAIN), centre_x(CUR_MAIN) - POS_X(op));
	op->acc_x = 0;
	op->grav = 0;
	op->spd_x = rcos(a) * 160 / 8;
	op->spd_y = rsin(a) * 160 / 8;
	MoveObject(op);
	DisplayCheck(op, 48, 64);

	if (ABS(centre_x(CUR_MAIN) - POS_X(op)) >= ARRIVE)
		return;
	if (ABS(centre_y(CUR_MAIN) - POS_Y(op)) >= ARRIVE)
		return;
	POS_X(op) = centre_x(CUR_MAIN);
	op->act2 = MODE_HOVER;
	op->act3 = 0;
	POS_Y(op) = centre_y(CUR_MAIN);
}

void m04_killer_moth(OBJECT *op)
{
	(*moth_act[op->act])(op);
}

void v02_moth_powder(OBJECT *op)
{
	(*powder_act[op->act])(op);
}

void e04_moth_flame(OBJECT *op)
{
	(*flame_act[op->act])(op);
}

static void moth_init(OBJECT *op)
{
	op->atk = MOTH_ATK;
	op->timer = 0;
	op->hp = MOTH_HP;
	op->muteki = 0;
	op->disp = 0;
	if (op->var == 0) {
		flame_mode(CUR_MAIN) = FLAME_DOWN;
		op->act2 = MODE_HOVER;
		op->col_box = NULL;
		op->dmg_box = &moth_dmg_box;
		op->atk_box = &moth_atk_box;
	} else {
		flame_mode(CUR_MAIN) = FLAME_OFF;
		op->act2 = MODE_ENTER;
		op->col_box = NULL;
		op->dmg_box = NULL;
		op->atk_box = NULL;
	}
	op->anime_tbl = moth_anime;
	op->pri = 5;
	op->flip = OBJ_FLIP_L;
	op->fall_f = 0;
	op->dmg_tbl = dmg_tbl_shot;
	op->layer = mega.layer;
	op->old_x = op->x;
	op->old_y = op->y;
	SetAnime(op, 0);
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav = 0;
	op->act = ACT_MAIN;
	op->act3 = 0;
	moth_make_flame(op, 0);
}

static void moth_main(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	if (op->act2 != MODE_ENTER)
		moth_face_player(op);
	AnimeTick(op);
	(*moth_mode[op->act2])(op);

	if (ContactWeapon(op) < 0) {
		SpawnExplosion(op);
		DropItem(op, 18);
		op->act = ACT_DEAD;
		return;
	}
	ContactMega(op);
	if (OffScreenCheck2(op, 80, 160)) {
		op->act = ACT_GONE;
		return;
	}
	if (op->act2 != MODE_ENTER)
		DisplayCheck(op, 48, 64);
}

static void moth_delete(OBJECT *op)
{
	DeleteObject(op);
}

static void moth_delete2(OBJECT *op)
{
	DeleteObject2(op);
}

static void moth_hover(OBJECT *op)
{
	(*moth_hover_tbl[op->act3])(op);
}

static void hover_start(OBJECT *op)
{
	flame_mode(CUR_MAIN) = FLAME_DOWN;
	SetAnime(op, 0);
	op->timer = HOVER_TIME;
	op->act3++;
}

static void hover_wait(OBJECT *op)
{
	short d;

	if (--op->timer == 0)
		op->act3++;
	d = POS_X(&mega) - POS_X(op);
	if (ABS(d) < NEAR_X)
		op->act3++;
}

static void hover_windup(OBJECT *op)
{
	SetAnime(op, 2);
	flame_mode(CUR_MAIN) = FLAME_DIAG;
	op->act3++;
}

static void hover_windup_wait(OBJECT *op)
{
	if (op->anime.flag == ANM_FIRE) {
		op->act2 = MODE_SWOOP;
		op->act3 = 0;
	}
}

static void moth_swoop(OBJECT *op)
{
	(*moth_swoop_tbl[op->act3])(op);
}

static void swoop_start(OBJECT *op)
{
	flame_mode(CUR_MAIN) = FLAME_SIDE;
	SetAnime(op, 4);
	op->act3++;
}

static void moth_circle(OBJECT *op)
{
	(*moth_circle_tbl[op->act3])(op);
}

static void circle_start(OBJECT *op)
{
	centre_x(CUR_MAIN) = POS_X(op);
	centre_y(CUR_MAIN) = POS_Y(op) - CIRCLE_R;
	angle(CUR_MAIN) = 3072;
	SetAnime(op, 6);
	flame_mode(CUR_MAIN) = FLAME_DOWN;
	op->timer = SHOT_TIME;
	shots(op) = SHOT_COUNT;
	op->act3++;
}

static void circle_move(OBJECT *op)
{
	short cx, cy;

	angle(CUR_MAIN) += spin(CUR_MAIN);
	angle(CUR_MAIN) &= 0xfff;
	cx = (rcos(angle(CUR_MAIN)) << 4) >> 12;
	cy = (rsin(angle(CUR_MAIN)) << 4) >> 12;
	POS_X(op) = centre_x(CUR_MAIN) + cx;
	POS_Y(op) = centre_y(CUR_MAIN) - cy;

	if (--op->timer != 0)
		return;
	op->timer = SHOT_TIME;
	PlaySound(SE_BANK_ENEMY, SE_MOTH_POWDER, op);
	moth_make_powder(op, 0);
	if (--shots(op) == 0) {
		op->act2 = MODE_HOVER;
		op->act3 = 0;
	}
}

static void moth_nop(OBJECT *op)
{
}

static void moth_enter(OBJECT *op)
{
	(*moth_enter_tbl[op->act3])(op);
}

static void enter_delay(OBJECT *op)
{
	int i;

	if (--op->timer != -1)
		return;
	centre_x(CUR_MAIN) = POS_X(op);
	centre_y(CUR_MAIN) = POS_Y(op);
	flame_mode(CUR_MAIN) = FLAME_SIDE;
	SetAnime(op, 4);
	i = GetRNG() & 15;
	POS_X(op) = bg_layer[0].scr_x + moth_point[i].x;
	POS_Y(op) = bg_layer[0].y + moth_point[i].y;
	moth_face_player(op);
	op->dmg_box = &moth_dmg_box;
	op->atk_box = &moth_atk_box;
	op->act3++;
}

static void moth_make_flame(OBJECT *op, u_char var)
{
	OBJECT *sp;

	if ((sp = GetMiscObject()) == NULL)
		return;
	sp->flag = 0x41;
	sp->type = 4;
	sp->var = var;
	sp->flip = op->flip;
	sp->tpage = op->tpage;
	sp->clut = op->clut & ~CLUT_FLASH;
	sp->anime_tbl = op->anime_tbl;
	sp->sprt_p = op->sprt_p;
	sp->layer = op->layer;
	sp->x.w.lo = 0;
	sp->x.w.hi = POS_X(op);
	sp->y.w.lo = 0;
	flame_parent(sp) = op;
	sp->y.w.hi = POS_Y(op);
}

static void moth_make_powder(OBJECT *op, u_char var)
{
	OBJECT *sp;

	if ((sp = GetVisualObject()) == NULL)
		return;
	sp->flag = 0x41;
	sp->type = 2;
	powder_parent(sp) = op;
	sp->var = var;
	sp->flip = op->flip;
	sp->x.w.lo = 0;
	sp->x.w.hi = POS_X(op);
	sp->y.w.lo = 0;
	sp->y.w.hi = POS_Y(op);
	sp->clut = op->clut & ~CLUT_FLASH;
	sp->anime_tbl = op->anime_tbl;
	sp->sprt_p = op->sprt_p;
	sp->tpage = op->tpage;
	sp->layer = op->layer;
}

static void moth_face_player(OBJECT *op)
{
	if (POS_X(op) < POS_X(&mega))
		op->flip = OBJ_FLIP_R;
	else
		op->flip = OBJ_FLIP_L;
}

static void powder_init(OBJECT *op)
{
	op->atk = POWDER_ATK;
	op->hp = 0;
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav = 0;
	op->pri = 4;
	op->fall_f = 1;
	op->dmg_box = NULL;
	SetAnime(op, 7);
	op->spd_y = -POWDER_FALL;
	op->act = 1;
	op->spd_x = 0;
	op->acc_x = 0;
	op->grav = 0;
	op->atk_box = &powder_atk_box;
	op->col_box = &powder_col_box;
	op->act2 = 0;
	op->act3 = 0;
	op->act4 = 0;
	op->disp = 0;
}

static void powder_move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
	ContactMega(op);
	if (op->col_f == 0) {
		CornerCheck(op);
		if (op->col_f == 0) {
			if (!OffScreenCheck2(op, 32, 32)) {
				DisplayCheck(op, 16, 16);
				return;
			}
		}
	}
	op->act++;
}

static void powder_delete(OBJECT *op)
{
	DeleteObject3(op);
}

static void flame_init(OBJECT *op)
{
	last_mode(CUR_MISC) = 0xff;
	op->flip = 0;
	last_mode_w(CUR_MISC) = 0;
	op->pri = 6;
	SetAnime(op, 1);
	op->disp = 0;
	op->act2 = 0;
	op->spd_x = 0;
	op->acc_x = 0;
	op->spd_y = 0;
	op->grav = 0;
	op->act = 1;
}

static void flame_follow(OBJECT *op)
{
	OBJECT *mp;
	char act;
	u_char mode;

	act = flame_parent(op)->act;
	if (act != ACT_MAIN) {
		op->disp = 0;
		op->act++;
		return;
	}
	AnimeTick(op);
	mp = flame_parent(op);
	op->flip = mp->flip;
	POS_X(op) = POS_X(mp);
	POS_Y(op) = POS_Y(mp);
	mode = flame_mode(mp);
	if (last_mode(CUR_MISC) != mode) {
		switch (mode) {
		case FLAME_OFF:
			op->disp = 0;
			break;
		case FLAME_DOWN:
			op->disp = mode;
			SetAnime(op, 1);
			break;
		case FLAME_DIAG:
			op->disp = act;
			SetAnime(op, 3);
			break;
		case FLAME_SIDE:
			op->disp = act;
			SetAnime(op, 5);
			break;
		}
	}
	last_mode(CUR_MISC) = mode;
}

static void flame_delete(OBJECT *op)
{
	DeleteObject3(op);
}
