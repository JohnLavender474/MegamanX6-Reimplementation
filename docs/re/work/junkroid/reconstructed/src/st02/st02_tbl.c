/*
 *	st02_tbl.c  --  ST02 object tables (excerpt)
 */
#include <sys/types.h>

#include "object.h"

extern void	M01_Mecha(OBJECT *op);
extern void	V00_MechaTama(OBJECT *op);

/* main object table   (0x800FA424) */
void	(*st02_main_tbl[])(OBJECT *) = {
	0,		/* 00 */
	M01_Mecha,	/* 01 */
	/* ... */
};

/* visual object table (0x800FA558) */
void	(*st02_visual_tbl[])(OBJECT *) = {
	V00_MechaTama,	/* 00 */
	/* ... */
};
