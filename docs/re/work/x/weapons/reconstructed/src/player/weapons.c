/*
 *	weapons.c  --  X's special weapons: input, energy, gauge, and the weapon objects
 *
 *	A plausible reconstruction from the disassembly; not original code, not compiled.
 *	Each function carries its address. Speeds are 16.16; a positive spd_y is up.
 */
#include "player.h"
#include "weapons.h"
#include "game.h"
#include "sound.h"

#define FX(a)		((long)((a) * 65536))
#define ME		(&mega)
#define ENERGY(m)	((m)->energy[(m)->wpn_slot])

extern PLAYER	mega;			/* 0x800970A0 */
extern u_short	pad_held, pad_old, pad_press;	/* 0x800C456C, 0x800C456E, 0x800C4570 */
extern short	energy_max;		/* mega+0x14A */
extern short	item_timer;		/* mega+0x148 */
extern long	frame_count;		/* 0x80097420 */
extern short	cam_x, cam_y;		/* layer camera, view top left */

extern void	AnimeTick(void *o);
extern void	SetAnime(void *o, int n);
extern void	MoveObject(void *o);
extern void	MoveObject2(void *o);
extern void	CollideCheck(void *o);
extern void	CornerCheck(void *o);
extern void	SolidObject(void *o);
extern void	SetSpeedDir(void *o, int dir);
extern int	OffScreenCheck2(void *o, int rx, int ry);
extern void	DisplayCheck(void *o, int rx, int ry);
extern void	DeleteObject3(void *o);
extern void	SpawnExplosion(void *o);
extern void	SpawnHitEffect(void *o);
extern void	ShotAtBuster(OBJECT *s, PLAYER *m, int type);
extern OBJECT	*GetWeaponObject(void);
extern OBJECT	*SpawnWeapon(int flag, int type, int var, void *parent);
extern OBJECT	*SpawnWeaponFrom(int flag, int type, int var, void *parent);
extern OBJECT	*SpawnEffect(int flag, int type, int var, void *parent);
extern void	PlaySound(int bank, int id, void *o);
extern void	PaletteSlotLoad(int pal, int slot, int n);
extern void	PlayerFrameStream(void *o, int x, int y);
extern void	PlayerSetAnime(PLAYER *m, int n);
extern int	GetRNG(void);
extern int	rcos(int a), rsin(int a);

static int	Ended(PLAYER *m, int slot, int charged)
{
	int	end = (m->lock != 0) || (m->gate != 0);

	if (m->wpn_slot != slot)
		end = 1;
	if (charged >= 0 && m->weapon == charged)
		end = 1;
	return end;
}

/* ---------------------------------------------------------------- input */

/* 0x8003D7AC */
u_short PadToButtons(u_short raw)
{
	u_short	out = 0, bit = 1;
	int	i;

	for (i = 0; i < 16; i++, bit <<= 1)
		if (key_cfg[i] & raw)
			out |= bit;
	if ((out & (KEY_R | KEY_L)) == (KEY_R | KEY_L))
		out &= ~(KEY_R | KEY_L);
	if ((out & (KEY_U | KEY_D)) == (KEY_U | KEY_D))
		out &= ~(KEY_U | KEY_D);
	if (mega.var == 0) {			/* X: exchange 0x10 and 0x20 */
		u_short x = (out << 1) & KEY_BUSTER;
		if (out & KEY_BUSTER)
			x |= KEY_WEAPON;
		out = (out & ~(KEY_WEAPON | KEY_BUSTER)) | x;
	}
	return out;
}

/* 0x8003CD44 */
void PlayerInput(void)
{
	mega.key = PadToButtons(pad_held);
	mega.key_old = PadToButtons(pad_old);
	mega.key_new = PadToButtons(pad_press);
}

/* ---------------------------------------------------------------- energy */

/* 0x8003EB0C */
int WeaponCost(PLAYER *m, int id)
{
	int	c;

	if (m->var) {
		c = cost_z[id];
		if (m->parts & PART_ENERGY_SAVER)
			return 3 * c;
		if (game.armors & 0x20)		/* Black Zero */
			return 4 * c;
		return 6 * c;
	}
	c = 6 * cost_x[id];
	if (c == 0)
		return 0;
	if (m->parts & PART_ENERGY_SAVER)
		c /= 2;
	else if (m->form == 1 || m->form == 3 || m->form == 4)
		c = 2 * c / 3;
	return c ? c : 1;
}

/* 0x8003FA00 */
int HasEnergy(PLAYER *m)
{
	short	e = ENERGY(m);

	return e != 0 && e >= WeaponCost(m, m->wpn_id);
}

/* 0x8003F740 */
void SpendEnergy(PLAYER *m, int id)
{
	if (m->var_144 == ITEM_FREE_ENERGY && id != 47 && id != 44 && id != 49)
		return;
	ENERGY(m) -= WeaponCost(m, id);
}

/* part of PlayerHitCheck (0x80039514): after PlayerHurt */
void GaugeOnHurt(PLAYER *m)
{
	int	i, low, lv;

	if (m->var == 0) {
		m->energy[0] += energy_max / 8;		/* Giga gauge: 8 hurts */
		if (m->energy[0] > energy_max)
			m->energy[0] = energy_max;
	} else {
		m->energy[5] += WeaponCost(m, 5) / 2;
		if (m->energy[5] > energy_max)
			m->energy[5] = energy_max;
	}
	if (m->parts & 0x800) {				/* the lowest gauge + 12 */
		low = 0;
		lv = m->energy[0];
		for (i = 1; i < 9; i++)
			if (m->energy[i] < lv) {
				lv = m->energy[i];
				low = i;
			}
		lv += 12;
		m->energy[low] = lv < energy_max ? lv : energy_max;
	}
}

/* part of PauseItemUse (0x80034288): kinds other than 4 (life) and 5 (weapon refill) */
void ItemEffectStart(int kind)
{
	PlaySound(0, 34, 0);
	item_timer = (kind == ITEM_SHADOW_BUSTER) ? 480 : 300;
	mega.var_144 = kind;
}

/* ---------------------------------------------------------------- slot 2: Ice Burst */

/* 0x801EF9C8, type 2 step 0 */
static void IceAtBuster(OBJECT *s)
{
	s->flip = mega.flip;
	ShotAtBuster(s, ME, s->type);
	if (s->anime.flag != 1)
		return;
	SetAnime(s, 1);
	W_S16(s, 0x88) = 3;
	s->spd_x = s->flip ? FX(4.0) : FX(-4.0);
	s->x.w.hi += s->flip ? 8 : -8;
	s->atk_box = &ice_atk;			/* (-6, -6, 12, 12) */
	s->col_box = &ice_col;			/* (0, 0, 16, 16) */
	s->acc_x = 0;
	s->spd_y = 0;
	s->grav = 10240;			/* 0.15625 */
	s->act2 = 1;
	CollideCheck(s);
	if (W_COLLIDE(s) & 7) {
		SpawnExplosion(s);
		s->disp = 0;
		s->act = 3;
	}
}

/* 0x801EFAB4, type 2 step 1: flight, a shard every 3 updates (3 in all) */
static void IceFly(OBJECT *s)
{
	MoveObject2(s);
	if (--W_S16(s, 0x88) == 0) {
		IceShardTick(s);		/* 0x801EF0FC -> 0x801EFB5C: type 52, 2.5 px/update */
		W_S16(s, 0x88) = 3;
	}
}

/* 0x801EFB04, type 2 step 2: the block rests 180 updates, blinks the last 30 */
static void IceRest(OBJECT *s)
{
	if (W_S16(s, 0x88) < 30)
		s->disp = W_S16(s, 0x88) & 1 ? 0 : 1;
	if (--W_S16(s, 0x88) == 0) {
		W_SOLID(s) = 0;
		s->disp = 0;
		s->act = 3;
	}
}

/* 0x801EEFC0, type 2 / 52 state 1 */
void IceMain(OBJECT *s)
{
	u_char	c;

	if (OffScreenCheck2(s, 16, 16))
		goto end;
	AnimeTick(s);
	ice_steps[s->act2](s);
	if (s->act2 == 2)
		return;
	if (SHOT_HIT(s))
		goto end;
	if (s->act2 == 0)
		return;
	c = W_U8(s, 0x8C);			/* collide flags of this update */
	if ((c & 0xC) == 0xC || (c & 3) == 3) {
		SpawnExplosion(s);
		goto end;
	}
	if (c == 8) {				/* on the floor: become a block */
		SetAnime(s, 2);
		W_SOLID(s) = 1;
		s->spd_x = s->acc_x = s->spd_y = s->grav = 0;
		s->atk_box = 0;
		W_S16(s, 0x88) = 180;
		s->act2 = 2;
		return;
	}
	if (c & 7) {
		SpawnExplosion(s);
		W_SOLID(s) = 0;
		s->disp = 0;
		s->act = 3;
	}
	return;
end:
	W_SOLID(s) = 0;
	s->disp = 0;
	s->act = 3;
}

/* ---------------------------------------------------------------- slot 4: Metal Anchor */

/* 0x800447CC, type 4 step 0 */
void AnchorBounce(OBJECT *s)
{
	u_char	c;

	MoveObject2(s);
	c = W_U8(s, 0x8F);
	if (c) {
		if (c & 4)
			s->spd_y = FX(-3.0);		/* ceiling */
		else if (c & 8) {			/* floor: up again */
			s->spd_y = mega.var ? FX(3.0) : FX(6.0);
			s->grav = mega.var ? 8192 : 16384;
		} else if (c & 2) {
			s->spd_x = FX(1.5);
			s->flip = 0x40;
		} else if (c & 1) {
			s->spd_x = FX(-1.5);
			s->flip = 0;
		}
		s->x = s->old_x;
		s->y = s->old_y;
		MoveObject2(s);
		W_U8(s, 0x90) += mega.var ? 2 : 1;	/* bounces */
	}
	if (SHOT_HIT(s)) {				/* a hit counts; it does not stop */
		SHOT_HIT(s) = 0;
		W_U8(s, 0x90) += mega.var ? 9 : 1;
	}
}

/* ---------------------------------------------------------------- slot 5: Ground Dash */

/* 0x801F0D58, type 5 step 1: thrown when X reaches sub-step 2 of step 44 */
void GroundThrow(OBJECT *s)
{
	if (mega.step != 44) {
		s->act = 2;
		return;
	}
	if (mega.step2 == 2) {
		PlaySound(1, 34, s);
		if (mega.key & KEY_U) {
			s->spd_x = FX(0.5);
			s->spd_y = FX(1.5);
		} else {
			s->spd_x = FX(1.0);
			s->spd_y = FX(1.0);
		}
		s->grav = 2048;				/* 0.03125 */
		if (mega.flip == 0)
			s->spd_x = -s->spd_x;
		s->atk_box = &ground_atk;		/* (-24, -24, 48, 48) */
		s->col_box = &ground_col;
		s->act2++;
	}
	s->x.w.hi = mega.x.w.hi + W_S16(s, 0x8C);
	s->y.w.hi = mega.y.w.hi + W_S16(s, 0x8E);
}

/* 0x8003FC88, pose for id 14: freeze the world */
void GroundChargedPoseStart(PLAYER *m)
{
	m->step = 75;
	m->step2 = 0;
	m->act4 = 0;
	game.stop[2] = game.stop[3] = game.stop[4] = game.stop[5] = 1;
	game.stop[6] = game.stop[7] = game.stop[8] = game.stop[9] = 1;
	game.player_lock_1c = 1;
	m->latch = 1;
	m->special_timer = 0;
}

/* 0x801ECD8C, step 75 sub-step 1 */
void GroundChargedWait(PLAYER *m)
{
	if (++m->special_timer < 60)
		return;
	memset(&game.stop[2], 0, 8);
	game.player_lock_1c = 0;
	m->step2++;
}

/* ---------------------------------------------------------------- slot 6: Meteor Rain */

/* 0x801F2000, type 15 var 1 step 2: 10 meteors, 30 then 20 updates apart */
void MeteorSpawner(OBJECT *s)
{
	if (--W_S16(s, 0x8E))
		return;
	W_S16(s, 0x8E) = 20;
	if (SpawnWeaponFrom(1, s->type, 16, s))
		W_STEP2(s)++;
	if (++W_S8(s, 0x8C) >= 10)
		s->act = 2;
}

/* ---------------------------------------------------------------- slot 7: Guard Shell */

/* 0x800460C8, part sub-step 2: a blocked enemy shot */
void GuardBlock(OBJECT *s)
{
	AnimeTick(s);
	if (!(W_U8(s, 0x9C) & 2))
		return;
	W_S8(s, 0x8F) = 1;
	SetAnime(s, s->var < 3 ? 2 : 3);
	W_U8(s, 0x8E) = 20;
	s->act2++;
}

/* 0x8004548C, part sub-step 3: pay, then a shard back */
void GuardShard(OBJECT *s)
{
	OBJECT	*o;

	AnimeTick(s);
	if (--W_U8(s, 0x8E))
		return;
	if (ENERGY(ME) >= WeaponCost(ME, ME->wpn_slot)) {
		SpendEnergy(ME, s->type);
		W_S8(s, 0x8F) = 0;
		if ((o = SpawnWeaponFrom(1, WPN_GUARD_SHARD, 5, SHOT_PARENT(s))) != 0) {
			PlaySound(1, 31, s);
			o->atk_box = &guard_shard_atk;	/* (-8, -8, 16, 16) */
			o->act = 1;
			o->act2 = 4;
			SetAnime(o, 6);
			o->spd_x = o->spd_y = 0;
			if (s->var == 1)
				o->spd_y = FX(3.0);
			else if (s->var == 2)
				o->spd_y = FX(-3.0);
			else
				o->spd_x = mega.flip ? FX(3.0) : FX(-3.0);
		}
	}
	SetAnime(s, s->var < 3 ? 0 : 1);
	W_U8(s, 0x9C) &= ~2;
	s->act2--;
}

/* ---------------------------------------------------------------- slot 8: Ray Arrow */

/* 0x801F23C8, type 8 step 1: forward until a wall, then along it */
void RayForward(OBJECT *s)
{
	AnimeTick(s);
	MoveObject(s);
	RayKeepY(s);				/* while X is in a ladder or wall step */
	TerrainTest(s);				/* 0x8002F06C */
	if (W_COLLIDE(s) & (s->flip ? 1 : 2)) {
		if (SpawnWeaponFrom(s->flag, s->type, 1, s))
			W_ACT4(s)++;
		W_COLLIDE(s) = 0;
		s->atk_box = &ray_atk_v;	/* (-6, -18, 12, 36) */
		s->col_box = &ray_col_v;
		SetAnime(s, 1);
		s->spd_y = (mega.key & KEY_D) ? FX(-6.75) : FX(6.75);
		s->spd_x = 0;
		s->act2++;
		return;
	}
	DisplayCheck(s, 32, 32);
	if (s->disp == 0) {
		s->act = 3;
		s->atk_box = s->col_box = 0;
	}
}
