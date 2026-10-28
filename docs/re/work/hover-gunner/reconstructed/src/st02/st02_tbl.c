/*
 *	st02_tbl.c  --  ST02 object tables (excerpt)
 */
#include <sys/types.h>

#include "object.h"

extern void	M05_Jet(OBJECT *op);
extern void	V04_JetTama(OBJECT *op);
extern void	X06_JetFire(OBJECT *op);

/* main object table   (0x800FA424) */
void	(*st02_main_tbl[])(OBJECT *) = {
	/* ... */
	M05_Jet,	/* 05 */
	/* ... */
};

/* visual object table (0x800FA558) */
void	(*st02_visual_tbl[])(OBJECT *) = {
	/* ... */
	V04_JetTama,	/* 04 */
	/* ... */
};

/* misc object table   (0x800FA814) */
void	(*st02_misc_tbl[])(OBJECT *) = {
	/* ... */
	X06_JetFire,	/* 06 */
	/* ... */
};
