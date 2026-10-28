/*
 *	s10_nightmare_pressure.c  --  stage object 10: the solid parts of Nightmare Pressure
 *	(side walls var 0/1, press plates var 16/17, head block var 32)
 *	stage 4 area 1, overlay 07 (0x800E9860)
 *
 *	Plausible reconstruction, not original code, not compiled.
 */
#include "object.h"
#include "game.h"

#define CAM0		(bg_cam[0])
#define CAM1		(bg_cam[1])

#define BODY(o)		(*(OBJECT **)&(o)->timer)		/* +0x7C */
#define OFS_X(o)	(*(short *)&(o)->work[0x82 - 0x7e])
#define OFS_Y(o)	(*(short *)&(o)->work[0x86 - 0x7e])
#define D_TIMER(o)	(*(short *)&(o)->work[0x88 - 0x7e])
#define D_GAP(o)	(*(short *)&(o)->work[0x8a - 0x7e])
#define B_HEAD(b)	(*(char *)&(b)->work[0x8f - 0x7e])
#define B_WALL(b)	(*(char *)&(b)->work[0x90 - 0x7e])
#define B_RISE(b)	(*(char *)&(b)->work[0x91 - 0x7e])
#define TOUCH(o)	(*((u_char *)(o) + 0x72))

extern HITBOX	np_wall_atk;		/* 0x800F88A4 (-32, 16, 16, 16) */
extern HITBOX	np_wall_dmg;		/* 0x800F88A8 (-16, -64, 36, 112) */
extern HITBOX	np_wall_col;		/* 0x800F88AC centre (2, -8), half (18, 56) */
extern HITBOX	np_press_dmg;		/* 0x800F88B4 (-80, -80, 160, 160) */
extern HITBOX	np_press_col;		/* 0x800F88B8 centre (0, 0), half (80, 80) */
extern HITBOX	np_head_col;		/* 0x800F88BC centre (0, 0), half (28, 56) */
extern u_char	np_junk_wall[];		/* 0x800F8688 {14, 14, 15, 15} */

extern int	NP_MoveToward(short *p, short to, short step);	/* 0x800ECD7C */

/* 0x800EB114 */
static void S10_Setup(OBJECT *op)
{
	*((u_char *)op + 0x75) = 1;
	*((u_char *)op + 0xa2) = 1;
	*((u_char *)op + 0x76) = 0;
	op->fall_f = 0;

	switch (op->var & 0xf0) {
	case 0:
		op->pri = 35;
		op->atk = 8;
		op->dmg_tbl = dmg_tbl_capsule;
		op->atk_box = &np_wall_atk;
		op->hp = 0;
		op->muteki = 0;
		op->hit_tag = 0;
		op->dmg_box = &np_wall_dmg;
		op->col_box = &np_wall_col;
		SetAnime(op, 16);
		op->disp = 0;
		op->act2 = 0;
		if (op->var == 0) {
			OFS_X(op) = 16;
			op->flip = OBJ_FLIP_R;
			OFS_Y(op) = -64;
		} else if (op->var == 1) {
			OFS_X(op) = 304;
			OFS_Y(op) = -64;
			op->flip = OBJ_FLIP_L;
		}
		break;
	case 16:
		op->atk = 127;
		op->dmg_tbl = dmg_tbl_capsule;
		op->hp = 0;
		op->muteki = 0;
		op->hit_tag = 0;
		op->atk_box = 0;
		op->dmg_box = &np_press_dmg;
		op->col_box = &np_press_col;
		SetAnime(op, 16);
		op->disp = 0;
		op->act2 = 1;
		if (op->var == 16) {
			OFS_X(op) = 80;
			OFS_Y(op) = -80;
			op->flip = OBJ_FLIP_L;
		} else if (op->var == 17) {
			OFS_X(op) = 240;
			OFS_Y(op) = -80;
			op->flip = OBJ_FLIP_R;
		}
		break;
	case 32:
		op->atk = 8;
		op->dmg_tbl = dmg_tbl_capsule;
		op->hp = 0;
		op->muteki = 0;
		op->hit_tag = 0;
		op->atk_box = 0;
		op->dmg_box = 0;
		op->col_box = &np_head_col;
		SetAnime(op, 16);
		op->disp = 0;
		op->act2 = 2;
		break;
	}
	op->act3 = 0;
	op->act++;
}

/* 0x800EB2E8 */
static void (*s10_steps[])(OBJECT *);

static void S10_Main(OBJECT *op)
{
	OBJECT *bp = BODY(op);

	op->old_x = op->x;
	op->old_y = op->y;
	if (bp->act != 1) {
		op->disp = 0;
		op->act2 = 0;
		op->act++;
		return;
	}
	if ((op->var & 0xf0) == 32) {
		POS_X(op) = POS_X(bp);
		POS_Y(op) = POS_Y(bp);
		if (B_HEAD(bp) == 0 || B_HEAD(bp) == 1)
			return;
	} else {
		POS_X(op) = OFS_X(op) + CAM0.x + CAM1.x;
		POS_Y(op) = OFS_Y(op) + CAM0.y + 952 - CAM1.y;
		if (bp->act2 == 9)
			POS_Y(op) += 16;
	}
	if (bp->act2 == 2)
		return;
	s10_steps[op->act2](op);
	ContactMega(op);
	if ((op->var & 0xf0) == 16) {
		SolidObject(op);
	} else if (MEGA_SQUEEZE == 0) {
		SolidObject(op);
		if ((op->var & 0xf0) == 32 && B_RISE(bp) == 1 &&
		    (TOUCH(op) & COL_U) && (MEGA_COLL & COL_D))
			MEGA_TOUCH = 0;
	}
	SolidWeaponHit(op);
	SolidPushShots(op);
}

/* 0x800ED264 */
static void S10_Death(OBJECT *op)
{
	switch (op->act2) {
	case 0:
		if ((op->var & 0xf0) == 0) {
			SpawnJunk(4, np_junk_wall, op);
			D_TIMER(op) = 60;
			D_GAP(op) = 5;
			op->act2++;
		} else {
			op->act2 = 2;
		}
		break;
	case 1:
		if (--D_TIMER(op) == 0) {
			op->act2++;
			break;
		}
		if (--D_GAP(op) == 0) {
			SpawnExplosionRand(op, 1, 16, 64);
			D_GAP(op) = 5;
		}
		break;
	case 2:
		DeleteObject3(op);
		break;
	}
}

/* 0x800ED364 / 0x800ED3A0  wall */
static void S10_Wall0(OBJECT *op)
{
	OBJECT *bp = BODY(op);

	op->disp = 1;
	switch (B_WALL(bp)) {
	case 0: NP_MoveToward(&OFS_Y(op), 64, 2);  break;
	case 1: NP_MoveToward(&OFS_Y(op), -64, 2); break;
	case 2: NP_MoveToward(&OFS_Y(op), -64, 4); break;
	}
}

static void S10_Nop(OBJECT *op) { }
static void S10_Next(OBJECT *op) { op->act3++; }

static void (*s10_wall_subs[])(OBJECT *) = { S10_Wall0, S10_Nop };
static void (*s10_press_subs[])(OBJECT *) = { S10_Next, S10_Nop };

static void S10_Wall(OBJECT *op)  { s10_wall_subs[op->act3](op); }
static void S10_Press(OBJECT *op) { s10_press_subs[op->act3](op); }

static void (*s10_steps[])(OBJECT *) = { S10_Wall, S10_Press, S10_Nop };
static void (*s10_states[])(OBJECT *) = { S10_Setup, S10_Main, S10_Death };

/* 0x800ED494 */
void NightmarePressureSolid(OBJECT *op)
{
	s10_states[op->act](op);
}
