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
, _hasRenderer(false)
{
}


SpinBoxButton::~SpinBoxButton()
{
}


SpinBoxButton::Type SpinBoxButton::type() const
{
    return _type;
}


const Gfx::Brush& SpinBoxButton::foreground() const
{
    return _foreground ? *_foreground
                       : Application::instance().styleOptions().foreground();
}


void SpinBoxButton::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );
    invalidate();
}


const Gfx::Pen& SpinBoxButton::contour() const
{
    return _contour ? *_contour
                    : Application::instance().styleOptions().contour();
}


void SpinBoxButton::setContour(const Gfx::Pen& p)
{
    _contour.reset( new Gfx::Pen(p) );
    invalidate();
}


void SpinBoxButton::setRenderer(SpinBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    invalidate();
}


void SpinBoxButton::onPressed()
{
    Base::onPressed();
}


void SpinBoxButton::onReleased()
{
    Base::onReleased();

    clicked().send();
}


void SpinBoxButton::onCanceled()
{
    Base::onCanceled();
}


void SpinBoxButton::onInvalidate()
{
    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _brush = foreground();
    _pen = contour();

    if( ! _hasRenderer )
        _renderer.reset( style.get<SpinBoxRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepareButton(*this, options, _brush, _pen);

    Base::onInvalidate();
}


void SpinBoxButton::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer )
        return;

    Painter painter(context);
    painter.setClip(rect);

    _renderer->renderButton(*this, options, painter, rect, _brush, _pen);
}

//////////////////////////////////////////////////////////////////////////
// SpinBox
//////////////////////////////////////////////////////////////////////////

SpinBox::SpinBox()
: _isEditable(true)
, _isAccepted(true)
, _isTextChanged(false)
, _isHighlighted(false)
, _value(0)
, _minimum(-20000)
, _maximum(10000)
, _downButton(SpinBoxButton::Down)
, _upButton(SpinBoxButton::Up)
, _spacing(0)
, _hasRenderer(false)
, _fontOverride(0)
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
    invalidate();
}


const Gfx::Brush& SpinBox::foreground() const
{
    return _downButton.foreground();
}


void SpinBox::setForeground(const Gfx::Brush& b)
{
    _downButton.setForeground(b);
    _upButton.setForeground(b);

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

    _downButton.setContour(p);
    _upButton.setContour(p);

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
    invalidate();
}


const Gfx::Font& SpinBox::font() const
{
    return _font;
}


void SpinBox::setFont(const Gfx::Font& font)
{
    _customFont = font;
    _fontOverride = OverrideAll;

    invalidate();
}


Gfx::Font SpinBox::getFont() const
{
    const Gfx::Font& base = Application::instance().styleOptions().font();

    if( _fontOverride == 0 )
        return base;

    if( _fontOverride == OverrideAll )
        return _customFont;

    std::size_t sz = (_fontOverride & OverrideSize) ? _customFont.size()
                                                    : base.size();
    Gfx::Font::Weight wt = (_fontOverride & OverrideWeight) ? _customFont.weight()
                                                            : base.weight();
    Gfx::Font::Slant sl = (_fontOverride & OverrideSlant) ? _customFont.slant()
                                                          : base.slant();

    if( base.hasStyleName() )
        return Gfx::Font(base.family(), sz, base.styleName(), wt, sl, base.stretch());

    if( base.category() != Gfx::Font::Category::None )
        return Gfx::Font(base.category(), sz, wt, sl, base.stretch());

    return Gfx::Font(base.family(), sz, wt, sl, base.stretch());
}


void SpinBox::setFontSize(std::size_t size)
{
    _customFont = Gfx::Font(_customFont.family(), size,
                            _customFont.weight(), _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideSize;

    invalidate();
}


void SpinBox::setFontWeight(Gfx::Font::Weight weight)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            weight, _customFont.slant(),
                            _customFont.stretch());
    _fontOverride |= OverrideWeight;

    invalidate();
}


void SpinBox::setFontSlant(Gfx::Font::Slant slant)
{
    _customFont = Gfx::Font(_customFont.family(), _customFont.size(),
                            _customFont.weight(), slant,
                            _customFont.stretch());
    _fontOverride |= OverrideSlant;

    invalidate();
}


void SpinBox::setRenderer(SpinBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    _upButton.setRenderer(renderer);
    _downButton.setRenderer(renderer);

    invalidate();
    repaint();
    relayout();
}


Gfx::SizeF SpinBox::onMeasure(PaintContext& ctx, const SizePolicy& policy)
{
    double itemsWidth = policy.width();
    double itemsHeight = _font.size() * 2.5;

    _downButton.measure(policy);
    _upButton.measure(policy);

    return Gfx::SizeF( itemsWidth + padding().leftRight(), 
                       itemsHeight + padding().topBottom() );
}


void SpinBox::onLayout(PaintContext& ctx, const Gfx::RectF& rect)
{
    Base::onLayout(ctx, rect);

    if( ! _renderer )
    {
        const Style& style = Application::instance().style();
        _renderer.reset( style.get<SpinBoxRenderer>() );
    }

    if( ! _renderer )
        return;

    Gfx::RectF downRect;
    Gfx::RectF upRect;
    
    _renderer->layout(*this, downRect, upRect, _textBox);

    _downButton.move( downRect.topLeft() );
    _downButton.resize( downRect.size() );

    _upButton.move( upRect.topLeft() );
    _upButton.resize( upRect.size() );

    Gfx::SizeF editSize = _textBox.size();
    editSize.subWidth(5);  // TODO: cursor

    Painter _painter( ctx );
    _painter.setFont(_font);

    _editor.setPosition( _textBox.topLeft() );
    _editor.setSize( _textBox.size() );
    _editor.layout(_painter, _line);
}


void SpinBox::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _backgroundBrush = background();
    _pen = contour();
    _textPen = textColor();
    _font = getFont();

    if( ! _hasRenderer )
        _renderer.reset( style.get<SpinBoxRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _backgroundBrush, _pen, _font, _textPen);
}


void SpinBox::onPaint(PaintContext& context, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Painter painter(context);
    painter.setClip(rect);

    //
    // spin box
    //

    _renderer->renderBackground( *this, options, painter, rect,
                                 _pen, _backgroundBrush );

    //
    // text with cursor
    //
    
    Gfx::RectF cursorRect;
    double cursorWidth = 5; // TODO: cursor

    if( _isEditable && hasFocus() )
    {
        painter.setFont(_font);
        double cursorX = _line.cursorToX( painter, _editor.cursorPosition() );
        cursorX += _line.position().x();
        
        cursorRect.set(Gfx::PointF( cursorX, _line.position().y() ),
                       Gfx::SizeF( cursorWidth, _line.maxHeight() ) );           
    }

    Gfx::PointF clipPos = _editor.position();
    Gfx::SizeF clipSize = _editor.size();
    clipSize.addWidth(cursorWidth);  // TODO: cursor
    
    Gfx::RectF clipRect(clipPos, clipSize);
    painter.setClip( Gfx::RectF(clipPos, clipSize) );

    Gfx::PointF textPos = _line.position();
    textPos.addY( _line.ascent() );

    _renderer->renderText(*this, options, painter, rect, 
                          _editor.text(), textPos, _font, _textPen, cursorRect);
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

    if( ! ev.isPress() || ! _textBox.contains( ev.position() ) )
        return true;  

    if(_isEditable)
    {
        Painter _painter( surface() );
        _painter.setFont(_font);

        std::size_t n = _line.xToCursor( _painter, ev.x() );
        _editor.setCursorPosition(n);
        repaint();
            
        Application::instance().inputMethod().begin(*this);
    }

    return true;
}


bool SpinBox::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    if( ! ev.isPress() || ! _textBox.contains( ev.position() ) )
        return true;  

    if(_isEditable)
    {
        Painter _painter( surface() );
        _painter.setFont(_font);

        std::size_t n = _line.xToCursor( _painter, ev.x() );
        _editor.setCursorPosition(n);
        repaint();

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
