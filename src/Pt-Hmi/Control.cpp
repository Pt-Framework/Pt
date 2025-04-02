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

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include <Pt/Hmi/Control.h>
#include <Pt/Hmi/Form.h>
#include <Pt/Hmi/Application.h>
#include <Pt/String.h>

#include <cmath>
#include <cassert>

namespace Pt {

namespace Hmi {

Control::Control()
: _parent(0)
, _form(0)
, _isCapture(false)
, _isLayoutInvalid(true)
, _show(true)
, _enabled(true)
, _isMeasureInvalid(true)
, _hasFocus(false)
, _focusPolicy(NoFocus)
, _focusIndex(0)
, _actionKey(Key::Space)
, _mnemonic(0)
{
    eventReceived() += Pt::slot(*this, &Control::onProcessLayoutEvent);
    eventReceived() += Pt::slot(*this, &Control::onProcessFocusEvent);
}


Control::~Control()
{
    while( ! _children.empty() )
        remove( *_children.back() );

    unparent();
}


void Control::setParent(View& parent)
{
    if(_parent == &parent)
        return;

    unparent();

    parent.onAttach(*this);
    _parent = &parent;

    _parent->onInit(*this);
    _parent->onEnableRequest(*this, _enabled);
    _parent->onShowRequest(*this, _show);
    _parent->onMoveRequest(*this, _requestedPosition);
    _parent->onResizeRequest(*this, _requestedSize);

    invalidate();

    onSetParent(_parent);
}


void Control::unparent()
{
    if( ! _parent )
        return;

    _parent->onRelease(*this);
    _parent->onDetach(*this);
    _parent = 0;
        
    onSetParent(0);
}


void Control::onAttach(Control& control)
{
    Base::onAttach(control);

    _children.push_back(&control);

    onAddControl(control);
}


void Control::onDetach(Control& control)
{
    Base::onDetach(control);

    std::vector<Control*>::iterator it;
    it = std::find(_children.begin(), _children.end(), &control);
    if( it != _children.end() )
        _children.erase(it);

    onRemoveControl(control);
}


void Control::onInit(Control& control)
{
    Base::onInit(control);

    //Gfx::PaintSurface* surface = _surface.surface();
    //Gfx::PointF surfacePos = _surface.position() + control.position();
    //control.setSurface(surface, surfacePos);
    
    control.setNextResponder(this);
    control.setForm(_form);

    double scaling = scaleFactor();
    
    RescaleEvent ev(control, scaling);
    control.processEvent(ev);
    //Application::instance().loop().commitEvent(ev);

    relayout();
}


void Control::onRelease(Control& control)
{
    Base::onRelease(control);

    control.setForm(0);
    //control.setSurface( 0, control.position() );
    control.setNextResponder(0);

    relayout();
}


void Control::setForm(Form* form)
{
    if(_form)
        _form->onRemoveElement(*this);

    _form = 0;

    if(form)
        form->onAddElement(*this);

    _form = form;

    std::vector<Control*>::iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
    {
        Control* control = *it;
        control->setForm(form);
    }
}


void Control::add(Control& control)
{
    control.setParent(*this);
}


void Control::remove(Control& control)
{
    control.unparent();
}


void Control::onAddControl(Control& control)
{
}


void Control::onRemoveControl(Control& control)
{
}


const std::vector<Control*>& Control::controls() const
{
    return _children;
}


//Gfx::PaintSurface& Control::surface()
//{
//    //return _surface;
//    return *this;
//}
//
//
//const Gfx::PaintSurface& Control::surface() const
//{
//    //return _surface;
//    return *this;
//}


//void Control::setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
//{
//    ViewSurface::resetSurface(surface, pos);
//
//    //if( ! surface )
//    //{
//    //    _surface.detach();
//    //}
//    //else
//    //{
//    //    Gfx::RectF clientRect( pos, size() );
//    //    _surface.attach(*surface, clientRect);
//    //}
//
//    //onSetSurface(surface, pos);
//}


void Control::onSetSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
{
    Base::onSetSurface(surface, pos);

    std::vector<Control*>::iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
    {
        Control* control = *it;

        Gfx::PointF surfacePos = pos + control->position();
        control->setSurface(surface, surfacePos);
    }
}


Gfx::PointF Control::onToControl(const Control& control, const Gfx::PointF& pos) const
{
    //const Visual* parentView = control.parent();

    //if( parentView == this || ! parentView )
        return pos - control.position();

    //return pos - parentView->onToControl(control, pos);    
}


Gfx::PointF Control::onFromControl(const Control& control, const Gfx::PointF& pos) const
{
    //const View* parentView = control.parent();

    //if( parentView == this || ! parentView )
        return pos + control.position();

//    return pos + parentView->onFromControl(control, pos);
}


const Gfx::RectF Control::geometry() const
{
    return Gfx::RectF( position(), size() );
}


Control::FocusPolicy Control::focusPolicy() const
{
    return _focusPolicy;
}


void Control::setFocusPolicy(FocusPolicy policy)
{
    _focusPolicy = policy;

    if(_form)
        _form->onSetFocusPolicy(*this, policy);
}


size_t Control::focusIndex() const
{
    return _focusIndex;
}


void Control::setFocusIndex(size_t index)
{
    _focusIndex = index;
    
    if(_form)
        _form->onSetFocusIndex(*this, index);
}


bool Control::hasFocus() const
{
    return _hasFocus;
}


void Control::focus()
{
    if(_form && focusPolicy() != Control::NoFocus)
        _form->onSetFocus(*this);
}


void Control::onProcessFocusEvent(const FocusEvent& ev)
{
    //std::clog << "FOCUS: " << typeid(*this).name() 
    //          << " " << ev.isFocused() << std::endl;
    onFocusEvent(ev);
}


void Control::onFocusEvent(const FocusEvent& ev)
{
    //if( _hasFocus && ! ev.isFocused() )
    //    Application::instance().inputMethod().finish();

    _hasFocus = ev.isFocused();

    // onFocusGained
    // onFocusLost

    invalidate();
}


Key Control::actionKey() const
{
    return _actionKey;
}


void Control::setActionKey(const Key& ak)
{
    _actionKey = ak;
}


void Control::onActionKey(const KeyEvent& kev)
{
}


const Key* Control::shortcut() const
{
    if(_shortcutKey.code() == Key::NoKey)
        return 0;

    return &_shortcutKey;
}


void Control::setShortcut(const Key* key)
{
    if( ! key )
        _shortcutKey.set(Key::NoKey);
    else
        _shortcutKey = *key;

    if(_form) 
        _form->onSetShortcut(*this, onGetShortcuts());
}


void Control::processShortcut(const Key& key)
{
    onShortcut(key);
}


const std::vector<Key> Control::onGetShortcuts()
{
    std::vector<Key> shc;

    if(_shortcutKey.empty())
        return shc;

    shc.push_back(_shortcutKey);

    return shc;
}

const std::vector<Pt::Char> Control::onGetMnemonics()
{
    std::vector<Pt::Char> mcs;

    if(_mnemonic == 0)
        return mcs;

    mcs.push_back(_mnemonic);
    return mcs;
}


void Control::onShortcut(const Key& key)
{
}


const Pt::Char* Control::mnemonic() const
{
    return _mnemonic != 0 ? &_mnemonic : 0;
}


void Control::setMnemonic(const Char& ch)
{
    _mnemonic = ch;
    if(_form)
        _form->onSetMnemonic(*this, onGetMnemonics());
}


String Control::setMnemonic(const String& text)
{
    String str;
    Char mnemonic = 0;

    bool onAmp = false;
    for(String::const_iterator it = text.begin(); it != text.end(); ++it)
    {
        if(onAmp)
        {
            if(*it != '&')
                mnemonic = *it;

            str += *it;
            onAmp = false;
        }
        else
        {
            if(*it == '&')
                onAmp = true;
            else
                str += *it;
        }
    }

    if(onAmp)
        str += '&';

    setMnemonic(mnemonic);

    return str;
}


void Control::setMnemonicControl(Control* control)
{
    _mnemonicEntered.disconnect();

    if(control)
        _mnemonicEntered += Pt::slot(*control, &Control::onMnemonic);
}


void Control::processMnemonic(Pt::Char m)
{
    onMnemonic(m);
}


void Control::onMnemonic(Pt::Char m)
{
    _mnemonicEntered.send(m);
}


void Control::onInvalidateEvent(const InvalidateEvent& ev)
{
    Base::onInvalidateEvent(ev);
}


void Control::onInvalidate()
{
    Base::onInvalidate();

    // TODO: relayout only if required in derived class
    relayout();
}


void Control::onRequestRepaint(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->onRepaintRequest(*this, rect);
}


void Control::onRepaintRequest(Control& control, const Gfx::RectF& rect)
{
    Base::onRepaintRequest(control, rect);

    Gfx::PointF controlPos = fromControl( control, rect.topLeft() );
    Gfx::RectF controlRect( controlPos, rect.size() );

    repaint(controlRect);
}


void Control::onProcessPaintEvent(const PaintEvent& ev)
{    
    const Gfx::RectF& r = ev.rect();
    if( r.isNull() )
        return;

    if( ! isVisible() )
        return;
        
    Base::onProcessPaintEvent(ev);

    //
    // paint child controls
    //
    std::vector<Control*>::const_iterator it;
    for(it = _children.begin() ; it != _children.end(); ++it)
    {
        Control* control = (*it);

        // clip control update rect
        Gfx::RectF updateRect = control->geometry().intersect(r);
        if( updateRect.isNull() )
            continue;

        // paint control rect
        Gfx::PointF updatePos = toControl( *control , updateRect.topLeft() );
        updateRect.setOrigin(updatePos);

        PaintEvent pev( *control, updateRect );
        control->processEvent(pev);
    }
}


void Control::onPaintEvent(const PaintEvent& ev)
{    
    //static int nnn = 0;
    //std::clog << "PAINT EVENT: " << typeid(*this).name() << " " << ++nnn << std::endl;

    Base::onPaintEvent(ev);

    //onPaint( _surface, ev.rect() );
}


void Control::onPaint(Gfx::PaintSurface&, const Gfx::RectF&)
{
}


void Control::onRelayoutRequest(Control& control)
{
    Base::onRelayoutRequest(control);

    relayout();
}


void Control::relayout()
{
    _isMeasureInvalid = true;
    _isLayoutInvalid = true;

    if(_parent)
        _parent->onRelayoutRequest(*this);
}


const SizePolicy& Control::sizePolicy() const
{
    return _sizePolicy;
}


void Control::setSizePolicy(const SizePolicy& policy)
{
    //Gfx::SizeF alignedSize = _surface.scaling().align( policy.size() );
    Gfx::SizeF alignedSize = scaling().align( policy.size() );

    _sizePolicy = policy;
    _sizePolicy.setSize(alignedSize);

    relayout();
}


Gfx::SizeF Control::preferredSize() const
{
    return _preferredSize;
}


Gfx::SizeF Control::measure(const SizePolicy& policy)
{
    SizePolicy contentPolicy = _sizePolicy;

    // use stricter size mode of parent and, if parent is fixed,
    // we also use the parents fixed width
    if( policy.horizontal() > _sizePolicy.horizontal() ||
        policy.horizontal() == SizePolicy::Fixed )
    {
        contentPolicy.setHorizontal( policy.horizontal() );
        contentPolicy.setWidth( policy.width() );
    }

    // use stricter size mode of parent and, if parent is fixed,
    // we also use the parents fixed height
    if( policy.vertical() > _sizePolicy.vertical() ||
        policy.vertical() == SizePolicy::Fixed )
    {
        contentPolicy.setVertical( policy.vertical() );
        contentPolicy.setHeight( policy.height() );
    }

    // apply minimum height, unless the size mode is fixed
    if( contentPolicy.vertical() != SizePolicy::Fixed &&
        contentPolicy.height() < minimumSize().height() )
    {
        contentPolicy.setHeight( minimumSize().height() );
    }

    // apply minimum width, unless the size mode is fixed
    if( contentPolicy.horizontal() != SizePolicy::Fixed &&
        contentPolicy.width() < minimumSize().width() )
    {
        contentPolicy.setWidth( minimumSize().width() );
    }

    bool doMeasure = contentPolicy != _lastPolicy || _isMeasureInvalid;
    if(doMeasure)
    {
        _lastPolicy = contentPolicy;

        //static int mmm = 0;
        //std::clog << "MEASURE: " << name() << " " << ++mmm << std::endl;

        bool isFixed = contentPolicy.vertical() == SizePolicy::Fixed &&
                       contentPolicy.horizontal() == SizePolicy::Fixed &&
                       controls().empty();

        _preferredSize = isFixed ? contentPolicy.size()
                                 : onMeasure(contentPolicy);

        // use fixed height, if size mode is fixed
        if(contentPolicy.vertical() == SizePolicy::Fixed)
            _preferredSize.setHeight( contentPolicy.height() );
        else if( _preferredSize.height() < minimumSize().height() )
            _preferredSize.setHeight( minimumSize().height() );

        if(contentPolicy.vertical() == SizePolicy::Maximum)
            _preferredSize.setHeight( std::min( _preferredSize.height(),
                                                contentPolicy.height() ) );

        // use fixed width, if size mode is fixed
        if(contentPolicy.horizontal() == SizePolicy::Fixed)
            _preferredSize.setWidth( contentPolicy.width() );
        else if( _preferredSize.width() < minimumSize().width() )
            _preferredSize.setWidth( minimumSize().width() );

        if(contentPolicy.horizontal() == SizePolicy::Maximum)
            _preferredSize.setWidth( std::min( _preferredSize.width(),
                                               contentPolicy.width() ) );
    
        _isMeasureInvalid = false;
        _isLayoutInvalid = true;
    }

    return preferredSize();
}


Gfx::SizeF Control::onMeasure(const SizePolicy& policy)
{
   return Gfx::SizeF(0, 0);
}


void Control::onProcessLayoutEvent(const LayoutEvent& ev)
{
    if( ! _isLayoutInvalid )
        return;

    //
    // align to physical pixel grid
    //
    //const Gfx::RectF& r = ev.rect();
    
    const Gfx::RectF& r = geometry();
    //Gfx::RectF rect = _surface.scaling().align(r);
    Gfx::RectF rect = scaling().align(r);

    //static int lll = 0;
    //std::clog << "LAYOUT: " << name() << " " << ++lll << std::endl;

    //
    // layout position and size of contents 
    //
    LayoutEvent lev(*this);
    lev.setRect(rect);
    onLayoutEvent(lev);
    
    //
    // layout content marked invalid
    //
    std::vector<Control*>::const_iterator it;
    for(it = _children.begin() ; it != _children.end(); ++it)
    {
        Control* control = (*it);

        LayoutEvent ev( *control, control->geometry() );
        Application::instance().commitEvent(ev);
    }

    _isLayoutInvalid = false;
}


// TODO: onLayoutEvent
void Control::onLayoutEvent(const LayoutEvent& ev)
{
    onLayout( ev.rect() );

     // TODO: repaint only if required in derived class
     repaint();
}


void Control::onLayout(const Gfx::RectF& rect)
{
    //
    // TODO: no need to pass rect
    //
}


void Control::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();

    for (size_t i = 0; i < _children.size(); ++i)
    {
        Control* control = _children[i];
        RescaleEvent ev(*control, scaling);
        control->processEvent(ev);
    }
}


void Control::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void Control::onRescale(double scaleFactor)
{
    Base::onRescale(scaleFactor);

    //const Gfx::Scaling& scaling = _surface.scaling();
    const Gfx::Scaling& scaling = this->scaling();

    _margin.set( scaling.align( _margin.left() ),
                 scaling.align( _margin.top() ),
                 scaling.align( _margin.right() ),
                 scaling.align( _margin.bottom() ) );

    _padding.set( scaling.align( _padding.left() ),
                  scaling.align( _padding.top() ),
                  scaling.align( _padding.right() ),
                  scaling.align( _padding.bottom() ) );
    
    _sizePolicy.setSize( scaling.align( _sizePolicy.size() ) );

    // TODO: invalidate in derived class only when neccessary
    invalidate();
    
    relayout();
}


void Control::onRequestMove(const Gfx::PointF& pos)
{
    _requestedPosition = pos;

    if(_parent)
        _parent->onMoveRequest(*this, _requestedPosition);
}


void Control::onMoveRequest(Control& control, const Gfx::PointF& pos)
{
    Base::onMoveRequest(control, pos);
}


void Control::onProcessMoveEvent(const MoveEvent& ev)
{
    Base::onProcessMoveEvent(ev);
}


void Control::onMoveEvent(const MoveEvent& ev)
{
    Base::onMoveEvent(ev);
}


void Control::onSetSizeLimits(const Gfx::SizeF& minSize,
                             const Gfx::SizeF& maxSize)
{
    Base::onSetSizeLimits(minSize, maxSize);
    relayout();
}


void Control::onRequestResize(const Gfx::SizeF& size)
{   
    _requestedSize = size;

    _isLayoutInvalid = true;

    //
    // maximum width and height
    //
    //if( alignedSize.width() > maximumSize().width() )
    //    alignedSize.setWidth( maximumSize().width() );

    //if( alignedSize.height() > maximumSize().height() )
    //    alignedSize.setHeight( maximumSize().height() );

    //if( alignedSize.width() < minimumSize().width() )
    //    alignedSize.setWidth( minimumSize().width() );

    //if( alignedSize.height() < minimumSize().height() )
    //    alignedSize.setHeight( minimumSize().height() );

    if(_parent)
        _parent->onResizeRequest(*this, _requestedSize);
}


void Control::onResizeRequest(Control& control, const Gfx::SizeF& size)
{
    Base::onResizeRequest(control, size);
}


void Control::onProcessResizeEvent(const ResizeEvent& ev)
{
    Base::onProcessResizeEvent(ev);
}


void Control::onResizeEvent(const ResizeEvent& ev)
{
    if( size() == ev.size() )
        return;

    //std::clog << "RESIZE: " << name() << ev.size().width() << std::endl;

    //Gfx::RectF updateRect( size() );
    //updateRect.unify( Gfx::RectF( ev.size() ) );
    //repaint(updateRect);

    //_surface.resize( ev.size() );

    //
    // TODO: layout content
    //
    //LayoutEvent lev( *this, geometry() );
    //Application::instance().commitEvent(lev);

    Base::onResizeEvent(ev);
}


bool Control::acceptsInput() const
{    
    if( ! isEnabled() )
        return false;

    if( ! isVisible() )
        return false;

    return true;
}


void Control::onRequestShow(bool isShown)
{
    _show = isShown;

    if(_parent)
        _parent->onShowRequest(*this, isShown);
}


void Control::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);
}


void Control::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);

    // parent should only layout visible children
    relayout();
}


void Control::onShow(bool isShown)
{
    Base::onShow(isShown);
}


void Control::onShowRequest(Control& control, bool visible)
{
    Base::onShowRequest(control, visible);

    ShowEvent sev(control, visible);
    control.processEvent(sev);
}


void Control::onRequestEnable(bool isEnable)
{
    _enabled = isEnable;

    if(_parent)
        _parent->onEnableRequest(*this, isEnable);
}


void Control::onProcessEnableEvent(const EnableEvent& ev)
{
    bool isEnabled = ev.enabled();
    if( ! _enabled )
        isEnabled = false;

    EnableEvent eev(*this, isEnabled);
    Base::onProcessEnableEvent(eev);

    for( size_t i = 0; i < _children.size(); ++i)
    {
        Control* control = _children[i];
        
        EnableEvent controlEvent(*control, isEnabled);
        control->processEvent(controlEvent);
    }
}


void Control::onEnableEvent(const EnableEvent& ev)
{
    Base::onEnableEvent(ev);

    // TODO: move to derived classes
    invalidate();
}


void Control::onEnable(bool e)
{
    Base::onEnable(e);
}


void Control::onEnableRequest(Control& control, bool enable)
{
    Base::onEnableRequest(control, enable);

    if( ! isEnabled() )
      enable = false;

    EnableEvent eev(control, enable);
    control.processEvent(eev);
}


void Control::onRequestActivate(bool active)
{
    if(_parent)
        _parent->onActivateRequest(*this, active);
}


void Control::onActivateRequest(Control& control, bool active)
{
    Base::onActivateRequest(control, active);

    if(_parent)
        _parent->onActivateRequest(*this, active);
}


void Control::raise()
{
    if(_parent)
        _parent->onRaiseRequest(*this);
}


void Control::onRaiseRequest(Control& control)
{
    Base::onRaiseRequest(control);

    std::vector<Control*>::iterator it = std::find(_children.begin(), 
                                                  _children.end(), &control);
    if( it == _children.end() )
        return;

    _children.erase(it);
    _children.push_back(&control);

    control.repaint( control.bounds() );
}


const Spacing& Control::margin() const
{
    return _margin;
}


void Control::setMargin(const Spacing& s)
{
    //const Gfx::Scaling& scaling = _surface.scaling();
    const Gfx::Scaling& scaling = this->scaling();

    _margin.set( scaling.align( s.left() ),
                 scaling.align( s.top() ),
                 scaling.align( s.right() ),
                 scaling.align( s.bottom() ) );

    relayout();
}


void Control::setMargin(double n)
{
    setMargin( Spacing(n) );
}


void Control::setMargin(double horiz, double vertical)
{
    setMargin( Spacing(horiz, vertical) );
}


const Spacing& Control::padding() const
{
    return _padding;
}


void Control::setPadding( const Spacing& p )
{
    //const Gfx::Scaling& scaling = _surface.scaling();
    const Gfx::Scaling& scaling = this->scaling();

    _padding.set( scaling.align( p.left() ),
                  scaling.align( p.top() ),
                  scaling.align( p.right() ),
                  scaling.align( p.bottom() ) );

    relayout();
}


void Control::setPadding(double n)
{
    setPadding( Spacing(n) );
}


void Control::setPadding(double horiz, double vertical)
{
    setPadding( Spacing(horiz, vertical) );
}

//
// Visual
//

Visual* Control::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    std::vector<Control*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Control* control = *it;
        Gfx::PointF pos = toControl(*control, p);
        Visual* hit = control->hitTest(pos);
        if(hit)
            return hit;
    }

    return this;
}


Gfx::PointF Control::onToParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->onFromControl(*this, pos);
}


Gfx::PointF Control::onFromParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->onToControl(*this, pos);
}


void Control::onProcessEvent(const Pt::Event& ev)
{
    Base::onProcessEvent(ev);
}


void Control::onProcessMouseEvent(const MouseEvent& ev)
{
    if( ! acceptsInput() )
        return;

    //
    // stop capture on press
    // 
    if(_isCapture)
    {
        if( ev.isRelease() )
        {
            setCapture(false);
            _isCapture = false;
        }
    }

    //
    // hit test
    // 
    Gfx::PointF pos = fromGlobal( ev.position() );

    std::vector<Control*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Control* control = *it;

        if( control->geometry().contains(pos) && 
            control->acceptsInput() )
        {
            control->processEvent(ev);
            return;
        }
    }

    //
    // start capture on press
    //
    if( ev.isPress() )
    {
        setCapture(true);
        _isCapture = true;
    }

    Base::onProcessMouseEvent(ev);
}


bool Control::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    if( ev.isPress(MouseEvent::Left) )
    {
        focus();
    }

    return false;
}


void Control::onProcessTouchEvent(const TouchEvent& ev)
{
    if( ! acceptsInput() )
        return;

    //
    // stop capture on press
    // 
    if(_isCapture)
    {
        if( ev.isRelease() )
        {
            setCapture(false);
            _isCapture = false;
        }
    }

    //
    // hit test
    // 
    Gfx::PointF pos = fromGlobal( ev.position() );

    std::vector<Control*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Control* control = *it;

        if( control->geometry().contains(pos) && 
            control->acceptsInput() )
        {
            control->processEvent(ev);
            return;
        }
    }

    //
    // start capture on press
    //
    if( ev.isPress() )
    {
        setCapture(true);
        _isCapture = true;
    }

    Base::onProcessTouchEvent(ev);
}


bool Control::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    if( ev.isPress() )
    {
        focus();
    }

    return false;
}


void Control::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Base::onProcessScrollEvent(ev);
}


bool Control::onScrollEvent(const ScrollEvent& ev)
{
    return Base::onScrollEvent(ev);
}


void Control::onProcessEnterEvent(const EnterEvent& ev)
{
    Base::onProcessEnterEvent(ev);
}


bool Control::onEnterEvent( const EnterEvent& ev)
{


    return Base::onEnterEvent(ev);
}


void Control::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Base::onProcessLeaveEvent(ev);
}


bool Control::onLeaveEvent(const LeaveEvent& ev)
{
    return Base::onLeaveEvent(ev);
}


void Control::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Base::onProcessKeyEvent(ev);
}


bool Control::onKeyEvent(const KeyEvent& ev)
{
    if( ev.key() == actionKey() && hasFocus() )
    {
        onActionKey(ev);
        return true;
    }

    return Base::onKeyEvent(ev);
}

} // namespace

} // namespace
