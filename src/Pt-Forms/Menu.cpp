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
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/MenuBar.h>
#include <Pt/Forms/MenuItem.h>
#include <Pt/Forms/MenuSubItem.h>
#include <Pt/Forms/MenuBarItem.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>
#include <assert.h>

namespace Pt {
namespace Forms {

Menu::Menu()
: _currentItem(0)
, _layout(Direction::Top)
, _iconWidth(0)
{
    setContent(&_layout);
}


Menu::~Menu()
{
}


void Menu::addItem(MenuItem& item)
{
   _layout.addItem(item);

}

void Menu::addItem(MenuSubItem& item)
{
    _layout.addItem(item);

    item.setParentMenu(this);
    item.triggered() += Pt::slot(*this, &Menu::onItemTriggered);
}

void Menu::removeItem(MenuItem& item)
{
    _layout.removeItem(item);
}

void Menu::removeItem(MenuSubItem& item)
{
    item.triggered() -= Pt::slot(*this, &Menu::onItemTriggered);
    item.setParentMenu(0);
    _layout.removeItem(item);
}

void Menu::onItemTriggered(MenuItemBase& item)
{
    MenuSubItem* smi =dynamic_cast<MenuSubItem*>(&item);

    assert(smi!= 0);

    Menu* menu = smi->menu();

    // TODO: open menu on mouse enter and close menu on mouse leave
    //       possibly delayed by a 500ms timer

    if( ! menu->isVisible() )
    {
        Pt::Gfx::PointF topRight(item.size().width(), 0);
        Pt::Gfx::PointF menuPos = item.toGlobal(topRight);
        menu->move(menuPos);

        Pt::Forms::SizePolicy policy(Pt::Forms::SizePolicy::Preferred, Pt::Forms::SizePolicy::Preferred);
        //menu->setAutoSize(policy);
        menu->autoSize(policy);

        menu->setAbove(true);
        menu->show();
    }
    else
    {
        menu->close();
    }
}

Pt::Forms::Widget* Menu::onFindMenu(const Pt::Gfx::PointF& screenPos)
{
    if( ! isVisible() )
        return 0;

    Pt::Gfx::RectF rect( position(), size() );

    if( rect.contains(screenPos) )
        return this;

    if( _currentItem == 0)
        return 0;

    MenuSubItem* smi = dynamic_cast<MenuSubItem*>(_currentItem);

    if (smi == 0)
        return 0;

    if( smi->menu() == 0)
        return 0;

    return smi->menu()->onFindMenu(screenPos);
}

void Menu::onOpenMenu(MenuSubItem& item)
{
    _currentItem = &item;
}

void Menu::onCloseMenu(MenuSubItem& item)
{
    if( _currentItem == &item)
        _currentItem = 0;
}

void Menu::onCancel()
{
    close();
}

double Menu::iconWidth() const
{
    return _iconWidth > 0 ? _iconWidth + _layout.padding().left()
                          : 0;
}

const Pt::Gfx::Brush& Menu::background() const
{
    return _background ? *_background
                       : Pt::Forms::Application::instance().styleOptions().background();
}

void Menu::setBackground(const Pt::Gfx::Brush& b)
{
    _background.reset( new Pt::Gfx::Brush(b) );
    invalidate();
}

const Pt::Gfx::Pen& Menu::contour() const
{
    return _contour ? *_contour
                    : Pt::Forms::Application::instance().styleOptions().contour();
}

void Menu::setContour(const Pt::Gfx::Pen& p)
{
    _contour.reset( new Pt::Gfx::Pen(p) );
    invalidate();
}

void Menu::onInvalidate()
{
    Pt::Forms::Popup::onInvalidate();

    const Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();
    const Pt::Forms::Style& style = Pt::Forms::Application::instance().style();

    _brush = background();
    _pen = contour();
}

void Menu::onPaint(PaintContext& ctx, const Pt::Gfx::RectF& rect)
{
    Pt::Forms::Popup::onPaint(ctx, rect);

    const Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();

    Painter painter( ctx );
    painter.setClip( rect );

    onRenderBackground(options, painter, rect);
}

void Menu::drawBorder(Painter& painter, const Pt::Gfx::RectF& borderRect) const
{
    const MenuItemBase* p = parentItem();

    if (!p)
    {
        painter.drawRect(borderRect);
        return;
    }

    const MenuBarItem * mbi = dynamic_cast<const MenuBarItem*>(p);

    if (mbi == 0)
    {
        painter.drawRect(borderRect);

        return;
    }

    if (!mbi->isMenuOpen())
    {
        painter.drawRect(borderRect);
        return;
    }

    const double firstWidth = p->size().width();
    const double x1 = borderRect.x();
    const double y1 = borderRect.y();

    const double x2 = borderRect.x() + borderRect.width();
    const double y2 = borderRect.y() + borderRect.height();

    painter.drawLine(Pt::Gfx::PointF(x1 + firstWidth, y1), Pt::Gfx::PointF(x2, y1));
    painter.drawLine(Pt::Gfx::PointF(x2, y1), Pt::Gfx::PointF(x2, y2));
    painter.drawLine(Pt::Gfx::PointF(x2, y2), Pt::Gfx::PointF(x1, y2));
    painter.drawLine(Pt::Gfx::PointF(x1, y2), Pt::Gfx::PointF(x1, y1));

}

void Menu::onRenderBackground(const Pt::Forms::StyleOptions& options,
                              Painter& painter, const Pt::Gfx::RectF& rect) const
{
    Pt::Gfx::SizeF size = this->size();

    size -= 1;

    const Gfx::Scaling& scaling = this->scaling();
    double inset = scaling.alignContour(_pen.size()) / 2;

    // icon strip on the left side

    double iconWidth = this->iconWidth();

    if (iconWidth > 0)
    {
        Pt::Gfx::RectF iconStrip(Pt::Gfx::PointF(0, 0),
            Pt::Gfx::SizeF(iconWidth, size.height()));

        Pt::Gfx::Brush brush = Pt::Gfx::Brush::verticalGradient(brush.color(),
            Pt::Gfx::Color(253, 253, 253));

        painter.setBrush(brush);
        painter.fillRect(iconStrip);
    }

    // menu border
    Pt::Gfx::RectF borderRect(this->size());
    borderRect.move(inset, inset);
    borderRect.shrink(2 * inset, 2 * inset);

    painter.setPen(_pen);
    drawBorder(painter, borderRect);
}

void Menu::onProcessMouseEvent(const Pt::Forms::MouseEvent& ev)
{
    const Pt::Gfx::PointF& screenPos = ev.position();
    Widget* menu = onFindMenu(screenPos);

    if(menu)
    {
        if(menu == this)
            Pt::Forms::Popup::onProcessMouseEvent(ev);
        else
            menu->processEvent(ev);

        return;
    }
    Pt::Forms::Popup::onProcessMouseEvent(ev);
}

void Menu::onShowEvent(const Pt::Forms::ShowEvent& ev)
{
    Pt::Forms::Popup::onShowEvent(ev);

    MenuSubItem* mi = dynamic_cast<MenuSubItem*>(parentItem());

    if (mi)
    {
        if( ev.visible() )
            mi->openMenu();
        else
            mi->closeMenu();
    }
}

void Menu::onCloseEvent(const Pt::Forms::CloseEvent& ev)
{
    MenuSubItem* mi = dynamic_cast<MenuSubItem*>(parentItem());

    if( mi)
        mi->closeMenu();

    Pt::Forms::Popup::onCloseEvent(ev);
}


bool Menu::onMouseEvent(const Pt::Forms::MouseEvent& ev)
{
    return Pt::Forms::Popup::onMouseEvent(ev);
}

bool Menu::onEnterEvent(const Pt::Forms::EnterEvent& ev)
{
    return Pt::Forms::Popup::onEnterEvent(ev);
}

bool Menu::onLeaveEvent(const Pt::Forms::LeaveEvent& ev)
{
    return Pt::Forms::Popup::onLeaveEvent(ev);
}

void Menu::onAddMenu(MenuSubItem& item)
{
}

void Menu::onRemoveMenu(MenuSubItem& item)
{
}

}}
