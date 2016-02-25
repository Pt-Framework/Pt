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
, _enabled(true)
, _visible(true)
, _cursor( Hmi::Cursor::defaultCursor() )
, _autoSize(false)
, _acceptFocus( false) 
, _name("")
, _hasFocus( false)
, _actionKey(Key::Space)
, _geometry(10, 100, 10, 100)
, _isValid(true)
, _mnemonic(0)
, _updateRect()
{      
    _eventReady += Pt::slot(*this, &Widget::onKeyEvent);
    _eventReady += Pt::slot(*this, &Widget::onPointerEvent);
    _eventReady += Pt::slot(*this, &Widget::onTouchEvent);
    _eventReady += Pt::slot(*this, &Widget::onScrollEvent);
}


Widget::~Widget()
{
    std::vector<Widget*>::iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
        remove(**it);

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

    if( ! visible() || ! isEnabled() )
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


Widget* Widget::findWidget( const std::string& name )
{
  if( _name == name )
      return this;

  std::vector<Widget*>::iterator it = _children.begin();

    for( ; it != _children.end(); ++it )
    {
        Widget* found = (*it)->findWidget( name );

        if( found )
            return found;        
    }
        
    return  0;
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
    onEvent(ev);
}


void Widget::onUpdate(const Gfx::RectF& rect)
{
    // The widget is already invalid in case of a nested update()
    // this means the parent must already be invalid or has just
    // been rendered. Therefore we stop the chain of update calls
    // towards the root of the widget/window tree.
    if( ! _isValid )
        return;

   // given rect in parent coordinates
   Gfx::RectF updateRect(rect);
   updateRect.setOrigin( position() + rect.topLeft() );

    // add the update area in parent coordinates
    _updateRect.unify(updateRect);

    if( parent() )
    {
        parent()->onUpdate(updateRect);   
    }
    else
    {
        if(_window)
            _window->update(updateRect);
    }
}


void Widget::update()
{
    // The widget is already invalid in case of a nested update()
    // this means the parent must already be invalid or has just
    // been rendered. Therefore we stop the chain of update calls
    // towards the root of the widget/window tree.
    if( ! _isValid )
        return;

    _isValid = false; 
   
    // the update area and window rect are already in parent coordinates
    _updateRect.unify(_geometry);

    if( parent() )
    {
        parent()->onUpdate(_updateRect);
    }
    else
    {
        if(_window)
            _window->update(_updateRect);
    }
}


//void Widget::update()
//{
//    // update rect was recorded in parent coordinates 
//    _updateRect.setOrigin( _updateRect.topLeft() - position() );
//
//    update(_updateRect);
//}
//
//
//void Widget::update(const Gfx::RectF& rect)
//{
//    // The widget is already invalid in case of a nested update()
//    // this means the parent must already be invalid or has just
//    // been rendered. Therefore we stop the chain of update calls
//    // towards the root of the widget/window tree.
//    if( ! _isValid )
//        return;
//
//    _isValid = false; 
//   
//   // report update rect in parent coordinates
//   Gfx::RectF updateRect(rect);
//   updateRect.setOrigin( position() + rect.topLeft() );
//
//    if( parent() )
//    {
//        parent()->onUpdate(updateRect);
//    }
//    else
//    {
//        if(_window)
//            _window->update(updateRect);
//    }
//}


void Widget::render(PaintSurface& surface, 
                    const Gfx::RectF& updateRect)
{
    if( ! visible() )
    {
        _isValid = true;
        return;
    }

    if( ! _isValid )
        onLayout();
    
    onRender(surface, updateRect);
    
    _updateRect.clear();
    _isValid = true;
}


void Widget::onLayout()
{         
}


void Widget::onRender(PaintSurface& surface, const Gfx::RectF& rect)
{
    PaintArea widgetSurface;

    for( size_t i = 0; i < _children.size(); ++i )
    {
        Widget* child = _children[i];

        if( rect.intersect( child->geometry() ).isNull() )
            continue;

        //static int _n = 0;
        //std::clog << _n++ << " RENDER RENDER RENDER " << child->name() << std::endl;

        ///Gfx::PointF paintOffset( pos.x() + child->position().x(),
        ///                         pos.y() + child->position().y() );

        // update rect in child coordinates
        Gfx::RectF updateRect(rect);
        updateRect.setOrigin( rect.topLeft() - child->position() );

        widgetSurface.set( surface, child->geometry() );
        child->render(widgetSurface, updateRect);
    }
}


void Widget::onEvent(const Pt::Event& ev)
{
    _eventReady.send(ev);
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


void Widget::onScrollEvent( const ScrollEvent& ev )
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
    Application::instance().mainScreen().setCursor( &cursor() );    
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


void Widget::setEnabled( bool e )
{
    _enabled = e;
}


void Widget::setSize(const Gfx::SizeF& size)
{
    _updateRect.unify(_geometry); 
    _geometry.setSize(size);
    _updateRect.unify(_geometry);            
}

    
void Widget::setPosition(const Gfx::PointF& pos)
{
    _updateRect.unify(_geometry); 
    _geometry.setOrigin(pos);
    _updateRect.unify(_geometry);   
}


void Widget::setGeometry(const Gfx::PointF& pos, const Gfx::SizeF& size)
{
  setPosition(pos);
  setSize(size);
}


void Widget::setVisible( bool b )
{
    _visible = b;
}

} // namespace

} // namespace
