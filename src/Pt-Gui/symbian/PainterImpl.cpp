/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2008 Peter Barth                                        *
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
#include "PainterImpl.h"
#include "PixmapImpl.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gfx/FontMetrics.h"
#include <iostream>

namespace Pt {

namespace Gui {

PainterImpl::PainterImpl()
: _font("sans-serif")
{
}


PainterImpl::~PainterImpl()
{
}


void PainterImpl::begin()
{   
}


void PainterImpl::end()
{
    std::vector<Paint*>::iterator it;
    for(it = _paintQueue.begin(); it != _paintQueue.end(); ++it)
    {
        (*it)->paint();
        delete *it;
    }

    _paintQueue.clear();
}


void PainterImpl::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
}


const Gfx::Pen& PainterImpl::pen() const
{
    return _pen;
}


void PainterImpl::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


const Gfx::Brush& PainterImpl::brush() const
{
    return _brush;
}


const Gfx::Font& PainterImpl::font() const
{
    return _font;
}


void PainterImpl::setFont(const Gfx::Font& font)
{
    _font = font;
}


Gfx::FontMetrics PainterImpl::fontMetrics() const
{
    return Gfx::FontMetrics(0, 0, 0, 0);
}


Gfx::FontMetrics PainterImpl::fontMetrics(const Pt::String& text) const
{
    return Gfx::FontMetrics(0, 0, 0, 0);
}


const std::list<std::string>& PainterImpl::fontFamilyNames()
{
    static const std::list<std::string> _fontList;
    return _fontList;
}


void PainterImpl::drawPixel(const Math::Point& to)
{

}


void PainterImpl::drawLine(const Math::Point& from, const Math::Point& to)
{
    _paintQueue.push_back( new DrawLine(from, to, _pen) );
}


void PainterImpl::drawRect(const Gfx::Rect& rect)
{
    _paintQueue.push_back( new DrawRect(rect, _pen) );
}


void PainterImpl::drawText(const Math::Point& to, const Pt::String& text)
{

}


void PainterImpl::drawPolyline(const Math::Point* points, const size_t pointCount)
{

}


void PainterImpl::drawEllipse(const Math::Point& topLeft, const Math::Size& size)
{

}


void PainterImpl::fillRect(const Gfx::Rect& rect)
{
    _paintQueue.push_back(new FillRect(rect, _brush) );
}


void PainterImpl::fillEllipse(const Math::Point& topLeft, const Math::Size& size)
{

}


void PainterImpl::fillPolygon(const Math::Point* points, const size_t pointCount)
{

}


void PainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm)
{
    Pt::Gfx::Region region( Pt::Math::Point(0, 0), pm.size() );
    _paintQueue.push_back(new DrawPixmap(to, pm, region) );
}


void PainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& pmRegion)
{
    _paintQueue.push_back(new DrawPixmap(to, pm, pmRegion) );
}


void PainterImpl::drawImage(const Math::Point& to, const Gfx::ARgbImage& image)
{
    this->drawImage( to.x(), to.y(), image.begin(), image.end(), image.width(), image.height() );
}


void PainterImpl::drawImage(const Math::Point& to, const Gfx::ARgbImage& image,
                            const Gfx::Region& imageRegion)
{

}


void PainterImpl::copyImageData(ssize_t toX, ssize_t toY, const char* data, size_t fromWidth, size_t fromHeight)
{

}


PainterImpl::DrawLine::DrawLine(const Math::Point& from, const Math::Point& to, const Gfx::Pen& pen)
: _from(from)
, _to(to)
, _pen(pen)
{
}


PainterImpl::DrawLine::~DrawLine()
{
}


void PainterImpl::DrawLine::paint()
{
}


PainterImpl::DrawRect::DrawRect(const Gfx::Rect& rect, const Gfx::Pen& pen)
: _rect(rect)
, _pen(pen)
{
}


PainterImpl::DrawRect::~DrawRect()
{

}


void PainterImpl::DrawRect::paint()
{
}


PainterImpl::DrawPixmap::DrawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& region)
: _to(to)
, _region(region)
{
}


PainterImpl::DrawPixmap::~DrawPixmap()
{
}


void PainterImpl::DrawPixmap::paint()
{ 
}


PainterImpl::FillRect::FillRect(const Gfx::Rect& rect, const Gfx::Brush& brush)
: _rect(rect)
, _brush(brush)
{

}

PainterImpl::FillRect::~FillRect()
{

}


void PainterImpl::FillRect::paint()
{
}

} // namespace Gui

} // namespace Pt


