/*
 *	zero.c  --  Zero's sabre, techniques and weapon objects (overlay 1)
 *	A plausible reconstruction from the disassembly of turtloid-6_zero-player. Not original
 *	code, not compiled. Each function carries its address.
 */
#include "zero.h"
#include "game.h"

#define PRESSED(m, k)	((m)->trg & (k))
#define HELD(m, k)	((m)->key & (k))
#define AT_HOLD(o)	((o)->anime.next == 0)
#define FACE_HELD(m)	do { if (HELD(m, KEY_L)) (m)->flip = 0; if (HELD(m, KEY_R)) (m)->flip = 0x40; } while (0)
#define FACE_PRESSED(m)	do { if (PRESSED(m, KEY_L)) (m)->flip = 0; if (PRESSED(m, KEY_R)) (m)->flip = 0x40; } while (0)

/* 0x801ECD3C */
static void SabreStart(PLAYER *m)
{
	Z_DD(m) = 1;
	Z_BUSY(m) = 1;
	Z_COMBO(m) = 0;
	m->dash_f = 0;
}

/* 0x801ECCD4 */
static int TrySabre(PLAYER *m)
{
	if (m->var == 0 || Z_LOCK(m) || !PRESSED(m, KEY_WEAPON) || Z_BUSY(m))
		return 0;
	SabreStart(m);
	return 1;
}

/* 0x801ECD90 */
static void SpawnSabre(PLAYER *m, int type)
{
	OBJECT	*o = GetWeaponObject();

	if (o == 0)
		return;
	o->flag = 1;
	o->type = type;
	o->var = Z_COMBO(m);
	o->layer = m->layer;
	*(long *)((char *)o + 0x84) = (u_char)m->anime_no;
	o->flip = m->flip;
}

/* 0x801ECD54 */
static void SpawnSabreOnFlag(PLAYER *m, int type)
{
	if (m->anime.flag & ZF_SPAWN) {
		m->anime.flag &= 0x7F;
		SpawnSabre(m, type);
	}
}

/* 0x801ECE60 */
static void GroundEnd(PLAYER *m)
{
	if (!AT_HOLD(m))
		return;
	Z_BUSY(m) = 0;
	if (game.exit_req)
		PlayerExit(m);
	else if (((char *)m)[0xD0])
		PlayerStartEvent(m);
	else
		ToStand(m);
}

/* 0x801ECE08 */
static int GroundMoves(PLAYER *m)
{
	if (TryLadder(m) || TryCrouch(m))
		return 1;
	if (TryDirection(m)) {
		ToWalkStart(m);
		return 1;
	}
	return 0;
}

static void StartGroundSabre(PLAYER *m)
{
	PlayerSetAnime(m, 88);
	PlaySound(1, 7, (OBJECT *)m);
	if (GetRNG() & 4)
		PlayVoice(m, 3);
	m->dash_a = 0;
	m->dash_b = 0;
	m->act2 = ST_ZSABRE;
	m->act3 = 0;
	/*
	 * Re-entry, as in the game (jal 0x801EAE7C at 0x801EB0E8): run the first update of the new
	 * swing now. It ends after one level only because of the data: frame 0 of animation 88 has
	 * flag 0x01 (ZF_HOLD), so the tail sets Z_BUSY and never reaches TrySabre again. Animation
	 * data without ZF_HOLD on that frame and with ZF_CANCEL would recurse without end.
	 */
	ZeroStep48(m);
}

/* 0x801EB12C */
static int ComboTry(PLAYER *m)
{
	u_char	f = m->anime.flag;
	int	behind;

	if (Z_LOCK(m) || !(f & (ZF_NEXT | ZF_TURN)))
		return 0;
	if (PRESSED(m, KEY_WEAPON) && (f & ZF_NEXT)) {
		Z_COMBO(m)++;
		PlayerSetAnime(m, 88 + Z_COMBO(m));
		PlaySound(1, combo_snd[Z_COMBO(m)][0], (OBJECT *)m);
		if (Z_COMBO(m) >= 2 || (GetRNG() & 4))
			PlayVoice(m, combo_snd[Z_COMBO(m)][1]);
	} else if (f & ZF_TURN) {
		if (m->act3 != 1)
			return 0;
		behind = (m->flip & 0x40) ? HELD(m, KEY_L) : HELD(m, KEY_R);
		if (!behind)
			return 0;
		m->act3 = 2;
		m->flip ^= 0x40;
		if (++Z_COMBO(m) >= 2)
			Z_COMBO(m) = 1;
		PlayerSetAnime(m, 88 + Z_COMBO(m));
		PlaySound(1, combo_snd[Z_COMBO(m)][0], (OBJECT *)m);
		if (GetRNG() & 4)
			PlayVoice(m, combo_snd[Z_COMBO(m)][1]);
	} else
		return 0;
	SpawnSabre(m, ZW_SLASH1 + Z_COMBO(m));
	return 1;
}

/* 0x801EAE7C: step 48, the ground combo */
void ZeroStep48(PLAYER *m)
{
	u_char	f;

	if (m->act3 == 0) {
		m->act3 = 1;
		goto tail;
	}
	AnimeTick((OBJECT *)m);
	SpawnSabreOnFlag(m, m->anime_no == 92 ? ZW_LAND : ZW_SLASH1);
	f = m->anime.flag;
	if (!(f & ZF_HOLD) && Z_COMBO(m) < 2 && TryShoenzan(m))
		return;
	if (ComboTry(m))
		return;
tail:
	f = m->anime.flag;
	if (!Z_LOCK(m)) {
		if (f & ZF_HOLD) {
			Z_BUSY(m) = 1;
		} else {
			Z_BUSY(m) = 0;
			if (TryDashJump(m))
				return;
			if (Z_COMBO(m) >= 2) {
				Z_BUSY(m) = 1;
			} else {
				if (TryCrouch(m))
					return;
				if (f & ZF_CANCEL) {
					Z_DD(m) = 1;
					if (TryRekkoha(m))
						return;
					Z_DD(m) = 0;
					if (TrySabre(m)) {
						StartGroundSabre(m);
						return;
					}
					if (GroundMoves(m))
						return;
				}
				Z_BUSY(m) = 1;
			}
		}
	}
	GroundEnd(m);
}

/* 0x801EB5F8 (part): the landing of steps 50 and 54 */
static int FallSabreLand(PLAYER *m)
{
	u_char	f;
	char	t;

	if (!(Z_TOUCH(m) & 8))
		return 0;
	m->jump_latch = 0;
	m->dash_a = m->dash_b = m->dash_f = 0;
	f = m->anime.flag;
	if (Z_BUSY(m) && m->act2 == ST_ZFALLSABRE && !(f & ZF_CANCEL)) {
		t = m->anime.duration;
		PlayerSetAnimeAt(m, 92, f & 0x1F);
		m->anime.duration = t;
		if (m->anime.flag & 0x1F) {
			m->anime.flag &= 0x7F;
			SpawnSabre(m, ZW_LAND);
		}
		m->act2 = ST_ZSABRE;
		m->act3 = 1;
		return 1;
	}
	Z_BUSY(m) = 0;
	ToLand(m);
	return 1;
}

/* 0x801EC86C */
int TryShoenzan(PLAYER *m)
{
	if (Z_LOCK(m) || !(Z_SKILL(m) & SK_SHOENZAN) || !HELD(m, KEY_U) || !PRESSED(m, KEY_WEAPON))
		return 0;
	Z_BUSY(m) = 1;
	PlayerSetAnime(m, 90);
	SpawnSabre(m, ZW_SLASH3);	/* drawn in palette 8, no box in step 55 */
	SpawnSabre(m, ZW_FIRE);
	PlaySound(1, 7, (OBJECT *)m);
	PlaySound(1, 22, (OBJECT *)m);
	PlayVoice(m, 5);
	ClearDashFlags(m);
	m->act2 = ST_SHOENZAN;
	m->act3 = 0;
	SabreStart(m);
	ZeroStep55(m);
	return 1;
}

/* 0x801EA454 */
int TrySentsuizan(PLAYER *m)
{
	if (Z_LOCK(m) || !(Z_SKILL(m) & SK_SENTSUIZAN) || !HELD(m, KEY_U) || !PRESSED(m, KEY_WEAPON) || Z_BUSY(m))
		return 0;
	Z_DD(m) = Z_BUSY(m) = 1;
	Z_COMBO(m) = 0;
	m->dash_f = 0;
	PlayerSetAnime(m, 101);
	PlaySound(1, 25, (OBJECT *)m);
	PlayVoice(m, 9);
	((char *)m)[0x61] = 0;
	PlayerPaletteRestore(m);
	m->giga = 0;
	m->act2 = ST_SENTSUIZAN;
	m->nohit_f = 1;
	m->act3 = 0;
	m->spd_x = m->flip ? 0x30000 : -0x30000;
	m->spd_y = -0x50000;
	ZeroStep57(m);
	return 1;
}

/* 0x801ECF90: step 57 sub-step 1 */
static void SentsuizanDive(PLAYER *m)
{
	AnimeTick((OBJECT *)m);
	SpawnSabreOnFlag(m, ZW_SENTSUIZAN);
	if (!(Z_TOUCH(m) & (m->flip ? 9 : 10))) {
		PlayerMove(m);
		return;
	}
	PlaySound(1, 26, (OBJECT *)m);
	if (!Z_LOCK(m))
		m->nohit_f = 0;
	if (Z_TOUCH(m) & 8) {
		if (STAGE_AREA() == 0x104)
			Quake(30, 2, 1);
		else
			Quake2(30, 2, 1);
		PlayerSetAnime(m, 102);
		Z_COMBO(m) = 1;
		SpawnSabreOnFlag(m, ZW_SENTSUIZAN);	/* var 1: the landing burst, anim 124 */
		m->jump_latch = 0;
		m->act3++;
	} else {
		m->jump_latch = 0;
		Z_BUSY(m) = 0;
		StandOrFall(m);
	}
}

/* 0x801EA560 */
int TryEnsuizan(PLAYER *m)
{
	if (Z_LOCK(m) || m->dash_b == 8 || !(Z_SKILL(m) & SK_ENSUIZAN) || !HELD(m, KEY_D) || !PRESSED(m, KEY_BUSTER))
		return 0;
	if (Z_BUSY(m))
		return 0;
	Z_DD(m) = Z_BUSY(m) = 1;
	Z_COMBO(m) = 0;
	m->dash_f = 0;
	ClearDashFlags(m);
	PlayerSetAnime(m, 103);
	SpawnSabre(m, ZW_ENSUIZAN);
	PlayVoice(m, 5);
	PlaySound(1, 24, (OBJECT *)m);
	((char *)m)[0x61] = 0;
	PlayerPaletteRestore(m);
	m->giga = 0;
	m->nohit_f = 1;
	m->act2 = ST_ENSUIZAN;
	m->act3 = 0;
	m->dash_b = 8;
	ZeroStep58(m);
	return 1;
}

/* 0x801EC504: step 58 */
void ZeroStep58(PLAYER *m)
{
	if (Z_LOCK(m)) {
		Z_BUSY(m) = 0;
		StandOrFall(m);
		return;
	}
	if (m->act3 == 0)
		m->act3 = 1;
	else {
		AnimeTick((OBJECT *)m);
		if (!(((u_char *)m)[0x71] & 8) && (m->anime.flag & ZF_CANCEL)) {
			m->nohit_f = 0;
			if (HELD(m, KEY_D) && PRESSED(m, KEY_BUSTER) && Z_COMBO(m) < 2) {
				Z_COMBO(m)++;
				PlayerSetAnime(m, 103);
				SpawnSabre(m, ZW_ENSUIZAN);
			}
		}
	}
	if (AT_HOLD(m)) {
		Z_BUSY(m) = 0;
		m->nohit_f = 0;
		StandOrFall(m);
	}
}

/* 0x801EA680 */
int TryRekkoha(PLAYER *m)
{
	if (Z_LOCK(m) || !(Z_SKILL(m) & SK_REKKOHA) || !PRESSED(m, KEY_GIGA))
		return 0;
	if (Z_ENERGY(m, ZS_REKKOHA) < WeaponCost(m, ZS_REKKOHA) || Z_BUSY(m))
		return 0;
	Z_SLOT(m) = ZS_REKKOHA;
	Z_DD(m) = Z_BUSY(m) = 1;
	Z_COMBO(m) = 0;
	m->dash_f = 0;
	ClearDashFlags(m);
	PlayerSetAnime(m, 104);
	PlayVoice(m, 7);
	((char *)m)[0x61] = 0;
	PlayerPaletteRestore(m);
	m->giga = 0;
	Z_E0(m) = 2;
	Z_E1(m) = 0;
	m->col_box = 0;
	m->jump_latch = 1;
	m->nohit_f = 1;
	game.player_lock = 1;
	SpendEnergy(m, Z_SLOT(m));
	m->act2 = ST_REKKOHA;
	m->act3 = 0;
	m->act4 = 0;
	ZeroStep59(m);
	return 1;
}

/* 0x801ED198: step 59 sub-step 4 */
static void RekkohaBeams(PLAYER *m)
{
	if (m->act4 >= 11) {
		Z_E1(m) = 0;
		m->act3++;
		return;
	}
	if (++Z_E1(m) % 10 == 1)
		RekkohaWave(m, Z_E1(m) / 10);	/* 0x801EAA24: beams at x (k = 0) or x - 32k and x + 32k */
}

/* 0x801EBEDC */
int TryZBuster(PLAYER *m)
{
	if (Z_LOCK(m) || !PRESSED(m, KEY_BUSTER) || Z_BUSY(m))
		return 0;
	Z_BUSY(m) = 1;
	Z_COMBO(m) = 0;
	ClearDashFlags(m);
	PlayerSetAnime(m, 110);
	if (GetRNG() & 4)
		PlayVoice(m, 9);
	m->spd_x = m->acc_x = m->spd_y = m->grav = 0;
	m->act2 = ST_ZBUSTER;
	m->act3 = 0;
	ZeroStep62(m);
	return 1;
}

/* 0x801EC298: the double jump, from TryAirDash while m+0x86 == 0 */
int TryDoubleJump(PLAYER *m)
{
	if (!PRESSED(m, KEY_JUMP))
		return 0;
	PlayerSetAnime(m, 95);
	PlaySound(1, 1, (OBJECT *)m);
	PlayVoice(m, 7);
	m->spd_x = m->acc_x = 0;
	m->dash_a = 0;
	m->dash_b = 6;
	m->dash_f = 0;
	m->spd_y = m->jump_spd;
	m->grav = m->jump_grav;
	AirControl(m);
	m->act2 = ST_JUMP;
	m->act3 = 0;
	return 1;
}

/* 0x801ED2A8: weapon types 24-31, 35-42, 48 */
void SabreObject(OBJECT *o)
{
	PLAYER	*m = (PLAYER *)&mega;
	int	gone;

	gone = m->hp < 1 || Z_LOCK(m) || Z_DEAD(m)
	    || *(long *)((char *)o + 0x84) != (u_char)m->anime_no
	    || (o->flip != m->flip && (u_char)(o->type - ZW_SLASH1) < 3)
	    || (o->type == ZW_ENSUIZAN && o->var != Z_COMBO(m));
	if (gone) {
		DeleteObject3(o);
		return;
	}
	if (o->act == 0)
		SabreInit(o, m);			/* 0x801ED3DC */
	else {
		AnimeTick(o);
		if (!(o->type == ZW_SENTSUIZAN && o->var == 0) && o->anime.next == 0) {
			DeleteObject3(o);
			return;
		}
	}
	SabreFollow(o, m);			/* 0x801ED6F0 */
}

/* 0x801ED6F0 */
void SabreFollow(OBJECT *o, PLAYER *m)
{
	char	f = o->anime.flag;
	HITBOX	*tbl;

	o->x = m->x;
	o->y = m->y;
	o->flip = m->flip;
	if (f == 0)
		o->atk_box = 0;
	else {
		tbl = sabre_box;
		if ((m->parts & PART_SABRE_LONG) && !(o->flag & 0x80) && sabre_tbl[o->type - 24].ext)
			tbl = sabre_box_long;
		o->atk_box = &tbl[f];
	}
	if (o->type == ZW_SENTSUIZAN)
		o->col_box = ((u_char)f - 75 < 5) ? &sabre_tbox[(u_char)f] : 0;
	switch (o->type) {
	case ZW_SLASH1: case ZW_SLASH2: case 37: case 38: case 39: case 40: case 41:
		o->hit_tag = o->var + 1;		/* one hit per swing */
		break;
	case ZW_SLASH3:
		o->hit_tag = o->anime.flag;		/* a new tag at each box change */
		break;
	default:
		if ((frame_count & 3) == 0)
			o->hit_tag = o->anime.flag;
	}
	if (o->type == ZW_RAKUKOJIN || o->type == ZW_SENTSUIZAN) {
		if (o->work90) {
			o->work90--;
			o->atk_box = 0;
		}
		if (o->shot_hit) {
			o->shot_hit = 0;
			o->work90 = 2;
			o->atk_box = 0;
			o->work8c++;
		}
		if (o->work8c >= 4)
			o->atk_box = 0;
	}
	if (o->type == ZW_SLASH3 && m->act2 == ST_SHOENZAN)
		o->atk_box = 0;
	DisplayCheck(o, 128, 128);
}
