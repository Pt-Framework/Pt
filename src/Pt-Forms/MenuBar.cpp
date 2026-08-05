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
#include <Pt/Forms/MenuBar.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/StyleOptions.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>

namespace Pt {
namespace Forms {

MenuBar::MenuBar()
: _layout(Direction::Left)
, _currentItem(0)
{
    Base::add(_layout);
}

MenuBar::~MenuBar()
{
}


const Pt::Gfx::Brush& MenuBar::background() const
{
    return _background ? *_background
                       : Pt::Forms::Application::instance().styleOptions().background();
}


void MenuBar::setBackground(const Pt::Gfx::Brush& b)
{
    _background.reset( new Pt::Gfx::Brush(b) );
    invalidate();
}


const Pt::Gfx::Pen& MenuBar::contour() const
{
    return _contour ? *_contour
                    : Pt::Forms::Application::instance().styleOptions().contour();
}


void MenuBar::setContour(const Pt::Gfx::Pen& p)
{
    _contour.reset( new Pt::Gfx::Pen(p) );
    invalidate();
}



void MenuBar::addItem(MenuBarItem& item)
{
    _layout.addItem(item);

    if(item.menu())
        item.menu()->setAnchor(this);

    item.triggered() += Pt::slot(*this, &MenuBar::onItemClicked);
    item.setParentMenu(this);
}


void MenuBar::removeItem(MenuBarItem& item)
{
    _layout.removeItem(item);

    item.triggered() -= Pt::slot(*this, &MenuBar::onItemClicked);

    item.setParentMenu(0);

    if(_currentItem == &item)
        _currentItem = 0;
}

void MenuBar::onAddMenu(MenuSubItem& item)
{
    item.menu()->setAnchor(this);
}

void MenuBar::onRemoveMenu(MenuSubItem& item)
{
    item.menu()->setAnchor(0);
}

Pt::Forms::Widget* MenuBar::onFindMenu(const Pt::Gfx::PointF& screenPos)
{
    if( ! isVisible() )
        return 0;

    Pt::Gfx::PointF pos = this->fromGlobal(screenPos);

    Pt::Gfx::RectF rect( Pt::Gfx::PointF(0,0), size() );

    if( rect.contains( pos ) )
        return this;

    if( ! _currentItem)
        return 0;

    if(_currentItem->menu() == 0)
        return 0;

    return _currentItem->menu()->findMenu(screenPos);
}


void MenuBar::onOpenMenu(MenuSubItem& item)
{
    if(_currentItem)
    {
        if(_currentItem->menu())
            _currentItem->menu()->close();
    }

    _currentItem = &item;

}


void MenuBar::onCloseMenu(MenuSubItem& item)
{
    if(_currentItem == &item)
    {
        _currentItem = 0;
        //setCapture(false);
    }

    invalidate();
}


void MenuBar::onCancel()
{
    if(_currentItem)
        _currentItem->cancel();
}


void MenuBar::onItemClicked(MenuItemBase& item)
{
    MenuBarItem * bari = (MenuBarItem*)&item;

    Menu* menu = bari->menu();

    if( menu == 0 )
        return;

    if( ! menu->isVisible() )
    {
        Pt::Gfx::PointF menuPos( 0, item.size().height() );
        menuPos = item.toGlobal(menuPos);
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


void MenuBar::onInvalidate()
{
    Base::onInvalidate();

    const Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();
    const Pt::Forms::Style& style = Pt::Forms::Application::instance().style();

    _brush = background();
    _pen = contour();
}


Pt::Gfx::SizeF MenuBar::onMeasure(const Pt::Forms::SizePolicy& policy)
{
    double hspace = padding().leftRight() + _layout.margin().leftRight();
    double vspace = padding().topBottom() + _layout.margin().topBottom();

    Pt::Forms::SizePolicy contentPolicy = policy;
    contentPolicy.setWidth( policy.size().width() - hspace );
    contentPolicy.setHeight( policy.size().height() - vspace );

    _layout.measure(contentPolicy);
    return _layout.preferredSize();
}


void MenuBar::onLayout(const Pt::Gfx::RectF& rect)
{
    Base::onLayout(rect);


    Pt::Gfx::PointF pos(padding().left() + _layout.margin().left(),
                    padding().top()  + _layout.margin().top());

    double hspace = padding().leftRight() + _layout.margin().leftRight();
    double vspace = padding().topBottom() + _layout.margin().topBottom();

    Pt::Gfx::SizeF size;
    size.setWidth( rect.width() - hspace );
    size.setHeight( rect.height() - vspace );

    _layout.move(pos);
    _layout.resize(size);
}


void MenuBar::onPaint(PaintContext& context, const Pt::Gfx::RectF& rect)
{
    Base::onPaint(context, rect);

    const Pt::Forms::StyleOptions& options = Pt::Forms::Application::instance().styleOptions();

    Painter painter(context);
    painter.setClip(rect);

    painter.setBrush(_brush);
    painter.fillRect(rect);

}


void MenuBar::onProcessMouseEvent(const Pt::Forms::MouseEvent& ev)
{
    const Pt::Gfx::PointF& screenPos = ev.position();
    Widget* menu = findMenu(screenPos);
    if(menu)
    {
        if(menu == this)
            Base::onProcessMouseEvent(ev);
        else
            menu->processEvent(ev);

        return;
    }

    if( ev.isPress() )
    {
        cancel();
        return;
    }
}


bool MenuBar::onMouseEvent(const Pt::Forms::MouseEvent& ev)
{
    if( _currentItem && ev.isPress() )
    {
        _currentItem->closeMenu();
    }

    return Base::onMouseEvent(ev);
}


bool MenuBar::onTouchEvent(const Pt::Forms::TouchEvent& ev)
{
    if( _currentItem && ev.isPress() )
    {
        _currentItem->closeMenu();
    }

    return Base::onTouchEvent(ev);
}

}}
