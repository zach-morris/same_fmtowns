// license:BSD-3-Clause
// copyright-holders:Paul Leaman
#include "emu.h"
#include "includes/blktiger.h"


#define BGRAM_BANK_SIZE 0x1000
#define BGRAM_BANKS 4


/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

TILEMAP_MAPPER_MEMBER(blktiger_state::bg8x4_scan)
{
	/* logical (col,row) -> memory offset */
	return (col & 0x0f) + ((row & 0x0f) << 4) + ((col & 0x70) << 4) + ((row & 0x30) << 7);
}

TILEMAP_MAPPER_MEMBER(blktiger_state::bg4x8_scan)
{
	/* logical (col,row) -> memory offset */
	return (col & 0x0f) + ((row & 0x0f) << 4) + ((col & 0x30) << 4) + ((row & 0x70) << 6);
}

TILE_GET_INFO_MEMBER(blktiger_state::get_bg_tile_info)
{
	/* the tile priority table is a guess compiled by looking at the game. It
	   was not derived from a PROM so it could be wrong. */
	static const uint8_t split_table[16] =
	{
		3,3,2,2,
		1,1,0,0,
		0,0,0,0,
		0,0,0,0
	};
	uint8_t attr = m_scroll_ram[2 * tile_index + 1];
	int color = (attr & 0x78) >> 3;
	tileinfo.set(1,
			m_scroll_ram[2 * tile_index] + ((attr & 0x07) << 8),
			color,
			(attr & 0x80) ? TILE_FLIPX : 0);
	tileinfo.group = split_table[color];
}

TILE_GET_INFO_MEMBER(blktiger_state::get_tx_tile_info)
{
	uint8_t attr = m_txvideoram[tile_index + 0x400];
	tileinfo.set(0,
			m_txvideoram[tile_index] + ((attr & 0xe0) << 3),
			attr & 0x1f,
			0);
}


/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

void blktiger_state::video_start()
{
	m_chon = 1;
	m_bgon = 1;
	m_objon = 1;
	m_screen_layout = 0;

	m_scroll_ram = std::make_unique<uint8_t[]>(BGRAM_BANK_SIZE * BGRAM_BANKS);

	m_tx_tilemap =    &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(blktiger_state::get_tx_tile_info)), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);
	m_bg_tilemap8x4 = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(blktiger_state::get_bg_tile_info)), tilemap_mapper_delegate(*this, FUNC(blktiger_state::bg8x4_scan)), 16, 16, 128, 64);
	m_bg_tilemap4x8 = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(blktiger_state::get_bg_tile_info)), tilemap_mapper_delegate(*this, FUNC(blktiger_state::bg4x8_scan)), 16, 16, 64, 128);

	m_tx_tilemap->set_transparent_pen(3);

	m_bg_tilemap8x4->set_transmask(0, 0xffff, 0x8000);  /* split type 0 is totally transparent in front half */
	m_bg_tilemap8x4->set_transmask(1, 0xfff0, 0x800f);  /* split type 1 has pens 4-15 transparent in front half */
	m_bg_tilemap8x4->set_transmask(2, 0xff00, 0x80ff);  /* split type 1 has pens 8-15 transparent in front half */
	m_bg_tilemap8x4->set_transmask(3, 0xf000, 0x8fff);  /* split type 1 has pens 12-15 transparent in front half */
	m_bg_tilemap4x8->set_transmask(0, 0xffff, 0x8000);
	m_bg_tilemap4x8->set_transmask(1, 0xfff0, 0x800f);
	m_bg_tilemap4x8->set_transmask(2, 0xff00, 0x80ff);
	m_bg_tilemap4x8->set_transmask(3, 0xf000, 0x8fff);

	m_tx_tilemap->set_scrolldx(128, 128);
	m_tx_tilemap->set_scrolldy(  6,   6);
	m_bg_tilemap8x4->set_scrolldx(128, 128);
	m_bg_tilemap8x4->set_scrolldy(  6,   6);
	m_bg_tilemap4x8->set_scrolldx(128, 128);
	m_bg_tilemap4x8->set_scrolldy(  6,   6);

	save_pointer(NAME(m_scroll_ram), BGRAM_BANK_SIZE * BGRAM_BANKS);
}



/***************************************************************************

  Memory handlers

***************************************************************************/

void blktiger_state::blktiger_txvideoram_w(offs_t offset, uint8_t data)
{
	m_txvideoram[offset] = data;
	m_tx_tilemap->mark_tile_dirty(offset & 0x3ff);
}

uint8_t blktiger_state::blktiger_bgvideoram_r(offs_t offset)
{
	return m_scroll_ram[offset + m_scroll_bank];
}

void blktiger_state::blktiger_bgvideoram_w(offs_t offset, uint8_t data)
{
	offset += m_scroll_bank;

	m_scroll_ram[offset] = data;
	m_bg_tilemap8x4->mark_tile_dirty(offset / 2);
	m_bg_tilemap4x8->mark_tile_dirty(offset / 2);
}

void blktiger_state::blktiger_bgvideoram_bank_w(uint8_t data)
{
	m_scroll_bank = (data % BGRAM_BANKS) * BGRAM_BANK_SIZE;
}


void blktiger_state::blktiger_scrolly_w(offs_t offset, uint8_t data)
{
	m_scroll_y[offset] = data;
	int scrolly = m_scroll_y[0] | (m_scroll_y[1] << 8);
	m_bg_tilemap8x4->set_scrolly(0, scrolly);
	m_bg_tilemap4x8->set_scrolly(0, scrolly);
}

void blktiger_state::blktiger_scrollx_w(offs_t offset, uint8_t data)
{
	m_scroll_x[offset] = data;
	int scrollx = m_scroll_x[0] | (m_scroll_x[1] << 8);
	m_bg_tilemap8x4->set_scrollx(0, scrollx);
	m_bg_tilemap4x8->set_scrollx(0, scrollx);
}


void blktiger_state::blktiger_video_control_w(uint8_t data)
{
	/* bits 0 and 1 are coin counters */
	machine().bookkeeping().coin_counter_w(0,data & 1);
	machine().bookkeeping().coin_counter_w(1,data & 2);

	/* bit 5 resets the sound CPU */
	m_audiocpu->set_input_line(INPUT_LINE_RESET, (data & 0x20) ? ASSERT_LINE : CLEAR_LINE);

	/* bit 6 flips screen */
	flip_screen_set(data & 0x40);

	/* bit 7 enables characters? Just a guess */
	m_chon = ~data & 0x80;
}

void blktiger_state::blktiger_video_enable_w(uint8_t data)
{
	/* not sure which is which, but I think that bit 1 and 2 enable background and sprites */
	/* bit 1 enables bg ? */
	m_bgon = ~data & 0x02;

	/* bit 2 enables sprites ? */
	m_objon = ~data & 0x04;
}

void blktiger_state::blktiger_screen_layout_w(uint8_t data)
{
	m_screen_layout = data;
	m_bg_tilemap8x4->enable(m_screen_layout);
	m_bg_tilemap4x8->enable(!m_screen_layout);
}



/***************************************************************************

  Display refresh

***************************************************************************/

void blktiger_state::draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	uint8_t *buffered_spriteram = m_spriteram->buffer();
	int offs;

	/* Draw the sprites. */
	for (offs = m_spriteram->bytes() - 4;offs >= 0;offs -= 4)
	{
		int attr = buffered_spriteram[offs+1];
		int sx = buffered_spriteram[offs + 3] - ((attr & 0x10) << 4);
		int sy = buffered_spriteram[offs + 2];
		int code = buffered_spriteram[offs] | ((attr & 0xe0) << 3);
		int color = attr & 0x07;
		int flipx = attr & 0x08;

		if (flip_screen())
		{
			sx = 240 - sx;
			sy = 240 - sy;
			flipx = !flipx;
		}

		m_gfxdecode->gfx(2)->transpen(bitmap,cliprect,
				code,
				color,
				flipx,flip_screen(),
				sx+128,sy+6,15);
	}
}

uint32_t blktiger_state::screen_update_blktiger(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(1023, cliprect);

	if (m_bgon)
		(m_screen_layout ? m_bg_tilemap8x4 : m_bg_tilemap4x8)->draw(screen, bitmap, cliprect, TILEMAP_DRAW_LAYER1, 0);

	if (m_objon)
		draw_sprites(bitmap, cliprect);

	if (m_bgon)
		(m_screen_layout ? m_bg_tilemap8x4 : m_bg_tilemap4x8)->draw(screen, bitmap, cliprect, TILEMAP_DRAW_LAYER0, 0);

	if (m_chon)
		m_tx_tilemap->draw(screen, bitmap, cliprect, 0, 0);

	return 0;
}