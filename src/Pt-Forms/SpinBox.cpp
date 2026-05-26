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
#include <Pt/Forms/Style.h>
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


void SpinBoxButton::setRenderer(SpinBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    invalidate();
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
, _customRenderer(false)
, _styleGeneration(0)
, _overrideFlags(0)
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
    return _background ? *_background
                       : Application::instance().styleOptions().textBackground();
}


void SpinBox::setBackground(const Gfx::Brush& b)
{
    _background.reset( new Gfx::Brush(b) );
    _overrideFlags |= OverrideBackground;

    if( SpinBoxRenderer* r = getRenderer() )
        r->setBackground(b);

    invalidate();
}


const Gfx::Brush& SpinBox::foreground() const
{
    if( _foreground )
        return *_foreground;

    return Application::instance().styleOptions().foreground();
}


void SpinBox::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    _overrideFlags |= OverrideForeground;

    if( SpinBoxRenderer* r = getRenderer() )
        r->setForeground(b);

    invalidate();
}


const Gfx::Pen& SpinBox::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void SpinBox::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    _overrideFlags |= OverrideContour;

    if( SpinBoxRenderer* r = getRenderer() )
        r->setContour(p);

    invalidate();
}


const Gfx::Color& SpinBox::textColor() const
{
    return _textColor ? *_textColor
                      : Application::instance().styleOptions().textColor();
}


void SpinBox::setTextColor(const Gfx::Color& color)
{
    _textColor.reset( new Gfx::Color(color) );
    _overrideFlags |= OverrideTextColor;

    if( SpinBoxRenderer* r = getRenderer() )
        r->setTextColor( Gfx::Pen(color) );

    invalidate();
}


const Gfx::Font& SpinBox::font() const
{
    if( _renderer )
        return _renderer->font();

    return Application::instance().styleOptions().font();
}


void SpinBox::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _overrideFlags |= OverrideFontAll;

    if( SpinBoxRenderer* r = getRenderer() )
        r->setFont(font);

    invalidate();
}


Gfx::Font SpinBox::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( ! (_overrideFlags & OverrideFontAny) )
        return base;

    if( _overrideFlags & OverrideFontAll )
        return _customFont;

    std::size_t sz = (_overrideFlags & OverrideFontSize) ? _customFont.size()
                                                        : base.size();
    Gfx::Font::Weight wt = (_overrideFlags & OverrideFontWeight) ? _customFont.weight()
                                                                 : base.weight();
    Gfx::Font::Slant sl = (_overrideFlags & OverrideFontSlant) ? _customFont.slant()
                                                               : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void SpinBox::setFontSize(std::size_t size)
{
    _customFont = _customFont.withSize(size);
    _overrideFlags |= OverrideFontSize;

    if( SpinBoxRenderer* r = getRenderer() )
        r->setFont( getFont() );

    invalidate();
}


void SpinBox::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = _customFont.withWeight(weight);
    _overrideFlags |= OverrideFontWeight;

    if( SpinBoxRenderer* r = getRenderer() )
        r->setFont( getFont() );

    invalidate();
}


void SpinBox::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = _customFont.withSlant(slant);
    _overrideFlags |= OverrideFontSlant;

    if( SpinBoxRenderer* r = getRenderer() )
        r->setFont( getFont() );

    invalidate();
}


void SpinBox::setRenderer(SpinBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _customRenderer = renderer != 0;

    _upButton.setRenderer(renderer);
    _downButton.setRenderer(renderer);

    invalidate();
    repaint();
    relayout();
}


SpinBoxRenderer* SpinBox::getRenderer()
{
    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        SpinBoxRenderer* proto = style.get<SpinBoxRenderer>();
        if( ! proto )
            return 0;

        _renderer.reset( proto->create() );

        _upButton.setRenderer( _renderer.get() );
        _downButton.setRenderer( _renderer.get() );
    }

    return _renderer.get();
}


void SpinBox::applyRenderer(SpinBoxRenderer* renderer)
{
    if( _overrideFlags & OverrideBackground )
        renderer->setBackground(*_background);

    if( _overrideFlags & OverrideForeground )
        renderer->setForeground(*_foreground);

    if( _overrideFlags & OverrideContour )
        renderer->setContour(*_contour);

    if( _overrideFlags & OverrideTextColor )
        renderer->setTextColor( Gfx::Pen(*_textColor) );

    if( _overrideFlags & OverrideFontAny )
        renderer->setFont( getFont() );
}


SpinBoxStyleFlags SpinBox::spinBoxStyleFlags() const
{
    StyleFlags common;

    if( isEnabled() )
        common.set(StyleFlags::Enabled);

    if( _isHighlighted )
        common.set(StyleFlags::Highlighted);

    if( hasFocus() )
        common.set(StyleFlags::Focused);

    SpinBoxStyleFlags state(common);

    if( _isEditable )
        state.set(SpinBoxStyleFlags::Editable);

    return state;
}


ButtonStyleFlags SpinBox::buttonStyleFlags(const SpinBoxButton& button) const
{
    StyleFlags common;

    if( button.isEnabled() )
        common.set(StyleFlags::Enabled);

    if( button.isHighlighted() )
        common.set(StyleFlags::Highlighted);

    ButtonStyleFlags state(common);

    if( button.isPressed() )
        state.set(ButtonStyleFlags::Pressed);

    return state;
}


void SpinBox::onInvalidate()
{
    Base::onInvalidate();

    std::size_t gen = Application::instance().styleOptions().generation();
    if( _styleGeneration != gen )
    {
        _styleGeneration = gen;
        if( ! _customRenderer )
            _renderer.reset();
    }

    if( ! _renderer )
    {
        bool hasOverride = (_overrideFlags != 0);
        if(hasOverride)
        {
            if( SpinBoxRenderer* renderer = getRenderer() )
                applyRenderer(renderer);
        }
        else
        {
            const Style& style = Application::instance().style();
            _renderer.reset( style.get<SpinBoxRenderer>() );

            _upButton.setRenderer( _renderer.get() );
            _downButton.setRenderer( _renderer.get() );
        }
    }
}


Gfx::SizeF SpinBox::onMeasure(const SizePolicy& policy)
{
    if( ! _renderer )
        return Gfx::SizeF();

    Gfx::SizeF contentSize(policy.width(), 0);
    Gfx::SizeF totalSize = _renderer->measureFrame( surface(), contentSize );

    return Gfx::SizeF( totalSize.width() + padding().leftRight(), 
                       totalSize.height() + padding().topBottom() );
}


void SpinBox::onLayout(const Gfx::RectF& rect)
{
    Base::onLayout(rect);

    if( ! _renderer )
        return;

    _renderer->layoutFrame(surface(),
                           Gfx::RectF( Gfx::PointF(padding().left(), padding().top()),
                                       Gfx::SizeF(size().width() - padding().leftRight(),
                                                  size().height() - padding().topBottom()) ),
                           _entryRect, _upButtonRect, _downButtonRect, _textRect);

    _downButton.move( _downButtonRect.topLeft() );
    _downButton.resize( _downButtonRect.size() );

    _upButton.move( _upButtonRect.topLeft() );
    _upButton.resize( _upButtonRect.size() );

    _editor.setPosition( _textRect.topLeft() );
    _editor.setSize( _textRect.size() );

    const Painter& painter = _renderer->textPainter( surface() );
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
    if( ! _renderer )
        return;

    SpinBoxStyleFlags state = spinBoxStyleFlags();
    ButtonStyleFlags upState = buttonStyleFlags(_upButton);
    ButtonStyleFlags downState = buttonStyleFlags(_downButton);

    Gfx::PointF textPos = _line.position();
    textPos.addY( _line.ascent() );

    Gfx::RectF cursor;
    if( _isEditable && hasFocus() )
    {
        const Painter& painter = _renderer->textPainter( surface() );

        double cursorX = _line.cursorToX(painter, _editor.cursorPosition());
        cursorX += _line.position().x();

        Gfx::PointF top(cursorX, _line.position().y());
        Gfx::PointF bottom(cursorX, _line.position().y() + _line.maxHeight());
        cursor = Gfx::RectF(top, bottom);
    }

    _renderer->renderFrame(context,
                           Gfx::RectF( Gfx::PointF(padding().left(), padding().top()),
                                       Gfx::SizeF(size().width() - padding().leftRight(),
                                                  size().height() - padding().topBottom()) ),
                           _entryRect, _upButtonRect, _downButtonRect,
                           state, upState, downState);

    _renderer->renderText(context, _textRect, _line.text(), textPos,
                          cursor, state);
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
