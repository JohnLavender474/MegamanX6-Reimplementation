/*
 *	s09_press_block.c  --  stage object 09 of overlay 06: the solid body of the Nightmare Press
 *	(and other solid blocks that follow BG layer 1)
 *
 *	Plausible reconstruction, not original code, not compiled.
 *	Binary: 0x800E9878 - 0x800E9F00; tables 0x800F35FC (states), 0x800F3608 (sub-steps).
 */
#include "game.h"

extern HITBOX	block_box[10];		/* 0x800F35A8: hurt boxes by var */
extern HITBOX	block_col[10];		/* 0x800F35D4: terrain boxes by var (half sizes) */
extern HITBOX	block_atk112;		/* 0x800F35D0: (-32, -9, 64, 18) */
extern HITBOX	block_col_init;		/* 0x800F4018: zero */
extern HITBOX	block_box_init;		/* 0x800F401C: zero */
extern ANIME	*block_anime[];		/* 0x800F35A0 */
extern u_char	dmg_tbl_block[];	/* 0x80075B9C */

#define BLK_Y0(o)	(*(short *)((u_char *)(o) + 0x86))
#define BLK_BOX7C(o)	((HITBOX *)((u_char *)(o) + 0x7C))
#define BLK_BOX80(o)	((HITBOX *)((u_char *)(o) + 0x80))
#define OBJ_B(o, n)	(*(u_char *)((u_char *)(o) + (n)))

static void block_init(OBJECT *op);
static void block_run(OBJECT *op);
static void block_follow(OBJECT *op);

static void (*block_state_tbl[])(OBJECT *) = {	/* 0x800F35FC */
	block_init, block_run, (void (*)(OBJECT *))DeleteObject,
};

static void (*block_step_tbl[])(OBJECT *) = {	/* 0x800F3608 */
	block_follow,
};

void PressBlock(OBJECT *op)			/* 0x800E9E98 */
{
	op->old_x = op->x;
	op->old_y = op->y;
	block_state_tbl[op->act](op);
}

static void block_init(OBJECT *op)		/* 0x800E9878 */
{
	int	v, hi, lo;

	if (mega.act2 == 0)
		return;
	op->act2 = 2;
	op->flag = 0x41;
	op->col_box = BLK_BOX7C(op);
	op->muteki = 0;
	op->act++;
	*BLK_BOX7C(op) = block_col_init;
	op->dmg_box = BLK_BOX80(op);
	*BLK_BOX7C(op) = block_box_init;	/* the binary writes the second copy to +0x7C too */

	v = op->var;
	if (v < 10) {
		op->col_box = &block_col[v];
		op->dmg_box = &block_box[v];
	} else if (v == 112) {
		BLK_BOX7C(op)->ox = 0;   BLK_BOX7C(op)->oy = 0;
		BLK_BOX7C(op)->w = 32;   BLK_BOX7C(op)->h = 8;
		BLK_BOX80(op)->ox = -32; BLK_BOX80(op)->oy = -8;
		BLK_BOX80(op)->w = 64;   BLK_BOX80(op)->h = 16;
		op->atk_box = &block_atk112;
	} else {
		hi = (v >> 4) & 15;
		lo = v & 15;
		BLK_BOX7C(op)->ox = 0;   BLK_BOX7C(op)->oy = 0;
		BLK_BOX7C(op)->w = hi * 8;
		BLK_BOX7C(op)->h = lo * 8;
		BLK_BOX80(op)->ox = -hi * 8;
		BLK_BOX80(op)->oy = -lo * 8;
		BLK_BOX80(op)->w = hi * 16;
		BLK_BOX80(op)->h = lo * 16;
	}
	op->dmg_tbl = dmg_tbl_block;
	op->atk = 127;
	op->anime_tbl = block_anime;
	op->flip = 0;
	op->hit_tag = 0;
	OBJ_B(op, 0x76) = 0;
	OBJ_B(op, 0xA2) = 1;
	OBJ_B(op, 0x88) = 0;
	OBJ_B(op, 0x75) = 1;
	BLK_Y0(op) = POS_Y(op);
	SetAnime(op, 0);
	op->act3 = 0;
}

static void block_solid(OBJECT *op)
{
	long	y;

	if (MEGA_UB(MEGA_SQZ_T) != 0)
		return;
	SolidObject(op);
	if (game.flag27 && (OBJ_B(op, 0x72) & COL_U) && (MEGA_UB(MEGA_COLF) & COL_D))
		MEGA_UB(MEGA_OBJF) = 0;
	if (OBJ_B(op, 0x76) != 0 && (OBJ_B(op, 0x72) & (COL_R | COL_L))) {
		y = mega.y.l;
		mega.y.l = y - mega.spd_y;
		SolidObject(op);
		if (OBJ_B(op, 0x76) == 0)
			mega.y.l -= mega.spd_y;
		else
			mega.y.l = y;
	}
}

static void block_run(OBJECT *op)		/* 0x800E9A30 */
{
	short	dx;

	if (game.stage == 4) {
		if (POS_Y(op) >= 672 && game.checkpoint == 0 && game.area == 0)
			return;
	} else if (game.stage < 19 && game.checkpoint != 2) {
		return;
	}
	SolidPre(op);
	if (MEGA_B(MEGA_FREEZE) != 2)
		block_step_tbl[op->act3](op);
	DisplayCheck(op, 160, 160);

	if (game.area == 0 && op->var == 0) {
		block_solid(op);
	} else {
		dx = POS_X(&mega) - POS_X(op);
		if (dx < 0)
			dx = -dx;
		if (dx < 160 && op->disp == 1)
			block_solid(op);
	}
	SolidSideFix1(op);
	SolidSideFix2(op);
	SolidOthers(op);
	op->disp = 0;
}

static void block_follow(OBJECT *op)		/* 0x800E9D5C */
{
	POS_Y(op) = BLK_Y0(op) + bg_layer[0].y - bg_layer[1].y;

	if (game.stage == 4) {
		if (game.area == 0) {
			if (op->var == 0) {
				if (game.checkpoint > 0)
					DeleteObject3(op);
			} else if (game.checkpoint >= 2) {
				DeleteObject3(op);
			}
			return;
		}
		if (op->var == 112 && (OBJ_B(op, 0x72) & COL_U))
			ContactMega(op);
		if (POS_X(&mega) >= 4480 || game.checkpoint > 0)
			DeleteObject3(op);
	} else if (game.stage >= 19 && game.checkpoint > 0) {
		DeleteObject3(op);
	}
}
