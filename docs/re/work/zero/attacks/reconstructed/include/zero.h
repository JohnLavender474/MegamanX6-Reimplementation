/*
 *	zero.h  --  Zero's overlay (overlay 1 at 0x801EA000): field names, ids and constants
 *	A reconstruction from the disassembly; all names are guesses.
 */
#ifndef _ZERO_H_
#define _ZERO_H_

#include "player.h"

/* PLAYER fields as Zero uses them (player.h names in brackets) */
#define Z_BUSY(m)	((m)->shot_pose)		/* +0x8E: an attack runs; no new attack */
#define Z_COMBO(m)	(((char *)(m))[0xCB])		/* +0xCB: hit number of the combo, spins of Ensuizan */
#define Z_SKILL(m)	((m)->skill)			/* +0xC9: techniques = game+0x60 */
#define Z_SLOT(m)	((m)->wpn_slot)			/* +0x93: energy slot of the technique */
#define Z_WSTATE(m)	((m)->wpn_state)		/* +0xA6: 1 Yammar Option out, 2 Guard Shell out */
#define Z_ENERGY(m, n)	((m)->energy[n])		/* +0xA8 + 2n */
#define Z_DD(m)		((m)->var_dd)			/* +0xDD */
#define Z_E0(m)		((m)->var_e0)			/* +0xE0 */
#define Z_E1(m)		(((u_char *)(m))[0xE1])		/* +0xE1: Rekkoha timer */
#define Z_E4(m)		(((u_char *)(m))[0xE4])
#define Z_LOCK(m)	((m)->lock)			/* +0xD3 */
#define Z_DEAD(m)	(((char *)(m))[0xCF])
#define Z_MODE(m)	((m)->var_144)			/* +0x144: 6 five Z-Buster shots, 2 +1 damage, 3 free energy */
#define Z_TOUCH(m)	(((u_char *)(m))[0x89])		/* 1 right wall, 2 left wall, 4 ceiling, 8 floor */

/* skill bits: bit n = the Maverick of stage n + 1 beaten */
#define SK_YAMMAR	0x01	/* Commander Yammark, stage 1: Yammar Option */
#define SK_HYOROGA	0x02	/* Blizzard Wolfang, stage 2: super jump and ceiling hold */
#define SK_SHOENZAN	0x04	/* Blaze Heatnix, stage 3 */
#define SK_RAKUKOJIN	0x08	/* Metal Shark Player, stage 4 */
#define SK_SENTSUIZAN	0x10	/* Ground Scaravich, stage 5 */
#define SK_ENSUIZAN	0x20	/* Rainy Turtloid, stage 6 */
#define SK_GUARDSHELL	0x40	/* Shield Sheldon, stage 7 */
#define SK_REKKOHA	0x80	/* Infinity Mijinion, stage 8 */

/* energy slots (+0x93) */
#define ZS_SABRE	0
#define ZS_HYOROGA	1
#define ZS_RAKUKOJIN	2
#define ZS_YAMMAR	3
#define ZS_GUARDSHELL	4
#define ZS_REKKOHA	5

/* steps */
#define ST_ZSABRE	48	/* ground combo */
#define ST_ZJUMPSABRE	49
#define ST_ZFALLSABRE	50
#define ST_ZLADDERSABRE	51
#define ST_ZWALLSABRE	52
#define ST_ZSJUMPSABRE	53	/* 49 after the super jump or the double jump (m+0x86 = 6) */
#define ST_ZSFALLSABRE	54
#define ST_SHOENZAN	55
#define ST_RAKUKOJIN	56
#define ST_SENTSUIZAN	57
#define ST_ENSUIZAN	58
#define ST_REKKOHA	59
#define ST_ZCROUCHSABRE	60
#define ST_ZHANGSABRE	61
#define ST_ZBUSTER	62
#define ST_HYOROGA	63

/* weapon ids (weapon object types) */
#define ZW_SLASH1	24
#define ZW_SLASH2	25
#define ZW_SLASH3	26
#define ZW_JUMP		27
#define ZW_LAND		28
#define ZW_SJUMP	29
#define ZW_LADDER	30
#define ZW_WALL		31
#define ZW_ICICLE	33
#define ZW_FIRE		34
#define ZW_RAKUKOJIN	35
#define ZW_SENTSUIZAN	36
#define ZW_CROUCH	37
#define ZW_HANG		38	/* 38-41 */
#define ZW_ENSUIZAN	42
#define ZW_BEAM		44
#define ZW_CEILING	48
#define ZW_BUSTER	58
#define W_YAMMAR	1	/* main-program weapon objects */
#define W_ANCHOR	4
#define W_GUARDSHELL	7

/* frame flags of Zero's sabre animations */
#define ZF_SPAWN	0x80	/* make the hit object */
#define ZF_CANCEL	0x40	/* new actions allowed */
#define ZF_NEXT		0x20	/* the next combo hit on a press */
#define ZF_TURN		0x10	/* the turn-around hit with the back direction */
#define ZF_HOLD		0x01	/* no cancel */

#define PART_SABRE_DMG	0x00400000	/* +1 sabre damage, palette 4 */
#define PART_SABRE_LONG	0x00800000	/* boxes 0x801EE580: 12 px longer */
#define PART_SABRE_X	0x01000000	/* palette 5, o+0x9C */
#define ARMOR_BLACK	0x20		/* game+0x5F: Black Zero */

typedef struct {
	u_char	anime;
	u_char	pal;
	u_char	ext;	/* uses the long boxes with PART_SABRE_LONG */
	u_char	b3;
} SABRE_TBL;

extern SABRE_TBL	sabre_tbl[];		/* 0x801EE68C, index type - 24 */
extern HITBOX		sabre_box[];		/* 0x801EE414 */
extern HITBOX		sabre_box_long[];	/* 0x801EE580 */
extern HITBOX		sabre_tbox[];		/* 0x801EE2D4 */
extern HITBOX		fire_box[];		/* 0x801EE6F4 */
extern u_char		combo_snd[3][2];	/* 0x801EE3BC */
extern short		zero_cost[10];		/* 0x80073DF4 */

extern OBJECT	*GetWeaponObject(void);
extern void	PlaySound(int bank, int id, OBJECT *op);
extern void	PlayVoice(PLAYER *m, int id);
extern void	PlayerSetAnime(PLAYER *m, int no);
extern void	PlayerSetAnimeAt(PLAYER *m, int no, int frame);
extern int	WeaponCost(PLAYER *m, int slot);
extern void	SpendEnergy(PLAYER *m, int slot);
extern void	ClearDashFlags(PLAYER *m);

#endif
