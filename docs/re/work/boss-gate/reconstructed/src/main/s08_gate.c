/*
 *	s08_gate.c
 *
 *	main program  stage object 08 : boss gate (shutter door before a boss)
 *	shared by all stages
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
	ACT_SHUT
};

enum {
	MODE_WAIT = 0,
	MODE_START,
	MODE_OPEN,
	MODE_PASS,
	MODE_CLOSE,
	MODE_FADE
};

#define GATE_BANK	0x80
#define GATE_PRI	6
#define PUSH_SLOW	0xa400
#define PUSH_MID	0xd400
#define PUSH_FAST	0x1c400
#define PUSH_RUN	0x3d800
#define GATE_LIMSPD	2
#define QUAKE_WAIT	10

#define F32(op, o)	(*(long *)((u_char *)(op) + (o)))
#define work7c(op)	F32(op, 0x7c)
#define old_spd(op)	F32(op, 0x80)
#define push(op)	F32(op, 0x84)
#define qtimer(op)	F32(op, 0x88)
#define fade_t(op)	(*(short *)((u_char *)(op) + 0xa0))
#define touch_f(op)	(*((u_char *)(op) + 0x72))
#define wall_kind(op)	(*((u_char *)(op) + 0x75))
#define mega_touch	(*((u_char *)&mega + 0x71))
#define mega_d0		(*((char *)&mega + 0xd0))
#define mega_e0		(*((char *)&mega + 0xe0))
#define mega_a4		(*((char *)&mega + 0xa4))
#define mega_5c		(*((char *)&mega + 0x5c))
#define mega_7a		(*((char *)&mega + 0x7a))
#define mega_8e		(*((char *)&mega + 0x8e))

extern ANIME	*gate_anime[];
extern void	SetSpriteBank(OBJECT *op, int type);
extern HITBOX	gate_col_box;
extern u_long	bg_frame;		/* 0x80097420 */
extern u_char	gate_flag_eab0;		/* 0x8008EAB0 */
extern u_short	fade_step;		/* 0x800CF854 */
extern u_short	fade_a;			/* 0x80097430 */
extern u_short	fade_b;			/* 0x80090D5C */
extern u_char	fade_c;			/* 0x80090D56 */
extern u_char	fade_d;			/* 0x8009742C */
extern u_char	fade_e;			/* 0x800CF850 */
extern long	fade_tbl[];		/* 0x80090C90 */
extern u_char	redraw_f;		/* 0x800C4560 */

static void	gate_init(OBJECT *op);
static void	gate_main(OBJECT *op);
static void	gate_shut(OBJECT *op);
static void	gate_wait(OBJECT *op, GAME *gp, OBJECT *mp);
static void	gate_start(OBJECT *op, GAME *gp, OBJECT *mp);
static void	gate_open(OBJECT *op, GAME *gp, OBJECT *mp);
static void	gate_pass(OBJECT *op, GAME *gp, OBJECT *mp);
static void	gate_close(OBJECT *op, GAME *gp, OBJECT *mp);
static void	gate_fade(OBJECT *op, GAME *gp, OBJECT *mp);
static int	gate_touch(OBJECT *op);
static void	gate_drop(OBJECT *op);

static void (*gate_act[])(OBJECT *) = {
	gate_init, gate_main, gate_shut
};

static void (*gate_mode[])(OBJECT *, GAME *, OBJECT *) = {
	gate_wait, gate_start, gate_open, gate_pass, gate_close, gate_fade
};

#define CAM	(&bg_layer[0])

static void stamp(void)
{
	if (game.boss_time == 0)
		game.boss_time = game.time;
}

/* 0x8005011C */
static void gate_init(OBJECT *op)
{
	op->flag = 0x49;
	op->pri = GATE_PRI;
	SetSpriteBank(op, GATE_BANK);	/* inlined in the binary: tpage, sprt_p, clut */
	op->anime_tbl = gate_anime;
	wall_kind(op) = 1;
	op->act = ACT_MAIN;
	op->flip = OBJ_FLIP_L;
	op->fall_f = 0;
	work7c(op) = 0;
	qtimer(op) = 0;
	op->col_box = &gate_col_box;	/* {0, 0, 16, 32} */
	op->act2 = MODE_WAIT;
	op->layer = mega.layer;
	op->var &= 0x3f;
	SetAnime(op, 0);
}

/* 0x80050E58 */
static void gate_main(OBJECT *op)
{
	(*gate_mode[op->act2])(op, &game, &mega);
}

/* 0x80050250 */
static void gate_wait(OBJECT *op, GAME *gp, OBJECT *mp)
{
	OBJECT	*np;

	SolidObjectCollide(op);
	gate_drop(op);

	switch (op->var) {
	case 7: case 8: case 12: case 10: case 14: case 15:
		if (gp->flag26 == 0) return;
		break;
	case 34:
		if (gp->flag28 == 0) return;
		break;
	case 44:
		if (gp->flag26 != 0) return;
		break;
	}
	if (!gate_touch(op))
		return;

	if (gp->lock_f == 0 && (op->var == 25 || op->var == 39)) {
		PlaySound(SE_BANK_5, SE_GATE_QUAKE, NULL);
		Rumble(4);
	}
	gp->lock_f = 1;
	MEGA_GATE(mp) = GATE_REQ;

	if (op->var == 25) {
		if (bg_layer[1].s42 >= -79) {
			if (!(bg_frame & 1))
				bg_layer[1].s42--;
			if (qtimer(op)) { qtimer(op)--; return; }
			Quake2(10, 2, 2);
			qtimer(op) = QUAKE_WAIT;
			return;
		}
		PlaySound(SE_BANK_5, SE_GATE_QUAKE_END, NULL);
		bg_layer[1].b03 = 0;
	} else if (op->var == 39) {
		if (push(op)++ < 112) {
			bg_layer[1].s42--;
			if (qtimer(op)) { qtimer(op)--; return; }
			Quake2(10, 2, 2);
			qtimer(op) = QUAKE_WAIT;
			return;
		}
		bg_layer[1].b03 = 0;
		PlaySound(SE_BANK_5, SE_GATE_QUAKE_END, NULL);
	}

	if (work7c(op) == 0)
		ClearEnemies();
	if (op->var == 7) {
		np = GetMainObject();
		np->flag = 0x41;
		np->type = 25;
		np->var = 4;
	}
	CAM->event_f &= 0x000f;
	op->act2 = MODE_START;
}

/* 0x80050540 */
static void gate_start(OBJECT *op, GAME *gp, OBJECT *mp)
{
	int	i;

	if (MEGA_GATE(mp) < 0) {
		MEGA_GATE(mp) = GATE_PASS;
		gp->stop_f = 1;
		gp->stop2_f = 1;
		mp->col_box = NULL;
		old_spd(op) = CAM->lim_spd;
		CAM->lim_spd = GATE_LIMSPD;
		if (op->var == 31) {
			GateFadeInit(0);
			fade_c = 1;
			fade_d = 1;
			fade_e = 1;
			fade_tbl[2] = 15;
			for (i = 4; i < 16; i++)
				fade_tbl[i - 1] = -1;
			redraw_f |= 1;
			fade_t(op) = 2;
			CAM->event_f &= 0x000f;
			op->act2 = MODE_FADE;
			return;
		}
		op->act2 = MODE_OPEN;
		PlaySound(SE_BANK_SYS, SE_GATE_A, op);
	}
	CAM->event_f &= 0x000f;
}

/* 0x80050670 */
static void gate_open(OBJECT *op, GAME *gp, OBJECT *mp)
{
	BGLAYER	*cp = CAM;
	short	x = POS_X(op);
	short	y = POS_Y(op);

	if (op->anime.flag < ANM_END) {
		AnimeTick(op);
		if (op->anime.flag == ANM_FIRE) {
			PlaySound(SE_BANK_SYS, SE_GATE_B, op);
			op->anime.flag = 0;
		}
		cp->event_f &= 0x000f;
		return;
	}

	gp->stop_f = 0;
	gp->stop2_f = 0;
	cp->dst_min_x = x;
	cp->dst_max_x = x;
	push(op) = PUSH_SLOW;

	switch (op->var) {
	case 1:  cp->dst_max_x = x + 80; break;
	case 2:  cp->dst_max_y = cp->dst_min_y = 224; cp->dst_max_x = x; break;
	case 3: case 40:
		cp->dst_min_x = cp->dst_max_x = x - 48;
		cp->dst_min_y = cp->dst_max_y = y - 64;
		push(op) = PUSH_MID; break;
	case 4: case 22: cp->dst_max_x = x + 32; stamp(); break;
	case 5:
		cp->dst_min_x = cp->dst_max_x = 6016;
		cp->dst_min_y = cp->dst_max_y = 304;
		push(op) = PUSH_MID; break;
	case 6:  cp->dst_max_x = 6384; cp->dst_min_y = 304; cp->dst_max_y = 320; stamp(); break;
	case 7:  cp->dst_min_x = 944; cp->dst_max_x = 976; cp->dst_max_y = 2624; cp->dst_min_y = y - 160; break;
	case 8:  cp->dst_min_x = x - 16; cp->dst_min_y = 1616; cp->dst_max_x = x - 16; break;
	case 9:  cp->dst_max_x = 2416; cp->dst_min_y = 2528; cp->dst_max_y = 2816; break;
	case 10: cp->dst_min_x = cp->dst_max_x = 2544; cp->dst_min_y = 944; push(op) = PUSH_RUN; break;
	case 11: cp->dst_max_x = 2416; cp->dst_min_y = 2080; cp->dst_max_y = 2304; break;
	case 12: cp->dst_min_x = cp->dst_max_x = 2544; cp->dst_min_y = 2160; push(op) = PUSH_RUN; break;
	case 13: cp->dst_min_x = x - 16; cp->dst_max_x = x + 16; break;
	case 14: cp->dst_max_x = 3504; cp->dst_max_y = 1184; break;
	case 15: cp->dst_min_x = x - 16; cp->dst_max_y = 944; cp->dst_max_x = x - 16; break;
	case 16: cp->dst_min_y = cp->dst_max_y = 944; cp->dst_max_x = x + 32; stamp(); break;
	case 17: case 21: case 32: case 42:
		cp->dst_min_x = x - 16; cp->dst_max_x = x - 16; break;
	case 19: cp->dst_min_x = cp->dst_max_x = 1264; cp->dst_min_y = 1104; cp->dst_max_y = 1184; break;
	case 20: cp->dst_max_x = 1680; cp->dst_min_y = 1136; cp->dst_max_y = 1184; stamp(); break;
	case 23: cp->dst_max_x = 9040; cp->dst_min_y = 640; cp->dst_max_y = 688; stamp(); break;
	case 24: cp->dst_max_y = 480; break;
	case 25: cp->dst_max_x = 7360; break;
	case 26: cp->dst_max_x = 8096; cp->dst_max_y = 160; break;
	case 27: cp->dst_min_y = 832; cp->dst_max_y = 928; cp->dst_max_x = cp->dst_min_x + 16; stamp(); break;
	case 28: cp->dst_min_x = cp->dst_max_x = 2880; break;
	case 29: cp->dst_max_x = 3216; cp->dst_max_y = cp->dst_min_y = 224; break;
	case 30: cp->dst_min_x = x - 48; cp->dst_max_x = y - 48; break;	/* sic */
	case 31: cp->dst_max_x = x + 32; cp->dst_min_y = y - 144; break;
	case 33: cp->dst_max_x = x + 32; break;
	case 35: case 38: case 45: case 47: case 49: case 51:
		cp->dst_max_x = x + 32; cp->dst_min_y = cp->dst_max_y = y - 160; break;
	case 36: stamp(); break;
	case 37: cp->dst_min_x = cp->dst_max_x = 2848; push(op) = PUSH_MID; break;
	case 39: cp->dst_max_y = cp->dst_min_y = 0; break;
	case 41: case 43:
		cp->dst_max_x = x + 32; cp->dst_min_y = cp->dst_max_y = y - 112; break;
	case 44: cp->dst_min_x = cp->dst_max_x = x - 112; push(op) = PUSH_FAST; break;
	case 46: cp->dst_min_x = cp->dst_max_x = x - 48; push(op) = PUSH_MID; break;
	case 50: cp->dst_min_y = cp->dst_max_y = y - 160; break;
	case 18: case 34: case 48:
	default:
		break;
	}
	op->act2 = MODE_PASS;
	cp->event_f &= 0x000f;
}

/* 0x80050EA0 */
static void gate_pass(OBJECT *op, GAME *gp, OBJECT *mp)
{
	BGLAYER	*cp = CAM;

	mp->x.l += push(op);
	if (cp->cx != cp->dst_min_x)
		return;

	MEGA_GATE(mp) = 0;
	PlayerSetTerrainBox(mp);
	op->act2 = MODE_CLOSE;
	SetAnime(op, 1);
	PlaySound(SE_BANK_SYS, SE_GATE_B, op);
	if (op->var == 18) {
		gate_flag_eab0 = 1;
		cp->dst_min_x = 3440;
	}
	cp->lim_spd = old_spd(op);
	cp->event_f &= 0x000f;
}

/* 0x80050F58 */
static void gate_close(OBJECT *op, GAME *gp, OBJECT *mp)
{
	if (op->anime.flag < ANM_END) {
		AnimeTick(op);
		if (op->anime.flag == ANM_FIRE) {
			PlaySound(SE_BANK_SYS, SE_GATE_A, op);
			op->anime.flag = 0;
		}
	} else {
		op->act = ACT_SHUT;
		op->act2 = MODE_WAIT;
		if (mega_d0 == 0 && MEGA_GATE(&mega) == 0 && mega_e0 == 0)
			game.lock_f = 0;
	}
	CAM->event_f &= 0x000f;
}

/* 0x80051018 */
static void gate_fade(OBJECT *op, GAME *gp, OBJECT *mp)
{
	if (--fade_t(op) != -1)
		return;
	fade_t(op) = 2;
	fade_a += 32;
	fade_step++;
	fade_b += 0x400;
	redraw_f |= 1;
	if (fade_step >= 31) {
		PlaySound(SE_BANK_SYS, SE_GATE_A, op);
		op->act2 = MODE_OPEN;
	}
}

/* 0x800510C4 */
static void gate_shut(OBJECT *op)
{
	if (op->var == 20)
		return;
	SolidObjectCollide(op);
	gate_drop(op);
}

/* 0x80050C88 */
static int gate_touch(OBJECT *op)
{
	HITBOX	*gb, *pb;
	short	d;

	if (mega_a4 || (mega_5c & 0x80) || mega.act2 == 3)
		return 0;
	if (mega.var == 0 ? mega_7a : mega_8e)
		return 0;
	if (POS_X(op) < POS_X(&mega))
		return 0;

	gb = op->col_box;
	pb = mega.col_box;
	d = (POS_X(op) + gb->ox) - (POS_X(&mega) + pb->ox);
	if (d < 0) d = -d;
	if (gb->w + pb->w < d)
		return 0;
	if ((short)(pb->oy + POS_Y(&mega) - pb->h) < (short)(gb->oy + POS_Y(op) - gb->h))
		return 0;
	return !((short)(gb->oy + POS_Y(op) + gb->h) < (short)(pb->oy + POS_Y(&mega) + pb->h));
}

/* 0x80051104 */
static void gate_drop(OBJECT *op)
{
	if (touch_f(op) & COL_U)
		mega_touch &= 0x0b;
	if (touch_f(op) & COL_D)
		mega_touch &= 0x07;
}

/* 0x80050DFC: stage object table entry 08 */
void Gate(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	(*gate_act[op->act])(op);
	DisplayCheck1(op);
}
