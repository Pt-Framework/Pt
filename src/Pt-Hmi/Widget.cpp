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
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/String.h>
#include <Pt/Utf8Codec.h>
#include "WindowImpl.h"

namespace Pt {

namespace Hmi {

Widget::Widget()
: _window(0)
, _parent(0)
, _enabled(true)
, _visible(true)
, _backgroundColor(Gfx::Color::fromRgb8(237,237,237))
, _foregroundColor( Gfx::Color::fromRgb8(0,0,0) )
, _backgroundImage()
, _backgroundImageLayout( NoLayout )
, _cursor( Hmi::Cursor::defaultCursor() )
, _acceptFocus( true) 
, _name("")
, _hasFocus( false)
, _actionKey(Key::Space)
, _size( 100, 100)
, _position( 10,10) 
, _contentAlignment(TopLeft)
, _isValid(false)
, _mnemonic(0)
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


void Widget::update()
{
    _isValid = false;

    onUpdate();

    if( parent() )
    {
        parent()->update();   
    }
    else
    {
        if(_window)
            _window->update();
    }
}


void Widget::render()
{
    if( ! visible() )
        return;

    // layout
    onLayout(_surface);

    // render
    if( ! _isValid )
    {
        _surface.clear();
        onPaint(_surface);
        _isValid = true;
    }

    // render children
    for( size_t i = 0; i < _children.size(); ++i )
    {
        Widget* child = _children[i];
        child->render();
        _surface.painter().drawSurface( child->position(), child->_surface );
    }
}


void Widget::updatePosAndSize(Widget& w, const Gfx::SizeF& s, const Gfx::PointF& p)
{       
   w._position = p;      
   w._size = s;              
   w._surface.resize( s );  
   w._isValid = false;
   
   _isValid = false;
}


void Widget::processEvent(const Pt::Event& ev)
{
    onEvent(ev);
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
        update();
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


void Widget::onUpdate()
{
}


void Widget::onLayout( PaintSurface& surface )
{
    Gfx::SizeF clientSize = surface.size();
    
    double posLeft  = _layout.padding().left();
    double posTop   = _layout.padding().top();
    double posRight  = clientSize.width() - _layout.padding().right();
    double posBottom = clientSize.height() - _layout.padding().bottom();
    
    std::vector<Widget*> fillLayoutChildren;

    for( size_t i = 0; i < widgets().size(); ++i )
    {
        Widget* child = _children[i];            
            
        Gfx::PointF point = child->position();

        switch( _layout.type() )
        {
            case Layout::None:
            {
              switch( child->docking().type() )
              {
                case Docking::None:        
                {
                  point.setX( point.x() );
                  point.setY( point.y() );            
                }
                break;

                case Docking::Left:
                {
                  point.setX( posLeft );
                  point.setY( posTop );            

                  Gfx::SizeF size( child->size().width(), (posBottom - posTop));
                  posLeft += child->size().width(); 
                            
                  updatePosAndSize( *child, size, point );           
                }
                break;

                case Docking::Top:
                {
                  point.setX( posLeft );
                  point.setY( posTop  );            
                  Gfx::SizeF size( (posRight - posLeft), child->size().height());

                  posTop += child->size().height();      
        
                  updatePosAndSize(*child, size, point );
                }
                break;

                case Docking::Right:
                {
                  posRight -= child->size().width();     
                  point.setX( posRight );
                  point.setY( posTop );                             
                    
                  Gfx::SizeF size( child->size().width(), (posBottom - posTop) );
        
                  updatePosAndSize(*child, size, point );
                }
                break;

                case Docking::Bottom:
                {
                  posBottom -= child->size().height();    
                  point.setX( posLeft );
                  point.setY( posBottom  );                              
                  Gfx::SizeF size( (posRight - posLeft), child->size().height() );
                  updatePosAndSize(*child, size, point);
                }
                break;

                case Docking::Fill:
                {
                  fillLayoutChildren.push_back( child );
                  continue;
                }      
              }
            }
            break;

            case Layout::LeftToRight:
            {                        
                point.setX( posLeft + child->margin().left() );
                point.setY( _layout.padding().top() + child->margin().top() ); 
                
                posLeft += child->size().width() + child->margin().left() + child->margin().right();

                Gfx::SizeF childSize( child->size().width(), 
                                      clientSize.height() - _layout.padding().top() - _layout.padding().bottom() -
                                      child->margin().top() - child->margin().bottom() );
                 
                updatePosAndSize(*child, childSize , point);
            }
            break;

            case Layout::RightToLeft:
            {
                posRight -= child->size().width();
                posRight -= child->margin().right();
                
                point.setX( posRight );              
                point.setY( _layout.padding().top() + child->margin().top()  ); 
                
                posRight -= child->margin().left();

                Gfx::SizeF childSize( child->size().width(), 
                                      clientSize.height() - _layout.padding().top() - _layout.padding().bottom() - 
                                      child->margin().top() - child->margin().bottom());
                                      
                                         
                updatePosAndSize( *child, childSize, point );
            }
            break;

            case Layout::TopToBottom:
            {
                point.setX( _layout.padding().left() + child->margin().left()  );
                point.setY( posTop + child->margin().top() );
                
                posTop += child->size().height() + child->margin().top() + child->margin().bottom();
                
                Gfx::SizeF childSize( clientSize.width() - 
                                      _layout.padding().left() - _layout.padding().right() -
                                      child->margin().left() - child->margin().right(), 
                                      child->size().height());
                                         
                updatePosAndSize(*child, childSize, point);
            }
            break;

            case Layout::BottomToTop:
            {
                posBottom -= child->size().height();
                posBottom -= child->margin().bottom();
                
                point.setX( _layout.padding().left() + child->margin().left() );
                point.setY( posBottom );
                
                posBottom -= child->margin().top();    

                Gfx::SizeF childSize( clientSize.width() - 
                                      _layout.padding().left() - _layout.padding().right() -
                                      child->margin().left() - child->margin().right(), 
                                      child->size().height() );

                updatePosAndSize(*child, childSize, point);
            }
            break;
        }                    
    }    

  if( fillLayoutChildren.size() != 0 )
  {
      Widget* child = fillLayoutChildren[0]; 
      Gfx::PointF point(posLeft, posTop);

      const double width  = posRight - posLeft;
      const double height = posBottom - posTop;

      updatePosAndSize( *child,Gfx::SizeF( width, height ), point );
  }
}


void Widget::onPaint( PaintSurface& surface )
{        
    const Gfx::SizeF& size = surface.size();

    if( size.width() < 0 || size.height() < 0)
        return; 

    const Gfx::Image& backImage = backgroundImage();
    Pt::Hmi::Painter& painter = surface.painter();
    
    Gfx::Brush brush( backgroundColor() );
    painter.setBrush(brush);  
    
    Gfx::PointF origin(0, 0);
    Gfx::RectF rect( origin, size );              
    painter.fillRect(rect);

    if( backImage.empty() )
        return;

    switch( backgroundImageLayout() )
    {                
        case NoLayout:
        {
            painter.drawImage( Pt::Gfx::PointF(0,0), backImage );
        }
        break;
            
        case Tile:
        {
            for( double x = 0; x < size.width();  x += backImage.width() )
            {
                for( double y = 0; y < size.height();  y += backImage.height() )
                    painter.drawImage(Gfx::PointF(x,y), backImage);
            }
        }
        break;

        case Center:
        {
            const double x = size.width()/2  - backImage.width()/2;
            const double y = size.height()/2  - backImage.height()/2;
            painter.drawImage(Gfx::PointF(x,y), backImage);
        }
        break;
            
        case Strech:
        {
            Gfx::Image strech = backImage.blockScale(Gfx::Size((int) size.width(), (int)size.height() ) );
            painter.drawImage( origin, strech );
        }
        break;

        case Zoom:
        {
            const double factor = size.width()/(double)backImage.width();
            Pt::Gfx::Size newSize( ( size_t)( backImage.width()*factor), (size_t)(backImage.height()*factor));

            Gfx::Image strech = backImage.blockScale(newSize);
            painter.drawImage(origin, strech);
        }
        break;
    }
}


void Widget::setEnabled( bool e )
{
    _enabled = e;
}


void Widget::setSize(const Gfx::SizeF& size)
{
    _size = size;              
    _surface.resize( _size );
    _isValid = false;
}

    
void Widget::setPosition(const Gfx::PointF& pos)
{
   _position = pos;
   _isValid = false;
}


void Widget::setVisible( bool b )
{
    _visible = b;
}


void Widget::setCaption(const std::string& text)
{  
    _caption.clear();
    _mnemonic = 0;

    bool onAmp = false;
    for(std::string::const_iterator it = text.begin(); it != text.end(); ++it)
    {
        if(onAmp)
        {
            if(*it != '&')
                _mnemonic = *it;

            _caption += *it;
            onAmp = false;
        }
        else
        {
            if(*it == '&')
                onAmp = true;
            else
                _caption += *it;
        }
    }

    if(onAmp)
        _caption += '&';
    
    Char* ch = _mnemonic != 0 ? &_mnemonic : 0;
    if( _window )
        _window->setMnemonic(*this, ch);

    setCaptionMetrics();
}


void Widget::setFont( const Gfx::Font& f )
{
    _font = f;
    setCaptionMetrics();
}


void Widget::setCaptionMetrics()
{  
    String text = Pt::Utf8Codec::decode( this->caption() );
    _captionMetrics = Hmi::Painter::fontMetrics( _font, text);
}

} // namespace

} // namespace
