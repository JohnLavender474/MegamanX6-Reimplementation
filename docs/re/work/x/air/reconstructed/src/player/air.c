/*
 *	air.c  --  player air movement (main program, shared by X and Zero)
 *
 *	A plausible reconstruction from the disassembly; not original code, not compiled.
 *	Functions are in address order. Zero's overlay tries are named by address.
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
extern void	PlayerSyncShootAnime(PLAYER *m, int n);
extern void	PlayerSetTerrainBox(PLAYER *m);
extern int	PlayerRequests(PLAYER *m);
extern void	PlaySound(int bank, int id, void *o);
extern void	StopSound(int bank, int id);
extern int	GetRNG(void);
extern int	TryLadder(PLAYER *m);
extern int	TrySuperJump(PLAYER *m);
extern int	TryDashInput(PLAYER *m);
extern int	DashStep(PLAYER *m);
extern void	ToLand(PLAYER *m);
extern void	ClearDashFlags(PLAYER *m);
extern void	StandAnime(PLAYER *m);
extern void	ClearShotPose(PLAYER *m);		/* 0x8003CDBC */
extern void	ShotPoseSetup(PLAYER *m);		/* 0x8003F3DC */
extern void	LadderFromAir(PLAYER *m);		/* 0x8003B7B4, part 2c */
extern void	WeaponFire(PLAYER *m, int a);		/* 0x8003F864 */
extern int	WeaponTryA(PLAYER *m);			/* 0x8003E344 */
extern void	WeaponTryB(PLAYER *m);			/* 0x8003E614 */
extern int	TileAtHands(PLAYER *m);			/* 0x8002FE30 */
extern int	TileHang(PLAYER *m);			/* 0x8003040C */
extern int	TileAtBody(PLAYER *m);			/* 0x8003015C */
extern int	TileAtFeet(PLAYER *m);			/* 0x8002FF70 */
extern int	TileAbove2(PLAYER *m, int x, int y);	/* 0x80030304 */
extern int	BarSnapY(PLAYER *m, int tile);		/* 0x800397F8 */
extern int	BarShape(PLAYER *m, int tile);		/* 0x8003A814 */
extern void	WaterSplash(PLAYER *m, int y);		/* 0x8003CA70 */
extern void	AirDashJet(PLAYER *m);			/* 0x8003D26C */
extern void	WallSlideDust(PLAYER *m);		/* 0x8003D2BC */
extern void	WallKickSpark(PLAYER *m);		/* 0x8003C964 */
extern void	*MakeObject(int a, int type, int var, void *p);	/* 0x8003D3F8 */
extern void	*MakeEffect(int a, int type, int var, void *p);	/* 0x8003D510 */
extern int	BarDashStep(PLAYER *m);			/* 0x80038594, not read */
extern int	XCeilingShot(PLAYER *m);		/* 0x8003F700, not read */
extern int	XShootLadderAir(PLAYER *m);		/* 0x801EB6E8 */
extern int	Zero_801ea560(PLAYER *m), Zero_801ea454(PLAYER *m), Zero_801ebf94(PLAYER *m);
extern int	Zero_801ec028(PLAYER *m), Zero_801ec178(PLAYER *m), Zero_801ec298(PLAYER *m);
extern int	Zero_801ea004(PLAYER *m), Zero_801ea124(PLAYER *m), Zero_801eca50(PLAYER *m);

extern u_char	voice_tbl[];			/* 0x80073CE4: X 2 3 6 6, Zero 2 4 7 7 */
extern void	(*air_dash_steps[])(PLAYER *);	/* 0x80073C10 */
extern void	(*super_jump_steps[])(PLAYER *);/* 0x80073C58 */
extern u_long	frame_count;

#define HELD(m, k)	((m)->key & (k))
#define PRESSED(m, k)	((m)->trg & (k))
#define FRONT(m)	((m)->flip ? WALL_R : WALL_L)
#define BEHIND(m)	((m)->flip ? WALL_L : WALL_R)

static int	TryAirDash(PLAYER *m);
static int	TryHang(PLAYER *m);
static int	TryWall(PLAYER *m);
static int	TryWallKick(PLAYER *m);
static int	WallHeld(PLAYER *m);
static int	TryLadderAir(PLAYER *m);
static int	HangExit(PLAYER *m);
static void	StartJump(PLAYER *m);
static void	ToFall(PLAYER *m);
static void	LandOrFall(PLAYER *m);
static void	StartAirDash(PLAYER *m);
static void	AirDashStop(PLAYER *m);
static void	AirDashShot(PLAYER *m);
static void	StartWallGrab(PLAYER *m);
static void	StartWallKick(PLAYER *m);
static void	StartWallSlide(PLAYER *m);
static void	WallRelease(PLAYER *m);
static void	PlayVoice(PLAYER *m, int id);
static void	AirControl(PLAYER *m);
static void	SplashCheck(PLAYER *m);
static int	AirWeaponPose(PLAYER *m);
static int	HangWeapon(PLAYER *m);
static int	UltimateTry(PLAYER *m);
static int	FalconTry(PLAYER *m);
static int	BladeTry(PLAYER *m);
static int	RopeDashStep(PLAYER *m);
static int	RopeClimb(PLAYER *m);
static int	RopeTryDash(PLAYER *m);
static int	BarMove(PLAYER *m);
static int	BarTryDash(PLAYER *m);
static int	FalconDashStep(PLAYER *m);

/* 0x80035620 -- step 6: jump (rising) */
void Jump(PLAYER *m)
{
	if (m->lock)
		m->spd_y = 0;
	if (m->wall & COL_U)
		m->spd_y = 0;
	if (!HELD(m, KEY_JUMP))
		m->spd_y = 0;
	if (m->var == 0) {
		if (AirWeaponPose(m))
			return;
	} else {
		if (Zero_801ea560(m) || Zero_801ea454(m) || Zero_801ebf94(m))
			return;
	}
	AnimeTick(m);
	if (m->anime.flag & FLG_EV80) {
		m->anime.flag &= FLG_FRAME;
		PlaySound(1, 1, m);
	}
	if (TryLadderAir(m))
		return;
	if (TryHang(m))
		return;
	if (m->jump_work == 0) {
		if (TryWall(m))
			return;
	} else {
		m->jump_work--;
	}
	if (TryAirDash(m))
		return;
	AirControl(m);
	SplashCheck(m);
	if (m->spd_y <= 0) {
		PlayerSetAnimeShoot(m, 11);
		m->jump_latch = -1;
		m->act = ST_FALL;
		m->act2 = 0;
		return;
	}
	PlayerSyncShootAnime(m, 10);
}

/* 0x8003605c -- step 27: hang on a rope (vertical) */
void Rope(PLAYER *m)
{
	char keep;

	if (HELD(m, KEY_L))
		m->flip = OBJ_FLIP_L;
	if (HELD(m, KEY_R))
		m->flip = OBJ_FLIP_R;
	if (HangWeapon(m) || HangExit(m))
		return;
	if (m->var && (Zero_801ea560(m) || Zero_801ea004(m)))
		return;
	if (m->act2 == 0) {
		AnimeTick(m);
		if (m->weapon_pose && m->var == 0) {
			keep = m->anime.duration;
			PlayerSetAnimeAt(m, 144, m->anime.flag & FLG_FRAME);
			m->anime.duration = keep;
		}
		if (m->anime.next == 0)
			m->act2 = 1;
		return;
	}
	RopeTryDash(m);
	RopeClimb(m);
	if (m->old_y != m->y) {
		if (m->hang_snd == 0)
			PlaySound(0, 39, m);
		if (++m->hang_snd >= 21)
			m->hang_snd = 0;
	}
}

/* 0x800361d4 -- step 28: hang on a bar (horizontal or slanted) */
void Bar(PLAYER *m)
{
	char keep;

	if (HELD(m, KEY_L))
		m->flip = OBJ_FLIP_L;
	if (HELD(m, KEY_R))
		m->flip = OBJ_FLIP_R;
	if (HangWeapon(m) || HangExit(m))
		return;
	if (m->var && (Zero_801ea560(m) || Zero_801ea124(m)))
		return;
	if (m->act2 == 0) {
		AnimeTick(m);
		if (m->weapon_pose && m->var == 0) {
			keep = m->anime.duration;
			PlayerSetAnimeAt(m, 148 + BarShape(m, TileAtHands(m)), m->anime.flag & FLG_FRAME);
			m->anime.duration = keep;
		}
		if (m->anime.next == 0)
			m->act2 = 1;
		return;
	}
	BarTryDash(m);
	BarMove(m);
	if (m->old_x != m->x) {
		if (m->hang_snd == 0)
			PlaySound(0, 39, m);
		if (++m->hang_snd >= 21)
			m->hang_snd = 0;
	}
}

/* 0x8003649c -- step 31 sub-step 0: rise at a fixed speed */
void SJumpRise(PLAYER *m)
{
	if (m->wall & COL_U) {
		if ((m->col_f & COL_U) && TileAbove2(m, POS_X(m), POS_Y(m)) == 57) {
			ToFall(m);
			return;
		}
		PlaySound(1, 4, m);
		PlayerSetAnime(m, 83);
		m->act2 = 2;
		m->act4 = m->var ? 120 : 180;		/* ceiling time */
		return;
	}
	m->act4++;
	PlayerMove(m);
	AnimeTick(m);
	PlayerSyncShootAnime(m, 10);
	if (((m->spd_y * m->act4) >> 16) >= 144) {
		m->act4 = 5;
		m->act2++;
		PlayerSetAnime(m, 11);
	}
}

/* 0x800365d4 -- step 31 sub-step 3: hold on to the ceiling */
void SJumpCeiling(PLAYER *m)
{
	if (HELD(m, KEY_R))
		m->flip = OBJ_FLIP_R;
	if (HELD(m, KEY_L))
		m->flip = OBJ_FLIP_L;
	if (TryDashInput(m)) {
		StartAirDash(m);
		PlayerSetAnime(m, 85);
		m->act = ST_SJUMP;
		m->act2 = 4;
		return;
	}
	AnimeTick(m);
	if (HELD(m, KEY_D) || PRESSED(m, KEY_JUMP) || !(m->wall & COL_U))
		m->act4 = 0;
	if (m->act4) {
		m->act4--;
		if (m->var == 0)
			XCeilingShot(m);
		else if (Zero_801eca50(m))
			return;
	}
	if (m->var == 0) {
		if (m->weapon_pose)
			PlayerSetAnime(m, 140);
		if (m->pose_end)
			PlayerSetAnime(m, 84);
	}
	if (m->act4 == 0 && m->anime.next == 0)
		LandOrFall(m);
}

/* 0x800368fc -- step 7: fall */
void Fall(PLAYER *m)
{
	if (m->wall & COL_D) {
		if (!TrySuperJump(m))
			ToLand(m);
		return;
	}
	if (m->var == 0) {
		if (AirWeaponPose(m))
			return;
	} else {
		if (Zero_801ea560(m) || Zero_801ea454(m) || Zero_801ec028(m))
			return;
	}
	if (TryLadderAir(m) || TryHang(m) || TryWall(m) || TryAirDash(m))
		return;
	AnimeTick(m);
	AirControl(m);
	SplashCheck(m);
	PlayerSyncShootAnime(m, 11);
}

/* 0x80036b10 -- step 9: wall grab (the first updates on a wall) */
void WallGrab(PLAYER *m)
{
	if (m->wall & COL_D) {
		ToLand(m);
		return;
	}
	if (m->lock) {
		ToFall(m);
		return;
	}
	if (AirWeaponPose(m))
		return;
	if (!(m->wall & (WALL_R | WALL_L))) {
		ToFall(m);
		return;
	}
	if (TryWallKick(m))
		return;
	AnimeTick(m);
	if (m->anime.flag & FLG_EV80) {
		m->flip = (m->wall & WALL_R) ? OBJ_FLIP_L : OBJ_FLIP_R;	/* face away */
		m->anime.flag &= FLG_FRAME;
	}
	if (--m->wall_timer == 0) {
		StartWallSlide(m);
		return;
	}
	PlayerSyncShootAnime(m, 13);
}

/* 0x80036c28 -- step 10: wall kick */
void WallKick(PLAYER *m)
{
	extern void WallKickPush(PLAYER *), WallKickRise(PLAYER *);

	if (m->act2 == 0)
		WallKickPush(m);
	else
		WallKickRise(m);
}

/* 0x80036c68 -- step 10 sub-step 0: the fixed push away from the wall */
void WallKickPush(PLAYER *m)
{
	int stop;

	if (m->lock) {
		ToFall(m);
		return;
	}
	if (AirWeaponPose(m))
		return;
	stop = (m->wall & COL_U) != 0;
	if (m->wall & BEHIND(m))
		stop = 1;
	if (stop) {
		ToFall(m);
		return;
	}
	AnimeTick(m);
	PlayerMoveAccel(m);
	SplashCheck(m);
	if (--m->wall_timer == 0) {
		m->spd_x = 0;
		m->act2++;
		return;
	}
	PlayerSyncShootAnime(m, 14);
}

/* 0x80036d4c -- step 10 sub-step 1: the rest of the kick, as a jump */
void WallKickRise(PLAYER *m)
{
	if (m->lock)
		m->spd_y = 0;
	if (m->wall & COL_U)
		m->spd_y = 0;
	if (!HELD(m, KEY_JUMP))
		m->spd_y = 0;
	if (m->var == 0) {
		if (AirWeaponPose(m))
			return;
	} else if (Zero_801ebf94(m)) {
		return;
	}
	if (TryLadderAir(m) || TryHang(m) || TryWall(m) || TryAirDash(m))
		return;
	AnimeTick(m);
	AirControl(m);
	SplashCheck(m);
	if (m->spd_y <= 0) {
		PlayerSetAnimeShoot(m, 11);
		m->jump_latch = -1;
		m->act = ST_FALL;
		m->act2 = 0;
		return;
	}
	PlayerSyncShootAnime(m, 14);
}

/* 0x80036e70 -- step 11: wall slide */
void WallSlide(PLAYER *m)
{
	extern void WallSlideDown(PLAYER *), WallPushOff(PLAYER *);

	if (m->act2 == 0)
		WallSlideDown(m);
	else
		WallPushOff(m);
}

/* 0x80036eb0 -- step 11 sub-step 0: slide down while the direction is held */
void WallSlideDown(PLAYER *m)
{
	if (m->wall & COL_D) {
		ToLand(m);
		return;
	}
	if (m->lock) {
		ToFall(m);
		return;
	}
	if (m->var == 0) {
		if (AirWeaponPose(m))
			return;
	} else if (Zero_801ea560(m) || Zero_801ec178(m)) {
		return;
	}
	if (!(m->wall & (WALL_R | WALL_L))) {
		ToFall(m);
		return;
	}
	if (TryWallKick(m))
		return;
	if (!WallHeld(m)) {
		WallRelease(m);
		return;
	}
	AnimeTick(m);
	PlayerMove(m);			/* spd_y = slide_spd, no gravity */
	SplashCheck(m);
	PlayerSyncShootAnime(m, 15);
}

/* 0x80036fbc -- step 11 sub-step 1: push off after the release */
void WallPushOff(PLAYER *m)
{
	if (m->wall & COL_D) {
		ToLand(m);
		return;
	}
	if (m->lock) {
		ToFall(m);
		return;
	}
	if (m->var == 0) {
		if (AirWeaponPose(m))
			return;
	} else if (Zero_801ea560(m) || Zero_801ec028(m)) {
		return;
	}
	AnimeTick(m);
	PlayerMoveAccel(m);
	SplashCheck(m);
	if (--m->wall_timer == 0) {
		m->spd_x = 0;
		m->act = ST_FALL;
		m->act2 = 0;
		return;
	}
	PlayerSyncShootAnime(m, 11);
}

/* 0x80037130 -- step 13: air dash */
void AirDash(PLAYER *m)
{
	if (m->wall & COL_D) {
		ToLand(m);
		return;
	}
	if (m->lock) {
		ToFall(m);
		return;
	}
	if (TryHang(m) || TryLadder(m))
		return;
	if (m->var == 0) {
		if (AirWeaponPose(m))
			return;
		if (m->shot_pose) {
			AirDashShot(m);
			return;
		}
	}
	AnimeTick(m);
	air_dash_steps[m->act2](m);
}

static void ZeroAirCancel(PLAYER *m)
{
	m->spd_x = 0;
	m->acc_x = 0;
	m->spd_y = 0;
	m->jump_latch = -1;
	m->dash_a = 0;
	m->grav = m->fall_grav;
}

/* 0x80037220 -- step 13 sub-step 0: the start pose (no move) */
void AirDashStart(PLAYER *m)
{
	if (m->var && Zero_801ec028(m)) {
		ZeroAirCancel(m);
		return;
	}
	if (m->anime.flag & FLG_EV40) {
		m->anime.flag &= FLG_FRAME;
		AirDashJet(m);
	}
	if (m->anime.flag & FLG_EV80) {
		m->anime.flag &= FLG_FRAME;
		PlaySound(1, 5, m);
		m->dash_f = 1;
		m->act2++;
	}
}

/* 0x800372dc -- step 13 sub-step 1: the dash */
void AirDashRun(PLAYER *m)
{
	if (m->var && Zero_801ec028(m)) {
		ZeroAirCancel(m);
		return;
	}
	if (DashStep(m)) {
		AirDashStop(m);
		return;
	}
	if (m->anime.flag & FLG_EV40) {
		m->anime.flag &= FLG_FRAME;
		AirDashJet(m);
	}
}

/* 0x8003737c -- step 13 sub-step 2: the end pose, falling */
void AirDashFall(PLAYER *m)
{
	if (m->wall & COL_D) {
		ToLand(m);
		return;
	}
	if (m->var && Zero_801ec028(m)) {
		ZeroAirCancel(m);
		return;
	}
	PlayerMoveAccel(m);
	SplashCheck(m);
	if (m->anime.next < 0 || HELD(m, KEY_R | KEY_L)) {
		PlayerSetAnime(m, 20);
		m->act = ST_FALL;
		m->act2 = 0;
	}
}

/* 0x800381f0 -- step 31: super jump */
void SuperJump(PLAYER *m)
{
	if (m->lock) {
		ToFall(m);
		return;
	}
	super_jump_steps[m->act2](m);
}

/* 0x80038248 -- step 31 sub-step 1: a short stop at the top */
void SJumpTop(PLAYER *m)
{
	if (frame_count & 1)
		AnimeTick(m);
	if (m->act4 && --m->act4)
		return;
	m->jump_latch = -1;
	m->spd_x = 0;
	m->acc_x = 0;
	m->spd_y = 0;
	m->grav = m->fall_grav;
	m->act = ST_FALL;
	m->act2 = 0;
}

/* 0x800382c8 -- step 31 sub-step 2: the ceiling grab animation */
void SJumpGrab(PLAYER *m)
{
	AnimeTick(m);
	if (m->anime.next == 0) {
		m->act2++;
		PlayerSetAnime(m, 84);
	}
}

/* 0x80038310 -- step 31 sub-step 4: air dash start from the ceiling */
void SJumpDashStart(PLAYER *m)
{
	AnimeTick(m);
	if (m->anime.flag & FLG_EV40) {
		m->anime.flag &= FLG_FRAME;
		AirDashJet(m);
	}
	if (m->anime.flag & FLG_EV80) {
		m->anime.flag &= FLG_FRAME;
		PlaySound(1, 5, m);
		m->dash_f = 1;
		m->act2++;
	}
}

/* 0x80038390 -- step 31 sub-step 5: air dash from the ceiling */
void SJumpDashRun(PLAYER *m)
{
	AnimeTick(m);
	if (DashStep(m)) {
		AirDashStop(m);
		m->act = ST_ADASH;		/* sub-step 2 of step 13 */
		return;
	}
	if (m->anime.flag & FLG_EV40) {
		m->anime.flag &= FLG_FRAME;
		AirDashJet(m);
	}
}

/* 0x80038788 -- rope dash, each update (1: still dashing) */
static int RopeDashStep(PLAYER *m)
{
	int keep, dir;

	keep = --m->dash_timer != 0;
	if (keep)
		keep = HELD(m, m->spd_y >= 0 ? KEY_U : KEY_D) != 0;
	if (!keep) {
		StopSound(1, 5);
		ClearDashFlags(m);
		/*
		 * The game has this code inline (no call). Here it is a call into RopeClimb, which calls
		 * back into RopeDashStep only while dash_a (+0x84) is set; ClearDashFlags just set it to
		 * 0, so this ends after one level.
		 */
		return RopeClimb(m);
	}
	dir = m->spd_y > 0 ? 1 : 2;
	PlayerMove(m);
	if (TileAtHands(m) != TILE_ROPE || (m->spd_y < 0 && (m->wall & COL_D))) {
		m->y = m->old_y;
		return 0;
	}
	m->y = m->old_y;
	PlayerMove(m);
	return dir;
}

/* 0x800389b4 -- rope climb (1 up, 2 down, 0 still) */
static int RopeClimb(PLAYER *m)
{
	int dir = 0;

	if (m->dash_a) {
		dir = RopeDashStep(m);
	} else if (HELD(m, KEY_U | KEY_D)) {
		if (HELD(m, KEY_U)) {
			dir = 1;
			m->spd_y = m->rope_spd;
		} else {
			dir = 2;
			m->spd_y = -m->rope_spd;
		}
		PlayerMove(m);
		if (TileAtHands(m) != TILE_ROPE || (m->spd_y < 0 && (m->wall & COL_D))) {
			m->y = m->old_y;
			dir = 0;
		} else {
			m->y = m->old_y;
			PlayerMove(m);
		}
	}
	/* animation: 160 (frame 2 for X, 3 for Zero) while still; 145 + dir, 160 + dir ... */
	return dir;
}

/* 0x80038ba8 -- rope dash start */
static int RopeTryDash(PLAYER *m)
{
	if (m->lock || m->dash_a)
		return 0;
	if (m->dash_key == 0 && !PRESSED(m, KEY_DASH))
		return 0;
	m->spd_y = HELD(m, KEY_U) ? m->rope_dash_spd : -m->rope_dash_spd;
	PlayerMove(m);
	if (TileAtHands(m) != TILE_ROPE || (m->spd_y < 0 && (m->wall & COL_D))) {
		m->y = m->old_y;
		return 0;
	}
	m->y = m->old_y;
	m->dash_a = 1;
	m->dash_b = 1;
	m->dash_timer = (m->parts & PART_DASH) ? 15 : 30;
	m->dash_f = 1;
	PlaySound(1, 5, m);
	return 1;
}

/* 0x80038cd0 -- bar move */
static int BarMove(PLAYER *m)
{
	int dir = 0, tile;

	if (m->dash_a) {
		dir = BarDashStep(m);
	} else if (HELD(m, KEY_R | KEY_L)) {
		if (HELD(m, KEY_L)) {
			dir = 1;
			m->spd_x = -m->bar_spd;
		} else {
			dir = 2;
			m->spd_x = m->bar_spd;
		}
		PlayerMove(m);
		tile = TileAtHands(m);
		m->x = m->old_x;
		if ((u_char)(tile - TILE_BAR_LO) < 11)
			PlayerMove(m);
		else
			dir = 0;
	}
	if (dir)
		BarSnapY(m, TileAtHands(m));
	/* animations 151/154 (weapon, shot pose), 163 + shape, 148 + shape */
	return dir;
}

/* 0x80038f64 -- bar dash start */
static int BarTryDash(PLAYER *m)
{
	int tile;

	if (m->lock || m->dash_a)
		return 0;
	if (m->dash_key == 0 && !PRESSED(m, KEY_DASH))
		return 0;
	m->spd_x = m->flip ? m->bar_dash_spd : -m->bar_dash_spd;
	PlayerMove(m);
	tile = TileAtHands(m);
	m->x = m->old_x;
	if ((u_char)(tile - TILE_BAR_LO) >= 11)
		return 0;
	m->dash_a = 1;
	m->dash_b = 1;
	m->dash_timer = (m->parts & PART_DASH) ? 15 : 30;
	m->dash_f = 1;
	PlaySound(1, 5, m);
	return 1;
}

/* 0x8003906c -- try the air dash (and the armour air actions) */
static int TryAirDash(PLAYER *m)
{
	if (m->lock || m->dash_b)
		return 0;
	if (m->var == 0) {
		if (m->form == FORM_FALCON)
			return FalconTry(m) != 0;
		if (m->form == FORM_BLADE) {
			if (BladeTry(m))
				return 1;
		} else if (UltimateTry(m)) {
			return 1;
		}
	} else if (Zero_801ec298(m)) {
		return 1;
	}
	if (m->var == 0 && (m->ladder_timer || m->form != FORM_ULTIMATE))
		return 0;
	if (m->lock)
		return 0;
	if (m->dash_key == 0 && !PRESSED(m, KEY_DASH))
		return 0;
	if (HELD(m, KEY_R))
		m->flip = OBJ_FLIP_R;
	if (HELD(m, KEY_L))
		m->flip = OBJ_FLIP_L;
	if (m->wall & FRONT(m))
		return 0;
	m->spd_x = m->flip ? m->dash_spd : -m->dash_spd;
	StartAirDash(m);
	return 1;
}

/* 0x80039984 -- try to hang on a rope or a bar (up held) */
static int TryHang(PLAYER *m)
{
	int tile, shape;

	if (m->lock)
		return 0;
	if (m->hang_wait && --m->hang_wait)
		return 0;
	if (!HELD(m, KEY_U))
		return 0;
	tile = TileHang(m);
	if (tile == TILE_ROPE) {
		if (m->shot_pose && m->var == 0) {
			PlayerSetAnime(m, 144);
		} else {
			PlaySound(0, 38, m);
			m->shot_pose = 0;
			PlayerSetAnime(m, 160);
		}
		m->x.w.lo = 0;
		m->y.w.lo = 0;
		m->act = ST_ROPE;
		m->act2 = 0;
		m->spd_x = 0;
		POS_X(m) = (POS_X(m) & 0xFFF0) + 8;
		ClearDashFlags(m);
		return 1;
	}
	if ((u_char)(tile - TILE_BAR_LO) >= 9 || m->spd_y > 0)
		return 0;
	if (tile >= 37 && tile < 41)
		shape = m->flip ? 1 : 2;
	else if (tile == 45)
		shape = 0;
	else
		shape = m->flip ? 2 : 1;
	if (m->shot_pose && m->var == 0) {
		PlayerSetAnime(m, 148 | shape);
	} else {
		PlaySound(0, 38, m);
		PlayerSetAnime(m, 163 + shape);
		m->shot_pose = 0;
	}
	m->act = ST_BAR;
	m->act2 = 0;
	m->spd_y = 0;
	BarSnapY(m, tile);
	ClearDashFlags(m);
	return 1;
}

/* 0x80039dfc -- every update: no floor under a standing player -> fall */
void CheckWalkOff(PLAYER *m)
{
	if (m->act == 0 || m->act == 1)
		return;
	if (m->jump_latch || m->pad97[0x0D] > 0 || m->dash_box < 0 || m->padd4)
		return;			/* +0xA4, +0xD5, +0xD4 */
	if (m->wall & COL_D)
		return;
	if (m->var)
		ClearShotPose(m);
	ToFall(m);
	PlayerSetTerrainBox(m);
}

/* 0x8003a0e8 -- Falcon air dash, each update (1: end) */
static int FalconDashStep(PLAYER *m)
{
	if (m->wall & FRONT(m))
		return 1;
	PlayerMoveAccel(m);
	if (m->flip) {
		if (m->spd_x < m->dash_min)
			m->spd_x = m->dash_min;
	} else if (m->spd_x > -m->dash_min) {
		m->spd_x = -m->dash_min;
	}
	if (--m->dash_timer == 0)
		return 1;
	if (m->flip)
		return HELD(m, KEY_L) != 0;
	return HELD(m, KEY_R) != 0;
}

/* 0x8003a2cc -- try a wall kick or a wall grab */
static int TryWall(PLAYER *m)
{
	if (m->lock || !(m->wall & (WALL_R | WALL_L)))
		return 0;
	if (TryWallKick(m))
		return 1;
	if (!WallHeld(m))
		return 0;
	StartWallGrab(m);
	return 1;
}

/* 0x8003a33c */
static int TryWallKick(PLAYER *m)
{
	if (m->lock || !PRESSED(m, KEY_JUMP) || m->wall_ok <= 0)
		return 0;
	StartWallKick(m);
	return 1;
}

/* 0x8003a394 -- the held direction points into a wall that allows a slide */
static int WallHeld(PLAYER *m)
{
	if (m->lock || m->wall_ok == 0)
		return 0;
	return (HELD(m, KEY_R | KEY_L) & m->wall) != 0;
}

/* 0x8003a488 -- up held on a ladder tile in the air */
static int TryLadderAir(PLAYER *m)
{
	if (m->lock || !HELD(m, KEY_U) || TileAtBody(m) != TILE_LADDER)
		return 0;
	if (m->shot_pose) {
		XShootLadderAir(m);
		return 1;
	}
	PlayerSetAnime(m, 31);
	LadderFromAir(m);
	return 1;
}

/* 0x8003a764 -- leave a rope or a bar */
static int HangExit(PLAYER *m)
{
	if (m->padca[6] <= 0) {		/* +0xD0 event request */
		if ((u_char)(TileAtHands(m) - TILE_BAR_LO) >= 11) {
			m->hang_wait = 4;
			ToFall(m);
			return 1;
		}
		if (!PRESSED(m, KEY_JUMP))
			return 0;
		if (HELD(m, KEY_D))
			ToFall(m);
		else
			StartJump(m);
		m->hang_wait = 4;
		return 1;
	}
	ToFall(m);
	return 1;
}

/* 0x8003af1c */
static void StartJump(PLAYER *m)
{
	m->jump_latch = 1;
	m->spd_x = 0;
	m->acc_x = 0;
	m->spd_y = m->jump_spd;
	m->grav = m->jump_grav;
	if (m->dash_a || HELD(m, KEY_DASH)) {		/* dash jump */
		m->dash_a = 1;
		m->dash_b = 2;
		m->dash_f = 1;
	}
	m->jump_work = 10;
	PlayerSetAnimeShoot(m, 10);
	if (GetRNG() & 4)
		PlayVoice(m, voice_tbl[m->var * 4 + (GetRNG() & 3)]);
	AirControl(m);
	m->act = ST_JUMP;
	m->act2 = 0;
}

/* 0x8003aff4 */
static void ToFall(PLAYER *m)
{
	PlayerSetAnimeShoot(m, 11);
	m->spd_x = 0;
	m->acc_x = 0;
	m->spd_y = 0;
	m->jump_latch = -1;
	m->grav = m->fall_grav;
	if (m->dash_a) {
		m->dash_a = 0;
		m->dash_f = -1;
	}
	m->act = ST_FALL;
	m->act2 = 0;
}

/* 0x8003b114 -- stand when on the floor, else fall */
static void LandOrFall(PLAYER *m)
{
	if (m->wall & COL_D) {
		m->jump_latch = 0;
		m->dash_b = 0;
		ClearDashFlags(m);
		if (PlayerRequests(m))
			return;
		m->act = ST_STAND;
		m->act2 = 0;
		if (m->var == 0 && m->shot_pose) {
			PlayerSetAnimeAt(m, 94, m->shot_timer < 5 ? 2 : (m->shot_timer < 9));
			if (m->shot_timer >= 9)
				m->anime.duration = m->shot_timer - 8;
		} else {
			StandAnime(m);
		}
		return;
	}
	ToFall(m);
}

/* 0x8003b298 */
static void StartAirDash(PLAYER *m)
{
	PlayerSetAnime(m, 18);
	m->spd_y = 0;
	m->grav = 0;
	m->dash_a = -1;
	m->acc_x = m->dash_acc;
	m->dash_timer = (m->parts & PART_DASH) ? 10 : 18;
	m->dash_b = 3;
	ClearShotPose(m);
	m->act = ST_ADASH;
	m->act2 = 0;
}

/* 0x8003b30c */
static void AirDashStop(PLAYER *m)
{
	PlayerSetAnime(m, 19);
	m->jump_latch = -1;
	m->dash_f = -1;
	m->spd_x = 0;
	m->acc_x = 0;
	m->spd_y = 0;
	m->dash_a = 0;
	m->act2 = 2;
	m->grav = m->fall_grav;
}

/* 0x8003b360 -- X shoots during the air dash */
static void AirDashShot(PLAYER *m)
{
	PlayerSetAnime(m, 132);
	m->jump_latch = -1;
	m->spd_x = 0;
	m->acc_x = 0;
	m->spd_y = 0;
	m->dash_a = 0;
	m->dash_f = 0;
	m->act = ST_FALL;
	m->act2 = 0;
	m->grav = m->fall_grav;
}

/* 0x8003b3b8 */
static void StartWallGrab(PLAYER *m)
{
	PlayerSetAnimeShoot(m, 13);
	PlaySound(1, 4, m);
	m->dash_f = -1;
	m->spd_x = 0;
	m->acc_x = 0;
	m->spd_y = 0;
	m->grav = 0;
	m->dash_a = 0;
	m->dash_b = 0;
	m->wall_timer = 8;
	m->flip = (m->wall & WALL_R) ? OBJ_FLIP_R : OBJ_FLIP_L;	/* face the wall */
	m->act = ST_WGRAB;
	m->act2 = 0;
}

/* 0x8003b440 */
static void StartWallKick(PLAYER *m)
{
	long spd;

	PlayerSetAnimeShoot(m, 14);
	PlaySound(1, 3, m);
	PlayVoice(m, 2);
	m->jump_latch = 1;
	if (HELD(m, KEY_DASH)) {
		m->dash_a = 1;
		m->dash_b = 1;
		m->dash_f = 1;
	} else {
		ClearDashFlags(m);
	}
	spd = m->dash_a ? m->dash_min : m->walk_spd;
	if (m->wall & WALL_R) {
		m->flip = OBJ_FLIP_R;
		m->spd_x = -spd;
	} else {
		m->flip = OBJ_FLIP_L;
		m->spd_x = spd;
	}
	m->acc_x = 0;
	m->wall_timer = 10;
	m->spd_y = m->kick_spd;
	m->grav = m->jump_grav;
	WallKickSpark(m);
	m->act = ST_WKICK;
	m->act2 = 0;
}

/* 0x8003b548 */
static void StartWallSlide(PLAYER *m)
{
	PlayerSetAnimeShoot(m, 15);
	if (m->form != FORM_SHADOW)
		WallSlideDust(m);
	m->jump_latch = -1;
	m->spd_x = 0;
	m->acc_x = 0;
	m->grav = 0;
	m->spd_y = m->slide_spd;
	ClearDashFlags(m);
	m->act = ST_WSLIDE;
	m->act2 = 0;
}

/* 0x8003b5b8 */
static void WallRelease(PLAYER *m)
{
	PlayerSetAnimeShoot(m, 11);
	m->spd_x = (m->wall & WALL_R) ? -m->walk_spd : m->walk_spd;
	m->jump_latch = -1;
	m->wall_timer = 4;
	m->act = ST_WSLIDE;
	m->acc_x = 0;
	m->spd_y = 0;
	m->act2 = 1;
	m->grav = m->fall_grav;
}

/* 0x8003cfe0 */
static void PlayVoice(PLAYER *m, int id)
{
	if (m->voice_timer <= 0)
		PlaySound(3, id & 0xFF, m);
}

/* 0x8003d18c -- air control and the move with gravity */
static void AirControl(PLAYER *m)
{
	long spd;

	if (m->lock) {
		m->spd_x = 0;
		PlayerMoveAccel(m);
		return;
	}
	spd = m->dash_a ? m->dash_min : m->walk_spd;
	if (HELD(m, KEY_R)) {
		m->flip = OBJ_FLIP_R;
		m->spd_x = (m->wall & WALL_R) ? 0 : spd;
	} else if (HELD(m, KEY_L)) {
		m->flip = OBJ_FLIP_L;
		m->spd_x = (m->wall & WALL_L) ? 0 : -spd;
	} else {
		m->spd_x = 0;
	}
	PlayerMoveAccel(m);
}

/* 0x8003d394 -- splash when the feet reach a water surface tile */
static void SplashCheck(PLAYER *m)
{
	if (TileAtFeet(m) == TILE_WATER)
		WaterSplash(m, (POS_Y(m) + m->col_box->oy + m->col_box->h) & ~15);
}

/* 0x8003e174 -- X: a special weapon in the air */
static int AirWeaponPose(PLAYER *m)
{
	extern signed char weapon_pose_tbl[];	/* 0x80073E00 */

	if (m->lock || m->var)
		return 0;
	if (WeaponTryA(m))
		return 1;
	WeaponTryB(m);
	if (m->weapon_pose == 0 || weapon_pose_tbl[m->weapon] == 0)
		return 0;
	if (m->weapon != 50)
		m->dash_a = 0;
	m->dash_f = 0;
	if (m->weapon == 5 || m->weapon == 14)
		PlayerSetAnime(m, m->act == ST_WSLIDE ? 127 : 122);
	WeaponFire(m, 1);
	return 1;
}

/* 0x8003e06c -- X: a special weapon on a rope or a bar */
static int HangWeapon(PLAYER *m)
{
	extern signed char weapon_pose_tbl[];

	if (m->lock || m->var)
		return 0;
	if (WeaponTryA(m))
		return 1;
	WeaponTryB(m);
	if (m->weapon_pose == 0 || weapon_pose_tbl[m->weapon] == 0)
		return 0;
	m->dash_a = 0;
	m->dash_b = 0;
	m->dash_f = 0;
	if (m->weapon == 5 || m->weapon == 14) {
		PlayerSetAnime(m, m->act == ST_ROPE ? 145 : 151);
		WeaponFire(m, 1);
	} else if (m->weapon == 50) {
		ToFall(m);
		WeaponFire(m, 1);
	} else {
		WeaponFire(m, 0);
	}
	return 1;
}

/* 0x8003f200 -- X form 4: jump in the air */
static int UltimateTry(PLAYER *m)
{
	if (m->form != FORM_ULTIMATE || !PRESSED(m, KEY_JUMP))
		return 0;
	m->spd_x = 0;
	m->acc_x = 0;
	m->spd_y = 0;
	m->grav = 0;
	m->dash_a = 0;
	m->dash_b = m->form;
	m->dash_f = 0;
	m->padd6[3] = 180;		/* +0xD9 */
	m->padd6[4] = 0;		/* +0xDA */
	PlayerSetAnimeShoot(m, 21);
	m->act = ST_UHOVER;
	m->act2 = 0;
	return 1;
}

/* 0x8003f28c -- X form 1: jump or dash in the air */
static int FalconTry(PLAYER *m)
{
	if (!PRESSED(m, KEY_JUMP) && !TryDashInput(m))
		return 0;
	StartAirDash(m);
	m->spd_y = 0;
	PlaySound(1, 5, m);
	m->act = ST_FALCON;
	PlayerSetAnime(m, 169);
	MakeObject(1, 23, 0, m);
	MakeEffect(1, 18, 0, 0);
	return 1;
}

/* 0x8003f32c -- X form 3: jump or dash in the air */
static int BladeTry(PLAYER *m)
{
	if (!PRESSED(m, KEY_JUMP) && !TryDashInput(m))
		return 0;
	PlayerSetAnime(m, 189);
	m->dash_a = 1;
	m->spd_x = 0;
	m->acc_x = 0;
	m->spd_y = 0;
	m->grav = 0;
	m->dash_timer = 15;
	m->dash_b = 7;
	m->padd6[3] = 0;		/* +0xD9 */
	ClearShotPose(m);
	m->act = ST_MACH;
	m->act2 = 0;
	MakeObject(33, 32, 0, m);
	return 1;
}
