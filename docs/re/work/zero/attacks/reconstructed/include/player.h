/*
 *	player.h  --  the player object (mega, 0x800970A0) as far as parts 2a, 2b and step 3 (attacks) read it
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
	u_short	key_old;		/* +0x7E: buttons held in the update before */
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
	char	weapon_pose;		/* +0x8F: 1 in the update of a shot, cleared each update */
	char	pose_end;		/* +0x90 */
	u_char	shot_timer;		/* +0x91 */
	u_char	shot_wait;		/* +0x92 */
	char	wpn_slot;		/* +0x93: 0 buster, 1-8 special weapons */
	char	wpn_id;			/* +0x94: id for the weapon button (0x10) */
	char	bst_id;			/* +0x95: id for the buster button (0x20) */
	char	weapon;			/* +0x96: id to fire this update, -1 none */
	char	last_shot;		/* +0x97 */
	u_char	shot_count;		/* +0x98 */
	u_char	wpn_count;		/* +0x99 */
	char	shot_cycle;		/* +0x9A */
	char	level[2];		/* +0x9B weapon, +0x9C buster */
	u_char	charge[2];		/* +0x9D weapon, +0x9E buster */
	char	blink_timer;		/* +0x9F */
	char	blink_phase;		/* +0xA0 */
	char	charge_pal;		/* +0xA1 */
	char	flash_pal;		/* +0xA2 */
	u_char	pada3;
	char	giga;			/* +0xA4 */
	u_char	pada5;
	char	wpn_state;		/* +0xA6 */
	u_char	pada7;
	short	energy[10];		/* +0xA8 */
	u_char	padbc[13];
	u_char	skill;			/* +0xC9: X: special weapons owned */
	u_char	padca[9];
	u_char	lock;
	u_char	padd4;
	char	dash_box;
	u_char	padd6[7];
	char	var_dd;		/* +0xDD: set with each shot */
	char	hang_snd;		/* +0xDE */
	char	splash;			/* +0xDF */
	char	var_e0;			/* +0xE0 */
	u_char	pade1[2];
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
	long	carry_x;		/* +0x138 */
	u_char	pad13c[8];
	long	var_144;		/* +0x144: 3 free weapon energy, 6 another buster mode (open) */
} PLAYER;

#define KEY_R		0x0001
#define KEY_L		0x0002
#define KEY_U		0x0004
#define KEY_D		0x0008
#define KEY_JUMP	0x0080
#define KEY_WEAPON	0x0010
#define KEY_BUSTER	0x0020
#define KEY_GIGA	0x0040
#define KEY_DASH	0x0100
#define KEY_WPN_A	0x0200
#define KEY_WPN_B	0x0400

#define REL_WEAPON	0x00100000
#define REL_BUSTER	0x00200000

#define PART_ENERGY	0x00000020
#define PART_BPLUS	0x00000080
#define PART_SPEED	0x00000100
#define PART_RAPID5	0x00040000
#define PART_UBUSTER	0x00080000
#define PART_QUICK	0x00100000

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
