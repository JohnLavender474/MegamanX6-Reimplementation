/*
 *	v16_sheldon.c  --  Shield Sheldon's shields and shots (visual object 16)
 *	                   and warp flashes (effect object 5), stage 7 / overlay 10
 *
 *	Plausible reconstruction, not original code, not compiled.
 */
#include "object.h"
#include "game.h"
#include "sound.h"

#define CAM_X		(bg_layer[0].x)
#define CAM_Y		(bg_layer[0].y)
#define RX(n)		(CAM_X + (n))
#define RY(n)		(CAM_Y + (n))
#define MIRROR_X(x)	(CAM_X - ((x) - CAM_X - 320))
#define ABS(a)		((a) < 0 ? -(a) : (a))
#define PX(n)		((long)(n) << 16)
#define W8(o, n)	(*(char *)((u_char *)(o) + (n)))
#define W16(o, n)	(*(short *)((u_char *)(o) + (n)))
#define W32(o, n)	(*(long *)((u_char *)(o) + (n)))
#define WP(o, n)	(*(OBJECT **)((u_char *)(o) + (n)))
#define BOSS(o)		WP(o, 0x7c)

extern HITBOX	shield_box, bullet_box, bullet_col;
extern u_char	dmg_tbl_one_hit[];

static u_char	mshield_route[2][7] = { { 1, 2, 3, 4, 5, 6, 0 }, { 4, 5, 6, 1, 2, 3, 0 } };
static short	mshield_pt[8][2] = {
	{ 0, 0 }, { 80, 176 }, { 16, 64 }, { 64, 0 },
	{ 240, 176 }, { 304, 64 }, { 256, 0 }, { 160, 176 } };

static void	Vs_End(OBJECT *op)
{
	op->atk_box = 0;
	op->dmg_box = 0;
	op->disp = 0;
	op->act = 3;
}

static int	Vs_FlyTo(OBJECT *op, short tx, short ty, int shift, int near)
{
	int	a = ratan2(RY(ty) - POS_Y(op), RX(tx) - POS_X(op));

	op->spd_x = rcos(a) << shift;
	op->spd_y = rsin(a) << shift;
	MoveObject(op);
	return ABS(tx - (POS_X(op) - CAM_X)) < near && ABS(ty - (POS_Y(op) - CAM_Y)) < near;
}

/* ---- set-up  0x800F60AC ---- */
static void	V16_Init(OBJECT *op)
{
	int	a;

	if (BOSS(op)->act != 1) { op->act = 3; op->disp = 0; return; }
	op->atk = 0; op->hp = 1; op->pri = 4; op->fall_f = 1;
	op->dmg_tbl = dmg_tbl_guard;
	op->col_box = 0; op->atk_box = 0; op->dmg_box = 0;
	op->act = 1; op->act2 = 0; op->act3 = 0; op->act4 = 0;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;

	switch (op->var & 0xf0) {
	case 0x00:
		SetAnime(op, 3);
		op->dmg_box = op->atk_box = &shield_box;
		op->atk = 8;
		if (op->var == 0) {
			if (POS_X(op) < RX(160)) { op->flip = OBJ_FLIP_R; op->spd_x = PX(2); }
			else			 { op->flip = OBJ_FLIP_L; op->spd_x = -PX(2); }
			op->act2 = 1;
		} else {
			op->flip = (POS_X(op) < RX(160)) ? OBJ_FLIP_R : OBJ_FLIP_L;
			op->act2 = 2;
		}
		break;
	case 0x10:
		PlaySound(SE_BANK_ENEMY, SE_SHD_SHOT, op);
		SetAnime(op, 28);
		op->dmg_tbl = dmg_tbl_one_hit;
		op->dmg_box = op->atk_box = &bullet_box;
		op->atk = 4;
		op->flip = OBJ_FLIP_L;
		switch (op->var) {
		case 16: op->spd_y = PX(2);  break;
		case 17: op->spd_x = -PX(2); break;
		case 18: op->spd_y = -PX(2); break;
		case 19: op->spd_x = PX(2);  break;
		}
		if (game.level >= LEVEL_HARD) {
			op->spd_x = op->spd_x * 3 / 2;
			op->spd_y = op->spd_y * 3 / 2;
		}
		op->act2 = 3;
		break;
	case 0x20:
		SetAnime(op, 3);
		op->dmg_box = op->atk_box = &shield_box;
		op->atk = 8;
		op->act2 = (op->var == 32) ? 4 : 5;
		break;
	case 0x30:
		PlaySound(SE_BANK_ENEMY, SE_SHD_SHOT, op);
		SetAnime(op, 34);
		op->atk_box = &bullet_box;
		op->atk = 8;
		op->disp = 1;
		op->dmg_box = 0;
		op->col_box = &bullet_col;
		a = ratan2(POS_Y(op) - POS_Y(&mega), POS_X(&mega) - POS_X(op));
		op->spd_x = rcos(a) << 5;
		op->spd_y = rsin(a) << 5;
		if (game.level >= LEVEL_HARD) {
			op->spd_y = rsin(a) << 6;
			op->spd_x <<= 1;
		}
		op->flip = op->spd_x ? OBJ_FLIP_R : OBJ_FLIP_L;
		op->act2 = 6;
		break;
	}
	op->clut &= ~CLUT_FLASH;
}

/* ---- shield A (var 0)  0x800FA0EC ---- */
static void	ShieldA(OBJECT *op)
{
	OBJECT	*bp = BOSS(op);

	switch (op->act3) {
	case 0:
		MoveObject(op);
		if (op->flip == OBJ_FLIP_L) {
			if (POS_X(op) <= RX(32)) { op->spd_x = PX(2); op->flip = OBJ_FLIP_R; op->act3++; }
		} else if (POS_X(op) >= RX(288)) { op->spd_x = -PX(2); op->flip = OBJ_FLIP_L; op->act3++; }
		break;
	case 1:
		MoveObject(op);
		if (op->flip == OBJ_FLIP_L ? POS_X(op) <= RX(112) : POS_X(op) >= RX(208))
			op->act3++;
		break;
	case 2:
		if (GetRNG() & 1) {
			bp->act4 = 1;
			op->flip ^= OBJ_FLIP_R;
			op->act3 = 3;
		} else {
			bp->act4 = 2;
			MoveObject(op);
			op->act3 = 4;
		}
		break;
	case 3:	/* 0x800F65DC */
		if (Vs_FlyTo(op, op->flip ? 272 : 64, 64, 6, 5)) {
			bp->act4 = 4;
			Vs_End(op);
		}
		break;
	case 4:
		MoveObject(op);
		if (op->flip == OBJ_FLIP_L ? POS_X(op) <= RX(48) : POS_X(op) >= RX(272)) {
			bp->act4 = 3;
			Vs_End(op);
		}
		break;
	}
}

/* ---- shield B (var 1)  0x800FA340 ---- */
static void	ShieldB(OBJECT *op)
{
	OBJECT	*bp = BOSS(op);
	short	tx;

	switch (op->act3) {
	case 0:	/* 0x800F6794 */
		tx = op->flip ? 272 : 64;
		if (Vs_FlyTo(op, tx, 64, 6, 5)) {
			POS_X(op) = RX(tx);
			POS_Y(op) = RY(64);
			op->act3++;
		}
		break;
	case 1:
		if (bp->act4 == 1)
			op->act3 = 2;
		if (bp->act4 == 2) {
			op->flip ^= OBJ_FLIP_R;
			op->act3 = 3;
		}
		break;
	case 2:
		if (bp->act3 == 8)
			Vs_End(op);
		break;
	case 3:	/* 0x800F6978 */
		tx = op->flip ? 272 : 48;
		if (Vs_FlyTo(op, tx, 144, 6, 5)) {
			POS_X(op) = RX(tx);
			POS_Y(op) = RY(144);
			op->act3++;
		}
		break;
	case 4:
		bp->act4 = 4;
		Vs_End(op);
		break;
	}
}

/* ---- mirror shield (var 32)  0x800FA42C ---- */
static void	MirrorShield(OBJECT *op)
{
	int	i, a, v;
	short	px;

	switch (op->act3) {
	case 0:
		W8(op, 0x94) = op->flip ? 1 : 0;
		W16(op, 0x88) = MIRROR_X(POS_X(op));
		W16(op, 0x8a) = POS_Y(op);
		W32(op, 0x84) = (GetRNG() & 1) ? 0 : 0x0f;
		if (!(GetRNG() & 1))
			W32(op, 0x84) |= 0xf0;
		op->act4 = 0;
		op->act3++;
		break;
	case 1:	/* 0x800F6B5C */
		i = mshield_route[(int)W8(op, 0x94)][(int)op->act4];
		px = POS_X(&mega) - CAM_X;
		W16(op, 0x90) = mshield_pt[i][0];
		W16(op, 0x92) = mshield_pt[i][1];
		if (i == 0) {
			W16(op, 0x90) = W16(op, 0x88) - CAM_X;
			W16(op, 0x92) = W16(op, 0x8a) - CAM_Y;
		} else if (i == 1 && (W32(op, 0x84) & 0x0f))
			W16(op, 0x90) = (px < 33) ? 32 : (px >= 144) ? 144 : (px & 0xff0);
		else if (i == 4 && (W32(op, 0x84) & 0xf0))
			W16(op, 0x90) = (px < 177) ? 176 : (px >= 272) ? 272 : (px & 0xff0);
		op->act3++;
		break;
	case 2:	/* 0x800F6D58 */
		a = ratan2(RY(W16(op, 0x92)) - POS_Y(op), RX(W16(op, 0x90)) - POS_X(op));
		v = op->act4 / 2 + 6;
		op->spd_x = (rcos(a) << 4) * v;
		op->spd_y = (rsin(a) << 4) * v;
		op->flip = (op->spd_x >= 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
		MoveObject(op);
		if (ABS(W16(op, 0x90) - (POS_X(op) - CAM_X)) >= 6 || ABS(W16(op, 0x92) - (POS_Y(op) - CAM_Y)) >= 6)
			break;
		POS_X(op) = RX(W16(op, 0x90));
		POS_Y(op) = RY(W16(op, 0x92));
		if (++op->act4 >= 7) {
			op->act4 = 0;
			op->act3++;
		} else {
			PlaySound(SE_BANK_ENEMY, SE_SHD_BOUNCE, op);
			Rumble(6);
			op->act3--;
		}
		break;
	case 3:
		BOSS(op)->act4 = 4;
		op->disp = 0;
		op->act = 3;
		break;
	}
}

static void	MirrorCopy(OBJECT *op)		/* 0x800FA538 */
{
	OBJECT	*ap = WP(op, 0x8c);

	POS_X(op) = MIRROR_X(POS_X(ap));
	POS_Y(op) = POS_Y(ap);
	op->anime.sprt = ap->anime.sprt;
	op->disp = ap->disp;
	op->flip = ap->flip ? OBJ_FLIP_L : OBJ_FLIP_R;
	if (ap->act != 1)
		op->act = 3;
}

static void	Straight(OBJECT *op)	{ MoveObject(op); }	/* 0x800FA61C */

static void	Aimed(OBJECT *op)				/* 0x800FA5BC */
{
	MoveObject(op);
	if (op->col_f == 0)
		CornerCheck(op);
	if (op->col_f) {
		SpawnHitEffect(op);
		op->disp = 0;
		op->act = 3;
	}
}

static void	V16_Nop(OBJECT *op) { }

static void	(*v16_step[])(OBJECT *) = {	/* 0x800FCF50 */
	V16_Nop, ShieldA, ShieldB, Straight, MirrorShield, MirrorCopy, Aimed
};

/* ---- main  0x800F6484 ---- */
static void	V16_Main(OBJECT *op)
{
	OBJECT	*bp = BOSS(op);
	int	g;

	if (bp->act != 1) { op->act = 3; op->disp = 0; return; }
	if (bp->act2 == 9) { op->act = 2; op->act2 = 0; op->disp = 0; return; }
	op->old_x = op->x;
	op->old_y = op->y;
	AnimeTick(op);
	(*v16_step[(int)op->act2])(op);
	if (op->act != 1) { op->disp = 0; return; }

	if (ContactWeapon(op) < 0) {
		op->disp = 0;
		SpawnExplosion(op);
		op->act = 3;
		return;
	}
	g = op->var & 0xf0;
	if (ContactMega(op) && (g == 0x10 || g == 0x30)) {
		op->disp = 0;
		SpawnHitEffect(op);
		op->act = 3;
		return;
	}
	if (OffScreenCheck2(op, 32, 32))
		op->act = 3;
	else
		DisplayCheck(op, 16, 16);
}

/* ---- fall (boss stunned)  0x800FA030 ---- */
static void	V16_Fall(OBJECT *op)
{
	if (op->act2 == 0) {
		op->act2++;
		W16(op, 0x88) = 0;
		op->spd_x = op->acc_x = op->spd_y = 0;
		op->grav = 0x4200;
	}
	MoveObject2(op);
	if (OffScreenCheck2(op, 64, 64)) {
		op->act = 3;
		return;
	}
	op->disp = (W16(op, 0x88)++ & 1) ? 1 : 0;
}

static void	V16_Delete(OBJECT *op)	{ DeleteObject3(op); }

static void	(*v16_act[])(OBJECT *) = { V16_Init, V16_Main, V16_Fall, V16_Delete };	/* 0x800FCF40 */

void	V16_Sheldon(OBJECT *op)		/* 0x800F9FE4 */
{
	op->old_x = op->x;
	op->old_y = op->y;
	(*v16_act[(int)op->act])(op);
}

/* ================================================================ */
/*	effect object 5  0x800FA8C4					*/
/* ================================================================ */
#define FX_BOSS(o)	WP(o, 0x50)

static void	E5_Init(OBJECT *op)		/* 0x800FA644 */
{
	switch (op->var) {
	case 0:  SetAnime(op, 4);  op->act2 = 0; break;
	case 16: SetAnime(op, 31); W16(op, 0x54) = 0; op->act2 = 1; break;
	case 32: SetAnime(op, 33); op->act2 = 2; break;
	default: return;
	}
	op->pri = 4;
	op->disp = 1;
	op->act = 1;
	op->act3 = 0;
	op->clut &= ~CLUT_FLASH;
}

static void	E5_Main(OBJECT *op)		/* 0x800FA710 */
{
	OBJECT	*bp = FX_BOSS(op);

	if (bp->act != 1 || bp->act2 == 9) {
		op->disp = 0;
		op->act++;
		return;
	}
	AnimeTick(op);
	switch (op->act2) {
	case 0:				/* 0x800FA7B8 */
		if (op->anime.flag == 2) {
			PlaySound(SE_BANK_ENEMY, SE_SHD_WARP_FX, 0);
			bp->act4 = 1;
		} else if (op->anime.flag == 1) {
			op->disp = 0;
			op->act++;
		}
		break;
	case 1:				/* 0x800FA81C */
		if (op->anime.flag == 1) {
			SetAnime(op, 31);
			if (W16(op, 0x54)++ >= 1) {
				op->disp = 0;
				op->act++;
			}
		} else if (op->anime.flag == 2)
			PlaySound(SE_BANK_ENEMY, SE_SHD_WARP_FX, op);
		break;
	case 2:				/* 0x800FA89C */
		if (op->anime.flag == 1) {
			op->disp = 0;
			op->act++;
		}
		break;
	}
}

static void	E5_Delete(OBJECT *op)	{ DeleteObject3(op); }

static void	(*e5_act[])(OBJECT *) = { E5_Init, E5_Main, E5_Delete };	/* 0x800FCFD4 */

void	E5_SheldonFx(OBJECT *op)
{
	(*e5_act[(int)op->act])(op);
}
