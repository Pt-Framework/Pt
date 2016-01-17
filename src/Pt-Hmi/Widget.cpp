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
, _focusedActionKey()
, _name("")
, _shortcutKey()
, _hasFocus( false)
, _size( 100, 100)
, _position( 10,10) 
, _isValid(false)
{      
    _eventReady += Pt::slot(*this, &Widget::onKeyEvent);
    _eventReady += Pt::slot(*this, &Widget::onPointerEvent);
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

    if( !visible() )
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


Gfx::PointF Widget::toClient( const Gfx::PointF& globalPoint )
{
    if( _parent == 0 )
        return globalPoint;

    Gfx::PointF parPoint = _parent->toClient( globalPoint );
    return Gfx::PointF( parPoint.x() - position().x(), parPoint.y() - position().y() );
}

 
Gfx::PointF Widget::fromClient( const Gfx::PointF& localPoint )
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


void Widget::updatePosAndSize(Widget& w, const Gfx::SizeF& s, const Gfx::PointF& p)
{       
  w.setPosition(p);
  w.setSize(s);          
  w._isValid = false;
  _isValid = false;
}


void Widget::invalidate()
{
    _isValid = false;

    if( parent() )
    {
        parent()->invalidate();   
    }
    else
    {
        if(_window)
            _window->invalidate();
    }
}


void Widget::render()
{
    if( ! visible() )
        return;

    // layout children
    onLayout( surface() );

    // render
    if( ! _isValid )
    {
        surface().clear();
        onRender( surface() );
        _isValid = true;
    }

    // render children
    for( size_t i = 0; i < _children.size(); ++i )
    {
        Widget* child = _children[i];

        child->render();

        surface().painter().drawSurface( child->position(), child->surface() );
    }
}


void Widget::onLayout( PaintSurface& surface )
{
    Gfx::SizeF clientSize = surface.size();
    double posLeft  = 0;
    double posTop   = 0;
    double posRight  = clientSize.width();
    double posBottom = clientSize.height();
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
                point.setX( posLeft );
                posLeft += child->size().width();
                point.setY( 0 );    
               updatePosAndSize( *child,Gfx::SizeF( child->size().width(), clientSize.height() ), point );

            }
            break;

            case Layout::RightToLeft:
            {
                posRight -= child->size().width();
                point.setX( posRight );              
                point.setY( 0 );    
               updatePosAndSize( *child,Gfx::SizeF( child->size().width(), clientSize.height() ), point );

            }
            break;

            case Layout::TopToButton:
            {
                point.setX( 0 );
              
                point.setY( posTop );
                posTop += child->size().height();    
                updatePosAndSize( *child,Gfx::SizeF( clientSize.width(), child->size().height() ), point );
            }
            break;

            case Layout::ButtomToTop:
            {
                point.setX( 0 );
                posBottom -= child->size().height(); 
                point.setY( posBottom  );    
                updatePosAndSize( *child,Gfx::SizeF( clientSize.width(), child->size().height() ), point );
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


void Widget::onRender( PaintSurface& surface )
{        
  const Gfx::SizeF& size = this->size();

    if( size.width() < 0 || size.height() < 0)
        return; 

    const Gfx::Image&   backImage = backgroundImage();
    Pt::Hmi::Painter&     painter = surface.painter();
 Gfx::PointF         pos(0,0);
    Gfx::RectF               rectClient( pos, size );
 Gfx::RectF               rect(Gfx::PointF(0,0), surface.size() );
  
  if( _parent != 0 )
  {
   Gfx::Brush    backBrush(_parent->backgroundColor() );            
    painter.setBrush(backBrush);
    painter.fillRect(rect);
  }


    Gfx::Brush    brush(backgroundColor());
    
    painter.setBrush(brush);                
    painter.fillRect(rectClient);

    if( !backImage.empty() )
    {
        switch( backgroundImageLayout())
        {                
            case NoLayout:
            {
                painter.drawImage( Pt::Gfx::PointF(0,0), backImage );
            }
            break;
            
            case Tile:
            {
                for( double x = pos.x(); x < size.width();  x += backImage.width() )
                {
                    for( double y = pos.y(); y < size.height();  y += backImage.height() )
                        painter.drawImage(Gfx::PointF(x,y), backImage);
                }
            }
            break;

            case Center:
            {
                const double x = pos.x() + size.width()/2  - backImage.width()/2;
                const double y = pos.y() + size.height()/2  - backImage.height()/2;
                painter.drawImage(Gfx::PointF(x,y), backImage);
            }
            break;
            
            case Strech:
            {
                Gfx::Image strech = backImage.blockScale(Gfx::Size((int) size.width(), (int)size.height() ) );
                painter.drawImage( pos, strech );
            }
            break;

            case Zoom:
            {
        const double factor = size.width()/(double)backImage.width();
        Pt::Gfx::Size newSize( ( size_t)( backImage.width()*factor), (size_t)(backImage.height()*factor));

       Gfx::Image strech = backImage.blockScale(newSize);
                
        painter.drawImage(pos, strech);
            }
            break;
        }
    }    
}


void Widget::onActionKey(const KeyEvent& kev)
{
}


void Widget::onShortcutKey(const KeyEvent& kev)
{
}


void Widget::onPointerEnter()
{
    Application::instance().mainScreen().setCursor( &cursor() );    
}


void Widget::onPointerLeave()
{    
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
        _window->setFocusWidget(this);
    }
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

        return;
    }

/*
    //// mnemonic handling
    if( !_mnemonicKey.empty() && ev.state() == Pt::Hmi::KeyEvent::KeyUp && _visible )
    {        
        std::string mnKey;

        if( ev.alt())
            mnKey = "A//";
            
        mnKey += ev.toUTF8String();

        if(_mnemonicKey == mnKey)
                onMnemonic();
    }

    // action key handling
    if( ev.toUTF8String() == focusedActionKey() && hasFocus() )    
    {
        onActionKey( ev.state() );        
    }
    
    // shortcurt Key
    if( ev.shortCutKey() == shortcutKey() )
    {
        onShortcutKey( ev.state() );        
    }

    // propagate to children
    for( size_t i = 0; i < children().size(); ++i)
        children()[i]->onKeyEvent(ev); */
}


void Widget::onMnemonic()
{
    _mnemonicEntered.send();
}


bool Widget::contains(const Gfx::PointF& p)
{
    if( p.x()  < size().width() && p.x() >= 0 && p.y() < size().height() && p.y() >= 0)
            return true;
 
    return false;
}


void Widget::focus()
{
    if(_window)
        _window->setFocusWidget(this);          
}


void Widget::onFocus(bool hasFocus)
{    
    _hasFocus = hasFocus;
    invalidate();
}

void Widget::setFocusIndex(size_t index)
{
    _focusIndex = index;

    if( _window )
        _window->updateFocusOrder();
}


void Widget::onSetSize(const Gfx::SizeF& size)
{
    _size = size;            
  
  surface().resize( _size );
    _isValid = false;
}

    
void Widget::onSetPosition(const Gfx::PointF& pos)
{
   _position = pos;
   _isValid = false;
}


void Widget::onSetVisible( bool b )
{
    _visible = b;
}



void Widget::onSetCaption( const std::string& c )
{
    _caption = c;    

    _mnemonicKey.clear();

    int index = getMnemonicIndex(_caption);

    if( index == std::string::npos )
        return;
                    
    std::string unescaped = Widget::removeMnemonic(_caption);

    //_mnemonicKey.setAlt(true);    
    //_mnemonicKey.setUnicode(  std::toupper(unescaped[index]));
}



size_t Widget::getMnemonicIndex(const std::string& text)
{     
    size_t pos = 0;
  
  if( text.empty() )    
      return std::string::npos;

    for( size_t i = 0; i < text.size() - 1; ++i)
    {                
        if( text[i] == '&'  && text[i +1] =='&' )
            ++i;
        else if (text[i] == '&'  &&     text[i +1] !='&')
            return pos;

        pos++;
    }

    return std::string::npos;
}


std::string Widget::removeMnemonic(const std::string& text)
{
    std::string removed;

    for( size_t i = 0; i < text.size(); ++i )
    {                
        if( text[i] != '&')
        {        
            removed += text[i];    
            continue;
        }
        
        if( (i + 1) == text.size() )
            continue;

        if(text[i+1] != '&')
            continue;

        removed += text[i];
        ++i;
    }

    return removed;        
}

void Widget::onSetEnabled( bool e )
{
    _enabled = e;
}


void Widget::bindMnemonic( Widget& w )
{
    _mnemonicEntered += Pt::slot( w, &Widget::onMnemonic);
}


void Widget::unbindMnemonic( Widget& w )
{
    _mnemonicEntered -= Pt::slot( w, &Widget::onMnemonic);
}


void Widget::unbindMnemonic()
{
    _mnemonicEntered.disconnectAll();
}

} // namespace

} // namespace
