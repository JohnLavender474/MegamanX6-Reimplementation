/*
 *	m51_sheldon.c  --  Shield Sheldon (stage 7 / overlay 10, main object 51)
 *
 *	var  0      the boss
 *	var 16..19  shield plates (up, left, down, right)
 *	var 32      mirror image
 *	var 48..51  corner turrets
 *	visual object 16: shields and shots; effect object 5: warp flashes
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
#define WP(o, n)	(*(OBJECT **)((u_char *)(o) + (n)))

/* work fields of the boss */
#define PARENT(o)	WP(o, 0x80)
#define TGT_X(o)	W16(o, 0x84)
#define TGT_Y(o)	W16(o, 0x86)
#define INV(o)		W16(o, 0x88)
#define PAT_MODE(o)	W8(o, 0x8a)
#define PAT_NO(o)	W8(o, 0x8b)
#define PAT_POS(o)	W8(o, 0x8c)
#define BLOCKED(o)	W8(o, 0x8d)
#define GUARD(o)	W8(o, 0x8e)
#define CNT(o)		W16(o, 0x7e)
/* work fields of the children */
#define HIT_F(o)	W8(o, 0x88)
#define FLASH_F(o)	W8(o, 0x89)

/* ---- data (0x800E9E28, 0x800FC734 ..) ---- */
static u_char	hp_bonus[4] = { 0, 4, 8, 16 };

HITBOX	shd_atk_box    = { -14, -14, 28, 28 };	/* 0x800FC734 */
HITBOX	shd_dmg_box    = { -16, -16, 32, 32 };	/* 0x800FC738 */
HITBOX	shd_col_box    = {   0,   0,  8,  8 };	/* 0x800FC73C */
HITBOX	shd_lunge_atk  = { -32, -24, 48, 40 };	/* 0x800FC740 */
HITBOX	shd_lunge_dmg  = { -20, -20, 40, 40 };	/* 0x800FC744 */
HITBOX	plate_v_box[2] = { { -16, -8, 32, 16 }, { -16, -8, 32, 16 } };	/* 0x800FC748 */
HITBOX	plate_h_box[2] = { { -8, -16, 16, 32 }, { -8, -16, 16, 32 } };	/* 0x800FC750 */
HITBOX	plate_open_box = {  -8,  -8, 16, 16 };	/* 0x800FC758 / 0x800FC75C */
HITBOX	shield_box     = { -16, -12, 32, 28 };	/* 0x800FCF28 / 0x800FCF2C */
HITBOX	bullet_box     = {  -4,  -4,  8,  8 };	/* 0x800FCF30 .. 0x800FCF38 */
HITBOX	bullet_col     = {   0,   0,  2,  2 };	/* 0x800FCF3C */

extern ANIME	*shd_anime_tbl[];		/* 0x800FCBBC, 41 animations */

/* step 3 (0x800FCCE4, 0x800FCCF4) */
static u_char	bounce_route[2][8] = { { 1, 2, 3, 4, 5, 6, 7, 0 }, { 4, 5, 6, 1, 2, 3, 7, 0 } };
static short	bounce_pt[8][2] = {
	{ 160, 32 }, { 80, 176 }, { 16, 80 }, { 96, 0 },
	{ 240, 176 }, { 304, 80 }, { 224, 0 }, { 160, 176 } };
/* mirror shield (0x800FCFA4, 0x800FCFB4) */
static u_char	mshield_route[2][7] = { { 1, 2, 3, 4, 5, 6, 0 }, { 4, 5, 6, 1, 2, 3, 0 } };
static short	mshield_pt[8][2] = {
	{ 0, 0 }, { 80, 176 }, { 16, 64 }, { 64, 0 },
	{ 240, 176 }, { 304, 64 }, { 256, 0 }, { 160, 176 } };

/* THINK (0x800FCE24, 0x800FCEA4) */
static u_char	pat_high[16][8] = {
	{4,3,0xff},{4,3,0xff},{4,3,0xff},{4,3,0xff},{4,3,0xff},{4,3,0xff},{4,3,0xff},{4,3,0xff},
	{4,3,0xff},{4,6,0xff},{4,6,0xff},{4,6,0xff},{4,6,0xff},{4,6,0xff},{4,6,0xff},{4,6,0xff} };
static u_char	pat_low[16][8] = {
	{5,7,0xff},{5,7,0xff},{5,7,0xff},{5,7,0xff},{5,7,0xff},{5,4,0xff},{5,4,0xff},{5,4,0xff},
	{5,4,0xff},{5,4,0xff},{5,4,0xff},{6,6,7,0xff},{6,6,7,0xff},{6,6,7,0xff},{6,6,7,0xff},
	{6,6,7,0xff} };

static short	corner[4][2] = { { 288, 32 }, { 32, 32 }, { 32, 144 }, { 288, 144 } };

/* overlay globals (0x800FD000, 0x800FD004) */
u_long	shd_fight_time;
long	shd_freeze;

static void	Shd_Think(OBJECT *op);
static void	Shd_SprtSet(OBJECT *op, int set);
static void	Shd_Shot(OBJECT *op, int var);
static void	Shd_ShotPair(OBJECT *op);
static void	Shd_Effect(OBJECT *op, short x, short y, int var);
static void	Shd_Turret(OBJECT *op, int var);
static void	Shd_FacePlayer(OBJECT *op);
static void	Shd_Plate(OBJECT *op, int var);
static void	Shd_Image(OBJECT *op, int var);
static void	Shd_Soul(int var);

static void	Shd_Boxes(OBJECT *op)
{
	op->atk_box = &shd_atk_box;
	op->dmg_box = &shd_dmg_box;
}

static void	Shd_NoBoxes(OBJECT *op)
{
	op->atk_box = 0;
	op->dmg_box = 0;
}

static void	Shd_Guard(OBJECT *op)
{
	op->dmg_tbl = dmg_tbl_guard;
	GUARD(op) = 1;
}

static void	Shd_Open(OBJECT *op)
{
	op->dmg_tbl = dmg_tbl_sheldon;
	GUARD(op) = 0;
}

static void	Shd_Voice(OBJECT *op, int no)
{
	PlaySound(SE_BANK_ENEMY, (GetRNG() & 1) ? no : no + 1, op);
}

/* the warp-in sound pattern of every step */
static int	Shd_WarpFlag(OBJECT *op)
{
	switch (op->anime.flag) {
	case 2:	PlaySound(SE_BANK_ENEMY, SE_SHD_WARP, op);	break;
	case 3:	PlaySound(SE_BANK_ENEMY, SE_SHD_WARP_OUT, op);	break;
	}
	return op->anime.flag == 1;
}

/* ================================================================ */
/*	set-up  0x800F44A0						*/
/* ================================================================ */
static void	Shd_Init(OBJECT *op)
{
	OBJECT	*pp;

	if (game.stage == 12 && op->var == 0) {
		op->clut = 0x7948;
		op->sprt_p = (char *)stage_sprt_bank + stage_sprt_bank->ofs[0];
	}
	op->atk = 10;
	op->dmg_tbl = dmg_tbl_guard;
	op->hp = 0;
	op->muteki = 0;
	op->disp = 0;
	op->layer = mega.layer;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
	op->fall_f = 0;
	op->anime_tbl = shd_anime_tbl;

	switch (op->var & 0xf0) {
	case 0x00:
		op->tpage = 0x140;
		op->pri = 5;
		Shd_Boxes(op);
		op->col_box = 0;
		SetAnime(op, 4);
		op->flip = OBJ_FLIP_L;
		shd_fight_time = 0;
		PAT_MODE(op) = -1;
		PAT_NO(op) = 0;
		PAT_POS(op) = 0;
		GUARD(op) = 0;
		shd_freeze = 0;
		op->act = 1;
		op->act2 = 8;
		break;
	case 0x10:
		pp = PARENT(op);
		if (pp->act != 1) { op->act = 7; op->disp = 0; return; }
		op->pri = 5;
		op->disp = 1;
		switch (op->var) {
		case 16: SetAnime(op, 24); op->atk_box = &plate_v_box[0]; op->dmg_box = &plate_v_box[1];
			 POS_X(op) = POS_X(pp); POS_Y(op) = POS_Y(pp) - 38; break;
		case 17: SetAnime(op, 25); op->atk_box = &plate_h_box[0]; op->dmg_box = &plate_h_box[1];
			 POS_X(op) = POS_X(pp) - 38; POS_Y(op) = POS_Y(pp); break;
		case 18: SetAnime(op, 24); op->atk_box = &plate_v_box[0]; op->dmg_box = &plate_v_box[1];
			 POS_X(op) = POS_X(pp); POS_Y(op) = POS_Y(pp) + 38; break;
		case 19: SetAnime(op, 25); op->atk_box = &plate_h_box[0]; op->dmg_box = &plate_h_box[1];
			 POS_X(op) = POS_X(pp) + 38; POS_Y(op) = POS_Y(pp); break;
		}
		op->col_box = 0;
		op->flip = OBJ_FLIP_L;
		op->act = 3;
		op->act2 = 2;
		break;
	case 0x20:
		PlaySound(SE_BANK_ENEMY, SE_SHD_IMAGE, op);
		pp = PARENT(op);
		if (pp->act != 1) { op->act = 7; op->disp = 0; return; }
		op->pri = 5;
		op->disp = 1;
		SetAnime(op, 8);
		Shd_Boxes(op);
		op->col_box = 0;
		POS_X(op) += (op->flip == OBJ_FLIP_L) ? 32 : -32;
		op->act = 4;
		op->act2 = 2;
		break;
	case 0x30:
		pp = PARENT(op);
		if (pp->act != 1) { op->act = 7; op->disp = 0; return; }
		op->flip = (op->var == 48 || op->var == 51) ? OBJ_FLIP_R : OBJ_FLIP_L;
		op->pri = 5;
		op->disp = 1;
		SetAnime(op, 2);
		Shd_Boxes(op);
		op->col_box = 0;
		FLASH_F(op) = 0;
		op->act = 5;
		op->act2 = 2;
		break;
	default:
		return;
	}
	op->act3 = 0;
	op->act4 = 0;
}

/* ================================================================ */
/*	boss main  0x800F48B4						*/
/* ================================================================ */
static void	(*shd_step_tbl[])(OBJECT *);	/* 0x800FCC80 */

static void	Shd_Main(OBJECT *op)
{
	int	r;

	op->old_x = op->x;
	op->old_y = op->y;
	AnimeTick(op);
	(*shd_step_tbl[op->act2])(op);

	if (op->act2 != 8 && op->var == 0)
		shd_fight_time++;

	BLOCKED(op) = 0;
	r = ContactWeapon(op);
	if (r < 0) {
		mega.nohit_f = 1;
		op->act = 2;
		op->act2 = 0;
		return;
	}
	if (r > 0) {
		if (r == 127)
			BLOCKED(op) = 1;
		else {
			op->dmg_tbl = dmg_tbl_guard;
			INV(op) = (r == 25 || r == 26) ? 2 : 30;
			if (r == 5 || r == 14 || r == 36) {	/* weapon ids 4, 13, 35 */
				op->dmg_tbl = dmg_tbl_guard;
				GUARD(op) = 1;
				INV(op) = 80;
				op->act2 = 9;
				op->act3 = 0;
			}
		}
	}
	if (INV(op)) {
		if (INV(op)-- & 2)	op->clut |= CLUT_FLASH;
		else			op->clut &= ~CLUT_FLASH;
		if (INV(op) == 0) {
			op->clut &= ~CLUT_FLASH;
			if (GUARD(op) == 1) {
				op->dmg_tbl = dmg_tbl_guard;
				GUARD(op) = 0;
			} else
				op->dmg_tbl = dmg_tbl_sheldon;
		}
	}
	ContactMega(op);
}

/* ================================================================ */
/*	death  0x800F4A74 (step 2), 0x800F7968, 0x800F79F4		*/
/* ================================================================ */
static void	Shd_Dead0(OBJECT *op)
{
	PlaySound(SE_BANK_ENEMY, SE_SHD_DEAD, op);
	W8(&mega, 0xca) = 0;
	PlayerEvent(20, mega.flip);
	SetAnime(op, 18);
	op->act2 = 1;
	op->timer = 127;
	CNT(op) = 25;
	op->muteki = 25;
	op->clut &= ~CLUT_FLASH;
	DisplayCheck(op, 96, 96);
}

static void	Shd_Dead1(OBJECT *op)
{
	OBJECT	*ep;

	if (--op->timer == 0) {
		op->act2 = 2;
		if ((ep = GetSysObject()) != 0) {
			ep->flag = 1;
			ep->type = 26;
			POS_X(ep) = POS_X(op);
			POS_Y(ep) = POS_Y(op);
			PARENT(op) = ep;
		}
	}
	DisplayCheck(op, 96, 96);
	if (--CNT(op) == -1) {
		op->clut ^= CLUT_FLASH;
		op->muteki -= 5;
		if (op->muteki >= 26)
			op->muteki = 0;
		CNT(op) = (op->muteki < 6) ? 5 : op->muteki;
	}
}

static void	Shd_Dead2(OBJECT *op)
{
	OBJECT	*ep = PARENT(op);

	op->disp = 0;
	if (ep->flag) {
		if (ep->act4 == 0) {
			if (--CNT(op) == -1) {
				CNT(op) = 5;
				op->clut ^= CLUT_FLASH;
			}
			DisplayCheck(op, 96, 96);
		}
		return;
	}
	PARENT(op) = 0;
	W16(op, 0x84) = W16(op, 0x86) = 0;
	W16(op, 0x88) = W16(op, 0x8a) = 0;
	W16(op, 0x8c) = W16(op, 0x8e) = 0;
	game.boss_bar = 0;
	game.boss = 0;
	if (game.stage == 12) {
		game.message = 0x80;
		game.clear_f[(int)game.checkpoint] = 1;
		game.checkpoint += 8;
	} else
		Shd_Soul(shd_fight_time < 5401 ? 0x10 : 0x11);
	DeleteObject3(op);
}

/* ================================================================ */
/*	step 3: bounce  0x800F4B9C, 0x800F4D7C, 0x800F8110 ..		*/
/* ================================================================ */
static void	Shd_BounceAim(OBJECT *op)
{
	int	i, row = op->timer;
	short	px;

	Shd_FacePlayer(op);
	i = bounce_route[row][(int)op->act4];
	TGT_X(op) = bounce_pt[i][0];
	TGT_Y(op) = bounce_pt[i][1];
	px = POS_X(&mega) - CAM_X;
	if (i == 1 && (CNT(op) & 0x0f))
		TGT_X(op) = (px < 49) ? 48 : (px >= 144) ? 144 : (px & 0xff0);
	if (i == 4 && (CNT(op) & 0xf0))
		TGT_X(op) = (px < 177) ? 176 : (px >= 272) ? 272 : (px & 0xff0);
	op->act3++;
}

static void	Shd_BounceFly(OBJECT *op)
{
	int	a, v;

	a = ratan2(RY(TGT_Y(op)) - POS_Y(op), RX(TGT_X(op)) - POS_X(op));
	v = op->act4 / 2 + 6;
	op->spd_x = (rcos(a) << 4) * v;
	op->spd_y = (rsin(a) << 4) * v;
	op->flip = (op->spd_x >= 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
	MoveObject(op);
	if (ABS(TGT_X(op) - (POS_X(op) - CAM_X)) >= 6 || ABS(TGT_Y(op) - (POS_Y(op) - CAM_Y)) >= 6)
		return;
	PlaySound(SE_BANK_ENEMY, SE_SHD_BOUNCE, op);
	Rumble(6);
	POS_X(op) = RX(TGT_X(op));
	POS_Y(op) = RY(TGT_Y(op));
	if (++op->act4 >= 8) {
		Shd_SprtSet(op, 0);
		Shd_FacePlayer(op);
		SetAnime(op, 20);
		op->act4 = 0;
		op->act3++;
	} else {
		if (op->act4 == 3)
			SetAnime(op, 0);
		op->act3--;
	}
}

static void	Shd_Bounce0(OBJECT *op)
{
	Shd_FacePlayer(op);
	SetAnime(op, 13);
	Shd_NoBoxes(op);
	op->timer = 0;
	CNT(op) = 0;
	op->act3++;
}

static void	Shd_Bounce1(OBJECT *op)
{
	if (!Shd_WarpFlag(op))
		return;
	PlaySound(SE_BANK_ENEMY, SE_SHD_WARP, op);
	SetAnime(op, 12);
	POS_X(op) = RX(160);
	POS_Y(op) = RY(32);
	Shd_FacePlayer(op);
	op->act3++;
}

static void	Shd_Bounce2(OBJECT *op)
{
	if (op->anime.flag != 1)
		return;
	SetAnime(op, 19);
	Shd_Boxes(op);
	op->act3++;
}

static void	Shd_Bounce3(OBJECT *op)
{
	if (op->anime.flag == 2) {
		PlaySound(SE_BANK_ENEMY, SE_SHD_WARP_OUT, op);
		return;
	}
	if (op->anime.flag != 1)
		return;
	Shd_SprtSet(op, 1);
	SetAnime(op, 1);
	op->timer = (POS_X(&mega) > RX(160)) ? 0 : 1;
	CNT(op) = (GetRNG() & 1) ? 0 : 0x0f;
	if (!(GetRNG() & 1))
		CNT(op) |= 0xf0;
	op->act4 = 0;
	op->act3++;
}

static void	Shd_Bounce6(OBJECT *op)
{
	if (op->anime.flag == 1)
		Shd_Think(op);
}

/* ================================================================ */
/*	step 4: shield throw  0x800F4F9C, 0x800F83C8 ..			*/
/* ================================================================ */
static void	Shd_Throw0(OBJECT *op)
{
	SetAnime(op, 13);
	Shd_NoBoxes(op);
	op->act3++;
}

static void	Shd_Throw1(OBJECT *op)
{
	if (!Shd_WarpFlag(op))
		return;
	PlaySound(SE_BANK_ENEMY, SE_SHD_WARP, op);
	SetAnime(op, 12);
	if (POS_X(&mega) < RX(160)) {
		op->flip = OBJ_FLIP_L;
		POS_X(op) = RX(272);
	} else {
		op->flip = OBJ_FLIP_R;
		POS_X(op) = RX(48);
	}
	POS_Y(op) = RY(144);
	op->act3++;
}

static void	Shd_Throw2(OBJECT *op)
{
	if (op->anime.flag != 1)
		return;
	SetAnime(op, 21);
	Shd_Boxes(op);
	op->act4 = 0;
	op->act3++;
}

static void	Shd_Throw3(OBJECT *op)
{
	if (op->anime.flag == 2) {
		Shd_Voice(op, SE_SHD_VOICE_A);
		PlaySound(SE_BANK_ENEMY, SE_SHD_THROW, op);
		Shd_Shot(op, 0);
	} else if (op->anime.flag == 1) {
		SetAnime(op, 22);
		op->act3++;
	}
}

static void	Shd_Throw4(OBJECT *op)
{
	if (op->anime.flag == 2) {
		Shd_Voice(op, SE_SHD_VOICE_A);
		PlaySound(SE_BANK_ENEMY, SE_SHD_THROW, op);
		Shd_Shot(op, 1);
		Shd_Open(op);
	} else if (op->anime.flag == 1) {
		SetAnime(op, 10);
		op->act3++;
	}
}

static void	Shd_Throw5(OBJECT *op)
{
	if (op->act4 == 1) {
		PlaySound(SE_BANK_ENEMY, SE_SHD_WARP, op);
		SetAnime(op, 17);
		Shd_NoBoxes(op);
		op->act3 = 6;
		op->act4 = 0;
	} else if (op->act4 == 2) {
		op->act3 = 8;
		op->act4 = 0;
	}
}

static void	Shd_Throw6(OBJECT *op)
{
	if (op->anime.flag == 1) {
		SetAnime(op, 16);
		if (op->flip == OBJ_FLIP_L) {
			op->flip = OBJ_FLIP_R;
			POS_X(op) = RX(64);
		} else {
			op->flip = OBJ_FLIP_L;
			POS_X(op) = RX(256);
		}
		POS_Y(op) = RY(64);
		op->act3++;
	} else if (op->anime.flag == 2)
		PlaySound(SE_BANK_ENEMY, SE_SHD_WARP, op);
}

static void	Shd_Throw7(OBJECT *op)
{
	if (op->anime.flag == 1) {
		PlaySound(SE_BANK_ENEMY, SE_SHD_CATCH, op);
		SetAnime(op, 8);
		Shd_Guard(op);
		Shd_Boxes(op);
		op->act3 = 8;
	} else if (op->anime.flag == 2)
		PlaySound(SE_BANK_ENEMY, SE_SHD_WARP, op);
}

static void	Shd_Throw8(OBJECT *op)
{
	if (op->act4 == 3) {
		PlaySound(SE_BANK_ENEMY, SE_SHD_CATCH, op);
		SetAnime(op, 8);
		Shd_Guard(op);
		op->act4 = 0;
	} else if (op->act4 == 4) {
		PlaySound(SE_BANK_ENEMY, SE_SHD_CATCH, op);
		SetAnime(op, 6);
		Shd_Guard(op);
		op->timer = 10;
		op->act4 = 0;
		op->act3++;
	}
}

static void	Shd_Wait(OBJECT *op)		/* 0x800F8848, 0x800F903C */
{
	if (--op->timer == 0)
		Shd_Think(op);
}

/* ================================================================ */
/*	step 5: plates  0x800F50A0, 0x800F88C0 ..			*/
/* ================================================================ */
static void	Shd_Warp160(OBJECT *op, short y)	/* sub 1 of steps 5, 6, 7 */
{
	if (!Shd_WarpFlag(op))
		return;
	PlaySound(SE_BANK_ENEMY, SE_SHD_WARP, op);
	SetAnime(op, 12);
	Shd_FacePlayer(op);
	op->flip = OBJ_FLIP_R;
	POS_X(op) = RX(160);
	POS_Y(op) = RY(y);
	op->act3++;
}

static void	Shd_Plates1(OBJECT *op)	{ Shd_Warp160(op, 64); }

static void	Shd_Plates2(OBJECT *op)
{
	if (op->anime.flag != 1)
		return;
	PlaySound(SE_BANK_ENEMY, SE_SHD_PLATE_OFF, op);
	SetAnime(op, 23);
	Shd_FacePlayer(op);
	Shd_Boxes(op);
	op->act3++;
}

static void	Shd_Plates3(OBJECT *op)
{
	if (op->anime.flag == 1) {
		Shd_Voice(op, SE_SHD_VOICE_B);
		SetAnime(op, 10);
		Shd_Open(op);
		shd_freeze = 0;
		Shd_Plate(op, 16);
		Shd_Plate(op, 17);
		Shd_Plate(op, 18);
		Shd_Plate(op, 19);
		op->timer = 600;
		op->act4 = 1;
		op->act3++;
	} else if (op->anime.flag == 2)
		PlaySound(SE_BANK_ENEMY, SE_SHD_PLATE_OFF2, op);
}

static void	Shd_Plates4(OBJECT *op)
{
	int	a;

	a = ratan2(POS_Y(op) - POS_Y(&mega), POS_X(&mega) - POS_X(op));
	op->spd_x = rcos(a) << 4;
	op->spd_y = rsin(a) << 4;
	if (ABS(POS_X(&mega) - POS_X(op)) >= 7 || ABS(POS_Y(&mega) - POS_Y(op)) >= 7) {
		op->flip = (op->spd_x >= 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
		if (shd_freeze)
			shd_freeze--;
		else
			MoveObject(op);
	}
	if (--op->timer == 0) {
		SetAnime(op, 6);
		Shd_Guard(op);
		op->act4 = 0;
		op->act3++;
	}
}

static void	Shd_ThinkSub(OBJECT *op)	{ Shd_Think(op); }

/* ================================================================ */
/*	step 6: mirror  0x800F5208, 0x800F8B78 ..			*/
/* ================================================================ */
static void	Shd_Mirror1(OBJECT *op)	{ Shd_Warp160(op, 64); }

static void	Shd_Mirror2(OBJECT *op)
{
	if (op->anime.flag != 1)
		return;
	SetAnime(op, 6);
	Shd_FacePlayer(op);
	Shd_Boxes(op);
	op->timer = 20;
	op->act3++;
}

static void	Shd_Mirror3(OBJECT *op)
{
	if (--op->timer)
		return;
	SetAnime(op, 8);
	op->timer = 20;
	Shd_Image(op, 32);
	op->act4 = 1;
	op->act3++;
}

static void	Shd_Mirror4(OBJECT *op)
{
	if (--op->timer)
		return;
	SetAnime(op, 15);
	Shd_NoBoxes(op);
	op->act4 = 2;
	op->act3++;
}

static void	Shd_Mirror5(OBJECT *op)
{
	short	p;

	if (op->anime.flag == 2) {
		PlaySound(SE_BANK_ENEMY, SE_SHD_WARP, op);
		return;
	}
	if (op->anime.flag != 1)
		return;
	if (POS_X(&mega) <= RX(160)) {
		op->flip = OBJ_FLIP_L;
		p = POS_X(&mega) - 48;
		if (p - CAM_X < 40)
			p = RX(40);
	} else {
		op->flip = OBJ_FLIP_R;
		p = POS_X(&mega) + 48;
		if (p - CAM_X >= 265)
			p = RX(264);
	}
	POS_X(op) = MIRROR_X(p);
	POS_Y(op) = (GetRNG() & 1) ? RY(48) : RY(160);
	SetAnime(op, 14);
	Shd_Boxes(op);
	op->act4 = 3;
	op->act3++;
}

static void	Shd_Mirror6(OBJECT *op)
{
	if (op->anime.flag == 2) {
		PlaySound(SE_BANK_ENEMY, SE_SHD_WARP, op);
		return;
	}
	if (op->anime.flag != 1)
		return;
	if (POS_Y(op) == RY(48)) {
		SetAnime(op, 22);
		op->act3++;
		return;
	}
	Shd_Voice(op, SE_SHD_VOICE_A);
	PlaySound(SE_BANK_ENEMY, SE_SHD_LUNGE, op);
	SetAnime(op, 29);
	op->dmg_tbl = dmg_tbl_sheldon;
	op->atk = 10;
	op->atk_box = &shd_lunge_atk;
	op->dmg_box = &shd_lunge_dmg;
	GUARD(op) = 0;
	op->act3 = 11;
}

static void	Shd_Mirror7(OBJECT *op)
{
	if (op->anime.flag == 2) {
		Shd_Voice(op, SE_SHD_VOICE_A);
		PlaySound(SE_BANK_ENEMY, SE_SHD_THROW, op);
		Shd_ShotPair(op);
		Shd_Open(op);
	} else if (op->anime.flag == 1) {
		SetAnime(op, 10);
		op->act3++;
	}
}

static void	Shd_Mirror8(OBJECT *op)
{
	if (op->act4 != 4)
		return;
	PlaySound(SE_BANK_ENEMY, SE_SHD_CATCH, op);
	op->dmg_tbl = dmg_tbl_guard;
	SetAnime(op, 8);
	GUARD(op) = 1;
	op->timer = 30;
	op->act3++;
}

static void	Shd_Mirror9(OBJECT *op)
{
	if (--op->timer)
		return;
	PlaySound(SE_BANK_ENEMY, SE_SHD_CATCH, op);
	SetAnime(op, 6);
	op->act4 = 0;
	op->timer = 20;
	op->act3++;
}

static void	Shd_Mirror11(OBJECT *op)
{
	if (op->anime.flag != 1)
		return;
	GUARD(op) = 1;
	op->dmg_tbl = dmg_tbl_guard;
	op->atk = 10;
	Shd_Boxes(op);
	op->timer = 30;
	op->act3 = 9;
}

/* ================================================================ */
/*	step 7: turrets  0x800F5378, 0x800F55A0, 0x800F9108 ..		*/
/* ================================================================ */
static void	Shd_Turrets1(OBJECT *op)	{ Shd_Warp160(op, 80); }

static void	Shd_Turrets2(OBJECT *op)
{
	if (op->anime.flag != 1)
		return;
	op->pri = 6;
	SetAnime(op, 30);
	Shd_FacePlayer(op);
	Shd_Boxes(op);
	op->timer = 20;
	op->act3++;
}

static void	Shd_Turrets3(OBJECT *op)
{
	switch (op->anime.flag) {
	case 2:
		Shd_Effect(op, POS_X(op), POS_Y(op), 16);
		break;
	case 3:
		Shd_Voice(op, SE_SHD_VOICE_C);
		op->act4 = 1;
		Shd_Turret(op, 48);
		Shd_Turret(op, 49);
		Shd_Turret(op, 50);
		Shd_Turret(op, 51);
		break;
	case 1:
		op->dmg_tbl = dmg_tbl_sheldon;
		GUARD(op) = 0;
		SetAnime(op, 17);
		Shd_NoBoxes(op);
		CNT(op) = 0;
		op->act3++;
		break;
	}
}

static void	Shd_Turrets4(OBJECT *op)
{
	int	c, t;

	if (op->anime.flag == 2)
		PlaySound(SE_BANK_ENEMY, CNT(op) == 0 ? SE_SHD_WARP : SE_SHD_WARP2, op);
	if (op->anime.flag != 1)
		return;
	op->pri = 4;
	c = GetRNG() & 3;
	op->flip = (c == 1 || c == 2) ? OBJ_FLIP_R : OBJ_FLIP_L;
	POS_X(op) = RX(corner[c][0]);
	POS_Y(op) = RY(corner[c][1]);
	t = (c + 1 + (GetRNG() & 0xff) % 3) & 3;
	TGT_X(op) = corner[t][0];
	TGT_Y(op) = corner[t][1];
	op->timer = 50;
	op->act3++;
}

static void	Shd_Turrets5(OBJECT *op)
{
	if (--op->timer)
		return;
	SetAnime(op, 16);
	op->act3++;
}

static void	Shd_Turrets6(OBJECT *op)
{
	if (op->anime.flag == 1) {
		Shd_Boxes(op);
		op->timer = 0;
		op->act3++;
	} else if (op->anime.flag == 2)
		PlaySound(SE_BANK_ENEMY, SE_SHD_WARP2, op);
}

static void	Shd_Turrets7(OBJECT *op)
{
	int	a;

	a = ratan2(RY(TGT_Y(op)) - POS_Y(op), RX(TGT_X(op)) - POS_X(op));
	op->spd_x = rcos(a) << 6;
	op->spd_y = rsin(a) << 6;
	if (game.level >= LEVEL_HARD) {
		op->spd_x = op->spd_x * 5 / 4;
		op->spd_y = op->spd_y * 5 / 4;
	}
	if (op->spd_y > 0)	SetAnime(op, op->spd_x ? 37 : 39);
	else if (op->spd_y < 0)	SetAnime(op, op->spd_x ? 38 : 40);
	else			SetAnime(op, 37);
	if (op->anime_no == 39 || op->anime_no == 40)
		op->flip = (POS_X(op) - CAM_X < 160) ? OBJ_FLIP_R : OBJ_FLIP_L;
	else
		op->flip = (op->spd_x >= 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
	MoveObject(op);
	if (ABS(TGT_X(op) - (POS_X(op) - CAM_X)) >= 4 || ABS(TGT_Y(op) - (POS_Y(op) - CAM_Y)) >= 4)
		return;
	POS_X(op) = RX(TGT_X(op));
	POS_Y(op) = RY(TGT_Y(op));
	if (++CNT(op) >= 4) {
		SetAnime(op, 6);
		Shd_Guard(op);
		op->act4 = 0;
		op->act3++;
	} else {
		SetAnime(op, 17);
		PlaySound(SE_BANK_ENEMY, SE_SHD_WARP2, op);
		Shd_NoBoxes(op);
		op->act3 = 4;
	}
}

static void	Shd_Turrets8(OBJECT *op)
{
	op->pri = 5;
	Shd_Think(op);
}

/* ================================================================ */
/*	step 8: intro  0x800F5880, 0x800F948C ..			*/
/* ================================================================ */
static void	Shd_Intro0(OBJECT *op)
{
	PlayerEvent(20, OBJ_FLIP_R);
	op->act3++;
}

static void	Shd_Intro1(OBJECT *op)
{
	OBJECT	*ep;

	if (game.message != 0 || mega.act2 != 20)
		return;
	if (game.stage != 12)
		PlayerEvent(29, OBJ_FLIP_R);
	if ((ep = GetSysObject()) != 0) {
		ep->flag = 1;
		ep->type = 24;
		PARENT(op) = ep;
	}
	op->timer = 30;
	op->act3++;
}

static void	Shd_Intro2(OBJECT *op)
{
	if (--op->timer == 0)
		PlayerEvent(20, OBJ_FLIP_R);
	if (PARENT(op)->flag)
		return;
	game.boss_bar = 1;
	game.boss_f = 0;
	game.boss = op;
	BossLevel();
	game.boss_hp = 32 + hp_bonus[game.boss_level];
	POS_X(op) = RX(240);
	POS_Y(op) = RY(144);
	op->old_x = op->x;
	op->old_y = op->y;
	Shd_Effect(op, POS_X(op), POS_Y(op), 0);
	op->act4 = 0;
	op->act3++;
}

static void	Shd_Intro3(OBJECT *op)
{
	if (op->act4 != 1)
		return;
	Shd_SprtSet(op, 1);
	PlaySound(SE_BANK_ENEMY, SE_SHD_APPEAR, op);
	SetAnime(op, 0);
	op->disp = 1;
	op->act4 = 0;
	op->act3++;
}

static void	Shd_Intro4(OBJECT *op)
{
	if (op->anime.flag != 1)
		return;
	SetAnime(op, 1);
	op->act3++;
}

static void	Shd_Intro5(OBJECT *op)
{
	if (op->anime.flag != 1)
		return;
	Shd_SprtSet(op, 0);
	SetAnime(op, 5);
	op->act3++;
}

static void	Shd_Intro6(OBJECT *op)
{
	if (op->anime.flag != 1)
		return;
	if (game.stage != 12) {
		TextOpen(0, 255, game.talk_f);
		game.talk_f = 1;
	}
	op->act3++;
}

static void	Shd_Intro7(OBJECT *op)
{
	if (text_busy)
		return;
	ShowBossBar(0);
	op->act3++;
}

static void	Shd_Intro8(OBJECT *op)
{
	if (BossBarBusy())
		return;
	CNT(op) = 3;
	op->act3++;
}

static void	Shd_Intro9(OBJECT *op)
{
	if (--CNT(op) == 0) {
		PlaySound(SE_BANK_SYS, SE_HP_FILL, 0);
		CNT(op) = 3;
	}
	if ((char)++op->hp == game.boss_hp)
		op->act3++;
}

static void	Shd_Intro10(OBJECT *op)
{
	PlayerEventEnd();
	Shd_Think(op);
}

/* ================================================================ */
/*	step 9: stunned  0x800F9848 ..					*/
/* ================================================================ */
static void	Shd_Stun0(OBJECT *op)
{
	PlaySound(SE_BANK_ENEMY, SE_SHD_STUN, op);
	op->atk = 10;
	Shd_Boxes(op);
	SetAnime(op, 18);
	if (POS_X(&mega) < POS_X(op)) {
		op->flip = OBJ_FLIP_L;
		op->spd_x = PX(12);
	} else {
		op->flip = OBJ_FLIP_R;
		op->spd_x = -PX(12);
	}
	op->acc_x = 0x4ccc;
	op->col_box = &shd_col_box;
	op->spd_y = 0;
	op->grav = 0;
	op->act3++;
}

static void	Shd_Stun1(OBJECT *op)
{
	MoveObject2(op);
	if (ABS(op->spd_x) <= PX(4))
		op->acc_x = 0;
	CollideCheck(op);
	if (!(op->col_f & (op->flip == OBJ_FLIP_L ? COL_R : COL_L)))
		return;
	PlaySound(SE_BANK_ENEMY, SE_SHD_BOUNCE, op);
	Rumble(6);
	Quake(10, 4, 2);
	op->timer = 40;
	op->col_box = 0;
	op->act3++;
}

static void	Shd_Stun2(OBJECT *op)
{
	if (--op->timer == 0)
		op->act3++;
}

/* ================================================================ */
/*	step 2: warp guard (no writer)  0x800F7DCC ..			*/
/* ================================================================ */
static void	Shd_WGuard0(OBJECT *op)
{
	SetAnime(op, 13);
	op->timer = 40;
	Shd_NoBoxes(op);
	CNT(op) = 0;
	op->dmg_tbl = dmg_tbl_sheldon_guard;
	GUARD(op) = 0;
	op->act3++;
}

static void	Shd_WGuardIn(OBJECT *op, short ref)
{
	if (!Shd_WarpFlag(op))
		return;
	PlaySound(SE_BANK_ENEMY, SE_SHD_WARP, op);
	SetAnime(op, 12);
	POS_X(op) = (ref < RX(160)) ? RX(256) : RX(64);
	POS_Y(op) = RY(80);
	Shd_FacePlayer(op);
	op->act3 = 2;
}

static void	Shd_WGuard1(OBJECT *op)	{ Shd_WGuardIn(op, POS_X(&mega)); }
static void	Shd_WGuard4(OBJECT *op)	{ Shd_WGuardIn(op, POS_X(op)); }

static void	Shd_WGuard2(OBJECT *op)
{
	if (op->anime.flag != 1)
		return;
	SetAnime(op, 6);
	Shd_Boxes(op);
	op->act3++;
	if (CNT(op) >= 2)
		Shd_Think(op);
}

static void	Shd_WGuard3(OBJECT *op)
{
	if (BLOCKED(op) == 1) {
		SetAnime(op, 13);
		Shd_NoBoxes(op);
		CNT(op)++;
		op->act3++;
		return;
	}
	if (--op->timer == 0) {
		Shd_Guard(op);
		Shd_Think(op);
	}
}

/* ================================================================ */
/*	THINK  0x800F5D58						*/
/* ================================================================ */
static void	Shd_Think(OBJECT *op)
{
	u_char	(*tbl)[8];
	u_char	s;

	op->act3 = 0;
	if (PAT_MODE(op) == 0 && op->hp <= game.boss_hp / 2) {
		PAT_NO(op) = GetRNG() & 7;
		PAT_MODE(op) = 1;
		op->act2 = pat_low[(int)PAT_NO(op)][0];
		PAT_POS(op) = 1;
		return;
	}
	switch (PAT_MODE(op)) {
	case 0:
	case 1:
		tbl = PAT_MODE(op) ? pat_low : pat_high;
		s = tbl[(int)PAT_NO(op)][(int)PAT_POS(op)];
		if (s == 0xff) {
			PAT_NO(op) = GetRNG() & 15;
			PAT_POS(op) = 0;
			s = tbl[(int)PAT_NO(op)][0];
		}
		op->act2 = s;
		PAT_POS(op)++;
		break;
	case -1:
		PAT_NO(op) = GetRNG() & 15;
		PAT_POS(op) = 1;
		PAT_MODE(op) = 0;
		op->act2 = pat_high[(int)PAT_NO(op)][0];
		break;
	}
}

/* ================================================================ */
/*	sprite sets, spawns  0x800F5F04 .. 0x800F77A0			*/
/* ================================================================ */
static void	Shd_SprtSet(OBJECT *op, int set)
{
	int	bank;

	op->tpage = set ? 0x540 : 0x140;
	if (game.stage == 12) {
		op->clut = set ? 0x794c : 0x7948;
		op->sprt_p = (char *)stage_sprt_bank + stage_sprt_bank->ofs[set];
		return;
	}
	bank = GetSpriteBank(op->type) + set;
	op->clut = ((bank * 4 + 32) & 15) | ((((GetSpriteBank(op->type) + 8 + set) / 4) + 480) << 6);
	op->sprt_p = (char *)sprt_bank + sprt_bank[bank].ofs[0];
}

static void	Shd_Shot(OBJECT *op, int var)		/* 0x800F6F44 */
{
	OBJECT	*sp;
	int	set1 = ((var & 0xf0) == 0x00);

	if ((sp = GetVisualObject()) == 0)
		return;
	sp->flag = 0x41;
	sp->type = 16;
	sp->var = var;
	sp->flip = op->flip;
	sp->anime_tbl = op->anime_tbl;
	sp->layer = op->layer;
	sp->x.l = op->x.l & 0xffff0000;
	sp->y.l = op->y.l & 0xffff0000;
	WP(sp, 0x7c) = op;
	if (set1)
		POS_X(sp) += (op->flip == OBJ_FLIP_L) ? -16 : 16;
	Shd_SprtSet(sp, set1);
	if (!set1)
		WP(sp, 0x7c) = PARENT(op);		/* plate and turret shots: the boss */
}

static void	Shd_ShotPair(OBJECT *op)		/* 0x800F71F4 */
{
	OBJECT	*a, *b;

	if ((a = GetVisualObject()) != 0) {
		a->flag = 0x41; a->type = 16; a->var = 32;
		a->flip = op->flip; a->anime_tbl = op->anime_tbl; a->layer = op->layer;
		a->x.l = PX(POS_X(op) + (op->flip == OBJ_FLIP_L ? -16 : 16));
		a->y.l = PX(POS_Y(op));
		WP(a, 0x7c) = op;
		Shd_SprtSet(a, 1);
	}
	if ((b = GetVisualObjectAfter(a, 0)) != 0) {	/* 0x800F77A0 */
		b->flag = 0x41; b->type = 16; b->var = 33;
		b->flip = (op->flip == 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
		b->anime_tbl = op->anime_tbl; b->layer = op->layer;
		b->x.l = PX(MIRROR_X(POS_X(a)));
		b->y.l = PX(POS_Y(a));
		WP(b, 0x7c) = op;
		WP(b, 0x8c) = a;
		Shd_SprtSet(b, 1);
	}
}

static void	Shd_Effect(OBJECT *op, short x, short y, int var)	/* 0x800F74D8 */
{
	OBJECT	*ep;

	if ((ep = GetEffectObject()) == 0)
		return;
	ep->flag = 0x41;
	ep->type = 5;
	ep->var = var;
	ep->flip = op->flip;
	ep->anime_tbl = op->anime_tbl;
	ep->layer = op->layer;
	ep->x.l = PX(x);
	ep->y.l = PX(y);
	WP(ep, 0x50) = (var == 32) ? PARENT(op) : op;
	Shd_SprtSet(ep, 0);
}

static void	Shd_Turret(OBJECT *op, int var)		/* 0x800F765C */
{
	OBJECT	*tp;

	if ((tp = GetMainObject()) == 0)
		return;
	tp->flag = 0x41;
	tp->type = 51;
	tp->flip = OBJ_FLIP_L;
	tp->var = var;
	tp->anime_tbl = op->anime_tbl;
	tp->layer = op->layer;
	POS_X(tp) = POS_X(op);
	POS_Y(tp) = POS_Y(op);
	Shd_SprtSet(tp, 1);
	PARENT(tp) = op;
}

/* ================================================================ */
/*	entry  0x800F78AC						*/
/* ================================================================ */
static void	Shd_DeadStep(OBJECT *op);
static void	Shd_PlateMain(OBJECT *op);
static void	Shd_ImageMain(OBJECT *op);
static void	Shd_TurretMain(OBJECT *op);
static void	Shd_Fall(OBJECT *op);
static void	Shd_Delete(OBJECT *op)	{ DeleteObject3(op); }

static void	(*shd_act_tbl[])(OBJECT *) = {	/* 0x800FCC60 */
	Shd_Init, Shd_Main, Shd_DeadStep, Shd_PlateMain,
	Shd_ImageMain, Shd_TurretMain, Shd_Fall, Shd_Delete
};

void	M51_Sheldon(OBJECT *op)
{
	if (op->var != 0 && op->act != 6 && PARENT(op)->act2 == 9) {
		op->act = 6;
		op->act2 = 0;
	}
	(*shd_act_tbl[(int)op->act])(op);
}

static void	(*shd_dead_tbl[])(OBJECT *) = { Shd_Dead0, Shd_Dead1, Shd_Dead2 };	/* 0x800FCCA8 */

static void	Shd_DeadStep(OBJECT *op)
{
	(*shd_dead_tbl[(int)op->act2])(op);
}

/* children: plates 0x800F5990, image 0x800F7AE8, turrets 0x800F7BF0, fall 0x800F7CD4 */
static int	Shd_ChildAlive(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);

	if (pp->act == 1 && pp->act4 != 0)
		return 1;
	op->act = 7;
	op->disp = 0;
	return 0;
}

static void	Shd_PlateMain(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);
	static signed char ofs[4][2] = { { 0, -38 }, { -38, 0 }, { 0, 38 }, { 38, 0 } };

	if (!Shd_ChildAlive(op))
		return;
	op->old_x = op->x;
	op->old_y = op->y;
	POS_X(op) = POS_X(pp) + ofs[op->var - 16][0];
	POS_Y(op) = POS_Y(pp) + ofs[op->var - 16][1];
	AnimeTick(op);
	switch (op->act2) {
	case 2:
		if (HIT_F(op) == 1) {
			SetAnime(op, 26);
			op->timer = 30;
			op->atk_box = op->dmg_box = &plate_open_box;
			op->act2++;
		}
		break;
	case 3:
		if (--op->timer == 0) {
			shd_freeze = 8;
			SetAnime(op, 27);
			op->act2++;
		}
		break;
	case 4:
		if (op->anime.flag == 1) {
			Shd_Shot(op, op->var);
			op->timer = 30;
			op->act2++;
		}
		break;
	case 5:
		if (--op->timer == 0) {
			if (op->var & 1) {
				SetAnime(op, 25);
				op->atk_box = &plate_h_box[0]; op->dmg_box = &plate_h_box[1];
			} else {
				SetAnime(op, 24);
				op->atk_box = &plate_v_box[0]; op->dmg_box = &plate_v_box[1];
			}
			op->act2 = 2;
		}
		break;
	}
	HIT_F(op) = 0;
	if (ContactWeapon(op))		/* dmg_tbl is 0: reads RAM 0 */
		HIT_F(op) = 1;
	ContactMega(op);
}

static void	Shd_ImageMain(OBJECT *op)
{
	OBJECT	*pp = PARENT(op);

	if (pp->act != 1 || pp->act4 == 0) {
		op->act = 7;
		op->disp = 0;
		return;
	}
	op->old_x = op->x;
	op->old_y = op->y;
	op->flip = (pp->flip == 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
	op->anime.sprt = pp->anime.sprt;
	op->disp = pp->disp;
	op->atk_box = pp->atk_box;
	op->dmg_box = pp->dmg_box;
	switch (op->act2) {
	case 2:
		if (pp->act4 == 2)
			op->act2++;
		break;
	case 3:
		if (pp->act4 != 3)
			break;
		op->act2++;
		/* fall through */
	case 4:
		POS_X(op) = MIRROR_X(POS_X(pp));
		POS_Y(op) = POS_Y(pp);
		break;
	}
	op->disp = (++op->timer & 1) ? 0 : 1;
	ContactWeapon(op);
	ContactMega(op);
}

static void	Shd_TurretMain(OBJECT *op)
{
	int	a, i;

	if (PARENT(op)->act != 1 || PARENT(op)->act4 == 0) {
		op->act = 7;
		op->disp = 0;
		return;
	}
	op->old_x = op->x;
	op->old_y = op->y;
	AnimeTick(op);
	i = op->var - 48;
	switch (op->act2) {
	case 2:
		a = ratan2(RY(corner[i][1]) - POS_Y(op), RX(corner[i][0]) - POS_X(op));
		op->spd_x = rcos(a) * 96;		/* 6 px/frame */
		op->spd_y = rsin(a) * 96;
		op->flip = (op->spd_x >= 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
		MoveObject(op);
		if (ABS(corner[i][0] - (POS_X(op) - CAM_X)) < 7
		 && ABS(corner[i][1] - (POS_Y(op) - CAM_Y)) < 7) {
			POS_X(op) = RX(corner[i][0]);
			POS_Y(op) = RY(corner[i][1]);
			SetAnime(op, (i < 2) ? 35 : 36);
			op->flip = (i == 1 || i == 2) ? OBJ_FLIP_R : OBJ_FLIP_L;
			op->act2++;
		}
		break;
	case 3:
		if (HIT_F(op) == 1) {
			Shd_Effect(op, POS_X(op), POS_Y(op), 32);
			FLASH_F(op) = 1;
			op->timer = 7;
			op->act2++;
		}
		break;
	case 4:
		FLASH_F(op) ^= 1;
		if (--op->timer == 0) {
			FLASH_F(op) = 0;
			Shd_Shot(op, op->var);
			op->act2--;
		}
		break;
	}
	HIT_F(op) = 0;
	if (ContactWeapon(op))
		HIT_F(op) = 1;
	ContactMega(op);
	if (FLASH_F(op) == 1)	op->clut |= CLUT_FLASH;
	else			op->clut &= ~CLUT_FLASH;
}

static void	Shd_Fall(OBJECT *op)
{
	if (op->act2 == 0) {
		op->act2++;
		op->timer = 0;
		op->spd_x = op->acc_x = op->spd_y = 0;
		op->grav = 0x4200;
	}
	MoveObject2(op);
	if (OffScreenCheck2(op, 64, 64)) {
		op->act = 7;
		return;
	}
	op->disp = (op->timer++ & 1) ? 1 : 0;
}

static void	Shd_FacePlayer(OBJECT *op)		/* 0x800F9E04 */
{
	op->flip = (mega.x.l < op->x.l) ? OBJ_FLIP_L : OBJ_FLIP_R;
}

static void	Shd_Child(OBJECT *op, int var, int opposite)	/* 0x800F9E3C, 0x800F9EE4 */
{
	OBJECT	*cp;

	if ((cp = GetMainObject()) == 0)
		return;
	cp->flag = 0x41;
	cp->type = 51;
	cp->flip = opposite ? (op->flip ? OBJ_FLIP_L : OBJ_FLIP_R) : OBJ_FLIP_L;
	cp->var = var;
	cp->clut = op->clut & ~CLUT_FLASH;
	cp->anime_tbl = op->anime_tbl;
	cp->sprt_p = op->sprt_p;
	cp->tpage = op->tpage;
	cp->layer = op->layer;
	POS_X(cp) = POS_X(op);
	POS_Y(cp) = POS_Y(op);
	PARENT(cp) = op;
}

static void	Shd_Plate(OBJECT *op, int var)	{ Shd_Child(op, var, 0); }
static void	Shd_Image(OBJECT *op, int var)	{ Shd_Child(op, var, 1); }

static void	Shd_Soul(int var)			/* 0x800F9FA4 */
{
	OBJECT	*vp;

	if ((vp = GetMainObject()) != 0) {
		vp->flag = 0x41;
		vp->type = 15;
		vp->var = var;
	}
}

/* ---- step tables ---- */
static void	Shd_Nop(OBJECT *op)	{ }

static void	(*shd_s2[])(OBJECT *) = { Shd_WGuard0, Shd_WGuard1, Shd_WGuard2, Shd_WGuard3, Shd_WGuard4 };
static void	(*shd_s3[])(OBJECT *) = { Shd_Bounce0, Shd_Bounce1, Shd_Bounce2, Shd_Bounce3,
					  Shd_BounceAim, Shd_BounceFly, Shd_Bounce6 };
static void	(*shd_s4[])(OBJECT *) = { Shd_Throw0, Shd_Throw1, Shd_Throw2, Shd_Throw3, Shd_Throw4,
					  Shd_Throw5, Shd_Throw6, Shd_Throw7, Shd_Throw8, Shd_Wait };
static void	(*shd_s5[])(OBJECT *) = { Shd_Throw0, Shd_Plates1, Shd_Plates2, Shd_Plates3,
					  Shd_Plates4, Shd_ThinkSub };
static void	(*shd_s6[])(OBJECT *) = { Shd_Throw0, Shd_Mirror1, Shd_Mirror2, Shd_Mirror3,
					  Shd_Mirror4, Shd_Mirror5, Shd_Mirror6, Shd_Mirror7,
					  Shd_Mirror8, Shd_Mirror9, Shd_Wait, Shd_Mirror11 };
static void	(*shd_s7[])(OBJECT *) = { Shd_Throw0, Shd_Turrets1, Shd_Turrets2, Shd_Turrets3,
					  Shd_Turrets4, Shd_Turrets5, Shd_Turrets6, Shd_Turrets7,
					  Shd_Turrets8 };
static void	(*shd_s8[])(OBJECT *) = { Shd_Intro0, Shd_Intro1, Shd_Intro2, Shd_Intro3, Shd_Intro4,
					  Shd_Intro5, Shd_Intro6, Shd_Intro7, Shd_Intro8, Shd_Intro9,
					  Shd_Intro10 };
static void	(*shd_s9[])(OBJECT *) = { Shd_Stun0, Shd_Stun1, Shd_Stun2, Shd_ThinkSub };

#define SUBSTEP(name, tbl)	static void name(OBJECT *op) { (*tbl[(int)op->act3])(op); }
SUBSTEP(Shd_Step2, shd_s2)
SUBSTEP(Shd_Step3, shd_s3)
SUBSTEP(Shd_Step4, shd_s4)
SUBSTEP(Shd_Step5, shd_s5)
SUBSTEP(Shd_Step6, shd_s6)
SUBSTEP(Shd_Step7, shd_s7)
SUBSTEP(Shd_Step8, shd_s8)
SUBSTEP(Shd_Step9, shd_s9)

static void	(*shd_step_tbl[])(OBJECT *) = {
	ReactMode, Shd_Nop, Shd_Step2, Shd_Step3, Shd_Step4,
	Shd_Step5, Shd_Step6, Shd_Step7, Shd_Step8, Shd_Step9
};

/* The visual object 16 (shields, shots) and the effect object 5 are in v16_sheldon.c. */
