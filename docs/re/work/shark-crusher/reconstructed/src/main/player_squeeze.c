/*
 *	player_squeeze.c  --  the parts of PlayerUpdate and PlayerHitCheck that kill the player
 *	under the Nightmare Press, and the Ride Armor flag that holds the press
 *
 *	Plausible reconstruction, not original code, not compiled.
 *	Binary: PlayerUpdate 0x80034FBC (flags at 0x80034FFC), PlayerHitCheck 0x80039228
 *	(0x80039254 - 0x800393F4), RideUpdate 0x800F2414 (overlay 06, 0x800F2480 - 0x800F24F8).
 */
#include "game.h"

extern void	PlayerEventEnd(void);		/* 0x8002ABA8, not read */
extern OBJECT	ride;				/* actor2, 0x800CD340 */
extern void	(*ride_state_tbl[])(OBJECT *);	/* 0x800F3A50 */

#define HP_HIT		0x80
#define HP_DEAD		((char)0x80)

/* in PlayerUpdate, before the timers and PlayerHitCheck */
void PlayerSideFlags(OBJECT *mp)
{
	MEGA_UB(MEGA_SIDEF) = MEGA_UB(MEGA_COLF) | MEGA_UB(MEGA_OBJF);
}

/* the first part of PlayerHitCheck: terrain crush, then the squeeze */
void PlayerCrushSqueeze(OBJECT *mp)
{
	u_char	sides, hp;
	int	i;

	if (mp->act2 == 0 || mp->act2 == 1)
		return;

	if ((MEGA_UB(MEGA_COLF) & (COL_U | COL_D)) == (COL_U | COL_D)) {
		game.max_damage += mp->hp & 0x7f;	/* the 32-bit total at game+0x98 */
		mp->hp = HP_HIT;
	}
	if ((MEGA_UB(MEGA_COLF) & (COL_R | COL_L)) == (COL_R | COL_L)) {
		game.max_damage += mp->hp & 0x7f;
		mp->hp = HP_HIT;
	}
	if ((char)mp->hp == HP_DEAD)
		return;

	sides = MEGA_UB(MEGA_SIDEF);
	if ((sides & (COL_U | COL_D)) != (COL_U | COL_D) && (sides & (COL_R | COL_L)) != (COL_R | COL_L))
		return;

	if (mp->var == 0) {			/* X */
		mp->nohit_f = 0;
		if (MEGA_B(MEGA_FREEZE) == 1) {
			MEGA_B(MEGA_FREEZE) = 0;
			game.lock_f = 0;
			PlayerEventEnd();
		}
	} else {
		mp->nohit_f = 0;
	}
	MEGA_UB(MEGA_SQZ_T) = 75;
	hp = mp->hp & 0x7f;
	mp->hp = hp;
	if (!((char)MEGA_UB(MEGA_SQZ_DMG) < hp))
		game.max_damage += hp;
	mp->hp = hp - MEGA_UB(MEGA_SQZ_DMG);
	for (i = 0; i < STAGE_OBJECT_MAX; i++) {
		*((u_char *)&stage_object[i] + 0x76) = 0;
		*((u_char *)&stage_object[i] + 0x72) = 0;
	}
	if ((char)mp->hp > 0) {
		mp->hp |= HP_HIT;
		*((u_char *)mp + 0xCA) = 0;
		mp->hit_kind = 1;
		*((u_char *)mp + 0xA5) = mp->flip;
		if ((u_char)(*((u_char *)mp + 0xE7) - 1) < 4)
			mp->hit_kind = 4;
		game.max_damage += (char)MEGA_UB(MEGA_SQZ_DMG);
	} else {
		mp->hp = HP_HIT;		/* dead: the death branch follows in the same update */
	}
}

/* RideUpdate (actor2 type 0), the part that tells the press to stop */
void RideUpdateFlags(OBJECT *rp)
{
	u_char *f = (u_char *)rp;

	f[0xA8] = f[0x70] | f[0x71];
	f[0x71] = 0;
	ride_state_tbl[rp->act](rp);
	rp->hp &= 0x7f;
	game.flag26 = ((f[0xA8] & (COL_U | COL_D)) == (COL_U | COL_D)) ? 1 : 0;
}
