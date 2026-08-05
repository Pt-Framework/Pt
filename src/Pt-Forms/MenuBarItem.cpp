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
#include <Pt/Forms/MenuBarItem.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/MenuBar.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>

namespace Pt {
namespace Forms {

MenuBarItem::MenuBarItem()
{
}


MenuBarItem::~MenuBarItem()
{
}



Pt::Gfx::SizeF MenuBarItem::onMeasure(const  Pt::Forms::SizePolicy& policy)
{
    Painter _painter(surface());
    _painter.setFont(_font);

    Pt::Gfx::TextMetrics fm = _painter.textMetrics(_text);
    Pt::Gfx::FontMetrics fontMet = _painter.fontMetrics();

    return Pt::Gfx::SizeF(fm.advance() + padding().leftRight(),
        fontMet.height() + padding().topBottom());
}


void MenuBarItem::onPaint(PaintContext& context, const Pt::Gfx::RectF& rect)
{
    MenuItemBase::onPaint(context,rect);

    const  Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();


    Painter painter(context);
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
    Pt::Gfx::TextMetrics fm = painter.textMetrics(_text);
    Pt::Gfx::FontMetrics fontMet = painter.fontMetrics();
    double textX = padding().left();
    double textY = (size().height() - fontMet.height()) / 2;
    textY += fontMet.ascent();
    Pt::Gfx::PointF textPos(textX, textY);

    Pt::Gfx::RectF mnemonicRect; // TODO

    painter.setFont(_font);
    painter.setPen(_textPen);
    painter.drawText(textPos, _text);

    if (!mnemonicRect.isEmpty())
    {
        double menmonicY = textPos.y() + 1;
        painter.drawLine(Pt::Gfx::PointF(mnemonicRect.left(), menmonicY),
            Pt::Gfx::PointF(mnemonicRect.right(), menmonicY));
    }
}

}}
