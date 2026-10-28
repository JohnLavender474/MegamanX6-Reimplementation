/*
 *	m04_rescue_reploid.c
 *
 *	stage object 04 : rescue Reploid (main program, shared by all stages)
 *	visual object 24 : the shots of an infected Reploid
 */
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>

#include "object.h"
#include "game.h"
#include "sound.h"

enum {
	ACT_INIT = 0,
	ACT_WAIT,
	ACT_HEAL,
	ACT_TELE,
	ACT_TELE_WAIT,
	ACT_BEAM,
	ACT_INFECT,
	ACT_OUT,
	ACT_GONE
};

enum {
	MODE_REACT = 0,
	MODE_NONE,
	MODE_CHANGE,
	MODE_ARM,
	MODE_WAIT,
	MODE_ATTACK
};

enum {
	RPL_NEW = 0,
	RPL_SAVED = 2,
	RPL_DEAD = 3,
	RPL_LOST = 4
};

#define RPL_SPECIAL	0x80
#define RPL_BANK	141
#define INF_HP		13
#define INF_ATK		6

#define catch_f(op)	((op)->act4)
#define base_y(op)	(*(short *)&(op)->work[0])
#define hover(op)	((op)->work[2])
#define left(op)	((op)->work[3])
#define tick(op)	((op)->work[4])
#define wait(op)	((op)->work[5])
#define side(op)	((op)->work[6])
#define glow(op)	((op)->work[7])
#define set_b(op)	((op)->work[8])

#define tama_id(op)	(*(short *)&((u_char *)(op))[0x88])
#define tama_par(op)	(*(OBJECT **)&((u_char *)(op))[0x7c])

extern ANIME	*rpl_anime[];
extern HITBOX	rpl_col_a, rpl_col_b;
extern HITBOX	rpl_atk_a, rpl_dmg_a, rpl_atk_b, rpl_dmg_b;
extern HITBOX	tama_atk, tama_col, bomb_col, bomb_atk, bomb_dmg;
extern u_char	dmg_tbl_normal[];
extern char	tama_ofs[8][4];
extern u_long	*sprite_tpage_tbl;
extern void	BombTrail(int a, int b, OBJECT *op, int c, int d);

static u_char	heal_tbl[4] = { 2, 4, 8, 16 };

static void	rpl_init(OBJECT *op);
static void	rpl_wait(OBJECT *op);
static void	rpl_heal_act(OBJECT *op);
static void	rpl_tele(OBJECT *op);
static void	rpl_tele_wait(OBJECT *op);
static void	rpl_beam(OBJECT *op);
static void	rpl_infect(OBJECT *op);
static void	rpl_out(OBJECT *op);
static void	rpl_gone(OBJECT *op);
static void	inf_none(OBJECT *op);
static void	inf_change(OBJECT *op);
static void	inf_arm(OBJECT *op);
static void	inf_wait(OBJECT *op);
static void	inf_attack(OBJECT *op);

static void (*rpl_act_tbl[])(OBJECT *) = {
	rpl_init, rpl_wait, rpl_heal_act, rpl_tele, rpl_tele_wait,
	rpl_beam, rpl_infect, rpl_out, rpl_gone
};

static void (*inf_mode_tbl[])(OBJECT *) = {
	HitReactionDispatch, inf_none, inf_change, inf_arm, inf_wait, inf_attack
};

void PauseObjects(u_char on)
{
	game.pause[0] = on;
	game.pause[1] = on;
	game.pause[2] = on;
	game.pause[3] = on;
	game.pause[4] = on;
	game.pause[5] = on;
	game.pause[6] = on;
	game.pause[7] = on;
	game.pause[9] = on;
}

static int face_player(OBJECT *op)
{
	if (side(op) == OBJ_FLIP_L) {
		if (mega.x.l < op->x.l)
			{ side(op) = OBJ_FLIP_L; return 0; }
		side(op) = OBJ_FLIP_R;
		return 1;
	}
	if (mega.x.l >= op->x.l)
		{ side(op) = OBJ_FLIP_R; return 0; }
	side(op) = OBJ_FLIP_L;
	return 1;
}

void SetReploidStatus(u_char no, u_char st)
{
	u_long *p = &game.reploid[no >> 3];
	int sh = (no & 7) << 2;

	*p = (*p & ~(0xf << sh)) | (st << sh);
}

int GetReploidStatus(u_char no)
{
	return (game.reploid[no >> 3] >> ((no & 7) << 2)) & 0xf;
}

static void anime4(OBJECT *op, int sl, int sr, int hl, int hr, int bl, int br)
{
	if (set_b(op))
		SetAnime(op, side(op) ? br : bl);
	else if (!hover(op))
		SetAnime(op, side(op) ? sr : sl);
	else
		SetAnime(op, side(op) ? hr : hl);
}

static void spawn_virus(OBJECT *op, u_char var)
{
	OBJECT *mp = GetMainObject();

	if (mp == NULL)
		return;
	mp->flag = 0x41;
	mp->type = 15;
	mp->var = var;
	mp->layer = op->layer;
	if ((var & 0xf0) == 0x30) {
		mp->x.w.lo = 0; mp->x.w.hi = POS_X(op);
		mp->y.w.lo = 0; mp->y.w.hi = POS_Y(op);
	}
}

static void spawn_tama(OBJECT *op, u_char var)
{
	OBJECT *tp = GetVisualObject();

	if (tp == NULL)
		return;
	tp->flag = 0x41;
	tp->type = 24;
	tp->var = var;
	tama_id(tp) = (char)op->var;
	tp->clut = op->clut & ~CLUT_FLASH;
	tp->anime_tbl = op->anime_tbl;
	tp->sprt_p = op->sprt_p;
	tp->tpage = op->tpage;
	tp->flip = side(op);
	tp->layer = op->layer;
	tp->x.w.lo = 0; tp->x.w.hi = POS_X(op);
	tp->y.w.lo = 0;
	tama_par(tp) = op;
	tp->y.w.hi = POS_Y(op);
}

void Reploid(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	rpl_act_tbl[op->act](op);
}

static void rpl_init(OBJECT *op)
{
	int bank, ofs;

	if (op->var != RPL_SPECIAL) {
		int st = GetReploidStatus(op->var);
		if (st >= RPL_SAVED && st <= RPL_LOST) {
			op->act = ACT_GONE;
			return;
		}
	}
	set_b(op) = ((op->var & 0xf0) == 0x10);

	op->flag = 0x41;
	op->dmg_tbl = dmg_tbl_normal;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
	op->pri = 2;
	op->anime_tbl = rpl_anime;
	op->layer = mega.layer;
	bank = GetSpriteBank(RPL_BANK);
	op->tpage = ((u_long *)sprite_tpage_tbl)[bank] >> 7;
	op->sprt_p = (char *)sprt_bank + sprt_bank->ofs[GetSpriteBank(RPL_BANK)];
	bank = GetSpriteBank(RPL_BANK);
	ofs = bank * 4 + 32;
	op->clut = (ofs % 16) | (((GetSpriteBank(RPL_BANK) + 8) / 4 + 480) << 6);
	op->flip = OBJ_FLIP_L;
	op->old_x = op->x;
	op->old_y = op->y;
	face_player(op);

	if (!set_b(op)) {
		op->col_box = &rpl_col_a;
		CollideCheck(op);
		if (op->col_f & COL_D) {
			hover(op) = 0;
			SetAnime(op, side(op) ? 11 : 0);
		} else {
			hover(op) = 1;
			base_y(op) = POS_Y(op);
			SetAnime(op, side(op) ? 14 : 5);
		}
	} else {
		op->col_box = &rpl_col_b;
		CollideCheck(op);
		SetAnime(op, side(op) ? 30 : 23);
	}
	op->act = ACT_WAIT;
	catch_f(op) = 1;
	op->act2 = 0;
	if (op->var == RPL_SPECIAL)
		catch_f(op) = 0;
}

static void give_reward(OBJECT *op, u_char amount, u_char r)
{
	char max = game.max_hp[mega.var];
	u_char hpf = mega.hp & 0x80;
	int i, lv;

	if ((mega.hp & 0x7f) < max) {
		PauseObjects(1);
		tick(op) = 4;
		left(op) = amount;
		op->act = ACT_HEAL;
		op->act2 = op->act3 = 0;
		op->flag |= 0x08;
	} else
		op->act = ACT_TELE;

	if (!((mega.hp & 0x7f) < game.max_hp[mega.var]))
		mega.hp = hpf | game.max_hp[mega.var];

	for (i = 0; i < 2; i++) {
		if (!(game.tanks & (0x1000 << i)))
			continue;
		lv = game.tank[i] & 0x7f;
		if (lv >= 32)
			continue;
		lv = (char)(r + lv);
		if (lv >= 32) {
			lv = 32;
			PlaySound(SE_BANK_SYS, SE_TANK_FULL, NULL);
		}
		game.tank[i] = lv | 0x80;
		break;
	}
}

static int rescue_test(OBJECT *op)
{
	OBJECT *pp = &mega;

	if (((char *)&mega)[0xd5] != 0) {
		pp = &mega_sub[0];
		if (pp->act2 == 0)
			pp = &mega_sub[1];
	}
	if ((pp->hp & 0x7f) == 0)
		return 0;
	if (!TerrainOverlap(op, pp))
		return 0;

	if (op->var != RPL_SPECIAL)
		SetReploidStatus(op->var, RPL_SAVED);
	op->act2 = 0;
	if (++game.lives < 10)
		PlaySound(SE_BANK_SYS, SE_1UP, NULL);
	else
		game.lives = 9;
	{
		u_char r = GetRNG() & 3;
		give_reward(op, heal_tbl[r], r);
	}
	return 1;
}

static void rpl_wait(OBJECT *op)
{
	AnimeTick(op);
	if (face_player(op))
		anime4(op, 0, 11, 5, 14, 23, 30);

	if (rescue_test(op)) {
		catch_f(op) = 0;
		return;
	}
	if (catch_f(op) == 2) {
		SetReploidStatus(op->var, RPL_LOST);
		op->act = ACT_INFECT;
		op->act2 = MODE_CHANGE;
		op->act3 = 0;
		catch_f(op) = 0;
		return;
	}
	if (hover(op) == 1)
		op->y.w.hi = base_y(op) + (rsin((frame_count & 0x7f) << 5) >> 10);

	if (OffScreenCheck2(op, 48, 48)) {
		if (op->var != RPL_SPECIAL) {
			op->disp = 0;
			op->act = ACT_OUT;
			catch_f(op) = 0;
		}
	} else
		DisplayCheck(op, set_b(op) ? 32 : 16, 32);
}

static void rpl_heal_act(OBJECT *op)
{
	char max;

	AnimeTick(op);
	if (op->act3 == 0) {
		if (--tick(op) != 0)
			return;
		tick(op) = 4;
		max = game.max_hp[mega.var];
		if ((mega.hp & 0x7f) < max)
			mega.hp = (mega.hp & 0x80) | (u_char)(mega.hp + 1);
		else {
			op->act3 = 1;
			mega.hp = (mega.hp & 0x80) | max;
		}
		PlaySound(SE_BANK_SYS, SE_HP_TICK, NULL);
		if (--left(op) == 0)
			op->act3 = 1;
	} else {
		PauseObjects(0);
		op->act = ACT_TELE;
		op->act2 = 0;
		op->flag &= ~0x08;
	}
}

static void rpl_tele(OBJECT *op)
{
	face_player(op);
	anime4(op, 9, 17, 10, 18, 25, 32);
	op->col_box = NULL;
	op->act = ACT_TELE_WAIT;
	wait(op) = 80;
}

static void rpl_tele_wait(OBJECT *op)
{
	AnimeTick(op);
	if (face_player(op))
		anime4(op, 9, 17, 10, 18, 25, 32);
	if (wait(op)) {
		wait(op)--;
		return;
	}
	op->spd_y = 0x80000;
	op->act = ACT_BEAM;
	op->spd_x = 0;
	SetAnime(op, set_b(op) ? 29 : 8);
}

static void rpl_beam(OBJECT *op)
{
	AnimeTick(op);
	if (op->anime.flag == ANM_FIRE)
		MoveObject(op);
	if (OffScreenCheck2(op, 64, 64))
		DeleteObject3(op);
	else
		DisplayCheck(op, 32, 32);
}

static void rpl_infect(OBJECT *op)
{
	AnimeTick(op);
	inf_mode_tbl[op->act2](op);

	if (ContactWeapon(op) < 0) {
		spawn_virus(op, 0x31);
		SetReploidStatus(op->var, RPL_DEAD);
		op->disp = 0;
		SpawnExplosion(op);
		op->act = ACT_GONE;
		return;
	}
	ContactMega(op);
	if (OffScreenCheck(op)) {
		SetReploidStatus(op->var, RPL_LOST);
		op->act = ACT_GONE;
		return;
	}
	DisplayCheck(op, 32, 32);
	if (glow(op))
		op->clut |= CLUT_FLASH;
}

static void rpl_out(OBJECT *op)
{
	DeleteObject2(op);
}

static void rpl_gone(OBJECT *op)
{
	DeleteObject3(op);
}

static void inf_none(OBJECT *op)
{
}

static void inf_change(OBJECT *op)
{
	switch (op->act3) {
	case 0:
		left(op) = 130;
		anime4(op, 1, 12, 6, 15, 24, 31);
		op->act3++;
		break;
	case 1:
		if (--left(op) == 0) {
			PlaySound(SE_BANK_ENEMY, SE_RPL_CHANGE, op);
			glow(op) = 1;
			left(op) = 60;
			op->act3++;
		}
		break;
	case 2:
		if (--left(op) == 0) {
			glow(op) = 0;
			op->act3 = 0;
			op->act2++;
		}
		break;
	}
}

static void inf_arm(OBJECT *op)
{
	op->hp = INF_HP;
	op->atk = INF_ATK;
	if (!set_b(op)) {
		wait(op) = 60;
		op->dmg_box = &rpl_dmg_a;
		op->atk_box = &rpl_atk_a;
		anime4(op, 19, 20, 21, 22, 0, 0);
	} else {
		wait(op) = 20;
		op->dmg_box = &rpl_dmg_b;
		op->atk_box = &rpl_atk_b;
		SetAnime(op, side(op) ? 35 : 28);
	}
	op->act2++;
}

static void inf_wait(OBJECT *op)
{
	if (--wait(op) != 0)
		return;
	anime4(op, 2, 13, 7, 16, 26, 33);
	op->act2++;
}

static void inf_attack(OBJECT *op)
{
	if (op->anime.flag == ANM_FIRE) {
		if (((char *)&mega)[0xd4] != 0)
			return;
		PlaySound(SE_BANK_ENEMY, SE_RPL_SHOT, op);
		if (!set_b(op)) {
			spawn_tama(op, 0);
			spawn_tama(op, 1);
		} else
			spawn_tama(op, 16);
	} else if (op->anime.flag == ANM_END) {
		side(op) = (mega.x.l < op->x.l) ? OBJ_FLIP_L : OBJ_FLIP_R;
		if (!set_b(op)) {
			anime4(op, 19, 20, 21, 22, 0, 0);
			wait(op) = 60;
		} else {
			SetAnime(op, side(op) ? 35 : 28);
			wait(op) = 20;
		}
		op->act2--;
	}
}

/*
 *	visual object 24 : shots
 */
static void tama_init(OBJECT *op);
static void tama_main(OBJECT *op);
static void tama_delete(OBJECT *op);

void (*rpl_tama_tbl[])(OBJECT *) = { tama_init, tama_main, tama_delete };

static void tama_init(OBJECT *op)
{
	char ofs[8][4];
	int g;

	memcpy(ofs, tama_ofs, sizeof(ofs));
	op->atk = 4;
	g = (u_short)tama_id(op) >> 4;
	op->hp = 1;
	op->dmg_tbl = NULL;
	op->fall_f = 1;
	op->disp = 0;
	op->col_box = NULL;
	op->atk_box = NULL;
	op->dmg_box = NULL;
	op->spd_y = 0;

	if (op->var == 0) {
		op->pri = 1;
		SetAnime(op, 3);
		if (op->flip == OBJ_FLIP_L) {
			op->x.w.hi += ofs[g][0];
			op->y.w.hi += ofs[g][1];
		} else {
			op->x.w.hi += ofs[g][2];
			op->y.w.hi += ofs[g][3];
		}
	} else if (op->var == 1) {
		op->pri = 1;
		SetAnime(op, 4);
		if (op->flip == OBJ_FLIP_L) {
			op->spd_x = -0x40000;
			op->y.w.hi += ofs[g][1];
			op->x.w.hi += ofs[g][0] - 20;
		} else {
			op->spd_x = 0x40000;
			op->y.w.hi += ofs[g][3];
			op->x.w.hi += ofs[g][2] + 20;
		}
		if (game.level >= LEVEL_HARD)
			op->spd_x <<= 1;
		tama_id(op) = 6;
	} else {
		SetAnime(op, 27);
		op->col_box = &bomb_col;
		op->atk_box = &bomb_atk;
		op->dmg_box = &bomb_dmg;
		op->x.w.hi += op->flip ? 28 : -28;
		op->y.w.hi -= 24;
		op->spd_y = 0x30000;
		op->acc_x = 0;
		op->hp = 2;
		op->grav = 0x1800;
		op->atk = 3;
		op->spd_x = ((mega.x.w.hi - POS_X(op)) << 16) / 70;
	}
	op->act = 1;
	op->act2 = op->act3 = op->act4 = 0;
	if (OffScreenCheck2(op, 32, 32) == 1)
		op->act++;
}

static void tama_main(OBJECT *op)
{
	AnimeTick(op);
	DisplayCheck(op, 16, 16);

	if (op->var == 0) {
		if (op->anime.flag == ANM_FIRE) {
			op->disp = 0;
			op->act++;
			return;
		}
	} else if (op->var == 1) {
		if (op->act2 == 0) {
			op->disp = 0;
			if (--tama_id(op) == 0) {
				op->atk_box = &tama_atk;
				op->col_box = &tama_col;
				op->disp = 1;
				op->act2++;
			}
		} else {
			MoveObject(op);
			if (op->col_f || (CornerCheck(op), op->col_f)) {
				SpawnHitEffect(op);
				op->disp = 0;
				op->act++;
				return;
			}
		}
	} else if (op->var == 16) {
		if (++op->act3 == 8) {
			BombTrail(0, 0, op, 1, 0);
			op->act3 = 0;
		}
		if (op->col_f || (CornerCheck(op), op->col_f)
		 || ContactWeapon(op) < 0 || ContactMega(op)) {
			SpawnExplosion(op);
			op->act = 2;
			return;
		}
		AnimeTick(op);
		MoveObject2(op);
		if (OffScreenCheck2(op, 16, 16))
			op->act++;
		else
			DisplayCheck(op, 16, 16);
		return;
	}
	ContactMega(op);
	if (OffScreenCheck2(op, 32, 32) == 1)
		op->act++;
}

static void tama_delete(OBJECT *op)
{
	DeleteObject3(op);
}
