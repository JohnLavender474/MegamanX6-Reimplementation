/*
 *	weapons.h  --  X's special weapons: ids, work fields, tables (player RE step 4)
 */
#ifndef _WEAPONS_H_
#define _WEAPONS_H_

#include "shot.h"

/* weapon ids = weapon object types; slot n uncharged = n, charged = n + 9 */
#define WPN_YAMMAR		1	/* Yammar Option (slot 1, Commander Yammark) */
#define WPN_ICE			2	/* Ice Burst (slot 2, Blizzard Wolfang) */
#define WPN_MAGMA		3	/* Magma Blade (slot 3, Blaze Heatnix) */
#define WPN_ANCHOR		4	/* Metal Anchor (slot 4, Metal Shark Player) */
#define WPN_GROUND		5	/* Ground Dash (slot 5, Ground Scaravich) */
#define WPN_METEOR		6	/* Meteor Rain (slot 6, Rainy Turtloid) */
#define WPN_GUARD		7	/* Guard Shell (slot 7, Shield Sheldon) */
#define WPN_RAY			8	/* Ray Arrow (slot 8, Infinity Mijinion) */
#define WPN_CHARGED(n)		((n) + 9)
#define WPN_GIGA_ULTIMATE	22	/* X's body in step 34 */
#define WPN_FALCON_DASH		23	/* X's body in step 35 */
#define WPN_MACH_DASH		32	/* X's body in step 74 */
#define WPN_SHADOW_GIGA		47
#define WPN_FALCON_GIGA		49
#define WPN_SABER_AIR		51
#define WPN_ICE_SHARD		52
#define WPN_GUARD_SHARD		53
#define WPN_OPTION_SHOT		54
#define WPN_BLADE_UP		56
#define WPN_BLADE_GIGA		57

/* mega+0xA6 (wpn_state): a charged weapon is live */
#define WST_ICE			1	/* blocks 2, 11; drops the charge; glow 41 */
#define WST_GUARD		2	/* blocks 7, 16 */
#define WST_METEOR		3	/* blocks 6, 15 */

/* mega+0x144 (var_144): timed item effect, timer +0x148 (300, 480 for 6) */
#define ITEM_SHOT_GUARD		1	/* no contact damage from enemy shots (visual objects) */
#define ITEM_ATTACK		2	/* weapon damage + 1 */
#define ITEM_FREE_ENERGY	3	/* special weapons cost nothing */
#define ITEM_SHADOW_BUSTER	6	/* form 2: the plain buster ids */

#define EFF_WEAPON_GFX		27	/* effect type 27: one-update frame loader */

/* weapon object work bytes */
#define W_U8(o, n)		(*(u_char *)((char *)(o) + (n)))
#define W_S8(o, n)		(*(char *)((char *)(o) + (n)))
#define W_S16(o, n)		(*(short *)((char *)(o) + (n)))
#define W_STEP2(o)		W_S8(o, 0x06)
#define W_ACT4(o)		W_S8(o, 0x07)
#define W_FLAG69(o)		W_S8(o, 0x45)	/* animation frame flag */
#define W_NEXT(o)		W_S8(o, 0x46)	/* 0 at the hold frame */
#define W_COLLIDE(o)		W_U8(o, 0x70)
#define W_SOLID(o)		W_S8(o, 0x75)	/* Ice Burst block: X can stand on it */

/* tables */
extern short	cost_x[];		/* 0x80073D74: bar units per id */
extern short	cost_z[];		/* 0x80073DF4 */
extern u_short	key_cfg[16];		/* 0x8006D600: pad mask per internal bit */
extern ANIME	*yammar_anime[];	/* 0x800779C0 */
extern ANIME	*ice_anime[];		/* 0x800779D8 */
extern ANIME	*ice2_anime[];		/* 0x800779F4 */
extern ANIME	*magma_anime[];		/* 0x80077A00 */
extern ANIME	*magma2_anime[];	/* 0x80077A08 */
extern ANIME	*anchor_anime[];	/* 0x80077A0C */
extern ANIME	*ground_anime[];	/* 0x80077A20 */
extern ANIME	*meteor_anime[];	/* 0x80077A28 */
extern ANIME	*guard_anime[];		/* 0x80077A40 */
extern ANIME	*ray_anime[];		/* 0x80077A64 */

#endif
