/*
 * Stage 07 (Shield Sheldon), main object type 13: laser puzzle parts,
 * visual type 11 (laser), misc type 7 (laser draw pieces).
 * Plausible reconstruction from the disassembly; not original code; not compiled.
 * Field offsets are used through the B/H/W macros where the struct has no name.
 */
#include "object.h"
#include "game.h"
#include "sound.h"

#define B(o, ofs)	(((u_char *)(o))[ofs])
#define SB(o, ofs)	(((char *)(o))[ofs])
#define H(o, ofs)	(*(short *)((u_char *)(o) + (ofs)))
#define W(o, ofs)	(*(long *)((u_char *)(o) + (ofs)))
#define P(o, ofs)	(*(OBJECT **)((u_char *)(o) + (ofs)))

/* main type 13 fields */
#define SHOT_ID		0x84	/* u16 work.84: own shot id */
#define IN_DIR		0x86	/* last caught laser direction, 64 = none */
#define TURN_IX		0x87	/* turning reflector list index */
#define DIR		0x88	/* 8-way (beamer) or 16-way (reflector) */
#define LINK		0x89	/* 0 none, 1 laser live, 2 laser timed out */
#define REPIN		0x8A
#define WAIT_TBL	0x8B
#define FIRE_REQ	0x8C
#define DEL_TIMER	0x7E

/* laser (visual 11) fields */
#define L_PARENT	0x7C
#define L_DIR		0x84
#define L_STEP3		0x88	/* 0,1,2: move on 0 */
#define L_LIFE		0x8A
#define L_PROBE		0x8C
#define L_ID		0x90
#define L_X0		0x92
#define L_Y0		0x94
#define L_ANM5		0x96
#define L_PIN		0x98

/* probe (weapon 79) fields */
#define W_ID		0x8C
#define W_PIN		0x98

extern u_short	shot_id_ctr;			/* 0x800FCFEC */
extern HITBOX	hb_relay;			/* 0x800FB964 (-8,-8,16,16) */
extern HITBOX	hb_reflector[16];		/* 0x800FB968 */
extern HITBOX	hb_gate_v, hb_gate_h;		/* 0x800FB9A8, 0x800FB9AC */
extern HITBOX	hb_laser;			/* 0x800FBF9C */
extern ANIME	*laser_anime[];			/* 0x800FBA58 */
extern u_char	dmg_tbl_reflector[];		/* 0x8007451C */
extern u_char	dmg_tbl_reflector_wait[];	/* 0x800745BC */
extern u_char	turn_setup[16][8];		/* 0x800FBAE0: {list, 0,0,0, start, 0,0,0} */
extern long	turn_list[][8];			/* 0x800FBB60 */
extern u_char	reflect_ix[16][8];		/* 0x800FBCC0 */
typedef struct { u_char dir, pad; short dx, dy; } REFLECT;
extern REFLECT	reflect_out[];			/* 0x800FBD40 */
extern u_char	aim8[32];			/* 0x800E987C */
extern short	gate_spawn_rect[5][4];		/* 0x800FBEF8 {x0,y0,x1,y1} */
extern short	gate_del_rect[6][4];		/* 0x800FBEC8 */
extern BLOCKCOPY gate_open_src[6];		/* 0x800FBE90 */
extern BLOCKCOPY gate_close_src;		/* 0x800FBEC0 */
extern u_char	laser_head_spr[8][2];		/* 0x800FBCA0 {sprite, flip} */
extern u_char	laser_tail_spr[8][2];		/* 0x800FBCB0 */

static void	Laser_Fire(OBJECT *op, char var);	/* 0x800EABD8 */
static void	Laser_Reflect(OBJECT *op, char var);	/* 0x800EADA8 */
static int	CatchLaser(OBJECT *op);			/* 0x800EAA00 */

/* ---- state 0: set-up (0x800EA13C) ---- */
static void Puzzle_Init(OBJECT *op)
{
	int hi, n;

	op->hp = 127;
	op->timer = 0;
	H(op, DEL_TIMER) = 2;
	op->atk = 1;
	op->muteki = 0;
	op->anime_tbl = laser_anime;
	B(op, IN_DIR) = 64;
	op->flip = 0;
	op->pri = 7;
	op->fall_f = 1;
	B(op, REPIN) = B(op, LINK) = B(op, FIRE_REQ) = B(op, WAIT_TBL) = 0;
	op->layer = mega.layer;
	op->col_box = 0; op->dmg_box = 0; op->atk_box = 0;
	op->dmg_tbl = dmg_tbl_reflector;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
	op->old_x = op->x; op->old_y = op->y;
	H(op, SHOT_ID) = shot_id_ctr++;

	hi = op->var & 0xF0;
	n = op->var & 0x0F;
	switch (hi) {
	case 0x00:	/* fixed beamer */
		B(op, DIR) = n; SetAnime(op, n + 8);
		op->act = 1; op->act2 = 3; break;
	case 0x10:	/* aiming beamer */
		B(op, DIR) = n; SetAnime(op, n + 8);
		op->act = 1; op->act2 = 4; break;
	case 0x20:	/* relay beamer */
		B(op, DIR) = n; op->dmg_box = &hb_relay; SetAnime(op, n);
		op->act = 1; op->act2 = 2; break;
	case 0x30:	/* fixed reflector */
		B(op, DIR) = n; op->dmg_box = &hb_reflector[n]; SetAnime(op, n + 16);
		op->act = 2; op->act2 = 2; break;
	case 0x40:	/* turning reflector: var becomes 0x40 | list */
		B(op, TURN_IX) = turn_setup[n][4];
		op->var = turn_setup[n][0] + 0x40;
		B(op, DIR) = turn_list[op->var & 0x0F][B(op, TURN_IX)];
		op->dmg_box = &hb_reflector[B(op, DIR)];
		SetAnime(op, B(op, DIR) + 16);
		op->act = 2; op->act2 = 2; break;
	case 0x50:	/* door */
		if (n == 4 || n == 5) { SetAnime(op, 32); op->dmg_box = &hb_gate_h; }
		else                  { SetAnime(op, 33); op->dmg_box = &hb_gate_v; }
		switch (n) {	/* jump table 0x800E9864; door 3 spawns nothing */
		case 0: SpawnInRect(gate_spawn_rect[0][0], gate_spawn_rect[0][2], gate_spawn_rect[0][1], gate_spawn_rect[0][3], 0); break;
		case 1: SpawnInRect(gate_spawn_rect[1][0], gate_spawn_rect[1][2], gate_spawn_rect[1][1], gate_spawn_rect[1][3], 0); break;
		case 2: SpawnInRect(gate_spawn_rect[2][0], gate_spawn_rect[2][2], gate_spawn_rect[2][1], gate_spawn_rect[2][3], 0); break;
		case 4: SpawnInRect(gate_spawn_rect[3][0], gate_spawn_rect[3][2], gate_spawn_rect[3][1], gate_spawn_rect[3][3], 0); break;
		case 5: SpawnInRect(gate_spawn_rect[4][0], gate_spawn_rect[4][2], gate_spawn_rect[4][1], gate_spawn_rect[4][3], 0); break;
		}
		op->act = 3; op->act2 = 2; break;
	}
	op->act3 = 0;
}

/* ---- beamer steps ---- */
static void Fixed_0(OBJECT *op) { op->timer = 3; op->act3++; }		/* 0x800ECB78 */
static void Fixed_1(OBJECT *op)						/* 0x800ECB90 */
{
	if (--op->timer == 0) { Laser_Fire(op, 0); op->act3++; }
}
static void Fixed_2(OBJECT *op)						/* 0x800ECBE4 */
{
	if (B(op, LINK) == 2) { B(op, LINK) = 0; op->timer = 120; op->act3++; }
}
static void Fixed_3(OBJECT *op) { if (--op->timer == 0) op->act3 = 0; }	/* 0x800ECC10 */

static u_char AimStep(OBJECT *op)					/* 0x800ED0AC */
{
	u_char want = aim8[AimAt(op, &mega)];
	u_char cur = B(op, DIR);
	u_char d = (want - cur) & 7;
	if (d == 0) return cur;
	return (d < 4 ? cur + 1 : cur - 1) & 7;
}

static void Aim_0(OBJECT *op)						/* 0x800ECC74 */
{
	short dx;
	if (B(op, LINK) == 2) B(op, LINK) = 0;
	if (B(op, LINK) != 0) return;
	dx = POS_X(&mega) - POS_X(op);
	if (dx < 0) dx = -dx;
	if (dx < 145) B(op, DIR) = AimStep(op);
	if ((op->var & 0xF0) == 0x10) SetAnime(op, B(op, DIR) + 8);
	else                          SetAnime(op, B(op, DIR));
	op->act3++;
}

static void Aim_1(OBJECT *op)						/* 0x800ECD40 */
{
	int d = B(op, IN_DIR) - B(op, DIR);
	if (d >= 0 ? d != 4 : B(op, IN_DIR) != B(op, DIR) - 4)
		Laser_Fire(op, 0);
	op->timer = 302;
	op->act3++;
}

static void Aim_2(OBJECT *op)						/* 0x800ECDB0 */
{
	if (B(op, LINK) == 2) B(op, LINK) = 0;
	if (--op->timer == 0) op->act3 = 0;
}

static void Nop(OBJECT *op) { }						/* 0x800ED288, 0x800ECDEC */
static void (*fixed_tbl[])(OBJECT *) = { Fixed_0, Fixed_1, Fixed_2, Fixed_3 };
static void Beamer_Fixed(OBJECT *op) { fixed_tbl[op->act3](op); }	/* 0x800ECB3C */
static void (*aim_tbl[])(OBJECT *) = { Aim_0, Aim_1, Aim_2 };
static void Beamer_Aim(OBJECT *op) { aim_tbl[op->act3](op); }		/* 0x800ECC38 */

extern void HitReactionDispatch(OBJECT *op);
static void (*beamer_steps[])(OBJECT *) = { HitReactionDispatch, Nop, Nop, Beamer_Fixed, Beamer_Aim };

/* ---- state 1: beamers (0x800EC9B8) ---- */
static void Beamer_Main(OBJECT *op)
{
	op->old_x = op->x; op->old_y = op->y;
	AnimeTick(op);
	beamer_steps[op->act2](op);
	if ((op->var & 0xF0) == 0x20 && CatchLaser(op) && op->act2 != 4) {
		op->act2 = 4; op->act3 = 0;
	}
	DisplayCheck(op, 16, 16);
}

/* ---- reflector step 2: re-fire (0x800ECDF4) ---- */
static void Reflector_Step(OBJECT *op)
{
	if (B(op, LINK) == 0) {
		if (B(op, FIRE_REQ) == 1) { Laser_Reflect(op, 0); B(op, FIRE_REQ) = 0; }
	} else if (B(op, LINK) == 2) {
		B(op, LINK) = 0; B(op, REPIN) = 1;
	}
}
static void (*reflector_steps[])(OBJECT *) = { HitReactionDispatch, Nop, Reflector_Step };

/* ---- state 2: reflectors (0x800EA4CC) ---- */
static void Reflector_Main(OBJECT *op)
{
	op->old_x = op->x; op->old_y = op->y;
	AnimeTick(op);
	reflector_steps[op->act2](op);
	DisplayCheck(op, 16, 16);
	op->hp = 127;
	if (ContactWeapon(op)) {
		B(op, WAIT_TBL) = 33;
		op->dmg_tbl = dmg_tbl_reflector_wait;
		if ((op->var & 0xF0) == 0x40) {
			if (B(op, LINK) == 1) { B(op, FIRE_REQ) = 1; B(op, LINK) = 0; }
			PlaySound(2, 78, op);
			B(op, TURN_IX) = (B(op, TURN_IX) + 1) & 7;
			B(op, DIR) = turn_list[op->var & 0x0F][B(op, TURN_IX)];
			op->dmg_box = &hb_reflector[B(op, DIR)];
			SetAnime(op, B(op, DIR) + 16);
		}
	}
	if (CatchLaser(op)) {
		if (B(op, LINK) == 0) B(op, FIRE_REQ) = 1;
	} else {
		B(op, LINK) = 0;
	}
	if (B(op, WAIT_TBL) != 0 && --B(op, WAIT_TBL) == 0)
		op->dmg_tbl = dmg_tbl_reflector;
}

/* ---- door ---- */
static void DeleteInRect(short *r)					/* 0x800ED1CC */
{
	OBJECT *o;
	for (o = main_objects; o < main_objects + 48; o++) {
		if (o->flag == 0 || o->type != 13) continue;
		if (POS_X(o) < r[0] || r[2] < POS_X(o)) continue;
		if (POS_Y(o) < r[1] || r[3] < POS_Y(o)) continue;
		H(o, DEL_TIMER) = 2;
		o->act = 4;
	}
}

static void Gate_Open(OBJECT *op)					/* 0x800EA650 */
{
	AnimeTick(op);
	if (op->anime.flag != 1) return;
	switch (op->var & 0x0F) {
	case 0: bg_layer[0].max_x = 3488; CopyTiles(0, 3136, 336, &gate_open_src[0]); break;
	case 1: bg_layer[0].max_x = 3824; CopyTiles(0, 3792, 384, &gate_open_src[1]); break;
	case 2: bg_layer[0].max_x = 4480; CopyTiles(0, 4128, 336, &gate_open_src[2]); break;
	case 3: bg_layer[0].max_x = 4832; CopyTiles(0, 4784, 352, &gate_open_src[3]); break;
	case 4: bg_layer[0].max_y = 512;  CopyTiles(0, 4688, 448, &gate_open_src[4]); break;
	case 5: bg_layer[0].max_y = 768;  CopyTiles(0, 3888, 704, &gate_open_src[5]); break;
	}
	op->act3++;
}
static void Gate_OpenEnd(OBJECT *op) { op->act2 = 4; op->act3 = 0; }	/* 0x800ECE90 */

static void Gate_Passed(OBJECT *op)					/* 0x800EA7C8 */
{
	int n = op->var & 0x0F;
	BGLAYER *cam = &bg_layer[0];
	if (n <= 3) {
		if (cam->x < POS_X(op) + 16) return;
		DeleteInRect(gate_del_rect[n]);
		cam->min_x = POS_X(op) + 16;
	} else {
		if (cam->y < POS_Y(op) + 16) return;
		DeleteInRect(gate_del_rect[n]);
		CopyTiles(0, n == 4 ? 4688 : 3888, n == 4 ? 448 : 704, &gate_close_src);
		if (n == 4) cam->min_x = 3840;
		cam->min_y = POS_Y(op) + 32;
		cam->max_y = POS_Y(op) + 32;
	}
	op->act3++;
}

static void (*gate_open_tbl[])(OBJECT *) = { Gate_Open, Gate_OpenEnd };
static void (*gate_pass_tbl[])(OBJECT *) = { Gate_Passed, Nop };
static void Gate_Step3(OBJECT *op) { gate_open_tbl[op->act3](op); }	/* 0x800ECE54 */
static void Gate_Step4(OBJECT *op) { gate_pass_tbl[op->act3](op); }	/* 0x800ECEA0 */
static void (*gate_steps[])(OBJECT *) = { HitReactionDispatch, Nop, Nop, Gate_Step3, Gate_Step4 };

/* ---- state 3: door (0x800ECA5C) ---- */
static void Gate_Main(OBJECT *op)
{
	op->old_x = op->x; op->old_y = op->y;
	gate_steps[op->act2](op);
	DisplayCheck(op, 48, 48);
	if (CatchLaser(op)) {
		if (op->act2 == 2) { PlaySound(2, 32, op); op->act2 = 3; op->act3 = 0; }
	} else {
		B(op, LINK) = 0;
	}
}

/* ---- state 4: deleted by a door (0x800ECB00) ---- */
static void Puzzle_Delete(OBJECT *op)
{
	if (--H(op, DEL_TIMER) == 0) DeleteObject3(op);
}

static void (*puzzle_states[])(OBJECT *) = { Puzzle_Init, Beamer_Main, Reflector_Main, Gate_Main, Puzzle_Delete };

void M13_LaserPuzzle(OBJECT *op)					/* 0x800EC8F8 */
{
	puzzle_states[op->act](op);
}

/* ---- catch test (0x800EAA00) ---- */
static int CatchLaser(OBJECT *op)
{
	OBJECT *w;
	short dx, dy;
	int hi, n;
	for (w = weapon_objects; w < weapon_objects + 32; w++) {
		if (w->flag == 0 || w->type != 79) continue;
		if (H(op, SHOT_ID) == H(w, W_ID)) continue;
		if (!BoxOverlap(w, op)) continue;
		if (SB(w, W_PIN) != 0) return 0;
		if (B(op, REPIN) != 0) {
			B(w, W_PIN) = 1;
			B(op, REPIN) = 0;
		} else {
			dx = dy = 0;
			hi = op->var & 0xF0;
			n = op->var & 0x0F;
			if (hi == 0x30 || hi == 0x40) {
				u_char ix = reflect_ix[B(op, DIR)][W(w, L_DIR)];
				if (ix != 255) { dx = reflect_out[ix].dx; dy = reflect_out[ix].dy; }
			} else if (hi == 0x50) {
				if (n == 4 || n == 5) { dx = 30; dy = -8; }
				else                  { dx = -8; dy = -30; }
			}
			B(w, W_PIN) = 2;
			POS_X(w) = POS_X(op) + dx;
			POS_Y(w) = POS_Y(op) + dy;
		}
		B(op, IN_DIR) = W(w, L_DIR);
		return 1;
	}
	return 0;
}

/* ---- laser spawn ---- */
static OBJECT *NewLaser(OBJECT *op, char var, int dir)
{
	OBJECT *lp = GetVisualObject();
	if (lp == 0) return 0;
	lp->flag = 0x41; lp->type = 11; lp->var = var;
	P(lp, L_PARENT) = op;
	W(lp, L_DIR) = dir;
	lp->flip = op->flip;
	lp->clut = op->clut & 0x7FFF;
	lp->anime_tbl = op->anime_tbl;
	lp->sprt_p = (char *)sprt_bank + sprt_bank->ofs[16];	/* sprite_bank_base word at +0x40 */
	lp->tpage = op->tpage;
	lp->layer = op->layer;
	return lp;
}

static void Laser_Fire(OBJECT *op, char var)				/* 0x800EABD8 */
{
	static const signed char ofs[8][2] = {
		{16, 0}, {12, -12}, {0, -16}, {-12, -12}, {-16, 0}, {-12, 12}, {0, 16}, {12, 12} };
	OBJECT *lp = NewLaser(op, var, B(op, DIR));
	if (lp == 0) return;
	PlaySound(2, 19, lp);
	lp->x.l = 0; POS_X(lp) = POS_X(op);
	lp->y.l = 0; POS_Y(lp) = POS_Y(op);
	if (B(op, DIR) < 8) { POS_X(lp) += ofs[B(op, DIR)][0]; POS_Y(lp) += ofs[B(op, DIR)][1]; }
	H(lp, L_ID) = shot_id_ctr;
	H(op, SHOT_ID) = shot_id_ctr++;
	B(op, LINK) = 1;
}

static void Laser_Reflect(OBJECT *op, char var)			/* 0x800EADA8 */
{
	u_char ix = reflect_ix[B(op, DIR)][B(op, IN_DIR)];
	OBJECT *lp;
	if (ix == 255 || reflect_out[ix].dir == 255) return;
	lp = NewLaser(op, var, reflect_out[ix].dir);
	if (lp == 0) return;
	PlaySound(2, 18, lp);
	lp->x.l = 0; POS_X(lp) = POS_X(op) + reflect_out[ix].dx;
	lp->y.l = 0; POS_Y(lp) = POS_Y(op) + reflect_out[ix].dy;
	H(lp, L_ID) = shot_id_ctr;
	H(op, SHOT_ID) = shot_id_ctr++;
	B(op, LINK) = 1;
}

/* ---- laser (visual 11) ---- */
static int LaserAlive(OBJECT *lp)					/* 0x800ED06C */
{
	OBJECT *p = P(lp, L_PARENT);
	if (p->act == 4) return 0;
	return B(p, LINK) != 0;
}

static void Laser_Init(OBJECT *lp)					/* 0x800EAF28 */
{
	OBJECT *w;
	if (!LaserAlive(lp)) { lp->act = 2; return; }
	lp->pri = 6;
	H(lp, L_LIFE) = 182;
	lp->atk = 1; lp->hp = 0; lp->fall_f = 1;
	H(lp, L_STEP3) = 0; H(lp, L_ANM5) = 0;
	lp->col_box = 0; lp->atk_box = &hb_laser; lp->dmg_box = 0;
	lp->act = 1; lp->act2 = lp->act3 = lp->act4 = 0; lp->disp = 0;
	H(lp, L_X0) = POS_X(lp); H(lp, L_Y0) = POS_Y(lp);
	w = GetWeaponObject();
	if (w) {
		w->flag = 0x41; w->type = 79; w->var = 0;
		P(w, 0x7C) = lp;
		w->flip = lp->flip;
		w->x = lp->x; w->y = lp->y;
		w->clut = lp->clut & 0x7FFF; w->anime_tbl = lp->anime_tbl;
		w->sprt_p = lp->sprt_p; w->tpage = lp->tpage; w->layer = lp->layer;
		W(w, 0x84) = W(lp, L_DIR);
		P(lp, L_PROBE) = w;
		H(w, W_ID) = H(lp, L_ID);
	}
	switch (W(lp, L_DIR)) {
	case 0: lp->spd_x =  0x100000; lp->spd_y = 0; break;
	case 1: lp->spd_x =  0x100000; lp->spd_y =  0x100000; break;
	case 2: lp->spd_x = 0;         lp->spd_y =  0x100000; break;
	case 3: lp->spd_x = -0x100000; lp->spd_y =  0x100000; break;
	case 4: lp->spd_x = -0x100000; lp->spd_y = 0; break;
	case 5: lp->spd_x = -0x100000; lp->spd_y = -0x100000; break;
	case 6: lp->spd_x = 0;         lp->spd_y = -0x100000; break;
	case 7: lp->spd_x =  0x100000; lp->spd_y = -0x100000; break;
	}
}

static void PinFromProbe(OBJECT *lp)					/* 0x800ECFD0 */
{
	OBJECT *w = P(lp, L_PROBE);
	B(lp, L_PIN) = B(w, W_PIN);
	if (SB(w, W_PIN) == 2) { POS_X(lp) = POS_X(w); POS_Y(lp) = POS_Y(w); }
	B(w, W_PIN) = 0;
}

static int HeadInWall(OBJECT *lp)					/* 0x800ED014 */
{
	u_char c = TileAt(lp, POS_X(lp), POS_Y(lp));
	if (c == 0) return 0;
	return (c < 37 || c >= 48);
}

static void Laser_Draw(OBJECT *lp)	/* the three misc type 7 objects, inline in the original */
{
	OBJECT *m;
	if ((m = GetMiscObject()) != 0) {	/* start sprite */
		m->type = 7; m->var = 1; m->pri = 5;
		POS_X(m) = H(lp, L_X0); POS_Y(m) = H(lp, L_Y0);
		P(m, 0x50) = lp;
		H(m, 0x54) = W(lp, L_DIR); H(m, 0x56) = H(lp, L_ANM5);
		m->anime.sprt = laser_tail_spr[W(lp, L_DIR)][0] + H(lp, L_ANM5);
		m->flag = 0x51; m->flip = laser_tail_spr[W(lp, L_DIR)][1];
	}
	if ((m = GetMiscObject()) != 0) {	/* body: start (x,+8) to head (+0x0E,+0x0C) */
		m->type = 7; m->var = 0; m->pri = 6;
		H(m, 0x0A) = H(lp, L_X0); H(m, 0x08) = H(lp, L_Y0);
		H(m, 0x0E) = POS_X(lp);   H(m, 0x0C) = POS_Y(lp);
		P(m, 0x50) = lp;
		H(m, 0x54) = W(lp, L_DIR); H(m, 0x56) = H(lp, L_STEP3);
		m->flag = 0x51;
	}
	if ((m = GetMiscObject()) != 0) {	/* head sprite */
		m->type = 7; m->var = 1; m->pri = 4;
		m->x = lp->x; m->y = lp->y;
		P(m, 0x50) = lp;
		H(m, 0x54) = W(lp, L_DIR); H(m, 0x56) = H(lp, L_STEP3);
		m->anime.sprt = laser_head_spr[W(lp, L_DIR)][0] + H(lp, L_STEP3);
		m->flag = 0x51; m->flip = laser_head_spr[W(lp, L_DIR)][1];
	}
}

static void Laser_Main(OBJECT *lp)					/* 0x800EB0F0 */
{
	OBJECT *p = P(lp, L_PARENT);
	if (LaserAlive(lp)) {
		PinFromProbe(lp);
		ContactMega(lp);
		if (H(lp, L_LIFE) == 0 || --H(lp, L_LIFE) != 0) {
			if (!HeadInWall(lp) && H(lp, L_STEP3) == 0 && SB(lp, L_PIN) != 2) {
				MoveObject(lp);
				if (POS_X(lp) < 0) POS_X(lp) = 0;
				if (POS_Y(lp) < 0) POS_Y(lp) = 0;
			}
			Laser_Draw(lp);
			H(lp, L_STEP3) = (H(lp, L_STEP3) + 1) % 3;
			H(lp, L_ANM5) = (H(lp, L_ANM5) + 1) % 5;
			return;
		}
		B(p, LINK) = 2;
	}
	lp->act++;
	lp->act2 = 0;
	Laser_Draw(lp);
}

static void Laser_End(OBJECT *lp)					/* 0x800EB734 */
{
	if (lp->act2 == 0) {
		lp->act2++;
		lp->act3 = 13;
		if (frame_count & 1) Laser_Draw(lp);
		return;
	}
	if (frame_count & 1) Laser_Draw(lp);
	if (--lp->act3 == 0) lp->act++;
}

static void Laser_Delete(OBJECT *lp) { DeleteObject3(lp); }		/* 0x800ED290 */
static void (*laser_states[])(OBJECT *) = { Laser_Init, Laser_Main, Laser_End, Laser_Delete };
void V11_Laser(OBJECT *lp) { laser_states[lp->act](lp); }		/* 0x800EC934 */

/* misc type 7 (0x800EC970): skipped while mega.grab+2 != 0; state 0 draws, state 1 deletes */
extern void LaserBodyDraw(OBJECT *m);					/* 0x800EBCD8, not reconstructed */
void M7_LaserPiece(OBJECT *m)
{
	if (MEGA_B(0xCC) != 0) return;
	if (m->act == 0) {
		if (m->var == 1) DisplayCheck(m, 128, 128);
		else             LaserBodyDraw(m);
		m->act++;
	} else {
		DeleteObject3(m);
	}
}
