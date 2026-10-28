/*
 *	shot.h  --  X's weapon objects (32 x 0xA0 at 0x800950A0) and the muzzle flash effect
 */
#ifndef _SHOT_H_
#define _SHOT_H_

#include "object.h"

typedef OBJECT SHOT;
typedef OBJECT EFFECT;


#define SHOT_HIT(s)		(*(char *)((char *)(s) + 0x98))
#define SHOT_PARENT(s)		(*(void **)((char *)(s) + 0x7C))
#define SHOT_DETACH(s)		(*(long *)((char *)(s) + 0x84))
#define EFF_PARENT(e)		(*(void **)((char *)(e) + 0x50))
#define HIT_TAG(o)		(*(u_char *)((char *)(o) + 0x64))
#define EFF_DETACH(e)		(*(char *)((char *)(e) + 0x5C))

#define WPN_LEMON		0
#define WPN_HALF		9
#define WPN_FULL		18
#define WPN_FULL_FALCON		19
#define WPN_FULL_ULTIMATE	20
#define WPN_ULTIMATE_PART	21
#define WPN_FULL_BLADE		43
#define WPN_SHADOW		45
#define WPN_SHADOW_FULL		46
#define WPN_SABER		50
#define WPN_NONE		-1

#define EFF_MUZZLE		2
#define EFF_SPARK		3

#define SE_CHARGE		7
#define SE_MUZZLE_LEMON		8
#define SE_MUZZLE_HALF		9
#define SE_MUZZLE_FULL		10
#define SE_SHOT_WALL		12
#define SE_WEAPON_CHANGE	12
#define VOICE_CHARGE_SHOT	6

#define CHARGE_HALF		25
#define CHARGE_FULL		90
#define CHARGE_HALF_QUICK	12
#define CHARGE_FULL_QUICK	45

#endif
