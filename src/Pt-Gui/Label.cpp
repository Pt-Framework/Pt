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

#include "Pt/Api.h"

#include "Pt/Gfx/Brush.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Math/Point.h"
#include "Pt/Math/Size.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gui/Label.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/Painter.h"
#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/Pixmap.h"

#include <iostream>

using namespace Pt::Gfx;
using namespace std;


namespace Pt {

namespace Gui {


Label::Label(Widget& parent, const Math::Point& at, const Math::Size& size, const Pt::String& text)
: Widget(parent, at, size)
, _backbuffer(new Pixmap(size.width(), size.height()))
, _text(text)
{
}


Label::~Label()
{
}


void Label::setText(const Pt::String& text)
{
    _text = text;
    this->update();
}


const Pt::String& Label::text() const
{
    return _text;
}


void Label::update()
{
    Painter widgetPainter     = painter();
    Painter backbufferPainter = _backbuffer->painter();

    Brush brush(backgroundColor());

    widgetPainter.setBrush(brush);
    widgetPainter.fillRect(Gfx::Rect(Math::Point(0, 0), size()));

    backbufferPainter.setBrush(brush);
    backbufferPainter.fillRect(Gfx::Rect(Math::Point(0, 0), size()));

    if( !_text.empty() ) {
        Pen pen(1, foregroundColor());
        Font font("Tahoma");     // TODO Font name

        widgetPainter.setPen(pen);
        backbufferPainter.setPen(pen);

        widgetPainter.setFont(font);
        backbufferPainter.setFont(font);

        widgetPainter.drawText( Math::Point(0, widgetPainter.fontMetrics().ascent()), _text.c_str() );
        backbufferPainter.drawText( Math::Point(0, widgetPainter.fontMetrics().ascent()), _text.c_str() );
    }
}


Math::Size Label::minimumSize()
{
    return Math::Size(0, 0);
}


Math::Size Label::preferredSize()
{
    FontMetrics metrics = painter().fontMetrics(_text);
    return Math::Size(metrics.width(), metrics.height());
}


void Label::_resizeEvent(const ResizeEvent& event)
{
    _backbuffer.reset(new Pixmap(event.width(), event.height()));

    this->update();
}


void Label::_paintEvent(const PaintEvent& event)
{
    painter().drawPixmap( event.origin(), *_backbuffer, event.region() );
}


void Label::_mouseEvent(const MouseEvent& event)
{
    if( event.button() == MouseEvent::LeftButton )
    {
        clicked.send();
    }
}


} // namespace Gui

} // namespace Pt
