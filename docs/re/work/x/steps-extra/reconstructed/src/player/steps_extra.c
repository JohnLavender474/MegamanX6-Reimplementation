/*
 *	steps_extra.c  --  player steps 32-77 (entries and the armour steps) and the form code
 *
 *	A plausible reconstruction from the disassembly; not original code, not compiled.
 *	Fields that player.h does not name are read with U8 / S16 / S32 at their offsets.
 *	The Zero steps (48-63) are only described in ../../notes.md.
 */
#include "player.h"
#include "game.h"
#include "sound.h"

#define U8(p, o)	(((u_char *)(p))[o])
#define S8(p, o)	(((char *)(p))[o])
#define S16(p, o)	(*(short *)((u_char *)(p) + (o)))
#define S32(p, o)	(*(long *)((u_char *)(p) + (o)))

#define FORM_NONE	0
#define FORM_ZERO	5

#define KEY_SHOT	0x0010
#define KEY_SPECIAL	0x0040

#define ST_LSHOT	32
#define ST_NOVA		34
#define ST_FGIGA	36
#define ST_SGIGA	37
#define ST_BGIGA	77
#define ST_WAIT_EXIT	73

#define FX(o)		S32(o, 0x7C)

extern u_char	game_work[];			/* game, 0x800CCED0 */
extern void	LoadMember(int no, int a, int b);	/* 0x80014F70 */
extern void	PlayerSetAnime(PLAYER *m, int n);
extern void	PlayerSetAnimeShoot(PLAYER *m, int n);
extern void	PlayerSyncShootAnime(PLAYER *m, int n);
extern void	PlayerMove(void *o);
extern void	PlayerMoveAccel(void *o);
extern void	AnimeTick(void *o);
extern void	PlaySound(int bank, int id, void *o);
extern void	ToFall(PLAYER *m);
extern void	StandOrFall(PLAYER *m);
extern void	StartAirDash(PLAYER *m);
extern void	StartCrouch(PLAYER *m);
extern void	ClearDashFlags(PLAYER *m);
extern void	ClearActionFlags(PLAYER *m);		/* 0x8003CDF0 */
extern void	PaletteRestore(PLAYER *m);		/* 0x8003C4EC */
extern void	PaletteSet(PLAYER *m, int n);		/* 0x8003CF54 */
extern void	PaletteSlot(int slot, int no, int n);	/* 0x8003CF24 */
extern int	TryWall(PLAYER *m);
extern int	TryDashInput(PLAYER *m);
extern OBJECT	*SpawnEffect(int f, int t, int v, void *a);	/* 0x8003D510 */
extern OBJECT	*SpawnWeapon(int f, int t, int v, void *a);	/* 0x8003D3F8 */
extern void	ShotSetupA(PLAYER *m);			/* 0x8003F480 */
extern void	ShotSetupB(PLAYER *m);			/* 0x8003DF58 */
extern void	ShotSetupC(PLAYER *m);			/* 0x8003F0BC */
extern int	AirWeaponPose(PLAYER *m);		/* 0x8003E174 */
extern void	ClearShotPose(PLAYER *m);		/* 0x8003CDBC */

/* 0x80013BF0 (part): the player member for the character and the form */
void LoadPlayerMember(void)
{
	int	no;

	if (S8(game_work, 0x38) != 0) {
		LoadMember(85, 0, 0);
		return;
	}
	switch (U8(game_work, 0x5E)) {
	case 1:		no = 87; break;
	case 2:		no = 88; break;
	case 3:		no = 89; break;
	default:	no = 86; break;		/* form 0 and form 4 */
	}
	LoadMember(no, 0, 0);
}

/* 0x8003D718 */
void PlayerLoadForm(PLAYER *m)
{
	m->form = U8(game_work, 0x5E);
	m->parts = *(u_long *)(game_work + 0x74 + 4 * m->var);
	if (m->form == FORM_SHADOW) {
		m->skill = 0;
		SpawnEffect(1, 27, 5, 0);
		PaletteSlot(2, 25, 1);
		PaletteSlot(2, 24, 1);
	} else {
		m->skill = U8(game_work, 0x60);
	}
}

/* overlay 16, 0x800E9898: the armour select list (0x800F1CB0, count 0x800F1CEB) */
int ArmourSelectList(u_char *list)
{
	u_char	armors = U8(game_work, 0x5F);
	int	n = 1;

	list[0] = FORM_NONE;
	if (armors & 0x01) list[n++] = FORM_FALCON;
	if (armors & 0x10) list[n++] = FORM_ZERO;
	if (armors & 0x02) list[n++] = FORM_SHADOW;
	if (armors & 0x04) list[n++] = FORM_BLADE;
	if (armors & 0x08) list[n++] = FORM_ULTIMATE;
	return n;
}

extern const short	x_cost[];		/* 0x80073D74, by weapon slot */
extern const short	zero_cost[];		/* 0x80073DF4, by weapon slot */

/* 0x8003EB0C: weapon energy cost of slot n; X forms 1, 3 and 4 pay two thirds */
int WeaponCost(PLAYER *m, char n)
{
	short	cost;

	if (m->var != 0) {
		if (m->parts & 0x20)
			return (short)(zero_cost[n] * 3);
		if (U8(game_work, 0x5F) & 0x20)
			return (short)(zero_cost[n] * 4);
		return (short)(zero_cost[n] * 6);
	}
	cost = x_cost[n] * 6;
	if (cost == 0)
		return 0;
	if (m->parts & 0x20)
		cost = cost / 2;
	else if (m->form == FORM_FALCON || m->form == FORM_BLADE || m->form == FORM_ULTIMATE)
		cost = (short)(2 * cost / 3);
	return cost ? cost : 1;
}

/* 0x8003E344: the armour special attack, X only */
int SpecialAttackTry(PLAYER *m)
{
	int	side;

	if (m->var != 0 || U8(m, 0x92) != 0)
		return 0;
	if (S16(m, 0xA8) != S16(m, 0x14A) || !(m->trg & KEY_SPECIAL))
		return 0;
	if (m->form < FORM_FALCON || m->form > FORM_ULTIMATE)
		return 0;
	if (m->form == FORM_ULTIMATE) {
		if (m->dash_b != 0)
			return 0;
		side = m->flip ? WALL_R : WALL_L;
		if (m->wall & side)
			return 0;
	}
	ClearActionFlags(m);
	U8(m, 0x93) = 0;
	if (m->var == 0) {
		ShotSetupA(m);
		ShotSetupB(m);
		ShotSetupC(m);
	}
	m->jump_latch = 1;
	U8(m, 0x61) = 0;
	U8(m, 0xA4) = 0;
	if (m->form != FORM_ULTIMATE)
		S16(m, 0xA8) = 0;
	switch (m->form) {
	case FORM_FALCON:
		PlayerSetAnime(m, (m->wall & 0x08) ? 174 : 177);
		m->act2 = ST_FGIGA;
		SpawnEffect(1, 27, 8, 0);
		break;
	case FORM_SHADOW:
		PlayerSetAnime(m, (m->wall & 0x08) ? 185 : 186);
		m->act4 = (m->wall & 0x08) ? 0 : 1;
		m->act2 = ST_SGIGA;
		m->dash_b = 5;
		U8(m, 0xE1) = 0;
		m->col_box = 0;
		U8(m, 0xE0) = 2;
		SpawnEffect(1, 27, 7, 0);
		break;
	case FORM_BLADE:
		PlayerSetAnime(m, (m->wall & 0x08) ? 185 : 186);
		m->act4 = (m->wall & 0x08) ? 0 : 1;
		m->act2 = ST_BGIGA;
		m->dash_b = 5;
		U8(m, 0xE1) = 0;
		m->col_box = 0;
		U8(m, 0xE0) = 2;
		break;
	default:
		PlayerSetAnime(m, 106);
		m->act2 = ST_NOVA;
		m->dash_b = 5;
		m->dash_a = 0;
		m->dash_f = 0;
		U8(m, 0xE1) = 24;
		m->spd_x = m->flip ? 0x28000 : -0x28000;
		m->spd_y = 0x38000;
		m->acc_x = 0;
		m->grav = m->fall_grav;
		U8(m, 0xE0) = 1;
		m->act4 = 0;
		break;
	}
	m->nohit_f = 1;
	U8(game_work, 0x1C) = 1;
	m->act3 = 0;
	return 1;
}

/* 0x8003F200: form 4, jump pressed in the air */
int UltimateHoverTry(PLAYER *m)
{
	if (m->form != FORM_ULTIMATE || !(m->trg & KEY_JUMP))
		return 0;
	m->spd_x = m->acc_x = m->spd_y = m->grav = 0;
	m->dash_a = 0;
	m->dash_b = FORM_ULTIMATE;
	m->dash_f = 0;
	U8(m, 0xD9) = 180;
	U8(m, 0xDA) = 0;
	PlayerSetAnimeShoot(m, 21);
	m->act2 = ST_UHOVER;
	m->act3 = 0;
	return 1;
}

extern const char	hover_bob[16];		/* 0x801F3714 */

/* 0x801EC35C: the hover ends at the time limit or a new jump press */
static int HoverEnd(PLAYER *m)
{
	int	end = (m->trg & KEY_JUMP) != 0;

	if (--U8(m, 0xD9) == 0)
		end = 1;
	if (end)
		ToFall(m);
	return end;
}

/* 0x801EC3AC: bob, then the side move; +1 forward, -1 back, 0 none */
static int HoverSide(PLAYER *m)
{
	m->y.w.hi += hover_bob[U8(m, 0xDA)];
	U8(m, 0xDA) = (U8(m, 0xDA) + 1) & 15;
	m->spd_x = 0;
	if (!(m->key & (KEY_R | KEY_L)))
		return 0;
	if (m->key & KEY_R) {
		if (m->wall & WALL_R)
			return 0;
		m->spd_x = m->walk_spd;
		return m->flip ? 1 : -1;
	}
	if (m->wall & WALL_L)
		return 0;
	m->spd_x = -m->walk_spd;
	return m->flip ? -1 : 1;
}

/* 0x801EC478 */
static void HoverStartMove(PLAYER *m, int dir)
{
	if (dir > 0) {
		PlayerSetAnimeShoot(m, 23);
		m->act3 = 3;
	} else {
		PlayerSetAnimeShoot(m, 25);
		m->act3 = 5;
	}
}

/* 0x801EB7C8: step 33, the Ultimate hover (sub-step table 0x801F36F8) */
void UltimateHover(PLAYER *m)
{
	int	dir;

	if (m->lock) {
		ToFall(m);
		return;
	}
	if (AirWeaponPose(m))
		return;
	AnimeTick(m);
	if (m->act3 == 0) {
		PlayerSyncShootAnime(m, 21);
		if (m->anime.flag & 0x80) {
			m->anime.flag &= 0x7F;
			SpawnEffect(33, 33, 0, 0);
			m->act3++;
		}
		return;
	}
	if (HoverEnd(m) || TryWall(m))
		return;
	if (m->act3 == 1 && TryDashInput(m) && U8(m, 0x92) == 0) {
		StartAirDash(m);
		return;
	}
	dir = HoverSide(m);
	switch (m->act3) {
	case 1:
		if (dir) {
			U8(m, 0xD9) = 40;
			HoverStartMove(m, dir);
		} else {
			PlayerSyncShootAnime(m, 21);
		}
		break;
	case 2:
		if (dir)
			HoverStartMove(m, dir);
		else
			PlayerSyncShootAnime(m, 22);
		break;
	case 3:
		if (dir > 0) {
			PlayerMove(m);
			PlayerSyncShootAnime(m, 23);
		} else if (dir < 0) {
			PlayerSetAnimeShoot(m, 25);
			m->act3 = 5;
		} else {
			PlayerSetAnimeShoot(m, 24);
			m->act3 = 4;
		}
		break;
	case 5:
		if (dir < 0) {
			PlayerMove(m);
			PlayerSyncShootAnime(m, 25);
		} else if (dir > 0) {
			PlayerSetAnimeShoot(m, 23);
			m->act3 = 3;
		} else {
			PlayerSetAnimeShoot(m, 26);
			m->act3 = 6;
		}
		break;
	default:
		if (dir) {
			HoverStartMove(m, dir);
		} else if (m->anime.next == 0) {
			PlayerSetAnimeShoot(m, 22);
			m->act3 = 2;
		} else {
			PlayerSyncShootAnime(m, m->act3 == 4 ? 24 : 26);
		}
		break;
	}
}

/* 0x801EC678: ceiling stops the rise; a blocked facing side stops the move */
static int NovaBlocked(PLAYER *m)
{
	if (m->wall & 0x04)
		m->spd_y = m->grav = 0;
	if (!(m->wall & (m->flip ? WALL_R : WALL_L)))
		return 0;
	m->spd_x = m->acc_x = 0;
	return 1;
}

static void NovaEnd(PLAYER *m)
{
	if (!m->lock)
		m->nohit_f = 0;
	U8(m, 0xE0) = 0;
	U8(game_work, 0x1C) = 0;
}

/* 0x801EB844: step 34, the form 4 special attack (sub-step table 0x801F3724) */
void NovaStep(PLAYER *m)
{
	int	blocked;

	if (m->lock) {
		PaletteRestore(m);
		U8(m, 0xE0) = 0;
		StandOrFall(m);
		return;
	}
	AnimeTick(m);
	blocked = NovaBlocked(m);
	switch (m->act3) {
	case 0:
		if (m->anime.flag & 0x80) {
			m->anime.flag = 0;
			m->act3++;
		} else {
			PlayerMoveAccel(m);
		}
		break;
	case 1:
		if (blocked && (m->anime.flag & 0x40)) {
			NovaEnd(m);
			StandOrFall(m);
		} else if (m->anime.flag & 0x20) {
			m->anime.flag = 0;
			SpawnWeapon(1, 22, 0, 0);
			PaletteSet(m, 73);
			PlaySound(1, 18, m);
			m->spd_x = m->flip ? 0x80000 : -0x80000;
			m->acc_x = m->spd_y = m->grav = 0;
			m->act3++;
		}
		break;
	default:
		if (blocked || U8(m, 0xE1) == 0) {
			NovaEnd(m);
			PaletteRestore(m);
			StandOrFall(m);
		} else {
			U8(m, 0xE1)--;
			PlayerMove(m);
		}
		break;
	}
}

/* 0x8003F32C: form 3, jump or dash pressed in the air */
int BladeMachTry(PLAYER *m)
{
	if (!(m->trg & KEY_JUMP) && !TryDashInput(m))
		return 0;
	PlayerSetAnime(m, 189);
	m->dash_a = 1;
	m->spd_x = m->acc_x = m->spd_y = m->grav = 0;
	m->dash_timer = 15;
	m->dash_b = 7;
	U8(m, 0xD9) = 0;
	ClearShotPose(m);
	m->act2 = ST_MACH;
	m->act3 = 0;
	SpawnWeapon(33, 32, 0, m);
	return 1;
}

/* 0x801EAECC: Mach Dash start, 0x801EC754: Mach Dash move (step 74) */
void MachDash(PLAYER *m)
{
	int	hit;

	if (m->lock) {
		StandOrFall(m);
		return;
	}
	AnimeTick(m);
	if (m->act3 == 0) {
		if (m->key & KEY_L)
			m->flip = 0;
		if (m->key & KEY_R)
			m->flip = 0x40;
		if (m->anime.next != 0)
			return;
		if ((m->key & (KEY_JUMP | KEY_DASH)) && ++U8(m, 0xD9) < 120)
			return;
		PlaySound(1, 5, m);
		if (m->key & KEY_U) {
			PlayerSetAnime(m, 190);
			m->spd_x = 0;
			m->spd_y = 2 * m->dash_min;
		} else if (m->key & KEY_D) {
			PlayerSetAnime(m, 191);
			m->spd_x = 0;
			m->spd_y = -2 * m->dash_min;
		} else {
			PlayerSetAnime(m, 192);
			m->spd_x = m->flip ? 2 * m->dash_min : -2 * m->dash_min;
			m->spd_y = 0;
		}
		m->dash_f = 1;
		m->nohit_f = 1;
		m->act3++;
		SpawnEffect(33, 25, 0, 0);
		return;
	}
	PlayerMove(m);
	if ((char)--m->dash_timer < 8)
		m->nohit_f = 0;
	if (m->spd_y > 0)
		hit = m->wall & 0x04;
	else if (m->spd_y < 0)
		hit = m->wall & 0x08;
	else if (m->spd_x > 0)
		hit = m->wall & WALL_R;
	else
		hit = m->wall & WALL_L;
	if (!hit && (char)m->dash_timer > 0)
		return;
	PlaySound(1, 6, m);
	m->nohit_f = 0;
	if (m->spd_y < 0 && (m->wall & 0x08) && (m->key & KEY_D)) {
		m->jump_latch = 0;
		m->dash_b = 0;
		ClearDashFlags(m);
		StartCrouch(m);
	} else {
		StandOrFall(m);
	}
}

typedef void	(*POSE)(PLAYER *m, int air);
extern POSE	weapon_pose[54];		/* 0x8001149C, by shot type - 3 */

/* 0x8003F864: the step of a weapon pose, by the shot type +0x96 */
void WeaponPoseStart(PLAYER *m, int air)
{
	u_char	n = U8(m, 0x96) - 3;

	if (n < 54)
		weapon_pose[n](m, air);
}

/* 0x80037E1C: step 73; no writer of step 73 was found */
void WaitExit(PLAYER *m)
{
	if (m->act3 != 0) {
		m->act = 3;
		m->act2 = 0;
		m->act3 = 0;
		return;
	}
	AnimeTick(m);
	if (S8(game_work, 0x0F) != 0)
		m->act3++;
}
