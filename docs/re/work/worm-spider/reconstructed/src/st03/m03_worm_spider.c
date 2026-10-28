/*
 *	m03_worm_spider.c
 *
 *	ST03 (Commander Yammark stage)  main object 03   : Worm Spider
 *	                                misc object 05   : debris, head spark, water ripple and spray
 *	                                visual object 03 : the energy ball
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
	MODE_WAIT,
	MODE_EMERGE,
	MODE_ATTACK,
	MODE_SWAY,
	MODE_RETRACT,
	MODE_JUMP
};

enum {
	WORM_FLOOR = 0,
	WORM_CEIL,
	WORM_WATER
};

enum {
	FX_DEBRIS_F = 0,
	FX_DEBRIS_C = 1,
	FX_SPARK_F = 2,
	FX_SPARK_C = 3,
	FX_RIPPLE = 16,
	FX_SPRAY = 17
};

#define WORM_HP		16
#define WORM_ATK	4
#define WORM_NEAR	96
#define SHOT_ATK	4
#define SHOT_SPD	80

#define own_box(op)	(*(HITBOX *)&(op)->work[2])
#define base_y(op)	(*(short *)&(op)->work[6])
#define jumps(op)	((op)->work[8])
#define hit_f(op)	((op)->work[9])
#define parent(op)	(*(OBJECT **)&(op)->atk_box)
#define fx_timer(op)	(*(short *)&(op)->dmg_box)
#define owner(op)	(*(OBJECT **)&(op)->timer)

extern ANIME	*worm_anime[];
extern HITBOX	worm_atk_box, worm_dmg_box;
extern HITBOX	shot_col_box, shot_atk_box, shot_dmg_box;
extern u_char	worm_box_h[];

static void	worm_init(OBJECT *op);
static void	worm_main(OBJECT *op);
static void	worm_delete(OBJECT *op);
static void	worm_delete2(OBJECT *op);
static void	worm_nop(OBJECT *op);
static void	worm_wait(OBJECT *op);
static void	worm_emerge(OBJECT *op);
static void	worm_attack(OBJECT *op);
static void	worm_sway(OBJECT *op);
static void	worm_retract(OBJECT *op);
static void	worm_jump(OBJECT *op);
static void	wait_start(OBJECT *op);
static void	wait_near(OBJECT *op);
static void	emerge_start(OBJECT *op);
static void	emerge_grow(OBJECT *op);
static void	emerge_wait(OBJECT *op);
static void	attack_start(OBJECT *op);
static void	attack_fire(OBJECT *op);
static void	attack_wait(OBJECT *op);
static void	sway_start(OBJECT *op);
static void	sway_wait(OBJECT *op);
static void	retract_start(OBJECT *op);
static void	retract_anim(OBJECT *op);
static void	retract_wait(OBJECT *op);
static void	jump_start(OBJECT *op);
static void	jump_air(OBJECT *op);
static void	jump_lob(OBJECT *op);
static void	jump_fall(OBJECT *op);
static void	fx_spawn(OBJECT *op, u_char var);
static void	fx_spawn_y(OBJECT *op, short y, u_char var);
static void	worm_face(OBJECT *op);
static void	shot_init(OBJECT *op);
static void	shot_main(OBJECT *op);
static void	shot_delete(OBJECT *op);
static void	fx_init(OBJECT *op);
static void	fx_main(OBJECT *op);
static void	fx_delete(OBJECT *op);
static void	fx_blink(OBJECT *op);

static void	(*worm_act[])(OBJECT *) = {
	worm_init, worm_main, worm_delete, worm_delete2
};

static void	(*worm_mode[])(OBJECT *) = {
	HitReactionDispatch, worm_nop, worm_wait, worm_emerge,
	worm_attack, worm_sway, worm_retract, worm_jump
};

static void	(*worm_wait_tbl[])(OBJECT *) = { wait_start, wait_near };
static void	(*worm_emerge_tbl[])(OBJECT *) = { emerge_start, emerge_grow, emerge_wait };
static void	(*worm_attack_tbl[])(OBJECT *) = { attack_start, attack_fire, attack_wait };
static void	(*worm_sway_tbl[])(OBJECT *) = { sway_start, sway_wait };
static void	(*worm_retract_tbl[])(OBJECT *) = { retract_start, retract_anim, retract_wait };
static void	(*worm_jump_tbl[])(OBJECT *) = { jump_start, jump_air, jump_lob, jump_fall };

static void	(*shot_act[])(OBJECT *) = { shot_init, shot_main, shot_delete };
static void	(*fx_act[])(OBJECT *) = { fx_init, fx_main, fx_delete, fx_blink };

static void worm_init(OBJECT *op)
{
	op->atk = WORM_ATK;
	op->anime_tbl = worm_anime;
	op->hp = WORM_HP;
	op->muteki = 0;
	op->disp = 0;
	hit_f(cur_main) = 0;
	op->col_box = NULL;
	op->dmg_box = NULL;
	op->atk_box = NULL;
	op->layer = mega.layer;
	op->flip = OBJ_FLIP_L;
	op->pri = 7;
	op->fall_f = 0;
	op->dmg_tbl = dmg_tbl_mecha_s;
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav = 0;
	op->old_x = op->x;
	op->old_y = op->y;
	worm_face(op);
	op->act = ACT_MAIN;
	op->act3 = 0;
	own_box(cur_main) = worm_dmg_box;

	if (op->var == WORM_FLOOR) {
		SetAnime(op, 0);
		op->act2 = MODE_WAIT;
		op->y.w.hi -= 32;
	} else if (op->var == WORM_CEIL) {
		SetAnime(op, 14);
		op->act2 = MODE_WAIT;
		op->y.w.hi += 32;
	} else if (op->var == WORM_WATER) {
		SetAnime(op, 7);
		jumps(cur_main) = 3;
		base_y(cur_main) = op->y.w.hi;
		fx_spawn(op, FX_RIPPLE);
		op->act2 = MODE_JUMP;
		op->col_box = NULL;
		op->atk_box = &worm_atk_box;
		op->dmg_box = &worm_dmg_box;
		op->y.w.hi += 26;
	}
}

static void emerge_start(OBJECT *op)
{
	PlaySound(SE_BANK_ENEMY, SE_WORM_MOVE, op);
	op->atk_box = &own_box(cur_main);
	op->dmg_box = &own_box(cur_main);
	if (op->var == WORM_FLOOR) {
		SetAnime(op, 2);
		own_box(cur_main).oy = 26 - worm_box_h[op->anime.flag - 1];
		own_box(cur_main).h = worm_box_h[op->anime.flag - 1];
		fx_spawn(op, FX_DEBRIS_F);
	} else if (op->var == WORM_CEIL) {
		SetAnime(op, 16);
		own_box(cur_main).oy = -30;
		own_box(cur_main).h = worm_box_h[op->anime.flag - 1];
		fx_spawn(op, FX_DEBRIS_C);
	}
	op->act3++;
}

static void emerge_grow(OBJECT *op)
{
	if (op->var == WORM_FLOOR) {
		own_box(cur_main).oy = 26 - worm_box_h[op->anime.flag - 1];
		own_box(cur_main).h = worm_box_h[op->anime.flag - 1];
	} else if (op->var == WORM_CEIL) {
		own_box(cur_main).oy = -30;
		own_box(cur_main).h = worm_box_h[op->anime.flag - 1];
	}
	if (op->anime.flag != 28)
		return;

	op->atk_box = &worm_atk_box;
	op->dmg_box = &worm_dmg_box;
	if (op->var == WORM_FLOOR)
		SetAnime(op, 4);
	else if (op->var == WORM_CEIL)
		SetAnime(op, 18);
	op->timer = (game.level < LEVEL_HARD) ? 30 : 15;
	op->act3++;
}

static void shot_init(OBJECT *op)
{
	long a;

	op->atk = SHOT_ATK;
	op->hp = 1;
	op->dmg_tbl = dmg_tbl_guard;
	op->pri = 5;
	op->fall_f = 1;
	SetAnime(op, 10);
	op->col_box = &shot_col_box;
	op->dmg_box = &shot_dmg_box;
	op->atk_box = &shot_atk_box;
	op->act = 1;
	op->act2 = 0;
	op->act3 = 0;
	op->act4 = 0;
	a = ratan2(POS_Y(op) - POS_Y(&mega), POS_X(&mega) - POS_X(op));
	op->acc_x = 0;
	op->grav = 0;
	op->spd_x = rcos(a) * SHOT_SPD;
	op->spd_y = rsin(a) * SHOT_SPD;
	if (OffScreenCheck2(op, 32, 32))
		op->act++;
	else
		DisplayCheck(op, 16, 16);
}

static void fx_init(OBJECT *op)
{
	OBJECT *wp = parent(op);

	op->pri = 6;
	op->flip = wp->flip;
	switch (op->var) {
	case FX_DEBRIS_F:
		POS_X(op) = POS_X(wp);
		POS_Y(op) = POS_Y(wp);
		SetAnime(op, 1);
		break;
	case FX_DEBRIS_C:
		POS_X(op) = POS_X(wp);
		POS_Y(op) = POS_Y(wp);
		SetAnime(op, 15);
		break;
	case FX_SPARK_F:
		POS_X(op) = POS_X(wp);
		POS_Y(op) = POS_Y(wp);
		SetAnime(op, 9);
		if (op->flip == OBJ_FLIP_L)
			POS_X(op) -= 12;
		else
			POS_X(op) += 12;
		POS_Y(op) -= 30;
		break;
	case FX_SPARK_C:
		POS_X(op) = POS_X(wp);
		POS_Y(op) = POS_Y(wp);
		SetAnime(op, 9);
		if (op->flip == OBJ_FLIP_L)
			POS_X(op) += 12;
		else
			POS_X(op) -= 12;
		POS_Y(op) += 30;
		break;
	case FX_RIPPLE:
		SetAnime(op, 11);
		break;
	case FX_SPRAY:
		op->pri = 5;
		SetAnime(op, 13);
		break;
	}
	op->spd_x = 0;
	op->acc_x = 0;
	op->spd_y = 0;
	op->grav = 0;
	op->act = 1;
	op->act2 = 0;
}

static void fx_main(OBJECT *op)
{
	OBJECT *sp;

	DisplayCheck(op, 16, 16);
	if (parent(op)->act != ACT_MAIN) {
		if (op->var != FX_RIPPLE)
			goto kill;
		op->act = 3;
		fx_timer(cur_misc) = 60;
		return;
	}

	AnimeTick(op);
	switch (op->var) {
	case FX_DEBRIS_F:
	case FX_DEBRIS_C:
		if (parent(op)->anime.flag == 27)
			goto kill;
		break;
	case FX_SPARK_F:
	case FX_SPARK_C:
		if (op->anime.flag == 1)
			goto kill;
		if (op->anime.flag != 2)
			break;
		if ((sp = GetVisualObject()) == NULL)
			break;
		sp->flag = 0x41;
		sp->type = 3;
		sp->flip = op->flip;
		sp->var = 0;
		owner(sp) = parent(op);
		sp->x.w.lo = 0;
		POS_X(sp) = POS_X(op);
		sp->y.w.lo = 0;
		POS_Y(sp) = POS_Y(op);
		sp->clut = op->clut & ~CLUT_FLASH;
		sp->anime_tbl = op->anime_tbl;
		sp->sprt_p = op->sprt_p;
		sp->tpage = op->tpage;
		sp->layer = op->layer;
		break;
	case FX_RIPPLE:
		if (parent(op)->anime.flag == 1)
			SetAnime(op, 12);
		break;
	case FX_SPRAY:
		if (op->anime.flag == 1)
			goto kill;
		break;
	}
	return;

kill:
	op->disp = 0;
	op->act++;
}

void m03_worm_spider(OBJECT *op)
{
	(*worm_act[op->act])(op);
}

void v03_worm_shot(OBJECT *op)
{
	(*shot_act[op->act])(op);
}

void e05_worm_fx(OBJECT *op)
{
	(*fx_act[op->act])(op);
}

static void worm_main(OBJECT *op)
{
	int r;

	op->old_x = op->x;
	op->old_y = op->y;
	AnimeTick(op);
	(*worm_mode[op->act2])(op);

	r = ContactWeapon(op);
	if (r > 0) {
		hit_f(cur_main) = 1;
	} else if (r < 0) {
		SpawnExplosion(op);
		DropItem(op, 18);
		op->act = ACT_DEAD;
		return;
	} else {
		hit_f(cur_main) = 0;
	}

	ContactMega(op);
	if (OffScreenCheck2(op, 80, 160))
		op->act = ACT_GONE;
	else
		DisplayCheck(op, 48, 64);
}

static void worm_delete(OBJECT *op)
{
	DeleteObject(op);
}

static void worm_delete2(OBJECT *op)
{
	DeleteObject2(op);
}

static void worm_wait(OBJECT *op)
{
	(*worm_wait_tbl[op->act3])(op);
}

static void wait_start(OBJECT *op)
{
	if (op->var == WORM_FLOOR)
		SetAnime(op, 0);
	else if (op->var == WORM_CEIL)
		SetAnime(op, 14);
	op->act3++;
}

static void wait_near(OBJECT *op)
{
	short d;

	d = POS_X(&mega) - POS_X(op);
	if (d < 0)
		d = -d;
	if (d > WORM_NEAR)
		return;
	d = POS_Y(&mega) - POS_Y(op);
	if (d < 0)
		d = -d;
	if (d > WORM_NEAR)
		return;
	op->act2 = MODE_EMERGE;
	op->act3 = 0;
}

static void worm_emerge(OBJECT *op)
{
	(*worm_emerge_tbl[op->act3])(op);
}

static void emerge_wait(OBJECT *op)
{
	if (--op->timer == 0) {
		op->act2 = MODE_ATTACK;
		op->act3 = 0;
	}
}

static void worm_attack(OBJECT *op)
{
	(*worm_attack_tbl[op->act3])(op);
}

static void attack_start(OBJECT *op)
{
	if (op->var == WORM_FLOOR)
		SetAnime(op, 8);
	else if (op->var == WORM_CEIL)
		SetAnime(op, 21);
	op->timer = 30;
	op->act3++;
}

static void attack_fire(OBJECT *op)
{
	if (--op->timer != 0)
		return;
	PlaySound(SE_BANK_ENEMY, SE_WORM_SHOT, op);
	if (op->var == WORM_FLOOR)
		fx_spawn(op, FX_SPARK_F);
	else if (op->var == WORM_CEIL)
		fx_spawn(op, FX_SPARK_C);
	op->timer = 20;
	op->act3++;
}

static void attack_wait(OBJECT *op)
{
	if (--op->timer == 0) {
		op->act2 = MODE_SWAY;
		op->act3 = 0;
	}
}

static void worm_sway(OBJECT *op)
{
	(*worm_sway_tbl[op->act3])(op);
}

static void sway_start(OBJECT *op)
{
	if (op->var == WORM_FLOOR)
		SetAnime(op, 5);
	else if (op->var == WORM_CEIL)
		SetAnime(op, 19);
	op->timer = (game.level < LEVEL_HARD) ? 90 : 45;
	op->act3++;
}

static void sway_wait(OBJECT *op)
{
	if (hit_f(cur_main) == 1) {
		op->act2 = MODE_RETRACT;
		op->act3 = 0;
	}
	if (--op->timer == 0) {
		op->act2 = MODE_ATTACK;
		op->act3 = 0;
	}
}

static void worm_retract(OBJECT *op)
{
	(*worm_retract_tbl[op->act3])(op);
}

static void retract_start(OBJECT *op)
{
	PlaySound(SE_BANK_ENEMY, SE_WORM_MOVE, op);
	op->col_box = NULL;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	if (op->var == WORM_FLOOR)
		SetAnime(op, 22);
	else if (op->var == WORM_CEIL)
		SetAnime(op, 23);
	op->act3++;
}

static void retract_anim(OBJECT *op)
{
	if (op->anime.flag == 1) {
		op->timer = 120;
		op->act3++;
	}
}

static void retract_wait(OBJECT *op)
{
	if (--op->timer == 0) {
		op->act2 = MODE_EMERGE;
		op->act3 = 0;
	}
}

static void worm_jump(OBJECT *op)
{
	(*worm_jump_tbl[op->act3])(op);
}

static void jump_start(OBJECT *op)
{
	SetAnime(op, 7);
	POS_Y(op) = base_y(cur_main) - 26;
	fx_spawn_y(op, base_y(cur_main), FX_SPRAY);
	op->spd_x = 0;
	op->acc_x = 0;
	op->spd_y = 0x88888;
	op->grav = 0x48d1;
	MoveObject2(op);
	op->act3++;
}

static void jump_air(OBJECT *op)
{
	MoveObject2(op);
	if (op->anime.flag != 3)
		return;
	if (--jumps(cur_main) != 0)
		op->act3 = 0;
	else
		op->act3++;
}

static void jump_lob(OBJECT *op)
{
	SetAnime(op, 7);
	POS_Y(op) = base_y(cur_main) - 26;
	fx_spawn_y(op, base_y(cur_main), FX_SPRAY);
	op->spd_y = 0x80000;
	op->acc_x = 0;
	op->grav = 0x8000;
	op->spd_x = (POS_X(&mega) - POS_X(op)) << 11;
	MoveObject2(op);
	op->act3++;
}

static void jump_fall(OBJECT *op)
{
	MoveObject2(op);
}

static void worm_nop(OBJECT *op)
{
}

static void fx_spawn(OBJECT *op, u_char var)
{
	OBJECT *sp;

	if ((sp = GetMiscObject()) == NULL)
		return;
	sp->flag = 0x41;
	sp->type = 5;
	sp->var = var;
	sp->flip = op->flip;
	sp->tpage = op->tpage;
	sp->clut = op->clut & ~CLUT_FLASH;
	sp->anime_tbl = op->anime_tbl;
	sp->sprt_p = op->sprt_p;
	sp->layer = op->layer;
	sp->x.w.lo = 0;
	POS_X(sp) = POS_X(op);
	sp->y.w.lo = 0;
	parent(sp) = op;
	POS_Y(sp) = POS_Y(op);
}

static void fx_spawn_y(OBJECT *op, short y, u_char var)
{
	OBJECT *sp;

	if ((sp = GetMiscObject()) == NULL)
		return;
	sp->flag = 0x41;
	sp->type = 5;
	sp->var = var;
	sp->flip = op->flip;
	sp->tpage = op->tpage;
	sp->clut = op->clut & ~CLUT_FLASH;
	sp->anime_tbl = op->anime_tbl;
	sp->sprt_p = op->sprt_p;
	sp->layer = op->layer;
	sp->x.w.lo = 0;
	POS_Y(sp) = y;
	sp->y.w.lo = 0;
	parent(sp) = op;
	POS_X(sp) = POS_X(op);
}

static void worm_face(OBJECT *op)
{
	if (POS_X(op) < POS_X(&mega))
		op->flip = OBJ_FLIP_R;
	else
		op->flip = OBJ_FLIP_L;
}

static void shot_main(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
	if (ContactWeapon(op) < 0) {
		SpawnExplosion(op);
		op->act++;
	} else {
		op->clut &= ~CLUT_FLASH;
	}
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
	SpawnHitEffect(op);
	op->act++;
}

static void shot_delete(OBJECT *op)
{
	DeleteObject3(op);
}

static void fx_delete(OBJECT *op)
{
	DeleteObject3(op);
}

static void fx_blink(OBJECT *op)
{
	AnimeTick(op);
	if (--fx_timer(cur_misc) == 0) {
		DeleteObject3(op);
		return;
	}
	op->disp = (fx_timer(cur_misc) & 1) ? 1 : 0;
}
