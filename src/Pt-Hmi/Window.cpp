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
  
  You should have received a copy of the GNU Lesser General Public License 
  along with this library; if not, write to the Free Software Foundation, 
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/ChildWindow.h>

namespace Pt {

namespace Hmi {

Window::Window(Window* parent)
: _parent(parent)
, _pointerWidget(0)
, _focusedWidget(0)
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
, _focuseMoveKey()
, _isClosed(true)
, _isActive(false)
{
    _eventReady += Pt::slot(*this, &Window::onKeyEvent);
    _eventReady += Pt::slot(*this, &Window::onPointerEvent);  
    _eventReady += Pt::slot(*this, &Window::onMoveEvent);
    _eventReady += Pt::slot(*this, &Window::onResizeEvent);
    _eventReady += Pt::slot(*this, &Window::onActivateEvent);
    _eventReady += Pt::slot(*this, &Window::onCloseEvent);
}


Window::~Window()
{
    setMainWidget(0);

    const std::vector<ChildWindow*>& children = _windowManager.windows();
    std::vector<ChildWindow*>::const_iterator it;
    for(it = children.begin(); it != children.end(); ++it)
    {
        (*it)->_parent = 0;
    }

    _windowManager.clear();
}
    

void Window::addWindow(ChildWindow& window)
{
    if(window._parent)
        window._parent->removeWindow(window);
    
    window._parent = this;
    _windowManager.add(&window);        
}


void Window::removeWindow(ChildWindow& window)
{
    _windowManager.remove(&window);
    window._parent = 0;
}


Window* Window::findWindow(const std::string& name)
{
    if(_title == name)
        return this;

    const std::vector<ChildWindow*>& windows = _windowManager.windows();

	std::vector<ChildWindow*>::const_iterator it;
	for(it = windows.begin(); it != windows.end(); ++it)
	{
        Window* w = (*it)->findWindow(name);
		if(w)
			return w;
	}

    return 0;
}


void Window::setMainWidget(Widget* widget)
{   
    if(_mainWidget)
        _mainWidget->setWindow(0);

    _mainWidget = widget;

    if(_mainWidget)
    {
        if( _mainWidget->parent() )
            _mainWidget->parent()->removeWidget(*_mainWidget);

        _mainWidget->setWindow(this);
    }
}


void Window::removeWidget(Widget& w)
{
    if( _pointerWidget == &w )
        _pointerWidget = 0;

    if( _focusedWidget == &w )
        _focusedWidget = 0;        
}


Widget* Window::findWidget(const std::string& name)
{
    return _mainWidget ? _mainWidget->findWidget(name) : 0;
}


void Window::setPointedWidget( Widget* widget ) 
{
    if( _pointerWidget == widget )
        return;

    if( _pointerWidget )            
        _pointerWidget->onPointerLeave();

    _pointerWidget = widget;

    if( _pointerWidget )
        _pointerWidget->onPointerEnter();
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


void Window::activate()
{
    onActivate();
}


void Window::close()
{
    onClose();    
}


void Window::onClose()
{
    setVisible( false );
}        

void Window::onRender( PaintSurface& surface )
{    
    if( _mainWidget )
        _mainWidget->render();

    _windowManager.render();
}


void Window::processEvent(const Pt::Event& ev)
{
    onEvent(ev);
}


void Window::onEvent(const Pt::Event& ev)
{
    _eventReady.send(ev);
}


void Window::onPointerEvent(const PointerEvent& ev)
{    
    if( _windowManager.pointerInput( ev ) )
    {
        this->setPointedWidget( 0 );
        return;
    }

    if( _mainWidget )
    {
        Widget* widget = _mainWidget->findWidget( Gfx::PointF( ev.x(), ev.y() ) );

        this->setPointedWidget( widget );    

        if( widget )
            widget->processEvent(ev);        
    }
}


void Window::onKeyEvent(const KeyEvent& ev)
{
  if( _windowManager.keyInput( ev ) )
      return;

    if( ! isEnabled() )
        return;
    
    if( ev.key() == _focuseMoveKey && ev.state() == Pt::Hmi::KeyEvent::KeyUp )
    {    
        if(  ev.key().shift() )
        {    
/*                if(!focusPrev() )
                    focusPrev();*/
        }
        else
        {
/*                if( !focusNext() )
                    focusNext();*/
        }        
        
    }

 if( _focusedWidget != 0 )
    _focusedWidget->onKeyEvent(ev);

}


void Window::onResizeEvent(const ResizeEvent& ev)
{    
    Window::setSize( ev.size() );
    _state = ev.state();
}


void Window::onMoveEvent( const MoveEvent& ev)
{
    setPosition( ev.position() );        
}


void Window::onActivateEvent(const ActivateEvent& ev)
{ 
    _isActive = ev.isActive();

    if( ! _isActive )
    {
        _windowManager.deactivate();
        return;
    }
      
//    invalidate();
}

void Window::onCloseEvent(const CloseEvent& ev)
{
    _isClosed =  true;
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


void Window::onSetEnabled( bool e )
{
    _enabled = e;
}


void Window::onSetVisible( bool b )
{
    _visible = b;
}


void Window::onSetSize(const Gfx::SizeF& size)
{
    _size = size;            
}

    
void Window::onSetPosition(const Gfx::PointF& pos)
{
   _position = pos;
}


void Window::onSetMinimumSize( const Gfx::SizeF& s )
{
    _minimumSize = s;
}


void Window::onSetMaximumSize(const Gfx::SizeF& s)
{
    _maximumSize = s;
}


void Window::registerShortcut( Widget* w )
{
   std::map<Key, std::vector<Widget*> >::iterator it = _shortcuts.find( w->shortcutKey() );

   if( it == _shortcuts.end() )
   {
    std::vector<Widget*> widgets;
    widgets.push_back( w);
    std::pair<Key,std::vector<Widget*> > pair( w->shortcutKey(), widgets );
    _shortcuts.insert( pair );
  }
  else
  {
    it->second.push_back( w);
  }
}


void Window::unregisterShortcut( Widget* w )
{
    std::map<Key, std::vector<Widget*> >::iterator it = _shortcuts.find( w->shortcutKey() );
    
    if( it == _shortcuts.end() )
      return;

   std::vector<Widget*>::iterator wt = std::find( it->second.begin(), it->second.end(), w );

   if( wt == it->second.end() )
    return;

    it->second.erase( wt );
}

} // namespace

} // namespace

