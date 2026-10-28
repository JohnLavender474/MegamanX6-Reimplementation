/*
 *	v22_metal_shark.c  --  Metal Shark Player shots and effects (overlay 07, visual object 22)
 *
 *	var 0        anchor
 *	var 16..21   debris (no damage)
 *	var 32       forming piece of a side kick
 *	var 48       beam (side kick var 1); var 49..55 its hit segments
 *	var 64       spark (side kick var 2)
 *	var 80..83   bee (side kick var 3)
 *
 *	Plausible reconstruction, not original code, not compiled.
 */
#include "object.h"
#include "game.h"
#include "sound.h"

#define PX(n)		((long)(n) << 16)
#define ABS(a)		((a) < 0 ? -(a) : (a))
#define W8(o, n)	(*(char *)((u_char *)(o) + (n)))
#define W16(o, n)	(*(short *)((u_char *)(o) + (n)))
#define W32(o, n)	(*(long *)((u_char *)(o) + (n)))
#define WP(o, n)	(*(OBJECT **)((u_char *)(o) + (n)))

#define BOSS(o)		WP(o, 0x7c)
#define ANGLE(o)	W32(o, 0x84)
#define T(o)		W16(o, 0x88)
#define KICK(o)		WP(o, 0x8c)
#define CX(o)		W16(o, 0x90)
#define CY(o)		W16(o, 0x92)
#define ASTOP(o)	W8(o, 0x94)
#define HITF(o)		W8(o, 0x95)
#define COUNTS(o)	W8(o, 0x96)

#define KIND(o)		((o)->var & 0xf0)

extern HITBOX	box_anchor[];		/* 0x800F977C / 0x800F9780 (-12,-12,24,24); col 0x800F9784 */
extern HITBOX	box_segment[];		/* 0x800F9788 (-16,-8,32,16) */
extern HITBOX	box_spark[];		/* 0x800F978C (-6,-6,12,12); col 0x800F9790 (0,0,6,6) */
extern HITBOX	box_bee[];		/* 0x800F9794 / 0x800F9798 (-4,-4,8,8); col 0x800F979C */
extern HITBOX	box_bee_blast[];	/* 0x800F97A0 (-16,-16,32,32) */
extern OBJECT	*shark_beam;		/* 0x800F9878 */
extern short	shark_pieces, shark_bees;

static OBJECT *NewVis(OBJECT *op, int var)
{
	OBJECT	*vp = GetVisualObject();

	if (vp == NULL)
		return NULL;
	vp->flag = 0x41;
	vp->type = 22;
	vp->var = var;
	vp->clut = op->clut & 0x7fff;
	vp->anime_tbl = op->anime_tbl;
	vp->sprt_p = op->sprt_p;
	vp->tpage = op->tpage;
	vp->flip = op->flip;
	vp->layer = op->layer;
	return vp;
}

void SharkDebris(OBJECT *op, int var, int x, int y)		/* 0x800F6880 */
{
	OBJECT	*vp = NewVis(op, var);

	if (vp == NULL)
		return;
	vp->x.l = PX(x);
	vp->y.l = PX(y);
	BOSS(vp) = op;
}

void SharkPiece(OBJECT *op, int var, int angle)			/* 0x800F6948 */
{
	OBJECT	*vp = NewVis(op, var);

	if (vp == NULL)
		return;
	ANGLE(vp) = angle;
	vp->x.l = PX(POS_X(op));
	vp->y.l = PX(POS_Y(op));
	BOSS(vp) = WP(op, 0x80);
	shark_pieces++;
}

void SharkShot(OBJECT *op, int var)				/* 0x800F6A1C */
{
	OBJECT	*vp = NewVis(op, var);

	if (vp == NULL)
		return;
	vp->x.l = PX(POS_X(op));
	vp->y.l = PX(POS_Y(op));
	BOSS(vp) = op;
}

void SharkBeamShot(OBJECT *op, int var)				/* 0x800F6AD0 */
{
	OBJECT	*vp = NewVis(op, var);

	if (vp == NULL)
		return;
	vp->x.l = PX(POS_X(op));
	vp->y.l = PX(POS_Y(op));
	KICK(vp) = op;
	BOSS(vp) = WP(op, 0x80);
	op->act4 = 1;
}

static void SharkSegment(OBJECT *op, int var, int x, int y)	/* 0x800F6B94 */
{
	OBJECT	*vp = NewVis(op, var);

	if (vp == NULL)
		return;
	vp->x.l = PX(x);
	vp->y.l = PX(y);
	BOSS(vp) = BOSS(op);
	KICK(vp) = KICK(op);
}

void SharkSpark(OBJECT *op, int angle, int x, int y)		/* 0x800F6C70 */
{
	OBJECT	*vp = NewVis(op, 64);

	if (vp == NULL)
		return;
	ANGLE(vp) = angle;
	vp->x.l = PX(x);
	vp->y.l = PX(y);
	BOSS(vp) = WP(op, 0x80);
}

void SharkBee(OBJECT *op, int angle)				/* 0x800F6D4C */
{
	OBJECT	*vp = NewVis(op, 80);

	if (vp == NULL)
		return;
	ANGLE(vp) = angle;
	vp->x.l = PX((op->flip == OBJ_FLIP_L) ? POS_X(op) - 36 : POS_X(op) + 36);
	vp->y.l = PX(POS_Y(op) - 12);
	KICK(vp) = op;
	BOSS(vp) = WP(op, 0x80);
	shark_bees++;
}

static void FacePlayer(OBJECT *op)				/* 0x800F6E48 */
{
	op->flip = (POS_X(op) < POS_X(&mega)) ? OBJ_FLIP_R : OBJ_FLIP_L;
}

static void FlipBySpeed(OBJECT *op)
{
	op->flip = (op->spd_x > 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
}

/* ---- set-up (state 0) ---- */

static void VisSetup(OBJECT *op)				/* 0x800F35A8 */
{
	if (BOSS(op)->act != 1) {
		op->act = 2;
		op->disp = 0;
		return;
	}
	op->atk = 0;
	op->hp = 1;
	op->pri = 4;
	op->fall_f = 1;
	op->dmg_tbl = dmg_tbl_guard;
	op->atk_box = op->dmg_box = op->col_box = NULL;
	op->disp = 0;
	op->act = 1;
	op->act2 = op->act3 = op->act4 = 0;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
	ASTOP(op) = 0;
	COUNTS(op) = 0;

	switch (KIND(op)) {
	case 0x00:
		SetAnime(op, 9);
		op->atk = 4;
		op->x.w.hi += (op->flip == OBJ_FLIP_L) ? -32 : 32;
		op->act2 = 1;
		break;
	case 0x10:
		op->flip = OBJ_FLIP_L;
		op->act2 = 0;
		break;
	case 0x20:
		op->flip = OBJ_FLIP_L;
		op->act2 = 2;
		break;
	case 0x30:
		op->atk = 4;
		if (op->var == 48) {
			SetAnime(op, 23);
			shark_beam = op;
			op->x.w.hi += (op->flip == OBJ_FLIP_L) ? -144 : 144;
			op->y.w.hi += 45;
			op->act2 = 3;
		} else {
			op->atk_box = box_segment;
			op->act2 = 4;
		}
		break;
	case 0x40:
		SetAnime(op, 20);
		op->atk = 4;
		op->act2 = 5;
		break;
	case 0x50:
		SetAnime(op, 27);
		COUNTS(op) = 1;
		op->dmg_tbl = dmg_tbl_minor;
		op->atk = 4;
		op->act2 = 6;
		op->var += GetRNG() & 3;
		break;
	}
}

/* ---- step 0: debris ---- */
static void Debris0(OBJECT *op)					/* 0x800F3988 */
{
	int	dx, h, t;

	if (op->var >= 19)
		SetAnime(op, 29 + (GetRNG() & 1));
	else
		SetAnime(op, 29 + GetRNG() % 3);
	op->flip = (GetRNG() & 1) ? OBJ_FLIP_L : OBJ_FLIP_R;
	op->col_box = NULL;
	dx = GetRNG() % 40 + 20;
	h = GetRNG() % 60 + 50;
	t = GetRNG() % 10 + 15;
	op->grav = PX(h) / (t * t / 2);
	op->acc_x = 0;
	op->spd_y = op->grav * t;
	switch (op->var - 16) {
	case 0: case 3:
		op->spd_x = (GetRNG() & 1) ? PX(-dx) / t : PX(dx) / t;
		break;
	case 1: case 4:
		op->spd_x = PX(-dx) / t;
		break;
	case 2: case 5:
		op->spd_x = PX(dx) / t;
		break;
	}
	op->act3++;
}

static void Debris1(OBJECT *op) { MoveObject2(op); }		/* 0x800F70D4 */

static void (*debris_tbl[])(OBJECT *) = { Debris0, Debris1 };	/* 0x800F97CC */

/* ---- step 1: anchor ---- */
static void Anchor0(OBJECT *op)					/* 0x800F7130 */
{
	int	a;

	op->dmg_box = box_anchor;
	op->atk_box = box_anchor + 1;
	op->col_box = box_anchor + 2;
	a = ratan2(POS_Y(op) - POS_Y(&mega), POS_X(&mega) - POS_X(op));
	op->spd_x = rcos(a) << 6;
	op->spd_y = rsin(a) << 6;
	op->acc_x = op->grav = 0;
	op->act3++;
}

static void Anchor1(OBJECT *op)					/* 0x800F71D8 */
{
	MoveObject2(op);
	if (op->col_f) {
		T(op) = 50;
		ASTOP(op) = 1;
		op->act3++;
	}
}

static void Anchor2(OBJECT *op)					/* 0x800F7224 */
{
	if (--T(op) != 0)
		return;
	FacePlayer(op);
	op->spd_x = (op->flip == OBJ_FLIP_L) ? -0x147ae : 0x147ae;	/* 1.28 */
	op->grav = 16777;						/* 0.256 */
	op->spd_y = 0x66666;						/* 6.4 */
	op->acc_x = 0;
	T(op) = 9;
	ASTOP(op) = 0;
	op->act3++;
}

static void Anchor3(OBJECT *op)					/* 0x800F72BC */
{
	MoveObject2(op);
	if (op->col_f == 0)
		return;
	if (--T(op) == -1) {
		SpawnExplosion(op);
		op->act = 2;
		return;
	}
	if (HITF(op) & COL_U)
		op->spd_y = -0x33333;					/* 3.2 down */
	else if (HITF(op) & COL_D) {
		op->spd_y = 0x66666;
		op->grav = 16777;
	} else if (HITF(op) & COL_L) {
		op->spd_x = 0x147ae;
		op->flip = OBJ_FLIP_R;
	} else if (HITF(op) & COL_R) {
		op->spd_x = -0x147ae;
		op->flip = OBJ_FLIP_L;
	}
	op->x = op->old_x;
	op->y = op->old_y;
	MoveObject2(op);
}

static void (*anchor_tbl[])(OBJECT *) = { Anchor0, Anchor1, Anchor2, Anchor3 };	/* 0x800F97D4 */

/* ---- step 2: forming piece ---- */
static void Piece0(OBJECT *op)					/* 0x800F73F0 */
{
	int	a;

	SetAnime(op, 29 + GetRNG() % 3);
	op->flip = (GetRNG() & 1) ? OBJ_FLIP_L : OBJ_FLIP_R;
	a = (ANGLE(op) + 2048) & 0xfff;
	CX(op) = POS_X(op);
	CY(op) = POS_Y(op);
	op->x.w.hi = CX(op) + (rcos(a) >> 3);
	op->y.w.hi = CY(op) + (rsin(a) >> 3);
	op->act3++;
}

static void Piece1(OBJECT *op)					/* 0x800F3B9C */
{
	int	a = ratan2(POS_Y(op) - CY(op), CX(op) - POS_X(op));

	op->spd_x = rcos(a) << 7;
	op->spd_y = rsin(a) << 7;
	MoveObject(op);
	if (ABS(CX(op) - POS_X(op)) < 9 && ABS(CY(op) - POS_Y(op)) < 9)
		op->act = 2;
}

static void (*piece_tbl[])(OBJECT *) = { Piece0, Piece1 };	/* 0x800F97E4 */

/* ---- step 3: beam ---- */
static void BeamFollow(OBJECT *op, OBJECT *kp)
{
	op->disp = kp->disp;
	if (kp->clut & CLUT_FLASH)
		op->clut |= CLUT_FLASH;
	else
		op->clut &= ~CLUT_FLASH;
}

static void Beam0(OBJECT *op)					/* 0x800F3C80 */
{
	static const short ofs[7][2] = {
		{ 112, -56 }, { 80, -40 }, { 48, -24 }, { 16, -8 },
		{ -16, 8 }, { -48, 24 }, { -80, 40 },
	};
	OBJECT	*kp = KICK(op);
	int	f;

	if (kp->act != 3) {
		op->act = 2;
		op->disp = 0;
		return;
	}
	BeamFollow(op, kp);
	f = op->anime.flag;
	if (f >= 1 && f <= 7) {
		int dx = ofs[f - 1][0];
		if (op->flip != OBJ_FLIP_L)
			dx = -dx;
		SharkSegment(op, 48 + f, POS_X(op) + dx, POS_Y(op) + ofs[f - 1][1]);
	} else if (f == 16) {
		T(op) = 60;
		op->act3++;
	}
}

static void Beam1(OBJECT *op)					/* 0x800F7500 */
{
	if (--T(op) == 0) {
		SetAnime(op, 24);
		op->act3++;
	}
}

static void Beam2(OBJECT *op)					/* 0x800F7554 */
{
	OBJECT	*kp = KICK(op);

	if (kp->act != 3) {
		op->act = 2;
		op->disp = 0;
		return;
	}
	BeamFollow(op, kp);
	if (op->anime.flag == 16) {
		op->act = 2;
		op->disp = 0;
		kp->act4 = 0;
	}
}

static void (*beam_tbl[])(OBJECT *) = { Beam0, Beam1, Beam2 };	/* 0x800F97EC */

/* ---- step 4: beam segment ---- */
static void Segment(OBJECT *op)					/* 0x800F75D8 */
{
	if (KICK(op)->act != 3 || shark_beam->anime.flag < (op->var & 15))
		op->act = 2;
}

/* ---- step 5: spark ---- */
static void Spark0(OBJECT *op)					/* 0x800F765C */
{
	op->disp = 1;
	op->atk_box = box_spark;
	op->col_box = box_spark + 1;
	op->spd_x = rcos(ANGLE(op)) * 96;
	op->spd_y = rsin(ANGLE(op)) * 96;
	op->act3++;
}

static void Spark1(OBJECT *op)					/* 0x800F76E0 */
{
	MoveObject(op);
	if (op->col_f == 0) {
		CornerCheck(op);
		if (op->col_f == 0)
			return;
	}
	ASTOP(op) = 1;
	T(op) = 60;
	op->act3++;
}

static void Spark2(OBJECT *op)					/* 0x800F7748 */
{
	if (--T(op) == 0) {
		T(op) = 30;
		op->act3++;
	}
}

static void Spark3(OBJECT *op)					/* 0x800F777C */
{
	op->disp = T(op) & 1;
	if (--T(op) == 0) {
		op->disp = 0;
		op->act = 2;
	}
}

static void (*spark_tbl[])(OBJECT *) = { Spark0, Spark1, Spark2, Spark3 };	/* 0x800F97F8 */

/* ---- step 6: bee ---- */
static void Bee0(OBJECT *op)					/* 0x800F7820 */
{
	op->disp = 1;
	op->atk_box = box_bee;
	op->dmg_box = box_bee + 1;
	op->col_box = box_bee + 2;
	op->spd_x = rcos(ANGLE(op)) * 48;
	op->spd_y = rsin(ANGLE(op)) * 48;
	op->flip = (op->spd_x > 0) ? OBJ_FLIP_R : OBJ_FLIP_L;
	T(op) = 32;
	op->act3++;
}

static void Bee1(OBJECT *op)					/* 0x800F78C8 */
{
	MoveObject(op);
	if (--T(op) == 0) {
		SetAnimeFrame(op, 27, 2 + (GetRNG() & 3) * 2);
		T(op) = 40;
		op->act3++;
	}
}

static void Bee2(OBJECT *op)					/* 0x800F793C */
{
	if (--T(op) == 0) {
		T(op) = 180;
		op->act3++;
	}
}

static void Bee3(OBJECT *op)					/* 0x800F3F94 */
{
	static const char mul[4] = { 8, 16, 24, 32 };	/* 0.5, 1, 1.5, 2 px */
	int	a;

	if (--T(op) == 0) {
		op->atk_box = box_bee_blast;
		op->disp = 0;
		op->dmg_box = op->col_box = NULL;
		T(op) = 34;
		op->act3++;
		return;
	}
	a = ratan2(POS_Y(op) - POS_Y(&mega), POS_X(&mega) - POS_X(op));
	if ((op->var & 15) < 4) {
		op->spd_x = rcos(a) * mul[op->var & 15];
		op->spd_y = rsin(a) * mul[op->var & 15];
	}
	op->acc_x = op->grav = 0;
	FlipBySpeed(op);
	MoveObject(op);
}

static void Bee4(OBJECT *op)					/* 0x800F7970 */
{
	if ((T(op) & 7) == 0)
		SpawnExplosionRand(op, 1, 16, 16);
	if (--T(op) == 0)
		op->act = 2;
}

static void (*bee_tbl[])(OBJECT *) = { Bee0, Bee1, Bee2, Bee3, Bee4 };	/* 0x800F9808 */

/* ---- step dispatch ---- */
static void VDebris(OBJECT *op)  { debris_tbl[op->act3](op); }	/* 0x800F7098 */
static void VAnchor(OBJECT *op)  { anchor_tbl[op->act3](op); }	/* 0x800F70F4 */
static void VPiece(OBJECT *op)   { piece_tbl[op->act3](op); }	/* 0x800F73B4 */
static void VBeam(OBJECT *op)    { beam_tbl[op->act3](op); }	/* 0x800F74C4 */
static void VSpark(OBJECT *op)   { spark_tbl[op->act3](op); }	/* 0x800F7620 */

static void VBee(OBJECT *op)					/* 0x800F77C8 */
{
	if (KICK(op)->act4 == 1)
		COUNTS(op) = 0;
	bee_tbl[op->act3](op);
}

static void (*vis_steps[])(OBJECT *) = {			/* 0x800F97B0 */
	VDebris, VAnchor, VPiece, VBeam, Segment, VSpark, VBee,
};

/* ---- main (state 1) ---- */
static void VisMain(OBJECT *op)					/* 0x800F37DC */
{
	int	k;

	if (BOSS(op)->act != 1) {
		op->act = 2;
		op->disp = 0;
		return;
	}
	op->old_x = op->x;
	op->old_y = op->y;
	if (ASTOP(op) == 0)
		AnimeTick(op);
	vis_steps[op->act2](op);
	if (op->act != 1) {
		op->disp = 0;
		return;
	}
	k = KIND(op);
	if (k != 0x30 && ContactWeapon(op) < 0)
		goto blast;
	if (ContactMega(op) && (k == 0x00 || k == 0x40))
		goto blast;
	switch (k) {
	case 0x00: case 0x10:
		if (OffScreenCheck2(op, 48, 48)) {
			op->act = 2;
			return;
		}
		/* fall through */
	case 0x20:
		DisplayCheck(op, 16, 16);
		return;
	case 0x40: case 0x50:
		if (OffScreenCheck2(op, 48, 48) == 1)
			op->act = 2;
		return;
	}
	return;
blast:
	SpawnExplosion(op);
	op->disp = 0;
	op->act = 2;
}

/* ---- delete (state 2) ---- */
static void VisDelete(OBJECT *op)				/* 0x800F7024 */
{
	if (KIND(op) == 0x20)
		shark_pieces--;
	else if (KIND(op) == 0x50 && COUNTS(op) == 1)
		shark_bees--;
	DeleteObject3(op);
}

static void (*vis_states[])(OBJECT *) = { VisSetup, VisMain, VisDelete };	/* 0x800F97A4 */

void SharkVisual(OBJECT *op)					/* 0x800F6FA0 */
{
	HITF(op) = op->col_f | W8(op, 0x71);
	op->old_x = op->x;
	op->old_y = op->y;
	vis_states[op->act](op);
	if (KIND(op) == 0) {
		CollideCheck(op);
		W8(op, 0x71) = 0;
	}
}
