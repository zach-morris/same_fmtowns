// license:BSD-3-Clause
// copyright-holders:Curt Coder
/*********************************************************************

    formats/c8280_dsk.h

    Commodore 8280 disk image format

*********************************************************************/
#ifndef MAME_FORMATS_C8280_DSK_H
#define MAME_FORMATS_C8280_DSK_H

#pragma once

#include "wd177x_dsk.h"

class c8280_format : public wd177x_format
{
public:
	c8280_format();

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_C8280_FORMAT;

#endif // MAME_FORMATS_C8280_DSK_H
