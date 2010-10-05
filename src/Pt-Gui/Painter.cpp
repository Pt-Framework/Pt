/*
 * Copyright (C) 2006 Marc Boris D�rner
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
// The symbian Impl tracks its own activation state
// give it a chance to clean up internal context
#ifdef __SYMBIAN32__
    _painterImpl->cleanUp();
#endif
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


void Painter::drawPixel(const Gfx::Point& to)
{
    this->begin();
    _painterImpl->drawPixel(to);
}


void Painter::drawLine(const Gfx::Point& from, const Gfx::Point& to)
{
    this->begin();
    _painterImpl->drawLine(from, to);
}

void Painter::drawText( const Gfx::Point& to, const Pt::String& text, const Gfx::ARgbColor* outline )
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


void Painter::drawEllipse(const Gfx::Point& topLeft, const Gfx::Size& size)
{
    this->begin();
    _painterImpl->drawEllipse(topLeft, size);
}


void Painter::fillEllipse(const Gfx::Point& topLeft, const Gfx::Size& size)
{
    this->begin();
    _painterImpl->fillEllipse(topLeft, size);
}


void Painter::drawPolyline(const Gfx::Point* points, const size_t pointCount)
{
    this->begin();
    _painterImpl->drawPolyline(points, pointCount);
}


void Painter::fillPolygon(const Gfx::Point* points, const size_t pointCount)
{
    this->begin();
    _painterImpl->fillPolygon(points, pointCount);
}


void Painter::drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image)
{
    if (image.empty()) {
        return;  // Don't try to draw empty images.
    }

    this->begin();
    _painterImpl->drawImage(to, image);
}


void Painter::drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image, const Gfx::Region& imageRect)
{
    if (image.empty()) {
        return;  // Don't try to draw empty images.
    }

    this->begin();
    _painterImpl->drawImage(to, image, imageRect);
}


void Painter::drawPixmap(const Gfx::Point& to, Pixmap& from, const Gfx::Region& fromRect)
{
    if (from.size().width() == 0 || from.size().height() == 0) {
        return;  // Don't try to draw empty pixmaps.
    }

    this->begin();
    _painterImpl->drawPixmap(to, from, fromRect);
}


void Painter::drawPixmap(const Gfx::Point& to, Pixmap& pm)
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

