/* Copyright (C) 2017 Marc Boris Duerner 

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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Forms/SpinBox.h>
#include <Pt/Forms/SpinBoxStyle.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/PaintContext.h>
#include <Pt/Forms/Painter.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Convert.h>

namespace Pt {

namespace Forms {

//////////////////////////////////////////////////////////////////////////
// SpinBoxButton
//////////////////////////////////////////////////////////////////////////

SpinBoxButton::SpinBoxButton(Type type)
: _type(type)
, _isPressed(false)
{
}


SpinBoxButton::~SpinBoxButton()
{
}


SpinBoxButton::Type SpinBoxButton::type() const
{
    return _type;
}


bool SpinBoxButton::isPressed() const
{
    return _isPressed;
}


void SpinBoxButton::onPressed()
{
    _isPressed = true;
    Base::onPressed();

    if( parent() )
        parent()->repaint();
}


void SpinBoxButton::onReleased()
{
    _isPressed = false;
    Base::onReleased();

    clicked().send();

    if( parent() )
        parent()->repaint();
}


void SpinBoxButton::onCanceled()
{
    _isPressed = false;
    Base::onCanceled();

    if( parent() )
        parent()->repaint();
}


void SpinBoxButton::onInvalidate()
{
    Base::onInvalidate();
}


void SpinBoxButton::onPaint(PaintContext& /*context*/, const Gfx::RectF& /*rect*/)
{
    // Painting is done by the parent SpinBox via combined render.
}

//////////////////////////////////////////////////////////////////////////
// SpinBox
//////////////////////////////////////////////////////////////////////////

SpinBox::SpinBox()
: _isEditable(true)
, _isAccepted(true)
, _isTextChanged(false)
, _isHighlighted(false)
, _pendingCursorX(-1)
, _value(0)
, _minimum(-20000)
, _maximum(10000)
, _downButton(SpinBoxButton::Down)
, _upButton(SpinBoxButton::Up)
{
    setFocusPolicy(Control::AcceptFocus);
    
    _editor.setText("0");

    _upButton.clicked() += Pt::slot(*this, &SpinBox::onUp);
    add(_upButton);

    _downButton.clicked() += Pt::slot(*this, &SpinBox::onDown);
    add(_downButton);
}


SpinBox::~SpinBox()
{
}


bool SpinBox::isEditable() const
{
    return _isEditable;
}


void SpinBox::setEditable(bool e)
{
    _isEditable = e;
    setFocusPolicy(_isEditable ? Control::AcceptFocus : Control::NoFocus);
    repaint();
}


int SpinBox::minimum() const
{
    return _minimum;
}


int SpinBox::maximum() const
{
    return _maximum;
}


void SpinBox::setRange(int min, int max)
{
    if(min > max)
        min = max;

    _minimum = min;
    _maximum = max;

    if(_value >= _minimum && _value <= _maximum)
        return;

    setValue(_value);
}


int SpinBox::value() const
{
    return _value;
}


void SpinBox::setValue(int n)
{
    if(n > _maximum)
        n = _maximum;

    if(n < _minimum)
        n = _minimum;

    _value = n;

    Pt::String str = toText(_value);
    _editor.setText(str);

    repaint();
    relayout();

    _valueEdited.send(_value);
}


const Pt::String& SpinBox::text() const
{
    return _editor.text();
}


bool SpinBox::isEmpty() const
{
    return _editor.isEmpty();
}


Pt::String SpinBox::toText(int n) const
{
    Pt::String s;
    Pt::formatInt(std::back_inserter(s), n);
    return s;
}


bool SpinBox::toValue(const Pt::String& str, int& n) const
{
    if( str == "-" || str == "+" || str.empty() )
        return false;

    bool ok = false;
    Pt::String::const_iterator it = Pt::parseInt(str.begin(), str.end(), n, ok);
    return it == str.end() && ok;
}


bool SpinBox::onInput(const Pt::String& str) const
{
    if( str == "-" || str == "+" || str.empty() )
        return true;

    int n = 0;
    bool ok = false;
    Pt::String::const_iterator it = Pt::parseInt(str.begin(), str.end(), n, ok);
    return it == str.end() && ok;
}


bool SpinBox::setInput(const Pt::String& str)
{
    bool accepted = onInput(str);
    if(accepted)
    {
        int n = 0;
        bool valid = toValue(str, n);

        if(n < _minimum || n > _maximum)
            return false;

        if(valid)
            _value = n;
    }

    return accepted;
}


void SpinBox::onStep(int n)
{
    _value += n;

    if( _value > _maximum )
        _value = _maximum;

    if( _value < _minimum )
        _value = _minimum;

    Pt::String str = toText(_value);
    _editor.setText(str);
    
    repaint();
    relayout();

    _valueEdited.send(_value);
}


void SpinBox::onUp()
{
    onStep(1);
}


void SpinBox::onDown()
{
    onStep(-1);
}


Adjustment SpinBox::textAdjustment() const
{
    return _editor.adjustment();
}


void SpinBox::setTextAdjustment(Adjustment a)
{
    _editor.setAdjustment(a);
    
    repaint();
    relayout();
}


std::size_t SpinBox::cursorPosition() const
{
    return _editor.cursorPosition();
}


void SpinBox::setCursorPosition(std::size_t n)
{
    _editor.setCursorPosition(n);
    
    repaint();
    relayout();
}


bool SpinBox::isAccepted() const
{
    return _isAccepted;
}


void SpinBox::setAccepted(bool a)
{
    _isAccepted = a;
    
    if( ! a )
    {
        setFocusPolicy(Control::KeepFocus);
    }
    else
    {
        setFocusPolicy(Control::AcceptFocus);
    }
}


bool SpinBox::isHighlighted() const
{
    return _isHighlighted;
}


Pt::Signal<int>& SpinBox::valueEdited()
{
    return _valueEdited;
}


Pt::Signal<const Pt::String&>& SpinBox::returnPressed()
{
    return _returnPressed;
}


Pt::Signal<const Pt::String&>& SpinBox::editingFinished()
{
    return _editingFinished;
}


const Gfx::Brush& SpinBox::background() const
{
    if( const Gfx::Brush* b = _spinBoxOptions.background() )
        return *b;

    return Application::instance().styleOptions().textBackground();
}


void SpinBox::setBackground(const Gfx::Brush& b)
{
    _spinBoxOptions.setBackground(b);
    invalidate();
}


const Gfx::Brush& SpinBox::foreground() const
{
    if( const Gfx::Brush* b = _spinBoxOptions.foreground() )
        return *b;

    return Application::instance().styleOptions().foreground();
}


void SpinBox::setForeground(const Gfx::Brush& b)
{
    _spinBoxOptions.setForeground(b);
    invalidate();
}


const Gfx::Pen& SpinBox::contour() const
{
    if( const Gfx::Pen* p = _spinBoxOptions.contour() )
        return *p;

    return Application::instance().styleOptions().contour();
}


void SpinBox::setContour(const Gfx::Pen& p)
{
    _spinBoxOptions.setContour(p);
    invalidate();
}


const Gfx::Color& SpinBox::textColor() const
{
    if( const Gfx::Color* c = _spinBoxOptions.textColor() )
        return *c;

    return Application::instance().styleOptions().textColor();
}


void SpinBox::setTextColor(const Gfx::Color& color)
{
    _spinBoxOptions.setTextColor(color);
    invalidate();
}


const Gfx::Font& SpinBox::font() const
{
    if( const Gfx::Font* f = _spinBoxOptions.font() )
        return *f;

    return Application::instance().styleOptions().font();
}


void SpinBox::setFont(const Gfx::Font& font)
{
    _spinBoxOptions.setFont(font);
    invalidate();
}


void SpinBox::setFontSize(std::size_t size)
{
    _spinBoxOptions.setFontSize(size);
    invalidate();
}


void SpinBox::setFontWeight(Gfx::Font::Weight weight)
{
    _spinBoxOptions.setFontWeight(weight);
    invalidate();
}


void SpinBox::setFontSlant(Gfx::Font::Slant slant)
{
    _spinBoxOptions.setFontSlant(slant);
    invalidate();
}


void SpinBox::setRenderer(SpinBoxRenderer* renderer)
{
    const StyleOptions& opts = Application::instance().styleOptions();

    if(renderer)
        _spinBoxStyle.bind(*renderer, opts, _spinBoxOptions);
    else
        _spinBoxStyle.bind(Application::instance().style(), opts, _spinBoxOptions);

    invalidate();
}


SpinBoxState SpinBox::spinBoxState() const
{
    SpinBoxState state;
    state.setEnabled( isEnabled() );
    state.setHovered( _isHighlighted );
    state.setFocused( hasFocus() );
    state.setEditable( _isEditable );
    state.setUpPressed( _upButton.isPressed() );
    state.setUpHovered( _upButton.isHovered() );
    state.setDownPressed( _downButton.isPressed() );
    state.setDownHovered( _downButton.isHovered() );
    return state;
}


void SpinBox::onInvalidate()
{
    Base::onInvalidate();

    const Style& style = Application::instance().style();
    const StyleOptions& opts = Application::instance().styleOptions();
    _spinBoxStyle.rebind(style, opts, _spinBoxOptions);
}


Gfx::SizeF SpinBox::onMeasure(const SizePolicy& policy)
{
    SpinBoxRenderer* renderer = _spinBoxStyle.renderer();
    if( ! renderer )
        return Gfx::SizeF();

    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF totalSize = renderer->measureFrame( surface(), contentSize );

    return Gfx::SizeF( totalSize.width() + padding().leftRight(), 
                       totalSize.height() + padding().topBottom() );
}


void SpinBox::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    SpinBoxRenderer* renderer = _spinBoxStyle.renderer();
    if( ! renderer )
        return;

    Gfx::PointF origin(padding().left(), padding().top());
    Gfx::SizeF inner(size().width() - padding().leftRight(),
                     size().height() - padding().topBottom());
    Gfx::RectF chromeRect(origin, inner);

    renderer->layoutChrome(surface(), chromeRect,
                           _entryRect, _upButtonRect, _downButtonRect, _textRect);

    _downButton.move( _downButtonRect.topLeft() );
    _downButton.resize( _downButtonRect.size() );

    _upButton.move( _upButtonRect.topLeft() );
    _upButton.resize( _upButtonRect.size() );

    _editor.setPosition( _textRect.topLeft() );
    _editor.setSize( _textRect.size() );

    const Painter& painter = renderer->textPainter( surface() );
    _editor.layout(painter, _line);

    if(_pendingCursorX >= 0)
    {
        std::size_t n = _line.xToCursor(painter, _pendingCursorX);
        _editor.setCursorPosition(n);
        _pendingCursorX = -1;
    }
}


void SpinBox::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    SpinBoxRenderer* renderer = _spinBoxStyle.renderer();
    if( ! renderer )
        return;

    SpinBoxState state = spinBoxState();

    Gfx::PointF origin(padding().left(), padding().top());
    Gfx::SizeF inner(size().width() - padding().leftRight(),
                     size().height() - padding().topBottom());
    Gfx::RectF widgetRect(origin, inner);

    onPaintChrome(context, widgetRect, _entryRect, _upButtonRect,
                  _downButtonRect, state);

    Gfx::PointF textPos = _line.position();
    textPos.addY( _line.ascent() );

    Gfx::RectF cursor;
    if( _isEditable && hasFocus() )
    {
        const Painter& painter = renderer->textPainter( surface() );

        double cursorX = _line.cursorToX(painter, _editor.cursorPosition());
        cursorX += _line.position().x();

        Gfx::PointF top(cursorX, _line.position().y());
        Gfx::PointF bottom(cursorX, _line.position().y() + _line.maxHeight());
        cursor = Gfx::RectF(top, bottom);
    }

    onPaintText(context, _textRect, _line.text(), textPos, cursor, state);
}


void SpinBox::onPaintChrome(PaintContext& context,
                            const Gfx::RectF& rect,
                            const Gfx::RectF& entryRect,
                            const Gfx::RectF& upButtonRect,
                            const Gfx::RectF& downButtonRect,
                            const SpinBoxState& state)
{
    SpinBoxRenderer* renderer = _spinBoxStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderChrome(context, rect, entryRect, upButtonRect,
                           downButtonRect, state);
}


void SpinBox::onPaintText(PaintContext& context,
                          const Gfx::RectF& textRect,
                          const String& text,
                          const Gfx::PointF& textPos,
                          const Gfx::RectF& cursor,
                          const SpinBoxState& state)
{
    SpinBoxRenderer* renderer = _spinBoxStyle.renderer();
    if( ! renderer )
        return;

    renderer->renderText(context, textRect, text, textPos, cursor, state);
}


bool SpinBox::onKeyEvent(const KeyEvent& ev)
{  
    Base::onKeyEvent(ev);

    if( ! ev.isPress() || ! _isEditable )
        return true;

    if( ev.key().code() == Pt::Forms::Key::ArrowLeft )
    {
        _editor.left();

        repaint();
        relayout();
    }
    else if( ev.key().code() == Pt::Forms::Key::ArrowRight )
    {
        _editor.right();

        repaint();
        relayout();
    }
    else if( ev.key().code() == Pt::Forms::Key::Return )
    {
        repaint();
        relayout();

        if( isAccepted() )
            _returnPressed.send( _editor.text() );
    }
    else if( ev.key().code() == Pt::Forms::Key::Delete )
    {
        std::size_t cursorPosition = _editor.cursorPosition();

        if( cursorPosition >= _editor.text().size() )
            return true;

        Pt::String str = _editor.text();
        str.erase(cursorPosition, 1);

        bool ok = setInput(str);
        if( ok )
        {
            _isTextChanged = true;
            _editor.del();

            repaint();
            relayout();

            _valueEdited.send(_value);
        }
    }
    else if( ev.key().code() == Pt::Forms::Key::Backspace )
    {
        std::size_t cursorPosition = _editor.cursorPosition();

        if( --cursorPosition >= _editor.text().size() )
            return true;

        Pt::String str = _editor.text();
        str.erase(cursorPosition, 1);

        bool ok = setInput(str);
        if( ok )
        {
            _isTextChanged = true;
            _editor.backspace();

            repaint();
            relayout();

            _valueEdited.send(_value);
        }
    }
    else
    {
        Pt::Char ch = ev.unicode();
        if( Pt::isprint(ch) )
        {
            std::size_t cursorPosition = _editor.cursorPosition();
            Pt::String str = _editor.text();
            str.insert(cursorPosition, 1, ch);

            bool ok = setInput(str);
            if( ok )
            {
                _isTextChanged = true;
                _editor.insert(ch);

                repaint();
                relayout();

                _valueEdited.send(_value);
            }
        }
    }

    return true;
}


bool SpinBox::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    if( ! ev.isPress() || ! _entryRect.contains( ev.position() ) )
        return true;  

    if(_isEditable)
    {
        _pendingCursorX = ev.x();
        relayout();

        Application::instance().inputMethod().begin(*this);
    }

    return true;
}


bool SpinBox::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    if( ! ev.isPress() || ! _entryRect.contains( ev.position() ) )
        return true;  

    if(_isEditable)
    {
        _pendingCursorX = ev.x();
        relayout();

        Application::instance().inputMethod().begin(*this);
    }

    return true;
}


bool SpinBox::onEnterEvent(const EnterEvent& ev)
{
    Base::onEnterEvent(ev);

    _isHighlighted = true;
    
    invalidate();
    return true;
}


bool SpinBox::onLeaveEvent(const LeaveEvent& ev)
{
    Base::onLeaveEvent(ev);

    _isHighlighted = false;

    invalidate();
    return true;
}


void SpinBox::onFocusEvent(const FocusEvent& ev)
{
    Base::onFocusEvent(ev);

    if( ! ev.isFocused() )
    {
        if( isAccepted() && _isTextChanged )
        {
            Pt::String str = toText(_value);
            _editor.setText(str);
    
            repaint();
            relayout();

            _isTextChanged = false;
            _editingFinished.send( _editor.text() );
        }
    }
    else if(_isEditable)
    {
        Application::instance().inputMethod().begin(*this);
    }
}

} // namespace

} // namespace
