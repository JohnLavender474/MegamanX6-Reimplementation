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
	char	stop[0x0a];
	u_char	pad1a[2];
	char	player_lock;
	char	checkpoint;
	u_char	pad1e[2];
	struct _OBJECT *boss;
	char	boss_bar;
	char	boss_f;
	u_char	pad26[6];
	char	clear_f[2];
	char	talk_f;
	u_char	pad2f[0x39];
	u_char	level;
	u_char	pad69[0x23];
	char	boss_hp;
	char	boss_count[2];
	char	boss_level;
	u_char	pad90[0x0c];
	short	max_damage;
} GAME;

enum {
	LEVEL_EASY = 0,
	LEVEL_NORMAL,
	LEVEL_HARD
};

typedef struct {
	u_char	pad00[0x0a];
	short	x;
	u_char	pad0c[2];
	short	y;
	u_char	pad10[0x0c];
	short	max_x;
	short	min_x;
	short	max_y;
	short	min_y;
	short	dest_max_x;
	short	dest_min_x;
	short	dest_max_y;
	short	dest_min_y;
	u_char	pad2c[0x1b];
	u_char	limit_spd;
	u_char	pad48[4];
	u_char	redraw;
	u_char	pad4d[0x07];
} BGLAYER;

typedef struct {
	long	ofs[2];
} SPRTBANK;

extern GAME	game;
extern SPRTBANK	*sprt_bank;
extern SPRTBANK	*stage_sprt_bank;
extern long	*tex_tbl;
extern BGLAYER	bg_layer[];
extern OBJECT	mega;
extern char	text_busy;

#define HARD_MODE()	(game.level >= LEVEL_HARD)

/* anime.c */
extern void	SetAnime(OBJECT *op, int no);
extern void	AnimeTick(OBJECT *op);

/* objsys.c */
extern OBJECT	*GetVisualObject(void);
extern OBJECT	*GetMainObject(void);
extern OBJECT	*GetSysObject(void);
extern int	GetSpriteBank(int type);
extern void	DeleteObject3(OBJECT *op);
extern void	DisplayCheck1(OBJECT *op);
extern void	DisplayCheck(OBJECT *op, short rx, short ry);
extern void	MoveObject2(OBJECT *op);
extern int	GetRNG(void);

/* boss.c */
extern void	BossLevel(void);
extern void	ShowBossBar(int no);
extern int	BossBarBusy(void);

/* player.c */
extern void	PlayerEvent(int act, int flip);
extern void	PlayerEventEnd(void);

/* text.c */
extern void	TextOpen(int no, int a, int b);

/* collide.c */
extern void	CollideCheck(OBJECT *op);
extern void	CornerCheck(OBJECT *op);
extern int	ContactMega(OBJECT *op);
extern int	ContactWeapon(OBJECT *op);

/* react.c */
extern void	ReactMode(OBJECT *op);

/* effect.c */
extern void	SpawnExplosion(OBJECT *op);
extern void	SpawnJunk(u_char n, u_char *anime, OBJECT *op);

/* sound.c */
extern int	PlaySound(int bank, int no, OBJECT *op);

/* move.c (more) */
extern void	MoveObject(OBJECT *op);
extern int	OffScreenCheck2(OBJECT *op, short rx, short ry);
extern OBJECT	*GetEffectObject(void);
extern OBJECT	*GetVisualObjectAfter(OBJECT *op, int back);	/* overlay 10, 0x800F77A0 */
extern void	Rumble(int n);
extern void	Quake(int a, int b, int c);
extern void	SpawnHitEffect(OBJECT *op);

/* libgte */
extern int	rsin(int a);
extern int	rcos(int a);
extern int	ratan2(int y, int x);

/* added for Nightmare Pressure (overlay 07) */
typedef struct {
	u_char	pad00[0x0a];
	short	x;
	u_short	y_lo;
	short	y;
	u_char	pad10[0x0c];
	short	max_x;
	short	min_x;
	short	max_y;
	short	min_y;
	short	dest_max_x;
	short	dest_min_x;
	short	dest_max_y;
	short	dest_min_y;
	u_char	pad2c[0x20];
	u_char	redraw;
	u_char	pad4d[0x07];
} BGCAM;

extern BGCAM	bg_cam[3];			/* 0x800971F8, 0x54 bytes each */
extern OBJECT	*GetMainObjectAfter(OBJECT *op, int back);	/* 0x8002BCCC */
extern OBJECT	*GetStageObject(void);			/* 0x8002C4C4 */
extern void	SpawnInRect(int x0, int x1, int y0, int y1, int flag);
extern void	SolidObject(OBJECT *op);
extern int	SolidWeaponHit(OBJECT *op);		/* 0x80032B34 */
extern void	SolidPushShots(OBJECT *op);		/* 0x8003208C */
extern void	SpawnExplosionRand(OBJECT *op, int type, int mx, int my);

#define MEGA_SPECIAL	(*(char *)((char *)&mega + 0xE0))
#define MEGA_SQUEEZE	(*(u_char *)((char *)&mega + 0xE5))
#define MEGA_TOUCH	(*(u_char *)((char *)&mega + 0x71))
#define MEGA_COLL	(*(u_char *)((char *)&mega + 0x89))
#define STAGE_FLAG26	(*(char *)((char *)&game + 0x26))

/* damage tables (dmgtbl.c) */
extern u_char	dmg_tbl_guard[];		/* 0x8007433C, every id deflected */
extern u_char	dmg_tbl_normal[];		/* 0x800743DC */
extern u_char	dmg_tbl_minor[];		/* 0x800741FC */
extern u_char	dmg_tbl_capsule[];		/* 0x80075B9C */

#endif
