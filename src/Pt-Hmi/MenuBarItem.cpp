/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/
#include <Pt/Hmi/MenuBarItem.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/MenuBar.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/StyleOptions.h>

namespace Pt {
namespace Hmi {

MenuBarItem::MenuBarItem()
{            
}


MenuBarItem::~MenuBarItem()
{
}



Pt::Gfx::SizeF MenuBarItem::onMeasure(const  Pt::Hmi::SizePolicy& policy)
{
    Pt::Gfx::Painter _painter(surface());
    _painter.setFont(_font);

    Pt::Gfx::FontMetrics fm = _painter.fontMetrics(_text);

    return Pt::Gfx::SizeF(fm.width() + padding().leftRight(),
        fm.height() + padding().topBottom());
}


void MenuBarItem::onPaint(Pt::Gfx::PaintSurface& surface, const Pt::Gfx::RectF& rect)
{    
    MenuBaseItem::onPaint(surface,rect);
    
    const  Pt::Hmi::StyleOptions& options = Pt::Hmi::Application::instance().styleOptions();


    Pt::Gfx::Painter painter(surface);
    painter.setClip(rect);

    if (isHighlighted())
    {
        painter.setBrush(_brush);
        painter.fillRect(Pt::Gfx::RectF(Pt::Gfx::PointF(0, 0), this->size()));
    }

    if (isMenuOpen())
    {
        painter.setPen(contour());
        const double x1 = 0;
        const double x2 = this->size().width() - 1;
        const double y1 = 0;
        const double y2 = this->size().height();

        painter.drawLine(Pt::Gfx::PointF(x1, y1), Pt::Gfx::PointF(x2, y1));

        painter.drawLine(Pt::Gfx::PointF(x1, y1), Pt::Gfx::PointF(x1, y2));
        painter.drawLine(Pt::Gfx::PointF(x2, y1), Pt::Gfx::PointF(x2, y2));
    }

    // item text    
    painter.setFont(_font);
    Pt::Gfx::FontMetrics fm = painter.fontMetrics(_text);
    double textX = padding().left();
    double textY = (size().height() - fm.height()) / 2;
    textY += fm.ascent();
    Pt::Gfx::PointF textPos(textX, textY);

    Pt::Gfx::RectF mnemonicRect; // TODO

    painter.setFont(_font);
    painter.setPen(_textPen);
    painter.drawText(textPos, _text);

    if (!mnemonicRect.isNull())
    {
        double menmonicY = textPos.y() + 1;
        painter.drawLine(Pt::Gfx::PointF(mnemonicRect.left(), menmonicY),
            Pt::Gfx::PointF(mnemonicRect.right(), menmonicY));
    }
}

}}
