#include "emu.h"

#include "drivenum.h"

GAME_EXTERN(___empty);
GAME_EXTERN(carmarty);
GAME_EXTERN(fmtmarty);
GAME_EXTERN(fmtmarty2);
GAME_EXTERN(fmtowns);
GAME_EXTERN(fmtownsftv);
GAME_EXTERN(fmtownshr);
GAME_EXTERN(fmtownsmx);
GAME_EXTERN(fmtownssj);
GAME_EXTERN(fmtownsux);
GAME_EXTERN(fmtownsv03);

game_driver const *const driver_list::s_drivers_sorted[11] =
{
	&GAME_NAME(___empty),
	&GAME_NAME(carmarty),
	&GAME_NAME(fmtmarty),
	&GAME_NAME(fmtmarty2),
	&GAME_NAME(fmtowns),
	&GAME_NAME(fmtownsftv),
	&GAME_NAME(fmtownshr),
	&GAME_NAME(fmtownsmx),
	&GAME_NAME(fmtownssj),
	&GAME_NAME(fmtownsux),
	&GAME_NAME(fmtownsv03),
};

std::size_t const driver_list::s_driver_count = 11;
