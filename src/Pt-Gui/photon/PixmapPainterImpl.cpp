/*
 * Copyright (C) 2006 Marc Boris Duerner
 * Copyright (C) 2005-2007 by Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "PixmapPainterImpl.h"
#include "PixmapImpl.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"

namespace Pt {

namespace Gui {

PixmapPainterImpl::PixmapPainterImpl( )
: _pixmap(0)
{
}


PixmapPainterImpl::~PixmapPainterImpl()
{
}


void PixmapPainterImpl::set(PixmapImpl& pixmap)
{
	_pixmap = &pixmap;
}


void PixmapPainterImpl::begin()
{
}


void PixmapPainterImpl::end()
{
}


void PixmapPainterImpl::drawLine(const Gfx::Point& from, const Gfx::Point& to)
{
	//PmMemStart( _pixmap->mc() );

	PhGC_t* old = PgSetGCCx( _pixmap->mc(), _gc );
    PgDrawILineCx( _pixmap->mc(), from.x(), from.y(), to.x(),to.y() );

	PgSetGCCx( _pixmap->mc(), old);
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
	//PmMemStop( _pixmap->mc() );
}


void PixmapPainterImpl::drawRect(const Gfx::Rect& rect)
{
	PhGC_t* old = PgSetGCCx( _pixmap->mc(), _gc );
	PgDrawIRectCx( _pixmap->mc(), rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height(), Pg_DRAW_STROKE);

	PgSetGCCx( _pixmap->mc(), old);
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
}


void PixmapPainterImpl::fillRect(const Gfx::Rect& rect)
{
	PhGC_t* old = PgSetGCCx( _pixmap->mc(), _gc );
	PgDrawIRectCx(_pixmap->mc(), rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height(), Pg_DRAW_FILL);

	PgSetGCCx( _pixmap->mc(), old);
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
}


void PixmapPainterImpl::drawPixmap(const Gfx::Point& to, Pixmap& pm)
{
	PhGC_t* old = PgSetGCCx( _pixmap->mc(), _gc );

	PhPoint_t _to = { to.x(), to.y() };
	PgDrawPhImageCx(_pixmap->mc(), &_to, pm.impl().image(), 0 );

	PgSetGCCx( _pixmap->mc(), old);
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
}


void PixmapPainterImpl::drawPixmap(const Gfx::Point& to, Pixmap& pm, const Gfx::Region& region)
{
	PhGC_t* old = PgSetGCCx( _pixmap->mc(), _gc );

	PhPoint_t _to = { to.x(), to.y() };
	PhRect_t rect = { region.x(), region.y(), region.x() + region.width(), region.y() + region.height() };
	PgDrawPhImageRectCxv(_pixmap->mc(), &_to, pm.impl().image(), &rect, NULL );

	PgSetGCCx( _pixmap->mc(), old);
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
}

} // namespace Gui

} // namespace Pt

