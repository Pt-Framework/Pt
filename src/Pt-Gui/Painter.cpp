/***************************************************************************
 *   Copyright (C) 2006 Marc Boris D�rner                                  *
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
#include "Pt/Gui/Painter.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/FontMetrics.h"

namespace Pt {

namespace Gui {

Painter::Painter(PainterImpl* painterImpl)
: _active(false)
{
    _painterImpl = painterImpl;
}


Painter::~Painter()
{
    if (_active) {
        _painterImpl->end();
    }
}


void Painter::setPen(const Gfx::Pen& pen)
{
    _painterImpl->setPen(pen);
}


const Gfx::Pen& Painter::pen() const
{
    return _painterImpl->pen();
}


void Painter::setBrush(const Gfx::Brush& brush)
{
    _painterImpl->setBrush(brush);
}


const Gfx::Brush& Painter::brush() const
{
    return _painterImpl->brush();
}


void Painter::setFont(const Gfx::Font& font)
{
    _painterImpl->setFont(font);
}


const Gfx::Font& Painter::font() const
{
    return _painterImpl->font();
}


Gfx::FontMetrics Painter::fontMetrics() const
{
    this->begin();
    return _painterImpl->fontMetrics();
}


Gfx::FontMetrics Painter::fontMetrics(Pt::String text) const
{
    this->begin();
    return _painterImpl->fontMetrics(text);
}


const std::list<std::string>& Painter::fontFamilyNames()
{
    this->begin();
    return _painterImpl->fontFamilyNames();
}


void Painter::drawPixel(const Math::Point& to)
{
    this->begin();
    _painterImpl->drawPixel(to);
}


void Painter::drawLine(const Math::Point& from, const Math::Point& to)
{
    this->begin();
    _painterImpl->drawLine(from, to);
}

void Painter::drawText( const Math::Point& to, const Pt::String& text, const Gfx::ARgbColor* outline )
{
    this->begin();
    _painterImpl->drawText(to, text);

}

void Painter::drawRect(const Gfx::Rect& rect)
{
    this->begin();
    _painterImpl->drawRect(rect);
}


void Painter::fillRect(const Gfx::Rect& rect)
{
    this->begin();
    _painterImpl->fillRect(rect);
}


void Painter::drawEllipse(const Math::Point& topLeft, const Math::Size& size)
{
    this->begin();
    _painterImpl->drawEllipse(topLeft, size);
}


void Painter::fillEllipse(const Math::Point& topLeft, const Math::Size& size)
{
    this->begin();
    _painterImpl->fillEllipse(topLeft, size);
}


void Painter::drawPolyline(const Math::Point* points, const size_t pointCount)
{
    this->begin();
    _painterImpl->drawPolyline(points, pointCount);
}


void Painter::fillPolygon(const Math::Point* points, const size_t pointCount)
{
    this->begin();
    _painterImpl->fillPolygon(points, pointCount);
}


void Painter::drawImage(const Math::Point& to, const Gfx::ARgbImage& image)
{
    if (image.empty()) {
        return;  // Don't try to draw empty images.
    }

    this->begin();
    _painterImpl->drawImage(to, image);
}


void Painter::drawImage(const Math::Point& to, const Gfx::ARgbImage& image, const Gfx::Region& imageRect)
{
    if (image.empty()) {
        return;  // Don't try to draw empty images.
    }

    this->begin();
    _painterImpl->drawImage(to, image, imageRect);
}


void Painter::drawPixmap(const Math::Point& to, Pixmap& from, const Gfx::Region& fromRect)
{
    if (from.size().width() == 0 || from.size().height() == 0) {
        return;  // Don't try to draw empty pixmaps.
    }

    this->begin();
    _painterImpl->drawPixmap(to, from, fromRect);
}


void Painter::drawPixmap(const Math::Point& to, Pixmap& pm)
{
    if (pm.size().width() == 0 || pm.size().height() == 0) {
        return;  // Don't try to draw empty pixmaps.
    }

    this->begin();
    _painterImpl->drawPixmap(to, pm);
}


void Painter::begin() const
{
    if (!_active) {
        _painterImpl->begin();
        _active = true;
    }
}


} // namespace Gui

} // namespace Pt

