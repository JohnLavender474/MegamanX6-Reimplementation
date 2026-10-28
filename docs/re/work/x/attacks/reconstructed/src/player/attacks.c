/*
 *	attacks.c  --  X's buster: weapon select, charge, the shot try, the shot objects
 *
 *	A plausible reconstruction from the disassembly; not original code, not compiled.
 */
#include "player.h"
#include "shot.h"
#include "game.h"
#include "sound.h"

extern void	AnimeTick(void *o);
extern void	SetAnime(void *o, int n);
extern void	MoveObject(void *o);
extern void	PlayerSetAnime(PLAYER *m, int n);
extern void	StandAnime(PLAYER *m);
extern void	PlaySound(int bank, int id, void *o);
extern void	StopSound(int bank, int id);
extern void	PlayVoice(PLAYER *m, int id);
extern int	GetRNG(void);
extern int	OffScreenCheck2(void *o, int rx, int ry);
extern void	DisplayCheck(void *o, int rx, int ry);
extern void	CornerCheck(void *o);
extern void	DeleteObject3(void *o);
extern OBJECT	*GetEffectObject(void);
extern void	PaletteNormal(PLAYER *m);
extern void	PaletteSet(PLAYER *m, int n);
extern void	ClearActionFlags(PLAYER *m);
extern int	WeaponCost(PLAYER *m, int id);
extern void	WeaponPalette(PLAYER *m);
extern int	TryGigaAttack(PLAYER *m);
extern void	WeaponPoseStep(PLAYER *m, int a);
extern void	ShotSpark(OBJECT *s);
extern u_short	XShotClut(void);
extern void	ZeroWeaponUpdate(PLAYER *m);

extern u_char	game_c9;			/* 0x800CCF99: bit 0 auto charge, bit 1 auto fire (guess) */
extern char	weapon_bits[];			/* 0x80073ED8 */
extern char	weapon_slot_tbl[];		/* 0x80073EE4 */
extern char	weapon_index_tbl[];		/* 0x80073EF0 */
extern u_char	shot_times[][2];		/* 0x80073EFC: shot_timer, shot_wait */
extern void	(*shot_spawn[])(PLAYER *);	/* 0x80073F74 */
extern char	shot_max[];			/* 0x80074074 */
extern char	shot_max_rapid[];		/* 0x800740B4 */
extern char	shot_single[];			/* 0x800740F4 */
extern char	shot_counted[];			/* 0x80074134 */
extern char	shot_single_cnt[];		/* 0x80074174 */
extern char	shot_pose_wpn[];		/* 0x800741B4 */
extern char	weapon_no_charge[];		/* 0x80073E40 */
extern u_char	stand_shot_anime[];		/* 0x801F3698 */
extern char	buster_ofs[][2];		/* 0x8007673C (set 0), 0x8007693C (set 1) */
extern char	buster_ofs1[][2];
extern short	shot_ofs[][2];			/* 0x801F3894 */
extern u_char	anime_set_tbl[];		/* 0x801F49F4 */
extern u_char	muzzle_tbl[][3];		/* 0x80077C0C: sprite set, animation, sound */
extern ANIME	*shot_anime_tbl[];		/* 0x8007789C */
extern ANIME	*falcon_anime_tbl[];		/* 0x80077A80 */
extern HITBOX	lemon_atk, lemon_col;		/* 0x801F3790, 0x801F37B0 */
extern HITBOX	charge_atk[], charge_col[];	/* 0x801F3794, 0x801F37B4 (by var) */
extern HITBOX	falcon_atk, falcon_col;		/* 0x801F37C4, 0x801F37C8 */
extern long	frame_count;			/* 0x80097420 */
extern u_char	*bank_dir(int n);		/* scratch+0x1C directory entry n */

/* 0x8002C32C */
OBJECT *GetWeaponObject(void)
{
	OBJECT *o;
	int i;

	for (o = (OBJECT *)0x800950A0; o < (OBJECT *)0x800964A0; o = (OBJECT *)((char *)o + 0xA0)) {
		if (o->flag == 0) {
			for (i = 0; i < 0xA0; i++)
				((char *)o)[i] = 0;
			o->atk_box = 0;
			o->dmg_box = 0;
			o->col_box = 0;
			return o;
		}
	}
	return 0;
}

/* 0x8003D3F8 */
OBJECT *SpawnWeapon(int flag, int type, int var, PLAYER *parent)
{
	OBJECT *o = GetWeaponObject();

	if (o == 0)
		return 0;
	o->flag = flag;
	o->type = type;
	o->var = var;
	o->layer = mega.layer;
	if (parent)
		SHOT_PARENT(o) = parent;
	return o;
}

/* 0x8003D510 */
OBJECT *SpawnEffect(int flag, int type, int var, void *parent)
{
	OBJECT *o = GetEffectObject();

	if (o == 0)
		return 0;
	o->flag = flag;
	o->type = type;
	o->var = var;
	o->layer = mega.layer;
	if (parent)
		EFF_PARENT(o) = parent;
	return o;
}

/* 0x8003F480 */
void ChargeReset(PLAYER *m)
{
	StopSound(1, SE_CHARGE);
	m->charge_pal = 0;
	m->level[0] = m->level[1] = 0;
	m->charge[0] = m->charge[1] = 0;
}

/* 0x8003F7BC */
void BusterChargedId(PLAYER *m, int k)
{
	if (m->var_144 != 6) {
		if (!(m->parts & PART_UBUSTER)) {
			if (m->level[k] == 0)
				return;
			if (m->level[k] == 1) {
				(&m->wpn_id)[k] = WPN_HALF;
				return;
			}
		}
		if (m->form == FORM_ULTIMATE) {
			(&m->wpn_id)[k] = WPN_FULL_ULTIMATE;
			return;
		}
		if (m->form == FORM_FALCON) {
			(&m->wpn_id)[k] = WPN_FULL_FALCON;
			return;
		}
		if (m->form == FORM_BLADE) {
			(&m->wpn_id)[k] = WPN_FULL_BLADE;
			return;
		}
	}
	(&m->wpn_id)[k] = WPN_FULL;
}

/* 0x8003DF58 */
void ShotIdsUpdate(PLAYER *m)
{
	if (m->form == FORM_SHADOW) {
		if (m->var_144 == 6 && m->step != ST_SJUMP) {
			BusterChargedId(m, 1);
			return;
		}
		m->wpn_id = WPN_SABER;
		m->bst_id = WPN_SHADOW;
		if (m->level[1] == 2 || (m->parts & PART_UBUSTER))
			m->bst_id = WPN_SHADOW_FULL;
		return;
	}
	m->wpn_id = m->wpn_slot;
	if (m->wpn_slot == 0)
		m->wpn_id = WPN_SABER;
	else if (m->level[0] == 2)
		m->wpn_id = m->wpn_slot + 9;
	else if ((m->parts & PART_UBUSTER) &&
		 (m->form == FORM_FALCON || m->form == FORM_ULTIMATE || m->form == FORM_BLADE))
		m->wpn_id = m->wpn_slot + 9;
	m->bst_id = WPN_LEMON;
	BusterChargedId(m, 1);
}

/* 0x8003DD6C */
void WeaponSelect(PLAYER *m)
{
	int held, trg, i, old;

	if (m->lock || m->wpn_count || m->dash_box || m->var_e0 || game.player_lock)
		return;
	if (m->step == 45 || m->step == 46 || m->step == 47 || m->step == 76)
		return;
	held = m->key & (KEY_WPN_A | KEY_WPN_B);
	trg = m->trg & (KEY_WPN_A | KEY_WPN_B);
	if (held == (KEY_WPN_A | KEY_WPN_B) || trg == (KEY_WPN_A | KEY_WPN_B)) {
		if (m->wpn_slot) {
			m->wpn_slot = 0;
			ChargeReset(m);
			PlaySound(0, SE_WEAPON_CHANGE, 0);
		}
		return;
	}
	if (trg == 0)
		return;
	old = m->wpn_slot;
	i = weapon_index_tbl[m->wpn_slot];
	if (trg & KEY_WPN_A) {
		for (;;) {
			if (++i == 9) {
				i = 0;
				break;
			}
			if (weapon_bits[i] & m->skill)
				break;
		}
	} else {
		for (;;) {
			if (--i < 0)
				i = 8;
			if (i == 0 || (weapon_bits[i] & m->skill))
				break;
		}
	}
	if (weapon_slot_tbl[i] != old) {
		m->wpn_slot = weapon_slot_tbl[i];
		ChargeReset(m);
		PlaySound(0, SE_WEAPON_CHANGE, 0);
	}
}

/* 0x8003F0BC */
void WeaponChanged(PLAYER *m)
{
	PaletteNormal(m);
	if (m->wpn_slot == 1)
		SpawnEffect(1, 27, 0, 0);
	else if (m->wpn_slot == 6)
		SpawnEffect(1, 27, 10, 0);
	else if (m->wpn_slot == 8)
		SpawnEffect(1, 27, 12, 0);
}

/* 0x8003F048: before the step */
void WeaponUpdate(PLAYER *m)
{
	int old;

	if (m->var) {
		ZeroWeaponUpdate(m);
		return;
	}
	old = m->wpn_slot;
	WeaponSelect(m);
	ShotIdsUpdate(m);
	if (m->wpn_slot != old)
		WeaponChanged(m);
}

/* 0x8003FA00 */
int HasEnergy(PLAYER *m)
{
	short e = m->energy[m->wpn_slot];

	if (e == 0)
		return 0;
	return !(e < WeaponCost(m, m->wpn_id));
}

/* 0x8003E9E8: the weapon button (0x10) */
void PickWeaponShot(PLAYER *m)
{
	if ((m->trg & KEY_WEAPON) || ((m->key & KEY_WEAPON) && (game_c9 & 2))) {
		if (m->wpn_slot && !HasEnergy(m))
			return;
		if (m->wpn_id == WPN_SABER && (m->step == ST_SJUMP || m->step == ST_CROUCH))
			return;
		m->weapon = m->wpn_id;
		return;
	}
	if ((*(u_long *)&m->key & (REL_WEAPON | KEY_WEAPON)) != REL_WEAPON)
		return;
	if (m->wpn_id == 0 || m->wpn_id == WPN_SABER || weapon_no_charge[m->wpn_id])
		return;
	if (m->wpn_slot && !HasEnergy(m))
		return;
	m->weapon = m->wpn_id;
}

/* 0x8003F954: the buster button (0x20) */
void PickBusterShot(PLAYER *m)
{
	if (m->weapon != WPN_NONE)
		return;
	if ((m->trg & KEY_BUSTER) || ((m->key & KEY_BUSTER) && (game_c9 & 2))) {
		m->weapon = m->bst_id;
		return;
	}
	if ((*(u_long *)&m->key & (REL_BUSTER | KEY_BUSTER)) != REL_BUSTER)
		return;
	if (m->bst_id == 0)
		return;
	if (m->bst_id == WPN_SHADOW && m->level[1] == 0)
		return;
	m->weapon = m->bst_id;
}

/* 0x8003F740 */
void SpendEnergy(PLAYER *m, int id)
{
	if (m->var_144 == 3 && id != 47 && id != 44 && id != 49)
		return;
	m->energy[m->wpn_slot] -= WeaponCost(m, m->wpn_id);
}

/* 0x8003E614 */
void TryShoot(PLAYER *m)
{
	int w, bad, max;

	if (m->shot_wait)
		return;
	m->weapon = WPN_NONE;
	PickWeaponShot(m);
	PickBusterShot(m);
	w = m->weapon;
	if (w == WPN_NONE)
		return;
	if (w == WPN_SHADOW_FULL && m->dash_b == 10) {
		m->weapon = WPN_NONE;
		return;
	}
	if (w == 43 && (m->key & KEY_U) && m->dash_b != 9) {
		w = 56;
		m->weapon = 56;
		m->dash_b = 9;
	}
	if (shot_pose_wpn[w]) {
		if (m->step == ST_WGRAB) {
			m->weapon = WPN_NONE;
			return;
		}
		m->var_dd = 1;
		m->weapon_pose = 1;
		return;
	}
	bad = 0;
	if (m->wpn_state == 1)
		bad = (w == 2 || w == 11);
	else if (m->wpn_state == 3)
		bad = (w == 6 || w == 15);
	else if (m->wpn_state == 2)
		bad = (w == 7 || w == 16);
	if (bad) {
		m->weapon = WPN_NONE;
		return;
	}
	max = (m->parts & PART_RAPID5) ? shot_max_rapid[w] : shot_max[w];
	if (!(m->shot_count < max))
		return;
	if (shot_single[w] && m->wpn_count)
		return;

	shot_spawn[w](m);

	if (w == 7 || w == 1 || w == 10) {
		m->shot_pose = 0;
		m->weapon_pose = 0;
	} else {
		m->shot_pose = 1;
		m->weapon_pose = 1;
	}
	m->var_dd = 1;
	m->last_shot = w;
	m->pose_end = 0;
	m->shot_timer = shot_times[w][0];
	m->shot_wait = shot_times[w][1];
	if ((m->parts & PART_UBUSTER) && w != 15 && m->var_144 != 6 && !(w >= 1 && w <= 7))
		m->shot_wait = shot_times[w][1] << 2;
	if (shot_counted[w])
		m->shot_count++;
	if (shot_single_cnt[w])
		m->wpn_count++;
	if (++m->shot_cycle == 3)
		m->shot_cycle = 0;

	if (!(w == 7 || (w == 5 && (m->wall & (m->flip ? WALL_R : WALL_L)))))
		SpendEnergy(m, w);

	if (m->wpn_slot == 0 && m->giga == 0) {
		if (w == WPN_LEMON)
			m->flash_pal = 6;
		if (w == WPN_HALF)
			m->flash_pal = 5;
		if (w == WPN_FULL)
			m->flash_pal = 7;
		if (w == WPN_SHADOW)
			m->flash_pal = 6;
	}
}

/* 0x8003FA60: lemon and special weapons */
void SpawnShot(PLAYER *m)
{
	SpawnWeapon(1, m->weapon, 0, m);
}

/* 0x8003FA8C: 9, 18, 19, 20 start as a muzzle flash; the flash makes the shot */
void SpawnChargeShot(PLAYER *m)
{
	SpawnEffect(0x21, EFF_MUZZLE, m->weapon, 0);
	if (m->weapon == WPN_FULL || m->weapon == WPN_FULL_ULTIMATE || m->weapon == WPN_FULL_FALCON)
		if (GetRNG() & 1)
			PlayVoice(m, VOICE_CHARGE_SHOT);
}

/* 0x8003ED98: 1 = drop the charge */
int ChargeCancel(PLAYER *m)
{
	u_long k = *(u_long *)&m->key;

	if (m->step == ST_SJUMP || m->wpn_state == 1)
		return 1;
	if (m->trg & (KEY_WEAPON | KEY_BUSTER))
		return 1;
	if ((m->key & KEY_WEAPON) && (game_c9 & 2))
		return 1;
	if ((m->key & KEY_BUSTER) && (game_c9 & 2))
		return 1;
	if ((game_c9 & 1) && m->wpn_slot == 0) {
		if ((k & (REL_WEAPON | KEY_WEAPON)) == REL_WEAPON)
			return 1;
		return (k & (REL_BUSTER | KEY_BUSTER)) == REL_BUSTER;
	}
	if ((k & (REL_WEAPON | KEY_WEAPON)) == REL_WEAPON)
		return 1;
	if ((k & (REL_BUSTER | KEY_BUSTER)) == REL_BUSTER)
		return 1;
	if (m->charge[0] && !(m->key & KEY_WEAPON))
		return 1;
	if (m->charge[1] && !(m->key & KEY_BUSTER))
		return 1;
	return 0;
}

/* 0x8003EEE4 */
void WeaponCharge(PLAYER *m)
{
	int half = CHARGE_HALF, full = CHARGE_FULL;

	if (m->wpn_slot == 0) {
		m->level[0] = 0;
		return;
	}
	if (!(m->key & KEY_WEAPON) || m->level[0] == 2)
		return;
	if (m->form != FORM_FALCON && m->form != FORM_ULTIMATE && m->form != FORM_BLADE)
		return;
	if (m->energy[m->wpn_slot] < WeaponCost(m, m->wpn_slot + 9))
		return;
	m->charge[0]++;
	if (m->parts & PART_QUICK) {
		full >>= 1;
		half = CHARGE_HALF_QUICK;
	}
	if (m->charge[0] == half) {
		PlaySound(1, SE_CHARGE, m);
		m->level[0] = 1;
	} else if (m->charge[0] == full)
		m->level[0] = 2;
}

/* 0x8003FFF4 */
void BusterCharge(PLAYER *m)
{
	int half = CHARGE_HALF, full = CHARGE_FULL;

	if (!(m->key & KEY_BUSTER) && !((game_c9 & 1) && m->wpn_slot == 0))
		return;
	if (m->level[1] == 2)
		return;
	m->charge[1]++;
	if (m->parts & PART_QUICK) {
		full = CHARGE_FULL_QUICK;
		half = CHARGE_HALF_QUICK;
	}
	if (m->charge[1] == half) {
		PlaySound(1, SE_CHARGE, m);
		m->level[1] = 1;
	} else if (m->charge[1] == full)
		m->level[1] = 2;
}

/* 0x8003FF90 */
int ChargeGlow(PLAYER *m, int k)
{
	if (m->level[k] == 0)
		return 0;
	if (m->level[k] == 1)
		m->charge_pal = 38;
	else if ((&m->wpn_id)[k] == WPN_FULL_ULTIMATE)
		m->charge_pal = 47;
	else
		m->charge_pal = 41;
	return 1;
}

/* 0x8003EC78: after the step */
void ChargeUpdate(PLAYER *m)
{
	if (m->var)
		return;
	if (m->var_e0 && m->form == FORM_ULTIMATE)
		return;
	if (m->step == 0 || m->step == 1 || m->step == 25 || m->step == 24)
		return;
	if (m->dash_box)
		return;
	if (ChargeCancel(m)) {
		ChargeReset(m);
		if (m->flash_pal)
			PaletteSet(m, 50);
		else
			PaletteNormal(m);
		if (m->wpn_state == 1)
			m->charge_pal = 41;
		return;
	}
	if (m->parts & PART_UBUSTER)
		return;
	WeaponCharge(m);
	BusterCharge(m);
	if (!ChargeGlow(m, 0))
		ChargeGlow(m, 1);
}

/* 0x8003F3DC */
void ChargeStop(PLAYER *m)
{
	if (m->var)
		return;
	ChargeReset(m);
	ShotIdsUpdate(m);
	PaletteNormal(m);
	if (m->wpn_slot == 1)
		SpawnEffect(1, 27, 0, 0);
	else if (m->wpn_slot == 6)
		SpawnEffect(1, 27, 10, 0);
	else if (m->wpn_slot == 8)
		SpawnEffect(1, 27, 12, 0);
}

/* part of PlayerUpdate, 0x80034FE8 and 0x8003504C */
void ShotTimers(PLAYER *m)
{
	m->weapon_pose = 0;
	m->pose_end = 0;
	if (m->shot_wait)
		m->shot_wait--;
	if (m->shot_timer && --m->shot_timer == 0) {
		m->pose_end = 1;
		m->shot_pose = 0;
	}
}

/* 0x8003CE38: after ChargeUpdate */
void ShotPalettes(PLAYER *m)
{
	if (m->step == 0 || m->step == 1 || m->giga > 0)
		return;
	if (m->flash_pal && --m->flash_pal == 0)
		PaletteNormal(m);
	if (m->giga < 0)
		m->charge_pal = 35;
	if (m->charge_pal == 0)
		return;
	if (m->blink_timer) {
		m->blink_timer--;
	} else {
		if (m->blink_phase == 0)
			PaletteSet(m, m->charge_pal);
		else
			PaletteNormal(m);
		m->blink_timer = 1;
		m->blink_phase ^= 1;
	}
	m->charge_pal = 0;
}

/* 0x801EBF28: X stand, in the update of a shot */
void StandShotAnime(PLAYER *m)
{
	PlayerSetAnime(m, stand_shot_anime[m->weapon]);
}

/* 0x801EE34C */
void ShotAtBuster(OBJECT *s, PLAYER *m, int type)
{
	char *d = anime_set_tbl[(u_char)m->anime_no] ? buster_ofs1[m->anime.sprt] : buster_ofs[m->anime.sprt];

	if (s->flip) {
		POS_X(s) = POS_X(m) - d[0];
		POS_X(s) -= shot_ofs[type][0];
	} else {
		POS_X(s) = POS_X(m) + d[0];
		POS_X(s) += shot_ofs[type][0];
	}
	POS_Y(s) = POS_Y(m) + d[1] + shot_ofs[type][1];
}

/* 0x801EEB7C */
void LemonFollow(OBJECT *s)
{
	PLAYER *m;

	if (SHOT_DETACH(s))
		return;
	m = SHOT_PARENT(s);
	if (m->shot_pose == 0)
		SHOT_DETACH(s) = 1;
	if (m->flip != s->flip)
		SHOT_DETACH(s) = 1;
	if (SHOT_DETACH(s) == 0)
		ShotAtBuster(s, m, s->type);
}

/* 0x801ECE0C: weapon type 0, state 0 */
void LemonInit(OBJECT *s)
{
	PLAYER *m = SHOT_PARENT(s);

	s->flag = 0x21;
	s->col_box = &lemon_col;
	s->disp = 1;
	s->atk_box = &lemon_atk;
	s->anime_tbl = shot_anime_tbl;
	s->tpage = 0;
	s->pri = 1;
	s->sprt_p = (char *)bank_dir(1);
	s->clut = (mega.parts & (PART_BPLUS | PART_SPEED)) ? 0x7844 : 0x7802;
	if (mega.parts & PART_RAPID5)
		s->clut = 0x7844;
	s->flip = m->flip;
	SHOT_DETACH(s) = 0;
	LemonFollow(s);
	s->x.w.lo = 0;
	s->y.w.lo = 0;
	s->spd_x = (m->parts & PART_SPEED) ? 0x8c000 : 0x6c000;
	if (s->flip == 0)
		s->spd_x = -s->spd_x;
	s->acc_x = 0;
	s->spd_y = 0;
	s->grav = 0;
	SetAnime(s, 21);
	s->act++;
	DisplayCheck(s, 12, 8);
	SpawnEffect(0x21, EFF_MUZZLE, 0, 0);
}

/* 0x801EE638 */
void ShotEnd(OBJECT *s)
{
	s->disp = 0;
	s->act = 3;
	s->atk_box = 0;
	s->col_box = 0;
}

/* 0x801EE4DC */
void LemonMove(OBJECT *s)
{
	MoveObject(s);
	if (s->col_f == 0) {
		CornerCheck(s);
		if (s->col_f == 0)
			return;
	}
	ShotSpark(s);
	ShotEnd(s);
}

/* 0x801EE538 */
void LemonHit(OBJECT *s)
{
	s->atk_box = 0;
	s->col_box = 0;
	if (SHOT_HIT(s) > 0) {
		ShotEnd(s);
		return;
	}
	s->spd_x = -s->spd_x;
	s->spd_y = (GetRNG() & 1) ? 0x40ccb : -0x40ccb;
	s->act = 2;
	s->act2 = 0;
	s->flip ^= 0x40;
	DisplayCheck(s, 12, 8);
}

/* 0x801EE458: state 1 */
void LemonMain(OBJECT *s)
{
	if (OffScreenCheck2(s, 12, 8)) {
		ShotEnd(s);
		return;
	}
	AnimeTick(s);
	if (SHOT_HIT(s) == 0) {
		LemonMove(s);
		DisplayCheck(s, 12, 8);
	} else
		LemonHit(s);
}

/* 0x801EE5D4: state 2, deflected */
void LemonBounce(OBJECT *s)
{
	if (OffScreenCheck2(s, 12, 8)) {
		ShotEnd(s);
		return;
	}
	AnimeTick(s);
	MoveObject(s);
	DisplayCheck(s, 12, 8);
}

/* 0x801EE650: state 3 */
void LemonDelete(OBJECT *s)
{
	PLAYER *m = SHOT_PARENT(s);

	s->atk_box = 0;
	m->shot_count--;
	DeleteObject3(s);
}

/* 0x801ECF64: weapon types 9 and 18, state 0 */
void ChargeShotInit(OBJECT *s)
{
	s->disp = 1;
	if (s->type == WPN_HALF) {
		s->var = 0;
		s->spd_x = (mega.parts & PART_SPEED) ? 0xa0000 : 0x80000;
	} else {
		s->var = 1;
		s->spd_x = (mega.parts & PART_SPEED) ? 0xc0000 : 0xa0000;
	}
	s->col_box = &charge_col[s->var];
	s->atk_box = &charge_atk[s->var];
	if (s->flip == 0)
		s->spd_x = -s->spd_x;
	s->acc_x = 0;
	s->spd_y = 0;
	s->grav = 0;
	SetAnime(s, s->var + 26);
	s->act++;
	DisplayCheck(s, 32, 20);
}

/* 0x801EE684: state 1 */
void ChargeShotMain(OBJECT *s)
{
	if (OffScreenCheck2(s, 32, 20) || SHOT_HIT(s)) {
		ShotEnd(s);
		return;
	}
	AnimeTick(s);
	MoveObject(s);
	DisplayCheck(s, 32, 20);
	if (s->col_f == 0) {
		CornerCheck(s);
		if (s->col_f == 0)
			return;
	}
	ShotSpark(s);
	ShotEnd(s);
}

/* 0x801EE72C: states 2, 3 of types 9, 18, 20, 21 */
void ChargeShotDelete(OBJECT *s)
{
	s->atk_box = 0;
	if (s->type != WPN_ULTIMATE_PART)
		mega.shot_count--;
	DeleteObject3(s);
}

/* 0x801EE774: weapon type 19, state 0 */
void FalconShotInit(OBJECT *s)
{
	s->flag = 1;
	s->disp = 1;
	HIT_TAG(s) = 1;
	s->atk_box = &falcon_atk;
	s->col_box = &falcon_col;
	s->anime_tbl = falcon_anime_tbl;
	s->clut = 0x7802;
	s->tpage = 0x540;
	s->pri = 1;
	s->spd_x = s->flip ? 0xa0000 : -0xa0000;
	s->acc_x = 0;
	s->spd_y = 0;
	s->grav = 0;
	SetAnime(s, 0);
	s->act2 = 0;
	s->act++;
	SpawnEffect(1, 27, 8, 0);
}

/* 0x801EE860: state 1 */
void FalconShotMain(OBJECT *s)
{
	if (OffScreenCheck2(s, 32, 32)) {
		s->disp = 0;
		s->act = 2;
		s->atk_box = 0;
		return;
	}
	AnimeTick(s);
	MoveObject(s);
	if ((frame_count & 3) == 0)
		HIT_TAG(s)++;
	if (s->col_f == 0) {
		CornerCheck(s);
		if (s->col_f == 0 && SHOT_HIT(s) == 0)
			return;
	}
	if (SHOT_HIT(s) == 0)
		ShotSpark(s);
	s->disp = 0;
	s->act = 2;
	s->atk_box = 0;
	s->col_box = 0;
}

/* 0x801EE940: state 2 */
void FalconShotDelete(OBJECT *s)
{
	mega.shot_count--;
	DeleteObject3(s);
}

/* 0x800499A0 */
void MuzzleFollow(EFFECT *e)
{
	if (EFF_DETACH(e))
		return;
	if (mega.shot_pose == 0)
		EFF_DETACH(e) = 1;
	if (mega.flip != e->flip)
		EFF_DETACH(e) = 1;
	if (EFF_DETACH(e) == 0)
		ShotAtBuster(e, &mega, e->var);
}

/* 0x80049270: effect type 2, state 0 */
void MuzzleInit(EFFECT *e)
{
	int k;

	e->disp = 1;
	if (e->var == WPN_HALF)
		k = 0;
	else if (e->var == WPN_FULL)
		k = 1;
	else if (e->var == WPN_FULL_ULTIMATE || e->var == WPN_FULL_FALCON)
		k = 2;
	else
		k = 3;
	e->anime_tbl = shot_anime_tbl;
	e->clut = 0x7802;
	e->tpage = 0;
	e->sprt_p = (char *)bank_dir(muzzle_tbl[k][0]);
	if (k == 3 && e->var == WPN_LEMON) {
		e->clut = XShotClut();
		if (mega.parts & PART_RAPID5)
			e->clut = 0x7844;
	}
	e->pri = 0;
	EFF_DETACH(e) = 0;
	e->flip = mega.flip;
	MuzzleFollow(e);
	e->x.w.lo = 0;
	e->y.w.lo = 0;
	SetAnime(e, muzzle_tbl[k][1]);
	PlaySound(1, muzzle_tbl[k][2], e);
	e->act++;
	DisplayCheck(e, 64, 32);
}

/* 0x80049854: step 0, make the shot at the frame with an event flag */
void MuzzleSpawn(EFFECT *e)
{
	OBJECT *s;

	if (e->anime.flag) {
		if (e->var != WPN_LEMON && e->var != WPN_SHADOW && (s = GetWeaponObject()) != 0) {
			s->flag = 0x21;
			s->var = 0;
			s->type = e->var;
			s->x = e->x;
			s->y = e->y;
			s->flip = e->flip;
			s->sprt_p = e->sprt_p;
			s->anime_tbl = e->anime_tbl;
			s->tpage = e->tpage;
			s->clut = e->clut;
			s->pri = e->pri;
			mega.shot_count++;
		}
		e->act2++;
	}
	DisplayCheck(e, 64, 32);
}

/* 0x80049950: step 1 */
void MuzzleEnd(EFFECT *e)
{
	if (e->anime.next == 0 || EFF_DETACH(e)) {
		DeleteObject3(e);
		return;
	}
	DisplayCheck(e, 64, 32);
}

/* 0x80049800: state 1 */
void MuzzleMain(EFFECT *e)
{
	AnimeTick(e);
	MuzzleFollow(e);
	if (e->act2 == 0)
		MuzzleSpawn(e);
	else
		MuzzleEnd(e);
}

/* 0x80049478: effect type 2 */
void MuzzleFlash(EFFECT *e)
{
	if (mega.step == 20) {
		DeleteObject3(e);
		return;
	}
	if (e->act == 0)
		MuzzleInit(e);
	else
		MuzzleMain(e);
}
