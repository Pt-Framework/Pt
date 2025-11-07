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

#include <Pt/Forms/Window.h>
#include <Pt/Forms/WindowFrame.h>
#include <Pt/Forms/WindowManager.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/WindowStateEvent.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/Gfx/Algorithm.h>

namespace Pt {

namespace Forms {

Window::Window(WindowManager* parent, WindowType type)
: _frame(0)
, _wm(0)
, _show(false)
, _isActive(false)
, _enabled(true)
, _isClosed(false)
, _requestedPosition(0, 0)
, _requestedSize(80, 80)
, _autoSize(false)
, _autoCenter(false)
, _type(type)
, _state(WindowState::Normal)
, _isAbove(false)
{
    eventReceived() += Pt::slot(*this, &Window::onProcessActivateEvent);
    eventReceived() += Pt::slot(*this, &Window::onProcessCloseEvent);
    eventReceived() += Pt::slot(*this, &Window::onProcessWindowStateEvent);

    if(parent)
        setParent(*parent);
}


Window::~Window()
{
    unparent();
}


void Window::unparent()
{
    if( ! _frame )
        return;

    _frame->onRelease(*this);

    _wm->onRelease(*_frame);
    _wm->onDetach(*_frame);
    _wm = 0;

    delete _frame;
    _frame = 0;

    onSetParent(0);
}


void Window::setParent(WindowManager& wm)
{
    if(_wm == &wm)
        return;

    unparent();

    _isClosed = false;

    _frame = wm.onAttach(*this);
    _wm = &wm;

    onSetParent(_frame);
    
    _frame->onInit(*this);
    _wm->onInit(*_frame);
}


void Window::onConnect(Screen& screen)
{
    //std::clog << "\nCONNECT: " << title() << std::endl;

    Base::onConnect(screen);

    _frame->onSetSizeLimits(*this, minimumSize(), maximumSize());
    _frame->onSetState(*this, _state);
    _frame->onSetTitle(*this, _title);
    _frame->onSetIcon(*this, _icon);
    _frame->onSetAbove(*this, _isAbove);

    if(_state == WindowState::Normal)
    {
        if(_autoSize)
        {
            _lastAutoSize = measure(_autoSizePolicy);

            //std::clog << "\nauto-size CONNECT: " << title() << " " 
            //                                     << _lastAutoSize.width() << "x" 
            //                                     << _lastAutoSize.height() << std::endl;
            resize(_lastAutoSize);
        }
        else
        {
            _requestedSize = _frame->onResize(*this, _requestedSize);
        }

        if(_autoCenter)
        {
            _autoCenter = false;
            _frame->onAutoCenter(*this, &_requestedSize);
        }
        else
        {
            _frame->onMove(*this, _requestedPosition);
        }
    }
    
    // TODO: do not activate popups
    _frame->onActivate(*this, _isActive);
    _frame->onEnable(*this, _enabled);
    _frame->onShow(*this, _show);
}


void Window::onDisconnect()
{
    //std::clog << "DISCONNECT: " << title() << std::endl;

    Base::onDisconnect();
}


WindowManager* Window::windowManager()
{
    return _wm;
}


const WindowManager* Window::windowManager() const
{
    return _wm;
}


WindowFrame* Window::frame()
{
    return _frame;   
}


const WindowFrame* Window::frame() const
{
    return _frame;
}


void Window::getImage(Gfx::ImageSurface& imageSurface) const
{
    if(_frame)
        _frame->getImage(imageSurface);
}


Window::Type Window::type() const
{
    return _type;
}


const Gfx::Image& Window::icon() const
{
    return _icon;
}


void Window::setIcon(const Gfx::Image& icon)
{
    _icon = icon;

    if(_frame)
        _frame->onSetIcon(*this, _icon);
}


const std::string& Window::title() const
{
    return _title;
}


void Window::setTitle(const std::string& t)
{
    _title = t;

    if(_frame)
        _frame->onSetTitle(*this, _title);
}


bool Window::isAbove() const
{
    return _isAbove;
}


void Window::setAbove(bool above)
{
    _isAbove = above;

    if(_frame)
        _frame->onSetAbove(*this, above);
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

///////////////////////////////////////////////////////////////////////
// layouting
///////////////////////////////////////////////////////////////////////

void Window::autoCenter()
{
    if( ! isConnected() )
    {
        _autoCenter = true;
        return;
    }

    // TODO: onAutoCenter only needs a bool flag for enabled/disabled
    if(_frame)
        _frame->onAutoCenter(*this, &_requestedSize);  
}


void Window::autoSize(const SizePolicy& policy)
{   
    _autoSizePolicy = policy;
    _autoSize = true;

    if( ! isConnected() )
    {
        return;
    }

    _lastAutoSize = measure(_autoSizePolicy);
    
    //std::clog << "\nauto-size BEGIN: " << title() << " " 
    //                                   << _lastAutoSize.width() << "x" 
    //                                   << _lastAutoSize.height() << std::endl;
    resize(_lastAutoSize);
}


void Window::autoSize()
{   
    SizePolicy policy(SizePolicy::Preferred,
                      SizePolicy::Preferred);
    autoSize(policy);
}


Gfx::SizeF Window::onProcessMeasure()
{
    if(_autoSize)
    {
        _lastAutoSize = measure(_autoSizePolicy);

        //std::clog << "measure auto-size: " << title() << " " 
        //          << _lastAutoSize.width() << "x"  << _lastAutoSize.height() << std::endl;
    }

    return Base::onProcessMeasure();
}


void Window::onProcessLayout(const Gfx::RectF& rect)
{
    if(_autoSize)
    {
        //std::clog << "layout auto-size: " << title() << " " 
        //          << _lastAutoSize.width() << "x" << _lastAutoSize.height() << std::endl;
        
        if( _lastAutoSize != size() )
        {
            resize(_lastAutoSize);
        }
    }

    Base::onProcessLayout(rect);
}


Gfx::SizeF Window::onMeasure(const SizePolicy& policy)
{
    Gfx::SizeF size = Base::onMeasure(policy);
    
    //std::clog << "measure: " << title() << " " 
    //          << size.width() << "x" << size.height() << std::endl;
    
    return size;
}


void Window::onLayout(const Gfx::RectF& rect)
{
    //std::clog << "layout: " << title() << " " 
    //          << rect.width() << "x" << rect.height() << std::endl;

    Base::onLayout(rect);
}

///////////////////////////////////////////////////////////////////////
// Widget
///////////////////////////////////////////////////////////////////////

Widget* Window::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) || ! isVisible() )
        return 0;

    Widget* hit = Form::onHitTest(p);
    if(hit)
        return hit;

    return this;
}


Gfx::PointF Window::onToParent(const Gfx::PointF& pos) const
{
    if( ! _frame )
        return pos;

    return _frame->onFromWindow(*this, pos);
}


Gfx::PointF Window::onFromParent(const Gfx::PointF& pos) const
{
    if( ! _frame )
        return pos;

    return _frame->onToWindow(*this, pos);
}


void Window::onProcessEvent(const Pt::Event& ev)
{
    Base::onProcessEvent(ev);
}

///////////////////////////////////////////////////////////////////////
// invalidation
///////////////////////////////////////////////////////////////////////

void Window::onInvalidateEvent(const InvalidateEvent& ev)
{
    Base::onInvalidateEvent(ev);
}


void Window::onInvalidate()
{
    Base::onInvalidate();

    _backgroundBrush = background();

    repaint( bounds() );
}

///////////////////////////////////////////////////////////////////////
// painting
///////////////////////////////////////////////////////////////////////

void Window::onRequestRepaint(const Gfx::RectF& rect)
{
    //std::clog << "REPAINT: " << title() << std::endl;

    if(_frame)
        _frame->onRepaint(*this, rect);
}


void Window::onProcessPaintEvent(const PaintEvent& ev)
{
    const Gfx::RectF& rect = ev.rect();
    if( rect.isNull() )
        return;

    if( ! this->isVisible() )
        return;

    Base::onProcessPaintEvent(ev);
}


void Window::onPaintEvent(const PaintEvent& ev)
{    
    //static int nnn = 0;
    //std::clog << "PAINT EVENT: " << typeid(*this).name() << " " << ++nnn << std::endl;

    Base::onPaintEvent(ev);

    Gfx::Painter painter( surface() );
    painter.setBrush(_backgroundBrush);
    painter.fillRect( ev.rect() );
}

///////////////////////////////////////////////////////////////////////
// scaling
///////////////////////////////////////////////////////////////////////

void Window::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);
}


void Window::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void Window::onRescale(double scaling)
{   
    //std::clog << "WINDOW onRescale: " << title() << " " << scaling << std::endl;

    Base::onRescale(scaling);

    //
    // realign geometry
    //
    if(_state == WindowState::Normal)
    {
        // TODO: handle _autoSize and _autoCenter

        if( ! _autoCenter )
            move(_requestedPosition);
            
        if( ! _autoSize )
            resize(_requestedSize);
    }
}

///////////////////////////////////////////////////////////////////////
// geometry
///////////////////////////////////////////////////////////////////////

void Window::onRequestMove(const Gfx::PointF& pos)
{
    //std::clog << "MOVE: " << title() << " " << pos.x() << ", " << pos.y() << std::endl;

    // undo auto-center if window is not connected, but manually moved 
    // after being set to auto-center 
    _autoCenter = false;

    _requestedPosition = pos;

    setState(WindowState::Normal);

    if( _frame && isConnected() )
    {
        _frame->onMove(*this, _requestedPosition);
    }
}


void Window::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);
}


void Window::onMoveEvent(const MoveEvent& ev)
{
    //std::clog << "onMoveEvent: " << title() << " " 
    //          << ev.position().x() << "x" << ev.position().y() << std::endl;

    // NOTE: we skip View and pass on to Widget, because we need to avoid
    //       triggering a repaint of the window. Moving should only cause 
    //       the WindowFrame to blit its surface to the WindowManager.
    Widget::onMoveEvent(ev);
}


void Window::onSetSizeLimits(const Gfx::SizeF& minSize,
                             const Gfx::SizeF& maxSize)
{
    Base::onSetSizeLimits(minSize, maxSize);
    
    if(_frame)
        _frame->onSetSizeLimits(*this, minSize, maxSize);
}


bool Window::isAutoSizeActive(const Gfx::SizeF& s)
{
    //Gfx::SizeF alignedSize = scaling().align(_lastAutoSize);

    if( std::abs(_lastAutoSize.width() - s.width()) > 1.0 || 
        std::abs(_lastAutoSize.height() - s.height()) > 1.0)
    {
        //std::clog << "auto-size END: " << name() << " " 
        //          << s.width() << "x" << s.height() << std::endl;

        return false;
    }

    return true;
}


void Window::onRequestResize(const Gfx::SizeF& s)
{
    //std::clog << "\nRESIZE REQUESTED: " << title() << " " 
    //          << s.width() << "x" << s.height() << std::endl;

    _requestedSize = s;

    setState(WindowState::Normal);
    
    if( _frame && isConnected() )
    {
        _requestedSize = _frame->onResize(*this, _requestedSize);

        if(_autoSize)
            _autoSize = isAutoSizeActive(_requestedSize);
    }

    Base::onRequestResize(_requestedSize);
}


void Window::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void Window::onResizeEvent(const ResizeEvent& ev)
{
    //std::clog << "\nRESIZE: " << title() << " " 
    //          << ev.size().width() << "x" << ev.size().height() << std::endl;
    
    if(_autoSize)
    {
        if(_autoSize)
            _autoSize = isAutoSizeActive( ev.size() );
    }

    Base::onResizeEvent(ev);

    _requestedSize = ev.size();
}

///////////////////////////////////////////////////////////////////////
// visibility
///////////////////////////////////////////////////////////////////////

void Window::onRequestShow(bool b)
{
    _show = b;

    if( ! _frame )
    {
        Screen& screen = Application::instance().screen();
        screen.addWindow(*this);
        return;
    }

    if(_frame)
        _frame->onShow(*this, b);
}


void Window::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);
}


void Window::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
    invalidate();
}


void Window::onShow(bool visible)
{
    Base::onShow(visible);
}


void Window::showModal()
{
    Window* activeWindow = 0;

    Screen& screen = Application::instance().screen();
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

        if( activeWindow && activeWindow->id() == w->id() )
            activeWindow->activate();
    }
}

///////////////////////////////////////////////////////////////////////
// enabling
///////////////////////////////////////////////////////////////////////

void Window::onRequestEnable(bool e)
{
    _enabled = e;

    if(_frame)
        _frame->onEnable(*this, e);
}


void Window::onProcessEnableEvent(const EnableEvent& ev)
{
    bool isEnabled = ev.enabled();
    if( ! _enabled )
        isEnabled = false;

    EnableEvent eev(*this, isEnabled);
    Base::onProcessEnableEvent(eev);
}


void Window::onEnableEvent(const EnableEvent& ev)
{        
    Base::onEnableEvent(ev);
}


void Window::onEnable(bool e)
{
    Base::onEnable(e);

    invalidate();
}

///////////////////////////////////////////////////////////////////////
// activation
///////////////////////////////////////////////////////////////////////

bool Window::isActive() const
{
    return _isActive;
}


void Window::onRequestActivate(bool active)
{
    _isActive = active;

    if( _frame )
        _frame->onActivate(*this, active);
    else
        _isActive = active;
}


void Window::onProcessActivateEvent(const ActivateEvent& ev)
{
    onActivateEvent(ev);
}


void Window::onActivateEvent(const ActivateEvent& ev)
{
    _isActive = ev.isActive();
}

///////////////////////////////////////////////////////////////////////
// window state
///////////////////////////////////////////////////////////////////////

WindowState Window::state() const
{   
    return _state;
}


void Window::setState(const WindowState& s)
{
    _state = s;

    if( _frame && isConnected() )
        _frame->onSetState(*this, _state);
}


void Window::onProcessWindowStateEvent(const WindowStateEvent& ev)
{
    onWindowStateEvent(ev);
}


void Window::onWindowStateEvent(const WindowStateEvent& ev)
{
    _state = ev.state();
}

///////////////////////////////////////////////////////////////////////
// closing
///////////////////////////////////////////////////////////////////////

bool Window::isClosed() const
{
    return _isClosed;
}


void Window::close()
{
    if(_frame)
      _frame->onClose(*this);
}


Signal<>& Window::closed()
{
    return _closed;
}


void Window::onProcessCloseEvent(const CloseEvent& ev)
{
    onCloseEvent(ev);
}


void Window::onCloseEvent(const CloseEvent& ev)
{
    // TODO: find better solution to set base class visible
    ShowEvent sev(*this, false);
    Application::instance().processEvent(sev);
    
    unparent();
    
    _isClosed = true;
    _closed.send();
}

///////////////////////////////////////////////////////////////////////
// input
///////////////////////////////////////////////////////////////////////

bool Window::acceptsInput() const
{
    if( ! isEnabled() )
        return false;

    if( ! isVisible() )
        return false;

    return true;
}


void Window::onProcessMouseEvent(const MouseEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Base::onProcessMouseEvent(ev);
}


void Window::onProcessTouchEvent(const TouchEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Base::onProcessTouchEvent(ev);
}


void Window::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! acceptsInput() )
        return;
  
    Base::onProcessScrollEvent(ev);
}


void Window::onProcessEnterEvent(const EnterEvent& ev)
{
    Base::onProcessEnterEvent(ev);
}


void Window::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Base::onProcessLeaveEvent(ev);
}


void Window::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! acceptsInput() )
        return;
    
    Base::onProcessKeyEvent(ev);
}


bool Window::onMouseEvent(const MouseEvent& ev)
{
    return Base::onMouseEvent(ev);
}


bool Window::onTouchEvent(const TouchEvent& ev)
{ 
    return Base::onTouchEvent(ev);
}


bool Window::onScrollEvent(const ScrollEvent& ev)
{
    return Base::onScrollEvent(ev);
}


bool Window::onKeyEvent(const KeyEvent& ev)
{
    return Base::onKeyEvent(ev);
}


bool Window::onEnterEvent(const EnterEvent& ev)
{
    //std::clog << "ENTER: " << _title << " " << id() << std::endl;
    Application::instance().setCursor( &Cursor::defaultCursor() );
    return Base::onEnterEvent(ev);
}


bool Window::onLeaveEvent(const LeaveEvent& ev )
{
    //std::clog << "LEAVE: " << _title << " " << id() << std::endl;
    return Base::onLeaveEvent(ev);
}

} // namespace

} // namespace
