/*
 *	m16_nightmare_pressure.c  --  main object 16: Nightmare Pressure (body and parts)
 *	                              visual object 13: its bombs, shots, missiles and beams
 *	stage 4 area 1, overlay 07 (0x800E9860)
 *
 *	Plausible reconstruction, not original code, not compiled.
 */
#include "object.h"
#include "game.h"
#include "sound.h"

#define CAM0		(bg_cam[0])
#define CAM1		(bg_cam[1])
#define CAM2		(bg_cam[2])

#define OFS_X32(o)	(*(long *)&(o)->work[0x80 - 0x7e])
#define OFS_X(o)	(*(short *)&(o)->work[0x82 - 0x7e])
#define INV(o)		(*(short *)&(o)->work[0x84 - 0x7e])
#define OFS_Y(o)	(*(short *)&(o)->work[0x86 - 0x7e])
#define SYSOBJ(o)	(*(OBJECT **)&(o)->work[0x88 - 0x7e])
#define MODE(o)		((o)->work[0x8c - 0x7e])
#define PAT(o)		(*(char *)&(o)->work[0x8d - 0x7e])
#define IDX(o)		((o)->work[0x8e - 0x7e])
#define HEAD(o)		(*(char *)&(o)->work[0x8f - 0x7e])
#define WALL(o)		((o)->work[0x90 - 0x7e])
#define RISE(o)		((o)->work[0x91 - 0x7e])
#define RNDCNT(o)	(*(char *)&(o)->work[0x92 - 0x7e])
#define COUNT(o)	(*(short *)&(o)->work[0])	/* +0x7E */
#define PARENT(o)	(*(OBJECT **)&(o)->work[0x80 - 0x7e])
#define SHOT_PARENT(o)	(*(OBJECT **)&(o)->timer)	/* +0x7C on a visual object */

#define MX(ox)		(CAM0.x + CAM1.x + (ox))
#define MY(oy)		(CAM0.y + 952 - CAM1.y + (oy))

extern ANIME	*np_anime_tbl[];		/* 0x800F8638, 17 animations */
extern HITBOX	np_part_box;			/* 0x800F852C (-8, -8, 16, 16) */
extern HITBOX	np_drill_box;			/* 0x800F8530 (-32, -10, 16, 20) */
extern HITBOX	np_bomb_col, np_bomb_atk;	/* 0x800F8840, 0x800F8844 */
extern HITBOX	np_blast_box;			/* 0x800F8848 (-16, -16, 32, 32) */
extern HITBOX	np_shot_col, np_shot_atk;	/* 0x800F884C, 0x800F8850 */
extern HITBOX	np_missile_atk, np_missile_dmg;	/* 0x800F8854, 0x800F8858 */
extern u_char	np_junk_pod[], np_junk_turret[];	/* 0x800F8680, 0x800F867C */
extern u_char	np_pat_hi[9][8], np_pat_lo[9][8];	/* 0x800F87B0, 0x800F87F8 */

static void	NP_MakePart(OBJECT *op, int var);
static void	NP_MakeStagePart(OBJECT *op, int var);
static void	NP_Think(OBJECT *op);
static void	NP_MakeShot(OBJECT *op, int var);
static int	NP_MoveToward(short *p, short to, short step);

/* 0x800E9E08 */
static void NP_Setup(OBJECT *op)
{
	if (op->var == 0) {
		op->atk = 8;
		op->hp = 0;
		op->muteki = 0;
		op->anime_tbl = np_anime_tbl;
		op->layer = mega.layer;
		op->pri = 34;
		op->dmg_tbl = dmg_tbl_normal;
		op->flip = OBJ_FLIP_L;
		op->fall_f = 0;
		op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
		OFS_X(op) = 160;
		OFS_Y(op) = -64;
		op->disp = 0;
		HEAD(op) = 0; WALL(op) = 0; RISE(op) = 0; RNDCNT(op) = 0;
		op->col_box = op->atk_box = op->dmg_box = 0;
		SetAnime(op, 0);
		MODE(op) = 0; PAT(op) = 0; IDX(op) = 0;
		NP_MakePart(op, 16);
		NP_MakePart(op, 17);
		NP_MakePart(op, 18);
		NP_MakePart(op, 32);
		NP_MakePart(op, 33);
		NP_MakePart(op, 34);
		NP_MakeStagePart(op, 0);
		NP_MakeStagePart(op, 1);
		NP_MakeStagePart(op, 16);
		NP_MakeStagePart(op, 17);
		NP_MakeStagePart(op, 32);
		op->act = 1;
		op->act2 = 2;
	} else {
		op->hp = 16;
		op->anime_tbl = np_anime_tbl;
		op->atk = 0;
		op->layer = mega.layer;
		op->pri = 33;
		INV(op) = 0;
		op->fall_f = 0;
		if ((op->var & 0xf0) == 16) {
			SetAnime(op, 5);
			op->dmg_tbl = dmg_tbl_normal;
			op->flip = (op->var == 17) ? OBJ_FLIP_R : OBJ_FLIP_L;
			op->act2 = 3;
		} else {
			SetAnime(op, 1);
			op->dmg_tbl = dmg_tbl_guard;
			op->flip = (op->var == 33) ? OBJ_FLIP_L : OBJ_FLIP_R;
			op->act2 = 2;
		}
		op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
		op->disp = 0;
		op->col_box = 0;
		op->atk_box = 0;
		op->dmg_box = &np_part_box;
		op->act = 3;
	}
	op->act3 = 0;
}

/* 0x800EA0B4 */
static void (*np_body_steps[])(OBJECT *);

static void NP_BodyMain(OBJECT *op)
{
	if (op->act2 != 2 && (char)op->hp <= 0) {
		op->hp = 0;
		op->act = 2;
		op->disp = 0;
		op->act2 = 0;
		op->act3 = 0;
		return;
	}
	if (CAM0.x >= 5440 && op->act2 != 9) {
		op->act2 = 9;
		op->act3 = 0;
	}
	op->old_x = op->x;
	op->old_y = op->y;
	AnimeTick(op);
	np_body_steps[op->act2](op);
	POS_X(op) = OFS_X(op) + CAM0.x + CAM1.x;
	POS_Y(op) = OFS_Y(op) + CAM0.y + 952 - CAM1.y;
	if (HEAD(op) >= 2)
		ContactMega(op);
}

/* 0x800EA1CC  death sub 1 */
static void NP_Death1(OBJECT *op)
{
	OBJECT *sp;
	char gap;

	if (--op->timer == 0) {
		if ((sp = GetSysObject()) != 0) {
			sp->flag = 1;
			sp->type = 26;
			sp->var = 1;
			POS_X(sp) = CAM0.x + CAM1.x + 160;
			POS_Y(sp) = CAM0.y - CAM1.y + 912;
			SYSOBJ(op) = sp;
		}
		op->act2++;
	}
	if (--COUNT(op) == -1) {
		op->clut ^= CLUT_FLASH;
		op->muteki -= 5;
		if (op->muteki >= 26)
			op->muteki = 0;
		gap = op->muteki;
		COUNT(op) = (gap < 6) ? 5 : gap;
	}
}

/* 0x800EA2E0  death sub 2: the move to the boss gate */
static void NP_Death2(OBJECT *op)
{
	OBJECT *sp = SYSOBJ(op);
	int i;

	if (sp->flag == 0)
		return;
	if (sp->act4 == 0) {
		if (--COUNT(op) == -1) {
			COUNT(op) = 5;
			op->clut ^= CLUT_FLASH;
		}
		return;
	}
	op->act2++;
	CAM1.x = CAM0.x = 5456;
	CAM0.min_x = CAM0.dest_min_x = CAM0.max_x = CAM0.dest_max_x = 5456;
	CAM2.x = 2728;
	CAM0.y = CAM1.y = CAM2.y = 224;
	CAM0.min_y = CAM0.dest_min_y = CAM0.max_y = CAM0.dest_max_y = 224;
	for (i = 0; i < 3; i++)
		bg_cam[i].redraw = 1;
	SpawnInRect(5408, 5824, 224, 512, 0);
	POS_X(&mega) = 5604;
	POS_Y(&mega) = 395;
	mega.old_x.w.hi = 5604;
	mega.old_y.w.hi = 395;
}

/* 0x800EA3F4  TURRET SHOTS sub 2: pick the turret */
static void NP_PickTurret(OBJECT *op)
{
	int mx = POS_X(&mega), my = POS_Y(&mega);
	int bx = POS_X(op), by = POS_Y(op);
	int d1, d2, d3, sel;

	if (mega.var == 0) {
		d1 = (mx - 16 - bx) * (mx - 16 - bx) + (my + 32 - by) * (my + 32 - by);
		d2 = (mx + 16 - bx) * (mx + 16 - bx) + (my - by) * (my - by);
		d3 = (mx - 16 - bx) * (mx - 16 - bx) + (my - 32 - by) * (my - 32 - by);
		if (d2 < d1)
			sel = (d3 < d2) ? 3 : 2;
		else
			sel = (d3 < d1) ? 3 : 1;
	} else {
		if (bx >= mx)
			sel = 2;
		else
			sel = (by < my) ? 3 : 1;
	}
	op->act4 = sel;
	op->act3++;
}

/* 0x800EA4F4 */
static void (*np_part_steps[])(OBJECT *);

static void NP_PartMain(OBJECT *op)
{
	OBJECT *bp = PARENT(op);
	char old_hp;
	int r;

	if (bp->act2 == 2)
		return;
	switch (op->var) {
	case 16: POS_X(op) = POS_X(bp) - 16; POS_Y(op) = POS_Y(bp) - 32; break;
	case 17: POS_X(op) = POS_X(bp) + 16; POS_Y(op) = POS_Y(bp);      break;
	case 18: POS_X(op) = POS_X(bp) - 16; POS_Y(op) = POS_Y(bp) + 32; break;
	case 32: POS_X(op) = POS_X(bp) + 16; POS_Y(op) = POS_Y(bp) - 32; break;
	case 33: POS_X(op) = POS_X(bp) - 16; POS_Y(op) = POS_Y(bp);      break;
	case 34: POS_X(op) = POS_X(bp) + 16; POS_Y(op) = POS_Y(bp) + 32; break;
	}
	op->old_x = op->x;
	op->old_y = op->y;
	if (bp->act != 1) {
		op->spd_x = 0;
		op->disp = 0;
		SpawnExplosion(op);
		SpawnJunk(4, ((op->var & 0xf0) == 16) ? np_junk_pod : np_junk_turret, op);
		op->act = 5;
		return;
	}
	if (HEAD(bp) == 0) {
		op->disp = 0;
		return;
	}
	if (HEAD(bp) == 1) {
		op->disp = bp->disp;
		return;
	}
	op->disp = bp->disp;
	AnimeTick(op);
	np_part_steps[op->act2](op);

	old_hp = op->hp;
	if ((op->var & 0xf0) == 16 && INV(op) == 0) {
		if (POS_X(&mega) < POS_X(bp))
			op->dmg_tbl = (op->var == 17) ? dmg_tbl_guard : dmg_tbl_normal;
		else
			op->dmg_tbl = (op->var == 17) ? dmg_tbl_normal : dmg_tbl_guard;
	}
	r = ContactWeapon(op);
	if (r != 0) {
		bp->hp -= (char)(old_hp - op->hp);
		if (r < 0) {
			SpawnExplosion(op);
			op->spd_x = 0;
			SpawnJunk(4, np_junk_pod, op);
			op->act = 4;
			op->act2 = 0;
			op->act3 = 0;
			return;
		}
	}
	if ((op->var & 0xf0) == 16) {
		if (r > 0 && r != 127) {
			op->dmg_tbl = dmg_tbl_guard;
			INV(op) = (r == 25 || r == 26) ? 2 : 30;
		}
		if (INV(op) != 0) {
			if (INV(op)-- & 2)
				op->clut |= CLUT_FLASH;
			else
				op->clut &= ~CLUT_FLASH;
			if (INV(op) == 0) {
				op->dmg_tbl = dmg_tbl_normal;
				op->clut &= ~CLUT_FLASH;
			}
		}
	}
	ContactMega(op);
}

/* 0x800EA860  THINK */
static void NP_Think(OBJECT *op)
{
	u_char (*tbl)[8];
	u_char e;

	op->act3 = 0;
	if (MODE(op) == 0 && (char)op->hp <= (char)game.boss_hp / 2) {
		PAT(op) = GetRNG() & 7;
		IDX(op) = 0;
		MODE(op) = 1;
		op->act2 = np_pat_lo[PAT(op)][0];
		IDX(op) = 1;
		return;
	}
	if (MODE(op) == 0)
		tbl = np_pat_hi;
	else if (MODE(op) == 1)
		tbl = np_pat_lo;
	else
		return;
	e = tbl[PAT(op)][IDX(op)];
	if (e == 0xff) {
		if (RNDCNT(op)++ < 3) {
			PAT(op) = GetRNG() & 7;
		} else {
			PAT(op) = 8;
			RNDCNT(op) = 0;
		}
		IDX(op) = 0;
		e = tbl[PAT(op)][0];
	}
	op->act2 = e;
	IDX(op)++;
}

/* 0x800EAA2C */
static void NP_MakeShot(OBJECT *op, int var)
{
	OBJECT *sp;

	if ((sp = GetVisualObject()) == 0)
		return;
	sp->flag = 0x41;
	sp->type = 13;
	sp->var = var;
	sp->flip = op->flip;
	sp->x.l = POS_X(op) << 16;
	sp->y.l = POS_Y(op) << 16;
	sp->clut = op->clut & ~CLUT_FLASH;
	sp->anime_tbl = op->anime_tbl;
	sp->sprt_p = op->sprt_p;
	sp->tpage = op->tpage;
	sp->layer = op->layer;
	SHOT_PARENT(sp) = op;
	if ((char)var == 0) {
		POS_X(sp) = CAM0.x + CAM1.x + 160;
		POS_Y(sp) = CAM0.y - CAM1.y + 936;
		POS_X(sp) += GetRNG() & 0x7f;
	} else if ((char)var == 48) {
		*(long *)&sp->work[0x84 - 0x7e] = op->act3;
	}
}

/* 0x800EAB50 .. 0x800EB110: see v13 below */

/* 0x800EB4C8 */
static void (*np_states[])(OBJECT *);

void NightmarePressure(OBJECT *op)
{
	np_states[op->act](op);
}

/* 0x800EB540 */
static void (*np_death_subs[])(OBJECT *);

static void NP_BodyDeath(OBJECT *op)
{
	np_death_subs[op->act2](op);
	POS_X(op) = OFS_X(op) + CAM0.x + CAM1.x;
	POS_Y(op) = OFS_Y(op) + CAM0.y + 952 - CAM1.y;
}

/* 0x800EB5C0  death sub 0 */
static void NP_Death0(OBJECT *op)
{
	game.checkpoint = 2;
	op->timer = 127;
	COUNT(op) = 25;
	op->muteki = 25;
	op->clut &= ~CLUT_FLASH;
	op->act2++;
	STAGE_FLAG26 = 1;
	PlayerEvent(20, mega.flip);
}

/* 0x800EB62C  death sub 3 */
static void NP_Death3(OBJECT *op)
{
	OBJECT *sp = SYSOBJ(op);

	if (sp->flag != 0) {
		if (sp->act4 == 0 && --COUNT(op) == -1) {
			COUNT(op) = 5;
			op->clut ^= CLUT_FLASH;
		}
		return;
	}
	PlayerEventEnd();
	DeleteObject3(op);
}

/* 0x800EB6B8  part killed */
static void NP_PartDead(OBJECT *op)
{
	OBJECT *bp = PARENT(op);

	if (op->act2 == 0) {
		op->disp = 0;
		op->col_box = 0;
		op->atk_box = 0;
		op->dmg_box = 0;
		op->act2++;
	}
	if (bp->act != 1) {
		op->act = 5;
		return;
	}
	if ((bp->act4 == 5 && op->var == 16) ||
	    (bp->act4 == 6 && op->var == 17) ||
	    (bp->act4 == 7 && op->var == 18))
		bp->act4 = 0;
}

/* 0x800EB770 */
static void NP_Delete(OBJECT *op)
{
	DeleteObject3(op);
}

/* 0x800EB790 .. 0x800EBA84  INTRO */
static void NP_Intro0(OBJECT *op)
{
	op->timer = 300;
	PlayerEvent(20, OBJ_FLIP_R);
	op->act3++;
}

static void NP_Intro1(OBJECT *op)
{
	if (mega.act2 != 20)
		return;
	CAM1.redraw = 1;
	game.boss_bar = 1;
	game.boss_f = 0;
	game.boss = op;
	game.boss_hp = 48;
	BossLevel();
	op->act3++;
}

static void NP_Intro2(OBJECT *op)
{
	CAM1.y -= 8;
	if (CAM1.y < 825) {
		PlaySound(SE_BANK_ENEMY, SE_NP_SLAM, op);
		Rumble(5);
		Quake(6, 10, 2);
		CAM1.y = 824;
		op->timer = 180;
		op->act3++;
	}
}

static void NP_Intro3(OBJECT *op)
{
	if ((op->timer & 7) == 0)
		Quake(6, (char)(op->timer / 30 + 1), 2);
	if (--op->timer == 0) {
		PlaySound(SE_BANK_ENEMY, SE_NP_SHAKE_END, op);
		op->act3++;
	}
}

static void NP_Intro4(OBJECT *op)
{
	CAM1.y += 8;
	if (CAM1.y >= 872) {
		CAM1.y = 872;
		COUNT(op) = 3;
		op->act3++;
	}
}

static void NP_Intro5(OBJECT *op)
{
	if (--COUNT(op) == 0) {
		PlaySound(SE_BANK_SYS, SE_HP_FILL, 0);
		COUNT(op) = 3;
	}
	if (++op->hp == 48) {
		PlayerEventEnd();
		NP_Think(op);
	}
}

/* 0x800EBA84  WAIT */
static void NP_Wait0(OBJECT *op) { op->timer = 60; op->act3++; }
static void NP_Wait1(OBJECT *op) { if (--op->timer == 0) NP_Think(op); }

/* 0x800EBB14  PRESS */
static void (*np_press_subs[])(OBJECT *);

static void NP_Press(OBJECT *op)
{
	if (MEGA_SPECIAL == 2 && op->act3 >= 2)
		return;
	np_press_subs[op->act3](op);
}

static void NP_Press0(OBJECT *op)
{
	PlaySound(SE_BANK_ENEMY, SE_NP_MOVE, op);
	WALL(op) = 1;
	op->timer = 20;
	op->act3++;
}

static void NP_Press1(OBJECT *op) { if (--op->timer == 0) op->act3++; }

static void NP_Press2(OBJECT *op)
{
	*(long *)&CAM1.y_lo -= 0xcccc;
	if (CAM1.y < 801) {
		PlaySound(SE_BANK_ENEMY, SE_NP_SLAM, op);
		Rumble(5);
		CAM1.y = 800;
		op->timer = 60;
		op->act3++;
	}
}

static void NP_Press3(OBJECT *op)
{
	if ((op->timer & 3) == 0)
		Quake(2, (GetRNG() & 3) + 1, 2);
	if (--op->timer == 0) {
		PlaySound(SE_BANK_ENEMY, SE_NP_SHAKE_END, op);
		op->timer = 30;
		op->act3++;
	}
}

static void NP_Press4(OBJECT *op)
{
	*(long *)&CAM1.y_lo += 0x18000;
	if (CAM1.y >= 872) {
		CAM1.y = 872;
		PlaySound(SE_BANK_ENEMY, SE_NP_MOVE, op);
		WALL(op) = 0;
		op->timer = 20;
		op->act3++;
	}
}

static void NP_Press5(OBJECT *op) { if (--op->timer == 0) NP_Think(op); }

/* 0x800EBDC4  BOMBS */
static void NP_Bomb0(OBJECT *op) { COUNT(op) = 0; op->act3++; }
static void NP_Bomb1(OBJECT *op) { op->timer = GetRNG() % 40 + 50; op->act3++; }

static void NP_Bomb2(OBJECT *op)
{
	if (--op->timer != 0)
		return;
	NP_MakeShot(op, 0);
	if (++COUNT(op) < 4)
		op->act3--;
	else
		op->act3++;
}

static void NP_Bomb3(OBJECT *op) { NP_Think(op); }

/* 0x800EBF1C  TURRET SHOTS */
static void NP_HeadOut(OBJECT *op, short x)
{
	OFS_X(op) = x;
	OFS_Y(op) = -64;
	PlaySound(SE_BANK_ENEMY, SE_NP_MOVE, op);
	HEAD(op) = 2;
	op->disp = 1;
	op->act3++;
}

static void NP_Gun0(OBJECT *op)  { NP_HeadOut(op, 160); COUNT(op) = 0; }
static void NP_Gun1(OBJECT *op)  { if (NP_MoveToward(&OFS_Y(op), 56, 2)) op->act3++; }

static void NP_Gun3(OBJECT *op)
{
	int limit;

	if (op->act4 != 0)
		return;
	if (mega.var == 0)
		limit = HARD_MODE() ? 3 : 2;
	else
		limit = 4;
	if (++COUNT(op) < limit) {
		op->act3--;
		return;
	}
	RISE(op) = 1;
	PlaySound(SE_BANK_ENEMY, SE_NP_MOVE, op);
	op->act3++;
}

static void NP_HeadIn(OBJECT *op)
{
	if (NP_MoveToward(&OFS_Y(op), -64, 2)) {
		op->disp = 0;
		RISE(op) = 0;
		HEAD(op) = 0;
		NP_Think(op);
	}
}

/* 0x800EC138  DRILL */
static void NP_Drill0(OBJECT *op) { NP_HeadOut(op, 48); }

static void NP_Drill1(OBJECT *op)
{
	if (NP_MoveToward(&OFS_Y(op), 56, 2)) {
		op->act4 = 4;
		op->act3++;
	}
}

static void NP_WaitAct4(OBJECT *op) { if (op->act4 == 0) op->act3++; }

static void NP_Drill3(OBJECT *op)
{
	OFS_X32(op) += 0x18000;
	if (OFS_X(op) >= 208) {
		PlaySound(SE_BANK_ENEMY, SE_NP_DRILL_STOP, op);
		OFS_X(op) = 208;
		op->act4 = 4;
		op->act3++;
	}
}

static void NP_Drill4(OBJECT *op)
{
	if (op->act4 == 0) {
		op->timer = 90;
		op->act3++;
	}
}

static void NP_Drill5(OBJECT *op)
{
	if (--op->timer == 0) {
		RISE(op) = 1;
		PlaySound(SE_BANK_ENEMY, SE_NP_MOVE, op);
		op->act3++;
	}
}

/* 0x800EC380  BEAM */
static void NP_Beam0(OBJECT *op) { NP_HeadOut(op, 160); }
static void NP_Beam1(OBJECT *op) { if (NP_MoveToward(&OFS_Y(op), 56, 2)) op->act3++; }

static void NP_Beam2(OBJECT *op)
{
	if (POS_X(op) < POS_X(&mega))
		op->act4 = 6;
	else if (POS_Y(op) < POS_Y(&mega))
		op->act4 = 7;
	else
		op->act4 = 5;
	op->act3++;
}

static void NP_Beam3(OBJECT *op)
{
	if (op->act4 == 0) {
		RISE(op) = 1;
		PlaySound(SE_BANK_ENEMY, SE_NP_MOVE, op);
		op->act3++;
	}
}

/* 0x800EC550  END CRUSH (cam0.x >= 5440) */
static void (*np_end_subs[])(OBJECT *);

static void NP_End(OBJECT *op)
{
	np_end_subs[op->act3](op);
	WALL(op) = 2;
	NP_MoveToward(&OFS_Y(op), -64, 4);
}

static void NP_End0(OBJECT *op)
{
	if (MEGA_SPECIAL == 2)
		return;
	*(long *)&CAM1.y_lo -= 0x40000;
	if (CAM1.y < 777) {
		PlaySound(SE_BANK_ENEMY, SE_NP_SLAM, op);
		Rumble(5);
		CAM1.y = 776;
		op->timer = 60;
		op->act3++;
	}
}

static void NP_End1(OBJECT *op)
{
	if ((op->timer & 7) == 0)
		Quake(6, (GetRNG() & 3) | 4, 2);
	if (--op->timer == 0)
		op->act3++;
}

static void NP_End2(OBJECT *op) { }

/* 0x800EC6C4  TURRET (parts 32..34) */
static void NP_Tur0(OBJECT *op)
{
	OBJECT *bp = PARENT(op);

	switch (bp->act4) {
	case 1: if (op->var != 32) return; break;
	case 2: if (op->var != 33) return; break;
	case 3: if (op->var != 34) return; break;
	case 4:
		if (op->var == 34) {
			SetAnime(op, 2);
			op->act3 = 4;
		}
		return;
	default:
		return;
	}
	SetAnime(op, 4);
	op->act3++;
}

static void NP_Tur1(OBJECT *op) { op->timer = 50; op->act3++; }

static void NP_Tur2(OBJECT *op)
{
	if (--op->timer != 0)
		return;
	if (mega.var == 0)
		NP_MakeShot(op, op->var - 16);		/* 16, 17, 18: aimed shot */
	else
		NP_MakeShot(op, op->var);		/* 32, 33, 34: missile */
	op->act3++;
}

static void NP_Tur3(OBJECT *op)
{
	SetAnime(op, 1);
	PARENT(op)->act4 = 0;
	op->act3 = 0;
}

static void NP_Tur4(OBJECT *op)
{
	if (op->anime.flag == 1) {
		op->atk_box = &np_drill_box;
		op->atk = 8;
		PARENT(op)->act4 = 0;
		op->act3++;
	} else if (op->anime.flag == 2) {
		PlaySound(SE_BANK_ENEMY, SE_NP_DRILL_OUT, op);
	}
}

static void NP_Tur5(OBJECT *op)
{
	if (PARENT(op)->act4 == 4) {
		PlaySound(SE_BANK_ENEMY, SE_NP_DRILL_IN, op);
		SetAnime(op, 3);
		op->act3++;
	}
}

static void NP_Tur6(OBJECT *op)
{
	if (op->anime.flag == 1) {
		SetAnime(op, 1);
		op->atk_box = 0;
		op->atk = 0;
		PARENT(op)->act4 = 0;
		op->act3 = 0;
	}
}

/* 0x800ECA7C  POD (parts 16..18) */
static void NP_Pod0(OBJECT *op)
{
	char a = PARENT(op)->act4;

	if ((a == 5 && op->var == 16) || (a == 6 && op->var == 17) || (a == 7 && op->var == 18)) {
		SetAnime(op, 6);
		op->act3++;
	}
}

static void NP_Pod1(OBJECT *op)
{
	if (op->anime.flag == 1) {
		op->timer = 180;
		op->act3++;
		NP_MakeShot(op, 48);
	} else if (op->anime.flag == 2) {
		PlaySound(SE_BANK_ENEMY, SE_NP_BEAM_CHARGE, op);
	}
}

static void NP_Pod2(OBJECT *op)
{
	if (--op->timer == 0) {
		SetAnime(op, 7);
		op->act3++;
	}
}

static void NP_Pod3(OBJECT *op)
{
	if (op->anime.flag == 1) {
		PARENT(op)->act4 = 0;
		op->act3 = 0;
	}
}

static void NP_Nop(OBJECT *op) { }

/* 0x800ECC44 */
static void NP_MakePart(OBJECT *op, int var)
{
	OBJECT *cp;

	if ((cp = GetMainObjectAfter(op, 0)) == 0)
		return;
	cp->flag = 0x41;
	cp->type = 16;
	cp->flip = 0;
	cp->var = var;
	cp->clut = op->clut & ~CLUT_FLASH;
	cp->anime_tbl = op->anime_tbl;
	cp->sprt_p = op->sprt_p;
	cp->tpage = op->tpage;
	PARENT(cp) = op;
	cp->layer = op->layer;
}

/* 0x800ECCD8 */
static void NP_MakeStagePart(OBJECT *op, int var)
{
	OBJECT *cp;

	if ((cp = GetStageObject()) == 0)
		return;
	cp->flag = 0x41;
	cp->type = 10;
	cp->var = var;
	cp->x.l = POS_X(op);
	*(OBJECT **)&cp->timer = op;
	cp->y.l = POS_Y(op);
	cp->clut = op->clut & ~CLUT_FLASH;
	cp->anime_tbl = op->anime_tbl;
	cp->sprt_p = op->sprt_p;
	cp->tpage = op->tpage;
	cp->layer = op->layer;
}

/* 0x800ECD7C */
static int NP_MoveToward(short *p, short to, short step)
{
	short d = *p - to;

	if ((d >= 0 && d <= step) || (d < 0 && -d <= step)) {
		*p = to;
		return 1;
	}
	if (to < *p)
		*p -= step;
	else if (*p < to)
		*p += step;
	return 0;
}

static void (*np_states[])(OBJECT *) = {
	NP_Setup, NP_BodyMain, NP_BodyDeath, NP_PartMain, NP_PartDead, NP_Delete
};
static void (*np_death_subs[])(OBJECT *) = { NP_Death0, NP_Death1, NP_Death2, NP_Death3 };
static void (*np_press_subs[])(OBJECT *) = {
	NP_Press0, NP_Press1, NP_Press2, NP_Press3, NP_Press4, NP_Press5
};
static void (*np_end_subs[])(OBJECT *) = { NP_End0, NP_End1, NP_End2 };

#define SUBSTEP(name, ...) \
	static void (*name##_tbl[])(OBJECT *) = { __VA_ARGS__ }; \
	static void name(OBJECT *op) { name##_tbl[op->act3](op); }

SUBSTEP(NP_Intro, NP_Intro0, NP_Intro1, NP_Intro2, NP_Intro3, NP_Intro4, NP_Intro5)
SUBSTEP(NP_Wait, NP_Wait0, NP_Wait1)
SUBSTEP(NP_Bombs, NP_Bomb0, NP_Bomb1, NP_Bomb2, NP_Bomb3)
SUBSTEP(NP_Guns, NP_Gun0, NP_Gun1, NP_PickTurret, NP_Gun3, NP_HeadIn)
SUBSTEP(NP_Drill, NP_Drill0, NP_Drill1, NP_WaitAct4, NP_Drill3, NP_Drill4, NP_Drill5, NP_HeadIn)
SUBSTEP(NP_Beam, NP_Beam0, NP_Beam1, NP_Beam2, NP_Beam3, NP_HeadIn)
SUBSTEP(NP_Turret, NP_Tur0, NP_Tur1, NP_Tur2, NP_Tur3, NP_Tur4, NP_Tur5, NP_Tur6)
SUBSTEP(NP_Pod, NP_Pod0, NP_Pod1, NP_Pod2, NP_Pod3)

static void (*np_body_steps[])(OBJECT *) = {
	ReactMode, NP_Nop, NP_Intro, NP_Wait, NP_Press, NP_Bombs, NP_Guns, NP_Drill, NP_Beam, NP_End
};
static void (*np_part_steps[])(OBJECT *) = { ReactMode, NP_Nop, NP_Turret, NP_Pod };

/* ------------------------------------------------------------------------
 *	visual object 13
 * ------------------------------------------------------------------------ */
#define V_TIMER(o)	(*(short *)&(o)->work[0x88 - 0x7e])
#define V_BOX(o)	(*(HITBOX *)&(o)->work[0x8c - 0x7e])
#define V_WTEST(o)	((o)->work[0x90 - 0x7e])
#define V_NOANIM(o)	((o)->work[0x91 - 0x7e])
#define V_HIDE(o)	((o)->work[0x92 - 0x7e])
#define V_START(o)	(*(long *)&(o)->work[0x84 - 0x7e])

/* 0x800EAB50 */
static void V13_Setup(OBJECT *op)
{
	OBJECT *pp = SHOT_PARENT(op);
	int a;

	op->pri = 32;
	op->dmg_tbl = dmg_tbl_minor;
	op->atk = 0;
	op->hp = 1;
	op->fall_f = 1;
	op->col_box = op->atk_box = op->dmg_box = 0;
	op->act = 1;
	op->act2 = op->act3 = op->act4 = 0;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
	V_NOANIM(op) = 0; V_WTEST(op) = 0; V_HIDE(op) = 0;

	switch (op->var & 0xf0) {
	case 0:
		SetAnime(op, 8);
		op->atk = 2;
		op->col_box = &np_bomb_col;
		op->atk_box = &np_bomb_atk;
		op->spd_x = 0;
		op->spd_y = -0x20000;
		op->grav = 0x1800;
		MoveObject(op);
		op->act3 = 0;
		break;
	case 16:
		PlaySound(SE_BANK_ENEMY, SE_NP_SHOT, op);
		SetAnime(op, 10);
		op->atk = 4;
		op->col_box = &np_shot_col;
		op->atk_box = &np_shot_atk;
		a = ratan2(POS_Y(op) - POS_Y(&mega), POS_X(&mega) - POS_X(op));
		op->spd_x = rcos(a) * 80;
		op->spd_y = rsin(a) * 80;
		op->flip = op->spd_x ? OBJ_FLIP_R : OBJ_FLIP_L;
		MoveObject(op);
		op->act3 = 1;
		break;
	case 32:
		PlaySound(SE_BANK_ENEMY, SE_NP_MISSILE, op);
		SetAnime(op, 9);
		op->atk = 4;
		op->hp = 2;
		op->dmg_box = &np_missile_dmg;
		op->atk_box = &np_missile_atk;
		V_TIMER(op) = 60;
		V_WTEST(op) = 1;
		if ((op->var & 0x0f) == 1) {
			op->spd_x = -0x18000;
			op->flip = OBJ_FLIP_L;
			POS_X(op) -= 44;
		} else {
			op->spd_x = 0x18000;
			op->flip = OBJ_FLIP_R;
			POS_X(op) += 44;
		}
		MoveObject(op);
		op->act3 = 2;
		break;
	case 48:
		PlaySound(SE_BANK_ENEMY, SE_NP_BEAM, op);
		SetAnime(op, 11);
		op->atk = 6;
		V_BOX(op).h = 24;
		op->atk_box = &V_BOX(op);
		V_BOX(op).oy = -12;
		V_BOX(op).ox = 127 - op->anime.flag * 16;
		V_BOX(op).w = op->anime.flag * 16;
		POS_X(op) = POS_X(pp) + (op->flip ? 164 : -164);
		op->act3 = 3;
		return;
	}
	if (OffScreenCheck2(op, 32, 32))
		op->act = 2;
	else {
		DisplayCheck(op, 16, 16);
		if (V_HIDE(op))
			op->disp = 0;
	}
}

/* 0x800EAE90 */
static void (*v13_steps[])(OBJECT *);

static void V13_Main(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	if (V_NOANIM(op) == 0)
		AnimeTick(op);
	v13_steps[op->act3](op);
	if (V_WTEST(op) && ContactWeapon(op) < 0)
		goto blow;
	if (ContactMega(op)) {
		switch (op->var & 0xf0) {
		case 0:
		case 32:
			goto blow;
		case 16:
			op->disp = 0;
			if (mega.var == 0)
				SpawnHitEffect(op);
			else
				SpawnExplosion(op);
			op->act = 2;
			return;
		}
	}
	if ((op->var & 0xf0) == 48)
		return;
	if (OffScreenCheck2(op, 32, 32)) {
		op->act = 2;
		return;
	}
	DisplayCheck(op, 16, 16);
	if (V_HIDE(op))
		op->disp = 0;
	return;
blow:
	op->disp = 0;
	SpawnExplosion(op);
	op->act = 2;
}

/* 0x800EAFFC  missile: follow Zero's height */
static void V13_Missile0(OBJECT *op)
{
	int dy;

	if (--V_TIMER(op) == 0) {
		op->spd_y = 0;
		MoveObject(op);
		op->act4++;
		return;
	}
	dy = POS_Y(&mega) - POS_Y(op);
	if ((dy >= 0 && dy < 3) || (dy < 0 && -dy < 3))
		op->spd_y = 0;
	else
		op->spd_y = (POS_Y(&mega) < POS_Y(op)) ? 0x18000 : -0x18000;
	if ((op->var & 0x0f) == 1 ? (POS_X(op) < POS_X(&mega)) : (POS_X(&mega) < POS_X(op))) {
		op->spd_y = 0;
		op->act4++;
	}
	MoveObject(op);
}

/* 0x800ECDF8 */
static void V13_Delete(OBJECT *op)
{
	if ((op->var & 0xf0) == 48)
		PlaySound(SE_BANK_ENEMY, SE_NP_BEAM_END, op);
	DeleteObject3(op);
}

/* 0x800ECE7C  bomb: falling */
static void V13_Bomb0(OBJECT *op)
{
	MoveObject2(op);
	if (op->spd_y < -0x40000)
		op->spd_y = -0x40000;
	if (op->col_f == 0) {
		CornerCheck(op);
		if (op->col_f == 0)
			return;
	}
	PlaySound(SE_BANK_ENEMY, SE_NP_BOMB_BOUNCE, op);
	op->spd_x = -0x10000;
	op->spd_y = 0x15555;
	op->col_f = 0;
	op->grav = 0xb60;
	op->act4++;
	MoveObject2(op);
}

/* 0x800ECF20  bomb: one bounce */
static void V13_Bomb1(OBJECT *op)
{
	MoveObject2(op);
	if (op->col_f == 0) {
		CornerCheck(op);
		if (op->col_f == 0)
			return;
	}
	PlaySound(SE_BANK_ENEMY, SE_NP_BOMB_BOUNCE, op);
	op->spd_x = -0x10000;
	V_NOANIM(op) = 1;
	op->spd_y = 0;
	V_TIMER(op) = 60;
	op->act4++;
}

/* 0x800ECFA0  bomb: roll */
static void V13_Bomb2(OBJECT *op)
{
	MoveObject(op);
	if (--V_TIMER(op) == 0)
		op->act4++;
}

/* 0x800ECFF0  bomb: blast */
static void V13_Bomb3(OBJECT *op)
{
	op->atk_box = &np_blast_box;
	SpawnExplosion(op);
	V_HIDE(op) = 1;
	V_TIMER(op) = 30;
	op->act4++;
}

/* 0x800ED03C */
static void V13_Bomb4(OBJECT *op)
{
	if (--V_TIMER(op) == 0)
		op->act = 2;
}

/* 0x800ED064  aimed shot */
static void (*v13_shot_subs[])(OBJECT *);

static void V13_Shot(OBJECT *op)
{
	v13_shot_subs[op->act4](op);
	if (op->col_f == 0) {
		CornerCheck(op);
		if (op->col_f == 0)
			return;
	}
	if (op->spd_y < 0)
		SpawnHitEffect(op);
	op->act = 2;
}

static void V13_Move1(OBJECT *op) { MoveObject(op); op->act4++; }
static void V13_Move(OBJECT *op)  { MoveObject(op); }

/* 0x800ED1D8  beam */
static void V13_Beam0(OBJECT *op)
{
	OBJECT *pp = SHOT_PARENT(op);

	POS_X(op) = POS_X(pp) + (op->flip ? 164 : -164);
	op->disp = 1;
	V_BOX(op).ox = 127 - op->anime.flag * 16;
	V_BOX(op).w = op->anime.flag * 16;
	if (V_START(op) != pp->act3) {
		op->atk_box = 0;
		op->disp = 0;
		op->act4++;
	}
}

static void V13_Beam1(OBJECT *op) { op->act = 2; }

static void (*v13_shot_subs[])(OBJECT *) = { V13_Move1, V13_Move };
SUBSTEP(V13_Bomb, V13_Bomb0, V13_Bomb1, V13_Bomb2, V13_Bomb3, V13_Bomb4)
SUBSTEP(V13_Missile, V13_Missile0, V13_Move)
SUBSTEP(V13_Beam, V13_Beam0, V13_Beam1)
static void (*v13_steps[])(OBJECT *) = { V13_Bomb, V13_Shot, V13_Missile, V13_Beam };
static void (*v13_states[])(OBJECT *) = { V13_Setup, V13_Main, V13_Delete };

/* 0x800EB504 */
void NightmarePressureShot(OBJECT *op)
{
	v13_states[op->act](op);
}
