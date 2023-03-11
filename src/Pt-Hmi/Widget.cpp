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

#include <Pt/Hmi/Widget.h>
#include <Pt/Hmi/Form.h>
#include <Pt/Hmi/Application.h>
#include <Pt/String.h>

#include <cmath>
#include <cassert>

namespace Pt {

namespace Hmi {

Widget::Widget()
: _parent(0)
, _form(0)
, _isCapture(false)
, _isLayoutInvalid(true)
, _show(true)
, _enabled(true)
, _hasFocus(false)
, _focusPolicy(NoFocus)
, _focusIndex(0)
, _hasCursor(false)
, _cursor()
, _actionKey(Key::Space)
, _mnemonic(0)
{
    eventReceived() += Pt::slot(*this, &Widget::onProcessFocusEvent);
}


Widget::~Widget()
{
    while( ! _children.empty() )
        remove( *_children.back() );

    unparent();
}


void Widget::setParent(View& parent)
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


void Widget::unparent()
{
    if( ! _parent )
        return;

    _parent->onRelease(*this);
    _parent->onDetach(*this);
    _parent = 0;
        
    onSetParent(0);
}


void Widget::onAttach(Widget& widget)
{
    _children.push_back(&widget);

    onAddWidget(widget);
}


void Widget::onDetach(Widget& widget)
{
    std::vector<Widget*>::iterator it;
    it = std::find(_children.begin(), _children.end(), &widget);
    if( it != _children.end() )
        _children.erase(it);

    onRemoveWidget(widget);
}


void Widget::onInit(Widget& widget)
{
    Gfx::PaintSurface* surface = _surface.surface();
    Gfx::PointF surfacePos = _surface.area().topLeft() + widget.position();

    widget.setSurface(surface, surfacePos);
    widget.setNextResponder(this);
    widget.setForm(_form);

    double scaling = scaleFactor();
    
    RescaleEvent ev(widget, scaling);
    widget.processEvent(ev);
    //Application::instance().loop().commitEvent(ev);

    relayout();
}


void Widget::onRelease(Widget& widget)
{
    widget.setForm(0);
    widget.setSurface( 0, widget.position() );
    widget.setNextResponder(0);

    relayout();
}


void Widget::setForm(Form* form)
{
    if(_form)
        _form->onRemoveElement(*this);

    _form = 0;

    if(form)
        form->onAddElement(*this);

    _form = form;

    std::vector<Widget*>::iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
    {
        Widget* widget = *it;
        widget->setForm(form);
    }
}


void Widget::add(Widget& w)
{
    w.setParent(*this);
}


void Widget::remove(Widget& w)
{
    w.unparent();
}


void Widget::onAddWidget(Widget& w)
{
}


void Widget::onRemoveWidget(Widget& w)
{
}


const std::vector<Widget*>& Widget::widgets() const
{
    return _children;
}


Gfx::PaintSurface& Widget::surface()
{
    return _surface;
}


const Gfx::PaintSurface& Widget::surface() const
{
    return _surface;
}


void Widget::setSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
{
    if( ! surface )
    {
        _surface.detach();
    }
    else
    {
        Gfx::RectF clientRect( pos, size() );
        _surface.attach(*surface, clientRect);
    }

    onSetSurface(surface, pos);

    std::vector<Widget*>::iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
    {
        Widget* widget = *it;

        Gfx::PaintSurface* surface = _surface.surface();
        Gfx::PointF surfacePos = pos + widget->position();

        widget->setSurface(surface, surfacePos);
    }
}


void Widget::onSetSurface(Gfx::PaintSurface* surface, const Gfx::PointF& pos)
{
}


Gfx::PointF Widget::onToWidget(const Widget& widget, const Gfx::PointF& pos) const
{
    //const Visual* parentView = widget.parent();

    //if( parentView == this || ! parentView )
        return pos - widget.position();

    //return pos - parentView->onToWidget(widget, pos);    
}


Gfx::PointF Widget::onFromWidget(const Widget& widget, const Gfx::PointF& pos) const
{
    //const View* parentView = widget.parent();

    //if( parentView == this || ! parentView )
        return pos + widget.position();

//    return pos + parentView->onFromWidget(widget, pos);
}


const Gfx::RectF Widget::geometry() const
{
    return Gfx::RectF( position(), size() );
}


Widget::FocusPolicy Widget::focusPolicy() const
{
    return _focusPolicy;
}


void Widget::setFocusPolicy(FocusPolicy policy)
{
    _focusPolicy = policy;

    if(_form)
        _form->onSetFocusPolicy(*this, policy);
}


size_t Widget::focusIndex() const
{
    return _focusIndex;
}


void Widget::setFocusIndex(size_t index)
{
    _focusIndex = index;
    
    if(_form)
        _form->onSetFocusIndex(*this, index);
}


bool Widget::hasFocus() const
{
    return _hasFocus;
}


void Widget::focus()
{
    if(_form)
        _form->onSetFocus(*this);
}


void Widget::onProcessFocusEvent(const FocusEvent& ev)
{
    onFocusEvent(ev);
}


void Widget::onFocusEvent(const FocusEvent& ev)
{
    //if( _hasFocus && ! ev.isFocused() )
    //    Application::instance().inputMethod().finish();

    _hasFocus = ev.isFocused();

    // onFocusGained
    // onFocusLost
}


Key Widget::actionKey() const
{
    return _actionKey;
}


void Widget::setActionKey(const Key& ak)
{
    _actionKey = ak;
}


void Widget::onActionKey(const KeyEvent& kev)
{
}


const Key* Widget::shortcut() const
{
    if(_shortcutKey.code() == Key::NoKey)
        return 0;

    return &_shortcutKey;
}


void Widget::setShortcut(const Key* key)
{
    if( ! key )
        _shortcutKey.set(Key::NoKey);
    else
        _shortcutKey = *key;

    if(_form)
        _form->onSetShortcut(*this, key);
}


void Widget::processShortcut(const KeyEvent& kev)
{
    onShortcut(kev);
}


void Widget::onShortcut(const KeyEvent& kev)
{
}


const Pt::Char* Widget::mnemonic() const
{
    return _mnemonic != 0 ? &_mnemonic : 0;
}


void Widget::setMnemonic(const Char& ch)
{
    _mnemonic = ch;

    const Char* m = ch != 0 ? &ch : 0;
    if(_form)
        _form->onSetMnemonic(*this, m);
}


String Widget::setMnemonic(const String& text)
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


void Widget::setMnemonicWidget(Widget* w)
{
    _mnemonicEntered.disconnect();

    if(w)
        _mnemonicEntered += Pt::slot(*w, &Widget::onMnemonic);
}


void Widget::processMnemonic()
{
    onMnemonic();
}


void Widget::onMnemonic()
{
    _mnemonicEntered.invoke();
}


void Widget::onInvalidateEvent(const InvalidateEvent& ev)
{
    Base::onInvalidateEvent(ev);
}


void Widget::onInvalidate()
{
    Base::onInvalidate();

    // TODO: relayout only if required in derived class
    relayout();
}


void Widget::onRequestRepaint(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->onRepaintRequest(*this, rect);
}


void Widget::onRepaintRequest(Widget& w, const Gfx::RectF& rect)
{
    Gfx::PointF widgetPos = fromWidget( w, rect.topLeft() );
    Gfx::RectF widgetRect( widgetPos, rect.size() );

    repaint(widgetRect);
}


void Widget::onProcessPaintEvent(const PaintEvent& ev)
{    
    const Gfx::RectF& r = ev.rect();
    if( r.isNull() )
        return;

    if( ! isVisible() )
        return;
        
    View::onProcessPaintEvent(ev);

    //
    // paint child widgets
    //
    std::vector<Widget*>::const_iterator it;
    for(it = _children.begin() ; it != _children.end(); ++it)
    {
        Widget* w = (*it);

        // clip widget update rect
        Gfx::RectF updateRect = w->geometry().intersect(r);
        if( updateRect.isNull() )
            continue;

        // paint widget rect
        Gfx::PointF updatePos = toWidget( *w , updateRect.topLeft() );
        updateRect.setOrigin(updatePos);

        PaintEvent pev( *w, updateRect );
        w->processEvent(pev);
    }
}


void Widget::onPaintEvent(const PaintEvent& ev)
{    
    //static int nnn = 0;
    //std::clog << "PAINT EVENT: " << typeid(*this).name() << " " << ++nnn << std::endl;

    Base::onPaintEvent(ev);

    onPaint( _surface, ev.rect() );
}


void Widget::onPaint(Gfx::PaintSurface&, const Gfx::RectF&)
{
}


void Widget::onRelayoutRequest(Widget&)
{
    relayout();
}


void Widget::onRequestRelayout()
{
    _isLayoutInvalid = true;

    if(_parent)
        _parent->onRelayoutRequest(*this);
}


void Widget::setSizePolicy(const SizePolicy& policy)
{
     Gfx::SizeF alignedSize = _surface.align( policy.size() );
    
    SizePolicy alignedPolicy = policy;
    alignedPolicy.setSize(alignedSize);
  
    Base::setSizePolicy(alignedPolicy);
}


Gfx::SizeF Widget::onProcessMeasure(const SizePolicy& policy)
{
    _isLayoutInvalid = true;

    if( policy.vertical() == SizePolicy::Fixed &&
        policy.horizontal() == SizePolicy::Fixed &&
        widgets().empty() )
    {
        return policy.size();
    }

    //static int mmm = 0;
    //std::clog << "MEASURE: " << name() << " " << ++mmm << std::endl;
    return onMeasure(policy);
}


Gfx::SizeF Widget::onMeasure(const SizePolicy& policy)
{
   return Gfx::SizeF(0, 0);
}


void Widget::onProcessLayoutEvent(const LayoutEvent& ev)
{
    if( ! _isLayoutInvalid )
        return;

    //
    // align to physical pixel grid
    //
    //const Gfx::RectF& r = ev.rect();
    
    const Gfx::RectF& r = geometry();
    Gfx::RectF rect = _surface.align(r);

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
    std::vector<Widget*>::const_iterator it;
    for(it = _children.begin() ; it != _children.end(); ++it)
    {
        Widget* widget = (*it);

        LayoutEvent ev( *widget, widget->geometry() );
        Application::instance().commitEvent(ev);
    }

    _isLayoutInvalid = false;
}


// TODO: onLayoutEvent
void Widget::onLayoutEvent(const LayoutEvent& ev)
{
    onLayout( ev.rect() );
}


void Widget::onLayout(const Gfx::RectF& rect)
{
    //
    // TODO: no need to pass rect
    //
}


void Widget::onProcessRescaleEvent(const RescaleEvent& ev)
{
    Base::onProcessRescaleEvent(ev);

    double scaling = ev.scaleFactor();

    for (size_t i = 0; i < _children.size(); ++i)
    {
        Widget* widget = _children[i];
        RescaleEvent ev(*widget, scaling);
        widget->processEvent(ev);
    }
}


void Widget::onRescaleEvent(const RescaleEvent& ev)
{
    Base::onRescaleEvent(ev);
}


void Widget::onRescale(double scaling)
{
    Base::onRescale(scaling);

    _margin.set( _surface.align( _margin.left() ),
                 _surface.align( _margin.top() ),
                 _surface.align( _margin.right() ),
                 _surface.align( _margin.bottom() ) );

    _padding.set( _surface.align( _padding.left() ),
                  _surface.align( _padding.top() ),
                  _surface.align( _padding.right() ),
                  _surface.align( _padding.bottom() ) );
    
    _sizePolicy.setSize( _surface.align( _sizePolicy.size() ) );

    // TODO: invalidate in derived class only when neccessary
    invalidate();
}


void Widget::onRequestMove(const Gfx::PointF& pos)
{
    _requestedPosition = pos;

    if(_parent)
        _parent->onMoveRequest(*this, _requestedPosition);
}


void Widget::onMoveRequest(Widget& widget, const Gfx::PointF& pos)
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


//void Widget::onProcessMoveEvent(const MoveEvent& ev)
//{
//    onMoveEvent(ev);
//}


void Widget::onMoveEvent(const MoveEvent& ev)
{
    if( position() == ev.position() )
        return;

    Gfx::PointF delta = ev.position() - position();
    Gfx::PointF surfacePos = _surface.area().topLeft() + delta;

    Gfx::PaintSurface* surface = _surface.surface();
    setSurface(surface, surfacePos);

    Gfx::RectF updateRect( size() );
    updateRect.unify( Gfx::RectF(delta, size()) );
    repaint(updateRect);

    View::onMoveEvent(ev);
}


void Widget::onSetSizeLimits(const Gfx::SizeF& minSize,
                             const Gfx::SizeF& maxSize)
{
    Base::onSetSizeLimits(minSize, maxSize);
    relayout();
}


void Widget::onRequestResize(const Gfx::SizeF& size)
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


void Widget::onResizeRequest(Widget& widget, const Gfx::SizeF& size)
{
    Gfx::SizeF alignedSize = _surface.align(size);

    ResizeEvent rev(widget, alignedSize);
    Application::instance().commitEvent(rev);
}


//void Widget::onProcessResizeEvent(const ResizeEvent& ev)
//{
//    onResizeEvent(ev);
//}


void Widget::onResizeEvent(const ResizeEvent& ev)
{
    if( size() == ev.size() )
        return;

    //std::clog << "RESIZE: " << name() << ev.size().width() << std::endl;

    Gfx::RectF updateRect( size() );
    updateRect.unify( Gfx::RectF( ev.size() ) );
    repaint(updateRect);

    _surface.resize( ev.size() );

    //
    // TODO: layout content
    //
    //LayoutEvent lev( *this, geometry() );
    //Application::instance().commitEvent(lev);

    View::onResizeEvent(ev);
}


bool Widget::acceptsInput() const
{    
    if( ! isEnabled() )
        return false;

    if( ! isVisible() )
        return false;

    return true;
}


void Widget::onRequestShow(bool isShown)
{
    _show = isShown;

    if(_parent)
        _parent->onShowRequest(*this, isShown);
}


void Widget::onProcessShowEvent(const ShowEvent& ev)
{
    Base::onProcessShowEvent(ev);
}


void Widget::onShowEvent(const ShowEvent& ev)
{
    Base::onShowEvent(ev);

    // parent should only layout visible children
    relayout();
}


void Widget::onShow(bool isShown)
{
    Base::onShow(isShown);
}


void Widget::onShowRequest(Widget& widget, bool visible)
{
    ShowEvent sev(widget, visible);
    widget.processEvent(sev);
}


void Widget::onRequestEnable(bool isEnable)
{
    _enabled = isEnable;

    if(_parent)
        _parent->onEnableRequest(*this, isEnable);
}


void Widget::onProcessEnableEvent(const EnableEvent& ev)
{
    bool isEnabled = ev.enabled();
    if( ! _enabled )
        isEnabled = false;

    EnableEvent eev(*this, isEnabled);
    Base::onProcessEnableEvent(eev);

    for( size_t i = 0; i < _children.size(); ++i)
    {
        Widget* w = _children[i];
        
        EnableEvent widgetEvent(*w, isEnabled);
        w->processEvent(widgetEvent);
    }
}


void Widget::onEnableEvent(const EnableEvent& ev)
{
    Base::onEnableEvent(ev);

    // TODO: move to derived classes
    invalidate();
}


void Widget::onEnable(bool e)
{
    Base::onEnable(e);
}


void Widget::onEnableRequest(Widget& widget, bool enable)
{
    if( ! isEnabled() )
      enable = false;

    EnableEvent eev(widget, enable);
    widget.processEvent(eev);
}


void Widget::activate(bool active)
{
    if(_parent)
        _parent->onActivateRequest(*this, active);
}


void Widget::onActivateRequest(Widget& w, bool active)
{
    if(_parent)
        _parent->onActivateRequest(*this, active);
}


void Widget::raise()
{
    if(_parent)
        _parent->onRaiseRequest(*this);
}


void Widget::onRaiseRequest(Widget& w)
{
    std::vector<Widget*>::iterator it = std::find(_children.begin(), 
                                                  _children.end(), &w);
    if( it == _children.end() )
        return;

    _children.erase(it);
    _children.push_back(&w);

    w.repaint( w.bounds() );
}


const Cursor* Widget::cursor() const
{
    if( ! _hasCursor )
        return &Cursor::defaultCursor();

    return &_cursor;
}


void Widget::setCursor(const Cursor* c)
{
    if( ! c )
    {
        _hasCursor = false;
        _cursor.clear();
        return;
    }

    _hasCursor = true;
    _cursor = *c;

    // TODO: if this is the pointer widget do the same as onEnterEvent
    // Application::instance().screen().setCursor( &cursor() );
}


const Spacing& Widget::margin() const
{
    return _margin;
}


void Widget::setMargin(const Spacing& s)
{  
    _margin.set( _surface.align( s.left() ),
                 _surface.align( s.top() ),
                 _surface.align( s.right() ),
                 _surface.align( s.bottom() ) );

    relayout();
}


void Widget::setMargin(double n)
{
    setMargin( Spacing(n) );
}


void Widget::setMargin(double horiz, double vertical)
{
    setMargin( Spacing(horiz, vertical) );
}


const Spacing& Widget::padding() const
{
    return _padding;
}


void Widget::setPadding( const Spacing& p )
{   
    _padding.set( _surface.align( p.left() ),
                  _surface.align( p.top() ),
                  _surface.align( p.right() ),
                  _surface.align( p.bottom() ) );

    relayout();
}


void Widget::setPadding(double n)
{
    setPadding( Spacing(n) );
}


void Widget::setPadding(double horiz, double vertical)
{
    setPadding( Spacing(horiz, vertical) );
}

//
// Visual
//

Visual* Widget::onHitTest(const Gfx::PointF& p)
{
    if( ! bounds().contains(p) )
        return 0;

    std::vector<Widget*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Widget* w = *it;
        Gfx::PointF pos = toWidget(*w, p);
        Visual* hit = w->hitTest(pos);
        if(hit)
            return hit;
    }

    return this;
}


Gfx::PointF Widget::onToParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->onFromWidget(*this, pos);
}


Gfx::PointF Widget::onFromParent(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    return _parent->onToWidget(*this, pos);
}


void Widget::onProcessEvent(const Pt::Event& ev)
{
    View::onProcessEvent(ev);
}


void Widget::onProcessMouseEvent(const MouseEvent& ev)
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

    std::vector<Widget*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Widget* widget = *it;

        if( widget->geometry().contains(pos) && 
            widget->acceptsInput() )
        {
            widget->processEvent(ev);
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


bool Widget::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    if( ev.isPress(MouseEvent::Left) )
    {
        focus();
    }

    return false;
}


void Widget::onProcessTouchEvent(const TouchEvent& ev)
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

    std::vector<Widget*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Widget* widget = *it;

        if( widget->geometry().contains(pos) && 
            widget->acceptsInput() )
        {
            widget->processEvent(ev);
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


bool Widget::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    if( ev.isPress() )
    {
        focus();
    }

    return false;
}


void Widget::onProcessScrollEvent(const ScrollEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Base::onProcessScrollEvent(ev);
}


bool Widget::onScrollEvent(const ScrollEvent& ev)
{
    return Base::onScrollEvent(ev);
}


void Widget::onProcessEnterEvent(const EnterEvent& ev)
{
    Base::onProcessEnterEvent(ev);
}


bool Widget::onEnterEvent( const EnterEvent& ev)
{
    //std::clog << "ENTER: " << typeid(*this).name() << " " << vid() << std::endl;
    Application::instance().setCursor( cursor() );

    return Base::onEnterEvent(ev);
}


void Widget::onProcessLeaveEvent(const LeaveEvent& ev)
{
    Base::onProcessLeaveEvent(ev);
}


bool Widget::onLeaveEvent(const LeaveEvent& ev)
{
    //std::clog << "LEAVE: " << typeid(*this).name() << " " << vid() << std::endl;
    Application::instance().setCursor( &Cursor::defaultCursor() );
    
    return Base::onLeaveEvent(ev);
}


void Widget::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! acceptsInput() )
        return;

    Base::onProcessKeyEvent(ev);
}


bool Widget::onKeyEvent(const KeyEvent& ev)
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
