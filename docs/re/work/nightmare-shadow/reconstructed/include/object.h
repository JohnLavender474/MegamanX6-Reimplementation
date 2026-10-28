/*
 *	object.h  --  object work structures
 */
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <sys/types.h>

typedef union {
	long	l;
	struct { u_short lo; short hi; } w;
} FIXED;

typedef struct {
	char	ox;
	char	oy;
	u_char	w;
	u_char	h;
} HITBOX;

typedef struct {
	u_char	time;
	u_char	flag;
	char	next;
	u_char	sprt;
} ANIME;

typedef struct {
	u_char	flag;
	u_char	type;
	u_char	var;
	u_char	kind;
	short	x;
	short	y;
} ENEMYSET;

typedef struct _OBJECT {
	char	flag;
	char	type;
	char	var;
	char	disp;
	char	act;
	char	act2;
	char	act3;
	char	act4;
	FIXED	x;
	FIXED	y;
	ENEMYSET *set;
	char	layer;
	u_char	flip;
	char	pri;
	char	anime_no;
	FIXED	old_x;
	FIXED	old_y;
	long	spd_x;
	long	spd_y;
	long	acc_x;
	long	grav;
	ANIME	**anime_tbl;
	ANIME	*anime_p;
	u_char	*tex_p;
	char	*sprt_p;
	u_short	tpage;
	u_short	clut;
	ANIME	anime;
	u_char	anime_new;
	u_char	pad49[7];
	HITBOX	*atk_box;
	HITBOX	*dmg_box;
	u_char	*dmg_tbl;
	u_char	hp;
	u_char	hp_old;
	u_char	pad5e[2];
	char	atk;
	char	muteki;
	u_char	pad62;
	char	hit_kind;
	char	hit_id;
	char	hit_tag;
	u_char	pad66;
	char	fall_f;
	HITBOX	*col_box;
	short	inner_x;
	short	inner_y;
	u_char	col_f;
	u_char	pad71[9];
	char	nohit_f;
	u_char	pad7b;
	short	timer;
	u_char	work[0x1e];
} OBJECT;

#define OBJ_FLIP_L		0x00
#define OBJ_FLIP_R		0x40

#define COL_R			0x01
#define COL_L			0x02
#define COL_U			0x04
#define COL_D			0x08

#define CLUT_FLASH		0x8000

#define POS_X(o)		((o)->x.w.hi)
#define POS_Y(o)		((o)->y.w.hi)

#define ANM_FIRE		1
#define ANM_END			2
#define ANM_BACK		3

#define HIT_BLOCKED		127

#endif
