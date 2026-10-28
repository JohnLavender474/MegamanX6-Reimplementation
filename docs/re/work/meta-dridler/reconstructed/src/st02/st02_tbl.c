/*
 *	st02_tbl.c  --  ST02 object tables (excerpt)
 */
#include <sys/types.h>

#include "object.h"

extern void	M00_Dridler(OBJECT *op);
extern void	X00_DridlerFx(OBJECT *op);

/* main object table   (0x800FA424) */
void	(*st02_main_tbl[])(OBJECT *) = {
	M00_Dridler,	/* 00 */
	/* ... */
};

/* misc object table   (0x800FA814) */
void	(*st02_misc_tbl[])(OBJECT *) = {
	X00_DridlerFx,	/* 00 */
	/* ... */
};
