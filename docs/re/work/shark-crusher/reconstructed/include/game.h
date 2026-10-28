/*
 *	game.h  --  global game work, engine entry points
 */
#ifndef _GAME_H_
#define _GAME_H_

#include "object.h"

typedef struct {
	u_char	pad00[0x0c];
	char	stage;
	char	area;
	u_char	pad0e;
	char	message;
	char	stop_f;		/* +0x10: 1 while a gate opens (meaning not known) */
	u_char	pad11;
	char	stop2_f;	/* +0x12 */
	u_char	pad13[0x09];
	char	lock_f;		/* +0x1C: player lock, trigger .. gate closed */
	char	checkpoint;	/* +0x1D */
	u_char	pad1e[0x02];
	struct _OBJECT *boss;
	char	boss_bar;
	char	boss_f;
	char	flag26;		/* +0x26: open condition of some gates; stage 4: Ride Armor squeezed */
	char	flag27;		/* +0x27: stage 4: 1 while the press rises */
	char	flag28;		/* +0x28: open condition of gate 44 */
	u_char	pad29[0x3f];
	u_char	level;
	u_char	pad69[0x23];
	char	boss_hp;
	u_char	pad8d[0x03];
	u_long	time;		/* +0x90: stage time in frames (likely) */
	u_char	pad94[0x08];
	short	max_damage;
	u_char	pad9e[0x39e];
	u_long	boss_time;	/* +0x43C: time stamp of the boss gate */
} GAME;

enum {
	LEVEL_EASY = 0,
	LEVEL_NORMAL,
	LEVEL_HARD
};

typedef struct {
	u_char	pad00[0x03];
	char	b03;		/* +0x03 */
	char	mode;		/* +0x04: scroll mode (layer 1 of stage 4: 9, x only) */
	u_char	pad05[0x05];
	short	cx;		/* +0x0A: camera x in px */
	u_char	pad0c[0x02];
	short	y;		/* +0x0E */
	u_char	pad10[0x0a];
	short	old_y;		/* +0x1A: written by the press before it moves layer 1 */
	short	max_x;		/* +0x1C: current limits */
	short	x;		/* +0x1E: current min x (earlier name kept) */
	short	dest_y;		/* +0x20: current max y (earlier name kept) */
	short	min_y;		/* +0x22 */
	short	dst_max_x;	/* +0x24: target limits, eased at lim_spd px/frame */
	short	dst_min_x;	/* +0x26 */
	short	dst_max_y;	/* +0x28 */
	short	dst_min_y;	/* +0x2A */
	u_char	pad2c[0x08];
	u_short	event_f;	/* +0x34 */
	u_char	pad36[0x0c];
	short	s42;		/* +0x42 */
	u_char	pad44[0x03];
	char	lim_spd;	/* +0x47 */
	u_char	pad48[0x04];
	u_char	redraw;		/* +0x4C */
	u_char	pad4d[0x07];
} BGLAYER;			/* 0x54 bytes; bg_layer[0] = 0x800971F8 */

typedef struct {
	short	x;
	short	y;
	u_short	w;
	u_short	h;
} BLOCKCOPY;

typedef struct {
	long	ofs[2];
} SPRTBANK;

extern GAME	game;
extern SPRTBANK	*sprt_bank;
extern BGLAYER	bg_layer[];
extern OBJECT	mega;
extern long	fall_max;

#define MAIN_OBJECT_MAX	48
extern OBJECT	main_object[MAIN_OBJECT_MAX];
extern void	HitReactionDispatch(OBJECT *op);

#define MEGA_GATE(m)	(*(char *)((u_char *)(m) + 0xd4))
#define GATE_REQ	1
#define GATE_READY	(-1)
#define GATE_PASS	(-2)

#define ITEM_TYPE	47

/* solid.c */
extern void	SolidObjectCollide(OBJECT *op);
extern void	ClearEnemies(void);
extern void	PlayerSetTerrainBox(OBJECT *mp);
extern void	GateFadeInit(int n);

#define HARD_MODE()	(game.level >= LEVEL_HARD)

/* player fields used by the press (offsets in mega) */
#define MEGA_B(o)	(*(char *)((u_char *)&mega + (o)))
#define MEGA_UB(o)	(*(u_char *)((u_char *)&mega + (o)))
#define MEGA_COLF	0x70	/* terrain flags of the last CollideCheck */
#define MEGA_OBJF	0x71	/* object push / touch flags, cleared after the step */
#define MEGA_SIDEF	0x89	/* MEGA_COLF | MEGA_OBJF, set before PlayerHitCheck */
#define MEGA_FREEZE	0xE0	/* 2: the press and the type 9 bodies stop (meaning not known) */
#define MEGA_SQZ_T	0xE5	/* squeeze timer, 75 after a squeeze */
#define MEGA_SQZ_DMG	0xE6	/* squeeze damage */

/* kind-7 objects: 4 slots of 0x30 at 0x800C4778, table 0x8007136C (names are a guess) */
typedef struct {
	char	flag;
	char	type;
	char	var;
	char	disp;
	char	act;
	char	act2;
	u_char	pad06[0x0a];
	ENEMYSET *set;
	u_short	timer;		/* +0x14 */
	u_short	count;		/* +0x16 */
	long	spd;		/* +0x18: 16.16 px/update, + = down */
	FIXED	ofs;		/* +0x1C: 16.16 px, + = lower */
	long	save;		/* +0x20 */
	u_char	pad24[0x0c];
} SYSOBJ;

extern void	DeleteSysObject(SYSOBJ *op);	/* DeleteObject3 */
extern int	PlaySoundSys(int bank, int no, SYSOBJ *op);	/* PlaySound with a kind-7 object */
extern void	SolidObject(OBJECT *op);
extern void	SolidSideFix1(OBJECT *op);	/* 0x80032BE8, not read */
extern void	SolidSideFix2(OBJECT *op);	/* 0x8003208C, not read */
extern void	SolidOthers(OBJECT *op);	/* 0x80031F4C, not read */
extern void	SolidPre(OBJECT *op);		/* 0x80032B34, not read */

#define STAGE_OBJECT_MAX	80
extern OBJECT	stage_object[STAGE_OBJECT_MAX];	/* 0xA4 bytes each in the binary */

/* anime.c */
extern void	SetAnime(OBJECT *op, int no);
extern void	AnimeTick(OBJECT *op);

/* objsys.c */
extern OBJECT	*GetVisualObject(void);
extern OBJECT	*GetMiscObject(void);
extern OBJECT	*GetMainObject(void);
extern OBJECT	*GetSysObject(void);
extern int	GetSpriteBank(int type);
extern void	DeleteObject(OBJECT *op);
extern void	DeleteObject2(OBJECT *op);
extern void	DeleteObject3(OBJECT *op);
extern int	OffScreenCheck(OBJECT *op);
extern int	OffScreenCheck2(OBJECT *op, short rx, short ry);
extern void	DisplayCheck1(OBJECT *op);
extern void	DisplayCheck(OBJECT *op, short rx, short ry);
extern void	MoveObject(OBJECT *op);
extern void	MoveObject2(OBJECT *op);
extern int	GetRNG(void);

/* calc.c */
extern u_char	AimAt(OBJECT *op, OBJECT *tp);
extern void	SetSpeedDir(OBJECT *op, u_char dir);

/* map.c */
extern void	CopyBlock(int layer, short x, short y, void *src);

/* camera.c */
extern void	Quake(char time, char w, char h);
extern void	Quake2(char a, char b, char c);
extern void	CameraClampLeft(short x);
extern void	CameraClampRight(short x);

/* boss.c */
extern void	BossInit(void);
extern void	ShowBossBar(int no);
extern int	BossBarBusy(void);

/* pad.c */
extern void	Rumble(int no);

/* music.c */
extern void	PlayMusic(int no, int vol);
extern void	StartBossMusic(void);

/* collide.c */
extern void	CollideCheck(OBJECT *op);
extern void	CornerCheck(OBJECT *op);
extern int	ContactMega(OBJECT *op);
extern int	ContactWeapon(OBJECT *op);
extern int	BoxOverlap(OBJECT *op, OBJECT *tp);

/* effect.c */
extern void	SpawnExplosion(OBJECT *op);
extern void	RockFall(int n, short x0, short y0, short x1, short y1, int m);
extern void	SpawnExplosionRand(OBJECT *op, char type, u_char rx, u_char ry);
extern void	SpawnJunk(u_char n, u_char *anime, OBJECT *op);
extern void	DropItem(OBJECT *op, char tbl);

/* sound.c */
extern int	PlaySound(int bank, int no, OBJECT *op);

/* damage tables (dmgtbl.c) */
extern u_char	dmg_tbl_shot[];
extern u_char	dmg_tbl_guard[];
extern u_char	dmg_tbl_mecha_s[];
extern u_char	dmg_tbl_d1000[];
extern u_char	dmg_tbl_d1000_tama[];

#endif
