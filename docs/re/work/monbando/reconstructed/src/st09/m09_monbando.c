/*
 *	m09_monbando.c
 *
 *	ST09 (Rainy Turtloid)  main object 09 : Monbando
 *	                  visual object 07 : its ground waves, missile and punch box
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
	MODE_WAIT,
	MODE_FIRE,
	MODE_RECOVER,
	MODE_FALL
};

enum {
	KIND_WAVE = 0,
	KIND_MISSILE,
	KIND_PUNCH
};

enum {
	ANM_LAND = 0,
	ANM_GUARD,
	ANM_PUNCH,
	ANM_MISSILE,
	ANM_SLAM,
	ANM_WAVE,
	ANM_ROCKET
};

#define MONB_HP		32
#define MONB_ATK	6
#define MONB_GRAV	0x4200
#define MONB_WAIT	61
#define WAVE_NUM	3
#define WAVE_GAP	6

#define WAVE_SPD	0x28000
#define WAVE_ATK	4
#define MISSILE_SPD	0x30000
#define MISSILE_HP	6
#define MISSILE_ATK	4
#define MISSILE_TIME	50
#define MISSILE_AIM	40
#define PUNCH_ATK	8

#define kind(op)	((op)->act3)
#define delay(op)	((op)->act3)
#define parent(op)	(*(OBJECT **)&(op)->timer)
#define aim_t(op)	(*(short *)&(op)->work[0x0e])

extern ANIME	*monb_anime[];
extern HITBOX	monb_col_box, monb_atk_box, monb_dmg_box;
extern HITBOX	wave_atk_box, wave_col_box;
extern HITBOX	missile_atk_box, missile_dmg_box, missile_col_box;
extern HITBOX	punch_atk_box, punch_dmg_box, punch_col_box;

static u_char	junk_anime[] = { 7, 8, 9, 10 };

static void	monb_init(OBJECT *op);
static void	monb_main(OBJECT *op);
static void	monb_delete(OBJECT *op);
static void	monb_wait(OBJECT *op);
static void	monb_fire(OBJECT *op);
static void	monb_recover(OBJECT *op);
static void	monb_fall(OBJECT *op);
static void	monb_fall_check(OBJECT *op);
static void	shot_init(OBJECT *op);
static void	shot_move(OBJECT *op);
static void	shot_delete(OBJECT *op);

static void	(*monb_act[])(OBJECT *) = {
	monb_init, monb_main, monb_delete, monb_delete
};

static void	(*monb_mode[])(OBJECT *) = {
	HitReactionDispatch, monb_wait, monb_fire, monb_recover, monb_fall
};

static void	(*shot_act[])(OBJECT *) = {
	shot_init, shot_move, shot_delete
};

static void monb_main(OBJECT *op)
{
	int res;

	op->dmg_tbl = (op->anime.sprt == 0) ? dmg_tbl_deflect : dmg_tbl_normal;
	op->old_x = op->x;
	op->old_y = op->y;
	(*monb_mode[op->act2])(op);

	if ((u_char)(op->act2 - MODE_FIRE) >= 2) {
		if (mega.x.l < op->x.l)
			op->flip = OBJ_FLIP_L;
		else
			op->flip = OBJ_FLIP_R;
	}
	monb_fall_check(op);

	res = ContactWeapon(op);
	if (res < 0) {
		SpawnExplosion(op);
		SpawnJunk(4, junk_anime, op);
		DropItem(op, 21);
		op->act = ACT_DEAD;
		return;
	}
	if (res == HIT_BLOCKED)
		op->timer = 0;

	if ((op->col_f & (COL_U | COL_D)) == (COL_U | COL_D)) {
		SpawnExplosion(op);
		SpawnJunk(4, junk_anime, op);
		DropItem(op, 21);
		op->act = ACT_DEAD;
		return;
	}

	ContactMega(op);
	if (OffScreenCheck2(op, 64, 64))
		op->act = ACT_GONE;
	else
		DisplayCheck(op, 32, 32);
}

static void shot_init(OBJECT *op)
{
	if (op->var == KIND_WAVE) {
		Quake2(8, 2, 2);
		PlaySound(SE_BANK_ENEMY, SE_MONB_SLAM, op);
		Rumble(5);
		SetAnime(op, ANM_WAVE);
		op->spd_y = -WAVE_SPD;
		op->spd_x = op->flip ? WAVE_SPD : -WAVE_SPD;
		op->acc_x = 0;
		op->grav = 0;
		op->x.w.hi += op->flip ? 36 : -36;
		op->col_box = &wave_col_box;
		op->atk_box = &wave_atk_box;
		op->hp = 0;
		op->atk = WAVE_ATK;
		op->dmg_box = NULL;
		op->dmg_tbl = dmg_tbl_e_shot;
		op->pri = 4;
		op->y.w.hi += 8;
	} else if (op->var == KIND_MISSILE) {
		PlaySound(SE_BANK_ENEMY, SE_MONB_MISSILE, op);
		SetAnime(op, ANM_ROCKET);
		op->spd_x = op->flip ? MISSILE_SPD : -MISSILE_SPD;
		op->acc_x = 0;
		op->spd_y = 0;
		op->grav = 0;
		op->x.w.hi += op->flip ? 32 : -32;
		op->hp = MISSILE_HP;
		op->atk = MISSILE_ATK;
		op->col_box = &missile_col_box;
		op->dmg_box = &missile_dmg_box;
		op->atk_box = &missile_atk_box;
		op->y.w.hi -= 16;
		op->dmg_tbl = HARD_MODE() ? dmg_tbl_deflect : dmg_tbl_e_shot;
		op->pri = 6;
		aim_t(op) = MISSILE_TIME;
	} else {
		PlaySound(SE_BANK_ENEMY, SE_MONB_PUNCH, op);
		SetAnime(op, ANM_ROCKET);
		op->spd_x = 0;
		op->acc_x = 0;
		op->spd_y = 0;
		op->grav = 0;
		op->x.w.hi += op->flip ? 48 : -48;
		op->atk = PUNCH_ATK;
		op->col_box = &punch_col_box;
		op->dmg_box = &punch_dmg_box;
		op->atk_box = &punch_atk_box;
		op->dmg_tbl = dmg_tbl_deflect;
		op->hp = 0;
		op->pri = 6;
		op->y.w.hi -= 16;
	}
	op->clut &= ~CLUT_FLASH;
	op->disp = 0;
	op->col_f = 0;
	op->muteki = 0;
	op->act++;
}

static void shot_move(OBJECT *op)
{
	OBJECT *wp;
	u_char old_f;
	int res;

	op->clut &= ~CLUT_FLASH;
	if (delay(op) != 0) {
		delay(op)--;
		return;
	}
	AnimeTick(op);
	MoveObject(op);
	res = ContactWeapon(op);

	if (op->var == KIND_WAVE) {
		old_f = op->col_f;
		op->disp = 1;
		CollideCheck(op);
		ContactMega(op);
		if (op->col_f == 0)
			CornerCheck(op);
		if (old_f & COL_D) {
			if (op->col_f & COL_D) {
				op->spd_y = 0;
				op->grav = 0;
			}
			if (!(op->col_f & COL_D)) {
				op->act = 2;
				return;
			}
		}
		if (op->col_f & (COL_L | COL_R)) {
			op->act = 2;
			return;
		}
		if (OffScreenCheck2(op, 32, 32)) {
			op->act = 2;
			return;
		}
		DisplayCheck(op, 16, 16);
		return;
	}

	if (op->var == KIND_MISSILE) {
		op->disp = 1;
		if (aim_t(op) != 0) {
			if (aim_t(op)-- < MISSILE_AIM) {
				if (POS_Y(op) + 8 < POS_Y(&mega))
					POS_Y(op) += 2;
				else if (POS_Y(op) - 8 > POS_Y(&mega))
					POS_Y(op) -= 2;
			}
		}
		if (ContactMega(op) || res < 0 || op->col_f != 0) {
			SpawnExplosion(op);
			op->act = 2;
			return;
		}
		CornerCheck(op);
		if (op->col_f != 0) {
			SpawnExplosion(op);
			op->act = 2;
			return;
		}
		if (OffScreenCheck2(op, 32, 32)) {
			op->act = 2;
			return;
		}
		DisplayCheck(op, 16, 16);
		return;
	}

	op->disp = 0;
	ContactMega(op);
	wp = parent(op);
	if (wp->anime.flag == ANM_BACK || wp->act == ACT_DEAD || wp->act == ACT_GONE) {
		op->act = 2;
		return;
	}
	if (OffScreenCheck2(op, 32, 32) == 1)
		op->act = 2;
}

void m09_monbando(OBJECT *op)
{
	(*monb_act[op->act])(op);
	CollideCheck(op);
}

static void monb_init(OBJECT *op)
{
	op->hp = MONB_HP;
	op->atk = MONB_ATK;
	op->muteki = 0;
	op->dmg_tbl = dmg_tbl_deflect;
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav = 0;
	op->fall_f = 0;
	op->col_f = 0;
	op->layer = mega.layer;
	op->old_x = op->x;
	op->old_y = op->y;
	if (mega.x.l < op->x.l)
		op->flip = OBJ_FLIP_L;
	else
		op->flip = OBJ_FLIP_R;
	op->anime_tbl = monb_anime;
	op->col_box = &monb_col_box;
	op->pri = 4;
	op->dmg_box = &monb_dmg_box;
	op->atk_box = &monb_atk_box;
	SetAnime(op, ANM_LAND);
	op->timer = 0;
	op->act = ACT_MAIN;
	op->act2 = MODE_FALL;
	op->act3 = 0;
}

static void monb_delete(OBJECT *op)
{
	if (op->act == ACT_DEAD)
		DeleteObject(op);
	else
		DeleteObject2(op);
}

static void monb_wait(OBJECT *op)
{
	AnimeTick(op);
	if (op->timer++ < MONB_WAIT)
		return;
	if (GetRNG() & 1) {
		if (PLAYER_CHAR() == PLAYER_X) {
			SetAnime(op, ANM_MISSILE);
			kind(op) = KIND_MISSILE;
		} else {
			SetAnime(op, ANM_PUNCH);
			kind(op) = KIND_PUNCH;
		}
	} else {
		SetAnime(op, ANM_SLAM);
		kind(op) = KIND_WAVE;
	}
	op->act2 = MODE_FIRE;
}

static void monb_fire(OBJECT *op)
{
	OBJECT *sp;
	int n, wait;

	AnimeTick(op);
	if (op->anime.flag != ANM_FIRE)
		return;
	n = (kind(op) == KIND_WAVE) ? WAVE_NUM : 1;
	for (wait = 0; n != 0; n--, wait += WAVE_GAP) {
		if ((sp = GetVisualObject()) == NULL)
			continue;
		sp->flag = 0x41;
		sp->type = 7;
		sp->var = kind(op);
		sp->tpage = op->tpage;
		sp->clut = op->clut & ~CLUT_FLASH;
		sp->anime_tbl = op->anime_tbl;
		sp->sprt_p = op->sprt_p;
		sp->flip = op->flip;
		sp->x = op->x;
		sp->act = 0;
		delay(sp) = wait;
		sp->y = op->y;
		parent(sp) = op;
		sp->flip = op->flip;
	}
	op->act2 = MODE_RECOVER;
}

static void monb_recover(OBJECT *op)
{
	AnimeTick(op);
	if (op->anime.sprt == 0) {
		op->act2 = MODE_WAIT;
		SetAnime(op, ANM_GUARD);
		op->timer = 0;
	}
}

static void monb_fall(OBJECT *op)
{
	if ((op->col_f & COL_D) && op->anime.flag == ANM_END) {
		op->act2 = MODE_WAIT;
		op->spd_x = 0;
		op->acc_x = 0;
		op->spd_y = 0;
		op->grav = 0;
		op->fall_f = 0;
		SetAnime(op, ANM_GUARD);
	} else {
		MoveObject2(op);
	}
	AnimeTick(op);
}

static void monb_fall_check(OBJECT *op)
{
	if (!(op->col_f & COL_D) && op->fall_f == 0) {
		op->act2 = MODE_FALL;
		op->grav = MONB_GRAV;
		op->spd_y = 0;
		op->acc_x = 0;
		op->fall_f = 1;
	}
}

static void shot_delete(OBJECT *op)
{
	DeleteObject3(op);
}

void e07_monbando_shot(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	(*shot_act[op->act])(op);
}
