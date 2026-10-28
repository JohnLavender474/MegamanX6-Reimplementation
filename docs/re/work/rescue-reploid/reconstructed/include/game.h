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
	char	pause[10];
	u_char	pad1a[6];
	struct _OBJECT *boss;
	char	boss_bar;
	char	boss_f;
	u_char	pad26[0x13];
	char	lives;
	u_char	pad3a[0x21];
	char	max_hp[2];
	u_char	pad5d[2];
	u_char	armors;
	u_char	pad60[3];
	u_char	tank[2];
	u_char	pad65[3];
	u_char	level;
	u_char	armor_parts;
	u_short	tanks;
	u_char	pad6c[0x20];
	char	boss_hp;
	u_char	pad8d[0x0f];
	short	max_damage;
	u_char	pad9e[0x3a];
	u_long	reploid[16];
} GAME;

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
extern u_long	frame_count;
extern OBJECT	mega_sub[2];

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
extern OBJECT	*GetStageObject(void);
extern void	DeleteObjectKeep(OBJECT *op);
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
extern int	TerrainOverlap(OBJECT *op, OBJECT *tp);
extern void	SolidObject(OBJECT *op);

/* effect.c */
extern void	SpawnExplosion(OBJECT *op);
extern void	RockFall(int n, short x0, short y0, short x1, short y1, int m);
extern void	SpawnExplosionRand(OBJECT *op, char type, u_char rx, u_char ry);
extern void	SpawnJunk(u_char n, u_char *anime, OBJECT *op);
extern void	DropItem(OBJECT *op, char tbl);
extern void	SpawnHitEffect(OBJECT *op);
extern OBJECT	*GetEffectObject(void);

/* sound.c */
extern int	PlaySound(int bank, int no, OBJECT *op);

/* damage tables (dmgtbl.c) */
extern u_char	dmg_tbl_shot[];
extern u_char	dmg_tbl_guard[];
extern u_char	dmg_tbl_mecha_s[];
extern u_char	dmg_tbl_d1000[];
extern u_char	dmg_tbl_d1000_tama[];

/* reploid.c (main program) */
extern int	GetReploidStatus(u_char no);
extern void	SetReploidStatus(u_char no, u_char st);
extern void	PauseObjects(u_char on);

/* text / stream */
extern void	StreamPlay(int no, int vol);
extern void	StreamStop(int mode);
extern void	StageMusicRestart(void);
extern void	TextOpen(int no, int a, int b);
extern u_char	stream_state;
extern u_char	text_state;

#endif
