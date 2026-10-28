/*
 *	player.h  --  the player object (mega, 0x800970A0) as far as parts 2a and 2b read it
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
	char	wall_ok;		/* +0x4A: the side tile allows wall slide and kick */
	u_char	pad4b[5];
	HITBOX	*atk_box;
	HITBOX	*dmg_box;
	u_char	*dmg_tbl;
	char	hp;
	u_char	pad5d[10];
	char	jump_latch;		/* +0x67: 1 rising, -1 in the air, 0 on the ground */
	HITBOX	*col_box;
	u_char	pad6c[4];
	u_char	col_f;
	u_char	touch;
	u_char	on_obj;
	u_char	pad73[6];
	char	spike;			/* +0x79: touched a spike wall */
	char	nohit_f;
	u_char	pad7b;
	u_short	key;
	u_short	pad7e;
	u_short	trg;
	u_short	pad82;
	char	dash_a;			/* +0x84: air speed is dash_min while not 0 */
	u_char	dash_timer;
	char	dash_b;			/* +0x86: not 0 blocks the air dash */
	char	dash_key;
	u_char	pad88;
	u_char	wall;
	u_char	wall_timer;		/* +0x8A */
	char	jump_work;		/* +0x8B: updates before a wall grab */
	char	dash_f;
	char	voice_timer;		/* +0x8D */
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
	u_char	padd6[8];
	char	hang_snd;		/* +0xDE */
	char	splash;			/* +0xDF */
	u_char	pade0[3];
	u_char	hang_wait;		/* +0xE3 */
	u_char	pade4[3];
	u_char	form;			/* +0xE7: 1 Falcon, 2 Shadow, 3 Blade, 4 Ultimate, 5 Zero (likely) */
	u_char	pade8[4];
	u_long	parts;
	long	walk1_spd;		/* +0xF0 */
	long	walk_spd;		/* +0xF4 */
	long	jump_spd;		/* +0xF8 */
	long	fall_max;		/* +0xFC */
	long	fall_grav;		/* +0x100 */
	long	dash_spd;		/* +0x104 */
	long	dash_min;		/* +0x108 */
	long	dash_acc;		/* +0x10C */
	long	slide_spd;		/* +0x110 */
	long	kick_spd;		/* +0x114 */
	long	ladder1_spd;		/* +0x118 (part 2c) */
	long	ladder_spd;		/* +0x11C (part 2c) */
	long	k120;			/* no reader found */
	long	bar_spd;		/* +0x124 */
	long	rope_spd;		/* +0x128 */
	long	bar_dash_spd;		/* +0x12C */
	long	rope_dash_spd;		/* +0x130 */
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
#define ST_FALL		7
#define ST_LAND		8
#define ST_WGRAB	9
#define ST_WKICK	10
#define ST_WSLIDE	11
#define ST_DASH		12
#define ST_ADASH	13
#define ST_CROUCH	26
#define ST_ROPE		27
#define ST_BAR		28
#define ST_SJUMP	31
#define ST_UHOVER	33
#define ST_FALCON	35
#define ST_MACH		74

#define FORM_FALCON	1
#define FORM_SHADOW	2
#define FORM_BLADE	3
#define FORM_ULTIMATE	4

#define TILE_LADDER	32
#define TILE_WATER	36
#define TILE_BAR_LO	37
#define TILE_BAR_HI	45
#define TILE_ROPE	46

#define ANM_SHOT_OFS	112

#define FLG_FRAME	0x3f
#define FLG_EV20	0x20
#define FLG_EV40	0x40
#define FLG_EV80	0x80

extern PLAYER	mega;

#endif
