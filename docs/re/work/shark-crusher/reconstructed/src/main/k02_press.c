/*
 *	k02_press.c  --  Nightmare Press: kind-7 object type 2 (main program)
 *
 *	Plausible reconstruction, not original code, not compiled.
 *	Binary: 0x80046E14 - 0x800477E0; tables 0x800762C0 (states), 0x800762C8 (sub-steps).
 */
#include "game.h"
#include "sound.h"

#define PRESS_TRAVEL	0x500000		/* 80.0 px */
#define FX(n)		((long)(n) << 16)

static void press_init(SYSOBJ *op);
static void press_run(SYSOBJ *op);
static void press_intro(SYSOBJ *op);
static void press_wait(SYSOBJ *op);
static void press_fall(SYSOBJ *op);
static void press_rise(SYSOBJ *op);
static void press_bottom(SYSOBJ *op);
static void press_hold(SYSOBJ *op);

static void (*press_state_tbl[])(SYSOBJ *) = {	/* 0x800762C0 */
	press_init, press_run,
};

static void (*press_step_tbl[])(SYSOBJ *) = {	/* 0x800762C8 */
	press_intro, press_wait, press_fall, press_rise, press_bottom, press_hold,
};

static long lABS(long v) { return v < 0 ? -v : v; }

static long press_down_speed(void) { return game.level < LEVEL_HARD ? FX(1) : 0x14000; }
static long press_up_speed(void)   { return game.level < LEVEL_HARD ? -FX(1) : -0x14000; }

void PressUpdate(SYSOBJ *op)			/* 0x80046E14 */
{
	BGLAYER	*l0 = &bg_layer[0];
	BGLAYER	*l1 = &bg_layer[1];
	HITBOX	*box;
	short	line, top, d, dy;

	if (game.stage >= 19) {
		if (game.checkpoint != 0)
			return;
	} else if (game.stage != 4 && game.checkpoint != 2) {
		return;
	}
	if (MEGA_B(MEGA_FREEZE) == 2)
		return;

	l1->old_y = l1->y;
	press_state_tbl[op->act](op);

	if (l0->dest_y != l0->min_y)
		l0->event_f = 0;

	l1->y = l0->y + op->var - op->ofs.w.hi;

	if ((mega.act2 == 31 && mega.act3 < 4) || mega.act2 == 63) {	/* step +0x05, sub +0x06 */
		dy = l1->y - l1->old_y;
		if (dy > 0)
			POS_Y(&mega) -= dy;
	}

	box = mega.col_box;
	if (game.stage >= 19) {
		if (box == 0)
			goto end;
		if (POS_X(&mega) >= 2896 || game.checkpoint == 1) {
			DeleteSysObject(op);
			return;
		}
		line = l0->y - l1->y + 240;
	} else if (game.stage == 4) {
		if (game.area != 0) {
			if (box == 0)
				goto end;
			if (POS_X(&mega) >= 4480 || game.checkpoint == 1) {
				DeleteSysObject(op);
				return;
			}
			if (POS_X(&mega) >= 4257)
				return;
			line = l0->y - l1->y + 240;
		} else {
			if (box == 0)
				goto end;
			if (POS_Y(&mega) >= 1280 || game.checkpoint >= 2) {
				DeleteSysObject(op);
				return;
			}
			if (game.checkpoint == 0) {
				if (POS_X(&mega) >= 4993)
					return;
				line = l0->y - l1->y + 256;
			} else {
				if (POS_X(&mega) >= 5057)
					return;
				line = l0->y - l1->y + 736;
			}
		}
	} else {
		if (box == 0)
			goto end;
		if (game.checkpoint >= 3) {
			DeleteSysObject(op);
			return;
		}
		if (POS_X(&mega) < 1920) {
			if (POS_X(&mega) < 1857)
				return;
			line = l0->y - l1->y + 2001;
		} else if (POS_X(&mega) < 3840) {
			if ((u_short)(POS_X(&mega) - 2304) >= 1473)
				return;
			line = l0->y - l1->y + 2065;
		} else {
			if (POS_X(&mega) >= 6080 || (u_short)(POS_X(&mega) - 4240) >= 1777)
				goto end;
			line = l0->y - l1->y + 2097;
		}
	}

	top = POS_Y(&mega) + box->oy - box->h;
	if (line >= top) {
		d = line - top;
		MEGA_UB(MEGA_SQZ_DMG) = 127;
		POS_Y(&mega) += d;
		MEGA_UB(MEGA_OBJF) |= COL_U;
	}
end:
	if (l0->dest_y != l0->min_y)
		l0->event_f = 0;
}

static void press_init(SYSOBJ *op)		/* 0x800472D0 */
{
	op->ofs.l = 0;
	op->spd = FX(2);
	if (game.stage >= 19) {
		op->count = 16;
		op->timer = 0;
		op->act2 = 0;
	} else if (game.stage == 4) {
		op->count = 8;
		op->timer = 0;
		op->act2 = 0;
	} else {
		bg_layer[1].mode = 9;
		bg_layer[1].redraw = 1;
		op->spd = game.level < LEVEL_HARD ? -FX(1) : -0x14000;
		op->timer = game.checkpoint == 0 ? 120 : 50;
		op->act2 = 1;
		op->count = 0;
	}
	op->act++;
}

static void press_run(SYSOBJ *op)		/* 0x80047394 */
{
	game.flag27 = 0;
	if (MEGA_B(MEGA_FREEZE) != 2)
		press_step_tbl[op->act2](op);
}

static void press_intro(SYSOBJ *op)		/* 0x800473EC */
{
	if (mega.act2 == 0)		/* player step (+0x05) 0: the beam-in */
		return;
	op->ofs.l += op->spd;
	if (--op->count == 0) {
		op->spd = press_down_speed();
		op->timer = 180;
		op->act2++;
	}
}

static void press_wait(SYSOBJ *op)		/* 0x80047460 */
{
	if (game.flag26) {
		Quake(60, 2, 2);
		op->act2 = 5;
		op->count = PRESS_TRAVEL / lABS(op->spd);
		op->spd = -FX(1);
		op->timer = 50;
		op->save = op->ofs.l;
		return;
	}
	if (--op->timer == 0) {
		op->count = PRESS_TRAVEL / lABS(op->spd);
		if (op->spd > 0) {
			op->act2 = 2;
		} else {
			op->act2 = 3;
			PlaySoundSys(SE_BANK_ENEMY, SE_PRESS_RISE, op);
		}
	}
}

static void press_fall(SYSOBJ *op)		/* 0x8004755C */
{
	if (game.flag26) {
		Quake(60, 2, 2);
		op->timer = 50;
		op->save = op->ofs.l;
		op->count = PRESS_TRAVEL / lABS(op->spd) - op->count;
		PlaySound(SE_BANK_ENEMY, SE_PRESS_SLAM, 0);
		Rumble(5);
		op->spd = -FX(1);
		op->act2 = 5;
		return;
	}
	op->ofs.l += op->spd;
	if (--op->count == 0) {
		Quake(60, 2, 2);
		op->act2 = 4;
		PlaySound(SE_BANK_ENEMY, SE_PRESS_SLAM, 0);
		Rumble(5);
		op->spd = -FX(1);
		op->timer = 40;
		op->save = op->ofs.l;
	}
}

static void press_rise(SYSOBJ *op)		/* 0x80047674 */
{
	game.flag27 = 1;
	op->ofs.l += op->spd;
	if (--op->count == 0) {
		op->spd = game.level < LEVEL_HARD ? FX(1) : 0x14000;
		op->timer = 50;
		op->act2 = 1;
	}
}

static void press_bottom(SYSOBJ *op)		/* 0x800476E0 */
{
	if (--op->timer == 0) {
		op->spd = press_up_speed();
		op->timer = game.checkpoint == 0 ? 120 : 50;
		op->act2 = 1;
		op->ofs.l = op->save;
	}
}

static void press_hold(SYSOBJ *op)		/* 0x80047760 */
{
	if (--op->timer == 0) {
		op->spd = game.level < LEVEL_HARD ? -FX(1) : -0x14000;
		op->act2 = 3;
		PlaySoundSys(SE_BANK_ENEMY, SE_PRESS_RISE, op);
		op->ofs.l = op->save;
	}
}
