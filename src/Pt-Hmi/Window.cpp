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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/ChildWindow.h>

namespace Pt {

namespace Hmi {

Window::Window(Window* parent)
: _winParent(parent)
, _apointedWidget( 0 )
, _focusedWidget( 0)
, _windowManager(*this)
, _minimumSize(0,0)
, _maximumSize( 2000,2000)
, _startPostion( WindowStartPosition::Manual )
, _state(WindowState::Normal )
, _showInTaskbar( false)
, _showTitle( true)
, _showMinimizeButton( true)
, _showMaximizeButton(true)
, _showSysMenu( true)
, _border( WindowBorder::Sizeable)
, _icon()
, _canClose( true)
, _firstShow(true)
, _focuseMoveKey("")
, _isClosed(true)
, _isActive(false)
{
    setAcceptFocus(false);    

    eventReady() += Pt::slot(*this, &Window::onMoveEvent);
    eventReady() += Pt::slot(*this, &Window::onResizeEvent);
    eventReady() += Pt::slot(*this, &Window::onActivateEvent);
    eventReady() += Pt::slot(*this, &Window::onCloseEvent);
}


Window::~Window()
{
    _windowManager.clear();
}
    

const std::vector<ChildWindow*> Window::windows() const
{
    return _windowManager.windows();
}


void Window::addWindow(ChildWindow& w)
{
    w._winParent = this;
    w.setVisible( true);
    _windowManager.add(&w);        
}


void Window::removeWindow(ChildWindow& w)
{
    _windowManager.remove(&w);
    w.setVisible( false);
    w._winParent = 0;
}


Window* Window::getWindow()
{
    return this;
}

Window* Window::windowParent() const
{
    return _winParent;
}


void Window::onWidgetRemoved( Widget& w  )
{
    if( _apointedWidget == &w )
        _apointedWidget = 0;

    if( _focusedWidget == &w )
        _focusedWidget = 0;        
}


void Window::activate()
{
    onActivate();
}


void Window::setPointedWidget( Widget* widget ) 
{
    if( _apointedWidget == widget )
        return;

    if( _apointedWidget )            
        _apointedWidget->onPointerLeave();

    _apointedWidget = widget;

    if( _apointedWidget )
        _apointedWidget->onPointerEnter();
}


void Window::setFocusedWidget( Widget* widget ) 
{
    if( _focusedWidget == widget )
        return;

    if( _focusedWidget )            
        _focusedWidget->setFocus(false);

    _focusedWidget = widget;

    if( _focusedWidget )
        _focusedWidget->setFocus(true);
}


void Window::onPointerEvent(const PointerEvent& ev)
{    
  if( _windowManager.pointerInput( ev ) )
    {
            this->setPointedWidget( 0 );    
      return;
    }

    Widget* widget = findWidget( Gfx::PointF( ev.x(), ev.y() ) );

    this->setPointedWidget( widget );    

    if( widget && widget != this )
        widget->processEvent(ev);        
}

void Window::onKeyEvent(const KeyEvent& ev)
{
  if( _windowManager.keyInput( ev ) )
      return;

    if( ! isEnabled() )
        return;
    
    if( ev.toUTF8String() == _focuseMoveKey && ev.state() == Pt::Hmi::KeyEvent::KeyUp )
    {    
        if(  ev.shift() )
        {    
                if(!focusPrev() )
                    focusPrev();
        }
        else
        {
                if( !focusNext() )
                    focusNext();
        }        

        invalidate();
    }

  Widget::onKeyEvent(ev);
}


void Window::onResizeEvent(const ResizeEvent& ev)
{    
    Window::setSize( ev.size() );
    _state = ev.state();
    invalidate();
}


void Window::onMoveEvent( const MoveEvent& ev)
{
    Widget::setPosition( ev.position() );    
    invalidate();
}

void Window::onShowTitle( bool s )
{
    _showTitle = s;
}


void Window::onShowMinimizeButton( bool s )
{
    _showMinimizeButton = s;
}


void Window::onShowMaximizeButton( bool s )
{
    _showMaximizeButton = s;
}


void Window::onShowSystemMenu( bool  s )
{
    _showSysMenu = s;
}


void Window::onState(const Hmi::WindowState::Type& s)
{
    _state = s;
}


void Window::onBorder(const Hmi::WindowBorder::Type& t)
{
    _border = t;
}

void Window::onShowInTaskbar(bool s)
{
    _showInTaskbar = s;
}


void Window::onIcon(const Gfx::Image& i)
{
    _icon = i;
}


void Window::onSetMinimumSize( const Gfx::SizeF& s )
{
    _minimumSize = s;
}


void Window::onSetMaximumSize(const Gfx::SizeF& s)
{
    _maximumSize = s;
}


void Window::onActivateEvent(const ActivateEvent& ev)
{ 
    _isActive = ev.isActive();

    if( ! _isActive )
    {
        _windowManager.deactivate();
        return;
    }
      
    invalidate();
}


void Window::close()
{
    onClose();    
}


void Window::onCloseEvent(const CloseEvent& ev)
{
    _isClosed =  true;
}


void Window::onClose()
{
    setVisible( false );
}        

void Window::onRender( PaintSurface& surface )
{
    Widget::onRender( surface );
    _windowManager.render();
}

} // namespace

} // namespace

