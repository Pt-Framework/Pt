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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/Hmi/SpinBox.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Convert.h>

namespace Pt {

namespace Hmi {

//////////////////////////////////////////////////////////////////////////
// SpinBoxButton
//////////////////////////////////////////////////////////////////////////

SpinBoxButton::SpinBoxButton(Type type)
: _type(type)
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

    setPressed(true);
}


void SpinBoxButton::onReleased()
{
    Base::onReleased();

    setPressed(false);

    clicked().send();
}


void SpinBoxButton::onCanceled()
{
    Base::onCanceled();

    setPressed(false);
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


void SpinBoxButton::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer )
        return;

    Painter painter(surface);
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
, _value(0)
, _minimum(-10000)
, _maximum(10000)
, _downButton(SpinBoxButton::Down)
, _upButton(SpinBoxButton::Up)
, _spacing(0)
, _hasRenderer(false)
{
    setTextInput(_isEditable);
    setFocusPolicy(Widget::NormalFocus);
    
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
    setTextInput(_isEditable);
    setFocusPolicy(_isEditable ? Widget::NormalFocus : Widget::NoFocus);
    update();
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

    update();
    relayout();

    _textEdited.send( _editor.text() );
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
  Pt::formatInt( std::back_inserter(s), n);
  return s;
}


int SpinBox::toValue(const Pt::String& str, bool& ok) const
{
    int n = 0;
    Pt::String::const_iterator it = Pt::parseInt(str.begin(), str.end(), n, ok);
    
    ok = it == str.end();
    return n;
}


void SpinBox::onUp()
{
    if( ++_value > _maximum )
        _value = _maximum;

    Pt::String str = toText(_value);
    _editor.setText(str);
    
    update();
    relayout();

    _textEdited.send( _editor.text() );
}


void SpinBox::onDown()
{
    if( --_value < _minimum )
        _value = _minimum;

    Pt::String str = toText(_value);
    _editor.setText(str);
    
    update();
    relayout();

    _textEdited.send( _editor.text() );
}


Adjustment SpinBox::textAdjustment() const
{
    return _editor.adjustment();
}


void SpinBox::setTextAdjustment(Adjustment a)
{
    _editor.setAdjustment(a);
    
    update();
    relayout();
}


std::size_t SpinBox::cursorPosition() const
{
    return _editor.cursorPosition();
}


void SpinBox::setCursorPosition(std::size_t n)
{
    _editor.setCursorPosition(n);
    
    update();
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
        setFocusPolicy(Widget::KeepFocus);
    }
    else
    {
        setFocusPolicy(Widget::NormalFocus);
    }
}


Pt::Signal<const Pt::String&>& SpinBox::textEdited()
{
    return _textEdited;
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
    return _foreground ? *_foreground
                       : Application::instance().styleOptions().foreground();
}


void SpinBox::setForeground(const Gfx::Brush& b)
{
    _foreground.reset( new Gfx::Brush(b) );

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


const std::string& SpinBox::font() const
{
    return _fontName ? *_fontName
                     : Application::instance().styleOptions().font().name();
}


void SpinBox::setFont(const std::string& fontName)
{
    _fontName.reset( new std::string(fontName) );
    invalidate();
}


std::size_t SpinBox::fontSize() const
{
    return _fontSize ? *_fontSize
                     : Application::instance().styleOptions().font().size();
}


void SpinBox::setFontSize(const std::size_t s)
{
    _fontSize.reset( new std::size_t(s) );
    invalidate();
}


Gfx::Font::Style SpinBox::fontStyle() const
{
    return _fontStyle ? *_fontStyle
                      : Application::instance().styleOptions().font().style();
}


void SpinBox::setFontStyle(Gfx::Font::Style style)
{
    _fontStyle.reset( new Gfx::Font::Style(style) );
    invalidate();
}


void SpinBox::setRenderer(SpinBoxRenderer* renderer)
{
    _renderer.reset(renderer);
    _hasRenderer = renderer != 0;

    _upButton.setRenderer(renderer);
    _downButton.setRenderer(renderer);

    invalidate();
    update();
    relayout();
}


Gfx::SizeF SpinBox::onMeasure(const SizePolicy& policy)
{
    double itemsWidth = policy.width();
    double itemsHeight = _font.size() * 2;

    _downButton.measure(policy);
    _upButton.measure(policy);

    return Gfx::SizeF( itemsWidth + padding().leftRight(), 
                       itemsHeight + padding().topBottom() );
}


void SpinBox::onLayout(const Gfx::RectF& rect)
{
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

    _downButton.layout(downRect);
    _upButton.layout(upRect);

    Gfx::SizeF editSize = _textBox.size();
    editSize.subWidth(5);  // TODO: cursor

    _editor.setFont(_font);
    _editor.setPosition( _textBox.topLeft() );
    _editor.setSize( _textBox.size() );
    _editor.layout(_line);
}


void SpinBox::onInvalidate()
{
    Base::onInvalidate();

    const StyleOptions& options = Application::instance().styleOptions();
    const Style& style = Application::instance().style();

    _backgroundBrush = background();
    _foregroundBrush = foreground();
    _pen = contour();
    _textPen = textColor();
    _font = Gfx::Font(font(), fontSize(), fontStyle());

    if( ! _hasRenderer )
        _renderer.reset( style.get<SpinBoxRenderer>() );
    
    if( ! _renderer )
        return;

    _renderer->prepare(*this, options, _backgroundBrush, _foregroundBrush, 
                       _pen, _font, _textPen);
}


void SpinBox::onPaint(PaintSurface& surface, const Gfx::RectF& rect)
{
    const StyleOptions& options = Application::instance().styleOptions();

    if( ! _renderer)
        return;

    Painter painter(surface);
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
        double cursorX = _line.cursorToX( _editor.cursorPosition() );
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


void SpinBox::onKeyEvent(const KeyEvent& ev)
{  
    Base::onKeyEvent(ev);

    if( ! ev.isPress() || ! _isEditable )
        return;

    if( ev.key().code() == Pt::Hmi::Key::ArrowLeft )
    {
        _editor.left();
        
        update();
        relayout();
    }
    else if( ev.key().code() == Pt::Hmi::Key::ArrowRight )
    {
        _editor.right();
        
        update();
        relayout();
    }
    else if( ev.key().code() == Pt::Hmi::Key::Return )
    {
        update();
        relayout();
        
        if( isAccepted() )
            _returnPressed.send( _editor.text() );
    }
    else if( ev.key().code() == Pt::Hmi::Key::Delete )
    {
        _isTextChanged = true;
        _editor.del();
        
        update();
        relayout();

        _textEdited.send( _editor.text() );
    }
    else if( ev.key().code() == Pt::Hmi::Key::Backspace )
    {
        _isTextChanged = true;
        _editor.backspace();
        
        update();
        relayout();

        _textEdited.send( _editor.text() );
    }
    else
    {
        Pt::Char ch = ev.unicode();
        if( Pt::isprint(ch) )
        {
            _isTextChanged = true;

            std::size_t cursorPosition = _editor.cursorPosition();
            Pt::String str = _editor.text();
            str.insert(cursorPosition, 1, ch);

            bool ok = false;
            int n = toValue(str, ok);
            if(ok)
            {
                _value = n;
                _editor.insert(ch);
                update();
                relayout();
            }
        }
    }
}


bool SpinBox::onMouseEvent(const MouseEvent& ev)
{
    Base::onMouseEvent(ev);

    if( ! ev.isPress() || ! _textBox.contains( ev.position() ) )
        return true;  

    if(_isEditable)
    {
        std::size_t n = _line.xToCursor( ev.x() );
        _editor.setCursorPosition(n);
        update();
            
        Application::instance().inputMethod().begin(*this);
    }

    return true;
}


void SpinBox::onTouchEvent(const TouchEvent& ev)
{
    Base::onTouchEvent(ev);

    if( ! ev.isPress() || ! _textBox.contains( ev.position() ) )
        return;  

    if(_isEditable)
    {
        std::size_t n = _line.xToCursor( ev.x() );
        _editor.setCursorPosition(n);
        update();

        Application::instance().inputMethod().begin(*this);
    }
}


void SpinBox::onFocusEvent(const FocusEvent& ev)
{
    Base::onFocusEvent(ev);

    if( ! ev.isFocused() )
    {
        if( isAccepted() && _isTextChanged)
        {
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
