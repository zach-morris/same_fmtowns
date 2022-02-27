// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#include "emu.h"
#include "oricext.h"
#include "jasmin.h"
#include "microdisc.h"

DEFINE_DEVICE_TYPE(ORICEXT_CONNECTOR, oricext_connector, "oricext_connector", "ORIC extension connector")

oricext_connector::oricext_connector(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, ORICEXT_CONNECTOR, tag, owner, clock),
	device_single_card_slot_interface<device_oricext_interface>(mconfig, *this),
	irq_handler(*this),
	cpu(*this, finder_base::DUMMY_TAG)
{
}

oricext_connector::~oricext_connector()
{
}

void oricext_connector::device_start()
{
	irq_handler.resolve_safe();
}

void oricext_connector::irq_w(int state)
{
	irq_handler(state);
}

device_oricext_interface::device_oricext_interface(const machine_config &mconfig, device_t &device) :
	device_interface(device, "oricext"),
	cpu(nullptr),
	connector(nullptr),
	bank_c000_r(nullptr),
	bank_e000_r(nullptr),
	bank_f800_r(nullptr),
	bank_c000_w(nullptr),
	bank_e000_w(nullptr),
	bank_f800_w(nullptr),
	rom(nullptr),
	ram(nullptr)
{
}

void device_oricext_interface::interface_pre_start()
{
	connector = downcast<oricext_connector *>(device().owner());
	cpu = connector->cpu.target();
	bank_c000_r = device().membank(":bank_c000_r");
	bank_e000_r = device().membank(":bank_e000_r");
	bank_f800_r = device().membank(":bank_f800_r");
	bank_c000_w = device().membank(":bank_c000_w");
	bank_e000_w = device().membank(":bank_e000_w");
	bank_f800_w = device().membank(":bank_f800_w");
	rom = (uint8_t *)cpu->memregion(DEVICE_SELF)->base();
	ram = (uint8_t *)device().memshare(":ram")->ptr();

	memset(junk_read, 0xff, sizeof(junk_read));
	memset(junk_write, 0x00, sizeof(junk_write));
}

WRITE_LINE_MEMBER(device_oricext_interface::irq_w)
{
	connector->irq_w(state);
}

void oricext_intf(device_slot_interface &device)
{
	device.option_add("jasmin", ORIC_JASMIN);
	device.option_add("microdisc", ORIC_MICRODISC);
}
