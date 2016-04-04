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
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/EraseEvent.h>
#include "MainWindowImpl.h"
#include "ChildWindowImpl.h"
#include <cassert>


namespace {

bool lowerFocusIndex(Pt::Hmi::Widget* a, Pt::Hmi::Widget* b )
{
    return a->focusIndex() < b->focusIndex();
}

}

namespace Pt {

namespace Hmi {

Window::Window( Window* parent )
: _impl(0)
, _parent(0)
, _pointerWidget(0)
, _focusWidget(0)
, _minimumSize(0,0)
, _maximumSize(2000,2000)
, _startPostion( WindowPosition::Manual )
, _state(WindowState::Normal )
, _border( WindowBorder::Sizeable)
, _icon()
, _canClose(true)
, _isClosed(false)
, _isActive(false)
, _mainWidget(0)
{    

    _windowManager.init(*this);

    _eventReady += Pt::slot(*this, &Window::onActivateEvent);
    _eventReady += Pt::slot(*this, &Window::onCloseEvent);
    _eventReady += Pt::slot(*this, &Window::onEnterEvent );
    _eventReady += Pt::slot(*this, &Window::onLeaveEvent );
    _eventReady += Pt::slot(*this, &Window::onMoveEvent );
    _eventReady += Pt::slot(*this, &Window::onResizeEvent );
    _eventReady += Pt::slot(*this, &Window::onShowEvent );  

    if(parent)
        parent->add(*this);

}


Window::~Window()
{
    if( ! _parent)
        Application::instance().mainScreen().unregisterWindow(*this);

    setMainWidget(0);

    const std::vector<Window*>& children = _windowManager.windows();

    while( ! children.empty() )
        remove( *children.back() );
 
    if(_parent)
        _parent->remove(*this);

    delete _impl;
}


Window* Window::parent()
{
   return _parent;
}


const Window* Window::parent() const
{
    return _parent;
}


const std::vector<Window*>& Window::windows() const 
{
    return _windowManager.windows();
}


std::vector<Window*>& Window::windows() 
{
    return _windowManager.windows();
}

    
Widget* Window::mainWidget() 
{
    return _mainWidget;
}


const Widget* Window::mainWidget()  const 
{
    return _mainWidget;
}


void Window::setMainWidget(Widget* widget)
{   
    if(_mainWidget)
        _mainWidget->setWindow(0);

    _mainWidget = widget;

    if(_mainWidget)
    {
        if( _mainWidget->parent() )
            _mainWidget->parent()->remove(*_mainWidget);

       _mainWidget->setWindow(this);
       _mainWidget->move(Gfx::PointF(0,0) );
       _mainWidget->resize( size());
    }
}


Widget* Window::pointerWidget()
{
    return _pointerWidget;
}


bool Window::isClosed() const
{
  return _isClosed;       
}


void Window::close()
{
    if( _impl )
        _impl->close();
}


bool Window::isActive() const
{
    return _isActive;
}


void Window::activate()
{
    if(_isActive)
        return;

    if( _impl ) 
        _impl->activate();
}


Gfx::PointF Window::toScreen(const Gfx::PointF& p) const
{
    Gfx::PointF pos = p + this->position();

    const Window* parentWindow = this->parent();

    for(; parentWindow; parentWindow = parentWindow->parent())
    {
        pos += parentWindow->position();
    }

    return pos;
}


void Window::update()
{
    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    update(rect);
}


void Window::update(const Gfx::RectF& rect)
{
    Window* parentWindow = this->parent();
    if( parentWindow )
    {
        parentWindow->onUpdate(*this, rect);
    }
    else
    {
        // TODO: make screen the parent window
        Application::instance().onUpdate(*this, rect);
    }
}


void Window::onUpdate(Window& child, const Gfx::RectF& rect)
{
    double borderWidth = Application::instance().windowBorderWidth();
    double titleHeight = Application::instance().windowTitleHeight();

    Gfx::PointF updatePos = rect.topLeft() + child.position();
    updatePos.addX( borderWidth );
    updatePos.addY( borderWidth + titleHeight );

    Gfx::RectF updateRect(updatePos, rect.size());
    this->update(updateRect);
}


void Window::focusPrev()
{
    moveFocus(_focusList.rbegin(), _focusList.rend());
}


void Window::focusNext()
{
    moveFocus(_focusList.begin(), _focusList.end());
}


void Window::paint(const Gfx::RectF& rect)
{
    const double borderWidth = Application::instance().windowBorderWidth();
    const double titleHeight = Application::instance().windowTitleHeight();

    if( ! this->isVisible() )
        return;

    Painter painter(_surface);
    painter.setBrush( Pt::Gfx::Color(0.9f, 0.9f, 0.9f) );
    painter.fillRect(rect);

    if( mainWidget() )
        mainWidget()->paint(rect);

    std::vector<Window*>& windows = this->windows();
    std::vector<Window*>::iterator child;
    for(child = windows.begin(); child != windows.end(); ++child)
    {
        Gfx::PointF pos( rect.topLeft() - (*child)->position() );
        pos.subX( borderWidth );
        pos.subY( borderWidth + titleHeight );

        Gfx::RectF updateRect( pos, rect.size() );
        (*child)->paint(updateRect);
    }

    PaintEvent pev(vid(), rect);
    Application::instance().loop().commitEvent(pev);
}


void Window::onPaintEvent(const PaintEvent& ev)
{
    if( ! this->isVisible() )
        return;
 
    const Gfx::RectF& rect = ev.rect();   

    _windowManager.render(_surface, rect);

    if(_impl)
        _impl->paint(ev.rect());
}


PixmapSurface& Window::surface()
{
    return _surface;
}


WindowManager& Window::windowManager()
{
    return _windowManager;
}


const WindowManager& Window::windowManager() const 
{
    return _windowManager;
}


WindowImpl* Window::impl()
{
    return _impl;   
}


void Window::runModal()
{
    const std::vector<Window*>& windows = Application::instance().mainScreen().windows();
    Window* activeWindow = 0;
    std::map<Window*, bool> states;

    std::vector<Window*>::const_iterator it;
    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* w = (*it);

        if(w != this)
        {
            if( w->isActive() )
                activeWindow = w;

            states[w] = w->enabled();
            w->enable(false);
        }
    }

    enable(true);
    show(true);

    while( ! isClosed() )
    {
        if( ! isActive() )
            activate();
        
        Application::instance().nextEvent();
    }
    
    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* w = (*it);

        if( activeWindow == w )
            activeWindow->activate();
                
        std::map<Window*, bool>::iterator  mapIt = states.find(w);
        if( mapIt != states.end() )
            mapIt->first->enable( mapIt->second );
    }
}


void Window::onPointerEvent(const MouseEvent& ev)
{
    if( _windowManager.pointerInput( ev ) )
        return;

    if( ! _mainWidget || 
        ! _mainWidget->isVisible() || 
        ! _mainWidget->enabled() )
    {
        Application::instance().mainScreen().setCursor( &Cursor::defaultCursor() ); 
        return;
    }

    Widget* widget = _mainWidget->findWidget( ev.position() );

    // widget can be null
    setPointerWidget(widget); 
  
    if(widget) 
    {
        MouseEvent clientEv(ev);

        clientEv.setPosition( widget->toClient(ev.position()) );
        widget->processEvent(clientEv);  
    }
}


void Window::onTouchEvent( const TouchEvent& ev )
{

}


void Window::onScrollEvent( const ScrollEvent& ev )
{
    if( ! _mainWidget )
        return;

    if( _pointerWidget )
        _pointerWidget->processEvent(ev);    
}


void Window::onEnterEvent( const EnterEvent& ev )
{
}


void Window::onLeaveEvent(const LeaveEvent& ev )
{
    setPointerWidget( 0 );
}


void Window::onKeyEvent(const KeyEvent& ev)
{
  //std::clog << "Window::onKeyEvent: " << (ev.isPress() ? "press " : "release ") 
  //          << ev.key().keyCode() << " " << ev.unicode().narrow() << std::endl;

    if( _windowManager.keyInput( ev ) )
        return;

    if( ! enabled() )
        return;

    std::map<Key, Widget*>::iterator s = _shortcuts.find( ev.key() );
    if( s != _shortcuts.end() )
    {
        s->second->onShortcut(ev);       
        return;
    }

    if( ev.isPress() && ev.key().hasModifiers(Key::Alt) )
    {
        std::map<Char, Widget*>::iterator m = _mnemonics.find( ev.unicode() );
        if( m != _mnemonics.end() )
        {
            m->second->onMnemonic();       
            return;
        }
    }
    
    if( _focusWidget )
        _focusWidget->onKeyEvent(ev);
    else
    {
        if(ev.key().keyCode() == Key::Tab && ev.isPress() )
        {
            focusNext();

            if(_focusWidget)
                _focusWidget->update();
        }
    }
}



void Window::onActivateEvent(const ActivateEvent& ev)
{ 
    _isActive = ev.isActive();
}


void Window::onCloseEvent(const CloseEvent& ev)
{
     _isClosed =  true;
}


const Gfx::SizeF& Window::minimumSize() const
{
    return _minimumSize;
}


void Window::setMinimumSize(const Gfx::SizeF& s)
{
    if( _impl ) 
        _impl->setMinimumSize(s);

    _minimumSize = s;
}


const Gfx::SizeF& Window::maximumSize() const
{
    return _maximumSize;
}


void Window::setMaximumSize(const Gfx::SizeF& s)
{
    if( _impl )
        _impl->setMaximumSize(s);

    _maximumSize = s;
}


Hmi::WindowPosition::Type Window::defaultPosition() const
{
    return _startPostion;
}


void Window::setDefaultPosition( Hmi::WindowPosition::Type p)
{    
    _startPostion = p;
}


Hmi::WindowState::Type Window::state() const
{
    return _state;
}


void Window::setState( WindowState::Type s)
{
    if( _impl )
        _impl->setState(s);

    _state = s;
}


WindowBorder::Type Window::border() const
{
    return _border;
}


void Window::setBorder(WindowBorder::Type t)
{
    if( _impl )
        _impl->setBorder(t);

    _border = t;
}


const Gfx::Image& Window::icon() const
{
    return _icon;
}


void Window::setIcon(const Gfx::Image& i)
{
    if( _impl )
        _impl->setIcon(i);

    _icon = i;
}


bool Window::isClosable() const
{
    return _canClose;
}


void Window::setClosable(bool c)
{
    _canClose = c;
}


const std::string& Window::title() const
{
    return _title;
}


void Window::setTitle( const std::string& t )
{
    if( _impl )
        _impl->setTitle(t);

    _title = t;
}


// TODO: are all windows updated correctly?
void Window::add(Window& child)
{
    if( child._parent == this )
        return;

    if( ! child._parent )
        Application::instance().mainScreen().unregisterWindow(child);

    if( child._parent )
        child._parent->_windowManager.remove(child);
    
    delete child._impl;
    child._impl = 0;

    child._parent = this;    
    child.createImpl();

    _windowManager.add(child);

    update();
}

// TODO: are all windows updated correctly?
void Window::remove(Window& child)
{
    if(child._parent != this)
        return;
    
    _windowManager.remove(child);

    delete child.impl();             
    child._impl = 0;

    child._parent = 0;
    child.createImpl();

    update();
}


void Window::createImpl()
{
    assert( ! _impl);

    if( _parent )
    {
        _impl = new ChildWindowImpl(this);
    }
    else
    {
        _impl = new MainWindowImpl(this);
        Application::instance().mainScreen().registerWindow(*this);
    }

    _impl->move(position());
    _impl->resize(size());
//    _impl->setDecoration( _decoration );
    _impl->setTitle( _title );
    _impl->setBorder( _border );
    _impl->setMaximumSize( _minimumSize);
    _impl->setMaximumSize( _maximumSize );
    _impl->setIcon(_icon);
    _impl->enable( enabled() );
    _impl->setState( _state );
    _impl->show(isVisible());
    
}


void Window::show(bool b)
{
    if( ! _impl )
        createImpl(); // also calls setVisible
    else  
        _impl->show(b);
}


void Window::enable(bool e)
{
    if( _impl )
        _impl->enable(e);
}

void Window::resize(const Gfx::SizeF& s)
{
    if(_impl)
        _impl->resize(s);
    else
        _size = s;
}

void Window::onShowEvent( const ShowEvent& ev )
{
    _visible = ev.visible();
}

void Window::onEnableEvent( const EnableEvent& ev )
{
    _enabled = ev.enabled();
}

void Window::onResizeEvent( const ResizeEvent& s )
{
    _size = s.size();
    _surface.resize(s.size());

    if( _mainWidget )
        _mainWidget->resize(s.size());
}


void Window::onMoveEvent(const MoveEvent& ev)
{
    _position = ev.position();
}


void Window::move(const Gfx::PointF& p)
{
    if( _impl )
        _impl->move(p);
    else
        _position = p;
}


const Gfx::Font& Window::font() const
{
    return _font; 
}


void Window::setFont(const Gfx::Font& ft)
{
    _font = ft;
}


WindowDecoration::Flags Window::decoration() const
{
    return _decoration;
}


void Window::setDecoration( WindowDecoration::Flags d )
{
    if( _impl )            
        _impl->setDecoration( d );

    _decoration = d;
}
    

void Window::addWidget(Widget& w)
{
    addFocusWidget(w);

    setShortcut( w, w.shortcut() );
    setMnemonic( w, w.mnemonic() );
}


void Window::removeWidget(Widget& w)
{
    if( _pointerWidget == &w )
        _pointerWidget = 0;

    if( _focusWidget == &w )
        _focusWidget = 0;

    if(_mainWidget == &w)
        _mainWidget = 0;

    removeFocusWidget(w);

    setShortcut(w, 0);
    setMnemonic(w, 0);
}


void Window::setShortcut(Widget& w, const Key* key)
{
    std::map<Key, Widget*>::iterator it = _shortcuts.begin();
    while( it != _shortcuts.end() )
    {
        if(it->second == &w)        
            _shortcuts.erase(it++);
        else
            ++it;
    }

    if(key)
        _shortcuts[*key] = &w;
}


void Window::setMnemonic(Widget& w, const Char* ch)
{
    std::map<Char, Widget*>::iterator it = _mnemonics.begin();
    while( it != _mnemonics.end() )
    {
        if(it->second == &w)
            _mnemonics.erase(it++);
        else
            ++it;
    }

    if(ch)
        _mnemonics[*ch] = &w;
}


void Window::setPointerWidget( Widget* widget ) 
{
    if( _pointerWidget == widget )
        return;

    if( _pointerWidget )
    {
        Widget* w = _pointerWidget;
        _pointerWidget = widget;
        w->onPointerLeave();
    }
    else
    {
        _pointerWidget = widget;
    }

    if( _pointerWidget )
        _pointerWidget->onPointerEnter();
}


Widget* Window::focusWidget()
{
    return _focusWidget;
}


void Window::setFocusWidget(Widget* widget) 
{
    assert( widget ? widget->window() == this : true);

    if( _focusWidget == widget )
        return;

    if( _focusWidget )        
    {
        _focusWidget->onFocus(false);
    }
    
    _focusWidget = widget;

    if( _focusWidget )
    {
        _focusWidget->onFocus(true);
    }
}


template <typename Iter>
void Window::moveFocus(Iter begin, Iter end)
{
    Iter current = std::find(begin, end, _focusWidget);
    Iter it = current;
    
    if( it != end )
        ++it;
    else
        it = begin;

    while(it != current)
    {
        if( it == end )
            it = begin;

        Widget* w = *it;
        
        if( w->acceptFocus() )
        {
            setFocusWidget(w);
            return;
        }

        ++it;
    }

    // handles the case when the current focus widget has just been set to 
    // not accept focus and no other widget can accept focus either.
    if( _focusWidget && ! _focusWidget->acceptFocus() )
    {
        setFocusWidget(0);
    }
}


void Window::addFocusWidget(Widget& w)
{
    if( w.window() != this )
        return;

    removeFocusWidget(w);

    if( _focusList.empty() )
        w.setFocusIndex(0);    
    else
        w.setFocusIndex( _focusList.back()->focusIndex() + 1);    

    _focusList.push_back(&w);    
}


void Window::removeFocusWidget(Widget& w)
{
    if( w.window() != this )
        return;

    std::vector<Widget*>::iterator it;
    it = std::find(_focusList.begin(), _focusList.end(), &w);

    if( it != _focusList.end() )
        _focusList.erase(it);

    // TODO: should w loose focus?
}


void Window::setFocusIndex(Widget& , size_t)
{
    std::sort(_focusList.begin(), _focusList.end(), &lowerFocusIndex);
}

} // namespace

} // namespace
