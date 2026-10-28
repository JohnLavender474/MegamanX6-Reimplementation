/*
 *	st03_tbl.c  --  ST03 object tables (excerpt)
 */
#include <sys/types.h>

#include "object.h"

extern void	M49_Yammark(OBJECT *op);
extern void	V15_YammarkFx(OBJECT *op);

/* main object table */
void	(*st03_main_tbl[])(OBJECT *) = {
	/* ... */
	M49_Yammark,	/* 49 */
	/* ... */
};

/* visual object table */
void	(*st03_visual_tbl[])(OBJECT *) = {
	/* ... */
	V15_YammarkFx,	/* 15 */
	/* ... */
};
