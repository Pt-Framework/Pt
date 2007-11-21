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
#include "PixmapPainterImpl.h"
#include "PixmapImpl.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"

namespace Pt {

namespace Gui {

PixmapPainterImpl::PixmapPainterImpl( )
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


void PixmapPainterImpl::drawLine(const Math::Point& from, const Math::Point& to)
{
	PmMemStart( _pixmap->mc() );
	PhGC_t* old = PgSetGCCx( _pixmap->mc(), _gc );

    PgDrawILineCx( _pixmap->mc(), from.x(), from.y(), to.x(),to.y() );
	PgFlush();
	PgSetGCCx( _pixmap->mc(), old);
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
	PmMemStop( _pixmap->mc() );
}


void PixmapPainterImpl::drawRect(const Gfx::Rect& rect)
{
	PmMemStart( _pixmap->mc() );
	PhGC_t* old = PgSetGCCx( _pixmap->mc(), _gc );
	PgDrawIRectCx( _pixmap->mc(), rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height(), Pg_DRAW_STROKE);
	
	PgFlush();
	PgSetGCCx( _pixmap->mc(), old);
	
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
	PmMemStop( _pixmap->mc() );
}


void PixmapPainterImpl::fillRect(const Gfx::Rect& rect)
{
	std::cerr << "PP :: fillRect: " << rect.x() << " " << rect.y() << " " << rect.width() << " " << rect.height() << std::endl;

	PmMemStart( _pixmap->mc() );
	PhGC_t* old = PgSetGCCx( _pixmap->mc(), _gc );
	PgDrawIRectCx(_pixmap->mc(), rect.x(), rect.y(), rect.x()+rect.width(), rect.y()+rect.height(), Pg_DRAW_FILL);
	
	PgFlush();
	PgSetGCCx( _pixmap->mc(), old);
	
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
	PmMemStop( _pixmap->mc() );
}


void PixmapPainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm)
{
	PmMemStart( _pixmap->mc() );
	PhGC_t* old = PgSetGCCx( _pixmap->mc(), _gc );
	
	PhPoint_t _to = { to.x(), to.y() };
	PgDrawPhImageCx(_pixmap->mc(), &_to, pm.impl().image(), 0 );
	
	PgFlush();
	PgSetGCCx( _pixmap->mc(), old);
	
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
	PmMemStop( _pixmap->mc() );
}


void PixmapPainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& region)
{
	PmMemStart( _pixmap->mc() );
	PhGC_t* old = PgSetGCCx( _pixmap->mc(), _gc );

	PhPoint_t _to = { to.x(), to.y() };
	PhRect_t rect = { region.x(), region.y(), region.x() + region.width(), region.y() + region.height() };
	//PhDim_t dim = {region.width(), region.height() };

	PgDrawPhImageRectCxv(_pixmap->mc(), &_to, pm.impl().image(), &rect, NULL );
	PgFlush();
	PgSetGCCx( _pixmap->mc(), old);
	
	PmMemFlush( _pixmap->mc(), _pixmap->image() );
	PmMemStop( _pixmap->mc() );
}

} // namespace Gui

} // namespace Pt

