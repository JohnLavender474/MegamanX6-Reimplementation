/*
 *	m14_weather_analyze.c
 *
 *	ST09 (Rainy Turtloid)  main object 14 : Weather Analyze
 *	  var <= 0 or >= 7 : the machine (record)   var 1-4 : sensors
 *	  var 5 : display                          var 6   : barrier
 */
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>

#include "object.h"
#include "game.h"
#include "sound.h"

enum { ACT_INIT = 0, ACT_MAIN, ACT_DEAD, ACT_GONE };
enum { MODE_REACT = 0, MODE_NONE, MODE_WAIT };

#define WA_HP		32
#define WA_GRAV		0x40000
#define WA_PARTS	4
#define WA_SPAWNED	4
#define WA_INIT_CNT	5
#define SENSOR_HP	16
#define SENSOR_ATK	4

#define WA_ROWS		7

typedef struct { short x, y; } POINT16;
typedef struct { short lo, hi; } RANGE16;

static u_char	wa_index;					/* 0x800F6BA0 */

extern HITBOX	wa_col_box;					/* 0x800F5CB0  0, 0, 20, 32 */
extern HITBOX	wa_dmg_box;					/* 0x800F5CB4  -16, -32, 32, 64 */
extern HITBOX	sensor_atk_box;					/* 0x800F5CB8  -6, -6, 12, 12 */
extern HITBOX	sensor_dmg_box;					/* 0x800F5CBC  -6, -6, 12, 12 */
extern HITBOX	barrier_atk_box;				/* 0x800F5CC0  -24, -32, 48, 64 */
extern u_char	sensor_junk[];					/* 0x800F5D94  10, 11, 12 */
extern u_char	wa_junk[];					/* 0x800F5D98  14, 15, 16, 17 */
extern ANIME	*wa_anime_tbl[];				/* 0x800F5D9C */
extern POINT16	wa_pos[WA_ROWS];				/* 0x800F5DF4 */
extern POINT16	sensor_pos[WA_ROWS][WA_PARTS];			/* 0x800F5E10 */
extern short	wa_cam_right[];					/* 0x800F5E80 */
extern RANGE16	wa_zone[WA_ROWS];				/* 0x800F5E90 */
extern BLOCKCOPY wa_tiles[2];					/* 0x800F5EAC */

static void	wa_none(OBJECT *op);
static void	wa_wait(OBJECT *op);
static void	sensor_none(OBJECT *op);
static void	sensor_none2(OBJECT *op);

static void (*wa_mode_tbl[])(OBJECT *) = {			/* 0x800F5EBC */
	HitReactionDispatch, wa_none, wa_wait
};
static void (*sensor_mode_tbl[])(OBJECT *) = {			/* 0x800F5ED8 */
	HitReactionDispatch, sensor_none, sensor_none2
};

/* 0x800EB2D4 */
static void wa_main(OBJECT *op)
{
	int	ret = ContactWeapon(op);

	if (mega.x.w.hi < wa_zone[wa_index].lo || wa_zone[wa_index].hi < mega.x.w.hi)
		game.stage_f = 0;
	else
		game.stage_f = 1;

	if (ret < 0) {
		if (wa_index == 5)
			camera.limit_l = camera.limit_r;
		if (wa_cam_right[wa_index])
			camera.limit_r = wa_cam_right[wa_index];
		if (wa_index == 1) {
			camera.limit_b = 224;
			CopyBlock(0, 2592, 96, &wa_tiles[0]);
			CopyBlock(0, 2832, 0, &wa_tiles[1]);
		}
		game.stage_f = 0;
		SpawnExplosion(op);
		SpawnJunk(4, wa_junk, op);
		SetAnime(op, 13);
		op->act = ACT_DEAD;
		op->clut &= ~CLUT_FLASH;
		DisplayCheck(op, 40, 64);
		return;
	}
	ContactMega(op);
	if (!OffScreenCheck2(op, 40, 64))
		DisplayCheck(op, 40, 64);
	wa_mode_tbl[op->act2](op);
	MoveObject2(op);
	AnimeTick(op);
}

/* 0x800EB4D4 */
static void sensor_init(OBJECT *op)
{
	OBJECT	*pp = *(OBJECT **)&op->work[2];		/* +0x80: the machine */
	int	n = op->var - 1;

	op->tpage = pp->tpage;
	op->anime_tbl = pp->anime_tbl;
	op->sprt_p = pp->sprt_p;
	op->clut = pp->clut;
	op->pri = 6;
	op->atk_box = &sensor_atk_box;
	op->fall_f = 0;
	op->disp = 0;
	op->col_box = 0;
	op->dmg_box = &sensor_dmg_box;
	op->dmg_tbl = dmg_tbl_normal;
	op->muteki = 0;
	op->flip = OBJ_FLIP_L;
	op->layer = mega.layer;
	op->x.w.hi = sensor_pos[wa_index][n].x;
	op->y.w.hi = sensor_pos[wa_index][n].y;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
	op->hp = SENSOR_HP;
	op->atk = SENSOR_ATK;
	SetAnime(op, 9);
	op->act2 = MODE_WAIT;
	op->act3 = 0;
	op->act++;
}

static void wa_sensor(OBJECT *op);
static void wa_display(OBJECT *op);
static void wa_barrier(OBJECT *op);

static void (*wa_act_tbl[])(OBJECT *);				/* 0x800F5DE4 */

/* 0x800EB5F8 */
void m14_weather_analyze(OBJECT *op)
{
	switch (op->var) {
	case 1: case 2: case 3: case 4:
		wa_sensor(op);
		return;
	case 5:
		wa_display(op);
		return;
	case 6:
		wa_barrier(op);
		return;
	}
	op->old_x = op->x;
	op->disp = 0;
	op->old_y = op->y;
	wa_act_tbl[op->act](op);
	CollideCheck(op);
}

/* 0x800EB6C0 */
static void wa_init(OBJECT *op)
{
	wa_index = op->var & 0x0f;
	op->var = 0;
	op->anime_tbl = wa_anime_tbl;
	op->pri = 6;
	op->col_box = &wa_col_box;
	op->dmg_tbl = dmg_tbl_normal;
	op->fall_f = 0;
	op->disp = 0;
	op->atk_box = 0;
	op->dmg_box = 0;
	op->muteki = 0;
	op->flip = OBJ_FLIP_L;
	op->spd_x = op->spd_y = op->acc_x = 0;
	op->grav = WA_GRAV;
	op->hp = WA_HP;
	op->atk = 0;
	op->layer = mega.layer;
	SetAnime(op, 0);
	op->x.w.hi = wa_pos[wa_index].x;
	op->y.w.hi = wa_pos[wa_index].y;
	game.stage_f = 1;
	op->act2 = MODE_WAIT;
	op->act3 = 0;
	op->act4 = WA_INIT_CNT;
	op->act++;
}

/* 0x800EB7D8 */
static void wa_none(OBJECT *op)
{
}

/* 0x800EB7E0 */
static void wa_wait(OBJECT *op)
{
	OBJECT	*np;
	int	i;

	if ((op->col_f & COL_D) && op->act4 == WA_INIT_CNT) {
		for (i = 1; i < 7; i++) {
			if ((np = GetMainObject()) != 0) {
				np->flag = op->flag;
				np->var = i;
				*(OBJECT **)&np->work[2] = op;
				np->type = op->type;
			}
		}
		op->act4 = WA_SPAWNED;
	}
	if (op->act4 == 0 && op->dmg_box == 0)
		op->dmg_box = &wa_dmg_box;
}

/* 0x800EB89C */
static void wa_dead(OBJECT *op)
{
	if (wa_index == 3)
		game.stage_f3 = 1;
	op->act++;
	DisplayCheck(op, 40, 64);
}

/* 0x800EB8F0 */
static void wa_gone(OBJECT *op)
{
	DisplayCheck(op, 40, 64);
}

static void (*wa_act_tbl[])(OBJECT *) = {
	wa_init, wa_main, wa_dead, wa_gone
};

static void sensor_main(OBJECT *op);
static void sensor_delete(OBJECT *op);

static void (*sensor_act_tbl[])(OBJECT *) = {			/* 0x800F5EC8 */
	sensor_init, sensor_main, sensor_delete, sensor_delete
};

/* 0x800EB914 */
static void wa_sensor(OBJECT *op)
{
	op->old_x = op->x;
	op->disp = 0;
	op->old_y = op->y;
	sensor_act_tbl[op->act](op);
	CollideCheck(op);
}

/* 0x800EB974 */
static void sensor_main(OBJECT *op)
{
	OBJECT	*pp;

	if (ContactWeapon(op) < 0) {
		pp = *(OBJECT **)&op->work[2];
		pp->act4--;
		SpawnExplosion(op);
		SpawnJunk(3, sensor_junk, op);
		op->act = ACT_DEAD;
		return;
	}
	ContactMega(op);
	if (!OffScreenCheck2(op, 16, 16))
		DisplayCheck(op, 16, 16);
	sensor_mode_tbl[op->act2](op);
	MoveObject2(op);
	AnimeTick(op);
}

/* 0x800EBA40 */
static void sensor_none(OBJECT *op)
{
}

/* 0x800EBA48 */
static void sensor_none2(OBJECT *op)
{
}

/* 0x800EBA50 */
static void sensor_delete(OBJECT *op)
{
	if (op->act == ACT_DEAD)
		DeleteObject(op);
	else
		DeleteObject2(op);
}

static void display_init(OBJECT *op);
static void display_main(OBJECT *op);
static void display_delete(OBJECT *op);

static void (*display_act_tbl[])(OBJECT *) = {			/* 0x800F5EE4 */
	display_init, display_main, display_delete, display_delete
};

/* 0x800EBA90 */
static void wa_display(OBJECT *op)
{
	op->old_x = op->x;
	op->disp = 0;
	op->old_y = op->y;
	display_act_tbl[op->act](op);
	CollideCheck(op);
}

/* 0x800EBAF0 */
static void display_init(OBJECT *op)
{
	OBJECT	*pp = *(OBJECT **)&op->work[2];

	op->tpage = pp->tpage;
	op->anime_tbl = pp->anime_tbl;
	op->sprt_p = pp->sprt_p;
	op->clut = pp->clut;
	op->pri = 4;
	op->fall_f = 0;
	op->disp = 0;
	op->col_box = 0;
	op->atk_box = 0;
	op->dmg_box = 0;
	op->dmg_tbl = 0;
	op->muteki = 0;
	op->flip = OBJ_FLIP_L;
	op->layer = mega.layer;
	op->x.w.hi += pp->x.w.hi;
	op->y.w.hi += pp->y.w.hi;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
	op->hp = 0;
	op->atk = 0;
	SetAnime(op, 1);
	op->act2 = 0;
	op->act3 = 0;
	op->act++;
}

/* 0x800EBBD4 */
static void display_main(OBJECT *op)
{
	OBJECT	*pp = *(OBJECT **)&op->work[2];

	switch (pp->act4) {
	case 0:
		op->act = ACT_DEAD;
		return;
	case 1:
		if (op->anime_no != 4)
			SetAnime(op, 4);
		break;
	case 2:
		if (op->anime_no != 3)
			SetAnime(op, 3);
		break;
	case 3:
		if (op->anime_no != 2)
			SetAnime(op, 2);
		break;
	}
	if (!OffScreenCheck2(op, 48, 64))
		DisplayCheck(op, 48, 64);
	MoveObject2(op);
	AnimeTick(op);
}

/* 0x800EBCD4 */
static void display_delete(OBJECT *op)
{
	if (op->act == ACT_DEAD)
		DeleteObject(op);
	else
		DeleteObject2(op);
}

static void barrier_init(OBJECT *op);
static void barrier_main(OBJECT *op);
static void barrier_delete(OBJECT *op);

static void (*barrier_act_tbl[])(OBJECT *) = {			/* 0x800F5EF4 */
	barrier_init, barrier_main, barrier_delete, barrier_delete
};

/* 0x800EBD14 */
static void wa_barrier(OBJECT *op)
{
	op->old_x = op->x;
	op->disp = 0;
	op->old_y = op->y;
	barrier_act_tbl[op->act](op);
	CollideCheck(op);
}

/* 0x800EBD74 */
static void barrier_init(OBJECT *op)
{
	OBJECT	*pp = *(OBJECT **)&op->work[2];

	op->tpage = pp->tpage;
	op->anime_tbl = pp->anime_tbl;
	op->sprt_p = pp->sprt_p;
	op->clut = pp->clut;
	op->atk_box = &barrier_atk_box;
	op->pri = 5;
	op->fall_f = 0;
	op->disp = 0;
	op->col_box = 0;
	op->dmg_box = 0;
	op->dmg_tbl = 0;
	op->muteki = 0;
	op->flip = OBJ_FLIP_L;
	op->layer = mega.layer;
	op->x.w.hi += pp->x.w.hi;
	op->y.w.hi += pp->y.w.hi;
	op->spd_x = op->spd_y = op->acc_x = op->grav = 0;
	op->hp = 0;
	op->atk = 0;
	SetAnime(op, 5);
	op->act2 = 0;
	op->act3 = 0;
	op->act++;
}

/* 0x800EBE60 */
static void barrier_main(OBJECT *op)
{
	OBJECT	*pp = *(OBJECT **)&op->work[2];

	switch (pp->act4) {
	case 0:
		op->act = ACT_DEAD;
		return;
	case 1:
		if (op->anime_no != 8)
			SetAnime(op, 8);
		break;
	case 2:
		if (op->anime_no != 7)
			SetAnime(op, 7);
		break;
	case 3:
		if (op->anime_no != 6)
			SetAnime(op, 6);
		break;
	}
	ContactMega(op);
	if (!OffScreenCheck2(op, 48, 64))
		DisplayCheck(op, 48, 64);
	MoveObject2(op);
	AnimeTick(op);
}

/* 0x800EBF64 */
static void barrier_delete(OBJECT *op)
{
	if (op->act == ACT_DEAD)
		DeleteObject(op);
	else
		DeleteObject2(op);
}
