/*
 *	m56_metal_shark.c  --  Metal Shark Player (stage 4 area 1 / overlay 07, main object 56)
 *
 *	var 0   the boss
 *	var 1   side kick "beam"   (player Zero)
 *	var 2   side kick "sparks" (player X)
 *	var 3   side kick "bees"   (Hard, 1/2)
 *	visual object 22: anchor, debris, pieces, beam, sparks, bees (v22_metal_shark.c)
 *
 *	Plausible reconstruction, not original code, not compiled.
 */
#include "object.h"
#include "game.h"
#include "sound.h"

#define ROOM_X		(bg_layer[0].dest_min_x)
#define ROOM_Y		(bg_layer[0].dest_min_y)
#define RX(n)		(ROOM_X + (n))
#define RY(n)		(ROOM_Y + (n))
#define ABS(a)		((a) < 0 ? -(a) : (a))
#define PX(n)		((long)(n) << 16)

#define W8(o, n)	(*(char *)((u_char *)(o) + (n)))
#define W16(o, n)	(*(short *)((u_char *)(o) + (n)))
#define WP(o, n)	(*(OBJECT **)((u_char *)(o) + (n)))

#define TIMER(o)	((o)->timer)		/* +0x7C */
#define CNT(o)		W16(o, 0x7e)
#define LINK(o)		WP(o, 0x80)		/* engine object 24/26; kick: the boss */
#define TGT_X(o)	W16(o, 0x84)
#define TGT_Y(o)	W16(o, 0x86)
#define KICK_INV(o)	W16(o, 0x84)
#define KICK_NSHOT(o)	W16(o, 0x86)
#define INV(o)		W16(o, 0x88)
#define PAT_MODE(o)	W8(o, 0x8a)
#define PAT_NO(o)	W8(o, 0x8b)
#define PAT_POS(o)	W8(o, 0x8c)
#define BURIED(o)	W8(o, 0x8d)
#define FIN_F(o)	W8(o, 0x8e)

enum { ST_SETUP, ST_BOSS, ST_DEATH, ST_KICK, ST_DELETE };
enum { ACT_REACT, ACT_NOP, ACT_INTRO, ACT_STUN, ACT_FIN, ACT_ANCHOR,
       ACT_SLAM, ACT_BURROW, ACT_RESURRECT, ACT_POPUP };

/* data in the overlay */
extern HITBOX	box_body0[], box_body1[];	/* 0x800F9088/8C, 0x800F90B0/B4 (-16,-24,32,56) */
extern HITBOX	box_fin[];			/* 0x800F90A8 (-32,-16,64,32) */
extern HITBOX	box_fin_small[];		/* 0x800F90A0 (-8,-8,16,16) */
extern HITBOX	box_drill0[], box_drill1[];	/* 0x800F9090 (-8,8,16,32), 0x800F9098 (-8,32,16,16) */
extern HITBOX	box_kick_beam[];		/* 0x800F90B8 (-16,-24,32,60) */
extern HITBOX	box_kick_spark[];		/* 0x800F90C0 (-24,-16,48,48) */
extern HITBOX	box_kick_bee[];			/* 0x800F90C8 (-16,-24,32,72) */
extern HITBOX	col_body[];			/* 0x800F907C (0,5,16,36) */
extern HITBOX	col_fin[], col_burrow[];	/* 0x800F9080, 0x800F9084 (0,0,16,4) */
extern ANIME	*shark_anime_tbl[];		/* 0x800F952C */
extern u_char	shark_pat_high[16][8];		/* 0x800F8F7C */
extern u_char	shark_pat_low[16][8];		/* 0x800F8FFC */
extern u_char	*shark_box_tbl[];		/* 0x800E9B1C, by sprite - 10 */
extern long	shark_out_vy, shark_out_grav;	/* 0x800F986C, 0x800F9870: always 0 */

short	shark_pieces;		/* 0x800F9868 */
short	shark_bees;		/* 0x800F986A */
u_long	shark_fight_time;	/* 0x800F9874 */

static const char	hp_add[4]   = { 0, 0, 4, 8 };		/* 0x800E9A6C */
static const char	kick_cnt[4] = { 2, 3, 4, 4 };		/* 0x800E9A70 */
static const short	bee_time[4] = { 120, 160, 200, 240 };	/* 0x800E9A74 */
static const char	bee_num[4]  = { 3, 5, 6, 10 };		/* 0x800E9A7C */

/* v22_metal_shark.c */
extern void	SharkDebris(OBJECT *op, int var, int x, int y);		/* 0x800F6880 */
extern void	SharkPiece(OBJECT *op, int var, int angle);		/* 0x800F6948 */
extern void	SharkShot(OBJECT *op, int var);				/* 0x800F6A1C */
extern void	SharkBeamShot(OBJECT *op, int var);			/* 0x800F6AD0 */
extern void	SharkSpark(OBJECT *op, int angle, int x, int y);	/* 0x800F6C70 */
extern void	SharkBee(OBJECT *op, int angle);			/* 0x800F6D4C */

static void	SharkThink(OBJECT *op);

static void FacePlayer(OBJECT *op)				/* 0x800F6E48 */
{
	op->flip = (POS_X(op) < POS_X(&mega)) ? OBJ_FLIP_R : OBJ_FLIP_L;
}

static void SharkSprtSet1(OBJECT *op)				/* 0x800F4144 */
{
	int	bank;

	op->tpage = 0x540;
	if (game.stage == 12) {
		op->clut = 0x794c;
		op->sprt_p = (char *)stage_sprt_bank + stage_sprt_bank->ofs[1];
		return;
	}
	bank = GetSpriteBank(op->type) + 1;
	op->clut = ((bank * 4 + 32) & 15) | ((((GetSpriteBank(op->type) + 9) >> 2) + 480) << 6);
	op->sprt_p = (char *)sprt_bank + sprt_bank[GetSpriteBank(op->type)].ofs[1];
}

static void SharkSprtSet0(OBJECT *op)				/* 0x800F6E78 */
{
	int	bank;

	op->tpage = 0x140;
	if (game.stage == 12) {
		op->clut = 0x7948;
		op->sprt_p = (char *)stage_sprt_bank + stage_sprt_bank->ofs[0];
		return;
	}
	bank = GetSpriteBank(op->type);
	op->clut = ((bank * 4 + 32) & 15) | ((((bank + 8) >> 2) + 480) << 6);
	op->sprt_p = (char *)sprt_bank + sprt_bank[bank].ofs[0];
}

static void SharkKick(OBJECT *op, int var)			/* 0x800F67D0 */
{
	OBJECT	*kp = GetMainObject();

	if (kp == NULL)
		return;
	kp->flag = 0x41;
	kp->type = 56;
	LINK(kp) = op;
	kp->var = var;
	kp->flip = op->flip;
	kp->x.l = PX(POS_X(op));
	kp->y.l = PX(POS_Y(op));
	kp->clut = op->clut & 0x7fff;
	kp->anime_tbl = op->anime_tbl;
	kp->sprt_p = op->sprt_p;
	kp->tpage = op->tpage;
	kp->layer = op->layer;
}

static void SharkSoul(int var)					/* 0x800F6F60 */
{
	OBJECT	*sp = GetMainObject();

	if (sp == NULL)
		return;
	sp->flag = 0x41;
	sp->type = 15;
	sp->var = var;
}

/* ---- state 0 ---- */

static void SharkSetup(OBJECT *op)				/* 0x800F1F80 */
{
	if (game.stage == 12) {
		op->clut = 0x7948;
		op->sprt_p = (char *)stage_sprt_bank + stage_sprt_bank->ofs[0];
	}
	op->dmg_tbl = dmg_tbl_shark;
	op->hp = 0;
	op->atk = 4;
	op->muteki = 0;
	op->disp = 0;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
	op->fall_f = 0;
	op->anime_tbl = shark_anime_tbl;
	op->pri = 5;
	op->atk_box = op->dmg_box = op->col_box = NULL;
	op->layer = mega.layer;
	SetAnime(op, 2);

	switch (op->var) {
	case 0:
		op->tpage = 0x140;
		PAT_MODE(op) = -1;
		op->flip = OBJ_FLIP_L;
		PAT_NO(op) = PAT_POS(op) = BURIED(op) = FIN_F(op) = 0;
		shark_fight_time = 0;
		op->atk_box = box_body0;
		op->dmg_box = box_body0 + 1;
		shark_pieces = shark_bees = 0;
		op->act = ST_BOSS;
		op->act2 = ACT_INTRO;
		break;
	case 1: case 2: case 3:
		op->hp = 24;
		op->pri = 6;
		op->disp = 0;
		op->act = ST_KICK;
		op->act2 = op->var + 1;
		break;
	}
	op->act3 = 0;
	op->act4 = 0;
}

/* ---- boss ---- */

static void SharkBoxBySprite(OBJECT *op)			/* 0x800F66FC */
{
	int	s;

	if (op->disp == 0) {
		op->atk_box = op->dmg_box = NULL;
		return;
	}
	s = op->anime.sprt;
	if (s >= 10 && s < 105) {
		op->atk_box = (HITBOX *)shark_box_tbl[s - 10];
		op->dmg_box = op->atk_box + 1;
	} else {
		op->atk_box = box_body0;
		op->dmg_box = box_body0 + 1;
	}
}

static void (*shark_acts[])(OBJECT *);			/* 0x800F95E0 */

static void SharkMain(OBJECT *op)				/* 0x800F213C */
{
	int	r, stun;

	AnimeTick(op);
	shark_acts[op->act2](op);
	SharkBoxBySprite(op);
	if (op->act2 != ACT_INTRO && op->var == 0)
		shark_fight_time++;

	r = ContactWeapon(op);
	if (r < 0) {
		mega.nohit_f = 1;
		op->act = ST_DEATH;
		op->act2 = 0;
		return;
	}
	if (r > 0 && r != 127) {
		op->dmg_tbl = dmg_tbl_guard;
		INV(op) = (r == 25 || r == 26) ? 2 : 30;
		stun = (r == 7 || r == 16 || r == 43);		/* ids 6, 15, 42 */
		if (op->act2 == ACT_RESURRECT) {
			if (stun)
				INV(op) = 120;
		} else if (op->act2 == ACT_BURROW) {
			if (stun)
				INV(op) = 120;
			else if (BURIED(op) == 1 && r == 36) {	/* id 35 */
				INV(op) = 10000;
				op->act2 = ACT_POPUP;
				op->act3 = 0;
			}
		} else if (stun) {
			INV(op) = 10000;
			op->act2 = ACT_STUN;
			op->act3 = 0;
		}
	}
	if (INV(op) != 0) {
		INV(op)--;
		if ((INV(op) + 1) & 2)
			op->clut |= CLUT_FLASH;
		else
			op->clut &= ~CLUT_FLASH;
		if (INV(op) == 0) {
			op->dmg_tbl = dmg_tbl_shark;
			op->clut &= ~CLUT_FLASH;
		}
	}
	ContactMega(op);
}

static void SharkNop(OBJECT *op)				/* 0x800F5C94 */
{
}

static void SharkThink(OBJECT *op)				/* 0x800F33F8 */
{
	u_char	(*tbl)[8];
	int	s;

	op->act3 = 0;
	op->atk = 4;
	if (PAT_MODE(op) == 0 && op->hp <= game.boss_hp / 2) {
		PAT_NO(op) = GetRNG() & 15;
		PAT_POS(op) = 1;
		PAT_MODE(op) = 1;
		op->act2 = shark_pat_low[PAT_NO(op)][0];
		return;
	}
	if (PAT_MODE(op) == -1) {
		PAT_NO(op) = GetRNG() & 15;
		PAT_POS(op) = 1;
		PAT_MODE(op) = 0;
		op->act2 = shark_pat_high[PAT_NO(op)][0];
		return;
	}
	if (PAT_MODE(op) != 0 && PAT_MODE(op) != 1)
		return;
	tbl = PAT_MODE(op) ? shark_pat_low : shark_pat_high;
	s = tbl[PAT_NO(op)][PAT_POS(op)];
	if (s == 0xff) {
		PAT_NO(op) = GetRNG() & 15;
		PAT_POS(op) = 0;
		s = tbl[PAT_NO(op)][0];
	}
	op->act2 = s;
	PAT_POS(op)++;
}

/* flying to a point: SLAM 0x800F25F0, BURROW 0x800F2738, RESURRECT 0x800F2C38 */
static int SharkFlyTo(OBJECT *op, int shift48, int near)
{
	int	a = ratan2(POS_Y(op) - TGT_Y(op), TGT_X(op) - POS_X(op));

	if (shift48) {
		op->spd_x = rcos(a) * 96;
		op->spd_y = rsin(a) * 96;
	} else {
		op->spd_x = rcos(a) << 7;
		op->spd_y = rsin(a) << 7;
	}
	op->flip = (op->spd_x >= 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
	MoveObject(op);
	return ABS(TGT_X(op) - POS_X(op)) < near && ABS(TGT_Y(op) - POS_Y(op)) < near;
}

static int SharkBlocked(OBJECT *op)
{
	if (op->col_f & COL_U)
		return 1;
	if (op->flip == OBJ_FLIP_L)
		return (op->col_f & COL_L) != 0;
	return (op->col_f & COL_R) != 0;
}

/* passed the player's x, or a wall in front */
static int SharkPassed(OBJECT *op)
{
	if (op->flip == OBJ_FLIP_L)
		return !(POS_X(&mega) < POS_X(op)) || (op->col_f & COL_L);
	return !(POS_X(op) < POS_X(&mega)) || (op->col_f & COL_R);
}

static void SharkJump(OBJECT *op, int h, int t)
{
	op->grav = PX(h) / (t * t / 2);
	op->spd_x = op->acc_x = 0;
	op->spd_y = op->grav * t;
}

/* -- INTRO -- */
static void SharkIntro0(OBJECT *op)				/* 0x800F44CC */
{
	PlayerEvent(20, 64);
	op->act3++;
}

static void SharkIntro1(OBJECT *op)				/* 0x800F4504 */
{
	OBJECT	*ep;

	if (game.message != 0 || mega.act2 != 20)
		return;
	if (game.stage != 12)
		PlayerEvent(29, 64);
	if ((ep = GetSysObject()) != NULL) {
		ep->flag = 1;
		ep->type = 24;
		LINK(op) = ep;
	}
	TIMER(op) = 20;
	op->act3++;
}

static void SharkIntro2(OBJECT *op)				/* 0x800F45A0 */
{
	if (--TIMER(op) == 0)
		PlayerEvent(20, 64);
	if (LINK(op)->flag != 0)
		return;
	game.boss_bar = 1;
	game.boss_f = 0;
	game.boss = op;
	BossLevel();
	game.boss_hp = 48 + hp_add[(u_char)game.boss_level];
	TIMER(op) = 120;
	op->act4 = 0;
	op->act3++;
}

static void SharkIntro3(OBJECT *op)				/* 0x800F2D98 */
{
	TIMER(op)--;
	if ((TIMER(op) & 3) == 0)
		SharkDebris(op, 16, RX(240), RY(224));
	if ((TIMER(op) & 15) == 0) {
		PlaySound(SE_BANK_ENEMY, SE_MSP_IMPACT, op);
		Rumble(5);
	}
	if (TIMER(op) >= 61)
		return;
	if ((TIMER(op) & 3) == 0)
		Quake2(2, 4, 2);
	if (TIMER(op) != 0)
		return;
	SetAnime(op, 0);
	op->disp = 1;
	op->x.l = PX(RX(240));
	op->y.l = PX(RY(272));
	op->old_x = op->x;
	op->old_y = op->y;
	op->grav = 0xb00000 / 3200;
	op->spd_y = 0x46640;
	op->spd_x = op->acc_x = 0;
	op->act3++;
}

static void SharkIntro4(OBJECT *op)				/* 0x800F4674 */
{
	if (op->spd_y < 0) {
		SetAnime(op, 1);
		op->col_box = col_body;
		op->act3++;
	}
	MoveObject2(op);
}

static void SharkIntro5(OBJECT *op)				/* 0x800F46CC */
{
	if (op->anime.flag == 1) {
		SetAnime(op, 6);
		op->act3++;
	} else if (op->anime.flag == 2)
		PlaySound(SE_BANK_ENEMY, SE_MSP_THROW, op);
}

static void SharkIntro6(OBJECT *op)				/* 0x800F472C */
{
	if (!(op->col_f & COL_D)) {
		MoveObject2(op);
		return;
	}
	PlaySound(SE_BANK_ENEMY, SE_MSP_LAND, op);
	SetAnime(op, 7);
	if (game.stage != 12) {
		TextOpen(0, 255, game.talk_f);
		game.talk_f = 1;
	}
	op->act3++;
}

static void SharkIntro7(OBJECT *op)				/* 0x800F47C8 */
{
	if (text_busy == 0) {
		HpBarShow(0);
		op->act3++;
	}
}

static void SharkIntro8(OBJECT *op)				/* 0x800F480C */
{
	if (!HpBarBusy()) {
		CNT(op) = 3;
		op->act3++;
	}
}

static void SharkIntro9(OBJECT *op)				/* 0x800F4848 */
{
	if (--CNT(op) == 0) {
		PlaySound(SE_BANK_SYS, SE_HP_FILL, NULL);
		CNT(op) = 3;
	}
	if ((char)++op->hp == game.boss_hp)
		op->act3++;
}

static void SharkIntro10(OBJECT *op)				/* 0x800F48CC */
{
	PlayerEventEnd();
	SharkThink(op);
}

static void (*intro_tbl[])(OBJECT *) = {			/* 0x800F96A8 */
	SharkIntro0, SharkIntro1, SharkIntro2, SharkIntro3, SharkIntro4, SharkIntro5,
	SharkIntro6, SharkIntro7, SharkIntro8, SharkIntro9, SharkIntro10,
};

static void SharkIntro(OBJECT *op) { intro_tbl[op->act3](op); }	/* 0x800F4490 */

/* -- STUNNED -- */
static void SharkStun0(OBJECT *op)				/* 0x800F4934 */
{
	StopSound(SE_BANK_ENEMY, SE_MSP_LOOP);
	PlaySound(SE_BANK_ENEMY, SE_MSP_STUN, op);
	PlaySound(SE_BANK_ENEMY, SE_MSP_SHOCK, op);
	SharkSprtSet1(op);
	SetAnime(op, 37);
	op->atk = 4;
	if (FIN_F(op) == 1) {
		FIN_F(op) = 0;
		op->col_box = col_body;
		op->y.w.hi -= 28;
	}
	TIMER(op) = 90;
	op->act3++;
}

static void SharkStun1(OBJECT *op)				/* 0x800F49D8 */
{
	if (--TIMER(op) == 0) {
		op->spd_x = op->acc_x = op->spd_y = 0;
		op->grav = 0x4000;
		op->act3++;
	}
}

static void SharkStun2(OBJECT *op)				/* 0x800F4A18 */
{
	if (!(op->col_f & COL_D)) {
		MoveObject2(op);
		return;
	}
	PlaySound(SE_BANK_ENEMY, SE_MSP_LOOP, op);
	PlaySound(SE_BANK_ENEMY, SE_MSP_LAND, op);
	SharkSprtSet0(op);
	SetAnime(op, 2);
	TIMER(op) = 50;
	op->act3++;
}

static void SharkStun3(OBJECT *op)				/* 0x800F4A9C */
{
	if (--TIMER(op) == 0) {
		INV(op) = 40;
		SharkThink(op);
	}
}

static void (*stun_tbl[])(OBJECT *) = { SharkStun0, SharkStun1, SharkStun2, SharkStun3 };
static void SharkStun(OBJECT *op) { stun_tbl[op->act3](op); }	/* 0x800F48F8 */

/* -- FIN -- */
static void SharkFin0(OBJECT *op)				/* 0x800F4D00 */
{
	SetAnime(op, 13);
	FacePlayer(op);
	SharkJump(op, 70, GetRNG() % 15 + 10);
	MoveObject2(op);
	op->act3++;
}

static void SharkFin1(OBJECT *op)				/* 0x800F4DB0 */
{
	MoveObject2(op);
	if (op->spd_y <= 0) {
		SetAnime(op, 14);
		op->act3++;
	}
}

static void SharkFin2(OBJECT *op)				/* 0x800F4DFC */
{
	MoveObject2(op);
	if (op->col_f & COL_D) {
		SetAnime(op, 38);
		op->act3++;
	}
}

static void SharkFin3(OBJECT *op)				/* 0x800F4E4C */
{
	if (op->anime.flag == 0)
		return;
	PlaySound(SE_BANK_ENEMY, SE_MSP_FIN, op);
	op->spd_x = (op->flip == OBJ_FLIP_L) ? -PX(4) : PX(4);
	op->col_box = col_fin;
	FIN_F(op) = 1;
	TIMER(op) = 50;
	op->acc_x = op->spd_y = op->grav = 0;
	op->y.w.hi += 28;
	op->act3++;
}

static void SharkFin4(OBJECT *op)				/* 0x800F2468 */
{
	MoveObject(op);
	if (SharkPassed(op)) {
		SetAnime(op, 39);
		TIMER(op) = 90;
		op->col_box = col_body;
		FIN_F(op) = 0;
		op->spd_x = op->acc_x = 0;
		op->y.w.hi -= 28;
		op->spd_y = shark_out_vy;
		op->grav = shark_out_grav;
		op->act3++;
		return;
	}
	if ((frame_count & 7) == 0) {
		if (op->flip == OBJ_FLIP_L)
			SharkDebris(op, 21, POS_X(op) - 30, POS_Y(op) + 12);
		else
			SharkDebris(op, 20, POS_X(op) + 30, POS_Y(op) + 12);
	}
	if (--TIMER(op) == 0) {
		TIMER(op) = 50;
		PlaySound(SE_BANK_ENEMY, SE_MSP_FIN, op);
	}
}

static void SharkFin5(OBJECT *op)				/* 0x800F4EE0 */
{
	if (--TIMER(op) == 0)
		SharkThink(op);
}

static void (*fin_tbl[])(OBJECT *) = {				/* 0x800F9614 */
	SharkFin0, SharkFin1, SharkFin2, SharkFin3, SharkFin4, SharkFin5,
};
static void SharkFin(OBJECT *op) { fin_tbl[op->act3](op); }	/* 0x800F4CC4 */

/* -- ANCHOR -- */
static void SharkAnchor0(OBJECT *op)				/* 0x800F4F58 */
{
	PlaySound(SE_BANK_ENEMY, SE_MSP_START, op);
	SetAnime(op, 3);
	FacePlayer(op);
	op->act3++;
}

static void SharkAnchor1(OBJECT *op)				/* 0x800F4FA8 */
{
	int	h;

	if (op->anime.flag != 1)
		return;
	SetAnime(op, 4);
	h = GetRNG() % 20 + 60;
	SharkJump(op, h, GetRNG() % 15 + 10);
	op->act3++;
}

static void SharkAnchor2(OBJECT *op)				/* 0x800F5094 */
{
	MoveObject2(op);
	if (op->spd_y <= 0) {
		FacePlayer(op);
		SetAnime(op, 8);
		op->act3++;
	}
}

static void SharkAnchor3(OBJECT *op)				/* 0x800F50EC */
{
	if (op->anime.flag == 1) {
		PlaySound(SE_BANK_ENEMY, SE_MSP_VOICE_A + GetRNG() % 3, op);
		PlaySound(SE_BANK_ENEMY, SE_MSP_THROW, op);
		SharkShot(op, 0);
	} else if (op->anime.flag == 2) {
		SetAnime(op, 6);
		op->act3++;
	}
}

static void SharkAnchor4(OBJECT *op)				/* 0x800F51F0 */
{
	if (op->col_f & COL_D) {
		PlaySound(SE_BANK_ENEMY, SE_MSP_LAND, op);
		SetAnime(op, 7);
		op->act3++;
	}
	MoveObject2(op);
}

static void SharkAnimeEndThink(OBJECT *op)			/* 0x800F5254, 0x800F5C64 */
{
	if (op->anime.flag == 1)
		SharkThink(op);
}

static void (*anchor_tbl[])(OBJECT *) = {			/* 0x800F962C */
	SharkAnchor0, SharkAnchor1, SharkAnchor2, SharkAnchor3, SharkAnchor4, SharkAnimeEndThink,
};
static void SharkAnchor(OBJECT *op) { anchor_tbl[op->act3](op); }	/* 0x800F4F1C */

/* -- SLAM -- */
static void SharkSlam0(OBJECT *op)				/* 0x800F52C0 */
{
	SetAnime(op, 10);
	FacePlayer(op);
	op->act3++;
}

static void SharkSetTarget(OBJECT *op, int above)		/* 0x800F52FC, 0x800F5648 */
{
	TGT_X(op) = POS_X(&mega);
	TGT_Y(op) = POS_Y(&mega) - above;
	if (TGT_X(op) < RX(40))
		TGT_X(op) = RX(40);
	else if (TGT_X(op) > RX(312))
		TGT_X(op) = RX(312);
	if (TGT_Y(op) < RY(48))
		TGT_Y(op) = RY(48);
}

static void SharkSlam1(OBJECT *op)				/* 0x800F52FC */
{
	if (op->anime.flag != 1)
		return;
	SharkSetTarget(op, 80);
	PlaySound(SE_BANK_ENEMY, SE_MSP_START, op);
	op->act3++;
}

static void SharkSlam2(OBJECT *op)				/* 0x800F25F0 */
{
	if (SharkFlyTo(op, 0, 9) || SharkBlocked(op)) {
		SetAnime(op, 11);
		TIMER(op) = 30;
		op->act3++;
	}
}

static void SharkSlam3(OBJECT *op)				/* 0x800F53C8 */
{
	if (--TIMER(op) != 0)
		return;
	PlaySound(SE_BANK_ENEMY, SE_MSP_SLAM_DROP, op);
	op->atk = 8;
	op->spd_x = 0;
	op->spd_y = -PX(5);
	op->act3++;
}

static void SharkSlam4(OBJECT *op)				/* 0x800F5430 */
{
	if (!(op->col_f & COL_D)) {
		MoveObject(op);
		return;
	}
	PlaySound(SE_BANK_ENEMY, SE_MSP_IMPACT, op);
	Rumble(5);
	Quake2(50, 3, 2);
	SetAnime(op, 12);
	SharkDebris(op, 16, POS_X(op), POS_Y(op) + 32);
	SharkDebris(op, 16, POS_X(op), POS_Y(op) + 32);
	SharkDebris(op, 17, POS_X(op), POS_Y(op) + 32);
	SharkDebris(op, 18, POS_X(op), POS_Y(op) + 32);
	op->act3++;
}

static void SharkSlam5(OBJECT *op)				/* 0x800F5530 */
{
	if (op->anime.flag == 1) {
		op->atk = 4;
		SetAnime(op, 2);
		TIMER(op) = 50;
		op->act3++;
	}
}

static void SharkTimerThink(OBJECT *op)			/* 0x800F5580, 0x800F5934 */
{
	if (--TIMER(op) == 0)
		SharkThink(op);
}

static void (*slam_tbl[])(OBJECT *) = {				/* 0x800F9644 */
	SharkSlam0, SharkSlam1, SharkSlam2, SharkSlam3, SharkSlam4, SharkSlam5, SharkTimerThink,
};
static void SharkSlam(OBJECT *op) { slam_tbl[op->act3](op); }	/* 0x800F5284 */

/* -- BURROW -- */
static void SharkBurrow0(OBJECT *op)				/* 0x800F55F8 */
{
	PlaySound(SE_BANK_ENEMY, SE_MSP_START, op);
	SetAnime(op, 15);
	FacePlayer(op);
	op->act3++;
}

static void SharkBurrow1(OBJECT *op)				/* 0x800F5648 */
{
	if (op->anime.flag == 1) {
		SharkSetTarget(op, 112);
		op->act3++;
	}
}

static void SharkBurrow2(OBJECT *op)				/* 0x800F2738 */
{
	if (SharkFlyTo(op, 0, 9) || SharkBlocked(op)) {
		SetAnime(op, 16);
		op->spd_x = 0;
		op->spd_y = -PX(5);
		op->act3++;
	}
}

static void SharkBurrow3(OBJECT *op)				/* 0x800F56F0 */
{
	if (!(op->col_f & COL_D)) {
		MoveObject(op);
		return;
	}
	PlaySound(SE_BANK_ENEMY, SE_MSP_IMPACT, op);
	Rumble(5);
	SetAnime(op, 32);
	op->spd_x = op->spd_y = 0;
	op->act3++;
}

static void SharkBurrow4(OBJECT *op)				/* 0x800F5764 */
{
	if ((frame_count & 3) == 0)
		SharkDebris(op, 16, POS_X(op), POS_Y(op) + 48);
	if (op->anime.flag != 1)
		return;
	SetAnime(op, 33);
	op->atk = 4;
	CNT(op) = 180;
	op->col_box = col_burrow;
	TIMER(op) = 90;
	op->disp = 0;
	BURIED(op) = 1;
	op->y.w.hi += 32;
	op->act3++;
}

static void SharkEmerge(OBJECT *op, int debris_dy)
{
	PlaySound(SE_BANK_ENEMY, SE_MSP_IMPACT, op);
	Rumble(5);
	SetAnimeFrame(op, 33, 4);
	op->atk = 4;
	op->spd_x = 0;
	op->spd_y = PX(4);
	BURIED(op) = 0;
	op->y.w.hi -= 32;
	TGT_Y(op) = POS_Y(op) - 72;
	SharkDebris(op, 16, POS_X(op), POS_Y(op) + debris_dy);
	SharkDebris(op, 16, POS_X(op), POS_Y(op) + debris_dy);
	SharkDebris(op, 17, POS_X(op), POS_Y(op) + debris_dy);
	SharkDebris(op, 18, POS_X(op), POS_Y(op) + debris_dy);
}

static void SharkBurrow5(OBJECT *op)				/* 0x800F2884 */
{
	if (--CNT(op) == 0) {
		SharkEmerge(op, 48);
		op->act3 += 2;
		return;
	}
	if (--TIMER(op) != 0) {
		MoveObject(op);
		return;
	}
	op->disp = 1;
	FacePlayer(op);
	op->spd_y = 0;
	op->spd_x = (op->flip == OBJ_FLIP_L) ? -PX(5) : PX(5);
	op->act3++;
}

static void SharkBurrow6(OBJECT *op)				/* 0x800F2A04 */
{
	if (--CNT(op) == 0) {
		SharkEmerge(op, 32);
		op->act3++;
		return;
	}
	if (CNT(op) == 27 || CNT(op) == 87)
		PlaySound(SE_BANK_ENEMY, SE_MSP_FIN, op);
	MoveObject(op);
	if (SharkPassed(op)) {
		TIMER(op) = 4;
		op->act3--;
		return;
	}
	if ((frame_count & 3) == 0) {
		if (op->flip == OBJ_FLIP_L)
			SharkDebris(op, 21, POS_X(op) - 8, POS_Y(op) + 8);
		else
			SharkDebris(op, 20, POS_X(op) + 8, POS_Y(op) + 8);
	}
}

static void SharkBurrow7(OBJECT *op)				/* 0x800F5814 */
{
	MoveObject(op);
	if (POS_Y(op) < TGT_Y(op)) {
		op->col_box = col_body;
		SetAnime(op, 4);
		op->spd_x = op->acc_x = op->spd_y = 0;
		op->grav = 4189;
		op->act3++;
	}
}

static void SharkLand7(OBJECT *op)				/* 0x800F5884, 0x800F5C00 */
{
	if (op->col_f & COL_D) {
		PlaySound(SE_BANK_ENEMY, SE_MSP_LAND, op);
		SetAnime(op, 7);
		op->act3++;
	}
	MoveObject2(op);
}

static void SharkBurrow9(OBJECT *op)				/* 0x800F58E8 */
{
	if (op->anime.flag == 1) {
		SetAnime(op, 2);
		TIMER(op) = 20;
		op->act3++;
	}
}

static void (*burrow_tbl[])(OBJECT *) = {			/* 0x800F9660 */
	SharkBurrow0, SharkBurrow1, SharkBurrow2, SharkBurrow3, SharkBurrow4, SharkBurrow5,
	SharkBurrow6, SharkBurrow7, SharkLand7, SharkBurrow9, SharkTimerThink,
};
static void SharkBurrow(OBJECT *op) { burrow_tbl[op->act3](op); }	/* 0x800F55BC */

/* -- RESURRECTION -- */
static void SharkResurrect0(OBJECT *op)				/* 0x800F59AC */
{
	PlaySound(SE_BANK_ENEMY, SE_MSP_START, op);
	SetAnime(op, 3);
	op->flip = (POS_X(op) < RX(176)) ? OBJ_FLIP_R : OBJ_FLIP_L;
	op->act3++;
}

static void SharkResurrect1(OBJECT *op)				/* 0x800F5A1C */
{
	if (op->anime.flag != 1)
		return;
	SetAnime(op, 4);
	TGT_X(op) = (op->flip == OBJ_FLIP_L) ? RX(48) : RX(304);
	TGT_Y(op) = RY(80);
	op->act3++;
}

static void SharkResurrect2(OBJECT *op)				/* 0x800F2C38 */
{
	if (SharkFlyTo(op, 1, 7)) {
		PlaySound(SE_BANK_ENEMY, SE_MSP_CORNER, op);
		SetAnime(op, 17);
		op->flip = (POS_X(op) < RX(176)) ? OBJ_FLIP_R : OBJ_FLIP_L;
		op->act3++;
		return;
	}
	if ((frame_count & 3) == 0) {
		if (op->flip == OBJ_FLIP_L)
			SharkDebris(op, 21, POS_X(op) - 8, POS_Y(op) + 8);
		else
			SharkDebris(op, 20, POS_X(op) + 8, POS_Y(op) + 8);
	}
}

static void SharkResurrect3(OBJECT *op)				/* 0x800F5AA8 */
{
	if (op->anime.flag == 2) {
		PlaySound(SE_BANK_ENEMY, SE_MSP_SUMMON, op);
		return;
	}
	if (op->anime.flag != 1)
		return;
	if (game.level >= LEVEL_HARD && (GetRNG() & 1)) {
		PlaySound(SE_BANK_ENEMY, SE_MSP_CALL_BEE, op);
		SharkKick(op, 3);
	} else if (mega.var == 0) {
		PlaySound(SE_BANK_ENEMY, SE_MSP_CALL_SPARK, op);
		SharkKick(op, 2);
	} else {
		PlaySound(SE_BANK_ENEMY, SE_MSP_CALL_BEAM, op);
		SharkKick(op, 1);
	}
	op->act4 = 1;
	op->act3++;
}

static void SharkResurrect4(OBJECT *op)				/* 0x800F5B9C */
{
	if (op->act4 != 0)
		return;
	StopSound(SE_BANK_ENEMY, SE_MSP_LOOP);
	SetAnime(op, 4);
	op->spd_x = op->acc_x = op->spd_y = 0;
	op->grav = 4189;
	op->act3++;
}

static void (*resurrect_tbl[])(OBJECT *) = {			/* 0x800F968C */
	SharkResurrect0, SharkResurrect1, SharkResurrect2, SharkResurrect3, SharkResurrect4,
	SharkLand7, SharkAnimeEndThink,
};
static void SharkResurrect(OBJECT *op) { resurrect_tbl[op->act3](op); }	/* 0x800F5970 */

/* -- POP-UP -- */
static void SharkPopUp0(OBJECT *op)				/* 0x800F4B14 */
{
	SetAnime(op, 28);
	op->atk = 4;
	op->y.w.hi -= 32;
	op->spd_y = 0x5553c;
	op->grav = 0x500000 / 450;
	op->acc_x = 0;
	if (POS_X(op) < POS_X(&mega)) {
		op->spd_x = (long)0xffe00000 / 30;
		op->flip = OBJ_FLIP_L;
	} else {
		op->spd_x = 0x200000 / 30;
		op->flip = OBJ_FLIP_R;
	}
	MoveObject2(op);
	op->act3++;
}

static void SharkPopUp1(OBJECT *op)				/* 0x800F4BE4 */
{
	if (op->spd_y <= 0) {
		op->col_box = col_body;
		op->act3++;
	}
	MoveObject2(op);
}

static void SharkPopUp2(OBJECT *op)				/* 0x800F4C28 */
{
	if (op->col_f & COL_D) {
		PlaySound(SE_BANK_ENEMY, SE_MSP_LAND, op);
		TIMER(op) = 50;
		op->act3++;
		return;
	}
	MoveObject2(op);
}

static void (*popup_tbl[])(OBJECT *) = { SharkPopUp0, SharkPopUp1, SharkPopUp2, SharkStun3 };
static void SharkPopUp(OBJECT *op) { popup_tbl[op->act3](op); }	/* 0x800F4AD8 */

static void (*shark_acts[])(OBJECT *) = {			/* 0x800F95E0 */
	ReactMode, SharkNop, SharkIntro, SharkStun, SharkFin, SharkAnchor,
	SharkSlam, SharkBurrow, SharkResurrect, SharkPopUp,
};

/* ---- state 2: death ---- */

static void SharkDeath0(OBJECT *op)				/* 0x800F42C8 */
{
	StopSound(SE_BANK_ENEMY, SE_MSP_LOOP);
	PlaySound(SE_BANK_ENEMY, SE_MSP_DEAD, op);
	W8(&mega, 0xca) = 0;
	PlayerEvent(20, mega.flip);
	SetAnime(op, 28);
	op->act2 = 1;
	TIMER(op) = 127;
	CNT(op) = 25;
	op->muteki = 25;
	op->clut &= ~CLUT_FLASH;
	DisplayCheck(op, 96, 96);
}

static void SharkDeath1(OBJECT *op)				/* 0x800F4360 */
{
	OBJECT	*ep;

	if (--TIMER(op) == 0) {
		op->act2 = 2;
		if ((ep = GetSysObject()) != NULL) {
			ep->flag = 1;
			ep->type = 26;
			ep->x.w.hi = POS_X(op);
			ep->y.w.hi = POS_Y(op);
			LINK(op) = ep;
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

static void SharkDeath2(OBJECT *op)				/* 0x800F2340 */
{
	OBJECT	*ep = LINK(op);

	op->disp = 0;
	if (ep->flag != 0) {
		if (ep->act4 != 0)
			return;
		if (--CNT(op) == -1) {
			CNT(op) = 5;
			op->clut ^= CLUT_FLASH;
		}
		DisplayCheck(op, 96, 96);
		return;
	}
	LINK(op) = NULL;
	W16(op, 0x84) = W16(op, 0x86) = 0;
	W16(op, 0x88) = W16(op, 0x8a) = W16(op, 0x8c) = W16(op, 0x8e) = 0;
	game.boss_bar = 0;
	game.boss = NULL;
	if (game.stage == 12) {
		game.message = 0x80;
		game.clear_f[game.checkpoint] = 1;	/* game[0x2C + checkpoint] */
		game.checkpoint += 8;
	} else
		SharkSoul(shark_fight_time < 5401 ? 0x10 : 0x11);
	DeleteObject3(op);
}

static void (*death_tbl[])(OBJECT *) = { SharkDeath0, SharkDeath1, SharkDeath2 };
static void SharkDeath(OBJECT *op) { death_tbl[op->act2](op); }	/* 0x800F428C */

/* ---- state 3: side kicks ---- */

static void KickPieces(OBJECT *op)
{
	static const short ang[9] = { 2560, 0, 1536, 2760, 200, 1736, 2960, 400, 1936 };
	int	i;

	for (i = 0; i < 9; i++)
		SharkPiece(op, 32, ang[i]);
}

static void KickBlink(OBJECT *op)				/* 0x800F5EEC, 0x800F62AC, 0x800F668C */
{
	if (TIMER(op) & 1) {
		op->disp = 1;
		AnimeTick(op);
	} else
		op->disp = 0;
}

static void KickCount(OBJECT *op)				/* 0x800F5CD8, 0x800F5FB8 */
{
	CNT(op) = kick_cnt[(u_char)game.boss_level];
	op->act3++;
}

static void KickNext(OBJECT *op) { op->act3++; }		/* 0x800F5D28 */

static void KickForm(OBJECT *op, int anime)			/* 0x800F5D3C, 0x800F6118, 0x800F64BC */
{
	if (shark_pieces != 0)
		return;
	PlaySound(SE_BANK_ENEMY, SE_MSP_KICK_FORM, op);
	op->disp = 1;
	SharkSprtSet1(op);
	SetAnime(op, anime);
	op->act3++;
}

static void KickFormed(OBJECT *op, int anime)			/* 0x800F5DA4, 0x800F6180, 0x800F6524 */
{
	if (op->anime.flag == 1) {
		SharkSprtSet0(op);
		SetAnime(op, anime);
		op->act3++;
	}
}

/* var 1: the beam */
static void Beam1(OBJECT *op)					/* 0x800F3014 */
{
	op->x.w.hi = POS_X(&mega);
	op->y.w.hi = RY(128);
	if (RX(176) < POS_X(&mega)) {
		op->flip = OBJ_FLIP_R;
		op->x.w.hi -= 112;
	} else {
		op->flip = OBJ_FLIP_L;
		op->x.w.hi += 112;
	}
	if (POS_X(op) < RX(40))
		op->x.w.hi = RX(40);
	else if (RX(312) < POS_X(op))
		op->x.w.hi = RX(312);
	if (POS_Y(op) < RY(32))
		op->y.w.hi = RY(32);
	KickPieces(op);
	op->act3++;
}

static void Beam3(OBJECT *op) { KickForm(op, 35); }
static void Beam4(OBJECT *op) { KickFormed(op, 21); }

static void Beam5(OBJECT *op)					/* 0x800F5DF8 */
{
	op->atk_box = box_kick_beam;
	op->dmg_box = box_kick_beam + 1;
	TIMER(op) = 50;
	FacePlayer(op);
	op->act3++;
}

static void Beam6(OBJECT *op)					/* 0x800F5E48 */
{
	if (--TIMER(op) != 0)
		return;
	PlaySound(SE_BANK_ENEMY, SE_MSP_KICK_BEAM, op);
	SetAnimeFrame(op, 22, 1);
	SharkBeamShot(op, 48);
	op->act3++;
}

static void Beam7(OBJECT *op)					/* 0x800F5EBC */
{
	if (op->act4 == 0) {
		op->atk_box = op->dmg_box = NULL;
		TIMER(op) = 30;
		op->act3++;
	}
}

static void Beam8(OBJECT *op)					/* 0x800F5EEC */
{
	KickBlink(op);
	if (--TIMER(op) != 0)
		return;
	op->disp = 0;
	if (--CNT(op) == 0)
		op->act = ST_DELETE;
	else
		op->act3 = 1;
}

static void (*beam_tbl[])(OBJECT *) = {				/* 0x800F9708 */
	KickCount, Beam1, KickNext, Beam3, Beam4, Beam5, Beam6, Beam7, Beam8,
};

/* var 2: the ceiling sparks */
static void Spark1(OBJECT *op)					/* 0x800F6008 */
{
	if (RX(176) < POS_X(op)) {
		op->x.w.hi = RX(80);
		op->flip = OBJ_FLIP_R;
	} else {
		op->x.w.hi = RX(272);
		op->flip = OBJ_FLIP_L;
	}
	op->y.w.hi = RY(16);
	KickPieces(op);
	op->act3++;
}

static void Spark2(OBJECT *op) { KickForm(op, 34); }
static void Spark3(OBJECT *op) { KickFormed(op, 18); }

static void Spark4(OBJECT *op)					/* 0x800F61D4 */
{
	SetAnime(op, 19);
	op->atk_box = box_kick_spark;
	op->dmg_box = box_kick_spark + 1;
	op->act3++;
}

static void Spark5(OBJECT *op)					/* 0x800F3180 */
{
	int	x, y, a;

	if (op->anime.flag != 1)
		return;
	TIMER(op) = 60;
	x = (op->flip == OBJ_FLIP_L) ? POS_X(op) - 16 : POS_X(op) + 16;
	y = POS_Y(op) + 52;
	a = ratan2(y - POS_Y(&mega), POS_X(&mega) - x);
	PlaySound(SE_BANK_ENEMY, SE_MSP_KICK_SPARK, op);
	SharkSpark(op, a - 455, x, y);
	SharkSpark(op, a, x, y);
	SharkSpark(op, a + 455, x, y);
	op->act3++;
}

static void Spark6(OBJECT *op)					/* 0x800F6220 */
{
	if (--TIMER(op) != 0)
		return;
	if (--CNT(op) == 0) {
		SetAnime(op, 18);
		op->atk_box = op->dmg_box = NULL;
		TIMER(op) = 30;
		op->act3++;
	} else
		op->act3 -= 2;
}

static void KickBlinkEnd(OBJECT *op)				/* 0x800F62AC */
{
	KickBlink(op);
	if (--TIMER(op) == 0) {
		op->disp = 0;
		op->act = ST_DELETE;
	}
}

static void (*spark_tbl[])(OBJECT *) = {			/* 0x800F972C */
	KickCount, Spark1, Spark2, Spark3, Spark4, Spark5, Spark6, KickBlinkEnd,
};

/* var 3: the bees */
static void Bee0(OBJECT *op)					/* 0x800F6354 */
{
	CNT(op) = bee_time[(u_char)game.boss_level];
	op->act3++;
}

static void Bee1(OBJECT *op)					/* 0x800F63AC */
{
	if (RX(176) < POS_X(op)) {
		op->x.w.hi = RX(64);
		op->flip = OBJ_FLIP_R;
	} else {
		op->x.w.hi = RX(288);
		op->flip = OBJ_FLIP_L;
	}
	op->y.w.hi = RY(64);
	KickPieces(op);
	op->act3++;
}

static void Bee2(OBJECT *op) { KickForm(op, 36); }
static void Bee3(OBJECT *op) { KickFormed(op, 25); }

static void Bee4(OBJECT *op)					/* 0x800F6578 */
{
	SetAnime(op, 26);
	op->atk_box = box_kick_bee;
	op->dmg_box = box_kick_bee + 1;
	op->act3++;
}

static void Bee5(OBJECT *op)					/* 0x800F3290 */
{
	int	n, a, d, i;

	if (CNT(op) != 0)
		CNT(op)--;
	if (op->anime.flag != 1)
		return;
	op->act4 = 0;
	shark_bees = 0;
	n = bee_num[(u_char)game.boss_level];
	if (op->flip == OBJ_FLIP_L) {
		d = -964 / (n - 1);
		a = 3042;
	} else {
		d = 964 / (n - 1);
		a = 3102;
	}
	PlaySound(SE_BANK_ENEMY, SE_MSP_KICK_BEE, op);
	for (i = 0; i < n; i++, a += d)
		SharkBee(op, a);
	KICK_NSHOT(op) = shark_bees;
	op->act3++;
}

static void Bee6(OBJECT *op)					/* 0x800F65C4 */
{
	int	n = KICK_NSHOT(op);

	if (CNT(op) == 0 || --CNT(op) == 0) {
		TIMER(op) = 30;
		op->atk_box = op->dmg_box = NULL;
		op->act4 = 1;
		op->act3++;
		return;
	}
	if (n - shark_bees < (n + 2) / 3)
		return;
	op->act4 = 1;
	op->act3 -= 2;
}

static void Bee7(OBJECT *op)					/* 0x800F668C */
{
	KickBlink(op);
	if (--TIMER(op) == 0) {
		op->act4 = 0;
		op->disp = 0;
		op->act = ST_DELETE;
	}
}

static void (*bee_tbl[])(OBJECT *) = {				/* 0x800F974C */
	Bee0, Bee1, Bee2, Bee3, Bee4, Bee5, Bee6, Bee7,
};

static void KickBeam(OBJECT *op)  { beam_tbl[op->act3](op); }	/* 0x800F5C9C */
static void KickSpark(OBJECT *op) { spark_tbl[op->act3](op); }	/* 0x800F5F7C */
static void KickBee(OBJECT *op)   { bee_tbl[op->act3](op); }	/* 0x800F6318 */

static void (*kick_acts[])(OBJECT *) = {			/* 0x800F96F4 */
	ReactMode, SharkNop, KickBeam, KickSpark, KickBee,
};

static void SharkKickMain(OBJECT *op)				/* 0x800F2EE4 */
{
	int	r;

	if (LINK(op)->act != ST_BOSS) {
		op->act = ST_DELETE;
		op->disp = 0;
		return;
	}
	AnimeTick(op);
	kick_acts[op->act2](op);
	r = ContactWeapon(op);
	if (r < 0) {
		SpawnExplosion(op);
		op->act = ST_DELETE;
		op->act2 = 0;
		return;
	}
	if (r > 0 && r != 127) {
		op->dmg_tbl = dmg_tbl_guard;
		KICK_INV(op) = (r == 25 || r == 26) ? 2 : 30;
	}
	if (KICK_INV(op) != 0) {
		KICK_INV(op)--;
		if ((KICK_INV(op) + 1) & 2)
			op->clut |= CLUT_FLASH;
		else
			op->clut &= ~CLUT_FLASH;
		if (KICK_INV(op) == 0) {
			op->dmg_tbl = dmg_tbl_shark;
			op->clut &= ~CLUT_FLASH;
		}
	}
	ContactMega(op);
}

/* ---- state 4 ---- */

static void SharkDelete(OBJECT *op)				/* 0x800F4454 */
{
	if (op->var != 0)
		LINK(op)->act4 = 0;
	DeleteObject3(op);
}

static void (*shark_states[])(OBJECT *) = {			/* 0x800F95CC */
	SharkSetup, SharkMain, SharkDeath, SharkKickMain, SharkDelete,
};

void MetalShark(OBJECT *op)					/* 0x800F4230 */
{
	op->old_x = op->x;
	op->old_y = op->y;
	shark_states[op->act](op);
	CollideCheck(op);
}
