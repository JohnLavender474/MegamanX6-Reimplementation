/*
 *	m23_nightmare_insect.c
 *
 *	ST03 (Commander Yammark)  main object 23   : Nightmare Insect
 *	                          visual object 27 : its blade wave (var 0), boomerang blade (var 1)
 *	                                             and hatchlings (var 2)
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
	MODE_IDLE,
	MODE_BLADE,
	MODE_BOOMERANG,
	MODE_LAY,
	MODE_REVIVE
};

enum {
	KID_BLADE = 0,
	KID_BOOMERANG,
	KID_HATCH
};

#define INSECT_HP	32
#define INSECT_ATK	8
#define IDLE_TIME	61
#define NEAR_X		64
#define LAY_TIME	161
#define LAY_STEP	20
#define HATCH_CALL_MAX	4
#define HATCH_MAX	8

#define BLADE_ATK	6
#define BLADE_SPD	0x30000
#define BLADE_AIM	80
#define BLADE_TIME	91
#define BOOM_BACK	48
#define HATCH_HP	3
#define HATCH_ATK	3
#define HATCH_SPD	0x18000
#define HATCH_WALK	241
#define HATCH_FLASH	51

#define VISUAL_TYPE	27

#define returned(op)	((op)->act4)
#define parent(op)	(*(OBJECT **)&(op)->timer)
#define ktimer(op)	(*(short *)&(op)->work[0x0e])
#define target_x(op)	(*(short *)&(op)->work[0x10])
#define target_y(op)	(*(short *)&(op)->work[0x12])

#define ABS(a)		((a) < 0 ? -(a) : (a))

extern ANIME	*insect_anime[];
extern HITBOX	insect_atk_box, insect_dmg_box;
extern HITBOX	blade_atk_box, blade_dmg_box;
extern HITBOX	hatch_atk_box, hatch_dmg_box, hatch_col_box, hatch_burst_box;

static u_char	insect_junk[] = { 8, 9, 10, 11 };
static u_char	hatch_junk[] = { 12, 13 };

static void	insect_init(OBJECT *op);
static void	insect_main(OBJECT *op);
static void	insect_delete(OBJECT *op);
static void	insect_idle(OBJECT *op);
static void	insect_blade(OBJECT *op);
static void	insect_boomerang(OBJECT *op);
static void	insect_lay(OBJECT *op);
static void	insect_revive(OBJECT *op);
static void	blade_start(OBJECT *op);
static void	blade_throw(OBJECT *op);
static void	blade_end(OBJECT *op);
static void	boom_start(OBJECT *op);
static void	boom_throw(OBJECT *op);
static void	boom_wait(OBJECT *op);
static void	boom_end(OBJECT *op);
static void	lay_start(OBJECT *op);
static void	lay_spawn(OBJECT *op);
static void	lay_end(OBJECT *op);
static void	lay_hold(OBJECT *op);

static void	kid_init(OBJECT *op);
static void	kid_main(OBJECT *op);
static void	kid_delete(OBJECT *op);
static void	kid_blade(OBJECT *op);
static void	kid_boomerang(OBJECT *op);
static void	kid_hatch(OBJECT *op);
static void	blade_fly(OBJECT *op);
static void	blade_stop(OBJECT *op);
static void	blade_chase(OBJECT *op);
static void	blade_stop2(OBJECT *op);
static void	blade_exit(OBJECT *op);
static void	boom_out(OBJECT *op);
static void	boom_back(OBJECT *op);
static void	hatch_fall(OBJECT *op);
static void	hatch_walk(OBJECT *op);
static void	hatch_burst(OBJECT *op);

static void	(*insect_act[])(OBJECT *) = {
	insect_init, insect_main, insect_delete, insect_delete
};

static void	(*insect_mode[])(OBJECT *) = {
	HitReactionDispatch, insect_idle, insect_blade, insect_boomerang, insect_lay, insect_revive
};

static void	(*insect_blade_tbl[])(OBJECT *) = {
	blade_start, blade_throw, blade_end
};

static void	(*insect_boom_tbl[])(OBJECT *) = {
	boom_start, boom_throw, boom_wait, boom_end
};

static void	(*insect_lay_tbl[])(OBJECT *) = {
	lay_start, lay_spawn, lay_end, lay_hold
};

static void	(*kid_act[])(OBJECT *) = {
	kid_init, kid_main, kid_delete
};

static void	(*kid_mode[])(OBJECT *) = {
	kid_blade, kid_boomerang, kid_hatch
};

static void	(*kid_blade_tbl[])(OBJECT *) = {
	blade_fly, blade_stop, blade_chase, blade_stop2, blade_exit
};

static void	(*kid_boom_tbl[])(OBJECT *) = {
	boom_out, boom_back
};

static void	(*kid_hatch_tbl[])(OBJECT *) = {
	hatch_fall, hatch_walk, hatch_burst
};

static int count_hatch(void)
{
	u_char *p;
	int n = 0;

	for (p = visual_object[0]; p < visual_object[VISUAL_OBJECT_MAX]; p += 0xa0) {
		if (p[0] && (*(u_long *)p & 0x00ffff00) == ((KID_HATCH << 16) | (VISUAL_TYPE << 8)))
			n++;
	}
	return n;
}

static void spawn_kid(OBJECT *op, int var)
{
	OBJECT *kp;

	if ((kp = GetVisualObject()) != NULL) {
		kp->flag = 0x41;
		kp->type = VISUAL_TYPE;
		kp->var = var;
		kp->tpage = op->tpage;
		kp->clut = op->clut & ~CLUT_FLASH;
		kp->sprt_p = op->sprt_p;
		kp->layer = op->layer;
		kp->x = op->x;
		kp->y = op->y;
		kp->anime_tbl = op->anime_tbl;
		parent(kp) = op;
		kp->flip = op->flip;
	}
}

static void face_mega(OBJECT *op)
{
	if (mega.x.l < op->x.l)
		op->flip = OBJ_FLIP_L;
	else
		op->flip = OBJ_FLIP_R;
}

static void aim_mega(OBJECT *op, int spd)
{
	long a;

	a = ratan2(POS_Y(&mega) - POS_Y(op), POS_X(&mega) - POS_X(op));
	op->spd_x = rcos(a) * spd;
	op->spd_y = -rsin(a) * spd;
}

static void insect_main(OBJECT *op)
{
	int r;

	insect_mode[op->act2](op);

	r = ContactWeapon(op);
	if (r == -2 || r == -11 || r == -55) {
		SpawnExplosion(op);
		SpawnJunk(4, insect_junk, op);
		DropItem(op, 21);
		op->act = ACT_DEAD;
		return;
	}
	if (r < 0) {
		SpawnExplosion(op);
		SpawnJunk(4, insect_junk, op);
		if (game.level < LEVEL_HARD)
			SetAnime(op, 5);
		else
			SetAnime(op, 15);
		op->hp = INSECT_HP;
		op->dmg_box = NULL;
		op->atk_box = NULL;
		op->act2 = MODE_REVIVE;
		return;
	}
	ContactMega(op);
	if (!OffScreenCheck2(op, 64, 64))
		DisplayCheck(op, 48, 48);
}

static void insect_idle(OBJECT *op)
{
	short dx;

	face_mega(op);
	AnimeTick(op);
	if (op->timer++ < IDLE_TIME)
		return;

	if (op->disp) {
		dx = ABS(POS_X(&mega) - POS_X(op));
		if (mega.var) {
			op->act2 = (dx < NEAR_X) ? MODE_LAY : MODE_BOOMERANG;
			if (count_hatch() >= HATCH_CALL_MAX)
				op->act2 = MODE_BOOMERANG;
		} else {
			op->act2 = (dx < NEAR_X) ? MODE_BLADE : MODE_LAY;
			if (count_hatch() >= HATCH_CALL_MAX)
				op->act2 = MODE_BLADE;
		}
	}
	op->act3 = 0;
}

static void lay_spawn(OBJECT *op)
{
	short t;

	AnimeTick(op);
	t = op->timer++;
	if (t >= LAY_TIME || count_hatch() >= HATCH_MAX) {
		SetAnime(op, 4);
		op->act3++;
		return;
	}
	if (t % LAY_STEP == 0)
		spawn_kid(op, KID_HATCH);
}

static void kid_init(OBJECT *op)
{
	op->pri = 5;
	op->act++;
	op->act3 = 0;
	ktimer(op) = 0;

	if (op->var == KID_BLADE) {
		SetAnime(op, 7);
		PlaySound(SE_BANK_ENEMY, SE_INSECT_BLADE, op);
		op->spd_x = op->flip ? BLADE_SPD : -BLADE_SPD;
		op->spd_y = 0;
		POS_X(op) += op->flip ? 16 : -16;
		op->atk = BLADE_ATK;
		op->atk_box = &blade_atk_box;
		op->col_box = NULL;
		op->dmg_box = &blade_dmg_box;
		op->dmg_tbl = dmg_tbl_deflect;
		op->act2 = KID_BLADE;
	} else if (op->var == KID_BOOMERANG) {
		SetAnime(op, 7);
		PlaySound(SE_BANK_ENEMY, SE_INSECT_BLADE, op);
		aim_mega(op, BLADE_AIM);
		POS_X(op) += op->flip ? 16 : -16;
		op->atk = BLADE_ATK;
		op->atk_box = &blade_atk_box;
		op->dmg_box = &blade_dmg_box;
		op->col_box = NULL;
		op->dmg_tbl = dmg_tbl_deflect;
		op->act2 = KID_BOOMERANG;
	} else if (op->var == KID_HATCH) {
		SetAnime(op, 6);
		op->spd_x = 0;
		op->spd_y = -HATCH_SPD;
		if (game.level >= LEVEL_HARD)
			op->spd_y = -0x30000;
		POS_X(op) += op->flip ? 32 : -32;
		op->hp = HATCH_HP;
		op->atk = HATCH_ATK;
		op->col_box = &hatch_col_box;
		op->dmg_box = &hatch_dmg_box;
		op->atk_box = &hatch_atk_box;
		op->dmg_tbl = dmg_tbl_small;
		op->act2 = KID_HATCH;
		POS_Y(op) -= 16;
	}
}

static void blade_fly(OBJECT *op)
{
	short dx, dy;

	AnimeTick(op);
	MoveObject(op);
	if (ktimer(op)++ < BLADE_TIME) {
		dx = ABS(POS_X(&mega) - POS_X(op));
		if (dx >= 8)
			return;
		dy = ABS(POS_Y(&mega) - POS_Y(op));
		if (dy >= 32)
			return;
	}
	ktimer(op) = 0;
	op->act3++;
}

static void blade_chase(OBJECT *op)
{
	short dx, dy;

	AnimeTick(op);
	MoveObject(op);
	if (ktimer(op)++ < BLADE_TIME) {
		dx = ABS(target_x(op) - POS_X(op));
		if (dx >= 8)
			return;
		dy = ABS(target_y(op) - POS_Y(op));
		if (dy >= 32)
			return;
	}
	aim_mega(op, BLADE_AIM);
	op->act3++;
}

static void boom_out(OBJECT *op)
{
	short dx, dy;

	AnimeTick(op);
	MoveObject(op);
	if (ktimer(op)++ < BLADE_TIME) {
		dx = ABS(POS_X(&mega) - POS_X(op));
		if (dx >= 32)
			return;
		dy = ABS(POS_Y(&mega) - POS_Y(op));
		if (dy >= 16)
			return;
	}
	ktimer(op) = 0;
	op->act3++;
}

static void boom_back(OBJECT *op)
{
	OBJECT *pp = parent(op);
	short tx, ty;
	long a;

	tx = op->flip ? POS_X(pp) + 16 : POS_X(pp) - 16;
	ty = POS_Y(pp);
	a = ratan2(ty - POS_Y(op), tx - POS_X(op));
	op->spd_x = rcos(a) * BOOM_BACK;
	op->spd_y = -rsin(a) * BOOM_BACK;
	AnimeTick(op);
	MoveObject(op);
	if (ABS(tx - POS_X(op)) < 16 && ABS(ty - POS_Y(op)) < 16) {
		returned(pp) = 1;
		op->act = ACT_DEAD;
	}
}

static void hatch_walk(OBJECT *op)
{
	HITBOX *bp;
	short fx, fy;

	AnimeTick(op);
	MoveObject(op);
	bp = op->col_box;
	if (op->flip)
		fx = POS_X(op) + bp->ox + bp->w;
	else
		fx = POS_X(op) - bp->ox - bp->w;
	fy = POS_Y(op) + bp->oy + bp->h + 8;

	if (TerrainPointCheck(op, fx, fy) == 0
	 || (op->col_f & (op->flip ? COL_R : COL_L))) {
		op->flip = (op->flip == 0) << 6;
		op->spd_x = -op->spd_x;
		MoveObject(op);
	}
	if (ktimer(op)++ >= HATCH_WALK) {
		ktimer(op) = 0;
		op->act3++;
	}
}

static void kid_main(OBJECT *op)
{
	OBJECT *pp = parent(op);

	if (ContactWeapon(op) < 0 || pp->act == ACT_DEAD || pp->act2 == MODE_REVIVE) {
		if (op->var == KID_HATCH) {
			SpawnJunk(2, hatch_junk, op);
			SpawnExplosion(op);
		}
		op->act = ACT_DEAD;
		return;
	}
	ContactMega(op);
	kid_mode[op->act2](op);

	if (!OffScreenCheck2(op, 64, 64))
		DisplayCheck1(op);
	else if (op->var == KID_HATCH && POS_Y(op) - bg_layer[op->layer].y >= 301)
		DeleteObject3(op);

	if (op->var == KID_HATCH) {
		if (op->col_f & COL_D)
			op->fall_f = 0;
		else
			op->fall_f = (op->spd_y < 0) ? -1 : 1;
	}
}

void m23_insect(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	insect_act[op->act](op);
	CollideCheck(op);
}

static void insect_init(OBJECT *op)
{
	op->hp = INSECT_HP;
	op->atk = INSECT_ATK;
	op->muteki = 0;
	op->dmg_tbl = dmg_tbl_insect;
	op->spd_x = 0;
	op->spd_y = 0;
	op->col_f = 0;
	op->layer = mega.layer;
	face_mega(op);
	op->anime_tbl = insect_anime;
	op->pri = 5;
	op->col_box = NULL;
	op->dmg_box = &insect_dmg_box;
	op->atk_box = &insect_atk_box;
	SetAnime(op, 0);
	op->timer = 0;
	op->act = ACT_MAIN;
	op->act2 = MODE_IDLE;
	returned(op) = 0;
}

static void insect_delete(OBJECT *op)
{
	if (op->act == ACT_DEAD)
		DeleteObject(op);
	else
		DeleteObject2(op);
}

static void insect_blade(OBJECT *op)
{
	insect_blade_tbl[op->act3](op);
}

static void insect_boomerang(OBJECT *op)
{
	insect_boom_tbl[op->act3](op);
}

static void insect_lay(OBJECT *op)
{
	insect_lay_tbl[op->act3](op);
}

static void insect_revive(OBJECT *op)
{
	AnimeTick(op);
	if (op->anime.flag == 3) {
		op->dmg_box = &insect_dmg_box;
		op->atk_box = &insect_atk_box;
		SetAnime(op, 0);
		op->timer = 0;
		op->act2 = MODE_IDLE;
	}
}

static void blade_start(OBJECT *op)
{
	SetAnime(op, 1);
	op->act3++;
}

static void blade_throw(OBJECT *op)
{
	AnimeTick(op);
	if (op->anime.flag == ANM_FIRE) {
		spawn_kid(op, KID_BLADE);
		op->act3++;
	}
}

static void blade_end(OBJECT *op)
{
	AnimeTick(op);
	if (op->anime.flag == ANM_END) {
		SetAnime(op, 0);
		op->timer = 0;
		op->act2 = MODE_IDLE;
	}
}

static void boom_start(OBJECT *op)
{
	SetAnime(op, 14);
	op->act3++;
}

static void boom_throw(OBJECT *op)
{
	char f;

	AnimeTick(op);
	f = op->anime.flag;
	if (f == ANM_FIRE) {
		spawn_kid(op, f);
		op->act3++;
	}
}

static void boom_wait(OBJECT *op)
{
	AnimeTick(op);
	if (returned(op) == 1) {
		returned(op) = 0;
		SetAnime(op, 2);
		op->act3++;
	}
}

static void boom_end(OBJECT *op)
{
	AnimeTick(op);
	if (op->anime.flag == ANM_END) {
		SetAnime(op, 0);
		op->timer = 0;
		op->act2 = MODE_IDLE;
	}
}

static void lay_start(OBJECT *op)
{
	SetAnime(op, 3);
	op->timer = 0;
	op->act3++;
}

static void lay_end(OBJECT *op)
{
	AnimeTick(op);
	if (op->anime.flag == ANM_END) {
		SetAnime(op, 0);
		op->timer = 0;
		op->act2 = MODE_IDLE;
	}
}

static void lay_hold(OBJECT *op)
{
	AnimeTick(op);
}

static void kid_delete(OBJECT *op)
{
	DeleteObject3(op);
}

void v27_insect_kid(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	kid_act[op->act](op);
	CollideCheck(op);
}

static void blade_stop(OBJECT *op)
{
	AnimeTick(op);
	if (ktimer(op)++ < BLADE_TIME)
		return;
	target_x(op) = POS_X(&mega);
	target_y(op) = POS_Y(&mega);
	aim_mega(op, BLADE_AIM);
	ktimer(op) = 0;
	if (game.level >= LEVEL_HARD)
		op->act3 += 1;
	else
		op->act3 += 3;
}

static void blade_stop2(OBJECT *op)
{
	AnimeTick(op);
	if (ktimer(op)++ < BLADE_TIME)
		return;
	aim_mega(op, BLADE_AIM);
	ktimer(op) = 0;
	op->act3++;
}

static void blade_exit(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
	if (OffScreenCheck2(op, 64, 64) == 1)
		DeleteObject3(op);
}

static void kid_blade(OBJECT *op)
{
	kid_blade_tbl[op->act3](op);
}

static void kid_boomerang(OBJECT *op)
{
	kid_boom_tbl[op->act3](op);
}

static void hatch_fall(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
	if (op->col_f & COL_D) {
		ktimer(op) = 0;
		op->spd_x = op->flip ? HATCH_SPD : -HATCH_SPD;
		if (game.level >= LEVEL_HARD)
			op->spd_x <<= 1;
		op->spd_y = 0;
		op->act3++;
	}
}

static void hatch_burst(OBJECT *op)
{
	if (!(ktimer(op) & 1))
		op->clut ^= CLUT_FLASH;
	if (ktimer(op)++ < HATCH_FLASH)
		return;
	op->atk_box = &hatch_burst_box;
	op->dmg_box = NULL;
	SpawnJunk(2, hatch_junk, op);
	SpawnExplosion(op);
	op->act = ACT_DEAD;
}

static void kid_hatch(OBJECT *op)
{
	kid_hatch_tbl[op->act3](op);
}
