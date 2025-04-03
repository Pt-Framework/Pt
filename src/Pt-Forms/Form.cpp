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

#include <Pt/Forms/Form.h>
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/LayoutEvent.h>

namespace {

bool lowerFocusIndex(Pt::Forms::Control* a, Pt::Forms::Control* b)
{
    return a->focusIndex() < b->focusIndex();
}

} // namespace

namespace Pt {

namespace Forms {

Form::Form()
: _mainControl(0)
, _layouts(0)
, _active(0)
, _focusControl(0)
{
    eventReceived() += Pt::slot(*this, &Form::onProcessLayoutEvent);
}


Form::~Form()
{
    if(_mainControl)
        _mainControl->unparent();
}


Control* Form::content() 
{
    return _mainControl;
}


const Control* Form::content()  const 
{
    return _mainControl;
}


void Form::setContent(Control* control)
{
    if(_mainControl)
    {
        _mainControl->unparent();
    }

    if(control)
    {
        control->setParent(*this);
    }
}


//Gfx::PaintSurface& Form::surface()
//{
//    //return _surface;
//    return *this;
//}
//
//
//const Gfx::PaintSurface& Form::surface() const
//{
//    //return _surface;
//    return *this;
//}


//void Form::setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
//{
//    ViewSurface::resetSurface(surface, pos);
//
//    //if( ! surface )
//    //{
//    //    _surface.detach();
//    //}
//    //else
//    //{
//    //    Gfx::RectF surfaceRect( pos, size() );
//    //    _surface.attach(*surface, surfaceRect);
//    //}
//}


void Form::onSetSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
{
    Base::onSetSurface(surface, pos);

    if(_mainControl)
        _mainControl->setSurface(surface, pos);
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

    if(_mainControl)
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

        LayoutEvent lev2(*_mainControl, rect);
        Application::instance().commitEvent(lev2);
    }
}


Gfx::SizeF Form::onMeasure()
{
    SizePolicy policy(SizePolicy::Fixed, SizePolicy::Fixed);
    policy.setSize( size() );
    
    return _mainControl ? _mainControl->measure(policy)
                       : policy.size();
}


void Form::onLayoutEvent(const LayoutEvent& ev)
{   
    if( _mainControl )
    {
        Gfx::PointF controlPos(0, 0);
        Gfx::SizeF controlSize = size();      
        //std::clog << "Form::onLayout " << _mainControl->name() << " " << controlSize.height() << std::endl;
        
        _mainControl->move(controlPos);
        _mainControl->resize(controlSize);
    }
}


Control* Form::focusControl()
{
    return _focusControl;
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
    Iter current = std::find(begin, end, _focusControl);
    Iter it = current;
    
    if( it != end )
        ++it;
    else
        it = begin;

    while(it != current)
    {
        if( it == end )
            it = begin;

        Control* control = *it;
        
        if( control->focusPolicy() != Control::NoFocus )
        {
            onSetFocus(*control);
            return;
        }

        ++it;
    }
}


void Form::onSetFocusPolicy(Control& control, FocusPolicy policy)
{
    if( _focusControl == &control && policy == View::NoFocus )
    {
        FocusEvent fev(*_focusControl, false);
        _focusControl->processEvent(fev);
        _focusControl = 0;
    }
}


void Form::onSetFocusIndex(Control& control, unsigned index)
{
    std::sort(_focusList.begin(), _focusList.end(), &lowerFocusIndex);
}


void Form::onSetFocus(Control& control)
{
    if( _focusControl == &control )
        return;

    if(_focusControl)
    {
        if(_focusControl->focusPolicy() == Control::KeepFocus)
            return;
    }

    if(_focusControl)
    {
        Window* imeWindow = Application::instance().inputMethod().activeWindow();
        if(imeWindow)
        {
          if( ! _focusControl->isDescendantOf(*imeWindow) )
              Application::instance().inputMethod().finish();
        }

        FocusEvent fev(*_focusControl, false);
        _focusControl->processEvent(fev);
    }
    
    _focusControl = &control;

    if(_focusControl)
    {
        FocusEvent fev(*_focusControl, true);
        _focusControl->processEvent(fev);
    }
}


void Form::onSetShortcut(Control& control, const std::vector<Key>& keys)
{
    std::map<Key, Control*>::iterator it = _shortcuts.begin();

    while( it != _shortcuts.end() )
    {
        if(it->second == &control)
            _shortcuts.erase(it++);
        else
            ++it;
    }

    for(size_t i = 0; i < keys.size();++i)
        _shortcuts[keys[i]] = &control;
}


void Form::onSetMnemonic(Control& control, const std::vector<Char>& chs)
{
    std::map<Char, Control*>::iterator it = _mnemonics.begin();

    while( it != _mnemonics.end() )
    {
        if(it->second == &control)
            _mnemonics.erase(it++);
        else
            ++it;
    }

    for( size_t i = 0; i < chs.size(); ++i)
        _mnemonics[chs[i]] = &control;
}


void Form::onAddElement(Control& control)
{
    //
    // focus handling
    //
    if( _focusList.empty() )
        control.setFocusIndex(0);
    else
        control.setFocusIndex( _focusList.back()->focusIndex() + 1);

    _focusList.push_back(&control);

    onSetShortcut( control, control.onGetShortcuts() );
    onSetMnemonic( control, control.onGetMnemonics() );
}


void Form::onRemoveElement(Control& control)
{
    if(_active == &control)
        _active = 0;

    //
    // focus handling
    //
    if( _focusControl == &control )
    {
        FocusEvent fev(*_focusControl, false);
        _focusControl->processEvent(fev);
        _focusControl = 0;
    }

    std::vector<Control*>::iterator it;
    it = std::find(_focusList.begin(), _focusList.end(), &control);

    if( it != _focusList.end() )
        _focusList.erase(it);

    onSetShortcut(control, std::vector<Pt::Forms::Key>());
    onSetMnemonic(control, std::vector<Pt::Char>());
}


//
// View
//

void Form::onAttach(Control& control)
{
    Base::onAttach(control);

    _mainControl = &control;
    
    relayout();
}


void Form::onDetach(Control& control)
{
    Base::onDetach(control);

    if(_active == &control)
        _active = 0;

      if(_mainControl == &control)
          _mainControl = 0;

      relayout();
}


void Form::onInit(Control& control)
{
    Base::onInit(control);

    //Gfx::PaintSurface* surface = _surface.surface();
    //Gfx::PointF surfacePos = _surface.position() + control.position();
    //control.setSurface(surface, surfacePos);
    
    control.setNextResponder(this);
    control.setForm(this);

    double scaling = scaleFactor();
    
    RescaleEvent ev(control, scaling);
    control.processEvent(ev);
}


void Form::onRelease(Control& control)
{
    Base::onRelease(control);

    control.setForm(0);
    //control.setSurface( 0, control.position() );
    control.setNextResponder(0);
}


Gfx::PointF Form::onToControl(const Control& control, const Gfx::PointF& pos) const
{
    return pos - control.position();   
}


Gfx::PointF Form::onFromControl(const Control& control, const Gfx::PointF& pos) const
{
    return pos + control.position();
}


void Form::onRaiseRequest(Control& control)
{
    Base::onRaiseRequest(control);
}

//
// Widget
//

Widget* Form::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    if(_mainControl)
    {
        Gfx::PointF pos = toControl(*_mainControl, p);
        Widget* hit = _mainControl->hitTest(pos);
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
    // paint main control
    //
    if(_mainControl)
    {
        Gfx::RectF updateRect = _mainControl->geometry().intersect(rect);
        if( updateRect.isNull() )
            return;

        Gfx::PointF updatePos = onToControl( *_mainControl, updateRect.topLeft() );
        updateRect.setOrigin(updatePos);

        PaintEvent pev( *_mainControl, updateRect );
        _mainControl->processEvent(pev);
    }
}


void Form::onPaintEvent(const PaintEvent& ev)
{
    Base::onPaintEvent(ev);
}


void Form::onRepaintRequest(Control& control, const Gfx::RectF& rect)
{
    Base::onRepaintRequest(control, rect);

    Gfx::PointF controlPos = onFromControl( control, rect.topLeft() );
    Gfx::RectF controlRect( controlPos, rect.size() );

    repaint(controlRect);
}


void Form::onRelayoutRequest(Control& control)
{
    Base::onRelayoutRequest(control);

    relayout();
}

//
// scaling
//

void Form::onProcessRescaleEvent(const RescaleEvent& ev)
{   
    Base::onProcessRescaleEvent(ev);

    if(_mainControl)
    {
        double scaling = ev.scaleFactor();
        RescaleEvent ev(*_mainControl, scaling);
        _mainControl->processEvent(ev);
    }
}  


void Form::onRescaleEvent(const RescaleEvent& ev)
{
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

    if(_mainControl)
    {
        EnableEvent eev( *_mainControl, ev.enabled() );
        _mainControl->processEvent(eev);
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


void Form::onEnableRequest(Control& control, bool enable)
{
    Base::onEnableRequest(control, enable);

    if( ! isEnabled() )
      enable = false;

    EnableEvent eev(control, enable);
    control.processEvent(eev);
}

//
// activation
//

void Form::onActivateRequest(Control& control, bool active)
{
    Base::onActivateRequest(control, active);

    if(active)
        _active = &control;

    if( ! active && _active == &control )
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


void Form::onShowRequest(Control& control, bool visible)
{
    Base::onShowRequest(control, visible);

    ShowEvent sev(control, visible);
    control.processEvent(sev);
}

//
// geometry
//

void Form::onMoveRequest(Control& control, const Gfx::PointF& pos)
{
    Base::onMoveRequest(control, pos);
}


void Form::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);
}


void Form::onMoveEvent(const MoveEvent& ev)
{
    Base::onMoveEvent(ev);
}


void Form::onResizeRequest(Control& control, const Gfx::SizeF& size)
{
    Base::onResizeRequest(control, size);
}


void Form::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void Form::onResizeEvent(const ResizeEvent& ev)
{
    if( size() == ev.size() )
        return;

    //_surface.resize( ev.size() );

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

    if(_mainControl && 
       _mainControl->geometry().contains(pos) && 
       _mainControl->acceptsInput() )
    {
        _mainControl->processEvent(ev);
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

    if(_mainControl && 
       _mainControl->geometry().contains(pos) && 
       _mainControl->acceptsInput() )
    {
        _mainControl->processEvent(ev);
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

    if(_focusControl)
    {
        _focusControl->scrollEvent(ev);
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
    //std::clog << "ENTER Form: " << name() << " " << id() << std::endl;
    return Base::onEnterEvent(ev);
}


void Form::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Base::onProcessLeaveEvent(ev);
}


bool Form::onLeaveEvent(const LeaveEvent& ev )
{
    //std::clog << "LEAVE Form: " << name()  << " " << id() << std::endl;
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
        std::map<Key, Control*>::iterator s = _shortcuts.find( ev.key() );
        if( s != _shortcuts.end() )
        {
            s->second->processShortcut(s->first);
            return;
        }
    }

    //
    // mnemonic navigation
    //
    if( ev.isPress() && ev.key().modifiers() == Key::Alt )
    {
        std::map<Char, Control*>::iterator m = _mnemonics.find( ev.unicode() );
        if( m != _mnemonics.end() )
        {
            m->second->processMnemonic(m->first);
            return;
        }
    }

    //
    // focus navigation
    //
    if( ev.key().code() == Key::Tab && ev.isPress() )
    {
        bool keepFocus = _focusControl && _focusControl->focusPolicy() == KeepFocus;
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
    if( _focusControl )
    {       
        _focusControl->processEvent(ev);
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
