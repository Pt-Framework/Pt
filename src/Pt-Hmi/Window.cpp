
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
#include <Pt/Gfx/Painter.h>
#include <Pt/Hmi/FocusEvent.h>
#include <Pt/Hmi/WindowStateEvent.h>

#include <cassert>

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
, _layouts(0)
, _invalidates(0)
, _parent(0)
, _parentWindow(0)
, _screen(0)
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
, _topMost(false)
{
    _windowManager.init(*this);

    _eventReady += Pt::slot(*this, &Window::onKeyEvent);
    _eventReady += Pt::slot(*this, &Window::processMouseEvent);
    _eventReady += Pt::slot(*this, &Window::processTouchEvent);
    _eventReady += Pt::slot(*this, &Window::processScrollEvent);
    _eventReady += Pt::slot(*this, &Window::paintEvent);
    _eventReady += Pt::slot(*this, &Window::onActivateEvent);
    _eventReady += Pt::slot(*this, &Window::onCloseEvent);
    _eventReady += Pt::slot(*this, &Window::onEnterEvent);
    _eventReady += Pt::slot(*this, &Window::onLeaveEvent);
    _eventReady += Pt::slot(*this, &Window::onMoveEvent);
    _eventReady += Pt::slot(*this, &Window::onResizeEvent);
    _eventReady += Pt::slot(*this, &Window::layoutEvent);
    _eventReady += Pt::slot(*this, &Window::onShowEvent);
    _eventReady += Pt::slot(*this, &Window::onEnableEvent);
    _eventReady += Pt::slot(*this, &Window::onWindowStateEvent);
    _eventReady += Pt::slot(*this, &Window::onInvalidateEvent);

    if(parent)
        parent->add(*this);
}


Window::~Window()
{
    if(_mainWidget)
        View::remove(*_mainWidget);

    while( ! _windows.empty() )
       remove( *_windows.back() );

    deinit();
}


PixmapSurface& Window::surface()
{
    return _surface;
}


void Window::setScreen(Screen* screen)
{
    if(_screen == screen)
        return;

    _screen = screen;

    double scaling = scaleFactor();
    _surface.setScaleFactor(scaling);

    if(screen)
    {
        move(_position);
        resize(_size);
    }

    _windowManager.setScreen(screen);

    std::vector<Window*>::iterator w;
    for(w = _windows.begin(); w != _windows.end(); ++w)
    {
        (*w)->setScreen(_screen);
    }

    if(_mainWidget)
        _mainWidget->setScreen(screen);

    onSetScreen(screen);
}


void Window::onSetScreen(Screen* screen)
{
}


void Window::init(Window* parent)
{
    if (_init)
        deinit();

    _parent = parent;

    if( ! _parent )
    {
        _impl = new MainWindowImpl(_type);
        Screen& screen = Application::instance().screen();
        _parent = &screen;
    }

    _parent->onInit(*this);

    _init = true;
    _isClosed = false;

    setParent(parent);

    if( ! parent )
    {
        Screen& screen = Application::instance().screen();
        setScreen(&screen);
    }
    else
    {
        setScreen(parent->_screen);
    }

    if( ! _enabled )
    {
        // defered initialization
        enable(_enabled);
    }
    else if (parent && !parent->isEnabled() && isEnabled())
    {
        // disable indirectly, when parent is disabled
        EnableEvent eev(vid(), false);
        Application::instance().loop().commitEvent(eev);
    }
    else if (!_enabledState && _enabled)
    {
        // enable when indirectly disabled, but parent is enabled
        enable(true);
    }

    if (_isActive)
        activate();

    if (_impl)
    {
        _impl->setTitle(_title);
        _impl->setMinimumSize(_minimumSize);
        _impl->setMaximumSize(_maximumSize);
        _impl->setIcon(_icon);
        _impl->setState(_state);
    }

    setTopMost(_topMost);
    show(_visible);
}


void Window::deinit()
{
    if( ! _init )
        return;

    releasePointer();

    _parent->onDeinit(*this);
    
    setScreen(0);

    if(_impl)
    {
        delete _impl;
        _impl = 0;
    }

    _parent = 0;
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


// WindowBase* Window::parent()
// {
//    return _parent;
// }


// const WindowBase* Window::parent() const
// {
//     return _parent;
// }


Screen* Window::screen()
{
    return _screen;
}


const Screen* Window::screen() const
{
    return _screen;
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
    if( child.parentWindow() == this )
        return;

    // TODO: pass WindowBase (WindowManager interface)
    child.init(this);
    repaint();
    
    onAddWindow(child);
}


void Window::remove(Window& child)
{
    if( child.parentWindow() != this )
        return;

    child.init(0);
    repaint();
    
    onRemoveWindow(child);
}


void Window::onAddWindow(Window&)
{
}


void Window::onRemoveWindow(Window& )
{
}


void Window::onParentChanged(Window* )
{
}


void Window::onAddWidget(Widget& w)
{
}


void Window::onRemoveWidget(Widget& w)
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


const Widget* Window::content()  const 
{
    return _mainWidget;
}


void Window::setContent(Widget* widget)
{
    if(_mainWidget)
        View::remove(*_mainWidget);

    if(widget)
        View::add(*widget);
}


Window* Window::onGetWindow()
{
    return this;
}


Screen* Window::onGetScreen()
{
    return _screen;
}


void Window::onAttach(Widget& widget)
{
    _mainWidget = &widget;
    relayout();
}


void Window::onDetach(Widget& widget)
{
    if(_mainWidget == & widget)
        _mainWidget = 0;

    relayout();
}


void Window::onRaise(Widget& widget)
{
}


void Window::onEnable(Widget& widget, bool isEnable)
{
}


void Window::onShow(Widget& widget, bool isShown)
{
}


void Window::addWidget(Widget& w)
{
    addFocusWidget(w);

    setShortcut( w, w.shortcut() );
    setMnemonic( w, w.mnemonic() );

    onAddWidget(w);
}


void Window::removeWidget(Widget& w)
{
    // TODO: do this in Widget's destructor
    if( Application::instance().pointerWidget() == &w )
        Application::instance().setPointerWidget(0);

    if( _focusWidget == &w )
        setFocusWidget(0);

    removeFocusWidget(w);

    setShortcut(w, 0);
    setMnemonic(w, 0);

    onRemoveWidget(w);
}


Widget* Window::findWidget(const Gfx::PointF& pos, bool input)
{
    if( ! isVisible() || ! isEnabled() )
        return 0;

    if( ! _mainWidget )
        return 0;

    if( _mainWidget->geometry().contains(pos) )
    {
        Widget* widget = _mainWidget->findWidget(pos, input);

        if( ! input )
          return widget ? widget : _mainWidget;

      if( widget && widget->acceptsInput() )
          return widget;

      if( _mainWidget->acceptsInput() )
          return _mainWidget;
    }

    return 0;
}


Widget* Window::findWidget(const Gfx::PointF& pos)
{
  return findWidget(pos, false);
}


Widget* Window::findWidget(const std::string& name)
{
    if( ! _mainWidget )
        return 0;

    if( _mainWidget->name() == name )
        return _mainWidget;

    return _mainWidget->findWidget(name);
}


Widget* Window::findWidget(Pt::uint64_t vid)
{
    if( ! _mainWidget )
        return 0;

    if( _mainWidget->vid() == vid )
        return _mainWidget;

    return _mainWidget->findWidget(vid);
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

    if(widget && widget->focusPolicy() == Widget::NoFocus)
        return;

    if( _focusWidget )
    {
        if( _focusWidget->focusPolicy() == Widget::KeepFocus)
            return;

        FocusEvent fev(_focusWidget->vid(), false);
        Application::instance().loop().commitEvent(fev);
    }
    
    _focusWidget = widget;

    if( _focusWidget )
    {
        FocusEvent fev(_focusWidget->vid(), true);
        Application::instance().loop().commitEvent(fev);
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
        
        if( w->focusPolicy() != Widget::NoFocus )
        {
            setFocusWidget(w);
            return;
        }

        ++it;
    }

    // handles the case when the current focus widget has just been set to 
    // not accept focus and no other widget can accept focus either.
    if( _focusWidget &&  _focusWidget->focusPolicy() == Widget::NoFocus )
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

    // TODO: should we loose focus?
}


void Window::setFocusIndex(Widget& , size_t)
{
    std::sort(_focusList.begin(), _focusList.end(), &lowerFocusIndex);
}


double Window::onScaleFactor() const
{
    if( ! _init )
        return 1.0;

    double scaling = _impl ? _impl->scaleFactor() : 1.0;

    return _parent->scaleFactor() * scaling;
}


Visual* Window::onParent() const
{
    return _parent;
}


Gfx::PointF Window::onToParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->toHost(*this, pos);
}


Gfx::PointF Window::onFromParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->fromHost(*this, pos);
}


Gfx::PointF Window::onToHost(const Window& w, const Gfx::PointF& pos) const
{
    return _windowManager.toParent(w, pos);
}


Gfx::PointF Window::onFromHost(const Window& w, const Gfx::PointF& pos) const
{
    return _windowManager.fromParent(w, pos);
}


const Gfx::Brush& Window::background() const
{
    return _background ? *_background
                       : Application::instance().styleOptions().background();
}


void Window::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    invalidate();
}


Pt::Signal<const Pt::Event&>& Window::eventReady()
{
    return _eventReady;
}


const Gfx::PointF& Window::onPosition() const
{
    return _position;
}


const Gfx::SizeF& Window::onSize() const
{
    return _size;
}


void Window::invalidate()
{
    ++_invalidates;

    InvalidateEvent ev(vid());
    Application::instance().loop().commitEvent(ev);
}


void Window::onInvalidateEvent(const InvalidateEvent& ev)
{
    --_invalidates;

    if(_invalidates > 0)
      return;

    onInvalidate();

    //relayout();
}


void Window::onInvalidate()
{
    _backgroundBrush = background();

    repaint();
}


void Window::onRelayout()
{   
    _layouts++;

    // TODO: if window is set to autoSize then measure the content
    //       first and then resize the window

    LayoutEvent ev( vid() );
    Application::instance().loop().commitEvent(ev); 
}


void Window::onRelayout(Widget& widget)
{
    onRelayout();
}


void Window::layoutEvent(const LayoutEvent& ev)
{
    --_layouts;

    if(_layouts > 0)
        return;

    // 1. Pass
    SizePolicy policy(SizePolicy::Fixed, SizePolicy::Fixed);
    policy.setSize(_size);
    measure(policy);

    // 2. Pass
    Gfx::RectF rect(_position, _size);
    layout(rect);
}


//Gfx::SizeF Window::setSizePolicy(const SizePolicy& policy)
//{
//    if(_mainWidget)
//        return _mainWidget->measure(policy);
//
//    return policy.size();
//}


// onMeasure
Gfx::SizeF Window::measure(const SizePolicy& policy)
{ 
    if( _mainWidget )
    {
        return _mainWidget->measure(policy);
    }

    return policy.size();
}

// onLayout
void Window::layout(const Gfx::RectF& rect)
{
    // TODO: consider moving and resizing the window according to the layout
    //       rect. In resize() and move() only record the new size and 
    //       position, then call relayout() to perform the move or resize.

    if( _mainWidget )
    {
        Gfx::RectF widgetRect( rect.size() );
        _mainWidget->layout(widgetRect);
    }
}


void Window::onRepaint(const Gfx::RectF& rect)
{
    //std::clog << "REPAINT: " << title() << std::endl;

    _damageRect.unify(rect);

    //if(_parent)
    //{
    //    Gfx::PointF parentPos = toParent( rect.topLeft() );
    //    Gfx::RectF parentRect( parentPos, rect.size() );
    //   _parent->repaint(parentRect);
    //}

    if(_parent)
       _parent->onRepaint(*this, rect);
}


void Window::onRepaint(Widget& widget, const Gfx::RectF& viewRect) 
{
    Gfx::PointF pos = viewRect.topLeft() + widget.position();
    Gfx::RectF rect( pos, viewRect.size() );

    onRepaint(rect);
}


void Window::onRepaint(Window& w, const Gfx::RectF& windowRect)
{
     Pt::Gfx::PointF pos = toHost( w, windowRect.topLeft() );
     Gfx::RectF rect( pos, windowRect.size() );
   
     onRepaint(rect);
}


void Window::paintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& rect = _damageRect;
    //const Gfx::RectF& rect = ev.rect();

    if( rect.isNull() )
        return;

    if( ! this->isVisible() )
        return;

    //if( _damageRect.isNull() )
    //    return;

    //onPaintContent(_damageRect);
    
    // onPaint
    onPaintContent(rect);

    //
    // paint child windows
    //
    std::vector<Window*>::iterator child;
    for(child = _windows.begin(); child != _windows.end(); ++child)
    {
        Gfx::PointF winPos = (*child)->fromParent( rect.topLeft() );
        Gfx::RectF winRect( winPos, rect.size() );

        winRect = winRect.intersect( Gfx::RectF( (*child)->size() ) );

        //paintContent(**child, winRect);
        PaintEvent pev((*child)->vid(), winRect);
        (*child)->processEvent(pev);

        // _windowManager.paintWindow(**child, _surface, rect);
    }

    _windowManager.paint(_surface, rect);

    _damageRect.clear();
}

// onPaint
void Window::onPaintContent(const Gfx::RectF& rect)
{
    if( rect.isNull() )
        return;

    if( ! this->isVisible() )
        return;

    //std::clog << "  PAINT(" << title() << "): " 
    //          << rect.width() << "x" << rect.height() << std::endl;
    
    //
    // paint window content
    //
    PaintEvent pev(vid(), rect);
    onPaintContent(pev);

    //
    // paint main child widget
    //
    Widget* widget = mainWidget();
    if(widget)
    {
        // clip widget update rect
        Gfx::RectF updateRect = mainWidget()->geometry().intersect(rect);

        // paint main widget rect
        if( ! updateRect.isNull() )
        {
            //paintContent(*widget, updateRect);
            widget->onPaintContent(updateRect);
        }
    }
}

// onPaintContent (window specific)
void Window::onPaintContent(const PaintEvent& ev)
{
    const Gfx::RectF& rect = ev.rect();

    Gfx::Painter painter(_surface);
    painter.setBrush(_backgroundBrush);
    painter.fillRect(rect);
}


bool Window::isActive() const
{
    return _isActive;
}


void Window::activate(bool active)
{
    _isActive = active;

    if( ! _init )
        return;

    _parent->onActivate(*this, active);

    ActivateEvent aev(vid(), active);
    Application::instance().loop().commitEvent(aev);
}


void Window::onActivate(Window& w, bool active)
{
    if(active)
        activate();

    _windowManager.onActivate(&w, active);
}


void Window::onActivateEvent(const ActivateEvent& ev)
{ 
    //_isActive = ev.isActive();
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
    
    invalidate();

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

    Window* activeWindow = 0;

    const Screen& screen = Application::instance().screen();
    const std::vector<Window*>& windows = screen.windows();

    // TODO: only disable enabled windows

    std::vector<Window*>::const_iterator it;
    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* w = (*it);

        w->enable(false);

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

    // TODO: only enable previously disabled windows

    for(it = windows.begin(); it != windows.end(); ++it)
    {
        Window* w = (*it);

        w->enable(true);

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
    _enabled = e;
    _enabledState = e;

    if( ! _init )
        return;

    _parent->onEnable(*this, e);
}


void Window::onEnable( Window& w, bool enable )
{
    _windowManager.onEnable(w, enable);
}


void Window::onEnableEvent(const EnableEvent& ev)
{        
    //if( ! ev.enabled() )
    //    _enabledState = false;
    //else
    //    _enabledState = _enabled;

    _enabledState = ev.enabled();

    for( size_t i = 0; i < _windows.size(); ++i)
    {
        Window* w = _windows[i];

        // skip directly disabled children, because they are either already
        // disabled or they should not be enabled
        if( ! w->_enabled )
            continue;

        EnableEvent eev( w->vid(), ev.enabled());
        Application::instance().loop().commitEvent(eev);
    }  
}


void Window::grabPointer()
{
    if(_init)
        Application::instance().grabPointer(*this);
}


void Window::releasePointer()
{
    Application::instance().releasePointer(*this);
}


void Window::move(const Gfx::PointF& pos)
{
    //
    // if not attached store position until later
    //
    if( ! _screen )
    {
        _position = pos;
        return;
    }

    //
    // align to physical pixel grid
    //
    //Gfx::PointF aligedPos = align(pos);

    //_position = aligedPos;

    //
    // request window move
    //
    _parent->moveWindow(*this, pos);
}


void Window::onMove(Window& w, const Gfx::PointF& pos)
{   
    //
    // align to physical pixel grid
    //
    Gfx::PointF aligedPos = align(pos);

    //
    // update window manager
    //
    Gfx::RectF updateRect = _windowManager.frameRect(w);

    _windowManager.onMove(w, aligedPos);

    Gfx::RectF movedRect = _windowManager.frameRect(w);
    updateRect.unify(movedRect);

    //
    // send move event
    //
    MoveEvent mev( w.vid(), aligedPos );
    Application::instance().loop().commitEvent(mev);

    //
    // request repaint
    //
    repaint(updateRect);
}


void Window::onMoveEvent(const MoveEvent& ev)
{    
    _position = ev.position();
}


void Window::resize(const Gfx::SizeF& s)
{
    if( ! _screen )
    {
        _size = s;
        return;
    }

    //
    // align to physical pixel grid
    //
    //Gfx::SizeF alignedSize = align(s);

    //_size = alignedSize;

    //
    // request window resize
    //
    _parent->resizeWindow(*this, s);
}


void Window::onResize(Window& w, const Gfx::SizeF& s)
{   
    //
    // align to physical pixel grid
    //
    Gfx::SizeF alignedSize = align(s);

    //
    // maximum width and height
    //
    if( alignedSize.width() > w.maximumSize().width() )
        alignedSize.setWidth( w.maximumSize().width() );

    if( alignedSize.height() > w.maximumSize().height() )
        alignedSize.setHeight( w.maximumSize().height() );

    if( alignedSize.width() < w.minimumSize().width() )
        alignedSize.setWidth( w.minimumSize().width() );

    if( alignedSize.height() < w.minimumSize().height() )
        alignedSize.setHeight( w.minimumSize().height() );

    //
    // send resize event
    //
    ResizeEvent rev( w.vid(), alignedSize );
    Application::instance().loop().commitEvent(rev);

    //
    // update window manager
    //
    _windowManager.onResize(w, alignedSize);
}


void Window::onResizeEvent(const ResizeEvent& ev)
{
    //std::clog << "RESIZE EVENT: " 
    //          << ev.size().width() << "x" << ev.size().height() << std::endl;

    _size = ev.size();

    _surface.resize( ev.size() );

    relayout();

    // NOTE: window manager also sends a paint event for the 
    //       updated window frame rect
    repaint();
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
    Gfx::RectF updateRect = _windowManager.frameRect(w);

    // the window has been closed, clean up
    _windowManager.onClose(w);

    repaint(updateRect);
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
    Gfx::RectF updateRect = _windowManager.frameRect(w);
    
    _windowManager.onFrameChanged(w);

    Gfx::RectF changedRect = _windowManager.frameRect(w);
    updateRect.unify(changedRect);

    repaint(updateRect);
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

    // TODO: notify parent?
}


bool Window::isTopMost() const
{
    return _topMost;
}


void Window::setTopMost(bool top)
{
    if( _impl )
          _impl->setTopMost(top);

    _topMost = top;

    if(_parent)
        _parent->onStateChanged(*this);
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

    // TODO: notify parent?
}


Window::State Window::state() const
{   
    return _state;
}


void Window::setState(Window::State s)
{
    if( _impl )
        _impl->setState(s);

    _state = s;

    if(_parent)
        _parent->onStateChanged(*this);
}


void Window::onStateChanged(Window& w)
{
    _windowManager.onStateChanged(w);
}


void Window::onWindowStateEvent(const WindowStateEvent& ev)
{
    _state = ev.state();
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


void Window::processMouseEvent(const MouseEvent& ev)
{
    if( _windowManager.mouseEvent(ev) )
        return;

    Widget* widget = findWidget( ev.position(), true );
    if( ! widget )
    {
        Application::instance().setCursor( &Cursor::defaultCursor() ); 
    }

    if( ev.isPress() )
    {
        if( ! widget )
        {
            Application::instance().inputMethod().finish();
        }
        else if( ! widget->isTextInput() )
        {
            Application::instance().inputMethod().finish();
        }
    }

    // widget may be null to unset the pointer widgetv
    Application::instance().setPointerWidget(widget);

    if( widget && widget->isEnabled() )
    {
        MouseEvent clientEv(ev);
        clientEv.setId( widget->vid() );
        clientEv.setPosition( widget->fromWindow(ev.position()) );

        // pass down responder chain
        widget->mouseEvent(clientEv);
        //Application::instance().loop().commitEvent(clientEv);
    }
}


Responder* Window::onNextResponder()
{
    if(_parent)
        return _parent;

    return 0;
}


Gfx::PointF Window::onToNextResponder(const Gfx::PointF& pos)
{
    return toParent(pos);
}


bool Window::onMouseEvent(const MouseEvent& ev)
{
    return false;
}


void Window::touchEvent(const TouchEvent& ev)
{
}


void Window::processTouchEvent(const TouchEvent& tev)
{ 
    onTouchEvent(tev);
}


void Window::onTouchEvent(const TouchEvent& tev)
{ 
    if( _windowManager.touchEvent(tev) )
        return;

    Widget* widget = findWidget( tev.position(), true );

    if( tev.isPress() )
    {
        if( ! widget)
        {
            Application::instance().inputMethod().finish();
        }
        else if( ! widget->isTextInput() )
        {
            Application::instance().inputMethod().finish();
        }
    }

    // widget may be null to unset the pointer widget
    if( tev.isRelease() )
    {
        Application::instance().setPointerWidget(0);
    }
    else
        Application::instance().setPointerWidget(widget);

    if( widget && widget->isEnabled() )
    {
        TouchEvent ev(tev);
        ev.setId( widget->vid() );
        ev.setPosition( widget->fromWindow(tev.position()) );
        //Application::instance().loop().commitEvent(ev); 
        widget->touchEvent(ev);
    }
}


void Window::scrollEvent(const ScrollEvent& ev)
{
}


void Window::processScrollEvent(const ScrollEvent& sev)
{
    onScrollEvent(sev);
}


void Window::onScrollEvent(const ScrollEvent& sev)
{
    if( _windowManager.scrollEvent(sev) )
        return;

    if( _focusWidget )
    {
        ScrollEvent ev(sev);
        ev.setId( _focusWidget->vid() );
        Application::instance().loop().commitEvent(ev);
        return;
    }

    Widget* widget = Application::instance().pointerWidget();

    if( widget && widget->isEnabled() )
    {
        ScrollEvent ev(sev);
        ev.setId( widget->vid() );
        //Application::instance().loop().commitEvent(ev);
        widget->scrollEvent(ev); 
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

    if( ev.isPress() && ev.key().modifiers() == Key::Alt )
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
        if(ev.key().code() == Key::Tab && ev.isPress() )
        {
            focusNext();

            if(_focusWidget)
            {
                _focusWidget->repaint();

                if( ! _focusWidget->isTextInput() )
                    Application::instance().inputMethod().finish();
            }
        }
    }
}


void Window::onEnterEvent(const EnterEvent& ev)
{
    if( ! isEnabled() )
        return; 
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
