/*   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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
// - show and enable event für widget
// - FocusEvent

namespace {

bool lowerFocusIndex(Pt::Hmi::Widget* a, Pt::Hmi::Widget* b )
{
    return a->focusIndex() < b->focusIndex();
}

} // namespace

namespace Pt {

namespace Hmi {

Window::Window(Window* parent, Window::Type type)
: _impl(0)
, _parent(0)
, _parentWindow(0)
, _mainWidget(0)
, _focusWidget(0)
, _init(false)
, _visible(false)
, _isActive(false)
, _enabled(true)
, _enabledState(true)
, _isClosed(false)
, _position(0,0)
, _size(10,10)
, _type(type)
, _minimumSize(0, 0)
, _maximumSize(64000, 64000)
, _state(Normal)
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


PixmapSurface& Window::surface()
{
    return _surface;
}


void Window::init(Window* parent)
{
    if(_init)
        deinit();

    _parent = parent;
    setParent(parent);

    if( ! _parent )
    {
        _impl = new MainWindowImpl(_type);
        _parent = &Application::instance().screen();
    }

    _parent->onInit(*this);
    
    _init = true;
    _isClosed = false;
    
    move(_position);
    resize(_size);
    enable(_enabled);

    if( _isActive )
        activate();

    if( _impl)
    {
        _impl->setTitle(_title);
        _impl->setMinimumSize(_minimumSize);
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

    releaseMouse();

    _parent->onDeinit(*this);

    if(_impl)
    {
        delete _impl;
        _impl = 0;
    }

    _init = false;
}


void Window::onInit(Window& w)
{
    _windowManager.add(w);
    _windows.push_back(&w);
}


void Window::onDeinit(Window& w)
{
    _windowManager.remove(w);

    std::vector<Window*>::iterator it;
    it = std::remove(_windows.begin(), _windows.end(), &w);
    _windows.erase(it, _windows.end());
}


WindowBase* Window::parent()
{
   return _parent;
}


const WindowBase* Window::parent() const
{
    return _parent;
}


Window& Window::mainWindow()
{
    Window* mainWindow = this;

    while( mainWindow->parentWindow() )
        mainWindow = mainWindow->parentWindow();

   return *mainWindow;
}


const Window& Window::mainWindow() const
{
    const Window* mainWindow = this;

    while( mainWindow->parentWindow() )
        mainWindow = mainWindow->parentWindow();

   return *mainWindow;
}


Window* Window::parentWindow()
{
   return _parentWindow;
}


const Window* Window::parentWindow() const
{
   return _parentWindow;
}


const std::vector<Window*>& Window::windows() const
{
    return _windows;
}


void Window::add(Window& child)
{
    if( child.parent() == this )
        return;

    child.init(this);
    update();
    
    onAddWindow(child);
}


void Window::remove(Window& child)
{
    if( child.parent() != this )
        return;

    child.init(0);
    update();
    
    onRemoveWindow(child);
}


void Window::onAddWindow(Window& w)
{
}


void Window::onRemoveWindow(Window& w)
{
}


void Window::onParentChanged(Window* w)
{
}


void Window::setParent(Window* parent)
{
    if(_parentWindow == parent)
        return;

    _parentWindow = parent;
    onParentChanged(parent);
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
    // TODO: do this in Widget's destructor
    if( Application::instance().pointerWidget() == &w )
        Application::instance().setPointerWidget(0);

    if( _focusWidget == &w )
        _focusWidget = 0;

    if(_mainWidget == &w)
        _mainWidget = 0;

    removeFocusWidget(w);

    setShortcut(w, 0);
    setMnemonic(w, 0);
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


Gfx::PointF Window::toParent(const Gfx::PointF& pos) const
{
    if( ! _init )
        return Gfx::PointF(0, 0);

    return _parent->onToParent(*this, pos);
}


Gfx::PointF Window::onToParent(const Window& w, const Gfx::PointF& pos) const
{
    return _windowManager.toParent(w, pos);
}


Gfx::PointF Window::fromParent(const Gfx::PointF& pos) const
{
    if( ! _init )
        return Gfx::PointF(0, 0);

    return _parent->onFromParent(*this, pos);
}


Gfx::PointF Window::onFromParent(const Window& w, const Gfx::PointF& pos) const
{
    return _windowManager.fromParent(w, pos);
}


Gfx::PointF Window::toScreen(const Gfx::PointF& pos) const
{
    if( ! _init )
        return Gfx::PointF(0, 0);

    Gfx::PointF p = toParent(pos);

    if(_parentWindow)
        return _parentWindow->toScreen(p);

    return p;    
}


Gfx::PointF Window::fromScreen(const Gfx::PointF& pos) const
{
    if( ! _init )
        return Gfx::PointF(0, 0);

    Gfx::PointF p = fromParent(pos);
    
    if(_parentWindow)
        return _parentWindow->fromScreen(p);

    return p;
}


void Window::update()
{
    Gfx::RectF rect( Gfx::PointF(0,0), size() );
    update(rect);
}


void Window::onUpdate(const Gfx::RectF& rect)
{
    if( ! _init )
        return;
    
    _damageRect.unify(rect);

    _parent->onUpdate(*this, rect);
}


void Window::onUpdate(Window& child, const Gfx::RectF& rect)
{
    _windowManager.onUpdate(child, rect);
}


void Window::repaint()
{
    if( _damageRect.isNull() )
        return;
    
    if( ! this->isVisible() )
        return;  

    onPaintBackground(_damageRect);

    if( mainWidget() )
        mainWidget()->repaint(_damageRect);

    std::vector<Window*>::iterator child;
    for(child = _windows.begin(); child != _windows.end(); ++child)
    {
        (*child)->repaint();
    }

    PaintEvent pev(vid(), _damageRect);
    Application::instance().loop().commitEvent(pev);

    _damageRect.clear();
}


void Window::onPaintEvent(const PaintEvent& ev)
{
    if( ! this->isVisible() )
        return; 

    onPaintContent( ev.rect() );

    if(_impl)
        _impl->paint( ev.rect() );
}


void Window::onPaintBackground(const Gfx::RectF& rect)
{
    Painter painter(_surface);
    painter.setBrush( Pt::Gfx::Color(0.9f, 0.9f, 0.9f) );
    painter.fillRect(rect);
}


void Window::onPaintContent(const Gfx::RectF& rect)
{
    _windowManager.paint(_surface, rect);
}


bool Window::isActive() const
{
    return _isActive;
}


void Window::activate()
{
    if( ! _init )
    {
        _isActive = true;
        return;
    }

    if(_parentWindow)
        _parentWindow->activate();

    _parent->onActivate(*this);
}


void Window::onActivate(Window& w)
{
    _windowManager.onActivate(&w);
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
    if( ! _init )
    {
        init(_parentWindow);
    }
    
    _parent->onShow(*this, b);
}


void Window::onShow( Window& w, bool visible )
{
    _windowManager.onShow(w, visible);
}


void Window::onShowEvent( const ShowEvent& ev )
{
    _visible = ev.visible();

    // TODO: release mouse when hidden?
}


void Window::showModal()
{
    deinit();

    const Screen& screen = Application::instance().screen();

    const std::vector<Window*>& windows = screen.windows();
    Window* activeWindow = 0;

    std::vector<Window*>::const_iterator it;

    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* w = (*it);

        w->onEnable(false);

        if( w->isActive() )
            activeWindow = w;            
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

        w->onEnable(true);

        if( activeWindow && activeWindow->vid() == w->vid() )
            activeWindow->activate();                     
    }
}


bool Window::isEnabled() const
{
    return _enabledState && _enabled;
}


void Window::enable(bool e)
{
    if( ! _init )
    {
        _enabled = e;
        _enabledState = e;
        return;
    }

    _parent->onEnable(*this, e);
}


void Window::onEnable( Window& w, bool enable )
{
    _windowManager.onEnable( w, enable );
}


void Window::onEnableEvent( const EnableEvent& ev )
{ 
    _enabled = ev.enabled();    
    _enabledState =  ev.enabled();
}


void Window::onEnable(bool e)
{
    if( ! e)
        _enabledState = false;
    else
        _enabledState = _enabled;
}


void Window::grabMouse()
{
    if(_init)
        Application::instance().grabMouse(*this);
}


void Window::releaseMouse()
{
    Application::instance().releaseMouse(*this);
}


const Gfx::PointF& Window::position() const
{
    return _position;
}


void Window::move(const Gfx::PointF& p)
{
    if( ! _init )
    {
        _position = p;
        return;
    }

    _parent->onMove(*this, p);
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
    if( ! _init )
    {
        _size = s;
        return;
    }

    _parent->onResize(*this, s);
}


void Window::onResize(Window& w, const Gfx::SizeF& to)
{   
    _windowManager.onResize(w, to);
}


void Window::onResizeEvent(const ResizeEvent& ev)
{
    _size = ev.size();
    _surface.resize(ev.size());

    if(_mainWidget)
        _mainWidget->resize( ev.size() );
}


bool Window::isClosed() const
{
    return _isClosed;
}


void Window::close()
{
    if( ! _init )
    {
        _isClosed = true;
        return;
    }

    _parent->onClosing(*this);
}


void Window::onClosing(Window& w)
{         
    // request to close the window
    _windowManager.onClosing(w);
}

 
void Window::onClose(Window& w)
{     
    // the window has been closed, clean up
    _windowManager.onClose(w);
}


void Window::onCloseEvent(const CloseEvent& ev)
{
    _parent->onClose(*this);

    _isClosed =  true;
    _visible = false;

    deinit();
}


Window::Type Window::type() const
{
    return _type;
}


void Window::setType(Type type)
{
    if( _impl )
        _impl->setType(type);

    _type = type;

    if(_parent)
        _parent->onFrameChanged(*this);
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

    if(_parent)
        _parent->onFrameChanged(*this);
}


const std::string& Window::title() const
{
    return _title;
}


void Window::setTitle(const std::string& t)
{
    if( _impl )
        _impl->setTitle(t);

    _title = t;

    if(_parent)
        _parent->onFrameChanged(*this);
}


void Window::onFrameChanged(Window& w)
{
    _windowManager.onFrameChanged(w);
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


Window::State Window::state() const
{   
    return _state;
}


void Window::setState(Window::State s)
{
    if( _impl )
        _impl->setState(s);

    // TODO: need a Maximize/Minimize event
    _state = s;

    if(_parent)
        _parent->onStateChanged(*this);
}


void Window::onStateChanged(Window& w)
{
    _windowManager.onStateChanged(w);
}


MainWindowImpl* Window::impl()
{
    return _impl;   
}


const MainWindowImpl* Window::impl() const
{
    return _impl;   
}


void Window::onEvent(const Pt::Event& ev)
{
    _eventReady.send(ev ); 
}


void Window::onMouseEvent(const MouseEvent& ev)
{    
    if( _windowManager.mouseEvent(ev) )
        return;    

    if( ! _mainWidget )
    {
        Application::instance().setCursor( &Cursor::defaultCursor() ); 
        return;
    }

    Widget* widget = findWidget( ev.position() );

    // widget may be null to unset the pointer widget
    Application::instance().setPointerWidget(widget);

    if(widget && widget->isEnabled())
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

    Widget* pointerWidget = Application::instance().pointerWidget();
    if( pointerWidget->window() == this )
    {
        ScrollEvent sev( pointerWidget->vid() );
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
    if( isEnabled() )
        return;

    Application::instance().setCursor( &Cursor::defaultCursor() ); 
}


void Window::onLeaveEvent(const LeaveEvent& ev )
{
    _windowManager.leaveEvent(ev);

    Application::instance().setPointerWidget(0);
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
