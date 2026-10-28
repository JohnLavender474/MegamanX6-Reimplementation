/*
 *	m27_mettaur_d2.c
 *
 *	ST10 (Shield Sheldon)  main object 27 : Mettaur D2
 *	                     visual object 32 : its shot
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
	ACT_HIDDEN,
	ACT_GONE
};

enum {
	MODE_REACT = 0,
	MODE_WALK,
	MODE_FALL,
	MODE_GUARD,
	MODE_SHOOT,
	MODE_JUMP
};

#define MET_HP		3
#define MET_ATK		1
#define MET_WALK_SPD	0x6000
#define MET_JUMP_SPD_Y	0x38000
#define MET_JUMP_SPD_X	0x10000
#define MET_GRAV	0x4200
#define MET_EDGE_DY	8
#define SHOT_ZONE_Y	40
#define SHOT_ZONE_X	80
#define SHOT_WAIT	61
#define GUARD_ZONE_Y	24
#define GUARD_ZONE_X	128
#define GUARD_TIME	60
#define REST_TIME	120

#define SHOT_ATK	2
#define SHOT_SPD	0x28000
#define SHOT_OFS_X	16
#define SHOT_OFS_Y	3

#define BUSY_GUARD	0x01
#define BUSY_SHOOT	0x02

#define busy(op)	((op)->work[2])
#define shot_count(op)	((op)->work[3])
#define guard_rest(op)	((op)->work[4])
#define shot_rest(op)	((op)->work[5])

extern ANIME	*met_anime[];
extern HITBOX	met_col_box, met_atk_box, met_guard_box, met_dmg_box;
extern HITBOX	shot_box, shot_col_box;

static u_char	junk_anime[] = { 9, 10, 11, 12 };

static void	met_shot_trigger(OBJECT *op);
static void	met_edge_check(OBJECT *op);
static void	met_guard_trigger(OBJECT *op);
static void	met_guard(OBJECT *op);
static void	met_init(OBJECT *op);
static void	met_main(OBJECT *op);
static void	met_hidden(OBJECT *op);
static void	met_jump(OBJECT *op);
static void	met_fall(OBJECT *op);
static void	met_shoot(OBJECT *op);
static void	met_walk(OBJECT *op);
static void	met_delete(OBJECT *op);
static void	shot_init(OBJECT *op);
static void	shot_move(OBJECT *op);
static void	shot_delete(OBJECT *op);

static void	(*met_mode[])(OBJECT *) = {
	HitReactionDispatch, met_walk, met_fall, met_guard, met_shoot, met_jump
};

static void	(*met_act[])(OBJECT *) = {
	met_init, met_main, met_delete, met_hidden, met_delete
};

static void	(*shot_act[])(OBJECT *) = {
	shot_init, shot_move, shot_delete
};

static void met_shot_trigger(OBJECT *op)
{
	short	dy;
	int	near;

	if (op->fall_f != 0 || (busy(op) & (BUSY_GUARD | BUSY_SHOOT)))
		return;
	if (shot_rest(op) != 0) {
		shot_rest(op)--;
		return;
	}
	dy = POS_Y(op) - POS_Y(&mega);
	if (dy < 0)
		dy = -dy;
	if (dy >= SHOT_ZONE_Y) {
		shot_count(op) = 0;
		return;
	}
	if (op->flip)
		near = (POS_X(&mega) < POS_X(op) + SHOT_ZONE_X);
	else
		near = (POS_X(op) - SHOT_ZONE_X < POS_X(&mega));
	if (near)
		shot_count(op)++;
	else
		shot_count(op) = 0;
	if (shot_count(op) >= SHOT_WAIT) {
		op->act2 = MODE_SHOOT;
		op->act3 = 0;
		SetAnime(op, 7);
		shot_count(op) = 0;
		busy(op) |= BUSY_SHOOT;
	}
}

static void met_edge_check(OBJECT *op)
{
	HITBOX	*cb;
	short	px, py;

	if (op->fall_f != 0)
		return;
	if (!(op->col_f & COL_D)) {
		SetAnime(op, 3);
		op->grav = MET_GRAV;
		op->act2 = MODE_FALL;
		op->act3 = 0;
		op->spd_y = 0;
		op->acc_x = 0;
		op->fall_f = -1;
		return;
	}
	cb = op->col_box;
	if (op->flip)
		px = POS_X(op) + cb->ox + cb->w;
	else
		px = POS_X(op) - cb->ox - cb->w;
	py = POS_Y(op) + cb->oy + cb->h + MET_EDGE_DY;
	if (TerrainPointCheck(op, px, py))
		return;
	if (op->var == 1) {
		op->flip = (op->flip == 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
		op->spd_x = -op->spd_x;
		return;
	}
	if (op->col_f & (COL_R | COL_L))
		return;
	SetAnime(op, 2);
	op->act2 = MODE_JUMP;
	op->act3 = 0;
	op->spd_y = MET_JUMP_SPD_Y;
	op->grav = MET_GRAV;
	op->spd_x = op->flip ? MET_JUMP_SPD_X : -MET_JUMP_SPD_X;
	op->fall_f = 1;
}

static void met_guard_trigger(OBJECT *op)
{
	OBJECT	*wp;
	short	dy;
	int	i;

	if (op->fall_f != 0 || (busy(op) & BUSY_GUARD))
		return;
	if (guard_rest(op) != 0) {
		guard_rest(op)--;
		return;
	}
	for (i = 0; i < WEAPON_OBJECT_MAX; i++) {
		wp = WEAPON(i);
		if (wp->flag == 0 || wp->type != WPN_LEMON)
			continue;
		dy = POS_Y(wp) - POS_Y(op);
		if (dy < 0)
			dy = -dy;
		if (op->flip) {
			if (mega.flip == 0 && op->x.l < mega.x.l && dy < GUARD_ZONE_Y
			    && POS_X(wp) < POS_X(op) + GUARD_ZONE_X)
				busy(op) |= BUSY_GUARD;
		} else {
			if (mega.flip != 0 && mega.x.l < op->x.l && dy < GUARD_ZONE_Y
			    && POS_X(op) - GUARD_ZONE_X < POS_X(wp))
				busy(op) |= BUSY_GUARD;
		}
		if (busy(op) & BUSY_GUARD) {
			op->act2 = MODE_GUARD;
			op->act3 = 0;
			SetAnime(op, 5);
			return;
		}
	}
}

static void met_guard(OBJECT *op)
{
	switch (op->act3) {
	case 0:
		op->act3++;
		SetAnime(op, 5);
		op->timer = GUARD_TIME;
		/* fall through */
	case 1:
		if (op->anime.flag) {
			op->dmg_tbl = dmg_tbl_deflect;
			op->dmg_box = &met_guard_box;
			op->atk_box = &met_guard_box;
		}
		if (--op->timer == 0) {
			op->act3++;
			SetAnime(op, 6);
		}
		break;
	case 2:
		if (op->anime.next == 0) {
			SetAnime(op, 1);
			op->dmg_tbl = dmg_tbl_normal;
			op->dmg_box = &met_dmg_box;
			op->atk_box = &met_atk_box;
			op->act2 = MODE_WALK;
			op->act3 = 0;
			guard_rest(op) = REST_TIME;
			busy(op) &= ~BUSY_GUARD;
		}
		break;
	}
	AnimeTick(op);
}

static void met_init(OBJECT *op)
{
	op->act2 = MODE_WALK;
	op->act++;
	op->old_x = op->x;
	op->old_y = op->y;
	op->layer = mega.layer;
	op->anime_tbl = met_anime;
	op->pri = 6;
	op->col_box = &met_col_box;
	op->atk_box = &met_atk_box;
	op->dmg_tbl = dmg_tbl_normal;
	op->dmg_box = &met_dmg_box;
	op->muteki = 0;
	op->fall_f = 0;
	op->col_f = 0;
	op->hp = MET_HP;
	op->atk = MET_ATK;
	op->spd_x = 0;
	op->acc_x = 0;
	op->spd_y = 0;
	op->grav = 0;
	busy(op) = 0;
	guard_rest(op) = 0;
	shot_count(op) = 0;
	shot_rest(op) = 0;
	op->flip = (mega.x.l < op->x.l) ? OBJ_FLIP_L : OBJ_FLIP_R;
	if ((u_char)(op->var - 2) < 2) {
		op->act = ACT_HIDDEN;
		op->dmg_box = 0;
		op->atk_box = 0;
		POS_X(op) = (POS_X(op) & ~15) + 8;
		POS_Y(op) = (POS_Y(op) & ~15) + 20;
		SetAnime(op, 0);
	} else {
		SetAnime(op, 1);
	}
}

static void met_main(OBJECT *op)
{
	int	ret;

	op->old_x = op->x;
	op->old_y = op->y;
	met_edge_check(op);
	met_shot_trigger(op);
	met_guard_trigger(op);
	met_mode[op->act2](op);
	ContactMega(op);
	ret = ContactWeapon(op);
	if (ret < 0) {
		SpawnExplosion(op);
		SpawnJunk(4, junk_anime, op);
		if (ret != KILL_BY_WPN55)
			DropItem(op, 18);
		op->act++;
		return;
	}
	if (OffScreenCheck2(op, 64, 64)) {
		op->act = ACT_GONE;
		return;
	}
	DisplayCheck(op, 37, 37);
	if ((op->col_f & (COL_U | COL_D)) == (COL_U | COL_D)
	    || (op->col_f & (COL_R | COL_L)) == (COL_R | COL_L)) {
		SpawnExplosion(op);
		SpawnJunk(4, junk_anime, op);
		op->disp = 0;
		op->act++;
		return;
	}
	CollideCheck(op);
}

void m27_mettaur_d2(OBJECT *op)
{
	met_act[op->act](op);
}

static void met_hidden(OBJECT *op)
{
	if (!TerrainPointCheck(op, POS_X(op), POS_Y(op))) {
		op->act = ACT_MAIN;
		op->act2 = MODE_WALK;
		op->var = 0;
		op->dmg_box = &met_dmg_box;
		op->atk_box = &met_atk_box;
	}
	if (OffScreenCheck2(op, 64, 64)) {
		op->act = ACT_DEAD;
		return;
	}
	if (op->var == 2)
		DisplayCheck(op, 37, 37);
}

static void met_jump(OBJECT *op)
{
	AnimeTick(op);
	if (op->act3 == 0) {
		if (op->anime.flag)
			op->act3++;
		return;
	}
	MoveObject2(op);
	if (op->spd_y < 0) {
		op->act3++;
		SetAnime(op, 3);
		op->grav = MET_GRAV;
		op->act2 = MODE_FALL;
		op->act3 = 0;
		op->spd_y = 0;
		op->acc_x = 0;
		op->fall_f = -1;
	}
}

static void met_fall(OBJECT *op)
{
	AnimeTick(op);
	if (op->act3 == 0) {
		op->dmg_tbl = dmg_tbl_normal;
		MoveObject2(op);
		if (op->col_f & COL_D) {
			SetAnime(op, 4);
			op->spd_y = 0;
			op->grav = 0;
			op->spd_x = 0;
			op->acc_x = 0;
			op->act3++;
		}
	} else if (op->anime.next == 0) {
		SetAnime(op, 1);
		op->act2 = MODE_WALK;
		op->act3 = 0;
		op->fall_f = 0;
	}
}

static void met_shoot(OBJECT *op)
{
	OBJECT	*sp;

	AnimeTick(op);
	if (op->anime.flag) {
		op->anime.flag = 0;
		if ((sp = GetVisualObject()) != 0) {
			PlaySound(SE_BANK_ENEMY, SE_MET_SHOT, op);
			sp->flag = 0x41;
			sp->type = 32;
			sp->x = op->x;
			sp->y = op->y;
			sp->sprt_p = op->sprt_p;
			sp->tpage = op->tpage;
			sp->clut = op->clut;
			sp->layer = op->layer;
			sp->anime_tbl = op->anime_tbl;
			sp->flip = op->flip;
		}
	}
	if (op->anime.next == 0) {
		op->act2 = MODE_WALK;
		op->act3 = 0;
		SetAnime(op, 1);
		shot_rest(op) = REST_TIME;
		busy(op) &= ~BUSY_SHOOT;
	}
}

static void met_walk(OBJECT *op)
{
	if (op->act3 == 0) {
		op->dmg_tbl = dmg_tbl_normal;
		op->dmg_box = &met_dmg_box;
		op->atk_box = &met_atk_box;
		op->act3++;
		SetAnime(op, 1);
		op->spd_x = op->flip ? MET_WALK_SPD : -MET_WALK_SPD;
		op->acc_x = 0;
		op->spd_y = 0;
		op->grav = 0;
	}
	AnimeTick(op);
	MoveObject(op);
	if (op->flip) {
		if (op->col_f & COL_R) {
			op->flip = OBJ_FLIP_L;
			op->spd_x = -op->spd_x;
		}
	} else {
		if (op->col_f & COL_L) {
			op->flip = OBJ_FLIP_R;
			op->spd_x = -op->spd_x;
		}
	}
}

static void met_delete(OBJECT *op)
{
	if (op->act == ACT_GONE)
		DeleteObject2(op);
	else
		DeleteObject(op);
}

void v32_mettaur_shot(OBJECT *op)
{
	shot_act[op->act](op);
}

static void shot_init(OBJECT *op)
{
	op->disp = 1;
	op->act++;
	op->spd_x = op->flip ? SHOT_SPD : -SHOT_SPD;
	op->acc_x = 0;
	op->spd_y = 0;
	op->grav = 0;
	POS_X(op) += op->flip ? SHOT_OFS_X : -SHOT_OFS_X;
	op->pri = 6;
	op->col_box = &shot_col_box;
	op->dmg_tbl = dmg_tbl_one_hit;
	op->dmg_box = &shot_box;
	op->atk_box = &shot_box;
	op->atk = SHOT_ATK;
	POS_Y(op) += SHOT_OFS_Y;
	SetAnime(op, 8);
}

static void shot_move(OBJECT *op)
{
	AnimeTick(op);
	MoveObject(op);
	if (ContactMega(op)) {
		op->act = 2;
		return;
	}
	if (ContactWeapon(op) < 0) {
		op->act = 2;
		return;
	}
	if (op->col_f == 0)
		CornerCheck(op);
	if (op->col_f != 0) {
		SpawnHitEffect(op);
		op->act++;
		return;
	}
	if (OffScreenCheck2(op, 32, 32)) {
		op->act++;
		return;
	}
	DisplayCheck(op, 16, 16);
}

static void shot_delete(OBJECT *op)
{
	DeleteObject3(op);
}
