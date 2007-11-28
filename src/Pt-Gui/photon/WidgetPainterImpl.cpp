/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                  *
 *   Copyright (C) 2005-2007 by Aloysius Indrayanto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "WidgetPainterImpl.h"
#include "PixmapImpl.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"
#include <iostream>

namespace Pt {

namespace Gui {

WidgetPainterImpl::WidgetPainterImpl( )
: _dc(0)
{
	PhDrawContext_t* old = PhDCSetCurrent(0);
	_dc = PhDCSetCurrent(old);
}


WidgetPainterImpl::~WidgetPainterImpl()
{
}


void WidgetPainterImpl::begin()
{
}


void WidgetPainterImpl::end()
{
}


void WidgetPainterImpl::drawLine(const Math::Point& from, const Math::Point& to)
{
	PgSetRegionCx( _dc, _rid );
	PhGC_t* old = PgSetGCCx(_dc, _gc);

    PgDrawILineCx( _dc, from.x(), from.y(), to.x(),to.y() );
	PgFlush();

	PgSetGCCx(_dc, old);
}


void WidgetPainterImpl::drawRect(const Gfx::Rect& rect)
{
	PgSetRegionCx( _dc, _rid );
	PhGC_t* old = PgSetGCCx(_dc, _gc);

	PgDrawIRectCx( _dc, rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height(), Pg_DRAW_STROKE);
	PgFlush();

	PgSetGCCx(_dc, old);
}


void WidgetPainterImpl::fillRect(const Gfx::Rect& rect)
{
	//std::cerr << "WP :: fillRect: " << rect.x() << " " << rect.y() << " " << rect.width() << " " << rect.height() << std::endl;

	PgSetRegionCx( _dc, _rid );
	PhGC_t* old = PgSetGCCx(_dc, _gc);

	PgDrawIRect(rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height(), Pg_DRAW_FILL);
	PgFlush();

	PgSetGCCx(_dc, old);
}


void WidgetPainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm)
{
	PgSetRegionCx( _dc, _rid );
	PhGC_t* old = PgSetGCCx(_dc, _gc);

	PhPoint_t _to = { to.x(), to.y() };
	PgDrawPhImageCx(_dc, &_to, pm.impl().image(), 0 );
	PgFlush();

	PgSetGCCx(_dc, old);
}


void WidgetPainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& region)
{
	PgSetRegionCx( _dc, _rid );
	PhGC_t* old = PgSetGCCx(_dc, _gc);
	//std::cerr << "drawPixmap: To: " << to.x() << ", " << to.y()  << "   From: " << region.x() << ", " << region.y() << ", " <<  region.width() << ", " << region.height()<< std::endl;
	PhPoint_t _to = { to.x(), to.y() };
	PhRect_t rect = { region.x(), region.y(), region.x() + region.width(), region.y() + region.height() };
	//PhDim_t dim = {region.width(), region.height() };

	PgDrawPhImageRectCxv(_dc, &_to, pm.impl().image(), &rect, NULL );
	PgFlush();

	PgSetGCCx(_dc, old);
}

} // namespace Gui

} // namespace Pt

