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
	u_char	pad0e[0x5a];
	u_char	level;
	u_char	pad69[0x33];
	short	max_damage;
} GAME;

enum {
	LEVEL_EASY = 0,
	LEVEL_NORMAL,
	LEVEL_HARD
};

typedef struct {
	u_char	pad00[0x0e];
	short	y;
	u_char	pad10[0x10];
	short	dest_y;
	u_char	pad22[0x2a];
	u_char	redraw;
	u_char	pad4d[0x07];
} BGLAYER;

extern GAME	game;
extern BGLAYER	bg_layer[];
extern OBJECT	mega;
extern long	fall_max;

#define HARD_MODE()	(game.level >= LEVEL_HARD)

/* anime.c */
extern void	SetAnime(OBJECT *op, int no);
extern void	AnimeTick(OBJECT *op);

/* objsys.c */
extern OBJECT	*GetVisualObject(void);
extern OBJECT	*GetMiscObject(void);
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

/* collide.c */
extern void	CollideCheck(OBJECT *op);
extern void	CornerCheck(OBJECT *op);
extern int	ContactMega(OBJECT *op);
extern int	ContactWeapon(OBJECT *op);
extern int	BoxOverlap(OBJECT *op, OBJECT *tp);

/* effect.c */
extern void	SpawnExplosion(OBJECT *op);
extern void	SpawnExplosionRand(OBJECT *op, char type, u_char rx, u_char ry);
extern void	SpawnJunk(u_char n, u_char *anime, OBJECT *op);
extern void	DropItem(OBJECT *op, char tbl);

/* sound.c */
extern int	PlaySound(int bank, int no, OBJECT *op);

/* damage tables (dmgtbl.c) */
extern u_char	dmg_tbl_shot[];
extern u_char	dmg_tbl_guard[];
extern u_char	dmg_tbl_mecha_s[];

#endif
