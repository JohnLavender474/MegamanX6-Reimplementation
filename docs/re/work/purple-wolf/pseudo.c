/*
 *	m06_wolfloid.c  (pseudo.c)
 *
 *	ST04 (Blizzard Wolfang stage, overlay 04)  main object 06   : Wolfloid (purple robot wolf)
 *	                                            visual object 05 : breath clouds and cannon ball
 *
 *	A plausible reconstruction, not original code, not compiled. The functions are in the order of
 *	the game code; each header gives the game address. Constants are the game's constants.
 *	Headers: include/object.h, include/game.h, include/sound.h (copied from worm-spider, with
 *	dmg_tbl_normal, PLAYER_IS_ZERO and SmokePuff added).
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
	MODE_BREATH_FLAT,
	MODE_CANNON_FLAT,
	MODE_BREATH_SLOPE_R,
	MODE_BREATH_SLOPE_L,
	MODE_CANNON_SLOPE_R,
	MODE_CANNON_SLOPE_L
};

enum {
	WOLF_SLOPE_R = 0,
	WOLF_SLOPE_L,
	WOLF_BREATH,
	WOLF_CANNON,
	WOLF_RIDE_BREATH,
	WOLF_RIDE_CANNON
};

enum {
	FX_BREATH = 0,
	FX_BREATH_SLOPE,
	FX_BALL
};

#define WOLF_HP		12
#define WOLF_ATK	4
#define WOLF_GRAV	0x40000
#define HOWL_EVERY	200
#define NEAR_BREATH	128
#define NEAR_BREATH_S	96
#define NEAR_CANNON	180
#define BREATH_TIME	90
#define BREATH_REST	40
#define FIRE_AT		12
#define FIRE_TIME	90
#define RIDE_DY		39
#define RIDE_LAND_Y	1313

#define BREATH_ATK	4
#define BALL_HP		2
#define BALL_ATK	3
#define BALL_VY		0x30000
#define BALL_GRAV	0x1800
#define BALL_DIV	70

#define rider_of(op)	(*(OBJECT **)&(op)->work[2])
#define owner(op)	(*(OBJECT **)&(op)->timer)
#define fx_count(op)	((op)->act3)

extern ANIME	*wolf_anime[];
extern HITBOX	wolf_col_box, wolf_atk_box, wolf_dmg_box;
extern HITBOX	breath_atk_box, breath_slope_d_box, breath_slope_u_box;
extern HITBOX	ball_col_box, ball_atk_box, ball_dmg_box;

static u_char	wolf_junk[] = { 19, 20, 21, 22, 23 };

static void	wolf_init(OBJECT *op);
static void	wolf_main(OBJECT *op);
static void	wolf_breath_flat(OBJECT *op);
static void	wolf_cannon_flat(OBJECT *op);
static void	wolf_breath_slope_r(OBJECT *op);
static void	wolf_breath_slope_l(OBJECT *op);
static void	wolf_cannon_slope_r(OBJECT *op);
static void	wolf_cannon_slope_l(OBJECT *op);
static void	wolf_nop(OBJECT *op);
static void	wolf_delete(OBJECT *op);
static void	breath_main(OBJECT *op);
static void	breath_s_init(OBJECT *op);
static void	breath_s_main(OBJECT *op);
static void	ball_init(OBJECT *op);
static void	ball_main(OBJECT *op);
static void	breath_init(OBJECT *op);
static void	fx_delete(OBJECT *op);

/* 0x800F6A84: states; 0x800F6A94: steps (the two tables are back to back) */
static void (*wolf_act_tbl[])(OBJECT *) = {
	wolf_init, wolf_main, wolf_delete, wolf_delete
};

static void (*wolf_mode_tbl[])(OBJECT *) = {
	HitReactionDispatch, wolf_nop,
	wolf_breath_flat, wolf_cannon_flat,
	wolf_breath_slope_r, wolf_breath_slope_l,
	wolf_cannon_slope_r, wolf_cannon_slope_l
};

/* 0x800F6ACC, 0x800F6AD8, 0x800F6AE4: visual type 5 by var */
static void (*breath_act_tbl[])(OBJECT *) = { breath_init, breath_main, fx_delete };
static void (*breath_s_act_tbl[])(OBJECT *) = { breath_s_init, breath_s_main, fx_delete };
static void (*ball_act_tbl[])(OBJECT *) = { ball_init, ball_main, fx_delete };

static void face_player(OBJECT *op)
{
	op->flip = (POS_X(op) < POS_X(&mega)) ? OBJ_FLIP_R : OBJ_FLIP_L;
}

static int near_x(OBJECT *op, int range)
{
	int dx = POS_X(&mega) - POS_X(op);

	if (dx < 0)
		dx = -dx;
	return dx <= range;
}

static OBJECT *spawn_fx(OBJECT *op, int var)
{
	OBJECT *fp = GetVisualObject();

	if (fp != NULL) {
		fp->type = 5;
		fp->var = var;
		owner(fp) = op;
		fp->flag = op->flag;
	}
	return fp;
}

static void wolf_die(OBJECT *op, int drop)
{
	SpawnExplosion(op);
	SpawnJunk(5, wolf_junk, op);
	if (drop)
		DropItem(op, 17);
	op->act = ACT_DEAD;
}

/* 0x800E9B30: state 0 */
static void wolf_init(OBJECT *op)
{
	op->anime_tbl = wolf_anime;
	op->layer = mega.layer;
	op->pri = 5;
	op->col_box = &wolf_col_box;
	op->atk_box = &wolf_atk_box;
	op->dmg_tbl = dmg_tbl_normal;
	op->hp = WOLF_HP;
	op->dmg_box = &wolf_dmg_box;
	op->grav = WOLF_GRAV;
	op->fall_f = 0;
	op->disp = 0;
	op->muteki = 0;
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->atk = WOLF_ATK;

	switch (op->var) {
	case WOLF_SLOPE_R:
		op->act2 = PLAYER_IS_ZERO() ? MODE_BREATH_SLOPE_R : MODE_CANNON_SLOPE_R;
		break;
	case WOLF_SLOPE_L:
		op->act2 = PLAYER_IS_ZERO() ? MODE_BREATH_SLOPE_L : MODE_CANNON_SLOPE_L;
		break;
	case WOLF_BREATH:
		op->act2 = MODE_BREATH_FLAT;
		break;
	case WOLF_CANNON:
		op->act2 = MODE_CANNON_FLAT;
		break;
	case WOLF_RIDE_BREATH:
	case WOLF_RIDE_CANNON:
		op->act2 = (op->var == WOLF_RIDE_BREATH) ? MODE_BREATH_FLAT : MODE_CANNON_FLAT;
		op->act4 = 0;
		op->col_box = NULL;
		op->spd_y = 0;
		op->grav = 0;
		break;
	default:
		/* hazard: var >= 6 sets no step, so act2 stays 0 and the wolf runs HitReactionDispatch
		 * each update; safe in the game because no record of section 1 has id >= 6 */
		break;
	}
	op->act3 = 0;
	op->act++;
}

/* 0x800E9C84: state 1 */
static void wolf_main(OBJECT *op)
{
	OBJECT *rp;

	if (ContactWeapon(op) < 0) {
		wolf_die(op, 1);
		return;
	}
	if ((op->col_f & (COL_U | COL_D)) == (COL_U | COL_D) || (op->col_f & (COL_L | COL_R)) == (COL_L | COL_R)) {
		wolf_die(op, 0);
		return;
	}
	ContactMega(op);

	if (op->var == WOLF_RIDE_BREATH || op->var == WOLF_RIDE_CANNON) {
		DisplayCheck(op, 64, 48);
		rp = rider_of(op);
		/* hazard: the carrier pointer is never tested; safe while the block deletes itself only
		 * after it has set state 2 or 4, and no other object reuses the slot in between */
		if (rp->act == 4 && op->act4 == 0) {
			wolf_die(op, 1);
			return;
		}
		if (rp->act == 2) {
			op->act4 = 1;
			op->var -= 2;
		}
	} else if (OffScreenCheck2(op, 64, 48)) {
		op->act = ACT_GONE;
		return;
	} else {
		DisplayCheck(op, 64, 48);
	}

	MoveObject2(op);
	AnimeTick(op);
	wolf_mode_tbl[op->act2](op);
}

/* 0x800E9DFC: step 2, the breath wolf on a flat floor (ids 2 and 4) */
static void wolf_breath_flat(OBJECT *op)
{
	OBJECT *rp = rider_of(op);

	if (op->var == WOLF_RIDE_BREATH && op->act4 == 0) {
		POS_Y(op) = POS_Y(rp) - RIDE_DY;
		if (POS_Y(op) >= RIDE_LAND_Y && rp->act3 == 6)
			op->col_box = &wolf_col_box;
	}
	face_player(op);
	op->timer++;

	switch (op->act3) {
	case 0:
	idle:
		SetAnime(op, 0);
		op->timer = 0;
		op->act3++;
		break;
	case 1:
		if (near_x(op, NEAR_BREATH))
			goto breath;
		if (op->timer % HOWL_EVERY == 0) {
			SetAnime(op, 1);
			op->act3++;
		}
		break;
	case 2:
		if (near_x(op, NEAR_BREATH)) {
		breath:
			spawn_fx(op, FX_BREATH);
			SetAnime(op, 2);
			op->timer = 0;
			op->act3 = 3;
		} else if (op->anime.next == 0) {
			op->act3 = 0;
		}
		break;
	case 3:
		if (op->timer == BREATH_TIME)
			goto idle;
		break;
	case 4:
		if (op->timer == BREATH_REST)
			op->act3 = 0;
		break;
	}
}

/* 0x800EA048: step 3, the cannon wolf on a flat floor (ids 3 and 5) */
static void wolf_cannon_flat(OBJECT *op)
{
	OBJECT *rp = rider_of(op);

	if (op->var == WOLF_RIDE_CANNON && op->act4 == 0) {
		POS_Y(op) = POS_Y(rp) - RIDE_DY;
		if (POS_Y(op) >= RIDE_LAND_Y && rp->act3 == 6)
			op->col_box = &wolf_col_box;
	}
	face_player(op);
	op->timer++;

	switch (op->act3) {
	case 0:
		SetAnime(op, 3);
		op->timer = 0;
		op->act3++;
		break;
	case 1:
		if (near_x(op, NEAR_CANNON))
			goto fire;
		if (op->timer % HOWL_EVERY == 0) {
			SetAnime(op, 4);
			op->act3++;
		}
		break;
	case 2:
		if (near_x(op, NEAR_CANNON)) {
		fire:
			SetAnime(op, 5);
			op->timer = 0;
			op->act3 = 3;
		} else if (op->anime.next == 0) {
			op->act3 = 0;
		}
		break;
	case 3:
		if (op->timer == FIRE_AT)
			spawn_fx(op, FX_BALL);
		else if (op->timer == FIRE_TIME)
			op->act3 = 0;
		break;
	}
}

/*
 * 0x800EA2A8 (step 4) and 0x800EA60C (step 5): the breath wolf on a slope (Zero, ids 0 and 1).
 * The two game functions are the same code with the two animation sets swapped; here one
 * function takes the set for "facing left" and the set for "facing right".
 */
static void breath_slope(OBJECT *op, int left, int right)
{
#define SLOPE_ANIME(n)	SetAnime(op, ((op->flip == OBJ_FLIP_L) ? left : right) + (n))
	op->timer++;

	switch (op->act3) {
	case 0:
		face_player(op);
		SLOPE_ANIME(0);
		op->timer = 0;
		op->act3++;
		break;
	case 1:
		if (near_x(op, NEAR_BREATH_S))
			goto breath;
		if (op->timer % HOWL_EVERY == 0) {
			face_player(op);
			SLOPE_ANIME(1);
			op->act3++;
		}
		break;
	case 2:
		if (near_x(op, NEAR_BREATH_S)) {
		breath:
			spawn_fx(op, FX_BREATH_SLOPE);
			face_player(op);
			SLOPE_ANIME(2);
			op->timer = 0;
			op->act3 = 3;
		} else if (op->anime.next == 0) {
			op->act3 = 0;
		}
		break;
	case 3:
		if (op->timer == BREATH_TIME) {
			face_player(op);
			SLOPE_ANIME(0);
			op->timer = 0;
			op->act3++;
		}
		break;
	case 4:
		if (op->timer == BREATH_REST)
			op->act3 = 0;
		break;
	}
#undef SLOPE_ANIME
}

static void wolf_breath_slope_r(OBJECT *op) { breath_slope(op, 6, 24); }
static void wolf_breath_slope_l(OBJECT *op) { breath_slope(op, 24, 6); }

/* 0x800EA970 (step 6) and 0x800EAC6C (step 7): the cannon wolf on a slope (X, ids 0 and 1) */
static void cannon_slope(OBJECT *op, int left, int right)
{
#define SLOPE_ANIME(n)	SetAnime(op, ((op->flip == OBJ_FLIP_L) ? left : right) + (n))
	op->timer++;

	switch (op->act3) {
	case 0:
		face_player(op);
		SLOPE_ANIME(0);
		op->timer = 0;
		op->act3++;
		break;
	case 1:
		if (near_x(op, NEAR_CANNON))
			goto fire;
		if (op->timer % HOWL_EVERY == 0) {
			face_player(op);
			SLOPE_ANIME(1);
			op->act3++;
		}
		break;
	case 2:
		if (near_x(op, NEAR_CANNON)) {
		fire:
			face_player(op);
			SLOPE_ANIME(2);
			op->timer = 0;
			op->act3 = 3;
		} else if (op->anime.next == 0) {
			op->act3 = 0;
		}
		break;
	case 3:
		if (op->timer == FIRE_AT)
			spawn_fx(op, FX_BALL);
		else if (op->timer == FIRE_TIME)
			op->act3 = 0;
		break;
	}
#undef SLOPE_ANIME
}

static void wolf_cannon_slope_r(OBJECT *op) { cannon_slope(op, 9, 27); }
static void wolf_cannon_slope_l(OBJECT *op) { cannon_slope(op, 27, 9); }

/* 0x800EAF68: visual 5 var 0 state 1, the flat breath cloud */
static void breath_main(OBJECT *op)
{
	OBJECT *wp = owner(op);

	op->flip = wp->flip;
	POS_X(op) = (wp->flip == OBJ_FLIP_L) ? POS_X(wp) - 45 : POS_X(wp) + 45;
	POS_Y(op) = POS_Y(wp) + 3;
	if (wp->act == ACT_DEAD) {
		op->act++;
		return;
	}
	fx_count(op)++;

	switch (op->act2) {
	case 0:
		if (op->anime.next == 0) {
			SetAnime(op, 13);
			op->atk_box = &breath_atk_box;
			op->act2++;
		}
		break;
	case 1:
		if (fx_count(op) == BREATH_TIME) {
			SetAnime(op, 14);
			op->atk_box = NULL;
			op->act2++;
		}
		break;
	case 2:
		if (op->anime.next == 0) {
			op->act++;
			return;
		}
		break;
	}
	ContactMega(op);
	AnimeTick(op);
	MoveObject2(op);
	if (!OffScreenCheck2(op, 32, 32))
		DisplayCheck(op, 32, 32);
	else if (wp->var != WOLF_RIDE_BREATH && wp->var != WOLF_RIDE_CANNON)
		op->act++;
}

/* 0x800EB110: visual 5 var 1 state 0, the slope breath cloud */
static void breath_s_init(OBJECT *op)
{
	OBJECT *wp = owner(op);
	int down;

	op->anime_tbl = wp->anime_tbl;
	op->tpage = wp->tpage;
	op->sprt_p = wp->sprt_p;
	op->clut = wp->clut & ~CLUT_FLASH;
	op->pri = 4;
	op->fall_f = 0;
	op->disp = 0;
	op->col_box = NULL;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	op->layer = wp->layer;
	op->flip = wp->flip;

	down = (wp->var == WOLF_SLOPE_R) == (op->flip == OBJ_FLIP_L);
	if (down) {
		POS_X(op) = (op->flip == OBJ_FLIP_L) ? POS_X(wp) - 41 : POS_X(wp) + 41;
		POS_Y(op) = POS_Y(wp) + 6;
		SetAnime(op, 15);
		op->atk_box = &breath_slope_d_box;
	} else {
		POS_X(op) = (op->flip == OBJ_FLIP_L) ? POS_X(wp) - 36 : POS_X(wp) + 36;
		POS_Y(op) = POS_Y(wp) - 17;
		SetAnime(op, 30);
		op->atk_box = &breath_slope_u_box;
	}
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav = 0;
	op->hp = 1;
	op->atk = BREATH_ATK;
	PlaySound(2, 16, op);
	op->act3 = 0;
	op->act2 = 0;
	op->act++;
}

/* 0x800EB29C: visual 5 var 1 state 1 */
static void breath_s_main(OBJECT *op)
{
	if (owner(op)->act == ACT_DEAD) {
		op->act++;
		return;
	}
	fx_count(op)++;

	switch (op->act2) {
	case 0:
		if (op->anime.next == 0)
			goto next_anime;
		break;
	case 1:
		ContactMega(op);
		if (fx_count(op) == BREATH_TIME) {
		next_anime:
			SetAnime(op, op->anime_no + 1);
			op->act2++;
		}
		break;
	case 2:
		if (op->anime.next == 0) {
			op->act++;
			return;
		}
		break;
	}
	AnimeTick(op);
	MoveObject2(op);
	if (OffScreenCheck2(op, 32, 32))
		op->act++;
	else
		DisplayCheck(op, 32, 32);
}

/* 0x800EB3C8: visual 5 var 2 state 0, the cannon ball */
static void ball_init(OBJECT *op)
{
	OBJECT *wp = owner(op);
	int left;

	op->anime_tbl = wp->anime_tbl;
	op->tpage = wp->tpage;
	op->sprt_p = wp->sprt_p;
	op->clut = wp->clut & ~CLUT_FLASH;
	op->pri = 4;
	op->col_box = &ball_col_box;
	op->atk_box = &ball_atk_box;
	op->fall_f = 0;
	op->disp = 0;
	op->dmg_box = &ball_dmg_box;
	op->dmg_tbl = dmg_tbl_normal;
	op->layer = wp->layer;
	SetAnime(op, 18);
	op->flip = wp->flip;

	left = (op->flip == OBJ_FLIP_L);
	if (wp->act2 == MODE_CANNON_FLAT) {
		POS_X(op) = left ? POS_X(wp) - 12 : POS_X(wp) + 12;
		POS_Y(op) = POS_Y(wp) - 12;
	} else if ((wp->act2 == MODE_CANNON_SLOPE_R) == left) {
		POS_X(op) = left ? POS_X(wp) - 16 : POS_X(wp) + 16;
		POS_Y(op) = POS_Y(wp) - 12;
	} else {
		POS_X(op) = POS_X(wp);
		POS_Y(op) = POS_Y(wp) - 14;
	}
	op->spd_y = BALL_VY;
	op->acc_x = 0;
	op->hp = BALL_HP;
	op->grav = BALL_GRAV;
	op->atk = BALL_ATK;
	op->spd_x = ((POS_X(&mega) - POS_X(op)) << 16) / BALL_DIV;
	PlaySound(2, 15, op);
	op->act3 = 0;
	op->act2 = 0;
	op->act++;
}

/* 0x800EB5A0: main type 6 entry */
void Wolfloid(OBJECT *op)
{
	op->old_x = op->x;
	op->disp = 0;
	op->old_y = op->y;
	wolf_act_tbl[op->act](op);
	CollideCheck(op);
}

/* 0x800EB600: step 1 */
static void wolf_nop(OBJECT *op)
{
}

/* 0x800EB608: states 2 and 3 */
static void wolf_delete(OBJECT *op)
{
	if (op->var == WOLF_RIDE_BREATH || op->var == WOLF_RIDE_CANNON)
		DeleteObject3(op);
	else if (op->act == ACT_DEAD)
		DeleteObject(op);
	else
		DeleteObject2(op);
}

/* 0x800EB670: visual type 5 entry */
void WolfloidFx(OBJECT *op)
{
	if (*(long *)&op->work[6] == 0 && op->col_f == 0)
		CornerCheck(op);
	op->disp = 0;
	switch (op->var) {
	case FX_BREATH:
		breath_act_tbl[op->act](op);
		break;
	case FX_BREATH_SLOPE:
		breath_s_act_tbl[op->act](op);
		break;
	case FX_BALL:
		ball_act_tbl[op->act](op);
		break;
	}
}

/* 0x800EB750: visual 5 var 0 state 0 */
static void breath_init(OBJECT *op)
{
	OBJECT *wp = owner(op);

	op->anime_tbl = wp->anime_tbl;
	op->tpage = wp->tpage;
	op->sprt_p = wp->sprt_p;
	op->clut = wp->clut & ~CLUT_FLASH;
	op->pri = 4;
	op->fall_f = 0;
	op->disp = 0;
	op->col_box = NULL;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->dmg_tbl = NULL;
	op->layer = wp->layer;
	SetAnime(op, 12);
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav = 0;
	op->hp = 1;
	op->atk = BREATH_ATK;
	PlaySound(2, 16, op);
	op->act3 = 0;
	op->act2 = 0;
	op->act++;
}

/* 0x800EB820, 0x800EB840, 0x800EB94C: visual 5 state 2 (three identical functions) */
static void fx_delete(OBJECT *op)
{
	DeleteObject3(op);
}

/* 0x800EB860: visual 5 var 2 state 1 */
static void ball_main(OBJECT *op)
{
	if (++fx_count(op) == 8) {
		SmokePuff(0, 0, op, 0, 1);
		fx_count(op) = 0;
	}
	if (op->col_f != 0 || ContactWeapon(op) < 0 || ContactMega(op) != 0) {
		SpawnExplosion(op);
		op->act = ACT_DEAD;
		return;
	}
	AnimeTick(op);
	MoveObject2(op);
	if (OffScreenCheck2(op, 16, 16))
		op->act++;
	else
		DisplayCheck(op, 16, 16);
}
