/*
 *	player.h  --  the player object (mega, 0x800970A0) as far as parts 2a and 2c read it
 */
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "object.h"

typedef struct {
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
	u_char	buf_no;
	u_char	pad4a[6];
	HITBOX	*atk_box;
	HITBOX	*dmg_box;
	u_char	*dmg_tbl;
	char	hp;
	u_char	pad5d[4];
	char	iframes;		/* +0x61 */
	char	atk_class;		/* +0x62 (attacker) */
	char	hit_class;		/* +0x63 */
	u_char	pad64[3];
	char	jump_latch;
	HITBOX	*col_box;
	u_char	pad6c[4];
	u_char	col_f;
	u_char	touch;
	u_char	on_obj;
	u_char	pad73[3];
	u_char	obj76;			/* +0x76 */
	u_char	pad77[2];
	char	spike;			/* +0x79 */
	char	nohit_f;
	u_char	pad7b;
	u_short	key;
	u_short	pad7e;
	u_short	trg;
	u_short	pad82;
	char	dash_a;
	u_char	dash_timer;
	char	dash_b;
	char	dash_key;
	u_char	pad88;
	u_char	wall;
	u_char	pad8a;
	char	jump_work;
	char	dash_f;
	char	low_hp_timer;		/* +0x8D */
	char	shot_pose;
	char	weapon_pose;
	char	pose_end;
	u_char	shot_timer;
	u_char	ladder_timer;
	char	charge;			/* +0x93 */
	u_char	pad94[2];
	char	weapon;
	u_char	pad97[0x0d];
	char	hit_lock;		/* +0xA4 */
	u_char	hit_flip;		/* +0xA5 */
	u_char	pada6[2];
	short	energy[9];		/* +0xA8 .. +0xB8 */
	u_char	padba[0x0f];
	u_char	skill;
	char	grab;			/* +0xCA */
	u_char	padcb[3];
	u_char	beam_thru;		/* +0xCE */
	u_char	cf;			/* +0xCF */
	char	event;			/* +0xD0 */
	u_char	event_step;		/* +0xD1 */
	u_char	event_flip;		/* +0xD2 */
	u_char	lock;
	char	gate;			/* +0xD4 */
	char	ride;			/* +0xD5 (part 2a: dash_box) */
	char	death_wave;		/* +0xD6 */
	u_char	death_timer;		/* +0xD7 */
	u_char	ride_anime;		/* +0xD8 */
	u_char	padd9[7];
	char	no_drain;		/* +0xE0 */
	u_char	pade1;
	u_char	capsule;		/* +0xE2 */
	u_char	pade3[2];
	u_char	squeeze_timer;		/* +0xE5 */
	char	squeeze_dmg;		/* +0xE6 */
	u_char	form;
	u_char	pade8[4];
	u_long	parts;
	long	walk1_spd;		/* +0xF0 */
	long	walk_spd;		/* +0xF4 */
	long	jump_spd;		/* +0xF8 */
	long	fall_max;		/* +0xFC */
	long	k100;
	long	dash_spd;		/* +0x104 */
	long	dash_min;		/* +0x108 */
	long	dash_acc;		/* +0x10C */
	long	k110;
	long	k114;
	long	k118;
	long	k11c;
	long	k120;
	long	k124;
	long	k128;
	long	k12c;
	long	k130;
	long	jump_grav;		/* +0x134 */
} PLAYER;

#define KEY_R		0x0001
#define KEY_L		0x0002
#define KEY_U		0x0004
#define KEY_D		0x0008
#define KEY_JUMP	0x0080
#define KEY_DASH	0x0100

#define WALL_R		0x01
#define WALL_L		0x02

#define PART_WALK	0x04
#define PART_JUMP	0x08
#define PART_DASH	0x10

#define ST_STAND	2
#define ST_WALK1	3
#define ST_WALK		4
#define ST_ZSTOP	5
#define ST_JUMP		6
#define ST_LAND		8
#define ST_DASH		12
#define ST_CROUCH	26
#define ST_SJUMP	31

#define ANM_SHOT_OFS	112

#define FLG_FRAME	0x3f
#define FLG_EV20	0x20
#define FLG_EV40	0x40
#define FLG_EV80	0x80

/* part 2c */
#define FLOOR		0x08
#define HP_HIT		0x80		/* hp bit 7: a hit waits; 0x80 alone = dead */

#define PART_SHOCK	0x200
#define PART_BLINK	0x400
#define PART_REFILL	0x800

#define TILE_LADDER	32
#define TILE_LADDER_TOP	33

#define ST_BEAM_IN	0
#define ST_BEAM_OUT	1
#define ST_LADDER	14
#define ST_LADDER_UP	15
#define ST_LADDER_DN	16
#define ST_HURT		17
#define ST_RIDE		18
#define ST_EV_HOLD	20
#define ST_EV_WALK	21
#define ST_EV_WARP	22
#define ST_EV_JUMP	23
#define ST_EV_CAPSULE	24
#define ST_VICTORY	25
#define ST_EV_WALK1	29
#define ST_EV_LEAP	30
#define ST_X_LADDER_SHOT 32

#define PS_INIT		0
#define PS_MAIN		1
#define PS_DEATH	2
#define PS_NONE		3

extern PLAYER	mega;

#endif
