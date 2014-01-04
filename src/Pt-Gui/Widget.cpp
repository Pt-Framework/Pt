/*
 * Copyright (C) 2006 Marc Boris Duerner
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
#include "WidgetImpl.h"

#include "Pt/Gui/Widget.h"

#include "Pt/Gui/Application.h"
#include "Pt/Gui/CloseEvent.h"
#include "Pt/Gui/Event.h"
#include "Pt/Gui/KeyEvent.h"
#include "Pt/Gui/LayoutData.h"
#include "Pt/Gui/LayoutManager.h"
#include "Pt/Gui/NullLayout.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/MouseMoveEvent.h"
#include "Pt/Gui/MoveEvent.h"
#include "Pt/Gui/Painter.h"
#include "Pt/Gui/PaintEvent.h"
#include "Pt/Gui/ResizeEvent.h"



#include <string>
#include <iostream>
#include <typeinfo>

using namespace Pt::Gfx;
using namespace std;


namespace Pt {

namespace Gui {


Widget::Widget(Widget& newParent, const Gfx::Point& at, const Gfx::Size& size)
: _parent(&newParent)
, _region(at, size)
, _foregroundColor( ARgbColor(0, 0, 0) )
, _backgroundColor( ARgbColor(65535, 65535, 65535) )
, _enabled(true)
{
    newParent.addChild(*this);
    _impl = new WidgetImpl( *this, &newParent, at, size );
    _layout.reset(NullLayout::createFor(*this));
}


Widget::Widget(const Gfx::Point& at, const Gfx::Size& size)
: _parent(0)
, _region(at, size)
, _foregroundColor( ARgbColor(0, 0, 0) )
, _backgroundColor( ARgbColor(65535, 65535, 65535) )
, _enabled(true)
{
    _impl = new WidgetImpl( *this, 0, at, size );
    _layout.reset(NullLayout::createFor(*this));
}


Widget::Widget(Widget& newParent)
: _parent(&newParent)
, _region(Gfx::Point(0, 0), Gfx::Size(400, 300))
, _foregroundColor( ARgbColor(0, 0, 0) )
, _backgroundColor( ARgbColor(65535, 65535, 65535) )
, _enabled(true)
{
    newParent.addChild(*this);
    _impl = new WidgetImpl(*this, &newParent, _region.topLeft(), _region.size());
    _layout.reset(NullLayout::createFor(*this));
}


Widget::Widget()
: _parent(0)
, _region(Gfx::Point(0, 0), Gfx::Size(400, 300))
, _foregroundColor( ARgbColor(0, 0, 0) )
, _backgroundColor( ARgbColor(65535, 65535, 65535) )
, _enabled(true)
{
    _impl = new WidgetImpl(*this, 0, _region.topLeft(), _region.size());
    _layout.reset(NullLayout::createFor(*this));
}

Widget::~Widget()
{
    // Unparent this widget from its parent.
    this->unparent();

    // Unparent all children of this widget. Children remove themselves from the child list of this widget.
    while (!_childWidgets.empty()) {
        Widget* w = _childWidgets.front();
        w->unparent();
    }

    destroyed.send(*this);
    delete _impl;
}


void Widget::setTitle(const Pt::String& text)
{
    _impl->setTitle(text);
}

Pt::String Widget::title()
{
    return _impl->title();
}


const Gfx::ARgbColor& Widget::backgroundColor() const
{
    return _backgroundColor;
}


void Widget::setBackgroundColor(const Gfx::ARgbColor& color)
{
    _backgroundColor = color;
    this->update();
}


const Gfx::ARgbColor& Widget::foregroundColor() const
{
    return _foregroundColor;
}


void Widget::setForegroundColor(const Gfx::ARgbColor& color)
{
    _foregroundColor = color;
    this->update();
}


void Widget::setInsets(const Insets& insets)
{
    if (_insets != insets) {
        _insets = insets;
        this->update();
    }
}


const Insets& Widget::insets() const
{
    return _insets;
}


const Pt::Gfx::Region& Widget::region() const
{
    return _region;
}


const Gfx::Size& Widget::size() const
{
    return region().size();
}

void Widget::move(ssize_t x, ssize_t y)
{
    if (x == _region.x() && y == _region.y()) {
        return;
    }

    _impl->move(x, y);

    _region.setX(x);
    _region.setY(y);

    this->updateLayout();
}


ssize_t Widget::x() const
{
    return _region.x();
}


ssize_t Widget::y() const
{
    return _region.y();
}


size_t Widget::width() const
{
    return _region.width();
}


size_t Widget::height() const
{
    return _region.height();
}


void Widget::resize(size_t width, size_t height)
{
    if (width == _region.width() && height == _region.height()) {
        return;
    }

    _impl->resize(width, height);

    _region.setWidth(width);
    _region.setHeight(height);

    this->updateLayout();
}


void Widget::resize(const Gfx::Size& newSize)
{
    this->resize(newSize.width(), newSize.height());
}


void Widget::show()
{
    _impl->show();
    this->updateLayout();
}


void Widget::hide()
{
    _impl->hide();
}


Gfx::Size Widget::minimumSize()
{
    return Gfx::Size(0, 0); // TODO
}


Gfx::Size Widget::preferredSize()
{
    // Non-top-level widgets that are not containers should override this method to provide a specific preferred size.
    // Non-top-level widgets and top-level widgets use the preferred size of their layout manager if they have one.
    // If they don't have a layout manager they return the current size.
    // TODO We can currently not check if the widget has no layout manager. Every widget has a layout manager, namely
    // NullLayout. So I currently check the returned preferred size to match (0, 0). If it does, I suspect that there is
    // no layout manager set. Is this good?

    Gfx::Size preferredSize = layout().preferredSize();
    if (preferredSize.width() == 0 && preferredSize.height() == 0) {
        // No layout manager given. Use the current size as preferred size.
        return size();
    }

    // A layout manager returns a useful preferred size. Use it as preferred size.
    return preferredSize;
}


void Widget::updateLayout()
{
    if (_layout.get() != 0) {
        _layout->update();
    }
}


void Widget::pack()
{
    if (parent() != 0) {
        return; // This method has a parent, so is not a top-level widget. Thus it can not be packed.
    }

    resize(preferredSize().width(), preferredSize().height());
}


void Widget::setLayout(Layout* layout)
{
    _layout.reset(layout);
}


Layout& Widget::layout() const
{
    return *_layout.get();
}


const std::list<Widget*>& Widget::childWidgets()
{
    return _childWidgets;
}


const std::list<Widget*>& Widget::childWidgets() const
{
    return _childWidgets;
}


void Widget::unparent()
{
    this->reparent(0);
}


void Widget::reparent(Widget* newParent)
{
    if (_parent == newParent) {
        return; // Same parent as it is at the moment.
    }

    if (_parent) {
        _parent->removeChild(*this);
    }

    if (newParent) {
        _parent = newParent;
        _impl->setParent(newParent);
        newParent->addChild(*this);
    } else {
        _parent = 0;
        _impl->setParent(0);
    }
}


Widget* Widget::parent() const
{
    return _parent;
}


void Widget::enable()
{
    this->setEnabled(true);
}


void Widget::disable()
{
    this->setEnabled(false);
}


void Widget::setEnabled(bool newEnabledState)
{
    if (newEnabledState != _enabled)
    {
        _enabled = newEnabledState;
        this->update();
    }
}


bool Widget::isEnabled() const
{
    return _enabled;
}


Painter Widget::painter()
{
    return _impl->painter();
}


void Widget::event(const Event& event)
{
    this->_event(event);
}


void Widget::closeEvent(const CloseEvent& event)
{
    //std::clog << "Widget::closeEvent" << std::endl;
    this->_closeEvent(event);
}


void Widget::mouseEvent(const MouseEvent& event)
{
    //std::clog << "[" << this << "] Widget::mouseEvent" << std::endl;
    this->_mouseEvent(event);
}


void Widget::mouseMoveEvent(const MouseMoveEvent& event)
{
    //std::clog << "[" << this << "] Widget::mouseEvent" << std::endl;
    this->_mouseMoveEvent(event);
}



void Widget::moveEvent(const MoveEvent& event)
{
    //std::clog << "[" << this << "] Widget::moveEvent" << std::endl;
    _region.setX(event.x());
    _region.setY(event.y());

    this->_moveEvent(event);
}


void Widget::paintEvent(const PaintEvent& event)
{
    //std::clog << "[" << this << "] Widget::paintEvent" << std::endl;
    this->_paintEvent(event);
}


void Widget::resizeEvent(const ResizeEvent& event)
{
    //std::clog << "[" << this << "] Widget::resizeEvent" << std::endl;

    _region.setWidth(event.width());
    _region.setHeight(event.height());

    this->_resizeEvent(event);

    this->updateLayout();
}


void Widget::keyEvent(const KeyEvent& event)
{
    this->_keyEvent(event);
}


void Widget::_event(const Event& e)
{
    const type_info& typeInfo = e.typeInfo();

    if (typeInfo == CloseEvent::TYPE_INFO) {
        const CloseEvent& ev = (const CloseEvent&)(e);
        this->closeEvent(ev);
    }
    else if (typeInfo == MouseEvent::TYPE_INFO && _enabled) {
        const MouseEvent& ev = (const MouseEvent&)(e);
        this->mouseEvent(ev);
    }
    else if (typeInfo == KeyEvent::TYPE_INFO && _enabled) {
        const KeyEvent& ev = (const KeyEvent&)(e);
        this->keyEvent(ev);
    }
    else if (typeInfo == MoveEvent::TYPE_INFO) {
        const MoveEvent& ev = (const MoveEvent&)(e);
        this->moveEvent(ev);
    }
    else if (typeInfo == MouseMoveEvent::TYPE_INFO && _enabled) {
        const MouseMoveEvent& ev = (const MouseMoveEvent&)(e);
        this->mouseMoveEvent(ev);
    }
    else if (typeInfo == ResizeEvent::TYPE_INFO) {
        const ResizeEvent& ev = (const ResizeEvent&)(e);
        this->resizeEvent(ev);
    }
    else if (typeInfo == PaintEvent::TYPE_INFO) {
        const PaintEvent& ev = (const PaintEvent&)(e);
        this->paintEvent(ev);
    }
}


void Widget::_closeEvent(const CloseEvent& event)
{
    //std::clog << "Widget::_closeEvent" << std::endl;
    this->hide();
    closed.send();
}


void Widget::_mouseEvent(const MouseEvent& event)
{
    //std::clog << "[" << this << "] Widget::_mouseEvent" << std::endl;
}


void Widget::_mouseMoveEvent(const MouseMoveEvent& event)
{
    //std::clog << "[" << this << "] Widget::_mouseMoveEvent" << std::endl;
}


void Widget::_moveEvent(const MoveEvent& event)
{
    //std::clog << "[" << this << "] Widget::_moveEvent" << std::endl;
}


void Widget::_paintEvent(const PaintEvent& event)
{
    //std::clog << "[" << this << "] Widget::_paintEvent" << std::endl;
}


void Widget::_resizeEvent(const ResizeEvent& event)
{
    //std::clog << "[" << this << "] Widget::_resizeEvent" << std::endl;
}


void Widget::_keyEvent(const KeyEvent& event)
{
    //std::clog << "[" << this << "] Widget::_keyEvent" << std::endl;
    //std::clog << "[" << this << "] text: " << event.text() << std::endl;
    //std::clog << "[" << this << "] code: " << event.code() << std::endl;
}


} // namespace Gui

} // namespace Pt
