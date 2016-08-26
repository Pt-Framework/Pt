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
#include <Pt/Gfx/Brush.h>
#include <Pt/String.h>
#include <cassert>

namespace Pt {

namespace Hmi {

Widget::Widget()
: _parent(0)
, _window(0)
, _content(0)
, _visible(true)
, _enabled(true)
, _enabledState(true)
, _hasFocus(false)
, _acceptsFocus(false) 
, _focusIndex(0)
, _acceptsInput(true)
, _cursor( Hmi::Cursor::defaultCursor() )
, _actionKey(Key::Space)
, _mnemonic(0)
, _autoSize(false)
{      
    _eventReady += Pt::slot(*this, &Widget::onKeyEvent );
    _eventReady += Pt::slot(*this, &Widget::onScrollEvent );
    _eventReady += Pt::slot(*this, &Widget::onMoveEvent );
    _eventReady += Pt::slot(*this, &Widget::onResizeEvent );
    _eventReady += Pt::slot(*this, &Widget::onPaintEvent );
    _eventReady += Pt::slot(*this, &Widget::onMouseEvent);
    _eventReady += Pt::slot(*this, &Widget::onTouchEvent);
    _eventReady += Pt::slot(*this, &Widget::onEnterEvent);
    _eventReady += Pt::slot(*this, &Widget::onLeaveEvent);
    _eventReady += Pt::slot(*this, &Widget::onEnableEvent);
    _eventReady += Pt::slot(*this, &Widget::onFocusEvent);
    _eventReady += Pt::slot(*this, &Widget::onShowEvent);
}


Widget::~Widget()
{
    releaseMouse();

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

    // disable indirectly, when parent is disabled
    if( ! isEnabled() && widget.isEnabled() )
    {
        EnableEvent eev( widget.vid(), false);
        Application::instance().loop().commitEvent(eev);
    }

    widget.setParent(this);
    widget.setWindow(_window);
    widget.update();

    onAddWidget(widget);
}


void Widget::remove(Widget& widget)
{
    std::vector<Widget*>::iterator it;
    it = std::find(_children.begin(), _children.end(), &widget);
    if( it == _children.end() )
        return;   
     
    _children.erase(it);

    if(&widget == _content)
        _content = 0;
    
    // enable when indirectly disabled
    if( ! widget._enabledState && widget._enabled)
        widget.enable(true);
    
    widget.setParent(0);
    widget.setWindow(0);

    update();    
    
    onRemoveWidget(widget); 
}


void Widget::onAddWidget(Widget& w)
{
}


void Widget::onRemoveWidget(Widget& w)
{
}


void Widget::onParentChanged(Widget* w)
{
}


void Widget::setParent(Widget* parent)
{
    _parent = parent;
    onParentChanged(parent);
}


void Widget::setWindow(Window* window)
{
    if(_window)
        _window->removeWidget(*this);

    _window = window;
      
    std::vector<Widget*>::iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
        (*it)->setWindow(window);

    if( _window )
        _window->addWidget(*this);
}


const std::vector<Widget*>& Widget::widgets() const
{
    return _children;
}


Widget* Widget::findWidget(const Gfx::PointF& pos, bool input)
{
    if( ! isVisible() || ! isEnabled() )
        return 0;

    std::vector<Widget*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Widget* child = *it;
        
        if( ! child->geometry().contains(pos) )
            continue;

        Gfx::PointF p = child->fromParent(pos);
        Widget* found = child->findWidget(p);

        if( ! input)
            return found ? found : child;

        if( found && found->acceptsInput() )
            return found;

        if( child->acceptsInput() )
            return child;

        break;
    }

    return 0;
}


Widget* Widget::findWidget(const Gfx::PointF& pos)
{
    return findWidget(pos, false); 
}


void Widget::setContent(Widget& widget)
{
    if(_content)
        remove(*_content);

    _content = &widget;
    
    add(widget); 
}


Gfx::PointF Widget::toParent(const Gfx::PointF& pos) const
{
    return  pos + _position;
}


Gfx::PointF Widget::fromParent(const Gfx::PointF& pos) const
{
    return pos - _position;
}


Gfx::PointF Widget::fromWindow(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    Gfx::PointF p = _parent->fromWindow(pos);
    return p - _position;
}


Gfx::PointF Widget::toWindow(const Gfx::PointF& p) const
{
    Gfx::PointF pos = p + this->position();

    const Widget* w = this;
    
    for(w = w->parent(); w != 0; w = w->parent())
    {
        pos += w->position();
    }

    return pos;
}


Gfx::PointF Widget::toScreen(const Gfx::PointF& pos) const
{
    Gfx::PointF screenPos = toWindow(pos);
    
    if( window() )
        return window()->toScreen(screenPos);

    return screenPos;
}
    

Gfx::PointF Widget::fromScreen(const Gfx::PointF& pos) const
{
    Gfx::PointF widgetPos;

    if( window() )
        widgetPos = window()->fromScreen(pos);

    return fromWindow( widgetPos );
}


bool Widget::acceptsInput() const
{
  return _acceptsInput;
}


void Widget::setAcceptInput(bool a)
{
    _acceptsInput = a;
}


bool Widget::acceptsFocus() const
{
    return _acceptsFocus;
}
     
        
void Widget::setAcceptsFocus(bool a) 
{
    _acceptsFocus = a;
}


size_t Widget::focusIndex() const
{
    return _focusIndex;
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


void Widget::setFocusIndex(size_t index)
{
    _focusIndex = index;

    if( _window )
        _window->setFocusIndex(*this, index);
}


void Widget::onFocusEvent(const FocusEvent& ev)
{    
    _hasFocus = ev.isFocused();
}


Key Widget::actionKey() const
{
    return _actionKey;
}


void Widget::setActionKey(const Key& ak)
{
    _actionKey = ak;
    this->onSetActionKey(ak);
}


void Widget::onSetActionKey(const Key&)
{
}


void Widget::onActionKey(const KeyEvent& kev)
{
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

    this->onSetShortcut(k);
}


void Widget::onSetShortcut(const Key*)
{
}
  

void Widget::onShortcut(const KeyEvent& kev)
{
}


const Pt::Char* Widget::mnemonic() const
{
    return _mnemonic != 0 ? &_mnemonic : 0;
}


void Widget::setMnemonic(const Char& ch)
{  
    _mnemonic = ch;

    const Char* m = ch != 0 ? &ch : 0;
    if( _window )
        _window->setMnemonic(*this, m);
}


String Widget::setMnemonic(const String& text)
{  
    String str;
    Char mnemonic = 0;

    bool onAmp = false;
    for(String::const_iterator it = text.begin(); it != text.end(); ++it)
    {
        if(onAmp)
        {
            if(*it != '&')
                mnemonic = *it;

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

    setMnemonic(mnemonic);

    return str;
}


void Widget::setMnemonicWidget(Widget* w)
{
    _mnemonicEntered.disconnect();

    if(w)
        _mnemonicEntered += Pt::slot(*w, &Widget::onMnemonic);
}


void Widget::onMnemonic()
{
    _mnemonicEntered.invoke();
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
    
    Gfx::PointF updatePos = toWindow( rect.topLeft() );
    Gfx::RectF updateRect( updatePos, rect.size() );

    w->update(updateRect);
}


void Widget::repaint(const Gfx::RectF& rect)
{
    if( ! isVisible() )
        return;

    Gfx::RectF widgetRect = rect.intersect( Gfx::RectF(Gfx::PointF(0,0),
                                                       this->size() ) );
    PaintEvent pev( vid(), widgetRect);
    Application::instance().loop().commitEvent(pev);

    const std::vector<Widget*>& widgets = this->widgets();
    std::vector<Widget*>::const_iterator it;
    for(it = widgets.begin() ; it != widgets.end(); ++it)
    {        
        Widget* w = (*it);

        Gfx::RectF updateRect = w->geometry().intersect(rect);
        if( updateRect.isNull() )
            continue;

        Gfx::PointF updatePos = w->fromParent( updateRect.topLeft() );
        updateRect.setOrigin(updatePos);

        w->repaint(updateRect);            
    }
}


bool Widget::isVisible() const
{
    return _visible;
}


void Widget::show( bool s )
{
    _visible = s;

    ShowEvent ev(vid(), s);
    Application::instance().loop().commitEvent(ev);

    update();
}


void Widget::onShowEvent(const ShowEvent& ev )
{
    _visible = ev.visible();
}


bool Widget::isEnabled() const
{
    return _enabledState && _enabled;
}


void Widget::enable(bool e)
{    
    _enabled = e;
    _enabledState = e;

    EnableEvent eev( vid(), e);
    Application::instance().loop().commitEvent(eev);

    update();
}


void Widget::onEnableEvent(const EnableEvent& ev)
{        
    //if( ! ev.enabled() )
    //    _enabledState = false;
    //else
    //    _enabledState = _enabled;

    _enabledState = ev.enabled();

    for( size_t i = 0; i < _children.size(); ++i)
    {
        Widget* w = _children[i];

        // skip directly disabled children, because they are either already
        // disabled or they should not be enabled
        if( ! w->_enabled )
            continue;

        EnableEvent eev( w->vid(), ev.enabled());
        Application::instance().loop().commitEvent(eev);
    }  
}


void Widget::raise()
{
    if( ! _parent )
        return;

    _parent->onRaise(*this);     
}


void Widget::onRaise(Widget& w)
{
    std::vector<Widget*>::iterator it = std::find(_children.begin(), _children.end(), &w);

    if( it == _children.end() )
        return;

    _children.erase(it);
    _children.push_back(&w);
    
    w.update();   
}


void Widget::grabMouse()
{
    Application::instance().grabMouse(*this);
}


void Widget::releaseMouse()
{
    Application::instance().releaseMouse(*this);
}


const Gfx::PointF& Widget::position() const
{
    return _position;
}


void Widget::move(const Gfx::PointF& pos)
{
    Gfx::RectF updateRect(Gfx::PointF(0, 0), _size);

    Gfx::PointF to = pos - _position;
    updateRect.unify( Gfx::RectF(to, _size) ); 
    
    MoveEvent mev(vid(), pos);
    Application::instance().loop().commitEvent(mev);

    // update needs to refer to previous position
    update(updateRect);

    _position = pos;
}


void Widget::onMoveEvent(const MoveEvent& ev)
{        
    _position = ev.position();
}


const Gfx::SizeF& Widget::size() const
{
    return _size;
}


void Widget::resize(const Gfx::SizeF& s)
{
    Gfx::SizeF updateSize( std::max( size().width(), s.width()), 
                           std::max( size().height(), s.height()) );

    Gfx::RectF updateRect(Gfx::PointF(0,0), updateSize);

    _size = s;

    ResizeEvent rev(vid(), s);
    Application::instance().loop().commitEvent(rev);
    
    update(updateRect);
}


void Widget::onResizeEvent(const ResizeEvent& ev)
{    
    _size = ev.size();

    if( _content ) 
      _content->resize( _size );

    onLayout();
}


const Gfx::RectF Widget::geometry() const
{
    return Gfx::RectF( position(), size() );
}


void Widget::setGeometry(const Gfx::PointF& pos, const Gfx::SizeF& size)
{
    move(pos);
    resize(size);
}


const Cursor& Widget::cursor() const
{
    return  _cursor;
}


void Widget::setCursor(const Cursor& c) 
{
    _cursor = c;

    // TODO: if this is the pointer widget do the same as onEnterEvent
    // Application::instance().screen().setCursor( &cursor() );
}


bool Widget::isAutoSize() const
{
    return _autoSize;
}


void Widget::setAutoSize(bool a)
{
    _autoSize = a;

    if( parent() )
       parent()->onLayout();
}


Gfx::SizeF Widget::preferredSize() const
{
    if(_autoSize)
        return this->onAutoSize();

    return size();
}


Gfx::SizeF Widget::onAutoSize() const
{
    return size();
}


const Spacing& Widget::margin() const
{
    return _margin;                  
}


void Widget::setMargin(const Spacing& s)
{
    _margin = s;

    if( parent() )
       parent()->onLayout();
}


void Widget::setMargin(double n)
{
    _margin.setAll(n);

    if( parent() )
       parent()->onLayout();
}


const Spacing& Widget::padding() const
{
    return _padding;
}


void Widget::setPadding( const Spacing& p )
{
    _padding = p;
    onLayout();
}


void Widget::setPadding(double n)
{
    _padding.setAll(n);
    onLayout();
}


const Docking& Widget::docking() const
{
    return _docking;
}


void Widget::setDocking(const Docking& d)
{
    _docking = d;
    
    if( parent() )
       parent()->onLayout();
}


void Widget::onLayout()
{         
}


void Widget::onClicked(const Gfx::PointF& pos)
{
}


void Widget::onEvent(const Pt::Event& ev)
{
    _eventReady.send(ev ); 
}


void Widget::onPaintEvent(const PaintEvent& ev)
{
}


void Widget::onMouseEvent(const MouseEvent& ev)
{ 
    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    if( ! rect.contains( ev.position() ) )
        return;

    if( ev.isPress(MouseEvent::Left) && _acceptsFocus )
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


void Widget::onEnterEvent( const EnterEvent& ev )
{
    Application::instance().setCursor( &cursor() ); 
}


void Widget::onLeaveEvent(const LeaveEvent& ev )
{

}

} // namespace

} // namespace
