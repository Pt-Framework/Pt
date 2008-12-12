/*
 * Copyright (C) 2006 Tobias Mueller
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

#include "Pt/Math/Point.h"
#include "Pt/Math/Size.h"
#include "Pt/Gui/ImageButton.h"
#include <Pt/Gui/Painter.h>
#include "Pt/Gui/CloseEvent.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/MouseMoveEvent.h"
#include "Pt/Gui/MoveEvent.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/Painter.h"
#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/KeyEvent.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/Brush.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/FontMetrics.h"

#include <iostream>
#include <algorithm>

using namespace std;


namespace Pt {
namespace Gui {


ImageButton::ImageButton(Widget& parent, const  Pt::Math::Point& at, const  Pt::Math::Size& size,
                         const Pt::Gfx::ARgbImage* normalState, const Pt::Gfx::ARgbImage* pressedState,
                         const Pt::Gfx::ARgbImage* disabledState)
: Widget(parent, at, size)
, _pressed(false)
, _backbuffer(new Pixmap(size.width(), size.height()))
, _normalStateImage(normalState)
, _pressedStateImage(pressedState)
, _disabledStateImage(disabledState)
{
    if (_normalStateImage == 0)
    {
        // TODO Throw exception.
    }
    
    setForegroundColor(Gfx::ARgbColor(0, 0, 0));
    setBackgroundColor(Gfx::ARgbColor(0xffff, 0xffff, 0xffff));
}


ImageButton::~ImageButton()
{
}

void ImageButton::update()
{
    Painter widgetPainter     = painter();
    Painter backbufferPainter = _backbuffer->painter();

    Pt::ssize_t offset = 0;
    const Pt::Gfx::ARgbImage* imageToDraw = _normalStateImage;

    if (!isEnabled() && _disabledStateImage != 0)
    {
        imageToDraw = _disabledStateImage;
    }
    else if (_pressed)
    {
        if (_pressedStateImage != 0)
        {
            imageToDraw = _pressedStateImage;
        }
        else
        {
            offset = 1;
        }
    }
    
    drawBackground(widgetPainter,     imageToDraw, offset);
    drawBackground(backbufferPainter, imageToDraw, offset);
}


void ImageButton::drawBackground(Painter& painter, const Pt::Gfx::ARgbImage* image, const Pt::ssize_t offset)
{
    painter.setBrush(Gfx::Brush(backgroundColor()));
    painter.fillRect(Gfx::Rect(Math::Point(0, 0), this->size()));

    ssize_t x = ((ssize_t)this->size().width()  - (ssize_t)image->width())  / 2;
    ssize_t y = ((ssize_t)this->size().height() - (ssize_t)image->height()) / 2;
    painter.drawImage(Math::Point(x + offset, y + offset), *image);
}



Math::Size ImageButton::minimumSize()
{
    return Math::Size(_normalStateImage->width() + insets().left() + insets().right(),
                      _normalStateImage->height() + insets().top() + insets().bottom());
}


Math::Size ImageButton::preferredSize()
{
    return Math::Size(_normalStateImage->width() + insets().left() + insets().right(),
                      _normalStateImage->height() + insets().top() + insets().bottom());
}


void ImageButton::_resizeEvent(const ResizeEvent& event)
{
    _backbuffer.reset( new Pixmap( event.width(), event.height() ) );

    Painter backbufferPainter = _backbuffer->painter();
    this->update();
}


void ImageButton::_paintEvent(const PaintEvent& event)
{
    painter().drawPixmap( event.origin(), *_backbuffer, event.region() );
}


void ImageButton::_mouseEvent(const MouseEvent& event)
{
    if (event.action() == MouseEvent::Press && event.button() == MouseEvent::LeftButton )
    {
        _pressed = true;
        this->update();
    }
    else if (event.action() == MouseEvent::Release && event.button() == MouseEvent::LeftButton && _pressed)
    {
        _pressed = false;
        this->update();
        clicked.send();
    }
}

void ImageButton::_mouseMoveEvent(const MouseMoveEvent& event)
{
    if (_pressed && event.action() == MouseMoveEvent::Exited) {
        _pressed = false;
        this->update();
    }
}


} // namespace Gui

} // namespace Pt
