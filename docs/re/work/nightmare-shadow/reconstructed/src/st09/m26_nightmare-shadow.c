/*
 *	m26_nightmare-shadow.c
 *
 *	ST09 (Rainy Turtloid)  main object 26 : Nightmare shadow
 *	                  var 0-5  spawner (one zone each)
 *	                  var 10   X shadow, var 11 Zero shadow
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
	ACT_SPLIT,
	ACT_GONE
};

enum {
	MODE_REACT = 0,
	MODE_SPAWN,
	MODE_DASH,
	MODE_APPEAR
};

enum {
	ANM_X_STAND = 0,
	ANM_X_DASH,
	ANM_Z_STAND,
	ANM_Z_DASH
};

#define VAR_X_SHADOW	10
#define VAR_Z_SHADOW	11

#define SHADOW_HP	4
#define SHADOW_ATK	6
#define SHADOW_PRI	4
#define BLINK_TIME	40
#define STAND_TIME	50
#define SPLIT_TIME	60
#define SPAWN_OFS	32
#define OUT_RANGE	128

#define split_r(op)	(*(short *)&(op)->work[0x02])
#define split_l(op)	(*(short *)&(op)->work[0x04])

extern ANIME	*shadow_anime[];
extern HITBOX	stand_atk_box, dash_atk_box, stand_dmg_box, dash_dmg_box;

static void	shadow_init(OBJECT *op);
static void	shadow_spawn(OBJECT *op);
static void	appear_wait(OBJECT *op);
static void	shadow_main(OBJECT *op);
static void	shadow_split(OBJECT *op);
static void	shadow_delete(OBJECT *op);
static void	shadow_dash(OBJECT *op);
static void	shadow_appear(OBJECT *op);
static void	appear_blink(OBJECT *op);
static void	appear_stand(OBJECT *op);

static void (*shadow_act[])(OBJECT *) = {
	shadow_init, shadow_main, shadow_split, shadow_delete
};

static void (*shadow_mode[])(OBJECT *) = {
	HitReactionDispatch, shadow_spawn, shadow_dash, shadow_appear
};

static void (*appear_mode[])(OBJECT *) = {
	appear_blink, appear_stand, appear_wait
};

static int spawn_time(void)
{
	return (GetRNG() % 3 + 1) * 100 + 200;
}

static int in_zone(int var)
{
	u_short	x = (u_short)POS_X(&mega);

	switch (var) {
	case 0:	return (u_short)(x - 321) < 2239;
	case 1:	return (u_short)(x - 3201) < 959;
	case 2:	return (u_short)(x - 1249) < 383;
	case 3:	return (u_short)(x - 4961) < 2351;
	case 4:	return (u_short)(x - 1) < 1055;
	case 5:	return (u_short)(x - 321) < 1823 && POS_Y(&mega) < 1072;
	}
	return 0;
}

static void shadow_init(OBJECT *op)
{
	int	bank;
	int	ok = 0;

	if (nightmare_no != NIGHTMARE_SHADOW) {
		DeleteObject3(op);
		return;
	}
	op->flag = 0x41;
	op->tpage = bank_tpage[GetSpriteBank(op->type)] >> 7;
	bank = GetSpriteBank(op->type) * 4 + 32;
	op->clut = (bank % 16) | (((GetSpriteBank(op->type) + 8) / 4 + 480) << 6);
	op->sprt_p = (char *)sprt_bank + ((long *)sprt_bank)[GetSpriteBank(op->type)];

	switch (op->var) {
	case 0: case 1: case 2: case 3: case 4: case 5:
		if (!in_zone(op->var))
			break;
		ok = 1;
		op->act2 = MODE_SPAWN;
		op->disp = 0;
		op->timer = spawn_time();
		break;
	case VAR_X_SHADOW:
		SetAnime(op, ANM_X_STAND);
		ok = 1;
		op->timer = BLINK_TIME;
		goto shadow;
	case VAR_Z_SHADOW:
		op->timer = BLINK_TIME;
		SetAnime(op, ANM_Z_STAND);
		ok = 1;
	shadow:
		op->dmg_box = NULL;
		op->atk_box = NULL;
		op->act2 = MODE_APPEAR;
		op->disp = ok;
		break;
	}
	if (!ok)
		return;

	op->hp = SHADOW_HP;
	op->muteki = 0;
	op->anime_tbl = shadow_anime;
	op->pri = SHADOW_PRI;
	op->col_box = NULL;
	op->fall_f = 0;
	op->dmg_tbl = dmg_tbl_nightmare_shadow;
	op->atk = SHADOW_ATK;
	op->layer = mega.layer;
	op->act = ACT_MAIN;
	op->act3 = 0;
	op->flip = (mega.x.l < op->x.l) ? OBJ_FLIP_L : OBJ_FLIP_R;
}

static void shadow_spawn(OBJECT *op)
{
	OBJECT	*np;

	if (op->var >= 6 || !in_zone(op->var))
		return;
	if (op->timer != 0) {
		op->timer--;
		return;
	}
	if ((np = GetMainObject()) != NULL) {
		np->flag = 0x41;
		np->type = 26;
		np->flip = op->flip;
		np->var = (mega.var == 0) ? VAR_X_SHADOW : VAR_Z_SHADOW;
		POS_X(np) = mega.flip ? POS_X(&mega) + SPAWN_OFS : POS_X(&mega) - SPAWN_OFS;
		POS_Y(np) = POS_Y(&mega);
		np->clut = op->clut & ~CLUT_FLASH;
		np->anime_tbl = op->anime_tbl;
		np->sprt_p = op->sprt_p;
		np->tpage = op->tpage;
		np->layer = op->layer;
	}
	op->timer = spawn_time();
}

static void appear_wait(OBJECT *op)
{
	long	a;

	if (--op->timer != 0)
		return;
	if (op->var == VAR_Z_SHADOW)
		SetAnime(op, ANM_Z_DASH);
	else
		SetAnime(op, ANM_X_DASH);
	a = ratan2(POS_Y(&mega) - POS_Y(op), POS_X(&mega) - POS_X(op));
	op->spd_x = rcos(a) << 6;
	op->spd_y = -(rsin(a) << 6);
	op->dmg_box = &dash_dmg_box;
	op->atk_box = &dash_atk_box;
	op->act2 = MODE_DASH;
	op->act3 = 0;
	op->flip = (mega.x.l < op->x.l) ? OBJ_FLIP_L : OBJ_FLIP_R;
}

void m26_nightmare_shadow(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	shadow_act[op->act](op);
	CollideCheck(op);
}

static void shadow_main(OBJECT *op)
{
	shadow_mode[op->act2](op);

	if (ContactWeapon(op) < 0) {
		op->act = ACT_SPLIT;
		op->timer = SPLIT_TIME;
		op->disp = 1;
		split_l(op) = POS_X(op);
		split_r(op) = POS_X(op);
		op->clut &= ~CLUT_FLASH;
		return;
	}
	ContactMega(op);
	if (OffScreenCheck2(op, OUT_RANGE, OUT_RANGE) == 1
	 && (u_char)(op->var - VAR_X_SHADOW) < 2)
		op->act = ACT_GONE;
}

static void shadow_split(OBJECT *op)
{
	split_r(op)++;
	split_l(op)--;
	if (frame_count & 1)
		POS_X(op) = split_l(op);
	else
		POS_X(op) = split_r(op);
	if (--op->timer == 0)
		DeleteObject3(op);
}

static void shadow_delete(OBJECT *op)
{
	DeleteObject3(op);
}

static void shadow_dash(OBJECT *op)
{
	MoveObject(op);
}

static void shadow_appear(OBJECT *op)
{
	appear_mode[op->act3](op);
}

static void appear_blink(OBJECT *op)
{
	op->disp ^= 1;
	if (--op->timer == 0) {
		op->dmg_box = &stand_dmg_box;
		op->atk_box = &stand_atk_box;
		op->act3++;
	}
}

static void appear_stand(OBJECT *op)
{
	op->disp = 1;
	op->timer = STAND_TIME;
	op->act3++;
}
