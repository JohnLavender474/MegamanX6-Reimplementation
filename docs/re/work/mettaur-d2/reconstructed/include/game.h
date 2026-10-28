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
	char	exit_f;
	char	stop[10];
	u_char	pad1a[2];
	char	unk1c;
	char	checkpoint;
	u_char	pad1e[2];
	struct _OBJECT *boss;
	char	boss_bar;
	char	boss_f;
	char	char_state;
	u_char	pad27[0x41];
	u_char	level;
	u_char	pad69[0x23];
	char	boss_hp;
	u_char	pad8d[0x0f];
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
	short	max_x2;
	short	min_x2;
	short	max_y2;
	short	min_y2;
	u_char	pad2c[0x18];
	u_char	follow;
	u_char	pad45[7];
	u_char	redraw;
	u_char	pad4d[7];
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
extern long	frame_count;
extern char	fade_dir;
extern u_char	bg_refresh;
extern u_char	checkpoint_set;

#define STOP_ALL()	{ int i; for (i = 0; i < 8; i++) game.stop[i] = 2; game.stop[9] = 2; }
#define GO_ALL()	{ int i; for (i = 0; i < 8; i++) game.stop[i] = 0; game.stop[9] = 0; }
#define MEGA_B(ofs)	(((u_char *)&mega)[ofs])

/* fade.c */
extern void	FadeIn(char speed);
extern void	FadeOut(char speed);

/* stage.c */
extern void	SpawnInRect(short x0, short x1, short y0, short y1, int flag);
extern void	ResetLayerCache(int layer);
extern void	ClearPlayerShots(void);

/* player.c */
extern void	MegaStopWeapon(OBJECT *op);
extern void	MegaStopCharge(OBJECT *op);
extern void	MegaClearA1(OBJECT *op);
extern void	MegaStopSound(OBJECT *op);
extern void	MegaResume(OBJECT *op);

#define MAIN_OBJECT_MAX	48
extern OBJECT	main_object[MAIN_OBJECT_MAX];
extern void	HitReactionDispatch(OBJECT *op);

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
extern void	Quake2(char time, char w, char h);
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
extern u_char	dmg_tbl_normal[];
extern u_char	dmg_tbl_deflect[];
extern u_char	dmg_tbl_e_shot[];

#define PLAYER_X		0
#define PLAYER_ZERO		1
#define PLAYER_CHAR()	(mega.var)

/* added for the stage 07 laser puzzle */
extern OBJECT	*GetWeaponObject(void);			/* 0x8002C32C */
extern void	CopyTiles(int layer, short dx, short dy, BLOCKCOPY *src);	/* 0x80027BA0 */
extern u_char	TileAt(OBJECT *op, short x, short y);	/* 0x80030234: collision value */
extern OBJECT	main_objects[48];			/* 0x8008EF48 */

/* added for Mettaur D2 */
#define WEAPON_OBJECT_MAX	32
#define WEAPON_OBJECT_SIZE	0xA0
extern u_char	weapon_area[WEAPON_OBJECT_MAX][WEAPON_OBJECT_SIZE];	/* 0x800950A0 */
#define WEAPON(i)	((OBJECT *)weapon_area[i])
#define WPN_LEMON	0
#define KILL_BY_WPN55	(-56)
extern int	TerrainPointCheck(OBJECT *op, short x, short y);	/* 0x80030CCC */
extern void	SpawnHitEffect(OBJECT *op);			/* 0x80049D90 */
extern u_char	dmg_tbl_one_hit[];				/* 0x8007429C */

#endif
