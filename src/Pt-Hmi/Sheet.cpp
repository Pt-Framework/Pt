/* Copyright (C) 2015 Marc Boris Duerner 
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include <Pt/Hmi/Sheet.h>
#include <Pt/Hmi/Form.h>
#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Application.h>

namespace {

bool lowerFocusIndex(Pt::Hmi::Widget* a, Pt::Hmi::Widget* b)
{
    return a->focusIndex() < b->focusIndex();
}

} // namespace

namespace Pt {

namespace Hmi {

Sheet::Sheet()
: _eventReceived()
, _parent(0)
, _mainWidget(0)
, _surface(0)
, _nextResponder(0)
, _invalidates(0)
, _layouts(0)
, _enabled(true)
, _enabledState(true)
, _pointer(0)
, _capture(0)
, _active(0)
, _focusWidget(0)
{
    _eventReceived += Pt::slot(*this, &Sheet::onProcessInvalidateEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessRescaleEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessRelayoutEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessPaintEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessMoveEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessResizeEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessEnableEvent);

    _eventReceived += Pt::slot(*this, &Sheet::onProcessMouseEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessTouchEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessScrollEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessEnterEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessLeaveEvent);
    _eventReceived += Pt::slot(*this, &Sheet::onProcessKeyEvent);
}


Sheet::~Sheet()
{
    if(_mainWidget)
        _mainWidget->setParent(0);

    setParent(0);
}


void Sheet::setParent(Form* parent)
{
    if(_parent == parent)
        return;

    unparent();

    if(parent)
    {
        parent->onAttach(*this);
        _parent = parent;

        _parent->onInit(*this);

        _parent->onMove(*this, _requestedGeometry.topLeft());
        _parent->onResize(*this, _requestedGeometry.size());
    }
}


void Sheet::unparent()
{
    if( ! _parent)
        return;

    release();

    _parent->onRelease(*this);
    _parent->onDetach(*this);
    _parent = 0;
}


void Sheet::onRelease()
{
    setPointer(false);
    setCapture(false);

    if(_mainWidget)
        _mainWidget->release();
}


void Sheet::setSurface(Gfx::PaintSurface* surface)
{
    if(_mainWidget)
        _mainWidget->setSurface( surface, _mainWidget->position() );

    _surface = surface;
}


Widget* Sheet::content() 
{
    return _mainWidget;
}


const Widget* Sheet::content()  const 
{
    return _mainWidget;
}


void Sheet::setContent(Widget* widget)
{
    if(_mainWidget)
    {
        _mainWidget->setParent(0);
    }

    if(widget)
    {
        widget->setParent(this);
    }
}


Widget* Sheet::findWidget(const Gfx::PointF& pos)
{
    if( _mainWidget )
    {
        if( _mainWidget->geometry().contains(pos) )
        {
            Gfx::PointF p = onToWidget(*_mainWidget, pos);
            Widget* found = _mainWidget->findWidget(p);
            return found ? found : _mainWidget;
        }
    }

    return 0;
}


Widget* Sheet::findWidget(const std::string& name)
{
    if( ! _mainWidget )
        return 0;

    if( _mainWidget->name() == name )
        return _mainWidget;

    return _mainWidget->findWidget(name);
}


Widget* Sheet::findWidget(Pt::uint64_t vid)
{
    if( ! _mainWidget )
        return 0;

    if( _mainWidget->vid() == vid )
        return _mainWidget;

    return _mainWidget->findWidget(vid);
}


Visual* Sheet::onGetParent() const
{
    if( ! _parent )
        return 0;

    return &_parent->visual();
}


Gfx::PointF Sheet::onToParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->onFromSheet(*this, pos);
}


Gfx::PointF Sheet::onFromParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->onToSheet(*this, pos);
}


void Sheet::onEvent(const Pt::Event& ev)
{
    _eventReceived.send(ev);
}


void Sheet::setNextResponder(Responder* r)
{
    _nextResponder = r;
}


Responder* Sheet::onNextResponder()
{
    return _nextResponder;
}


Gfx::PointF Sheet::onToWidget(const Widget& widget, const Gfx::PointF& pos) const
{
    const View* parentView = widget.parent();

    if( parentView == this || ! parentView )
        return pos - widget.position();

    return pos - parentView->toWidget(widget, pos);    
}


Gfx::PointF Sheet::onFromWidget(const Widget& widget, const Gfx::PointF& pos) const
{
    const View* parentView = widget.parent();

    if( parentView == this || ! parentView )
        return pos + widget.position();

    return pos + parentView->fromWidget(widget, pos);
}


Visual& Sheet::onGetVisual()
{
    return *this;
}


void Sheet::onAttach(Widget& widget)
{
    _mainWidget = &widget;
    
    onRelayout(widget);
}


void Sheet::onDetach(Widget& widget)
{
  if(_active == &widget)
      _active = 0;

    if(_pointer == &widget)
        _pointer = 0;

    if(_mainWidget == &widget)
        _mainWidget = 0;

    if(_capture == &widget)
        _capture = 0;

    onRelayout(widget);
}


void Sheet::onInit(Widget& widget)
{
    widget.setSurface( _surface, widget.position() );
    widget.setNextResponder(this);
    widget.setSheet(this);

    double scaling = _surface ? _surface->scaleFactor() : 1.0;

    RescaleEvent ev(widget, scaling);
    //w.processEvent(ev);
    Application::instance().loop().commitEvent(ev);
}


void Sheet::onRelease(Widget& widget)
{
    widget.setSheet(0);
    widget.setSurface( 0, widget.position() );
    widget.setNextResponder(0);
}


void Sheet::onRegister(Widget& widget)
{
    //
    // focus handling
    //
    if( _focusList.empty() )
        widget.setFocusIndex(0);
    else
        widget.setFocusIndex( _focusList.back()->focusIndex() + 1);

    _focusList.push_back(&widget);

    onSetShortcut( widget, widget.shortcut() );
    onSetMnemonic( widget, widget.mnemonic() );
}


void Sheet::onDeregister(Widget& widget)
{
    if(_active == &widget)
        _active = 0;
    
    if(_pointer == &widget)
        _pointer = 0;

    //
    // focus handling
    //
    if( _focusWidget == &widget )
    {
        FocusEvent fev(*_focusWidget, false);
        _focusWidget->processEvent(fev);
        _focusWidget = 0;
    }

    std::vector<Widget*>::iterator it;
    it = std::find(_focusList.begin(), _focusList.end(), &widget);

    if( it != _focusList.end() )
        _focusList.erase(it);

    onSetShortcut(widget, 0);
    onSetMnemonic(widget, 0);
}


void Sheet::invalidate()
{
    ++_invalidates;

    InvalidateEvent ev(*this);
    Application::instance().commitEvent(ev);
}


void Sheet::onProcessInvalidateEvent(const InvalidateEvent& ev)
{
    --_invalidates;

    if(_invalidates > 0)
      return;

    onInvalidateEvent(ev);
}


void Sheet::onInvalidateEvent(const InvalidateEvent& ev)
{
    onInvalidate();
}


void Sheet::onInvalidate()
{
    relayout();
}


void Sheet::onRepaint(Widget& w, const Gfx::RectF& rect)
{
    Gfx::PointF widgetPos = onFromWidget( w, rect.topLeft() );
    Gfx::RectF widgetRect( widgetPos, rect.size() );

    repaint(widgetRect);
}


void Sheet::repaint(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->onRepaint(*this, rect);
}


void Sheet::onProcessPaintEvent(const PaintEvent& ev)
{    
    const Gfx::RectF& r = ev.rect();

    if( r.isNull() )
        return;

    //
    // paint view
    //
    onPaintEvent(ev);

    //
    // paint main widget
    //
    if( ! _mainWidget )
        return;

    // clip widget update rect
    Gfx::RectF updateRect = _mainWidget->geometry().intersect(r);
    if( updateRect.isNull() )
        return;

    // paint widget rect
    Gfx::PointF updatePos = onToWidget( *_mainWidget, updateRect.topLeft() );
    updateRect.setOrigin(updatePos);

    PaintEvent pev( *_mainWidget, updateRect );
    _mainWidget->processEvent(pev);
}


void Sheet::onPaintEvent(const PaintEvent& ev)
{    
    const Gfx::RectF& rect = ev.rect();

    //static int nnn = 0;
    //std::clog << "PAINT EVENT: " << typeid(*this).name() << " " << ++nnn << std::endl;

    Gfx::RectF updateRect = rect.intersect( Gfx::RectF( size() ) );

    if(_surface)
        onPaint(*_surface, updateRect);
}


void Sheet::onPaint(Gfx::PaintSurface&, const Gfx::RectF&)
{
}


void Sheet::onRelayout(Widget& widget)
{   
    //std::clog << "RELAYOUT" << name() <<  std::endl;

    relayout();
}


void Sheet::relayout()
{
    //std::clog << "RELAYOUT" << name() <<  std::endl;

    _layouts++;

    RelayoutEvent ev(*this);
    Application::instance().loop().commitEvent(ev);
}


void Sheet::onProcessRelayoutEvent(const RelayoutEvent& ev)
{
    if(_layouts == 0)
    {
        //std::clog << "RELAYOUT EVENT " << title() << " skipped" << std::endl;
        return;
    }

    --_layouts;

    if(_layouts > 0)
        return;

    //std::clog << "RELAYOUT EVENT " << name() << std::endl;

    //
    // 1. Pass
    //  
    SizePolicy policy(SizePolicy::Preferred, SizePolicy::Preferred);
    policy.setSize( size() );
    measure(policy);

    // align to physical pixel grid
    Gfx::RectF rect( position(), size() );

    if(_surface)
        rect = _surface->align(rect);

    //
    // 2. Pass layout position and size of contents
    //
    layout(rect);

    // layout content marked invalid
    if( _mainWidget )
    {
        Gfx::RectF widgetRect( rect.size() );
        
        LayoutEvent lev(_mainWidget->vid(), widgetRect);
        _mainWidget->processEvent(lev);
    }
}


Gfx::SizeF Sheet::measure(const SizePolicy& policy)
{
    return onMeasure(policy);
}


Gfx::SizeF Sheet::onMeasure(const SizePolicy& policy)
{
    if( _mainWidget )
        return _mainWidget->measure(policy);

    return policy.size();
}


void Sheet::layout(const Gfx::RectF& rect)
{
    onLayout(rect);
}


void Sheet::onLayout(const Gfx::RectF& rect)
{
    //
    // TODO: no need to pass rect
    //

    if( _mainWidget )
    {
        Gfx::RectF widgetRect( rect.size() );

        _mainWidget->move( widgetRect.topLeft() );
        _mainWidget->resize( widgetRect.size() );
    }
}


void Sheet::onProcessRescaleEvent(const RescaleEvent& ev)
{   
    onRescaleEvent(ev);

    double scaling = ev.scaleFactor();

    if(_mainWidget)
    {
        RescaleEvent ev(*_mainWidget, scaling);
        _mainWidget->processEvent(ev);
    }
}  


void Sheet::onRescaleEvent(const RescaleEvent& ev)
{
    onRescale( ev.scaleFactor() );
}


void Sheet::onRescale(double scaling)
{
    // realign geometry
    move( _requestedGeometry.topLeft() );
    resize( _requestedGeometry.size() );

    relayout();
}


const Gfx::RectF& Sheet::geometry() const
{
    return _alignedGeometry;
}


const Gfx::PointF& Sheet::position() const
{
    return _alignedGeometry.topLeft();
}


void Sheet::move(const Gfx::PointF& pos)
{
    _requestedGeometry.setOrigin(pos);

    if(_parent)
        _parent->onMove(*this, pos);
    else
        _alignedGeometry.setOrigin(pos);
}


void Sheet::onProcessMoveEvent(const MoveEvent& ev)
{
    onMoveEvent(ev);
}


void Sheet::onMoveEvent(const MoveEvent& ev)
{    
    _alignedGeometry.setOrigin( ev.position() );
}


const Gfx::SizeF& Sheet::size() const
{
    return _alignedGeometry.size();
}


void Sheet::resize(const Gfx::SizeF& s)
{
    _requestedGeometry.setSize(s);
    
    if(_parent)
        _parent->onResize(*this, s);
    else
        _alignedGeometry.setSize(s);
}


void Sheet::onProcessResizeEvent(const ResizeEvent& ev)
{
    onResizeEvent(ev);
}


void Sheet::onResizeEvent(const ResizeEvent& ev)
{
    _alignedGeometry.setSize( ev.size() );

    relayout();
}


bool Sheet::acceptsInput() const
{    
    if( ! isEnabled() )
        return false;

    return true;
}


bool Sheet::isEnabled() const
{
    return _enabledState;
}


void Sheet::onProcessEnableEvent(const EnableEvent& ev)
{
    bool wasEnabled = isEnabled();

    _enabledState = ev.enabled();

    if( wasEnabled != isEnabled() )
    {
        onEnable( ev.enabled() );
    }

    if(_mainWidget)
    {
        onEnable( *_mainWidget, ev.enabled() );
    }
}


void Sheet::onEnable(bool e)
{
}


void Sheet::onEnable(Widget& widget, bool enable)
{
    if( ! isEnabled() )
      enable = false;

    EnableEvent eev(widget, enable);
    widget.processEvent(eev);
}


void Sheet::onShow(Widget& widget, bool isShown)
{
    ShowEvent sev(widget, isShown);
    widget.processEvent(sev);
}


void Sheet::onMove(Widget& widget, const Gfx::PointF& pos)
{
    widget.setSurface(_surface, pos);
}


void Sheet::onResize(Widget& widget, const Gfx::SizeF& size)
{
}


void Sheet::onRaise(Widget& widget)
{
}


Widget* Sheet::focusWidget()
{
    return _focusWidget;
}


void Sheet::focusPrev()
{
    moveFocus(_focusList.rbegin(), _focusList.rend());
}


void Sheet::focusNext()
{
    moveFocus(_focusList.begin(), _focusList.end());
}


template <typename Iter>
void Sheet::moveFocus(Iter begin, Iter end)
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
            onSetFocus(*w);
            return;
        }

        ++it;
    }
}


void Sheet::onSetFocusPolicy(Widget& w, FocusPolicy policy)
{
    if( _focusWidget == &w && policy == View::NoFocus )
    {
        FocusEvent fev(*_focusWidget, false);
        _focusWidget->processEvent(fev);
        _focusWidget = 0;
    }
}


void Sheet::onSetFocusIndex(Widget& w, unsigned index)
{
    std::sort(_focusList.begin(), _focusList.end(), &lowerFocusIndex);
}


void Sheet::onSetFocus(Widget& widget)
{
    if( _focusWidget == &widget )
        return;

    if(_focusWidget)
    {
        if(_focusWidget->focusPolicy() == Widget::KeepFocus)
            return;
    }

    if(widget.focusPolicy() == Widget::NoFocus)
    {
        Application::instance().inputMethod().finish();
        return;
    }

    if( _focusWidget )
    {
        FocusEvent fev(*_focusWidget, false);
        _focusWidget->processEvent(fev);
    }
    
    _focusWidget = &widget;

    if( _focusWidget )
    {
        FocusEvent fev(*_focusWidget, true);
        _focusWidget->processEvent(fev);
    }
}


void Sheet::onSetShortcut(Widget& w, const Key* key)
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


void Sheet::onSetMnemonic(Widget& w, const Char* ch)
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


void Sheet::onProcessMouseEvent(const MouseEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Gfx::PointF pos = fromGlobal( ev.position() );

    if(_mainWidget && 
       _mainWidget->geometry().contains(pos) && 
       _mainWidget->acceptsInput() )
    {
        _mainWidget->processEvent(ev);
        return;
    }

    //
    // process event
    // 
    setPointer(true);
    
    mouseEvent(ev);
}


bool Sheet::onMouseEvent(const MouseEvent& ev)
{
    if(ev.isPress(MouseEvent::Left) )
        Application::instance().inputMethod().finish();

    return false;
}


void Sheet::onProcessTouchEvent(const TouchEvent& ev)
{ 
    if( ! acceptsInput() )
        return;

    //
    // continue press sequence capture
    //  
    if(_capture)
    {
        _capture->processEvent(ev);

        if( ev.isRelease() )
            _capture = 0;
        
        return;
    }

    Gfx::PointF pos = fromGlobal( ev.position() );

    //
    // hit test
    // 
    Widget* widget = 0;
    
    if(_mainWidget && 
       _mainWidget->geometry().contains(pos) && 
       _mainWidget->acceptsInput() )
    {
        widget = _mainWidget;
    }

    _pointer = _mainWidget;

    if(widget)
    {
        //
        // start press sequence capture
        // 
        if( ev.isPress() )
            _capture = widget;

        widget->processEvent(ev);
        return;
    }
    
    //
    // handle event
    // 
    setPointer(true);

    touchEvent(ev);
}


bool Sheet::onTouchEvent(const TouchEvent& ev)
{
    if(ev.isPress() )
        Application::instance().inputMethod().finish();

    return false;
}


void Sheet::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! acceptsInput() )
        return;

    //
    // TODO: disptch by responder chain obsoletes dispatch 
    //       to _active and _focus
    //

    if(_active)
    {
        _active->processEvent(ev);
        return;
    }

    if(_focusWidget)
    {
        _focusWidget->scrollEvent(ev);
        return;
    }

    //if(_pointer)
    //{
    //    _pointer->processEvent(ev);
    //    return; 
    //}

    scrollEvent(ev);
}


bool Sheet::onScrollEvent(const ScrollEvent& ev)
{
    return false;
}


void Sheet::onProcessEnterEvent(const EnterEvent& ev)
{
    enterEvent(ev);
}


bool Sheet::onEnterEvent(const EnterEvent& ev)
{
    //std::clog << "ENTER SHEET: " << name() << " " << vid() << std::endl;
    Application::instance().setCursor(0);
    return true;
}


void Sheet::onProcessLeaveEvent(const LeaveEvent& ev)
{
    leaveEvent(ev);
}


bool Sheet::onLeaveEvent(const LeaveEvent& ev )
{
    //std::clog << "LEAVE SHEET: " << name()  << " " << vid() << std::endl;
    Application::instance().setCursor(0);
    _pointer = 0;
    return true;
}


void Sheet::onActivate(Widget& widget, bool active)
{
    _active = active ? &widget : 0;
}


void Sheet::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! acceptsInput() )
        return;

    if(_active)
    {
        _active->processEvent(ev);
        return;
    }

    //
    // shortcut navigation
    //
    if( ev.isPress() )
    {
        std::map<Key, Widget*>::iterator s = _shortcuts.find( ev.key() );
        if( s != _shortcuts.end() )
        {
            s->second->processShortcut(ev);
            return;
        }
    }

    //
    // mnemonic navigation
    //
    if( ev.isPress() && ev.key().modifiers() == Key::Alt )
    {
        std::map<Char, Widget*>::iterator m = _mnemonics.find( ev.unicode() );
        if( m != _mnemonics.end() )
        {
            m->second->processMnemonic();
            return;
        }
    }

    //
    // focus navigation
    //
    if( ev.key().code() == Key::Tab && ev.isPress() )
    {
        bool keepFocus = _focusWidget && _focusWidget->focusPolicy() == KeepFocus;
        if( ! keepFocus )
        {
            if( ev.key().modifiers() == Key::Shift )
                focusPrev();
            else
                focusNext();

            return;
        }
    }

    //
    // pass event to responder chain
    //
    if( _focusWidget )
    {       
        _focusWidget->keyEvent(ev);
    }
    else
    {
        keyEvent(ev);
    }
}


bool Sheet::onKeyEvent(const KeyEvent& ev)
{
    return false;
}

} // namespace

} // namespace
