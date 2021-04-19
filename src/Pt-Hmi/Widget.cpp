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
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Layout.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Screen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/String.h>

#include <cmath>
#include <cassert>

namespace Pt {

namespace Hmi {

Widget::Widget()
: _screen(0)
, _window(0)
, _parent(0)
, _invalidates(0)
, _isLayoutInvalid(true)
, _visible(true)
, _enabled(true)
, _enabledState(true)
, _hasFocus(false)
, _focusPolicy(NoFocus)
, _focusIndex(0)
, _acceptsInput(true)
, _textInput(false)
, _cursor( Hmi::Cursor::defaultCursor() )
, _actionKey(Key::Space)
, _mnemonic(0)
{
    _eventReady += Pt::slot(*this, &Widget::onKeyEvent);
    _eventReady += Pt::slot(*this, &Widget::scrollEvent);
    _eventReady += Pt::slot(*this, &Widget::onMoveEvent);
    _eventReady += Pt::slot(*this, &Widget::onResizeEvent);
    _eventReady += Pt::slot(*this, &Widget::mouseEvent);
    _eventReady += Pt::slot(*this, &Widget::touchEvent);
    _eventReady += Pt::slot(*this, &Widget::onEnterEvent);
    _eventReady += Pt::slot(*this, &Widget::onLeaveEvent);
    _eventReady += Pt::slot(*this, &Widget::onEnableEvent);
    _eventReady += Pt::slot(*this, &Widget::onFocusEvent);
    _eventReady += Pt::slot(*this, &Widget::onShowEvent);
    _eventReady += Pt::slot(*this, &Widget::onInvalidateEvent);
}


Widget::~Widget()
{
    releasePointer();

    while( ! _children.empty() )
        remove( *_children.back() );

    if(_parent)
        _parent->remove(*this);

    if(_window)
        _window->removeWidget(*this);
}


Window* Widget::window()
{
    return _window;
}


const Window* Widget::window() const
{
    return _window;
}


Widget* Widget::parent()
{
    return _parent;
}


const Widget* Widget::parent() const
{
    return _parent;
}


void Widget::add(Widget& widget)
{
    if(widget.parent() == this)
        return;

    if( widget.parent() )
        widget.parent()->remove(widget);

    _children.push_back(&widget);

    // disable indirectly, when parent is disabled
    if( ! isEnabled() && widget.isEnabled() )
    {
        EnableEvent eev( widget.vid(), false);
        Application::instance().loop().commitEvent(eev);
    }

    widget.setParent(this);
    widget.setWindow(_window);
    widget.setScreen(_screen);

    relayout();
    widget.update();

    onAddWidget(widget);
}


void Widget::remove(Widget& widget)
{
    std::vector<Widget*>::iterator it;
    it = std::find(_children.begin(), _children.end(), &widget);
    if( it == _children.end() )
        return;

    _children.erase(it);

    // enable when indirectly disabled
    if( ! widget._enabledState && widget._enabled)
        widget.enable(true);

    widget.setScreen(0);
    widget.setParent(0);
    widget.setWindow(0);

    relayout();
    update();

    onRemoveWidget(widget);
}


void Widget::onSetWindow(Window* w)
{
}


void Widget::onSetScreen(Screen* s)
{
}


void Widget::onAddWidget(Widget& w)
{
}


void Widget::onRemoveWidget(Widget& w)
{
}


void Widget::onParentChanged(Widget* w)
{
}


void Widget::setScreen(Screen* screen)
{
    if(_screen == screen)
        return;

    _screen = screen;

    invalidate();

    relayout();

    _margin = align(_margin);
    _padding = align(_padding);

    _sizePolicy.setSize( align( _sizePolicy.size() ) );

    for (size_t i = 0; i < _children.size(); ++i)
        _children[i]->setScreen(screen);

    onSetScreen(screen);
}


void Widget::setParent(Widget* parent)
{
    _parent = parent;
    onParentChanged(parent);
}


void Widget::setWindow(Window* window)
{
    if(_window)
        _window->removeWidget(*this);

    _window = window;

    std::vector<Widget*>::iterator it;
    for(it = _children.begin(); it != _children.end(); ++it)
        (*it)->setWindow(window);

    if( _window )
        _window->addWidget(*this);

    // this serves as the initial invalidate
    invalidate();

    // previous relayouts did not work without parent window
    relayout();

    onSetWindow(window);
}


const std::vector<Widget*>& Widget::widgets() const
{
    return _children;
}


Widget* Widget::findWidget(const Gfx::PointF& pos, bool input)
{
    if( ! isVisible() || ! isEnabled() )
        return 0;

    std::vector<Widget*>::reverse_iterator it;
    for(it = _children.rbegin(); it != _children.rend(); ++it)
    {
        Widget* child = *it;

        if( ! child->isVisible() || ! child->isEnabled() )
            continue;

        if( ! child->geometry().contains(pos) )
            continue;

        Gfx::PointF p = child->fromParent(pos);
        Widget* found = child->findWidget(p, input);

        if( ! input)
            return found ? found : child;

        if( found && found->acceptsInput() )
            return found;

        if( child->acceptsInput() )
            return child;

        break;
    }

    return 0;
}


Widget* Widget::findWidget(const Gfx::PointF& pos)
{
    return findWidget(pos, false);
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


Gfx::PointF Widget::fromWindow(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    Gfx::PointF p = _parent->fromWindow(pos);
    return p - _position;
}


Gfx::PointF Widget::toWindow(const Gfx::PointF& pos) const
{
    if( ! _parent )
        return pos;

    Gfx::PointF p = pos + _position;
    return _parent->toWindow(p);
}


Visual* Widget::onParent() const
{
    if(_parent)
        return _parent;

    return _window;
}


Gfx::PointF Widget::onToParent(const Gfx::PointF& pos) const
{
    return  pos + _position;
}


Gfx::PointF Widget::onFromParent(const Gfx::PointF& pos) const
{
    return pos - _position;
}


double Widget::onScaleFactor() const
{
    if( window() )
        return window()->scaleFactor();

    return 1.0;
}


bool Widget::acceptsInput() const
{
  return _acceptsInput;
}


void Widget::setAcceptInput(bool a)
{
    _acceptsInput = a;
}


void Widget::setTextInput(bool b)
{
    _textInput = b;
}


bool Widget::isTextInput() const
{
    return _textInput;
}


void Widget::setFocusPolicy(FocusPolicy f)
{
    _focusPolicy = f;
}


Widget::FocusPolicy Widget::focusPolicy() const
{
    return _focusPolicy;
}


size_t Widget::focusIndex() const
{
    return _focusIndex;
}


bool Widget::hasFocus() const
{
    return _hasFocus;
}


void Widget::focus()
{
    if(_window)
        _window->setFocusWidget(this);
}


void Widget::setFocusIndex(size_t index)
{
    _focusIndex = index;

    if( _window )
        _window->setFocusIndex(*this, index);
}


void Widget::onFocusEvent(const FocusEvent& ev)
{
    _hasFocus = ev.isFocused();
}


Key Widget::actionKey() const
{
    return _actionKey;
}


void Widget::setActionKey(const Key& ak)
{
    _actionKey = ak;
    this->onSetActionKey(ak);
}


void Widget::onSetActionKey(const Key&)
{
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


void Widget::setShortcut(const Key* k)
{
    if( ! k )
        _shortcutKey.set(Key::NoKey);
    else
        _shortcutKey = *k;

    if(_window)
        _window->setShortcut(*this, k);

    this->onSetShortcut(k);
}


void Widget::onSetShortcut(const Key*)
{
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
    if( _window )
        _window->setMnemonic(*this, m);
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


void Widget::onMnemonic()
{
    _mnemonicEntered.invoke();
}


const SizePolicy& Widget::sizePolicy() const
{
    return _sizePolicy;
}


void Widget::setSizePolicy(const SizePolicy& policy)
{
    _sizePolicy = policy;
    _sizePolicy.setSize( align(policy.size()) );

    relayout();
}


void Widget::invalidate()
{
    ++_invalidates;

    InvalidateEvent ev( vid() );
    Application::instance().loop().commitEvent(ev);
}


void Widget::onInvalidateEvent(const InvalidateEvent& ev)
{
    --_invalidates;

    if(_invalidates > 0)
      return;

    onInvalidate();

    relayout();
}


void Widget::onInvalidate()
{
}


//void Widget::repaint()
//{
//    Gfx::RectF rect( Gfx::PointF(0, 0), size() );
//    repaint(rect);
//}
//
//
//void Widget::repaint(const Gfx::RectF& rect)
//{
//    // Window* parentWindow = window();
//    // Widget* parentWidget = parent();
//
//    // if(parentWidget)
//    // {
//    //     Gfx::PointF updatePos = toParent( rect.topLeft() );
//    //     Gfx::RectF updateRect( updatePos, rect.size() );
//
//    //     parentWidget->repaint(updateRect);
//    // }
//    // else if(parentWindow)
//    // {
//    //     Gfx::PointF updatePos = toParent( rect.topLeft() );
//    //     Gfx::RectF updateRect( updatePos, rect.size() );
//
//    //     parentWindow->repaint(updateRect);
//    // }
//
//    Visual* pr = parent();
//    if( ! pr )
//        pr = window();
//
//    if( ! pr )
//        return;
//
//    Gfx::PointF updatePos = toParent( rect.topLeft() );
//    Gfx::RectF updateRect( updatePos, rect.size() );
//    pr->repaint(updateRect);
//}


void Widget::onRepaint(const Gfx::RectF& rect)
{
    Visual* p = Visual::parent();
    if(p)
    {
        Gfx::PointF parentPos = toParent( rect.topLeft() );
        Gfx::RectF parentRect( parentPos, rect.size() );
        p->repaint(parentRect);
    }
}

// onPaint
void Widget::onPaintContent(const Gfx::RectF& r)
{
    if( r.isNull() )
        return;

    if( ! isVisible() )
        return;

    Gfx::RectF rect = r.intersect( Gfx::RectF(_size) );

    // onPaintContent (widget specific)
    PaintEvent pev( vid(), rect );
    onPaintEvent(pev);

    std::vector<Widget*>::const_iterator it;
    for(it = _children.begin() ; it != _children.end(); ++it)
    {
        Widget* w = (*it);

        // clip widget update rect
        Gfx::RectF updateRect = w->geometry().intersect(rect);
        if( updateRect.isNull() )
            continue;

        // paint widget rect
        Gfx::PointF updatePos = w->fromParent( updateRect.topLeft() );
        updateRect.setOrigin(updatePos);
        paintContent(*w, updateRect);
    }
}

// onPaintContent (widget specific)
void Widget::onPaintEvent(const PaintEvent& ev)
{
}


void Widget::relayout()
{
    _isLayoutInvalid = true;

    Window* parentWindow = window();
    Widget* parentWidget = parent();
    
    if(parentWidget)
    {
        parentWidget->relayout();
    }
    else if(parentWindow)
    {
        parentWindow->relayout();
    }
}


Gfx::SizeF Widget::preferredSize() const
{
    return _preferredSize;
}


const Gfx::PointF& Widget::position() const
{
    return _position;
}


const Gfx::SizeF& Widget::onSize() const
{
    return _size;
}


const Gfx::RectF Widget::geometry() const
{
    return Gfx::RectF( position(), size() );
}

// onMeasure
Gfx::SizeF Widget::onMeasureContent(const SizePolicy& policy)
{
    //static int nn = 0;
    //std::clog << "MEASURE: " << typeid(*this).name() << " " << ++nn << std::endl;

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

    return _preferredSize;
}

// onMeasureContent
Gfx::SizeF Widget::onMeasure(const SizePolicy& policy)
{
   return Gfx::SizeF(0, 0);
}

// onLayout
void Widget::onLayoutContent(const Gfx::RectF& r)
{
    //static int nn = 0;
    //std::clog << "LAYOUT: " << typeid(*this).name() << " " << ++nn << std::endl;

    //
    // align to physical pixel grid
    //
    Gfx::RectF rect = align(r);

    //
    // update widget position
    //
    bool moved = rect.topLeft() != _position;
    if(moved)
    {
        Gfx::PointF p = align( rect.topLeft() );
        Gfx::PointF to = p - _position;

        Gfx::RectF updateRect(_size);
        updateRect.unify( Gfx::RectF(to, _size) );

        MoveEvent mev(vid(), p);
        onMoveEvent(mev);

        repaint(updateRect);
    
        _position = p;
    }

    //
    // update widget size
    //
    bool resized = rect.size() != _size;
    if(resized)
    {
        const Gfx::SizeF& s = align( rect.size() );

        Gfx::SizeF updateSize( std::max( _size.width(), s.width()),
                               std::max( _size.height(), s.height()) );

        Gfx::RectF updateRect(Gfx::PointF(0,0), updateSize);

        ResizeEvent rev(vid(), s);
        onResizeEvent(rev);

        repaint(updateRect);

        _size = s;
    }

    //
    // layout content
    //
    if(resized /*|| moved*/)
        _isLayoutInvalid = true;

    if(_isLayoutInvalid)
    {
        onLayout(rect);
    }

    _isLayoutInvalid = false;
}

// onLayoutContent
void Widget::onLayout(const Gfx::RectF& rect)
{
}


void Widget::onMoveEvent(const MoveEvent& ev)
{
    _position = ev.position();
}


void Widget::onResizeEvent(const ResizeEvent& ev)
{
    _size = ev.size();
}


bool Widget::isVisible() const
{
    return _visible;
}


void Widget::show(bool s)
{
    if(_visible == s)
        return;

    _visible = s;

    if( parent() )
        parent()->relayout();

    ShowEvent ev(vid(), s);
    Application::instance().loop().commitEvent(ev);

    repaint();
}


void Widget::onShowEvent(const ShowEvent& ev)
{
}


bool Widget::isEnabled() const
{
    return _enabledState && _enabled;
}


void Widget::enable(bool e)
{
    _enabled = e;
    _enabledState = e;

    EnableEvent eev( vid(), e);
    Application::instance().loop().commitEvent(eev);

    invalidate();
}


void Widget::onEnableEvent(const EnableEvent& ev)
{
    _enabledState = ev.enabled();

    for( size_t i = 0; i < _children.size(); ++i)
    {
        Widget* w = _children[i];

        // skip directly disabled children, because they are 
        // either already disabled or they should not be enabled
        if( ! w->_enabled )
            continue;

        EnableEvent eev( w->vid(), ev.enabled());
        Application::instance().loop().commitEvent(eev);
    }
}


void Widget::raise()
{
    if( ! _parent )
        return;

    _parent->onRaise(*this);
}


void Widget::onRaise(Widget& w)
{
    std::vector<Widget*>::iterator it = std::find(_children.begin(), _children.end(), &w);

    if( it == _children.end() )
        return;

    _children.erase(it);
    _children.push_back(&w);

    w.repaint();
}


void Widget::grabPointer()
{
    Application::instance().grabPointer(*this);
}


void Widget::releasePointer()
{
    Application::instance().releasePointer(*this);
}


const Cursor& Widget::cursor() const
{
    return  _cursor;
}


void Widget::setCursor(const Cursor& c)
{
    _cursor = c;

    // TODO: if this is the pointer widget do the same as onEnterEvent
    // Application::instance().screen().setCursor( &cursor() );
}


const Spacing& Widget::margin() const
{
    return _margin;
}


void Widget::setMargin(const Spacing& s)
{
    _margin = align(s);

    if( parent() )
       parent()->relayout();
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
    _padding = align(p);
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


Pt::Signal<const Pt::Event&>& Widget::eventReady()
{
    return _eventReady;
}


void Widget::onEvent(const Pt::Event& ev)
{
    _eventReady.send(ev );
}


void Widget::mouseEvent(const MouseEvent& ev)
{
  bool consumed = onMouseEvent(ev);

  if( consumed )
     return;

  Widget* parentWidget = this->parent();

  if(parentWidget)
  {
      MouseEvent ev2(ev);
      ev2.setId (parentWidget->vid() );
      //Application::instance().loop().commitEvent(ev2);
      parentWidget->mouseEvent(ev);
  }
}

void Widget::touchEvent(const TouchEvent& ev)
{
    bool consumed = onTouchEvent(ev);

    if (consumed)
        return;

    Widget* w = this->parent();

    if (w)
    {
        TouchEvent ev2(ev);
        ev2.setId(w->vid());
        Application::instance().loop().commitEvent(ev2);
    }
}


bool Widget::onMouseEvent(const MouseEvent& ev)
{
    if( ev.isPress(MouseEvent::Left) )
    {
        focus();
    }

    return false;
}


bool Widget::onTouchEvent(const TouchEvent& ev)
{
    if( ev.isPress() )
    {
        focus();
    }

    return false;
}


void Widget::scrollEvent(const ScrollEvent& ev)
{
    bool consumed = onScrollEvent(ev);
    if(consumed)
        return;

    Widget* w = this->parent();
    if(w)
    {
        ScrollEvent ev2(ev);
        ev2.setId (w->vid() );
        Application::instance().loop().commitEvent(ev2);
    }
}


bool Widget::onScrollEvent(const ScrollEvent& ev)
{
    return false;
}


void Widget::onKeyEvent(const KeyEvent& ev)
{
    if( (ev.key().code() == Key::Tab) && ev.isPress() )
    {
        if( ev.key().modifiers() == Key::Shift )
            _window->focusPrev();
        else
            _window->focusNext();

        Widget* focusWidget = _window->focusWidget();

        if(focusWidget)
            focusWidget->repaint();

        return;
    }

    if( ev.key() == actionKey() && hasFocus() )
    {
        onActionKey(ev);
    }
}


void Widget::onEnterEvent( const EnterEvent& ev)
{
    Application::instance().setCursor( &cursor() );
}


void Widget::onLeaveEvent(const LeaveEvent& ev)
{
}

} // namespace

} // namespace
