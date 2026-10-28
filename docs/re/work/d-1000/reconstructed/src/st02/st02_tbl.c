/*
 *	st02_tbl.c  --  ST02 object tables (excerpt)
 */
#include <sys/types.h>

#include "object.h"

extern void	M48_D1000(OBJECT *op);
extern void	V01_D1000Fx(OBJECT *op);

/* main object table   (0x800FA424) */
void	(*st02_main_tbl[])(OBJECT *) = {
	/* ... */
	M48_D1000,	/* 48 */
	/* ... */
};

/* visual object table (0x800FA558) */
void	(*st02_visual_tbl[])(OBJECT *) = {
	/* ... */
	V01_D1000Fx,	/* 01 */
	/* ... */
};
