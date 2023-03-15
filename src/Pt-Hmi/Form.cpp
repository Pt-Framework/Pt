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

#include <Pt/Hmi/Form.h>
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

Form::Form()
: _mainWidget(0)
, _layouts(0)
, _active(0)
, _focusWidget(0)
{
    eventReceived() += Pt::slot(*this, &Form::onProcessLayoutEvent);
}


Form::~Form()
{
    if(_mainWidget)
        _mainWidget->unparent();
}


Widget* Form::content() 
{
    return _mainWidget;
}


const Widget* Form::content()  const 
{
    return _mainWidget;
}


void Form::setContent(Widget* widget)
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


void Form::setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
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
        _mainWidget->setSurface(surface, pos);
}


void Form::relayout()
{
    _layouts++;

    LayoutEvent ev( *this, bounds() );
    Application::instance().loop().commitEvent(ev);
}


void Form::onProcessLayoutEvent(const LayoutEvent& ev)
{
    if(_layouts == 0)
    {
        return;
    }

    --_layouts;

    if(_layouts > 0)
    {
        //std::clog << "RELAYOUT EVENT " << " deferred" << std::endl;
        return;
    }

    //std::clog << "RELAYOUT EVENT" << std::endl;

    if(_mainWidget)
    {
        //
        // 1. Pass
        //  
        onMeasure();

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


Gfx::SizeF Form::onMeasure()
{
    SizePolicy policy(SizePolicy::Fixed, SizePolicy::Fixed);
    policy.setSize( size() );
    
    return _mainWidget ? _mainWidget->measure(policy)
                       : policy.size();
}


void Form::onLayoutEvent(const LayoutEvent& ev)
{   
    if( _mainWidget )
    {
        Gfx::PointF widgetPos(0, 0);
        Gfx::SizeF widgetSize = size();      
        //std::clog << "Form::onLayout " << _mainWidget->name() << " " << widgetSize.height() << std::endl;
        
        _mainWidget->move(widgetPos);
        _mainWidget->resize(widgetSize);
    }
}


Widget* Form::focusWidget()
{
    return _focusWidget;
}


void Form::focusPrev()
{
    moveFocus(_focusList.rbegin(), _focusList.rend());
}


void Form::focusNext()
{
    moveFocus(_focusList.begin(), _focusList.end());
}


template <typename Iter>
void Form::moveFocus(Iter begin, Iter end)
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


void Form::onSetFocusPolicy(Widget& w, FocusPolicy policy)
{
    if( _focusWidget == &w && policy == View::NoFocus )
    {
        FocusEvent fev(*_focusWidget, false);
        _focusWidget->processEvent(fev);
        _focusWidget = 0;
    }
}


void Form::onSetFocusIndex(Widget& w, unsigned index)
{
    std::sort(_focusList.begin(), _focusList.end(), &lowerFocusIndex);
}


void Form::onSetFocus(Widget& widget)
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


void Form::onSetShortcut(Widget& w, const Key* key)
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


void Form::onSetMnemonic(Widget& w, const Char* ch)
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


void Form::onAddElement(Widget& widget)
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


void Form::onRemoveElement(Widget& widget)
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

void Form::onAttach(Widget& widget)
{
    _mainWidget = &widget;
    
    relayout();
}


void Form::onDetach(Widget& widget)
{
  if(_active == &widget)
      _active = 0;

    if(_mainWidget == &widget)
        _mainWidget = 0;

    relayout();
}


void Form::onInit(Widget& widget)
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


void Form::onRelease(Widget& widget)
{
    widget.setForm(0);
    widget.setSurface( 0, widget.position() );
    widget.setNextResponder(0);
}


Gfx::PointF Form::onToWidget(const Widget& widget, const Gfx::PointF& pos) const
{
    return pos - widget.position();   
}


Gfx::PointF Form::onFromWidget(const Widget& widget, const Gfx::PointF& pos) const
{
    return pos + widget.position();
}


void Form::onRaiseRequest(Widget& widget)
{
}

//
// Visual
//

Visual* Form::onHitTest(const Gfx::PointF& p)
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

void Form::onProcessEvent(const Pt::Event& ev)
{
    Base::onProcessEvent(ev);
}

//
// invalidation
//

void Form::onInvalidateEvent(const InvalidateEvent& ev)
{
    Base::onInvalidateEvent(ev);
}


void Form::onInvalidate()
{
    Base::onInvalidate();
    
    relayout();
}

//
// painting
//

void Form::onProcessPaintEvent(const PaintEvent& ev)
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


void Form::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
}


void Form::onRepaintRequest(Widget& w, const Gfx::RectF& rect)
{
    Gfx::PointF widgetPos = onFromWidget( w, rect.topLeft() );
    Gfx::RectF widgetRect( widgetPos, rect.size() );

    repaint(widgetRect);
}


void Form::onRelayoutRequest(Widget& widget)
{
    relayout();
}

//
// scaling
//

void Form::onProcessRescaleEvent(const RescaleEvent& ev)
{   
    Base::onProcessRescaleEvent(ev);

    if(_mainWidget)
    {
        double scaling = ev.scaleFactor();
        RescaleEvent ev(*_mainWidget, scaling);
        _mainWidget->processEvent(ev);
    }
}  


void Form::onRescaleEvent(const RescaleEvent& ev)
{
    //if(_mainWidget)
    //{
    //    double scaling = ev.scaleFactor();
    //    RescaleEvent ev(*_mainWidget, scaling);
    //    _mainWidget->processEvent(ev);
    //}

    Base::onRescaleEvent(ev);
}


void Form::onRescale(double scaling)
{
    Base::onRescale(scaling);
}

//
// enable
//

void Form::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    if(_mainWidget)
    {
        EnableEvent eev( *_mainWidget, ev.enabled() );
        _mainWidget->processEvent(eev);
    }
}


void Form::onEnableEvent(const EnableEvent& ev)
{    
    Base::onEnableEvent(ev);
}


void Form::onEnable(bool e)
{
    Base::onEnable(e);
}


void Form::onEnableRequest(Widget& widget, bool enable)
{
    if( ! isEnabled() )
      enable = false;

    EnableEvent eev(widget, enable);
    widget.processEvent(eev);
}

//
// activation
//

void Form::onActivateRequest(Widget& widget, bool active)
{
    if(active)
        _active = &widget;

    if( ! active && _active == &widget )
        _active = 0;
}

//
// visibility
//

void Form::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);
}


void Form::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);
}


void Form::onShow(bool visible)
{
    Base::onShow(visible);
}


void Form::onShowRequest(Widget& widget, bool isShown)
{
    ShowEvent sev(widget, isShown);
    widget.processEvent(sev);
}

//
// geometry
//

void Form::onMoveRequest(Widget& widget, const Gfx::PointF& pos)
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


void Form::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);
}


void Form::onMoveEvent(const MoveEvent& ev)
{
    if( position() == ev.position() )
        return;

    Base::onMoveEvent(ev);
}


void Form::onResizeRequest(Widget& widget, const Gfx::SizeF& size)
{
    Gfx::SizeF alignedSize = _surface.align(size);

    ResizeEvent rev(widget, alignedSize);
    Application::instance().commitEvent(rev);
}


void Form::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void Form::onResizeEvent(const ResizeEvent& ev)
{
    if( size() == ev.size() )
        return;

    Base::onResizeEvent(ev);

    relayout();
}

//
// input capture
//

void Form::onRequestCapture(bool capture)
{
    Base::onRequestCapture(capture);
}

//
// input
//

void Form::onProcessMouseEvent(const MouseEvent& ev)
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


bool Form::onMouseEvent(const MouseEvent& ev)
{
    return Base::onMouseEvent(ev);
}


void Form::onProcessTouchEvent(const TouchEvent& ev)
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


bool Form::onTouchEvent(const TouchEvent& ev)
{
    return Base::onTouchEvent(ev);
}


void Form::onProcessScrollEvent(const ScrollEvent& ev)
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


bool Form::onScrollEvent(const ScrollEvent& ev)
{
    return Base::onScrollEvent(ev);
}


void Form::onProcessEnterEvent(const EnterEvent& ev)
{
    Base::onProcessEnterEvent(ev);
}


bool Form::onEnterEvent(const EnterEvent& ev)
{
    //std::clog << "ENTER Form: " << name() << " " << vid() << std::endl;
    return Base::onEnterEvent(ev);
}


void Form::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Base::onProcessLeaveEvent(ev);
}


bool Form::onLeaveEvent(const LeaveEvent& ev )
{
    //std::clog << "LEAVE Form: " << name()  << " " << vid() << std::endl;
    return Base::onLeaveEvent(ev);
}


void Form::onProcessKeyEvent(const KeyEvent& ev)
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


bool Form::onKeyEvent(const KeyEvent& ev)
{
    return Base::onKeyEvent(ev);
}

} // namespace

} // namespace
