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
, _nextResponder(0)
, _pointer(0)
, _capture(0)
, _isCapture(false)
, _isLayoutInvalid(true)
, _visible(true)
, _enabled(true)
//, _enabledState(true)
, _hasFocus(false)
, _focusPolicy(NoFocus)
, _focusIndex(0)
, _hasCursor(false)
, _cursor()
, _actionKey(Key::Space)
, _mnemonic(0)
{
    _eventReceived += Pt::slot(*this, &Widget::onProcessMouseEvent);
    _eventReceived += Pt::slot(*this, &Widget::onProcessTouchEvent);
    _eventReceived += Pt::slot(*this, &Widget::onProcessScrollEvent);
    _eventReceived += Pt::slot(*this, &Widget::onProcessEnterEvent);
    _eventReceived += Pt::slot(*this, &Widget::onProcessLeaveEvent);
    _eventReceived += Pt::slot(*this, &Widget::onProcessKeyEvent);

    //_eventReceived += Pt::slot(*this, &Widget::onProcessInvalidateEvent);
    //_eventReceived += Pt::slot(*this, &Widget::onProcessPaintEvent);
    _eventReceived += Pt::slot(*this, &Widget::onProcessLayoutEvent);
    //_eventReceived += Pt::slot(*this, &Widget::onProcessRescaleEvent);
    //_eventReceived += Pt::slot(*this, &Widget::onProcessMoveEvent);
    //_eventReceived += Pt::slot(*this, &Widget::onProcessResizeEvent);
    _eventReceived += Pt::slot(*this, &Widget::onProcessShowEvent);
    //_eventReceived += Pt::slot(*this, &Widget::onProcessEnableEvent);
    
    _eventReceived += Pt::slot(*this, &Widget::onProcessFocusEvent);
}


Widget::~Widget()
{
    while( ! _children.empty() )
        remove( *_children.back() );

    setParent(0);
}

// implment add method in derived class
void Widget::add(Widget& w)
{
    w.setParent(this);
}

// implment remove method in derived class
void Widget::remove(Widget& w)
{
    w.setParent(0);
}


void Widget::setNextResponder(Responder* r)
{
    _nextResponder = r;
}


Gfx::PaintSurface& Widget::surface()
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

    std::vector<Widget*>::iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
    {
        Widget* widget = *it;

        Gfx::PaintSurface* surface = _surface.surface();
        Gfx::PointF surfacePos = pos + widget->position();

        widget->setSurface(surface, surfacePos);
    }
}


//const View* Widget::parent() const
//{
//    return _parent;
//}
//
//
//View* Widget::parent()
//{
//    return _parent;
//}


void Widget::setParent(View* parent)
{
    if(_parent == parent)
        return;

    unparent();

    if(parent)
    {
        parent->onAttach(*this);
        _parent = parent;

        _parent->onInit(*this);

        _parent->onEnableRequest(*this, _enabled);
        _parent->onShow(*this, _visible);

        onParentChanged(_parent);
    }
}


void Widget::unparent()
{
    if( ! _parent )
        return;

    _parent->onRelease(*this);
    _parent->onDetach(*this);
    _parent = 0;
        
    onParentChanged(0);
}


void Widget::setForm(Form* form)
{
    if(_form)
        _form->onDeregister(*this);

    _form = 0;

    if(form)
        form->onRegister(*this);

    _form = form;

    std::vector<Widget*>::iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
    {
        Widget* widget = *it;
        widget->setForm(form);
    }
}


void Widget::onAttach(Widget& widget)
{
    _children.push_back(&widget);

    onAddWidget(widget);
}


void Widget::onDetach(Widget& widget)
{
    if(_pointer == &widget)
        _pointer = 0;

    if(_capture == &widget)
        _capture = 0;

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
    //w.processEvent(ev);
    Application::instance().loop().commitEvent(ev);

    relayout();
}


void Widget::onRelease(Widget& widget)
{
    widget.setForm(0);
    widget.setSurface( 0, widget.position() );
    widget.setNextResponder(0);

    relayout();
}


void Widget::onAddWidget(Widget& w)
{
}


void Widget::onRemoveWidget(Widget& w)
{
}


void Widget::onParentChanged(View*)
{
}


const std::vector<Widget*>& Widget::widgets() const
{
    return _children;
}


Widget* Widget::findWidget(const Gfx::PointF& pos)
{
    std::vector<Widget*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Widget* widget = *it;

        if( widget->geometry().contains(pos) )
        {
            Gfx::PointF p = toWidget(*widget, pos);
            Widget* found = widget->findWidget(p);
            return found ? found : widget;
        }
    }

    return 0;
}


Widget* Widget::findWidget(Pt::uint64_t vid)
{
    std::vector<Widget*>::const_iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
    {
        Widget* child = *it;

        if( child->vid() == vid )
            return child;

        Widget* widget = child->findWidget(vid);
        if( widget )
            return widget;
    }

    return 0;
}


Widget* Widget::findWidget(const std::string& name)
{
    std::vector<Widget*>::const_iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
    {
        Widget* child = *it;

        if( child->name() == name )
            return child;

        Widget* widget = child->findWidget(name);
        if( widget )
            return widget;
    }

    return 0;
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


//const Gfx::PointF& Widget::position() const
//{
//    return _position;
//}
//
//
//const Gfx::SizeF& Widget::size() const
//{
//    return _size;
//}


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


//void Widget::invalidate()
//{
//    ++_invalidates;
//
//    InvalidateEvent ev(*this);
//    Application::instance().commitEvent(ev);
//}


//void Widget::onInvalidateRequest()
//{
//    ++_invalidates;
//
//    InvalidateEvent ev(*this);
//    Application::instance().commitEvent(ev);
//}


//void Widget::onProcessInvalidateEvent(const InvalidateEvent& ev)
//{
//    --_invalidates;
//
//    if(_invalidates > 0)
//      return;
//
//    onInvalidateEvent(ev);
//}


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


void Widget::onRepaintRequest(const Gfx::RectF& rect)
{
    if(_parent)
        _parent->onRepaint(*this, rect);
}


void Widget::onRepaint(Widget& w, const Gfx::RectF& rect)
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
    View::onPaintEvent(ev);

    const Gfx::RectF& rect = ev.rect();

    //static int nnn = 0;
    //std::clog << "PAINT EVENT: " << typeid(*this).name() << " " << ++nnn << std::endl;

    Gfx::RectF updateRect = rect.intersect( bounds() );
    onPaint(_surface, updateRect);
}


void Widget::onPaint(Gfx::PaintSurface&, const Gfx::RectF&)
{
}


void Widget::onRelayout(Widget&)
{   
    //std::clog << "RELAYOUT" << name() <<  std::endl;

    relayout();
}


void Widget::relayout()
{
    if(_isLayoutInvalid)
        return;

    _isLayoutInvalid = true;

    if(_parent)
        _parent->onRelayout(*this);
}


const SizePolicy& Widget::sizePolicy() const
{
    return _sizePolicy;
}


void Widget::setSizePolicy(const SizePolicy& policy)
{
    _sizePolicy = policy;

    Gfx::SizeF alignedSize = _surface.align( policy.size() );
    _sizePolicy.setSize(alignedSize);

    relayout();
}


Gfx::SizeF Widget::preferredSize() const
{
    return _preferredSize;
}


Gfx::SizeF Widget::measure(const SizePolicy& policy)
{
    //static int nnn = 0;
    //std::clog << "MEASURE: " << typeid(*this).name() << " " << ++nnn << std::endl;

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
        contentPolicy.height() < _minimumSize.height() )
    {
        contentPolicy.setHeight( _minimumSize.height() );
    }

    // apply minimum width, unless the size mode is fixed
    if( contentPolicy.horizontal() != SizePolicy::Fixed &&
        contentPolicy.width() < _minimumSize.width() )
    {
        contentPolicy.setWidth( _minimumSize.width() );
    }

    bool doMeasure = contentPolicy != _lastPolicy || _isLayoutInvalid;

    if(doMeasure)
    {
        _lastPolicy = contentPolicy;

        if( contentPolicy.vertical() != SizePolicy::Fixed ||
            contentPolicy.horizontal() != SizePolicy::Fixed ||
            ! widgets().empty() )
        {
            //static int mmm = 0;
            //std::clog << "ON MEASURE: " << typeid(*this).name() << " " << ++mmm << std::endl;
            _preferredSize = onMeasure(contentPolicy);
        }

        // use fixed height, if size mode is fixed
        if(contentPolicy.vertical() == SizePolicy::Fixed)
            _preferredSize.setHeight( contentPolicy.height() );
        else if( _preferredSize.height() < _minimumSize.height() )
            _preferredSize.setHeight( _minimumSize.height() );

        if(contentPolicy.vertical() == SizePolicy::Maximum)
            _preferredSize.setHeight( std::min( _preferredSize.height(),
                                                contentPolicy.height() ) );

        // use fixed width, if size mode is fixed
        if(contentPolicy.horizontal() == SizePolicy::Fixed)
            _preferredSize.setWidth( contentPolicy.width() );
        else if( _preferredSize.width() < _minimumSize.width() )
            _preferredSize.setWidth( _minimumSize.width() );

        if(contentPolicy.horizontal() == SizePolicy::Maximum)
            _preferredSize.setWidth( std::min( _preferredSize.width(),
                                               contentPolicy.width() ) );
    }

    return preferredSize();
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

    //
    // layout position and size of contents 
    //
    onLayout(rect);
    
    //
    // layout content marked invalid
    //
    std::vector<Widget*>::const_iterator it;
    for(it = _children.begin() ; it != _children.end(); ++it)
    {
        Widget* widget = (*it);

        LayoutEvent ev( *widget, widget->geometry() );
        widget->processEvent(ev);
    }

    _isLayoutInvalid = false;
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

    invalidate();
    relayout();
}


void Widget::move(const Gfx::PointF& pos)
{
    //
    // align to physical pixel grid
    //
    Gfx::PointF aligedPos = _surface.align(pos);

    if( position() == aligedPos )
    {
        //std::clog << "MOVE skipped: " << typeid(*this).name() << std::endl;
        return;
    }

    //
    // unified repaint area
    //
    Gfx::RectF updateRect( size() );
    updateRect.unify( Gfx::RectF(aligedPos, size()) );

    //
    // send move event
    //
    MoveEvent mev(*this, aligedPos);
    /// XXX Application::instance().processEvent(mev);
    Application::instance().commitEvent(mev);

    if(_parent)
        _parent->onMove(*this, aligedPos);

    //
    // request repaint
    //
    //std::clog << "MOVE REPAINT: " << typeid(*this).name() << std::endl;
    repaint(updateRect);
}


void Widget::onMove(Widget& widget, const Gfx::PointF& pos)
{
    Gfx::PointF surfacePos = _surface.area().topLeft() + pos;
    Gfx::PaintSurface* surface = _surface.surface();
    widget.setSurface(surface, surfacePos);
}


//void Widget::onProcessMoveEvent(const MoveEvent& ev)
//{
//    onMoveEvent(ev);
//}


void Widget::onMoveEvent(const MoveEvent& ev)
{
    View::onMoveEvent(ev);

//    if( _position == ev.position() )
//        return;
//
//    //static int nn = 0;
//    //std::clog << "MOVE: " << typeid(*this).name() << " " << ++nn << std::endl;
//
//    _position = ev.position();
}


void Widget::resize(const Gfx::SizeF& size)
{   
    Gfx::SizeF alignedSize = _surface.align(size);

    if( this->size() == alignedSize )
    {
        //std::clog << "RESIZE skipped: " << typeid(*this).name() << std::endl;
        return;
    }
    
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

    ResizeEvent rev(*this, alignedSize);
    /// XXX Application::instance().processEvent(rev);
    Application::instance().commitEvent(rev);

    if(_parent)
        _parent->onResize(*this, alignedSize);

    //std::clog << "RESIZE REPAINT: " << typeid(*this).name() << std::endl;
    Gfx::RectF updateRect( this->size() );

    Gfx::RectF resizedRect(size);
    updateRect.unify(resizedRect);

    repaint(updateRect);
}


void Widget::onResize(Widget& widget, const Gfx::SizeF& size)
{
}


//void Widget::onProcessResizeEvent(const ResizeEvent& ev)
//{
//    onResizeEvent(ev);
//}


void Widget::onResizeEvent(const ResizeEvent& ev)
{
    if( size() == ev.size() )
        return;

    View::onResizeEvent(ev);

    //static int nn = 0;
    //std::clog << "RESIZE: " << typeid(*this).name() << " " << ++nn << std::endl;

    //_size = ev.size();
    _surface.resize( ev.size() );

    //
    // layout contents 
    //
    Gfx::RectF geometry( position(), ev.size() );
    onLayout(geometry);
}


bool Widget::acceptsInput() const
{    
    if( ! isEnabled() )
        return false;

    if( ! isVisible() )
        return false;

    return true;
}


bool Widget::isVisible() const
{
    return _visible;
}


void Widget::show(bool s)
{
    _visible = s;

    if(_parent)
        _parent->onShow(*this, s);
    else  
        _visible = s;
}


void Widget::onShow(Widget& widget, bool visible)
{
    ShowEvent sev(widget, visible);
    widget.processEvent(sev);
}


void Widget::onProcessShowEvent(const ShowEvent& ev)
{
    onShow( ev.visible() );
}


void Widget::onShow(bool isShown)
{
    _visible = isShown;

    relayout();
    repaint( bounds() );
}


//bool Widget::isEnabled() const
//{
//    return _enabledState && _enabled;
//}


void Widget::enable(bool e)
{
    _enabled = e;

    if(_parent)
        _parent->onEnableRequest(*this, e);
}


void Widget::onProcessEnableEvent(const EnableEvent& ev)
{
    Base::onProcessEnableEvent(ev);

    for( size_t i = 0; i < _children.size(); ++i)
    {
        Widget* w = _children[i];
        
        EnableEvent eev(*w, ev.enabled());
        w->processEvent(eev);
    }
}


void Widget::onEnableEvent(const EnableEvent& ev)
{
    Base::onEnableEvent(ev);
}


void Widget::onEnable(bool e)
{
    Base::onEnable(e);

    invalidate();
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
        _parent->onActivate(*this, active);
}


void Widget::onActivate(Widget& w, bool active)
{
    if(_parent)
        _parent->onActivate(*this, active);
}


void Widget::raise()
{
    if(_parent)
        _parent->onRaise(*this);
}


void Widget::onRaise(Widget& w)
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
        return 0;

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


const Gfx::SizeF& Widget::minimumSize() const
{
    return _minimumSize;
}


void Widget::setMinimumSize(const Gfx::SizeF& s)
{
    _minimumSize = s;
}


void Widget::setMinimumSize(double w, double h)
{
    _minimumSize.set(w, h);
}


double Widget::minimumWidth() const
{
    return _minimumSize.width();
}


void Widget::setMinimumWidth(double h)
{
    _minimumSize.setWidth(h);
    relayout();
}


double Widget::minimumHeight() const
{
    return _minimumSize.height();
}


void Widget::setMinimumHeight(double h)
{
    _minimumSize.setHeight(h);
    relayout();
}


Pt::Signal<const Pt::Event&>& Widget::eventReceived()
{
    return _eventReceived;
}


Visual* Widget::onGetParent() const
{
    return _parent;
}


Visual* Widget::onHitTest(const Gfx::PointF& p)
{
    std::vector<Widget*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Widget* w = *it;
        Gfx::PointF pos = toWidget(*w, p);
        Visual* hit = w->hitTest(pos);
        if(hit)
            return hit;
    }

    Gfx::RectF bounds( size() );
    if( bounds.contains(p) )
        return this;

    return 0;
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


void Widget::onEvent(const Pt::Event& ev)
{
    View::onEvent(ev);

    _eventReceived.send(ev);
}


Responder* Widget::onNextResponder()
{
    return _nextResponder;
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
    Visual* visual = 0;

    std::vector<Widget*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Widget* widget = *it;

        if( widget->geometry().contains(pos) && 
            widget->acceptsInput() )
        {
            visual = widget;
            break;
        }
    }

    _pointer = visual;

    if(visual)
    {
        //
        // start press sequence capture
        // 
        if( ev.isPress() )
            _capture = visual;

      visual->processEvent(ev);
      return;
    }

    //
    // handle event
    //
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

    //if(_pointer)
    //{
    //    _pointer->processEvent(ev);
    //    return; 
    //}

    scrollEvent(ev);
}


bool Widget::onScrollEvent(const ScrollEvent& ev)
{
    return false;
}


void Widget::onProcessEnterEvent(const EnterEvent& ev)
{
    enterEvent(ev);
}


bool Widget::onEnterEvent( const EnterEvent& ev)
{
    //std::clog << "ENTER: " << typeid(*this).name() << " " << vid() << std::endl;
    Application::instance().setCursor( cursor() );
    return true;
}


void Widget::onProcessLeaveEvent(const LeaveEvent& ev)
{
    leaveEvent(ev);
}


bool Widget::onLeaveEvent(const LeaveEvent& ev)
{
    //std::clog << "LEAVE: " << typeid(*this).name() << " " << vid() << std::endl;
    Application::instance().setCursor(0);
    return true;
}


void Widget::onProcessKeyEvent(const KeyEvent& ev)
{
    if( ! acceptsInput() )
        return;

    keyEvent(ev);
}


bool Widget::onKeyEvent(const KeyEvent& ev)
{
    if( ev.key() == actionKey() && hasFocus() )
    {
        onActionKey(ev);
        return true;
    }

    return false;
}

} // namespace

} // namespace
