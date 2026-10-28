/*
 *	player.h  --  the player object (mega, 0x800970A0) as far as part 2a reads it
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
	u_char	pad5d[10];
	char	jump_latch;
	HITBOX	*col_box;
	u_char	pad6c[4];
	u_char	col_f;
	u_char	touch;
	u_char	on_obj;
	u_char	pad73[7];
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
	u_char	pad8d;
	char	shot_pose;
	char	weapon_pose;
	char	pose_end;
	u_char	shot_timer;
	u_char	ladder_timer;
	u_char	pad93[3];
	char	weapon;
	u_char	pad97[0x32];
	u_char	skill;
	u_char	padca[9];
	u_char	lock;
	u_char	padd4;
	char	dash_box;
	u_char	padd6[0x11];
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

extern PLAYER	mega;

#endif
