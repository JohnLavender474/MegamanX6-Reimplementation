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
	u_char	pad10[0x10];
	struct _OBJECT *boss;
	char	boss_bar;
	char	boss_f;
	u_char	pad26[0x42];
	u_char	level;
	u_char	pad69[0x23];
	char	boss_hp;
	u_char	pad8d[0x0f];
	short	max_damage;
	u_char	pad9e[0x34];
	u_short	soul[2];
	u_char	padd6[2];
	u_long	reploid[2];
	u_char	pade0[0x98];
	u_char	soul_done[12][32];
} GAME;

#define REPLOID_WAIT		0
#define REPLOID_SAVED		2
#define REPLOID_KILLED		3
#define REPLOID_LOST		4

extern ENEMYSET	*enemy_set_tbl[][2];
extern u_char	soul_done_row[];
extern u_long	frame_count;
extern OBJECT	alt_body[2];

#define STAGE_OBJECT_MAX	80
extern u_char	stage_object[STAGE_OBJECT_MAX][0xa4];

extern OBJECT	*GetRayObject(void);
extern int	TerrainBoxOverlap(OBJECT *op, OBJECT *tp);

enum {
	LEVEL_EASY = 0,
	LEVEL_NORMAL,
	LEVEL_HARD
};

typedef struct {
	u_char	pad00[0x0e];
	short	y;
	u_char	pad10[0x0e];
	short	x;
	short	dest_y;
	u_char	pad22[0x2a];
	u_char	redraw;
	u_char	pad4d[0x07];
} BGLAYER;

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

#define VISUAL_OBJECT_MAX	32
extern u_char	visual_object[VISUAL_OBJECT_MAX][0xa0];

#define HARD_MODE()	(game.level >= LEVEL_HARD)

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
extern u_char	TerrainPointCheck(OBJECT *op, short x, short y);

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
extern u_char	dmg_tbl_small[];
extern u_char	dmg_tbl_deflect[];
extern u_char	dmg_tbl_insect[];

#endif
