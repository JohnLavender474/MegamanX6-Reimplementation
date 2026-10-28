/*
 *	ground.c  --  player ground movement (main program, shared by X and Zero)
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
extern void	PlayerSetTerrainBox(PLAYER *m);
extern int	PlayerRequests(PLAYER *m);
extern void	PlaySound(int bank, int id, void *o);
extern void	StopSound(int bank, int id);
extern int	GetRNG(void);
extern int	TileAbove(PLAYER *m);
extern int	TileBelow(PLAYER *m);
extern int	WeaponPose(PLAYER *m);
extern int	TrySuperJump(PLAYER *m);
extern void	StartJump(PLAYER *m);
extern void	LadderFromGround(PLAYER *m);
extern void	LadderDown(PLAYER *m);
extern void	DashDust(PLAYER *m);
extern void	DashTrail(PLAYER *m);
extern void	DashEvent40(PLAYER *m);
extern void	PlayVoice(PLAYER *m, int id);
extern void	JumpVoiceDone(PLAYER *m);
extern void	XIdle(PLAYER *m);
extern void	ZeroIdle(PLAYER *m);
extern int	ZeroTryA(PLAYER *m), ZeroTryB(PLAYER *m), ZeroTryC(PLAYER *m);
extern int	ZeroTryD(PLAYER *m), ZeroTryE(PLAYER *m), ZeroTryF(PLAYER *m);

extern u_char	game_c9;			/* 0x800CCF99 */
extern u_char	voice_tbl[];			/* 0x80073CE4 */
extern HITBOX	dash_box_x;			/* 0x80073D58 */
extern HITBOX	dash_box_zero;			/* 0x80073D5C */
extern void	(*dash_steps[])(PLAYER *);	/* 0x80073C04 */
extern char	game_max_hp[];			/* game+0x5B */

/* 0x8003D598 */
void PlayerPhysicsDefaults(PLAYER *m)
{
	m->walk1_spd = 0x8000;
	m->jump_grav = 0x4200;
	m->k100 = 0x4200;
	m->dash_acc = -0x3000;
	m->walk_spd = 0x20000;
	m->jump_spd = 0x5d000;
	m->fall_max = -0x68000;
	m->dash_spd = 0x68000;
	m->dash_min = 0x42000;
	m->k110 = -0x17000;
	m->k114 = 0x5a000;
	m->k118 = 0x10000;
	m->k11c = 0x18000;
	m->k120 = 0x20000;
	m->k124 = 0x20000;
	m->k128 = 0x18000;
	m->k12c = 0x42000;
	m->k130 = 0x42000;
}

/* 0x8003D630 */
void PlayerPhysicsParts(PLAYER *m)
{
	PlayerPhysicsDefaults(m);
	if (m->parts & PART_JUMP)
		m->jump_grav -= 0x1080;
	if (m->form == 2)
		m->k110 = 0;
	if (m->parts & PART_DASH) {
		m->dash_spd += 0x1a000;
		m->dash_acc += 0xc00;
		m->dash_min += 0x10800;
		m->k12c += 0x10800;
		m->k130 += 0x10800;
	}
	if (m->parts & PART_WALK) {
		m->walk1_spd += 0x4000;
		m->walk_spd += 0x10000;
	}
}

/* 0x8003D10C */
void StandAnime(PLAYER *m)
{
	if (m->hp < game_max_hp[m->var] / 3)
		PlayerSetAnime(m, 6);
	else
		PlayerSetAnime(m, 5);
}

/* 0x8003F4C4 */
void PlayerSetAnimeShoot(PLAYER *m, int n)
{
	if (m->var == 0 && m->shot_pose)
		n += ANM_SHOT_OFS;
	PlayerSetAnime(m, n);
}

/* 0x8003F508 */
void PlayerSyncShootAnime(PLAYER *m, int n)
{
	char keep;

	if (m->var != 0)
		return;
	if (!m->pose_end) {
		if (!m->shot_pose)
			return;
		n += ANM_SHOT_OFS;
		if (m->anime_no == n)
			return;
	}
	keep = m->anime.time;
	PlayerSetAnimeAt(m, n, m->anime.flag & FLG_FRAME);
	m->anime.time = keep;
}

/* 0x8003CD9C */
void ClearDashFlags(PLAYER *m)
{
	m->dash_a = 0;
	m->dash_b = 0;
	if (m->dash_f > 0)
		m->dash_f = -1;
}

/* 0x80039CFC */
int TryDirection(PLAYER *m)
{
	if (m->lock || !(m->key & (KEY_R | KEY_L)))
		return 0;
	if (m->key & KEY_R) {
		m->flip = OBJ_FLIP_R;
		if (m->wall & WALL_R)
			return 0;
		m->spd_x = m->walk1_spd;
	} else {
		m->flip = OBJ_FLIP_L;
		if (m->wall & WALL_L)
			return 0;
		m->spd_x = -m->walk1_spd;
	}
	return 1;
}

/* 0x80039D7C */
int TryDirectionWalk(PLAYER *m)
{
	if (m->lock || !(m->key & (KEY_R | KEY_L)))
		return 0;
	if (m->key & KEY_R) {
		m->flip = OBJ_FLIP_R;
		if (m->wall & WALL_R)
			return 0;
		m->spd_x = m->walk_spd;
	} else {
		m->flip = OBJ_FLIP_L;
		if (m->wall & WALL_L)
			return 0;
		m->spd_x = -m->walk_spd;
	}
	return 1;
}

/* 0x80039EB0 */
int TryDashInput(PLAYER *m)
{
	if (m->lock)
		return 0;
	if (!m->dash_key && !(m->trg & KEY_DASH))
		return 0;
	if (m->key & KEY_R)
		m->flip = OBJ_FLIP_R;
	if (m->key & KEY_L)
		m->flip = OBJ_FLIP_L;
	if (m->flip) {
		if (m->wall & WALL_R)
			return 0;
		m->spd_x = m->dash_spd;
	} else {
		if (m->wall & WALL_L)
			return 0;
		m->spd_x = -m->dash_spd;
	}
	return 1;
}

/* 0x8003A9C4 */
void StartDash(PLAYER *m)
{
	m->dash_a = 1;
	m->dash_b = 1;
	if (m->trg & KEY_JUMP) {
		m->jump_latch = 1;
		m->dash_f = 1;
		m->dash_b = 2;
		m->spd_x = 0;
		m->acc_x = 0;
		m->jump_work = 10;
		m->spd_y = m->jump_spd;
		m->grav = m->jump_grav;
		PlayerSetAnimeShoot(m, 10);
		if (GetRNG() & 4)
			PlayVoice(m, voice_tbl[m->var * 4 + (GetRNG() & 3)]);
		JumpVoiceDone(m);
		m->act = ST_JUMP;
	} else {
		PlayerSetAnimeShoot(m, 16);
		m->spd_y = 0;
		m->grav = 0;
		m->acc_x = m->dash_acc;
		m->dash_timer = (m->parts & PART_DASH) ? 15 : 30;
		m->act = ST_DASH;
	}
	m->act2 = 0;
}

/* 0x80039C40 */
int TryDashJump(PLAYER *m)
{
	if (TryDashInput(m)) {
		StartDash(m);
		return 1;
	}
	if (m->trg & KEY_JUMP) {
		StartJump(m);
		return 1;
	}
	return 0;
}

/* 0x8003AE8C */
void ToWalkStart(PLAYER *m)
{
	PlayerSetAnimeShoot(m, 7);
	m->spd_y = 0;
	m->acc_x = 0;
	m->grav = 0;
	PlayerMove(m);
	m->act = ST_WALK1;
	m->act2 = 0;
}

/* 0x8003AED4 */
void ToWalk(PLAYER *m)
{
	PlayerSetAnimeShoot(m, 8);
	m->spd_y = 0;
	m->acc_x = 0;
	m->grav = 0;
	PlayerMove(m);
	m->act = ST_WALK;
	m->act2 = 0;
}

/* 0x80039BC4 */
int TryLand(PLAYER *m)
{
	if (TryDashInput(m)) {
		StartDash(m);
		return 1;
	}
	if (m->trg & KEY_JUMP) {
		StartJump(m);
		return 1;
	}
	if (TryDirectionWalk(m)) {
		ToWalk(m);
		return 1;
	}
	return 0;
}

/* 0x8003B9AC */
void StartCrouch(PLAYER *m)
{
	if (PlayerRequests(m))
		return;
	PlayerSetAnime(m, (*(u_long *)&m->dash_f & 0xffff0000) ? 109 : 81);
	m->act = ST_CROUCH;
	m->act2 = 0;
}

/* 0x80039C9C */
int TryCrouch(PLAYER *m)
{
	if (m->lock || !(game_c9 & 8) || !(m->key & KEY_D))
		return 0;
	StartCrouch(m);
	return 1;
}

/* 0x8003A3DC */
int TryLadder(PLAYER *m)
{
	if (m->lock || m->ladder_timer)
		return 0;
	if ((m->key & KEY_U) && TileAbove(m) == 32) {
		LadderFromGround(m);
		return 1;
	}
	if ((m->key & KEY_D) && TileBelow(m) == 33) {
		LadderDown(m);
		return 1;
	}
	return 0;
}

static void stand_pose(PLAYER *m)
{
	int frame;

	m->act = ST_STAND;
	m->act2 = 0;
	if (m->var == 0 && m->shot_pose) {
		frame = (m->shot_timer < 9) ? 1 : 0;
		if (m->shot_timer < 5)
			frame = 2;
		PlayerSetAnimeAt(m, 94, frame);
		if (m->shot_timer >= 9)
			m->anime.time = m->shot_timer - 8;
	} else
		StandAnime(m);
}

/* 0x8003AD18 */
void ToStand(PLAYER *m)
{
	ClearDashFlags(m);
	if (PlayerRequests(m))
		return;
	stand_pose(m);
}

/* 0x8003ADB4 */
void Stop(PLAYER *m)
{
	if (PlayerRequests(m))
		return;
	if (m->var != 0) {
		PlayerSetAnime(m, 9);
		m->act = ST_ZSTOP;
		m->act2 = 0;
		return;
	}
	ClearDashFlags(m);
	if (PlayerRequests(m))
		return;
	stand_pose(m);
}

/* 0x8003B054 */
void ToLand(PLAYER *m)
{
	PlaySound(1, 2, m);
	m->jump_latch = 0;
	ClearDashFlags(m);
	if (PlayerRequests(m) || WeaponPose(m))
		return;
	if (m->var && ZeroTryC(m))
		return;
	if (TryLand(m))
		return;
	if (m->var && ZeroTryD(m))
		return;
	PlayerSetAnimeShoot(m, 12);
	m->act = ST_LAND;
	m->act2 = 0;
}

/* step 2, 0x80035274 */
void PlayerStand(PLAYER *m)
{
	PlayerSetTerrainBox(m);
	if (PlayerRequests(m) || TryLadder(m) || WeaponPose(m))
		return;
	if (m->var && (ZeroTryA(m) || ZeroTryB(m) || ZeroTryC(m)))
		return;
	if (TrySuperJump(m) || TryDashJump(m))
		return;
	if (m->var) {
		if (ZeroTryD(m))
			return;
		if (!ZeroTryE(m) && ZeroTryF(m))
			return;
	}
	if (TryCrouch(m))
		return;
	if (TryDirection(m)) {
		ToWalkStart(m);
		return;
	}
	if (m->var == 0)
		XIdle(m);
	else
		ZeroIdle(m);
}

/* step 3, 0x800353C0 */
void PlayerWalkStart(PLAYER *m)
{
	if (PlayerRequests(m) || TryLadder(m) || WeaponPose(m))
		return;
	if (m->var && (ZeroTryA(m) || ZeroTryB(m) || ZeroTryC(m)))
		return;
	if (TryDashJump(m))
		return;
	if (m->var && (ZeroTryD(m) || ZeroTryF(m)))
		return;
	if (!TryDirection(m)) {
		Stop(m);
		return;
	}
	AnimeTick(m);
	if (m->anime.next < 0) {
		ToWalk(m);
		return;
	}
	PlayerMove(m);
	PlayerSyncShootAnime(m, 7);
}

/* step 4, 0x800354E8 */
void PlayerWalk(PLAYER *m)
{
	if (PlayerRequests(m) || TryLadder(m) || WeaponPose(m))
		return;
	if (m->var && (ZeroTryA(m) || ZeroTryB(m) || ZeroTryC(m)))
		return;
	if (TrySuperJump(m) || TryDashJump(m))
		return;
	if (m->var) {
		if (ZeroTryD(m))
			return;
		if (!ZeroTryE(m) && ZeroTryF(m))
			return;
	}
	if (TryCrouch(m))
		return;
	if (!TryDirectionWalk(m)) {
		Stop(m);
		return;
	}
	AnimeTick(m);
	PlayerMove(m);
	PlayerSyncShootAnime(m, 8);
}

/* step 8, 0x80036A04 */
void PlayerLand(PLAYER *m)
{
	if (PlayerRequests(m) || TryLadder(m) || TrySuperJump(m) || WeaponPose(m))
		return;
	if (m->var && (ZeroTryA(m) || ZeroTryC(m)))
		return;
	if (TryLand(m))
		return;
	if (m->var && (ZeroTryD(m) || ZeroTryF(m)))
		return;
	if (TryCrouch(m))
		return;
	AnimeTick(m);
	if (m->anime.next < 0)
		Stop(m);
	else
		PlayerSyncShootAnime(m, 12);
}

/* 0x8003CD00 */
void DashTerrainBox(PLAYER *m)
{
	if (m->dash_box < 0)
		m->col_box = 0;
	else
		m->col_box = m->var ? &dash_box_zero : &dash_box_x;
}

/* 0x80039FFC: 1 = end the dash */
int DashStep(PLAYER *m)
{
	int h;

	if (m->wall & (m->flip ? WALL_R : WALL_L))
		return 1;
	PlayerMoveAccel(m);
	if (m->flip) {
		if (m->spd_x < m->dash_min)
			m->spd_x = m->dash_min;
	} else {
		if (-m->dash_min < m->spd_x)
			m->spd_x = -m->dash_min;
	}
	if (--m->dash_timer == 0)
		return 1;
	h = m->key & (KEY_DASH | KEY_R | KEY_L);
	if (h == 0)
		return 1;
	if (m->flip)
		return (h & KEY_L) != 0;
	return (h & KEY_R) != 0;
}

/* 0x8003B218 */
void DashEnd(PLAYER *m)
{
	PlayerSetAnimeShoot(m, 17);
	StopSound(1, 5);
	m->spd_x = m->flip ? m->dash_min : -m->dash_min;
	m->dash_f = -1;
	m->dash_a = 0;
	m->dash_b = 0;
	m->act2 = 2;
	m->acc_x = m->dash_acc;
}

/* dash sub-step 0, 0x800357A8 */
void PlayerDashStart(PLAYER *m)
{
	if (WeaponPose(m))
		return;
	if (m->var && (ZeroTryA(m) || ZeroTryB(m) || ZeroTryC(m) || ZeroTryF(m)))
		return;
	if (m->trg & KEY_JUMP) {
		StartJump(m);
		return;
	}
	if (m->var && ZeroTryD(m))
		return;
	if (m->anime.flag & FLG_EV20) {
		m->anime.flag &= 0x0f;
		DashDust(m);
	}
	if (m->anime.flag & FLG_EV80) {
		m->anime.flag &= 0x0f;
		PlaySound(1, 5, m);
		m->dash_f = 1;
		m->act2++;
		return;
	}
	PlayerSyncShootAnime(m, 16);
}

/* dash sub-step 1, 0x800358D4 */
void PlayerDashRun(PLAYER *m)
{
	PlayerSetTerrainBox(m);
	if (WeaponPose(m))
		return;
	if (m->var) {
		if (ZeroTryA(m) || ZeroTryB(m) || ZeroTryC(m))
			return;
		if (!ZeroTryE(m) && ZeroTryF(m))
			return;
	}
	if (TrySuperJump(m))
		return;
	if (m->trg & KEY_JUMP) {
		StartJump(m);
		return;
	}
	if (m->var && ZeroTryD(m))
		return;
	if (DashStep(m)) {
		DashEnd(m);
		return;
	}
	if (m->anime.flag & FLG_EV40) {
		m->anime.flag &= 0x0f;
		DashEvent40(m);
	}
	if (!(m->dash_timer & 3))
		DashTrail(m);
	PlayerSyncShootAnime(m, 16);
	DashTerrainBox(m);
}

/* dash sub-step 2, 0x80035A2C */
void PlayerDashSlide(PLAYER *m)
{
	if (TryLadder(m) || WeaponPose(m))
		return;
	if (m->var && (ZeroTryA(m) || ZeroTryB(m) || ZeroTryC(m)))
		return;
	if (TryLand(m))
		return;
	if (m->var && (ZeroTryD(m) || ZeroTryF(m)))
		return;
	if (m->anime.flag & FLG_EV80) {
		m->anime.flag &= 0x7f;
		PlaySound(1, 6, m);
	}
	if ((m->anime.flag & FLG_EV40) && m->dash_timer == 0) {
		if (m->wall & (m->flip ? WALL_R : WALL_L))
			m->spd_x = 0;
		if (m->spd_x) {
			PlayerMoveAccel(m);
			if (m->flip ? m->spd_x < 0 : m->spd_x > 0)
				m->spd_x = 0;
		}
	}
	if (m->anime.next < 0)
		ToStand(m);
	else
		PlayerSyncShootAnime(m, 17);
}

/* step 12, 0x800370B0 */
void PlayerDash(PLAYER *m)
{
	if (!PlayerRequests(m)) {
		AnimeTick(m);
		dash_steps[m->act2](m);
	}
	if (m->act != ST_DASH && m->col_box)
		PlayerSetTerrainBox(m);
}
