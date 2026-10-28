/*
 *	m26_light_capsule.c
 *
 *	stage object 26 : Dr. Light capsule (main program, shared by all stages)
 *	                  var 16 : the capsule lid
 */
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>

#include "object.h"
#include "game.h"
#include "sound.h"

#define CAP_LID		16
#define CAP_SPECIAL	8
#define CAP_BANK	128
#define CAP_VOICE	28
#define LID_OFS_Y	78
#define NEAR_X		64
#define NEAR_UP		104
#define NEAR_DOWN	16

#define cap_parent(op)	(*(OBJECT **)&(op)->timer)
#define cap_timer(op)	(*(long *)&(op)->timer)
#define mega_b(n)	(((u_char *)&mega)[n])

extern ANIME	*cap_anime[];
extern HITBOX	cap_col_box, cap_dmg_box, lid_col_box, lid_dmg_box;
extern u_char	dmg_tbl_capsule[];
extern u_char	cap_part_bit[8];
extern char	cap_pic_var[8];
extern u_char	cap_msg[8];
extern void	SoundPan(int no, int pan);
extern void	MusicRestore(void);
extern void	PaletteRefresh(OBJECT *op);
extern void	SetSpriteBank(OBJECT *op, int bank);
extern void	SolidPlayer(OBJECT *op);
extern void	SolidVisual(OBJECT *op);
extern void	SolidMain(OBJECT *op);
extern void	SolidWeaponHit(OBJECT *op);
extern void	SolidLink(OBJECT *op);

static void	cap_init(OBJECT *op);
static void	cap_main(OBJECT *op);
static void	cap_near(OBJECT *op);
static void	cap_voice(OBJECT *op);
static void	cap_text(OBJECT *op);
static void	cap_talk(OBJECT *op);
static void	cap_close(OBJECT *op);
static void	cap_closed(OBJECT *op);
static void	cap_enter(OBJECT *op);
static void	cap_ph2(OBJECT *op);
static void	cap_ph3(OBJECT *op);
static void	cap_hold(OBJECT *op);
static void	cap_give(OBJECT *op);
static void	cap_leave(OBJECT *op);

static void (*cap_mode_tbl[])(OBJECT *) = {
	cap_near, cap_voice, cap_text, cap_talk, cap_close, cap_closed,
	cap_enter, cap_ph2, cap_ph3, cap_hold, cap_give, cap_leave
};

static void pause_on(u_char on)
{
	game.pause[6] = on;
	game.pause[2] = on;
	game.pause[3] = on;
	game.pause[7] = on;
}

static void spawn_effect(OBJECT *op, char type, char var)
{
	OBJECT *ep = GetEffectObject();

	if (ep == NULL)
		return;
	ep->flag = 0x49;
	ep->type = type;
	ep->var = var;
	*(OBJECT **)&ep->atk_box = op;
	ep->layer = op->layer;
}

void LightCapsule(OBJECT *op)
{
	OBJECT *pp;

	op->old_x = op->x;
	op->old_y = op->y;

	if (op->var != CAP_LID) {
		if (op->act == 0)
			cap_init(op);
		else
			cap_main(op);
		return;
	}

	pp = cap_parent(op);
	if (pp->flag == 0) {
		DeleteObject3(op);
		return;
	}
	if (op->act == 0) {
		op->col_box = &lid_col_box;
		op->dmg_box = &lid_dmg_box;
		((u_char *)op)[0xa2] = 1;
		op->dmg_tbl = dmg_tbl_capsule;
		((u_char *)op)[0x75] = 1;
		op->x.w.hi = POS_X(pp);
		op->act++;
		op->y.w.hi = POS_Y(pp) - LID_OFS_Y;
	}
	SolidWeaponHit(op);
	SolidLink(op);
	if (mega_b(0xd1) == 0)
		SolidPlayer(op);
	SolidVisual(op);
	SolidMain(op);
}

static int cap_taken(OBJECT *op)
{
	u_char bit;
	int taken = 0;

	if (op->var == CAP_SPECIAL) {
		if (mega.var == 0)
			return (game.armors & 0x08) != 0 || mega_b(0xe7) != 0;
		return (game.armors & 0x20) != 0;
	}
	bit = cap_part_bit[op->var];
	if ((game.armors & 0x04) && (bit & 0x0f))
		taken = 1;
	if ((game.armors & 0x02) && (bit & 0xf0))
		taken = 1;
	if (game.armor_parts & bit)
		taken = 1;
	return taken;
}

static void cap_init(OBJECT *op)
{
	OBJECT *lp;

	if (cap_taken(op)) {
		DeleteObject(op);
		return;
	}
	op->dmg_tbl = dmg_tbl_capsule;
	((u_char *)op)[0xa2] = 1;
	op->disp = 1;
	op->flag |= 0x08;
	op->col_box = &cap_col_box;
	op->dmg_box = &cap_dmg_box;
	op->anime_tbl = cap_anime;
	op->layer = mega.layer;
	SetSpriteBank(op, GetSpriteBank(CAP_BANK));
	op->pri = 4;
	op->flip = OBJ_FLIP_L;
	((u_char *)op)[0x75] = 1;
	mega_b(0xe2) = 0;
	SetAnime(op, 0);

	for (;;) {
		CollideCheck(op);
		if (op->col_f & COL_D)
			break;
		op->y.w.hi += 16;
	}

	lp = GetStageObject();
	if (lp != NULL) {
		lp->flag = 0x09;
		lp->type = 26;
		lp->var = CAP_LID;
		cap_parent(lp) = op;
		lp->layer = op->layer;
	}

	if (op->var == CAP_SPECIAL)
		spawn_effect(op, 35, mega.var == 0 ? 16 : 17);
	else
		spawn_effect(op, 35, cap_pic_var[op->var]);

	op->act2 = 0;
	op->act++;
	DisplayCheck(op, 136, 136);
}

static void cap_main(OBJECT *op)
{
	AnimeTick(op);
	cap_mode_tbl[op->act2](op);
	SolidWeaponHit(op);
	if (mega_b(0xd1) == 0)
		SolidPlayer(op);
	SolidVisual(op);
	SolidMain(op);
	SolidLink(op);
	DisplayCheck(op, 136, 136);
}

static void cap_near(OBJECT *op)
{
	short dx = mega.x.w.hi - POS_X(op);
	short dy;

	if (dx < 0)
		dx = -dx;
	if (dx > NEAR_X)
		return;
	dy = mega.y.w.hi - POS_Y(op);
	if (dy > 0) {
		if (dy > NEAR_DOWN)
			return;
	} else if (-dy > NEAR_UP)
		return;

	SetAnime(op, 10);
	if (mega.x.l - op->x.l > 0) {
		SoundPan(SE_CAPSULE_OPEN, 0);
		op->flip = OBJ_FLIP_R;
	} else {
		SoundPan(SE_CAPSULE_OPEN, 0x40);
		op->flip = OBJ_FLIP_L;
	}
	pause_on(1);
	op->act3 = 0;
	op->act2++;
}

static void cap_voice(OBJECT *op)
{
	if (op->act3 == 0) {
		if (op->anime.flag) {
			StreamStop(3);
			op->anime.flag = 0;
			op->act3++;
		}
	} else if (stream_state == 0) {
		StreamPlay(CAP_VOICE, 127);
		op->act3 = 0;
		op->act2++;
	}
}

static void cap_text(OBJECT *op)
{
	if (stream_state == 2) {
		TextOpen(cap_msg[op->var], 255, 0);
		op->act2++;
	}
}

static void cap_talk(OBJECT *op)
{
	if (text_state == 1) {
		SetAnime(op, 11);
		op->act3 = 0;
		op->act2++;
	}
}

static void cap_close(OBJECT *op)
{
	if (op->act3 == 0) {
		if (text_state == 0) {
			StreamStop(3);
			op->act3++;
		}
	} else if (stream_state == 0) {
		StageMusicRestart();
		SetAnime(op, 12);
		op->act3 = 0;
		op->act2++;
	}
}

static void cap_closed(OBJECT *op)
{
	if (op->anime.flag) {
		MusicRestore();
		pause_on(0);
		op->act2++;
	}
}

static void cap_enter(OBJECT *op)
{
	if (!(((u_char *)op)[0x72] & 0x08))
		return;
	SetAnime(op, 1);
	SoundPan(SE_CAPSULE_IN, mega.flip);
	spawn_effect(op, 36, 0);
	mega_b(0xe2) = 1;
	pause_on(1);
	mega.x = op->x;
	op->act2++;
}

static void cap_ph2(OBJECT *op)
{
	mega.x = op->x;
	if (mega_b(0xe2) != 2)
		return;
	spawn_effect(op, 37, 0);
	spawn_effect(op, 37, 1);
	spawn_effect(op, 37, 2);
	spawn_effect(op, 37, 3);
	op->act2++;
}

static void cap_ph3(OBJECT *op)
{
	mega.x = op->x;
	if (mega_b(0xe2) == 3) {
		SetAnime(op, 2);
		cap_timer(op) = 40;
		op->act2++;
	}
}

static void cap_hold(OBJECT *op)
{
	mega.x = op->x;
	if (cap_timer(op) == 0)
		op->act2++;
	else
		cap_timer(op)--;
}

static void cap_give(OBJECT *op)
{
	mega_b(0xe2) = 4;
	mega.x = op->x;
	SetAnime(op, 13);
	if (op->var == CAP_SPECIAL) {
		if (mega.var != 0) {
			game.armors |= 0x20;
			PaletteRefresh(&mega);
		}
	} else
		game.armor_parts |= cap_part_bit[op->var];
	pause_on(0);
	op->act2++;
}

static void cap_leave(OBJECT *op)
{
	if (OffScreenCheck2(op, 136, 136) == 1)
		DeleteObjectKeep(op);
}
