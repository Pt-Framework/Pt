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

#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Layout.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/String.h>
#include <cassert>

namespace Pt {

namespace Hmi {

Widget::Widget()
: _window(0)
, _parent(0)
, _cursor( Hmi::Cursor::defaultCursor() )
, _autoSize(false)
, _acceptFocus( false) 
, _hasFocus( false)
, _enabled(true)
, _visible(true)
, _actionKey(Key::Space)
, _mnemonic(0)
{      
    _eventReady += Pt::slot(*this, &Widget::onKeyEvent );
    _eventReady += Pt::slot(*this, &Widget::onPointerEvent );
    _eventReady += Pt::slot(*this, &Widget::onTouchEvent );
    _eventReady += Pt::slot(*this, &Widget::onScrollEvent );
    _eventReady += Pt::slot(*this, &Widget::onMoveEvent );
    _eventReady += Pt::slot(*this, &Widget::onResizeEvent );
    _eventReady += Pt::slot(*this, &Widget::onPaintEvent );
    _eventReady += Pt::slot(*this, &Widget::onKeyEvent);
    _eventReady += Pt::slot(*this, &Widget::onPointerEvent);
    _eventReady += Pt::slot(*this, &Widget::onTouchEvent);
    _eventReady += Pt::slot(*this, &Widget::onScrollEvent);
}


Widget::~Widget()
{
    while( ! _children.empty() )
        remove( *_children.back() );

    if(_parent)
        _parent->remove(*this);

    if(_window)
        _window->removeWidget(*this);
}


Window* Widget::window()
{
    return _window;
}


const Window* Widget::window() const
{
    return _window;
}


Widget* Widget::parent()
{         
    return _parent;
}


const Widget* Widget::parent() const
{
    return _parent;
}


void Widget::add(Widget& widget)
{
    if(widget.parent() == this)
        return;

    if( widget.parent() )
        widget.parent()->remove(widget);

    _children.push_back(&widget);
    widget._parent = this;

    widget.setWindow(_window);   
    widget.update();
}


void Widget::remove(Widget& widget)
{
    std::vector<Widget*>::iterator it;
    it = std::find(_children.begin(), _children.end(), &widget);
    if( it == _children.end() )
        return;
    
    _children.erase(it);
    widget._parent = 0;

    widget.setWindow(0);
    widget.update();
}


std::vector<Widget*>& Widget::widgets()
{
    return _children;
}


const std::vector<Widget*>& Widget::widgets() const
{
    return _children;
}


void Widget::setWindow(Window* window)
{
    onFocus(false);

    if(_window)
        _window->removeWidget(*this);

    _window = window;
      
    std::vector<Widget*>::iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
        (*it)->setWindow(window);

    if( _window )
        _window->addWidget(*this);            
}


Widget* Widget::findWidget( const Gfx::PointF& pos )
{
    std::vector<Widget*>::reverse_iterator it = _children.rbegin();

    if( ! isVisible() || ! isEnabled() )
        return 0;

    for( ; it != _children.rend(); ++it )
    {
        Widget* child = *it;

        Gfx::PointF localPos = child->toClient( pos );        
        
        if( child->contains( localPos ) )
        {
            Widget* found = child->findWidget( pos );

            if( found )
                return found;
        }
    }
        
    return contains( toClient( pos ) )  ? this : 0;
}


bool Widget::contains(const Gfx::PointF& p)
{
    if( p.x()  < size().width() && p.x() >= 0 && p.y() < size().height() && p.y() >= 0)
        return true;
 
    return false;
}


Gfx::PointF Widget::toClient(const Gfx::PointF& globalPoint)
{
    if( _parent == 0 )
        return globalPoint;

    Gfx::PointF parPoint = _parent->toClient( globalPoint );
    return Gfx::PointF( parPoint.x() - position().x(), parPoint.y() - position().y() );
}

 
Gfx::PointF Widget::fromClient(const Gfx::PointF& localPoint)
{
    double x = localPoint.x();
    double y = localPoint.y();
    const Widget* widget = parent();
    

    while( widget != 0  &&  widget->parent() != 0 )
    {                        
        x += widget->position().x();
        y += widget->position().y();    

        widget = widget->parent();
    }    

    return Gfx::PointF(x,y);
}


Gfx::PointF Widget::toWindowPosition(const Gfx::PointF& p) const
{
    Gfx::PointF pos = p + this->position();

    const Widget* w = this;
    
    for(w = w->parent(); w != 0; w = w->parent())
    {
        pos += w->position();
    }

    return pos;
}


Gfx::PointF Widget::toScreen(const Gfx::PointF& p) const
{
    Gfx::PointF pos = p + this->position();

    const Widget* parentWidget = this->parent();
    
    for(; parentWidget; parentWidget = parentWidget->parent())
    {
        pos += parentWidget->position();
    }

    const Window* parentWindow = this->window();
    if( ! parentWindow )
        return pos;

    for(; parentWindow; parentWindow = parentWindow->parent())
    {
        pos += parentWindow->position();
    }

    return pos;
}


bool Widget::hasFocus() const
{
    return _hasFocus;
}


void Widget::focus()
{
    if(_window)
        _window->setFocusWidget(this);          
}


bool Widget::acceptFocus() const
{
    return _acceptFocus;
}
     
        
void Widget::setAcceptFocus(bool a) 
{
    _acceptFocus = a;
}


size_t Widget::focusIndex() const
{
    return _focusIndex;
}


void Widget::setFocusIndex(size_t index)
{
    _focusIndex = index;

    if( _window )
        _window->setFocusIndex(*this, index);
}


Key Widget::actionKey() const
{
    return _actionKey;
}


void Widget::setActionKey( Key ak )
{
  _actionKey = ak;
}


const Key* Widget::shortcut() const
{
    if(_shortcutKey.keyCode() == Key::NoKey)
        return 0;
           
    return &_shortcutKey;
}


void Widget::setShortcut(const Key* k)
{
    if( ! k )
        _shortcutKey.set(Key::NoKey);
    else
        _shortcutKey = *k;

    if(_window)
        _window->setShortcut(*this, k);
}


void Widget::setMnemonicWidget(Widget* w)
{
    _mnemonicEntered.disconnect();

    if(w)
        _mnemonicEntered += Pt::slot(*w, &Widget::onMnemonic);
}


const Pt::Char* Widget::mnemonic() const
{
    return _mnemonic != 0 ? &_mnemonic : 0;
}


String Widget::setMnemonic(const String& text)
{  
    String str;
    _mnemonic = 0;

    bool onAmp = false;
    for(String::const_iterator it = text.begin(); it != text.end(); ++it)
    {
        if(onAmp)
        {
            if(*it != '&')
                _mnemonic = *it;

            str += *it;
            onAmp = false;
        }
        else
        {
            if(*it == '&')
                onAmp = true;
            else
                str += *it;
        }
    }

    if(onAmp)
        str += '&';
    
    Char* ch = _mnemonic != 0 ? &_mnemonic : 0;
    if( _window )
        _window->setMnemonic(*this, ch);

    return str;
}


void Widget::processEvent(const Pt::Event& ev)
{
     _eventReady.send(ev);
}


void Widget::update()
{
    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    update(rect);
}


void Widget::update(const Gfx::RectF& rect)
{   
    Window* w = window();
    if( ! w )
        return;
    
    Gfx::PointF updatePos = toWindowPosition( rect.topLeft() );
    Gfx::RectF updateRect( updatePos, rect.size() );

    w->update(updateRect);
}


void Widget::paint(const Gfx::RectF& rect)
{
    PaintEvent pev( vid(), rect);
    Application::instance().loop().commitEvent(pev);

    std::vector<Widget*>& widgets = this->widgets();
    std::vector<Widget*>::iterator it;
    for(it = widgets.begin() ; it != widgets.end(); ++it)
    {        
        Widget* w = (*it);

        if( w->geometry().intersect(rect).isNull() )
            continue;

        Gfx::RectF updateRect( rect.topLeft() - w->position(), rect.size() );
        w->paint(updateRect);            
    }
}

// TODO: move this to a Layout base class
void Widget::onLayout()
{         
}


void Widget::enable( bool b )
{
    _enabled = b;
    update();
}


void Widget::setGeometry(const Gfx::PointF& pos, const Gfx::SizeF& size)
{
    move(pos);
    resize(size);
}


void Widget::show( bool s )
{
    _visible = s;
    update();
}


void Widget::move(const Gfx::PointF& pos)
{
    Gfx::RectF updateRect(Gfx::PointF(0, 0), _size); 

    Gfx::PointF to = pos - _position;
    updateRect.unify( Gfx::RectF(to, _size) ); 

    MoveEvent mev(vid(), pos);
    Application::instance().loop().commitEvent(mev);

    update(updateRect);
}


void Widget::resize(const Gfx::SizeF& s)
{
    Gfx::SizeF updateSize( std::max( size().width(), s.width()), 
                           std::max( size().height(), s.height()) );

    Gfx::RectF updateRect(Gfx::PointF(0,0), updateSize);

    ResizeEvent rev(vid(), s);
    Application::instance().loop().commitEvent(rev);
    
    update(updateRect);
}
 

void Widget::onEvent(const Pt::Event& ev)
{
    _eventReady.send(ev ); 
}


void Widget::onPaintEvent(const PaintEvent& ev)
{
}


void Widget::onResizeEvent(const ResizeEvent& ev)
{    
    _size = ev.size();

    // TODO: move this to a Layout base class
    onLayout();
}


void Widget::onMoveEvent(const MoveEvent& ev)
{    
    _position = ev.position();
}


void Widget::onPointerEvent(const MouseEvent& ev)
{        
    if( ev.isPress(MouseEvent::Left) && _acceptFocus )
    {
        focus();
    }

    if( ev.isRelease(MouseEvent::Left) && hasFocus() )
    {
        onClicked( ev.position() );
    }
}


void Widget::onTouchEvent(const TouchEvent& ev)
{
}


void Widget::onScrollEvent(const ScrollEvent& ev)
{
}


void Widget::onKeyEvent(const KeyEvent& ev)
{
    if( (ev.key().keyCode() == Key::Tab) && ev.isPress() )
    {
        if( ev.key().hasModifiers(Key::Shift) )
            _window->focusPrev();
        else
            _window->focusNext();

        update();

        Widget* focusWidget = _window->focusWidget();

        if(focusWidget)
            focusWidget->update();

        return;
    }

    if( ev.key() == actionKey() && hasFocus() )    
    {
        onActionKey(ev);        
    }
}


void Widget::onPointerEnter()
{
    Application::instance().screen().setCursor( &cursor() );    
}


void Widget::onPointerLeave()
{    
}


void Widget::onClicked(const Gfx::PointF& pos)
{
}


void Widget::onFocus(bool hasFocus)
{    
    _hasFocus = hasFocus;
}


void Widget::onActionKey(const KeyEvent& kev)
{
}


void Widget::onShortcut(const KeyEvent& kev)
{
}


void Widget::onMnemonic()
{
    _mnemonicEntered.invoke();
}


Gfx::SizeF Widget::preferredSize() const
{
    if(_autoSize)
        return this->onAutoSize();

    return size();
}


void Widget::setAutoSize(bool a)
{
    _autoSize = a;
}


bool Widget::isAutoSize() const
{
    return _autoSize;
}


Gfx::SizeF Widget::onAutoSize() const
{
    return Gfx::SizeF(0, 0);
}

} // namespace

} // namespace
