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

#include "MainWindowImpl.h"
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Painter.h>
#include <cassert>

//
// TODO:
// - runModal() funktioniert nicht für child windows
// - show() muss children anzeigen, die nicht hidden sind
// - WindowManager muss Cursor setzen, nicht Window::onPointerEvent
// - FocusEvent
//

namespace {

bool lowerFocusIndex(Pt::Hmi::Widget* a, Pt::Hmi::Widget* b )
{
    return a->focusIndex() < b->focusIndex();
}

} // namespace

namespace Pt {

namespace Hmi {

Window::Window(Window* parent)
: _impl(0)
, _parent(0)
, _mainWidget(0)
, _pointerWidget(0)
, _focusWidget(0)
, _init(false)
, _visible(true)
, _isActive(false)
, _enabled(true)
, _isClosed(false)
, _position(0,0)
, _size(10,10)
, _minimumSize(0,0)
, _maximumSize(2000,2000)
, _startPostion( WindowPosition::Manual )
, _state(WindowState::Normal )
, _border( WindowBorder::Sizeable)
, _icon()
, _canClose(true)
{    
    _windowManager.init(*this);

    _eventReady += Pt::slot(*this, &Window::onKeyEvent );
    _eventReady += Pt::slot(*this, &Window::onMouseEvent );
    _eventReady += Pt::slot(*this, &Window::onTouchEvent );
    _eventReady += Pt::slot(*this, &Window::onScrollEvent );
    _eventReady += Pt::slot(*this, &Window::onPaintEvent );
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
    setMainWidget(0);

    while( ! _windows.empty() )
       remove( *_windows.back() );
 
    deinit();
}


void Window::init(Window* parent)
{
    if(_init || _isClosed)
        return;

    if( ! _impl && ! parent )
    {
        _impl = new MainWindowImpl();
        Application::instance().screen().registerWindow(*this);
    }
    else
    {
        parent->_windowManager.add(*this);
        parent->_windows.push_back(this);
    }

    _parent = parent;
    _init = true;  

    move(_position);
    resize(_size);
    enable(_enabled);

    if( _isActive )
        activate();

    if( _impl)
    {
        _impl->setTitle(_title);
        _impl->setBorder(_border);
        _impl->setMaximumSize(_minimumSize);
        _impl->setMaximumSize(_maximumSize );
        _impl->setIcon(_icon);
        _impl->setState(_state);  
    }  
    
    show(_visible);
}


void Window::deinit()
{
    if( ! _init )
        return;

    if(_impl)
    {
        Application::instance().screen().unregisterWindow(*this);
        delete _impl;
        _impl = 0;
    }
    else if(_parent)
    {
        _parent->_windowManager.remove(*this);

        std::vector<Window*>::iterator it;
        it = std::find(_parent->_windows.begin(), _parent->_windows.end(), this);
        if( it != _parent->_windows.end() )
            _parent->_windows.erase(it); 
    }

    _init = false;
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
    return _windows;
}


void Window::add(Window& child)
{
    if( child.parent() == this )
        return;

    child.deinit();
    child.init(this);

    update();
}


void Window::remove(Window& child)
{
    if( child.parent() != this )
        return;

    child.deinit();
    child.init(0);

    update();
}


Window* Window::activeWindow()
{
    std::vector<Window*>::const_iterator it;

    for(it = _windows.begin(); it != _windows.end(); ++it)
    {
        Window* window = *it;
    
        if( window->isActive() )
            return window;

        window = window->activeWindow();
        if(window)
            return window;
    }

    return 0;
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

    if( ! _mainWidget )
        return;
    
    if( _mainWidget->parent() )
        _mainWidget->parent()->remove(*_mainWidget);

    _mainWidget->setWindow(this);
    _mainWidget->move( Gfx::PointF(0,0) );
    _mainWidget->resize( size() );
}


Widget* Window::findWidget(const Gfx::PointF& pos)
{
    if( ! isVisible() || ! isEnabled() )
        return 0;

    if(_mainWidget)
    {
        if( _mainWidget->geometry().contains(pos) )
        {
            Widget* widget = _mainWidget->findWidget(pos);
            return widget ? widget : _mainWidget;
        }
    }

    return 0;
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


Widget* Window::pointerWidget()
{
    return _pointerWidget;
}


const Widget* Window::pointerWidget() const
{
    return _pointerWidget;
}


void Window::setPointerWidget( Widget* widget ) 
{
    if( _pointerWidget == widget )
        return;

    if( _pointerWidget )
    {
        Widget* w = _pointerWidget;
        _pointerWidget = widget;

        LeaveEvent ev( w->vid() );
        Application::instance().loop().commitEvent(ev);
    }
    else
    {
        _pointerWidget = widget;
    }

    if( _pointerWidget )
    {
        EnterEvent ev( _pointerWidget->vid() );
        Application::instance().loop().commitEvent(ev);
    }
}


Widget* Window::focusWidget()
{
    return _focusWidget;
}


void Window::focusPrev()
{
    moveFocus(_focusList.rbegin(), _focusList.rend());
}


void Window::focusNext()
{
    moveFocus(_focusList.begin(), _focusList.end());
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
        
        if( w->acceptsFocus() )
        {
            setFocusWidget(w);
            return;
        }

        ++it;
    }

    // handles the case when the current focus widget has just been set to 
    // not accept focus and no other widget can accept focus either.
    if( _focusWidget && ! _focusWidget->acceptsFocus() )
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


void Window::update()
{
    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    update(rect);
}


void Window::update(const Gfx::RectF& rect)
{
    if( _impl )
    {
        Application::instance().screen().onUpdate(*this, rect);
    }
    else if(_parent)
    {
        _parent->onUpdate(*this, rect);
    }
}


void Window::onUpdate(Window& child, const Gfx::RectF& rect)
{
    _windowManager.onUpdate(child, rect);    
}


bool Window::isActive() const
{
    return _isActive;
}


void Window::activate()
{
    if(_impl) 
    {
        _impl->activate();
        Application::instance().screen().onActivate(*this);
    }
    else if(_parent)
    {
        _parent->onActivate(*this);
    }
    else
    {
        _isActive = true;
    }    
}


void Window::onActivate(Window& w)
{
    _windowManager.onActivate(w);
}


void Window::onActivateEvent(const ActivateEvent& ev)
{ 
    _isActive = ev.isActive();
}


bool Window::isVisible() const
{
    return _visible;
}


void Window::show(bool b)
{
    init(_parent); 
    
    if(_impl)
    {
        _impl->show(b);
        Application::instance().screen().onShow(*this, b);
    }
    else if(_parent)
    {
        _parent->onShow(*this, b);
    }
}


void Window::onShow( Window& w, bool visible )
{
    _windowManager.onShow(w, visible);
}


void Window::onShowEvent( const ShowEvent& ev )
{
    _visible = ev.visible();
}


bool Window::isEnabled() const
{
    return _enabled;
}


void Window::enable(bool e)
{
    if(_impl)
    {
        _impl->enable(e);
        Application::instance().screen().onEnable(*this, e);
    }
    else if(_parent)
    {
        _parent->onEnable(*this, e);
    }
    else
    {
        _enabled = e;
    }
}


void Window::onEnable( Window& w, bool enable )
{
    _windowManager.onEnable( w, enable );
}


void Window::onEnableEvent( const EnableEvent& ev )
{ 
    _enabled = ev.enabled();    
}


const Gfx::PointF& Window::position() const
{
    return _position;
}


void Window::move(const Gfx::PointF& p)
{
    if(_impl)
    {
        _impl->move(p);
        Application::instance().screen().onMove(*this, p);
    }
    else if(_parent)
    {
        _parent->onMove(*this, p);
    }
    else
    {
        _position = p;
    }
}


void Window::onMove(Window& w, const Gfx::PointF& to)
{   
    _windowManager.onMove(w, to);
}


void Window::onMoveEvent(const MoveEvent& ev)
{    
    _position = ev.position();
}


const Gfx::SizeF& Window::size() const
{
    return _size;
}


void Window::resize(const Gfx::SizeF& s)
{
    if(_impl)
    {
        _impl->resize(s);
        Application::instance().screen().onResize(*this, s);
    }
    else if(_parent)
    {
        _parent->onResize(*this, s);
    }
    else
    {
        _size = s;
    }
}


void Window::onResize(Window& w, const Gfx::SizeF& to)
{   
    _windowManager.onResize(w, to);
}


void Window::onResizeEvent(const ResizeEvent& s)
{
    _size = s.size();
    _surface.resize(s.size());

    if( _mainWidget )
        _mainWidget->resize(s.size());
}


bool Window::isClosed() const
{
    return _isClosed;       
}


void Window::close()
{
    if(_impl)
    {
        _impl->close();
        Application::instance().screen().onClosing(*this);
    }
    else if(_parent)
    {
        _parent->onClosing(*this);
    }
    else
    {
        _isClosed = true;
    }
}


void Window::onClosing(Window& w)
{         
    // request to close the window
    _windowManager.onClosing(w); 
}

 
void Window::onClose(Window& w)
{     
    // the window has been closed, clean up
    _windowManager.remove(w);
    update();
}


void Window::onCloseEvent(const CloseEvent& ev)
{
    if(_impl)
    {
        Application::instance().screen().onClose(*this);
    }
    else if(_parent)
    {
        _parent->onClose(*this);
    }

    _isClosed =  true;       

    deinit();
}


void Window::runModal()
{
    const Screen& screen = Application::instance().screen();

    const std::vector<Window*>& windows = _parent ? _parent->windows()
                                                  : screen.windows();
    Window* activeWindow = 0;
    std::map<Window*, bool> states;

    std::vector<Window*>::const_iterator it;
    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* w = (*it);

        if(w == this)
            continue;
        
        if( w->isActive() )
            activeWindow = w;

        states[w] = w->isEnabled();
        w->enable(false);
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
                
        std::map<Window*, bool>::iterator mapIt = states.find(w);
        if( mapIt != states.end() )
            mapIt->first->enable( mapIt->second );
    }
}


PixmapSurface& Window::surface()
{
    return _surface;
}


MainWindowImpl* Window::impl()
{
    return _impl;   
}


void Window::onPaint(const Gfx::RectF& rect)
{
    const double borderWidth = _windowManager.borderWidth();
    const double titleHeight = _windowManager.titleHeight();

    if( ! this->isVisible() )
        return;

    Painter painter(_surface);
    painter.setBrush( Pt::Gfx::Color(0.9f, 0.9f, 0.9f) );
    painter.fillRect(rect);

    if( mainWidget() )
        mainWidget()->onPaint(rect);

    std::vector<Window*>::iterator child;
    for(child = _windows.begin(); child != _windows.end(); ++child)
    {
        Gfx::PointF pos( rect.topLeft() - (*child)->position() );
        pos.subX( borderWidth );
        pos.subY( borderWidth + titleHeight );

        Gfx::RectF updateRect( pos, rect.size() );
        (*child)->onPaint(updateRect);
    }

    PaintEvent pev(vid(), rect);
    Application::instance().loop().commitEvent(pev);
}


void Window::onEvent(const Pt::Event& ev)
{
    _eventReady.send(ev ); 
}


void Window::onPaintEvent(const PaintEvent& ev)
{
    if( ! this->isVisible() )
        return; 

    _windowManager.paintEvent(ev);
}


void Window::onMouseEvent(const MouseEvent& ev)
{
    if( ! isEnabled() )
        return;

    if( _windowManager.mouseEvent(ev) )
        return;

    if( ! _mainWidget || 
        ! _mainWidget->isVisible() || 
        ! _mainWidget->isEnabled() )
    {
        // TODO: the window manager should do this
        Application::instance().setCursor( &Cursor::defaultCursor() ); 
        return;
    }

    Widget* widget = findWidget( ev.position() );

    // widget can be null
    setPointerWidget(widget);
  
    if(widget) 
    {
        MouseEvent clientEv(ev);
        clientEv.setId( widget->vid() );
        clientEv.setPosition( widget->fromWindow(ev.position()) );
        Application::instance().loop().commitEvent(clientEv); 
    }
}


void Window::onTouchEvent(const TouchEvent& ev)
{
}


void Window::onScrollEvent(const ScrollEvent& ev)
{
    if( ! _mainWidget )
        return;

    if( _pointerWidget )
    {
        ScrollEvent sev( _pointerWidget->vid() );
        Application::instance().loop().commitEvent(sev);
    }    
}


void Window::onKeyEvent(const KeyEvent& ev)
{
    if( ! isEnabled() )
        return;

    if( _windowManager.keyEvent(ev) )
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
    {
        KeyEvent kev(ev);
        kev.setId( _focusWidget->vid() );
        Application::instance().loop().commitEvent(kev);
    }
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


void Window::onEnterEvent(const EnterEvent& ev)
{
}


void Window::onLeaveEvent(const LeaveEvent& ev )
{
    _windowManager.leaveEvent(ev);

    setPointerWidget(0);
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

} // namespace

} // namespace
