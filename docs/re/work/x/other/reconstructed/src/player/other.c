/*
 *	other.c  --  player ladder, hit reaction, death, beams, ride and event steps
 *	(main program, shared by X and Zero; part 2c)
 *
 *	A plausible reconstruction from the disassembly; not original code, not compiled.
 */
#include "player.h"
#include "game.h"
#include "sound.h"

extern void	AnimeTick(void *o);
extern void	PlayerMove(void *o);
extern void	PlayerMoveAccel(void *o);
extern void	PlayerSetAnime(PLAYER *m, int n);
extern void	PlayerSetAnimeAt(PLAYER *m, int n, int frame);
extern void	PlayerSetAnimeShoot(PLAYER *m, int n);
extern void	PlayerSetTerrainBox(PLAYER *m);
extern int	PlayerRequests(PLAYER *m);
extern int	PlayerDamageScale(PLAYER *m, int d);
extern void	PlayerLoadForm(PLAYER *m);
extern void	PlayerPhysicsParts(PLAYER *m);
extern void	PlayerEventEnd(void);
extern void	PlaySound(int bank, int id, void *o);
extern void	StopSound(int bank, int id);
extern void	StreamPlay(int n, int vol);
extern void	Rumble(int n);
extern int	GetRNG(void);
extern int	OffScreenCheck2(void *o, int rx, int ry);
extern OBJECT	*GetMiscObject(void);
extern int	TileAt(PLAYER *m);			/* 0x8003015C */
extern void	ToStand(PLAYER *m);
extern void	Stop(PLAYER *m);
extern void	StartJump(PLAYER *m);
extern void	StartFall(PLAYER *m);			/* 0x8003AFF4, part 2b */
extern void	StandAnime(PLAYER *m);
extern void	ClearDashFlags(PLAYER *m);
extern void	ClearShotFlags(PLAYER *m);		/* 0x8003CDBC */
extern void	ClearActionFlags(PLAYER *m);		/* 0x8003CDF0 */
extern void	ClearDeathFlags(PLAYER *m);		/* 0x8003CE30 */
extern void	PlayerPalette(PLAYER *m);		/* 0x8003C4EC */
extern void	PlayerFlash(PLAYER *m, int n);		/* 0x8003CF54 */
extern void	ChargeStop(PLAYER *m);			/* 0x8003F3DC */
extern void	ChargeOff(PLAYER *m);			/* 0x8003F000 */
extern void	LowHpWarn(PLAYER *m);			/* 0x8003D014 */
extern void	WaterSplashCheck(PLAYER *m);		/* 0x8003D394 */
extern void	PlayVoice(PLAYER *m, int id);		/* 0x8003CFE0 */
extern int	XLadderShoot(PLAYER *m);		/* 0x8003E260 */
extern void	XLadderShotStart(PLAYER *m);		/* 0x801EB6E8 (overlay 0) */
extern int	ZeroTryB(PLAYER *m);			/* 0x801EA560 (overlay 1) */
extern int	ZeroLadderSabre(PLAYER *m);		/* 0x801EC0F0 (overlay 1) */
extern void	CameraFollow(int on);
extern int	ZeroGigaGain(PLAYER *m, int n);	/* 0x8003EB0C */
extern void	RideClear(void);			/* 0x8002ABA8 */

extern void	(*ladder_steps[])(PLAYER *);		/* 0x80073C1C */
extern void	(*hurt_steps[])(PLAYER *);		/* 0x80073C30 */
extern u_char	hurt_anime[];				/* 0x80073C70 */
extern long	hurt_speed[][4];			/* 0x80073C78: speed_x, accel_x, speed_y, gravity */
extern u_char	hurt_iframes[];				/* 0x80073CD8 */
extern void	(*death_steps[])(PLAYER *);		/* 0x80073D20 */
extern char	death_dirs[4][8];			/* 0x80073D30 */
extern void	(*player_init[])(PLAYER *);		/* 0x80073CF4 */
extern u_char	voice_tbl[];				/* 0x80073CE4 */
extern OBJECT	stage_objects[80];			/* 0x800C1220, 0xA4 each */
extern BGLAYER	camera_layer[3];			/* 0x800971F8 */
extern u_char	stream_state;				/* 0x800CD40C */
extern long	frame_count;				/* 0x80097420 */
extern u_char	contact_off;				/* 0x800C4568 */

#define ENERGY_MAX(m)	(*(short *)((u_char *)(m) + 0x14a))
#define CAM_B(l, o)	(((u_char *)&camera_layer[l])[o])
#define CAM_H(l, o)	(*(short *)((u_char *)&camera_layer[l] + (o)))

#define SNAP_X(m)	((m)->x.w.lo = 0, (m)->x.w.hi = ((m)->x.w.hi & ~15) + 8)

/* ---------------------------------------------------------------- ladder */

/* 0x8003B63C */
void LadderFromGround(PLAYER *m)
{
	PlayerSetAnime(m, 27);
	m->jump_latch = 1;
	m->y.w.lo = 0;
	SNAP_X(m);
	m->spd_x = 0;
	m->acc_x = 0;
	m->grav = 0;
	m->spd_y = m->k118;			/* 1.0 */
	ClearDashFlags(m);
	ClearShotFlags(m);
	m->act2 = ST_LADDER;
	m->act3 = 0;
}

/* 0x8003B6B4 */
void LadderClimbTop(PLAYER *m)
{
	PlayerSetAnime(m, 28);
	m->col_box = 0;
	m->y.w.lo = 0;
	m->y.w.hi &= ~15;
	ClearShotFlags(m);
	m->act2 = ST_LADDER;
	m->act3 = 1;
}

/* 0x8003B708 */
void LadderDown(PLAYER *m)
{
	PlayerSetAnime(m, 29);
	m->y.w.lo = 0;
	m->col_box = 0;
	m->jump_latch = 1;
	SNAP_X(m);
	ClearDashFlags(m);
	ClearShotFlags(m);
	m->act2 = ST_LADDER;
	m->act3 = 2;
}

/* 0x8003B774 */
void LadderOffBottom(PLAYER *m)
{
	PlayerSetAnime(m, 30);
	ClearShotFlags(m);
	m->act2 = ST_LADDER;
	m->act3 = 3;
}

/* 0x8003B7B4 */
void ToLadderUp(PLAYER *m)
{
	m->y.w.lo = 0;
	SNAP_X(m);
	m->spd_x = 0;
	m->acc_x = 0;
	m->grav = 0;
	m->spd_y = m->k11c;			/* 1.5 */
	ClearDashFlags(m);
	m->act2 = ST_LADDER_UP;
	m->act3 = 0;
}

/* 0x8003B810 */
void ToLadderDown(PLAYER *m)
{
	m->y.w.lo = 0;
	SNAP_X(m);
	m->spd_x = 0;
	m->acc_x = 0;
	m->grav = 0;
	m->spd_y = -m->k11c;			/* -1.5 */
	ClearDashFlags(m);
	m->act2 = ST_LADDER_DN;
	m->act3 = 0;
}

/* 0x8003A488: from the jump, the fall and step 10 */
int TryLadderAir(PLAYER *m)
{
	if (m->lock)
		return 0;
	if (!(m->key & KEY_U))
		return 0;
	if (TileAt(m) != TILE_LADDER)
		return 0;
	if (m->shot_pose) {
		XLadderShotStart(m);
		return 1;
	}
	PlayerSetAnime(m, 31);
	ToLadderUp(m);
	return 1;
}

/* 0x8003A514 */
int LadderExit(PLAYER *m)
{
	int	t = TileAt(m);
	int	off;

	if (m->wall & FLOOR) {
		LadderOffBottom(m);
		return 1;
	}
	if ((m->key & KEY_U) && t == TILE_LADDER_TOP) {
		LadderClimbTop(m);
		return 1;
	}
	off = (u_char)(t - TILE_LADDER) >= 2;
	if (m->trg & KEY_JUMP)
		off = 1;
	if (!off)
		return 0;
	PlayerSetAnime(m, 30);
	m->act2 = ST_LADDER;
	m->act3 = 4;
	return 1;
}

/* 0x8003A5D8: X's ladder shot (step 32) */
int LadderShotExit(PLAYER *m)
{
	if (TileAt(m) == TILE_LADDER && !(m->trg & KEY_JUMP))
		return 0;
	StartFall(m);
	return 1;
}

/* 0x8003744C: step 14 */
void PlayerLadder(PLAYER *m)
{
	ladder_steps[m->act3](m);
}

/* 0x80037488: sub-step 0, from the ground */
void LadderStepFromGround(PLAYER *m)
{
	AnimeTick(m);
	PlayerMove(m);
	if (m->anime.next < 0) {
		PlayerSetAnime(m, 31);
		ToLadderUp(m);
	}
}

/* 0x800374D8: sub-step 1, onto the top */
void LadderStepTop(PLAYER *m)
{
	AnimeTick(m);
	if (m->anime.flag & FLG_EV80) {
		m->anime.flag = 0;
		m->y.w.hi -= 16;
		m->old_y.w.hi = m->y.w.hi - 16;
		PlayerSetTerrainBox(m);
	}
	if (m->anime.next < 0) {
		m->jump_latch = 0;
		Stop(m);
	}
}

/* 0x80037548: sub-step 2, down from the top */
void LadderStepDown(PLAYER *m)
{
	AnimeTick(m);
	if (m->anime.flag & FLG_EV80) {
		m->anime.flag = 0;
		m->y.w.hi = (m->y.w.hi & ~15) + 32;
	}
	if (m->anime.next < 0) {
		m->y.w.hi += 16;
		PlayerSetTerrainBox(m);
		PlayerSetAnimeAt(m, 32, 3);
		ToLadderDown(m);
	}
}

/* 0x800375D0: sub-step 3, off at the bottom */
void LadderStepOff(PLAYER *m)
{
	AnimeTick(m);
	if (m->anime.next < 0) {
		m->jump_latch = 0;
		Stop(m);
	}
}

/* 0x80037610: step 15, climb up */
void PlayerLadderUp(PLAYER *m)
{
	char	keep;

	if (LadderExit(m))
		return;
	if (m->var == 0) {
		if (XLadderShoot(m))
			return;
	} else {
		if (ZeroTryB(m))
			return;
		if (ZeroLadderSabre(m))
			return;
	}
	if (m->trg & KEY_L)
		m->flip = 0;
	if (m->trg & KEY_R)
		m->flip = 0x40;
	if (m->key & KEY_U) {
		AnimeTick(m);
		PlayerMove(m);
	} else if (m->key & KEY_D) {
		keep = m->anime.duration;
		PlayerSetAnimeAt(m, 32, m->anime.flag);
		m->anime.duration = keep;
		ToLadderDown(m);
	}
}

/* 0x80037704: step 16, climb down */
void PlayerLadderDown(PLAYER *m)
{
	char	keep;

	if (LadderExit(m))
		return;
	if (m->var == 0) {
		if (XLadderShoot(m))
			return;
	} else {
		if (ZeroTryB(m))
			return;
		if (ZeroLadderSabre(m))
			return;
	}
	if (m->trg & KEY_L)
		m->flip = 0;
	if (m->trg & KEY_R)
		m->flip = 0x40;
	if (m->key & KEY_U) {
		keep = m->anime.duration;
		PlayerSetAnimeAt(m, 31, m->anime.flag);
		m->anime.duration = keep;
		ToLadderUp(m);
	} else if (m->key & KEY_D) {
		AnimeTick(m);
		PlayerMove(m);
	}
}

/* 0x800377FC: sub-step 4, let go */
void LadderStepLetGo(PLAYER *m)
{
	AnimeTick(m);
	if (m->anime.next < 0)
		StartFall(m);
}

/* ---------------------------------------------------------------- hit reaction */

/* 0x80039228: each update, before the step */
void PlayerHitCheck(PLAYER *m)
{
	int	i, low, min;
	short	*e;

	if (m->act2 == ST_BEAM_IN || m->act2 == ST_BEAM_OUT)
		return;

	if ((m->col_f & 0x0c) == 0x0c) {
		GAME_W(G_DAMAGE) += m->hp & 0x7f;
		m->hp = (char)HP_HIT;
	}
	if ((m->col_f & 0x03) == 0x03) {
		GAME_W(G_DAMAGE) += m->hp & 0x7f;
		m->hp = (char)HP_HIT;
	}
	if (m->hp != (char)HP_HIT
	    && ((m->wall & 0x0c) == 0x0c || (m->wall & 0x03) == 0x03)) {
		m->nohit_f = 0;
		if (m->var == 0 && m->no_drain == 1) {
			m->no_drain = 0;
			GAME_B(G_LOCK) = 0;
			RideClear();
		}
		m->squeeze_timer = 75;
		m->hp &= 0x7f;
		if (m->squeeze_dmg >= m->hp)
			GAME_W(G_DAMAGE) += m->hp;
		m->hp -= m->squeeze_dmg;
		for (i = 79; i >= 0; i--) {
			((u_char *)&stage_objects[i])[0x76] = 0;
			((u_char *)&stage_objects[i])[0x72] = 0;
		}
		if (m->hp > 0) {
			m->hp |= HP_HIT;
			m->grab = 0;
			m->hit_class = 1;
			m->hit_flip = m->flip;
			if ((u_char)(m->form - 1) < 4)
				m->hit_class = 4;
			GAME_W(G_DAMAGE) += m->squeeze_dmg;
		} else {
			m->hp = (char)HP_HIT;
		}
	}
	if (m->spike && m->iframes == 0 && m->nohit_f != 1) {
		GAME_W(G_DAMAGE) += m->hp & 0x7f;
		m->hp = (char)HP_HIT;
	}
	if (m->hp == (char)HP_HIT) {
		m->hp = 0;
		GAME_B(G_LOCK) = 1;
		PlayerDieFlags(m);
		ChargeStop(m);
		m->col_box = 0;
		m->dmg_box = 0;
		m->act = PS_DEATH;
		m->act2 = 0;
		m->act3 = 0;
		return;
	}
	if (m->iframes) {
		if (m->iframes >= 60)
			m->iframes--;
		else if (!(m->parts & PART_BLINK) || (frame_count & 1)) {
			if (--m->iframes == 0) {
				m->hit_lock = 0;
				PlayerPalette(m);
			}
		}
	}
	if (m->hp > 0)
		return;

	PlayerHurt(m);

	if (m->var == 0) {
		m->energy[0] += ENERGY_MAX(m) / 8;
		if (ENERGY_MAX(m) < m->energy[0]
		    || ENERGY_MAX(m) - m->energy[0] < ENERGY_MAX(m) / 8)
			m->energy[0] = ENERGY_MAX(m);
	} else {
		m->energy[5] += ZeroGigaGain(m, 5) / 2;
		if (m->energy[5] >= ENERGY_MAX(m))
			m->energy[5] = ENERGY_MAX(m);
	}
	if (m->parts & PART_REFILL) {
		low = 0;
		min = m->energy[0];
		e = &m->energy[1];
		for (i = 1; i < 9; i++, e++) {
			if (*e < min) {
				min = *e;
				low = i;
			}
		}
		min += 12;
		if (ENERGY_MAX(m) < min)
			min = ENERGY_MAX(m);
		m->energy[low] = min;
	}
}

/* 0x8003963C */
void PlayerHurt(PLAYER *m)
{
	int	c, still;

	PlayerSetTerrainBox(m);
	m->hp &= 0x7f;
	Rumble(1);
	PlaySound(SE_BANK_VOICE, (GetRNG() & 1) ^ 1, m);
	PlaySound(SE_BANK_VOICE, SE_HURT, m);
	if (m->act2 == ST_HURT)
		return;

	m->hit_lock = 1;
	m->cf = 1;
	m->dash_a = 0;
	ClearShotFlags(m);
	m->dash_f = 0;
	ClearDeathFlags(m);

	c = m->hit_class;
	m->flip = m->hit_flip;
	PlayerSetAnime(m, hurt_anime[c]);
	m->spd_x = hurt_speed[c][0];
	m->acc_x = hurt_speed[c][1];
	m->spd_y = hurt_speed[c][2];
	m->grav = hurt_speed[c][3];

	still = 0;
	if (m->parts & PART_SHOCK)
		still = 1;
	else if (m->var != 0 && (GAME_B(G_ARMORS) & 0x20))
		still = 1;
	if (still && ((u_int)(c - 1) < 2 || c == 4)) {
		m->acc_x = 0;
		m->spd_x = 0;
		m->grav = 0;
		m->spd_y = 0;
	}
	if (m->flip)
		m->spd_x = -m->spd_x;
	m->iframes = hurt_iframes[c];
	if ((u_int)(c - 2) < 2 || c == 5)
		m->jump_latch = 1;
	m->act2 = ST_HURT;
	m->act3 = 0;
}

/* 0x80037838: step 17 */
void PlayerHurtStep(PLAYER *m)
{
	AnimeTick(m);
	hurt_steps[m->hit_class](m);
}

static void HurtDone(PLAYER *m)
{
	if (m->lock) {
		m->hit_lock = 0;
		m->iframes = 0;
	} else {
		LowHpWarn(m);
		m->hit_lock = -1;
	}
}

/* 0x80037880: classes 0, 1, 4 */
void HurtLight(PLAYER *m)
{
	HurtMove(m);
	if (m->var == 0) {
		if (m->anime_new == 208)
			PlayerFlash(m, 82);
		else
			PlayerPalette(m);
	}
	if (m->iframes == 60) {
		HurtEnd(m);
		HurtDone(m);
	}
}

/* 0x80037920: class 5 */
void HurtPush(PLAYER *m)
{
	HurtMove2(m);
	WaterSplashCheck(m);
	if (m->act3 == 0) {
		m->act3++;
		return;
	}
	if (m->iframes == 60) {
		StartFall(m);
		HurtDone(m);
	}
}

/* 0x800379AC: class 3 */
void HurtGrab(PLAYER *m)
{
	if (m->key & KEY_L)
		m->flip = 0;
	if (m->key & KEY_R)
		m->flip = 0x40;
	if (m->grab > 0)
		return;
	if (m->grab == 0) {
		m->hit_lock = 0;
		HurtEnd(m);
	} else if (m->grab == -1) {
		PlayerSetAnime(m, 37);
		m->grab = -2;
	} else if (m->anime.next < 0) {
		PlayerSetAnime(m, 36);
		m->grab = 1;
	}
}

/* 0x80037A60 */
void HurtMove(PLAYER *m)
{
	if (m->gate)
		return;
	if (m->spd_x && (m->wall & (m->spd_x > 0 ? WALL_R : WALL_L))) {
		m->spd_x = 0;
		m->acc_x = 0;
	}
	PlayerMoveAccel(m);
	if (m->flip ? m->spd_x > 0 : m->spd_x < 0) {
		m->spd_x = 0;
		m->acc_x = 0;
	}
}

/* 0x80037B10 */
void HurtMove2(PLAYER *m)
{
	if (m->gate)
		return;
	if (m->spd_x && (m->wall & (m->spd_x > 0 ? WALL_R : WALL_L))) {
		m->spd_x = 0;
		m->spd_y = 0;
		m->acc_x = 0;
	}
	PlayerMoveAccel(m);
	if (m->flip ? m->spd_x > 0 : m->spd_x < 0) {
		m->spd_x = 0;
		m->acc_x = 0;
	}
}

/* 0x80035BDC: class 2 (and 6..9) */
void HurtHeavy(PLAYER *m)
{
	HurtMove(m);
	WaterSplashCheck(m);
	if (m->act3 == 0) {
		m->act3++;
		return;
	}
	if (m->jump_latch && (m->wall & FLOOR)) {
		m->jump_latch = 0;
		if (!(m->parts & PART_SHOCK)
		    && !(m->var != 0 && (GAME_B(G_ARMORS) & 0x20)))
			m->spd_x = m->flip ? -0x8000 : 0x8000;
		m->spd_y = 0;
		m->grav = 0;
	}
	if (m->iframes != 60)
		return;
	if (m->wall & FLOOR) {
		m->jump_latch = 0;
		ToStand(m);
	} else if (m->grav) {
		PlayerSetAnime(m, 11);
		m->jump_latch = -1;
		m->act2 = 7;
		m->act3 = 0;
	} else {
		StartFall(m);
	}
	HurtDone(m);
}

/* 0x8003B114 */
void HurtEnd(PLAYER *m)
{
	if (m->wall & FLOOR) {
		m->jump_latch = 0;
		m->dash_b = 0;
		ClearDashFlags(m);
		if (PlayerRequests(m))
			return;
		m->act2 = ST_STAND;
		m->act3 = 0;
		if (m->var == 0 && m->shot_pose) {
			PlayerSetAnimeAt(m, 94, m->shot_timer < 5 ? 2 : m->shot_timer < 9);
			if (m->shot_timer >= 9)
				m->anime.duration = m->shot_timer - 8;
		} else {
			StandAnime(m);
		}
		return;
	}
	PlayerSetAnimeShoot(m, 11);
	m->spd_x = 0;
	m->acc_x = 0;
	m->spd_y = 0;
	m->jump_latch = -1;
	m->grav = m->k100;
	if (m->dash_a) {
		m->dash_a = 0;
		m->dash_f = -1;
	}
	m->act2 = 7;
	m->act3 = 0;
}

/* 0x8003D07C: damage while held (class 3) */
void PlayerGrabDamage(int d)
{
	PLAYER	*m = &mega;

	m->grab = -1;
	if ((m->hp & 0x7f) == 0)
		return;
	if (d)
		m->hp -= PlayerDamageScale(m, d);
	if (m->hp > 0) {
		m->hp |= HP_HIT;
	} else {
		m->grab = 0;
		GAME_B(G_LOCK) = 1;
		m->hp = (char)HP_HIT;
	}
}

/* 0x8003A980 */
void PlayerDieFlags(PLAYER *m)
{
	m->cf = 1;
	m->dash_a = 0;
	ClearShotFlags(m);
	m->dash_f = 0;
	ClearDeathFlags(m);
}

/* ---------------------------------------------------------------- death (state 2) */

/* 0x8003CC78 */
void PlayerDeath(PLAYER *m)
{
	death_steps[m->act2](m);
}

static void SetStopFlags(int v)
{
	int	i;

	for (i = 0x12; i <= 0x1a; i++)
		GAME_B(i) = v;
}

/* 0x8003D8A8 */
void DeathStart(PLAYER *m)
{
	StopSound(SE_BANK_PLAYER, SE_DASH);
	StopSound(SE_BANK_PLAYER, 7);
	Rumble(0);
	if (m->ride) {
		m->disp = 0;
		m->death_timer = 1;
	} else {
		SetStopFlags(1);
		m->death_timer = 8;
		PlayerSetAnime(m, 33);
	}
	m->act2++;
}

/* 0x8003D954 */
void DeathFreeze(PLAYER *m)
{
	if (--m->death_timer)
		return;
	SetStopFlags(0);
	m->disp = 0;
	PlaySound(SE_BANK_VOICE, VO_DEATH_A, m);
	PlaySound(SE_BANK_VOICE, VO_DEATH_B, m);
	m->death_timer = 0;
	m->death_wave = 0;
	m->act2++;
	DeathOrbs(m);
}

/* 0x8003DAF4 */
void DeathParticle(int var)
{
	OBJECT	*o = GetMiscObject();

	if (o == 0)
		return;
	o->flag = 0x21;
	o->type = 17;
	o->var = var;
	o->act = 0;
	o->act2 = 0;
	o->act3 = 0;
}

/* 0x8003DB44 */
void DeathWave(int n)
{
	int	i;

	for (i = 0; i < 8; i++)
		DeathParticle(death_dirs[n][i]);
}

/* 0x8003D9F8 */
void DeathOrbs(PLAYER *m)
{
	int	i;

	if ((m->death_timer & 3) == 0)
		for (i = 0; i < 4; i++)
			DeathParticle((GetRNG() & 0x1f) | 0x80);
	if (m->death_wave == 3) {
		m->death_timer = 20;
		m->act2++;
		return;
	}
	if (m->death_timer) {
		m->death_timer--;
		return;
	}
	DeathWave(m->death_wave);
	if (m->death_wave == 2)
		DeathWave(3);
	m->death_timer = 18;
	m->death_wave++;
}

/* 0x8003DACC */
void DeathWait(PLAYER *m)
{
	if (m->death_timer) {
		m->death_timer--;
		return;
	}
	m->act = PS_NONE;
	m->act2 = 0;
}

/* ---------------------------------------------------------------- set-up and beams */

/* 0x8003CBE4: state 0 */
void PlayerInit(PLAYER *m)
{
	int	n = 0;

	if (GAME_B(G_READY) == 0)
		return;
	m->disp = 1;
	GAME_B(0x1f) = 1;
	if (game.stage == 12 && game.area == 0)
		n = (u_char)(GAME_B(G_CHECKPOINT) - 2) < 16;
	m->act++;
	player_init[n](m);
}

/* 0x8003D850 */
void PlayerInitBeam(PLAYER *m)
{
	PlayerSetAnime(m, 1);
	PlaySound(SE_BANK_PLAYER, SE_BEAM_IN, m);
	m->spd_y = -0x80000;			/* 8 px down */
	m->spd_x = 0;
	m->acc_x = 0;
	m->grav = 0;
	m->jump_latch = -1;
	m->act2 = ST_BEAM_IN;
}

/* 0x80035144: step 0 */
void PlayerBeamIn(PLAYER *m)
{
	AnimeTick(m);
	if (m->act3 == 0) {
		if (m->beam_thru) {
			if (--m->beam_thru == 0)
				PlayerSetTerrainBox(m);
			PlayerMove(m);
			return;
		}
		if (m->wall & FLOOR) {
			PlayerSetAnime(m, 2);
			m->jump_latch = 0;
			CAM_B(0, 0x44) = 1;
			CAM_B(1, 0x44) = 1;
			CAM_B(2, 0x44) = 1;
			m->act3++;
			return;
		}
		PlayerMove(m);
		WaterSplashCheck(m);
		return;
	}
	if (m->anime.flag) {
		m->anime.flag = 0;
		PlayerLoadForm(m);
		PlayerPhysicsParts(m);
	}
	if (m->anime.next != 0)
		return;
	GAME_B(G_LOCK) = 0;
	if (m->var != 0) {
		PlayerLoadForm(m);
		PlayerPhysicsParts(m);
	}
	ToStand(m);
}

/* 0x8003B954 */
void BeamOut(PLAYER *m)
{
	PlaySound(SE_BANK_PLAYER, SE_BEAM_OUT, m);
	m->spd_y = 0x80000;			/* 8 px up */
	m->spd_x = 0;
	m->acc_x = 0;
	m->grav = 0;
	m->col_box = 0;
	m->jump_latch = 1;
	m->act2 = ST_BEAM_OUT;
	m->act3 = 0;
}

/* 0x8003674C: step 1 */
void PlayerBeamOut(PLAYER *m)
{
	short	cy;

	AnimeTick(m);
	if (m->act3 == 0) {
		if (m->anime.next != 0)
			return;
		cy = CAM_H(0, 0x0e);
		CAM_H(0, 0x28) = cy;
		CAM_H(0, 0x2a) = cy;
		CAM_H(0, 0x20) = cy;
		CAM_H(0, 0x22) = cy;
		PlayerSetAnime(m, 4);
		m->col_box = 0;
		m->act3++;
		return;
	}
	if (OffScreenCheck2(m, 32, 64)) {
		m->disp = 0;
		m->act = PS_NONE;
		m->act2 = 0;
		m->act3 = 0;
		return;
	}
	PlayerMove(m);
	WaterSplashCheck(m);
}

/* 0x8003B870 */
void PlayerExit(PLAYER *m)
{
	int	req;

	ClearActionFlags(m);
	ChargeStop(m);
	req = GAME_B(G_EXIT_REQ);
	GAME_B(G_LOCK) = 1;
	if (req & 0x10) {
		StandAnime(m);
		ChargeOff(m);
		if (m->var == 0) {
			StreamPlay(4, 127);
		} else {
			StreamPlay(5, 127);
			m->charge = 0;
		}
		m->act2 = ST_VICTORY;
		m->act3 = 0;
		return;
	}
	if (req & 0x40) {
		m->act = PS_NONE;
		m->act2 = 0;
		m->act3 = 0;
		return;
	}
	PlayerSetAnime(m, 3);
	ChargeOff(m);
	if (m->var != 0)
		m->charge = 0;
	BeamOut(m);
}

/* 0x80038144: step 25 */
void PlayerVictory(PLAYER *m)
{
	AnimeTick(m);
	if (m->act3 == 0) {
		if (stream_state)
			return;
		PlayerSetAnime(m, 39);
		m->act3++;
		return;
	}
	if (m->anime.flag & FLG_EV80) {
		m->anime.flag = 0;
		PlaySound(SE_BANK_SYS, SE_POSE, 0);
	}
	if (m->anime.next == 0) {
		PlayerSetAnime(m, 40);
		BeamOut(m);
	}
}

/* ---------------------------------------------------------------- ride */

/* 0x8003A268: each update */
void PlayerRideCheck(PLAYER *m)
{
	if (m->ride <= 0)
		return;
	ClearActionFlags(m);
	ChargeStop(m);
	m->col_box = 0;
	m->jump_latch = 1;
	m->act2 = ST_RIDE;
	m->act3 = 1;
	m->ride = -m->ride;
}

/* 0x80037BC4: step 18 */
void PlayerRide(PLAYER *m)
{
	if (GAME_B(G_EXIT_REQ)) {
		PlayerExit(m);
		return;
	}
	if (m->ride == 0) {
		PlayerSetTerrainBox(m);
		StartJump(m);
		m->dash_b = 1;
		GAME_B(G_LOCK) = 0;
		return;
	}
	if (m->act3 == 0) {
		PlayerSetAnime(m, m->ride_anime);
		m->act3++;
	}
	AnimeTick(m);
}

/* ---------------------------------------------------------------- events */

/* 0x8003A678 */
void EventWalkTo(PLAYER *m)
{
	int	d = m->x.w.hi - 64 - CAM_H(m->layer, 0x0a);
	int	stop = 0;

	if (d == 0) {
		stop = 1;
	} else if (d > 0) {
		m->flip = 0;
		stop = d < 3;
	} else {
		m->flip = 0x40;
		stop = d >= -2;
	}
	if (!stop)
		return;
	StandAnime(m);
	m->event = -1;
	m->flip = 0x40;
	m->act2 = ST_EV_HOLD;
	m->x.w.hi = CAM_H(m->layer, 0x0a) + 64;
}

/* 0x8003AAD0 */
void PlayerStartEvent(PLAYER *m)
{
	m->act3 = 0;
	m->act2 = m->event_step;
	ClearActionFlags(m);

	switch (m->act2) {			/* table 0x800113C4 */
	case ST_EV_HOLD:
		if ((u_char)(m->anime_no - 5) >= 2)
			StandAnime(m);
		break;
	case 73:
		if (m->anime_no != 6)
			PlayerSetAnime(m, 6);
		break;
	case ST_EV_WALK:
		PlayerSetAnime(m, 38);
		m->spd_y = 0;
		m->acc_x = 0;
		m->grav = 0;
		if (m->event_flip == 0)
			EventWalkTo(m);
		else
			m->flip = 0x40;
		return;
	case ST_EV_WALK1:
		PlayerSetAnime(m, 38);
		m->event = -1;
		m->spd_x = 0x10000;
		m->spd_y = 0;
		m->acc_x = 0;
		m->grav = 0;
		m->flip = m->event_flip;
		if (m->event_flip == 0)
			m->spd_x = -0x10000;
		return;
	case ST_EV_LEAP:
		PlayerSetAnime(m, 38);
		m->jump_latch = 1;
		m->spd_x = 0x20000;
		m->acc_x = 0;
		m->spd_y = 0x2e800;
		m->grav = 0x840;
		m->act3 = 0;
		PlayVoice(m, voice_tbl[m->var * 4 + (GetRNG() & 3)]);
		PlayerSetAnime(m, 10);
		m->flip = m->event_flip;
		return;
	case ST_EV_WARP:
		PlayerSetAnime(m, 3);
		break;
	case ST_EV_JUMP:
		PlayerSetAnime(m, 10);
		PlayerSetTerrainBox(m);
		m->spd_x = 0;
		m->acc_x = 0;
		m->jump_latch = 1;
		m->spd_y = m->jump_spd;
		m->grav = m->k100;
		m->flip = m->event_flip;
		return;
	case ST_EV_CAPSULE:
		PlayerSetAnime(m, m->var ? 5 : 107);
		ChargeOff(m);
		m->flip = m->event_flip;
		return;
	default:
		return;
	}
	m->event = -1;
	m->flip = m->event_flip;
}

/* 0x80037DC4: step 20 */
void PlayerEventHold(PLAYER *m)
{
	PlayerSetTerrainBox(m);
	if (PlayerRequests(m))
		return;
	if (m->event == 0) {
		m->act2 = ST_STAND;
		m->act3 = 0;
		return;
	}
	AnimeTick(m);
}

/* 0x80037E88: step 21 */
void PlayerEventWalk(PLAYER *m)
{
	if (GAME_B(G_EXIT_REQ)) {
		PlayerExit(m);
		return;
	}
	m->spd_x = m->anime.flag ? m->walk_spd : m->walk1_spd;
	if (m->flip == 0)
		m->spd_x = -m->spd_x;
	PlayerMove(m);
	AnimeTick(m);
	if (m->event_flip == 0)
		EventWalkTo(m);
}

/* 0x80037F34: step 29 */
void PlayerEventWalk1(PLAYER *m)
{
	if (PlayerRequests(m))
		return;
	PlayerMove(m);
	AnimeTick(m);
}

/* 0x80037F70: step 22 */
void PlayerEventWarp(PLAYER *m)
{
	if (PlayerRequests(m))
		return;
	if (m->act3)
		return;
	AnimeTick(m);
	if (m->anime.next == 0) {
		m->disp = 0;
		m->act3++;
	}
}

/* 0x80037FD4: step 23 */
void PlayerEventJump(PLAYER *m)
{
	if (m->act3 == 0) {
		AnimeTick(m);
		PlayerMoveAccel(m);
		if (m->spd_y <= 0) {
			PlayerSetAnime(m, 11);
			m->jump_latch = -1;
			m->spd_y = 0;
			m->act3++;
		}
		return;
	}
	if (m->wall & FLOOR) {
		ClearActionFlags(m);
		StandAnime(m);
		m->jump_latch = 0;
		m->event = -1;
		m->act2 = ST_EV_HOLD;
		m->act3 = 0;
		return;
	}
	AnimeTick(m);
	PlayerMoveAccel(m);
}

/* 0x8003809C: step 24 */
void PlayerEventCapsule(PLAYER *m)
{
	AnimeTick(m);
	if (m->act3 == 0) {
		if (m->capsule == 4) {
			PlayerSetAnime(m, 39);
			m->act3++;
		}
		return;
	}
	if (m->anime.flag & FLG_EV80) {
		m->anime.flag = 0;
		PlaySound(SE_BANK_SYS, SE_POSE, 0);
	}
	if (m->anime.next == 0) {
		PlayerEventEnd();
		ToStand(m);
	}
}

/* 0x80036364: step 30 */
void PlayerEventLeap(PLAYER *m)
{
	AnimeTick(m);
	PlayerMoveAccel(m);
	if (m->act3 == 2) {
		if (m->anime.next == 0)
			PlayerRequests(m);
		return;
	}
	if (m->act3 == 1) {
		if (m->spd_x > 0)
			return;
		PlayerSetAnime(m, 82);
		m->spd_x = 0;
		m->acc_x = 0;
		m->act3++;
		return;
	}
	if (m->anime.flag & FLG_EV80) {
		m->anime.flag &= FLG_FRAME;
		PlaySound(SE_BANK_PLAYER, SE_JUMP, m);
	}
	if (m->spd_y <= 0) {
		PlayerSetAnime(m, 11);
		m->jump_latch = -1;
	}
	if (m->jump_latch == -1 && (m->wall & FLOOR)) {
		PlayerSetAnime(m, 81);
		m->spd_x = 0x20000;
		m->acc_x = -0x1000;
		m->spd_y = 0;
		m->grav = 0;
		m->jump_latch = 0;
		m->act3++;
	}
}

/* ---------------------------------------------------------------- Zero */

/* 0x80036800: step 5 (Zero's stop pose) */
void PlayerZeroStop(PLAYER *m)
{
	extern int TryLadder(PLAYER *), TryDashJump(PLAYER *), TryDirection(PLAYER *);
	extern int ZeroTryA(PLAYER *), ZeroTryC(PLAYER *), ZeroTryD(PLAYER *), ZeroTryF(PLAYER *);
	extern void ToWalkStart(PLAYER *);

	if (PlayerRequests(m) || TryLadder(m))
		return;
	if (m->var != 0 && (ZeroTryA(m) || ZeroTryB(m) || ZeroTryC(m)))
		return;
	if (TryDashJump(m))
		return;
	if (m->var != 0 && (ZeroTryD(m) || ZeroTryF(m)))
		return;
	if (TryDirection(m)) {
		ToWalkStart(m);
		return;
	}
	AnimeTick(m);
	if (m->anime.next < 0)
		ToStand(m);
}

/* 0x801EC9C4 (overlay 1): Zero's idle */
void ZeroIdle(PLAYER *m)
{
	if (m->anime_no == 6 && m->hp >= (char)GAME_B(0x5b + m->var) / 3) {
		PlayerSetAnime(m, 5);
		return;
	}
	AnimeTick(m);
}
