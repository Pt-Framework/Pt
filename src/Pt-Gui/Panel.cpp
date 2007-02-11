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

#include "Pt/Gfx/Brush.h"
#include "Pt/Gui/Panel.h"
#include "Pt/Math/Point.h"
#include "Pt/Math/Size.h"
#include "Pt/Math/Rect.h"
#include "Pt/Gui/Painter.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/ResizeEvent.h"
#include "Pt/Gui/LayoutManager.h"

using namespace Pt::Gfx;
using namespace std;


namespace Pt {

namespace Gui {


Panel::Panel(Widget& parent, const Math::Point& at, const Math::Size& size)
: Widget(parent, at, size)
{
}


Panel::~Panel()
{
}


void Panel::update()
{
    Brush brush(backgroundColor());

    Painter p = painter();
    p.setBrush(brush);
    p.fillRect(Math::Rect(Math::Point(0, 0), size()));
}


Math::Size Panel::minimumSize()
{
    return layout().minimumSize();
}


Math::Size Panel::preferredSize()
{
    return layout().preferredSize();
}


void Panel::_resizeEvent(const ResizeEvent& event)
{
    this->update();
}


void Panel::_paintEvent(const PaintEvent& event)
{
    this->update();
}


} // namespace Gui

} // namespace Pt
