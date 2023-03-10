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
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/LayoutEvent.h>

namespace {

bool lowerFocusIndex(Pt::Hmi::Widget* a, Pt::Hmi::Widget* b)
{
    return a->focusIndex() < b->focusIndex();
}

} // namespace

namespace Pt {

namespace Hmi {

Sheet::Sheet()
: _mainWidget(0)
, _layouts(0)
, _autoSize(false)
, _active(0)
, _focusWidget(0)
{
    eventReceived() += Pt::slot(*this, &Sheet::onProcessLayoutEvent);
}


Sheet::~Sheet()
{
    if(_mainWidget)
        _mainWidget->unparent();
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
        _mainWidget->unparent();
    }

    if(widget)
    {
        widget->setParent(*this);
    }
}


void Sheet::setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
{
    if( ! surface )
    {
        _surface.detach();
    }
    else
    {
        Gfx::RectF surfaceRect( pos, size() );
        _surface.attach(*surface, surfaceRect);
    }

    if(_mainWidget)
        _mainWidget->setSurface( surface, pos );
}


bool Sheet::isAutoSize() const
{
    return _autoSize;
}


void Sheet::setAutoSize(const SizePolicy& policy)
{
    onSetAutoSize(policy);
    
    _sizePolicy = policy;
    _autoSize = true;

    relayout();
}


void Sheet::onSetAutoSize(const SizePolicy& policy)
{
}


Gfx::SizeF Sheet::onMeasure(const SizePolicy& policy)
{
    if( _mainWidget )
        return _mainWidget->measure(policy);

    return policy.size();
}


void Sheet::onRequestRelayout()
{
    _layouts++;

    LayoutEvent ev( *this, bounds() );
    Application::instance().loop().commitEvent(ev);
}


void Sheet::onProcessLayoutEvent(const LayoutEvent& ev)
{
    if(_layouts == 0)
    {
        //std::clog << "RELAYOUT EVENT " << " skipped" << std::endl;
        return;
    }

    --_layouts;

    if(_layouts > 0)
        return;

    //std::clog << "RELAYOUT EVENT" << std::endl;

    if(_mainWidget)
    {
        //
        // 1. Pass
        //  
        if(_autoSize)
        {
            _mainWidget->measure(_sizePolicy);
        }
        else
        {
            SizePolicy policy(SizePolicy::Fixed, SizePolicy::Fixed);
            policy.setSize( size() );
            _mainWidget->measure(policy);
        }

        //
        // 2. Pass layout position and size of contents
        //
        Gfx::RectF rect( size() );
        LayoutEvent lev(*this, rect);
        onLayoutEvent(lev);

        LayoutEvent lev2(*_mainWidget, rect);
        Application::instance().commitEvent(lev2);
    }
}


void Sheet::onLayoutEvent(const LayoutEvent& ev)
{
    onLayout( ev.rect() );
}


void Sheet::onLayout(const Gfx::RectF& rect)
{
    //
    // TODO: no need to pass rect
    //

    if(_autoSize)
    {
        resize( _mainWidget->preferredSize() );
    }

    if( _mainWidget )
    {
        //std::clog << "Sheet::onLayout " << name() << " " << size().width() << std::endl;

        Gfx::RectF widgetRect( size() );

        _mainWidget->move( widgetRect.topLeft() );
        _mainWidget->resize( widgetRect.size() );
    }
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

    if(_focusWidget)
    {
        Window* imeWindow = Application::instance().inputMethod().activeWindow();
        if(imeWindow)
        {
          if( ! _focusWidget->isDescendantOf(*imeWindow) )
              Application::instance().inputMethod().finish();
        }

        FocusEvent fev(*_focusWidget, false);
        _focusWidget->processEvent(fev);
    }
    
    _focusWidget = &widget;

    if(_focusWidget)
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


void Sheet::onAddElement(Widget& widget)
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


void Sheet::onRemoveElement(Widget& widget)
{
    if(_active == &widget)
        _active = 0;

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


//
// View
//

void Sheet::onAttach(Widget& widget)
{
    _mainWidget = &widget;
    
    relayout();
}


void Sheet::onDetach(Widget& widget)
{
  if(_active == &widget)
      _active = 0;

    if(_mainWidget == &widget)
        _mainWidget = 0;

    relayout();
}


void Sheet::onInit(Widget& widget)
{
    Gfx::PaintSurface* surface = _surface.surface();
    Gfx::PointF surfacePos = _surface.area().topLeft() + widget.position();

    widget.setSurface(surface, surfacePos);
    widget.setNextResponder(this);
    widget.setForm(this);

    double scaling = scaleFactor();
    RescaleEvent ev(widget, scaling);
    widget.processEvent(ev);
}


void Sheet::onRelease(Widget& widget)
{
    widget.setForm(0);
    widget.setSurface( 0, widget.position() );
    widget.setNextResponder(0);
}


Gfx::PointF Sheet::onToWidget(const Widget& widget, const Gfx::PointF& pos) const
{
    return pos - widget.position();   
}


Gfx::PointF Sheet::onFromWidget(const Widget& widget, const Gfx::PointF& pos) const
{
    return pos + widget.position();
}


void Sheet::onRaiseRequest(Widget& widget)
{
}


//
// Visual
//

Visual* Sheet::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    if(_mainWidget)
    {
        Gfx::PointF pos = toWidget(*_mainWidget, p);
        Visual* hit = _mainWidget->hitTest(pos);
        if(hit)
            return hit;
    }

    return this;
}

void Sheet::onProcessEvent(const Pt::Event& ev)
{
    Base::onProcessEvent(ev);
}

//
// invalidation
//

void Sheet::onInvalidateEvent(const InvalidateEvent& ev)
{
    Base::onInvalidateEvent(ev);
}


void Sheet::onInvalidate()
{
    Base::onInvalidate();
    
    relayout();
}

//
// painting
//

void Sheet::onProcessPaintEvent(const PaintEvent& ev)
{    
    const Gfx::RectF& rect = ev.rect();
    if( rect.isNull() )
        return;

    Base::onProcessPaintEvent(ev);

    //
    // paint main widget
    //
    if(_mainWidget)
    {
        Gfx::RectF updateRect = _mainWidget->geometry().intersect(rect);
        if( updateRect.isNull() )
            return;

        Gfx::PointF updatePos = onToWidget( *_mainWidget, updateRect.topLeft() );
        updateRect.setOrigin(updatePos);

        PaintEvent pev( *_mainWidget, updateRect );
        _mainWidget->processEvent(pev);
    }
}


void Sheet::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
}


void Sheet::onRepaintRequest(Widget& w, const Gfx::RectF& rect)
{
    Gfx::PointF widgetPos = onFromWidget( w, rect.topLeft() );
    Gfx::RectF widgetRect( widgetPos, rect.size() );

    repaint(widgetRect);
}


void Sheet::onRelayoutRequest(Widget& widget)
{
    relayout();
}

//
// scaling
//

void Sheet::onProcessRescaleEvent(const RescaleEvent& ev)
{   
    Base::onProcessRescaleEvent(ev);

    if(_mainWidget)
    {
        double scaling = ev.scaleFactor();
        RescaleEvent ev(*_mainWidget, scaling);
        _mainWidget->processEvent(ev);
    }
}  


void Sheet::onRescaleEvent(const RescaleEvent& ev)
{
    //if(_mainWidget)
    //{
    //    double scaling = ev.scaleFactor();
    //    RescaleEvent ev(*_mainWidget, scaling);
    //    _mainWidget->processEvent(ev);
    //}

    Base::onRescaleEvent(ev);
}


void Sheet::onRescale(double scaling)
{
    Base::onRescale(scaling);
}

//
// enable
//

void Sheet::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    if(_mainWidget)
    {
        EnableEvent eev( *_mainWidget, ev.enabled() );
        _mainWidget->processEvent(eev);
    }
}


void Sheet::onEnableEvent(const EnableEvent& ev)
{    
    Base::onEnableEvent(ev);
}


void Sheet::onEnable(bool e)
{
    Base::onEnable(e);
}


void Sheet::onEnableRequest(Widget& widget, bool enable)
{
    if( ! isEnabled() )
      enable = false;

    EnableEvent eev(widget, enable);
    widget.processEvent(eev);
}

//
// activation
//

void Sheet::onActivateRequest(Widget& widget, bool active)
{
    if(active)
        _active = &widget;

    if( ! active && _active == &widget )
        _active = 0;
}

//
// visibility
//

void Sheet::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);
}


void Sheet::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
}


void Sheet::onShow(bool visible)
{
    Base::onShow(visible);
}


void Sheet::onShowRequest(Widget& widget, bool isShown)
{
    ShowEvent sev(widget, isShown);
    widget.processEvent(sev);
}

//
// geometry
//

void Sheet::onMoveRequest(Widget& widget, const Gfx::PointF& pos)
{
    //
    // align to physical pixel grid
    //
    Gfx::PointF aligedPos = _surface.align(pos);

    //
    // send move event
    //
    MoveEvent mev(widget, aligedPos);
    Application::instance().commitEvent(mev);
}


void Sheet::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);
}


void Sheet::onMoveEvent(const MoveEvent& ev)
{
    if( position() == ev.position() )
        return;

    Base::onMoveEvent(ev);
}


void Sheet::onResizeRequest(Widget& widget, const Gfx::SizeF& size)
{
    Gfx::SizeF alignedSize = _surface.align(size);

    ResizeEvent rev(widget, alignedSize);
    Application::instance().commitEvent(rev);
}


void Sheet::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void Sheet::onResizeEvent(const ResizeEvent& ev)
{
    if( size() == ev.size() )
        return;

    Base::onResizeEvent(ev);

    relayout();
}

//
// input capture
//

void Sheet::onRequestCapture(bool capture)
{
    Base::onRequestCapture(capture);
}

//
// input
//

void Sheet::onProcessMouseEvent(const MouseEvent& ev)
{
    //if( ! acceptsInput() )
    //    return;

    Gfx::PointF pos = fromGlobal( ev.position() );

    if(_mainWidget && 
       _mainWidget->geometry().contains(pos) && 
       _mainWidget->acceptsInput() )
    {
        _mainWidget->processEvent(ev);
        return;
    }

    Base::onProcessMouseEvent(ev);
}


bool Sheet::onMouseEvent(const MouseEvent& ev)
{
    return Base::onMouseEvent(ev);
}


void Sheet::onProcessTouchEvent(const TouchEvent& ev)
{ 
    //if( ! acceptsInput() )
    //    return;

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
    Base::onProcessTouchEvent(ev);
}


bool Sheet::onTouchEvent(const TouchEvent& ev)
{
    return Base::onTouchEvent(ev);
}


void Sheet::onProcessScrollEvent(const ScrollEvent& ev)
{
    //if( ! acceptsInput() )
    //    return;

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

    Base::onProcessScrollEvent(ev);
}


bool Sheet::onScrollEvent(const ScrollEvent& ev)
{
    return Base::onScrollEvent(ev);
}


void Sheet::onProcessEnterEvent(const EnterEvent& ev)
{
    Base::onProcessEnterEvent(ev);
}


bool Sheet::onEnterEvent(const EnterEvent& ev)
{
    //std::clog << "ENTER Form: " << name() << " " << vid() << std::endl;
    return Base::onEnterEvent(ev);
}


void Sheet::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Base::onProcessLeaveEvent(ev);
}


bool Sheet::onLeaveEvent(const LeaveEvent& ev )
{
    //std::clog << "LEAVE Form: " << name()  << " " << vid() << std::endl;
    return Base::onLeaveEvent(ev);
}


void Sheet::onProcessKeyEvent(const KeyEvent& ev)
{
    //if( ! acceptsInput() )
    //    return;

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
        _focusWidget->processEvent(ev);
    }
    else
    {
        Base::onProcessKeyEvent(ev);
    }
}


bool Sheet::onKeyEvent(const KeyEvent& ev)
{
    return Base::onKeyEvent(ev);
}

} // namespace

} // namespace
