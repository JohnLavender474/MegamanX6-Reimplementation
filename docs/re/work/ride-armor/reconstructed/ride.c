/*
 *	ride.c  --  Ride Armor (actor2 type 0, overlay 06, Metal Shark Player's stage)
 *
 *	Reconstructed from the disassembly of 0x800EE158..0x800F2730 (shark-1_ride.state.gz).
 *	The control flow follows the code; names are ours. Steps of the var != 0 table
 *	(0x800F3AAC) are not reconstructed: the init always clears var.
 */
#include "../../x/attacks/reconstructed/include/player.h"

#define MEGA_RIDE(p)		(((char *)(p))[0xD5])
#define MEGA_RIDE_ANIME(p)	(((u_char *)(p))[0xD8])
#define MEGA_EVENT(p)		(((char *)(p))[0xD0])
#define MEGA_STOPPED(p)		(((char *)(p))[0xCC])

typedef struct {
	OBJECT	o;			/* +0x00..+0x7B (+0x7C..: work, below) */
} RIDE_BASE;

typedef struct {
	char	flag, type, var, disp, act, act2, act3, act4;
	FIXED	x, y;
	ENEMYSET *set;
	char	layer; u_char flip; char pri; char anime_no;
	FIXED	old_x, old_y;
	long	spd_x, spd_y, acc_x, grav;
	ANIME	**anime_tbl; ANIME *anime_p; u_char *tex_p; char *sprt_p;
	u_short	tpage, clut;
	ANIME	anime;
	u_char	anime_new, pad49[7];
	HITBOX	*atk_box, *dmg_box;
	u_char	*dmg_tbl;
	char	hp; u_char hp_old, pad5e[2];
	char	atk, muteki; u_char pad62; char hit_kind, hit_id, hit_tag; u_char pad66;
	char	fall_f;
	HITBOX	*col_box;
	short	inner_x, inner_y;
	u_char	col_f, push_f;		/* +0x70, +0x71 */
	u_char	pad72[10];
	u_char	pad7c, air_dash;	/* +0x7C, +0x7D */
	u_char	cool, shots;		/* +0x7E, +0x7F */
	u_char	pad80, queued;		/* +0x80, +0x81 */
	u_char	pad82[2];
	u_char	hit_flip, lock;		/* +0x84, +0x85 */
	u_char	shot_n, combo;		/* +0x86, +0x87 */
	u_short	key, key_trg;		/* +0x88, +0x8A */
	u_char	blink, destroy;		/* +0x8C, +0x8D */
	u_char	tap, tap_time;		/* +0x8E, +0x8F */
	u_char	timer;			/* +0x90 */
	u_char	cd_int, cd_cnt;		/* +0x91, +0x92 */
	u_char	kb;			/* +0x93 */
	u_char	charge, charge_t;	/* +0x94, +0x95 */
	u_short	f96;			/* +0x96 */
	u_char	f97;			/* +0x97 */
	u_char	pad97;
	short	move_spd;		/* +0x98: x 256 */
	short	tap_dir;		/* +0x9A */
	short	save[4];		/* +0x9C..+0xA2 */
	short	kb_spd;			/* +0xA4 */
	u_short	charge_key;		/* +0xA6 */
	u_char	coll;			/* +0xA8 */
	u_char	padA9[7];
} RIDE;					/* 0xB0 bytes */

#define RF_LEAVE	0x01
#define RF_KNOCK	0x02
#define RF_GONE		0x04
#define RF_DASHKEY	0x08
#define RF_NOCHARGE	0x10
#define RF_INSIDE	0x40
#define RF_ZERO		0x80

#define KEY_R		0x0001
#define KEY_L		0x0002
#define KEY_U		0x0004
#define KEY_SHOT	0x0030
#define KEY_JUMP	0x0080
#define KEY_DASH	0x0100

extern PLAYER	mega;
extern RIDE	actor2;
extern HITBOX	ride_col_box, ride_board_box, ride_dmg_box;	/* 0x800F3A44, 0x800F3A48, 0x800F3A4C */
extern u_char	ride_junk[];					/* 0x800F3A3C: anims 24..30 */
extern ANIME	*ride_anime_tbl[];				/* 0x800F3D4C */
extern void	(*ride_steps[])(RIDE *);			/* 0x800F3A74 */
extern void	(*ride_charge_steps[])(RIDE *);			/* 0x800F3A5C */

static void ride_set(RIDE *r, int act)	{ r->act = act; r->act2 = 0; }
static void x_anime(RIDE *r, int n)
{
	if (r->destroy == 0) { MEGA_RIDE_ANIME(&mega) = n; ((OBJECT *)&mega)->act2 = 0; }
}

static int ride_leave(RIDE *r)
{
	if ((r->key & 7) == KEY_U) {
		if (r->key_trg & KEY_JUMP) { PlaySound(2, 37, 0); return 1; }
		return 0;
	}
	return ((OBJECT *)&mega)->hp == (char)0x80;
}

static int ride_can_attack(RIDE *r)
{
	if (!(r->key_trg & KEY_SHOT)) return 0;
	return r->var == 0 || r->cool == 0;
}

static int ride_face(RIDE *r)
{
	if (r->key & KEY_L) { r->flip = OBJ_FLIP_L; return 1; }
	if (r->key & KEY_R) { r->flip = OBJ_FLIP_R; return 1; }
	return 0;
}

static void ride_punch_obj(RIDE *r, int type, int var)
{
	OBJECT *w = GetWeaponObject();

	if (w == 0) return;
	w->flag = 0x41; w->type = type; w->var = var;
	w->x.w.hi = POS_X(r); w->y.w.hi = POS_Y(r);
	w->anime_tbl = r->anime_tbl; w->tpage = r->tpage; w->sprt_p = r->sprt_p;
	w->clut = r->clut & ~CLUT_FLASH; w->pri = r->pri; w->flip = r->flip;
	*(RIDE **)((char *)w + 0x7C) = r;
}

/* step 0: empty, waiting for X (0x800EE924) */
void RideIdle(RIDE *r)
{
	HITBOX *keep;

	if (r->act2 == 0) {
		r->act2 = 1; r->col_box = &ride_col_box; SetAnime((OBJECT *)r, 17); r->blink = 0;
		return;
	}
	if (!(r->coll & COL_D)) { ride_set(r, 12); return; }
	if (MEGA_EVENT(&mega) || game.exit_req) return;
	if (((OBJECT *)&mega)->var && ((char *)&mega)[0x8E]) return;
	if (((char *)&mega)[0x7A] || mega.var_e0 || (r->f96 & 0x8200)) return;
	keep = r->col_box;
	r->col_box = &ride_board_box;
	if (TerrainBoxOverlap((OBJECT *)&mega, (OBJECT *)r) && !game.exit_req
	    && ((OBJECT *)&mega)->hp > 0 && mega.giga == 0 && mega.spike == 0) {
		ride_set(r, 10);
		SetAnime((OBJECT *)r, 18);
		r->f97 |= RF_INSIDE;
		MEGA_RIDE(&mega) = (r == &actor2) ? 1 : 2;	/* PlayerRideCheck: step 18, ride = -ride */
	}
	r->col_box = keep;
}

/* step 1: stand (0x800EEC50) */
void RideStand(RIDE *r)
{
	if (r->act2 == 0) {
		r->act2 = 1; r->move_spd = 352; SetAnime((OBJECT *)r, 0); x_anime(r, 41); r->combo = 0;
	}
	if (r->charge == 2) { ride_set(r, 13); return; }
	if (ride_leave(r)) { ride_set(r, 11); return; }
	if (!(r->coll & COL_D)) { ride_set(r, 12); return; }
	if (!(r->f97 & RF_KNOCK)) {
		if (ride_face(r)) ride_set(r, 2);
		else if (r->key_trg & KEY_JUMP) ride_set(r, 4);
		else if (ride_can_attack(r)) ride_set(r, 3);
	}
	AnimeTick((OBJECT *)r);
}

/* step 2: walk (0x800EEE00) */
void RideWalk(RIDE *r)
{
	if (r->act2 == 0) {
		r->act2 = 1; r->move_spd = 352; SetAnime((OBJECT *)r, 1); x_anime(r, 42); r->combo = 0;
	}
	if (r->charge == 2) { ride_set(r, 13); return; }
	if (ride_leave(r)) { ride_set(r, 11); return; }
	if (!(r->coll & COL_D)) { ride_set(r, 12); return; }
	if (!(r->f97 & RF_KNOCK)) {
		if (!ride_face(r)) { ride_set(r, 1); return; }
		if (r->key_trg & KEY_JUMP) { ride_set(r, 4); return; }
		if (ride_can_attack(r)) { ride_set(r, 3); return; }
		r->spd_x = r->flip ? 0x16000 : -0x16000;
		r->x.l += r->spd_x;
	}
	AnimeTick((OBJECT *)r);
	if (r->anime.flag) { PlaySound(2, 42, 0); r->anime.flag = 0; }
}

/* step 3: punch, two in a row (0x800EEFF0) */
void RidePunch(RIDE *r)
{
	if (r->act2 == 0) {
		r->act2 = 1;
		if (r->combo == 1) {
			ride_punch_obj(r, 62, 2); SetAnime((OBJECT *)r, 10); x_anime(r, 50); PlaySound(2, 43, 0);
		} else {
			ride_punch_obj(r, 61, 0); SetAnime((OBJECT *)r, 8); x_anime(r, 49); PlaySound(2, 44, 0);
		}
		r->queued = 0;
	}
	if (r->anime.next == 0) {
		if (r->queued == 0) { r->combo = 0; ride_set(r, 1); }
		else ride_set(r, 3);
		return;
	}
	if ((r->anime.flag & ANM_FIRE) && ride_can_attack(r) && r->queued == 0 && r->combo < 2) {
		r->combo++; r->queued = 1;
	}
	AnimeTick((OBJECT *)r);
}

/* step 4: jump (0x800EF288) */
void RideJump(RIDE *r)
{
	if (r->act2 == 0) {
		r->spd_y = 0x58000; r->grav = 0x4200; r->act2 = 1; r->spd_x = r->acc_x = 0; r->fall_f = 1;
		SpawnEffect(r, 12, 6);
		SetAnime((OBJECT *)r, 2); x_anime(r, 43); r->combo = 0;
		PlaySound(2, 39, 0);
	}
	if (ride_can_attack(r)) { ride_set(r, 6); return; }
	if (ride_leave(r)) { ride_set(r, 11); return; }
	if (r->f97 & RF_KNOCK) return;
	r->spd_x = 0;
	if (ride_face(r)) r->spd_x = r->flip ? (r->move_spd << 8) : -(r->move_spd << 8);
	if ((r->coll & COL_U) || r->spd_y < 0 || !(r->key & KEY_JUMP)) { ride_set(r, 12); return; }
	MoveObject2((OBJECT *)r);
}

/* step 7: dash (0x800EF950) */
void RideDash(RIDE *r)
{
	int fwd;

	if (r->act2 == 0) {
		r->act2 = 1; r->move_spd = 1056;
		r->spd_x = r->flip ? 0x42000 : -0x42000;
		r->timer = 30; SetAnime((OBJECT *)r, 5); x_anime(r, 47);
		PlaySound(2, 41, 0); SpawnEffect(r, 12, 5);
	}
	if (ride_leave(r)) { ride_set(r, 11); return; }
	if (r->f97 & RF_KNOCK) { AnimeTick((OBJECT *)r); return; }
	if (r->key_trg & KEY_JUMP) { ride_set(r, 4); return; }
	if (ride_can_attack(r)) { ride_set(r, 9); return; }
	if (!(r->coll & COL_D)) { ride_set(r, 12); return; }
	if (r->coll & (COL_R | COL_L)) { ride_set(r, 8); return; }
	fwd = r->key & (r->flip ? KEY_R : KEY_L);
	if (!fwd && !((r->f97 & RF_DASHKEY) && (r->key & KEY_DASH))) { ride_set(r, 8); return; }
	r->x.l += r->spd_x;
	if ((char)r->timer-- < 0) { ride_set(r, 8); return; }
	AnimeTick((OBJECT *)r);
}

/* step 11: X leaves (0x800EFFCC) */
void RideLeave(RIDE *r)
{
	if (r->act2 == 0) {
		r->act2 = 1; r->muteki = 1; r->col_box = &ride_col_box;
		r->blink = 0; r->charge = 0; r->charge_key = 0;
		if (!(r->f97 & RF_KNOCK)) { r->spd_x = r->acc_x = 0; r->spd_y = 0x20000; r->grav = 0x4000; }
		if (r->f97 & RF_INSIDE) {
			if (!(r->f97 & RF_KNOCK)) r->spd_y = r->grav = 0;
			MEGA_RIDE(&mega) = 0;			/* PlayerRide: terrain box, StartJump */
			r->f97 ^= RF_INSIDE;
			PlayerSetTerrainBox(&mega);
		}
		if (!(r->coll & COL_D)) ride_set(r, 12);
		else SetAnime((OBJECT *)r, 19);
		r->clut &= ~CLUT_FLASH;
	}
	if (r->coll & COL_D) r->spd_y = 0;
	if (r->anime.next == 0) { ride_set(r, 0); r->blink = 0; r->charge = 0; r->charge_key = 0; }
	if (!(r->f97 & RF_KNOCK)) MoveObject2((OBJECT *)r);
	AnimeTick((OBJECT *)r);
}

/* knockback after a hit (0x800F1C80) */
static void RideKnockback(RIDE *r)
{
	long s;

	if (r->kb == 1) {
		r->kb = 2;
		r->save[0] = r->spd_x >> 8; r->save[1] = r->spd_y >> 8;
		r->save[2] = r->acc_x >> 8; r->save[3] = r->grav >> 8;
		r->spd_x = r->flip ? -(r->kb_spd << 8) : (r->kb_spd << 8);
		r->acc_x = 0x5800; r->spd_y = 0; r->grav = 0x5800;
	} else if (r->kb == 2) {
		MoveObject2((OBJECT *)r);
		s = r->spd_x < 0 ? -r->spd_x : r->spd_x;
		if (s < 0x4800) {
			r->kb = 0; r->f97 ^= RF_KNOCK;
			r->spd_x = r->save[0] << 8; r->acc_x = r->save[2] << 8; r->grav = r->save[3] << 8;
		}
		if (r->coll & COL_D) r->spd_y = 0;
	}
}

/* camera limits and the pit (0x800F1DA8) */
static void RideBounds(RIDE *r)
{
	CAMERA *c = &camera[r->layer];

	if (r->f97 & RF_INSIDE) {
		if (POS_X(r) + 32 >= c->max_x + 320) POS_X(r) = c->max_x + 288;
		if (POS_X(r) - 32 < c->min_x && !(r->coll & COL_R)) POS_X(r) = c->min_x + 32;
	}
	if (POS_Y(r) - 32 >= c->max_y + 240) {
		r->act = 2;
		if (r->f97 & RF_INSIDE) ((OBJECT *)&mega)->hp = (char)0x80;
	}
}

/* state 1 (0x800F1EC4) */
void RideMain(RIDE *r)
{
	int i;

	r->disp = 0;
	if (OffScreenCheck((OBJECT *)r)) return;
	if (MEGA_RIDE(&mega) && (r->coll & COL_R) && POS_X(r) - camera[0].x < 32) {
		((OBJECT *)&mega)->hp = r->hp = (char)0x80;
		for (i = 0; i < 8; i++) SpawnExplosionRand((OBJECT *)r, 0, 32, 32);
		DeleteObject3((OBJECT *)r);
		game.stage_flag = 0; r->coll = 0;
		return;
	}
	if (r->hp < 0) {
		if (r->hp == (char)0x80) {
			if (!(r->f97 & RF_ZERO)) { r->f97 |= RF_ZERO; r->cd_cnt = r->cd_int = 30; r->blink = 0; }
			r->hp = 32;
		}
		r->kb_spd = 0x400; r->blink = 121;
		SpawnEffect(r, 12, 8);
		r->kb = 1; r->lock = 1; r->f97 |= RF_KNOCK;
	}
	if (r->blink && --r->blink == 0 && (r->f97 & RF_LEAVE)) ((OBJECT *)&mega)->disp = 1;
	if ((r->f97 & (RF_INSIDE | RF_LEAVE)) == (RF_INSIDE | RF_LEAVE) || (game.exit_req && (r->f97 & RF_INSIDE))) {
		r->f97 ^= RF_LEAVE; ride_set(r, 11);
	}
	RideDoubleTap(r); RideDashKey(r); RideCharge(r);
	if (r->destroy == 0) {
		ride_steps[r->act](r);
		RideKnockback(r);
		CollideCheck((OBJECT *)r);
		if (r->f97 & RF_INSIDE) {
			((OBJECT *)&mega)->x = r->x; ((OBJECT *)&mega)->y = r->y; ((OBJECT *)&mega)->flip = r->flip;
			if (r->blink) {
				if ((r->blink & 3) == 0) r->clut |= CLUT_FLASH; else r->clut &= ~CLUT_FLASH;
			}
		}
		if (r->f97 & RF_ZERO) {
			r->clut &= ~CLUT_FLASH;
			if (--r->cd_cnt == 0) {
				r->clut |= CLUT_FLASH;
				r->cd_cnt = r->cd_int;
				if (--r->cd_int == 0) { r->destroy = 1; r->clut &= ~CLUT_FLASH; }
			}
		}
		Display((OBJECT *)r);
	}
	if (r->destroy == 1) {
		r->destroy = 2; r->blink = 255;
		MEGA_RIDE(&mega) = 0;
		r->f97 ^= RF_INSIDE;
		PlayerSetTerrainBox(&mega);
		r->timer = 30; r->f97 |= RF_GONE;
		SpawnJunk(7, ride_junk, (OBJECT *)r);
	} else if (r->destroy == 2) {
		if (--r->timer == 0) {
			r->act = 2; SpawnExplosion((OBJECT *)r); r->disp = 0; StopSound(2, 8);
		} else SpawnExplosionRand((OBJECT *)r, 0, 31, 31);
	} else if (r->blink == 0) {
		r->lock = 0; r->clut &= ~CLUT_FLASH;
	}
	RideBounds(r);
}

/* state 0 (0x800EE1D4) */
void RideInit(RIDE *r)
{
	int bank;

	r->disp = 1; r->act = 1; r->act2 = 0; r->layer = 0;
	r->flag |= 0x40;
	r->flip = r->var ? OBJ_FLIP_L : OBJ_FLIP_R;
	r->var = 0;
	r->anime_tbl = ride_anime_tbl;
	bank = GetSpriteBank(225);
	r->sprt_p = sprite_bank_base + sprite_bank_base_ofs[bank];
	r->tpage = bank_tpage[bank] >> 7;
	r->clut = BankClut(bank);
	r->pri = 3; r->col_box = &ride_col_box;
	r->pad7c = 1; r->hp = 32;
	r->atk = 0; r->muteki = 0; r->fall_f = 0;
	r->spd_x = r->spd_y = r->acc_x = r->grav = 0;
	r->dmg_box = r->atk_box = 0; r->dmg_tbl = 0;
	/* +0x7D..+0x87, +0x8C..+0x93, +0x97, +0xA6 cleared */
}

/* state 2 (0x800F250C) */
void RideRemove(RIDE *r)
{
	if (r->set) r->set->flag = 0x80;
	game.stage_flag = 0;
	r->coll = 0;
	r->flag = r->var = r->disp = r->act = r->act2 = r->act3 = r->act4 = 0;
}

/* actor2 type 0 (0x800F2414) */
void RideEntry(RIDE *r)
{
	static void (*state[])(RIDE *) = { RideInit, RideMain, RideRemove };	/* 0x800F3A50 */

	if (MEGA_EVENT(&mega) == 0) { r->key = mega.key; r->key_trg = mega.key_trg; }
	else r->key = r->key_trg = 0;
	if (MEGA_STOPPED(&mega)) { Display((OBJECT *)r); return; }
	r->old_x = r->x; r->old_y = r->y;
	r->coll = r->col_f | r->push_f; r->push_f = 0;
	state[r->act](r);
	r->hp &= 0x7F;
	game.stage_flag = (r->coll & (COL_U | COL_D)) == (COL_U | COL_D);
}

/* stage logic type 30 (0x800EE158) */
void RideSpawner(OBJECT *o)
{
	o->act = 1; o->act2 = 0;
	actor2.flag = 0x41;
	actor2.act = actor2.act2 = actor2.act3 = actor2.act4 = 0;
	actor2.type = o->var; actor2.var = o->var;
	actor2.set = o->set; actor2.x = o->x; actor2.y = o->y;
	DeleteObjectKeep(o);
}
