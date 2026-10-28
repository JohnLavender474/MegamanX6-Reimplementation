/*
 *	m30_guardian.c
 *
 *	ST10 (Shield Sheldon)  main object 30 : Guardian
 *	                     visual object 35 : its shot
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
	ACT_DELETE,
	ACT_GONE
};

enum {
	MODE_REACT = 0,
	MODE_RESUME,
	MODE_NOP2,
	MODE_WAIT,
	MODE_TURN,
	MODE_ATTACK,
	MODE_NOP6,
	MODE_UNUSED
};

#define GRD_HP		6
#define GRD_ATK		6
#define GRD_ATK_BIG	7
#define GRD_BIG(op)	((op)->var & 0xf0)
#define GRD_RIGHT(op)	((op)->var & 0x0f)
#define WAIT_TIME	120
#define LOOK_MASK	31
#define NEAR_X		188
#define TURN_TIME	20
#define SHOT_COUNT	4
#define FIRST_AIM	30
#define NEXT_AIM	20
#define REST_TIME	10
#define MUZZLE_X	6
#define DEAD_TIME	32
#define DEAD_BOOM	6

#define SHOT_HP		1
#define SHOT_ATK	3
#define SHOT_MUL	3

#define DIR_UP		8
#define DIR_DOWN	24

#define timer2(op)	(*(short *)&(op)->work[0])
#define aim_dir(op)	((op)->work[3])
#define open_f(op)	((op)->work[13])
#define resume(op)	((op)->work[22])

extern ANIME	*grd_anime[];
extern HITBOX	grd_atk_close, grd_atk_open, grd_atk_bar, grd_dmg_body, grd_col_box;
extern HITBOX	big_atk_close, big_atk_open, big_atk_bar, big_dmg_body, big_col_box;
extern HITBOX	grd_shield_close, grd_shield_open, grd_col_open;
extern HITBOX	shot_box, shot_col_box;

HITBOX	*grd_box_close;			/* 0x800FCFF0 */
HITBOX	*grd_box_open;			/* 0x800FCFF4 */
HITBOX	*grd_box_body;			/* 0x800FCFF8 */
HITBOX	*grd_box_bar;			/* 0x800FCFFC */

static u_char	junk_anime[] = { 8, 9, 10, 11 };

static void	grd_init(OBJECT *op);
static void	grd_main(OBJECT *op);
static void	atk_fire(OBJECT *op);
static void	grd_dead(OBJECT *op);
static void	grd_delete(OBJECT *op);
static void	grd_nop(OBJECT *op);
static void	grd_wait(OBJECT *op);
static void	wait_start(OBJECT *op);
static void	wait_hover(OBJECT *op);
static void	wait_near(OBJECT *op);
static void	grd_turn(OBJECT *op);
static void	turn_start(OBJECT *op);
static void	turn_flip(OBJECT *op);
static void	turn_end(OBJECT *op);
static void	grd_attack(OBJECT *op);
static void	atk_start(OBJECT *op);
static void	atk_open(OBJECT *op);
static void	atk_aim(OBJECT *op);
static void	atk_rest(OBJECT *op);
static void	atk_close(OBJECT *op);
static void	grd_nop6(OBJECT *op);
static void	grd_unused(OBJECT *op);
static void	unused0(OBJECT *op);
static void	unused1(OBJECT *op);
static void	unused2(OBJECT *op);
static void	unused3(OBJECT *op);
static void	grd_resume(OBJECT *op);
static int	player_behind(OBJECT *op);
static void	shot_init(OBJECT *op);
static void	shot_move(OBJECT *op);
static void	shot_delete(OBJECT *op);

static void	(*grd_act[])(OBJECT *) = {
	grd_init, grd_main, grd_dead, grd_delete, grd_delete
};

static void	(*grd_mode[])(OBJECT *) = {
	HitReactionDispatch, grd_resume, grd_nop, grd_wait,
	grd_turn, grd_attack, grd_nop6, grd_unused
};

static void	(*grd_wait_tbl[])(OBJECT *) = { wait_start, wait_hover, wait_near };
static void	(*grd_turn_tbl[])(OBJECT *) = { turn_start, turn_flip, turn_end };
static void	(*grd_attack_tbl[])(OBJECT *) = {
	atk_start, atk_open, atk_aim, atk_fire, atk_rest, atk_close
};
static void	(*grd_unused_tbl[])(OBJECT *) = { unused0, unused1, unused2, unused3 };

static void	(*shot_act[])(OBJECT *) = { shot_init, shot_move, shot_delete };

static void grd_init(OBJECT *op)
{
	int	bank;

	op->hp = GRD_HP;
	op->muteki = 0;
	op->anime_tbl = grd_anime;
	op->fall_f = 0;
	op->pri = 6;
	op->dmg_box = 0;
	op->atk_box = 0;
	op->col_f = 0;
	op->spd_x = 0;
	op->spd_y = 0;
	op->acc_x = 0;
	op->grav = 0;
	op->work[27] = 1;
	open_f(op) = 0;
	op->layer = mega.layer;
	op->old_x = op->x;
	op->old_y = op->y;
	if (!GRD_BIG(op)) {
		op->col_box = &grd_col_box;
		op->atk = GRD_ATK;
		grd_box_close = &grd_atk_close;
		grd_box_open = &grd_atk_open;
		grd_box_body = &grd_dmg_body;
		grd_box_bar = &grd_atk_bar;
	} else {
		bank = GetSpriteBank(op->type);
		op->tpage = bank_tpage[bank + 1] >> 7;
		bank = GetSpriteBank(op->type);
		op->clut = ((480 + (GetSpriteBank(op->type) + 9) / 4) << 6)
			 | (((bank + 1) * 4 + 32) % 16);
		bank = GetSpriteBank(op->type);
		op->atk = GRD_ATK_BIG;
		op->col_box = &big_col_box;
		op->sprt_p = (char *)sprt_bank + sprt_bank->ofs[bank + 1];
		grd_box_close = &big_atk_close;
		grd_box_open = &big_atk_open;
		grd_box_body = &big_dmg_body;
		grd_box_bar = &big_atk_bar;
	}
	SetAnime(op, 0);
	op->flip = GRD_RIGHT(op) ? OBJ_FLIP_R : OBJ_FLIP_L;
	op->act = ACT_MAIN;
	op->act2 = MODE_WAIT;
	op->act3 = 0;
}

static void grd_main(OBJECT *op)
{
	int	ret;

	op->old_x = op->x;
	op->old_y = op->y;
	AnimeTick(op);
	grd_mode[op->act2](op);
	if (op->act != ACT_DEAD) {
		op->atk_box = open_f(op) ? grd_box_open : grd_box_close;
		ContactMega(op);
		op->atk_box = grd_box_bar;
		ContactMega(op);
	}
	op->dmg_tbl = dmg_tbl_deflect;
	if (open_f(op) == 0) {
		op->dmg_box = &grd_shield_close;
		op->col_box = &grd_col_box;
	} else {
		op->dmg_box = &grd_shield_open;
		op->col_box = &grd_col_open;
	}
	ret = ContactWeapon(op);
	if (ret >= 0) {
		op->dmg_tbl = dmg_tbl_normal;
		op->dmg_box = grd_box_body;
		ret = ContactWeapon(op);
		if (ret >= 0) {
			if (OffScreenCheck2(op, 96, 96)) {
				op->act = ACT_GONE;
				return;
			}
			DisplayCheck(op, 64, 64);
			if ((op->col_f & (COL_U | COL_D)) != (COL_U | COL_D)
			    && (op->col_f & (COL_R | COL_L)) != (COL_R | COL_L)) {
				CollideCheck(op);
				return;
			}
			goto explode;
		}
	}
	if (ret != KILL_BY_WPN55)
		DropItem(op, 20);
explode:
	op->col_box = 0;
	SpawnExplosion(op);
	SpawnJunk(4, junk_anime, op);
	op->timer = DEAD_TIME;
	timer2(op) = DEAD_BOOM;
	op->disp = 0;
	op->act = ACT_DEAD;
}

static void atk_fire(OBJECT *op)
{
	OBJECT	*sp;
	u_char	dir;

	dir = aim_dir(op);
	if ((u_char)(dir - 9) < 15) {
		if (op->flip)
			goto back;
	} else if ((u_char)(dir - DIR_UP) >= 17) {
		if (!op->flip)
			goto back;
	}
	goto fire;
back:
	op->timer = REST_TIME;
	op->act3++;
fire:
	if (op->act3 == 4)
		return;
	SetAnime(op, 4);
	if ((sp = GetVisualObject()) != 0) {
		sp->flag = 0x41;
		sp->type = 35;
		sp->var = op->var;
		sp->tpage = op->tpage;
		sp->clut = op->clut;
		sp->anime_tbl = op->anime_tbl;
		sp->sprt_p = op->sprt_p;
		sp->flip = op->flip;
		sp->layer = op->layer;
		sp->x = op->x;
		sp->y = op->y;
		SetSpeedDir(sp, aim_dir(op));
		sp->act = 0;
		sp->spd_x *= SHOT_MUL;
		sp->spd_y *= SHOT_MUL;
	}
	if (--op->timer == 0) {
		op->timer = REST_TIME;
		op->act3++;
	} else {
		timer2(op) = NEXT_AIM;
		op->act3--;
	}
}

void m30_guardian(OBJECT *op)
{
	grd_act[op->act](op);
}

static void grd_dead(OBJECT *op)
{
	if (--op->timer == 0) {
		op->act = ACT_DELETE;
		return;
	}
	if (--timer2(op) == 0) {
		timer2(op) = DEAD_BOOM;
		SpawnExplosionRand(op, 1, 16, 16);
	}
}

static void grd_delete(OBJECT *op)
{
	*(long *)&op->work[2] = 0;
	*(long *)&op->work[6] = 0;
	*(long *)&op->work[10] = 0;
	*(long *)&op->work[14] = 0;
	*(long *)&op->work[22] = 0;
	if (op->act == ACT_GONE)
		DeleteObject2(op);
	else
		DeleteObject(op);
}

static void grd_nop(OBJECT *op)
{
}

static void grd_wait(OBJECT *op)
{
	grd_wait_tbl[op->act3](op);
}

static void wait_start(OBJECT *op)
{
	SetAnime(op, 0);
	op->timer = WAIT_TIME;
	op->act3++;
}

static void wait_hover(OBJECT *op)
{
	if ((op->timer & LOOK_MASK) == 0 && player_behind(op)) {
		op->act2 = MODE_TURN;
		op->act3 = 0;
		return;
	}
	if (--op->timer == 0)
		op->act3++;
}

static void wait_near(OBJECT *op)
{
	if (POS_X(&mega) - POS_X(op) < NEAR_X) {
		op->act2 = player_behind(op) ? MODE_TURN : MODE_ATTACK;
		op->act3 = 0;
	}
}

static void grd_turn(OBJECT *op)
{
	grd_turn_tbl[op->act3](op);
}

static void turn_start(OBJECT *op)
{
	SetAnime(op, 2);
	op->act3++;
}

static void turn_flip(OBJECT *op)
{
	if (op->anime.flag) {
		op->flip ^= OBJ_FLIP_R;
		SetAnime(op, 1);
		op->timer = TURN_TIME;
		op->act3++;
	}
}

static void turn_end(OBJECT *op)
{
	if (--op->timer == 0) {
		op->act2 = MODE_ATTACK;
		op->act3 = 0;
	}
}

static void grd_attack(OBJECT *op)
{
	grd_attack_tbl[op->act3](op);
}

static void atk_start(OBJECT *op)
{
	SetAnime(op, 3);
	op->act3++;
}

static void atk_open(OBJECT *op)
{
	if (op->anime.flag == ANM_END)
		open_f(op) = 1;
	if (op->anime.flag == ANM_FIRE) {
		op->timer = SHOT_COUNT;
		timer2(op) = FIRST_AIM;
		op->act3++;
	}
}

static void atk_aim(OBJECT *op)
{
	if (--timer2(op) != 0)
		return;
	POS_X(op) += op->flip ? MUZZLE_X : -MUZZLE_X;
	aim_dir(op) = AimAt(op, &mega);
	POS_X(op) += op->flip ? -MUZZLE_X : MUZZLE_X;
	op->act3++;
}

static void atk_rest(OBJECT *op)
{
	if (--op->timer == 0) {
		SetAnime(op, 12);
		op->act3++;
	}
}

static void atk_close(OBJECT *op)
{
	if (op->anime.flag == ANM_END)
		open_f(op) = 0;
	if (op->anime.flag == ANM_FIRE) {
		op->act2 = MODE_WAIT;
		op->act3 = 0;
	}
}

static void grd_nop6(OBJECT *op)
{
}

static void grd_unused(OBJECT *op)
{
	grd_unused_tbl[op->act3](op);
}

static void unused0(OBJECT *op) { }
static void unused1(OBJECT *op) { }
static void unused2(OBJECT *op) { }
static void unused3(OBJECT *op) { }

static void grd_resume(OBJECT *op)
{
	op->act2 = resume(op);
}

static int player_behind(OBJECT *op)
{
	return (op->flip ^ ((POS_X(op) < POS_X(&mega)) << 6)) != 0;
}

void v35_guardian_shot(OBJECT *op)
{
	shot_act[op->act](op);
}

static void shot_init(OBJECT *op)
{
	op->dmg_tbl = dmg_tbl_one_hit;
	op->act = 1;
	op->disp = 1;
	op->pri = 0;
	op->clut &= ~CLUT_FLASH;
	POS_X(op) += op->flip ? MUZZLE_X : -MUZZLE_X;
	PlaySound(SE_BANK_ENEMY, SE_GUARDIAN_SHOT, op);
	op->col_box = &shot_col_box;
	op->dmg_box = &shot_box;
	op->atk_box = &shot_box;
	op->hp = SHOT_HP;
	op->atk = SHOT_ATK;
	SetAnime(op, 5);
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
		SpawnExplosion(op);
		op->act = 2;
	} else {
		op->clut &= ~CLUT_FLASH;
	}
	if (op->col_f == 0)
		CornerCheck(op);
	if (op->col_f != 0) {
		SpawnHitEffect(op);
		op->act = 2;
		return;
	}
	if (OffScreenCheck2(op, 32, 32)) {
		op->act = 2;
		return;
	}
	DisplayCheck(op, 16, 16);
}

static void shot_delete(OBJECT *op)
{
	DeleteObject3(op);
}
