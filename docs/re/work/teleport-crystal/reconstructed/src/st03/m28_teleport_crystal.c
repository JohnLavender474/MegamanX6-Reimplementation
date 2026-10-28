/*
 *	m28_teleport_crystal.c  --  main object 28: teleporter crystal (stage 03)
 */
#include "object.h"
#include "game.h"
#include "sound.h"

typedef struct {
	short	x;
	short	y;
	short	cam[3][2];
	short	min_x;
	short	max_x;
	short	min_y;
	short	max_y;
	u_char	pad[8];
} WARP;

#define DEST(op)	(*(short *)&(op)->work[0x00])
#define IMG_A(op)	(*(short *)&(op)->work[0x02])
#define IMG_B(op)	(*(short *)&(op)->work[0x04])
#define RET_X(op)	(*(short *)&(op)->work[0x06])
#define SAVE_COL(op)	(*(HITBOX **)&(op)->work[0x0a])

#define SPLIT_TIME	29
#define AREA_DELAY	90
#define FADE_SPEED	8

static HITBOX	atk_box_tc = {   0,   0, 16, 24 };
static HITBOX	dmg_box_tc = { -18, -28, 28, 40 };
static HITBOX	col_box_tc = { -18, -28, 28, 40 };

static ANIME	anime_tc0[] = {
	{ 3, 0, 1, 0 }, { 3, 0, 1, 1 }, { 3, 0, 1, 2 },
	{ 3, 0, 1, 3 }, { 3, 0, 1, 4 }, { 3, 0, -5, 5 },
};
static ANIME	anime_tc1[] = {
	{ 3, 0, 1, 6 }, { 3, 0, 1, 7 }, { 3, 0, 1, 8 },
	{ 3, 0, 1, 9 }, { 3, 0, 1, 10 }, { 3, 0, -5, 11 },
};
static ANIME	*anime_tbl_tc[] = { anime_tc0, anime_tc1 };

static WARP	warp_tbl[4] = {
	{   72, 1115, { {    0,  960 }, { 1664,    0 }, {   0, 240 } },    0,  368,  960,  960 },
	{  120, 2074, { {    0, 1904 }, {    0, 1904 }, {   0, 476 } },    0, 6080, 1904, 1904 },
	{ 1952, 1115, { { 1920,  960 }, { 1664,    0 }, { 480, 240 } }, 1920, 2608,  960,  960 },
	{ 1952, 1114, { { 1920,  960 }, { 1664,    0 }, { 480, 240 } }, 1920, 2608,  960,  960 },
};

static void	tc_init(OBJECT *op);
static void	tc_main(OBJECT *op);
static void	tc_delete(OBJECT *op);
static void	tc_touch(OBJECT *op);
static void	tc_area(OBJECT *op);
static void	tc_move(OBJECT *op);
static void	tc_line_delay(OBJECT *op);
static void	tc_line(OBJECT *op);
static void	area_start(OBJECT *op);
static void	area_split(OBJECT *op);
static void	area_wait(OBJECT *op);
static void	move_start(OBJECT *op);
static void	move_split(OBJECT *op);
static void	move_jump(OBJECT *op);
static void	move_join(OBJECT *op);
static void	move_end(OBJECT *op);
static void	move_kill(OBJECT *op);

static void	(*tc_act_tbl[])(OBJECT *) = { tc_init, tc_main, tc_delete, tc_delete };
static void	(*tc_act2_tbl[])(OBJECT *) = {
	HitReactionDispatch, tc_touch, tc_area, tc_move, tc_line_delay, tc_line
};
static void	(*tc_area_tbl[])(OBJECT *) = { area_start, area_split, area_wait };
static void	(*tc_move_tbl[])(OBJECT *) = {
	move_start, move_split, move_jump, move_join, move_end, move_kill
};

static void tc_init(OBJECT *op)
{
	op->atk = 4;
	op->hp = 32;
	op->muteki = 0;
	op->fall_f = 0;
	op->col_f = 0;
	op->layer = mega.layer;
	op->old_x = op->x;
	op->old_y = op->y;
	if (mega.x.l < op->x.l)
		op->flip = 0;
	else
		op->flip = 0x40;
	op->anime_tbl = anime_tbl_tc;
	op->pri = 6;
	op->col_box = &col_box_tc;
	op->dmg_box = &dmg_box_tc;
	op->atk_box = &atk_box_tc;

	switch (op->var) {
	case 0:
		SetAnime(op, 0);
		op->act2 = 1;
		break;
	case 1:
		SetAnime(op, 0);
		op->act2 = 1;
		break;
	case 2:
		SetAnime(op, 0);
		op->act2 = 1;
		break;
	case 3:
		op->disp = 0;
		op->act2 = 4;
		break;
	case 4:
		op->disp = 0;
		op->act2 = 5;
		break;
	}
	op->timer = 0;
	op->act = 1;
	op->act3 = 0;
}

static void area_start(OBJECT *op)
{
	if (mega.hp == 0)
		return;

	bg_layer[0].follow = 0;
	bg_layer[1].follow = 0;
	bg_layer[2].follow = 0;
	STOP_ALL();
	op->flag |= 0x08;
	SAVE_COL(op) = mega.col_box;
	mega.col_box = 0;
	mega.act2 = 2;
	MegaStopWeapon(&mega);
	MegaStopCharge(&mega);
	MegaClearA1(&mega);
	MegaStopSound(&mega);
	ClearPlayerShots();
	op->timer = 0;
	IMG_A(op) = mega.x.w.hi;
	IMG_B(op) = mega.x.w.hi;
	op->act3++;
	PlaySound(SE_BANK_SYS1, SE_WARP, op);
}

static void area_wait(OBJECT *op)
{
	short a = ++IMG_A(op);
	short b = --IMG_B(op);

	if (frame_count & 1)
		mega.x.w.hi = b;
	else
		mega.x.w.hi = a;

	if (fade_dir == 0) {
		bg_layer[0].follow = 1;
		bg_layer[1].follow = 1;
		bg_layer[2].follow = 1;
		mega.x.w.hi = RET_X(op);
		GO_ALL();
		mega.col_box = SAVE_COL(op);
	}
}

static void move_start(OBJECT *op)
{
	if (op->timer != 0) {
		op->timer--;
		return;
	}
	PlaySound(SE_BANK_SYS1, SE_WARP, op);
	bg_layer[0].follow = 0;
	bg_layer[1].follow = 0;
	bg_layer[2].follow = 0;
	STOP_ALL();
	op->flag |= 0x08;
	SAVE_COL(op) = mega.col_box;
	mega.col_box = 0;
	mega.act2 = 2;
	MegaStopWeapon(&mega);
	MegaStopCharge(&mega);
	MegaClearA1(&mega);
	MegaStopSound(&mega);
	ClearPlayerShots();
	op->timer = 0;
	IMG_A(op) = mega.x.w.hi;
	IMG_B(op) = mega.x.w.hi;
	op->act3++;
}

static void move_jump(OBJECT *op)
{
	short a = ++IMG_A(op);
	short b = --IMG_B(op);
	WARP *wp;
	int i;

	if (frame_count & 1)
		mega.x.w.hi = b;
	else
		mega.x.w.hi = a;

	if (fade_dir != 0)
		return;

	switch (op->var) {
	case 2:
		if (PLAYER_CHAR() == PLAYER_X) {
			DEST(op) = 0;
			game.checkpoint = 1;
		} else {
			DEST(op) = 1;
			game.checkpoint = op->var;
		}
		checkpoint_set = 1;
		break;
	case 3:
		game.checkpoint = 3;
		DEST(op) = 2;
		break;
	case 4:
		game.checkpoint = 3;
		DEST(op) = 3;
		break;
	}

	wp = &warp_tbl[DEST(op)];
	mega.x.w.hi = wp->x;
	mega.y.w.hi = wp->y;
	IMG_A(op) = wp->x + 64;
	IMG_B(op) = wp->x - 64;
	if (PLAYER_CHAR() != PLAYER_X)
		mega.y.w.hi--;
	mega.nohit_f = 0;
	game.unk1c = 0;
	MEGA_B(0xe0) = 0;
	mega.old_x = mega.x;
	mega.old_y = mega.y;
	ResetLayerCache(0);
	bg_refresh = 1;
	MEGA_B(0xe4) = 0;

	for (i = 0; i < 3; i++)
		bg_layer[i].follow = 1;
	for (i = 0; i < 3; i++) {
		bg_layer[i].x = wp->cam[i][0];
		bg_layer[i].y = wp->cam[i][1];
	}
	bg_layer[0].min_x = wp->min_x;
	bg_layer[0].max_x = wp->max_x;
	bg_layer[0].min_y = wp->min_y;
	bg_layer[0].max_y = wp->max_y;
	bg_layer[0].min_x2 = wp->min_x;
	bg_layer[0].max_x2 = wp->max_x;
	bg_layer[0].min_y2 = wp->min_y;
	bg_layer[0].max_y2 = wp->max_y;
	for (i = 0; i < 3; i++)
		bg_layer[i].redraw = 1;
	mega.fall_f = 0;
	ClearPlayerShots();
	MEGA_B(0xe4) = 0;
	MEGA_B(0xa4) = 0;
	mega.muteki = 0;
	SpawnInRect(bg_layer[0].x - 48, bg_layer[0].x + 368,
		    bg_layer[0].y - 48, bg_layer[0].y + 288, 0);
	FadeIn(FADE_SPEED);
	for (i = 0; i < 3; i++)
		bg_layer[i].follow = 0;
	op->act3++;
	PlaySound(SE_BANK_SYS1, SE_WARP, op);
}

static void move_end(OBJECT *op)
{
	short a, b;

	op->timer++;
	a = --IMG_A(op);
	b = ++IMG_B(op);
	if (frame_count & 1)
		mega.x.w.hi = b;
	else
		mega.x.w.hi = a;

	if (op->timer < SPLIT_TIME)
		return;

	bg_layer[0].follow = 1;
	bg_layer[1].follow = 1;
	bg_layer[2].follow = 1;
	mega.x.w.hi = warp_tbl[DEST(op)].x;
	mega.col_box = SAVE_COL(op);
	MegaResume(&mega);
	GO_ALL();
	op->flag &= ~0x08;
	op->act3++;
}

void m28_teleport_crystal(OBJECT *op)
{
	(*tc_act_tbl[op->act])(op);
	CollideCheck(op);
}

static void tc_main(OBJECT *op)
{
	op->old_x = op->x;
	op->old_y = op->y;
	AnimeTick(op);
	(*tc_act2_tbl[op->act2])(op);
	if ((u_char)(op->var - 3) >= 2)
		DisplayCheck(op, 64, 64);
}

static void tc_delete(OBJECT *op)
{
	if (op->act == 2)
		DeleteObject(op);
	else
		DeleteObject2(op);
}

static void tc_touch(OBJECT *op)
{
	if (!BoxOverlap(op, &mega))
		return;

	if (op->var == 2) {
		op->timer = 0;
		op->act2 = 3;
		op->act3 = 0;
		RET_X(op) = mega.x.w.hi;
	} else {
		op->act2 = 2;
		op->act3 = 0;
		RET_X(op) = mega.x.w.hi;
	}
}

static void tc_line_delay(OBJECT *op)
{
	if (game.char_state != 0)
		return;
	if (mega.x.w.hi < op->x.w.hi)
		return;
	RET_X(op) = mega.x.w.hi;
	op->timer = AREA_DELAY;
	op->act2 = 3;
	op->act3 = 0;
}

static void tc_line(OBJECT *op)
{
	if (mega.x.w.hi < op->x.w.hi)
		return;
	RET_X(op) = mega.x.w.hi;
	op->timer = 0;
	op->act2 = 3;
	op->act3 = 0;
}

static void tc_area(OBJECT *op)
{
	(*tc_area_tbl[op->act3])(op);
}

static void tc_move(OBJECT *op)
{
	(*tc_move_tbl[op->act3])(op);
}

static void area_split(OBJECT *op)
{
	short a, b;

	op->timer++;
	a = ++IMG_A(op);
	b = --IMG_B(op);
	if (frame_count & 1)
		mega.x.w.hi = b;
	else
		mega.x.w.hi = a;

	if (op->timer < SPLIT_TIME)
		return;

	FadeOut(FADE_SPEED);
	game.exit_f = op->var | 0x40;
	op->act3++;
}

static void move_split(OBJECT *op)
{
	short a, b;

	op->timer++;
	a = ++IMG_A(op);
	b = --IMG_B(op);
	if (frame_count & 1)
		mega.x.w.hi = b;
	else
		mega.x.w.hi = a;

	if (op->timer < SPLIT_TIME)
		return;

	FadeOut(FADE_SPEED);
	op->act3++;
}

static void move_join(OBJECT *op)
{
	short a = --IMG_A(op);
	short b = ++IMG_B(op);

	if (frame_count & 1)
		mega.x.w.hi = b;
	else
		mega.x.w.hi = a;

	if (fade_dir == 0) {
		op->timer = 0;
		op->act3++;
	}
}

static void move_kill(OBJECT *op)
{
	DeleteObject3(op);
}
